// #include "readFiss.h"
// #include <TH2.h>
// #include <TF1.h>
// #include <TStyle.h>
// #include <TCanvas.h>
//
// void readFiss::Spectra()
// {
//    const int numDet = 40;
//
//    TH1D** h_nT;
//    TH1D** h_gT;
//
//    TH1D** h_nL;
//    TH1D** h_gL;
//
//    h_nT = new TH1D*[numDet];
//    h_gT = new TH1D*[numDet];
//
//
//    h_nL = new TH1D*[numDet];
//    h_gL = new TH1D*[numDet];
//
//    TString headNDet = "detN";
//    TString headGDet = "detG";
//
//    TString s_tof = "_ToF";
//    TString s_lo = "_LO";
//
//    TString nameDet;
//
//
//    int numTbins = 100;
//    int numLbins = 300;
//
//    double minTime = -10;
//    double maxTime = 50;
//    double minLO = 0;
//    double maxLO = 4;
//
//    for(int d = 0; d < numDet; d++)
//    {
//    	nameDet = headNDet + to_string(d) + s_tof;
//    	h_nT[d] = new TH1D(nameDet, "Neutron Time of Flight; time (ns); counts", numTbins, minTime, maxTime);
//
//    	nameDet = headGDet + to_string(d) + s_tof;
//    	h_gT[d] = new TH1D(nameDet, "Gamma Time of Flight; time (ns); counts", numTbins, minTime, maxTime);
//
//
//       nameDet = headNDet + to_string(d) + s_lo;
//       h_nL[d] = new TH1D(nameDet, "Neutron Light Output; light output (MeVee); counts", numLbins, minLO, maxLO);
//
//       nameDet = headGDet + to_string(d) + s_lo;
//       h_gL[d] = new TH1D(nameDet, "Gamma Light Output; light output (MeVee); counts", numLbins, minLO, maxLO);
//
//    }
//
//    TH1I* h_nD = new TH1I("nDets", "Neutron Detector Counts; det; counts", 40, 0, 40);
//    TH1I* h_gD = new TH1I("gDets", "Gamma Detector Counts; det; counts", 40, 0, 40);
//
//
//
//    cout << "now looping through histories" << endl;
//
//    if (fChain == 0) return;
//
//    int detFill;
//    double timeFill;
//    double loFill;
//
//    Long64_t nentries = fChain->GetEntriesFast();
//    // nentries = 1000000;
//
//    Long64_t nbytes = 0, nb = 0;
//    for (Long64_t jentry=0; jentry<nentries;jentry++) {
//       Long64_t ientry = LoadTree(jentry);
//       if (ientry < 0) break;
//       nb = fChain->GetEntry(jentry);   nbytes += nb;
//
//        if(jentry%100000 == 0)
//       {
//         cout << jentry << " entries done. " << endl;
//       }
//
//       for(int nMult = 0; nMult < neutronMult; nMult ++)
//       {
//       	detFill = (int)neutronDet[nMult];
//       	timeFill = neutronDetTimes[nMult];
//          loFill = neutronLightOut[nMult];
//
//       	h_nD->Fill(detFill); // fill det counts
//       	h_nT[detFill]->Fill(timeFill); // fill time counts
//          h_nL[detFill]->Fill(loFill);
//       }
//
//       for(int gMult = 0; gMult < gammaMult; gMult ++)
//       {
//          detFill = (int)photonDet[gMult];
//          timeFill = photonDetTimes[gMult];
//          loFill = photonLightOut[gMult];
//
//          h_gD->Fill(detFill); // fill det counts
//          h_gT[detFill]->Fill(timeFill); // fill time counts
//          h_gL[detFill]->Fill(loFill);
//       }
//    }
//
//
//    // set batch mode
//    // gROOT->SetBatch(kTRUE);
//
//    // output file
//    TFile* fileOut = new TFile("cfdNeut.root", "RECREATE");
//
//
// /*
//   ___ _        _              ___          _
//  | _ \ |_  ___| |_ ___ _ _   | _ \___ __ _| |__
//  |  _/ ' \/ _ \  _/ _ \ ' \  |  _/ -_) _` | / /
//  |_| |_||_\___/\__\___/_||_| |_| \___\__,_|_\_\
//
// */
//
//    double posPhot[numDet] = {0};
//
//    TDirectory* cd_gT;
//    cd_gT = fileOut->mkdir("gCFD");
//    cd_gT->cd();
//
//    TF1* fitGaus = new TF1("gausGamma", "[0]*exp(-0.5*((x-[1])/[2])^2)",  2.5, 4);
//    fitGaus->SetLineColor(kRed);
//
//    TFitResultPtr fit_gT;
//
//    TLine* linePos;
//
//    double ampMax;
//    double muMax;
//    double sigMax;
//
//    double muGuess, stdGuess;
//
//    TCanvas** c_gT;
//    c_gT = new TCanvas*[numDet];
//
//    TString s_gT = "gammaTime_d";
//    TString namegT;
//
//    for(int d = 0; d<numDet; d++)
//    {
//
//       if(h_gT[d]->GetEntries() == 0)
//       {
//          cout << d << " empty." << endl;
//          continue;
//       }
//
//
//       namegT = s_gT + to_string(d);
//       c_gT[d] = new TCanvas(namegT, "Gamma ToF");
//       c_gT[d]->cd();
//
//       muGuess = h_gT[d]->GetMean();
//       stdGuess = h_gT[d]->GetStdDev();
//
//       fitGaus->SetParameter(1, muGuess);
//       fitGaus->SetParameter(2, stdGuess);
//
//       fit_gT = h_gT[d]->Fit("gausGamma", "SQ");
//
//       ampMax = fit_gT->Parameter(0);
//       muMax = fit_gT->Parameter(1);
//       sigMax = fit_gT->Parameter(2);
//
//       h_gT[d]->Draw();
//
//       // fitGaus->SetParameters(ampMax, muMax, sigMax);
//       fitGaus->Draw("SAME");
//
//       linePos = new TLine(muMax, 0, muMax, 2*ampMax);
//       linePos->SetLineColor(kGreen);
//       linePos->Draw("SAME");
//
//
//       cout << d << ": " << muMax << " " << sigMax << endl;
//
//       c_gT[d]->Write();
//
//       posPhot[d] = muMax;
//
//    }
//
//
// /*
//   _  _          _                   ___ ___ ___
//  | \| |___ _  _| |_ _ _ ___ _ _    / __| __|   \
//  | .` / -_) || |  _| '_/ _ \ ' \  | (__| _|| |) |
//  |_|\_\___|\_,_|\__|_| \___/_||_|  \___|_| |___/
//
// */
//    // directory
//    TDirectory* cd_nCFD;
//    cd_nCFD = fileOut->mkdir("nCFD");
//    cd_nCFD->cd();
//
//    // canvas
//    TCanvas** c_nT;
//    c_nT = new TCanvas*[numDet];
//
//    TString s_nT = "neutronTime_d";
//    TString namenT;
//
//    // declare the analysis variables
//    int maxCount;
//    int maxBin;
//
//    int iterCount;
//    int iterBin;
//
//    double halfCount;
//    // int halfBin;
//
//    double interpTime;
//    double cfdTime;
//
//    double cfdDet[numDet];
//    double cfdDist[numDet];
//
//    TLine* vertCFD;
//    TLine* horCFD;
//
//    for(int d = 0; d<numDet; d++)
//    {
//
//       if(h_nT[d]->GetEntries() == 0)
//       {
//          cout << d << " empty." << endl;
//          continue;
//       }
//
//       namenT = s_nT + to_string(d);
//       c_nT[d] = new TCanvas(namenT, "Neutron ToF");
//       c_nT[d]->cd();
//
//       // smooth the histogram
//       // h_nT[d]->Smooth();
//
//       // find the max counts and the target counts
//       maxCount = h_nT[d]->GetMaximum();
//       maxBin = h_nT[d]->GetMaximumBin();
//       halfCount = maxCount*0.5;
//
//       // iterate until half max found
//       iterCount = maxCount;
//       iterBin = maxBin;
//       while(iterCount > halfCount)
//       {
//       	iterBin--;
//       	iterCount = h_nT[d]->GetBinContent(iterBin);
//       }
//
//       // cout << "found: " << iterCount << endl;
//
//       interpTime = (halfCount - h_nT[d]->GetBinContent(iterBin))/(h_nT[d]->GetBinContent(iterBin+1) - h_nT[d]->GetBinContent(iterBin));
//
//       // convert to bin centers
//       cfdTime = h_nT[d]->GetBinCenter(iterBin) + interpTime;
//
//       // store time
//       cfdDet[d] = d;
//       cfdDist[d] = cfdTime - posPhot[d];
//
//       vertCFD = new TLine(cfdTime, 0, cfdTime, maxCount*2);
//       horCFD = new TLine(0, halfCount, 100, halfCount);
//       vertCFD->SetLineColor(kGreen);
//       horCFD->SetLineColor(kOrange);
//
//       // output the result
//       cout << d << ": " << cfdDist[d] << endl;
//
//       h_nT[d]->Draw("SAME");
//       vertCFD->Draw("SAME");
//       horCFD->Draw("SAME");
//
//       // h_nT[d]->Write();
//       c_nT[d]->Write();
//
//    }
//
//    /*
//
//   _    _      _   _      ___       _             _
//  | |  (_)__ _| |_| |_   / _ \ _  _| |_ _ __ _  _| |_
//  | |__| / _` | ' \  _| | (_) | || |  _| '_ \ || |  _|
//  |____|_\__, |_||_\__|  \___/ \_,_|\__| .__/\_,_|\__|
//         |___/                         |_|
//
//    */
//
//    // gamma
//    // directory
//    TDirectory* cd_gL;
//    cd_gL = fileOut->mkdir("gLO");
//    cd_gL->cd();
//
//    for(int d = 0; d<numDet; d++)
//    {
//       h_gL[d]->Write();
//    }
//
//    // neutron
//    // directory
//    TDirectory* cd_nL;
//    cd_nL = fileOut->mkdir("nLO");
//    cd_nL->cd();
//
//    for(int d = 0; d<numDet; d++)
//    {
//       h_nL[d]->Write();
//    }
//
//
// /*
//    ___                        ___             _ _
//   / __|__ _ _ ___ ____ _ ___ | _ \___ ____  _| | |_ ___
//  | (__/ _` | ' \ V / _` (_-< |   / -_|_-< || | |  _(_-<
//   \___\__,_|_||_\_/\__,_/__/ |_|_\___/__/\_,_|_|\__/__/
// */
//
//    fileOut->cd();
//
//    // draw the neutron informations
//    TCanvas* c1 = new TCanvas("collectRes");
//    c1->Divide(1,2);
//
//    // draw line
//    c1->cd(1);
//    TGraph* grN = new TGraph(numDet, cfdDet, cfdDist);
//    grN->SetLineColor(kRed);
//    grN->SetName("graphN");
//    grN->Draw();
//
//    // normalize detector histogram
//    c1->cd(2);
//    h_nD->Draw();
//
//    c1->Write();
//
//
//    fileOut->Write();
//    // gROOT->SetBatch(kFALSE);
//    fileOut->Close();
//
//
//    /*
//
//   ___     _     _      ___ _____   __
//  | _ \_ _(_)_ _| |_   / __/ __\ \ / /
//  |  _/ '_| | ' \  _| | (__\__ \\ V /
//  |_| |_| |_|_||_\__|  \___|___/ \_/
//
//
//    */
//
//    ofstream o_neutToF;
//    ofstream o_gamToF;
//    ofstream o_neutLO;
//    ofstream o_gamLO;
//
//    o_neutToF.open("nToF.csv");
//    o_gamToF.open("gToF.csv");
//    o_neutLO.open("nLO.csv");
//    o_gamLO.open("gLO.csv");
//
//    // center and counts for each bin
//    double cent;
//    int counts;
//
//    // time of flight
//    for(int tBin = 0; tBin < numTbins; tBin++)
//    {
//       cent = h_nT[0]->GetBinCenter(tBin);
//
//       o_neutToF << cent << ", ";
//       o_gamToF << cent << ", ";
//
//       for(int d = 0; d < numDet; d++)
//       {
//          o_neutToF << h_nT[d]->GetBinContent(tBin);
//          o_gamToF << h_gT[d]->GetBinContent(tBin);
//
//          if(d < numDet-1)
//          {
//             o_neutToF << ", ";
//             o_gamToF << ", ";
//          }
//
//          else if(d == numDet -1)
//          {
//             o_neutToF << "\n";
//             o_gamToF << "\n";
//          }
//
//       }
//    }
//
//    // light output
//    for(int lBin = 0; lBin < numLbins; lBin++)
//    {
//       cent = h_nL[0]->GetBinCenter(lBin);
//
//       o_neutLO << cent << ", ";
//       o_gamLO << cent << ", ";
//
//       for(int d = 0; d < numDet; d++)
//       {
//          o_neutLO << h_nL[d]->GetBinContent(lBin);
//          o_gamLO << h_gL[d]->GetBinContent(lBin);
//
//          if(d < numDet-1)
//          {
//             o_neutLO << ", ";
//             o_gamLO << ", ";
//          }
//
//          else if(d == numDet -1)
//          {
//             o_neutLO << "\n";
//             o_gamLO << "\n";
//          }
//
//       }
//    }
//
//
//    o_neutToF.close();
//    o_gamToF.close();
//    o_neutLO.close();
//    o_gamLO.close();
//
// }
