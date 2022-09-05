{
    gROOT->ProcessLine(".x macros/startana.C");
    Int_t nbinsx = 125;
    Double_t xlo = 0;
    Double_t xhi = 250;
    Int_t nbinsy = 125;
    Double_t ylo = 0;
    Double_t yhi = 750;
    TH2D *hst0 = new TH2D("hst0","hst0",nbinsx,xlo,xhi,nbinsy,ylo,yhi);
    TH2D *hst1 = new TH2D("hst1","hst1",nbinsx,xlo,xhi,nbinsy,ylo,yhi);
    ch->Draw("aph0:xdcos0>>hst0","goodevent","colz");
    ch->Draw("aph1:xdcos1>>hst1","goodevent","colz");
    hst0->SetDirectory(0);
    hst1->SetDirectory(0);
    TFile *ofile = new TFile("./hists/xdhists.root","recreate");
    hst0->Write();
    hst1->Write();
    ofile->Close();
}
