//example of histograms

{
    // 1D histogram of doubles,    bins, xmin, xmax
    h1 = new TH1D("h1", "Big Bins; x; counts", 3, 0, 10);
    h2 = new TH1D("h2", "Normal Bins; x; counts", 100, 0, 10);
    h3 = new TH1D("h3", "Way too many bins; x; counts", 1e5, 0, 10);
    
    for(int i = 0; i<10000; ++i){
        double x = gRandom->Gaus(5, 2);
        h1->Fill(x);
        h2->Fill(x);
        h3->Fill(x);
    }

    c1 = new TCanvas("c1", "Bin Size", 1200, 400);
    c1->Divide(3, 1);

    c1->cd(1);
    h1->Draw();

    c1->cd(2);
    h2->Draw();

    c1->cd(3);
    h3->Draw();

    /*
    h->SetMinimum(0);
    h->SetFillColor(kBlue -7);
    h->SetStat(1);
    gStyle->SetOptStat(111111);
    h->Draw();
    */
}