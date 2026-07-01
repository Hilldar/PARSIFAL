bool skip_exp = false;

TMultiGraph *mg_QS, *mg_QE, *mg_QR, *mg_gainQ, *mg_gainS, *mg_gainE, *mg_gainR, *mg_hvQ, *mg_hvS, *mg_hvE, *mg_hvR;
TLegend *leg;
void Add_scan_APV(int runID_i, int runID_f, int color, string legenda);
void Add_scan_TIGER(int runID_i, int runID_f, int color, string legenda);
TString Fill();

string name_leg;

//Comment and uncomment the APV or TIGER dataset depending on your needs

//APV
int     ssize = 10;
double  q_exp[10]  = {7.53333,15.9,20.2667,32.75,50.6,60.5667,83.9333,97.6333,124.217,140.533}; 
double  s_exp[10]  = {1.375,1.83,2.015,2.405,2.945,3.215,4.03,4.45,5.26,5.75};  
double  e_exp[10]  = {0.52,0.86,0.93,0.96,0.95,0.95,0.94,0.94,0.94,0.94};
double  r_exp[10]  = {112,68,58,56,57,59,50,50,50,50};
double  g_exp[10]  = {460,880,1100,1700,2600,3250,5000,6250,9700,12000};
double hv_exp[10]  = {520,550,560,580,600,610,630,640,660,670};
double eq_exp[10]  = {0.7,1.6,2.0,3.2,5.1,6.0,8.4,9.7,12.4,14.0};
double es_exp[10]  = {0.14,0.18,0.2,0.24,0.29,0.32,0.4,0.45,0.526,0.575};
double ee_exp[10]  = {0.02,0.02,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01};
double er_exp[10]  = {8,3,3,3,3,2,1,1,1,1};
double ehv_exp[10]  = {};
double eg_exp[10];
for(int i=0;i<10;i++) eg_exp[i] = 0.1*g_exp[i];  
name_leg="Experimental data - APV"; 


/*
//TIGER
int ssize=15;
double  q_exp[15]  = {10.53,10.2,9.9,9.96,10.35,11.54,14.56,19.45,27.14,37.39,50.32,62.23,73.54,84.53,97.87};
double  s_exp[15]  = {1.08,1.08,1.1,1.12,1.12,1.14,1.23,1.39,1.58,1.77,2.01,2.22,2.5,2.99,3.77};
double  e_exp[15]  = {0.049,0.065,0.102,0.2,0.37,0.616,0.816,0.91,0.943,0.959,0.968,0.972,0.973,0.975,0.974};
double  r_exp[15]  = {255,222,121,110,108,110,108,96,71,68,64,61,66,58,75};
double  g_exp[15]  = {33.1818,51.3402,79.4357,122.906,190.166,294.232,455.249,704.38,1089.85,1686.26,2609.04,4036.82,6245.94,9663.98,14952.5};
double hv_exp[15]  = {400,420,440,460,480,500,520,540,560,580,600,620,640,660,680};
double eq_exp[15]  = {1.53,1.2,0.9,0.96,1.35,1.14,1.5,2.45,3.14,3.39,5.32,6.23,7.54,8.53,9.87};
double es_exp[15]  = {};
double ee_exp[15]  = {};
double er_exp[15]  = {};
double ehv_exp[15] = {};
double eg_exp[15];
for(int i=0;i<15;i++) {
  es_exp[i] = 0.1*s_exp[i];
  eq_exp[i] = 0.1*q_exp[i];
  ee_exp[i] = 0.01*e_exp[i];
  er_exp[i] = 0.05*r_exp[i];
  eg_exp[i] = 0.1*g_exp[i];
 }
name_leg="Experimental data - TIGER";
*/



void HVscan(){
  cout<<"HV scan plot macro"<<endl;
  //Dummy for plot
  double dummy[2]  = {-100,1000};
  double dammy[2]  = {-100,-1000};
  TGraphErrors *g_dummy = new TGraphErrors(2,dummy,dammy,0,0);
  
  TGraphErrors *g_QS_exp = new TGraphErrors(ssize,q_exp,s_exp,eq_exp,es_exp);
  g_QS_exp->SetLineColor(kBlack);
  g_QS_exp->SetMarkerStyle(20);
  g_QS_exp->SetMarkerSize(1.2);
  g_QS_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_QE_exp = new TGraphErrors(ssize,q_exp,e_exp,eq_exp,ee_exp);
  g_QE_exp->SetLineColor(kBlack);
  g_QE_exp->SetMarkerStyle(20);
  g_QE_exp->SetMarkerSize(1.2);
  g_QE_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_QR_exp = new TGraphErrors(ssize,q_exp,r_exp,eq_exp,er_exp);
  g_QR_exp->SetLineColor(kBlack);
  g_QR_exp->SetMarkerStyle(20);
  g_QR_exp->SetMarkerSize(1.2);
  g_QR_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_gainQ_exp = new TGraphErrors(ssize,g_exp,q_exp,eg_exp,eq_exp);
  g_gainQ_exp->SetLineColor(kBlack);
  g_gainQ_exp->SetMarkerStyle(20);
  g_gainQ_exp->SetMarkerSize(1.2);
  g_gainQ_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_gainS_exp = new TGraphErrors(ssize,g_exp,s_exp,eg_exp,es_exp);
  g_gainS_exp->SetLineColor(kBlack);
  g_gainS_exp->SetMarkerStyle(20);
  g_gainS_exp->SetMarkerSize(1.2);
  g_gainS_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_gainE_exp = new TGraphErrors(ssize,g_exp,e_exp,eg_exp,ee_exp);
  g_gainE_exp->SetLineColor(kBlack);
  g_gainE_exp->SetMarkerStyle(20);
  g_gainE_exp->SetMarkerSize(1.2);
  g_gainE_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_gainR_exp = new TGraphErrors(ssize,g_exp,r_exp,eg_exp,er_exp);
  g_gainR_exp->SetLineColor(kBlack);
  g_gainR_exp->SetMarkerStyle(20);
  g_gainR_exp->SetMarkerSize(1.2);
  g_gainR_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_hvQ_exp = new TGraphErrors(ssize,hv_exp,q_exp,ehv_exp,eq_exp);
  g_hvQ_exp->SetLineColor(kBlack);
  g_hvQ_exp->SetMarkerStyle(20);
  g_hvQ_exp->SetMarkerSize(1.2);
  g_hvQ_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_hvS_exp = new TGraphErrors(ssize,hv_exp,s_exp,ehv_exp,es_exp);
  g_hvS_exp->SetLineColor(kBlack);
  g_hvS_exp->SetMarkerStyle(20);
  g_hvS_exp->SetMarkerSize(1.2);
  g_hvS_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_hvE_exp = new TGraphErrors(ssize,hv_exp,e_exp,ehv_exp,ee_exp);
  g_hvE_exp->SetLineColor(kBlack);
  g_hvE_exp->SetMarkerStyle(20);
  g_hvE_exp->SetMarkerSize(1.2);
  g_hvE_exp->SetMarkerColor(kBlack);

  TGraphErrors *g_hvR_exp = new TGraphErrors(ssize,hv_exp,r_exp,ehv_exp,er_exp);
  g_hvR_exp->SetLineColor(kBlack);
  g_hvR_exp->SetMarkerStyle(20);
  g_hvR_exp->SetMarkerSize(1.2);
  g_hvR_exp->SetMarkerColor(kBlack);
  
  mg_QS = new TMultiGraph();
  mg_QS->Add(g_dummy);
  mg_QE = new TMultiGraph();
  mg_QE->Add(g_dummy);
  mg_QR = new TMultiGraph();
  mg_QR->Add(g_dummy);
  mg_gainQ = new TMultiGraph();
  mg_gainQ->Add(g_dummy);
  mg_gainS = new TMultiGraph();
  mg_gainS->Add(g_dummy);
  mg_gainE = new TMultiGraph();
  mg_gainE->Add(g_dummy);
  mg_gainR = new TMultiGraph();
  mg_gainR->Add(g_dummy);
  mg_hvQ = new TMultiGraph();
  mg_hvQ->Add(g_dummy);
  mg_hvS = new TMultiGraph();
  mg_hvS->Add(g_dummy);
  mg_hvE = new TMultiGraph();
  mg_hvE->Add(g_dummy);
  mg_hvR = new TMultiGraph();
  mg_hvR->Add(g_dummy);

  
  leg = new TLegend(0.12,0.55,0.88,0.88);
  leg->AddEntry(g_QS_exp, name_leg.c_str(),"lp");

  TString name = "ciao";
  name = Fill();
  if(!skip_exp){
    mg_QS->Add(g_QS_exp);
    mg_QE->Add(g_QE_exp);
    mg_QR->Add(g_QR_exp);
    mg_gainQ->Add(g_gainQ_exp);
    mg_gainS->Add(g_gainS_exp);
    mg_gainE->Add(g_gainE_exp);
    mg_gainR->Add(g_gainR_exp);
    mg_hvQ->Add(g_hvQ_exp);
    mg_hvS->Add(g_hvS_exp);
    mg_hvE->Add(g_hvE_exp);
    mg_hvR->Add(g_hvR_exp);
  }
  
  TCanvas *c1 = new TCanvas("c1","",1200,600);
  //c1->Divide(3,1);
  //Charge vs Size
  //c1->cd(1);
  c1->SetGrid();
  c1->SetGridx();
  c1->SetGridy();
  mg_QS->Draw("APL");
  mg_QS->GetXaxis()->SetRangeUser(0,200);
  mg_QS->GetYaxis()->SetRangeUser(0,15);
  mg_QS->SetTitle("HV scan SIM + EXP");
  mg_QS->GetXaxis()->SetTitle("Charge [fC]");
  mg_QS->GetYaxis()->SetTitle("Size");
  mg_QS->GetXaxis()->SetNdivisions(16);
  mg_QS->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name+"(","pdf");
  //Charge vs Efficiency
  //c1->cd(2);
  mg_QE->Draw("APL");
  mg_QE->GetXaxis()->SetRangeUser(0,200);
  mg_QE->GetYaxis()->SetRangeUser(0,3);
  mg_QE->SetTitle("HV scan SIM + EXP");
  mg_QE->GetXaxis()->SetTitle("Charge [fC]");
  mg_QE->GetYaxis()->SetTitle("Efficiency");
  mg_QE->GetXaxis()->SetNdivisions(16);
  mg_QE->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //Charge vs Resolution
  //c1->cd(3);
  mg_QR->Draw("APL");
  mg_QR->GetXaxis()->SetRangeUser(0,200);
  mg_QR->GetYaxis()->SetRangeUser(0,1000);
  mg_QR->SetTitle("HV scan SIM + EXP");
  mg_QR->GetXaxis()->SetTitle("Charge [fC]");
  mg_QR->GetYaxis()->SetTitle("Spatial resolution [um]");
  mg_QR->GetXaxis()->SetNdivisions(16);
  mg_QR->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //Gain vs Charge
  mg_gainQ->Draw("APL");
  mg_gainQ->GetXaxis()->SetRangeUser(0,20000);
  mg_gainQ->GetYaxis()->SetRangeUser(0,400);
  mg_gainQ->SetTitle("HV scan SIM + EXP");
  mg_gainQ->GetXaxis()->SetTitle("Gain");
  mg_gainQ->GetYaxis()->SetTitle("Charge [fC]");
  mg_gainQ->GetXaxis()->SetNdivisions(16);
  mg_gainQ->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //Gain vs Size
  mg_gainS->Draw("APL");
  mg_gainS->GetXaxis()->SetRangeUser(0,20000);
  mg_gainS->GetYaxis()->SetRangeUser(0,15);
  mg_gainS->SetTitle("HV scan SIM + EXP");
  mg_gainS->GetXaxis()->SetTitle("Gain");
  mg_gainS->GetYaxis()->SetTitle("Size");
  mg_gainS->GetXaxis()->SetNdivisions(16);
  mg_gainS->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //Gain vs Efficiency
  mg_gainE->Draw("APL");
  mg_gainE->GetXaxis()->SetRangeUser(0,20000);
  mg_gainE->GetYaxis()->SetRangeUser(0,3);
  mg_gainE->SetTitle("HV scan SIM + EXP");
  mg_gainE->GetXaxis()->SetTitle("Gain");
  mg_gainE->GetYaxis()->SetTitle("Efficiency");
  mg_gainE->GetXaxis()->SetNdivisions(16);
  mg_gainE->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //Gain vs Resolution
  mg_gainR->Draw("APL");
  mg_gainR->GetXaxis()->SetRangeUser(0,20000);
  mg_gainR->GetYaxis()->SetRangeUser(0,1000);
  mg_gainR->SetTitle("HV scan SIM + EXP");
  mg_gainR->GetXaxis()->SetTitle("Gain");
  mg_gainR->GetYaxis()->SetTitle("Resolution");
  mg_gainR->GetXaxis()->SetNdivisions(16);
  mg_gainR->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //HV vs Charge
  mg_hvQ->Draw("APL");
  mg_hvQ->GetXaxis()->SetRangeUser(400,700);
  mg_hvQ->GetYaxis()->SetRangeUser(0,400);
  mg_hvQ->SetTitle("HV scan SIM + EXP");
  mg_hvQ->GetXaxis()->SetTitle("HV [V]");
  mg_hvQ->GetYaxis()->SetTitle("Charge [fC]");
  mg_hvQ->GetXaxis()->SetNdivisions(16);
  mg_hvQ->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //HV vs Size
  mg_hvS->Draw("APL");
  mg_hvS->GetXaxis()->SetRangeUser(400,700);
  mg_hvS->GetYaxis()->SetRangeUser(0,15);
  mg_hvS->SetTitle("HV scan SIM + EXP");
  mg_hvS->GetXaxis()->SetTitle("HV [V]");
  mg_hvS->GetYaxis()->SetTitle("Size");
  mg_hvS->GetXaxis()->SetNdivisions(16);
  mg_hvS->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //HV vs Efficiency
  mg_hvE->Draw("APL");
  mg_hvE->GetXaxis()->SetRangeUser(400,700);
  mg_hvE->GetYaxis()->SetRangeUser(0,3);
  mg_hvE->SetTitle("HV scan SIM + EXP");
  mg_hvE->GetXaxis()->SetTitle("HV [V]");
  mg_hvE->GetYaxis()->SetTitle("Efficiency");
  mg_hvE->GetXaxis()->SetNdivisions(16);
  mg_hvE->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name,"pdf");
  //HV vs Resolution
  mg_hvR->Draw("APL");
  mg_hvR->GetXaxis()->SetRangeUser(500,700);
  mg_hvR->GetYaxis()->SetRangeUser(0,1000);
  mg_hvR->SetTitle("HV scan SIM + EXP");
  mg_hvR->GetXaxis()->SetTitle("HV [V]");
  mg_hvR->GetYaxis()->SetTitle("Resolution");
  mg_hvR->GetXaxis()->SetNdivisions(16);
  mg_hvR->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(name+")","pdf");
  exit(0);
}


void Add_scan_APV(int runID_i, int runID_f, int color, string legenda){

  double g_sim[10]  = {460,880,1100,1700,2600,3250,5000,6250,9700,12000};
  double eg_sim[10] = {0,0,0,0,0,0,0,0,0,0};
  int good_run=0;
  vector<double> v_q, v_s, v_e, v_r, v_eq, v_es, v_ee, v_er;
  for(int i_run=runID_i;i_run<=runID_f;i_run++){
    string filename = Form("../../data/%i/summary.txt",i_run);
    std::ifstream file(filename);
    if(!file.good()) continue;
    good_run++;
    float Angle, Qind_MPV, Qread, RatioQ, Size, CCres, TPCres, T0res, Eff;
    file>>Angle>>Qind_MPV>>Qread>>RatioQ>>Size>>CCres>>TPCres>>T0res>>Eff;
    cout<<filename<<endl;
    cout << Angle << " " << Qind_MPV << " " << Qread << " " << RatioQ << " " << Size << " " << CCres << " " << TPCres << " " << T0res << " " << Eff << endl;
    v_q.push_back(Qread);
    v_eq.push_back(Qread*0.05);
    v_s.push_back(Size);
    v_es.push_back(Size*0.05);
    v_e.push_back(Eff);
    v_ee.push_back(sqrt(Eff*(1-Eff)/2000));
    v_r.push_back(CCres);
    v_er.push_back(0.05*CCres);
  }
  for (size_t i = 0; i < v_q.size(); i++) {
    cout << v_q[i] << " ";
  }
  cout << endl;
  TGraphErrors *g_QS = new TGraphErrors(good_run,&v_q[0], &v_s[0], &v_eq[0], &v_es[0]);
  g_QS->SetLineColor(color);
  g_QS->SetMarkerStyle(20);
  g_QS->SetMarkerSize(1.2);
  g_QS->SetMarkerColor(color);
  TGraphErrors *g_QE = new TGraphErrors(good_run,&v_q[0], &v_e[0], &v_eq[0], &v_ee[0]);
  g_QE->SetLineColor(color);
  g_QE->SetMarkerStyle(20);
  g_QE->SetMarkerSize(1.2);
  g_QE->SetMarkerColor(color);
  TGraphErrors *g_QR = new TGraphErrors(good_run,&v_q[0], &v_r[0], &v_eq[0], &v_er[0]);
  g_QR->SetLineColor(color);
  g_QR->SetMarkerStyle(20);
  g_QR->SetMarkerSize(1.2);
  g_QR->SetMarkerColor(color);
  TGraphErrors *g_gainQ = new TGraphErrors(10,g_sim,&v_q[0],eg_sim,&v_eq[0]);
  g_gainQ->SetLineColor(color);
  g_gainQ->SetMarkerStyle(20);
  g_gainQ->SetMarkerSize(1.2);
  g_gainQ->SetMarkerColor(color);
  TGraphErrors *g_gainS = new TGraphErrors(10,g_sim,&v_s[0],eg_sim,&v_es[0]);
  g_gainS->SetLineColor(color);
  g_gainS->SetMarkerStyle(20);
  g_gainS->SetMarkerSize(1.2);
  g_gainS->SetMarkerColor(color);
  TGraphErrors *g_gainE = new TGraphErrors(10,g_sim,&v_e[0],eg_sim,&v_ee[0]);
  g_gainE->SetLineColor(color);
  g_gainE->SetMarkerStyle(20);
  g_gainE->SetMarkerSize(1.2);
  g_gainE->SetMarkerColor(color);
  TGraphErrors *g_gainR = new TGraphErrors(10,g_sim,&v_r[0],eg_sim,&v_er[0]);
  g_gainR->SetLineColor(color);
  g_gainR->SetMarkerStyle(20);
  g_gainR->SetMarkerSize(1.2);
  g_gainR->SetMarkerColor(color);
  TGraphErrors *g_hvQ = new TGraphErrors(10,hv_exp,&v_q[0],ehv_exp,&v_eq[0]);
  g_hvQ->SetLineColor(color);
  g_hvQ->SetMarkerStyle(20);
  g_hvQ->SetMarkerSize(1.2);
  g_hvQ->SetMarkerColor(color);
  TGraphErrors *g_hvS = new TGraphErrors(10,hv_exp,&v_s[0],ehv_exp,&v_es[0]);
  g_hvS->SetLineColor(color);
  g_hvS->SetMarkerStyle(20);
  g_hvS->SetMarkerSize(1.2);
  g_hvS->SetMarkerColor(color);
  TGraphErrors *g_hvE = new TGraphErrors(10,hv_exp,&v_e[0],ehv_exp,&v_ee[0]);
  g_hvE->SetLineColor(color);
  g_hvE->SetMarkerStyle(20);
  g_hvE->SetMarkerSize(1.2);
  g_hvE->SetMarkerColor(color);
  TGraphErrors *g_hvR = new TGraphErrors(10,hv_exp,&v_r[0],ehv_exp,&v_er[0]);
  g_hvR->SetLineColor(color);
  g_hvR->SetMarkerStyle(20);
  g_hvR->SetMarkerSize(1.2);
  g_hvR->SetMarkerColor(color);


  mg_QS->Add(g_QS);
  mg_QE->Add(g_QE);
  mg_QR->Add(g_QR);
  mg_gainQ->Add(g_gainQ);
  mg_gainS->Add(g_gainS);
  mg_gainE->Add(g_gainE);
  mg_gainR->Add(g_gainR);
  mg_hvQ->Add(g_hvQ);
  mg_hvS->Add(g_hvS);
  mg_hvE->Add(g_hvE);
  mg_hvR->Add(g_hvR);

  leg->AddEntry(g_QS, legenda.c_str(), "lp");


}


void Add_scan_TIGER(int runID_i, int runID_f, int color, string legenda){
  cout<<"---------------------"<<endl;
  cout<<legenda<<endl;
  cout<<endl;


  double g_sim[15]  = {33.1818,51.3402,79.4357,122.906,190.166,294.232,455.249,704.38,1089.85,1686.26,2609.04,4036.82,6245.94,9663.98,14952.5};
  double eg_sim[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int good_run=0;
  vector<double> v_q, v_s, v_e, v_r, v_eq, v_es, v_ee, v_er;
  for(int i_run=runID_i;i_run<=runID_f;i_run++){
    string filename = Form("../../data/%i/summary.txt",i_run);
    std::ifstream file(filename);
    if(!file.good()) {
      cout<<"-------------------------"<<endl;
      cout<<"*************************"<<endl;
      cout<<"The run "<<i_run<<" is BAD"<<endl;
      cout<<"*************************"<<endl;
      cout<<"-------------------------"<<endl;

      continue;
    }
    good_run++;
    float Angle, Qind_MPV, Qread, RatioQ, Size, CCres, TPCres, T0res, Eff;
    file>>Angle>>Qind_MPV>>Qread>>RatioQ>>Size>>CCres>>TPCres>>T0res>>Eff;
    cout<<filename<<endl;
    cout <<i_run<<" "<< Angle << " " << Qind_MPV << " " << Qread << " " << RatioQ << " " << Size << " " << CCres << " " << TPCres << " " << T0res << " " << Eff << endl;
    v_q.push_back(Qread);
    v_eq.push_back(Qread*0.05);
    v_s.push_back(Size);
    v_es.push_back(Size*0.05);
    v_e.push_back(Eff);
    v_ee.push_back(sqrt(Eff*(1-Eff)/2000));
    v_r.push_back(CCres);
    v_er.push_back(0.05*CCres);
  }
  //Print
  cout<<"Q: ";
  for (size_t i = 0; i < v_q.size(); i++) {
    cout << v_q[i] << " ";
  }
  cout << endl;
  cout<<"S: ";
  for (size_t i = 0; i < v_s.size(); i++) {
    cout << v_s[i] << " ";
  }
  cout << endl;
  cout<<"R: ";
  for (size_t i = 0; i < v_r.size(); i++) {
    cout << v_r[i] << " ";
  }
  cout << endl;
  cout<<"E: ";
  for (size_t i = 0; i < v_e.size(); i++) {
    cout << v_e[i] << " ";
  }
  cout << endl;

  cout << endl;
  TGraphErrors *g_QS = new TGraphErrors(good_run,&v_q[0], &v_s[0], &v_eq[0], &v_es[0]);
  g_QS->SetLineColor(color);
  g_QS->SetMarkerStyle(20);
  g_QS->SetMarkerSize(1.2);
  g_QS->SetMarkerColor(color);
  TGraphErrors *g_QE = new TGraphErrors(good_run,&v_q[0], &v_e[0], &v_eq[0], &v_ee[0]);
  g_QE->SetLineColor(color);
  g_QE->SetMarkerStyle(20);
  g_QE->SetMarkerSize(1.2);
  g_QE->SetMarkerColor(color);
  TGraphErrors *g_QR = new TGraphErrors(good_run, &v_q[0],&v_r[0], &v_eq[0], &v_er[0]);
  g_QR->SetLineColor(color);
  g_QR->SetMarkerStyle(20);
  g_QR->SetMarkerSize(1.2);
  g_QR->SetMarkerColor(color);
  TGraphErrors *g_gainQ = new TGraphErrors(good_run,g_sim,&v_q[0],eg_sim,&v_eq[0]);
  g_gainQ->SetLineColor(color);
  g_gainQ->SetMarkerStyle(20);
  g_gainQ->SetMarkerSize(1.2);
  g_gainQ->SetMarkerColor(color);
  TGraphErrors *g_gainS = new TGraphErrors(15,g_sim,&v_s[0],eg_sim,&v_es[0]);
  g_gainS->SetLineColor(color);
  g_gainS->SetMarkerStyle(20);
  g_gainS->SetMarkerSize(1.2);
  g_gainS->SetMarkerColor(color);
  TGraphErrors *g_gainE = new TGraphErrors(15,g_sim,&v_e[0],eg_sim,&v_ee[0]);
  g_gainE->SetLineColor(color);
  g_gainE->SetMarkerStyle(20);
  g_gainE->SetMarkerSize(1.2);
  g_gainE->SetMarkerColor(color);
  TGraphErrors *g_gainR = new TGraphErrors(15,g_sim,&v_r[0],eg_sim,&v_er[0]);
  g_gainR->SetLineColor(color);
  g_gainR->SetMarkerStyle(20);
  g_gainR->SetMarkerSize(1.2);
  g_gainR->SetMarkerColor(color);
  TGraphErrors *g_hvQ = new TGraphErrors(15,hv_exp,&v_q[0],ehv_exp,&v_eq[0]);
  g_hvQ->SetLineColor(color);
  g_hvQ->SetMarkerStyle(20);
  g_hvQ->SetMarkerSize(1.2);
  g_hvQ->SetMarkerColor(color);
  TGraphErrors *g_hvS = new TGraphErrors(15,hv_exp,&v_s[0],ehv_exp,&v_es[0]);
  g_hvS->SetLineColor(color);
  g_hvS->SetMarkerStyle(20);
  g_hvS->SetMarkerSize(1.2);
  g_hvS->SetMarkerColor(color);
  TGraphErrors *g_hvE = new TGraphErrors(15,hv_exp,&v_e[0],ehv_exp,&v_ee[0]);
  g_hvE->SetLineColor(color);
  g_hvE->SetMarkerStyle(20);
  g_hvE->SetMarkerSize(1.2);
  g_hvE->SetMarkerColor(color);
  TGraphErrors *g_hvR = new TGraphErrors(15,hv_exp,&v_r[0],ehv_exp,&v_er[0]);
  g_hvR->SetLineColor(color);
  g_hvR->SetMarkerStyle(20);
  g_hvR->SetMarkerSize(1.2);
  g_hvR->SetMarkerColor(color);
  

  mg_QS->Add(g_QS);
  mg_QE->Add(g_QE);
  mg_QR->Add(g_QR);
  mg_gainQ->Add(g_gainQ);
  mg_gainS->Add(g_gainS);
  mg_gainE->Add(g_gainE);
  mg_gainR->Add(g_gainR);
  mg_hvQ->Add(g_hvQ);
  mg_hvS->Add(g_hvS);
  mg_hvE->Add(g_hvE);
  mg_hvR->Add(g_hvR);



  
  leg->AddEntry(g_QS, legenda.c_str(), "lp");
}

TString Fill(){
  TString nome;

  /*
  //TUNED TIGER
  Add_scan_TIGER(10, 24 , 2, "Simulated data - TIGER");
  nome="TIGER_TUNED_DATA.pdf";
  */

  //TUNED APV
  Add_scan_APV(30, 39, 2, "Simulated data - APV");
  nome="APV_TUNED_DATA.pdf";
    
  // return macro
  return nome;
  
}


