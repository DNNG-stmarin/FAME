#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"


void evtFilterPlots(Int_t runnum)
{
  const char* format = "rootfiles/run%d_evt.root";
  char fname[128];
  sprintf(fname, format, runnum);
  TFile* f0 = TFile::Open(fname);
  TTree* t = (TTree*)f0->Get("et");

  TCanvas *c0 = new TCanvas("c0", "", 1600, 900);
  c0->Divide(2, 2);

  c0->cd(1);
  t->Draw("sum0Tflt[]:Iteration$>>tf(1280, 0, 1280, 256)", "", "colz");
  c0->cd(2);
  t->Draw("sum0Eflt[]:Iteration$>>ef(1280, 0, 1280, 256)", "", "colz");
  c0->cd(3);
  t->Draw("sumtTrig[0]>>tt0(256,0,1280)", "sumtTrig>0", "");
  c0->cd(4);
  t->Draw("sumtPeak[0]>>tp0(256,0,1280)", "", "");

  auto *c1 = new TCanvas("c1", "", 1200, 600);
  c1->Divide(2,1);
  c1->cd(1);
  t->Draw("sum0wf[]:Iteration$>>(1280, 0, 1280, 256)", "", "colz");
  c1->cd(2);
  t->Draw("sumph[0]>>ep0(100)", "", "");

}
