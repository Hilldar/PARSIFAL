/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Readout/Readout.h"
namespace PARSIFAL{
  Readout::Readout(int setup, int electronics, Geometry *geo):
    Setup(setup),
    Electronics(electronics),
    geometry(geo),
    PrintInfo(true),
    PrintNTuple(true)
  {
    r = new TRandom3();
    r->SetSeed(SEED);
    if(Get_Setup()==0 || Get_Setup()==1){
      channel.reserve(geo->Get_NumberOfStrip1()+geo->Get_NumberOfStrip2());
      //Strip case
      for(int ich=0;ich<geo->Get_NumberOfStrip1();ich++){
	int type=Xview; // X view
	double posX = (ich-0.5*geo->Get_NumberOfStrip1())*geo->Get_Pitch1();
	Position *pos = new Position(posX,0,0,0);
	ElectronicChannel* ch = new ElectronicChannel(type,ich,*pos);
	if(!NO_Readout) channel.push_back(ch);
	delete pos;
      }
      for(int ich=0;ich<geo->Get_NumberOfStrip2();ich++){
	int type=Yview; // Y view
	double posY = (ich-0.5*geo->Get_NumberOfStrip2())*geo->Get_Pitch2();
	Position *pos = new Position(0,posY,0,0);
	ElectronicChannel* ch = new ElectronicChannel(type,ich+geo->Get_NumberOfStrip1(),*pos);
	if(!NO_Readout) channel.push_back(ch);
	delete pos;
      }
    }
    else{
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<>        Readout        <>-<>"<<endl;
      cout<<"<>-<>  Check the setup ID   <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
    }
    if(Get_Electronics()==0){
      Initialize_APV();
    }
    if(Get_PrintInfo()){
      cout<<"----    Readout    ----"<<endl;
      cout<<"Readout configuration : "<<Get_Setup()<<endl;
      cout<<"----------------------"<<endl;
    }
    for(int it = 0; it < n_ns; it++) {
      TString name = "f"; name += it;
      f[it] = new TF1(name, "[0] * ((x - [1]) / [2]) * TMath::Exp(-(x - [1]) / [2])", it, n_ns);
    }

  };

  Readout::~Readout(){
    for(int it = 0; it < n_ns; it++) delete f[it];
    delete r;

    delete cap_probability_1;
    delete cap_probability_2;
    delete cap_time_1;
    delete cap_time_2;
    delete cap_charge_1;
    delete cap_charge_2;
    delete r_cap;
  };

  
  vector<ElectronicChannel*> Readout::Read(){
    if(NO_Readout) return channel;
    Simulate_Electronics();
    return channel;
  }

  vector<ElectronicChannel*> Readout::Read(vector<Secondary*> electron){
    if(NO_Readout) return channel;
    if(electron.empty()) return channel;   
    Define_ChannelID(electron);
    Simulate_Electronics();
    return channel;
  };

  int Readout::Define_View(){
    if(Get_Setup()==0){ //Strips 2D
      double charge_sharing = geometry->Get_ChargeSharing();
      if(charge_sharing/(1+charge_sharing) < r->Uniform(0,1)) return Xview;
      else return Yview;
    }
    else if(Get_Setup()==1){
      return Xview;
    }
    return -1;
  };
  
  void Readout::Define_ChannelID(vector<Secondary*> electron){
    if(electron.empty()) return;
    //Associate the electrons to the correct strip
    double charge_sharing = geometry->Get_ChargeSharing();
    for(int iele=0;iele<electron.size();iele++){
      if(charge_sharing/(1+charge_sharing) < r->Uniform(0,1)){
	electron.at(iele)->Set_ElectronicChannel(Get_ChannelID(Xview,electron.at(iele)));
      }
      else{
	electron.at(iele)->Set_ElectronicChannel(Get_ChannelID(Yview,electron.at(iele)));
      }
    }	
    return;	
  };
  
  void Readout::Induce_on_channel(int type, double position, double time, int weight){
    if(channel.empty()) return;
    for(int ich=0;ich<channel.size();ich++){
      if(type==channel.at(ich)->Get_Type() && type==Xview){
        if(abs(position-channel.at(ich)->Get_Position().Get_X())<0.5*geometry->Get_Pitch1()){
          channel.at(ich)->Fill_Time(time,weight);
        }
      }
      if(type==channel.at(ich)->Get_Type() && type==Yview){
        if(abs(position-channel.at(ich)->Get_Position().Get_Y())<0.5*geometry->Get_Pitch2()){
          channel.at(ich)->Fill_Time(time,weight);
        }
      }
    }
    return;
  };


  int Readout::Get_ChannelID(int type, Secondary* ele){
    if(channel.empty()) return -1;
    for(int ich=0;ich<channel.size();ich++){
      if(type==channel.at(ich)->Get_Type() && type==Xview){
	if(abs(ele->Get_PositionFinal().Get_X()-channel.at(ich)->Get_Position().Get_X())<0.5*geometry->Get_Pitch1()){
	  channel.at(ich)->Fill_Time(ele->Get_PositionFinal().Get_T(),1);
	  return ich;
	}
      }
      if(type==channel.at(ich)->Get_Type() && type==Yview){
	if(abs(ele->Get_PositionFinal().Get_Y()-channel.at(ich)->Get_Position().Get_Y())<0.5*geometry->Get_Pitch2()){
	  channel.at(ich)->Fill_Time(ele->Get_PositionFinal().Get_T(),1);
	  return ich;
	}
      }
    }
    return -1;
  };
  
  void Readout::Simulate_Electronics(){
    if(Get_Electronics()==0) Simulate_APV();
    return;
  }
  
  void Readout::Simulate_APV(){
    if(channel.empty()) return;
    Background_APV();
    Integration_APV();
    Capacitive_Induction();    
    Extract_Charge_Time();
    return;
  };

  void Readout::Initialize_APV(){
    //Values experimentally measured
    r_cap = new TRandom3();
    r_cap->SetSeed(SEED);
    cap_probability_1 = new TF1("cap_probability_1","pol4", 0, 1800); 
    cap_probability_2 = new TF1("cap_probability_2","expo(0)+expo(2)", 0, 1800); 
    cap_probability_1->SetParameters(-9.428,0.1063,-0.0002091,2.015e-07,-5.479e-11);
    cap_probability_2->SetParameters(2.6668,-0.00047385,4.73604,-0.00946892);
    cap_time_1 = new TF1("cap_time_1","pol3", 0, 1800); 
    cap_time_2 = new TF1("cap_time_2","pol3", 0, 1800); 
    cap_time_1->SetParameters(5.708,0.04036,-3.24e-05,6.657e-09);
    cap_time_2->SetParameters(13.47,0.006266,2.068e-05,-1.153e-08);
    cap_charge_1 = new TF1("cap_charge_1","expo(0)+expo(2)+expo(4)", 0, 1800); 
    cap_charge_2 = new TF1("cap_charge_2","expo(0)+expo(2)+expo(4)", 0, 1800); 
    cap_charge_1->SetParameters(1.363,0.0005104,3.386,-0.002523,8.683,-0.02639);
    cap_charge_2->SetParameters(1.957,-0.0003587,3.417,-0.003275,9.875,-0.03142);
    return;
  }

  void Readout::Background_APV(){
    if(NO_Noise) return;
    for(int ich=0;ich<channel.size();ich++){
      for(int itime=0;itime<27;itime++){
	// contribution evaluated from pedestals
	double bkg = r->Gaus(0,noise_APV/ele_to_ADC);
	// contribution evaluated from random trigger
	double freq_noise = 4.6e-6;
	if(r->Rndm() < freq_noise) {
	  double rnd_bkg = r->Landau(37.42, 4.24);
	  rnd_bkg /= ele_to_ADC; // ele 
	  bkg += rnd_bkg;
	}
	double q_bkg = bkg * ele_to_fC;
	double time = itime*timestep_APV;
	channel.at(ich)->Get_Histo_apv()->Fill(time, q_bkg); // fC 
      }
    }
    return;
  }

  void Readout::Integration_APV(){
    for(int ich=0;ich<channel.size();ich++){
      if(channel.at(ich)->Get_Histo_raw()->GetMaximum()==0) continue;
      for(int it = 0; it < n_ns; it++) {
	double t = it;
	int    timebin = channel.at(ich)->Get_Histo_raw()->FindBin(t);
	double integral = channel.at(ich)->Get_Histo_raw()->GetBinContent(timebin);
	f[it]->SetParameter(0, TMath::Exp(1.)*integral); //TO BE RESET?
	f[it]->SetParameter(1, t);
	f[it]->SetParameter(2, tau_APV);
      }
      TH1F *hintegratore = new TH1F("hintegratore", "", n_ns, 0, n_ns);
      for(int it = 0; it < n_ns; it++) {
	double t = it;
	double qmeas = 0;
	for(int jt = 0; jt < n_ns; jt++) {
	  double t2 = jt;
	  if(t2 < t)  qmeas += f[jt]->Eval(t);
	}
	hintegratore->SetBinContent(it, qmeas);
      }
      double jitter = jitter_APV;
      for(int iapv = 0; iapv < timebin_APV; iapv++) {
	double t = jitter + iapv * timestep_APV;
	double timebin = hintegratore->FindBin(t);
	double qmeas = hintegratore->GetBinContent(timebin);
	channel.at(ich)->Get_Histo_apv()->AddBinContent(iapv+1, qmeas);
      }
      for(int i=1;i<=n_ns;i++) channel.at(ich)->h_time_int->SetBinContent(i,hintegratore->GetBinContent(i));// REWRITE !!!!! check!!!
      delete hintegratore;
    }
    return;
  }

  void Readout::Capacitive_Induction(){
    if(NO_Capacitive) return;
    if(Get_Setup()==0){
      for(int ich=0;ich<channel.size();ich++){
	double q_peak = channel.at(ich)->Get_Histo_apv()->GetMaximum(); // fC
	double q_peak_adc = q_peak * fC_to_ADC; // ADC  
	if (Capacitive_Induction_1(q_peak_adc) &&
	    ich!=0 &&
	    ich!=channel.size()-1
	    ){
	  for(int itime = 0; itime < 27; itime++) {
	    double t_in = (itime+0.5) * 25;
	    double q_in = channel.at(ich)->Get_Histo_apv()->GetBinContent(itime+1) * fC_to_ADC;
	    if(q_in>1800) q_in=1800;
	    if(q_in<0)    continue;
	    // induce to i-strip +/- 1  
	    double q_1 = Get_Q_induced_1(q_in);
	    double t_1 = Get_T_induced_1(q_in,t_in);
	    if(channel.at(ich)->Get_Type()==channel.at(ich-1)->Get_Type()) {
	      channel.at(ich-1)->Get_Histo_cap()->Fill(t_1,q_1/fC_to_ADC);
	    }
	    if(channel.at(ich)->Get_Type()==channel.at(ich+1)->Get_Type()) {
	      channel.at(ich+1)->Get_Histo_cap()->Fill(t_1,q_1/fC_to_ADC);
	    }
	    if (Capacitive_Induction_2(q_peak_adc) &&
		ich!=1 &&
		ich!=channel.size()-2
		){
	      double q_2 = Get_Q_induced_2(q_1);
	      double t_2 = Get_T_induced_2(q_in,t_in);
	      if(channel.at(ich)->Get_Type()==channel.at(ich-2)->Get_Type()){
		channel.at(ich-2)->Get_Histo_cap()->Fill(t_2,q_2/fC_to_ADC);
	      }
	      if(channel.at(ich)->Get_Type()==channel.at(ich+2)->Get_Type()){
		channel.at(ich+2)->Get_Histo_cap()->Fill(t_2,q_2/fC_to_ADC);
	      }
	    }
	  }
	}
      }
      for(int ich=0;ich<channel.size();ich++){
	channel.at(ich)->Get_Histo_apv()->Add(channel.at(ich)->Get_Histo_cap(),1); 
      }
    }
    return;
  }

  bool Readout::Capacitive_Induction_1(double q_in){
    bool is_induced = false;
    double prob = r_cap->Uniform();
    prob *= 100;
    if(q_in<200) return false;
    if(q_in>1800) q_in = 1800;
    if(prob<cap_probability_1->Eval(q_in)) is_induced = true;
    return is_induced;
  }

  bool Readout::Capacitive_Induction_2(double q_in){
    bool is_induced = false;
    double prob = r_cap->Uniform();
    prob *= 100;
    if(q_in>1800) q_in = 1800;
    if(prob<cap_probability_2->Eval(q_in)) is_induced = true;
    return is_induced;
  }

  double Readout::Get_T_induced_1(double q_in, double t_in){
    double t=t_in;
    if(q_in>1800) q_in = 1800;
    double tt = cap_time_1->Eval(q_in);
    t -= r->Gaus(tt, 30);
    return t;
  }

  double Readout::Get_T_induced_2(double q_in, double t_in){
    double t=t_in;
    if(q_in>1800) q_in = 1800;
    t -= r->Gaus(cap_time_2->Eval(q_in), 30);
    return t;
  }

  double Readout::Get_Q_induced_1(double q_in){
    double ch=q_in;
    if(q_in>1800) q_in = 1800;
    double ratio = cap_charge_1->Eval(q_in)/100;
    if(ratio>0.5) ratio = 0.5; 
    ch *= ratio;
    return ch;
  }

  double Readout::Get_Q_induced_2(double q_in){
    double ch=q_in;
    if(q_in>1800) q_in = 1800;
    double ratio = cap_charge_1->Eval(q_in)/100;
    if(ratio>0.5) ratio = 0.5; 
    ch *= ratio;
    return ch;
  }


  void Readout::Extract_Charge_Time(){
    for(int ich=0;ich<channel.size();ich++){
      //Set the channel charge
      channel.at(ich)->Set_Charge(Get_Charge_APV(channel.at(ich)));
      //Set the channel time 
      channel.at(ich)->Set_Time(Get_Time_APV(channel.at(ich)));
      channel.at(ich)->Set_dTime(Get_dTime_APV(channel.at(ich)));
    }
    return;
  }
  
  double Readout::Get_Charge_APV(ElectronicChannel *ch){
    TH1D *h_time = ch->Get_Histo_apv();
    //Saturation
    double q_peak = h_time->GetMaximum(); // fC
    double q_peak_adc = q_peak * fC_to_ADC; // ADC
    
    if(q_peak_adc > 1800) {
      q_peak_adc = 1800;
      q_peak = q_peak_adc / fC_to_ADC; // fC

      for(int ibin=1; ibin<=27; ibin++) {
	if(h_time->GetBinContent(ibin) * fC_to_ADC > 1800) h_time->SetBinContent(ibin, q_peak);
      }
    }
    return q_peak;
  }

  double Readout::Get_Time_APV(ElectronicChannel *ch){
    Measure_Time_APV(ch);
    return Get_Time();
  }
  
  double Readout::Get_dTime_APV(ElectronicChannel *ch){
    //Measure_Time_APV(ch); --> already measured in the functuon Get_Time_APV
    return Get_dTime();
  }

  void Readout::Measure_Time_APV(ElectronicChannel *ch){
    TH1D *h_time = ch->Get_Histo_apv();
    double minbin = 0;
    double maxbin    = h_time->GetMaximumBin();
    double QMaxHisto = h_time->GetMaximum();
    for(int ibin = maxbin; ibin > 1; ibin--){
      if(h_time->GetBinContent(ibin) < 0.1 * QMaxHisto) {
	minbin = ibin;
	break;
      }
    }
    double startvalues[5] = {0};
    double fitlimup[5]    = {0};
    double fitlimlow[5]   = {0};
    double meanfirstbin = (h_time->GetBinContent(minbin-1) + h_time->GetBinContent(minbin-2) + h_time->GetBinContent(minbin-3))/3.;
    double sigma_MFB = TMath::Sqrt( ( pow(h_time->GetBinContent(minbin-1) - meanfirstbin, 2) + pow(h_time->GetBinContent(minbin-2) - meanfirstbin,2) + pow(h_time->GetBinContent(minbin-3)- meanfirstbin,2)) / 3);
    if(sigma_MFB < TMath::Abs(meanfirstbin)) sigma_MFB = TMath::Abs(meanfirstbin);
    startvalues[0] = meanfirstbin;
    startvalues[1] = QMaxHisto;
    startvalues[2] = 0.5* (minbin + maxbin) * timestep_APV;
    startvalues[3] = 0.5*timestep_APV;
    // put limits
    fitlimlow[0] = startvalues[0] - 2 * sigma_MFB;
    fitlimlow[1] = startvalues[1] - 0.3 * startvalues[1];
    fitlimlow[2] = minbin * timestep_APV;
    fitlimlow[3] = 0.1*timestep_APV;
    fitlimup[0] = startvalues[0] + 2 * sigma_MFB;
    fitlimup[1] = startvalues[1] + 0.3 * startvalues[1];
    fitlimup[2] = maxbin * timestep_APV;
    fitlimup[3] = 0.9*timestep_APV;
    // the real Fermi Dirac
    double minFD = minbin - 4;
    double maxFD = maxbin + 0;
    TF1 *f_FD = new TF1("f_FD", "[0] + [1]/(1+TMath::Exp(-(x - [2])/[3]))", minFD*timestep_APV, maxFD*timestep_APV);
    f_FD->SetParameters(startvalues[0], startvalues[1], startvalues[2], startvalues[3]);
    f_FD->SetParLimits(0, fitlimlow[0], fitlimup[0]);
    f_FD->SetParLimits(1, fitlimlow[1], fitlimup[1]);
    f_FD->SetParLimits(2, fitlimlow[2], fitlimup[2]);
    f_FD->SetParLimits(3, fitlimlow[3], fitlimup[3]);

    if(h_time->GetEntries() > 0) h_time->Fit("f_FD","WQRB");  // quiet/range/params // CHECK added if()

    double tFD = 0;
    tFD = r->Gaus(f_FD->GetParameter(2), 7.12);
    double dtFD = 0.;
    dtFD = f_FD->GetParError(2);
    delete f_FD;
    Set_Time(tFD);
    Set_dTime(dtFD);
  }
}
