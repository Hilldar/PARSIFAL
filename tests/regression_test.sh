#!/bin/bash
# Automated deterministic regression test for PARSIFAL across APV, TIGER, and TORA.
# Runs into an isolated temporary data directory (tests/tmp_data) so production
# data/ directories are never modified.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${REPO_ROOT}"

SEED=12345
NEVENTS=10
THREADS=2
UPDATE_BASELINE=0
KEEP_TMP=0

while [ $# -gt 0 ]; do
  case "$1" in
    --update-baseline) UPDATE_BASELINE=1; shift ;;
    --seed)            SEED="$2"; shift 2 ;;
    --nevents)         NEVENTS="$2"; shift 2 ;;
    --threads)         THREADS="$2"; shift 2 ;;
    --keep-tmp)        KEEP_TMP=1; shift ;;
    -h|--help)
      echo "Usage: $0 [--update-baseline] [--seed U] [--nevents N] [--threads T] [--keep-tmp]"
      exit 0
      ;;
    *)
      echo "ERROR: Unknown argument '$1'" >&2
      exit 1
      ;;
  esac
done

if [ ! -x ./bin/Simulate ] || [ ! -x ./bin/Analysis ]; then
  echo "ERROR: ./bin/Simulate or ./bin/Analysis not found. Build first with 'make all'." >&2
  exit 2
fi

if [ -f bash_PARSIFAL ]; then
  # shellcheck disable=SC1091
  source bash_PARSIFAL
fi

TMP_DATA="${REPO_ROOT}/tests/tmp_data"
rm -rf "${TMP_DATA}"
mkdir -p "${TMP_DATA}"

cleanup() {
  if [ "${KEEP_TMP}" -eq 0 ]; then
    rm -rf "${TMP_DATA}"
  fi
}
trap cleanup EXIT

export PARSIFAL="${REPO_ROOT}"
export PARSIFAL_home="${REPO_ROOT}"
export PARSIFAL_data="${TMP_DATA}"
export OMP_NUM_THREADS="${THREADS}"

# Representative high-HV runs for APV (36), TIGER (21), and TORA (56) at angle 0
CASES=("APV:36:0" "TIGER:21:0" "TORA:56:0")

echo "Running PARSIFAL deterministic regression test (seed=${SEED}, nevents=${NEVENTS}, OMP_NUM_THREADS=${THREADS})..."

for entry in "${CASES[@]}"; do
  IFS=":" read -r label run_id angle <<< "${entry}"
  sim_log="${TMP_DATA}/sim_${label}_${run_id}.log"
  ana_log="${TMP_DATA}/ana_${label}_${run_id}.log"
  echo "  -> ${label} (run ${run_id}, angle ${angle} deg)..."
  ./bin/Simulate "${run_id}" "${angle}" --seed "${SEED}" --nevents "${NEVENTS}" > "${sim_log}" 2>&1
  ./bin/Analysis "${run_id}" "${angle}" > "${ana_log}" 2>&1
  if grep -Eqi '(segmentation violation|\*\*\* Break \*\*\*|ERROR:)' "${sim_log}" "${ana_log}"; then
    echo "ERROR: Runtime error detected in logs for ${label} (run ${run_id}):" >&2
    grep -Eni '(segmentation violation|\*\*\* Break \*\*\*|ERROR:)' "${sim_log}" "${ana_log}" >&2
    exit 3
  fi
done

PY_ARGS=(
  --data-dir "${TMP_DATA}"
  --baseline "${SCRIPT_DIR}/baseline.json"
  --seed "${SEED}"
  --nevents "${NEVENTS}"
  --cases "${CASES[@]}"
)
if [ "${UPDATE_BASELINE}" -eq 1 ]; then
  PY_ARGS+=(--update-baseline)
fi

python3 "${SCRIPT_DIR}/check_regression.py" "${PY_ARGS[@]}"
