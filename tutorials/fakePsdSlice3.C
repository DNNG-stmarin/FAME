
{
using namespace ROOT::Math;


//header 


/*
                _            _      
 __ ___ _ _  __| |_ __ _ _ _| |_ ___
/ _/ _ \ ' \(_-<  _/ _` | ' \  _(_-<
\__\___/_||_/__/\__\__,_|_||_\__/__/ */
    int const numBins = 100;
    int const xMin = 0;
    int const xMax = 10;
    int const numParticles = 50;
    double const photonMean = 3;
    double const photonSigma = .2;
    double const neutronMean = 4.5;
    double const neutronSigma = .3;
    double const maxMisclass = 0.005;
    double predictedCenter = (photonMean + neutronMean)/2 * 10;
    double shiftValue = 0.04;
    bool doubleDiscrimLine = true;

/*
          _             _      __ _ _        
 ___ _  _| |_ _ __ _  _| |_   / _(_) |___ ___
/ _ \ || |  _| '_ \ || |  _| |  _| | / -_|_-<
\___/\_,_|\__| .__/\_,_|\__| |_| |_|_\___/__/
             |_|                              */
    //- Create a TFile to store the output
    TFile* outFile = new TFile("misclassFile.root", "recreate");

/*  
  __ _ _                        _   _             
 / _(_) |_   ___ __ _ _  _ __ _| |_(_)___ _ _  ___
|  _| |  _| / -_) _` | || / _` |  _| / _ \ ' \(_-<
|_| |_|\__| \___\__, |\_,_\__,_|\__|_\___/_||_/__/
                   |_|                             */
    //	- Define a TF1 for photon PSD (single gaussian)
    TF1* photonGauss = new TF1("fphoton", "[0]*exp(-0.5*((x-[1])/[2])^2)", xMin, xMax);
    //	- Define a TF1 for neutron PSD (single gaussian)
    TF1* neutronGauss = new TF1("fneutron", "[0]*exp(-0.5*((x-[1])/[2])^2)", xMin, xMax);
    //	- Define a TF1 for combined (in terms of the other 2)
    TF1* doubleGauss = new TF1("fdouble", "fphoton + fneutron", xMin, xMax);
    //	- Define a TF1 for the difference between neutrons and photons
    TF1* differenceGauss = new TF1("fdifference", "fphoton - fneutron", xMin, xMax);

    cout << "done definding fits" << endl;

    // enforcing limits on perameters 
    doubleGauss->SetParLimits(0, 0, numParticles);
    doubleGauss->SetParLimits(1, xMin, xMax);
    doubleGauss->SetParLimits(2, xMin, xMax);
    doubleGauss->SetParLimits(3, 0, numParticles);
    doubleGauss->SetParLimits(4, xMin, xMax);
    doubleGauss->SetParLimits(5, xMin, xMax);



/* 
    _      __ _            _    _    _                               
 __| |___ / _(_)_ _  ___  | |_ (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
/ _` / -_)  _| | ' \/ -_) | ' \| (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
\__,_\___|_| |_|_||_\___| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
                                             |___/                    */
    TH1D* photonH = new TH1D("photonH", "photons;energy;counts", numBins, xMin, xMax); 
    TH1D* neutronH = new TH1D("neutronH", "neutrons;energy;counts", numBins, xMin, xMax); 
// chnage this to a plus and a minus one
    TH1D* psdH = new TH1D("psdH", "Psd Slice Plot Labeled;Energy;Counts", numBins, xMin, xMax); 
        
/* 
  __ _ _ _   _    _    _                               
 / _(_) | | | |_ (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
|  _| | | | | ' \| (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
|_| |_|_|_| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
                               |___/                    */
    // puts random gaussian particles into the three histograms
    for(int i = 0; i< numParticles; ++i)
    { 
        //photon
        double r1 = gRandom->Gaus(photonMean, photonSigma); // MN
        if(i< numParticles){
            //neutron
            double r2 = gRandom->Gaus(neutronMean, neutronSigma); // MN
            neutronH->Fill(r2);
            psdH->Fill(r2);
        }
        photonH->Fill(r1);
        psdH->Fill(r1);
    }
/* 
                _               _    _    _                               
 __ _ _ _  __ _| |_  _ ______  | |_ (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
/ _` | ' \/ _` | | || |_ / -_) | ' \| (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
\__,_|_||_\__,_|_|\_, /__\___| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
                  |__/                            |___/                    */
    //	- Clone the histograms and get guesses for the fit parameters (mean, amplitude, standardDeviations)
    // particle clone for photons
    TH1D* photonClone; 
    photonClone = (TH1D*)psdH->Clone("h3Clone");
    // particle clone for neutrons
    TH1D* neutronClone; 
    neutronClone = (TH1D*)psdH->Clone("h3Clone");
    

/* 
      _        _               
 _ __| |_  ___| |_ ___ _ _  ___
| '_ \ ' \/ _ \  _/ _ \ ' \(_-<
| .__/_||_\___/\__\___/_||_/__/
|_|                             */
    // changes the definition of the photon clone to only be from the predicted center point
    photonClone->GetXaxis()->SetRange(0, predictedCenter);
    // finds the photon max by findng the maximum from the clone
    double photonMax = photonClone->GetMaximum();
    // finds the bin of the max from the clone
    double centerPhoton = photonClone->GetMaximumBin() / 10.0;
    // sets an approximate guess for the width of photon part of the histogram
    double widthPhoton = photonClone->GetStdDev();
    // sets an approximate point as the spot between the neutron and photon curves
    int approxCenter = centerPhoton * 10 + widthPhoton * 10;
cout << "done solving for photons"   << endl;

/*
               _                   
 _ _  ___ _  _| |_ _ _ ___ _ _  ___
| ' \/ -_) || |  _| '_/ _ \ ' \(_-<
|_||_\___|\_,_|\__|_| \___/_||_/__/ */
    // changes the definition of the clone to only be from the approximate center point
    neutronClone->GetXaxis()->SetRange(approxCenter, 100);
    // find the neutron max by finding the maximum from the clone
    double neutronMax = neutronClone->GetMaximum();
    // find the bin of the neutron maximum
    double centerNeutron = neutronClone->GetMaximumBin() / 10.0;
    // sets an approximate guess for the width of the neutron part of the histogram
    double widthNeutron = neutronClone->GetStdDev();
cout << "done solving for neutrons" << endl;
/*
   __ _ _   _   _             _   _          _    _    _                            
 / _(_) |_| |_(_)_ _  __ _  | |_| |_  ___  | |_ (_)__| |_ ___  __ _ _ _ __ _ _ __  
|  _| |  _|  _| | ' \/ _` | |  _| ' \/ -_) | ' \| (_-<  _/ _ \/ _` | '_/ _` | '  \ 
|_| |_|\__|\__|_|_||_\__, |  \__|_||_\___| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_|
                     |___/                                    |___/                 */
    //	- define the pointer for the fit
    //	- use the parameters from (4) to inform the fit
    //	- extract the parameters from the combined fit and set the parameters for the individual fits

    //doubleGauss fitResults = 0;
    TFitResultPtr fitResults = 0;
    doubleGauss->SetParameters(photonMax, centerPhoton, widthPhoton, neutronMax, centerNeutron, widthNeutron);
    // stores the results of the fit 
    fitResults = psdH->Fit("fdouble", "S Q N B");
    
    // store fit results
    double fitPhotonMax, fitCenterPhoton, fitWidthPhoton, fitNeutronMax, fitCenterNeutron, fitWidthNeutron;
    fitPhotonMax = fitResults->Parameter(0);
    fitCenterPhoton = fitResults->Parameter(1);
    fitWidthPhoton = fitResults->Parameter(2);
    fitNeutronMax = fitResults->Parameter(3);
    fitCenterNeutron = fitResults->Parameter(4);
    fitWidthNeutron = fitResults->Parameter(5);

    //assign fit parameters to defined curves
    photonGauss->SetParameters(fitPhotonMax, fitCenterPhoton, fitWidthPhoton);
    neutronGauss->SetParameters(fitNeutronMax, fitCenterNeutron, fitWidthNeutron);
    doubleGauss->SetParameters(fitPhotonMax, fitCenterPhoton, fitWidthPhoton, fitNeutronMax, fitCenterNeutron, fitWidthNeutron);
    differenceGauss->SetParameters(fitPhotonMax, fitCenterPhoton, fitWidthPhoton, fitNeutronMax, fitCenterNeutron, fitWidthNeutron);

cout << "done with the fit" << endl;
/*
    _ _            _         _ _          
 __| (_)___ __ _ _(_)_ __   | (_)_ _  ___ 
/ _` | (_-</ _| '_| | '  \  | | | ' \/ -_)
\__,_|_/__/\__|_| |_|_|_|_| |_|_|_||_\___| */

    double discrimLine = differenceGauss->GetX(0, fitCenterPhoton, fitCenterNeutron);


 // misclassification 

    //photon 
    double photonMisclass = 1 - normal_cdf(discrimLine, fitWidthPhoton, fitCenterPhoton);

    //neutron
    double neutronMisclass = normal_cdf(discrimLine, fitWidthNeutron, fitCenterNeutron);

 // double discrim line
    double discrimLineP = discrimLine;
    double discrimLineN = discrimLine;
    if(doubleDiscrimLine){
        while(photonMisclass > maxMisclass){
            discrimLineP += shiftValue;
            //calculate p missclass again
            photonMisclass = 1 - normal_cdf(discrimLineP, fitWidthPhoton, fitCenterPhoton);
            if(discrimLineP >= xMax){
                cout << "desired discrimination not found" << endl;
                break;
            }
            }
        while(neutronMisclass > maxMisclass){
            discrimLineN -= shiftValue;
            //calculate n missclass again
            neutronMisclass = normal_cdf(discrimLineN, fitWidthNeutron, fitCenterNeutron);
            if(discrimLineN <= xMin){
                cout << "desired discrimination not found" << endl;
                break;
            }
            }
    } 

 /* 
     _                 
  __| |_ _ __ ___ __ __
 / _` | '_/ _` \ V  V /
 \__,_|_| \__,_|\_/\_/  */

                       
    //	- Create a canvas
    //	- cd into the canvas
    //	- draw the combined histograms
    //	- draw all 4 fits: neutron, photon, sum, difference
    //	- Define and draw three discrimination lines: middle, 2 stdDev from neutron, and 2 stdDev from photon
    //	- Define a legend for the lines
    TCanvas* c1 = new TCanvas("c1", "Example", 1000, 500); 
    c1->cd();
    // plots the two sepearate histograms

    photonH->SetMinimum(0);
    photonH->SetFillColor(kRed);
    //photonH->Draw();

    neutronH->SetMinimum(0);
    neutronH->SetFillColor(kBlue);
    //neutronH->Draw("SAME");
    

    // plots the combined psd slice histogram 
    psdH->SetMinimum(0);
    //psdH->SetFillColor(kOrange);
    psdH->SetLineColor(kBlack);
    psdH->Draw();


    //plot the fit equations
    photonGauss->Draw("SAME");
    photonGauss->SetLineColor(kRed);
    neutronGauss->Draw("SAME");
    neutronGauss->SetLineColor(kBlue);
    doubleGauss->Draw("SAME");
    doubleGauss->SetLineColor(kOrange);
    differenceGauss->Draw("SAME");
    differenceGauss->SetLineColor(kGreen);


// plots the discrimination line ontop of the psdSlice histogram
 // plots the discrimination line ontop of the psdSlice histogram
TLine* discLine = new TLine (discrimLine, 0, discrimLine, fitPhotonMax);
discLine->Draw("SAME");

TLine* discLineP = new TLine (discrimLineP, 0, discrimLineP, fitPhotonMax);
discLineP->SetLineColor(kRed);
discLineP->SetLineStyle(kDashed);
discLineP->Draw("SAME");

TLine* discLineN = new TLine (discrimLineN, 0, discrimLineN, fitPhotonMax);
discLineN->SetLineColor(kBlue);
discLineN->SetLineStyle(kDashed);
discLineN->Draw("SAME");


// adds a legend to the plot
legend = new TLegend(0.1,0.7,0.48,0.9);
legend->AddEntry(psdH,"Histogram Slice of PSD", "l");
legend->AddEntry(photonGauss,"Photon Gauss Curve", "l");
legend->AddEntry(neutronGauss,"Neutron Gauss Curve", "l");
legend->AddEntry(doubleGauss,"Photon and Neutron Curves Combined", "l");
legend->AddEntry(differenceGauss,"Photon Curve - Neutron Curve", "l");
legend->AddEntry(discLine, "Discrimination Line", "l");
legend->AddEntry(discLineP, "Photon Discrimination Line", "l");
legend->AddEntry(discLineN, "Neutron Discrimination Line", "l");
legend->Draw("SAME");



/*                  
  ___ __ ___ _____ 
 (_-</ _` \ V / -_)
 /__/\__,_|\_/\___| */
                    

    //	- Write the canvas
    //	- Write the file
    //	- Close the file
    c1->Write();
    outFile->Write();
    outFile->Close();

}

