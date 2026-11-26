#include <cstdint>
#include <omp.h>
#include "Readout/Readout.h"
namespace PARSIFAL2{
  Readout::Readout(int setup, int electronics, Geometry *geo):
    PrintInfo(true),
    PrintNTuple(true),
    Setup(setup),
    Electronics(electronics),
    geometry(geo)
  {
    r = new TRandom3(SEED ? SEED + 404 : 0);
    sigma_noise_fC = 2.0; // 2.0 fC
    threshold_fC   = 2.0; // 2.0 fC
    mmdaq_ped      = 1.0;
    mmdaq_thr_factor = 0.8;
    thrT_TIGER     = 10;  // mV
    thrE_TIGER     = 10;  // mV
    TIGER_Get_Maximum = false;

    gain_TORA      = 12.7; // mV/fC
    tau_TORA       = 50.0; // ns
    thrRise_TORA   = 10;   // mV
    thrTrail_TORA  = 0;    // mV

    IT_Lenght          = 170; // ns
    IT_amplitude       = 0.00588; // fC/ns = 1/IT_Length
    IT_ratio_fast_slow = 0.85; // 85% of the signal comes from ions and 15% from electrons

    FFT_fmin = 0;
    FFT_fmax = 1e9;

    Initialize();

    if(Get_PrintInfo()){
      cout<<"----    Readout    ----"<<endl;
      cout<<"Readout configuration : "<<Get_Setup()<<endl;
      cout<<"Readout electronics   : "<<Get_Electronics()<<endl;
      cout<<"----------------------"<<endl; 
    }
  };

  Readout::~Readout(){
    delete r;
    for(auto* ch : channel) delete ch;
    channel.clear();
    if(Get_Electronics()==0){
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
        noise_amplitude = sqrt(pow(sigma_noise_fC,2) + pow(e_noise_amplitude_APV,2)) / slope;
      }
      if(Get_Electronics()==1){
        //(noise_amplitude = 0.005; // TIGER: E-branch
        //float slope = 76.75;
        //2025.08.28
        //float slope = 27.73;
        //2025.10.15
        float slope = 43.48;
        noise_amplitude = sqrt(pow(sigma_noise_fC,2) + pow(e_noise_amplitude_TIGER,2)) / slope;
      }
      if(Get_Electronics()==2){
        // float slope = 43.48; // temporary value : to be tuned
        float slope = (139.521-10064.3/(tau_TORA+77.102)) * 43.48/59.0906; //tau_TORA = shaping time 
        noise_amplitude = sigma_noise_fC/slope; // TORA: STD sigma noise = slope * max amplitude input noise current  
        //noise_amplitude = sqrt(pow(sigma_noise_fC,2) + pow(e_noise_amplitude_TORA,2)) / slope;  
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
    // Set electronics on channels and threshold
    for(size_t ich=0;ich<channel.size();ich++){
      channel.at(ich)->Set_Electronics(Get_Electronics());     
      channel.at(ich)->Set_V_thr_T(thrT_TIGER);
      channel.at(ich)->Set_V_thr_E(thrE_TIGER);  
    }
    // init
    if(Get_Electronics()==0){
      Initialize_APV();
    }
    if(Get_Electronics()==1){
      for(size_t ich=0;ich<channel.size();ich++){
        channel.at(ich)->Set_V_thr_T(thrT_TIGER);
        channel.at(ich)->Set_V_thr_E(thrE_TIGER);      
      }
      Initialize_TIGER();
    }
    if(Get_Electronics()==2){
      for(size_t ich=0;ich<channel.size();ich++){
        channel.at(ich)->Set_V_thr_T(thrRise_TORA);   
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
    const size_t n_ch = channel.size();
    #pragma omp parallel for
    for(size_t ich = 0; ich < n_ch; ich++){
      Waveform* h_raw = channel[ich]->Get_Histo_raw();
      if(h_raw->GetMaximum() == 0) continue;
      const double* raw = h_raw->GetArray();
      double* tot = channel[ich]->Get_Histo_tot()->GetArray();
      double integral = 0;
      for(int it = 1; it <= n_ns; it++) {
        integral += raw[it];
        tot[it] = integral;
      }
    }
    return;
  }

  int Readout::Define_View(){
    if(Get_Setup()==0 || Get_Setup()==1){ // Strips 1D and 2D case
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
    // Associate electrons to the matching readout strip
    for(size_t iele=0;iele<electron.size();iele++){
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

    if(Get_Setup()==0 || Get_Setup()==1){
      if(type==Xview){
        for(size_t ich=0;ich<channel.size();ich++){
          if(type==channel.at(ich)->Get_Type()){
            if(abs(position-channel.at(ich)->Get_Position().Get_X())<0.5*geometry->Get_Pitch1()){
              // Ion tail induction
              if(!NO_Ion_Tail){
                const float ion_amp = weight*IT_amplitude*IT_ratio_fast_slow;
                for(int itime_ion=time;itime_ion<time+IT_Lenght;itime_ion++){
                  if(itime_ion<n_ns){
                    channel.at(ich)->Fill_Time(itime_ion,ion_amp);
                  }
                }
              }
              // Electron induction
              float ele_amp = weight*electron_weight;
              channel.at(ich)->Fill_Time(time,ele_amp);
              channel.at(ich)->Add_electrons(weight);
              return;
            }
          }
        }
        cout<<"Electron X outside the readout -> x: "<<position<<" type: "<<type<<endl;
      }
      else if(type==Yview){
        for(size_t ich=0;ich<channel.size();ich++){
          if(type==channel.at(ich)->Get_Type()){
            if(abs(position-channel.at(ich)->Get_Position().Get_Y())<0.5*geometry->Get_Pitch2()){
              // Ion tail induction (consistent with Xview)
              if(!NO_Ion_Tail){
                const float ion_amp = weight*IT_amplitude*IT_ratio_fast_slow;
                for(int itime_ion=time;itime_ion<time+IT_Lenght;itime_ion++){
                  if(itime_ion<n_ns){
                    channel.at(ich)->Fill_Time(itime_ion,ion_amp);
                  }
                }
              }
              float ele_amp = weight*electron_weight;
              channel.at(ich)->Fill_Time(time,ele_amp);
              channel.at(ich)->Add_electrons(weight);
              return;
            }
          }
        }
        cout<<"Electron Y outside the readout -> y: "<<position<<" type: "<<type<<endl;
      }
    }
    else if(Get_Setup()==2 && type==Phiview){
      for(size_t ich=0;ich<channel.size();ich++){
        if(type==channel.at(ich)->Get_Type()){
          double Phi_Pitch = abs(channel.at(1)->Get_Position().Get_Phi()-channel.at(0)->Get_Position().Get_Phi());
          if(abs(position-channel.at(ich)->Get_Position().Get_Phi())<0.5*Phi_Pitch){
            channel.at(ich)->Fill_Time(time,weight);
            channel.at(ich)->Add_electrons(weight);
            return;
          }
        }
      }
      cout<<"Electron Phi outside the readout -> Phi: "<<position<<" type: "<<type<<endl;
    }
    return;
  };


  int Readout::Get_ChannelID(int type, Secondary* ele){
    float electron_weight = 1;
    if(!NO_Ion_Tail) electron_weight = 1-IT_ratio_fast_slow;
    
    if(channel.empty()) return -1;
    const double t_ele = ele->Get_PositionFinal().Get_T();
    for(size_t ich=0;ich<channel.size();ich++){
      if(type==channel.at(ich)->Get_Type() && type==Xview){
        if(abs(ele->Get_PositionFinal().Get_X()-channel.at(ich)->Get_Position().Get_X())<0.5*geometry->Get_Pitch1()){
          // Ion tail induction
          if(!NO_Ion_Tail){
            for(int itime_ion=t_ele;itime_ion<t_ele+IT_Lenght;itime_ion++){
              if(itime_ion<n_ns){
                channel.at(ich)->Fill_Time(itime_ion,IT_amplitude*IT_ratio_fast_slow);
              }
            }
          }
          // Electron induction
          channel.at(ich)->Fill_Time(t_ele,electron_weight);
          channel.at(ich)->Add_electrons(1);
          return static_cast<int>(ich);
        }
      }
      if(type==channel.at(ich)->Get_Type() && type==Yview){
        if(abs(ele->Get_PositionFinal().Get_Y()-channel.at(ich)->Get_Position().Get_Y())<0.5*geometry->Get_Pitch2()){
          if(!NO_Ion_Tail){
            for(int itime_ion=t_ele;itime_ion<t_ele+IT_Lenght;itime_ion++){
              if(itime_ion<n_ns){
                channel.at(ich)->Fill_Time(itime_ion,IT_amplitude*IT_ratio_fast_slow);
              }
            }
          }
          channel.at(ich)->Fill_Time(t_ele,electron_weight);
          channel.at(ich)->Add_electrons(1);
          return static_cast<int>(ich);
        }
      }
      if(type==channel.at(ich)->Get_Type() && type==Phiview){
        double Phi_Pitch = abs(channel.at(1)->Get_Position().Get_Phi()-channel.at(0)->Get_Position().Get_Phi());
        if(abs(ele->Get_PositionFinal().Get_Phi()-channel.at(ich)->Get_Position().Get_Phi())<0.5*Phi_Pitch){
          channel.at(ich)->Fill_Time(t_ele,1);
          channel.at(ich)->Add_electrons(1);
          return static_cast<int>(ich);
        }
      }
    }
    return -1;
  };
  
  void Readout::Simulate_Electronics(){
    Injection_External_Signal();
    Background();
    Filtering();
    Integration();
    Extract_Charge_Time();
    //if(Get_Electronics()==0) Simulate_APV();
    //if(Get_Electronics()==1) Simulate_TIGER();
    //if(Get_Electronics()==2) Simulate_TORA();

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
    // Experimentally measured capacitive induction parametrizations
    r_cap = new TRandom3(SEED ? SEED + 405 : 0);
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
    for(size_t ich=0;ich<channel.size();ich++){
      channel.at(ich)->Get_Histo_tiger_T()->Reset();
      channel.at(ich)->Get_Histo_tiger_E()->Reset();
    }
    return;
  }

  void Readout::Initialize_TORA(){
    for(size_t ich=0;ich<channel.size();ich++){
      channel.at(ich)->Get_Histo_int_tora()->Reset();
      channel.at(ich)->Set_Gain_TORA(gain_TORA);
    }
    return;
  }

  void Readout::Update_param_TORA(){
    gain_TORA  =  Get_gain_TORA();     
    thrRise_TORA  =  Get_thrRise_TORA();
    thrTrail_TORA  =  Get_thrTrail_TORA();
    Set_TORA_thr_rise_mV(thrRise_TORA);
    Set_TORA_thr_trail_mV(thrTrail_TORA);

    for(size_t ich=0;ich<channel.size();ich++){
      channel.at(ich)->Set_Gain_TORA(gain_TORA);
    }
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
    const size_t n_ch = channel.size();
    const int total_ns = n_ns + n_ns_buffer;
    // Generate per-channel seeds sequentially to avoid OpenMP data races on the shared RNG
    std::vector<UInt_t> seeds(n_ch);
    for(size_t ich = 0; ich < n_ch; ich++) seeds[ich] = r->Integer(UINT32_MAX);

    #pragma omp parallel for
    for(size_t ich = 0; ich < n_ch; ich++){
      TRandom3 r_local(seeds[ich]);
      double* __restrict__ buf = channel[ich]->Get_Histo_cur_buffer()->GetArray();
      if(noise_1_over_f){
        const int max_freq = 1e6; // 1 GHz
        for(int itime = 1; itime <= total_ns; itime++){
          double bkg = 0.0;
          for(int ifreq = 1; ifreq < max_freq; ifreq *= 10){
            bkg += r_local.Gaus() * noise_amplitude * std::sin(itime * ifreq);
          }
          buf[itime] += bkg;
        }
      }
      if(white_noise){
        const int n_freq = 50;
        // Use exact trigonometric angle-addition recurrence to eliminate sin() calls in the time loop
        for(int i = 0; i < n_freq; i++){
          const double amp   = r_local.Gaus() * noise_amplitude;
          const double omega = TMath::TwoPi() * r_local.Uniform(min_freq_noise, max_freq_noise);
          const double phase = r_local.Uniform(0, TMath::TwoPi());
          const double cw = std::cos(omega);
          const double sw = std::sin(omega);
          double s = std::sin(phase);
          double c = std::cos(phase);
          for(int itime = 1; itime <= total_ns; itime++){
            const double s_next = s * cw + c * sw;
            const double c_next = c * cw - s * sw;
            s = s_next;
            c = c_next;
            buf[itime] += amp * s;
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
    const int total_ns = n_ns + n_ns_buffer;
    const int elec = Get_Electronics();

    // Helper to find effective truncated kernel length where tail drops below 1e-6 of peak
    auto compute_klen = [&](const std::vector<double>& k) {
      double k_max = 0.0;
      for(double v : k) k_max = std::max(k_max, std::fabs(v));
      if(k_max == 0.0) return 0;
      const double eps = 1e-6 * k_max;
      int last = 0;
      for(int dt = 0; dt < static_cast<int>(k.size()); dt++){
        if(std::fabs(k[dt]) > eps) last = dt;
      }
      return last + 1;
    };

    // Precompute shaper impulse-response lookup tables once per call
    std::vector<double> kernel_0;
    std::vector<double> kernel_1;
    int klen_0 = 0;
    int klen_1 = 0;
    if(elec == 0){
      kernel_0.resize(total_ns);
      for(int dt = 0; dt < total_ns; dt++) kernel_0[dt] = APV_shaper(dt);
      klen_0 = compute_klen(kernel_0);
    } else if(elec == 1){
      kernel_0.resize(total_ns);
      kernel_1.resize(total_ns);
      for(int dt = 0; dt < total_ns; dt++){
        kernel_0[dt] = T_branch(dt);
        kernel_1[dt] = E_branch(dt);
      }
      klen_0 = compute_klen(kernel_0);
      klen_1 = compute_klen(kernel_1);
    } else if(elec == 2){
      kernel_0.resize(total_ns);
      for(int dt = 0; dt < total_ns; dt++){
        kernel_0[dt] = (TORA_shaper_order == 3) ? TORA_shaper_3ord(dt) : TORA_shaper(dt);
      }
      klen_0 = compute_klen(kernel_0);
    }

    const size_t n_ch = channel.size();
    #pragma omp parallel for
    for(size_t ich = 0; ich < n_ch; ich++){
      ElectronicChannel* ch = channel[ich];
      const double* __restrict__ cur_buf     = ch->Get_Histo_cur_buffer()->GetArray();
      const double* __restrict__ cur_buf_fft = ch->Get_Histo_cur_buffer_FFT()->GetArray();
      double* __restrict__ cur_out     = ch->Get_Histo_cur()->GetArray();
      double* __restrict__ cur_fft_out = ch->Get_Histo_cur_FFT()->GetArray();

      // Copy post-buffer window into output current waveforms
      for(int it = n_ns_buffer + 1; it < total_ns; it++) {
        cur_out[it - n_ns_buffer]     += cur_buf[it];
        cur_fft_out[it - n_ns_buffer] += cur_buf_fft[it];
      }

      // Skip channels with zero signal
      if(ch->Get_Histo_cur_buffer_FFT()->GetMaximum() == 0) continue;

      // Time-domain convolution with truncated precomputed kernels and __restrict__ pointers for SIMD
      if(elec == 0){
        double* __restrict__ out_apv = ch->Get_Histo_int_apv()->GetArray();
        const double* __restrict__ k_apv = kernel_0.data();
        for(int it = 1; it < total_ns; it++) {
          const double val = cur_buf_fft[it];
          if(val == 0.0) continue;
          const int jt_start = (it > n_ns_buffer) ? it : (n_ns_buffer + 1);
          const int jt_end   = std::min(total_ns, it + klen_0);
          for(int jt = jt_start; jt < jt_end; jt++) {
            out_apv[jt - n_ns_buffer] += val * k_apv[jt - it];
          }
        }
        // Discretize APV signal in 25 ns time bins
        double jitter = 0;
        Waveform* h_int_apv = ch->Get_Histo_int_apv();
        Waveform* h_apv     = ch->Get_Histo_apv();
        for(int iapv = 0; iapv < timebin_APV; iapv++) {
          double t = jitter + iapv * timestep_APV;
          int timebin = h_int_apv->FindBin(t);
          double qmeas = h_int_apv->GetBinContent(timebin) * fC_to_ADC;
          h_apv->AddBinContent(iapv + 1, qmeas);
        }
      } else if(elec == 1){
        double* __restrict__ out_tigT = ch->Get_Histo_tiger_T()->GetArray();
        double* __restrict__ out_tigE = ch->Get_Histo_tiger_E()->GetArray();
        const double* __restrict__ k_tigT = kernel_0.data();
        const double* __restrict__ k_tigE = kernel_1.data();
        const int klen_max = std::max(klen_0, klen_1);
        for(int it = 1; it < total_ns; it++) {
          const double val = cur_buf_fft[it];
          if(val == 0.0) continue;
          const int jt_start = (it > n_ns_buffer) ? it : (n_ns_buffer + 1);
          const int jt_end   = std::min(total_ns, it + klen_max);
          for(int jt = jt_start; jt < jt_end; jt++) {
            const int out_idx = jt - n_ns_buffer;
            const int dt = jt - it;
            out_tigT[out_idx] += val * k_tigT[dt];
            out_tigE[out_idx] += val * k_tigE[dt];
          }
        }
      } else if(elec == 2){
        double* __restrict__ out_tora = ch->Get_Histo_int_tora()->GetArray();
        const double* __restrict__ k_tora = kernel_0.data();
        for(int it = 1; it < total_ns; it++) {
          const double val = cur_buf_fft[it];
          if(val == 0.0) continue;
          const int jt_start = (it > n_ns_buffer) ? it : (n_ns_buffer + 1);
          const int jt_end   = std::min(total_ns, it + klen_0);
          for(int jt = jt_start; jt < jt_end; jt++) {
            out_tora[jt - n_ns_buffer] += val * k_tora[jt - it];
          }
        }
      }
    }
  }
 
  void Readout::Injection_External_Signal(){
    // Inject a square wave on the central channel
    int t0_ns = n_ns/10;
    if(Square_Wave){
      for(size_t ich=0;ich<channel.size();ich++){
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
      for(size_t ich=0;ich<channel.size();ich++){
        double q_peak = channel.at(ich)->Get_Histo_apv()->GetMaximum(); // fC
        double q_peak_adc = q_peak * fC_to_ADC; // ADC  
        if (Capacitive_Induction_1(q_peak_adc) &&
            ich!=0 &&
            ich!=channel.size()-1
            ){
          for(int itime = 0; itime < timebin_APV; itime++) {
            double t_in = (itime+0.5) * 25;
            double q_in = channel.at(ich)->Get_Histo_apv()->GetBinContent(itime+1) * fC_to_ADC;
            if(q_in>1800) q_in=1800;
            if(q_in<0)    continue;
            // Induce on i-strip +/- 1  
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
      for(size_t ich=0;ich<channel.size();ich++){
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
    double ratio = cap_charge_2->Eval(q_in)/100;
    if(ratio>0.5) ratio = 0.5; 
    ch *= ratio;
    return ch;
  }


  void Readout::Filtering(){
    if(channel.empty()) return;
    double freq_cut_min = FFT_fmin; // Hz
    double freq_cut_max = FFT_fmax; // Hz
    if(NO_FFT || (FFT_fmin==0 && FFT_fmax==0)){
      for(size_t ich = 0; ich < channel.size(); ich++){
        int nbins = channel.at(ich)->Get_Histo_cur_buffer()->GetNbinsX();
        for(int ii = 1; ii <= nbins; ii++){
          channel.at(ich)->Get_Histo_cur_buffer_FFT()->SetBinContent(
              ii, channel.at(ich)->Get_Histo_cur_buffer()->GetBinContent(ii));
        }
      }
      return;
    }
    // Reuse FFT plans across all channels and release them on exit
    int nbin_histo_cur = channel.at(0)->Get_Histo_cur_buffer()->GetNbinsX();
    TVirtualFFT *fft_r2c = TVirtualFFT::FFT(1, &nbin_histo_cur, "R2C EX");
    TVirtualFFT *fft_c2r = TVirtualFFT::FFT(1, &nbin_histo_cur, "C2R EX");
    int n_freq_bins = (nbin_histo_cur / 2) + 1;
    std::vector<double> v_histo_cur(nbin_histo_cur);
    std::vector<double> re_out(n_freq_bins);
    std::vector<double> im_out(n_freq_bins);
    std::vector<double> wf_filtered_raw(nbin_histo_cur);

    for(size_t ich = 0; ich < channel.size(); ich++){
      Waveform* h_in  = channel.at(ich)->Get_Histo_cur_buffer();
      Waveform* h_out = channel.at(ich)->Get_Histo_cur_buffer_FFT();
      for(int ii = 0; ii < nbin_histo_cur; ii++){
        v_histo_cur[ii] = h_in->GetBinContent(ii + 1);
      }
      // Forward FFT
      fft_r2c->SetPoints(v_histo_cur.data());
      fft_r2c->Transform();
      fft_r2c->GetPointsComplex(re_out.data(), im_out.data());
      // Frequency-domain bandpass filter
      const double fs_hz = 1e9;
      for (int k = 0; k < n_freq_bins; k++) {
        double current_freq_hz = (double)k * fs_hz / nbin_histo_cur;
        if (current_freq_hz < freq_cut_min ||   // High-pass
            current_freq_hz > freq_cut_max ) {  // Low-pass
          re_out[k] = 0.0;
          im_out[k] = 0.0;
        }
      }
      // Inverse FFT and ROOT 1/N normalization
      fft_c2r->SetPointsComplex(re_out.data(), im_out.data());
      fft_c2r->Transform();
      fft_c2r->GetPoints(wf_filtered_raw.data());
      for(int ii = 0; ii < nbin_histo_cur; ii++){
        h_out->SetBinContent(ii + 1, wf_filtered_raw[ii] / nbin_histo_cur);
      }
    }
    delete fft_r2c;
    delete fft_c2r;
  }

  void Readout::Extract_Charge_Time(){
    for(size_t ich = 0; ich < channel.size(); ich++){

      if(Get_Electronics()==0){ // APV
        // Compute channel charge and threshold condition first
        channel.at(ich)->Set_Charge(Get_Charge_APV(channel.at(ich)));
        float SumQ = 0;
        int nbins_apv = channel.at(ich)->Get_Histo_apv()->GetNbinsX();
        for(int ii = 1; ii <= nbins_apv; ii++) SumQ += channel.at(ich)->Get_Histo_apv()->GetBinContent(ii);
        bool is_above = (SumQ / nbins_apv > mmdaq_thr_factor * mmdaq_ped * fC_to_ADC &&
                         channel.at(ich)->Get_Charge() > threshold_fC);
        channel.at(ich)->Set_AboveThr(is_above);

        // Run expensive Fermi-Dirac time fit only on channels above threshold
        if(is_above){
          channel.at(ich)->Set_Time(Get_Time_APV(channel.at(ich)));
          channel.at(ich)->Set_dTime(Get_dTime_APV(channel.at(ich)));
        } else {
          channel.at(ich)->Set_Time(-1);
          channel.at(ich)->Set_dTime(-1);
        }

        // Save the APV Qt hit distribution
        if(is_above){
          int hit_q_min = 0;
          int hit_q_step = 10;
          int hit_q_max = 80;
          for(int iq=0; iq<hit_q_max/hit_q_step; iq++){
            int hit_q_step_i = hit_q_min+iq*hit_q_step;
            int hit_q_step_ip = hit_q_min+(iq+1)*hit_q_step;
            if(channel.at(ich)->Get_Charge()>hit_q_step_i && channel.at(ich)->Get_Charge()<hit_q_step_ip && !_histo_hit_Qt_APV.empty()){
              for(int ii=0;ii<nbins_apv;ii++){
                _histo_hit_Qt_APV.at(iq)->AddBinContent(ii+1,channel.at(ich)->Get_Histo_apv()->GetBinContent(ii+1));
              }
            }
          }
        }
      }
      if(Get_Electronics()==1){ // TIGER
        // Set channel charge and time
        channel.at(ich)->Set_Charge(Get_Charge_TIGER(channel.at(ich)));
        channel.at(ich)->Set_Time(Get_Time_TIGER(channel.at(ich)));
        channel.at(ich)->Set_dTime(Get_dTime_TIGER(channel.at(ich)));
        // Set threshold flag
        if(channel.at(ich)->Get_AboveThr_E() && channel.at(ich)->Get_AboveThr_T()) channel.at(ich)->Set_AboveThr(true);
        else channel.at(ich)->Set_AboveThr(false);
        if(channel.at(ich)->Get_AboveThr() && (channel.at(ich)->Get_Charge()==0 || channel.at(ich)->Get_Time()==0)) cout<<"ich: "<<ich<<" Q: "<<channel.at(ich)->Get_Charge()<<" T: "<<channel.at(ich)->Get_Time()<<endl;
      }
      if(Get_Electronics()==2){ // TORA
        // Set channel charge and time
        channel.at(ich)->Set_Charge(Get_Charge_TORA(channel.at(ich)));
        channel.at(ich)->Set_Time(Get_Time_TORA(channel.at(ich)));
        channel.at(ich)->Set_dTime(Get_dTime_TORA(channel.at(ich)));
        // Require both charge and time branches above threshold so rejected channels (-1) never form hits
        if(channel.at(ich)->Get_AboveThr_E() && channel.at(ich)->Get_AboveThr_T()) channel.at(ich)->Set_AboveThr(true);
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
    Waveform *h_time = ch->Get_Histo_apv();
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
    // Parameter limits
    fitlimlow[0] = startvalues[0] - 5 * sigma_MFB;
    fitlimlow[1] = startvalues[1] - 0.3 * startvalues[1];
    fitlimlow[2] = minbin * timestep_APV;
    fitlimlow[3] = 0.1*timestep_APV;
    fitlimup[0] = startvalues[0] + 5 * sigma_MFB;
    fitlimup[1] = startvalues[1] + 0.1 * startvalues[1];
    fitlimup[2] = maxbin * timestep_APV;
    fitlimup[3] = 0.9*timestep_APV;
    // Update for uRWELL / uRTube
    startvalues[0]  =    0;
    fitlimlow[0]    = -0.1;
    fitlimup[0]     =  0.1;
    // Fermi-Dirac fit on temporary detached 27-bin TH1D
    double minFD = (minbin - 20) * timestep_APV;
    double maxFD = (maxbin +  3) * timestep_APV;
    TH1D h_fit = h_time->ToTH1D("h_apv_fit", "h_apv_fit");
    TF1 f_FD("f_FD", "[0] + [1]/(1+TMath::Exp(-(x - [2])/[3]))", minFD, maxFD);
    f_FD.SetParameters(startvalues[0], startvalues[1], startvalues[2], startvalues[3]);
    f_FD.SetParLimits(0, fitlimlow[0], fitlimup[0]);
    f_FD.SetParLimits(1, fitlimlow[1], fitlimup[1]);
    f_FD.SetParLimits(2, fitlimlow[2], fitlimup[2]);
    f_FD.SetParLimits(3, fitlimlow[3], fitlimup[3]);
    if(QMaxHisto > 0){
      h_fit.Fit(&f_FD, "QWRBMN0");
    }
    double tFD  = r->Gaus(f_FD.GetParameter(2), 7.12);
    double dtFD = f_FD.GetParError(2);
    Set_Time(tFD);
    Set_dTime(dtFD);
    return;
  }

  double Readout::Get_Charge_TIGER(ElectronicChannel *ch){
    Waveform *h_time = ch->Get_Histo_tiger_E();
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
    if(time_thr > 0){
      for(int i = time_thr; i < maxbin; i++) {
        if(i == maxbin - 1) time_falling = 2 * maxbin;
        if(h_time->GetBinContent(i) < thrE_TIGER) {
          time_falling = i - 1;
          break;
        }
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
    Waveform *h_time = ch->Get_Histo_tiger_T();
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
    if(time_thr > 0){
      for(int i = time_thr + 1; i < maxbin; i++) {
        if(i == maxbin - 1) time_falling = 2 * maxbin;
        if(h_time->GetBinContent(i) < thrT_TIGER) {
          time_falling = i - 1;
          break;
        }
      }
    }
    ch->Set_t_rising_T(time_thr);
    ch->Set_t_falling_T(time_falling);
    if(enable_tfine_tiger && time_thr > 1){
      double denom = h_time->GetBinContent(time_thr) - h_time->GetBinContent(time_thr - 1);
      if(denom != 0){
        efine_time_thr = 1. - (thrT_TIGER - h_time->GetBinContent(time_thr - 1)) / denom;
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
    Waveform *h_time = ch->Get_Histo_int_tora();
    int maxbin       = h_time->GetNbinsX();
    float time_thr     = -999; // leading edge
    float time_falling = -999; // falling edge
    ch->Set_AboveThr_E(false);
    for(int i = 1; i < maxbin - 1; i++) {
      if(h_time->GetBinContent(i) > thrRise_TORA) {
        time_thr = i;
        break;
      }
    }
    ch->Set_t_thr_E(time_thr);
    ch->Set_t_rising_E(time_thr);

    // Saturation: clip waveform at per-event smeared threshold q_peak and advance peak index to plateau end
    ch->Set_Saturation_TORA(r->Gaus(saturation_TORA, sigma_saturation_TORA));
    float q_peak = ch->Get_Saturation_TORA();
    int timestamp_at_peak = h_time->GetMaximumBin();
    if(!NO_Saturation){
      for(int i = 1; i < maxbin; i++) {
        if(h_time->GetBinContent(i) > q_peak) {
          h_time->SetBinContent(i, q_peak);
          timestamp_at_peak = i;
        }
      }
    }
    if(time_thr >= 0) {
      time_thr = static_cast<int>(time_thr / timestep_TORA) * timestep_TORA;
    }
    float time_peak = static_cast<int>(timestamp_at_peak / timestep_TORA) * timestep_TORA;

    // Falling edge search starting from the signal peak (or end of saturation plateau)
    for(int i = timestamp_at_peak; i < maxbin; i++) {
      if(i == maxbin - 1) time_falling = 2 * maxbin;
      if(h_time->GetBinContent(i) < thrTrail_TORA) {
        time_falling = i - 1;
        break;
      }
    }
    ch->Set_t_falling_E(time_falling);

    if(time_thr < 0 || time_peak >= maxbin || time_falling < 0 || time_falling >= maxbin) return -1;
    if(gain_TORA <= 0) return -1;

    float charge = h_time->GetBinContent(h_time->GetMaximumBin()) / gain_TORA;
    if(charge <= thrTrail_TORA / gain_TORA) return -1;

    ch->Set_AboveThr_E(true);
    ch->Set_t_Q_E(time_peak);
    return charge;
  }

  double Readout::Get_Time_TORA(ElectronicChannel *ch){
    Waveform *h_time = ch->Get_Histo_int_tora();
    double maxbin    = h_time->GetNbinsX();
    float time_thr      = -1;  // leading edge
    float time_falling  = -999; // falling edge
    float efine_time_thr=0;
    ch->Set_AboveThr_T(false);
    for(int i=1;i<maxbin-1;i++) {
      if(h_time->GetBinContent(i)>thrRise_TORA) {
        time_thr = i;
        ch->Set_AboveThr_T(true);
        break;
      }
    }
    for(int i=h_time->GetMaximumBin();i<maxbin;i++) {
      if(i==maxbin-1) time_falling=2*maxbin;
      if(h_time->GetBinContent(i)<thrTrail_TORA) {
        time_falling = i-1;
	      break;
      }
    }
    ch->Set_t_rising_T(time_thr);
    ch->Set_t_falling_T(time_falling);

    // TFine
    // if(enable_tfine_tiger){
    //   if(time_thr!=0){
	  //     efine_time_thr = 1 - (thrRise_TORA-h_time->GetBinContent(time_thr-1))/(h_time->GetBinContent(time_thr)-h_time->GetBinContent(time_thr-1));
    //   }
    // }

    // Saturation
    // float q_peak  = ch->Get_Saturation_TORA();
    // if(!NO_Saturation){
    //   for(int i=1;i<maxbin;i++) {
    //     if(h_time->GetBinContent(i)>saturation_TORA) h_time->SetBinContent(i,q_peak);
    //   }
    // }
    if(time_thr<0 || time_falling<0) return time_thr;
    // Digitize time in TORA timestep (5 ns)
    time_thr = ((int)(1+time_thr/timestep_TORA))*timestep_TORA - efine_time_thr*timestep_TORA;
    return time_thr;
  }

  double Readout::Get_dTime_TORA(ElectronicChannel *ch){
    return timestep_TORA;
  }


  void Readout::Measure_Signal_Lenght(){
    for(size_t ich=0;ich<channel.size();ich++){
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


