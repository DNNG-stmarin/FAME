{
    TFile *ifile = new TFile("processed_data/run82_wfproc.root");
    TTree *itree = (TTree*)ifile->Get("pt");
    // input branches
    UChar_t bnum;
    UChar_t chnum;
    ULong64_t ts; // 32 bit unsigned integer
    itree->SetBranchAddress("bnum",&bnum);
    itree->SetBranchAddress("chnum",&chnum);
    itree->SetBranchAddress("ts",&ts);
    // loop over the input tree
    Int_t nevt = itree->GetEntries();
    for (int i=0;i<nevt;++i)
    {
    }
}
