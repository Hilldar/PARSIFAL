#ifndef ElectronicChannel_h
#define ElectronicChannel_h
#include <algorithm>
#include <vector>
#include "TH1D.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPaveText.h"
using namespace std;

// XYZ are referred to the detector frame where the anode is on the XY plane and the electric drift field is along Z
namespace PARSIFAL2{

  // Lightweight 1-based waveform buffer backed by std::vector<double> (matching ROOT TH1D bin indexing)
  class Waveform {
  public:
    Waveform(int nbins = 0, double xmin = 0.0, double xmax = 1.0)
        : nbins_(nbins), xmin_(xmin), xmax_(xmax),
          inv_bw_(nbins > 0 ? nbins / (xmax - xmin) : 0.0),
          data_(nbins > 0 ? static_cast<size_t>(nbins + 2) : 0, 0.0) {}

    int     GetNbinsX() const { return nbins_; }
    double  GetXmin()   const { return xmin_; }
    double  GetXmax()   const { return xmax_; }
    double* GetArray()        { return data_.data(); }
    const double* GetArray() const { return data_.data(); }

    void Reset() { std::fill(data_.begin(), data_.end(), 0.0); }

    double GetBinContent(int bin) const {
      if (bin < 0 || bin > nbins_ + 1) return 0.0;
      return data_[bin];
    }
    void SetBinContent(int bin, double val) {
      if (bin >= 0 && bin <= nbins_ + 1) data_[bin] = val;
    }
    void AddBinContent(int bin, double val) {
      if (bin >= 0 && bin <= nbins_ + 1) data_[bin] += val;
    }
    int FindBin(double x) const {
      if (nbins_ <= 0) return 0;
      if (x < xmin_) return 0;
      if (x >= xmax_) return nbins_ + 1;
      return 1 + static_cast<int>((x - xmin_) * inv_bw_);
    }
    void Fill(double x, double w = 1.0) {
      int b = FindBin(x);
      if (b >= 0 && b <= nbins_ + 1) data_[b] += w;
    }
    double GetMaximum() const {
      if (nbins_ <= 0) return 0.0;
      double m = data_[1];
      for (int i = 2; i <= nbins_; ++i) {
        if (data_[i] > m) m = data_[i];
      }
      return m;
    }
    double GetMinimum() const {
      if (nbins_ <= 0) return 0.0;
      double m = data_[1];
      for (int i = 2; i <= nbins_; ++i) {
        if (data_[i] < m) m = data_[i];
      }
      return m;
    }
    int GetMaximumBin() const {
      if (nbins_ <= 0) return 0;
      int best = 1;
      double m = data_[1];
      for (int i = 2; i <= nbins_; ++i) {
        if (data_[i] > m) { m = data_[i]; best = i; }
      }
      return best;
    }
    void Add(const Waveform* other, double scale = 1.0) {
      int n = std::min(nbins_, other->nbins_);
      for (int i = 1; i <= n; ++i) data_[i] += scale * other->data_[i];
    }

    // Materialize a detached ROOT TH1D on demand (used only for fits and diagnostic plotting)
    TH1D ToTH1D(const char* name, const char* title,
                const char* xtitle = "Time [ns]", const char* ytitle = "") const {
      TH1D h(name, title, nbins_ > 0 ? nbins_ : 1, xmin_, xmax_);
      h.SetDirectory(nullptr);
      const int max_i = std::min(nbins_ + 1, static_cast<int>(data_.size()) - 1);
      for (int i = 0; i <= max_i; ++i) {
        h.SetBinContent(i, data_[i]);
      }
      h.GetXaxis()->SetTitle(xtitle);
      h.GetYaxis()->SetTitle(ytitle);
      h.GetYaxis()->SetTitleOffset(0.9);
      return h;
    }

  private:
    int    nbins_;
    double xmin_;
    double xmax_;
    double inv_bw_;
    std::vector<double> data_;
  };

  class ElectronicChannel{
  public:
    // Constructors
    ElectronicChannel():
      h_time_raw(), h_time_tot(), h_time_res(), h_time_cap(),
      h_time_cur(), h_time_cur_FFT(), h_time_cur_buffer(), h_time_cur_buffer_FFT(),
      h_time_int_apv(), h_time_int_tigE(), h_time_int_tigT(),
      h_time_apv(), h_time_int_tora(),
      PrintNTuple(PrintNTuple_Channel), ElectronicsType(0), channelID(-1), type(0),
      pitch(0), charge(0), time(0), dtime(0), n_electrons(0),
      above_thr(false), above_thr_E(false), above_thr_T(false),
      t_thr_E(0), t_Q_E(0), t_rising_raw(0), t_rising_E(0), t_rising_T(0),
      t_falling_raw(0), t_falling_E(0), t_falling_T(0),
      V_thr_T(10), V_thr_E(10), V_thrRise(10), V_thrTrail(0),
      saturation_apv(saturation_APV), saturation_tiger(saturation_TIGER),
      saturation_tora(saturation_TORA), gain_TORA(12.7) {};

    ElectronicChannel(int tipo, int ich, Position pos, double passo):
      h_time_raw(n_ns, 0, n_ns),
      h_time_tot(n_ns, 0, n_ns),
      h_time_res(n_ns, 0, n_ns),
      h_time_cap(static_cast<int>(timebin_APV), 0, nbin),
      h_time_cur(n_ns, 0, n_ns),
      h_time_cur_FFT(n_ns, 0, n_ns),
      h_time_cur_buffer(n_ns + n_ns_buffer, -n_ns_buffer, n_ns),
      h_time_cur_buffer_FFT(n_ns + n_ns_buffer, -n_ns_buffer, n_ns),
      h_time_int_apv(n_ns, 0, n_ns),
      h_time_int_tigE(n_ns, 0, n_ns),
      h_time_int_tigT(n_ns, 0, n_ns),
      h_time_apv(static_cast<int>(timebin_APV), 0, timebin_APV * timestep_APV),
      h_time_int_tora(n_ns, 0, n_ns),
      PrintNTuple(PrintNTuple_Channel), position(pos), ElectronicsType(0),
      channelID(ich), type(tipo), pitch(passo), charge(0), time(0), dtime(0), n_electrons(0),
      above_thr(false), above_thr_E(false), above_thr_T(false),
      t_thr_E(0), t_Q_E(0), t_rising_raw(0), t_rising_E(0), t_rising_T(0),
      t_falling_raw(0), t_falling_E(0), t_falling_T(0),
      V_thr_T(10), V_thr_E(10), V_thrRise(10), V_thrTrail(0),
      saturation_apv(saturation_APV), saturation_tiger(saturation_TIGER),
      saturation_tora(saturation_TORA), gain_TORA(12.7)
    {
      // Per-channel APV saturation dispersion
      TRandom3 r(ich);
      Set_Saturation_APV(r.Gaus(saturation_APV, sigma_saturation_APV));
    };

    ~ElectronicChannel() = default;

    // Getters
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
    double    Get_t_rising_T    ()          {return t_rising_T;};
    double    Get_t_falling_raw ()          {return t_falling_raw;};
    double    Get_t_falling_E   ()          {return t_falling_E;};
    double    Get_t_falling_T   ()          {return t_falling_T;};
    int       Get_N_electrons   ()          {return n_electrons;};
    Waveform* Get_Histo_raw     ()          {return &h_time_raw;};
    Waveform* Get_Histo_tot     ()          {return &h_time_tot;};
    Waveform* Get_Histo_res     ()          {return &h_time_res;};
    Waveform* Get_Histo_cap     ()          {return &h_time_cap;};
    Waveform* Get_Histo_cur     ()          {return &h_time_cur;};
    Waveform* Get_Histo_cur_FFT ()          {return &h_time_cur_FFT;};
    Waveform* Get_Histo_cur_buffer()        {return &h_time_cur_buffer;};
    Waveform* Get_Histo_cur_buffer_FFT()    {return &h_time_cur_buffer_FFT;};
    Waveform* Get_Histo_int_apv ()          {return &h_time_int_apv;};
    Waveform* Get_Histo_tiger_E ()          {return &h_time_int_tigE;};
    Waveform* Get_Histo_tiger_T ()          {return &h_time_int_tigT;};
    Waveform* Get_Histo_apv     ()          {return &h_time_apv;};
    Waveform* Get_Histo_int_tora()          {return &h_time_int_tora;};
    bool      Get_AboveThr      ()          {return above_thr;};
    bool      Get_AboveThr_T    ()          {return above_thr_T;};
    bool      Get_AboveThr_E    ()          {return above_thr_E;};
    float     Get_Saturation_APV()          {return saturation_apv;};
    float     Get_Saturation_TIGER()        {return saturation_tiger;};
    float     Get_Saturation_TORA()         {return saturation_tora;};
    double    Get_Gain_TORA()               {return gain_TORA;};

    // Setters
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
    void      Set_V_thrRise     (float  io) {V_thrRise=io;};
    void      Set_V_thrTrail    (float  io) {V_thrTrail=io;};
    void      Set_Saturation_APV(float  io) {saturation_apv=io;};
    void      Set_Saturation_TIGER(float io){saturation_tiger=io;};
    void      Set_Saturation_TORA(float io) {saturation_tora=io;};
    void      Set_Gain_TORA     (double io) {gain_TORA=io;};

    void      ResetNelectrons   ()          {n_electrons=0;};
    void      Reset             ()          {Reset_Time(); Set_Charge(0); Set_Time(0); Set_dTime(0);};
    void      Reset_Time        () {
      h_time_raw.Reset();
      h_time_tot.Reset();
      h_time_res.Reset();
      h_time_cap.Reset();
      h_time_cur.Reset();
      h_time_cur_FFT.Reset();
      h_time_cur_buffer.Reset();
      h_time_cur_buffer_FFT.Reset();
      h_time_int_apv.Reset();
      h_time_int_tigE.Reset();
      h_time_int_tigT.Reset();
      h_time_int_tora.Reset();
      h_time_apv.Reset();
    };
    void      Fill_Time         (double io, float weight) {h_time_raw.Fill(io, 1.6e-4 * weight);};

    void      Print_Time(int ievent){
      if(Get_Electronics()==0) Print_Time_APV(ievent);
      if(Get_Electronics()==1) Print_Time_TIGER(ievent);
      if(Get_Electronics()==2) Print_Time_TORA(ievent);
    }

    void      Print_Time_APV    (int ievent) {
      TH1D h_raw = h_time_raw.ToTH1D(Form("raw_%i_%i",type,channelID), Form("Current raw_%i_%i",type,channelID), "Time [ns]", "Current [1 fC / 1 ns]");
      TH1D h_tot = h_time_tot.ToTH1D(Form("tot_%i_%i",type,channelID), Form("Charge[fC] tot_%i_%i",type,channelID), "Time [ns]", "Charge [fC]");
      TH1D h_cur = h_time_cur.ToTH1D(Form("cur_%i_%i",type,channelID), Form("Current cur_%i_%i",type,channelID), "Time [ns]", "Current [1 fC / 1 ns]");
      TH1D h_res = h_time_res.ToTH1D(Form("res_%i_%i",type,channelID), Form("Charge[fC] res_%i_%i",type,channelID), "Time [ns]", "Charge [fC]");
      TH1D h_fft = h_time_cur_FFT.ToTH1D(Form("cur_FFT_%i_%i",type,channelID), Form("Current cur_FFT_%i_%i",type,channelID), "Time [ns]", "Current [1 fC / 1 ns]");
      TH1D h_int = h_time_int_apv.ToTH1D(Form("int_%i_%i",type,channelID), Form("Charge[fC] int_%i_%i",type,channelID), "Time [ns]", "Charge [fC]");
      TH1D h_apv = h_time_apv.ToTH1D(Form("apv_%i_%i",type,channelID), Form("Voltage[ADC] apv_%i_%i",type,channelID), "Time [ns]", "Voltage [ADC]");
      h_apv.GetYaxis()->SetRangeUser(-200, 2000);

      TCanvas c("ccc","ccc",1600,1600);
      c.Divide(3,3);
      c.cd(1); h_raw.Draw("hist");
      c.cd(2); h_tot.Draw();
      c.cd(3);
      c.cd(4); h_cur.Draw("hist");
      c.cd(5); h_res.Draw();
      c.cd(6); h_fft.Draw("hist");
      c.cd(7); h_int.Draw();
      c.cd(8); h_apv.Draw();
      float thr_APV_fC = 45;
      TLine l_V_thr_T(0, static_cast<float>(thr_APV_fC * fC_to_ADC), n_ns, static_cast<float>(thr_APV_fC * fC_to_ADC));
      if(!Get_AboveThr()){
        l_V_thr_T.SetLineColor(kRed);
        l_V_thr_T.Draw("same");
      }
      c.cd(9);
      TPaveText text_apv(0.2,0.2,0.8,0.8);
      text_apv.AddText(Form("APV thr = %.0f fC",thr_APV_fC));
      text_apv.AddText(Form("Time measured = %.2f ns",Get_Time()));
      text_apv.AddText(Form("Charge measured = %.2f fC",Get_Charge()));
      text_apv.AddText(Form("Max charge collected = %.2f fC",Get_Histo_tot()->GetMaximum()));
      text_apv.Draw();
      c.SaveAs(Form("pdf/Event_%i_Channel_%i.pdf",ievent,Get_ChannelID()));
      c.SaveAs(Form("pdf/Event_%i_Channel_%i.png",ievent,Get_ChannelID()));
    };

    void      Print_Time_TIGER  (int ievent) {
      TH1D h_raw  = h_time_raw.ToTH1D(Form("raw_%i_%i",type,channelID), Form("Current raw_%i_%i",type,channelID), "Time [ns]", "Current [1 fC / 1 ns]");
      TH1D h_tot  = h_time_tot.ToTH1D(Form("tot_%i_%i",type,channelID), Form("Charge[fC] tot_%i_%i",type,channelID), "Time [ns]", "Charge [fC]");
      TH1D h_cur  = h_time_cur.ToTH1D(Form("cur_%i_%i",type,channelID), Form("Current cur_%i_%i",type,channelID), "Time [ns]", "Current [1 fC / 1 ns]");
      TH1D h_res  = h_time_res.ToTH1D(Form("res_%i_%i",type,channelID), Form("Charge[fC] res_%i_%i",type,channelID), "Time [ns]", "Charge [fC]");
      TH1D h_tigT = h_time_int_tigT.ToTH1D(Form("tig_T_%i_%i",type,channelID), Form("Voltage[mV] tig_T_%i_%i",type,channelID), "Time [ns]", "Voltage [1 mV / 1 ns]");
      TH1D h_tigE = h_time_int_tigE.ToTH1D(Form("tig_E_%i_%i",type,channelID), Form("Voltage[mV] tig_E_%i_%i",type,channelID), "Time [ns]", "Voltage [1 mV / 1 ns]");
      h_tigT.GetYaxis()->SetRangeUser(-200, 900);
      h_tigE.GetYaxis()->SetRangeUser(-200, 900);

      TCanvas c("ccc","ccc",1600,1600);
      c.Divide(3,3);
      c.cd(1); h_raw.Draw("hist");
      c.cd(2); h_tot.Draw();
      c.cd(3);
      c.cd(4); h_cur.Draw("hist");
      c.cd(5); h_res.Draw();
      c.cd(6);
      c.cd(7); h_tigT.Draw();
      float t_thr_T_val = Get_Time();
      TLine l_t_thr_T(t_thr_T_val, h_tigT.GetMinimum() - 10, t_thr_T_val, V_thr_T);
      TLine l_V_thr_T(0, V_thr_T, Get_AboveThr_T() ? t_thr_T_val : n_ns, V_thr_T);
      l_V_thr_T.SetLineColor(kRed);
      l_t_thr_T.SetLineColor(kRed);
      l_V_thr_T.Draw("same");
      if(Get_AboveThr_T()) l_t_thr_T.Draw("same");

      c.cd(8); h_tigE.Draw();
      float V_Q = Get_Charge() * gain_TIGER;
      TLine l_t_thr_E(Get_t_thr_E(), h_tigE.GetMinimum() - 10, Get_t_thr_E(), V_thr_E);
      TLine l_V_thr_E(0, V_thr_E, Get_AboveThr_E() ? Get_t_thr_E() : n_ns, V_thr_E);
      TLine l_t_Q(Get_t_Q_E(), h_tigE.GetMinimum() - 10, Get_t_Q_E(), V_Q);
      TLine l_V_Q(0, V_Q, Get_t_Q_E(), V_Q);
      l_V_thr_E.SetLineColor(kRed);
      l_t_thr_E.SetLineColor(kRed);
      l_t_Q.SetLineColor(kRed);
      l_V_Q.SetLineColor(kRed);
      l_V_thr_E.Draw("same");
      if(Get_AboveThr_E()){
        l_t_thr_E.Draw("same");
        l_t_Q.Draw("same");
        l_V_Q.Draw("same");
      }

      c.cd(9);
      TPaveText text_tiger(0.2,0.2,0.8,0.8);
      text_tiger.AddText(Form("Tiger T thr = %.0f mV",V_thr_T));
      text_tiger.AddText(Form("Tiger E thr = %.0f mV",V_thr_E));
      text_tiger.AddText(Form("Time measured = %.2f ns",Get_Time()));
      text_tiger.AddText(Form("Charge measured = %.2f fC",Get_Charge()));
      text_tiger.AddText(Form("Voltage at Qmeas = %.2f mV",Get_Charge()*gain_TIGER));
      text_tiger.AddText(Form("Time at Qmeas = %.2f ns",Get_t_Q_E()));
      text_tiger.AddText(Form("Time at thr_E = %.2f ns",Get_t_thr_E()));
      text_tiger.AddText(Form("Max charge collected = %.2f fC",Get_Histo_tot()->GetMaximum()));
      text_tiger.Draw();
      c.SaveAs(Form("pdf/Event_%i_Channel_%i.pdf",ievent,Get_ChannelID()));
      c.SaveAs(Form("pdf/Event_%i_Channel_%i.png",ievent,Get_ChannelID()));
    };

    void      Print_Time_TORA   (int ievent) {
      TH1D h_raw  = h_time_raw.ToTH1D(Form("raw_%i_%i",type,channelID), Form("Current raw_%i_%i",type,channelID), "Time [ns]", "Current [1 fC / 1 ns]");
      TH1D h_tot  = h_time_tot.ToTH1D(Form("tot_%i_%i",type,channelID), Form("Charge[fC] tot_%i_%i",type,channelID), "Time [ns]", "Charge [fC]");
      TH1D h_cur  = h_time_cur.ToTH1D(Form("cur_%i_%i",type,channelID), Form("Current cur_%i_%i",type,channelID), "Time [ns]", "Current [1 fC / 1 ns]");
      TH1D h_res  = h_time_res.ToTH1D(Form("res_%i_%i",type,channelID), Form("Charge[fC] res_%i_%i",type,channelID), "Time [ns]", "Charge [fC]");
      TH1D h_tora = h_time_int_tora.ToTH1D(Form("tora_T_%i_%i",type,channelID), Form("Voltage[mV] tora_T_%i_%i",type,channelID), "Time [ns]", "Voltage [1 mV / 1 ns]");
      h_tora.GetYaxis()->SetRangeUser(-200, 900);

      TCanvas c("ccc","ccc",1600,1600);
      c.Divide(3,3);
      c.cd(1); h_raw.Draw("hist");
      c.cd(2); h_tot.Draw();
      c.cd(3);
      c.cd(4); h_cur.Draw("hist");
      c.cd(5); h_res.Draw();
      c.cd(6);
      c.cd(7); h_tora.Draw();

      float t_thr_T_val = Get_Time();
      float t_thr_Tfall = Get_t_falling_T();
      TLine l_t_thr_T(t_thr_T_val, h_tora.GetMinimum() - 10, t_thr_T_val, V_thrRise);
      TLine l_V_thr_T(0, V_thrRise, Get_AboveThr_T() ? t_thr_T_val : n_ns, V_thrRise);
      TLine l_t_thr_Tfall(t_thr_Tfall, h_tora.GetMinimum() - 10, t_thr_Tfall, V_thrTrail);
      TLine l_V_thr_Tfall(0, V_thrTrail, t_thr_Tfall, V_thrTrail);
      l_V_thr_T.SetLineColor(kRed);
      l_t_thr_T.SetLineColor(kRed);
      l_V_thr_Tfall.SetLineColor(kGreen);
      l_t_thr_Tfall.SetLineColor(kGreen);
      l_V_thr_T.Draw("same");
      if(Get_AboveThr_T()){
        l_t_thr_T.Draw("same");
        l_V_thr_Tfall.Draw("same");
        l_t_thr_Tfall.Draw("same");
      }

      c.cd(8);
      TPaveText text_tora(0.2,0.2,0.8,0.8);
      text_tora.AddText(Form("TORA thr Rising edge = %.0f mV",V_thrRise));
      text_tora.AddText(Form("TORA thr Trailing edge = %.0f mV",V_thrTrail));
      text_tora.AddText(Form("Gain TORA = %.1f mV/fC",Get_Gain_TORA()));
      text_tora.AddText(Form("Rising time = %.2f ns",Get_Time()));
      text_tora.AddText(Form("Falling time = %.2f ns",Get_t_falling_T()));
      text_tora.AddText(Form("ToT = %.2f ns",Get_t_falling_T()-Get_Time()));
      text_tora.AddText(Form("Charge measured = %.2f fC",Get_Charge()));
      text_tora.AddText(Form("Voltage at Qmeas = %.2f mV",Get_Charge()*Get_Gain_TORA()));
      text_tora.AddText(Form("Max charge collected = %.2f fC",Get_Histo_tot()->GetMaximum()));
      text_tora.Draw();
      c.SaveAs(Form("pdf/Event_%i_Channel_%i.pdf",ievent,Get_ChannelID()));
      c.SaveAs(Form("pdf/Event_%i_Channel_%i.png",ievent,Get_ChannelID()));
    };

    // Waveforms (value members backed by std::vector<double>)
    Waveform  h_time_raw;
    Waveform  h_time_tot;
    Waveform  h_time_res;
    Waveform  h_time_cap;
    Waveform  h_time_cur;
    Waveform  h_time_cur_FFT;
    Waveform  h_time_cur_buffer;
    Waveform  h_time_cur_buffer_FFT;
    Waveform  h_time_int_apv;
    Waveform  h_time_int_tigE;
    Waveform  h_time_int_tigT;
    Waveform  h_time_apv;
    Waveform  h_time_int_tora;
    // Constant
    const int nbin = 1000;
  private:
    bool      PrintNTuple;
    Position  position;
    int       ElectronicsType;
    int       channelID;
    int       type;   // 1=X, 2=Y
    double    pitch;
    double    charge; // fC
    double    time;   // ns
    double    dtime;  // ns
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
    float     V_thrRise;
    float     V_thrTrail;
    float     saturation_apv;
    float     saturation_tiger;
    float     saturation_tora;
    double    gain_TORA;
    void Set_Position(Position io) {position=io;};
  };
}
#endif
