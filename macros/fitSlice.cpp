

void fitSlice(){

  //open and read files
  TFile* coincFile = new TFile("CoincidenceTrees.root", "READ");
  TFile* out = new TFile("TestOutput.root", "RECREATE");
  if (!coincFile->IsOpen())
    printf("File did not successfully open\n");

  TDirectory* dir = (TDirectory*)coincFile->Get("0;1");
  //coincFile->cd("0;1/");
  //coincFile->SetDirectory(gDirectory);

  if(coincFile->GetListOfKeys()->Contains("CoincidenceTree"))
    printf("\nCoincidenceTree found\n");
  TTree* coincTree = (TTree*)dir->Get("CoincidenceTree");
  //coincFile->GetObject("CoincidenceTree", coincTree);
  //TTree* coincTree = (TTree*)coincFile->Get("CoincidenceTree");
  //TFile* regFile = new TFile("regFile.root", "WRITE");

  TCanvas* canvas = new TCanvas("name", "name", 800, 500);

  //get psd energy
  TH2F* psdErg = new TH2F("psdErg", "psdErg;lightoutput MeVee;psd", 100, 0, 4, 200, 0, 0.4);
  coincTree->Draw("totPSP:totDep>>psdErg","totPSP>0 & totChan==2", "COLZ");
  canvas->Update();

  int step_size = 1;
  int bins = psdErg->GetNbinsX();
  Double_t energySlice[bins]; //energy slice
  Double_t disc[bins]; //discrimination
  int n=0;
  cout << bins << endl;

  //loop through bins in psd energy
  for(int i=0; i<bins; i+=step_size) {
    //projection must have certain num entries to be analyzed
    if(psdErg->ProjectionY("psdErgSlice", i, i+step_size)->GetEntries() > 500) { //min num entries (500)
      cout << psdErg->ProjectionY("psdErgSlice", i, i+step_size)->GetEntries() << endl;
      TH1F* psdErgSlice = (TH1F*)psdErg->ProjectionY("psdErgSlice", i, i+step_size); //only 1 bin or 2 bins (like 1-2)
      psdErgSlice->Draw();
      n++;

      //bounds
      double minPSD_fit = 0;
      double maxPSD_fit = 0.3;

      //photon and neutron fits
      TF1* fitPSD_n = new TF1("fitPSDn", "[0]*e^(-(x - [1])^2/(2*[2]^2))", minPSD_fit, maxPSD_fit);
      TF1* fitPSD_p = new TF1("fitPSDp", "[0]*e^(-(x - [1])^2/(2*[2]^2))", minPSD_fit, maxPSD_fit);

      //smooth histograms before fitting
      TH1F* smooth = (TH1F*) psdErgSlice->Clone();
      smooth->SetLineColor(kRed);
      smooth->Smooth(1);
      //smooth->Draw("SAME");

      //attributes of each fit
      TF1* fitPSD_np = new TF1("fitPSDnp", "fitPSDn + fitPSDp");
      TF1* intersection = new TF1("intersect", "fitPSDp - fitPSDn");
      smooth->GetXaxis()->SetRangeUser(0,0.18);
      Double_t pMax = smooth->GetMaximum();
      Double_t binpMax = smooth->GetMaximumBin();
      Double_t xpMax = smooth->GetXaxis()->GetBinCenter(binpMax);
      Double_t pMean = smooth->GetMean();
      Double_t pRMS = smooth->GetRMS();
      smooth->GetXaxis()->SetRangeUser(0.18, 0.3);
      Double_t nMax = smooth->GetMaximum();
      Double_t binnMax = smooth->GetMaximumBin();
      Double_t xnMax = smooth->GetXaxis()->GetBinCenter(binnMax);
      Double_t nMean = smooth->GetMean();
      Double_t nRMS = smooth->GetRMS();

      //reset
      smooth->GetXaxis()->SetRangeUser(0,0.3);

      //set parameters for combined fit
      fitPSD_np->SetParNames("AP", "mP", "sP", "AN", "mN", "sN");
      cout << "      " << pMax << " " << pMean << " " << pRMS << " " << nMax << " " << nMean << " " << nRMS << endl;
      fitPSD_np->SetParameters(pMax, pMean, pRMS, nMax, nMean, nRMS); //helped by smoothing
      intersection->SetParameters(pMax, pMean, pRMS, nMax, nMean, nRMS);
      fitPSD_np->SetLineColor(kGreen);
      fitPSD_np->Draw("SAME");

      //standard psd
      //Double_t psd = (xpMax+xnMax)/2;
      //Double_t ypsd = fitPSD_np->Eval(psd);
      //cout << psd << " " << ypsd << endl;

      //NEEDS TO BE THE FIT THAT MIN AND MAX IS TAKEN
      //Double_t xtemp = fitPSD_np->GetMinimumX(xpMax, xnMax);

      //if(ypsd >= 1) {
        //cout << "temp: " << xtemp;
        //psd = xtemp;
      //}

      //intersection psd
      fitPSD_p->SetParameters(pMax, pMean, pRMS);
      fitPSD_n->SetParameters(nMax, nMean, nRMS);
      //fit
      fitPSD_p->SetLineColor(kOrange+8);
      //fitPSD_p->Draw("SAME");
      fitPSD_n->SetLineColor(kRed);
      //fitPSD_n->Draw("SAME");

      //remove absolute value
      //TF1* intersection = new TF1("intersect", "fitPSDp - fitPSDn"); //i have no idea why this is not working
      //fitPSD_p->SetParameters(pMax-5, pMean, pRMS-0.002); //setting parameters is not necessary either
      //fitPSD_n->SetParameters(nMax-5, nMean, nRMS-0.002);
      intersection->SetLineColor(kYellow);
      //intersection->Draw("SAME");

      Double_t psd = intersection->GetX(0, xpMax, xnMax);

      psdErgSlice->GetXaxis()->SetRangeUser(0,0.3);
      cout << endl << endl << endl;
      canvas->Update();
      TLine* line = new TLine(psd, 0, psd, gPad->GetUymax());
      line->SetLineColor(kBlue);
      line->Draw("SAME");

      disc[i] = psd;
      energySlice[i] = psdErgSlice->GetBin(i);

      out->cd();
      canvas->Write();
    }
  }

  for(int i=0; i<n; i++) {
    cout << "x:" << energySlice[i] << " ";
    cout << "y:" << disc[i] << " \n";
  }
  cout << endl;

  TGraph* gr = new TGraph(n, energySlice, disc);
  TCanvas* c = new TCanvas("c", "c", 800, 500);
  gr->Draw("AC*");
  gr->Write();

  out->Close();
}
