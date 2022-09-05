{
    gROOT->ProcessLine(".x macros/startana.C");

    TH2D *tkelin = new TH2D("tkelin","tkelin",150,0,50,110,0,220);
    tkelin->GetXaxis()->SetTitle("Neutron Kinetic Energy (MeV)");
    tkelin->GetYaxis()->SetTitle("Total Kinetic Energy (MeV)");
    TH2F *tkelog = twodhist("tkelog",40,0.1,200,110,0,220);
    //TH2F *tkelog = twodhist("tkelog",10,0.1,200,110,0,220);
   //TH2F *tkelog = twodhist("tkelog",20,0.1,100,110,0,220);
    tkelog->GetXaxis()->SetTitle("Neutron Kinetic Energy (MeV)");
    tkelog->GetYaxis()->SetTitle("Total Kinetic Energy (MeV)");

    // functional form for fitting
    TF1 *gaus = new TF1("gaus","gaus",150,220);

    // functional form of U5 theory
    TF1 *pol = new TF1("pol","169.13 - 0.266*x",0,9);

    /*
    TCanvas *clin = new TCanvas("clin","clin",800,600);
    clin->cd(1);
    //ch->Draw("tke_roughcal:nke>>tkelin","goodevent","colz");
    ch->Draw("tke_roughcal:nke>>tkelin","goodevent&&cos0>0.15","colz");
    gPad->SetLogz(1);
    tkelin->FitSlicesY(gaus);
    TH1D *tkelin_means = gDirectory->Get("tkelin_1");
    tkelin_means->GetXaxis()->SetTitle("Neutron Kinetic Energy (MeV)");
    tkelin_means->GetYaxis()->SetTitle("Total Kinetic Energy (MeV)");
    tkelin_means->SetTitle("");
    tkelin_means->Draw();
    pol->Draw("same");

    TLegend *legend = new TLegend(0.7,0.8,0.99,0.99);
    legend->AddEntry(pol,"Madland 2006");
    legend->AddEntry(tkelin_means,"WNR Data");
    legend->Draw("same");
    TCanvas *logc = new TCanvas("logc","logc",800,600);
    logc->cd(1);
    gPad->SetLogz(1);
    */
    //gPad->SetLogx(1);
    ch->Draw("tke_roughcal:nke>>tkelog","goodevent","colz");
    //ch->Draw("tke_roughcal:nke>>tkelog","goodevent&&cos0>0.15","colz");
    tkelog->FitSlicesY(gaus);
    TH1D *tkelog_means = gDirectory->Get("tkelog_1");
    TH1D *tkelog_widths = gDirectory->Get("tkelog_2");
    tkelog_means->GetXaxis()->SetTitle("Neutron Kinetic Energy (MeV)");
    tkelog_means->GetYaxis()->SetTitle("Total Kinetic Energy (MeV)");
    tkelog_means->SetTitle("");
    tkelog_means->Draw();
    tkelog_widths->GetXaxis()->SetTitle("Neutron Kinetic Energy (MeV)");
    tkelog_widths->GetYaxis()->SetTitle("Total Kinetic Energy Sigma (MeV)");
    tkelog_widths->SetTitle("");
    gPad->SetLogx(1);
    //tkelog_widths->Draw();
    //pol->Draw("same");
    TLegend *legend2 = new TLegend(0.7,0.8,0.99,0.99);
    //legend2->AddEntry(pol,"Madland 2006");
    //legend2->AddEntry(tkelog_means,"WNR Data");
    //legend2->Draw("same");



}
