{
    TFile *ifile = new TFile("/home/smosby/dance/fare2/RootOutput_diag/Target_Histos40800.root");
    //TFile *ifile = new TFile("/tmp/Au_Histos40503.root");
    TTree *itree = (TTree*)ifile->Get("pt");
    float cryse[162];
    float en;
    itree->SetBranchAddress("cryse[162]",cryse);
    itree->SetBranchAddress("en",&en);
    int nevts = itree->GetEntries();
    TH1D *temphist = new TH1D("temphist","temphist",300,0,10);
    temphist->GetXaxis()->SetTitle("Esum");
    temphist->GetYaxis()->SetTitle("Cts");
    temphist->SetTitle("Example Title");
    for (int i=0;i<nevts;++i)
    {
        itree->GetEntry(i);
        int mcr = 0;
        double etot = 0;
        if (en<11 || en>13) continue;
        for (int j=0;j<162;++j)
        {
            if (cryse[j]>0)
            {
                mcr += 1;
                etot += cryse[j];
            }
        }
        if (mcr > 2)
        {
            temphist->Fill(etot);
        }
        if (i%10000==0)
        {
            printf("on event %i of %i\n",i,nevts);
        }
        if (i>10000) break;
    }
    temphist->Draw();
}
