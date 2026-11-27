#include "main/Parsifal.h"
#include "TRandom.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <omp.h>
#include <sstream>

int nshot = 1;
static int g_override_nevents = -1;
static bool g_cli_set_resistive_model = false;

using namespace PARSIFAL2;

// Format a system_clock time_point as "YYYY-MM-DD HH:MM:SS" in local time
static string FormatDateTime(const chrono::system_clock::time_point& tp) {
  time_t tt = chrono::system_clock::to_time_t(tp);
  tm local_tm{};
  localtime_r(&tt, &local_tm);
  ostringstream oss;
  oss << put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

// Format a duration in seconds as "HH:MM:SS"
static string FormatHMS(double seconds) {
  long total_sec = static_cast<long>(seconds + 0.5);
  long hrs  = total_sec / 3600;
  long mins = (total_sec % 3600) / 60;
  long secs = total_sec % 60;
  ostringstream oss;
  oss << setfill('0') << setw(2) << hrs << ":"
      << setw(2) << mins << ":"
      << setw(2) << secs;
  return oss.str();
}

// Return human-readable resistive model name from code
static string GetResistiveModelName(int code) {
  switch (code) {
    case 0:  return "STD (T2K+shepherd, Gaussian Erf)";
    case 1:  return "Djunes (DRD1 2024-01-31)";
    case 2:  return "Morello (linear sigma, Gaussian Erf)";
    case 3:  return "Exponential / Laplace";
    case 4:  return "Lorentzian / Cauchy";
    case 5:  return "2D-PRF (1/r^3 dipole)";
    case 6:  return "Dixit Prompt+Delayed (Voigt)";
    case 7:  return "Two-Scale Dixit (Prompt + bi-RC)";
    default: return "Unknown (fallback STD)";
  }
}

// Return human-readable ASIC name from electronics code
static string GetElectronicsName(int code) {
  switch (code) {
    case 0:  return "APV";
    case 1:  return "TIGER";
    case 2:  return "TORA";
    default: return "Unknown";
  }
}

// Write human-readable simulation metadata, run configuration, and timing to data/<run>/info_sim_angle<angle>.txt
static void Write_InfoSim(const string& info_path,
                          const string var[25],
                          int angle_deg,
                          double angle_rad,
                          const TString& root_outfile,
                          const string& start_datetime,
                          const string& end_datetime,
                          double sim_run_sec,
                          double total_elapsed_sec) {
  ofstream info(info_path, ios::out | ios::trunc);
  if (!info.is_open()) {
    cerr << "ERROR: Could not open " << info_path << " for writing." << endl;
    return;
  }

  int n_events   = stoi(var[1]);
  int elec_code  = static_cast<int>(stod(var[22]));
  double ms_per_evt = (n_events > 0) ? (sim_run_sec * 1000.0 / n_events) : 0.0;
  auto on_off = [](bool active) { return active ? "ON" : "OFF"; };

  info << "================================================================================\n"
       << "PARSIFAL Simulation Info\n"
       << "================================================================================\n\n"
       << "[Execution Summary]\n"
       << "Run ID                     : " << var[0] << "\n"
       << "Detector Setup             : uRWELL (Planar 1D)\n"
       << "Incident Angle (XZ)        : " << angle_deg << " deg ("
       << fixed << setprecision(4) << angle_rad << " rad)\n"
       << "Number of Events           : " << n_events << "\n"
       << "Output ROOT File           : " << root_outfile << "\n"
       << "Start Date & Time          : " << start_datetime << "\n"
       << "End Date & Time            : " << end_datetime << "\n"
       << setprecision(3)
       << "Simulation Run Time        : " << sim_run_sec << " s ("
       << setprecision(1) << ms_per_evt << " ms/event)\n"
       << setprecision(3)
       << "Total Elapsed Time         : " << total_elapsed_sec << " s ("
       << FormatHMS(total_elapsed_sec) << ")\n"
       << "OpenMP Max Threads         : " << omp_get_max_threads() << "\n\n"

       << "[Detector & Physics Configuration]\n"
       << "Electronics (ASIC)         : " << GetElectronicsName(elec_code)
       << " (code " << elec_code << ")\n"
       << "Strip Pitch                : " << var[23] << " mm\n"
       << "Mean Detector Gain         : " << var[7] << "\n"
       << "Gain Tuning Factor         : " << var[2] << "\n"
       << "Spatial Diffusion Factor   : " << var[3] << "\n"
       << "Temporal Diffusion Factor  : " << var[4] << "\n"
       << "Ion Tail Length            : " << var[10] << " ns\n"
       << "Ion Tail Slow/Total Ratio  : " << var[11] << "\n\n"

       << "[Resistive Layer]\n"
       << "Resistive Model            : " << GetResistiveModelName(Resistive_Model)
       << " (code " << Resistive_Model
       << (g_cli_set_resistive_model ? ", CLI override)\n" : ", from run_list.txt)\n")
       << "Resistive RC (tau)         : " << var[5] << " ns\n"
       << "Resistive Sigma0           : " << var[6] << " mm\n\n"

       << "[Noise & Frequency Filter]\n"
       << "Noise                      : " << var[8] << " fC\n"
       << "General Threshold          : " << var[9] << " fC\n"
       << "FFT High-Pass Min Freq     : " << var[20] << " Hz\n"
       << "FFT Low-Pass Max Freq      : " << var[21] << " Hz\n\n"

       << "[ASIC Parameters]\n"
       << "APV Pedestal Std           : " << var[12] << "\n"
       << "APV Threshold Factor       : " << var[13] << "\n"
       << "TIGER E-Branch Threshold   : " << var[14] << " fC\n"
       << "TIGER T-Branch Threshold   : " << var[15] << " fC\n"
       << "TORA Rising Threshold      : " << var[16] << " fC\n"
       << "TORA Trailing Threshold    : " << var[17] << " fC\n"
       << "TORA Gain                  : " << var[18] << " mV/fC\n"
       << "TORA Shaping Time (tau)    : " << var[19] << " ns\n\n"

       << "[Runtime Simulation Flags (Common.h / CLI)]\n"
       << "Fast Simulation            : " << on_off(fast_simulation) << "\n"
       << "Magnetic Field (Bfield)    : " << on_off(Bfield) << "\n"
       << "Primary Ionization         : " << on_off(!NO_PrimaryIonization) << "\n"
       << "Secondary Ionization       : " << on_off(!NO_SecondaryIonization) << "\n"
       << "Detector Gain              : " << on_off(!NO_Gain) << "\n"
       << "Electron Drift             : " << on_off(!NO_Drift) << "\n"
       << "Resistive Dispersion       : " << on_off(!NO_Resistive) << "\n"
       << "Readout Electronics        : " << on_off(!NO_Readout) << "\n"
       << "Ion Tail                   : " << on_off(!NO_Ion_Tail) << "\n"
       << "Noise                      : " << on_off(!NO_Noise) << "\n"
       << "Saturation                 : " << on_off(!NO_Saturation) << "\n"
       << "FFT Filter                 : " << on_off(!NO_FFT) << "\n"
       << "Capacitive Induction       : " << on_off(!NO_Capacitive) << "\n"
       << "uTPC Correction            : " << on_off(!NO_uTPC_Correction) << "\n"
       << "TORA Shaper Order          : " << TORA_shaper_order << "\n"
       << "Resistive Model            : " << GetResistiveModelName(Resistive_Model) << " (code " << Resistive_Model << ")\n"
       << "Plot Hits Above Thr        : " << on_off(Print_Plot_Above_Thr) << "\n"
       << "Seed                       : " << SEED << "\n"
       << "Time Window (n_ns)         : " << n_ns << " ns (+ " << n_ns_buffer << " ns buffer)\n"
       << "================================================================================\n";

  info.close();
  cout << "Simulation info written: " << info_path << endl;
}

void Run_i(string folder_i, int angle_i){
  // Seed global ROOT RNG so TF1::GetRandom() is reproducible when SEED > 0
  if (gRandom) gRandom->SetSeed(SEED);

  const auto wall_start   = chrono::system_clock::now();
  const auto steady_start = chrono::steady_clock::now();
  const string start_datetime = FormatDateTime(wall_start);

  double aangle = angle_i*TMath::DegToRad();
  ifstream infile;
  infile.open("run_list.txt", ios::in);
  const int var_size = 25;
  string var[var_size];
  // 0  -> runID
  // 1  -> nEvent
  // 2  -> Gain Factor
  // 3  -> Space Diffusion Factor
  // 4  -> Time  Diffusion Factor
  // 5  -> Resistive RC [ns]
  // 6  -> Resistive Sigma0 [mm]
  // 7  -> Mean Detector Gain
  // 8  -> Noise [fC]
  // 9  -> Threshold [fC]
  // 10 -> Ion Tail signal length [ns]
  // 11 -> Ratio signal Slow(ion) over Fast(electron)+Slow(ion)
  // 12 -> APV ped
  // 13 -> APV threshold user factor
  // 14 -> TIGER v_thr E [fC]
  // 15 -> TIGER v_thr T [fC]
  // 16 -> TORA v_thr rise [fC]
  // 17 -> TORA v_thr trail [fC]
  // 18 -> TORA gain [mV/fC]
  // 19 -> TORA peak_time (tau) [ns]
  // 20 -> FFT min freq. cut [Hz]
  // 21 -> FFT max freq. cut [Hz]
  // 22 -> Electronics (0=APV, 1=TIGER, 2=TORA)
  // 23 -> Strip Pitch [mm]
  // 24 -> Resistive Model (0=STD, 1=Djunes, 2=Morello, 3=Exp, 4=Lorentz, 5=2D-PRF, 6=Dixit Voigt, 7=Two-Scale Dixit)
  var[0]="?";
  string line;
  while(getline(infile, line)){
    istringstream iss(line);
    if(!(iss >> var[0] >> var[1] >> var[2] >> var[3] >> var[4] >> var[5] >> var[6] >> var[7]
             >> var[8] >> var[9] >> var[10] >> var[11] >> var[12] >> var[13] >> var[14] >> var[15]
             >> var[16] >> var[17] >> var[18] >> var[19] >> var[20] >> var[21] >> var[22] >> var[23])){
      var[0]="?";
      continue;
    }
    // Backward-compatible optional 25th column (Res_Model); defaults to 0 (STD) if omitted
    if(!(iss >> var[24])) var[24] = "0";
    if(var[0]==folder_i) {
      nshot = stoi(var[1]);
      break;
    }
    else{
      var[0]="?";
    }
  }
  
  string name_ang = Form("run_angle%i.root",(int)angle_i);
  if(var[0]=="?") {
    cout<<"ERROR: Check the file run_list.txt"<<endl;
    return;
  }
  if(!g_cli_set_resistive_model) {
    Resistive_Model = stoi(var[24]);
  }
  if(g_override_nevents > 0) {
    var[1] = to_string(g_override_nevents);
    nshot = g_override_nevents;
  }
  //if(folder_i=="1" || folder_i=="2"){
  if(false){
    /*
    cout<<"SIMULATE TRIPLE-GEM"<<endl;
    PARSIFAL *tripleGEM = new PARSIFAL();
    tripleGEM->Set_Particle_AngleXZ(aangle);
    tripleGEM->Set_OutfileName(folder_i,name_ang);
    tripleGEM->Set_nShots(nshot);
    tripleGEM->Initialization_tripleGEM();
    tripleGEM->Set_GainFactor(stod(var[2]));
    tripleGEM->Set_SpaceDiffusionFactor(stod(var[3]));
    tripleGEM->Set_TimeDiffusionFactor(stod(var[4]));
    tripleGEM->Run();
    tripleGEM->Terminate();
    delete tripleGEM;
    */
  }
  else{
    cout<<"SIMULATER µRWELL"<<endl;
    PARSIFAL *rwell = new PARSIFAL();
    // Pre init
    rwell->Set_Particle_AngleXZ(aangle);
    rwell->Set_OutfileName(folder_i,name_ang);
    rwell->Set_nShots(stoi(var[1])); 
    rwell->Set_Electronics(stod(var[22]));
    rwell->Set_Strip_Pitch(stod(var[23]));
    // Init
    rwell->Initialization_rwell();
    // Post Init
    rwell->Set_GainFactor(stod(var[2]));
    rwell->Set_SpaceDiffusionFactor(stod(var[3]));
    rwell->Set_TimeDiffusionFactor(stod(var[4]));
    rwell->Set_TauResist(stod(var[5]));
    rwell->Set_Sigma0Resist(stod(var[6]));
    rwell->Set_MeanDetectorGain(stod(var[7]));
    rwell->Set_TORA_tau(stod(var[19]));
    rwell->Set_Noise_fC(stod(var[8]));
    rwell->Set_Threshold_fC(stod(var[9]));
    rwell->Set_IonTailLength(stod(var[10]));
    rwell->Set_FastSlowRatio(stod(var[11]));
    rwell->Set_APV_ped(stod(var[12]));
    rwell->Set_APV_thr(stod(var[13]));
    rwell->Set_TIGER_thr_E(stod(var[14]));
    rwell->Set_TIGER_thr_T(stod(var[15]));
    rwell->Set_TORA_thr_rise(stod(var[16]));
    rwell->Set_TORA_thr_trail(stod(var[17]));
    rwell->Set_gain_TORA(stod(var[18]));
    rwell->Set_FFT_fmin(stod(var[20]));
    rwell->Set_FFT_fmax(stod(var[21]));
    rwell->Update_param_TORA();
    // Run
    const auto run_start = chrono::steady_clock::now();
    rwell->Run();
    const auto run_end = chrono::steady_clock::now();
    TString root_outfile = rwell->GetOutfileName();
    rwell->Terminate(); 
    delete rwell;

    const auto wall_end   = chrono::system_clock::now();
    const auto steady_end = chrono::steady_clock::now();
    const double sim_run_sec = chrono::duration<double>(run_end - run_start).count();
    const double total_elapsed_sec = chrono::duration<double>(steady_end - steady_start).count();

    const char* data_dir_env = getenv("PARSIFAL_data");
    string base_dir = data_dir_env ? string(data_dir_env) : string("data");
    string info_path = base_dir + "/" + folder_i + "/info_sim_angle" + to_string(angle_i) + ".txt";
    Write_InfoSim(info_path, var, angle_i, aangle, root_outfile,
                  start_datetime, FormatDateTime(wall_end),
                  sim_run_sec, total_elapsed_sec);
  }
  return;
}

static void PrintUsage(const char* prog) {
  cerr << "Usage: " << prog << " <runID> [angle_deg] [options]\n"
       << "Optional runtime overrides:\n"
       << "  --seed <U>          Random seed (0 = random, >0 = deterministic)\n"
       << "  --nevents <N>       Override number of events from run_list.txt\n"
       << "  --bfield <0|1>      Enable/disable magnetic field\n"
       << "  --no-noise <0|1>    Disable/enable noise\n"
       << "  --no-sat <0|1>      Disable/enable saturation\n"
       << "  --no-fft <0|1>      Disable/enable FFT filter\n"
       << "  --plot-hits <0|1>   Save PDF/PNG waveforms for hits above threshold\n"
       << "  --tora-shaper <1|3>      TORA shaper order (1 or 3)\n"
       << "  --resistive-model <0..7> Override Res_Model from run_list.txt (0=STD, 1=Djunes, 2=Morello, 3=Exp, 4=Lorentz, 5=2D-PRF, 6=Dixit Voigt, 7=Two-Scale Dixit)\n";
}

int main(int argc, char* argv[]){
  if(argc<2) {
    PrintUsage(argv[0]);
    return 1;
  }
  string folder = argv[1];
  if(argc<3){
    const int n_angle = 7;
    int angles[n_angle]={0,5,7,10,15,20,30};
    for(int iangle=0;iangle<n_angle;iangle++){
      gSystem->Exec(Form("echo RUN %s angle: %i",folder.c_str(),angles[iangle])); 
      gSystem->Exec(Form("./bin/Simulate %s %i &",folder.c_str(),angles[iangle]));
    }
  }
  else {
    int angle = atoi(argv[2]);
    // Parse optional runtime CLI flags
    for(int i = 3; i < argc; i++){
      string arg = argv[i];
      auto need_val = [&](const string& flag) {
        if(i + 1 >= argc){
          cerr << "ERROR: missing value for " << flag << endl;
          PrintUsage(argv[0]);
          return false;
        }
        return true;
      };
      if(arg == "--seed")        { if(!need_val(arg)) return 1; SEED = static_cast<UInt_t>(stoul(argv[++i])); }
      else if(arg == "--nevents"){ if(!need_val(arg)) return 1; g_override_nevents = stoi(argv[++i]); }
      else if(arg == "--bfield") { if(!need_val(arg)) return 1; Bfield = (stoi(argv[++i]) != 0); }
      else if(arg == "--no-noise"){ if(!need_val(arg)) return 1; NO_Noise = (stoi(argv[++i]) != 0); }
      else if(arg == "--no-sat") { if(!need_val(arg)) return 1; NO_Saturation = (stoi(argv[++i]) != 0); }
      else if(arg == "--no-fft") { if(!need_val(arg)) return 1; NO_FFT = (stoi(argv[++i]) != 0); }
      else if(arg == "--plot-hits"){ if(!need_val(arg)) return 1; Print_Plot_Above_Thr = (stoi(argv[++i]) != 0); }
      else if(arg == "--tora-shaper"){ if(!need_val(arg)) return 1; TORA_shaper_order = stoi(argv[++i]); }
      else if(arg == "--resistive-model"){ if(!need_val(arg)) return 1; Resistive_Model = stoi(argv[++i]); g_cli_set_resistive_model = true; }
      else if(arg == "-h" || arg == "--help"){ PrintUsage(argv[0]); return 0; }
      else {
        cerr << "ERROR: unknown option '" << arg << "'" << endl;
        PrintUsage(argv[0]);
        return 1;
      }
    }
    Run_i(folder,angle);
  }
  return 0;
};
