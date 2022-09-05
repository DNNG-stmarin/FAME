#include "fragFiss.h"
#include "constants.h"

#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

void fragFiss::PostCalibPHD(int iterationPost)
{
  cout << "Recalibrating anode histograms with pulse height defect" << endl;

  TH1D* h1_PHDAn1 = new TH1D("h1_PHDAn1_"+ TString(to_string(iterationPost)),"h1_PHDAn1", N_BINS_APH, 0, MAX_KE);
  TH1D* h1_PHDAn2 = new TH1D("h1_PHDAn2_"+ TString(to_string(iterationPost)),"h1_PHDAn2", N_BINS_APH, 0, MAX_KE);


  Long64_t nentries = fragTree->GetEntries();

  double pThetaAv;

  for (Long64_t jentry = 0; jentry < nentries; jentry++)
  {
     Long64_t ientry = fragTree->LoadTree(jentry);
     if(ientry < 0) break; // reached the end of the file

     fragTree->GetEntry(jentry);

     if( !((pAn1 > MIN_ANODE1) && (pAn2 > MIN_ANODE2) && (pTheta1 > MIN_ANG1) && (pTheta2 > MIN_ANG2)) ) continue;
     if(iterationPost == infoSystem->NUM_RECURSIONS - 1)
     {
       if(pA1 < MIN_MASS_ANALYSIS || pA1 > MAX_MASS_ANALYSIS || pA2 < MIN_MASS_ANALYSIS || pA2 > MAX_MASS_ANALYSIS) continue;
       if(pTheta1 < MIN_ANG_ANALYSIS || pTheta1 > MAX_ANG_ANALYSIS || pTheta2 < MIN_ANG_ANALYSIS || pTheta2 > MAX_ANG_ANALYSIS) continue;
     }
     else
     {
       if(pA1 < MIN_MASS_RECURSION|| pA1 > MAX_MASS_RECURSION || pA2 < MIN_MASS_RECURSION || pA2 > MAX_MASS_RECURSION) continue;
       if(pTheta1 < MIN_ANG_RECURSION || pTheta1 > MAX_ANG_RECURSION || pTheta2 < MIN_ANG_RECURSION || pTheta2 > MAX_ANG_RECURSION) continue;
     }

     // grid inefficiency correction
     pAn1 = (pAn1 - GRID_INEFFICIENCY*(pAn1 + pGr1))/(1 - GRID_INEFFICIENCY);
     pAn2 = (pAn2 - GRID_INEFFICIENCY*(pAn2 + pGr2))/(1 - GRID_INEFFICIENCY);

     pTheta1 /= g_AngMass1->Eval(pA1);
     pTheta2 /= g_AngMass1->Eval(pA2);

     pThetaAv = 0.5*(pTheta1 + pTheta2);
     // pTheta1 = pThetaAv;
     // pTheta2 = pThetaAv;

     if(MASS_DEP_ATT)
     {
       pAn1 = pAn1 - (g_slopeMass1->Eval(pA1))*1.0/pTheta1;
       pAn2 = pAn2 - (g_slopeMass2->Eval(pA2))*1.0/pTheta2;
     }
     else if(MASS_DEP_ATT_SURF)
     {
       // cout << pAn2 << " ";
       pAn1 = pAn1 + (g_interpMass1->Eval(pA1) - g2_massAttSurf1[iterationPost]->Interpolate(pA1, 1.0/pTheta1));
       // pAn1 = pAn1 - (g_slopeMass1->Eval(pA1))*1.0/pTheta1;
       pAn2 = pAn2 + (g_interpMass2->Eval(pA2) - g2_massAttSurf2[iterationPost]->Interpolate(pA2, 1.0/pTheta2));

       // cout << pAn2 << endl;
     }
     else if(MIXED_DEP)
     {
       // cout << pAn2 << " ";
       double diffD1 = (g_slopeMass1->Eval(pA1))*1.0/pTheta1 - (f_massAtt1->Eval(1.0/pTheta1) - f_massAtt1->Eval(0.0));
       double diffD2 = (g_slopeMass2->Eval(pA2))*1.0/pTheta2 - (f_massAtt2->Eval(1.0/pTheta2) - f_massAtt2->Eval(0.0));

       if(iterationPost < infoSystem->NUM_RECURSIONS - 1)
       {
         pAn1 -= f_massAtt1->Eval(1.0/pTheta1) - f_massAtt1->Eval(0.0);
         pAn2 -= f_massAtt2->Eval(1.0/pTheta2) - f_massAtt2->Eval(0.0);
       }
       else // on the last iteration go hard
       {
         pAn1 = pAn1 + (g_interpMass1->Eval(pA1) - g2_massAttSurf1[iterationPost]->Interpolate(pA1, 1.0/pTheta1));
         // pAn1 = pAn1 - (g_slopeMass1->Eval(pA1))*1.0/pTheta1;
         pAn2 = pAn2 + (g_interpMass2->Eval(pA2) - g2_massAttSurf2[iterationPost]->Interpolate(pA2, 1.0/pTheta2));
       }
       // pAn1 -= RECURSION_FRACTION_1*diffD1;
       // pAn2 -= RECURSION_FRACTION_2*diffD2;
     }
     else if(RECURSIVE_DEP)
     {
       // cout << pAn2 << " ";
       // double diffD1 = (g_slopeMass1->Eval(pA1))*1.0/pTheta1 - (f_massAtt1->Eval(1.0/pTheta1) - f_massAtt1->Eval(0.0));
       // double diffD2 = (g_slopeMass2->Eval(pA2))*1.0/pTheta2 - (f_massAtt2->Eval(1.0/pTheta2) - f_massAtt2->Eval(0.0));

       double diffD1 = -(g_interpMass1->Eval(pA1) - g2_massAttSurf1[iterationPost]->Interpolate(pA1, 1.0/pTheta1)) - (f_massAtt1->Eval(1.0/pTheta1) - f_massAtt1->Eval(0.0));
       double diffD2 = -(g_interpMass2->Eval(pA2) - g2_massAttSurf2[iterationPost]->Interpolate(pA2, 1.0/pTheta2)) - (f_massAtt2->Eval(1.0/pTheta2) - f_massAtt2->Eval(0.0));


       pAn1 -= f_massAtt1->Eval(1.0/pTheta1) - f_massAtt1->Eval(0.0);
       pAn2 -= f_massAtt2->Eval(1.0/pTheta2) - f_massAtt2->Eval(0.0);
       if(iterationPost >= infoSystem->NUM_RECURSIONS - 1)
       {
         pAn1 -= RECURSION_FRACTION_1*diffD1;
         pAn2 -= RECURSION_FRACTION_2*diffD2;
       }
     }
     else
     {
       pAn1 -= f_massAtt1->Eval(1.0/pTheta1) - f_massAtt1->Eval(0.0);
       pAn2 -= f_massAtt2->Eval(1.0/pTheta2) - f_massAtt2->Eval(0.0);
     }


     pKE1 = g_massCalib1->Eval(pAn1) + g_phd->Eval(pA1);
     pKE2 = g_massCalib2->Eval(pAn2) + g_phd->Eval(pA2);

     if( (pAn1 > MIN_ANODE1) && (pAn2 > MIN_ANODE2) && (pTheta1 > MIN_ANG1) && (pTheta2 > MIN_ANG2)
       && (pTheta1 > MIN_ANG_CALIB) && (pTheta2 > MIN_ANG_CALIB) && (pTheta1 < MAX_ANG_CALIB) && (pTheta2 < MAX_ANG_CALIB))
     {
       h1_PHDAn1->Fill(pKE1);
       h1_PHDAn2->Fill(pKE2);
     }
   }


  TF1* gaussFit = new TF1("gaussFit_pCalib", "[0] * e^(-((x - [1])^2)/(2*[2]^2))", 0, MAX_KE);
  // yield 1
  // TF1* f_gaussYield1 = new TF1("f_gaussYield1", "gaussFit + gaussFit", 0, MAX_KE);
  // TF1* f_gaussYield2 = new TF1("f_gaussYield2", "gaussFit + gaussFit", 0, MAX_KE);
  TF1* f_gaussYield1 = new TF1("f_gaussYield1_pCalib", "gaussFit_pCalib + gaussFit_pCalib + gaussFit_pCalib", 0, MAX_KE);
  TF1* f_gaussYield2 = new TF1("f_gaussYield2_pCalib", "gaussFit_pCalib + gaussFit_pCalib + gaussFit_pCalib", 0, MAX_KE);

  double ampGuess, meanGuess, sigGuess;
  ampGuess = h1_PHDAn1->GetMaximum();
  meanGuess = h1_PHDAn1->GetMean();
  sigGuess = g_massCalib1->Eval(SIG_ANODE) - g_massCalib1->Eval(0);

  cout << "1: " <<  ampGuess << " " << meanGuess << " " << sigGuess << endl;
  // cout << ampGuess << " " << meanGuess << sigGuess << " " << endl;
  // f_gaussYield1->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
  f_gaussYield1->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess, ampGuess/10, meanGuess, sigGuess*2);
  f_gaussYield1->SetParLimits(1, g_massCalib1->Eval(MIN_ANODE1), meanGuess);
  f_gaussYield1->SetParLimits(4, meanGuess, g_massCalib2->Eval(MAX_APH));
  f_gaussYield1->SetParLimits(6, 0, ampGuess/5);
  f_gaussYield1->SetParLimits(7, meanGuess*0.8, meanGuess*1.2);
  h1_PHDAn1->Fit(f_gaussYield1, "N Q", "", 0, MAX_APH);

  ampGuess = h1_PHDAn2->GetMaximum();
  meanGuess = h1_PHDAn2->GetMean();
  sigGuess =  g_massCalib2->Eval(SIG_ANODE) - g_massCalib2->Eval(0);

  cout << "2: " <<  ampGuess << " " << meanGuess << " " << sigGuess << endl;
  // cout << ampGuess << " " << meanGuess << " " << sigGuess << endl;
  // f_gaussYield2->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
  f_gaussYield2->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess, ampGuess/10, meanGuess, sigGuess*2);
  f_gaussYield2->SetParLimits(1, g_massCalib2->Eval(MIN_ANODE2), meanGuess);
  f_gaussYield2->SetParLimits(4, meanGuess, g_massCalib2->Eval(MAX_APH));
  f_gaussYield2->SetParLimits(7, meanGuess*0.8, meanGuess*1.2);
  f_gaussYield2->SetParLimits(6, 0, ampGuess/5);
  h1_PHDAn2->Fit(f_gaussYield2, "N Q", "", 0, MAX_APH);

  double centroids1[2];
  double centroids2[2];
  double sigmas1[2];
  double sigmas2[2];

  centroids1[0] = f_gaussYield1->GetParameter(1);
  centroids1[1] = f_gaussYield1->GetParameter(4);

  sigmas1[0] = f_gaussYield1->GetParameter(2);
  sigmas1[1] = f_gaussYield1->GetParameter(5);

  centroids2[0] = f_gaussYield2->GetParameter(1);
  centroids2[1] = f_gaussYield2->GetParameter(4);

  sigmas2[0] = f_gaussYield2->GetParameter(2);
  sigmas2[1] = f_gaussYield2->GetParameter(5);


  if (centroids1[0] > centroids1[1])
  {
    double temp = centroids1[0];
    centroids1[0] = centroids1[1];
    centroids1[1] = temp;

    temp = sigmas1[0];
    sigmas1[0] = sigmas1[1];
    sigmas1[1] = temp;
  }
  if (centroids2[0] > centroids2[1])
  {
    double temp = centroids2[0];
    centroids2[0] = centroids2[1];
    centroids2[1] = temp;

    temp = sigmas2[0];
    sigmas2[0] = sigmas2[1];
    sigmas2[1] = temp;
  }

  cout << "centroids source side: " << centroids1[0] << " " << centroids1[1] << endl;
  cout << "centroids backing side: " << centroids2[0] << " " << centroids2[1] << endl;

  cout << "sigmas source side: " << sigmas1[0] << " " << sigmas1[1] << endl;
  cout << "sigmas backing side: " << sigmas2[0] << " " << sigmas2[1] << endl;

  // calibrated results (now without PHD correction)
  double kineticPeaks[2];
  kineticPeaks[0] = infoSystem->KINETIC_PEAKS[0];
  kineticPeaks[1] = infoSystem->KINETIC_PEAKS[1];

  TF1* f_calib1 = new TF1("f_calib1", "[0] + [1]*x", 0, 2*infoSystem->KINETIC_PEAKS[1]);
  TF1* f_calib2 = new TF1("f_calib2", "[0] + [1]*x", 0, 2*infoSystem->KINETIC_PEAKS[1]);

  g_phdCalib1 = new TGraph(2, centroids1, kineticPeaks);
  g_phdCalib1->SetName("g_phdCalib1");
  g_phdCalib1->Fit(f_calib1, "Q");

  g_phdCalib2 = new TGraph(2, centroids2, kineticPeaks);
  g_phdCalib2->SetName("g_phdCalib2");
  g_phdCalib2->Fit(f_calib2, "Q");

  cout << "f_calib1: " << f_calib1->GetParameter(0) << " " << f_calib1->GetParameter(1) << endl;
  cout << "f_calib2: " << f_calib2->GetParameter(0) << " " << f_calib2->GetParameter(1) << endl;

  TCanvas* c_PHDGain = new TCanvas("c_PHDGain_" + TString(to_string(iterationPost)), "c_PHDGain", 400, 500);
  //c_PHDGain->Divide(1,3);

  // draw
  c_PHDGain->cd(1);
  h1_PHDAn1->SetLineColor(kBlue);
  h1_PHDAn2->SetLineColor(kRed);
  h1_PHDAn1->Draw();
  h1_PHDAn2->Draw("SAME");
  f_gaussYield1->SetLineColor(kBlue);
  f_gaussYield2->SetLineColor(kRed);
  f_gaussYield1->Draw("SAME");
  f_gaussYield2->Draw("SAME");

  fragDiagnostics->cd();
  cd_recursion->cd();
  c_PHDGain->Write();




}
