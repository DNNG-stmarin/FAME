{
    Double_t xlolim = 1.08; // 150 good start
    Double_t xhilim = 3.;
    TFile *ifile = new TFile("./hists/energyloss.root");
    TH2D *hst0 = (TH2D*)ifile->Get("hst0");
    hst0->SetDirectory(0);
    ifile->Close();
    Int_t binlolim = hst0->GetXaxis()->FindBin(xlolim);
    Int_t binhilim = hst0->GetXaxis()->FindBin(xhilim);

    FILE *ofile = fopen("./hists/eloss0.txt","w");
    TGraph *gout = new TGraph();
    Int_t counter = 0;

    for (Int_t i = binlolim; i < binhilim+1;++i)
    {
        counter += 1;
        TH1D * temphist = hst0->ProjectionY("_py",i,i);
        Double_t mean = temphist->GetMean();
        Double_t location = hst0->GetXaxis()->GetBinCenter(i);
        fprintf(ofile,"%f %f\n",location,mean);
        gout->SetPoint(counter,location,mean);
    }
    fclose(ofile);
    gout->SetMarkerColor(kRed);
    gout->Draw("A*");
    gout->Fit("pol1","","",xlolim,xhilim);

}
