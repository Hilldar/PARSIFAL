/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Particle_h
#define Particle_h
#include "Common/Common.h"
#include "Common/Position.h"
#include "Geometry/Geometry.h"
#include <iostream>
#include <fstream>
using namespace std;
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{
  class Particle{
  public:
    //Constructor
    Particle(int       particleID, 
             Position *position,   //mm  
             double    angleXZ,    //rad
             double    angleYZ,    //rad
             Geometry *geometry
    );
    //Destructor
    ~Particle() {};
    //Function
    bool      Get_PrintInfo    ()             {return PrintInfo;};
    bool      Get_PrintNTuple  ()             {return PrintNTuple;};
    double    Get_AngleXZ      ()             {return AngleXZ;};
    double    Get_AngleYZ      ()             {return AngleYZ;};
    int       Get_ParticleID   ()             {return ParticleID;};
    Position* Get_Position     ()             {return position;};
    double    Get_MeanPrimary  ()             {return MeanPrimary;};
    double    Get_ProbabilitySecondary(int i) {return ProbabilitySecondary[i];};
  private:
    //Variables
    bool      PrintInfo;
    bool      PrintNTuple;
    int       ParticleID;
    Position *position;
    double    AngleXZ;
    double    AngleYZ;
    Geometry *geometry;
    double    MeanPrimary;
    double    ProbabilitySecondary[100];
    //Function
    void      Set_PrintInfo            (bool io)                                {PrintInfo=io;};
    void      Set_PrintNTuple          (bool io)                                {PrintNTuple=io;};
    void      Set_ParticleID           (int io)                                 {ParticleID=io;};
    void      Set_AngleXZ              (double io)                              {AngleXZ=io;};
    void      Set_AngleYZ              (double io)                              {AngleYZ=io;};
    void      Set_Position             (double x, double y, double z, double t) {position= new Position(x,y,z,t);};
    void      Set_MeanPrimary          (double io)                              {MeanPrimary=io;};
    void      Set_ProbabilitySecondary (double io[100])                         {for(int i=0;i<100;i++)ProbabilitySecondary[i]=io[i];}
  };
  Particle* Particle1D(int particleID, double X, double T, double angleXZ, Geometry *geo);
  Particle* Particle2D(int particleID, double X, double Y, double T, double angleXZ, double angleYZ, Geometry *geo);
  Particle* Particle1D(int particleID, double angleXZ, Geometry *geo);
  Particle* Particle2D(int particleID, double angleXZ, double angleYZ, Geometry *geo);
}
#endif


