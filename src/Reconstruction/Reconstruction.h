#ifndef Reconstruction_h
#define Reconstruction_h
#include "Common/Common.h"
#include "Common/Position.h"
#include "Common/ElectronicChannel.h"
#include "Common/Hit.h"
#include "Common/Cluster.h"
#include "Geometry/Geometry.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TSystem.h"
#include "TLine.h"
#include "Ionization/Ionization.h"
using namespace std;
namespace PARSIFAL2{
  class Reconstruction{
  public:
    //Constructur
    Reconstruction(int setup,Geometry *geometry);
    //Destructor
    ~Reconstruction() {};
    //Function
    bool               Get_PrintInfo   () {return PrintInfo;};
    bool               Get_PrintNTuple () {return PrintNTuple;};
    vector<Hit*>       Digitize        (vector<ElectronicChannel*>);
    vector<Cluster1D*> Clusterize      (vector<Hit*>);
    //Function to Get Extra objects
    Ionization *ionio;
    void Get_Ionization(Ionization *ionio_IN){ionio=ionio_IN;};
    vector<Primary*> primi;
    void Get_Primary(vector<Primary*> primi_IN){for(int i=0;i<primi_IN.size();i++)primi.push_back(primi_IN.at(i));};
  private:
    //Variable
    bool      PrintInfo;
    bool      PrintNTuple;
    Geometry *geometry;
    int       Setup;
    vector<Hit*> hit_collection;
    double    drift_velocity, T0_TPC;
    
    //Function
    void      Set_PrintInfo     (bool io)   {PrintInfo=io;};
    void      Set_PrintNTuple   (bool io)   {PrintNTuple=io;};
    void      Set_Drift_Velocity(double io) {drift_velocity=io;};
    void      Set_T0_TPC        (double io) {T0_TPC=io;};
    void      Set_Hit_Collection(vector<Hit*> io) {hit_collection=io;};
    void      Set_is_TPC        (Cluster1D* cl1d, bool io){cl1d->Set_is_TPC(io);};
    void      Correction_Iso    (int size, vector<double> *position_tpc, vector<double> *drift_tpc, vector<double> *q_tpc);

    int       Get_Setup         () {return Setup;};
    double    Get_Drift_Velocity() {return drift_velocity;};   
    double    Get_T0_TPC        () {return T0_TPC;};
    double    Get_TPC           (Cluster1D* cl1d);
    //Function for Hits
    int       Get_Hit_type      (int hitID) {return hit_collection.at(hitID)->Get_Type();};
    int       Get_Hit_channel   (int hitID) {return hit_collection.at(hitID)->Get_ChannelID();};
    double    Get_Hit_charge    (int hitID) {return hit_collection.at(hitID)->Get_Charge();};
    double    Get_Hit_X         (int hitID) {return hit_collection.at(hitID)->Get_Position().Get_X();};
    double    Get_Hit_Y         (int hitID) {return hit_collection.at(hitID)->Get_Position().Get_Y();};
    double    Get_Hit_T         (int hitID) {return hit_collection.at(hitID)->Get_Time();};
    double    Get_Hit_dT        (int hitID) {return hit_collection.at(hitID)->Get_dTime();};
    double    Get_Hit_position1D(int hitID) {if(Get_Hit_type(hitID)==Xview) return Get_Hit_X(hitID); else if (Get_Hit_type(hitID)==Yview) return Get_Hit_Y(hitID); cout<<"No 1D Hit position \n"; return -9999;};
    double    Get_Pitch         (int hitID) {if(Get_Hit_type(hitID)==Xview) return geometry->Get_Pitch1(); else if (Get_Hit_type(hitID)==Yview) return geometry->Get_Pitch2(); cout<<"No pitch from hit \n"; return -9999;};
  };
}
#endif
