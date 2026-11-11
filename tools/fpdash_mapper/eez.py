"""Reads the global variables of an EEZ Studio project (.eez-project, JSON)."""
import json
from dataclasses import dataclass

# EEZ variable types the firmware can write (canmap_ui_type_t)
SUPPORTED_TYPES = ('integer', 'float', 'string', 'boolean')


@dataclass
class EezVar:
    name: str
    type: str
    default: str = ''

    @property
    def supported(self):
        return self.type in SUPPORTED_TYPES

    @property
    def system(self):
        """Variables not fed by the map: sys_* are written by the firmware
        (hmi/bridge), ui_* are UI-local (e.g. thresholds edited in EEZ Studio)."""
        return self.name.startswith(('sys_', 'ui_'))


def load(path):
    with open(path, encoding='utf-8') as f:
        project = json.load(f)
    out = []
    for v in project.get('variables', {}).get('globalVariables', []):
        t = v.get('type', '')
        out.append(EezVar(v['name'], t, v.get('defaultValue', '')))
    return out
