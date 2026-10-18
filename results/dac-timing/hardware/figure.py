#!/usr/bin/env python3
"""Compact view of the already-measured CH3/CH4 native transition samples."""
import csv,json
from pathlib import Path
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
root=Path(__file__).resolve().parent
m=json.loads((root/'analysis.json').read_text());rows=list(csv.DictReader((root/'edge-controls-native.csv').open()))
colors={'emulator':'#d45b24','delayed_fade':'#007f79','hardware':'#4e5e9a'}
labels={'emulator':'Before','delayed_fade':'Corrected timing','hardware':'Recorded DMG'}
fig,axs=plt.subplots(2,2,figsize=(12,6.6),layout='constrained')
for row,ident in enumerate([182,184]):
 case=next(c for c in m['cases'] if c['id']==ident)
 for col,kind in enumerate(['DAC-off','DAC-on']):
  ax=axs[row,col];event=next(e for e in case['events'] if e['kind']==kind);on=float(event['write']['run_start_seconds'])
  for name in colors:
   values=[v for v in rows if int(v['case'])==ident and v['event']==kind and v['source']==name]
   ax.plot([(float(v['mapped_time_s'])-on)*1000 for v in values], [float(v['recording_amplitude'])*1000 for v in values],color=colors[name],label=labels[name],lw=1.5,marker='.',ms=2.8)
  ax.axvline(0,color='#555',lw=.8,ls=':');ax.axvline(32/48,color='#555',lw=.8,ls='--');ax.grid(alpha=.18)
  ax.set_xlim(-32/48,64/48);ax.set_title(f"{'CH3 · wave' if ident==182 else 'CH4 · noise'} — {'DAC disabled' if kind=='DAC-off' else 'DAC enabled'}",fontsize=11)
  ax.set_xlabel('Milliseconds from CPU register write');ax.set_ylabel('Recording amplitude ×1000')
axs[0,0].legend(loc='lower left',fontsize=8)
fig.suptitle('The DAC fade was one synthesis delay early',fontsize=16)
fig.text(.5,-.035,'Dotted: CPU write. Dashed: 32-sample synthesis midpoint (0.667 ms). Same frozen hardware timing/gain for both renders; native samples.',ha='center',fontsize=8)
for ext in ['png','pdf']:fig.savefig(root/f'wave-noise-timing.{ext}',dpi=170,bbox_inches='tight')
