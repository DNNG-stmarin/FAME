

void chamber() {

  //open and read files
  TFile* coincFile = new TFile("CoincidenceTrees.root", "READ");
  TFile* out = new TFile("ChamberOutput.root", "RECREATE");
  if (!coincFile->IsOpen())
    printf("File did not successfully open\n");

  TDirectory* dir = (TDirectory*)coincFile->Get("54941;1");
  //coincFile->cd("0;1/");
  //coincFile->SetDirectory(gDirectory);

  if(coincFile->GetListOfKeys()->Contains("CoincidenceTree"))
    printf("\nCoincidenceTree found\n");
  TTree* coincTree = (TTree*)dir->Get("CoincidenceTree");
  //coincFile->GetObject("CoincidenceTree", coincTree);
  //TTree* coincTree = (TTree*)coincFile->Get("CoincidenceTree");
  //TFile* regFile = new TFile("regFile.root", "WRITE");

  TCanvas* canvas = new TCanvas("name", "name", 800, 500);
  canvas->SetLogy();

  //get psd energy
  TH1F* fissErg = new TH1F("fissErg", "ModFissDep;Fission Energy (MeVee);Counts", 500, 0, 0.005);
  coincTree->Draw("tDep>>fissErg", "", "");
  canvas->Update();

  TSpectrum *s = new TSpectrum(20);
  Int_t nfound = s->Search(fissErg, 10, "", 0.001);

  Double_t *xpeaks;
  xpeaks = s->GetPositionX();

  TLine* alpha = new TLine(xpeaks[0], 0, xpeaks[0], pow(10,5));
  TLine* fission = new TLine(xpeaks[1], 0, xpeaks[1], pow(10,5));
  alpha->Draw("SAME");
  fission->Draw("SAME");

  fissErg->GetXaxis()->SetRangeUser(xpeaks[0], xpeaks[1]);
  Double_t ycut = fissErg->GetMinimum();
  Double_t bincut = fissErg->GetMinimumBin();
  Double_t xcut = fissErg->GetXaxis()->GetBinCenter(bincut);
  cout << xcut << "\n";
  fissErg->GetXaxis()->SetRangeUser(0, 0.005);
  TLine* cut = new TLine(xcut, 0, xcut, pow(10,5));
  cut->SetLineColor(kRed);
  cut->Draw("SAME");

  Double_t binlimit = fissErg->FindLastBinAbove(0);
  Double_t xlimit = fissErg->GetXaxis()->GetBinCenter(binlimit);
  cout << xlimit << "\n";
  TLine* limit = new TLine(xlimit, 0, xlimit, pow(10,5));
  limit->SetLineColor(kRed);
  limit->Draw("SAME");

  out->cd();
  canvas->Write();
  //out->Close();
}
