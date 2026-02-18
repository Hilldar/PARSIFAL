#ifndef Parsifal_h
#define Parsifal_h
#include "TSystem.h"
#include <fstream>
#include <iostream>
#include "Common/Common.h"
#include "Common/Position.h"
#include "Common/Primary.h"
#include "Common/Secondary.h"
#include "Common/ElectronicChannel.h"
#include "Common/Hit.h"
#include "Common/Cluster.h"
#include "Geometry/Geometry.h"
#include "Particle/Particle.h"
#include "Ionization/Ionization.h"
#include "ElectronDrift/ElectronDrift.h"
#include "DetectorGain/DetectorGain.h"
#include "Readout/Readout.h"
#include "Reconstruction/Reconstruction.h"
#include "Resistive/Resistive.h"
#include "Signal/Signal.h"
#include "TROOT.h"
#include "TRint.h"
namespace PARSIFAL2{
  class PARSIFAL{
  public:
    //Construction
    PARSIFAL();
    //Destruction
    ~PARSIFAL();

    //Main Function 
    void Initialize_oFile();
    void Write_oFile();
    void Clean();
    void FreeMemory();
    void Initialization_urtube();
    void Initialization_tripleGEM();
    void Initialization_rwell();
    void Run();
    void Terminate();
    void Plot_PDF_Channels();
    //Function support
    int     Get_RunNumber     ()           {return run_number;};
    TString GetOutfileName    ()           {return name_outfile;};
    void Set_RunNumber        (int io)     {run_number = io;};
    void Set_OutfileName      (TString io) {
      name_outfile=(TString)(getenv("PARSIFAL_data"))+=io;
    };
    void Set_OutfileName      (TString io_dir, TString io_file) {
      Set_RunNumber(io_dir.Atoi());
      name_outfile=(TString)(getenv("PARSIFAL_data"))+"/"+io_dir;
      gSystem->Exec("mkdir -p "+name_outfile);
      name_outfile+="/"+io_file;
    };
    void Set_Particle_AngleXZ (double io)  {party_angle_xz[0]=io;};
    void Set_Particle_DOCA    (double io)  {party_DOCA=io;};
    void Set_nShots           (int io)     {nShots = io;};
    void Set_GainFactor       (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Detector gain tuning factor set to      : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_GainTuningFactor(io);
    };
    void Set_SpaceDiffusionFactor (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Spatial diffusion tuning factor set to  : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_SpaceTuningFactor(io);
    };
    void Set_TimeDiffusionFactor (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Temporal diffusion tuning factor set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_TimeTuningFactor(io);
    };
    void Set_TauResist (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Tau Resist value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_TauResist(io);
    };
    void Set_Sigma0Resist (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Sigma0 Resist value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_Sigma0Resist(io);
    };
    void Set_MeanDetectorGain (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Mean Detector Gain value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_MeanDetectorGain(io);
    };
    void Set_Noise_fC (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Noise value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_Noise_fC(io);
    };
    void Set_Threshold_fC (double io)  {
      cout<<"************************************************"<<endl;
      cout<<"Threshold value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_Threshold_fC(io);
    };
    void Set_IonTailLength (double io){
      cout<<"************************************************"<<endl;
      cout<<"Ion Tail Length value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_IonTailLength(io);
    }
    void Set_FastSlowRatio (double io){
      cout<<"************************************************"<<endl;
      cout<<"Ratio signal Ion/(Ion+ele) value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_FastSlowRatio(io);
    }
    void Set_APV_ped(double io){
      cout<<"************************************************"<<endl;
      cout<<"APV std pedestal value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_APV_ped(io);
    }
    void Set_APV_thr(double io){
      cout<<"************************************************"<<endl;
      cout<<"APV user factor threshold value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_APV_thr(io);
    }
    void Set_TIGER_thr_E(double io){
      cout<<"************************************************"<<endl;
      cout<<"TIGER threshold E branch value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_TIGER_thr_E(io);
    }
    void Set_TIGER_thr_T(double io){
      cout<<"************************************************"<<endl;
      cout<<"TIGER threshold T branch value set to : "<<io<<endl;
      cout<<"************************************************"<<endl;
      signal->Set_TIGER_thr_T(io);
    }
    
  private:
    //Input variables
    int nShots = 10;
    double party_angle_xz[1] = {0};
    //Variables
    Geometry *geo;
    Position *D4;
    Particle *party;
    Ionization *ionio;
    Reconstruction *recon;
    Signal *signal;
    vector<Primary*> primi;
    vector<Secondary*> secondary;
    vector<Secondary*> electrons;
    vector<Secondary*> electrons_resistive;
    vector<ElectronicChannel*> channels;
    vector<Hit*> hit;
    vector<Cluster1D*> cluster1d;
    TFile *file;
    TTree *tree;	
    TRint *gRoot;
    int   run_number;
    
    //Output variables
    TString name_outfile="data/ciao.root";
    int event;
    //Geometry
    int    geo_numview;
    double geo_pitch1;
    double geo_pitch2;
    double geo_driftgap;
    //Particle
    int    party_partID;
    double party_angleXZ;
    double party_angleYZ;
    double party_DOCA;
    double party_posI[4];
    double party_primary;
    double party_prob[100];
    //Ionization
    double ionio_positionI[4];
    double ionio_positionF[4];
    int    ionio_primary;
    int    ionio_secondary; 
    // Primary
    vector<double>      primary_positionX;
    vector<double>      primary_positionY;
    vector<double>      primary_positionZ;
    vector<double>      primary_positionT;
    vector<int>         primary_secondary;
    vector<int>         primary_secondary_gain;
    vector<int>         primary_ID;
    //Secondary
    vector<int>         secondary_gain;
    //Electrons
    vector<double>      electron_positionX_initial;
    vector<double>      electron_positionY_initial;
    vector<double>      electron_positionZ_initial;
    vector<double>      electron_positionPhi_initial;
    vector<double>      electron_positionT_initial;
    vector<double>      electron_positionX_final;
    vector<double>      electron_positionY_final;
    vector<double>      electron_positionZ_final;
    vector<double>      electron_positionPhi_final;
    vector<double>      electron_positionT_final;
    vector<int>         electron_primary_ID;
    vector<int>         electron_electronic_channel;
    //Electronic Channel
    vector<double>      channel_ID;
    vector<int>         channel_type;
    vector<double>      channel_charge;
    vector<double>      channel_time;
    vector<double>      channel_dtime;
    vector<bool>        channel_above_thr;
    vector<bool>        channel_above_thr_E;
    vector<bool>        channel_above_thr_T;
    vector<double>      channel_t_rising_raw;
    vector<double>      channel_t_rising_E;
    vector<double>	channel_t_rising_T;
    vector<double>      channel_t_falling_raw;
    vector<double>      channel_t_falling_E;
    vector<double>      channel_t_falling_T;
    vector<double>      channel_positionX;
    vector<double>      channel_positionY;
    vector<double>      channel_positionZ;
    vector<double>      channel_positionPhi;
    vector<int>         channel_n_electrons;
    //Hits
    int                 nhit;
    vector<int>         hit_ID;
    vector<int>         hit_channelID;
    vector<int>         hit_clusterID;
    vector<int>         hit_type;
    vector<double>      hit_charge;
    vector<double>      hit_time;
    vector<double>      hit_dtime;
    vector<double>	hit_t_rising_E;
    vector<double>      hit_t_rising_T;
    vector<double>      hit_t_falling_E;
    vector<double>      hit_t_falling_T;
    vector<double>      hit_positionX;
    vector<double>      hit_positionY;
    //Cluster1D
    int                 ncluster1d;
    vector<int>         cluster1d_ID;
    //vector<vector<int>> *cluster1d_hitID=0;
    vector<int>         cluster1d_type;
    vector<int>         cluster1d_size;
    vector<double>      cluster1d_charge;
    vector<double>      cluster1d_time0;
    vector<double>      cluster1d_timeF; 
    vector<double>      cluster1d_positionCC_X;
    vector<double>      cluster1d_positionCC_Y;
    vector<double>      cluster1d_positionTPC_X;
    vector<double>      cluster1d_positionTPC_Y;
    vector<bool>        cluster1d_high_Q;
    vector<bool>        cluster1d_faster;
    //General
    int                 tot_ele_gained;
    //Plot hit
    vector<TH1F*>        histo_hit_Qt_APV;
  };//class
};//namespace
#endif

