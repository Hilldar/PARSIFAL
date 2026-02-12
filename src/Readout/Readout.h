#ifndef Readout_h
#define Readout_h
#include "Common/Common.h"
#include "Geometry/Geometry.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
#include "Common/ElectronicChannel.h"
#include "TF1.h"
using namespace std;
namespace PARSIFAL2{
  class Readout{
  public:
    //Constructur
    Readout(int setup, int electronics, Geometry *geometry);
    //Destructor
    ~Readout();
    //Function
    bool                       Get_PrintInfo     () {return PrintInfo;};
    bool                       Get_PrintNTuple   () {return PrintNTuple;};
    vector<ElectronicChannel*> Read              ();
    vector<ElectronicChannel*> Read              (vector<Secondary*>);
    int                        Define_View       ();
    void                       Define_ChannelID  (vector<Secondary*>);
    void                       Integrate_Charge  ();
    void                       Induce_on_channel (int type, double position, double time, int weight);
    ElectronicChannel*         Get_Channel       (int ich) {return channel.at(ich);};
    vector<ElectronicChannel*> Get_Channels      () {return channel;}
    int                        Get_N_Channels    () {return channel.size();};
    void                       Clean             () {for(int i=0;i<channel.size();i++) channel.at(i)->ResetNelectrons();};
    void                       Set_Noise_fC      (double io);
    void                       Set_Threshold_fC  (double io){threshold_fC=io;};
    void                       Set_IonTailLength (double io){IT_Lenght=io; IT_amplitude=1./IT_Lenght;};
    void                       Set_FastSlowRatio (double io){IT_ratio_fast_slow=io;};
    void                       Set_APV_ped       (double io){mmdaq_ped=io;};
    void                       Set_APV_thr       (double io){mmdaq_thr_factor=io;};
    void                       Set_TIGER_thr_E   (double io){thrE_TIGER=io*gain_TIGER; for(int i=0;i<channel.size();i++) channel.at(i)->Set_V_thr_E(thrE_TIGER);};
    void      	      	       Set_TIGER_thr_T   (double io){thrT_TIGER=io*gain_TIGER; for(int i=0;i<channel.size();i++) channel.at(i)->Set_V_thr_T(thrT_TIGER);};
    void                       Set_APV_Plot_Hit  (vector<TH1F*> io){_histo_hit_Qt_APV=io;};
    
  private:
    //Variable
    bool      PrintInfo;
    bool      PrintNTuple;
    TRandom3 *r;
    int       Setup;
    int       Electronics;
    Geometry *geometry;
    vector<ElectronicChannel*> channel;
    //noise
    float     noise_amplitude;
    double    sigma_noise_fC;
    double    threshold_fC;
    const float max_freq_noise = 340e-3; // 340 MHz (1timebin = 1ns)
    const float min_freq_noise =   1e-6 ;  // 1 kHz (1timebin = 1ns)
    const bool white_noise    = true;
    const bool noise_1_over_f = false;
    //apv
    float     mmdaq_ped;
    float     mmdaq_thr_factor;
    //tiger
    float     thrT_TIGER;
    float     thrE_TIGER;
    bool      TIGER_Get_Maximum;
    //ion tail signal contribution
    float     IT_Lenght ; //ns
    float     IT_amplitude; //fC/ns
    float     IT_ratio_fast_slow; // signal fraction due to ions and (1-ratio) due to electrons
    //Function
    void      Initialize();
    void      Set_PrintInfo     (bool io) {PrintInfo=io;};
    void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
    void      Set_Setup         (int io)  {Setup=io;};
    
    int       Get_Setup         ()        {return Setup;}
    int       Get_Electronics   ()        {return Electronics;}
    int       Get_ChannelID     (int type, Secondary* ele);
    void      Simulate_Electronics();
    void      Simulate_APV();
    void      Simulate_TIGER();
    void      Initialize_APV();
    void      Initialize_TIGER();
    //void      Background_APV();
    //void      Background_TIGER();
    void      Background();
    //void      Integration_APV();
    //void      Integration_TIGER();
    void      Integration();
    void      Extract_Charge_Time();
    void      Capacitive_Induction();
    void      Injection_External_Signal();
    void      Measure_Signal_Lenght();
    double    Get_Charge_APV(ElectronicChannel *ch);
    void      Measure_Time_APV(ElectronicChannel *ch);
    double    Get_Time_APV(ElectronicChannel *ch);
    double    Get_dTime_APV(ElectronicChannel *ch);
    double    Get_Charge_TIGER(ElectronicChannel *ch);
    double    Get_Time_TIGER(ElectronicChannel *ch);
    double    Get_dTime_TIGER(ElectronicChannel *ch);

    //Variable capacitive effect
    TF1* cap_probability_1;
    TF1* cap_probability_2;
    TF1* cap_time_1;
    TF1* cap_time_2;
    TF1* cap_charge_1;
    TF1* cap_charge_2;
    TRandom3 *r_cap;
    //Function capacitive
    bool Capacitive_Induction_1(double q);
    bool Capacitive_Induction_2(double q);
    double Get_T_induced_1(double q, double t);
    double Get_T_induced_2(double q, double t);
    double Get_Q_induced_1(double q);
    double Get_Q_induced_2(double q);
    //Integrator
    TF1 *f[n_ns];
    //APV
    double time, dtime;
    void Set_Time(double io){time=io;};
    double Get_Time(){return time;};
    void Set_dTime(double io){dtime=io;};
    double Get_dTime(){return dtime;};
    double APV_shaper(double t){
      if (t > 0) return TMath::Exp(1.)*(t/tau_APV*TMath::Exp(-t/tau_APV));
      return 0;
    }
    vector<TH1F*> _histo_hit_Qt_APV;
    //Tiger
    double T_branch(double t) {
      t = t - 10.;
      if (t > 0) return 2000 * (0.00181928 * exp(-t / 3.) - 0.0147059 * exp(-t / 20.) + 0.0128866 * exp(-t / 100.));
      return 0;
    }
    double E_branch(double t) {
      t = t - 5;
      if (t > 0) return 0.000627357 * (1358.7 * exp(-t * 0.0385647) * t + 1358.7 * exp(-t * 0.0114353) * t + 100164. * exp(-t * 0.0385647) - 100164. * exp(-0.0114353 * t));
      return 0;
    }

  };
}
#endif
