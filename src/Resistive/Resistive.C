/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Resistive/Resistive.h"
using namespace std;
namespace PARSIFAL{
  Resistive::Resistive(int setup,Geometry* geo):
    Setup(setup),
    geometry(geo),
    PrintInfo(true),
    PrintNTuple(true)
  {
    if(Get_Setup()!=0 && Get_Setup()!=1){
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<>      Resistive        <>-<>"<<endl;
      cout<<"<>-<>  Check the setup ID   <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
    }
    if(Get_PrintInfo()){
      cout<<"----     New Resistive         ----"<<endl;
      cout<<"Setup configuration              : "<<Get_Setup()<<endl;
      cout<<"----------------------"<<endl;
    }
  };
  
  vector<Secondary*> Resistive::Diffusion(vector<Secondary*> pre_resistive){
    vector<Secondary*> post_resistive;
    if(pre_resistive.empty()) return post_resistive;
    for(int isec=0;isec<pre_resistive.size();isec++){
      for(int iele=0;iele<pre_resistive.at(isec)->Get_GainedElectrons();iele++){
        int ID = pre_resistive.at(isec)->Get_PrimaryID();
        Position pos_initi = pre_resistive.at(isec)->Get_PositionInitial();
        Position pos_final = Get_FinalPosition(&pos_initi);
        Secondary *second = new Secondary(ID,pos_initi,pos_final);
        post_resistive.push_back(second);
      }
    }
    return post_resistive;
  };

  Position* Resistive::Get_FinalPosition(Position *pos_init){
    if(NO_Resistive) return pos_init;
    double xi = pos_init->Get_X();
    double yi = pos_init->Get_Y();
    double zi = pos_init->Get_Z();
    double ti = pos_init->Get_T();
    double xf=Get_Diff_X(xi);
    double yf=Get_Diff_Y(yi);
    double zf=Get_Diff_Z(zi);
    double tf=Get_Diff_T(ti);
    Position *pos= new Position(xf,yf,zf,tf);
    return pos;
  };

  double Resistive::Get_Diff_X(double x){
    return x;
  };
  double Resistive::Get_Diff_Y(double y){
    return y;
  };
  double Resistive::Get_Diff_Z(double z){
    return z;
  };
  double Resistive::Get_Diff_T(double t){
    return t;
  };
}
