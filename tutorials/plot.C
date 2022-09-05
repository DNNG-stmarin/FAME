//Example of Reading a Data File

{
    g = new TGraph("data.txt");
    g->SetMarkerStyle(21);
    g->SetLineColor(kRed);
    g->SetTitle("Data Plot with TGraph");
    // draws the points 
    g->Draw("AP");
    // L draws a line connecting them (in order)
    // C draws a curve connecting them (in order)



}