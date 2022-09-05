Int_t
TTree::draw_meant(Int_t nbins, double lo, double hi)
{
    Int_t bnum = 0;
    Int_t chnum = 0;
    char names[6][100] = {"wf","c","a1","g1","a2","g2"};
    for (int i=0;i<1;++i)
    {
        if (gDirectory->Get(names[i]))
        {
            gDirectory->Get(names[i])->Delete();
        }
        TH1F *hist = new TH1F(names[i],names[i],nbins,lo,hi);
        Int_t goodevent = 0;
         for (int eye=0;eye<GetEntries()-1;++eye)
         {
      	    GetEntry(eye);
      	    Int_t lbnum = GetLeaf("bnum")->GetValue(0);
            Int_t lchnum = GetLeaf("chnum")->GetValue(0);
            if (lbnum == bnum && lchnum == chnum)
            {
               double stuff = GetLeaf(names[i])->GetValue(0);
      	       GetEntry(eye+1);
               double stuff2 = GetLeaf(names[i])->GetValue(0);
               hist->Fill((stuff2-stuff)*4.);
            }
        }

        hist->Draw();
    }
    return goodevent;
}

