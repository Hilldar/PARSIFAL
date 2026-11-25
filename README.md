# PARSIFAL

**PARSIFAL** is a C++/ROOT parameterized Monte Carlo simulator for Micro-Pattern Gaseous Detectors (MPGDs), covering ionization, electron drift and diffusion, gas amplification, resistive charge dispersion, front-end ASIC response (**APV**, **TIGER**, **TORA**), and 1D cluster reconstruction (Charge Centroid and µTPC).

## Authors
<ul>
<li>Riccardo Farinelli          <rfarinelli@bo.infn.it></li>
<li>Lia Lavezzi                 <lia.lavezzi@to.infn.it></li>
<li>Francesco Chiapponi         <chiappon@bo.infn.it></li>
</ul>
All rights reserved.

## License
For the licensing terms see `$PARSIFAL/LICENSE` (GNU GPLv3).

---

## Features & Simulation Pipeline

- **Supported detectors:** µ-RWELL (Planar 1D, default active setup), triple-GEM (Planar 2D, with/without magnetic field), µRTube (`R-Phi`).
- **Supported front-end ASICs:** `0` = **APV**, `1` = **TIGER**, `2` = **TORA**.
- **Physics & readout chain:**
  1. Primary & secondary ionization (`Ionization`, Garfield++-parametrized gas mixtures: `Ar:iC4H10 90:10` and `Ar:CO2:CF4 45:15:40`).
  2. Electron drift and spatial/temporal diffusion (`ElectronDrift`).
  3. Gas multiplication (`DetectorGain`, Polya/histogram sampling).
  4. Fast (electron) + slow (ion tail) signal induction and resistive charge dispersion (`Resistive`, T2K+Shepherd model).
  5. White noise with pre-$t_0$ buffer, optional FFT bandpass filtering, shaper convolution, saturation, and digitization (`Readout`).
  6. Hit finding and 1D clustering with Charge Centroid (CC) and µTPC reconstruction (`Reconstruction`).
- **Parallelism:** OpenMP multi-threading (`-O2 -fopenmp`).

## Branches

| Branch | Description |
| :--- | :--- |
| `triplegem` | Legacy triple-GEM simulation code |
| `main` | µ-RWELL simulation with ion tail, T2K resistive dispersion, white noise, and OpenMP |
| `Tora` | TORA ASIC integration, FFT filtering, independent rise/trail thresholds, APV/TIGER tuning |
| `Parsifal-AI` | Critical physics/OpenMP/ROOT bugfixes, kernel optimizations, `info_sim_angle<deg>.txt`, parametric `HVscan.C` |

---

## Requirements

- A working installation of [ROOT 6](https://root.cern.ch/) (C++17).
- `g++` with OpenMP support (`-fopenmp`).

## Quick Start (HOWTO)

```bash
# 1. Load ROOT environment
source $ROOTSYS/bin/thisroot.sh

# 2. Enter the PARSIFAL directory and generate bash_PARSIFAL
cd /path/to/PARSIFAL
source init_PARSIFAL.sh

# 3. Load PARSIFAL environment variables and aliases
source bash_PARSIFAL

# 4. Build Simulate and Analysis
parsifal -M        # clean build (required whenever .h headers change)
# parsifal -m      # incremental build

# 5. Show available commands
parsifal -h
```

### Running Simulations, Analysis & Plots

```bash
# Run simulation + analysis at 0 deg (1-event sanity configs)
parsifal -S 2 0    # APV
parsifal -S 1 0    # TIGER
parsifal -S 0 0    # TORA

# Run analysis only / open ROOT output
parsifal -A <runID> [angle]
parsifal -O <runID> <angle>

# Generate HV scan comparison PDFs against experimental APV/TIGER data
root -l -b -q 'src/Plot/HVscan.C("APV","data","pdf")'
root -l -b -q 'src/Plot/HVscan.C("TIGER","data","pdf")'
```

---

## Configuration (`run_list.txt`)

Each numeric row in `run_list.txt` specifies a 24-column run configuration:

| Col | Name | Description / Unit | Col | Name | Description / Unit |
|---:|---|---|---:|---|---|
| 0 | `Run` | Run ID | 12 | `APV_ped` | APV pedestal std |
| 1 | `nEvt` | Number of events | 13 | `APV_thr` | APV threshold factor |
| 2 | `GainFctr` | Gain tuning factor | 14 | `TIGER_E_fC_thr` | TIGER E-branch threshold [fC] |
| 3 | `SpaceDiffFctr` | Spatial diffusion factor | 15 | `TIGER_T_fC_thr` | TIGER T-branch threshold [fC] |
| 4 | `TimeDiffFctr` | Temporal diffusion factor | 16 | `TORA_thr_rise` | TORA rising threshold [fC] |
| 5 | `Res_RC` | Resistive RC $\tau$ [ns] | 17 | `TORA_thr_trail` | TORA trailing threshold [fC] |
| 6 | `Res_Sigma0` | Resistive $\sigma_0$ [mm] | 18 | `TORA_gain` | TORA gain [mV/fC] |
| 7 | `DetGain` | Mean detector gain | 19 | `TORA_tau` | TORA shaping time $\tau$ [ns] |
| 8 | `Noise_fC` | Input noise [fC] | 20 | `FFT_fmin` | FFT high-pass cutoff [Hz] |
| 9 | `Thr_fC` | General threshold [fC] | 21 | `FFT_fmax` | FFT low-pass cutoff [Hz] |
| 10 | `IT_Length` | Ion tail length [ns] | 22 | `Electronics` | `0`=APV, `1`=TIGER, `2`=TORA |
| 11 | `IT_ratio` | Slow (ion) signal fraction | 23 | `Strip_Pitch` | Strip pitch [mm] |

### Compile-Time Switches

- **`src/Common/Common.h`:** stage bypass flags `NO_PrimaryIonization`, `NO_SecondaryIonization`, `NO_Gain`, `NO_Drift`, `NO_Resistive`, `NO_Readout`, `NO_Saturation`, `NO_Noise`, `NO_FFT`, `NO_Capacitive`, `NO_Hit`, `NO_Cluster`, `NO_uTPC_Correction`, `NO_Ion_Tail` (`false` = active, `true` = bypassed), plus `fast_simulation`, `Bfield`, `SEED`, `n_ns`, `n_ns_buffer`.
- **`src/main/Parsifal.C`:** `print_plot_above_thr` (kept `0` by default for multi-event scans; set to `1` only for single-event waveform PDF/PNG inspection).

## Output Files (`data/<runID>/`)

- `run_angle<deg>.root`: ROOT `TTree` (`tree`) with ionization, channel, hit, and 1D cluster branches.
- `info_sim_angle<deg>.txt`: human-readable run report with start/end timestamps, simulation and total elapsed time, OpenMP threads, decoded `run_list.txt` parameters, and compile-time flags.
- `summary.txt`: reconstructed analysis metrics (`Angle`, `Qind MPV`, `Qread`, `RatioQ`, `Size`, `CCres`, `TPCres`, `T0res`, `Eff`).

## Known Limitations & Future Upgrades

- Finalize TORA front-end charge/time extraction in `Readout::Get_Charge_TORA`.
- Extend ionization parameterizations beyond muons in `Ar:iC4H10 (90:10)` and `Ar:CO2:CF4 (45:15:40)`.
- Move detector setup selection (triple-GEM / µ-RWELL / µRTube) and `Common.h` compile-time flags to runtime configuration.

---

## ChangeLog

### 2026-11-25
- Updated and synchronized `README`, `README.md`, and `ChangeLog` across project branches.
- Refactored `src/Plot/HVscan.C` to support parametric APV and TIGER comparison modes against experimental data.
- Updated `run_list.txt` APV scan (runs 30–39) event counts from 100 to 1000.
- Added `log/` to `.gitignore` and created the `Parsifal-AI` development branch.

### 2026-11-24
- Fixed critical physics, reconstruction, and NTuple bugs (uninitialized `y` in `Ionization`, premature `~Primary()`, inverted `fast_T` comparison, triple-GEM collection efficiency in `fast_simulation`, `MagField` propagation, 200-entry secondary CDF, APV `SumQ`, TIGER threshold guards, `cap_charge_2`, `Phiview` branch, boolean sentinels).
- Fixed OpenMP data races in `Resistive::Resist()` and `Readout::Background()`.
- Fixed memory leaks and ROOT ownership (`SetDirectory(nullptr)` on `ElectronicChannel` histograms, `delete` instead of manual destructors, `FreeMemory()`, FFT plan reuse and cleanup).
- Fixed `Makefile` (`-O2 -Wall`), `PARSIFAL.sh`, `main_sim`/`main_ana` exit codes, and `summary.txt` parallel write race.
- Added per-angle simulation summary files `data/<run>/info_sim_angle<deg>.txt` with timestamps, elapsed time, and readable configuration.
- Optimized performance at unchanged physics (precomputed shaper and resistive kernels with `TH1D::GetArray()`, cached `TF1::GetRandom()`, `"N0"` APV fit, default `print_plot_above_thr = 0`).
- Validated APV (runs 30–39) and TIGER (runs 10–24) HV scans against experimental data.

### 2026-06-30
- Included TORA ASIC and tuning of APV and TIGER.

### 2025-10-15
- Modified noise calibrations.

### 2025-08-08
- Reduced frequency range in noise and modified white noise model.
- Moved from Morello to T2K resistive model.

### 2025-07-03
- Parallelized the code with OpenMP.

### 2025-05-21
- Included TIGER noise on E-branch (previously fixed to 0.1 fC).

### 2025-05-10
- Modified TIGER saturation.

### 2025-03-19
- Updated white noise calibration (fixed amplitude with shaping time).
- Changed resistive model from Morello to T2K.

### 2025-03-14
- Changed noise model from 1/f to white noise.
- Included buffer current to improve noise behavior at `t = 0`.
- Updated electron drift parameters in µRWELL.
- Evaluated noise independently for each channel.

### 2025-02-13
- Modified APV channel above-threshold condition (`sigma_noise -> sigma_noise/2 + charge_cut`).

### 2025-02-03
- Updated order of steps: µRWELL performs electron diffusion then gain; GEM performs gain then diffusion.
- Split signal induction into fast and slow components keeping unitary total charge (`IT_ratio_fast_slow`).
- Modified `run_list.txt` to increase the number of custom parameters.

### 2024-12-18
- Added noise amplitude tuning with expected `sigma_noise_fC` from white noise simulation.

### 2024-11-22
- Added above-threshold flag and signal length measurement on channels.

### 2024-10-14
- Implemented white noise, ion tail, and time information on clusters.

### 2024-08-30
- Implemented `Injection_External_Signal` in `Readout` to generate a square wave on the current histogram.

### 2021-04-27
- Implemented the resistive package scaffold (without analytical formula).

### 2021-04-19
- Implemented the µRWELL simulation without resistive layer.
