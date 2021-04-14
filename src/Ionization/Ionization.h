#ifndef Ionization_h
#define Ionization_h
#include "Common/Common.h"
#include "Common/Position.h"
#include "Common/Primary.h"
#include "Common/Position.h"
#include "Common/Primary.h"
#include "Common/Secondary.h"
#include "Geometry/Geometry.h"
#include "Particle/Particle.h"
using namespace std;
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{

  class Ionization{
  public:
    //Constructor
    Ionization(Particle* particle, Geometry* geometry);
    //Destructor
    ~Ionization() { };

    //Function
    void      UpdatePosition(Particle *party); // CHECK 
    bool      Get_PrintInfo              () {return PrintInfo;};
    bool      Get_PrintNTuple            () {return PrintNTuple;};
    Position  Get_PositionInitial        () {return pos_initial;};
    Position  Get_PositionFinal          () {return pos_final;};
    Particle* Get_Particle               () {return particle;};
    Geometry* Get_Geometry               () {return geometry;};
    int       Get_TotalNumberOfPrimary   () {return TotalNumberOfPrimary;} 
    int       Get_TotalNumberOfSecondary () {return TotalNumberOfSecondary;} 

    vector<Primary*>   PrimaryIonization();
    vector<Secondary*> SecondaryIonization(vector<Primary*> primary);
  private:
    //Variables
    bool      PrintInfo;
    bool      PrintNTuple;
    Particle *particle;
    Geometry *geometry;
    Position  pos_initial;
    Position  pos_final;
    int       TotalNumberOfPrimary;
    int       TotalNumberOfSecondary;

    //Function
    Position  Measure_PositionFinal();
    void      Set_PrintInfo              (bool io)      {PrintInfo=io;};
    void      Set_PrintNTuple            (bool io)      {PrintNTuple=io;};
    void      Set_PositionInitial        (Position io)  {pos_initial=io;};
    void      Set_PositionFinal          (Position io)  {pos_final=io;};
    void      Set_TotalNumberOfPrimary   (int io)       {TotalNumberOfPrimary=io;} 
    void      Set_TotalNumberOfSecondary (int io)       {TotalNumberOfSecondary=io;} 

  };

}
#endif


