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
      Set_R(0);
      Set_Phi(0);
    };
    Position(double x, double y, double z, double t):X(x),Y(y),Z(z),T(t){
      Set_X(x);
      Set_Y(y);
      Set_Z(z);
      Set_T(t);
      Set_R(sqrt(pow(x,2)+pow(z,2)));
      Set_Phi(x,z);
    };
  Position(double r, double phi, double t):R(r),Phi(phi),T(t){
      Set_Phi(phi);
      Set_R(r);
      Set_X(Get_R()*cos(Get_Phi()));
      Set_Y(0);
      Set_Z(Get_R()*sin(Get_Phi()));
      Set_T(t);
    };
    Position(Position* position){
      Set_X  (position->Get_X()  );
      Set_Y  (position->Get_Y()  );
      Set_Z  (position->Get_Z()  );
      Set_T  (position->Get_T()  );
      Set_R  (position->Get_R()  );
      Set_Phi(position->Get_Phi());
    };
    // CHECK - copy constructor
    Position(const Position &position){
      X   = position.X;
      Y   = position.Y;
      Z   = position.Z;
      T   = position.T;
      R   = position.R;
      Phi = position.Phi;
    };
    //Destructor
    ~Position() {};
    //Function
    double Get_X()                                    {return X;};
    double Get_Y()                                    {return Y;};
    double Get_Z()                                    {return Z;};
    double Get_T()                                    {return T;};
    double Get_R()                                    {return R;};
    double Get_Phi()                                  {return Phi;};
    void   Set_Position(double x, double y, double z, double t) {Set_X(x);Set_Y(y);Set_Z(z);Set_T(t);};
    //Function
    void   Set_X  (double io)                         {X=io;};
    void   Set_Y  (double io)                         {Y=io;};
    void   Set_Z  (double io)                         {Z=io;};
    void   Set_T  (double io)                         {T=io;};
    void   Set_R  (double io)                         {R=io;};
    void   Set_Phi(double io)                         {Phi=io;};
  private:
    //Variable
    double X,Y,Z,T,R,Phi;
    //Function
    void   Set_Phi(double x, double z)                {
                                                       double pi = TMath::Pi(); 
						       double r  = sqrt(pow(x,2)+pow(z,2));
						       //cosi manca l'offset della dead-area
                                                       if     (z==0 && x== r)  Phi=0;
						       else if(x==0 && z== r)  Phi=pi/2.;
						       else if(z==0 && x==-r)  Phi=pi;
						       else if(x==0 && z==-r)  Phi=3*pi/2.;
						       else if(x==0 && z== 0)  Phi=0;
						       else if(x>0  && z>0  )  Phi=pi/2.-asin(x/r);
						       else if(x<0  && z>0  )  Phi=(asin(abs(x/r))+pi/2.);
						       else if(x<0  && z<0  )  Phi=(1.5*pi-asin(abs(x/r)));
						       else if(x>0  && z<0  )  Phi=(asin(abs(x/r))+3*pi/2.);
                                                      };
  }; 
}
#endif


