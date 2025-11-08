#!/usr/bin/env python3
import json, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
trace = ROOT / 'build' / 'traceability.json'
if not trace.exists():
    print('traceability.json not found at', trace)
    sys.exit(1)
D = json.loads(trace.read_text(encoding='utf-8'))
metrics = D.get('metrics', {})
scn = metrics.get('requirement_to_scenario', {}).get('details', {})
tst = metrics.get('requirement_to_test', {}).get('details', {})

def missing(d):
    out = []
    for rid, refs in d.items():
        f = refs.get('forward_refs') or []
        r = refs.get('reverse_refs') or []
        if len(f) == 0 and len(r) == 0:
            out.append(rid)
    return sorted(out)

miss_scn = missing(scn)
miss_tst = missing(tst)
items = D.get('items', [])
qa_ids = [it['id'] for it in items if str(it.get('id','')).upper().startswith('QA')]
test_ids = [it['id'] for it in items if str(it.get('id','')).upper().startswith('TEST')]
linked = set()
for refs in scn.values():
    for k in ('forward_refs','reverse_refs'):
        for x in refs.get(k) or []:
            linked.add(x)
for refs in tst.values():
    for k in ('forward_refs','reverse_refs'):
        for x in refs.get(k) or []:
            linked.add(x)
orph_qa = sorted([q for q in qa_ids if q not in linked])
orph_test = sorted([t for t in test_ids if t not in linked])

print('Missing Scenario links:', len(miss_scn))
if miss_scn:
    print('  IDs:', ', '.join(miss_scn[:50]))
print('Missing Test links:', len(miss_tst))
if miss_tst:
    print('  IDs:', ', '.join(miss_tst[:50]))
print('Orphan QA scenarios:', len(orph_qa))
if orph_qa:
    print('  IDs:', ', '.join(orph_qa[:50]))
print('Orphan TEST cases:', len(orph_test))
if orph_test:
    print('  IDs:', ', '.join(orph_test[:50]))
