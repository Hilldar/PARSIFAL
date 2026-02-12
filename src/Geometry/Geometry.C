#include "Geometry/Geometry.h"
using namespace std;
namespace PARSIFAL2{

  Geometry::Geometry(int type, int numberofview, double pitch1, double pitch2, int nstrip1, int nstrip2, double driftgap, double drifttotal, int charge_sharing):
    Type(type),
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
    if(nStrip1!=0) Set_Min_X(-(nStrip1-2)*pitch1/2.);
    if(nStrip2!=0) Set_Min_Y(-(nStrip2-2)*pitch2/2.);
    if(nStrip1!=0) Set_Max_X( (nStrip1-2)*pitch1/2.);
    if(nStrip2!=0) Set_Max_Y( (nStrip2-2)*pitch2/2.);

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
      cout<<"X range                  : "<<Get_Min_X()<<" "<<Get_Max_Y()<<endl;
      cout<<"Y range                  : "<<Get_Min_Y()<<" "<<Get_Max_Y()<<endl;
      cout<<"----------------------"<<endl;
    }
  };

  Geometry* Planar1D(double pitch1, int nstrip1, double driftgap, double drifttotal){
    Geometry *geo = new Geometry(type_X,1,pitch1,0,nstrip1,0,driftgap, drifttotal, 0);
    return geo;
  }
  Geometry* Planar2D(double pitch1, double pitch2, int nstrip1, int nstrip2, double driftgap, double drifttotal){
    Geometry *geo = new Geometry(type_XY,2,pitch1,pitch2,nstrip1,nstrip2,driftgap, drifttotal, 1);
    return geo;
  }
  Geometry* Planar1D(double pitch1, double driftgap){
    Geometry *geo = new Geometry(type_X,1,pitch1,0,50,0,driftgap,driftgap, 0);
    return geo;
  }
  Geometry* Planar2D(double pitch1, double pitch2, double driftgap){
    Geometry *geo = new Geometry(type_XY,2,pitch1,pitch2,20,20,driftgap,driftgap,1);
    return geo;
  }
  Geometry* RPhi(double pitch, double driftgap){
    double r_in = 9.;
    int n_strip = 2*3.1415*r_in/pitch;
    Geometry *geo = new Geometry(type_RPhi,1,pitch,0,n_strip,0,driftgap, driftgap, 0);
    return geo;
  }

}

