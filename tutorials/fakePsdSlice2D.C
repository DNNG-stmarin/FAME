
{
using namespace ROOT::Math;
    gROOT->SetBatch(kTRUE);

//header 


/*
                _            _      
 __ ___ _ _  __| |_ __ _ _ _| |_ ___
/ _/ _ \ ' \(_-<  _/ _` | ' \  _(_-<
\__\___/_||_/__/\__\__,_|_||_\__/__/ */

    // constant values
    // plots and seperation
    int const numBins = 100;
    int const numSlices = 100;
    int const xMin = 0;
    int const xMax = 10;
    int const yMin = 0;
    int const yMax = 10;
    int numBinsX = 100;
    int numBinsY = 100;
    int sliceWidth = 3;
    int const numParticles = 150000;
    double photonEnergyMean = 2;
    double neutronEnergyMean = 2;
  
    // discrimination line constants
    double const maxMisclass = 0.005;
    double shiftValue = 0.04;
    bool doubleDiscrimLine = false;

    // mean and sigma predetermined moving constants 
    //double const photonMean = 3;
    TF1* photonMean = new TF1("photonMean", "[0]*exp(-[1]*x) + [2]*x + [3]");
    //double const photonSigma = .2;
    TF1* photonSigma = new TF1("photonSigma", "[0]*exp(-[1]*x) + [2]*x + [3]");
    //double const neutronMean = 4.5;
    TF1* neutronMean = new TF1("neutronMean", "[0]*exp(-[1]*x) + [2]*x + [3]");
    //double const neutronSigma = .3;
    TF1* neutronSigma = new TF1("neutronSigma", "[0]*exp(-[1]*x) + [2]*x + [3]");
    

    // parameters for the 2D histogram (amplitude, bend, angle, mean)
    photonMean->SetParameters(0, 0, 0, 2);
    photonSigma->SetParameters(0, 0, -0.02, 0.5);
    neutronMean->SetParameters(1.5, 1, 0, 4.5);
    neutronSigma->SetParameters(0, 0, -0.04, 0.5);

    // value used for finding the gaussian fits
    double predictedCenter = (photonMean->Eval(0) + neutronMean->Eval(0)) / 2;

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
    //2Ddiscrimination fit
    TF1 * discrimFit = new TF1("fitDiscrim", "[0]*exp(-[1]*x) + [2]*x + [3]");

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

    TH2D* psdH2 = new TH2D("psdH2", "PSD vs Energy Plot;Energy;PSD;Counts", numBinsX, xMin, xMax, numBinsY, yMin, yMax); 
        
/* 
  __ _ _ _   _    _    _                               
 / _(_) | | | |_ (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
|  _| | | | | ' \| (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
|_| |_|_|_| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
                               |___/                    */
   //initalizing loop vars
   double psdNeutron, psdPhoton, eNeutron, ePhoton;

    // starts for loop to fill the 2D histogram 
    for(int i = 0; i< numParticles; ++i)
    { 
        // gets a random energy value for the particle
        ePhoton = gRandom->Exp(photonEnergyMean);
        eNeutron = gRandom->Exp(neutronEnergyMean);
        
        //evaluate the mean and sigma functions at the energys 
        double photonMeanValue = photonMean->Eval(ePhoton);
        double neutronMeanValue = neutronMean->Eval(eNeutron);
        double photonSigmaValue = photonSigma->Eval(ePhoton);
        double neutronSigmaValue = neutronSigma->Eval(eNeutron); 

        //find the photon particles
        psdPhoton = gRandom->Gaus(photonMeanValue, photonSigmaValue); 
        psdH2->Fill(ePhoton, psdPhoton);
        if(i< numParticles){
            //find the neutron particles
            psdNeutron = gRandom->Gaus(neutronMeanValue, neutronSigmaValue); 
            psdH2->Fill(eNeutron, psdNeutron);
        }
    }
        

/*
             _        _               
  _ __  __ _(_)_ _   | |___  ___ _ __ 
 | '  \/ _` | | ' \  | / _ \/ _ \ '_ \
 |_|_|_\__,_|_|_||_| |_\___/\___/ .__/
                                |_|   */

// loop variables 
double discrimLineErg [numBins] = {0};
double discrimLinePt [numBins] = {0};
int discrimIndex = 0;
//
double neutronMuExp [numBins] = {0};
double neutronSigmaExp [numBins] = {0};
double neutronDiscrim [numBins] = {0};
double photonMuExp [numBins] = {0};
double photonSigmaExp [numBins] = {0};
double photonDiscrim [numBins] = {0};


// start for loop, goes through slices until numBinsY, slices are the width of sliceWidth
for(int sliceNum = 0; sliceNum < numBinsY; sliceNum += sliceWidth){

    /* 
                    _               _    _    _                               
     __ _ _ _  __ _| |_  _ ______  | |_ (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
    / _` | ' \/ _` | | || |_ / -_) | ' \| (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
    \__,_|_||_\__,_|_|\_, /__\___| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
                    |__/                            |___/                    */
    
        // gets the histogram for the slice that is being looked at 
        TH1D* psdH = psdH2->ProjectionY( "psdH", sliceNum, sliceNum + sliceWidth);
        // finds the approximate center value, so gaussian curves can be fit
        int predictedCenterBin = psdH->GetBin(predictedCenter);
        // histogram clone for photons
        TH1D* photonClone; 
        photonClone = (TH1D*)psdH->Clone("h3Clone");
        // histogram clone for neutrons
        TH1D* neutronClone; 
        neutronClone = (TH1D*)psdH->Clone("h3Clone");
        

    /* 
        _        _               
    _ __| |_  ___| |_ ___ _ _  ___
    | '_ \ ' \/ _ \  _/ _ \ ' \(_-<
    | .__/_||_\___/\__\___/_||_/__/
    |_|                             */
        
        // changes the definition of the photon clone to only be from the predicted center point
        photonClone->GetXaxis()->SetRange(0, predictedCenterBin * 10);
        // finds the photon max by findng the maximum from the clone
        double photonMax = photonClone->GetMaximum();
        // finds the bin of the max from the clone
        double centerPhoton = photonClone->GetMaximumBin() / 10.0;
        // sets an approximate guess for the width of photon part of the histogram
        double widthPhoton = photonClone->GetStdDev();
        // sets an approximate point as the spot between the neutron and photon curves
        int approxCenter = centerPhoton * 10 + widthPhoton * 10;
        
        
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
        
        //cout << sliceNum << " " << approxCenter << " " << centerNeutron << " " << widthNeutron << endl;

    /*
      __ _ _   _   _             _   _          _    _    _                            
     / _(_) |_| |_(_)_ _  __ _  | |_| |_  ___  | |_ (_)__| |_ ___  __ _ _ _ __ _ _ __  
    |  _| |  _|  _| | ' \/ _` | |  _| ' \/ -_) | ' \| (_-<  _/ _ \/ _` | '_/ _` | '  \ 
    |_| |_|\__|\__|_|_||_\__, |  \__|_||_\___| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_|
                        |___/                                    |___/                 */
        // uses the photon and neutron data gathered to find gaussian fits that approximate the histogram

        // finds the fit for the double gaussian based on the photon and neutron data 
        TFitResultPtr fitResults = 0;
        
        // set fit parameter limits
        doubleGauss->SetParLimits(1, xMin, approxCenter);
        doubleGauss->SetParLimits(4, approxCenter, xMax);

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


        //assign fit parameters to the defined curves
        photonGauss->SetParameters(fitPhotonMax, fitCenterPhoton, fitWidthPhoton);
        neutronGauss->SetParameters(fitNeutronMax, fitCenterNeutron, fitWidthNeutron);
        doubleGauss->SetParameters(fitPhotonMax, fitCenterPhoton, fitWidthPhoton, fitNeutronMax, fitCenterNeutron, fitWidthNeutron);
        differenceGauss->SetParameters(fitPhotonMax, fitCenterPhoton, fitWidthPhoton, fitNeutronMax, fitCenterNeutron, fitWidthNeutron);

        //cout << "   " << fitCenterNeutron << " " << fitWidthNeutron << endl;
    /*
        _ _            _         _ _          
     __| (_)___ __ _ _(_)_ __   | (_)_ _  ___ 
    / _` | (_-</ _| '_| | '  \  | | | ' \/ -_)
    \__,_|_/__/\__|_| |_|_|_|_| |_|_|_||_\___| */

        // solve for the discrimination line using the differenceGauss fit 
        double discrimLine = differenceGauss->GetX(0, fitCenterPhoton, fitCenterNeutron);
    
        // finds the missclassification values for photons and neutrons
        //photon 
        double photonMisclass = 1 - normal_cdf(discrimLine, fitWidthPhoton, fitCenterPhoton);
        //neutron
        double neutronMisclass = normal_cdf(discrimLine, fitWidthNeutron, fitCenterNeutron);


        //saves the discrimination values so that they can be correctly plotted onto the 2Dhist plot 
        // defines sliceEnergy so that the values can be saved in the correct spot
        double sliceEnergy = psdH2->GetXaxis()->GetBinCenter(sliceNum + int(sliceWidth/2));
        //saves the energy and discrimination line values to the correct location
        discrimLineErg[discrimIndex] = sliceEnergy;
        discrimLinePt[discrimIndex] = discrimLine;
    
        neutronMuExp[discrimIndex] = fitCenterNeutron;
        neutronSigmaExp[discrimIndex] = fitWidthNeutron;
        photonMuExp[discrimIndex] = fitCenterPhoton;
        photonSigmaExp[discrimIndex] = fitWidthPhoton;

        discrimIndex += 1;



    
    /* 
        _                      _ _           
     __| |_ _ __ ___ __ __  __| (_)__ ___ ___
    / _` | '_/ _` \ V  V / (_-< | / _/ -_|_-<
    \__,_|_| \__,_|\_/\_/  /__/_|_\__\___/__/ */
                                          
        // Create Canvas for plotting
        // Create Canvas for plotting
        TString nameCan = "c_" + to_string(sliceNum);
        TCanvas* c1 = new TCanvas(nameCan, "Example", 1000, 500); 
        c1->cd();

        // plot the histogram of the slice
        psdH->SetMinimum(0);
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
        TLine* discLine = new TLine (discrimLine, 0, discrimLine, fitPhotonMax);
        discLine->Draw("SAME");

        c1->Write();


        /*
        // plots the double discrimination lines if they exist 
        //// need to add an if statement here
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
        legend->AddEntry(discrmLine, "Discrimination Line", "l");
        //legend->AddEntry(discLineP, "Photon Discrimination Line", "l");
        //legend->AddEntry(discLineN, "Neutron Discrimination Line", "l");
        legend->Draw("SAME");
        */

// end of the for loop 
}

/*
                     _          _     _        _      
  __ _ _ _ __ _ _ __| |_    ___| |__ (_)___ __| |_ ___
 / _` | '_/ _` | '_ \ ' \  / _ \ '_ \| / -_) _|  _(_-<
 \__, |_| \__,_| .__/_||_| \___/_.__// \___\__|\__/__/ 
 |___/         |_|                 |__/               */

    // canvas 
    TCanvas* c_ergDisc = new TCanvas("ergDisc", "Energy Discrimination", 1000, 500); 
    c_ergDisc->cd();

    cout << "Now creating graph objects" << endl;
    //creates the discrimination line for the 2D histogram 
    TGraph* g_ergDiscrimination = new TGraph(discrimIndex, discrimLineErg, discrimLinePt);
    g_ergDiscrimination->SetName("g_ergDiscrimination");
    g_ergDiscrimination->SetLineColor(kRed);
    g_ergDiscrimination->SetLineWidth(5);
    
    //
    TGraph* g_neutronMu = new TGraph(discrimIndex, discrimLineErg, neutronMuExp);
    g_neutronMu->SetName("g_neutronMu");
    g_neutronMu->SetLineColor(kBlue);
    g_neutronMu->SetLineWidth(5);

    //
    TGraph* g_neutronSigma = new TGraph(discrimIndex, discrimLineErg, neutronSigmaExp);
    g_neutronSigma->SetName("g_neutronSigma");
    //g_neutronSigma->SetLineColor(kRed);

    //
    TGraph* g_photonMu = new TGraph(discrimIndex, discrimLineErg, photonMuExp);
    g_photonMu->SetName("g_photonMu");
    g_photonMu->SetLineColor(kGreen);
    g_photonMu->SetLineWidth(5);

    //
    TGraph* g_photonSigma = new TGraph(discrimIndex, discrimLineErg, photonSigmaExp);
    g_photonSigma->SetName("g_photonSigma");
    //g_photonSigma->SetLineColor(kRed);

    cout << "done with graph object creation" << endl;
    
    // fit the discrimination line for the 2D histogram
    TFitResultPtr discrimFitResults = 0;
    discrimFitResults = g_ergDiscrimination->Fit("fitDiscrim", "S Q N B");
    
    // fit the photon mean
    TFitResultPtr pMuFitResults = 0;
    pMuFitResults = g_photonMu->Fit("fitDiscrim", "S Q N B");
    
    // fit the neutron mean 
    TFitResultPtr nMuFitResults = 0;
    nMuFitResults = g_neutronMu->Fit("fitDiscrim", "S Q N B");
    
    // fit the photon sigma 
    TFitResultPtr pSiFitResults = 0;
    pSiFitResults = g_photonSigma->Fit("fitDiscrim", "S Q N B");
    
    // fit the neutron sigma 
    TFitResultPtr nSiFitResults = 0;
    nSiFitResults = g_neutronSigma->Fit("fitDiscrim", "S Q N B");
    

    // cout 
    cout << pMuFitResults->Parameter(0) << " " << pMuFitResults->Parameter(1) << " " << pMuFitResults->Parameter(2) << " " << pMuFitResults->Parameter(3) << endl;
    cout << nMuFitResults->Parameter(0) << " " << nMuFitResults->Parameter(1) << " " << nMuFitResults->Parameter(2) << " " << nMuFitResults->Parameter(3) << endl;

 /* 
     _                   ___ ___  _  _ _    _   
  __| |_ _ __ ___ __ __ /_  )   \| || (_)__| |_ 
 / _` | '_/ _` \ V  V /  / /| |) | __ | (_-<  _|
 \__,_|_| \__,_|\_/\_/  /___|___/|_||_|_/__/\__| */
                                                
    // plots the 2D histogram with a discrimination line
    psdH2->Draw("COLZ");
    g_ergDiscrimination->Draw("SAME");
    g_photonMu->Draw("SAME");
    g_neutronMu->Draw("SAME");

 /*                  
  ___ __ ___ _____ 
 (_-</ _` \ V / -_)
 /__/\__,_|\_/\___| */
                    
    // writes the canvases to a file, and closes it 
    c_ergDisc->Write();
    outFile->Write();
    outFile->Close(); 

    cout << "finished" << endl;
    gROOT->SetBatch(kFALSE);
 
        
}

