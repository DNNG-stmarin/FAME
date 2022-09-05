//Examples of ROOT TFormula class

{
    //sets up a new canvas size, and overall title
    c1 = new TCanvas("c1", "My Root Plots", 1000, 500);
    // divdes the canvans into the wanted number of subplots (row, col)
    c1->Divide(3,1);

    // this plot will be in the first subplot
    c1->cd(1);
    // define the new plot 
    // TF1 - 2 dimensional, name, function, xmin, x max
    f1 = new TF1("f1", "sin(x)", 0, 5);
    //color for the line
    f1->SetLineColor(kBlue+2);
    //title; xlabel; ylabel
    f1->SetTitle("Sin(x); x axis; yaxis");
    //prints the actual plot
    f1->Draw();

    c1->cd(2);
    f2 = new TF1("f2", "cos(x)", 0, 5);
    f2->SetLineColor(kRed);
    f2->SetTitle("Cos(x); x axis; y axis");
    f2->Draw();

    // this plot is in the third subplot
    c1->cd(3);
    // TF2 - 3 dimensional plot, name, function, xmin, xmax, ymin, ymax
    f3 = new TF2("f3", "10*cos(x)*sin(y)", -3, 3, 0, 10);
    f2->SetTitle("10*cos(x)*sin(y); x axis; y axis");
    // draws the plot as a surface 
    f3->Draw("surf2");

    // f2->Draw("SAME") plots f2 ontop of the other plot (no subplots)
}