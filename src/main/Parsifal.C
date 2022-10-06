/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "main/Parsifal.h"
using namespace PARSIFAL2;

PARSIFAL::PARSIFAL() {
}

PARSIFAL::~PARSIFAL(){
  //Clean(); --> it crashes
  delete geo;
  delete D4;
  delete party;
  delete ionio;
  delete signal;
  delete recon;
  // ----------------------------
  //  delete tree; // do not try to delete it is automatically deleted wwith the file!
  delete file;
}

void PARSIFAL::Initialization_tripleGEM(){
  cout<<"Ciao"<<endl;
  file = new TFile(name_outfile,"RECREATE");
  tree = new TTree("tree","tree");
  geo = Planar2D(0.65, 0.65, 5);
  D4 = new Position(0,0,0,0);
  party = new Particle(1, D4, party_angle_xz[0], -0.0, geo); 
  ionio = new Ionization(party,geo);
  signal = new Signal(0,0,Bfield,geo);
  recon = new Reconstruction(0,geo);
  event=0;
  Initialize_oFile();
  return;
}

void PARSIFAL::Initialization_rwell(){
  cout<<"Ciao"<<endl;
  file = new TFile(name_outfile,"RECREATE");
  tree = new TTree("tree","tree");
  geo = Planar1D(0.4, 6);
  D4 = new Position(0,0,0,0);
  party = new Particle(2, D4, party_angle_xz[0], -0.0, geo);
  ionio = new Ionization(party,geo);
  signal = new Signal(1,0,Bfield,geo);
  recon = new Reconstruction(1,geo);
  event=0;
  Initialize_oFile();
  return;
}


void PARSIFAL::Run(){
  TRandom3 *r = new TRandom3();
  r->SetSeed(SEED); // CHECK
  for(event=0;event<nShots;event++){
    //
    //Init
    Clean();
    party->Get_Position()->Set_X(r->Rndm()*geo->Get_Pitch1());
    party->Get_Position()->Set_Y(r->Rndm()*geo->Get_Pitch2());
    ionio = new Ionization(party,geo);
    //
    //Process 
    //Ionization
    primi     = ionio->PrimaryIonization();
    if(fast_simulation){
      //--> I have to create function from "primi" to "channels" 
      //--> then I have to reduce the Read function 
      channels = signal->Generate_Signal(primi);
    }
    else{
      //Create the electrons from the primary clusters
      secondary = ionio->SecondaryIonization(primi);
      //Amplification and drift to the anode
      secondary           = signal->Gain(secondary);
      electrons           = signal->Drift(secondary);
      electrons_resistive = signal->Resist(electrons);
      //Readout
      channels  = signal->Read(electrons_resistive);
    }
    //
    //Process
    //Reconstruction
    recon->Get_Ionization(ionio); //Debug uTPC
    recon->Get_Primary(primi);    //Debug uTPC
    hit       = recon->Digitize(channels);
    cluster1d = recon->Clusterize(hit);
    //
    //Terminate
    if(hit.empty()) nhit = 0;
    else            nhit = hit.size();
    if(nhit)  ncluster1d = cluster1d.size();
    else      ncluster1d = 0;
    Write_oFile();  
    cout<<"Event "<<event<<endl;
    MemInfo_t info;
    if((double) info.fMemUsed/info.fMemTotal > 0.9) {
      cout << "TOO MUCH MEMORY USED! " << (double) info.fMemUsed/info.fMemTotal <<  endl;
      break;
    }
    FreeMemory();
  }
  delete r;
  return;
}


void PARSIFAL::Terminate(){
  file->Write();
  file->Close();
  cout<<"---- Output file ----"<<endl;
  cout<<"File written: "<<name_outfile<<endl;
  return;
}

void PARSIFAL::Clean(){
  nhit=0;
  ncluster1d=0;
  primi.clear();
  secondary.clear();
  electrons.clear();
  for(int i=0;i<channels.size();i++) channels.at(i)->Reset(); // CHECK not needed if I use the FreeMemory
  channels.clear();
  hit.clear();
  cluster1d.clear();
  primary_positionX.clear();
  primary_positionY.clear();
  primary_positionZ.clear();
  primary_positionT.clear();
  primary_secondary.clear();
  primary_secondary_gain.clear();
  primary_ID.clear();
  secondary_gain.clear();
  electron_positionX_initial.clear();
  electron_positionY_initial.clear();
  electron_positionZ_initial.clear();
  electron_positionT_initial.clear();
  electron_positionX_final.clear();
  electron_positionY_final.clear();
  electron_positionZ_final.clear();
  electron_positionT_final.clear();
  electron_primary_ID.clear();
  electron_electronic_channel.clear();
  channel_ID.clear();
  channel_type.clear();
  channel_charge.clear();
  channel_time.clear();
  channel_dtime.clear();
  channel_positionX.clear();
  channel_positionY.clear();
  hit_ID.clear();
  hit_channelID.clear();
  hit_clusterID.clear();
  hit_type.clear();
  hit_charge.clear();
  hit_time.clear();
  hit_dtime.clear();
  hit_positionX.clear();
  hit_positionY.clear();
  cluster1d_ID.clear();
  cluster1d_type.clear();
  cluster1d_size.clear();
  cluster1d_charge.clear();
  cluster1d_positionCC_X.clear();
  cluster1d_positionCC_Y.clear();
  cluster1d_positionTPC_X.clear();
  cluster1d_positionTPC_Y.clear();
}
void PARSIFAL::FreeMemory() {
  if(cluster1d.size()>0) {   for (vector<Cluster1D*>::iterator itr = cluster1d.begin(); itr != cluster1d.end(); itr++) { delete *itr; }}
  if(hit.size()>0) {  for (vector<Hit*>::iterator itr = hit.begin(); itr != hit.end(); itr++) { delete *itr; }}
  if(electrons.size()>0) {  for (vector<Secondary*>::iterator itr = electrons.begin(); itr != electrons.end(); itr++) { delete *itr; }}
  if(secondary.size()>0) {  for (vector<Secondary*>::iterator itr = secondary.begin(); itr != secondary.end(); itr++) { delete *itr; }}
  if(primi.size()>0) {   for (vector<Primary*>::iterator itr = primi.begin(); itr != primi.end(); itr++) { delete *itr;  }}
}

void PARSIFAL::Initialize_oFile(){
  tree->Branch("event" ,&event  ,"event/I");  
  if(PrintNTuple_Geo){
    tree->Branch("geo_numview" ,&geo_numview  ,"geo_numview/I");
    tree->Branch("geo_pitch1"  ,&geo_pitch1   ,"geo_pitch1/D");
    tree->Branch("geo_pitch2"  ,&geo_pitch2   ,"geo_pitch2/D");
    tree->Branch("geo_driftgap",&geo_driftgap ,"geo_driftgap/D");
  }
  if(PrintNTuple_Party){
    tree->Branch("party_partID"   ,&party_partID  ,"party_partID/I");
    tree->Branch("party_angleXZ"  ,&party_angleXZ ,"party_angleXZ/D");
    tree->Branch("party_angleYZ"  ,&party_angleYZ ,"party_angleYZ/D");
    tree->Branch("party_primary"  ,&party_primary ,"party_primary/D");
    tree->Branch("party_positionI", party_posI    ,"party_positionI[4]/D");
    tree->Branch("party_prob"     , party_prob    ,"party_prob[100]/D");
  }
  if(PrintNTuple_Ionio){
    tree->Branch("ionio_positionI", ionio_positionI ,"ionio_posI[4]/D");
    tree->Branch("ionio_positionF", ionio_positionF ,"ionio_posF[4]/D");
    tree->Branch("ionio_primary"  ,&ionio_primary   ,"ionio_primary/I");
    tree->Branch("ionio_secondary",&ionio_secondary ,"ionio_secondary/I");
  }
  if(PrintNTuple_Primary){
    tree->Branch("primary_positionX" , "vector<double>" ,&primary_positionX);
    tree->Branch("primary_positionY" , "vector<double>" ,&primary_positionY);
    tree->Branch("primary_positionZ" , "vector<double>" ,&primary_positionZ);
    tree->Branch("primary_positionT" , "vector<double>" ,&primary_positionT);
    tree->Branch("primary_secondary" , "vector<int>"    ,&primary_secondary);
    tree->Branch("primary_ID"        , "vector<int>"    ,&primary_ID);
    tree->Branch("secondary_gain"    , "vector<int>"    ,&secondary_gain);
  }
  if(PrintNTuple_Secondary){
    tree->Branch("electron_positionX_initial"   , "vector<double>" ,&electron_positionX_initial);
    tree->Branch("electron_positionY_initial"   , "vector<double>" ,&electron_positionY_initial);
    tree->Branch("electron_positionZ_initial"   , "vector<double>" ,&electron_positionZ_initial);
    tree->Branch("electron_positionT_initial"   , "vector<double>" ,&electron_positionT_initial);
    tree->Branch("electron_positionX_final"     , "vector<double>" ,&electron_positionX_final);
    tree->Branch("electron_positionY_final"     , "vector<double>" ,&electron_positionY_final);
    tree->Branch("electron_positionZ_final"     , "vector<double>" ,&electron_positionZ_final);
    tree->Branch("electron_positionT_final"     , "vector<double>" ,&electron_positionT_final);
    tree->Branch("electron_primary_ID"          , "vector<int>"    ,&electron_primary_ID);
    tree->Branch("electron_electronic_channel"  , "vector<int>"    ,&electron_electronic_channel);  
  }
  if(PrintNTuple_Channel){
    tree->Branch("channel_ID"        , "vector<int>"    ,&channel_ID);  
    tree->Branch("channel_type"      , "vector<int>"    ,&channel_type);  
    tree->Branch("channel_charge"    , "vector<double>" ,&channel_charge);
    tree->Branch("channel_time"      , "vector<double>" ,&channel_time);
    tree->Branch("channel_dtime"     , "vector<double>" ,&channel_dtime);
    tree->Branch("channel_positionX" , "vector<double>" ,&channel_positionX);
    tree->Branch("channel_positionY" , "vector<double>" ,&channel_positionY);
  }
  if(PrintNTuple_Hit){
    tree->Branch("nhit"          , &nhit            ,"nhit/I");  
    tree->Branch("hit_ID"        , "vector<int>"    ,&hit_ID);  
    tree->Branch("hit_channelID" , "vector<int>"    ,&hit_channelID);
    tree->Branch("hit_clusterID" , "vector<int>"    ,&hit_clusterID);  
    tree->Branch("hit_type"      , "vector<int>"    ,&hit_type);  
    tree->Branch("hit_charge"    , "vector<double>" ,&hit_charge);
    tree->Branch("hit_time"      , "vector<double>" ,&hit_time);
    tree->Branch("hit_dtime"     , "vector<double>" ,&hit_dtime);
    tree->Branch("hit_positionX" , "vector<double>" ,&hit_positionX);
    tree->Branch("hit_positionY" , "vector<double>" ,&hit_positionY);
  }  
  if(PrintNTuple_Cluster1d){
    tree->Branch("ncluster1d"             , &ncluster1d           ,"ncluster1d/I");  
    tree->Branch("cluster1d_ID"           , "vector<int>"         ,&cluster1d_ID);
    //tree->Branch("cluster1d_hitID"        , "vector<vector<int>>" ,&cluster1d_hitID);
    tree->Branch("cluster1d_type"         , "vector<int>"         ,&cluster1d_type);
    tree->Branch("cluster1d_size"         , "vector<int>"         ,&cluster1d_size);
    tree->Branch("cluster1d_charge"       , "vector<double>"      ,&cluster1d_charge);
    tree->Branch("cluster1d_positionCC_X" , "vector<double>"      ,&cluster1d_positionCC_X);
    tree->Branch("cluster1d_positionCC_Y" , "vector<double>"      ,&cluster1d_positionCC_Y);
    tree->Branch("cluster1d_positionTPC_X" , "vector<double>"      ,&cluster1d_positionTPC_X);
    tree->Branch("cluster1d_positionTPC_Y" , "vector<double>"      ,&cluster1d_positionTPC_Y);
  }
  return;  
}

void PARSIFAL::Write_oFile(){
  
  //Output
  if(PrintNTuple_Geo){
    geo_numview  = geo->Get_NumberOfView();
    geo_pitch1   = geo->Get_Pitch1();
    geo_pitch2   = geo->Get_Pitch2();
    geo_driftgap = geo->Get_DriftGap();
  }
  if(PrintNTuple_Party){
    party_partID   = party->Get_ParticleID();
    party_angleXZ  = party->Get_AngleXZ();
    party_angleYZ  = party->Get_AngleYZ();
    party_primary  = party->Get_MeanPrimary();
    party_posI[0]  = party->Get_Position()->Get_X();
    party_posI[1]  = party->Get_Position()->Get_Y();
    party_posI[2]  = party->Get_Position()->Get_Z();
    party_posI[3]  = party->Get_Position()->Get_T();
    for(int i=0;i<100;i++) party_prob[i] = party->Get_ProbabilitySecondary(i);
  }
  if(PrintNTuple_Ionio){
    ionio_positionI[0] = ionio->Get_PositionInitial().Get_X();
    ionio_positionI[1] = ionio->Get_PositionInitial().Get_Y();
    ionio_positionI[2] = ionio->Get_PositionInitial().Get_Z();
    ionio_positionI[3] = ionio->Get_PositionInitial().Get_T();
    ionio_positionF[0] = ionio->Get_PositionFinal().Get_X();
    ionio_positionF[1] = ionio->Get_PositionFinal().Get_Y();
    ionio_positionF[2] = ionio->Get_PositionFinal().Get_Z();
    ionio_positionF[3] = ionio->Get_PositionFinal().Get_T();
    ionio_primary      = ionio->Get_TotalNumberOfPrimary();
    ionio_secondary    = ionio->Get_TotalNumberOfSecondary();
  }
  if(PrintNTuple_Secondary){
    if(secondary.empty()){
      secondary_gain.push_back(0);
    }
    else{
      for(int i=0;i<secondary.size();i++){
        secondary_gain.push_back(secondary.at(i)->Get_GainedElectrons());
      }
    }
  }
  if(PrintNTuple_Primary){
    if(primi.empty()){
      primary_positionX.push_back(-999);
      primary_positionY.push_back(-999);
      primary_positionZ.push_back(-999);
      primary_positionT.push_back(-999);
      primary_secondary.push_back(0);
      primary_ID.push_back(-1);
      secondary_gain.push_back(0);
    }
    else{
      for(int i=0;i<primi.size();i++){
	primary_positionX.push_back(primi.at(i)->Get_Position().Get_X());
	primary_positionY.push_back(primi.at(i)->Get_Position().Get_Y());
	primary_positionZ.push_back(primi.at(i)->Get_Position().Get_Z());
	primary_positionT.push_back(primi.at(i)->Get_Position().Get_T());
	primary_secondary.push_back(primi.at(i)->Get_NumberOfSecondary());
	primary_ID.push_back(i);
	if(secondary_gain.empty() || secondary_gain.size()) secondary_gain.push_back(0);
	else secondary_gain.push_back(secondary.at(i)->Get_GainedElectrons());
      }
    }
  }
  if(PrintNTuple_Secondary){  // CHECK in realta' gli oggetti electron_* sono prodotti solo da Drift
    if(electrons.empty()){
      electron_positionX_initial.push_back(-999);
      electron_positionY_initial.push_back(-999);
      electron_positionZ_initial.push_back(-999);
      electron_positionT_initial.push_back(-999);
      electron_positionX_final.push_back(-999);
      electron_positionY_final.push_back(-999);
      electron_positionZ_final.push_back(-999);
      electron_positionT_final.push_back(-999);
      electron_primary_ID.push_back(.1);
      electron_electronic_channel.push_back(-1);
    }
    else{
      for(int i=0;i<electrons.size();i++){
	electron_positionX_initial.push_back(electrons.at(i)->Get_PositionInitial().Get_X());
	electron_positionY_initial.push_back(electrons.at(i)->Get_PositionInitial().Get_Y());
	electron_positionZ_initial.push_back(electrons.at(i)->Get_PositionInitial().Get_Z());
	electron_positionT_initial.push_back(electrons.at(i)->Get_PositionInitial().Get_T());
	electron_positionX_final.push_back(electrons.at(i)->Get_PositionFinal().Get_X());
	electron_positionY_final.push_back(electrons.at(i)->Get_PositionFinal().Get_Y());
	electron_positionZ_final.push_back(electrons.at(i)->Get_PositionFinal().Get_Z());
	electron_positionT_final.push_back(electrons.at(i)->Get_PositionFinal().Get_T());
	electron_primary_ID.push_back(electrons.at(i)->Get_PrimaryID());
	electron_electronic_channel.push_back(electrons.at(i)->Get_ElectronicChannel());
      }
    }
  }
  if(PrintNTuple_Secondary){
    if(secondary.empty()){
      secondary_gain.push_back(0);
    }
    else{
      for(int i=0;i<secondary.size();i++){
	secondary_gain.push_back(secondary.at(i)->Get_GainedElectrons());      
      }
    }
  }
  if(PrintNTuple_Channel){
    if(channels.empty()){
      channel_ID.push_back(-1);
      channel_type.push_back(-1);
      channel_charge.push_back(-999);
      channel_time.push_back(-999);
      //channel_dtime.push_back(-999);
      channel_positionX.push_back(-999);
      channel_positionY.push_back(-999);
    }
    else{
      for(int i=0;i<channels.size();i++){
	channel_ID.push_back(channels.at(i)->Get_ChannelID());
	channel_type.push_back(channels.at(i)->Get_Type());
	channel_charge.push_back(channels.at(i)->Get_Charge());
	channel_time.push_back(channels.at(i)->Get_Time());
	//channel_dtime.push_back(channels.at(i)->Get_dTime());
	channel_positionX.push_back(channels.at(i)->Get_Position().Get_X());
	channel_positionY.push_back(channels.at(i)->Get_Position().Get_Y());

	if(plot_APV_histos){
	  file->cd();
	  TCanvas *cc1 = new TCanvas("cc1","cc1",800,600);
	  cc1->cd();
	  channels.at(i)->Get_Histo_raw()->Write();
	  cc1->SaveAs((TString)Form("delete/raw_%i.root",i));
	  channels.at(i)->Get_Histo_apv()->Write();
	  cc1->SaveAs((TString)Form("delete/APV_%i.root",i));
	  channels.at(i)->Get_Histo_int()->Write();
	  cc1->SaveAs((TString)Form("delete/int_%i.root",i));
	  channels.at(i)->Get_Histo_cap()->Write();
	  cc1->SaveAs((TString)Form("delete/cap_%i.root",i));
	  delete cc1;
	}
      }
    }
  }
  if(PrintNTuple_Hit){
    if(hit.empty()){
      hit_ID.push_back(-1);
      hit_channelID.push_back(-1);
      hit_clusterID.push_back(-1);
      hit_type.push_back(-1);
      hit_charge.push_back(-999);
      hit_time.push_back(-999);
      // hit_dtime.push_back(-999);
      hit_positionX.push_back(-999);
      hit_positionY.push_back(-999);
    }
    else{
      for(int i=0;i<hit.size();i++){
	hit_ID.push_back(hit.at(i)->Get_HitID());
	hit_channelID.push_back(hit.at(i)->Get_ChannelID());
	hit_clusterID.push_back(hit.at(i)->Get_ClusterID());
	hit_type.push_back(hit.at(i)->Get_Type());
	hit_charge.push_back(hit.at(i)->Get_Charge());
	hit_time.push_back(hit.at(i)->Get_Time());
	hit_dtime.push_back(hit.at(i)->Get_dTime());
	hit_positionX.push_back(hit.at(i)->Get_Position().Get_X());
	hit_positionY.push_back(hit.at(i)->Get_Position().Get_Y());
      }    
    }
  }
  if(PrintNTuple_Cluster1d){
    if(cluster1d.empty()){
      cluster1d_ID.push_back(-1);
      cluster1d_type.push_back(-1);
      cluster1d_charge.push_back(-999);
      cluster1d_size.push_back(0);
      cluster1d_positionCC_X.push_back(-999);
      cluster1d_positionCC_Y.push_back(-999);
      cluster1d_positionTPC_X.push_back(-999);
      cluster1d_positionTPC_Y.push_back(-999);
    }
    else{
      for(int i=0;i<cluster1d.size();i++){
	cluster1d_ID.push_back(cluster1d.at(i)->Get_ClusterID());
	cluster1d_type.push_back(cluster1d.at(i)->Get_Type());
	cluster1d_charge.push_back(cluster1d.at(i)->Get_Charge());
	cluster1d_size.push_back(cluster1d.at(i)->Get_Size());
	cluster1d_positionCC_X.push_back(cluster1d.at(i)->Get_Position_CC()->Get_X());
	cluster1d_positionCC_Y.push_back(cluster1d.at(i)->Get_Position_CC()->Get_Y());
	cluster1d_positionTPC_X.push_back(cluster1d.at(i)->Get_Position_TPC()->Get_X());
        cluster1d_positionTPC_Y.push_back(cluster1d.at(i)->Get_Position_TPC()->Get_Y());
	//for(int j=0;j<cluster1d.at(i)->Get_Size();j++) cluster1d_hitID->at(i).push_back(cluster1d.at(i)->Get_HitID(j));
      }    
    }
  }
  tree->Fill();
  return;  
}
