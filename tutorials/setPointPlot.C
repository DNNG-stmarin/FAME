//Example of graph with SetPoint

{
    g = new TGraph();
    for(int i=0; i<10; i++) {
        g->SetPoint(i, i, i*i - 4*i + 7);
    }
    g->SetMarkerStyle(22);
    g->Draw("ALP");

}