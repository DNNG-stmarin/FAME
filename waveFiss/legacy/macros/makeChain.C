{
    TChain *wt = new TChain("wt");
    char buf[1000];
    for (Int_t i=1000;i<1020;++i)
    {
        sprintf(buf,"./data/data5_%i.root",i);
        wt->Add(buf);
    }
}
