/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "ElectronDrift/ElectronDrift.h"
using namespace std;
namespace PARSIFAL{
  ElectronDrift::ElectronDrift(int setup, bool MagField, Geometry* geo):
    Setup(setup),
    MagneticField(MagField),
    geometry(geo),
    PrintInfo(true),
    PrintNTuple(true)
  {
    tuning_factor_diff = 1.0;
    tuning_factor_time = 1.0;
    r = new TRandom3();

    r->SetSeed(SEED);
    if(Get_Setup()==0){
      // no mag field -----------
      // >>> spatial shift
      SpatialShiftDrift[0] = 0.; // OK // shift drift
      SpatialShiftDrift[1] = 0.; // OK // shift drift
      SpatialShiftTrans = 0.; // OK // shift transfer
      SpatialShiftInduc = 0.; // OK // shift induction
      SpatialDiffusionDrift[0] = 0.0275728;    // OK // sigma drift
      SpatialDiffusionDrift[1] = -0.0460094;   // OK // sigma drift
      SpatialDiffusionDrift[2] = 0.102019;     // OK // sigma drift
      SpatialDiffusionDrift[3] = -0.202226;    // OK // sigma drift
      SpatialDiffusionTrans = 0.0167369;  // OK // sigma transfer
      SpatialDiffusionInduc = 0.0158942;  // OK // sigma induction
      // >>> temporal shift
      TemporalShiftDrift[0] = 136.981;  // OK // shift drift
      TemporalShiftDrift[1] = -271.213; // OK // shift drift
      TemporalShiftTrans = 56.6527; // OK // shift transfer
      TemporalShiftInduc = 56.5305; // OK // sigma induction
      TemporalDiffusionDrift[0] = 3.52458;  // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDrift[1] = -2.01135; // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDrift[2] = -4.15946; // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDrift[3] = 0;
      TemporalDiffusionTrans = 2.12464; // OK // sigma transfer
      TemporalDiffusionInduc = 1.93061; // OK // sigma induction
      // mag field --------------
      // >>> spatial shift
      SpatialShiftDriftMagField[0] = 0.241024; // OK // shift drift 
      SpatialShiftDriftMagField[1] = -0.47836; // OK // shift drift 
      SpatialShiftTransMagField = 0.03891; // OK // shift transfer
      SpatialShiftInducMagField = 0.02285; // OK // shift induction
      SpatialDiffusionDriftMagField[0] = 0.0334884;    // OK // sigma drift
      SpatialDiffusionDriftMagField[1] = -0.0661326;   // OK // sigma drift
      SpatialDiffusionDriftMagField[2] = 0.17014;      // OK // sigma drift
      SpatialDiffusionDriftMagField[3] = -0.302228;    // OK // sigma drift
      SpatialDiffusionTransMagField = 0.0174;          // OK // sigma transfer
      SpatialDiffusionInducMagField = 0.01611;         // OK // sigma induction
      // >>> temporal shift
      TemporalShiftDriftMagField[0] = 147.213;  // OK // shift drift
      TemporalShiftDriftMagField[1] = -291.508; // OK // shift drift
      TemporalShiftTransMagField = 57.6; // OK // shift transfer
      TemporalShiftInducMagField  = 56.78; // OK // shift induction
      // >>> temporal sigma 
      TemporalDiffusionDriftMagField[0] = 3.94043;  // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM 
      TemporalDiffusionDriftMagField[1] = -2.58493; // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDriftMagField[2] = -4.25339; // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDriftMagField[3] = 0;
      TemporalDiffusionTransMagField = 2.155;       // OK // sigma transfer
      TemporalDiffusionInducMagField = 1.933;       // OK // sigma induction
    }
    else if(Get_Setup()==1){
      // no mag field -----------                                                                                                                                                                               // >>> spatial shift                                                                                                                                                                                
      SpatialShiftDrift[0] = 0.; // OK // shift drift 
      SpatialShiftDrift[1] = 0.; // OK // shift drift
      SpatialShiftTrans = 0.; // OK // shift transfer
      SpatialShiftInduc = 0.; // OK // shift induction 
      SpatialDiffusionDrift[0] = 0.000452964;    // OK // sigma drift
      SpatialDiffusionDrift[1] = 0.0400226;   // OK // sigma drift                                                                                                                                       
      SpatialDiffusionDrift[2] = -0.0878887;     // OK // sigma drift
      SpatialDiffusionDrift[3] = 0.0775006;    // OK // sigma drift
      SpatialDiffusionTrans = 0;  // OK // sigma transfer 
      SpatialDiffusionInduc = 0;  // OK // sigma induction
      // >>> temporal shift
      double par0 = -6.31906;
      double par1 = 464.536;
      TemporalShiftDrift[0] = -6.319060;  // OK // shift drift
      TemporalShiftDrift[1] = 464.536; // OK // shift drift
      TemporalShiftTrans = 0; // OK // shift transfer
      TemporalShiftInduc = 0; // OK // sigma induction
      double rap0 = 2.57168;
      double rap1 = 19.0958;
      double rap2 = -44.7537;
      double rap3 = 38.6046;
      TemporalDiffusionDrift[0] = 2.57168;  // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDrift[1] = 19.0958; // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDrift[2] = -44.7537; // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionDrift[3] = 38.6046; // OK // sigma drift MUST BE COMBINED WITH A QUADRATIC SUM
      TemporalDiffusionTrans = 0; // OK // sigma transfer                                                                                                                                           
      TemporalDiffusionInduc = 0; // OK // sigma induction                                                                                                                                          
      // mag field -------------- UNSET
      // >>> spatial shift 
      SpatialShiftDriftMagField[0] = 0;
      SpatialShiftDriftMagField[1] = 0;
      SpatialShiftTransMagField = 0;
      SpatialShiftInducMagField = 0;
      SpatialDiffusionDriftMagField[0] = 0;
      SpatialDiffusionDriftMagField[1] = 0;
      SpatialDiffusionDriftMagField[2] = 0;
      SpatialDiffusionDriftMagField[3] = 0;
      SpatialDiffusionTransMagField = 0;
      SpatialDiffusionInducMagField = 0;
      // >>> temporal shift
      TemporalShiftDriftMagField[0] = 0;
      TemporalShiftDriftMagField[1] = 0;
      TemporalShiftTransMagField = 0;
      TemporalShiftInducMagField  = 0;
      // >>> temporal sigma 
      TemporalDiffusionDriftMagField[0] = 0;
      TemporalDiffusionDriftMagField[1] = 0;
      TemporalDiffusionDriftMagField[2] = 0;
      TemporalDiffusionDriftMagField[3] = 0;
      TemporalDiffusionTransMagField = 0;
      TemporalDiffusionInducMagField = 0;
    }
    else{
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
    if(Get_Setup()==0) z = 5 - z; //TO FIX 
    //if(Get_Setup()==1) z = 6 - z; //TO FIX
    z/=10;
    double shift, sigma;
    double dt=0;
    if(Get_MagneticField()==0){
      shift = TemporalShiftDrift[0]+TemporalShiftDrift[1]*z+TemporalShiftTrans+TemporalShiftInduc;
      sigma = TemporalDiffusionDrift[0]+TemporalDiffusionDrift[1]*z+TemporalDiffusionDrift[2]*pow(z,2)+TemporalDiffusionDrift[3]*pow(z,3);
    }
    if(Get_MagneticField()==1){
      shift = TemporalShiftDriftMagField[0]+TemporalShiftDriftMagField[1]*z+TemporalShiftTransMagField+TemporalShiftInducMagField;
      sigma = TemporalDiffusionDriftMagField[0]+TemporalDiffusionDriftMagField[1]*z+TemporalDiffusionDriftMagField[2]*pow(z,2)+TemporalDiffusionDriftMagField[3]*pow(z,3);
    }
    sigma = sqrt(pow(sigma,2)+2*(pow(TemporalDiffusionTrans,2))+pow(TemporalDiffusionInduc,2));
    dt = r->Gaus(shift,Get_TimeTuningFactor()*sigma);
    return (t+dt);
  };
}
