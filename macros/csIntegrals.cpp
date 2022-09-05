#include <fstream>

void csIntegrals() {

  gROOT->SetBatch(kTRUE);
  ofstream myfile;
  string filename = "edgeLocations.csv";
  myfile.open(filename);

  const int numDet = 54;
  TString prefHist = "Integrals/i0_liquid";
  TString postHist = "_integral3_baseRestored";
  TString nameHist, nameDet;

  TFile* fileCs = new TFile("csCalib.root", "READ");
  TFile* out = new TFile("CSOutput.root", "RECREATE");
  TFolder* histos = (TFolder*)fileCs->Get("histos");

  TH1D** listCsHist;
  listCsHist = new TH1D* [numDet];

  for(int d=0; d<numDet; d++) {
    if(d<10)
      nameDet = "0" + to_string(d);
    else
      nameDet = to_string(d);
    //cout << "reading detector " << nameDet << " at ";

    nameHist = prefHist+nameDet+postHist;
    listCsHist[d] = (TH1D*)histos->FindObject(nameHist);
    //cout << listCsHist[d] << endl;

    TCanvas* canvas = new TCanvas(nameHist, nameHist, 800, 500);
    listCsHist[d]->Draw();
    canvas->Update();

    //x range limit
    double xlim = listCsHist[d]->GetXaxis()->GetBinCenter(listCsHist[d]->FindLastBinAbove(100));

    //compton edge limit
    double comptbinlim = listCsHist[d]->FindLastBinAbove(3000);
    double comptxlim = listCsHist[d]->GetXaxis()->GetBinCenter(comptbinlim);
    listCsHist[d]->GetXaxis()->SetRangeUser(0, xlim);
    TLine* comptLine = new TLine(comptxlim, 0, comptxlim, gPad->GetUymax());
    comptLine->SetLineColor(kRed);
    comptLine->Draw("SAME");

    //cout << comptbinlim << " " << comptxlim << endl;

    //first peak
    double ultymax = listCsHist[d]->GetMaximum();
    double ultbinmax = listCsHist[d]->GetMaximumBin();
    double ultxmax = listCsHist[d]->GetXaxis()->GetBinCenter(ultbinmax);
    TLine* ultmaxLine = new TLine(ultxmax, 0, ultxmax, gPad->GetUymax());
    ultmaxLine->SetLineColor(kBlue);
    ultmaxLine->Draw("SAME");

    //approximation of divide
    double divide = 0.0015;
    TLine* divideLine = new TLine(divide, 0, divide, gPad->GetUymax());
    divideLine->SetLineColor(kBlack);
    divideLine->Draw("SAME");
    listCsHist[d]->GetXaxis()->SetRangeUser(divide, xlim);

    //second peak
    double ymax = listCsHist[d]->GetMaximum();
    double binmax = listCsHist[d]->GetMaximumBin();
    double xmax = listCsHist[d]->GetXaxis()->GetBinCenter(binmax);
    TLine* maxLine = new TLine(xmax, 0, xmax, gPad->GetUymax());
    maxLine->SetLineColor(kGreen);
    maxLine->Draw("SAME");

    listCsHist[d]->GetXaxis()->SetRangeUser(0, xlim);

    double percent = (comptxlim-xmax)*(1-0.9) + xmax;
    TLine* percentLine = new TLine(percent, 0, percent, gPad->GetUymax());
    percentLine->SetLineColor(kCyan);
    percentLine->Draw("SAME");

    myfile << percent << "\n";

    out->cd();
    canvas->Write();
  }

  gROOT->SetBatch(kFALSE);
  myfile.close();
  out->Close();
}
