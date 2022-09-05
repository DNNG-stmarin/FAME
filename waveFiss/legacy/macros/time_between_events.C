{
    TFile *ifile = new TFile("./tke_rootfiles/run1635_evt.root");
    //TFile *ifile = new TFile("./tke_rootfiles/run619_evt.root");
    TTree *itree = (TTree*)ifile->Get("et");
    Double_t dt;
    Double_t ct;
    Double_t aph[2];
    itree->SetBranchAddress("dt",&dt);
    itree->SetBranchAddress("ct",&ct);
    itree->SetBranchAddress("aph[2]",aph);

    Double_t last_dt;
    Double_t last_ct;
    Double_t last_aph;

    Int_t nevt = itree->GetEntries();
    itree->GetEntry(0);
    last_dt = dt;
    last_ct = ct;
    last_aph = aph[0];

    TH1D *hist_noc = new TH1D("hist_nc","hist_nc",18192,0,10240000);
    TH1D *hist_lot = new TH1D("hist_lot","hist_lot",1024,0,10240000);
    TH1D *hist_hit = new TH1D("hist_hit","hist_hit",1024,0,10240000);
    TH2D *hist2 = new TH2D("hist2","hist2",256,0,1024000,512,0,50e6);

    for (int i=1;i<nevt;++i)
    {
        itree->GetEntry(i);
        if (last_aph<-50&&aph[0]<-50)
        {
            hist_noc->Fill(last_ct-ct);
            if (dt<1e6)
            {
                hist_lot->Fill(last_ct-ct);
            }
            if (dt>15e6)
            {
                hist_hit->Fill(last_ct-ct);
            }
            hist2->Fill(last_ct-ct,last_dt);
            //cout << last_ct - ct << endl;
        }
        last_dt = dt;
        last_ct = ct;
        last_aph = aph[0];
    }
    hist_noc->Draw();
    hist_lot->SetLineColor(kRed);
    hist_lot->Scale(hist_noc->Integral()/hist_lot->Integral());
    hist_hit->SetLineColor(kBlue);
    hist_hit->Scale(hist_noc->Integral()/hist_hit->Integral());
    //hist_lot->Draw("same");
    //hist_hit->Draw("same");

    //hist2->Draw("colz");
}
