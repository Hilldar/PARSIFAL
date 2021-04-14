#include "ElectronDrift/ElectronDrift.h"
using namespace std;
namespace PARSIFAL2{
  ElectronDrift::ElectronDrift(int setup, bool MagField, Geometry* geo):
    Setup(setup),
    MagneticField(MagField),
    geometry(geo),
    PrintInfo(true),
    PrintNTuple(true)
  {
    tuning_factor_diff = 1.5;
    tuning_factor_time = 1.5;
    r = new TRandom3();

    r->SetSeed(SEED);
    if(Get_Setup()==0){
      // no mag field -----------
      // >>> spatial shift
      // shift drift
      SpatialShiftDrift[0] = 0.; // OK
      SpatialShiftDrift[1] = 0.; // OK
      // shift transfer
      SpatialShiftTrans = 0.; // OK
      // shift induction
      SpatialShiftInduc = 0.; // OK
      //
      // >>> spatial sigma
      // sigma drift
      SpatialDiffusionDrift[0] = 0.0275728;    // OK
      SpatialDiffusionDrift[1] = -0.0460094;   // OK
      SpatialDiffusionDrift[2] = 0.102019;     // OK
      SpatialDiffusionDrift[3] = -0.202226;    // OK
      // sigma transfer
      SpatialDiffusionTrans = 0.0167369;  // OK
      // sigma induction
      SpatialDiffusionInduc = 0.0158942;  // OK
      // 
      // >>> temporal shift
      // shift drift
      TemporalShiftDrift[0] = 136.981;  // OK
      TemporalShiftDrift[1] = -271.213; // OK
      // shift transfer
      TemporalShiftTrans = 56.6527; // OK
      // sigma induction
      TemporalShiftInduc = 56.5305; // OK
      //
      // >>> temporal sigma
      // sigma drift VANNO SOMMATI IN QUADRATURA
      TemporalDiffusionDrift[0] = 3.52458;  // OK
      TemporalDiffusionDrift[1] = -2.01135; // OK
      TemporalDiffusionDrift[2] = -4.15946; // OK
      // sigma transfer
      TemporalDiffusionTrans = 2.12464; // OK
      // sigma induction
      TemporalDiffusionInduc = 1.93061; // OK
      
      // mag field --------------
      // >>> spatial shift
      // shift drift
      SpatialShiftDriftMagField[0] = 0.241024; // OK
      SpatialShiftDriftMagField[1] = -0.47836; // OK
      // shift transfer
      SpatialShiftTransMagField = 0.03891; // OK
      // shift induction
      SpatialShiftInducMagField = 0.02285; // OK
      //
      // >>> spatial sigma
      // sigma drift
      SpatialDiffusionDriftMagField[0] = 0.0334884;    // OK
      SpatialDiffusionDriftMagField[1] = -0.0661326;   // OK
      SpatialDiffusionDriftMagField[2] = 0.17014;      // OK
      SpatialDiffusionDriftMagField[3] = -0.302228;    // OK
      // sigma transfer
      SpatialDiffusionTransMagField = 0.0174;          // OK
      // sigma induction
      SpatialDiffusionInducMagField = 0.01611;         // OK
      //
      // >>> temporal shift
      // shift drift
      TemporalShiftDriftMagField[0] = 147.213;  // OK
      TemporalShiftDriftMagField[1] = -291.508; // OK
      // shift transfer
      TemporalShiftTransMagField = 57.6; // OK
      // shift induction
      TemporalShiftInducMagField  = 56.78; // OK
      // >>> temporal sigma 
      // sigma drift VANNO SOMMATI IN QUADRATURA
      TemporalDiffusionDriftMagField[0] = 3.94043;  // OK
      TemporalDiffusionDriftMagField[1] = -2.58493; // OK
      TemporalDiffusionDriftMagField[2] = -4.25339; // OK
      // sigma transfer
      TemporalDiffusionTransMagField = 2.155;       // OK
      // sigma induction
      TemporalDiffusionInducMagField = 1.933;       // OK


    }
    if(Get_Setup()!=0){
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<>      ElectronDrift    <>-<>"<<endl;
      cout<<"<>-<>  Check the setup ID   <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
    }
    if(Get_PrintInfo()){
      cout<<"---- New ElectronDrift ----"<<endl;
      cout<<"Setup configuration              : "<<Get_Setup()<<endl;
      cout<<"Magnetic field                   : "<<Get_MagneticField()<<endl;
      cout<<"Spatial diffusion tuning factor  : "<<Get_SpaceTuningFactor()<<endl;
      cout<<"Temporal diffusion tuning factor : "<<Get_TimeTuningFactor()<<endl;
      cout<<"----------------------"<<endl;
    }
  };

  vector<Secondary*> ElectronDrift::Drift(vector<Secondary*> pre_drift){
    vector<Secondary*> post_drift;
    if(pre_drift.empty()) return post_drift;
    for(int isec=0;isec<pre_drift.size();isec++){
      for(int iele=0;iele<pre_drift.at(isec)->Get_GainedElectrons();iele++){
	int ID = pre_drift.at(isec)->Get_PrimaryID();
	Position pos_initi = pre_drift.at(isec)->Get_PositionInitial();
	Position pos_final = Get_FinalPosition(&pos_initi);
	Secondary *second = new Secondary(ID,pos_initi,pos_final);
	post_drift.push_back(second);
      }
    }
    return post_drift;
  };

  double ElectronDrift::Get_Drift_X(double z){
    return Get_X(0,z);
  }
  double ElectronDrift::Get_Drift_Y(double z){
    return Get_Y(0,z);
  }
  double ElectronDrift::Get_Drift_T(double z){
    return Get_T(0,z);
  }
  
  Position* ElectronDrift::Get_FinalPosition(Position *pos_init){
    if(NO_Drift) return pos_init;
    double xi = pos_init->Get_X();
    double yi = pos_init->Get_Y();
    double zi = pos_init->Get_Z();
    double ti = pos_init->Get_T();
    double xf=Get_X(xi,zi);
    double yf=Get_Y(yi,zi);
    double zf=Get_Z();
    double tf=Get_T(ti,zi);
    Position *pos= new Position(xf,yf,zf,tf);
    return pos;
  }
  double ElectronDrift::Get_X(double x, double z){
    z/=10;
    double shift=0;
    double sigma=0;
    double dx = 0;
    if(Get_MagneticField()==0){
      shift = SpatialShiftDrift[0]+SpatialShiftDrift[1]*z+SpatialShiftTrans+SpatialShiftInduc;
      sigma = SpatialDiffusionDrift[0]+SpatialDiffusionDrift[1]*z+SpatialDiffusionDrift[2]*pow(z,2)+SpatialDiffusionDrift[3]*pow(z,3);
    }
    if(Get_MagneticField()==1){
      shift = SpatialShiftDriftMagField[0]+SpatialShiftDriftMagField[1]*z+SpatialShiftTransMagField+SpatialShiftInducMagField;
      sigma = SpatialDiffusionDriftMagField[0]+SpatialDiffusionDriftMagField[1]*z+SpatialDiffusionDriftMagField[2]*pow(z,2)+SpatialDiffusionDriftMagField[3]*pow(z,3);
    }
    sigma = sqrt(pow(sigma,2)+2*(pow(SpatialDiffusionTrans,2))+pow(SpatialDiffusionInduc,2));
    dx=10*r->Gaus(shift,Get_SpaceTuningFactor()*sigma);
    return (x+dx);
  }
  double ElectronDrift::Get_Y(double y, double z){
    z/=10;
    double shift, sigma;
    double dy = 0;
    shift = SpatialShiftDrift[0]+SpatialShiftDrift[1]*z+SpatialShiftTrans+SpatialShiftInduc;
    sigma = SpatialDiffusionDrift[0]+SpatialDiffusionDrift[1]*z+SpatialDiffusionDrift[2]*pow(z,2)+SpatialDiffusionDrift[3]*pow(z,3);
    sigma = sqrt(pow(sigma,2)+2*(pow(SpatialDiffusionTrans,2))+pow(SpatialDiffusionInduc,2));
    dy = 10* r->Gaus(shift,Get_SpaceTuningFactor()*sigma);
    return (y+dy);
  }
  double ElectronDrift::Get_Z(){
    return geometry->Get_DriftTotal();
  }
  double ElectronDrift::Get_T(double t, double z){
    z = 5 - z; //TO FIX 
    z/=10;
    double shift, sigma;
    double dt=0;
    if(Get_MagneticField()==0){
      shift = TemporalShiftDrift[0]+TemporalShiftDrift[1]*z+TemporalShiftTrans+TemporalShiftInduc;
      sigma = TemporalDiffusionDrift[0]+TemporalDiffusionDrift[1]*z+TemporalDiffusionDrift[2]*pow(z,2);
    }
    if(Get_MagneticField()==1){
      shift = TemporalShiftDriftMagField[0]+TemporalShiftDriftMagField[1]*z+TemporalShiftTransMagField+TemporalShiftInducMagField;
      sigma = TemporalDiffusionDriftMagField[0]+TemporalDiffusionDriftMagField[1]*z+TemporalDiffusionDriftMagField[2]*pow(z,2);
    }
    sigma = sqrt(pow(sigma,2)+2*(pow(TemporalDiffusionTrans,2))+pow(TemporalDiffusionInduc,2));
    dt = r->Gaus(shift,Get_TimeTuningFactor()*sigma);
    return (t+dt);
  };
}
