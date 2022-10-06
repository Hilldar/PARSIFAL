/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Particle/Particle.h"
using namespace std;
using namespace PARSIFAL;

int main(int argc, const char* argv[]){
  //
  //
  //Input check
  //
  //
  //Initialization
  //
  //
  //Run
  Position *D3 = new Position(1,2,3,0);
  Particle *party = new Particle(1, D3, 0.1, -0.2);

  Particle *parti = Particle1D(1, -0.05, 0.2);
  Particle *part0 = Particle2D(1, -0.05, -0.04, 0.2, 0.1);

  //
  //
  //Terminate
  return 0;
};

