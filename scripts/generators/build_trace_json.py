#!/usr/bin/env python3
"""Build Traceability JSON

Consumes build/spec-index.json and produces build/traceability.json capturing
forward and backward links for visualization or further QA tooling.
"""
from __future__ import annotations
import json
from pathlib import Path
from collections import defaultdict

ROOT = Path(__file__).resolve().parents[2]
INDEX = ROOT / 'build' / 'spec-index.json'
OUT = ROOT / 'build' / 'traceability.json'

PREFIX_ORDER = ['StR', 'REQ', 'DES', 'ARC', 'ADR', 'QA', 'TEST']


def main() -> int:
    if not INDEX.exists():
        print('Missing spec-index.json; run spec_parser first')
        return 1
    data = json.loads(INDEX.read_text(encoding='utf-8'))
    items = data.get('items', [])
    by_id = {i['id']: i for i in items}

    forward = {i['id']: i.get('references', []) for i in items}
    backward = defaultdict(list)
    for src, refs in forward.items():
        for r in refs:
            backward[r].append(src)

    # Simple completeness metrics (raw forward-reference presence)
    metrics = {}
    for prefix in PREFIX_ORDER:
        ids = [i['id'] for i in items if i['id'].startswith(prefix)]
        linked_forward = [i for i in ids if any(r for r in forward[i])]
        metrics_key = prefix.lower() if prefix != 'REQ' else 'requirement_raw'
        metrics[metrics_key] = {
            'total': len(ids),
            'with_links': len(linked_forward),
            'coverage_pct': (len(linked_forward) / len(ids) * 100) if ids else 100.0,
            'definition': 'Forward reference presence (may include intra-class links)'
        }

    # Refined requirement coverage: requirement must link (forward or backward) to at least one non-REQ artifact
    req_ids = [i['id'] for i in items if i['id'].startswith('REQ')]
    non_req_prefixes = {'StR','DES','ARC','ADR','QA','TEST'}
    refined_with = 0
    refined_details = {}
    for rid in req_ids:
        fwd = forward.get(rid, [])
        fwd_non_req = [r for r in fwd if any(r.startswith(p) for p in non_req_prefixes)]
        bwd_sources = [src for src in backward.get(rid, []) if any(src.startswith(p) for p in non_req_prefixes)]
        has_any = bool(fwd_non_req or bwd_sources)
        if has_any:
            refined_with += 1
        refined_details[rid] = {
            'forward_non_req': sorted(fwd_non_req),
            'backward_non_req_sources': sorted(bwd_sources),
            'has_any_non_req_link': has_any
        }
    metrics['requirement'] = {
        'total': len(req_ids),
        'with_links': refined_with,
        'coverage_pct': (refined_with / len(req_ids) * 100) if req_ids else 100.0,
        'definition': 'Requirement linked (forward/backward) to at least one non-REQ artifact (StR, DES, ARC, ADR, QA, TEST)',
        'details': refined_details
    }

    # Requirement-specific linkage dimensions
    # We consider a requirement linked to ADR/Scenario/Test if EITHER:
    #  (a) The requirement lists a forward reference to an ADR/QA/TEST item, OR
    #  (b) Any ADR/QA/TEST item lists a reference to the requirement (backward inference)
    # This symmetric approach prevents under-reporting when only downstream artifacts
    # embed the linkage (common when ADRs or scenarios enumerate the driving REQs).
    # reuse req_ids

    # Pre-compute reverse requirement linkage sets from non-requirement items
    reverse_links = {
        'ADR': defaultdict(set),  # requirement -> set(ADR ids)
        'QA': defaultdict(set),   # requirement -> set(QA scenario ids)
        'TEST': defaultdict(set), # requirement -> set(TEST ids)
    }
    for itm in items:
        iid = itm['id']
        if iid.startswith(('ADR','QA','TEST')):
            for ref in itm.get('references', []):
                if ref.startswith('REQ'):
                    if iid.startswith('ADR'):
                        reverse_links['ADR'][ref].add(iid)
                    elif iid.startswith('QA'):
                        reverse_links['QA'][ref].add(iid)
                    elif iid.startswith('TEST'):
                        reverse_links['TEST'][ref].add(iid)

    def req_link_stat(target_prefix: str):
        count_total = len(req_ids)
        count_with = 0
        details = {}
        for rid in req_ids:
            fwd_refs = forward.get(rid, [])
            has_forward = any(r.startswith(target_prefix) for r in fwd_refs)
            rev_set = reverse_links[target_prefix].get(rid, set()) if target_prefix in reverse_links else set()
            if has_forward or rev_set:
                count_with += 1
            details[rid] = {
                'forward_refs': [r for r in fwd_refs if r.startswith(target_prefix)],
                'reverse_refs': sorted(list(rev_set)),
            }
        pct = (count_with / count_total * 100) if count_total else 100.0
        return {
            'total_requirements': count_total,
            'requirements_with_link': count_with,
            'coverage_pct': pct,
            'details': details,
            'inference': 'forward+backward'
        }

    metrics['requirement_to_design'] = req_link_stat('DES')
    metrics['requirement_to_ADR'] = req_link_stat('ADR')
    metrics['requirement_to_scenario'] = req_link_stat('QA')
    metrics['requirement_to_test'] = req_link_stat('TEST')

    OUT.write_text(
        json.dumps(
            {
                'items': items,
                'forward': forward,
                'backward': backward,
                'metrics': metrics,
            },
            indent=2,
        ),
        encoding='utf-8',
    )
    print(f"Wrote {OUT}")
    return 0

if __name__ == '__main__':
    import sys
    raise SystemExit(main())
