#ifndef ElectronicChannel_h
#define ElectronicChannel_h
#include "TH1D.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPaveText.h"
using namespace std; 
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{
  class ElectronicChannel{
  public:
    //Constructor
  ElectronicChannel():PrintNTuple(PrintNTuple_Channel),above_thr(false){
    h_time_raw      = new TH1D();
    h_time_tot      = new TH1D();
    h_time_res      = new TH1D();
    h_time_cap      = new TH1D();
    h_time_cur      = new TH1D();
    h_time_cur_buffer = new TH1D();
    h_time_int_apv  = new TH1D();
    h_time_int_tigT = new TH1D();
    h_time_int_tigE = new TH1D();
    h_time_apv      = new TH1D();
    h_time_int_tora = new TH1D();
  };
  ElectronicChannel(int tipo, int ich, Position pos, double passo):
    type(tipo),channelID(ich),position(pos),pitch(passo),n_electrons(0),PrintNTuple(PrintNTuple_Channel),above_thr(false){
      //APV
      TRandom3 *r = new TRandom3();
      r->SetSeed(ich);
      Set_Saturation_APV(r->Gaus(saturation_APV,sigma_saturation_APV));
      
      //Signal induced by electrons+ions
      h_time_raw       = new TH1D(Form("  raw_%i_%i",tipo,ich),Form("Current       raw_%i_%i",tipo,ich),n_ns, 0,  n_ns);         //raw signal induced by electrons+ions
      //Integrated charge
      h_time_tot       = new TH1D(Form("  tot_%i_%i",tipo,ich),Form("Charge[fC]    tot_%i_%i",tipo,ich),n_ns, 0, n_ns);          //integrated charge over time
      h_time_res       = new TH1D(Form("  res_%i_%i",tipo,ich),Form("Charge[fC]    res_%i_%i",tipo,ich),n_ns, 0, n_ns);          //resistive signa
      h_time_cap       = new TH1D(Form("  cap_%i_%i",tipo,ich),Form("              cap_%i_%i",tipo,ich),timebin_APV,0,nbin);     //capacitive induction
      //Electronics input
      h_time_cur       = new TH1D(Form("  cur_%i_%i",tipo,ich),Form("Current       cur_%i_%i",tipo,ich),n_ns, 0, n_ns);          //current after the resistive
      h_time_cur_buffer= new TH1D(Form("  cur_buffer_%i_%i",tipo,ich),Form("Current       cur_buffer_%i_%i",tipo,ich),n_ns+n_ns_buffer, -n_ns_buffer,  n_ns); //raw signal + buffer induced by electrons+ions
      //Electronics true
      h_time_int_apv   = new TH1D(Form("  int_%i_%i",tipo,ich),Form("Charge[fC]    int_%i_%i",tipo,ich),n_ns, 0, n_ns);          //avp integrator circuit
      h_time_int_tigT  = new TH1D(Form("tig_T_%i_%i",tipo,ich),Form("Voltage[mV] tig_T_%i_%i",tipo,ich),n_ns, 0,  n_ns);         //tiger Tbranch signal
      h_time_int_tigE  = new TH1D(Form("tig_E_%i_%i",tipo,ich),Form("Voltage[mV] tig_E_%i_%i",tipo,ich),n_ns, 0,  n_ns);         //tiger Ebranch signal
      h_time_int_tora  = new TH1D(Form("tora_T_%i_%i",tipo,ich),Form("Voltage[mV] tora_T_%i_%i",tipo,ich),n_ns, 0,  n_ns);         //tora Tbranch signal
      //Electronics discretized
      h_time_apv       = new TH1D(Form("  apv_%i_%i",tipo,ich),Form("Voltage[ADC]  apv_%i_%i",tipo,ich), timebin_APV, 0, timebin_APV*timestep_APV);//apv signal
      
      
      h_time_raw->GetXaxis()->SetTitle("Time [ns]");
      h_time_tot->GetXaxis()->SetTitle("Time [ns]");
      h_time_res->GetXaxis()->SetTitle("Time [ns]");
      h_time_cap->GetXaxis()->SetTitle("Time [ns]");
      h_time_cur->GetXaxis()->SetTitle("Time [ns]");
      h_time_int_apv->GetXaxis()->SetTitle("Time [ns]");
      h_time_int_tigT->GetXaxis()->SetTitle("Time [ns]");
      h_time_int_tigE->GetXaxis()->SetTitle("Time [ns]");
      h_time_apv->GetXaxis()->SetTitle("Time [ns]");
      h_time_int_tora->GetXaxis()->SetTitle("Time [ns]");

      h_time_raw->GetYaxis()->SetTitle("Current [1 fC / 1 ns]");
      h_time_tot->GetYaxis()->SetTitle("Charge [fC]");
      h_time_res->GetYaxis()->SetTitle("Charge [fC]");
      h_time_cap->GetYaxis()->SetTitle("Current [1 fC / 1 ns]");
      h_time_cur->GetYaxis()->SetTitle("Current [1 fC / 1 ns]");
      h_time_int_apv->GetYaxis()->SetTitle("Charge [fC]");
      h_time_int_tigT->GetYaxis()->SetTitle("Voltage [1 mV / 1 ns]");
      h_time_int_tigE->GetYaxis()->SetTitle("Voltage [1 mV / 1 ns]");
      h_time_apv->GetYaxis()->SetTitle("Voltage [ADC]");
      h_time_int_tora->GetYaxis()->SetTitle("Voltage [1 mV / 1 ns]");

      h_time_raw->GetYaxis()->SetTitleOffset(0.9);
      h_time_tot->GetYaxis()->SetTitleOffset(0.9);
      h_time_res->GetYaxis()->SetTitleOffset(0.9);
      h_time_cap->GetYaxis()->SetTitleOffset(0.9);
      h_time_cur->GetYaxis()->SetTitleOffset(0.9);
      h_time_int_apv->GetYaxis()->SetTitleOffset(0.9);
      h_time_int_tigT->GetYaxis()->SetTitleOffset(0.9);
      h_time_int_tigE->GetYaxis()->SetTitleOffset(0.9);
      h_time_apv->GetYaxis()->SetTitleOffset(0.9);
      h_time_int_tora->GetYaxis()->SetTitleOffset(0.9);

      h_time_int_tigT->GetYaxis()->SetRangeUser(-200,900);
      h_time_int_tigE->GetYaxis()->SetRangeUser(-200,900);
      h_time_int_tora->GetYaxis()->SetRangeUser(-200,900);
      //h_time_apv->GetYaxis()->SetRangeUser(-200,2000);

      V_thr_T = 10;
      V_thr_E = 10;
  };
    //Destructor
    ~ElectronicChannel() {
      //delete position;
      h_time_raw->~TH1D();
      h_time_tot->~TH1D();
      h_time_res->~TH1D();
      h_time_cap->~TH1D();
      h_time_cur->~TH1D();
      h_time_cur_buffer->~TH1D();
      h_time_int_apv->~TH1D();
      h_time_int_tigT->~TH1D();
      h_time_int_tigE->~TH1D();
      h_time_apv->~TH1D();
      h_time_int_tora->~TH1D();

      /*
      delete h_time_raw;
      delete h_time_apv;
      delete h_time_tiger_T;
      delete h_time_tiger_E;
      delete h_time_cap;
      delete h_time_int;
      delete h_time_res;
      delete h_time_tot;
      delete h_time_cur;
      */
    };
    //Function
    int       Get_Electronics   ()          {return ElectronicsType;};
    bool      Get_PrintNTuple   ()          {return PrintNTuple;};
    Position  Get_Position      ()          {return position;};
    int       Get_ChannelID     ()          {return channelID;};
    int       Get_Type          ()          {return type;};
    double    Get_Pitch         ()          {return pitch;};
    double    Get_Charge        ()          {return charge;};
    double    Get_Time          ()          {return time;};
    double    Get_dTime         ()          {return dtime;};
    double    Get_t_thr_E       ()          {return t_thr_E;};
    double    Get_t_Q_E         ()          {return t_Q_E;};
    double    Get_t_rising_raw  ()          {return t_rising_raw;};
    double    Get_t_rising_E    ()          {return t_rising_E;};
    double    Get_t_rising_T    ()    	    {return t_rising_T;};
    double    Get_t_falling_raw ()          {return t_falling_raw;};
    double    Get_t_falling_E   ()          {return t_falling_E;};
    double    Get_t_falling_T   ()    	    {return t_falling_T;};
    int       Get_N_electrons   ()          {return n_electrons;};
    TH1D*     Get_Histo_raw     ()          {return h_time_raw;};
    TH1D*     Get_Histo_tot     ()          {return h_time_tot;};
    TH1D*     Get_Histo_res     ()          {return h_time_res;};
    TH1D*     Get_Histo_cap     ()          {return h_time_cap;};
    TH1D*     Get_Histo_cur     ()          {return h_time_cur;};
    TH1D*     Get_Histo_cur_buffer()        {return h_time_cur_buffer;};
    TH1D*     Get_Histo_int_apv ()          {return h_time_int_apv;};
    TH1D*     Get_Histo_tiger_E ()          {return h_time_int_tigE;};
    TH1D*     Get_Histo_tiger_T ()          {return h_time_int_tigT;};
    TH1D*     Get_Histo_apv     ()          {return h_time_apv;};
    TH1D*     Get_Histo_int_tora  ()          {return h_time_int_tora;};
    bool      Get_AboveThr      ()          {return above_thr;};
    bool      Get_AboveThr_T    ()          {return above_thr_T;};
    bool      Get_AboveThr_E    ()          {return above_thr_E;};
    float     Get_Saturation_APV()          {return saturation_apv;};
    float     Get_Saturation_TIGER()        {return saturation_tiger;};
    float     Get_Saturation_TORA()         {return saturation_tora;};
    
    void      Set_Electronics   (int    io) {ElectronicsType=io;};
    void      Set_Charge        (double io) {charge=io;};
    void      Set_Time          (double io) {time=io;};
    void      Set_dTime         (double io) {dtime=io;};
    void      Set_t_thr_E       (double io) {t_thr_E=io;};
    void      Set_t_Q_E         (double io) {t_Q_E=io;};
    void      Set_t_rising_raw  (double io) {t_rising_raw=io;};
    void      Set_t_rising_E    (double io) {t_rising_E=io;};
    void      Set_t_rising_T    (double io) {t_rising_T=io;};
    void      Set_t_falling_raw (double io) {t_falling_raw=io;};
    void      Set_t_falling_E   (double io) {t_falling_E=io;};
    void      Set_t_falling_T   (double io) {t_falling_T=io;};
    void      Add_electrons     (int    io) {n_electrons+=io;};
    void      Set_AboveThr      (bool   io) {above_thr=io;};
    void      Set_AboveThr_T    (bool   io) {above_thr_T=io;};
    void      Set_AboveThr_E    (bool   io) {above_thr_E=io;};
    void      Set_V_thr_T       (float  io) {V_thr_T=io;};
    void      Set_V_thr_E       (float  io) {V_thr_E=io;};
    void      Set_Saturation_APV(float  io) {saturation_apv=io;};
    void      Set_Saturation_TIGER(float  io) {saturation_tiger=io;};
    void      Set_Saturation_TORA(float  io) {saturation_tora=io;};

    
    void      ResetNelectrons   ()          {n_electrons=0;};
    
    void      Reset             () {Reset_Time();Set_Charge(0);Set_Time(0);Set_dTime(0);};
    void      Reset_Time        () {
      h_time_raw->Reset();
      h_time_tot->Reset();
      h_time_res->Reset();
      h_time_cap->Reset();
      h_time_cur->Reset();
      h_time_cur_buffer->Reset();
      h_time_int_apv->Reset();
      h_time_int_tigE->Reset();
      h_time_int_tigT->Reset();
      h_time_int_tora->Reset();
      h_time_apv->Reset();
    };
    void      Fill_Time         (double io, float weight) {h_time_raw->Fill(io,1.6e-4*weight);}; // 1 electron * weight
    void      Print_Time(int ievent){
      if(Get_Electronics()==0) Print_Time_APV(ievent);
      if(Get_Electronics()==1) Print_Time_TIGER(ievent);
      if(Get_Electronics()==2) Print_Time_TORA(ievent);
    }
    void      Print_Time_APV    (int ievent) {
      TCanvas *c = new TCanvas("ccc","ccc",1600,1600);
      c->Divide(3,3);
      c->cd(1); h_time_raw->Draw("hist");
      c->cd(2); h_time_tot->Draw();
      c->cd(3);
      c->cd(4); h_time_cur->Draw("hist");
      c->cd(5); h_time_res->Draw();
      c->cd(6);
      c->cd(7); h_time_int_apv->Draw();
      c->cd(8);
      h_time_apv->Draw();
      float thr_APV_fC = 45;
      if(Get_AboveThr()){
	1;
      }
      else{
	      TLine *l_V_thr_T = new TLine(0,(float)(thr_APV_fC*fC_to_ADC),n_ns,(float)(thr_APV_fC*fC_to_ADC));
        l_V_thr_T->SetLineColor(kRed);
        l_V_thr_T->Draw("same");
      }
      c->cd(9);
      TPaveText *text_apv = new TPaveText(0.2,0.2,0.8,0.8);
      text_apv->AddText(Form("APV thr = %.0f fC",thr_APV_fC));
      text_apv->AddText(Form("Time measured = %.2f ns",Get_Time()));
      text_apv->AddText(Form("Charge measured = %.2f fC",Get_Charge()));
      text_apv->AddText(Form("Max charge collected = %.2f fC",Get_Histo_tot()->GetMaximum()));
      text_apv->Draw();      
      c->SaveAs(Form("pdf/Event_%i_Channel_%i.pdf",ievent,Get_ChannelID()));
      c->SaveAs(Form("pdf/Event_%i_Channel_%i.png",ievent,Get_ChannelID()));
      delete c;
    };

    void      Print_Time_TIGER    (int ievent) {
      TCanvas *c = new TCanvas("ccc","ccc",1600,1600);
      c->Divide(3,3);
      c->cd(1); h_time_raw->Draw("hist");
      c->cd(2); h_time_tot->Draw();
      c->cd(3);
      c->cd(4); h_time_cur->Draw("hist");
      c->cd(5); h_time_res->Draw();
      c->cd(6);
      c->cd(7); h_time_int_tigT->Draw();
      float t_thr_T = Get_Time();
      if(Get_AboveThr_T()){
        TLine *l_t_thr_T = new TLine(t_thr_T,h_time_int_tigT->GetMinimum()-10,t_thr_T,V_thr_T);
        TLine *l_V_thr_T = new TLine(0,V_thr_T,t_thr_T,V_thr_T);
        l_V_thr_T->SetLineColor(kRed);
        l_t_thr_T->SetLineColor(kRed);
        l_V_thr_T->Draw("same");
        l_t_thr_T->Draw("same");
      }
      else{
        TLine *l_V_thr_T = new TLine(0,V_thr_T,n_ns,V_thr_T);
        l_V_thr_T->SetLineColor(kRed);
        l_V_thr_T->Draw("same");       
      }
      c->cd(8); h_time_int_tigE->Draw();
      float t_thr_E = Get_Time();
      float V_Q     = Get_Charge()*gain_TIGER;
      if(Get_AboveThr_E()){
        TLine *l_t_thr_E = new TLine(Get_t_thr_E(),Get_Histo_tiger_E()->GetMinimum()-10,Get_t_thr_E(),V_thr_E);
        TLine *l_V_thr_E = new TLine(0,V_thr_E,Get_t_thr_E(),V_thr_E);
        TLine *l_t_Q     = new TLine(Get_t_Q_E(),Get_Histo_tiger_E()->GetMinimum()-10,Get_t_Q_E(),V_Q);
        TLine *l_V_Q     = new TLine(0,V_Q,Get_t_Q_E(),V_Q);
        l_V_thr_E->SetLineColor(kRed);
        l_t_thr_E->SetLineColor(kRed);
        l_t_Q->SetLineColor(kRed);
        l_V_Q->SetLineColor(kRed);
        l_V_thr_E->Draw("same");
        l_t_thr_E->Draw("same");
        l_t_Q->Draw("same");
        l_V_Q->Draw("same");
      }
      else{
        TLine *l_V_thr_E = new TLine(0,V_thr_E,n_ns,V_thr_E);
        l_V_thr_E->SetLineColor(kRed);
        l_V_thr_E->Draw("same");
      }
      c->cd(9);
      TPaveText *text_tiger = new TPaveText(0.2,0.2,0.8,0.8);
      text_tiger->AddText(Form("Tiger T thr = %.0f mV",V_thr_T));
      text_tiger->AddText(Form("Tiger E thr = %.0f mV",V_thr_E));
      text_tiger->AddText(Form("Time measured = %.2f ns",Get_Time()));
      text_tiger->AddText(Form("Charge measured = %.2f fC",Get_Charge()));
      text_tiger->AddText(Form("Voltage at Qmeas = %.2f mV",Get_Charge()*gain_TIGER));
      text_tiger->AddText(Form("Time at Qmeas = %.2f ns",Get_t_Q_E()));
      text_tiger->AddText(Form("Time at thr_E = %.2f ns",Get_t_thr_E()));
      text_tiger->AddText(Form("Max charge collected = %.2f fC",Get_Histo_tot()->GetMaximum()));
      text_tiger->Draw();
      c->SaveAs(Form("pdf/Event_%i_Channel_%i.pdf",ievent,Get_ChannelID()));
      c->SaveAs(Form("pdf/Event_%i_Channel_%i.png",ievent,Get_ChannelID()));
      delete c;
    };

    void      Print_Time_TORA    (int ievent) {
      TCanvas *c = new TCanvas("ccc","ccc",1600,1600);
      c->Divide(3,3);
      c->cd(1); h_time_raw->Draw("hist");
      c->cd(2); h_time_tot->Draw();
      c->cd(3);
      c->cd(4); h_time_cur->Draw("hist");
      c->cd(5); h_time_res->Draw();
      c->cd(6);
      c->cd(7); h_time_int_tora->Draw();
      
      float t_thr_T = Get_Time();
      if(Get_AboveThr_T()){
        TLine *l_t_thr_T = new TLine(t_thr_T,h_time_int_tora->GetMinimum()-10,t_thr_T,V_thr_T);
        TLine *l_V_thr_T = new TLine(0,V_thr_T,t_thr_T,V_thr_T);
        l_V_thr_T->SetLineColor(kRed);
        l_t_thr_T->SetLineColor(kRed);
        l_V_thr_T->Draw("same");
        l_t_thr_T->Draw("same");
      }
      else{
        TLine *l_V_thr_T = new TLine(0,V_thr_T,n_ns,V_thr_T);
        l_V_thr_T->SetLineColor(kRed);
        l_V_thr_T->Draw("same");  
      }
      c->cd(8);
      TPaveText *text_tora = new TPaveText(0.2,0.2,0.8,0.8);
      text_tora->AddText(Form("APV thr = %.0f fC",t_thr_T));
      text_tora->AddText(Form("Time measured = %.2f ns",Get_Time()));
      text_tora->AddText(Form("Charge measured = %.2f fC",Get_Charge()));
      text_tora->AddText(Form("Max charge collected = %.2f fC",Get_Histo_tot()->GetMaximum()));
      text_tora->Draw();      
      c->SaveAs(Form("pdf/Event_%i_Channel_%i.pdf",ievent,Get_ChannelID()));
      c->SaveAs(Form("pdf/Event_%i_Channel_%i.png",ievent,Get_ChannelID()));
      delete c;
    };
  
    //Variables
    TH1D*     h_time_raw;
    TH1D*     h_time_tot;
    TH1D*     h_time_res;
    TH1D*     h_time_cap;
    TH1D*     h_time_cur;
    TH1D*     h_time_cur_buffer;
    TH1D*     h_time_int_apv;
    TH1D*     h_time_int_tigE;
    TH1D*     h_time_int_tigT;
    TH1D*     h_time_apv;
    TH1D*     h_time_int_tora;
    //constant
    const int nbin = 1000;
  private:
    //Variable
    bool      PrintNTuple;
    Position  position;
    int       ElectronicsType;
    int       channelID;
    int       type;   //1=X, 2=Y
    double    pitch;
    double    charge; //fC
    double    time;   //ns
    double    dtime;  //ns
    int       n_electrons;
    bool      above_thr;
    bool      above_thr_E;
    bool      above_thr_T;
    double    t_thr_E;
    double    t_Q_E;
    double    t_rising_raw;
    double    t_rising_E;
    double    t_rising_T;
    double    t_falling_raw;
    double    t_falling_E;
    double    t_falling_T;
    float     V_thr_T;
    float     V_thr_E;
    float     saturation_apv;
    float     saturation_tiger;
    float     saturation_tora;
    //Function
    void Set_Position          (Position io) {position=io;};
  };
}
#endif
