/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Primary_h
#define Primary_h
#include "Common/Position.h"
using namespace std; 
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{
  class Primary{
  public:
    //Constructor
    Primary():PrintNTuple(PrintNTuple_Primary){};
    Primary(Position pos, int numberofsecondary):position(pos),NumberOfSecondary(numberofsecondary),PrintNTuple(PrintNTuple_Primary){};
    //Destructor
    ~Primary() {};
    //Function
    Position  Get_Position          () {return position;};
    int       Get_NumberOfSecondary () {return NumberOfSecondary;};
    bool      Get_PrintNTuple       () {return PrintNTuple;};
  private:
    //Variable
    bool      PrintNTuple;
    Position  position;
    int       NumberOfSecondary;
    //Function
    void Set_Position          (Position io) {position=io;};
    void Set_NumberOfSecondary (int io)      {NumberOfSecondary=io;};
  };
}
#endif


