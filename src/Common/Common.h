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

  //Simulation
  const bool Bfield                  = false;
  const bool NO_PrimaryIonization    = false;
  const bool NO_SecondaryIonization  = false;
  const bool NO_Gain                 = false;
  const bool NO_Drift                = false;
  const bool NO_Resistive            = false; 
  const bool NO_Readout              = false;
  const bool NO_Saturation           = false;
  const bool NO_Noise                = false;
  const bool NO_Capacitive           = true;
  const bool NO_Hit                  = false;
  const bool NO_Cluster              = false;
  const bool NO_uTPC_Correction      = true;
  const bool NO_Ion_Tail             = false;
  //Ion tail
  //const float IT_Lenght              = 150;    //100;  //50;  //250;  //170;
  //const	float IT_amplitude           = 0.00667;//0.01; //0.02;//0.004;//0.006;// = 1/IT_Lenght
  //const float IT_ratio_fast_slow     = 0.85; // 15% of the signal is electron and 85% is ions
  //Special signals
  const bool  Single_Electron        = false;
  const bool  Fix_Ionization         = false;
  const int   FIX_IONIZATION         = 100;
  const bool  Square_Wave            = false;
  const float SW_Lenght              =  100;
  const float SW_Amplitude           =  0.1;
  //Gain
  const bool Fix_Gain                = false;
  const int  FIX_GAIN                = 1230; //0;//6250;
  //General
  const double SEED = 0;
  const bool   Write_Output_File = true; // anche se falso, ci sono cmq problemi di memoria
  const bool   fast_simulation = true; // se falso va inserito limite sul numero di eventi o sull'utilizzo della memoria
  //Readout
  #define n_ns 1000//1000//675
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
