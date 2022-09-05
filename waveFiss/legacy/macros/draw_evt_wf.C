Int_t
TTree::draw_evt_wf(Int_t evtnum,Int_t bnum, Int_t chnum,Int_t ns_wf)
{
    // make and divide up the canvas
    //TCanvas *c = new TCanvas("c","c",1000,750);
     Int_t goodevent = 0;
    for (int eye=evtnum;eye<GetEntries();++eye)
    {
      GetEntry(eye);
      Int_t lbnum = GetLeaf("bnum")->GetValue(0);
      Int_t lchnum = GetLeaf("chnum")->GetValue(0);
      if (lbnum == bnum && lchnum == chnum)
      {
         goodevent = eye;
         break;
      }
    }
    char names[6][100] = {"wf","c","a1","g1","a2","g2"};
    for (int i=0;i<1;++i)
    {
        //c->cd(i+1);
        //Int_t len = GetLeaf(names[i])->GetNdata();
        Int_t len = ns_wf;
        if (gDirectory->Get(names[i]))
        {
            gDirectory->Get(names[i])->Delete();
        }
        TH1F *hist = new TH1F(names[i],names[i],len,0,len);
        for (int j=0;j<len;++j)
        {
            hist->SetBinContent(j,GetLeaf(names[i])->GetValue(j));
        }
        hist->Draw();
    }
    return goodevent;
}
