#include "main/Parsifal.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <omp.h>
#include <sstream>

int nshot = 1;

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
                          const string var[24],
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

       << "[Compile-Time Flags (Common.h)]\n"
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
       << "Seed                       : " << setprecision(0) << SEED << "\n"
       << "Time Window (n_ns)         : " << n_ns << " ns (+ " << n_ns_buffer << " ns buffer)\n"
       << "================================================================================\n";

  info.close();
  cout << "Simulation info written: " << info_path << endl;
}

void Run_i(string folder_i, int angle_i){
  const auto wall_start   = chrono::system_clock::now();
  const auto steady_start = chrono::steady_clock::now();
  const string start_datetime = FormatDateTime(wall_start);

  double aangle = angle_i*TMath::DegToRad();
  ifstream infile;
  infile.open("run_list.txt", ios::in);
  const int var_size = 24;
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
  var[0]="?";
  while(infile>>var[0]>>var[1]>>var[2]>>var[3]>>var[4]>>var[5]>>var[6]>>var[7]>>var[8]>>var[9]>>var[10]>>var[11]>>var[12]>>var[13]>>var[14]>>var[15]>>var[16]>>var[17]>>var[18]>>var[19]>>var[20]>>var[21]>>var[22]>>var[23]){
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

int main(int argc, char* argv[]){
  if(argc<2) {
    cerr<<"RUN the analysis with the configuration ID"<<endl;
    return 1;
  }
  if(argc<3){
    string folder = argv[1];
    const int n_angle = 7;
    int angles[n_angle]={0,5,7,10,15,20,30};
    for(int iangle=0;iangle<n_angle;iangle++){
      gSystem->Exec(Form("echo RUN %s angle: %i",folder.c_str(),angles[iangle])); 
      gSystem->Exec(Form("./bin/Simulate %s %i &",folder.c_str(),angles[iangle]));
    }
  }
  else {
    string folder = argv[1];
    int angle =atoi(argv[2]);
    Run_i(folder,angle);
  }
  return 0;
};
