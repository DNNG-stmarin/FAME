{
    TFile *ifile = new TFile("processed_data/run92.root");
    TTree *itree = (TTree*)ifile->Get("t");

    TFile *ofile = new TFile("processed_data/run92_wfproc.root","recreate");
    TTree *otree = new TTree("pt","Pulse tree from waveform analysis");
    //
    ULong64_t last_ts[2][8];
    ULong64_t ts_base[2][8];
    for (int i=0;i<2;++i)
    {
        for (int j=0;j<8;++j)
        {
            last_ts[i][j]=0;
            ts_base[i][j]=0;
        }
    }

    // input branches
    UChar_t bnum;
    UChar_t chnum;
    UInt_t ts; // 32 bit unsigned integer
    itree->SetBranchAddress("bnum",&bnum);
    itree->SetBranchAddress("chnum",&chnum);
    itree->SetBranchAddress("ts",&ts);


    // output branches
    UChar_t bnum_out;
    UChar_t chnum_out;
    ULong64_t ts_out;
    otree->Branch("bnum",&bnum_out);
    otree->Branch("chnum",&chnum_out);
    otree->Branch("ts",&ts_out);

    // loop over the input tree
    Int_t nevt = itree->GetEntries();
    for (int i=0;i<nevt;++i)
    {
        itree->GetEntry(i);
        // check timestamps for rollover
        if (ts<last_ts[bnum][chnum])
        {
            printf("hey i found rollover on board %i channel %i event %i\n",bnum,chnum,i);
            ts_base[bnum][chnum] =ts_base[bnum][chnum] + pow(2,32);
            cout << "My ts base is now " << ts_base[bnum][chnum] << endl;

        }
        ts_out = ts_base[bnum][chnum] + ts;
        last_ts[bnum][chnum] = ts;
        // build rest of tree
        bnum_out = bnum;
        chnum_out = chnum;
        otree->Fill();
    }
    otree->Write();
    ofile->Close();
    ifile->Close();
}
