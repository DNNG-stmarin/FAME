#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

void pAvsPh(Int_t runnum)
{
  const char* format = "rootfiles/run%d_evt.root";
  char        fname[128];
  sprintf(fname, format, runnum);
  TFile* f0 = TFile::Open(fname);
  TTree* t  = (TTree*)f0->Get("et");

  TH2D* anode[2];
  anode[0] = new TH2D("a0", "", 100, -100, 700, 100, -1000, 6.0e4);
  anode[1] = new TH2D("a1", "", 100, -100, 700, 100, -1000, 6.0e4);
  TH2D* sum[2];
  sum[0] = new TH2D("sum0", "", 100, -100, 2000, 100, -10000, 2.5e5);
  sum[1] = new TH2D("sum1", "", 100, -100, 2000, 100, -10000, 2.5e5);
  TGraph* wf[4];
  for (int eye = 0; eye < 4; eye++) { wf[eye] = new TGraph(); }
  Double_t area = 0;

  for (Long64_t eye = 0; eye < t->GetEntries(); eye++) {
    t->GetEntry(eye);
    for (int jay = 0; jay < 1280; jay++) {
      wf[0]->SetPoint(jay, jay, t->GetLeaf("a0wf")->GetValue(jay));
      wf[1]->SetPoint(jay, jay, t->GetLeaf("a1wf")->GetValue(jay));
      // wf[2]->SetPoint(jay, jay, t->GetLeaf("sum0wf")->GetValue(jay));
      // wf[3]->SetPoint(jay, jay, t->GetLeaf("sum1wf")->GetValue(jay));
    }
    area = wf[0]->Integral(t->GetLeaf("tTrig")->GetValue(0), t->GetLeaf("tTrig")->GetValue(0) + 450);
    anode[0]->Fill(t->GetLeaf("aph")->GetValue(0), area);
    area = wf[1]->Integral(t->GetLeaf("tTrig")->GetValue(1), t->GetLeaf("tTrig")->GetValue(1) + 450);
    anode[1]->Fill(t->GetLeaf("aph")->GetValue(1), area);
    // area = wf[0]->Integral(t->GetLeaf("sumtTrig")->GetValue(0), t->GetLeaf("sumtTrig")->GetValue(0) + 450);
    // sum[0]->Fill(t->GetLeaf("sumPh")->GetValue(0), area);
    // area = wf[1]->Integral(t->GetLeaf("sumtTrig")->GetValue(1), t->GetLeaf("sumtTrig")->GetValue(1) + 450);
    // sum[1]->Fill(t->GetLeaf("sumPh")->GetValue(1), area);
  }

  TCanvas* c0 = new TCanvas("c0", "", 1600, 900);
  c0->Divide(1, 2);
  c0->cd(1);
  anode[0]->Draw("colz");
  c0->cd(2);
  anode[1]->Draw("colz");
  // c0->cd(2);
  // sum[0]->Draw("colz");
  // c0->cd(4);
  // sum[1]->Draw("colz");
}
