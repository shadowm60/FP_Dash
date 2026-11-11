"""Minimal DBC reader: frames, signals, comments, float types, value tables.

Only what the dash needs. Multiplexed signals are read but flagged, because
the firmware does not decode multiplexing yet.
"""
import re
from dataclasses import dataclass, field

_BO = re.compile(r'^BO_\s+(\d+)\s+(\w+)\s*:\s*(\d+)\s+(\S+)')
_SG = re.compile(
    r'^\s*SG_\s+(\w+)\s*(M|m\d+M?)?\s*:\s*(\d+)\|(\d+)@([01])([+-])\s*'
    r'\(\s*([^,]+?)\s*,\s*([^)]+?)\s*\)\s*\[\s*([^|]*?)\s*\|\s*([^\]]*?)\s*\]\s*"([^"]*)"')
_CM_SG = re.compile(r'CM_\s+SG_\s+(\d+)\s+(\w+)\s+"((?:[^"\\]|\\.)*)"\s*;', re.S)
_CM_BO = re.compile(r'CM_\s+BO_\s+(\d+)\s+"((?:[^"\\]|\\.)*)"\s*;', re.S)
_VALTYPE = re.compile(r'SIG_VALTYPE_\s+(\d+)\s+(\w+)\s*:\s*(\d)\s*;')
_VAL = re.compile(r'VAL_\s+(\d+)\s+(\w+)\s+((?:-?\d+\s+"[^"]*"\s*)+);')


@dataclass
class DbcSignal:
    name: str
    start: int
    length: int
    motorola: bool          # @0
    signed: bool            # '-'
    factor: float
    offset: float
    minimum: str
    maximum: str
    unit: str
    is_float: bool = False
    multiplex: str = ''     # '', 'M' or 'mN'
    comment: str = ''
    values: dict = field(default_factory=dict)

    @property
    def value_type(self):
        if self.is_float:
            return 'float'
        return 'signed' if self.signed else 'unsigned'


@dataclass
class DbcFrame:
    raw_id: int             # as in the file (bit 31 = extended)
    name: str
    dlc: int
    sender: str
    signals: list = field(default_factory=list)
    comment: str = ''

    @property
    def ext(self):
        return bool(self.raw_id & 0x80000000)

    @property
    def can_id(self):
        return self.raw_id & 0x1FFFFFFF


def _num(s):
    s = s.strip()
    try:
        return int(s)
    except ValueError:
        return float(s)


def load(path):
    """Returns a list of DbcFrame. Raises ValueError on unreadable SG_/BO_ lines."""
    with open(path, encoding='utf-8', errors='replace') as f:
        text = f.read()

    frames = []
    by_id = {}
    cur = None
    for lineno, line in enumerate(text.splitlines(), 1):
        if line.startswith('BO_ '):
            m = _BO.match(line)
            if not m:
                raise ValueError(f'{path}:{lineno}: cannot read BO_ line')
            raw_id = int(m.group(1))
            cur = DbcFrame(raw_id, m.group(2), int(m.group(3)), m.group(4))
            # VECTOR__INDEPENDENT_SIG_MSG holds signals not sent in any frame
            if m.group(2) == 'VECTOR__INDEPENDENT_SIG_MSG':
                cur = None
                continue
            frames.append(cur)
            by_id[raw_id] = cur
        elif line.lstrip().startswith('SG_ '):
            if cur is None:
                continue
            m = _SG.match(line)
            if not m:
                raise ValueError(f'{path}:{lineno}: cannot read SG_ line')
            cur.signals.append(DbcSignal(
                name=m.group(1), multiplex=m.group(2) or '',
                start=int(m.group(3)), length=int(m.group(4)),
                motorola=(m.group(5) == '0'), signed=(m.group(6) == '-'),
                factor=_num(m.group(7)), offset=_num(m.group(8)),
                minimum=m.group(9), maximum=m.group(10), unit=m.group(11)))
        elif line.strip() == '':
            cur = None

    def find(raw_id, name):
        fr = by_id.get(raw_id)
        if fr:
            for s in fr.signals:
                if s.name == name:
                    return s
        return None

    for m in _CM_SG.finditer(text):
        s = find(int(m.group(1)), m.group(2))
        if s:
            s.comment = m.group(3).replace('\\"', '"')
    for m in _CM_BO.finditer(text):
        fr = by_id.get(int(m.group(1)))
        if fr:
            fr.comment = m.group(2).replace('\\"', '"')
    for m in _VALTYPE.finditer(text):
        s = find(int(m.group(1)), m.group(2))
        if s and m.group(3) == '1':
            s.is_float = True
    for m in _VAL.finditer(text):
        s = find(int(m.group(1)), m.group(2))
        if s:
            for v, label in re.findall(r'(-?\d+)\s+"([^"]*)"', m.group(3)):
                s.values[int(v)] = label
    return frames
