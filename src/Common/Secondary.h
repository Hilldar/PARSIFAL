/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Secondary_h
#define Secondary_h
#include "Common/Position.h"
#include "Common/Primary.h"
#include <vector>
using namespace std; 
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL{
  class Secondary{
  public:
    //Constructor
  Secondary():position_initial(NULL), position_final(NULL), PrintNTuple(PrintNTuple_Secondary){};
  Secondary(int ID, Position pos_init, Position pos_final):
    primary_ID(ID), 
      position_initial(pos_init),
      position_final(pos_final),
      gained_electrons(1),
      electronic_channel(-1),
      PrintNTuple(PrintNTuple_Secondary){};
    //Destructor
    ~Secondary() {    };
    //Function
    Position  Get_PositionInitial   ()             {return position_initial;};
    Position  Get_PositionFinal     ()             {return position_final;};
    int       Get_PrimaryID         ()             {return primary_ID;};
    int       Get_GainedElectrons   ()             {return gained_electrons;}; 
    int       Get_ElectronicChannel ()             {return electronic_channel;};
    bool      Get_PrintNTuple       ()             {return PrintNTuple;};
    
    void      Set_PositionFinal     (Position io)  {position_final=io;};
    void      Set_GainedElectrons   (int io)       {gained_electrons=io;};
    void      Set_ElectronicChannel (int io)       {electronic_channel=io;};
  private:
    //Variable
    bool      PrintNTuple;
    Position  position_initial;
    Position  position_final;
    int       primary_ID;
    int       gained_electrons;
    int       electronic_channel;
    //Function
    void Set_PositionInitial      (Position* io) {position_initial=io;};
    void Set_PrimaryID            (int io)       {primary_ID=io;};
  };
}
#endif
