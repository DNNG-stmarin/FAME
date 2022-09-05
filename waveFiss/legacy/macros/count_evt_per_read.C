{
    //TFile *ifile = new TFile("./tke_rootfiles/run1755.root"); // good canonical lujan run
    //TFile *ifile = new TFile("./tke_rootfiles/run1523.root"); // channel 2 triggering too much
    //TFile *ifile = new TFile("./tke_rootfiles/run1570.root"); // all detectors on - no downscale
    //TFile *ifile = new TFile("./tke_rootfiles/run1635.root"); // first run w/ good trigger, downscaling
    //TFile *ifile = new TFile("./tke_rootfiles/run1905.root"); // first run w/ gamma dets in
    TFile *ifile = new TFile("./tke_rootfiles/run619.root"); // a WNR run
    TTree *itree = (TTree*)ifile->Get("t");

    UChar_t bnum,chnum;
    UChar_t bnum_old,chnum_old;
    UChar_t bnum_want = 0;
    UChar_t chnum_want = 0;

    itree->SetBranchAddress("bnum",&bnum);
    itree->SetBranchAddress("chnum",&chnum);

    TH1D *hist = new TH1D("hist","hist",1030,0,1030);

    Int_t nevt = itree->GetEntries();

    itree->GetEntry(0);
    bnum_old = bnum;
    chnum_old = chnum;
    Int_t number_in_read = 1;

    for (int i=1;i<nevt;++i)
    {
        if (i%100000==0)
        {
            cout << "on event " << i << " of " << nevt << endl;
        }
        itree->GetEntry(i);
        if (chnum==chnum_old && bnum==bnum_old)
        {
            number_in_read += 1;
        } else
        {
            if (chnum==chnum_want && bnum==bnum_want)
            {
                hist->Fill(number_in_read);
            }
            number_in_read = 1;
            chnum_old = chnum;
            bnum_old = bnum;
        }

    }
    hist->Draw();
}
