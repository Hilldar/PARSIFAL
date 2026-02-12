#ifndef Geometry_h
#define Geometry_h
#include "Common/PrintNTuple.h"
#include <iostream>
#include <fstream>
using namespace std;

namespace PARSIFAL2{

  class Geometry{
  public:
    //Constructor
    Geometry(int type,
	     int numberofview,
             double pitch1,    // mm
             double pitch2,    // mm
             int    nstrip1,
             int    nstrip2,
             double driftpag,  // mm
             double drifttotal, // mm
             int    charge_sharing
    );
    //Destructor
    ~Geometry() {};

    //Variable
    //Function
    bool   Get_PrintInfo      () {return PrintInfo;};
    bool   Get_PrintNTuple    () {return PrintNTuple;};
    int    Get_Type           () {return Type;};
    int    Get_NumberOfView   () {return NumberOfView;};
    double Get_Pitch1         () {if(Get_NumberOfView()>0) return Pitch1; else return 0;};
    double Get_Pitch2         () {if(Get_NumberOfView()>1) return Pitch2; else return 0;};
    int    Get_NumberOfStrip1 () {if(Get_NumberOfView()>0) return nStrip1; else return 0;};
    int    Get_NumberOfStrip2 () {if(Get_NumberOfView()>1) return nStrip2; else return 0;};
    double Get_DriftGap       () {return DriftGap;};
    double Get_DriftTotal     () {return DriftTotal;};
    double Get_ChargeSharing  () {return ChargeSharing;};
    int    Get_Max_Defaul_Strip(){return max_default_strip;};
    float  Get_Min_X           (){return min_x;};
    float  Get_Min_Y           (){return min_y;};
    float  Get_Max_X           (){return max_x;};
    float  Get_Max_Y           (){return max_y;};
    
				
  private:  
    //Variables
    bool   PrintInfo;
    bool   PrintNTuple;
    int    Type;
    int    NumberOfView;
    double Pitch1;     // mm
    double Pitch2;     // mm
    int    nStrip1;
    int    nStrip2;
    double DriftGap;   // mm
    double DriftTotal; // mm
    double ChargeSharing;
    const int  max_default_strip = 128;
    float min_x, min_y,max_x,max_y;
    //Function

    void   Set_PrintInfo      (bool io)   {PrintInfo=io;};
    void   Set_PrintNTuple    (bool io)   {PrintNTuple=io;};
    void   Set_NumberOfView   (int io)    {NumberOfView=io;};
    void   Set_Pitch1         (double io) {if(Get_NumberOfView()>0) Pitch1=io;};
    void   Set_Pitch2         (double io) {if(Get_NumberOfView()>1) Pitch2=io;};
    void   Set_NumberOfStrip1 (int io)    {if(Get_NumberOfView()>0) nStrip1=io;};
    void   Set_NumberOfStrip2 (int io)    {if(Get_NumberOfView()>1) nStrip2=io;};
    void   Set_DriftGap       (double io) {DriftGap=io;};
    void   Set_DriftTotal     (double io) {DriftTotal=io;};
    void   Set_ChargeSharing  (double io) {ChargeSharing=io;};
    void   Set_Min_X          (float io)  {min_x=io;};
    void   Set_Min_Y          (float io)  {min_y=io;};
    void   Set_Max_X          (float io)  {max_x=io;};
    void   Set_Max_Y          (float io)  {max_y=io;};


  };

  const int  type_X    = 0;
  const int  type_XY   = 1;
  const int  type_RPhi = 2;

  Geometry* Planar1D(double pitch,                                           double driftgap                   );
  Geometry* Planar2D(double pitch1, double pitch2,                           double driftgap                   );
  Geometry* Planar1D(double pitch,                 int nstrip,               double driftgap, double drifttotal);
  Geometry* Planar2D(double pitch1, double pitch2, int nstrip1, int nstrip2, double driftgap, double drifttotal);
  Geometry* RPhi    (double pitch,                                           double driftgap);
}
#endif


