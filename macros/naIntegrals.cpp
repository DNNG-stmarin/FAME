#include <fstream>

void naIntegrals() {

  gROOT->SetBatch(kTRUE);
  ofstream myfile;
  string filename = "edgeLocationsNA.csv";
  myfile.open(filename);

  const int numDet = 54;
  const int desiredPeak = 3;
  TString prefHist = "Integrals/i0_liquid";
  TString postHist = "_integral3_baseRestored";
  TString nameHist, nameDet;

  TFile* fileNa = new TFile("naCalib.root", "READ");
  TFile* out = new TFile("NAOutput.root", "RECREATE");
  TFolder* histos = (TFolder*)fileNa->Get("histos");

  TH1D** listNaHist;
  listNaHist = new TH1D* [numDet];

  for(int d=0; d<numDet; d++) {
    if(d<10)
      nameDet = "0" + to_string(d);
    else
      nameDet = to_string(d);

    nameHist = prefHist+nameDet+postHist;
    listNaHist[d] = (TH1D*)histos->FindObject(nameHist);

    TCanvas* canvas = new TCanvas(nameHist, nameHist, 800, 500);
    listNaHist[d]->Draw();
    canvas->Update();

    TH1D* smooth = (TH1D*) listNaHist[d]->Clone();
    smooth->SetLineColor(kRed);
    smooth->Smooth(2);
    smooth->Draw("SAME");
    canvas->Update();

    //x range limit
    double xlim = smooth->GetXaxis()->GetBinCenter(smooth->FindLastBinAbove(300));
    smooth->GetXaxis()->SetRangeUser(0, xlim);

    TSpectrum *s = new TSpectrum(100*desiredPeak);
    Int_t nfound = s->Search(smooth, 25, "", 0.01); //30, 0.08
    cout << "\nDetector " << d << ": Found " << nfound << " peaks\n";

    Double_t *xpeaks;
    xpeaks = s->GetPositionX();
    Double_t linesx[3];
    Double_t linesy[3];

    std::sort(xpeaks, xpeaks+nfound);

    int i=0;
    for(int p=0; p<nfound; p++) {
      if(p==0 && (xpeaks[p]<0.00052 || xpeaks[p]>0.0007)) {
        nfound = 0;
        continue;
      }
      if((nfound>desiredPeak) && p==2) {
          continue;
      }

      Double_t xp = xpeaks[p];
      Int_t bin = smooth->GetXaxis()->FindBin(xp);
      Double_t yp = smooth->GetBinContent(bin);
      TLine* line = new TLine(xp, 0, xp, gPad->GetUymax());
      line->SetLineColor(kBlack);
      if(p!=0)  line->Draw("SAME");
      cout << "   " << xp;
      linesx[i] = xp;
      linesy[i] = yp;
      i++;
    }

    if(nfound<1) {
      cout << "   " << "\"0.00055\"";
      TLine* optLine = new TLine(0.00055, 0, 0.00055, gPad->GetUymax());
      optLine->SetLineColor(kBlue);
      optLine->Draw("SAME");
      linesx[i] = 0.00055;
      linesy[i] = smooth->GetBinContent(smooth->GetXaxis()->FindBin(0.00055));
      i++;
    }

    if(nfound<2) {
      cout << "   " << "\"0.0015\"";
      TLine* optLine = new TLine(0.0015, 0, 0.0015, gPad->GetUymax());
      optLine->SetLineColor(kBlue);
      optLine->Draw("SAME");
      linesx[i] = 0.0015;
      linesy[i] = smooth->GetBinContent(smooth->GetXaxis()->FindBin(0.0015));
      i++;
    }

    if(nfound<desiredPeak) {
      cout << "   " << "\"0.0052\"";
      TLine* optLine = new TLine(0.0052, 0, 0.0052, gPad->GetUymax());
      optLine->SetLineColor(kBlue);
      optLine->Draw("SAME");
      linesx[i] = 0.0052;
      linesy[i] = smooth->GetBinContent(smooth->GetXaxis()->FindBin(0.0052));
      i++;
    }
    cout << "\n";

    for(int i=0; i<desiredPeak; i++) {
      cout << "   " << linesx[i];
    }
    cout << "\n";

    for(int i=0; i<desiredPeak; i++) {
      cout << "   " << linesy[i];
    }
    cout << "\n";

    double comptbinSec = smooth->FindLastBinAbove(1200);
    double comptxSec = smooth->GetXaxis()->GetBinCenter(comptbinSec);
    TLine* comptLineSec = new TLine(comptxSec, 0, comptxSec, gPad->GetUymax());
    comptLineSec->SetLineColor(kRed);
    comptLineSec->Draw("SAME");

    //double comptbinSec = smooth->FindLastBinAbove(20000);
    double comptxFirst = 0.0025;
    TLine* comptLineFirst = new TLine(comptxFirst, 0, comptxFirst, gPad->GetUymax());
    comptLineFirst->SetLineColor(kRed);
    comptLineFirst->Draw("SAME");

    double percentFirst = (comptxFirst-linesx[1])*(1-0.9) + linesx[1];
    TLine* percentLineFirst = new TLine(percentFirst, 0, percentFirst, gPad->GetUymax());
    percentLineFirst->SetLineColor(kCyan);
    percentLineFirst->Draw("SAME");

    double percentSec = (comptxSec-linesx[2])*(1-0.9) + linesx[2];
    TLine* percentLineSec = new TLine(percentSec, 0, percentSec, gPad->GetUymax());
    percentLineSec->SetLineColor(kCyan);
    percentLineSec->Draw("SAME");

    cout << "\n";
    myfile << percentFirst << "," << percentSec << "\n";
    out->cd();
    canvas->Write();
  }

  gROOT->SetBatch(kFALSE);
  myfile.close();
  out->Close();
}
