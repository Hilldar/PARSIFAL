#include "Geometry/Geometry.h"
using namespace std;
using namespace PARSIFAL2;

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
  Geometry *geo = new Geometry(2,1,1,5);
  Geometry *gea = Planar1D(0.4,5);
  Geometry *gei = Planar2D(0.4,0.6,5);
  //
  //
  //Terminate
  return 0;
};

