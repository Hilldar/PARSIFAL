#ifndef Resistive_h
#define Resistive_h
#include "Common/Common.h"
#include "Geometry/Geometry.h"
#include "Readout/Readout.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
#include <iostream>
#include <fstream>
using namespace std;
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{
  class Resistive{
  public:
    //Constructur                                                                                                                                                                                      
    Resistive(int setup, Geometry* geo, Readout* read);
    //Destructor                                                                                                                                                                                       
    ~Resistive() {};
    //Function                                                                                                                                                                                         
    bool               Get_PrintInfo     () {return PrintInfo;};
    bool               Get_PrintNTuple   () {return PrintNTuple;};
    Position*          Get_FinalPosition (Position* pos_init);
    vector<Secondary*> Diffusion         (vector<Secondary*> pre_resistive);
    void               Resist            ();
    void               Set_TauResist     (double io){tau = io;};
    void               Set_Sigma0Resist  (double io){sigma0 = io;};

    //Function Get
    double    Get_Diff_X        (double x);
    double    Get_Diff_Y        (double y);
    double    Get_Diff_T        (double t);
  private:
    //Variables
    bool      PrintInfo;
    bool      PrintNTuple;
    int       Setup;
    Geometry *geometry;
    Readout  *readout;
    TRandom3 *r;
    double sigma_res_diff;
    double sigma0;
    double tau;
    bool skip_resistive;
    //Function
    /*
    //Morello model
    double left (double pitch, int strip_step, double t0, double t){ return (-pitch/2. + strip_step*pitch)/(sqrt(2.)*sigma0*(1+(t-t0)/tau));}
    double right(double pitch, int strip_step, double t0, double t){ return ( pitch/2. + strip_step*pitch)/(sqrt(2.)*sigma0*(1+(t-t0)/tau));}
    double Q_t  (double pitch, int strip_step, double t0, double t, double q){if(t>=t0) return (q/2.*(TMath::Erf(right(pitch,strip_step,t0,t))-TMath::Erf(left(pitch,strip_step,t0,t)))); else return 0;}
    */

    //T2K model
    double sigma_res(double t, double t0){return (sqrt(2*sigma0*sigma0*(t-t0)/tau));}
    double left (double pitch, int strip_step, double t0, double t){ return (-pitch/2. + strip_step*pitch)/(sqrt(2.)*sigma_res(t,t0));}
    double right(double pitch, int strip_step, double t0, double t){ return ( pitch/2. + strip_step*pitch)/(sqrt(2.)*sigma_res(t,t0));}
    double Q_t  (double pitch, int strip_step, double t0, double t, double q){if(t>=t0) return (q/2.*(TMath::Erf(right(pitch,strip_step,t0,t))-TMath::Erf(left(pitch,strip_step,t0,t)))); else return 0;}

    
    
    //Function Set                                                                                                                                                                                     
    void      Set_PrintInfo     (bool io) {PrintInfo=io;};
    void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
    void      Set_Setup         (int io)  {Setup=io;};
    //Function Get                                                                                                                                                                                     
    double    Get_Setup         ()        {return Setup;}
    //double    Get_Diff_X        (double x);
    //double    Get_Diff_Y        (double y);
    double    Get_Diff_Z        (double z);
  };
}
#endif


