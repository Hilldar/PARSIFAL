/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH1D.h"
#include "TF1.h"
#include <iostream>
#include <fstream>
using namespace std;

bool run_parallel = false;
bool Check_File(TString name, TTree *tree, TFile *file);


int main(int argc, const char* argv[]){
  if(argc<2) {
      cout<<"RUN the analysis with the configuration ID"<<endl;
      return 0;
  }
  if(argc<3){
    int folder =atoi(argv[1]);
    ofstream outfile;
    outfile.open(Form("data/%i/summary.txt",folder));
    int angles[7]={0,5,10,15,20,30,40};
    //for(int iangle=0;iangle<=40;iangle+=10){ 
    for(int iangle=0;iangle<7;iangle++){
      if(run_parallel){
	//gSystem->Exec(Form("ts -n $exe_parsifal2 -A %i %i",folder,iangle));
	gSystem->Exec(Form("ts -n $exe_parsifal2 -A %i %i",folder,angles[iangle]));
	gSystem->Exec("sleep 1");
      }
      else{
	int angle = angles[iangle];
	double aangle = angle*TMath::DegToRad();
	TString name_ang = Form("data/%i/run_angle%i.root",folder,(int)angle);
	TFile *infile = new TFile(name_ang);
	TTree *intree = (TTree*)infile->Get("tree");
	if(Check_File(name_ang,intree,infile)){
	  if(intree->GetEntries()){
	    TH1D *h_CC = new TH1D("h_CC","h_CC",500,-2,4);
	    intree->Draw("cluster1d_positionCC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_CC","cluster1d_size>0 && cluster1d_type==1 && abs(cluster1d_positionCC_X-0.5*(ionio_posF[0]+ionio_posI[0]))<5");
	    TF1 *f_gaus_CC = new TF1 ("f_gaus_CC","gaus");
            h_CC->Fit("f_gaus_CC","WQ");
	    TString cut = Form("cluster1d_size>0 && cluster1d_type==1 && abs(cluster1d_positionCC_X-0.5*(ionio_posF[0]+ionio_posI[0])-%f)<%f",f_gaus_CC->GetParameter(1),f_gaus_CC->GetParameter(2)*5);
	    TH1D *h_charge = new TH1D("h_charge","h_charge",100,0,300);
	    intree->Draw("cluster1d_charge>>h_charge",cut);
	    TH1I *h_size = new TH1I("h_size","h_size",20,0,20);
	    intree->Draw("cluster1d_size>>h_size",cut);
	    TH1D *h_TPC = new TH1D("h_TPC","h_TPC",500,-2,4);
	    intree->Draw("cluster1d_positionTPC_X-(ionio_posI[0]+ionio_posF[0])/2>>h_TPC",cut);
	    TF1 *f_gaus_TPC = new TF1 ("f_gaus_TPC","gaus");
	    h_TPC->Fit("f_gaus_TPC","WQ");
	    outfile<<angle<<" "<<h_charge->GetMean()<<" "<<h_size->GetMean()<<" "<<f_gaus_CC->GetParameter(2)*1000<<" "<<f_gaus_TPC->GetParameter(2)*1000<<endl;
	  }
	  else
	    cout<<name_ang<<" has no entries"<<endl;
	}
      }
    }
  }
  else {
    int angle =atoi(argv[1]);
    double aangle = angle*TMath::DegToRad();
    string name_ang = Form("run_angle%i.root",(int)angle);
  }
  return 1;
};

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
    cout<<"ERROR in file "<<name<<b1<<b2<<b3<<endl;
    return false;
  }
}
