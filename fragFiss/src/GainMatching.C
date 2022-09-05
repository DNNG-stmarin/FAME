#include "fragFiss.h"
#include "constants.h"
#include <TH2.h>
#include <TF1.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

void fragFiss::GainMatching()
{

   if (eventChain == 0) return;
   // Put this somewhere else
   TH1D* h1_uncalibratedAn1 = new TH1D("h1_uncalibratedAn1","h1_uncalibratedAn1", N_BINS_APH, 0, MAX_APH);
   TH1D* h1_uncalibratedAn2 = new TH1D("h1_uncalibratedAn2","h1_uncalibratedAn2", N_BINS_APH, 0, MAX_APH);

   TH2D* h2_calAngle1 = new TH2D("h1_calAngle1","h1_calAngle1;angle;anode",N_BINS_ANGLE, 0, MAX_ANG1, N_BINS_APH, 0, MAX_APH);
   TH2D* h2_calAngle2 = new TH2D("h1_calAngle2","h1_calAngle2;angle;anode",N_BINS_ANGLE, 0, MAX_ANG2, N_BINS_APH, 0, MAX_APH);

  /*
         _             _       ___ _ _ _   _
        /_\  _ _  __ _| |___  | __(_) | | | |   ___  ___ _ __
       / _ \| ' \/ _` | / -_) | _|| | | | | |__/ _ \/ _ \ '_ \
      /_/ \_\_||_\__, |_\___| |_| |_|_|_| |____\___/\___/ .__/
              |___/                                  |_|
  */

   Long64_t nbytes = 0, nb = 0;
   double cos1, cos2;
   double ua1, ua2;
   cout << "Filling gain matching histograms" << endl;

   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = eventChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      if(aph[0] < MIN_ANODE1 || aph[1] < MIN_ANODE2) continue;

      if(ccoinc != 1)
      {
        continue;
      }

      aph[0] = (aph[0] - GRID_INEFFICIENCY*(aph[0] + gph[0]))/(1 - GRID_INEFFICIENCY);
      aph[1] = (aph[1] - GRID_INEFFICIENCY*(aph[1] + gph[1]))/(1 - GRID_INEFFICIENCY);

      // calculate the angles
      if(g_Ang1->Eval(aph[0]) > 0)
      {
        cos1 = (gph[0]/aph[0])/f_Ang1->Eval(aph[0]);
      }
      else cos1 = -1;

      if(g_Ang2->Eval(aph[1]) > 0)
      {
        cos2 = (gph[1]/aph[1])/f_Ang2->Eval(aph[1]);
      }
      else cos2 = -1;


      // kinetic energy
      ua1 = aph[0];
      ua2 = aph[1];

      if(ua1 < MIN_ANODE1 || ua2 < MIN_ANODE2) continue;

      if(DOUBLE_ATT_LINE)
      {
        // use double line evaluation
        if(ua1 < f_sepAtt1->Eval(1.0/cos1)) // heavy fragment
        {
          ua1 -= f_att1H->Eval(1.0/cos1) - f_att1H->Eval(0);
        }
        else // light fragment
        {
          ua1 -= f_att1L->Eval(1.0/cos1) - f_att1L->Eval(0);
        }

        if(ua2 < f_sepAtt2->Eval(1.0/cos2)) // heavy fragment
        {
          ua2 -= f_att2H->Eval(1.0/cos2) - f_att2H->Eval(0);
        }
        else // light fragment
        {
          ua2 -= f_att2L->Eval(1.0/cos2) - f_att2L->Eval(0);
        }
      }
      else
      {
        // // use the single line evaluation
        ua1 -= f_att1->Eval(1.0/cos1) - f_att1->Eval(0);
        ua2 -= f_att2->Eval(1.0/cos2) - f_att2->Eval(0);
      }

      h2_calAngle1->Fill(cos1, ua1);
      h2_calAngle2->Fill(cos2, ua2);


      if( (ua1 > MIN_ANODE1) && (ua2 > MIN_ANODE2) && (cos1 > MIN_ANG1) && (cos2 > MIN_ANG2)
       && (cos1 > MIN_ANG_CALIB) && (cos2 > MIN_ANG_CALIB) && (cos1 < MAX_ANG_CALIB) && (cos2 < MAX_ANG_CALIB ) )
          // && (cos1 < MAX_ANG1) && (cos2 < MAX_ANG2))
      {
        h1_uncalibratedAn1->Fill(ua1);
        h1_uncalibratedAn2->Fill(ua2);
      }

   }

   TF1* gaussFit = new TF1("gaussFit_gain", "[0] * e^(-((x - [1])^2)/(2*[2]^2))", 0, MAX_APH);
   // yield 1
   TF1* f_gaussYield1 = new TF1("f_gaussYield1_gain", "gaussFit_gain + gaussFit_gain", 0, MAX_APH);
   TF1* f_gaussYield2 = new TF1("f_gaussYield2_gain", "gaussFit_gain + gaussFit_gain", 0, MAX_APH);

   double ampGuess, meanGuess, sigGuess;
   ampGuess = h1_uncalibratedAn1->GetMaximum();
   meanGuess = h1_uncalibratedAn1->GetMean();
   sigGuess = SIG_ANODE;
   // cout << ampGuess << " " << meanGuess << sigGuess << " " << endl;
   f_gaussYield1->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
   f_gaussYield1->SetParLimits(1, MIN_ANODE2, meanGuess);
   f_gaussYield1->SetParLimits(4, meanGuess, MAX_APH);
   h1_uncalibratedAn1->Fit(f_gaussYield1, "N Q", "", 0, MAX_APH);

   ampGuess = h1_uncalibratedAn2->GetMaximum();
   meanGuess = h1_uncalibratedAn2->GetMean();
   sigGuess = SIG_ANODE;
   // cout << ampGuess << " " << meanGuess << " " << sigGuess << endl;
   f_gaussYield2->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
   f_gaussYield2->SetParLimits(1, MIN_ANODE2, meanGuess);
   f_gaussYield2->SetParLimits(4, meanGuess, MAX_APH);
   h1_uncalibratedAn2->Fit(f_gaussYield2, "N Q", "", 0, MAX_APH);

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


   // TF1* f_intersect = new TF1("f_intersect", [0] + [1] * x);
   // TF1* f_intersect = new TF1("f_intersect", [0] + [1] * x);
   // TF1* f_intersect = new TF1("f_intersect", [0] + [1] * x);
   //
   g_gainMatch = new TGraph(2, centroids2, centroids1);
   g_gainMatch->SetName("g_gainMatch");
   g_gainMatch->Fit("pol1", "Q");



   // calibrated results
   double kineticPeaks[2];
   kineticPeaks[0] = infoSystem->KINETIC_PEAKS[0] - PHD_H;
   kineticPeaks[1] = infoSystem->KINETIC_PEAKS[1] - PHD_L;

   TF1* f_calib1 = new TF1("f_calib1", "[0] + [1]*x", 0, 2*infoSystem->KINETIC_PEAKS[1]);
   TF1* f_calib2 = new TF1("f_calib2", "[0] + [1]*x", 0, 2*infoSystem->KINETIC_PEAKS[1]);

   g_calib1 = new TGraph(2, centroids1, kineticPeaks);
   g_calib1->SetName("g_calib1");
   g_calib1->Fit(f_calib1, "Q");

   g_calib2 = new TGraph(2, centroids2, kineticPeaks);
   g_calib2->SetName("g_calib2");
   g_calib2->Fit(f_calib2, "Q");

   cout << "f_calib1: " << f_calib1->GetParameter(0) << " " << f_calib1->GetParameter(1) << endl;
   cout << "f_calib2: " << f_calib2->GetParameter(0) << " " << f_calib2->GetParameter(1) << endl;


   //
   // // canvas with gain matched results
   TCanvas* c_gain = new TCanvas("c_gain", "c_gain", 400, 500);
   c_gain->Divide(1,3);

   // draw
   c_gain->cd(1);
   h1_uncalibratedAn1->SetLineColor(kBlue);
   h1_uncalibratedAn2->SetLineColor(kRed);
   h1_uncalibratedAn1->Draw();
   h1_uncalibratedAn2->Draw("SAME");
   f_gaussYield1->SetLineColor(kBlue);
   f_gaussYield2->SetLineColor(kRed);
   f_gaussYield1->Draw("SAME");
   f_gaussYield2->Draw("SAME");

   c_gain->cd(2);
   g_gainMatch->Draw();

   c_gain->cd(3);
   g_calib1->Draw();
   g_calib2->Draw("SAME");

   // write file
   fragDiagnostics->cd();
   cd_diagnostics->cd();
   c_gain->Write();

   TCanvas* c_angCal = new TCanvas("c_angCal", "c_angCal", 400, 500);
   c_angCal->Divide(1,2);

   c_angCal->cd(1);
   h2_calAngle1->Draw("COLZ");

   c_angCal->cd(2);
   h2_calAngle2->Draw("COLZ");

   c_angCal->Write();


 }
