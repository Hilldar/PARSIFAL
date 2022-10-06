/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Cluster_h
#define Cluster_h
#include "Common/Common.h"
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{
  class Cluster1D{
  public:
    //Constructor
    Cluster1D():PrintNTuple(PrintNTuple_Cluster1d){};
    Cluster1D(Hit *hit):
    type(hit->Get_Type()),
    PrintNTuple(PrintNTuple_Cluster1d),
    is_TPC(false)  
    {
	Set_HitID(hit->Get_HitID());
    };
    //Destructor
    ~Cluster1D() {
      delete position_CC;
      delete position_TPC;
    };
    //Function
    bool      Get_PrintNTuple  ()          {return PrintNTuple;};
    Position* Get_Position_CC  ()          {return position_CC;};
    Position* Get_Position_TPC ()          {return position_TPC;};
    int       Get_Type         ()          {return type;};
    int       Get_HitID        (int io)    {return hitID.at(io);};
    double    Get_Charge       ()          {return charge;};
    int       Get_Size         ()          {return size;};
    int       Get_ClusterID    ()          {return clusterID;};

    void      Set_Position_CC  (Position* io) {position_CC=io;};
    void      Set_Position_CC  (double io)    {if(Get_Type()==Xview) position_CC  = new Position(io,0,0,0); if(Get_Type()==Yview) position_CC  = new Position(0,io,0,0);};
    void      Set_Position_TPC (double io)    {if(Get_Type()==Xview) position_TPC = new Position(io,0,0,0); if(Get_Type()==Yview) position_TPC = new Position(0,io,0,0);};
    void      Set_HitID        (int io)       {hitID.push_back(io);};
    void      Set_Size         ()             {size=hitID.size();};
    void      Set_Charge       (double io)    {charge=io;};
    void      Set_ClusterID    (int io)       {clusterID=io;};
    void      Set_is_TPC       (bool io)      {is_TPC=io;}; 
  private:
    //Variable
    bool         PrintNTuple;
    Position*    position_CC;
    Position*    position_TPC;
    vector<int>  hitID;
    int          clusterID;
    int          type;   //1=X, 2=Y
    double       charge; //fC
    int          size;
    bool         is_TPC;
    //Function
  };
}
#endif


