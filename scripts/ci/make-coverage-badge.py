#!/usr/bin/env python3
"""
Create an SVG coverage badge from gcovr summary JSON.
Input: build/coverage/summary.json with keys lines_valid, lines_covered, line_coverage_percent
Output: build/coverage/coverage-badge.svg (shields-like minimal style)
"""
from __future__ import annotations

import json
from pathlib import Path
import sys


TEMPLATE = """<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="20" role="img" aria-label="coverage: {pct}%">
  <linearGradient id="s" x2="0" y2="100%">
    <stop offset="0" stop-color="#bbb" stop-opacity=".1"/>
    <stop offset="1" stop-opacity=".1"/>
  </linearGradient>
  <mask id="m"><rect width="{width}" height="20" rx="3" fill="#fff"/></mask>
  <g mask="url(#m)">
    <rect width="70" height="20" fill="#555"/>
    <rect x="70" width="{value_w}" height="20" fill="{color}"/>
    <rect width="{width}" height="20" fill="url(#s)"/>
  </g>
  <g fill="#fff" text-anchor="middle" font-family="Verdana,Geneva,DejaVu Sans,sans-serif" font-size="11">
    <text x="35" y="15">coverage</text>
    <text x="{text_x}" y="15">{pct}%</text>
  </g>
</svg>"""


def color_for(pct: float) -> str:
    if pct >= 90:
        return "#4c1"  # brightgreen
    if pct >= 80:
        return "#97CA00"  # green
    if pct >= 60:
        return "#dfb317"  # yellow
    if pct >= 40:
        return "#fe7d37"  # orange
    return "#e05d44"  # red


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    summary_path = root / "build" / "coverage" / "summary.json"
    out_svg = root / "build" / "coverage" / "coverage-badge.svg"
    data = json.loads(summary_path.read_text(encoding="utf-8"))
    pct = float(data.get("line_coverage_percent", 0.0))
    label_w = 70
    value_str = f"{pct:.0f}%" if pct >= 10 else f"{pct:.1f}%"
    # Rough width estimate: 6px per char + padding
    value_w = max(40, 6 * len(value_str) + 10)
    width = label_w + value_w
    text_x = label_w + value_w // 2
    svg = TEMPLATE.format(width=width, value_w=value_w, text_x=text_x, pct=f"{pct:.0f}", color=color_for(pct))
    out_svg.write_text(svg, encoding="utf-8")
    print(f"Wrote badge: {out_svg}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
