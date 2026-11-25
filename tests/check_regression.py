#!/usr/bin/env python3
"""Validate deterministic PARSIFAL regression test outputs against a baseline."""

import argparse
import json
import math
import pathlib
import re
import sys

METRICS = [
    ("angle", int),
    ("QindMPV", float),
    ("Qread", float),
    ("RatioQ", float),
    ("Size", float),
    ("CCres", float),
    ("TPCres", float),
    ("T0res", float),
    ("Eff", float),
]


def parse_summary(path: pathlib.Path) -> dict:
    """Parse the last non-empty line of a PARSIFAL summary.txt file."""
    if not path.is_file():
        raise FileNotFoundError(f"Missing summary file: {path}")
    lines = [ln.strip() for ln in path.read_text().splitlines() if ln.strip()]
    if not lines:
        raise ValueError(f"Empty summary file: {path}")
    fields = lines[-1].split()
    if len(fields) != len(METRICS):
        raise ValueError(
            f"Expected {len(METRICS)} columns in {path}, got {len(fields)}: {lines[-1]}"
        )
    result = {}
    for (name, cast), raw in zip(METRICS, fields):
        val = cast(raw)
        if isinstance(val, float) and not math.isfinite(val):
            raise ValueError(f"Non-finite value for {name} in {path}: {raw}")
        result[name] = val
    return result


def parse_info_sim(path: pathlib.Path) -> dict:
    """Extract key metadata fields from info_sim_angle<deg>.txt."""
    if not path.is_file():
        raise FileNotFoundError(f"Missing info_sim file: {path}")
    text = path.read_text()
    meta = {}
    for key, pattern in [
        ("asic", r"^Electronics \(ASIC\)\s*:\s*(\S+)"),
        ("nevents", r"^Number of Events\s*:\s*(\d+)"),
        ("seed", r"^Seed\s*:\s*(\d+)"),
        ("ms_per_event", r"^Simulation Run Time\s*:.*?\(([\d.]+)\s*ms/event\)"),
    ]:
        m = re.search(pattern, text, re.MULTILINE)
        if m:
            meta[key] = m.group(1)
    return meta


def check_sanity(case_name: str, metrics: dict) -> list[str]:
    """Run basic physical sanity checks on summary metrics."""
    errors = []
    if metrics["QindMPV"] <= 0.0:
        errors.append(f"{case_name}: QindMPV <= 0 ({metrics['QindMPV']})")
    if metrics["Qread"] <= 0.0:
        errors.append(f"{case_name}: Qread <= 0 ({metrics['Qread']})")
    if metrics["Size"] < 1.0:
        errors.append(f"{case_name}: cluster Size < 1 ({metrics['Size']})")
    if not (0.0 <= metrics["Eff"] <= 1.0):
        errors.append(f"{case_name}: Eff out of [0,1] ({metrics['Eff']})")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-dir", required=True, help="Temporary data directory")
    parser.add_argument("--baseline", required=True, help="Path to baseline.json")
    parser.add_argument(
        "--cases",
        nargs="+",
        required=True,
        help="List of label:run_id:angle entries (e.g. APV:36:0 TIGER:21:0 TORA:56:0)",
    )
    parser.add_argument("--seed", type=int, required=True, help="Expected RNG seed")
    parser.add_argument("--nevents", type=int, required=True, help="Expected event count")
    parser.add_argument(
        "--tol",
        type=float,
        default=1e-4,
        help="Absolute tolerance when comparing numeric metrics (default: 1e-4)",
    )
    parser.add_argument(
        "--update-baseline",
        action="store_true",
        help="Write observed metrics to --baseline instead of comparing",
    )
    args = parser.parse_args()

    data_dir = pathlib.Path(args.data_dir)
    baseline_path = pathlib.Path(args.baseline)

    observed = {}
    errors = []

    print(
        f"{'Case':<10} {'Run':>4} {'QindMPV':>9} {'Qread':>9} {'RatioQ':>8} "
        f"{'Size':>7} {'CCres':>6} {'TPCres':>7} {'T0res':>6} {'Eff':>5} {'ms/evt':>8}"
    )
    print("-" * 92)

    for entry in args.cases:
        label, run_id, angle = entry.split(":")
        run_dir = data_dir / run_id
        summary = parse_summary(run_dir / "summary.txt")
        info = parse_info_sim(run_dir / f"info_sim_angle{angle}.txt")

        if int(info.get("seed", -1)) != args.seed:
            errors.append(f"{label}: expected seed {args.seed}, got {info.get('seed')}")
        if int(info.get("nevents", -1)) != args.nevents:
            errors.append(f"{label}: expected nevents {args.nevents}, got {info.get('nevents')}")

        errors.extend(check_sanity(label, summary))
        observed[label] = {
            "run_id": int(run_id),
            "angle": int(angle),
            "seed": args.seed,
            "nevents": args.nevents,
            "metrics": summary,
        }

        ms_evt = info.get("ms_per_event", "-")
        print(
            f"{label:<10} {run_id:>4} {summary['QindMPV']:>9.4f} {summary['Qread']:>9.4f} "
            f"{summary['RatioQ']:>8.4f} {summary['Size']:>7.4f} {summary['CCres']:>6.0f} "
            f"{summary['TPCres']:>7.0f} {summary['T0res']:>6.1f} {summary['Eff']:>5.2f} {ms_evt:>8}"
        )

    if errors:
        print("\nSANITY CHECK FAILED:", file=sys.stderr)
        for err in errors:
            print(f"  - {err}", file=sys.stderr)
        return 1

    if args.update_baseline:
        baseline_path.parent.mkdir(parents=True, exist_ok=True)
        baseline_path.write_text(json.dumps(observed, indent=2) + "\n")
        print(f"\nBaseline updated: {baseline_path}")
        return 0

    if not baseline_path.is_file():
        print(
            f"\nERROR: Baseline file {baseline_path} not found. "
            f"Run with --update-baseline first.",
            file=sys.stderr,
        )
        return 1

    expected = json.loads(baseline_path.read_text())
    mismatches = []
    for label, obs in observed.items():
        if label not in expected:
            mismatches.append(f"{label}: missing from baseline")
            continue
        exp_m = expected[label]["metrics"]
        obs_m = obs["metrics"]
        for name, _ in METRICS:
            diff = abs(obs_m[name] - exp_m[name])
            if diff > args.tol:
                mismatches.append(
                    f"{label}.{name}: expected {exp_m[name]}, got {obs_m[name]} (diff={diff:.6g})"
                )

    if mismatches:
        print("\nREGRESSION CHECK FAILED:", file=sys.stderr)
        for m in mismatches:
            print(f"  - {m}", file=sys.stderr)
        return 1

    print("\nREGRESSION CHECK PASSED (all 3 ASICs match baseline deterministically).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
