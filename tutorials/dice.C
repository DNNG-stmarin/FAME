//probablity problem
{
    h1 = new TH1D("h1", "one die;x;counts", 6, 0.5, 6.5);
    h2 = new TH1D("h2", "two dice;x;counts", 12, 0.5, 12.5);
    
    for(int i = 0; i<10000; ++i){
        int r1 = gRandom->Integer(6) + 1;
        int r2 = gRandom->Integer(6) + 1;
        h1->Fill(r1);
        h2->Fill(r1+r2);
    }

    c1 = new TCanvas("c1", "Bin Size", 1000, 500);
    c1->Divide(2, 1);

    c1->cd(1);
    h1->SetMinimum(0);
    h1->SetFillColor(kBlue);
    h1->Draw();

    c1->cd(2);
    h2->SetMinimum(0);
    h2->SetFillColor(kBlue +2);
    h2->Draw();

    cout << "Counts for sum of two dice" << endl;
    for(int i = 1; i<=h2->GetNbinsX(); ++i){
        cout << i << "\t" << h2->GetBinCenter(i) << "\t" << h2->GetBinContent(i)<< endl;
    }

}