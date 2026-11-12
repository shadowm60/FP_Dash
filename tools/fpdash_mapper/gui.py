"""tkinter GUI: map DBC signals to EEZ Studio variables, export the map XML."""
import os
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

import dbc
import eez
import mapmodel
from mapmodel import UiBinding

UI_TYPES = ('integer', 'float', 'string', 'boolean')


class MapperApp:
    def __init__(self, root, smap=None, eez_vars=None, map_path=None, dbc_path=None, eez_path=None):
        self.root = root
        self.map = smap or mapmodel.SignalMap()
        self.eez_vars = eez_vars or []
        self.map_path = map_path
        self.dbc_path = dbc_path
        self.eez_path = eez_path
        self.sel = None             # (frame, signal) being edited
        self.item_sig = {}          # tree item id -> (frame, signal)
        self.item_frame = {}        # tree item id -> frame

        root.title('FP_Dash signal mapper')
        root.geometry('1250x760')
        self._build()
        self.refresh_tree()
        self.refresh_status()

    # ------------------------------------------------------------ layout

    def _build(self):
        bar = ttk.Frame(self.root, padding=4)
        bar.pack(fill='x')
        for text, cmd in (('Open DBC...', self.open_dbc), ('Open EEZ project...', self.open_eez),
                          ('Open map...', self.open_map), ('Save map', self.save_map),
                          ('Save map as...', self.save_map_as), ('Auto-map by name', self.auto_map)):
            ttk.Button(bar, text=text, command=cmd).pack(side='left', padx=2)
        ttk.Label(bar, text='  Bitrate').pack(side='left')
        self.bitrate = tk.StringVar(value=str(self.map.bitrate))
        ttk.Combobox(bar, textvariable=self.bitrate, width=9,
                     values=('125000', '250000', '500000', '1000000')).pack(side='left')
        ttk.Label(bar, text='  Filter').pack(side='left')
        self.filter = tk.StringVar()
        self.filter.trace_add('write', lambda *a: self.refresh_tree())
        ttk.Entry(bar, textvariable=self.filter, width=18).pack(side='left')

        pane = ttk.PanedWindow(self.root, orient='horizontal')
        pane.pack(fill='both', expand=True)

        # signal tree
        left = ttk.Frame(pane)
        cols = ('unit', 'dec', 'log', 'ui')
        self.tree = ttk.Treeview(left, columns=cols, selectmode='browse')
        self.tree.heading('#0', text='Frame / signal')
        self.tree.heading('unit', text='Unit')
        self.tree.heading('dec', text='Dec')
        self.tree.heading('log', text='Log')
        self.tree.heading('ui', text='UI variables')
        self.tree.column('#0', width=300)
        self.tree.column('unit', width=70)
        self.tree.column('dec', width=40, anchor='center')
        self.tree.column('log', width=40, anchor='center')
        self.tree.column('ui', width=260)
        self.tree.tag_configure('off', foreground='#999999')
        self.tree.tag_configure('bound', foreground='#0050c0')
        ysb = ttk.Scrollbar(left, orient='vertical', command=self.tree.yview)
        self.tree.configure(yscrollcommand=ysb.set)
        self.tree.pack(side='left', fill='both', expand=True)
        ysb.pack(side='right', fill='y')
        self.tree.bind('<<TreeviewSelect>>', self.on_select)
        pane.add(left, weight=3)

        # editor
        right = ttk.Frame(pane, padding=8)
        pane.add(right, weight=2)
        self.title = ttk.Label(right, text='Select a signal', font=('TkDefaultFont', 11, 'bold'))
        self.title.grid(row=0, column=0, columnspan=4, sticky='w')
        self.info = ttk.Label(right, text='', foreground='#555555', wraplength=420, justify='left')
        self.info.grid(row=1, column=0, columnspan=4, sticky='w', pady=(0, 8))

        self.enabled = tk.BooleanVar()
        self.log = tk.BooleanVar()
        self.unit = tk.StringVar()
        self.decimals = tk.StringVar()
        self.timeout = tk.StringVar()
        ttk.Checkbutton(right, text='Include in map (decode)', variable=self.enabled,
                        command=self.apply_signal).grid(row=2, column=0, columnspan=2, sticky='w')
        ttk.Checkbutton(right, text='Log to parameter CSV', variable=self.log,
                        command=self.apply_signal).grid(row=2, column=2, columnspan=2, sticky='w')
        ttk.Label(right, text='Unit').grid(row=3, column=0, sticky='w')
        e = ttk.Entry(right, textvariable=self.unit, width=12)
        e.grid(row=3, column=1, sticky='w')
        e.bind('<FocusOut>', lambda ev: self.apply_signal())
        ttk.Label(right, text='Decimals').grid(row=3, column=2, sticky='w')
        sb = ttk.Spinbox(right, from_=0, to=6, textvariable=self.decimals, width=5, command=self.apply_signal)
        sb.grid(row=3, column=3, sticky='w')
        sb.bind('<FocusOut>', lambda ev: self.apply_signal())
        ttk.Label(right, text='Frame timeout ms').grid(row=4, column=0, sticky='w')
        e = ttk.Entry(right, textvariable=self.timeout, width=12)
        e.grid(row=4, column=1, sticky='w')
        e.bind('<FocusOut>', lambda ev: self.apply_signal())

        ttk.Separator(right).grid(row=5, column=0, columnspan=4, sticky='ew', pady=8)
        ttk.Label(right, text='Shown in UI variables', font=('TkDefaultFont', 10, 'bold')).grid(
            row=6, column=0, columnspan=4, sticky='w')
        self.bindings = tk.Listbox(right, height=5, exportselection=False)
        self.bindings.grid(row=7, column=0, columnspan=4, sticky='ew')
        self.bindings.bind('<<ListboxSelect>>', self.on_binding_select)

        self.b_var = tk.StringVar()
        self.b_type = tk.StringVar(value='string')
        self.b_scale = tk.StringVar(value='1')
        self.b_offset = tk.StringVar(value='0')
        self.b_format = tk.StringVar()
        self.b_step = tk.StringVar(value='0')
        self.b_period = tk.StringVar(value='0')
        ttk.Label(right, text='Variable').grid(row=8, column=0, sticky='w', pady=(6, 0))
        self.var_combo = ttk.Combobox(right, textvariable=self.b_var, width=28)
        self.var_combo.grid(row=8, column=1, columnspan=3, sticky='w', pady=(6, 0))
        self.var_combo.bind('<<ComboboxSelected>>', self.on_var_chosen)
        ttk.Label(right, text='Type').grid(row=9, column=0, sticky='w')
        ttk.Combobox(right, textvariable=self.b_type, values=UI_TYPES, width=10,
                     state='readonly').grid(row=9, column=1, sticky='w')
        ttk.Label(right, text='Format').grid(row=9, column=2, sticky='w')
        ttk.Entry(right, textvariable=self.b_format, width=10).grid(row=9, column=3, sticky='w')
        ttk.Label(right, text='Scale').grid(row=10, column=0, sticky='w')
        ttk.Entry(right, textvariable=self.b_scale, width=10).grid(row=10, column=1, sticky='w')
        ttk.Label(right, text='Offset').grid(row=10, column=2, sticky='w')
        ttk.Entry(right, textvariable=self.b_offset, width=10).grid(row=10, column=3, sticky='w')
        ttk.Label(right, text='Step').grid(row=11, column=0, sticky='w')
        ttk.Entry(right, textvariable=self.b_step, width=10).grid(row=11, column=1, sticky='w')
        ttk.Label(right, text='Period ms').grid(row=11, column=2, sticky='w')
        ttk.Entry(right, textvariable=self.b_period, width=10).grid(row=11, column=3, sticky='w')
        ttk.Label(right, text='shown = value x scale + offset  (e.g. kPa -> bar: 0.01, lambda -> AFR: 14.7).\n'
                              'Format (string variables): printf with one float, e.g. "%.1f", empty = decimals.\n'
                              'Step: round to a multiple (e.g. 50 rpm), 0 = off. Period: update at most every\n'
                              'N ms (e.g. 500 for temperatures), 0 = default (gauges 40 ms, texts 100 ms).',
                  foreground='#555555').grid(row=12, column=0, columnspan=4, sticky='w')
        btns = ttk.Frame(right)
        btns.grid(row=13, column=0, columnspan=4, sticky='w', pady=4)
        ttk.Button(btns, text='Add', command=self.add_binding).pack(side='left', padx=2)
        ttk.Button(btns, text='Update selected', command=self.update_binding).pack(side='left', padx=2)
        ttk.Button(btns, text='Remove selected', command=self.remove_binding).pack(side='left', padx=2)

        ttk.Separator(right).grid(row=13, column=0, columnspan=4, sticky='ew', pady=8)
        ttk.Label(right, text='Checks', font=('TkDefaultFont', 10, 'bold')).grid(row=14, column=0, sticky='w')
        self.warn = tk.Text(right, height=12, width=60, wrap='word')
        self.warn.grid(row=15, column=0, columnspan=4, sticky='nsew')
        right.rowconfigure(15, weight=1)
        right.columnconfigure(3, weight=1)

        self.status = ttk.Label(self.root, text='', padding=4, relief='sunken')
        self.status.pack(fill='x')

    # ------------------------------------------------------------ tree

    def refresh_tree(self):
        keep = self.sel
        self.tree.delete(*self.tree.get_children())
        self.item_sig.clear()
        self.item_frame.clear()
        flt = self.filter.get().lower() if hasattr(self, 'filter') else ''
        for fr in self.map.frames:
            sigs = [s for s in fr.signals
                    if not flt or flt in s.name.lower() or any(flt in u.var.lower() for u in s.ui)]
            if flt and not sigs:
                continue
            fid = self.tree.insert('', 'end', text=f'{fr.id_text}  {fr.name}', open=True)
            self.item_frame[fid] = fr
            for s in sigs:
                tags = ('off',) if not s.enabled else (('bound',) if s.ui else ())
                iid = self.tree.insert(fid, 'end', text=s.name, tags=tags, values=(
                    s.unit, s.decimals, 'yes' if s.log and s.enabled else '',
                    ', '.join(u.var for u in s.ui)))
                self.item_sig[iid] = (fr, s)
                if keep and keep[1] is s:
                    self.tree.selection_set(iid)
                    self.tree.see(iid)

    def on_select(self, ev=None):
        cur = self.tree.selection()
        if not cur or cur[0] not in self.item_sig:
            return
        self.sel = self.item_sig[cur[0]]
        fr, s = self.sel
        self.title.config(text=f'{s.name}   ({fr.name} {fr.id_text})')
        order = 'motorola' if s.motorola else 'intel'
        info = (f'bits {s.start}|{s.length} {order} {s.value_type}, '
                f'value = raw x {s.factor} + {s.offset}')
        if s.comment:
            info += f'\n{s.comment}'
        if s.multiplex:
            info += f'\nmultiplexed ({s.multiplex}): not decoded by the firmware yet'
        self.info.config(text=info)
        self.enabled.set(s.enabled)
        self.log.set(s.log)
        self.unit.set(s.unit)
        self.decimals.set(str(s.decimals))
        self.timeout.set(str(fr.timeout_ms))
        self.refresh_bindings()

    def apply_signal(self):
        if not self.sel:
            return
        fr, s = self.sel
        s.enabled = self.enabled.get()
        s.log = self.log.get()
        s.unit = self.unit.get()
        try:
            s.decimals = max(0, min(9, int(self.decimals.get())))
        except ValueError:
            pass
        try:
            fr.timeout_ms = max(0, int(self.timeout.get()))
        except ValueError:
            pass
        self.refresh_tree()
        self.refresh_status()

    # ------------------------------------------------------------ bindings

    def refresh_bindings(self):
        self.bindings.delete(0, 'end')
        if not self.sel:
            return
        for u in self.sel[1].ui:
            extra = []
            if u.scale != 1.0:
                extra.append(f'x{u.scale:g}')
            if u.offset != 0.0:
                extra.append(f'{u.offset:+g}')
            if u.format:
                extra.append(f'"{u.format}"')
            if u.step > 0:
                extra.append(f'step {u.step:g}')
            if u.period_ms > 0:
                extra.append(f'{u.period_ms} ms')
            self.bindings.insert('end', f'{u.var}  ({u.type})  {" ".join(extra)}')
        self.var_combo['values'] = [v.name for v in self.eez_vars if v.supported and not v.system]

    def on_binding_select(self, ev=None):
        i = self.bindings.curselection()
        if not i or not self.sel:
            return
        u = self.sel[1].ui[i[0]]
        self.b_var.set(u.var)
        self.b_type.set(u.type)
        self.b_scale.set(f'{u.scale:g}')
        self.b_offset.set(f'{u.offset:g}')
        self.b_format.set(u.format)
        self.b_step.set(f'{u.step:g}')
        self.b_period.set(str(u.period_ms))

    def on_var_chosen(self, ev=None):
        for v in self.eez_vars:
            if v.name == self.b_var.get():
                self.b_type.set(v.type)

    def _binding_from_fields(self):
        var = self.b_var.get().strip()
        if not var:
            messagebox.showwarning('Mapper', 'Choose a UI variable first.')
            return None
        try:
            scale = float(self.b_scale.get() or 1)
            offset = float(self.b_offset.get() or 0)
            step = float(self.b_step.get() or 0)
            period = int(self.b_period.get() or 0)
        except ValueError:
            messagebox.showwarning('Mapper', 'Scale, offset and step must be numbers, period a whole number.')
            return None
        if step < 0 or not 0 <= period <= 60000:
            messagebox.showwarning('Mapper', 'Step must be >= 0, period 0..60000 ms.')
            return None
        fmt = self.b_format.get().strip()
        if fmt and fmt.count('%') - 2 * fmt.count('%%') != 1:
            messagebox.showwarning('Mapper', 'Format needs exactly one conversion, e.g. "%.1f".')
            return None
        return UiBinding(var=var, type=self.b_type.get(), scale=scale, offset=offset, format=fmt,
                         step=step, period_ms=period)

    def add_binding(self):
        if not self.sel:
            return
        b = self._binding_from_fields()
        if b:
            self.sel[1].ui.append(b)
            self._after_binding_change()

    def update_binding(self):
        i = self.bindings.curselection()
        if not i or not self.sel:
            return
        b = self._binding_from_fields()
        if b:
            self.sel[1].ui[i[0]] = b
            self._after_binding_change()

    def remove_binding(self):
        i = self.bindings.curselection()
        if not i or not self.sel:
            return
        del self.sel[1].ui[i[0]]
        self._after_binding_change()

    def _after_binding_change(self):
        self.refresh_bindings()
        self.refresh_tree()
        self.refresh_status()

    # ------------------------------------------------------------ files

    def open_dbc(self):
        path = filedialog.askopenfilename(filetypes=[('DBC', '*.dbc'), ('All', '*')])
        if not path:
            return
        try:
            new = mapmodel.from_dbc(dbc.load(path), os.path.basename(path))
        except (OSError, ValueError) as e:
            messagebox.showerror('Mapper', str(e))
            return
        if self.map.frames and messagebox.askyesno(
                'Mapper', 'Keep the current settings and UI bindings for signals that are in the new DBC?'):
            new = mapmodel.merge(self.map, new)
        self.map = new
        self.dbc_path = path
        self.sel = None
        self.refresh_tree()
        self.refresh_status()

    def open_eez(self):
        path = filedialog.askopenfilename(filetypes=[('EEZ Studio project', '*.eez-project'), ('All', '*')])
        if not path:
            return
        try:
            self.eez_vars = eez.load(path)
        except (OSError, ValueError, KeyError) as e:
            messagebox.showerror('Mapper', str(e))
            return
        self.eez_path = path
        self.refresh_bindings()
        self.refresh_status()

    def open_map(self):
        path = filedialog.askopenfilename(filetypes=[('FP_Dash map', '*.xml'), ('All', '*')])
        if not path:
            return
        try:
            self.map = mapmodel.read_xml(path)
        except (OSError, ValueError) as e:
            messagebox.showerror('Mapper', str(e))
            return
        self.map_path = path
        self.bitrate.set(str(self.map.bitrate))
        self.sel = None
        self.refresh_tree()
        self.refresh_status()

    def _write(self, path):
        try:
            self.map.bitrate = int(self.bitrate.get())
        except ValueError:
            messagebox.showerror('Mapper', 'Bitrate must be a number.')
            return
        mapmodel.write_xml(self.map, path)
        self.map_path = path
        self.refresh_status(f'Saved {path}')

    def save_map(self):
        if self.map_path:
            self._write(self.map_path)
        else:
            self.save_map_as()

    def save_map_as(self):
        path = filedialog.asksaveasfilename(defaultextension='.xml', initialfile='map.xml',
                                            filetypes=[('FP_Dash map', '*.xml')])
        if path:
            self._write(path)

    def auto_map(self):
        if not self.eez_vars:
            messagebox.showinfo('Mapper', 'Open the EEZ Studio project first.')
            return
        added = mapmodel.auto_map(self.map, self.eez_vars)
        self.refresh_tree()
        self.refresh_bindings()
        self.refresh_status(f'Auto-map: {len(added)} bindings added')

    # ------------------------------------------------------------ status

    def refresh_status(self, msg=''):
        n_sig = sum(1 for _, s in self.map.all_signals() if s.enabled)
        n_ui = sum(len(s.ui) for _, s in self.map.all_signals() if s.enabled)
        parts = [f'{len(self.map.frames)} frames, {n_sig} signals, {n_ui} UI bindings',
                 f'EEZ: {os.path.basename(self.eez_path) if self.eez_path else "-"} ({len(self.eez_vars)} vars)',
                 f'map: {self.map_path or "(not saved)"}']
        if msg:
            parts.insert(0, msg)
        self.status.config(text='   |   '.join(parts))
        self.warn.delete('1.0', 'end')
        warnings = mapmodel.check(self.map, self.eez_vars)
        self.warn.insert('end', '\n'.join(warnings) if warnings else 'No problems found.')


def run(**kw):
    root = tk.Tk()
    MapperApp(root, **kw)
    root.mainloop()
