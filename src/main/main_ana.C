#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TF1.h"
#include <iostream>
#include <fstream>
using namespace std;

const int NSIGMA_SIGNAL = 5;

bool Check_File(TString name, TTree *tree, TFile *file);

void Run_i(string ifolder, int iangle){

  int folder = atoi(ifolder.c_str());
  ofstream outfile;
  outfile.open(Form("data/%i/summary.txt",folder));
  double aangle = iangle*TMath::DegToRad();
  TString name_ang = Form("data/%i/run_angle%i.root",folder,(int)iangle);
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
	float eff = (float)(intree->GetEntries(cut))/intree->GetEntries();
	if(f_gaus_CC->GetParameter(2)*1000>500) eff = 0;
	outfile<<Form("%i \t %.4f \t %.4f \t %.4f \t %.4f \t %.0f \t \t %.0f \t \t %.1f \t \t %.2f",iangle,f_landau->GetParameter(1) * 1.6e-4,h_charge->GetMean(),rel_charge->GetMean(),h_size->GetMean(),f_gaus_CC->GetParameter(2)*1000,f_gaus_TPC->GetParameter(2)*1000,f_gaus_time0->GetParameter(2),eff)<<endl;
	//outfile<<angle<<" \t "<< f_landau->GetParameter(1) * 1.6e-4 << " \t " << h_charge->GetMean() << " \t " << rel_charge->GetMean() << " \t " << h_size->GetMean() <<" \t "<< f_gaus_CC->GetParameter(2) * 1000 <<" \t  "<< f_gaus_TPC->GetParameter(2) * 1000 << " \t " << f_gaus_time0->GetParameter(2) << endl;
	}
      else
        cout<<name_ang<<" has no entries"<<endl;
    }
}


int main(int argc, const char* argv[]){                                                                                                                                                                                                                                            
  if(argc<2) {
    cout<<"RUN the analysis with the configuration ID"<<endl;
    return 0;
  }
  if(argc<3){
    string folder = argv[1];
    const int n_angle = 7;
    int angles[n_angle]={0,5,7,10,15,20,30};
    for(int iangle=0;iangle<n_angle;iangle++){
      gSystem->Exec(Form("echo RUN %s angle: %i",folder.c_str(),angles[iangle]));
      gSystem->Exec(Form("./bin/Analysis %s %i &",folder.c_str(),angles[iangle]));
    }
  }
  else {
    string folder = argv[1];
    int angle =atoi(argv[2]);
    Run_i(folder,angle);
  }
  return 1;
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
  if(b1*b2*b3) {
    cout<<"Init file "<<name<<endl;
    return true;
  }
  else {
    cout<<"ERROR in file "<<name<<" "<<b1<<b2<<b3<<endl;
    return false;
  }
}
