#include "fragFiss.h"
#include "constants.h"

#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

void fragFiss::PostCalib(int iterationPost)
{
  cout << "Recalibrating anode histograms" << endl;

  TH1D* h1_massAn1 = new TH1D("h1_massAn1_"+ TString(to_string(iterationPost)),"h1_massAn1", N_BINS_APH, 0, MAX_APH);
  TH1D* h1_massAn2 = new TH1D("h1_massAn2_"+ TString(to_string(iterationPost)),"h1_massAn2", N_BINS_APH, 0, MAX_APH);

  TH3D* h3_massKEAttCalib1 = new TH3D("h3_massKEAttCalib1_"+ TString(to_string(iterationPost)),"h3_massKEAttCalib1; mass;  1/angle; kinetic energy", N_BINS_MASS_TH, MIN_MASS, MAX_MASS, N_BINS_RATIO,-1, 5, N_BINS_APH,0,MAX_APH);
  TH3D* h3_massKEAttCalib2 = new TH3D("h3_massKEAttCalib2_"+ TString(to_string(iterationPost)),"h3_massKEAttCalib2; mass; 1/angle; kinetic energy", N_BINS_MASS_TH, MIN_MASS, MAX_MASS, N_BINS_RATIO,-1, 5, N_BINS_APH,0,MAX_APH);

  Long64_t nentries = fragTree->GetEntries();

  double pThetaAv;

  for (Long64_t jentry = 0; jentry < nentries; jentry++)
  {
     Long64_t ientry = fragTree->LoadTree(jentry);
     if(ientry < 0) break; // reached the end of the file


     fragTree->GetEntry(jentry);

     // if(pA1 < MIN_MASS_ANALYSIS || pA1 > MAX_MASS_ANALYSIS || pA2 < MIN_MASS_ANALYSIS || pA2 > MAX_MASS_ANALYSIS) continue;

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
     pTheta2 /= g_AngMass2->Eval(pA2);

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
         // cout << diffD1 << " " << diffD2 << endl;
         pAn1 -= RECURSION_FRACTION_1*diffD1;
         pAn2 -= RECURSION_FRACTION_2*diffD2;
       }

     }
     else
     {
       pAn1 -= f_massAtt1->Eval(1.0/pTheta1) - f_massAtt1->Eval(0.0);
       pAn2 -= f_massAtt2->Eval(1.0/pTheta2) - f_massAtt2->Eval(0.0);
     }


     // cout << pA1 << " " << pTheta1 << " " << - (g_slopeMass1->Eval(pA1))*1.0/pTheta1 << endl;

     if( (pAn1 > MIN_ANODE1) && (pAn2 > MIN_ANODE2) && (pTheta1 > MIN_ANG1) && (pTheta2 > MIN_ANG2))
     {

       h3_massKEAttCalib1->Fill(pA1, 1.0/pTheta1, pAn1);
       h3_massKEAttCalib2->Fill(pA2, 1.0/pTheta2, pAn2);

      if((pTheta1 > MIN_ANG_CALIB) && (pTheta2 > MIN_ANG_CALIB) && (pTheta1 < MAX_ANG_CALIB) && (pTheta2 < MAX_ANG_CALIB))
      {
        h1_massAn1->Fill(pAn1);
        h1_massAn2->Fill(pAn2);
      }

     }
   }

   /*

   ___                 _               _             _         _
  / __|__ _ _  _ _____(_)__ _ _ _     /_\  _ _  __ _| |_  _ __(_)___
 | (_ / _` | || (_-<_-< / _` | ' \   / _ \| ' \/ _` | | || (_-< (_-<
  \___\__,_|\_,_/__/__/_\__,_|_||_| /_/ \_\_||_\__,_|_|\_, /__/_/__/
                                                       |__/
   */


  TF1* gaussFit = new TF1("gaussFit_calib", "[0] * e^(-((x - [1])^2)/(2*[2]^2))", 0, MAX_APH);
  // yield 1
  TF1* f_gaussYield1 = new TF1("f_gaussYield1_calib", "gaussFit_calib + gaussFit_calib", 0, MAX_APH);
  TF1* f_gaussYield2 = new TF1("f_gaussYield2_calib", "gaussFit_calib + gaussFit_calib", 0, MAX_APH);

  double ampGuess, meanGuess, sigGuess;
  ampGuess = h1_massAn1->GetMaximum();
  meanGuess = h1_massAn1->GetMean();
  sigGuess = SIG_ANODE;
  // cout << ampGuess << " " << meanGuess << sigGuess << " " << endl;
  f_gaussYield1->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
  f_gaussYield1->SetParLimits(1, MIN_ANODE2, meanGuess);
  f_gaussYield1->SetParLimits(4, meanGuess, MAX_APH);
  h1_massAn1->Fit(f_gaussYield1, "N Q", "", 0, MAX_APH);

  ampGuess = h1_massAn2->GetMaximum();
  meanGuess = h1_massAn2->GetMean();
  sigGuess = SIG_ANODE;
  // cout << ampGuess << " " << meanGuess << " " << sigGuess << endl;
  f_gaussYield2->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
  f_gaussYield2->SetParLimits(1, MIN_ANODE2, meanGuess);
  f_gaussYield2->SetParLimits(4, meanGuess, MAX_APH);
  h1_massAn2->Fit(f_gaussYield2, "N Q", "", 0, MAX_APH);

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


/*

   _  _   _                     _   _             ___ _           _
  /_\| |_| |_ ___ _ _ _  _ __ _| |_(_)___ _ _    / __| |_  ___ __| |__
 / _ \  _|  _/ -_) ' \ || / _` |  _| / _ \ ' \  | (__| ' \/ -_) _| / /
/_/ \_\__|\__\___|_||_\_,_\__,_|\__|_\___/_||_|  \___|_||_\___\__|_\_\


*/
  // check that the attenuation has been calibrated out

  TProfile** p1_massAtt1;
  TProfile** p1_massAtt2;

  p1_massAtt1 = new TProfile*[N_BINS_MASS_TH];
  p1_massAtt2 = new TProfile*[N_BINS_MASS_TH];

  int numMass1 = 0;
  int numMass2 = 0;

  TF1* f_massAttCalibProj1 = new TF1("f_massAttCalibProj1", "[0] + [1]*x", -10, 10);
  TF1* f_massAttCalibProj2 = new TF1("f_massAttCalibProj2", "[0] + [1]*x", -10, 10);

  for(int m = 1; m <= N_BINS_MASS_TH; m++)
  {

    h3_massKEAttCalib1->GetXaxis()->SetRange(m,m);

    TH2D* h2_mKEAttCalib1  = ((TH2D*)h3_massKEAttCalib1->Project3D("zy"));

    if(h2_mKEAttCalib1->GetEntries() < MIN_COUNT_MASS_ATT) continue;

    TProfile* p1_temp1 = h2_mKEAttCalib1->ProfileX();

    p1_massAtt1[numMass1] = (TProfile*)p1_temp1->Clone();

    p1_massAtt1[numMass1]->SetName("p1_massAtt1_" + (TString)to_string(m));
    p1_massAtt1[numMass1]->Fit(f_massAttCalibProj1, "QS", "", MIN_INV_ANG_FIT_TIP, MAX_INV_ANG_FIT_TIP);

    numMass1++;
  }

  for(int m = 1; m <= N_BINS_MASS_TH; m++)
  {

    h3_massKEAttCalib2->GetXaxis()->SetRange(m,m);

    TH2D* h2_mKEAttCalib2  = ((TH2D*)h3_massKEAttCalib2->Project3D("zy"));

    if(h2_mKEAttCalib2->GetEntries() < MIN_COUNT_MASS_ATT) continue;

    TProfile* p1_temp2 = h2_mKEAttCalib2->ProfileX();

    p1_massAtt2[numMass2] = (TProfile*)p1_temp2->Clone();
    p1_massAtt2[numMass2]->SetName("p1_massAtt2_" + (TString)to_string(m));
    p1_massAtt2[numMass2]->Fit(f_massAttCalibProj2, "QS", "", MIN_INV_ANG_FIT_TIP, MAX_INV_ANG_FIT_TIP);

    numMass2++;
  }




  // cout << "centroids source side: " << centroids1[0] << " " << centroids1[1] << endl;
  // cout << "centroids backing side: " << centroids2[0] << " " << centroids2[1] << endl;
  //
  // cout << "sigmas source side: " << sigmas1[0] << " " << sigmas1[1] << endl;
  // cout << "sigmas backing side: " << sigmas2[0] << " " << sigmas2[1] << endl;

  // calibrated results
  double kineticPeaks[2];
  kineticPeaks[0] = infoSystem->KINETIC_PEAKS[0] - PHD_H;
  kineticPeaks[1] = infoSystem->KINETIC_PEAKS[1] - PHD_L;

  TF1* f_calib1 = new TF1("f_calib1", "[0] + [1]*x", 0, 2*infoSystem->KINETIC_PEAKS[1]);
  TF1* f_calib2 = new TF1("f_calib2", "[0] + [1]*x", 0, 2*infoSystem->KINETIC_PEAKS[1]);

  g_massCalib1 = new TGraph(2, centroids1, kineticPeaks);
  g_massCalib1->SetName("g_massCalib1");
  g_massCalib1->Fit(f_calib1, "Q");

  g_massCalib2 = new TGraph(2, centroids2, kineticPeaks);
  g_massCalib2->SetName("g_massCalib2");
  g_massCalib2->Fit(f_calib2, "Q");

  // cout << "f_calib1: " << f_calib1->GetParameter(0) << " " << f_calib1->GetParameter(1) << endl;
  // cout << "f_calib2: " << f_calib2->GetParameter(0) << " " << f_calib2->GetParameter(1) << endl;

  TCanvas* c_gain = new TCanvas("c_massGain_" + TString(to_string(iterationPost)), "c_massGain", 400, 500);
  //c_gain->Divide(1,3);

  // draw
  c_gain->cd(1);
  h1_massAn1->SetLineColor(kBlue);
  h1_massAn2->SetLineColor(kRed);
  h1_massAn1->Draw();
  h1_massAn2->Draw("SAME");
  f_gaussYield1->SetLineColor(kBlue);
  f_gaussYield2->SetLineColor(kRed);
  f_gaussYield1->Draw("SAME");
  f_gaussYield2->Draw("SAME");


  TCanvas* c_massAttSingCalib = new TCanvas("c_massAttSingCalib" + TString(to_string(iterationPost)), "c_massAttSingCalib", 400, 500);
  c_massAttSingCalib->Divide(5,2);

  int jPan = 0;
  int canPan = 1;
  while(jPan < numMass1 & jPan < numMass2)
  {
    c_massAttSingCalib->cd(canPan);
    p1_massAtt1[jPan]->Draw();
    p1_massAtt2[jPan]->Draw("SAME");
    jPan += 5;
    canPan++;
  }




  fragDiagnostics->cd();
  cd_recursion->cd();
  c_gain->Write();
  c_massAttSingCalib->Write();







}
