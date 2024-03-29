/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Reconstruction/main.h"
using namespace PARSIFAL;

const int nShots = 10;

int main(int argc, const char* argv[]){
  //Initialization
  cout<<"Ciao"<<endl;
  TString name_outfile="ciao_10deg.root";
  file = new TFile(name_outfile,"RECREATE");
  tree = new TTree("tree","tree");
  geo = Planar2D(0.65, 0.65, 5);
  D4 = new Position(0,0,0,0);
  //party = new Particle(1, D4, 0.523, -0.0, geo); //30deg
  party = new Particle(1, D4, 0.17, -0.0, geo); //10deg
  //party = new Particle(1, D4, 0., -0.0, geo);
  ionio = new Ionization(party,geo); 
  gain = new DetectorGain(0, false);
  drift = new ElectronDrift(0, false, geo);
  anode = new Readout(0,0,geo);
  recon = new Reconstruction();
  primi.push_back(new Primary());
  secondary.push_back(new Secondary());
  electrons.push_back(new Secondary());
  channels.push_back(new ElectronicChannel()); 
  hit.push_back(new Hit());
  cluster1d.push_back(new Cluster1D());
  event=0;
  Initialize_oFile();
  //
  //Run
  for(event=0;event<nShots;event++){
    Clean();
    primi     = ionio->PrimaryIonization();
    secondary = ionio->SecondaryIonization(primi);
    secondary = gain->Gain(secondary);
    electrons = drift->Drift(secondary);
    channels  = anode->Read(electrons);
    for(int i=0;i<channels.size();i++) cout<<i<<" "<<channels.at(i)->Get_Position()->Get_X()<<" "<<channels.at(i)->Get_Charge()<<" "<<channels.at(i)->Get_Time()<<endl;
    hit       = recon->Digitize(channels); 
    cluster1d = recon->Clusterize(hit);
    nhit      = hit.size();
    ncluster1d= cluster1d.size();
    //channels.at(5)->Print_Time();
    Write_oFile();
    if(event%10==0) cout<<"Event "<<event<<endl;
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

void PARSIFAL::Clean(){
  nhit=0;
  ncluster1d=0;
  primi.clear();
  secondary.clear();
  electrons.clear();
  for(int i=0;i<channels.size();i++) channels.at(i)->Reset_Time();
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
}

void PARSIFAL::Initialize_oFile(){
  tree->Branch("event" ,&event  ,"event/I");  
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
    tree->Branch("primary_ID"        , "vector<int>"    ,&primary_ID);
  }
  if(secondary.at(0)->Get_PrintNTuple()){
    tree->Branch("secondary_gain"             , "vector<int>"    ,&secondary_gain);
  }
  if(electrons.at(0)->Get_PrintNTuple()){
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
  if(channels.at(0)->Get_PrintNTuple()){
    tree->Branch("channel_ID"        , "vector<int>"    ,&channel_ID);  
    tree->Branch("channel_type"      , "vector<int>"    ,&channel_type);  
    tree->Branch("channel_charge"    , "vector<double>" ,&channel_charge);
    tree->Branch("channel_time"      , "vector<double>" ,&channel_time);
    tree->Branch("channel_dtime"     , "vector<double>" ,&channel_dtime);
    tree->Branch("channel_positionX" , "vector<double>" ,&channel_positionX);
    tree->Branch("channel_positionY" , "vector<double>" ,&channel_positionY);
  }  
  if(hit.at(0)->Get_PrintNTuple()){
    tree->Branch("nhit"          , &nhit            ,"nhit/I");  
    tree->Branch("hit_ID"        , "vector<int>"    ,&hit_ID);  
    tree->Branch("hit_clusterID" , "vector<int>"    ,&hit_clusterID);  
    tree->Branch("hit_type"      , "vector<int>"    ,&hit_type);  
    tree->Branch("hit_charge"    , "vector<double>" ,&hit_charge);
    tree->Branch("hit_time"      , "vector<double>" ,&hit_time);
    tree->Branch("hit_dtime"     , "vector<double>" ,&hit_dtime);
    tree->Branch("hit_positionX" , "vector<double>" ,&hit_positionX);
    tree->Branch("hit_positionY" , "vector<double>" ,&hit_positionY);
  }  
  if(cluster1d.at(0)->Get_PrintNTuple()){
    tree->Branch("ncluster1d"             , &ncluster1d           ,"ncluster1d/I");  
    tree->Branch("cluster1d_ID"           , "vector<int>"         ,&cluster1d_ID);
    //tree->Branch("cluster1d_hitID"        , "vector<vector<int>>" ,&cluster1d_hitID);
    tree->Branch("cluster1d_type"         , "vector<int>"         ,&cluster1d_type);
    tree->Branch("cluster1d_size"         , "vector<int>"         ,&cluster1d_size);
    tree->Branch("cluster1d_charge"       , "vector<double>"      ,&cluster1d_charge);
    tree->Branch("cluster1d_positionCC_X" , "vector<double>"      ,&cluster1d_positionCC_X);
    tree->Branch("cluster1d_positionCC_Y" , "vector<double>"      ,&cluster1d_positionCC_Y);
  }

  return;  
}

void PARSIFAL::Write_oFile(){
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
    for(int i=0;i<primi.size();i++){
      primary_positionX.push_back(primi.at(i)->Get_Position()->Get_X());
      primary_positionY.push_back(primi.at(i)->Get_Position()->Get_Y());
      primary_positionZ.push_back(primi.at(i)->Get_Position()->Get_Z());
      primary_positionT.push_back(primi.at(i)->Get_Position()->Get_T());
      primary_secondary.push_back(primi.at(i)->Get_NumberOfSecondary());
      primary_ID.push_back(i);
    }
  }
  if(electrons.at(0)->Get_PrintNTuple()){
    for(int i=0;i<electrons.size();i++){
      electron_positionX_initial.push_back(electrons.at(i)->Get_PositionInitial()->Get_X());
      electron_positionY_initial.push_back(electrons.at(i)->Get_PositionInitial()->Get_Y());
      electron_positionZ_initial.push_back(electrons.at(i)->Get_PositionInitial()->Get_Z());
      electron_positionT_initial.push_back(electrons.at(i)->Get_PositionInitial()->Get_T());
      electron_positionX_final.push_back(electrons.at(i)->Get_PositionFinal()->Get_X());
      electron_positionY_final.push_back(electrons.at(i)->Get_PositionFinal()->Get_Y());
      electron_positionZ_final.push_back(electrons.at(i)->Get_PositionFinal()->Get_Z());
      electron_positionT_final.push_back(electrons.at(i)->Get_PositionFinal()->Get_T());
      electron_primary_ID.push_back(electrons.at(i)->Get_PrimaryID());
      electron_electronic_channel.push_back(electrons.at(i)->Get_ElectronicChannel());
    }
  }
  if(secondary.at(0)->Get_PrintNTuple()){
    for(int i=0;i<secondary.size();i++){
      secondary_gain.push_back(secondary.at(i)->Get_GainedElectrons());      
    }
  }
  if(channels.at(0)->Get_PrintNTuple()){
    for(int i=0;i<channels.size();i++){
      channel_ID.push_back(channels.at(i)->Get_ChannelID());
      channel_type.push_back(channels.at(i)->Get_Type());
      channel_charge.push_back(channels.at(i)->Get_Charge());
      channel_time.push_back(channels.at(i)->Get_Time());
      channel_dtime.push_back(channels.at(i)->Get_dTime());
      channel_positionX.push_back(channels.at(i)->Get_Position()->Get_X());
      channel_positionY.push_back(channels.at(i)->Get_Position()->Get_Y());
    }
  }
  if(hit.at(0)->Get_PrintNTuple()){
    for(int i=0;i<hit.size();i++){
      hit_ID.push_back(hit.at(i)->Get_HitID());
      hit_channelID.push_back(hit.at(i)->Get_ChannelID());
      hit_clusterID.push_back(hit.at(i)->Get_ClusterID());
      hit_type.push_back(hit.at(i)->Get_Type());
      hit_charge.push_back(hit.at(i)->Get_Charge());
      hit_time.push_back(hit.at(i)->Get_Time());
      hit_dtime.push_back(hit.at(i)->Get_dTime());
      hit_positionX.push_back(hit.at(i)->Get_Position()->Get_X());
      hit_positionY.push_back(hit.at(i)->Get_Position()->Get_Y());
    }    
  }
  if(cluster1d.at(0)->Get_PrintNTuple()){
    for(int i=0;i<cluster1d.size();i++){
      cluster1d_ID.push_back(cluster1d.at(i)->Get_ClusterID());
      cluster1d_type.push_back(cluster1d.at(i)->Get_Type());
      cluster1d_charge.push_back(cluster1d.at(i)->Get_Charge());
      cluster1d_size.push_back(cluster1d.at(i)->Get_Size());
      cluster1d_positionCC_X.push_back(cluster1d.at(i)->Get_Position_CC()->Get_X());
      cluster1d_positionCC_Y.push_back(cluster1d.at(i)->Get_Position_CC()->Get_Y());
      //for(int j=0;j<cluster1d.at(i)->Get_Size();j++) cluster1d_hitID->at(i).push_back(cluster1d.at(i)->Get_HitID(j));
    }    
  }
  tree->Fill();
  return;  
}

