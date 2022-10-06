/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

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
  const bool NO_PrimaryIonization = false;
  const bool NO_SecondaryIonization = false;
  const bool NO_Gain = false;
  const bool NO_Drift = false;
  const bool NO_Resistive = false;
  const bool NO_Readout = false;
  const bool NO_Noise = false;
  const bool NO_Capacitive= true;
  const bool NO_uTPC_Correction = true;
  const bool NO_Hit = false;
  const bool NO_Cluster = false;
  const bool fast_simulation = true;
  const bool Bfield = false;

  //General
  const double SEED = 0;
  //Readout - Induction
  const double tau_APV = 50;
  const double jitter_APV = 0;
  const double timebin_APV = 27;
  const double timestep_APV = 25;
  #define n_ns 675
  const double noise_APV = 15; //ADC
  const double thr_APV   = 1.67; //fC
  const double fC_to_ADC = 30;
  const double ele_to_fC = 1.6e-4;
  const double ele_to_ADC = ele_to_fC * fC_to_ADC; 
  //Readout - Anode
  const int    Xview  = 1;
  const int    Yview  = 2;
  //Reconstruction - µTPC
  const int    dead_strip = 0;
  const int    TPC_min_hit = 2;
  const double extra_gap_range = 0.2;
  //Plot
  const bool plot_APV_histos = false;
}
#endif
