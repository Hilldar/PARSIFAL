/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Position_h
#define Position_h
using namespace std; 
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
// Z=0 is cathode plane
namespace PARSIFAL2{
  class Position{
  public: 
    //Construtor
    Position(){
      Set_X(0);
      Set_Y(0);
      Set_Z(0);
      Set_T(0);
    };
    Position(double x, double y, double z, double t):X(x),Y(y),Z(z),T(t){
      Set_X(x);
      Set_Y(y);
      Set_Z(z);
      Set_T(t);
    };
    Position(Position* position){
      Set_X(position->Get_X());
      Set_Y(position->Get_Y());
      Set_Z(position->Get_Z());
      Set_T(position->Get_T());
    };
    // CHECK - copy constructor
    Position(const Position &position){
      X = position.X;
      Y = position.Y;
      Z = position.Z;
      T = position.T;
    };
    //Destructor
    ~Position() {};
    //Function
    double Get_X()                                    {return X;};
    double Get_Y()                                    {return Y;};
    double Get_Z()                                    {return Z;};
    double Get_T()                                    {return T;};
    void   Set_Position(double x, double y, double z, double t) {Set_X(x);Set_Y(y);Set_Z(z);Set_T(t);};
    //Function
    void   Set_X(double io)                           {X=io;};
    void   Set_Y(double io)                           {Y=io;};
    void   Set_Z(double io)                           {Z=io;};
    void   Set_T(double io)                           {T=io;};
  private:
    //Variable
    double X,Y,Z,T;
  }; 
}
#endif


