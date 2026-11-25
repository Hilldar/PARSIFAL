#include <algorithm>
#include <omp.h>
#include "Resistive/Resistive.h"
using namespace std;
namespace PARSIFAL2{
  Resistive::Resistive(int setup,Geometry* geo, Readout* read):
    Setup(setup),
    geometry(geo),
    readout(read),
    PrintInfo(true),
    PrintNTuple(true)
  {
    r = new TRandom3();
    r->SetSeed(SEED);
    skip_resistive = NO_Resistive;

    sigma_res_diff=0.35;//si usa solo nel resistivo "smearing" e non quello reale
    
    //questi si usano davvero
    sigma0 = 0.001;//0.010;//10 µm;
    tau = 6; //18; //80ns era  5ns

    if(Get_Setup()==0){
      //triple-GEM setup has no resistive layer
      skip_resistive = true;
    }

    if(Get_Setup()!=0 && Get_Setup()!=1){
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<>      Resistive        <>-<>"<<endl;
      cout<<"<>-<>  Check the setup ID   <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
    }
    if(Get_PrintInfo()){
      cout<<"----     New Resistive         ----"<<endl;
      cout<<"Setup configuration              : "<<Get_Setup()<<endl;
      cout<<"----------------------"<<endl;
    }
  };

  void Resistive::Resist(){
    const int n_strip_resistive = 7;
    const int n_ch = readout->Get_N_Channels();
    if(skip_resistive) {
      #pragma omp parallel for
      for(int ich = 0; ich < n_ch; ich++){
        for(int it = 1; it <= n_ns; it++) {
          double raw_charge = readout->Get_Channel(ich)->Get_Histo_raw()->GetBinContent(it);
          readout->Get_Channel(ich)->Get_Histo_cur_buffer()->AddBinContent(it + n_ns_buffer, raw_charge);
        }
      }
    }
    else{
      // Precompute resistive charge-sharing kernel K(|strip_step|, dt) once per call
      // Assumes uniform pitch within each view (same pitch per channel type)
      const int max_dt = n_ns;
      // Map pitch -> kernel table [0..n_strip_resistive][0..max_dt-1]
      double pitch_x = 0.0, pitch_y = 0.0;
      for(int ich = 0; ich < n_ch; ich++){
        int t = readout->Get_Channel(ich)->Get_Type();
        if(t == Xview && pitch_x == 0.0) pitch_x = readout->Get_Channel(ich)->Get_Pitch();
        if(t == Yview && pitch_y == 0.0) pitch_y = readout->Get_Channel(ich)->Get_Pitch();
      }
      auto build_kernel = [&](double pitch) {
        std::vector<double> tbl((n_strip_resistive + 1) * max_dt, 0.0);
        if(pitch <= 0.0) return tbl;
        for(int step = 0; step <= n_strip_resistive; step++){
          double* row = &tbl[step * max_dt];
          for(int dt = 0; dt < max_dt; dt++){
            row[dt] = Q_t(pitch, step, 0.0, static_cast<double>(dt), 1.0);
          }
        }
        return tbl;
      };
      std::vector<double> kernel_x = build_kernel(pitch_x);
      std::vector<double> kernel_y = build_kernel(pitch_y);

      // Parallelize over destination channel jch so each thread writes only to its own histogram
      #pragma omp parallel for schedule(dynamic)
      for(int jch = 0; jch < n_ch; jch++){
        TH1D* h_res_j = readout->Get_Channel(jch)->Get_Histo_res();
        double* res_arr = h_res_j->GetArray();
        int type_j = readout->Get_Channel(jch)->Get_Type();
        const double* kernel_tbl = (type_j == Yview && !kernel_y.empty()) ? kernel_y.data() : kernel_x.data();
        const double expected_pitch = (type_j == Yview) ? pitch_y : pitch_x;
        int ich_min = std::max(0, jch - n_strip_resistive);
        int ich_max = std::min(n_ch - 1, jch + n_strip_resistive);
        // Loop over neighboring source channels symmetrically
        for(int ich = ich_min; ich <= ich_max; ich++){
          if(readout->Get_Channel(ich)->Get_Type() != type_j) continue;
          TH1D* h_raw_i = readout->Get_Channel(ich)->Get_Histo_raw();
          if(h_raw_i->GetMaximum() == 0) continue;
          const double* raw_arr = h_raw_i->GetArray();
          const int abs_step = std::abs(jch - ich);
          const double pitch = readout->Get_Channel(ich)->Get_Pitch();
          if(pitch == expected_pitch && pitch > 0.0){
            const double* k_row = &kernel_tbl[abs_step * max_dt];
            for(int it = 1; it <= n_ns; it++) {
              const double raw_charge = raw_arr[it];
              if(raw_charge == 0.0) continue;
              for(int jt = it; jt <= n_ns; jt++) {
                res_arr[jt] += raw_charge * k_row[jt - it];
              }
            }
          } else {
            for(int it = 1; it <= n_ns; it++) {
              const double raw_charge = raw_arr[it];
              if(raw_charge == 0.0) continue;
              for(int jt = it; jt <= n_ns; jt++) {
                res_arr[jt] += Q_t(pitch, jch - ich, it, jt, raw_charge);
              }
            }
          }
        }
      }
      // Fill resistive current histogram
      #pragma omp parallel for
      for(int ich = 0; ich < n_ch; ich++){
        TH1D* h_res = readout->Get_Channel(ich)->Get_Histo_res();
        if(h_res->GetMaximum() == 0) continue;
        const double* res_arr = h_res->GetArray();
        double* cur_buf_arr = readout->Get_Channel(ich)->Get_Histo_cur_buffer()->GetArray();
        for(int it = 1; it <= n_ns; it++) {
          cur_buf_arr[it + n_ns_buffer] += res_arr[it] - res_arr[it - 1];
        }
      }
    }
    return;
  }

  vector<Secondary*> Resistive::Diffusion(vector<Secondary*> pre_resistive){
    vector<Secondary*> post_resistive;
    if(pre_resistive.empty()) return post_resistive;
    for(int isec=0;isec<pre_resistive.size();isec++){
      for(int iele=0;iele<pre_resistive.at(isec)->Get_GainedElectrons();iele++){
        int ID = pre_resistive.at(isec)->Get_PrimaryID();
        Position pos_initi = pre_resistive.at(isec)->Get_PositionFinal();
        Position pos_final = Get_FinalPosition(&pos_initi);
        Secondary *second = new Secondary(ID,pos_initi,pos_final);
        post_resistive.push_back(second);
      }
    }
    return post_resistive;
  };

  Position* Resistive::Get_FinalPosition(Position *pos_init){
    if(NO_Resistive) return pos_init;
    double xi = pos_init->Get_X();
    double yi = pos_init->Get_Y();
    double zi = pos_init->Get_Z();
    double ti = pos_init->Get_T();
    double xf = xi + Get_Diff_X(xi);
    double yf = yi + Get_Diff_Y(yi);
    double zf = zi + Get_Diff_Z(zi);
    double tf = ti + Get_Diff_T(ti);
    Position* pos= new Position(xf,yf,zf,tf);
    return pos;
  };

  double Resistive::Get_Diff_X(double x){
    double dx=0;
    cout<<"SET THIS TO ZERO"<<endl;
    dx = r->Gaus(0,sigma_res_diff);
    return dx;
  };
  double Resistive::Get_Diff_Y(double y){
    double dy=0;
    cout<<"SET THIS TO ZERO"<<endl;
    dy = r->Gaus(0,sigma_res_diff);
    return dy;
  };
  double Resistive::Get_Diff_Z(double z){
    double dz=0;
    return dz;
  };
  double Resistive::Get_Diff_T(double t){
    double dt=0;
    return dt;
  };
}
