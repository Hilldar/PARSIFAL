#include "Signal/Signal.h"
namespace PARSIFAL2{

  Signal::Signal(int setup, int electronics, bool MagField, Geometry *geo){
    gain    = new DetectorGain(setup, Bfield);
    drift   = new ElectronDrift(setup, Bfield, geo);
    anode   = new Readout(setup,electronics,geo);
    resisto = new Resistive(setup,geo);
  };
  Signal::~Signal(){
    delete gain;                                                                                                                         
    delete drift;
    delete resisto;
    delete anode;
  };

  vector<ElectronicChannel*> Signal::Generate_Signal(vector<Primary*> primary){
    int iX=0;
    int iY=0;
    int gain_speedup = 1000;//1000; //3:1000 (top)  . 4:100 (nice). 5:10000 (approximate) . 6:100000 (no)
    if(primary.empty()) return anode->Read();
    for(int iprimi=0;iprimi<primary.size();iprimi++){ //loop on the primary electron clusters
      for(int iele=0; iele<primary.at(iprimi)->Get_NumberOfSecondary();iele++){ //lopp on the electrons inside the primary clusters
	int gain_this = gain->Get_Gain();
	gain_this /= gain_speedup;
	for(int igain=0; igain<gain_this;igain++){ //loop on the gained electrons via amplification
	  int view = anode->Define_View();
	  double z    = primary.at(iprimi)->Get_Position().Get_Z();
	  double time = primary.at(iprimi)->Get_Position().Get_T();
	  time += drift->Get_Drift_T(z);
	  if(view == Xview) {
	    iX++;
	    double x = primary.at(iprimi)->Get_Position().Get_X();
	    x += drift->Get_Drift_X(z);
	    x += resisto->Get_Diff_X(x);
	    anode->Induce_on_channel(Xview,x,time,gain_speedup);
	  }
	  else if(view == Yview) {
	    iY++;
	    double y = primary.at(iprimi)->Get_Position().Get_Y();
            y += drift->Get_Drift_Y(z);
	    y += resisto->Get_Diff_Y(y);
            anode->Induce_on_channel(Yview,y,time,gain_speedup);
	  }
	  else cout<<"Error in Signal::Generate_Signal --> no induction defined for this Readout type \n";
	  //if(igain==0){
	  //  cout<<"iPrimy: "<<iprimi<<" \t X: "<<primary.at(iprimi)->Get_Position().Get_X()<<" \t Z: "<<primary.at(iprimi)->Get_Position().Get_Z()<<" \t T: "<<primary.at(iprimi)->Get_Position().Get_T()<<" \t T(1): "<<time<<endl;
	  //}
	} //end loop on gained electrons after amplification
      } //end loop on electrons in the primary cluster
    } //end loop on primary clusters
    //cout<<"NUMBER OF ELECTRONS ON X : "<<iX<<" and ON Y : "<<iY<<endl;
    return anode->Read();
  };
}//end namespace
