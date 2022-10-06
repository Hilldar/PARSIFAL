/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Ionization/main.h"
using namespace std;
using namespace PARSIFAL2;

int main(int argc, const char* argv[]){
  //Initialization
  cout<<"Ciao"<<endl;
  TString name_outfile="ciao.root";
  file = new TFile(name_outfile,"RECREATE");
  tree = new TTree("tree","tree");
  geo = Planar2D(0.4, 0.4,5);
  D4 = new Position(1,2,3,-0.1);
  party = new Particle(1, D4, 0.1, -0.2, geo);
  ionio = new Ionization(party,geo); 
  primi.push_back(new Primary());
  Initialize_oFile();
  //
  //Run
  for(int irun=0;irun<100;irun++){
    primi = ionio->PrimaryIonization();
    Write_oFile();
  }
  //
  //Terminate
  file->Write();
  file->Close();
  cout<<"---- Output file ----"<<endl;
  cout<<"File written: "<<name_outfile<<endl;
  cout<<"----------------------"<<endl;      
  return 0;
};

void PARSIFAL2::Initialize_oFile(){
  if(geo->Get_PrintNTuple()){
    tree->Branch("geo_numview" ,&geo_numview  ,"geo_numview/I");
    tree->Branch("geo_pitch1"  ,&geo_pitch1   ,"geo_pitch1/D");
    tree->Branch("geo_pitch2"  ,&geo_pitch2   ,"geo_pitch2/D");
    tree->Branch("geo_driftgap",&geo_driftgap ,"geo_driftgap/D");
  }
  if(party->Get_PrintNTuple()){
    tree->Branch("party_partID"  ,&party_partID  ,"party_partID/I");
    tree->Branch("party_angleXZ" ,&party_angleXZ ,"party_angleXZ/D");
    tree->Branch("party_angleYZ" ,&party_angleYZ ,"party_angleYZ/D");
    tree->Branch("party_primary" ,&party_primary ,"party_primary/D");
    tree->Branch("party_position", party_pos     ,"party_position[4]/D");
    tree->Branch("party_prob"    , party_prob    ,"party_prob[100]/D");
  }
  if(ionio->Get_PrintNTuple()){
    tree->Branch("ionio_positionI", ionio_positionI ,"ionio_posI[4]/D");
    tree->Branch("ionio_positionI", ionio_positionF ,"ionio_posI[4]/D");
    tree->Branch("ionio_primary"  ,&ionio_primary   ,"ionio_primary/I");
    tree->Branch("ionio_secondary",&ionio_secondary ,"ionio_secondary/I");
  }
  if(primi.at(0)->Get_PrintNTuple()){
    tree->Branch("primary_positionX" , "vector<double>" ,&primary_positionX);
    tree->Branch("primary_positionY" , "vector<double>" ,&primary_positionY);
    tree->Branch("primary_positionZ" , "vector<double>" ,&primary_positionZ);
    tree->Branch("primary_positionT" , "vector<double>" ,&primary_positionT);
    tree->Branch("primary_secondary" , "vector<int>"    ,&primary_secondary);
  }
  return;  
}

void PARSIFAL2::Write_oFile(){
  //Output
  if(geo->Get_PrintNTuple()){
    geo_numview  = geo->Get_NumberOfView();
    geo_pitch1   = geo->Get_Pitch1();
    geo_pitch2   = geo->Get_Pitch2();
    geo_driftgap = geo->Get_DriftGap();
  }
  if(party->Get_PrintNTuple()){
    party_partID   = party->Get_ParticleID();
    party_angleXZ  = party->Get_AngleXZ();
    party_angleYZ  = party->Get_AngleYZ();
    party_primary  = party->Get_MeanPrimary();
    party_pos[0]   = party->Get_Position()->Get_X();
    party_pos[1]   = party->Get_Position()->Get_Y();
    party_pos[2]   = party->Get_Position()->Get_Z();
    party_pos[3]   = party->Get_Position()->Get_T();
    for(int i=0;i<100;i++) party_prob[i] = party->Get_ProbabilitySecondary(i);
  }
  if(ionio->Get_PrintNTuple()){
    ionio_positionI[0] = ionio->Get_PositionInitial()->Get_X();
    ionio_positionI[1] = ionio->Get_PositionInitial()->Get_Y();
    ionio_positionI[2] = ionio->Get_PositionInitial()->Get_Z();
    ionio_positionI[3] = ionio->Get_PositionInitial()->Get_T();
    ionio_positionF[0] = ionio->Get_PositionFinal()->Get_X();
    ionio_positionF[1] = ionio->Get_PositionFinal()->Get_Y();
    ionio_positionF[2] = ionio->Get_PositionFinal()->Get_Z();
    ionio_positionF[3] = ionio->Get_PositionFinal()->Get_T();
    ionio_primary      = ionio->Get_TotalNumberOfPrimary();
    ionio_secondary    = ionio->Get_TotalNumberOfSecondary();
  }
  if(primi.at(0)->Get_PrintNTuple()){
    for(int i=0;i<ionio->Get_TotalNumberOfPrimary();i++){
      primary_positionX.push_back(primi.at(i)->Get_Position()->Get_X());
      primary_positionY.push_back(primi.at(i)->Get_Position()->Get_Y());
      primary_positionZ.push_back(primi.at(i)->Get_Position()->Get_Z());
      primary_positionT.push_back(primi.at(i)->Get_Position()->Get_T());
      primary_secondary.push_back(primi.at(i)->Get_NumberOfSecondary());
    }
  }
  tree->Fill();
  return;  
}

