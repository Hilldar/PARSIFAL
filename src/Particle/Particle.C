/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Particle/Particle.h"
using namespace std;
namespace PARSIFAL2{
  Particle::Particle(int particleID, Position *position, double angleXZ, double angleYZ, Geometry *geo):
  PrintInfo(true),
  PrintNTuple(PrintNTuple_Party),
  ParticleID(particleID),
  position(position),
  AngleXZ(angleXZ),
  AngleYZ(angleYZ),
  geometry(geo)
  {
    if(Get_ParticleID()==1){
      //Valid for ArIso 90:10 at 1.5 kV/cm in 1mm of gas --> to measure by garfield++ with a proper code
      Set_MeanPrimary(3.60032);//(18.0016/5);
      double const_int_prob_electron_per_cluster[200]={0.902864,0.920309,0.93045,0.936283,0.940138,0.943808,0.948277,0.954485,0.96098,0.966479,0.970936,0.974523,0.977442,0.979843,0.981959,0.983657,0.985152,0.986421,0.987404,0.988233,0.990002,0.990783,0.991482,0.992107,0.992666,0.993168,0.99362,0.994028,0.994397,0.994731,0.995036,0.995313,0.995567 ,0.9958,0.996015,0.996212,0.996395,0.996564,0.996721,0.996867,0.997003,0.99713,0.997248,0.99736,0.997464,0.997562,0.997655,0.997742,0.997825,0.997903,0.997976,0.998046,0.998113,0.998176,0.998236,0.998293,0.998348,0.9984,0.998449,0.998497,0.998542,0.998586,0.998628,0.998668,0.998706,0.998743,0.998779,0.998813,0.998846,0.998877,0.998908,0.998938,0.998966,0.998993,0.99902,0.999046,0.999071,0.999095,0.999118,0.99914,0.999162,0.999183,0.999204,0.999224,0.999243,0.999262,0.99928,0.999298,0.999315,0.999332,0.999348,0.999364,0.99938,0.999395,0.99941,0.999424,0.999438,0.999452,0.999465,0.999478};
      Set_ProbabilitySecondary(const_int_prob_electron_per_cluster);
    }
    else if(Get_ParticleID()==2){
      //Valid for ArCO2CF4 45:15:40 at 0.5 kV/cm in 1mm of gas 
      Set_MeanPrimary(4.96); //(29.81/6);
      double const_int_prob_electron_per_cluster[200]={0.729205,0.856153,0.902242,0.923029,0.935444,0.943189,0.950713,0.957423,0.96316,0.967801,0.971469,0.97451,0.97705,0.979182,0.980687,0.981909,0.983038,0.983979,0.984731,0.985828,0.987083,0.987804,0.988807,0.989654,0.990437,0.991064,0.991754,0.992224,0.992757,0.993071,0.993635,0.993792,0.994231,0.994701,0.994921,0.995172,0.99536,0.995611,0.995861,0.996112,0.996332,0.996457,0.996677,0.996771,0.996833,0.996896,0.996959,0.997116,0.99721,0.99721,0.997304,0.997429,0.997429,0.997523,0.997711,0.997837,0.997868,0.997931,0.998056,0.998056,0.998119,0.99815,0.998213,0.998307,0.998338,0.99837,0.99837,0.998401,0.998401,0.998401,0.998432,0.998432,0.998432,0.998464,0.998464,0.998495,0.998495,0.998558,0.998558,0.998558,0.998558,0.998652,0.998652,0.998715,0.998715,0.998746,0.998746,0.998746,0.998746,0.998777,0.998809,0.998809,0.998809,0.998871,0.998934,0.998965,0.998965,0.998965,0.998965,0.999028,0.999059,0.999059,0.999059,0.999059,0.999059,0.999059,0.999059,0.999059,0.999059,0.999091,0.999122,0.999122,0.999122,0.999122,0.999122,0.999122,0.999153,0.999185,0.999185,0.999216,0.999216,0.999216,0.999216,0.999248,0.999248,0.999248,0.999248,0.999248,0.999279,0.99931,0.99931,0.99931,0.999342,0.999373,0.999373,0.999404,0.999404,0.999404,0.999436,0.999436,0.999467,0.99953,0.99953,0.99953,0.99953,0.999561,0.999561,0.999561,0.999561,0.999592,0.999592,0.999592,0.999592,0.999592,0.999592,0.999592,0.999624,0.999624,0.999655,0.999655,0.999655,0.999655,0.999655,0.999686,0.999686,0.999686,0.999686,0.999686,0.999686,0.999686,0.999686,0.999686,0.999718,0.999718,0.999718,0.999718,0.999718,0.999749,0.999781,0.999781,0.999781,0.999781,0.999781,0.999781,0.999781,0.999843,0.999875,0.999875,0.999875,0.999875,0.999906,0.999906,0.999937,0.999937,0.999937,0.999969,0.999969,1,1,1};
      Set_ProbabilitySecondary(const_int_prob_electron_per_cluster);
    }
    else{
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<> Check the particle ID <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      position->Set_Position(0,0,0,0);
      Set_AngleXZ(0);
      Set_AngleYZ(0);
    }
    if(Get_PrintInfo()){
      cout<<"---- New Particle ----"<<endl;
      cout<<"Particle ID          : "<<ParticleID<<endl;
      cout<<"Position X           : "<<position->Get_X()<<endl;
      cout<<"Position Y           : "<<position->Get_Y()<<endl;
      cout<<"Position Z           : "<<position->Get_Z()<<endl;
      cout<<"Position T           : "<<position->Get_T()<<endl;
      cout<<"Incident angleXZ     : "<<Get_AngleXZ()<<endl;
      cout<<"Incident angleYZ     : "<<Get_AngleYZ()<<endl;
      cout<<"Mean Primary in 1mm  : "<<Get_MeanPrimary()<<endl;
      cout<<"Secondary probability: "<<Get_ProbabilitySecondary(0)<<" "<<Get_ProbabilitySecondary(1)<<" "<<Get_ProbabilitySecondary(2)<<" ... etc"<<endl;
      cout<<"----------------------"<<endl;
    }
  };
  Particle* Particle1D(int particleID, double X, double angleXZ, Geometry *geo){
    Position *position = new Position(X,0,0,0);
    Particle *party = new Particle(particleID, position, angleXZ, 0, geo);
    return party;
  };
  Particle* Particle2D(int particleID, double X, double Y, double angleXZ, double angleYZ, Geometry *geo){
    Position *position = new Position(X,Y,0,0);
    Particle *party = new Particle(particleID, position, angleXZ, angleYZ, geo);
    return party;
  };
  Particle* Particle1D(int particleID, double angleXZ, Geometry *geo){
    Position *position = new Position(0,0,0,0);
    Particle *party = new Particle(particleID, position, angleXZ, 0, geo);
    return party;
  };
  Particle* Particle2D(int particleID, double angleXZ, double angleYZ, Geometry *geo){
    Position *position = new Position(0,0,0,0);
    Particle *party = new Particle(particleID, position, angleXZ, angleYZ, geo);
    return party;
  };
}

