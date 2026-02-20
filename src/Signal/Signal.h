#ifndef Signal_h
#define Signal_h
#include "Common/Common.h"
#include "Geometry/Geometry.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
#include "Common/ElectronicChannel.h"
#include "ElectronDrift/ElectronDrift.h"
#include "DetectorGain/DetectorGain.h"
#include "Readout/Readout.h"
#include "Resistive/Resistive.h"
using namespace std;
namespace PARSIFAL2{
  class Signal{
  public:
    Signal(int setup, int electronics, bool MagField, Geometry *geo);
    ~Signal();
    //Fast
    vector<ElectronicChannel*> Generate_Signal (vector<Primary*>);
    vector<ElectronicChannel*> Get_Channels    ();
    //Slow
    vector<Secondary*>         Gain            (vector<Secondary*> secondary) {return gain->Gain        (secondary);};
    vector<Secondary*>         Drift           (vector<Secondary*> secondary) {return drift->Drift      (secondary);}; 
    vector<Secondary*>         Resist          (vector<Secondary*> secondary) {return resisto->Diffusion(secondary);};
    vector<ElectronicChannel*> Read            (vector<Secondary*> secondary) {return reado->Read       (secondary);};
    void                       Integrate_Charge()                             {return reado->Integrate_Charge();};
    
    //Tuning
    void Set_GainTuningFactor (double io){   gain->Set_TuningFactor     (io);};
    void Set_SpaceTuningFactor(double io){  drift->Set_SpaceTuningFactor(io);};
    void Set_TimeTuningFactor (double io){  drift->Set_TimeTuningFactor (io);};
    void Set_TauResist        (double io){resisto->Set_TauResist        (io);};
    void Set_Sigma0Resist     (double io){resisto->Set_Sigma0Resist     (io);};
    void Set_MeanDetectorGain (double io){   gain->Set_MeanDetectorGain (io);};
    void Set_Noise_fC         (double io){  reado->Set_Noise_fC         (io);};
    void Set_Threshold_fC     (double io){  reado->Set_Threshold_fC     (io);};
    void Set_IonTailLength    (double io){  reado->Set_IonTailLength    (io);};
    void Set_FastSlowRatio    (double io){  reado->Set_FastSlowRatio  	(io);};
    void Set_APV_ped          (double io){  reado->Set_APV_ped          (io);};
    void Set_APV_thr          (double io){  reado->Set_APV_thr          (io);};
    void Set_TIGER_thr_E      (double io){  reado->Set_TIGER_thr_E  	  (io);};
    void Set_TIGER_thr_T      (double io){  reado->Set_TIGER_thr_T      (io);};
    void Set_TORA_thr_T       (double io){  reado->Set_TORA_thr_T       (io);};
    void Set_TORA_gain        (double io){  reado->Set_TORA_gain        (io);};
    void Set_TORA_tau         (double io){  reado->Set_TORA_tau        (io);};
    void Set_APV_Plot_Hit     (vector<TH1F*> io){reado->Set_APV_Plot_Hit(io);};

    
    //General
    int  Get_Total_Electron_Gained()         {if(!fast_simulation) return gain->Get_Total_Electron_Gained(); else return tot_ele_gained;};
    void Set_Total_Electron_Gained(double io){tot_ele_gained=io;};
    void Clean                    ()         {reado->Clean();};
  private:
    //Variable
    ElectronDrift *drift;
    DetectorGain *gain;
    Readout *reado;
    Resistive *resisto;
    int tot_ele_gained;
    int setup;
    //Function

  };//end class
}//end namespace
#endif
