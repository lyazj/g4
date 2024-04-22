#include "LiseHelper.hh"
#include <iostream>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TAxis.h>

using namespace std;

int main()
{
  TCanvas *canvas = new TCanvas("canvas", "canvas", 1600, 1600);
  canvas->SetGrid();
  canvas->SetLogx();
  canvas->SetLogy();
  canvas->SetMargin(0.10, 0.05, 0.10, 0.05);

  TLegend *legend = new TLegend(0.60, 0.15, 0.90, 0.35);
  legend->SetNColumns(2);
  legend->SetFillColorAlpha(0, 0.8);

  vector<string> items = LiseHelper::ListItem();
  vector<LiseHelper> lises; lises.reserve(items.size());
  size_t i = 0;
  for(const string &item : items) {
    lises.emplace_back(item);
    LiseHelper &lise = lises.back();

    cout << lise.GetPath() << "\t"
         << lise.GetBeam() << "\t"
         << lise.GetTarget() << "\t"
         << lise.GetE2x()->GetN() << endl;

    string title;
    title += ";Energy [" + LiseHelper::GetEnergyUnit() + "]";
    title += ";Penetration depth [" + LiseHelper::GetDepthUnit() + "]";

    TGraph *E2x = lise.GetE2x();
    E2x->SetLineColor(i + 21);
    E2x->SetTitle(title.c_str());
    E2x->GetXaxis()->SetTitleOffset(1.3);
    E2x->GetYaxis()->SetTitleOffset(1.4);
    E2x->Draw(i++ == 0 ? "AL" : "L");
    legend->AddEntry(E2x, (lise.GetBeam() + " on " + lise.GetTarget()).c_str());
  }

  legend->Draw();
  canvas->SaveAs("Lise.pdf");
  canvas->SaveAs("Lise.png");

  delete legend;
  delete canvas;
  return 0;
}
