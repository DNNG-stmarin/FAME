//Example of graph with SetPoint

{
    //Define Data
    g = new TGraph;
    double x,y;
    for(int i =0; i<10; ++i){
        x = 0.5*i;
        y = 4*x*x + exp(-x/3) + gRandom->Gaus(0,1);
        g->SetPoint(i, x, y);

    }

    // Define Fit Function
    f = new TF1 ("f", "[0]*x*x + exp(-x*[2])", 0, 5);
    f-> SetParNames("quad", "decay");
    f->SetParameters(1, 1);

    g->Fit(f);

    g->SetMarkerStyle(22);
    g->Draw("AP");
}