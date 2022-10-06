/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Geometry/Geometry.h"
#include <vector>
using namespace std;
namespace PARSIFAL2{

  Geometry::Geometry(int numberofview, double pitch1, double pitch2, int nstrip1, int nstrip2, double driftgap, double drifttotal, int charge_sharing):
    NumberOfView(numberofview),
    PrintInfo(true),
    PrintNTuple(PrintNTuple_Geo),
    Pitch1(0),
    Pitch2(0),
    nStrip1(0),
    nStrip2(0),
    DriftGap(driftgap),
    DriftTotal(drifttotal),
    ChargeSharing(charge_sharing)
  {
    Set_Pitch1(pitch1);
    Set_Pitch2(pitch2);
    Set_NumberOfStrip1(nstrip1);
    Set_NumberOfStrip2(nstrip2);
    if(Get_PrintInfo()){
      cout<<"---- New Geometry ----"<<endl;
      cout<<"Number of view           : "<<Get_NumberOfView()<<endl;
      cout<<"Pitch 1° view            : "<<Get_Pitch1()<<" mm"<<endl;
      cout<<"Pitch 2° view            : "<<Get_Pitch2()<<" mm"<<endl;
      cout<<"Number of strips 1° view : "<<Get_NumberOfStrip1()<<endl;
      cout<<"Number of strips 2° view : "<<Get_NumberOfStrip2()<<endl;
      cout<<"Drift gap thickness      : "<<Get_DriftGap()<<" mm"<<endl;
      cout<<"Total gaps thickness     : "<<Get_DriftTotal()<<" mm"<<endl;
      cout<<"Charge Sharing           : "<<Get_ChargeSharing()<<endl;
      cout<<"----------------------"<<endl;
    }
  };

  Geometry* Planar1D(double pitch1, int nstrip1, double driftgap, double drifttotal){
    Geometry *geo = new Geometry(1,pitch1,0,nstrip1,0,driftgap, drifttotal, 1);
    return geo;
  }
  Geometry* Planar2D(double pitch1, double pitch2, int nstrip1, int nstrip2, double driftgap, double drifttotal){
    Geometry *geo = new Geometry(2,pitch1,pitch2,nstrip1,nstrip2,driftgap, drifttotal, 1);
    return geo;
  }
  Geometry* Planar1D(double pitch1, double driftgap){
    Geometry *geo = new Geometry(1,pitch1,0,50,0,driftgap,driftgap, 1);
    return geo;
  }
  Geometry* Planar2D(double pitch1, double pitch2, double driftgap){
    Geometry *geo = new Geometry(2,pitch1,pitch2,50,50,driftgap,driftgap,1);
    return geo;
  }
}

