#include "LiseHelper.hh"
#include <iostream>
#include <vector>
#include <memory>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TAxis.h>

using namespace std;

int main()
{
  TCanvas *canvas = new TCanvas("canvas", "canvas", 1600, 1600);
  canvas->SetGrid();
  canvas->SetMargin(0.12, 0.05, 0.10, 0.05);

  TLegend *legend_E1_E2 = new TLegend(0.60, 0.70, 0.90, 0.90);
  legend_E1_E2->SetNColumns(2);
  legend_E1_E2->SetFillColorAlpha(0, 0.8);
  TLegend *legend_E2_E3 = new TLegend(0.60, 0.70, 0.90, 0.90);
  legend_E2_E3->SetNColumns(2);
  legend_E2_E3->SetFillColorAlpha(0, 0.8);
  TLegend *legend_E1_El = new TLegend(0.60, 0.70, 0.90, 0.90);
  legend_E1_El->SetNColumns(2);
  legend_E1_El->SetFillColorAlpha(0, 0.8);
  TLegend *legend_E0_E1 = new TLegend(0.60, 0.70, 0.90, 0.90);
  legend_E0_E1->SetNColumns(2);
  legend_E0_E1->SetFillColorAlpha(0, 0.8);

  string runpath = BASEDIR "/run";
  string energy_unit = LiseHelper::GetEnergyUnit();
  if(energy_unit.substr(energy_unit.size() - 2) == "/u") {
    energy_unit.erase(energy_unit.size() - 2);
  }

  vector<string> items = LiseHelper::ListItem();
  vector<unique_ptr<TGraph>> E1_E2s; E1_E2s.reserve(items.size());
  vector<unique_ptr<TGraph>> E2_E3s; E2_E3s.reserve(items.size());
  vector<unique_ptr<TGraph>> E1_Els; E1_Els.reserve(items.size());
  vector<unique_ptr<TGraph>> E0_E1s; E0_E1s.reserve(items.size());
  size_t i = 0;
  double E0_max = 0, E1_max = 0, E2_max = 0, E3_max = 0, El_max = 0;
  for(const string &item : items) {
    LiseHelper lise(item);

    cout << lise.GetPath() << "\t"
         << lise.GetBeam() << "\t"
         << lise.GetTarget() << "\t"
         << lise.GetE2x()->GetN() << endl;

    string path = runpath + "/" + lise.GetBeam() + "_" + lise.GetTarget() + ".root";
    TFile *file = new TFile(path.c_str());
    if(!file->IsOpen()) {
      throw runtime_error(string("Failed to open file: ") + path);
    }
    TTree *tree = (TTree *)file->Get("tree");
    //int A, Z;
    vector<double> *E = nullptr;
    //tree->SetBranchAddress("A", &A);
    //tree->SetBranchAddress("Z", &Z);
    tree->SetBranchAddress("E", &E);

    TGraph *E1_E2 = new TGraph;
    TGraph *E2_E3 = new TGraph;
    TGraph *E1_El = new TGraph;
    TGraph *E0_E1 = new TGraph;
    E1_E2s.emplace_back(E1_E2);
    E2_E3s.emplace_back(E2_E3);
    E1_Els.emplace_back(E1_El);
    E0_E1s.emplace_back(E0_E1);
    for(int j = 0; j < tree->GetEntries(); j++) {
      tree->GetEntry(j);
      double E0 = E->at(0), E1 = E->at(1), E2 = E->at(2), E3 = E->at(3), El = E1 + E2 + E3;
      E0_max = max(E0_max, E0), E1_max = max(E1_max, E1), E2_max = max(E2_max, E2), E3_max = max(E3_max, E3), El_max = max(El_max, El);
      if(E2) E1_E2->AddPoint(E1, E2);
      if(E3) E2_E3->AddPoint(E2, E3);
      if(El > E1) E1_El->AddPoint(E1, El);
      E0_E1->AddPoint(E0, E1);
    }
    E1_E2->Sort();
    E2_E3->Sort();
    E1_El->Sort();
    E0_E1->Sort();

    E1_E2->SetLineColorAlpha(i++ + 21, 1.0);
    E1_E2->SetTitle((";E_{1} [" + energy_unit + "];E_{2} [" + energy_unit + "]").c_str());
    E1_E2->GetXaxis()->SetTitleOffset(1.1);
    E1_E2->GetYaxis()->SetTitleOffset(1.3);
    legend_E1_E2->AddEntry(E1_E2, (lise.GetBeam() + " on " + lise.GetTarget()).c_str());

    E2_E3->SetLineColorAlpha(i++ + 21, 1.0);
    E2_E3->SetTitle((";E_{2} [" + energy_unit + "];E_{3} [" + energy_unit + "]").c_str());
    E2_E3->GetXaxis()->SetTitleOffset(1.1);
    E2_E3->GetYaxis()->SetTitleOffset(1.4);
    legend_E2_E3->AddEntry(E2_E3, (lise.GetBeam() + " on " + lise.GetTarget()).c_str());

    E1_El->SetLineColorAlpha(i++ + 21, 1.0);
    E1_El->SetTitle((";E_{1} [" + energy_unit + "];E_{loss} [" + energy_unit + "]").c_str());
    E1_El->GetXaxis()->SetTitleOffset(1.1);
    E1_El->GetYaxis()->SetTitleOffset(1.6);
    legend_E1_El->AddEntry(E1_El, (lise.GetBeam() + " on " + lise.GetTarget()).c_str());

    E0_E1->SetLineColorAlpha(i++ + 21, 1.0);
    E0_E1->SetTitle((";E_{0} [" + energy_unit + "];E_{1} [" + energy_unit + "]").c_str());
    E0_E1->GetXaxis()->SetTitleOffset(1.1);
    E0_E1->GetYaxis()->SetTitleOffset(1.3);
    legend_E0_E1->AddEntry(E0_E1, (lise.GetBeam() + " on " + lise.GetTarget()).c_str());
  }

  for(i = E1_E2s.size(); i; --i) {
    TGraph *E1_E2 = E1_E2s[i - 1].get();
    E1_E2->GetXaxis()->SetRangeUser(0, E1_max * 1.2);
    E1_E2->GetYaxis()->SetRangeUser(0, E2_max * 1.2);
    E1_E2->Draw(i == E1_E2s.size() ? "AL" : "L");
  }
  legend_E1_E2->Draw();
  canvas->SaveAs((runpath + "/" + "E1_E2.pdf").c_str());
  canvas->SaveAs((runpath + "/" + "E1_E2.png").c_str());

  for(i = E2_E3s.size(); i; --i) {
    TGraph *E2_E3 = E2_E3s[i - 1].get();
    E2_E3->GetXaxis()->SetRangeUser(0, E2_max * 1.2);
    E2_E3->GetYaxis()->SetRangeUser(0, E3_max * 1.2);
    E2_E3->Draw(i == E2_E3s.size() ? "AL" : "L");
  }
  legend_E2_E3->Draw();
  canvas->SaveAs((runpath + "/" + "E2_E3.pdf").c_str());
  canvas->SaveAs((runpath + "/" + "E2_E3.png").c_str());

  for(i = E1_Els.size(); i; --i) {
    TGraph *E1_El = E1_Els[i - 1].get();
    E1_El->GetXaxis()->SetRangeUser(0, E1_max * 1.2);
    E1_El->GetYaxis()->SetRangeUser(0, El_max * 1.2);
    E1_El->Draw(i == E1_Els.size() ? "AL" : "L");
  }
  legend_E1_El->Draw();
  canvas->SaveAs((runpath + "/" + "E1_El.pdf").c_str());
  canvas->SaveAs((runpath + "/" + "E1_El.png").c_str());

  for(i = E0_E1s.size(); i; --i) {
    TGraph *E0_E1 = E0_E1s[i - 1].get();
    E0_E1->GetXaxis()->SetRangeUser(0, E0_max * 1.2);
    E0_E1->GetYaxis()->SetRangeUser(0, E1_max * 1.2);
    E0_E1->Draw(i == E0_E1s.size() ? "AL" : "L");
  }
  legend_E0_E1->Draw();
  canvas->SaveAs((runpath + "/" + "E0_E1.pdf").c_str());
  canvas->SaveAs((runpath + "/" + "E0_E1.png").c_str());

  delete legend_E0_E1;
  delete legend_E1_El;
  delete legend_E2_E3;
  delete legend_E1_E2;
  delete canvas;
  return 0;
}
