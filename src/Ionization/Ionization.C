/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Ionization/Ionization.h"
using namespace std;
namespace PARSIFAL2{

  Ionization::Ionization(Particle *party, Geometry *geo):
  PrintInfo(false),
  PrintNTuple(PrintNTuple_Ionio),
  particle(party),
  geometry(geo)
  {
    if(particle->Get_ParticleID()<=0){
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<> Check the particle ID <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
    }
    else{
      Set_PositionInitial(particle->Get_Position());
      Set_PositionFinal(Measure_PositionFinal());
    }
    if(Get_PrintInfo()){
      cout<<"---- New Ionization ----"<<endl;
      cout<<"Position initial X: "<<Get_PositionInitial().Get_X()<<endl;
      cout<<"Position initial Y: "<<Get_PositionInitial().Get_Y()<<endl;
      cout<<"Position initial Z: "<<Get_PositionInitial().Get_Z()<<endl;
      cout<<"Position initial T: "<<Get_PositionInitial().Get_T()<<endl;
      cout<<"Position final   X: "<<Get_PositionFinal().Get_X()<<endl;
      cout<<"Position final   Y: "<<Get_PositionFinal().Get_Y()<<endl;
      cout<<"Position final   Z: "<<Get_PositionFinal().Get_Z()<<endl;
      cout<<"Position final   T: "<<Get_PositionFinal().Get_T()<<endl;
      cout<<"----------------------"<<endl;
    }
  };

  // CHECK
  void Ionization::UpdatePosition(Particle *party)  {
    Set_PositionInitial(particle->Get_Position());
    Set_PositionFinal(Measure_PositionFinal());
    if(Get_PrintInfo()){
      cout<<"---- New Ionization ----"<<endl;
      cout<<"Position initial X: "<<Get_PositionInitial().Get_X()<<endl;
      cout<<"Position initial Y: "<<Get_PositionInitial().Get_Y()<<endl;
      cout<<"Position initial Z: "<<Get_PositionInitial().Get_Z()<<endl;
      cout<<"Position initial T: "<<Get_PositionInitial().Get_T()<<endl;
      cout<<"Position final   X: "<<Get_PositionFinal().Get_X()<<endl;
      cout<<"Position final   Y: "<<Get_PositionFinal().Get_Y()<<endl;
      cout<<"Position final   Z: "<<Get_PositionFinal().Get_Z()<<endl;
      cout<<"Position final   T: "<<Get_PositionFinal().Get_T()<<endl;
      cout<<"----------------------"<<endl;
    }
  };

  Position Ionization::Measure_PositionFinal(){
    double x = Get_PositionInitial().Get_X() + Get_Geometry()->Get_DriftGap()*TMath::Tan(Get_Particle()->Get_AngleXZ());
    double y = Get_PositionInitial().Get_Y() + Get_Geometry()->Get_DriftGap()*TMath::Tan(Get_Particle()->Get_AngleYZ());
    double z = Get_PositionInitial().Get_Z() + Get_Geometry()->Get_DriftGap();
    double t = Get_PositionInitial().Get_T();
    Position pos(x,y,z,t);
    return pos;
  }
  
  vector<Primary*> Ionization::PrimaryIonization(){
    int iion=0;
    vector<Primary*> primi;
    int totalnumberofsecondary=0;
    TRandom3 *r = new TRandom3();
    r->SetSeed(SEED);
    double x0 = Get_PositionInitial().Get_X();
    double y0 = Get_PositionInitial().Get_Y();
    double z0 = Get_PositionInitial().Get_Z();
    double x1 = Get_PositionFinal().Get_X();
    double y1 = Get_PositionFinal().Get_Y();
    double z1 = Get_PositionFinal().Get_Z();
    double time = Get_PositionInitial().Get_T();
    double dpag = Get_Geometry()->Get_DriftGap();
    double track_length_limit = TMath::Sqrt(pow((x1 - x0),2) + pow((y1 - y0),2) + pow((z1 - z0),2));
    // e' il numero di cluster/mm (NON in 0.5 cm!!)
    double n_ion_mm = Get_Particle()->Get_MeanPrimary(); // CHECK the value -> checked
    double track_length = 0;
  
    while(1 && !NO_PrimaryIonization) {
      //Generate primary electorns
      double u = r->Uniform(0, 1);
      // step along the trajectory, that for now is a straight line [x0, z0] -> [x1, z1]
      double dl_extracted = -(1./n_ion_mm) * TMath::Log(1 - u);
      track_length += dl_extracted;
      if(track_length > track_length_limit) break;
      iion++;
      double t = (track_length_limit - track_length)/track_length_limit;
      double tmp_z = z0 * t + z1 * (1 - t);
      double x = x0 * t + x1 * (1 - t);
      double y = y0 * t + y1 * (1 - t);
      if(tmp_z<=z0||tmp_z>=z1){if(Get_PrintInfo())cout<<"Compute z outside gas gap"<<endl;break;}
      //Generate secondary electrons
      int n_elec=0;
      double prob = r->Rndm();
      for(int j=0;j<100;j++){
        n_elec=j+1;
        if(prob<Get_Particle()->Get_ProbabilitySecondary(j)) break;
      }
      if(n_elec>100){if(Get_PrintInfo())cout<<"increase the size above 100"<<endl;n_elec=100;}

      totalnumberofsecondary+=n_elec;
      Position pos(x,y,z0+z1-tmp_z,time);
      Primary *primo = new Primary(pos,n_elec);
      primi.push_back(primo);
      if(Get_PrintInfo()){
        cout<<"---- New primary ionization ----"<<endl;
        cout<<"Position X         : "<<primo->Get_Position().Get_X()<<endl;
        cout<<"Position Y         : "<<primo->Get_Position().Get_Y()<<endl;
        cout<<"Position Z         : "<<primo->Get_Position().Get_Z()<<endl;
        cout<<"Position T         : "<<primo->Get_Position().Get_T()<<endl;
        cout<<"Number of secondary: "<<primo->Get_NumberOfSecondary()<<endl;
        cout<<"----------------------"<<endl;      
      }
    }
    if(!primi.empty() || !NO_PrimaryIonization) Set_TotalNumberOfPrimary(primi.size());
    else if (!NO_SecondaryIonization) Set_TotalNumberOfPrimary(1);
    else Set_TotalNumberOfPrimary(0);
    Set_TotalNumberOfSecondary(totalnumberofsecondary);
    if(Get_PrintInfo()){
        cout<<"---- Ionization completed ----"<<endl;
        cout<<"Total number of primary electrons  : "<<Get_TotalNumberOfPrimary()<<endl;
        cout<<"Total number of secondary electrons: "<<Get_TotalNumberOfSecondary()<<endl;
        cout<<"----------------------"<<endl;      
    }

    delete r;
    return primi;
  };

  vector<Secondary*> Ionization::SecondaryIonization(vector<Primary*> primary){
    int isec=0;
    vector<Secondary*> secondary;
    if(primary.empty()) return secondary;
    for(int iprimi=0;iprimi<primary.size();iprimi++){
      for(int iele=0; iele<primary.at(iprimi)->Get_NumberOfSecondary();iele++){
        Position pos = primary.at(iprimi)->Get_Position();
        Secondary *second = new Secondary(iprimi, pos, pos);
        secondary.push_back(second);
	isec++;
      }
    }
    return secondary;
  };
}


