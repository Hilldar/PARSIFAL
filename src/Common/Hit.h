/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Hit_h
#define Hit_h
#include "Common/ElectronicChannel.h"
using namespace std; 
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{
  class Hit{
  public:
    //Constructor
    Hit():PrintNTuple(PrintNTuple_Hit){};
    Hit(ElectronicChannel *ch):
    hitID(-1),
    channelID(ch->Get_ChannelID()),
    clusterID(-1),
    type(ch->Get_Type()),
    position(ch->Get_Position()),
    charge(ch->Get_Charge()),
    time(ch->Get_Time()),
    dtime(ch->Get_dTime()),
    PrintNTuple(PrintNTuple_Hit)
    {};
    //Destructor
    ~Hit() {
      //delete position;
    };
    //Function
    bool      Get_PrintNTuple ()          {return PrintNTuple;};
    Position  Get_Position    ()          {return position;};
    int       Get_ChannelID   ()          {return channelID;};
    int       Get_HitID       ()          {return hitID;};
    int       Get_ClusterID   ()          {return clusterID;};
    int       Get_Type        ()          {return type;};
    double    Get_Charge      ()          {return charge;};
    double    Get_Time        ()          {return time;};
    double    Get_dTime       ()          {return dtime;};

    void      Set_ClusterID   (int io)       {clusterID=io;};
    void      Set_HitID       (int io)       {hitID=io;};    
  private:
    //Variable
    bool      PrintNTuple;
    Position  position;
    int       hitID;
    int       channelID;
    int       clusterID;
    int       type;   //1=X, 2=Y
    double    charge; //fC
    double    time;   //ns
    double    dtime;  //ns
    //Function
    void      Set_Position    (Position io) {position=io;};
  };
}
#endif


