#include "fragFiss.h"
#include "constants.h"
#include <TH2.h>
#include <TF1.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

void fragFiss::ELossCorrection()
{

   if (eventChain == 0) return;
   // Put this somewhere else
   TH2D* h2_backingRem1 = new TH2D("h2_backingRem1","h2_backingRem1", N_BINS_RATIO,-1, 5, N_BINS_APH, 0, MAX_APH);
   TH2D* h2_backingRem2 = new TH2D("h2_backingRem2","h2_backingRem2", N_BINS_RATIO,-1, 5, N_BINS_APH, 0, MAX_APH);

   TH2D* h2_uncalAngle1 = new TH2D("h1_uncalAngle1","h1_uncalAngle1;angle;anode",N_BINS_ANGLE, 0, MAX_ANG1, N_BINS_APH, 0, MAX_APH);
   TH2D* h2_uncalAngle2 = new TH2D("h1_uncalAngle2","h1_uncalAngle2;angle;anode",N_BINS_ANGLE, 0, MAX_ANG2, N_BINS_APH, 0, MAX_APH);


  /*
         _             _       ___ _ _ _   _
        /_\  _ _  __ _| |___  | __(_) | | | |   ___  ___ _ __
       / _ \| ' \/ _` | / -_) | _|| | | | | |__/ _ \/ _ \ '_ \
      /_/ \_\_||_\__, |_\___| |_| |_|_|_| |____\___/\___/ .__/
              |___/                                  |_|
  */

   Long64_t nbytes = 0, nb = 0;
   double cos1, cos2;
   cout << "Filling backing attenuation histograms" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = eventChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      // eliminate non-coincident events
      if(ccoinc != 1)
      {
        continue;
      }

      if(aph[0] < MIN_ANODE1 || aph[1] < MIN_ANODE2) continue;

      // grid inefficiency
      aph[0] = (aph[0] - GRID_INEFFICIENCY*(aph[0] + gph[0]))/(1 - GRID_INEFFICIENCY);
      aph[1] = (aph[1] - GRID_INEFFICIENCY*(aph[1] + gph[1]))/(1 - GRID_INEFFICIENCY);


      // calculate the angles
      if(g_Ang1->Eval(aph[0]) > 0)
      {
        // cos1  = (gph[0]/aph[0])/g_Ang1->Eval(aph[0]); // use data
        cos1  = (gph[0]/aph[0])/f_Ang1->Eval(aph[0]); // use fit to data
      }
      else cos1  = -1;

      if(g_Ang2->Eval(aph[1]) > 0)
      {
        // cos2  = (gph[1]/aph[1])/g_Ang2->Eval(aph[1]); // use data
        cos2  = (gph[1]/aph[1])/f_Ang2->Eval(aph[1]); // use fit to data
      }
      else cos2  = -1;

      if(aph[0] > MIN_ANODE1)
      {
        h2_backingRem1->Fill(1.0/cos1, aph[0]);
        h2_uncalAngle1->Fill(cos1, aph[0]);
      }
      if(aph[1] > MIN_ANODE2)
      {
        h2_backingRem2->Fill(1.0/cos2, aph[1]);
        h2_uncalAngle2->Fill(cos2, aph[1]);
      }


   }

    /*
     ___                            _     ___
    | __| _ __ _ __ _ _ __  ___ _ _| |_  / __| ___ _ __
    | _| '_/ _` / _` | '  \/ -_) ' \  _| \__ \/ -_) '_ \
    |_||_| \__,_\__, |_|_|_\___|_||_\__| |___/\___| .__/
               |___/                             |_|

    */
   // compute the light and heavy fragment separator line as a function of the attenuation

   // determine the x axis
   double attAx[N_BINS_RATIO] = {0};
   int MIN_INV_ANG_BIN, MAX_INV_ANG_BIN;
   for(int t = 0; t < N_BINS_RATIO; t++)
   {
     attAx[t] = h2_backingRem1->GetXaxis()->GetBinCenter(t+1);
     if(attAx[t] < MIN_INV_ANG_FIT) MIN_INV_ANG_BIN = t+1;
     else if(attAx[t] < MAX_INV_ANG_FIT) MAX_INV_ANG_BIN = t-1;
   }
   // number of inverse ang bins
   int NUM_INV_ANG_BIN = MAX_INV_ANG_BIN - MIN_INV_ANG_BIN;

   // set up the fit functions
   TF1* gaussFit = new TF1("gaussFit_att", "[0] * e^(-((x - [1])^2)/(2*[2]^2))", 0, MAX_APH);
   // yield 1
   TF1* f_gaussYield1 = new TF1("f_gaussYield1_att", "gaussFit_att + gaussFit_att", 0, MAX_APH);
   TF1* f_gaussYield2 = new TF1("f_gaussYield2_att", "gaussFit_att + gaussFit_att", 0, MAX_APH);


   // create arrays to store data
   double** centroids1;
   double** centroids2;
   double** sigmas1;
   double** sigmas2;

   double* attAxC;
   double* sepLineAtt1;
   double* sepLineAtt2;

   centroids1 = new double*[2];
   centroids2 = new double*[2];
   sigmas1 = new double*[2];
   sigmas2 = new double*[2];

   attAxC = new double [NUM_INV_ANG_BIN];
   sepLineAtt1 = new double [NUM_INV_ANG_BIN];
   sepLineAtt2 = new double [NUM_INV_ANG_BIN];

   for(int i = 0; i < 2; i++)
   {
     centroids1[i] = new double [NUM_INV_ANG_BIN];
     centroids2[i] = new double [NUM_INV_ANG_BIN];
     sigmas1[i] = new double [NUM_INV_ANG_BIN];
     sigmas2[i] = new double [NUM_INV_ANG_BIN];
   }

   // now loop through the allowable values of angles
   for(int c = MIN_INV_ANG_BIN; c < MAX_INV_ANG_BIN; c++)
   {
     TH1D* h_sliceTKEatt1 = h2_backingRem1->ProjectionY("h_sliceTKEatt1_" + TString(c), c, c);
     TH1D* h_sliceTKEatt2 = h2_backingRem2->ProjectionY("h_sliceTKEatt2_" + TString(c), c, c);

     double ampGuess, meanGuess, sigGuess;
     ampGuess = h_sliceTKEatt1->GetMaximum();
     meanGuess = h_sliceTKEatt1->GetMean();
     sigGuess = SIG_ANODE;
     // cout << ampGuess << " " << meanGuess << sigGuess << " " << endl;
     f_gaussYield1->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
     f_gaussYield2->SetParLimits(1, MIN_ANODE2, meanGuess);
     f_gaussYield2->SetParLimits(4, meanGuess, MAX_APH);
     h_sliceTKEatt1->Fit(f_gaussYield1, "N Q", "", 0, MAX_APH);

     ampGuess = h_sliceTKEatt2->GetMaximum();
     meanGuess = h_sliceTKEatt2->GetMean();
     sigGuess = SIG_ANODE;
     // cout << ampGuess << " " << meanGuess << " " << sigGuess << endl;
     f_gaussYield2->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
     f_gaussYield2->SetParLimits(1, MIN_ANODE2, meanGuess);
     f_gaussYield2->SetParLimits(4, meanGuess, MAX_APH);
     h_sliceTKEatt2->Fit(f_gaussYield2, "N Q", "", 0, MAX_APH);

     // store the results
     attAxC[c-MIN_INV_ANG_BIN] = attAx[c];

     centroids1[0][c-MIN_INV_ANG_BIN] = f_gaussYield1->GetParameter(1);
     centroids1[1][c-MIN_INV_ANG_BIN] = f_gaussYield1->GetParameter(4);
     sepLineAtt1[c-MIN_INV_ANG_BIN] = 0.5*(centroids1[0][c-MIN_INV_ANG_BIN] + centroids1[1][c-MIN_INV_ANG_BIN]);
     sigmas1[0][c-MIN_INV_ANG_BIN] =  f_gaussYield1->GetParameter(2);
     sigmas1[1][c-MIN_INV_ANG_BIN] =  f_gaussYield1->GetParameter(5);

     centroids2[0][c-MIN_INV_ANG_BIN] = f_gaussYield2->GetParameter(1);
     centroids2[1][c-MIN_INV_ANG_BIN] = f_gaussYield2->GetParameter(4);
     sepLineAtt2[c-MIN_INV_ANG_BIN] = 0.5*(centroids2[0][c-MIN_INV_ANG_BIN] + centroids2[1][c-MIN_INV_ANG_BIN]);
     sigmas2[0][c-MIN_INV_ANG_BIN] =  f_gaussYield2->GetParameter(2);
     sigmas2[1][c-MIN_INV_ANG_BIN] =  f_gaussYield2->GetParameter(5);

   }
   // create the graphs
   TGraph* g_cent1H = new TGraph(NUM_INV_ANG_BIN, attAxC, centroids1[0]);
   TGraph* g_cent1L = new TGraph(NUM_INV_ANG_BIN, attAxC, centroids1[1]);
   TGraph* g_sig1H = new TGraph(NUM_INV_ANG_BIN, attAxC, sigmas1[0]);
   TGraph* g_sig1L = new TGraph(NUM_INV_ANG_BIN, attAxC, sigmas1[1]);
   TGraph* g_sep1 = new TGraph(NUM_INV_ANG_BIN, attAxC, sepLineAtt1);

   TGraph* g_cent2H = new TGraph(NUM_INV_ANG_BIN, attAxC, centroids2[0]);
   TGraph* g_cent2L = new TGraph(NUM_INV_ANG_BIN, attAxC, centroids2[1]);
   TGraph* g_sig2H = new TGraph(NUM_INV_ANG_BIN, attAxC, sigmas2[0]);
   TGraph* g_sig2L = new TGraph(NUM_INV_ANG_BIN, attAxC, sigmas2[1]);
   TGraph* g_sep2 = new TGraph(NUM_INV_ANG_BIN, attAxC, sepLineAtt2);


   g_cent1H->SetName("g_cent1H");
   g_cent1L->SetName("g_cent1L");
   g_sig1H->SetName("g_sig1H");
   g_sig1L->SetName("g_sig1L");
   g_cent2H->SetName("g_cent2H");
   g_cent2L->SetName("g_cent2L");
   g_sig2H->SetName("g_sig2H");
   g_sig2L->SetName("g_sig2L");
   g_sep1->SetName("g_sep1");
   g_sep2->SetName("g_sep2");


   if(QUADRATIC_ATT)
   {
     // quadratic attenuation
     f_att1H = new TF1("f_att1H", "[0] + [1]*x + [2]*x^2", -10, 10);
     f_att1L = new TF1("f_att1L", "[0] + [1]*x + [2]*x^2", -10, 10);
     f_att2H = new TF1("f_att2H", "[0] + [1]*x + [2]*x^2", -10, 10);
     f_att2L = new TF1("f_att2L", "[0] + [1]*x + [2]*x^2", -10, 10);
   }
   else
   {
     // linear attenuation
     f_att1H = new TF1("f_att1H", "[0] + [1]*x", -10, 10);
     f_att1L = new TF1("f_att1L", "[0] + [1]*x", -10, 10);
     f_att2H = new TF1("f_att2H", "[0] + [1]*x", -10, 10);
     f_att2L = new TF1("f_att2L", "[0] + [1]*x", -10, 10);
   }


   f_sepAtt1 = new TF1("f_sepAtt1", "[0] + [1]*x", -10, 10);
   f_sepAtt2 = new TF1("f_sepAtt2", "[0] + [1]*x", -10, 10);

   g_cent1H->Fit(f_att1H, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);
   g_cent1L->Fit(f_att1L, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);
   g_cent2H->Fit(f_att2H, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);
   g_cent2L->Fit(f_att2L, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);

   g_sep1->Fit(f_sepAtt1, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);
   g_sep2->Fit(f_sepAtt2, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);


  /*
     _  _   _                     _   _
    /_\| |_| |_ ___ _ _ _  _ __ _| |_(_)___ _ _
   / _ \  _|  _/ -_) ' \ || / _` |  _| / _ \ ' \
  /_/ \_\__|\__\___|_||_\_,_\__,_|\__|_\___/_||_|
  */

   // find the profiles
   TProfile* p1_backing1 = h2_backingRem1->ProfileX();
   TProfile* p1_backing2 = h2_backingRem2->ProfileX();

   // fits
   f_att1 = new TF1("f_att1", "[0] + [1]*x", -10, 10);
   f_att2 = new TF1("f_att1", "[0] + [1]*x", -10, 10);

   p1_backing1->Fit(f_att1, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);
   p1_backing2->Fit(f_att2, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);

   // analysis of fits
   // ask fredrik about this
   double crossPoint = (f_att2->GetParameter(0) - f_att1->GetParameter(0))/(f_att1->GetParameter(1) - f_att2->GetParameter(1));
   double heightPoint = f_att1->Eval(crossPoint);

   cout << "The intersect point is " << crossPoint << " and the height is " << heightPoint << endl;


   // write for all
   if(DOUBLE_ATT_LINE)
   {
     cout << "f_att1L: " << f_att1L->GetParameter(0) << " " << f_att1L->GetParameter(1) << endl;
     cout << "f_att1H: " << f_att1H->GetParameter(0) << " " << f_att1H->GetParameter(1) << endl;
     cout << "f_att2L: " << f_att2L->GetParameter(0) << " " << f_att2L->GetParameter(1) << endl;
     cout << "f_att2H: " << f_att2H->GetParameter(0) << " " << f_att2H->GetParameter(1) << endl;
   }
   else
   {
     cout << "f_att1: " << f_att1->GetParameter(0) << " " << f_att1->GetParameter(1) << endl;
     cout << "f_att2: " << f_att2->GetParameter(0) << " " << f_att2->GetParameter(1) << endl;
   }


    /*
     ___ _     _
    | _ \ |___| |_
    |  _/ / _ \  _|
    |_| |_\___/\__|

    */

   // canvas with angle results
   TCanvas* c_backing = new TCanvas("c_backing", "c_backing", 400, 500);
   c_backing->Divide(1,3);

   c_backing->cd(1);
   p1_backing1->Draw();
   p1_backing1->SetName("p1_backing1");
   p1_backing1->SetLineColor(kBlue);
   f_att1->Draw("SAME");


   p1_backing2->Draw("SAME");
   p1_backing2->SetName("p1_backing2");
   p1_backing2->SetLineColor(kRed);
   f_att2->Draw("SAME");

   c_backing->cd(2);
   h2_backingRem1->Draw("COLZ");
   // centers
   g_cent1H->SetLineColor(kRed);
   g_cent1L->SetLineColor(kRed);
   g_cent1H->SetLineWidth(2);
   g_cent1L->SetLineWidth(2);
   g_cent1H->Draw("SAME");
   g_cent1L->Draw("SAME");
   // sep
   g_sep1->SetLineColor(kBlack);
   g_sep1->SetLineStyle(kDashed);
   g_sep1->SetLineWidth(2);
   g_sep1->Draw("SAME");
   f_sepAtt1->Draw("SAME");
   f_att1L->SetLineColor(kRed);
   f_att1H->SetLineColor(kRed);
   f_att1L->Draw("SAME");
   f_att1H->Draw("SAME");


   c_backing->cd(3);
   h2_backingRem2->Draw("COLZ");
   // centers
   g_cent2H->SetLineColor(kGreen);
   g_cent2L->SetLineColor(kGreen);
   g_cent2H->SetLineWidth(2);
   g_cent2L->SetLineWidth(2);
   g_cent2H->Draw("SAME");
   g_cent2L->Draw("SAME");
   // sep
   g_sep2->SetLineColor(kBlack);
   g_sep2->SetLineStyle(kDashed);
   g_sep2->SetLineWidth(2);
   g_sep2->Draw("SAME");
   f_sepAtt2->Draw("SAME");
   f_att2L->SetLineColor(kRed);
   f_att2H->SetLineColor(kRed);
   f_att2L->Draw("SAME");
   f_att2H->Draw("SAME");

   TCanvas* c_sigmas = new TCanvas("c_sigmas", "c_sigmas", 400, 500);
   c_sigmas->cd();
   g_sig1H->SetLineColor(kBlue);
   g_sig1L->SetLineColor(kBlue);
   // g_sig1H->SetLineStyle(kSolid);
   g_sig1L->SetLineColor(kDashed);

   g_sig2H->SetLineColor(kRed);
   g_sig2L->SetLineColor(kRed);
   // g_sig2H->SetLineStyle(kSolid);
   g_sig2L->SetLineColor(kDashed);

   g_sig1H->Draw();
   g_sig1L->Draw("SAME");
   g_sig2H->Draw("SAME");
   g_sig2L->Draw("SAME");


   TCanvas* c_angUncal = new TCanvas("c_angUncal", "c_angUncal", 400, 500);
   c_angUncal->Divide(1,2);

   c_angUncal->cd(1);
   h2_uncalAngle1->Draw("COLZ");

   c_angUncal->cd(2);
   h2_uncalAngle2->Draw("COLZ");


   fragDiagnostics->cd();
   cd_diagnostics->cd();
   c_backing->Write();
   c_angUncal->Write();
   c_sigmas->Write();


 }
