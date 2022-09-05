//Randomly generated PSD Slice (Double Gaussian Fit)
{
// ╦ ╦┬┌─┐┌┬┐┌─┐┌─┐┬─┐┌─┐┌┬┐
// ╠═╣│└─┐ │ │ ││ ┬├┬┘├─┤│││
// ╩ ╩┴└─┘ ┴ └─┘└─┘┴└─┴ ┴┴ ┴         

    // creates bins for the neutron and photon histagram 
    photonH = new TH1D("photonH", "photons;;energy;counts", 100, 0, 10);
    neutronH = new TH1D("neutronH", "neutrons;energy;counts", 100, 0, 10);
    // creates bins for the combined PSD slice histogram
    psdH = new TH1D("psdH", "combined;energy;counts", 100, 0, 10);
    
    // puts random gaussian particles into the three histograms
    for(int i = 0; i<10500; ++i){
        //photon
        double r1 = gRandom->Gaus(3, .2);
        if(i< 10500){
            //neutron
            double r2 = gRandom->Gaus(4.5, 1);
            neutronH->Fill(r2);
            psdH->Fill(r2);
        }
        photonH->Fill(r1);
        psdH->Fill(r1);
    }
    //creates a new canvas to plot things onto
    c1 = new TCanvas("c1", "Example", 1000, 500);

    /* plots the two sepearate histograms

    h1->SetMinimum(0);
    h1->SetFillColor(kBlue + 2);
    h1->Draw();

    h2->SetMinimum(0);
    h2->SetFillColor(kRed +2);
    h2->Draw("SAME");
    */

   // plots the combined psd slice histogram 
    psdH->SetMinimum(0);
    // sets the fill color ot orange
    psdH->SetFillColor(kOrange + 1);
    psdH->Draw("SAME");

    // Intializes the fit resutls 
    TFitResultPtr fitResults = 0;

// ╔═╗┬┌┬┐  ╔═╗┌─┐┬ ┬┌─┐┬┌─┐┌┐┌  ╔═╗┬ ┬┬─┐┬  ┬┌─┐┌─┐
// ╠╣ │ │   ║ ╦├─┤│ │└─┐│├─┤│││  ║  │ │├┬┘└┐┌┘├┤ └─┐
// ╚  ┴ ┴   ╚═╝┴ ┴└─┘└─┘┴┴ ┴┘└┘  ╚═╝└─┘┴└─ └┘ └─┘└─┘   

    // defines a fit equation for the double gaussian
    doubleGaussF = new TF1 ("f1", "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[4])/[5])^2)", 0, 10);
    
    // sets starter parameters for the fit
    doubleGaussF->SetParameters(1000, 3, 0.5, 400, 4.5, 1.5);
    // stores the results of the fit 
    fitResults = psdH->Fit(doubleGaussF, "S Q");
    // prints out the result of parameter 2
    //cout << fitResults->Parameter(2) << endl;

    // clones the psdSlice histogram so that parameters can be found from the plot
    TH1D* psdClone; 
    psdClone = (TH1D*)psdH->Clone("h3Clone");

// ╔═╗┬ ┬┌─┐┌┬┐┌─┐┌┐┌┌─┐
// ╠═╝├─┤│ │ │ │ ││││└─┐
// ╩  ┴ ┴└─┘ ┴ └─┘┘└┘└─┘
    // finds the photon max by findng the maximum from the clone
    double photonMax = psdClone->GetMaximum();
    // finds the bin of the max from the clone
    double centerPhoton = psdClone->GetMaximumBin() / 10.0;
    // sets an approximate guess for the width of photon part of the histogram
    double widthPhoton = 0.5;
    // sets an approximate point as the spot between the neutron and photon curves
    int approxCenter = centerPhoton * 10 + widthPhoton * 10;

// ╔╗╔┌─┐┬ ┬┌┬┐┬─┐┌─┐┌┐┌┌─┐
// ║║║├┤ │ │ │ ├┬┘│ ││││└─┐
// ╝╚╝└─┘└─┘ ┴ ┴└─└─┘┘└┘└─┘
    // changes the definition of the clone to only be from the approximate center point
    h3Clone->GetXaxis()->SetRange(approxCenter, 100);
    // find the neutron max by finding the maximum from the clone
    double neutronMax = psdClone->GetMaximum();
    // find the bin of the neutron maximum
    double centerNeutron = psdClone->GetMaximumBin() / 10.0;
    // sets an approximate guess for the width of the neutron part of the histogram
    double widthNeutron = 1.5;

// ╔╦╗┬┌─┐┌─┐┬─┐┬┌┬┐  ╦  ┬┌┐┌┌─┐
//  ║║│└─┐│  ├┬┘││││  ║  ││││├┤ 
// ═╩╝┴└─┘└─┘┴└─┴┴ ┴  ╩═╝┴┘└┘└─┘
    // finds the log part of the equation
    double logAs = ((photonMax * widthNeutron) / (neutronMax * widthPhoton));
    // finds the square root part of the equation
    double root = ((centerPhoton - centerNeutron)^2) - (2 * (widthPhoton ^2 - widthNeutron ^2) * (log(logAs)));
    // finds the numerator part of the equation
    double num = centerNeutron * (widthPhoton ^ 2) - (widthNeutron * (centerPhoton * widthNeutron + (widthPhoton * (sqrt(root)))));
    // finds the denominator part of the equation
    double denom = (widthPhoton ^ 2) - (widthNeutron ^ 2);
    // finds the location of the discrimination line
    double discrimLine = num / denom ;

    // plots the discrimination line ontop of the psdSlice histogram
    l1 = new TLine ("l1", discrimLine, 0, discrimLine, 10);
    l1->Draw("SAME");
 
}

