#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TF1.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

const int NSIGMA_SIGNAL = 5;

// Robust Gaussian fit with peak-centered bounded recovery when the default
// unconstrained fit drifts off-peak or collapses to a sub-bin spike.
static void FitBoundedGaussian(TH1* h, TF1* f, double min_sigma, double max_sigma){
  if(!h || !f) return;
  if(h->GetEntries() <= 0){
    f->SetParameters(0.0, 0.0, max_sigma);
    return;
  }
  const int max_bin   = h->GetMaximumBin();
  const double peak_x = h->GetBinCenter(max_bin);
  const double peak_y = h->GetBinContent(max_bin);
  const double bin_w  = h->GetBinWidth(max_bin);
  const double mean_lo = peak_x - 3.0 * bin_w;
  const double mean_hi = peak_x + 3.0 * bin_w;

  // Stage 1: preserve exact default fit behavior when it converges cleanly on the peak
  f->ReleaseParameter(0);
  f->ReleaseParameter(1);
  f->ReleaseParameter(2);
  h->Fit(f, "WMQN0");
  const double raw_mean  = f->GetParameter(1);
  const double raw_sigma = f->GetParameter(2);
  if(raw_sigma >= min_sigma && raw_sigma <= max_sigma &&
     raw_mean >= mean_lo && raw_mean <= mean_hi){
    return;
  }

  // Stage 2: recover from off-peak drift or sub-bin collapse using peak-seeded bounds
  int left_bin = max_bin;
  while(left_bin > 1 && h->GetBinContent(left_bin) > 0.5 * peak_y) left_bin--;
  int right_bin = max_bin;
  while(right_bin < h->GetNbinsX() && h->GetBinContent(right_bin) > 0.5 * peak_y) right_bin++;
  const double fwhm_sigma = ((right_bin - left_bin) * bin_w) / 2.35482;
  double init_sigma = (fwhm_sigma >= min_sigma) ? fwhm_sigma : h->GetRMS();
  init_sigma = std::clamp(init_sigma, min_sigma, max_sigma);

  f->SetParameters(peak_y, peak_x, init_sigma);
  f->SetParLimits(0, 0.0, peak_y * 5.0 + 1.0);
  f->SetParLimits(1, mean_lo, mean_hi);
  f->SetParLimits(2, min_sigma, max_sigma);

  // Omit "M" (Improve) here so status == 0 directly indicates Migrad convergence
  int status = h->Fit(f, "WQBN0");
  const double fit_mean  = f->GetParameter(1);
  const double fit_sigma = std::fabs(f->GetParameter(2));
  if(status != 0 || fit_sigma < min_sigma || fit_sigma > max_sigma ||
     fit_mean < mean_lo || fit_mean > mean_hi){
    f->SetParameters(peak_y, peak_x, init_sigma);
  }
}

static string GetDataDir(){
  const char *env = getenv("PARSIFAL_data");
  return (env && env[0] != '\0') ? string(env) : string("data");
}

bool Check_File(TString name, TTree *tree, TFile *file);

void Run_i(string ifolder, int iangle, bool append = false){

  int folder = atoi(ifolder.c_str());
  string data_dir = GetDataDir();
  ofstream outfile;
  outfile.open(Form("%s/%i/summary.txt",data_dir.c_str(),folder), append ? ios::app : ios::trunc);
  TString name_ang = Form("%s/%i/run_angle%i.root",data_dir.c_str(),folder,(int)iangle);
  TFile *infile = new TFile(name_ang);
    TTree *intree = (TTree*)infile->Get("tree");
    if(Check_File(name_ang,intree,infile)){
      if(intree->GetEntries()){
	// Raw residual CC for cut (use goff / N0 to avoid creating a TCanvas)
	TH1D *h_CC = new TH1D("h_CC","h_CC",100,-2,5);
        intree->Draw("cluster1d_positionCC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_CC","cluster1d_size>0 && cluster1d_type==1 && abs(cluster1d_positionCC_X-0.5*(ionio_posF[0]+ionio_posI[0]))<5 && cluster1d_high_Q","goff");
        TF1 *f_gaus_CC = new TF1 ("f_gaus_CC","gaus");
        // Min sigma = 0.5 * bin_width (0.035 mm = 35 um) prevents sub-bin spike collapse
        const double cc_min_sigma = 0.5 * h_CC->GetBinWidth(1);
        FitBoundedGaussian(h_CC, f_gaus_CC, cc_min_sigma, 2.0);
	// Selection cut
	TString cut = Form("cluster1d_size>0 && cluster1d_type==1 && abs(cluster1d_positionCC_X-0.5*(ionio_posF[0]+ionio_posI[0])-%f)<%f && cluster1d_high_Q",f_gaus_CC->GetParameter(1),f_gaus_CC->GetParameter(2)*NSIGMA_SIGNAL);
        cout<<cut<<endl;
	// Induced charge (MPV) with bounded Landau fit and peak fallback
	TF1* f_landau = new TF1("f_landau", "landau");
        double max_e = intree->GetMaximum("tot_electron_gained");
        if(max_e <= 0) max_e = 1.0;
        TH1I* tot_e = new TH1I("tot_e", "tot_e", 400, 0, max_e);
        intree->Draw("tot_electron_gained>>tot_e", cut, "goff");
        double qind_mpv = 0.0;
        if(tot_e->GetEntries() > 0){
          double peak_x = tot_e->GetBinCenter(tot_e->GetMaximumBin());
          double rms_x  = tot_e->GetRMS();
          f_landau->SetParameters(tot_e->GetMaximum(), peak_x, rms_x > 0 ? 0.25 * rms_x : 0.05 * max_e);
          f_landau->SetParLimits(1, 0.0, max_e);
          f_landau->SetParLimits(2, 0.0, max_e);
          int fit_status = tot_e->Fit("f_landau", "WMQBN0");
          double mpv_e = f_landau->GetParameter(1);
          // Fall back to histogram peak bin if Landau fit failed or hit boundary
          if(fit_status == 0 && mpv_e > 0.0 && mpv_e < max_e) qind_mpv = mpv_e * 1.6e-4;
          else qind_mpv = peak_x * 1.6e-4;
        }
	// Cluster charge
	TH1D *h_charge = new TH1D("h_charge","h_charge",100,0,1000);
        intree->Draw("cluster1d_charge>>h_charge",cut,"goff");
	// Cluster charge / induced charge
	TH1D* rel_charge = new TH1D("rel_charge", "rel_charge", 100, 0, 1);
        intree->Draw("cluster1d_charge/(tot_electron_gained*1.6e-4)>>rel_charge", cut, "goff");
	// Size
	TH1I *h_size = new TH1I("h_size","h_size",20,0,20);
        intree->Draw("cluster1d_size>>h_size",cut,"goff");
	// Time0
	TH1F *h_time0 = new TH1F("h_time0","h_time0",100,0,100);
        intree->Draw("cluster1d_time0>>h_time0",cut,"goff");
        TF1 *f_gaus_time0 = new TF1("f_gaus_time0","gaus");
        h_time0->Fit("f_gaus_time0","WMQN0");
	// Residual TPC
	TH1D *h_TPC = new TH1D("h_TPC","h_TPC",100,-2,4);
        intree->Draw("cluster1d_positionTPC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_TPC",cut,"goff");
        TF1 *f_gaus_TPC = new TF1 ("f_gaus_TPC","gaus");
        h_TPC->Fit("f_gaus_TPC","WMQN0");
	// Residual CC
	intree->Draw("cluster1d_positionCC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_CC",cut,"goff");
        FitBoundedGaussian(h_CC, f_gaus_CC, cc_min_sigma, 2.0);
	// Write summary metrics
	float eff = (float)(intree->GetEntries(cut))/intree->GetEntries();
	if(f_gaus_CC->GetParameter(2)*1000>500) eff = 0;
	outfile<<Form("%i \t %.4f \t %.4f \t %.4f \t %.4f \t %.0f \t \t %.0f \t \t %.1f \t \t %.2f",iangle,qind_mpv,h_charge->GetMean(),rel_charge->GetMean(),h_size->GetMean(),f_gaus_CC->GetParameter(2)*1000,f_gaus_TPC->GetParameter(2)*1000,f_gaus_time0->GetParameter(2),eff)<<endl;
	}
      else
        cout<<name_ang<<" has no entries"<<endl;
    }
    delete infile;
}


int main(int argc, const char* argv[]){
  if(argc<2) {
    cerr<<"RUN the analysis with the configuration ID"<<endl;
    return 1;
  }
  if(argc<3){
    string folder = argv[1];
    const int n_angle = 7;
    int angles[n_angle]={0,5,7,10,15,20,30};
    // Truncate summary file once before running all angles sequentially to avoid write races
    string data_dir = GetDataDir();
    ofstream(Form("%s/%i/summary.txt", data_dir.c_str(), atoi(folder.c_str())), ios::trunc).close();
    for(int iangle=0;iangle<n_angle;iangle++){
      cout<<"RUN "<<folder<<" angle: "<<angles[iangle]<<endl;
      Run_i(folder, angles[iangle], true);
    }
  }
  else {
    string folder = argv[1];
    int angle = atoi(argv[2]);
    Run_i(folder, angle, false);
  }
  return 0;
};


  
/*
int main(int argc, const char* argv[]){
  if(argc<2) {
      cout<<"RUN the analysis with the configuration ID"<<endl;
      return 0;
  }
  int folder =atoi(argv[1]);
  ofstream outfile;
  outfile.open(Form("data/%i/summary.txt",folder));
  const int n_angle = 8;
  int angles[n_angle]={0,5,7,10,15,20,30,40};
  for(int iangle=0;iangle<7;iangle++){
    int angle = angles[iangle];
    double aangle = angle*TMath::DegToRad();
    TString name_ang = Form("data/%i/run_angle%i.root",folder,(int)angle);
    TFile *infile = new TFile(name_ang);
    TTree *intree = (TTree*)infile->Get("tree");
    if(Check_File(name_ang,intree,infile)){
      if(intree->GetEntries()){
	//Raw residual CC for cut
	TH1D *h_CC = new TH1D("h_CC","h_CC",100,-2,5);
	intree->Draw("cluster1d_positionCC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_CC","cluster1d_size>0 && cluster1d_type==1 && abs(cluster1d_positionCC_X-0.5*(ionio_posF[0]+ionio_posI[0]))<5 && cluster1d_high_Q");
	TF1 *f_gaus_CC = new TF1 ("f_gaus_CC","gaus");
	h_CC->Fit("f_gaus_CC","WMQ");
	//the cut
	TString cut = Form("cluster1d_size>0 && cluster1d_type==1 && abs(cluster1d_positionCC_X-0.5*(ionio_posF[0]+ionio_posI[0])-%f)<%f && cluster1d_high_Q",f_gaus_CC->GetParameter(1),f_gaus_CC->GetParameter(2)*NSIGMA_SIGNAL);
	cout<<cut<<endl;
	//inducedCharge (MPV)
	TF1* f_landau = new TF1("f_landau", "landau");
	double max_e = intree->GetMaximum("tot_electron_gained");
	TH1I* tot_e = new TH1I("tot_e", "tot_e", 400, 0, max_e);
	intree->Draw("tot_electron_gained>>tot_e", cut);
	tot_e->Fit("f_landau", "WMQ");
	//clusterCharge
	TH1D *h_charge = new TH1D("h_charge","h_charge",100,0,1000);
	intree->Draw("cluster1d_charge>>h_charge",cut);
	//clusterCharge/inducedCharge
	TH1D* rel_charge = new TH1D("rel_charge", "rel_charge", 100, 0, 1);
	intree->Draw("cluster1d_charge/(tot_electron_gained*1.6e-4)>>rel_charge", cut);
	//Size
	TH1I *h_size = new TH1I("h_size","h_size",20,0,20);
	intree->Draw("cluster1d_size>>h_size",cut);
	//Time0
	TH1F *h_time0 = new TH1F("h_time0","h_time0",100,0,100);
	intree->Draw("cluster1d_time0>>h_time0",cut);
	TF1 *f_gaus_time0 = new TF1("f_gaus_time0","gaus");
	h_time0->Fit("f_gaus_time0","WMQ");
	//Residual TPC
	TH1D *h_TPC = new TH1D("h_TPC","h_TPC",100,-2,4);
	intree->Draw("cluster1d_positionTPC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_TPC",cut);
	TF1 *f_gaus_TPC = new TF1 ("f_gaus_TPC","gaus");
	h_TPC->Fit("f_gaus_TPC","WMQ");
	//Residual CC
	intree->Draw("cluster1d_positionCC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_CC",cut);
	h_CC->Fit("f_gaus_CC","WMQ");

	//echo -e "Angle \t Qind MPV \t Qread \t RatioQ \t Size \t CCres \t TPCres \t T0res"
	outfile<<Form("%i \t %.4f \t %.4f \t %.4f \t %.4f \t %.0f \t \t %.0f \t \t %.1f \t \t %.2f",angle,f_landau->GetParameter(1) * 1.6e-4,h_charge->GetMean(),rel_charge->GetMean(),h_size->GetMean(),f_gaus_CC->GetParameter(2)*1000,f_gaus_TPC->GetParameter(2)*1000,f_gaus_time0->GetParameter(2),(float)(intree->GetEntries(cut))/intree->GetEntries())<<endl;
	//outfile<<angle<<" \t "<< f_landau->GetParameter(1) * 1.6e-4 << " \t " << h_charge->GetMean() << " \t " << rel_charge->GetMean() << " \t " << h_size->GetMean() <<" \t "<< f_gaus_CC->GetParameter(2) * 1000 <<" \t  "<< f_gaus_TPC->GetParameter(2) * 1000 << " \t " << f_gaus_time0->GetParameter(2) << endl;
      }
      else
	cout<<name_ang<<" has no entries"<<endl;
    }
  }
  return 1;
};
*/
bool Check_File(TString name, TTree *tree, TFile *file){
  bool b1(0),b2(0),b3(0);
  b1=file->IsOpen();
  b2=(bool)tree;
  if(b2) b3=tree->IsFolder();
  if(b1 && b2 && b3) {
    cout<<"Init file "<<name<<endl;
    return true;
  }
  else {
    cout<<"ERROR in file "<<name<<" "<<b1<<b2<<b3<<endl;
    return false;
  }
}
