void
TTree::draw_evt_wf(Int_t evtnum)
{
    // make and divide up the canvas
    TCanvas *c = new TCanvas("c","c",1000,750);
    c->Divide(2,3);
    c->cd(1);
    GetEntry(evtnum);
    char names[6][100] = {"t0","c","a1","g1","a2","g2"};
    //char names[6][100] = {"g1"};
    for (int i=0;i<6;++i)
    {
        c->cd(i+1);
        Int_t len = GetLeaf(names[i])->GetNdata();
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


    /*
    // Optional fit routine
    if (kFALSE)
    {
        c->cd(3);
        // Try to fit the dying exponential
        TF1 *expf = new TF1("expf","[0]*exp(-x/[1])+[2]",700,980);
        expf->SetParameter(0,4500.);
        expf->SetParameter(1,-60000.);
        expf->SetParameter(1,50000.);
        a1->Fit("expf","RM");
    }
    */

    return;
}
