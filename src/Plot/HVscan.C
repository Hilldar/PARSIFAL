#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TString.h"

using namespace std;

// Global containers shared with Add_scan_* helpers
TMultiGraph *mg_QS, *mg_QE, *mg_QR;
TMultiGraph *mg_gainQ, *mg_gainS, *mg_gainE, *mg_gainR;
TMultiGraph *mg_hvQ, *mg_hvS, *mg_hvE, *mg_hvR;
TLegend *leg;

// Experimental reference datasets
const int kSizeAPV = 10;
const double q_exp_apv[kSizeAPV]   = {7.53333, 15.9, 20.2667, 32.75, 50.6, 60.5667, 83.9333, 97.6333, 124.217, 140.533};
const double s_exp_apv[kSizeAPV]   = {1.375, 1.83, 2.015, 2.405, 2.945, 3.215, 4.03, 4.45, 5.26, 5.75};
const double e_exp_apv[kSizeAPV]   = {0.52, 0.86, 0.93, 0.96, 0.95, 0.95, 0.94, 0.94, 0.94, 0.94};
const double r_exp_apv[kSizeAPV]   = {112, 68, 58, 56, 57, 59, 50, 50, 50, 50};
const double g_exp_apv[kSizeAPV]   = {460, 880, 1100, 1700, 2600, 3250, 5000, 6250, 9700, 12000};
const double hv_exp_apv[kSizeAPV]  = {520, 550, 560, 580, 600, 610, 630, 640, 660, 670};
const double eq_exp_apv[kSizeAPV]  = {0.7, 1.6, 2.0, 3.2, 5.1, 6.0, 8.4, 9.7, 12.4, 14.0};
const double es_exp_apv[kSizeAPV]  = {0.14, 0.18, 0.2, 0.24, 0.29, 0.32, 0.4, 0.45, 0.526, 0.575};
const double ee_exp_apv[kSizeAPV]  = {0.02, 0.02, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01};
const double er_exp_apv[kSizeAPV]  = {8, 3, 3, 3, 3, 2, 1, 1, 1, 1};
const double ehv_exp_apv[kSizeAPV] = {0};

const int kSizeTIGER = 15;
const double q_exp_tiger[kSizeTIGER]   = {10.53, 10.2, 9.9, 9.96, 10.35, 11.54, 14.56, 19.45, 27.14, 37.39, 50.32, 62.23, 73.54, 84.53, 97.87};
const double s_exp_tiger[kSizeTIGER]   = {1.08, 1.08, 1.1, 1.12, 1.12, 1.14, 1.23, 1.39, 1.58, 1.77, 2.01, 2.22, 2.5, 2.99, 3.77};
const double e_exp_tiger[kSizeTIGER]   = {0.049, 0.065, 0.102, 0.2, 0.37, 0.616, 0.816, 0.91, 0.943, 0.959, 0.968, 0.972, 0.973, 0.975, 0.974};
const double r_exp_tiger[kSizeTIGER]   = {255, 222, 121, 110, 108, 110, 108, 96, 71, 68, 64, 61, 66, 58, 75};
const double g_exp_tiger[kSizeTIGER]   = {33.1818, 51.3402, 79.4357, 122.906, 190.166, 294.232, 455.249, 704.38, 1089.85, 1686.26, 2609.04, 4036.82, 6245.94, 9663.98, 14952.5};
const double hv_exp_tiger[kSizeTIGER]  = {400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680};
const double ehv_exp_tiger[kSizeTIGER] = {0};

static TGraphErrors* MakeStyledGraph(int n, const double* x, const double* y,
                                     const double* ex, const double* ey, int color) {
  TGraphErrors* g = new TGraphErrors(n, x, y, ex, ey);
  g->SetLineColor(color);
  g->SetLineWidth(2);
  g->SetMarkerStyle(20);
  g->SetMarkerSize(1.2);
  g->SetMarkerColor(color);
  return g;
}

static void Add_scan(int runID_i, int runID_f, const double* g_ref, const double* hv_ref,
                     int n_ref, int n_evt_err, int color, const string& legenda,
                     const TString& data_dir) {
  cout << "---------------------" << endl;
  cout << legenda << endl;
  vector<double> v_g, v_eg, v_hv, v_ehv;
  vector<double> v_q, v_s, v_e, v_r, v_eq, v_es, v_ee, v_er;

  for (int i_run = runID_i; i_run <= runID_f; i_run++) {
    int idx = i_run - runID_i;
    if (idx >= n_ref) break;
    TString filename = Form("%s/%i/summary.txt", data_dir.Data(), i_run);
    ifstream file(filename.Data());
    if (!file.good()) {
      cout << "WARNING: missing " << filename << ", skipping run " << i_run << endl;
      continue;
    }
    float Angle, Qind_MPV, Qread, RatioQ, Size, CCres, TPCres, T0res, Eff;
    if (!(file >> Angle >> Qind_MPV >> Qread >> RatioQ >> Size >> CCres >> TPCres >> T0res >> Eff)) {
      cout << "WARNING: malformed " << filename << ", skipping run " << i_run << endl;
      continue;
    }
    cout << i_run << "  Q=" << Qread << "  S=" << Size << "  R=" << CCres << "  E=" << Eff << endl;
    v_g.push_back(g_ref[idx]);
    v_eg.push_back(0.0);
    v_hv.push_back(hv_ref[idx]);
    v_ehv.push_back(0.0);
    v_q.push_back(Qread);
    v_eq.push_back(Qread * 0.05);
    v_s.push_back(Size);
    v_es.push_back(Size * 0.05);
    v_e.push_back(Eff);
    v_ee.push_back(sqrt(max(Eff * (1.0f - Eff), 0.0f) / n_evt_err));
    v_r.push_back(CCres);
    v_er.push_back(0.05 * CCres);
  }

  int n = static_cast<int>(v_q.size());
  if (n == 0) {
    cout << "ERROR: no valid runs found in " << runID_i << "-" << runID_f << endl;
    return;
  }

  TGraphErrors* g_QS = MakeStyledGraph(n, v_q.data(), v_s.data(), v_eq.data(), v_es.data(), color);
  mg_QS->Add(g_QS);
  mg_QE->Add(MakeStyledGraph(n, v_q.data(), v_e.data(), v_eq.data(), v_ee.data(), color));
  mg_QR->Add(MakeStyledGraph(n, v_q.data(), v_r.data(), v_eq.data(), v_er.data(), color));
  mg_gainQ->Add(MakeStyledGraph(n, v_g.data(), v_q.data(), v_eg.data(), v_eq.data(), color));
  mg_gainS->Add(MakeStyledGraph(n, v_g.data(), v_s.data(), v_eg.data(), v_es.data(), color));
  mg_gainE->Add(MakeStyledGraph(n, v_g.data(), v_e.data(), v_eg.data(), v_ee.data(), color));
  mg_gainR->Add(MakeStyledGraph(n, v_g.data(), v_r.data(), v_eg.data(), v_er.data(), color));
  mg_hvQ->Add(MakeStyledGraph(n, v_hv.data(), v_q.data(), v_ehv.data(), v_eq.data(), color));
  mg_hvS->Add(MakeStyledGraph(n, v_hv.data(), v_s.data(), v_ehv.data(), v_es.data(), color));
  mg_hvE->Add(MakeStyledGraph(n, v_hv.data(), v_e.data(), v_ehv.data(), v_ee.data(), color));
  mg_hvR->Add(MakeStyledGraph(n, v_hv.data(), v_r.data(), v_ehv.data(), v_er.data(), color));

  leg->AddEntry(g_QS, legenda.c_str(), "lp");
}

static void DrawPage(TCanvas* c1, TMultiGraph* mg, const char* title,
                     const char* xtitle, const char* ytitle,
                     double xmin, double xmax, double ymin, double ymax,
                     const TString& pdf_arg) {
  mg->Draw("APL");
  mg->GetXaxis()->SetRangeUser(xmin, xmax);
  mg->GetYaxis()->SetRangeUser(ymin, ymax);
  mg->SetTitle(title);
  mg->GetXaxis()->SetTitle(xtitle);
  mg->GetYaxis()->SetTitle(ytitle);
  mg->GetXaxis()->SetNdivisions(16);
  mg->GetYaxis()->SetNdivisions(16);
  leg->Draw();
  c1->SaveAs(pdf_arg, "pdf");
}

// Run with:
//   root -l -b -q 'HVscan.C("APV")'
//   root -l -b -q 'HVscan.C("TIGER")'
void HVscan(TString mode = "APV", TString data_dir = "../../data", TString out_dir = "../../pdf") {
  mode.ToUpper();
  cout << "HV scan plot macro - mode: " << mode << endl;

  mg_QS = new TMultiGraph();
  mg_QE = new TMultiGraph();
  mg_QR = new TMultiGraph();
  mg_gainQ = new TMultiGraph();
  mg_gainS = new TMultiGraph();
  mg_gainE = new TMultiGraph();
  mg_gainR = new TMultiGraph();
  mg_hvQ = new TMultiGraph();
  mg_hvS = new TMultiGraph();
  mg_hvE = new TMultiGraph();
  mg_hvR = new TMultiGraph();

  leg = new TLegend(0.50, 0.72, 0.88, 0.88);
  TString out_pdf;
  TString plot_title;

  if (mode == "APV") {
    double eg_apv[kSizeAPV];
    for (int i = 0; i < kSizeAPV; i++) eg_apv[i] = 0.1 * g_exp_apv[i];

    TGraphErrors* g_QS_exp = MakeStyledGraph(kSizeAPV, q_exp_apv, s_exp_apv, eq_exp_apv, es_exp_apv, kBlack);
    mg_QS->Add(g_QS_exp);
    mg_QE->Add(MakeStyledGraph(kSizeAPV, q_exp_apv, e_exp_apv, eq_exp_apv, ee_exp_apv, kBlack));
    mg_QR->Add(MakeStyledGraph(kSizeAPV, q_exp_apv, r_exp_apv, eq_exp_apv, er_exp_apv, kBlack));
    mg_gainQ->Add(MakeStyledGraph(kSizeAPV, g_exp_apv, q_exp_apv, eg_apv, eq_exp_apv, kBlack));
    mg_gainS->Add(MakeStyledGraph(kSizeAPV, g_exp_apv, s_exp_apv, eg_apv, es_exp_apv, kBlack));
    mg_gainE->Add(MakeStyledGraph(kSizeAPV, g_exp_apv, e_exp_apv, eg_apv, ee_exp_apv, kBlack));
    mg_gainR->Add(MakeStyledGraph(kSizeAPV, g_exp_apv, r_exp_apv, eg_apv, er_exp_apv, kBlack));
    mg_hvQ->Add(MakeStyledGraph(kSizeAPV, hv_exp_apv, q_exp_apv, ehv_exp_apv, eq_exp_apv, kBlack));
    mg_hvS->Add(MakeStyledGraph(kSizeAPV, hv_exp_apv, s_exp_apv, ehv_exp_apv, es_exp_apv, kBlack));
    mg_hvE->Add(MakeStyledGraph(kSizeAPV, hv_exp_apv, e_exp_apv, ehv_exp_apv, ee_exp_apv, kBlack));
    mg_hvR->Add(MakeStyledGraph(kSizeAPV, hv_exp_apv, r_exp_apv, ehv_exp_apv, er_exp_apv, kBlack));
    leg->AddEntry(g_QS_exp, "Experimental data - APV", "lp");

    Add_scan(30, 39, g_exp_apv, hv_exp_apv, kSizeAPV, 100, kRed, "Simulated data - APV (30-39)", data_dir);
    out_pdf = Form("%s/HVscan_APV.pdf", out_dir.Data());
    plot_title = "HV scan SIM + EXP (APV)";
  } else if (mode == "TIGER") {
    double eq_tiger[kSizeTIGER], es_tiger[kSizeTIGER], ee_tiger[kSizeTIGER], er_tiger[kSizeTIGER], eg_tiger[kSizeTIGER];
    for (int i = 0; i < kSizeTIGER; i++) {
      eq_tiger[i] = 0.1 * q_exp_tiger[i];
      es_tiger[i] = 0.1 * s_exp_tiger[i];
      ee_tiger[i] = 0.01 * e_exp_tiger[i];
      er_tiger[i] = 0.05 * r_exp_tiger[i];
      eg_tiger[i] = 0.1 * g_exp_tiger[i];
    }

    TGraphErrors* g_QS_exp = MakeStyledGraph(kSizeTIGER, q_exp_tiger, s_exp_tiger, eq_tiger, es_tiger, kBlack);
    mg_QS->Add(g_QS_exp);
    mg_QE->Add(MakeStyledGraph(kSizeTIGER, q_exp_tiger, e_exp_tiger, eq_tiger, ee_tiger, kBlack));
    mg_QR->Add(MakeStyledGraph(kSizeTIGER, q_exp_tiger, r_exp_tiger, eq_tiger, er_tiger, kBlack));
    mg_gainQ->Add(MakeStyledGraph(kSizeTIGER, g_exp_tiger, q_exp_tiger, eg_tiger, eq_tiger, kBlack));
    mg_gainS->Add(MakeStyledGraph(kSizeTIGER, g_exp_tiger, s_exp_tiger, eg_tiger, es_tiger, kBlack));
    mg_gainE->Add(MakeStyledGraph(kSizeTIGER, g_exp_tiger, e_exp_tiger, eg_tiger, ee_tiger, kBlack));
    mg_gainR->Add(MakeStyledGraph(kSizeTIGER, g_exp_tiger, r_exp_tiger, eg_tiger, er_tiger, kBlack));
    mg_hvQ->Add(MakeStyledGraph(kSizeTIGER, hv_exp_tiger, q_exp_tiger, ehv_exp_tiger, eq_tiger, kBlack));
    mg_hvS->Add(MakeStyledGraph(kSizeTIGER, hv_exp_tiger, s_exp_tiger, ehv_exp_tiger, es_tiger, kBlack));
    mg_hvE->Add(MakeStyledGraph(kSizeTIGER, hv_exp_tiger, e_exp_tiger, ehv_exp_tiger, ee_tiger, kBlack));
    mg_hvR->Add(MakeStyledGraph(kSizeTIGER, hv_exp_tiger, r_exp_tiger, ehv_exp_tiger, er_tiger, kBlack));
    leg->AddEntry(g_QS_exp, "Experimental data - TIGER", "lp");

    Add_scan(10, 24, g_exp_tiger, hv_exp_tiger, kSizeTIGER, 1000, kRed, "Simulated data - TIGER (10-24)", data_dir);
    out_pdf = Form("%s/HVscan_TIGER.pdf", out_dir.Data());
    plot_title = "HV scan SIM + EXP (TIGER)";
  } else {
    cout << "ERROR: unknown mode '" << mode << "'. Use \"APV\" or \"TIGER\"." << endl;
    return;
  }

  TCanvas* c1 = new TCanvas("c1", "", 1200, 600);
  c1->SetGrid();
  c1->SetGridx();
  c1->SetGridy();

  DrawPage(c1, mg_QS,    plot_title, "Charge [fC]", "Cluster Size",             0,   200, 0,   8,   out_pdf + "(");
  DrawPage(c1, mg_QE,    plot_title, "Charge [fC]", "Efficiency",               0,   200, 0,   1.2, out_pdf);
  DrawPage(c1, mg_QR,    plot_title, "Charge [fC]", "Spatial resolution [um]",  0,   200, 0,   400, out_pdf);
  DrawPage(c1, mg_gainQ, plot_title, "Gain",        "Charge [fC]",              0, 16000, 0,   200, out_pdf);
  DrawPage(c1, mg_gainS, plot_title, "Gain",        "Cluster Size",             0, 16000, 0,   8,   out_pdf);
  DrawPage(c1, mg_gainE, plot_title, "Gain",        "Efficiency",               0, 16000, 0,   1.2, out_pdf);
  DrawPage(c1, mg_gainR, plot_title, "Gain",        "Spatial resolution [um]",  0, 16000, 0,   400, out_pdf);
  DrawPage(c1, mg_hvQ,   plot_title, "HV [V]",      "Charge [fC]",            380,   700, 0,   200, out_pdf);
  DrawPage(c1, mg_hvS,   plot_title, "HV [V]",      "Cluster Size",           380,   700, 0,   8,   out_pdf);
  DrawPage(c1, mg_hvE,   plot_title, "HV [V]",      "Efficiency",             380,   700, 0,   1.2, out_pdf);
  DrawPage(c1, mg_hvR,   plot_title, "HV [V]",      "Spatial resolution [um]",380,   700, 0,   400, out_pdf + ")");

  cout << "Saved multi-page PDF: " << out_pdf << endl;
  delete c1;
}
