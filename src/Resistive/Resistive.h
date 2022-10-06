/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Resistive_h
#define Resistive_h
#include "Common/Common.h"
#include "Geometry/Geometry.h"
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
    Resistive(int setup, Geometry* geo);
    //Destructor                                                                                                                                                                                       
    ~Resistive() {};
    //Function                                                                                                                                                                                         
    bool               Get_PrintInfo     () {return PrintInfo;};
    bool               Get_PrintNTuple   () {return PrintNTuple;};
    Position*          Get_FinalPosition (Position *pos_init);
    vector<Secondary*> Diffusion         (vector<Secondary*> pre_resistive);
    //Function Get
    double    Get_Diff_X        (double x);
    double    Get_Diff_Y        (double y);
  private:
    //Variables
    bool      PrintInfo;
    bool      PrintNTuple;
    int       Setup;
    Geometry *geometry;
    //Function

    //Function Set                                                                                                                                                                                     
    void      Set_PrintInfo     (bool io) {PrintInfo=io;};
    void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
    void      Set_Setup         (int io)  {Setup=io;};
    //Function Get                                                                                                                                                                                     
    double    Get_Setup         ()        {return Setup;}
    //double    Get_Diff_X        (double x);
    //double    Get_Diff_Y        (double y);
    double    Get_Diff_Z        (double z);
    double    Get_Diff_T        (double t);
  };
}
#endif


