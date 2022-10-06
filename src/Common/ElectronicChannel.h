/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef ElectronicChannel_h
#define ElectronicChannel_h
#include "TH1D.h"
#include "TCanvas.h"
using namespace std; 
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL{
  class ElectronicChannel{
  public:
    //Constructor
  ElectronicChannel():PrintNTuple(PrintNTuple_Channel){h_time_raw = new TH1D();h_time_apv = new TH1D();h_time_cap = new TH1D();};
    ElectronicChannel(int tipo, int ich, Position pos):
    type(tipo),channelID(ich),position(pos),PrintNTuple(PrintNTuple_Channel){
      h_time_raw = new TH1D(Form("raw_%i_%i",tipo,ich),Form("raw_%i_%i",tipo,ich),nbin,0,nbin);   //raw signal induced by electrons
      h_time_apv = new TH1D(Form("apv_%i_%i",tipo,ich),Form("apv_%i_%i",tipo,ich),27,0,nbin);     //apv signal
      h_time_cap = new TH1D(Form("cap_%i_%i",tipo,ich),Form("cap_%i_%i",tipo,ich),27,0,nbin);     //capacitive induction
      h_time_int = new TH1D(Form("int_%i_%i",tipo,ich),Form("int_%i_%i",tipo,ich),n_ns, 0, n_ns); //integrator circuit
    };
    //Destructor
    ~ElectronicChannel() {
      //delete position;
      delete h_time_raw;
      delete h_time_apv;
      delete h_time_cap;
      delete h_time_int;
    };
    //Function
    bool      Get_PrintNTuple ()          {return PrintNTuple;};
    Position  Get_Position    ()          {return position;};
    int       Get_ChannelID   ()          {return channelID;};
    int       Get_Type        ()          {return type;};
    double    Get_Charge      ()          {return charge;};
    double    Get_Time        ()          {return time;};
    double    Get_dTime       ()          {return dtime;};
    TH1D*     Get_Histo_raw   ()          {return h_time_raw;};
    TH1D*     Get_Histo_apv   ()          {return h_time_apv;};
    TH1D*     Get_Histo_cap   ()          {return h_time_cap;};
    TH1D*     Get_Histo_int   ()          {return h_time_int;};

    void      Set_Charge      (double io) {charge=io;};
    void      Set_Time        (double io) {time=io;};
    void      Set_dTime       (double io) {dtime=io;};

    void      Reset           () {Reset_Time();Set_Charge(0);Set_Time(0);Set_dTime(0);};
    void      Reset_Time      () {h_time_raw->Reset();h_time_apv->Reset();h_time_cap->Reset();};
    void      Fill_Time       (double io, int weight) {h_time_raw->Fill(io,1.6e-4*weight);}; // 1 electron * weight
    //void      Fill_Time       (double io, double io_fC) {h_time->Fill(io,io_fC);} // Some charge in fC
    void      Print_Time      () {
      TCanvas *c = new TCanvas("ccc","ccc",800,600);
      h_time_raw->Draw();
      c->SaveAs("ciao.pdf");
      // c->~TCanvas();
      delete c;
    };
    //Variables
    TH1D*     h_time_raw;
    TH1D*     h_time_apv;
    TH1D*     h_time_cap;
    TH1D*     h_time_int;
    //constant
    const int nbin = 675;
  private:
    //Variable
    bool      PrintNTuple;
    Position  position;
    int       channelID;
    int       type;   //1=X, 2=Y
    double    charge; //fC
    double    time;   //ns
    double    dtime;  //ns
    //Function
    void Set_Position          (Position io) {position=io;};
  };
}
#endif


