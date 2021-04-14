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
using namespace std;
namespace PARSIFAL2{
  class Signal{
  public:
    Signal(int setup, int electronics, bool MagField, Geometry *geo);
    ~Signal();
    //Fast
    vector<ElectronicChannel*> Generate_Signal (vector<Primary*>);
    //Slow
    vector<Secondary*>         Gain            (vector<Secondary*> secondary) {return gain->Gain  (secondary);};
    vector<Secondary*>         Drift           (vector<Secondary*> secondary) {return drift->Drift(secondary);}; 
    vector<ElectronicChannel*> Read            (vector<Secondary*> secondary) {return anode->Read (secondary);};
    
    //Tuning
    void Set_GainTuningFactor (double io){ gain->Set_TuningFactor(io);};
    void Set_SpaceTuningFactor(double io){drift->Set_SpaceTuningFactor(io);};
    void Set_TimeTuningFactor (double io){drift->Set_TimeTuningFactor(io);};
  private:
    ElectronDrift *drift;
    DetectorGain *gain;
    Readout *anode;
  };//end class
}//end namespace
#endif
