#ifndef Common_h
#define Common_h
#include "Common/PrintNTuple.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std; 
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{

  // Runtime-configurable simulation switches (can be overridden via CLI flags in main_sim.C)
  inline bool   Bfield                  = false;
  inline bool   NO_PrimaryIonization    = false;
  inline bool   NO_SecondaryIonization  = false;
  inline bool   NO_Gain                 = false;
  inline bool   NO_Drift                = false;
  inline bool   NO_Resistive            = false;
  inline bool   NO_Readout              = false;
  inline bool   NO_Saturation           = false;
  inline bool   NO_Noise                = false;
  inline bool   NO_FFT                  = false;
  inline bool   NO_Capacitive           = true;
  inline bool   NO_Hit                  = false;
  inline bool   NO_Cluster              = false;
  inline bool   NO_uTPC_Correction      = true;
  inline bool   NO_Ion_Tail             = false;
  inline int    TORA_shaper_order       = 1;     // 1 = 1st-order shaper, 3 = 3rd-order shaper
  inline int    Resistive_Model         = 0;     // 0=STD, 1=Djunes, 2=Morello, 3=Exp, 4=Lorentz, 5=2D-PRF, 6=Dixit Voigt, 7=Two-Scale Dixit
  inline bool   Print_Plot_Above_Thr    = false; // Save waveform PDF/PNG for hits above threshold
  // Special signals
  inline bool   Single_Electron         = false;
  inline bool   Fix_Ionization          = false;
  const  int    FIX_IONIZATION          = 100;
  inline bool   Square_Wave             = false;
  const  float  SW_Lenght               = 100;
  const  float  SW_Amplitude            = 0.1;
  // Gain
  inline bool   Fix_Gain                = false;
  const  int    FIX_GAIN                = 1230;
  // General
  inline UInt_t SEED                    = 0;     // 0 = random seed; >0 = deterministic reproducible run
  inline bool   Write_Output_File       = true;
  inline bool   fast_simulation         = true;
  //Readout
  #define n_ns 1500//1000//675
  #define n_ns_buffer 500
  //const double sigma_noise_fC = 2.0; //fC
  //Readout - Induction APV
  const double tau_APV = 50;
  const double jitter_APV = 0;
  const double timebin_APV = 27;
  const double timestep_APV = 25;
  //const double noise_APV = 0.05; //amplitude -> removed
  //const double noise_APV_fC = 0.83; //fC -> dovrebbe essere definita con una funzione : noise_APV_fC = noise_APV * 16.47 (misurata con toyMC)
  //const double thr_APV_fC   =  0.1;//3;  //fC -> DOVE VIENE USATA? DOVREBBE ESSERE MESSA NELLA RECO -> ora è usata solo nel plot Print_Time
  //const double mmdaq_thr_factor = 0.8; 
  const double fC_to_ADC = 30;
  const double saturation_APV = 1970;
  const double sigma_saturation_APV = 71;
  const double e_noise_amplitude_APV = 0.5; //fC            // NB. value to be verified
  //const double ele_to_ADC = ele_to_fC * fC_to_ADC;
  //Readout - Induction TIGER
  const double jitter_TIGER = 2;
  const double timestep_TIGER = 6.25; //ns
  //const double noise_TIGER = 0.5; //fC
  //const double thrT_TIGER = 5;//30;   //mV
  //const double thrE_TIGER = 5;//30; //mV
  const double gain_TIGER = 12.7; //fC/mV
  const int    integration_time_TIGER = 6;
  const double saturation_TIGER = 612;
  const double sigma_saturation_TIGER = 15;
  const bool   enable_tfine_tiger = true;
  const double e_noise_amplitude_TIGER = 0.3; //fC          // NB. value to be verified

  //TORA
  const double timestep_TORA = 5; //ns
  const double saturation_TORA = 600; //mV according to Maxim
  const double sigma_saturation_TORA = 15;
  const double e_noise_amplitude_TORA = 0.65; //fC

  // const double saturation_TORA = -90 * gain_TORA + 1180; //mV (assuming linear relation betwenn gain and max input charge)


  //Readout
  const int    Xview  = 1;
  const int    Yview  = 2;
  const int    Phiview  = 3;
  const double ele_to_fC = 1.6e-4;
  //Reconstruction
  const int    dead_strip = 2;
  const int    TPC_min_hit = 2;
  const double extra_gap_range = 0.2;
  //Plot
  //const bool plot_APV_histos = false;
}
#endif
