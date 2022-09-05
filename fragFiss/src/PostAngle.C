#include "fragFiss.h"
#include "constants.h"
#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

void fragFiss::PostAngle(int iterationPost)
{

  // cout << "Posteriori charge corrections" << endl;
  fragDiagnostics->cd();

  TH2D* h2_massAngle1 = new TH2D("h2_massAngle1_"+ TString(to_string(iterationPost)),"h2_massAngle1;angle;mass",N_BINS_ANGLE,0,MAX_ANG1, N_BINS_MASS_TH, MIN_MASS, MAX_MASS);
  TH2D* h2_massAngle2 = new TH2D("h2_massAngle2_"+ TString(to_string(iterationPost)),"h2_massAngle2;angle;mass",N_BINS_ANGLE,0,MAX_ANG1, N_BINS_MASS_TH, MIN_MASS, MAX_MASS);

  const int MIN_ANGLE_POST_BIN = (int)(MIN_ANG_POST/MAX_ANG1*N_BINS_ANGLE);
  const int MAX_ANGLE_POST_BIN = (int)(MAX_ANG_POST/MAX_ANG1*N_BINS_ANGLE);

  //out << MIN_ANGLE_POST_BIN << " " << MAX_ANGLE_POST_BIN << endl;

  const int MIN_MASS_BR_BIN = (int)((MIN_MASS_BR-MIN_MASS)/(MAX_MASS - MIN_MASS)*N_BINS_MASS_TH);
  const int MAX_MASS_BR_BIN = (int)((MAX_MASS_BR - MIN_MASS)/(MAX_MASS - MIN_MASS)*N_BINS_MASS_TH);
  const int MID_MASS_BIN = (int)((A_TOT/2.0 - MIN_MASS)/(MAX_MASS - MIN_MASS)*N_BINS_MASS_TH);

  cout << "Filling mass angle histogram" << endl;
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

     pAn1 = (pAn1 - GRID_INEFFICIENCY*(pAn1 + pGr1))/(1 - GRID_INEFFICIENCY);
     pAn2 = (pAn2 - GRID_INEFFICIENCY*(pAn2 + pGr2))/(1 - GRID_INEFFICIENCY);

     pThetaAv = 0.5*(pTheta1 + pTheta2);

     // h3_anodeMassGrid1->Fill(pGr1/pAn1, pAn1, pA1);
     // h3_anodeMassGrid2->Fill(pGr2/pAn2, pAn2, pA2);

     h2_massAngle1->Fill(pTheta1, pA1);
     h2_massAngle2->Fill(pTheta2, pA2);
  }

  /*
     _             _       ___ _ _ _   _
    /_\  _ _  __ _| |___  | __(_) | | | |   ___  ___ _ __
   / _ \| ' \/ _` | / -_) | _|| | | | | |__/ _ \/ _ \ '_ \
  /_/ \_\_||_\__, |_\___| |_| |_|_|_| |____\___/\___/ .__/
             |___/                                  |_|
  */
   // Determine angle
   double *chargeLength1, *chargeLength2, *massAx;
   chargeLength1 = new double[N_BINS_MASS_TH];
   chargeLength2 = new double[N_BINS_MASS_TH];
   massAx = new double[N_BINS_MASS_TH];


   // analysis for selection
   double *angM1, *angM2, *cosAx;
   angM1 = new double[N_BINS_ANGLE];
   angM2 = new double[N_BINS_ANGLE];
   cosAx = new double[N_BINS_ANGLE];

   double *dangM1, *dangM2, *dcosAx;
   dangM1 = new double[N_BINS_ANGLE - 1];
   dangM2 = new double[N_BINS_ANGLE - 1];
   dcosAx = new double[N_BINS_ANGLE - 1];

   double *d2angM1, *d2angM2, *d2cosAx;
   d2angM1 = new double[N_BINS_ANGLE - 1];
   d2angM2 = new double[N_BINS_ANGLE - 1];
   d2cosAx = new double[N_BINS_ANGLE - 1];



   for (int b=0; b < N_BINS_MASS_TH; b++)
   {
     massAx[b] = h2_massAngle1->GetYaxis()->GetBinCenter(b+1);

     TH1D* projMass1 = h2_massAngle1->ProjectionX("projMass1" + TString(b),b+1,b+1);
     TH1D* projMass2 = h2_massAngle2->ProjectionX("projMass2" + TString(b),b+1,b+1);

     projMass1->Smooth(2);
     projMass2->Smooth(2);

     // line conversion

     int maxBinangM1 = -1;
     int maxBinangM2 = -1;
     double maxangM1 = -1;
     double maxangM2 = -1;

     for (int i = MIN_ANGLE_POST_BIN; i < MAX_ANGLE_POST_BIN; i++)
     {
       cosAx[i] = 0.5*(projMass1->GetBinCenter(i+1) + (projMass2->GetBinCenter(i+1)));
       angM1[i]  = projMass1->GetBinContent(i+1);
       angM2[i]  = projMass2->GetBinContent(i+1);

       // find maximum and bin
       if(angM1[i] > maxangM1)
       {
         maxangM1 = angM1[i];
         maxBinangM1 = i;
       }

       if(angM2[i] > maxangM2)
       {
         maxangM2 = angM2[i];
         maxBinangM2 = i;
       }

     }

     // derivative filter
     int dMinBinangM1 = -1;
     int dMinBinangM2 = -1;
     double dMinangM1 = -1;
     double dMinangM2 = -1;

     double dPrev = 0;
     double dPrev1 = 0;
     double dPrev2 = 0;
     double dNext = 0;
     double dNext1 = 0;
     double dNext2 = 0;

     bool dFound1 = false;
     bool dFound2 = false;

     // derivative filter
     for(int i = MIN_ANGLE_POST_BIN; i < MAX_ANGLE_POST_BIN - 1; i++)
     {
       // calculate local derivative
       dcosAx[i] = (cosAx[i+1] + cosAx[i]) / 2.0;
       dangM1[i] = (angM1[i+1] - angM1[i]) / dcosAx[i];
       dangM2[i] = (angM2[i+1] - angM2[i]) / dcosAx[i];

       // cout << dAPH1[i] << " ";
     }
     // cout << endl;

     // second derivative filter
     for(int i = MIN_ANGLE_POST_BIN; i < MAX_ANGLE_POST_BIN - 2; i++)
     {
       // calculate local derivative
       d2cosAx[i] = (dcosAx[i+1] + dcosAx[i]) / 2.0;
       d2angM1[i] = (dangM1[i+1] - dangM1[i]);
       d2angM2[i] = (dangM2[i+1] - dangM2[i]);

       // cout << dAPH1[i] << " ";
     }


     for (int i = MIN_ANGLE_POST_BIN; i < MAX_ANGLE_POST_BIN-2; i++)
     {

       // calculate backward and forward derivatives
       dPrev    = (cosAx[i]- cosAx[i-1]) / 2.0;
       dPrev1 = (angM1[i] - angM1[i-1]) / dPrev;
       dPrev2 = (angM2[i] - angM2[i-1]) / dPrev;

       dNext    = (cosAx[i+2]- cosAx[i+1]) / 2.0;
       dNext1 = (angM1[i+2] - angM1[i+1]) / dNext;
       dNext2 = (angM2[i+2] - angM2[i+1]) / dNext;

       if ( (dPrev1 < 0) && (dNext1 < 0) )
       {
         // find maximum and bin
         if( (dangM1[i] < dMinangM1) && (i >= maxBinangM1) )
         {
           dMinangM1 = dangM1[i];
           dMinBinangM1 = i;
           dFound1 = true;
         }
       }

       if ( (dPrev2 < 0) && (dNext2 < 0) )
       {
         if( (dangM2[i] < dMinangM2) && (i >= maxBinangM2) )
         {
           dMinangM2 = dangM2[i];
           dMinBinangM2 = i;
           dFound2 = true;
         }
       }

     }
     // cout << phAx[b] << " " << dMinangM1 << " " << dMinangM2 << endl;
     //
     if (dFound1)
     {
       int iterEdge1 = dMinBinangM1+1;

       // place the edge at the bin before the crossing
       while((d2angM1[iterEdge1] < 0) && (iterEdge1 < N_BINS_RATIO-2))
       {
         iterEdge1++;
       }
       while((d2angM1[iterEdge1] > 0) && (iterEdge1 > 0))
       {
         iterEdge1--;
       }

      //  cout << projMass1->GetBinCenter(iterEdge1) << endl;


       double slope1 = (d2angM1[iterEdge1+1] - d2angM1[iterEdge1])/(d2cosAx[iterEdge1+1] - d2cosAx[iterEdge1]);

       double crossPoint1 = d2cosAx[iterEdge1] - d2angM1[iterEdge1]/slope1;

       chargeLength1[b] = crossPoint1;

       // cout << slope1 << endl;
     }
     else {chargeLength1[b] = chargeLength1[b-1];}

     if (dFound2)
     {
       int iterEdge2 = dMinBinangM2+1;

       // place the edge at the bin before the crossing
       while((d2angM2[iterEdge2] < 0) && (iterEdge2 < N_BINS_RATIO-2))
       {
         iterEdge2++;
       }
       while((d2angM2[iterEdge2] > 0) && (iterEdge2 > 0))
       {
         iterEdge2--;
       }


       double slope2 = (d2angM2[iterEdge2+1] - d2angM2[iterEdge2])/(d2cosAx[iterEdge2+1] - d2cosAx[iterEdge2]);

       double crossPoint2 = d2cosAx[iterEdge2] - d2angM2[iterEdge2]/slope2;

       chargeLength2[b] = crossPoint2;
     }
     else {chargeLength2[b] = chargeLength2[b-1];}


     // cout << phAx[b] << " " << chargeLength1[b] << " "<< chargeLength2[b] << endl;

     if (projMass1->GetEntries() < MIN_ENTRIES_PROJ)
     {
       chargeLength1[b] = 0;
     }
     if (projMass2->GetEntries() < MIN_ENTRIES_PROJ)
     {
       chargeLength2[b] = 0;
     }
     // cout << b << ": " << chargeLength1[b] << " " << chargeLength2[b] << endl;

   }



   /*
    ___              _             _     _
   | _ )_ _ ___ __ _| |___ __  ___(_)_ _| |_ ___
   | _ \ '_/ -_) _` | / / '_ \/ _ \ | ' \  _(_-<
   |___/_| \___\__,_|_\_\ .__/\___/_|_||_\__/__/
                       |_|
   */

    cout << "beginning breakpoint analysis for mass" << endl;


     // search parameters
     int brMinA1 = MAX_MASS_BR_BIN;
     int brMinA2 = MAX_MASS_BR_BIN;
     int brMaxA1 = MIN_MASS_BR_BIN;
     int brMaxA2 = MIN_MASS_BR_BIN;

     double minBreak1 = 1;
     double maxBreak1 = -1;

     cout << "searching from " << MIN_MASS_BR_BIN << " to " << MAX_MASS_BR_BIN << ", and stopping at " << MID_MASS_BIN;
     cout <<   "(" <<  massAx[MID_MASS_BIN] <<  ")" << endl;
     // find the breakpoints
     // cout << "1" << endl;
     for(int b = MIN_MASS_BR_BIN+1; b < MAX_MASS_BR_BIN; b++)
     {
       // cout << "1 " << b << endl;
       if((chargeLength1[b+1] - chargeLength1[b] >= BREAK_POINT_MASS) & (b + 1 < MID_MASS_BIN))
       {
        maxBreak1 = chargeLength1[b+1] - chargeLength1[b];
        brMaxA1 = b+1;
        // cout << brMaxA1 << " " << maxBreak1 << endl;
       }
       if((chargeLength1[b] - chargeLength1[b-1] <= -BREAK_POINT_MASS) & (b - 1 > MID_MASS_BIN))
       {
        minBreak1 = chargeLength1[b] - chargeLength1[b-1] ;
        brMinA1 = b-1;
       }
     }

    double minBreak2 = 1;
    double maxBreak2 = -1;

    // cout << "2" << endl;
    for(int b = MIN_MASS_BR_BIN+1; b < MAX_MASS_BR_BIN; b++)
    {
      // cout << "1 " << b << endl;
      if((chargeLength2[b+1] - chargeLength2[b] >= BREAK_POINT_MASS) & (b + 1 < MID_MASS_BIN))
      {
       maxBreak2 = chargeLength2[b+1] - chargeLength2[b];
       brMaxA2 = b+1;
      }
      if((chargeLength2[b] - chargeLength2[b-1] <= -BREAK_POINT_MASS) & (b - 1 > MID_MASS_BIN))
      {
       minBreak2 = chargeLength2[b] - chargeLength2[b-1] ;
       brMinA2 = b-1;
      }
    }

    // cout << brMinA1 << " " << brMaxA1 << endl;

    cout << "Breakpoints for mass anode 1: " << massAx[brMaxA1] << " " << massAx[brMinA1] << endl;
    cout << "Breakpoints for mass anode 2: " << massAx[brMaxA2] << " " << massAx[brMinA2] << endl;

    // // get rid of points past the breakpoints
    for (int b=0; b < N_BINS_MASS_TH; b++)
    {
      if(b <= brMaxA1) chargeLength1[b] = chargeLength1[brMaxA1 + 2];
      else if(b >= brMinA1) chargeLength1[b] = chargeLength1[brMinA1 - 2];

      if(b <= brMaxA2) chargeLength2[b] = chargeLength2[brMaxA2 + 2];
      else if(b >= brMinA2) chargeLength2[b] = chargeLength2[brMinA2 - 2];
    }


   // store final results
   g_AngMass1 = new TGraph(N_BINS_MASS_TH, massAx, chargeLength1);
   g_AngMass2 = new TGraph(N_BINS_MASS_TH, massAx, chargeLength2);

   f_AngMass1 = new TF1("f_AngMass1", "[0] + [1]*x + [2]*x^2", MIN_MASS, MAX_MASS);
   f_AngMass2 = new TF1("f_AngMass2", "[0] + [1]*x + [2]*x^2", MIN_MASS, MAX_MASS);

   g_AngMass1->Fit("f_AngMass1", "Q R", "", massAx[brMaxA1 + 4], massAx[brMinA1 - 4]);
   g_AngMass2->Fit("f_AngMass2", "Q R", "", massAx[brMaxA2 + 4], massAx[brMinA2 - 4]);

   cout << "mass correction angle: " << f_AngMass1->GetParameter(0) << " and " << f_AngMass2->GetParameter(0) << endl;


  fragDiagnostics->cd();
  cd_recursion->cd();

  TGraph* g_xAngMass1 = new TGraph(N_BINS_MASS_TH, chargeLength1, massAx);
  g_xAngMass1->SetName("g_xAngMass1");
  TGraph* g_xAngMass2 = new TGraph(N_BINS_MASS_TH, chargeLength2, massAx);
  g_xAngMass2->SetName("g_xAngMass2");

  // canvas with angle results
  TCanvas* c_angleMass = new TCanvas("c_angleMass_" + TString(to_string(iterationPost)), "c_angleMass", 600, 800);
  c_angleMass->Divide(1,3);

  c_angleMass->cd(1);
  h2_massAngle1->Draw();
  h2_massAngle1->SetOption("COLZ");
  g_xAngMass1->Draw("SAME");
  g_xAngMass1->SetLineColor(kRed);
  g_xAngMass1->SetLineWidth(2);

  c_angleMass->cd(2);
  h2_massAngle2->Draw();
  h2_massAngle2->SetOption("COLZ");
  g_xAngMass2->Draw("SAME");
  g_xAngMass2->SetLineColor(kRed);
  g_xAngMass2->SetLineWidth(2);


  c_angleMass->cd(3);
  g_AngMass1->SetLineColor(kBlack);
  g_AngMass2->SetLineColor(kRed);
  // f_AngMass1->SetLineColor(kBlack);
  // f_AngMass2->SetLineColor(kBlack);
  // f_AngMass1->SetLineStyle(kDashed);
  // f_AngMass2->SetLineStyle(kDashed);

  g_AngMass1->Draw();
  g_AngMass2->Draw("SAME");
  // f_AngMass1->Draw("SAME");
  // f_AngMass2->Draw("SAME");


  c_angleMass->Write();

  //
  // h3_anodeMassGrid1->Write();
  // h3_anodeMassGrid2->Write();



}
