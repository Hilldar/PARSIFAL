/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "Reconstruction/Reconstruction.h"
namespace PARSIFAL2{
  Reconstruction::Reconstruction(int setup, Geometry *geo):
    PrintInfo(true),
    PrintNTuple(true),
    geometry(geo),
    Setup(setup)
  {
    if(Get_PrintInfo()){
      cout<<"----   New Reconstruction    ----"<<endl;
      cout<<"----------------------"<<endl;
    }
    if(Get_Setup()==0){
      Set_Drift_Velocity(0.037); // mm/ns
      Set_T0_TPC(141); //ns
    }
    else if(Get_Setup()==1){
      Set_Drift_Velocity(0.039); // mm/ns 
      Set_T0_TPC(0); //ns 
    }
    else{
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<>    Reconstruction     <>-<>"<<endl;
      cout<<"<>-<>  Check the setup ID   <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;      
    }
  };
  vector<Hit*> Reconstruction::Digitize(vector<ElectronicChannel*> chan){
    vector<Hit*> hits;
    if(chan.empty() || NO_Readout || NO_Hit)return hits;
    for(int i=0;i<chan.size();i++){
      if(chan.at(i)->Get_Charge()>thr_APV){
	Hit* hit = new Hit(chan.at(i));
	hits.push_back(hit);
      }
    }
    //Finalize the hits
    for(int i=0;i<hits.size();i++) hits.at(i)->Set_HitID(i);
    return hits;
  };
  
  vector<Cluster1D*> Reconstruction::Clusterize(vector<Hit*> hit){
    vector<Cluster1D*> cluster;
    if(hit.empty() || NO_Readout || NO_Cluster || NO_Hit) return cluster;
    Set_Hit_Collection(hit);
    for(int i=0;i<hit.size();i++){
      if(i==0) {
	Cluster1D *clus = new Cluster1D(hit.at(0));
	cluster.push_back(clus);
      }
      else if(
	      Get_Hit_type(i) == Get_Hit_type(i-1) &&
	      abs(Get_Hit_channel(i) - Get_Hit_channel(i-1)) == (1+dead_strip)
	      ){
	cluster.at(cluster.size()-1)->Set_HitID(i);
      }
      else{
	Cluster1D *clus = new Cluster1D(hit.at(i));
	cluster.push_back(clus);
      }
    }
    //Finalize the clusters
    for(int icl=0;icl<cluster.size();icl++){
      double charge=0;
      double cc=0;
      cluster.at(icl)->Set_ClusterID(icl);
      cluster.at(icl)->Set_Size();
      for(int ihit=0;ihit<cluster.at(icl)->Get_Size();ihit++){
	int hit_ID = cluster.at(icl)->Get_HitID(ihit);
	//Charge
	double hit_charge = Get_Hit_charge(hit_ID);
	charge+=hit_charge;
	//Charge Centroid
	if(cluster.at(icl)->Get_Type()==Xview) cc += hit_charge * Get_Hit_X(hit_ID);
	if(cluster.at(icl)->Get_Type()==Yview) cc += hit_charge * Get_Hit_Y(hit_ID);
	//Link cluster and hit
	hit.at(cluster.at(icl)->Get_HitID(ihit))->Set_ClusterID(icl);
      }
      cluster.at(icl)->Set_Charge(charge);
      cluster.at(icl)->Set_Position_CC(cc/charge);
      cluster.at(icl)->Set_Position_TPC(Get_TPC(cluster.at(icl)));
    }
    return cluster;
  };

  double Reconstruction::Get_TPC(Cluster1D* cl1D){
    bool print_TPC = false;
    const int clustersize = cl1D->Get_Size();
    if(clustersize<TPC_min_hit) return -9999;
    vector<double> position_tpc, drift_tpc, dpos_tpc, ddrift_tpc, q_tpc;
    double gap = geometry->Get_DriftGap();
    for(int iHit=0;iHit<clustersize;iHit++){
      int hit_ID = cl1D->Get_HitID(iHit);
      double drift_velocity = Get_Drift_Velocity();
      double drift_path = (Get_Hit_T(hit_ID)-Get_T0_TPC()/*-Track_T0()*/) * drift_velocity;
      //Fill the vector with the hit info
      if(drift_path > -extra_gap_range && drift_path < extra_gap_range + gap){
	position_tpc.push_back(Get_Hit_position1D(hit_ID));
	drift_tpc.push_back(drift_path);
	if(print_TPC) cout<<"iHit: "<<iHit<<" \t hit_ID: "<<hit_ID<<" \t X: "<<Get_Hit_position1D(hit_ID)<<" \t Z: "<<drift_path<<" \t T: "<<Get_Hit_T(hit_ID)<<endl;
	dpos_tpc.push_back(sqrt(pow(Get_Pitch(hit_ID)/sqrt(12.),2) + pow(Get_Pitch(hit_ID)/sqrt(12.) * cl1D->Get_Charge()/cl1D->Get_Size() / Get_Hit_charge(hit_ID) ,2)));
	ddrift_tpc.push_back(Get_Hit_dT(hit_ID) * drift_velocity);
	//ddrift_tpc.push_back(5 * drift_velocity);
	q_tpc.push_back(Get_Hit_charge(hit_ID));
      }
    }
    //Corrections
    Correction_Iso(cl1D->Get_Size(),&position_tpc,&drift_tpc,&q_tpc);
    //µTPC fit
    if(position_tpc.size()>TPC_min_hit){
      if(print_TPC) for(int i=0;i<position_tpc.size();i++) cout<<"hit: "<<i<<" X: "<<position_tpc.at(i)<<" Z: "<<drift_tpc.at(i)<<endl;
      TCanvas *c_tpc = new TCanvas("c_tpc","c_tpc",800,600);
      TGraphErrors *g = new TGraphErrors(position_tpc.size(),&position_tpc[0],&drift_tpc[0],&dpos_tpc[0],&ddrift_tpc[0]);
      TF1 *f = new TF1("f","pol1");
      g->Fit("f","QFW");
      if(print_TPC) g->Draw("AP");
      g->Fit("f","FQ");
      double q=f->GetParameter(0);
      double m=f->GetParameter(1);
      double tpc = (0.5*gap - q)/m;
      //Draw the plot
      if(print_TPC) {
	vector<double> x_mc, z_mc;
	double mc_left = ionio->Get_PositionInitial().Get_X();
	double mc_right= ionio->Get_PositionFinal().Get_X();
	for(int i=0;i<primi.size();i++){
	  if(primi.at(i)->Get_Position().Get_Z()>0.001 && primi.at(i)->Get_Position().Get_X()>mc_left && primi.at(i)->Get_Position().Get_X()<mc_right){
	    x_mc.push_back(primi.at(i)->Get_Position().Get_X());
	    z_mc.push_back(primi.at(i)->Get_Position().Get_Z());
	    //cout<<"iPrimy: "<<i<<" \t X: "<<primi.at(i)->Get_Position().Get_X()<<" \t Z: "<<primi.at(i)->Get_Position().Get_Z()<<endl;
	  }
	}
	c_tpc->Update();
	TGraph *g_mc = new TGraph(z_mc.size(),&x_mc[0],&z_mc[0]);
	g_mc->SetMarkerStyle(24);
	g_mc->SetMarkerColor(kBlue);
	g->GetYaxis()->SetRangeUser(0,5);
	g->SetMarkerStyle(20);
	g->SetMarkerColor(kRed);
	TLine *l_recY = new TLine(c_tpc->GetUxmin(),2.5,tpc,2.5);
	l_recY->SetLineColor(kRed);
	l_recY->Draw("");
	TLine *l_recX = new TLine(tpc,0,tpc,2.5);
	l_recX->SetLineColor(kRed);
	l_recX->Draw("");
	double mc_half_gap = 0.5*(ionio->Get_PositionFinal().Get_X()+ionio->Get_PositionInitial().Get_X());
	TLine *l_mcX = new TLine(mc_half_gap,0,mc_half_gap,5);
        l_mcX->SetLineColor(kBlue);
        l_mcX->Draw();
	TLine *l_mcX_left = new TLine(mc_left,0,mc_left,5);
	l_mcX_left->SetLineColor(kBlue);
	l_mcX_left->SetLineStyle(10);
	l_mcX_left->Draw();
	TLine *l_mcX_right = new TLine(mc_right,0,mc_right,5);
	l_mcX_right->SetLineColor(kBlue);
	l_mcX_right->SetLineStyle(10);
        l_mcX_right->Draw();
	g_mc->Draw("same *");
	gSystem->Exec("mkdir -p tpcplot");
	if(cl1D->Get_Type()==1) c_tpc->SaveAs(Form("tpcplot/tpc_event_%f.png",tpc));
	delete g_mc;
      }
      //Terminate
      delete c_tpc;
      delete g;
      delete f;  
      Set_is_TPC(cl1D,true);
	return tpc;									  
    }    
    return -9999;
  };

  void Reconstruction::Correction_Iso(int NHit, vector<double> *XYV, vector<double> *Z, vector<double> *Q){
    if(NO_uTPC_Correction) return;
    int good = XYV->size();
    if(NHit<TPC_min_hit || good<TPC_min_hit) return;
    for (int i = 0; i < good; ++i) XYV->at(i) = 10.*XYV->at(i);
    double m_corr = -0.45 ;
    double q_corr =  0.36 ;
    double m_y_correction = 0.294905 + NHit * (-0.025459);
    double q_y_correction = -0.41335 + NHit * 0.01714;

    if (NHit>=5) m_corr += (-0.002936) * pow(NHit,2) + (0.02494)  * NHit + (0.01097);
    if (NHit<5)  m_corr += (0.06);
    if (NHit>=5) q_corr += (0.002964)  * pow(NHit,2) + (-0.02983) * NHit + (0.01258);
    if (NHit<5)  q_corr += (-0.06);

    if (Z->at(0)>Z->at(good-1)){
      for (int i = 0; i < good; ++i){
          if (i!=good-1)  XYV->at(i) = XYV->at(i) - ((double)Q->at(i+1)/Q->at(i)) * (m_corr) + q_corr;
          if (i==0)       XYV->at(i) = XYV->at(i) - ((double)Q->at(i+1)/Q->at(i)) * (0.20) + (-0.25);
          if (i==good-1)  XYV->at(i) = XYV->at(i) - (0.18);
      }
      for(int i=0;i<good;i++){
        XYV->at(i) = XYV->at(i) - m_y_correction * i + q_y_correction;
        if (good>=5) XYV->at(i) = XYV->at(i) - (0.002964) * pow(NHit,2) + (-0.02783) * NHit + (0.01258);
        if (good<5)  XYV->at(i) = XYV->at(i) - (-0.06);
      }
    }
    if (Z->at(0)<Z->at(good-1)){
      for (int i = 0; i < good; ++i){
          if (i!=0)       XYV->at(i) = XYV->at(i) + ((double)Q->at(i-1)/Q->at(i)) * (m_corr) + q_corr;
          if (i==good-1)  XYV->at(i) = XYV->at(i) + ((double)Q->at(i-1)/Q->at(i)) * (0.20) + (-0.25);
          if (i==0)       XYV->at(i) = XYV->at(i) + (0.18);
      }
      for(int i=0;i<good;i++){
        XYV->at(i) = XYV->at(i) - m_y_correction * i + q_y_correction;
        if (good>=5) XYV->at(i) = XYV->at(i) - (0.002964) * pow(NHit,2) + (-0.02783) * NHit + (0.01258);
        if (good<5)  XYV->at(i) = XYV->at(i) - (-0.06);
      }
    }

    for (int i = 0; i < good; ++i) XYV->at(i)=0.1*XYV->at(i);
    return;
  };
}
