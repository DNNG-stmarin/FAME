#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"


void spFilterPlots(Int_t runnum, Int_t chnum)
{
  const char* format = "rootfiles/run%d_sp.root";
  char* fname;
  char* bname;
  sprintf(fname, format, runnum);
  sprintf(bname, "pt0_%d", chnum);
  TFile *f0 = TFile::Open(fname);
  TTree* t = (TTree*)f0->Get(bname);

  TCanvas *c0 = new TCanvas("c0", "", 1600, 900);
  c0->Divide(2, 2);

  c0->cd(1);
  t->Draw("Tflt[]:Iteration$>>tf(1280, 0, 1280, 256)", "", "colz");
  c0->cd(2);
  t->Draw("Eflt[]:Iteration$>>ef(1280, 0, 1280, 256)", "", "colz");
  c0->cd(3);
  t->Draw("tTrig>>tt0(256,0,1280)", "tTrig>0", "");
  c0->cd(4);
  t->Draw("tPeak>>tp0(256,0,1280)", "", "");

  auto *c1 = new TCanvas("c1", "", 1200, 600);
  c1->Divide(2,1);
  c1->cd(1);
  t->Draw("wf[]:Iteration$>>(1280, 0, 1280, 256)", "", "colz");
  c1->cd(2);
  t->Draw("peak>>ep0(256, 0, 800)", "peak>0", "");

}
