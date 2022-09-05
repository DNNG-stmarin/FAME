{
  Double_t ylolim = 150.; // 150 good start
  Double_t yhilim = 600.;
  Double_t thresh = 50.; //
  Double_t threshminus = 15;
  Double_t threshplus = 10;
  TFile *ifile = new TFile("./hists/xdhists.root");
  TH2D *hst0 = (TH2D*)ifile->Get("hst0");
  hst0->SetDirectory(0);
  ifile->Close();
  Int_t binlolim = hst0->GetYaxis()->FindBin(ylolim);
  Int_t binhilim = hst0->GetYaxis()->FindBin(yhilim);

  cout << "bin lo lim " << binlolim << " and hilim " << binhilim << endl;

  TGraph *gout = new TGraph();
  Int_t counter = 0;

  FILE *ofile = fopen("./hists/xd0.txt","w");

  for (Int_t i = binlolim; i < binhilim+1;++i){
    counter += 1;
    TH1D * temphist = hst0->ProjectionX("_px",i,i);

    // now sense the rising edge
    for (Int_t j = temphist->GetNbinsX();j>0;--j){
      Double_t ncts = temphist->GetBinContent(j);
      if (ncts > thresh){
        Int_t bll = j-threshminus; // bin lower limit
        Int_t bul = j+threshplus; // bin upper limit
        Int_t bbig = j;
        Double_t max_content = ncts;
        for (Int_t k = j; k>bll-1; --k){
          //cout << "Max counts " << max_content << " this bin " << temphist->GetBinContent(k) << " index " << k << endl;
          if (max_content<temphist->GetBinContent(k)){
            max_content =temphist->GetBinContent(k);
            bbig = k;
          }

        }
        if (bbig>bll){
          bbig -= 1;
        }
        Double_t bcll = temphist->GetXaxis()->GetBinCenter(bbig); // bin center low limit
        Double_t bcmid = temphist->GetXaxis()->GetBinCenter(j);  // bin center middle
        Double_t bcul = temphist->GetXaxis()->GetBinCenter(bul); // bin center upper limit
        TF1 *woodsaxon = new TF1("ws","[0]/(1+exp((x-[1])/[2]))",bcll,bcul);
        woodsaxon->SetParameter(0,temphist->GetBinContent(bll)); // height of the Woods-Saxon
        woodsaxon->SetParameter(1,bcmid); // mid point of the woods saxon
        woodsaxon->SetParameter(2,bcul-bcll); // diffuseness
        temphist->Fit(woodsaxon,"MRN0Q"); // prolly want N or 0 eventually
        Double_t temp = woodsaxon->GetParameter(1);
        Double_t temp2 = hst0->GetYaxis()->GetBinCenter(i);
        gout->SetPoint(counter,temp2,temp);
        fprintf(ofile,"%d %d\n",temp2,temp);
        break;
      }
    }
    //temphist->Draw();

  }

  fclose(ofile);
  gout->Draw("A*");

}
