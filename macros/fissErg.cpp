

void fissErg() {

  gROOT->SetBatch(kTRUE);

  //search file and tree
  TFile* coincFile = new TFile("Fiss.root", "READ");
  TFile* out = new TFile("FissOutput.root", "RECREATE");
  if (!coincFile->IsOpen())
    printf("File did not successfully open\n");

  if(coincFile->GetListOfKeys()->Contains("Fiss"))
    printf("\nFiss found\n");

  TTree* coincTree = (TTree*)coincFile->Get("Fiss");

  //naming
  TString firstbase = "fisErg > ";
  TString confirst;
  TString secondbase = " & fisErg < ";
  TString consecond;
  TString selectBin = "fisErg > 0 & fisErg < 0.2";
  double min = 0;
  double max = 0.2;
  TString sMin = to_string(min);
  TString sMax = to_string(max);
  TString name;

  //
  TH2F *h = new TH2F("h", "h", 10, 0, 9, 10, 0 ,9);
  int bins = 21;
  Double_t neutron[bins];
  Double_t neutronerr[bins];
  Double_t gamma[bins];
  Double_t gammaerr[bins];
  Double_t fissenergy[bins];
  Double_t cov[bins];
  Double_t zero[bins];

  for (int i=0; i<bins; i++) {
    name = selectBin;
    TCanvas* canvas = new TCanvas(selectBin, selectBin, 800, 500);
    coincTree->Draw("neutronMult:gammaMult>>h", selectBin, "COLZ");
    canvas->Update();

    neutron[i] = h->GetMean(2);
    neutronerr[i] = h->GetMeanError(2);
    gamma[i] = h->GetMean(1);
    gammaerr[i] = h->GetMeanError(1);
    fissenergy[i] = (min+max)/2;
    cov[i] = h->GetCovariance();
    zero[i] = 0;

    min += 0.4;
    max += 0.4;
    sMin = to_string(min);
    sMax = to_string(max);
    confirst = firstbase + sMin;
    consecond = secondbase + sMax;
    selectBin = confirst + consecond;

    out->cd();
    canvas->Write();
  }

  for(int i=0; i<bins; i++) {
    cout << fissenergy[i] << " " << neutron[i] << " " << gamma[i] << " " << cov[i] << endl;
    cout << neutronerr[i] << " " << gammaerr[i] << "\n\n";
  }

  TCanvas* multcanvas = new TCanvas("mults", "Fission Energy Multiplicity", 800, 500);
  TGraphErrors* g2 = new TGraphErrors(bins, fissenergy, neutron, zero, neutronerr);
  TGraphErrors* g1 = new TGraphErrors(bins, fissenergy, gamma, zero, gammaerr);
  g1->SetMarkerColor(5);
  g1->SetTitle("Neutron and Gamma Ray Multiplicity vs Fission Energy;Fission Energy (A.U.);Multiplicity");
  g1->Draw("AC*");
  g2->SetMarkerColor(kBlue);
  g2->Draw("C*");
  TLegend* legend = new TLegend();
  //legend->SetHeader("Legend", "C");
  legend->AddEntry(g1, "Gamma Ray Multiplicity");
  legend->AddEntry(g2, "Neutron Multiplicity");
  legend->Draw();
  multcanvas->Update();
  multcanvas->Write();

  TCanvas* covcanvas = new TCanvas("cov", "Covariance", 800, 500);
  TGraph* g3 = new TGraph(bins, fissenergy, cov);
  g3->SetMarkerColor(kRed);
  g3->SetTitle("Multiplicity Covariance vs Fission Energy;Fission Energy;Covariance");
  g3->Draw("AC*");
  covcanvas->Update();
  covcanvas->Write();


  gROOT->SetBatch(kFALSE);
  out->Close();
}
