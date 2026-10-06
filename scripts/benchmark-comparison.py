#!/usr/bin/env python3
"""Render a direct snprintf vs ctprintf comparison from Google Benchmark JSON.

Usage: benchmark-comparison.py results.json output.html [summary.md]
"""
import html
import json
import sys
from collections import defaultdict


def load(path):
    with open(path) as f:
        data = json.load(f)
    times = defaultdict(dict)
    for b in data["benchmarks"]:
        if b.get("run_type") == "aggregate" and b.get("aggregate_name") != "mean":
            continue
        name = b.get("run_name", b["name"])
        _, impl, case = name.split("_", 2)
        times[case][impl] = (b["cpu_time"], b.get("time_unit", "ns"))
    return {c: v for c, v in times.items() if "snprintf" in v and "ctprintf" in v}


def main():
    cases = load(sys.argv[1])
    rows = []
    for case, v in sorted(cases.items()):
        s, unit = v["snprintf"]
        c, _ = v["ctprintf"]
        rows.append((case, s, c, unit, s / c))

    md = ["| Case | snprintf | ctprintf | speedup |", "|---|---|---|---|"]
    for case, s, c, unit, r in rows:
        md.append(f"| {case} | {s:.1f} {unit} | {c:.1f} {unit} | {r:.2f}x |")
    if len(sys.argv) > 3:
        with open(sys.argv[3], "a") as f:
            f.write("## snprintf vs ctprintf\n\n" + "\n".join(md) + "\n")

    body = []
    for case, s, c, unit, r in rows:
        m = max(s, c)
        body.append(
            f"<h2>{html.escape(case)} <small>({r:.2f}x {'faster' if r >= 1 else 'slower'})</small></h2>"
            f"<div class=bar><div class=sn style='width:{s / m * 100:.1f}%'>snprintf {s:.1f} {unit}</div></div>"
            f"<div class=bar><div class=ct style='width:{c / m * 100:.1f}%'>ctprintf {c:.1f} {unit}</div></div>"
        )
    page = f"""<!doctype html><meta charset=utf-8><title>snprintf vs ctprintf</title>
<style>body{{font-family:sans-serif;max-width:800px;margin:2em auto}}
.bar{{background:#eee;margin:4px 0}}.bar div{{padding:4px;color:#fff;white-space:nowrap}}
.sn{{background:#888}}.ct{{background:#2a7}}small{{font-weight:normal;color:#555}}</style>
<h1>snprintf vs ctprintf</h1><p>Lower is better (CPU time per call).
<a href="./">History dashboard</a></p>{''.join(body)}"""
    with open(sys.argv[2], "w") as f:
        f.write(page)


main()
