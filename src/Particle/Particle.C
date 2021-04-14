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
    if(Get_ParticleID()<=0){
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<> Check the particle ID <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      position->Set_Position(0,0,0,0);
      Set_AngleXZ(0);
      Set_AngleYZ(0);
    }
    else{
      //Valid for ArIso at 1.5 kV/cm in 1mm of gas --> to measure by garfield++ with a proper code
      Set_MeanPrimary(3.60032);//(18.0016/5);
      double const_int_prob_electron_per_cluster[100]={0.902864,0.920309,0.93045,0.936283,0.940138,0.943808,0.948277,0.954485,0.96098,0.966479,0.970936,0.974523,0.977442,0.979843,0.981959,0.983657,0.985152,0.986421,0.987404,0.988233,0.990002,0.990783,0.991482,0.992107,0.992666,0.993168,0.99362,0.994028,0.994397,0.994731,0.995036,0.995313,0.995567 ,0.9958,0.996015,0.996212,0.996395,0.996564,0.996721,0.996867,0.997003,0.99713,0.997248,0.99736,0.997464,0.997562,0.997655,0.997742,0.997825,0.997903,0.997976,0.998046,0.998113,0.998176,0.998236,0.998293,0.998348,0.9984,0.998449,0.998497,0.998542,0.998586,0.998628,0.998668,0.998706,0.998743,0.998779,0.998813,0.998846,0.998877,0.998908,0.998938,0.998966,0.998993,0.99902,0.999046,0.999071,0.999095,0.999118,0.99914,0.999162,0.999183,0.999204,0.999224,0.999243,0.999262,0.99928,0.999298,0.999315,0.999332,0.999348,0.999364,0.99938,0.999395,0.99941,0.999424,0.999438,0.999452,0.999465,0.999478};
      Set_ProbabilitySecondary(const_int_prob_electron_per_cluster);
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

