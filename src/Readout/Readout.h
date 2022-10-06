/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Readout_h
#define Readout_h
#include "Common/Common.h"
#include "Geometry/Geometry.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
#include "Common/ElectronicChannel.h"
#include "TF1.h"
using namespace std;
namespace PARSIFAL{
  class Readout{
  public:
    //Constructur
    Readout(int setup, int electronics, Geometry *geometry);
    //Destructor
    ~Readout();
    //Function
    bool                       Get_PrintInfo     () {return PrintInfo;};
    bool                       Get_PrintNTuple   () {return PrintNTuple;};
    vector<ElectronicChannel*> Read              ();
    vector<ElectronicChannel*> Read              (vector<Secondary*>);
    int                        Define_View       ();
    void                       Induce_on_channel (int type, double position, double time, int weight);

  private:
    //Variable
    bool      PrintInfo;
    bool      PrintNTuple;
    TRandom3 *r;
    int       Setup;
    int       Electronics;
    Geometry *geometry;
    vector<ElectronicChannel*> channel;
    
    //Function
    void      Set_PrintInfo     (bool io) {PrintInfo=io;};
    void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
    void      Set_Setup         (int io)  {Setup=io;};
    
    int       Get_Setup         ()        {return Setup;}
    int       Get_Electronics   ()        {return Electronics;}
    int       Get_ChannelID     (int type, Secondary* ele);
    void      Define_ChannelID  (vector<Secondary*>);
    void      Simulate_Electronics();
    void      Simulate_APV();
    void      Initialize_APV();
    void      Background_APV();
    void      Integration_APV();
    void      Extract_Charge_Time();
    void      Capacitive_Induction();
    double    Get_Charge_APV(ElectronicChannel *ch);
    void      Measure_Time_APV(ElectronicChannel *ch);
    double    Get_Time_APV(ElectronicChannel *ch);
    double    Get_dTime_APV(ElectronicChannel *ch);

    //Variable capacitive effect
    TF1* cap_probability_1;
    TF1* cap_probability_2;
    TF1* cap_time_1;
    TF1* cap_time_2;
    TF1* cap_charge_1;
    TF1* cap_charge_2;
    TRandom3 *r_cap;
    //Function capacitive
    bool Capacitive_Induction_1(double q);
    bool Capacitive_Induction_2(double q);
    double Get_T_induced_1(double q, double t);
    double Get_T_induced_2(double q, double t);
    double Get_Q_induced_1(double q);
    double Get_Q_induced_2(double q);
    //Integrator
    //define n_ns 675
    TF1 *f[n_ns];
    //APV
    double time, dtime;
    void Set_Time(double io){time=io;};
    double Get_Time(){return time;};
    void Set_dTime(double io){dtime=io;};
    double Get_dTime(){return dtime;};
  };
}
#endif
