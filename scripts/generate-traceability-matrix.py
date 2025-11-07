#!/usr/bin/env python3
r"""Generate a simple traceability matrix skeleton and orphan report.
Parses markdown files for ID patterns and produces:
 - reports/traceability-matrix.md (basic table placeholders)
 - reports/orphans.md (lists missing link elements)

Patterns recognized (with optional 4-char category prefix for complex projects):
  StR-[ABCD-]?\d{3}              (e.g., StR-001 or StR-CORE-001)
  REQ-(ABCD-)?(F|NF)-[A-Z+-]?\d{3}  (e.g., REQ-F-001 or REQ-AUTH-F-001)
  ADR-[ABCD-]?\d{3}              (e.g., ADR-001 or ADR-INFRA-001)
  ARC-C-[ABCD-]?\d{3}            (e.g., ARC-C-001 or ARC-C-CORE-001)
  QA-SC-[ABCD-]?\d{3}            (e.g., QA-SC-001 or QA-SC-PERF-001)
  TEST-[ABCD-]?[A-Z0-9-]+        (e.g., TEST-LOGIN-001 or TEST-AUTH-LOGIN-001)

Heuristics (improve later):
 - If a REQ appears in architecture spec or ADR file, we assume linkage
 - Components appear if referenced in architecture spec or views
 - Scenarios appear if in architecture-quality-scenarios file

Exit code 0 even if orphans exist (validation script enforces later).
"""
from __future__ import annotations
import re, pathlib, os, yaml
from collections import defaultdict

ROOT = pathlib.Path(__file__).resolve().parent.parent
REPORTS = ROOT / 'reports'
REPORTS.mkdir(exist_ok=True)

PATTERNS = {
    # Support optional 4-char category: StR-ABCD-001 or StR-001
    'stakeholder': re.compile(r'StR-(?:[A-Z]{4}-)?(?:[A-Z]+-)??\d{3}'),
    # REQ patterns - support both strict and flexible formats:
    # Strict: REQ-AUTH-F-001, REQ-F-001, REQ-AUTH-NF-001, REQ-NF-001
    # Flexible: REQ-FUNC-AUDIO-001, REQ-SEC-001, REQ-PERF-TIMING-001
    'requirement': re.compile(r'REQ-(?:[A-Z]{4}-)?(?:(?:F|NF)-(?:[A-Z]+-)?|(?:[A-Z]+-)+)\d{3}'),
    # ADR-AUTH-001 or ADR-001
    'adr': re.compile(r'ADR-(?:[A-Z]{4}-)??\d{3}'),
    # ARC-C-CORE-001 or ARC-C-001
    'component': re.compile(r'ARC-C-(?:[A-Z]{4}-)??\d{3}'),
    # QA-SC-PERF-001 or QA-SC-001
    'scenario': re.compile(r'QA-SC-(?:[A-Z]{4}-)??\d{3}'),
    # TEST-AUTH-LOGIN-001 or TEST-LOGIN-001 (flexible: alphanumeric + dash)
    'test': re.compile(r'TEST-(?:[A-Z]{4}-)?[A-Z0-9-]+'),
    # Design IDs
    'design_component': re.compile(r'DES-C-\d{3}'),
    'design_interface': re.compile(r'DES-I-\d{3}'),
    'design_data': re.compile(r'DES-D-\d{3}'),
}

# Source file extensions to scan for design and requirement references
SOURCE_EXTENSIONS = {'.hpp', '.h', '.cpp', '.c', '.cc', '.py', '.ts', '.js'}

def is_guidance(path: pathlib.Path, text: str) -> bool:
    """Check if a file should be excluded from traceability scanning.
    
    Excludes:
    - Template files in spec-kit-templates/
    - Prompt files in .github/prompts/
    - Instruction files in .github/instructions/
    - Copilot instruction files
    - Files with specType: guidance in front matter
    """
    # Exclude template and guidance directories
    excluded_paths = [
        '.github/prompts',
        '.github/instructions', 
        'spec-kit-templates',
        'docs/'  # Documentation and guides
    ]
    if any(seg in path.as_posix() for seg in excluded_paths):
        return True
    if 'copilot-instructions.md' in path.name.lower():
        return True
    if 'ADR-template.md' in path.name:
        return True
    if 'template' in path.name.lower():
        return True
    # front matter specType: guidance
    if text.startswith('---'):
        try:
            fm = text.split('---', 2)[1]
            meta = yaml.safe_load(fm) or {}
            if meta.get('specType') == 'guidance':
                return True
        except Exception:
            pass
    return False

files = []
front_matter_meta: dict[pathlib.Path, dict] = {}
for p in ROOT.rglob('*.md'):
    if 'node_modules' in p.parts or 'reports' in p.parts:
        continue
    try:
        txt = p.read_text(encoding='utf-8', errors='ignore')
    except Exception:
        continue
    if is_guidance(p, txt):
        continue
    # capture YAML front matter (if present) for stronger linkage
    if txt.startswith('---'):
        try:
            fm = txt.split('---', 2)[1]
            meta = yaml.safe_load(fm) or {}
            if isinstance(meta, dict):
                front_matter_meta[p] = meta
        except Exception:
            pass
    files.append(p)
index: dict[str, set[str]] = {k: set() for k in PATTERNS}
occurrence: dict[str, dict[str, set[pathlib.Path]]] = {k: defaultdict(set) for k in PATTERNS}

for path in files:
    try:
        text = path.read_text(encoding='utf-8', errors='ignore')
    except Exception:
        continue
    # Skip placeholder example IDs (e.g., REQ-F-000, ADR-XXX) from counting
    text = re.sub(r'ADR-XXX', '', text)
    text = re.sub(r'REQ-(F|NF)-000', '', text)
    for key, pat in PATTERNS.items():
        for match in pat.findall(text):
            index[key].add(match)
            occurrence[key][match].add(path)

# --- Source scanning for traceability (DES-* and REQ-* inside code) ---
source_design_map: dict[str, set[pathlib.Path]] = defaultdict(set)  # design id -> source paths
source_requirement_map: dict[str, set[pathlib.Path]] = defaultdict(set)  # requirement id -> source paths

for src in ROOT.rglob('*'):
    if not src.is_file():
        continue
    if src.suffix not in SOURCE_EXTENSIONS:
        continue
    # Skip build / external / node_modules
    if any(part in src.parts for part in ('build', 'node_modules', '.git')):
        continue
    try:
        content = src.read_text(encoding='utf-8', errors='ignore')
    except Exception:
        continue
    # Find design IDs in source
    for design_key in ('design_component', 'design_interface', 'design_data'):
        for match in PATTERNS[design_key].findall(content):
            source_design_map[match].add(src)
    # Direct requirement references in source (e.g., comments tagging REQ-F-001)
    for req_match in PATTERNS['requirement'].findall(content):
        source_requirement_map[req_match].add(src)

# Simple linkage inference
req_links = defaultdict(set)  # requirement -> linked identifiers (adr/component/scenario/test)
for adr in index['adr']:
    # parse requirements referenced in ADR file names or contents
    for req in index['requirement']:
        for path in occurrence['adr'][adr]:
            if req in path.read_text(encoding='utf-8', errors='ignore'):
                req_links[req].add(adr)
for scen in index['scenario']:
    scen_text_files = occurrence['scenario'][scen]
    text = '\n'.join(p.read_text(encoding='utf-8', errors='ignore') for p in scen_text_files)
    for req in index['requirement']:
        if req in text:
            req_links[req].add(scen)
for comp in index['component']:
    comp_in_files = occurrence['component'][comp]
    text = '\n'.join(p.read_text(encoding='utf-8', errors='ignore') for p in comp_in_files)
    for req in index['requirement']:
        if req in text:
            req_links[req].add(comp)
for test in index['test']:
    # Link tests to requirements if they co-occur in the same file
    test_files = occurrence['test'][test]
    for p in test_files:
        try:
            t = p.read_text(encoding='utf-8', errors='ignore')
        except Exception:
            continue
        for req in PATTERNS['requirement'].findall(t):
            req_links[req].add(test)

# Link design specs to requirements by co-occurrence in same file
for design_key in ('design_component', 'design_interface', 'design_data'):
    for des in index[design_key]:
        for p in occurrence[design_key][des]:
            try:
                t = p.read_text(encoding='utf-8', errors='ignore')
            except Exception:
                continue
            for req in PATTERNS['requirement'].findall(t):
                req_links[req].add(des)

# Propagate source files via design linkage (DES-* -> SRC:<path>)
for req, links in list(req_links.items()):
    design_ids = [l for l in links if re.match(r'^DES-(C|I|D)-\d{3}$', l)]
    for did in design_ids:
        for src_path in source_design_map.get(did, set()):
            rel = src_path.relative_to(ROOT).as_posix()
            req_links[req].add(f'SRC:{rel}')

# Direct requirement source references (REQ-* inside code)
for req_id, paths in source_requirement_map.items():
    for p in paths:
        rel = p.relative_to(ROOT).as_posix()
        req_links[req_id].add(f'SRC:{rel}')

# Link using YAML front matter if available: use document id -> relatedRequirements
for p, meta in front_matter_meta.items():
    doc_id = str(meta.get('id') or '').strip()
    if not doc_id:
        continue
    # check if doc_id matches a known pattern we index
    matched_key = None
    for key, pat in PATTERNS.items():
        if key == 'requirement':
            continue
        if pat.fullmatch(doc_id):
            matched_key = key
            break
    rel = meta.get('relatedRequirements') or []
    if matched_key and isinstance(rel, (list, tuple)):
        for req in rel:
            if isinstance(req, str) and PATTERNS['requirement'].fullmatch(req.strip()):
                req_links[req.strip()].add(doc_id)

# Orphan detection
orphans = {
    'requirements_no_links': sorted([r for r in index['requirement'] if not req_links.get(r)]),
    'scenarios_no_req': sorted([
        s for s in index['scenario']
        if not any(s in links for links in req_links.values())
    ]),
    'components_no_req': sorted([
        c for c in index['component']
        if not any(c in links for links in req_links.values())
    ]),
    'adrs_no_req': sorted([
        a for a in index['adr']
        if not any(a in links for links in req_links.values())
    ]),
}

# Allow empty repository without producing noise
if not any(index.values()) and os.environ.get('ALLOW_EMPTY_SPECS'):
    (REPORTS / 'traceability-matrix.md').write_text('# Traceability Matrix\n\n_No governed spec items found (empty scaffold mode)._', encoding='utf-8')
    (REPORTS / 'orphans.md').write_text('# Orphan Analysis\n\n_No governed spec items found (empty scaffold mode)._', encoding='utf-8')
    print('Empty scaffold: generated placeholder traceability outputs.')
    raise SystemExit(0)

def req_group(req_id: str) -> str:
    if re.search(r'^REQ-(?:[A-Z]{4}-)?F-', req_id):
        return 'Functional Requirements'
    if re.search(r'^REQ-(?:[A-Z]{4}-)?NF-', req_id):
        return 'Non-Functional Requirements'
    return 'Other Requirements'

def req_sort_key(req_id: str):
    # Extract trailing numeric code for stable sorting; fallback to full id
    m = re.search(r'(\d{3})(?!.*\d)', req_id)
    num = int(m.group(1)) if m else 0
    return (req_group(req_id), req_id, num)

# Summary metrics
total_reqs = len(index['requirement'])
linked_reqs = sum(1 for r in index['requirement'] if req_links.get(r))
coverage = (linked_reqs / total_reqs * 100.0) if total_reqs else 0.0

def group_counts(prefix_regex: str):
    subset = [r for r in index['requirement'] if re.search(prefix_regex, r)]
    if not subset:
        return (0, 0, 0.0)
    linked = sum(1 for r in subset if req_links.get(r))
    return (len(subset), linked, linked / len(subset) * 100.0)

fn_total, fn_linked, fn_cov = group_counts(r'^REQ-(?:[A-Z]{4}-)?F-')
nf_total, nf_linked, nf_cov = group_counts(r'^REQ-(?:[A-Z]{4}-)?NF-')

matrix_lines = [
    '# Traceability Matrix (Categorized)',
    '',
    '## Summary',
    '',
    f'- Requirements total: {total_reqs}',
    f'- Requirements linked (≥1 element): {linked_reqs}',
    f'- Overall Coverage: {coverage:.1f}%',
    f'- Functional Coverage: {fn_linked}/{fn_total} ({fn_cov:.1f}%)',
    f'- Non-Functional Coverage: {nf_linked}/{nf_total} ({nf_cov:.1f}%)',
    '',
    '### Legend',
    '- ADR: Architecture Decision Record',
    '- ARC-C: Architecture Components',
    '- DES-C/I/D: Design Component / Interface / Data Model',
    '- QA-SC: Quality Scenario',
    '- TEST: Verification and Validation Spec',
    '',
]

sections = [
    ('Functional Requirements', r'^REQ-(?:[A-Z]{4}-)?F-'),
    ('Non-Functional Requirements', r'^REQ-(?:[A-Z]{4}-)?NF-'),
    ('Other Requirements', r'^(?!REQ-(?:[A-Z]{4}-)?(?:F|NF)-)')
]

def categorize_links(links: set[str]) -> dict[str, list[str]]:
    cats = {
        'ADR': [],
        'Architecture': [],
        'Design Components': [],
        'Design Interfaces': [],
        'Design Data Models': [],
        'Scenarios': [],
        'Tests': [],
        'Source Files': [],
    }
    for l in sorted(links):
        if re.match(r'^ADR-', l):
            cats['ADR'].append(l)
        elif re.match(r'^ARC-C-', l):
            cats['Architecture'].append(l)
        elif re.match(r'^DES-C-\d{3}$', l):
            cats['Design Components'].append(l)
        elif re.match(r'^DES-I-\d{3}$', l):
            cats['Design Interfaces'].append(l)
        elif re.match(r'^DES-D-\d{3}$', l):
            cats['Design Data Models'].append(l)
        elif re.match(r'^QA-SC-', l):
            cats['Scenarios'].append(l)
        elif re.match(r'^TEST-', l):
            cats['Tests'].append(l)
        elif re.match(r'^SRC:', l):
            cats['Source Files'].append(l)
    return cats

for section_title, pattern in sections:
    section_reqs = sorted([r for r in index['requirement'] if re.search(pattern, r)], key=req_sort_key)
    if not section_reqs:
        continue
    matrix_lines.append(f'## {section_title}')
    matrix_lines.append('')
    matrix_lines.append('| Requirement | ADRs | Architecture Components | Design Components | Design Interfaces | Design Data Models | Scenarios | Tests | All Linked |')
    matrix_lines.append('|-------------|------|-------------------------|-------------------|-------------------|--------------------|-----------|-------|-------------|-----------|')
    for req in section_reqs:
        raw_links = req_links.get(req, set())
        cats = categorize_links(raw_links)
        def fmt(key):
            return ', '.join(cats[key]) if cats[key] else ''
        all_flat = ', '.join(sorted(raw_links)) if raw_links else ''
        matrix_lines.append(
            f"| {req} | {fmt('ADR')} | {fmt('Architecture')} | {fmt('Design Components')} | {fmt('Design Interfaces')} | {fmt('Design Data Models')} | {fmt('Scenarios')} | {fmt('Tests')} | {fmt('Source Files')} | {all_flat or '(none)'} |"
        )
    matrix_lines.append('')

(REPORTS / 'traceability-matrix.md').write_text('\n'.join(matrix_lines), encoding='utf-8')

orphan_lines = ['# Orphan Analysis', '']
for category, items in orphans.items():
    orphan_lines.append(f'## {category}')
    if not items:
        orphan_lines.append('- None')
    else:
        for item in items:
            orphan_lines.append(f'- {item}')
    orphan_lines.append('')
(REPORTS / 'orphans.md').write_text('\n'.join(orphan_lines), encoding='utf-8')

print('Generated reports/traceability-matrix.md and reports/orphans.md')
