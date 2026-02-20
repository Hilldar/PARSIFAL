#include <omp.h>
#include "Readout/Readout.h"
namespace PARSIFAL2{
  Readout::Readout(int setup, int electronics, Geometry *geo):
    Setup(setup),
    Electronics(electronics),
    geometry(geo),
    PrintInfo(true),
    PrintNTuple(true)
  {
    r = new TRandom3();
    r->SetSeed(SEED);
    Initialize();
    sigma_noise_fC = 2.0; // 2.0 fC
    threshold_fC   = 2.0; // 2.0 fC
    thrT_TIGER     = 10;  // threshold_fC*gain_TIGER;
    thrE_TIGER 	   = 10;  // threshold_fC*gain_TIGER;
    TIGER_Get_Maximum = false;
    
    gain_TORA     = 12.7; //fC/mV
    thrT_TORA     = 10;  // threshold_fC*gain_TIGER;
    // thrE_TORA 	  = 10;  // threshold_fC*gain_TIGER;

    IT_Lenght          = 170; // ns
    IT_amplitude       = 0.00588; // fC/ns = 1/IT_Length
    IT_ratio_fast_slow = 0.85; // 85% of the signal comes from ions and 15% from electrons
    
    if(Get_PrintInfo()){
      cout<<"----    Readout    ----"<<endl;
      cout<<"Readout configuration : "<<Get_Setup()<<endl;
      cout<<"Readout electronics   : "<<Get_Electronics()<<endl;
      cout<<"----------------------"<<endl; 
    }
    //to be removed ?
    if(Get_Electronics()==0){
      for(int it = 0; it < n_ns; it++) {
        TString name = "f"; name += it;
        f[it] = new TF1(name, "[0] * ((x - [1]) / [2]) * TMath::Exp(-(x - [1]) / [2])", it, n_ns);
      }
    }
    /* NON VA QUI
    //Noise amplitude in noise 1/f
    if(noise_1_over_f){
      if(Get_Electronics()==0) noise_amplitude = sigma_noise_fC/16.47; //APV
      if(Get_Electronics()==1) noise_amplitude = sigma_noise_fC/21.91; //TIGER E-branch
      //if(Get_Electronics()==1) noise_amplitude = sigma_noise_fC/17.36; //TIGER T-branch
    }
    //White noise
    if(white_noise){
      //float slope = 156.375-15844.1/(tau_APV+111.638); //tau_APV = shaping time
      if(Get_Electronics()==0){
	float slope = 176.6-21539.47/(tau_APV+133.3); //tau_APV = shaping time
	noise_amplitude = sigma_noise_fC/slope; // APV: STD sigma noise = slope * max amplitude input noise current
      }
      if(Get_Electronics()==1){
	//noise_amplitude = 0.005; // TIGER: E-branch
	cout<<"AAAAAAAA"<<endl;
	float slope = 21.9114;
	noise_amplitude = sigma_noise_fC/slope; 
      }
    }
    */
  };

  Readout::~Readout(){
    if(Get_Electronics()==0){
      for(int it = 0; it < n_ns; it++) delete f[it];
      delete r;
      delete cap_probability_1;
      delete cap_probability_2;
      delete cap_time_1;
      delete cap_time_2;
      delete cap_charge_1;
      delete cap_charge_2;
      delete r_cap;
    }
  };

  void Readout::Set_Noise_fC(double io){
    sigma_noise_fC=io;
    //Configure the noise level
    //
    //Noise amplitude in noise 1/f
    if(noise_1_over_f){
      if(Get_Electronics()==0) noise_amplitude = sigma_noise_fC/16.47; //APV
      if(Get_Electronics()==1) noise_amplitude = sigma_noise_fC/21.91; //TIGER E-branch
      //if(Get_Electronics()==1) noise_amplitude = sigma_noise_fC/17.36; //TIGER T-branch
      }
    //White noise
    if(white_noise){
      if(Get_Electronics()==0){
	//float slope = 156.375-15844.1/(tau_APV+111.638); //tau_APV = shaping time 
	//float slope = 176.6-21539.47/(tau_APV+133.3); //tau_APV = shaping time
	//noise_amplitude = sigma_noise_fC/slope; // APV: STD sigma noise = slope * max amplitude input noise current
	//2025.08.28
	//float slope = 35.31;
	//2025.10.15
	float slope = 44.14;
        noise_amplitude = sigma_noise_fC/slope;
      }
      if(Get_Electronics()==1){
	//(noise_amplitude = 0.005; // TIGER: E-branch
	//float slope = 76.75;
	//2025.08.28
	//float slope = 27.73;
	//2025.10.15
	float slope = 43.48;
        noise_amplitude = sigma_noise_fC/slope;
      }
    }
  };
  
  void Readout::Initialize(){
    //Create the channels
    if(Get_Setup()==0 || Get_Setup()==1){
      //cout<<"Init readout"<<endl;
      channel.reserve(geometry->Get_NumberOfStrip1()+geometry->Get_NumberOfStrip2());
      //Strip 1D and 2D case
      for(int ich=0;ich<geometry->Get_NumberOfStrip1();ich++){
        int type=Xview; // X view
        double posX = (ich-0.5*geometry->Get_NumberOfStrip1())*geometry->Get_Pitch1();
        Position *pos = new Position(posX,0,0,0);
        ElectronicChannel* ch = new ElectronicChannel(type,ich,*pos,geometry->Get_Pitch1());
        if(!NO_Readout) channel.push_back(ch);
        delete pos;
      }
      for(int ich=0;ich<geometry->Get_NumberOfStrip2();ich++){
        int type=Yview; // Y view
        double posY = (ich-0.5*geometry->Get_NumberOfStrip2())*geometry->Get_Pitch2();
        Position *pos = new Position(0,posY,0,0);
        ElectronicChannel* ch = new ElectronicChannel(type,ich+geometry->Get_NumberOfStrip1(),*pos,geometry->Get_Pitch2());
        if(!NO_Readout) channel.push_back(ch);
        delete pos;
      }
    }
    else if(Get_Setup()==2){
      channel.reserve(geometry->Get_NumberOfStrip1()+geometry->Get_NumberOfStrip2());
      //Strip 1D
      for(int ich=0;ich<geometry->Get_NumberOfStrip1();ich++){
        int type=Phiview;
        double posPhi = (ich+0.5)/(double)(geometry->Get_NumberOfStrip1())*(TMath::Pi()*2);
        Position *pos = new Position(9, posPhi,0);
        ElectronicChannel* ch = new ElectronicChannel(type,ich,*pos,geometry->Get_Pitch1()); //forse invece di Get_Pitch metterei Pitch_Phi
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
    //Set electronics on channels and threshold
    for(int ich=0;ich<channel.size();ich++){
      channel.at(ich)->Set_Electronics(Get_Electronics());     
    }
    //init
    if(Get_Electronics()==0){
      Initialize_APV();
    }
    if(Get_Electronics()==1){
      for(int ich=0;ich<channel.size();ich++){
        channel.at(ich)->Set_V_thr_T(thrT_TIGER);
        channel.at(ich)->Set_V_thr_E(thrE_TIGER);      
      }
      Initialize_TIGER();
    }
    if(Get_Electronics()==2){
      for(int ich=0;ich<channel.size();ich++){
        channel.at(ich)->Set_V_thr_T(thrT_TORA);   
      }
      Initialize_TORA();
    }

    return;
  }

  vector<ElectronicChannel*> Readout::Read(){
    if(NO_Readout) return channel;
    Simulate_Electronics();
    return channel;
  }

  vector<ElectronicChannel*> Readout::Read(vector<Secondary*> electron){
    //da rimuovere, non si usa piu
    if(NO_Readout) return channel;
    if(electron.empty()) return channel;   
    Define_ChannelID(electron);
    Integrate_Charge();
    //resisto->Resist();
    Simulate_Electronics();
    return channel;
  };

  void Readout::Integrate_Charge(){
    #pragma omp parallel for
    for(int ich=0;ich<channel.size();ich++){
      if(channel.at(ich)->Get_Histo_raw()->GetMaximum()==0) continue;
      double integral = 0;
      for(int it = 0; it < n_ns; it++) {
        int timebin = channel.at(ich)->Get_Histo_raw()->FindBin(it);
        integral += channel.at(ich)->Get_Histo_raw()->GetBinContent(timebin);
        channel.at(ich)->Get_Histo_tot()->SetBinContent(timebin,integral);
      }
    }
    return;
  }

  int Readout::Define_View(){
    if(Get_Setup()==0 || Get_Setup()==1){ //Strips 1D and 2D case
      double charge_sharing = geometry->Get_ChargeSharing();
      if(charge_sharing/(1+charge_sharing) < r->Uniform(0,1)) return Xview;
      else return Yview;
    }
    else if(Get_Setup()==2){
      return Phiview;
    }
    return -1;
  };
  
  void Readout::Define_ChannelID(vector<Secondary*> electron){
    if(electron.empty()) return;
    //Associate the electrons to the correct strip
    for(int iele=0;iele<electron.size();iele++){
      int view_ele = Define_View();
      int ch_id = Get_ChannelID(view_ele,electron.at(iele));
      electron.at(iele)->Set_ElectronicChannel(ch_id);
    }	
    return;	
  };

  void Readout::Induce_on_channel(int type, double position, double time, int weight){
    if(channel.empty()) return;
    float electron_weight = 1;
    if(!NO_Ion_Tail) electron_weight = 1-IT_ratio_fast_slow;
    //manca ion tail sulle Y

    
    if(Get_Setup()==0 || Get_Setup()==1){
      if(type==Xview){
        for(int ich=0;ich<channel.size();ich++){
          if(type==channel.at(ich)->Get_Type()){
            if(abs(position-channel.at(ich)->Get_Position().Get_X())<0.5*geometry->Get_Pitch1()){
              //Ion Tail Induction
              if(!NO_Ion_Tail){
                for(int itime_ion=time;itime_ion<time+IT_Lenght;itime_ion++){
                  if(itime_ion<n_ns){
		                float ion_amp = weight*IT_amplitude*IT_ratio_fast_slow;
                    channel.at(ich)->Fill_Time(itime_ion,ion_amp);
                  }
                }
              }
              //Electron Induction
              float ele_amp = weight*electron_weight;
              channel.at(ich)->Fill_Time(time,ele_amp);
              channel.at(ich)->Add_electrons(weight);
              return;
            }
          }
        }
        cout<<"Electron X outside the readout -> x: "<<position<<" tye: "<<type<<endl;
      }
      else if(type==Yview){
	      for(int ich=0;ich<channel.size();ich++){
          if(type==channel.at(ich)->Get_Type()){
            if(abs(position-channel.at(ich)->Get_Position().Get_Y())<0.5*geometry->Get_Pitch2()){
	            channel.at(ich)->Fill_Time(time,weight);
              channel.at(ich)->Add_electrons(weight);
              return;
            }
          }
        }
	      cout<<"Electron Y outside the readout -> y: "<<position<<" tye: "<<type<<endl;
      }
      else if(type==Phiview){
	      for(int ich=0;ich<channel.size();ich++){
          if(type==channel.at(ich)->Get_Type()){
	          double Phi_Pitch = abs(channel.at(1)->Get_Position().Get_Phi()-channel.at(0)->Get_Position().Get_Phi());
            if(abs(position-channel.at(ich)->Get_Position().Get_Phi())<0.5*Phi_Pitch){
              channel.at(ich)->Fill_Time(time,weight);
              channel.at(ich)->Add_electrons(weight);
              return;
            }
          }
        }
	      cout<<"Electron Phi outside the readout -> Phi: "<<position<<" tye: "<<type<<endl;
      }
    }
    return;
  };


  int Readout::Get_ChannelID(int type, Secondary* ele){
    //dovrebbe essere cosi in modo da unificare le due funzioni
    //int Get_Channelx,y() {
    //  int ch_out;
    //  Induce_on_channel(x,y) -> qui ci metto ch_out = ich;
    //  return ch_out;
    //}

    float electron_weight = 1;
    if(!NO_Ion_Tail) electron_weight = 1-IT_ratio_fast_slow;
    
    if(channel.empty()) return -1;
    for(int ich=0;ich<channel.size();ich++){
      if(type==channel.at(ich)->Get_Type() && type==Xview){
        if(abs(ele->Get_PositionFinal().Get_X()-channel.at(ich)->Get_Position().Get_X())<0.5*geometry->Get_Pitch1()){
          //Ion Tail Induction
          if(!NO_Ion_Tail){
            for(int itime_ion=time;itime_ion<ele->Get_PositionFinal().Get_T()+IT_Lenght;itime_ion++){
              if(itime_ion<n_ns){
                channel.at(ich)->Fill_Time(itime_ion,IT_amplitude*IT_ratio_fast_slow);
              }
            }
          }
          //Electron Induction
          channel.at(ich)->Fill_Time(ele->Get_PositionFinal().Get_T(),electron_weight);
          channel.at(ich)->Add_electrons(1);
          return ich;
        }
      }
      if(type==channel.at(ich)->Get_Type() && type==Yview){
        if(abs(ele->Get_PositionFinal().Get_Y()-channel.at(ich)->Get_Position().Get_Y())<0.5*geometry->Get_Pitch2()){
          channel.at(ich)->Fill_Time(ele->Get_PositionFinal().Get_T(),1);
          channel.at(ich)->Add_electrons(1);
	  return ich;
	}
      }
      if(type==channel.at(ich)->Get_Type() && type==Phiview){
        double Phi_Pitch = abs(channel.at(1)->Get_Position().Get_Phi()-channel.at(0)->Get_Position().Get_Phi());
        if(abs(ele->Get_PositionFinal().Get_Phi()-channel.at(ich)->Get_Position().Get_Phi())<0.5*Phi_Pitch){
          channel.at(ich)->Fill_Time(ele->Get_PositionFinal().Get_T(),1);
          channel.at(ich)->Add_electrons(1);
          return ich;
	}
      }
    }
    return -1;
  };
  
  void Readout::Simulate_Electronics(){
    Injection_External_Signal();
    if(Get_Electronics()==0) Simulate_APV();
    if(Get_Electronics()==1) Simulate_TIGER();
    if(Get_Electronics()==2) Simulate_TORA();
    //Analysis
    Measure_Signal_Lenght();
    return;
  }
  
  void Readout::Simulate_APV(){
    bool print_here = 0;
    if(channel.empty()) return;
    //Background_APV();
    if(print_here) cout<<"Background"<<endl;
    Background();
    //Integration_APV();
    if(print_here) cout<<"Integration"<<endl;
    Integration();
    //Capacitive_Induction();
    if(print_here) cout<<"Extraction"<<endl;
    Extract_Charge_Time();
    if(print_here) cout<<"Stop SimulationExtract_Charge_Time"<<endl;
    return;
  };

  void Readout::Simulate_TIGER(){
    if(channel.empty()) return;
    //Background_TIGER();
    Background();
    //Integration_TIGER();
    Integration();
    //Capacitive_Induction();
    Extract_Charge_Time();
    return;
  };

    void Readout::Simulate_TORA(){
    if(channel.empty()) return;
    //Background_TORA();
    Background();
    //Integration_TORA();
    Integration();
    //Capacitive_Induction();
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

  void Readout::Initialize_TIGER(){
    for(int ich=0;ich<channel.size();ich++){
      for(int jt = 0; jt < n_ns; jt++){
        channel.at(ich)->Get_Histo_tiger_T()->SetBinContent(jt,0);
        channel.at(ich)->Get_Histo_tiger_E()->SetBinContent(jt,0);
      }
    }
    return;
  }

  void Readout::Initialize_TORA(){
    for(int ich=0;ich<channel.size();ich++){
      for(int jt = 0; jt < n_ns; jt++){
        channel.at(ich)->Get_Histo_int_tora()->SetBinContent(jt,0);
        // channel.at(ich)->Get_Histo_tora_E()->SetBinContent(jt,0);
      }
    }
    return;
  }

  /*
  void Readout::Background_APV(){
    if(NO_Noise) return;
    for(int ich=0;ich<channel.size();ich++){
      if(channel.at(ich)->Get_Histo_cur()->GetMaximum()==0) continue;
      //Background - white noise
      if(!NO_Noise){
	const int max_freq = 1e6; //1 GHz
	TRandom3 *r_bkg = new TRandom3();
	r_bkg->SetSeed();
	for(int itime=1;itime<n_ns;itime++){
	  float bkg=0;
	  for(int ifreq=1;ifreq<max_freq;ifreq*=10){
	    bkg+=r_bkg->Gaus()*noise_amplitude*sin(itime*ifreq);
	  }
	  channel.at(ich)->Get_Histo_cur()->AddBinContent(itime, bkg);
	}
	delete r_bkg;
      }
    }
    return;
  }
  */
  
  //void Readout::Background_TIGER(){
  void Readout::Background(){
    if(NO_Noise) return;
    #pragma omp parallel for
    for(int ich=0;ich<channel.size();ich++){
      //if(channel.at(ich)->Get_Histo_cur()->GetMaximum()==0) continue;
      if(!NO_Noise){
        if(noise_1_over_f){
          const int max_freq = 1e6; //1 GHz
          TRandom3 *r_bkg = new TRandom3();
          r_bkg->SetSeed();
          for(int itime=1;itime<n_ns+n_ns_buffer;itime++){
            float bkg=0;
            for(int ifreq=1;ifreq<max_freq;ifreq*=10){
              bkg+=r_bkg->Gaus()*noise_amplitude*sin(itime*ifreq);
            }
            channel.at(ich)->Get_Histo_cur_buffer()->AddBinContent(itime, bkg);
          }
          delete r_bkg;
        }
        if(white_noise){
          TRandom3 *r_bkg = new TRandom3();
                r_bkg->SetSeed();
          const int n_freq = 50;
          float freq_amplitude[n_freq];
          float freq_value[n_freq];
          float freq_phase[n_freq];
          for(int i=0;i<n_freq;i++){
            freq_amplitude[i]=r->Gaus()*noise_amplitude;
            freq_value[i]=r->Uniform(min_freq_noise,max_freq_noise);
            freq_phase[i]=r->Uniform(0,6.283);
          }
          for(int itime=1;itime<n_ns+n_ns_buffer;itime++){
                  float bkg=0;
            for(int i=0;i<n_freq;i++){
              bkg+=freq_amplitude[i]*sin(6.283*itime*freq_value[i]+freq_phase[i]);
            }
            channel.at(ich)->Get_Histo_cur_buffer()->AddBinContent(itime, bkg);
          }
        }
      }
    }
  }
  
  /*
  void Readout::Integration_APV(){
    for(int ich=0;ich<channel.size();ich++){
      if(channel.at(ich)->Get_Histo_cur()->GetMaximum()==0) continue;
      for(int it = 1; it < n_ns; it++) {
        double integral = 0;
        integral = channel.at(ich)->Get_Histo_cur()->GetBinContent(it);
        for(int jt = it; jt < n_ns; jt++){
	  channel.at(ich)->Get_Histo_int_apv()->AddBinContent(jt,integral*APV_shaper(jt-it));
	}
      }
      double jitter = 0;
      for(int iapv = 0; iapv < timebin_APV; iapv++) {
	double t = jitter + iapv * timestep_APV;
	double timebin = channel.at(ich)->Get_Histo_int_apv()->FindBin(t);
	double qmeas = channel.at(ich)->Get_Histo_int_apv()->GetBinContent(timebin)*fC_to_ADC;
	channel.at(ich)->Get_Histo_apv()->AddBinContent(iapv+1, qmeas);
      }	
    }
    return;
  }
  */

  void Readout::Integration(){
    #pragma omp parallel for
    for(int ich=0;ich<channel.size();ich++){
      //Copy the buffer on the final one
      for(int it = n_ns_buffer+1; it < n_ns+n_ns_buffer; it++) channel.at(ich)->Get_Histo_cur()->AddBinContent(it-n_ns_buffer,channel.at(ich)->Get_Histo_cur_buffer()->GetBinContent(it));
      //Shaper
      if(channel.at(ich)->Get_Histo_cur_buffer()->GetMaximum()==0) continue;
      for(int it = 1; it < n_ns+n_ns_buffer; it++) {
        for(int jt = it; jt < n_ns+n_ns_buffer; jt++){
          if(jt>n_ns_buffer){
            if(Get_Electronics()==0){
              channel.at(ich)->Get_Histo_int_apv()->AddBinContent(jt-n_ns_buffer,channel.at(ich)->Get_Histo_cur_buffer()->GetBinContent(it)*APV_shaper(jt-it));
            }
            if(Get_Electronics()==1){
              channel.at(ich)->Get_Histo_tiger_T()->AddBinContent(jt-n_ns_buffer,channel.at(ich)->Get_Histo_cur_buffer()->GetBinContent(it)*T_branch(jt-it));
              channel.at(ich)->Get_Histo_tiger_E()->AddBinContent(jt-n_ns_buffer,channel.at(ich)->Get_Histo_cur_buffer()->GetBinContent(it)*E_branch(jt-it));
            }
            if(Get_Electronics()==2){
              channel.at(ich)->Get_Histo_int_tora()->AddBinContent(jt-n_ns_buffer,channel.at(ich)->Get_Histo_cur_buffer()->GetBinContent(it)*TORA_shaper(jt-it));
              // channel.at(ich)->Get_Histo_tiger_E()->AddBinContent(jt-n_ns_buffer,channel.at(ich)->Get_Histo_cur_buffer()->GetBinContent(it)*E_branch(jt-it));
            }
          }
        }
      }
      //Discetizing in APV time-bin (25ns)
      if(Get_Electronics()==0){
        double jitter = 0;
        for(int iapv = 0; iapv < timebin_APV; iapv++) {
          double t = jitter + iapv * timestep_APV;
          double timebin = channel.at(ich)->Get_Histo_int_apv()->FindBin(t);
          double qmeas = channel.at(ich)->Get_Histo_int_apv()->GetBinContent(timebin)*fC_to_ADC;
          channel.at(ich)->Get_Histo_apv()->AddBinContent(iapv+1, qmeas);
        } 
      }
    }
  }
 
  void Readout::Injection_External_Signal(){
    //Inject a sware wave on all the channels
    //t0 = 1/10 time window
    int t0_ns = n_ns/10;
    //Amplitude and Lenght defined in Common/Common.h
    if(Square_Wave){
      for(int ich=0;ich<channel.size();ich++){
        if(ich==channel.size()/2){
          for(int i_ns=0;i_ns<SW_Lenght;i_ns++){
            channel.at(ich)->Get_Histo_cur_buffer()->AddBinContent(i_ns+n_ns_buffer+t0_ns,SW_Amplitude);
          }
        }
      }
    }
    return;
  }
  
  void Readout::Capacitive_Induction(){
    if(NO_Capacitive) return;
    if(Get_Setup()==0){
      for(int ich=0;ich<channel.size();ich++){
        double q_peak = channel.at(ich)->Get_Histo_apv()->GetMaximum(); // fC
        double q_peak_adc = q_peak * fC_to_ADC; // ADC  
        //cout<<ich<<" "<<q_peak_adc<<endl;
        if (Capacitive_Induction_1(q_peak_adc) &&
            ich!=0 &&
            ich!=channel.size()-1
            ){
          for(int itime = 0; itime < timebin_APV; itime++) {
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
    bool print_here = true;
    //Warning in <Fit>: Fit data is empty da qui dentro
    //#pragma omp parallel for
    for(int ich=0;ich<channel.size();ich++){

      if(Get_Electronics()==0){ //APV
        //Set the channel charge
        channel.at(ich)->Set_Charge(Get_Charge_APV(channel.at(ich)));
        //Set the channel time 
        channel.at(ich)->Set_Time(Get_Time_APV(channel.at(ich)));
        channel.at(ich)->Set_dTime(Get_dTime_APV(channel.at(ich)));
        float SumQ=0;
        for(int ii=0;ii<channel.at(ich)->Get_Histo_apv()->GetNbinsX();ii++) SumQ+=channel.at(ich)->Get_Histo_apv()->GetBinContent(ii);
        //for(int ii=0;ii<channel.at(ich)->Get_Histo_apv()->GetNbinsX();ii++) cout<<ii<<" "<<channel.at(ich)->Get_Histo_apv()->GetBinContent(ii)<<endl;
        //cout<<"Ch: "<<ich<<" SumQ/Nbin "<<(SumQ/timestep_APV)<<" > 0.8*sigma_noise*fC_to_ADC = "<<(0.8*sigma_noise_fC*30)<<" is good: "<<(bool)(SumQ/timestep_APV>0.8*sigma_noise_fC*30)<<endl;
        //if(channel.at(ich)->Get_Charge()>thr_APV) channel.at(ich)->Set_AboveThr(true);
        if(SumQ/timestep_APV>mmdaq_thr_factor*mmdaq_ped*fC_to_ADC && channel.at(ich)->Get_Charge()>threshold_fC) channel.at(ich)->Set_AboveThr(true);
        //else if(channel.at(ich)->Get_Charge()<=0) channel.at(ich)->Set_AboveThr(false);
        else channel.at(ich)->Set_AboveThr(false);
        //Save the apv distribution Qt hit
        if(channel.at(ich)->Get_AboveThr()){
          int hit_q_min = 0;
          int hit_q_step = 10;
          int hit_q_max = 80;
          for(int iq=0; iq<hit_q_max/hit_q_step; iq++){
            int hit_q_step_i = hit_q_min+iq*hit_q_step;
            int hit_q_step_ip = hit_q_min+(iq+1)*hit_q_step;
            if(channel.at(ich)->Get_Charge()>hit_q_step_i && channel.at(ich)->Get_Charge()<hit_q_step_ip && _histo_hit_Qt_APV.size()){
              for(int ii=0;ii<channel.at(ich)->Get_Histo_apv()->GetNbinsX();ii++){
                _histo_hit_Qt_APV.at(iq)->AddBinContent(ii+1,channel.at(ich)->Get_Histo_apv()->GetBinContent(ii+1));
              }
            }
          }
        }
      
      }
      if(Get_Electronics()==1){ //TIGER
        //Set the channel charge
        channel.at(ich)->Set_Charge(Get_Charge_TIGER(channel.at(ich)));
        //Set the channel time
        channel.at(ich)->Set_Time(Get_Time_TIGER(channel.at(ich)));
        channel.at(ich)->Set_dTime(Get_dTime_TIGER(channel.at(ich)));
        //Set threshold flag
        if(channel.at(ich)->Get_AboveThr_E() * channel.at(ich)->Get_AboveThr_T()) channel.at(ich)->Set_AboveThr(true);
        else channel.at(ich)->Set_AboveThr(false);
        if(channel.at(ich)->Get_AboveThr() && (channel.at(ich)->Get_Charge()==0 || channel.at(ich)->Get_Time()==0)) cout<<"ich: "<<ich<<" Q: "<<channel.at(ich)->Get_Charge()<<" T: "<<channel.at(ich)->Get_Time()<<endl;
      }
      if(Get_Electronics()==2){ //TORA
        //Set the channel charge
        channel.at(ich)->Set_Charge(Get_Charge_TORA(channel.at(ich)));
        //Set the channel time
        channel.at(ich)->Set_Time(Get_Time_TORA(channel.at(ich)));
        channel.at(ich)->Set_dTime(Get_dTime_TORA(channel.at(ich)));
        //Set threshold flag
        if(channel.at(ich)->Get_AboveThr_T()) channel.at(ich)->Set_AboveThr(true);
        else channel.at(ich)->Set_AboveThr(false);
        if(channel.at(ich)->Get_AboveThr() && (channel.at(ich)->Get_Charge()==0 || channel.at(ich)->Get_Time()==0)) cout<<"ich: "<<ich<<" Q: "<<channel.at(ich)->Get_Charge()<<" T: "<<channel.at(ich)->Get_Time()<<endl;
      }
    }
    return;
  }
  
  double Readout::Get_Charge_APV(ElectronicChannel *ch){
    //Saturation
    double q_peak = 0;
    if(ch->Get_Histo_apv()->GetMaximumBin()==1) {
      return 0;
    }
    q_peak = ch->Get_Histo_apv()->GetMaximum(); // ADC
    if(q_peak > ch->Get_Saturation_APV() && !NO_Saturation) {
      //Change the saturation level for each channel and each run
      ch->Set_Saturation_APV(r->Gaus(saturation_APV,sigma_saturation_APV));
      q_peak  = ch->Get_Saturation_APV();
      for(int ibin=1; ibin<=timebin_APV; ibin++) {
	      if(ch->Get_Histo_apv()->GetBinContent(ibin) > ch->Get_Saturation_APV()) ch->Get_Histo_apv()->SetBinContent(ibin, q_peak);
      }
    }
    return (ch->Get_Histo_apv()->GetBinContent(ch->Get_Histo_apv()->GetMaximumBin())/fC_to_ADC);
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
    if(h_time->GetMaximumBin()==1 || h_time->GetMaximumBin()==0 || h_time->GetBinContent(h_time->GetMaximumBin())<=1) {
      Set_Time(-1);
      Set_dTime(-1);
      return;
    }
    double minbin = 0;
    double maxbin    = h_time->GetMaximumBin();
    double QMaxHisto = h_time->GetBinContent(h_time->GetMaximumBin());
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
    fitlimlow[0] = startvalues[0] - 5 * sigma_MFB;
    fitlimlow[1] = startvalues[1] - 0.3 * startvalues[1];
    fitlimlow[2] = minbin * timestep_APV;
    fitlimlow[3] = 0.1*timestep_APV;
    fitlimup[0] = startvalues[0] + 5 * sigma_MFB;
    fitlimup[1] = startvalues[1] + 0.1 * startvalues[1];
    fitlimup[2] = maxbin * timestep_APV;
    fitlimup[3] = 0.9*timestep_APV;
    //update for uRtube
    startvalues[0]  =    0;
    fitlimlow[0]    = -0.1;
    fitlimup[0]     =  0.1;
    // the real Fermi Dirac
    double minFD = minbin - 20;//4;
    double maxFD = maxbin +  3;//0;
    minFD*=timestep_APV;
    maxFD*=timestep_APV;
    TF1 *f_FD = new TF1("f_FD", "[0] + [1]/(1+TMath::Exp(-(x - [2])/[3]))", minFD, maxFD);
    f_FD->SetParameters(startvalues[0], startvalues[1], startvalues[2], startvalues[3]);
    f_FD->SetParLimits(0, fitlimlow[0], fitlimup[0]);
    f_FD->SetParLimits(1, fitlimlow[1], fitlimup[1]);
    f_FD->SetParLimits(2, fitlimlow[2], fitlimup[2]);
    f_FD->SetParLimits(3, fitlimlow[3], fitlimup[3]);
    if(h_time->GetBinContent(h_time->GetMaximumBin())>0){
      h_time->Fit("f_FD","QWRBM");  // quiet/range/params // CHECK added if()
    }
    double tFD = 0;
    tFD = r->Gaus(f_FD->GetParameter(2), 7.12); //QUESTO È JITTER MA NO VA QUI? O SI? |!!!!! ODVREBBE ESSERE LO STESSO PER TUTTI I CANALI. FORSE VA TOLTO
    double dtFD = 0.;
    dtFD = f_FD->GetParError(2);
    //f_FD->~TF1();
    delete f_FD;
    Set_Time(tFD);
    Set_dTime(dtFD);
    return;
  }

  double Readout::Get_Charge_TIGER(ElectronicChannel *ch){
    TH1D *h_time = ch->Get_Histo_tiger_E();
    double maxbin    = h_time->GetNbinsX();
    float time_thr = -999; // leading edge
    float time_falling = -999; // falling edge
    ch->Set_AboveThr_E(false);
    for(int i=1;i<maxbin-1;i++) {
      if(h_time->GetBinContent(i)>thrE_TIGER) {
        time_thr = i;
        break;
      }
    }
    for(int i=time_thr;i<maxbin;i++) {
      if(i==maxbin-1) time_falling=2*maxbin;
      if(h_time->GetBinContent(i)<thrE_TIGER) {
        time_falling = i-1;
        break;
      }
    }
    ch->Set_t_thr_E(time_thr);
    ch->Set_t_rising_E(time_thr);
    ch->Set_t_falling_E(time_falling);
    //Saturation
    ch->Set_Saturation_TIGER(r->Gaus(saturation_TIGER,sigma_saturation_TIGER));
    float q_peak  = ch->Get_Saturation_TIGER();
    if(!NO_Saturation){
      for(int i=1;i<maxbin;i++) {
	      if(h_time->GetBinContent(i)>saturation_TIGER) h_time->SetBinContent(i,q_peak);
      }
    }
    time_thr = ((int)(time_thr/timestep_TIGER))*timestep_TIGER;
    float time_peak = time_thr + 4*6.25*(integration_time_TIGER+0.5);
    if(time_thr<0 || time_peak>=maxbin) return -1;
    ch->Set_AboveThr_E(true);
    time_peak = ((int)(time_peak/timestep_TIGER))*timestep_TIGER;
    ch->Set_t_Q_E(time_peak);
    float charge = h_time->GetBinContent((int)time_peak)/gain_TIGER;
    if(TIGER_Get_Maximum) charge = h_time->GetMaximum()/gain_TIGER;
    return charge;
  }

  double Readout::Get_Time_TIGER(ElectronicChannel *ch){
    TH1D *h_time = ch->Get_Histo_tiger_T();
    double maxbin    = h_time->GetNbinsX();
    float time_thr      = -1;  // leading edge
    float time_falling  = -999; // falling edge
    float efine_time_thr=0;
    ch->Set_AboveThr_T(false);
    for(int i=1;i<maxbin-1;i++) {
      if(h_time->GetBinContent(i)>thrT_TIGER) {
        time_thr = i;
        ch->Set_AboveThr_T(true);
        break;
      }
    }
    for(int i=time_thr+1;i<maxbin;i++) {
      if(i==maxbin-1) time_falling=2*maxbin;
      if(h_time->GetBinContent(i)<thrT_TIGER) {
        time_falling = i-1;
	      break;
      }
    }
    ch->Set_t_rising_T(time_thr);
    ch->Set_t_falling_T(time_falling);
    if(enable_tfine_tiger){
      if(time_thr!=0){
	      efine_time_thr = 1 - (thrT_TIGER-h_time->GetBinContent(time_thr-1))/(h_time->GetBinContent(time_thr)-h_time->GetBinContent(time_thr-1));
      }
    }
    //Saturation
    float q_peak  = ch->Get_Saturation_TIGER();
    if(!NO_Saturation){
      for(int i=1;i<maxbin;i++) {
        if(h_time->GetBinContent(i)>saturation_TIGER) h_time->SetBinContent(i,q_peak);
      }
    }
    if(time_thr<0) return time_thr;
    //digitize the time in timestep 6.25ns
    time_thr = ((int)(1+time_thr/timestep_TIGER))*timestep_TIGER - efine_time_thr*timestep_TIGER;
    return time_thr;
  }

  double Readout::Get_dTime_TIGER(ElectronicChannel *ch){
    return timestep_TIGER;
  }

  double Readout::Get_Charge_TORA(ElectronicChannel *ch){
    TH1D *h_time = ch->Get_Histo_int_tora();
  //   double maxbin    = h_time->GetNbinsX();
  //   float time_thr = -999; // leading edge
  //   float time_falling = -999; // falling edge
  //   ch->Set_AboveThr_E(false);
  //   for(int i=1;i<maxbin-1;i++) {
  //     if(h_time->GetBinContent(i)>thrE_TIGER) {
  //       time_thr = i;
  //       break;
  //     }
  //   }
  //   for(int i=time_thr;i<maxbin;i++) {
  //     if(i==maxbin-1) time_falling=2*maxbin;
  //     if(h_time->GetBinContent(i)<thrE_TIGER) {
  //       time_falling = i-1;
  //       break;
  //     }
  //   }
  //   ch->Set_t_thr_E(time_thr);
  //   ch->Set_t_rising_E(time_thr);
  //   ch->Set_t_falling_E(time_falling);
  //   //Saturation
  //   ch->Set_Saturation_TIGER(r->Gaus(saturation_TIGER,sigma_saturation_TIGER));
  //   float q_peak  = ch->Get_Saturation_TIGER();
  //   if(!NO_Saturation){
  //     for(int i=1;i<maxbin;i++) {
	//       if(h_time->GetBinContent(i)>saturation_TIGER) h_time->SetBinContent(i,q_peak);
  //     }
  //   }
  //   time_thr = ((int)(time_thr/timestep_TIGER))*timestep_TIGER;
  //   float time_peak = time_thr + 4*6.25*(integration_time_TIGER+0.5);
  //   if(time_thr<0 || time_peak>=maxbin) return -1;
  //   ch->Set_AboveThr_E(true);
  //   time_peak = ((int)(time_peak/timestep_TIGER))*timestep_TIGER;
  //   ch->Set_t_Q_E(time_peak);
  //   float charge = h_time->GetBinContent((int)time_peak)/gain_TORA;
  //   if(TIGER_Get_Maximum) charge = h_time->GetMaximum()/gain_TORA;
    float charge = h_time->GetMaximum()/gain_TORA;
    return charge;
  }

  double Readout::Get_Time_TORA(ElectronicChannel *ch){
    TH1D *h_time = ch->Get_Histo_int_tora();
    double maxbin    = h_time->GetNbinsX();
    float time_thr      = -1;  // leading edge
    float time_falling  = -999; // falling edge
    float efine_time_thr=0;
    ch->Set_AboveThr_T(false);
    for(int i=1;i<maxbin-1;i++) {
      if(h_time->GetBinContent(i)>thrT_TORA) {
        time_thr = i;
        ch->Set_AboveThr_T(true);
        break;
      }
    }
    for(int i=time_thr+1;i<maxbin;i++) {
      if(i==maxbin-1) time_falling=2*maxbin;
      if(h_time->GetBinContent(i)<thrT_TORA) {
        time_falling = i-1;
	      break;
      }
    }
    ch->Set_t_rising_T(time_thr);
    ch->Set_t_falling_T(time_falling);

    // TFine
    // if(enable_tfine_tiger){
    //   if(time_thr!=0){
	  //     efine_time_thr = 1 - (thrT_TORA-h_time->GetBinContent(time_thr-1))/(h_time->GetBinContent(time_thr)-h_time->GetBinContent(time_thr-1));
    //   }
    // }

    // Saturation
    // float q_peak  = ch->Get_Saturation_TORA();
    // if(!NO_Saturation){
    //   for(int i=1;i<maxbin;i++) {
    //     if(h_time->GetBinContent(i)>saturation_TORA) h_time->SetBinContent(i,q_peak);
    //   }
    // }
    if(time_thr<0) return time_thr;
    //digitize the time in timestep 6.25ns
    time_thr = ((int)(1+time_thr/timestep_TORA))*timestep_TORA - efine_time_thr*timestep_TORA;
    return time_thr;
  }

  double Readout::Get_dTime_TORA(ElectronicChannel *ch){
    return timestep_TORA;
  }


  void Readout::Measure_Signal_Lenght(){
    for(int ich=0;ich<channel.size();ich++){
      double t_first = -999;
      double t_last = 999;
      if(channel.at(ich)->Get_Histo_raw()->GetMaximum()==0) {
        channel.at(ich)->Set_t_rising_raw(-999);
        channel.at(ich)->Set_t_falling_raw(-999);
        continue;
      }
      for(int it = 0; it < n_ns; it++) {
        int timebin = channel.at(ich)->Get_Histo_raw()->FindBin(it);
        float qt = channel.at(ich)->Get_Histo_raw()->GetBinContent(timebin);
        if(qt && t_first<0) t_first = it;
        if(qt) t_last=it;
      }
      channel.at(ich)->Set_t_rising_raw(t_first);
      channel.at(ich)->Set_t_falling_raw(t_last);
    }
  }

}


