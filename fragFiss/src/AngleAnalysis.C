#include "fragFiss.h"
#include "constants.h"
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

void fragFiss::AngleAnalysis()
{

   if (eventChain == 0) return;
   // Put this somewhere else
   TH2D* h2_anodeGrid1 = new TH2D("h2_anodeGrid1","h2_anodeGrid1;gph/aph;aph",N_BINS_RATIO,0,ALL_RATIO,N_BINS_APH,0,MAX_APH);
   TH2D* h2_anodeGrid2 = new TH2D("h2_anodeGrid2","h2_anodeGrid2;gph/aph;aph",N_BINS_RATIO,0,ALL_RATIO,N_BINS_APH,0,MAX_APH);

   const int MIN_BIN_RATIO = (int)(MIN_RATIO/ALL_RATIO*N_BINS_RATIO);
   const int MAX_BIN_RATIO = (int)(MAX_RATIO/ALL_RATIO*N_BINS_RATIO);

   const int MIN_BR_1_BIN = (int)(MIN_BR_1/MAX_APH*N_BINS_APH);
   const int MIN_BR_2_BIN = (int)(MIN_BR_2/MAX_APH*N_BINS_APH);
   const int MAX_BR_1_BIN = (int)(MAX_BR_1/MAX_APH*N_BINS_APH);
   const int MAX_BR_2_BIN = (int)(MAX_BR_2/MAX_APH*N_BINS_APH);
   const int MID_BR_1_BIN = (int)(MID_BR_1/MAX_APH*N_BINS_APH);
   const int MID_BR_2_BIN = (int)(MID_BR_2/MAX_APH*N_BINS_APH);

   // cout << MIN_BIN_RATIO << " " << MAX_BIN_RATIO << " " << MIN_BR_1_BIN << " " << MAX_BR_1_BIN << endl;

  /*
         _             _       ___ _ _ _   _
        /_\  _ _  __ _| |___  | __(_) | | | |   ___  ___ _ __
       / _ \| ' \/ _` | / -_) | _|| | | | | |__/ _ \/ _ \ '_ \
      /_/ \_\_||_\__, |_\___| |_| |_|_|_| |____\___/\___/ .__/
              |___/                                  |_|
  */

   Long64_t nbytes = 0, nb = 0;
   cout << "Filling angle histograms" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {

      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = eventChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      if(ccoinc != 1)
      {
        continue;
      }

      aph[0] = (aph[0] - GRID_INEFFICIENCY*(aph[0] + gph[0]))/(1 - GRID_INEFFICIENCY);
      aph[1] = (aph[1] - GRID_INEFFICIENCY*(aph[1] + gph[1]))/(1 - GRID_INEFFICIENCY);


      h2_anodeGrid1->Fill(gph[0]/aph[0], aph[0]);
      h2_anodeGrid2->Fill(gph[1]/aph[1], aph[1]);
   }


  /*
     _             _       ___ _ _ _   _
    /_\  _ _  __ _| |___  | __(_) | | | |   ___  ___ _ __
   / _ \| ' \/ _` | / -_) | _|| | | | | |__/ _ \/ _ \ '_ \
  /_/ \_\_||_\__, |_\___| |_| |_|_|_| |____\___/\___/ .__/
             |___/                                  |_|
  */
   // Determine angle
   double *chargeLength1, *chargeLength2, *phAx;
   chargeLength1 = new double[N_BINS_APH];
   chargeLength2 = new double[N_BINS_APH];
   phAx = new double[N_BINS_APH];


   // analysis for selection
   double *APH1, *APH2, *cosAx;
   APH1 = new double[N_BINS_RATIO];
   APH2 = new double[N_BINS_RATIO];
   cosAx = new double[N_BINS_RATIO];

   double *dAPH1, *dAPH2, *dcosAx;
   double *d2APH1, *d2APH2, *d2cosAx;
   dAPH1 = new double[N_BINS_RATIO - 1];
   dAPH2 = new double[N_BINS_RATIO - 1];
   dcosAx = new double[N_BINS_RATIO - 1];

   d2APH1 = new double[N_BINS_RATIO - 1];
   d2APH2 = new double[N_BINS_RATIO - 1];
   d2cosAx = new double[N_BINS_RATIO - 1];

   for (int b=0; b < N_BINS_APH; b++)
   {
     phAx[b] = 0.5*(h2_anodeGrid1->GetYaxis()->GetBinCenter(b+1) + h2_anodeGrid2->GetYaxis()->GetBinCenter(b+1));
     TH1D* projAPH1 = h2_anodeGrid1->ProjectionX("projAPH1" + TString(b),b+1,b+1);
     TH1D* projAPH2 = h2_anodeGrid2->ProjectionX("projAPH2" + TString(b),b+1,b+1);

     projAPH1->Smooth(2);
     projAPH2->Smooth(2);

     // line conversion

     int maxBinAPH1 = -1;
     int maxBinAPH2 = -1;
     double maxAPH1 = -1;
     double maxAPH2 = -1;

     for (int i = MIN_BIN_RATIO; i < N_BINS_RATIO; i++)
     {
       cosAx[i] = 0.5*(projAPH1->GetBinCenter(i+1) + projAPH2->GetBinCenter(i+1));

       APH1[i]  = projAPH1->GetBinContent(i+1);
       APH2[i]  = projAPH2->GetBinContent(i+1);

       // find maximum and bin
       if(APH1[i] > maxAPH1)
       {
         maxAPH1 = APH1[i];
         maxBinAPH1 = i;
       }

       if(APH2[i] > maxAPH2)
       {
         maxAPH2 = APH2[i];
         maxBinAPH2 = i;
       }

     }

     // derivative filter

     int dMinBinAPH1 = -1;
     int dMinBinAPH2 = -1;
     double dMinAPH1 = -1;
     double dMinAPH2 = -1;

     double dPrev = 0;
     double dPrev1 = 0;
     double dPrev2 = 0;
     double dNext = 0;
     double dNext1 = 0;
     double dNext2 = 0;

     bool dFound1 = false;
     bool dFound2 = false;

     // derivative filter
     for(int i = MIN_BIN_RATIO; i < N_BINS_RATIO - 1; i++)
     {
       // calculate local derivative
       dcosAx[i] = (cosAx[i+1] + cosAx[i])/2.0;
       dAPH1[i] = (APH1[i+1] - APH1[i]) / dcosAx[i];
       dAPH2[i] = (APH2[i+1] - APH2[i]) / dcosAx[i];

       // cout << dAPH1[i] << " ";
     }
     // cout << endl;

     // second derivative filter
     for(int i = MIN_BIN_RATIO; i < N_BINS_RATIO - 2; i++)
     {
       // calculate local derivative
       d2cosAx[i] = (dcosAx[i+1] + dcosAx[i])/2.0;
       d2APH1[i] = (dAPH1[i+1] - dAPH1[i]);
       d2APH2[i] = (dAPH2[i+1] - dAPH2[i]);

       // cout << d2APH1[i] << " ";
     }
     // cout << endl;


     for (int i = MIN_BIN_RATIO; i < N_BINS_RATIO - 2; i++)
     {
       // calculate backward and forward derivatives
       dPrev    = (cosAx[i]- cosAx[i-1]) / 2.0;
       dPrev1 = (APH1[i] - APH1[i-1]) / dPrev;
       dPrev2 = (APH2[i] - APH2[i-1]) / dPrev;

       dNext    = (cosAx[i+2]- cosAx[i+1]) / 2.0;
       dNext1 = (APH1[i+2] - APH1[i+1]) / dNext;
       dNext2 = (APH2[i+2] - APH2[i+1]) / dNext;

       if ( (dPrev1 < 0) && (dNext1 < 0) )
       {
         // find maximum and bin
         if( (dAPH1[i] < dMinAPH1) && (i >= maxBinAPH1) )
         {
           dMinAPH1 = dAPH1[i];
           dMinBinAPH1 = i;
           dFound1 = true;
         }
       }

       if ( (dPrev2 < 0) && (dNext2 < 0) )
       {
         if( (dAPH2[i] < dMinAPH2) && (i >= maxBinAPH2) )
         {
           dMinAPH2 = dAPH2[i];
           dMinBinAPH2 = i;
           dFound2 = true;
         }
       }

     }
     // cout << phAx[b] << " " << dMinAPH1 << " " << dMinAPH2 << endl;
     // assign it to the neighbors?
     if (dFound1)
     {
       int iterEdge1 = dMinBinAPH1+1;
       //cout << phAx[b] << " "  << projAPH1->GetBinCenter(iterEdge1) << endl;

       // place the edge at the bin before the crossing
       while((d2APH1[iterEdge1] < 0) && (iterEdge1 < N_BINS_RATIO-2))
       {
         iterEdge1++;
       }
       while((d2APH1[iterEdge1] > 0) && (iterEdge1 > 0))
       {
         iterEdge1--;
       }


       double slope1 = (d2APH1[iterEdge1+1] - d2APH1[iterEdge1])/(d2cosAx[iterEdge1 + 1] - d2cosAx[iterEdge1] );

       double crossPoint1 = d2cosAx[iterEdge1] - d2APH1[iterEdge1]/slope1;

       chargeLength1[b] = crossPoint1;

       // cout << phAx[b] << " " << crossPoint1 << endl;

     }
     else {chargeLength1[b] = chargeLength1[b-1];}

     if (dFound2)
     {

       int iterEdge2 = dMinBinAPH2+1;

       // place the edge at the bin before the crossing
       while((d2APH2[iterEdge2] < 0) && (iterEdge2 < N_BINS_RATIO-2))
       {
         iterEdge2++;
       }
       while((d2APH2[iterEdge2] > 0) && (iterEdge2 > 0))
       {
         iterEdge2--;
       }


       double slope2 = (d2APH2[iterEdge2+1] - d2APH2[iterEdge2])/(d2cosAx[iterEdge2+1] - d2cosAx[iterEdge2] );

       double crossPoint2 = d2cosAx[iterEdge2] - d2APH2[iterEdge2]/slope2;

       chargeLength2[b] = crossPoint2;

     }
     else {chargeLength2[b] = chargeLength2[b-1];}

     // chargeLength1[b] = projAPH1->GetBinCenter(dMinBinAPH1+1);
     // chargeLength2[b] = projAPH2->GetBinCenter(dMinBinAPH2+1);

     // cout << phAx[b] << " " << chargeLength1[b] << " "<< chargeLength2[b] << endl;

     if (projAPH1->GetEntries() < MIN_ENTRIES_PROJ)
     {
       chargeLength1[b] = 0;
     }
     if (projAPH2->GetEntries() < MIN_ENTRIES_PROJ)
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



    int brMinA1 = MAX_BR_1_BIN;
    int brMinA2 = MIN_BR_1_BIN;
    int brMaxA1 = MAX_BR_2_BIN;
    int brMaxA2 = MIN_BR_2_BIN;

    double minBreak1 = 1;
    double maxBreak1 = -1;
    // find the breakpoints
    for(int b = MIN_BR_1_BIN+1; b < MAX_BR_1_BIN; b++)
    {
      // cout << "1 " << b << endl;
      if((chargeLength1[b+1] - chargeLength1[b] >= BREAK_POINT_RATIO) & (b + 1 < MID_BR_1_BIN))
      {
       maxBreak1 = chargeLength1[b+1] - chargeLength1[b];
       brMaxA1 = b+1;
      }
      if((chargeLength1[b] - chargeLength1[b-1] <= -BREAK_POINT_RATIO) & (b - 1 > MID_BR_1_BIN))
      {
       minBreak1 = chargeLength1[b] - chargeLength1[b-1] ;
       brMinA1 = b-1;
      }
    }

   double minBreak2 = 1;
   double maxBreak2 = -1;

   // find the breakpoints
   for(int b = MIN_BR_2_BIN+1; b < MAX_BR_2_BIN; b++)
   {
     // cout <<  "2 " << b << endl;
     if((chargeLength2[b+1] - chargeLength2[b] >= BREAK_POINT_RATIO) & (b + 1 < MID_BR_2_BIN))
     {
      maxBreak2 = chargeLength2[b+1] - chargeLength2[b];
      brMaxA2 = b+1;
     }
     if((chargeLength2[b] - chargeLength2[b-1] <= -BREAK_POINT_RATIO) & (b -1 > MID_BR_2_BIN))
     {
      minBreak2 = chargeLength2[b] - chargeLength2[b-1] ;
      brMinA2 = b-1;
     }
   }

   // cout << brMinA1 << " " << brMaxA1 << endl;

   cout << "Breakpoints for anode 1: " << phAx[brMaxA1] << " " << phAx[brMinA1] << endl;
   cout << "Breakpoints for anode 2: " << phAx[brMaxA2] << " " <<  phAx[brMinA2] << endl;


   // get rid of points past the breakpoints
   for (int b=0; b < N_BINS_APH; b++)
   {
     if(b <= brMaxA1 - 1) chargeLength1[b] = chargeLength1[brMaxA1 + 2];
     else if(b >= brMinA1 + 1) chargeLength1[b] = chargeLength1[brMinA1 - 2];

     if(b <= brMaxA2 - 1 ) chargeLength2[b] = chargeLength2[brMaxA2 + 2];
     else if(b >= brMinA2  + 1) chargeLength2[b] = chargeLength2[brMinA2 - 2];
   }

   // store final results
   g_Ang1 = new TGraph(N_BINS_APH, phAx, chargeLength1);
   g_Ang2 = new TGraph(N_BINS_APH, phAx, chargeLength2);


   // fit functions
   f_Ang1 = new TF1("f_Ang1", "[0] + [1]*x^2 + [2]*x", 0, MAX_APH);
   f_Ang2 = new TF1("f_Ang2", "[0] + [1]*x^2 + [2]*x", 0, MAX_APH);

   g_Ang1->Fit(f_Ang1, "Q R", "", phAx[brMaxA1+2], phAx[brMinA1-2]);
   g_Ang2->Fit(f_Ang2, "Q R", "", phAx[brMaxA2+2], phAx[brMinA2-2]);



   // f_Ang1->SetParameters(chargeLength1[brMinA1 + 1], (chargeLength1[brMaxA1 - 1] - chargeLength1[brMinA1 + 1])


    //    ___                            _     ___
    // | __| _ __ _ __ _ _ __  ___ _ _| |_  / __| ___ _ __
    // | _| '_/ _` / _` | '  \/ -_) ' \  _| \__ \/ -_) '_ \
    // |_||_| \__,_\__, |_|_|_\___|_||_\__| |___/\___| .__/
    //            |___/                             |_|

   bool fragSepAng = false;

   TGraph *g_centAng1H, *g_centAng1L, *g_centAng2H, *g_centAng2L;

   if(fragSepAng)
   {
     cout << "beginning fragment separation analysis" << endl;
     // set up the fit functions
     TF1* gaussFit = new TF1("gaussFit_ang", "[0] * e^(-((x - [1])^2)/(2*[2]^2))", 0, MAX_APH);
     // yield 1
     TF1* f_gaussYield1 = new TF1("f_gaussYield1_ang", "gaussFit_ang + gaussFit_ang", 0, MAX_APH);
     TF1* f_gaussYield2 = new TF1("f_gaussYield2_ang", "gaussFit_ang + gaussFit_ang", 0, MAX_APH);


     // create arrays to store data
     double** centroids1;
     double** centroids2;

     double* angAxC;
     double* invAngC;

     centroids1 = new double*[2];
     centroids2 = new double*[2];

     angAxC = new double [N_BINS_RATIO];
     invAngC = new double [N_BINS_RATIO];

     for(int i = 0; i < 2; i++)
     {
       centroids1[i] = new double [N_BINS_RATIO];
       centroids2[i] = new double [N_BINS_RATIO];
     }

     int indC = 0;
     // now loop through the allowable values of angles
     for(int c = MIN_BIN_RATIO; c < MAX_BIN_RATIO; c++)
     {

       double angle = h2_anodeGrid1->GetXaxis()->GetBinCenter(c+1); // store the results
       angAxC[indC] = angle;
       invAngC[indC] = 1.0/angle;

       TH1D* h_sliceTKEatt1 = h2_anodeGrid1->ProjectionY("h_sliceTKEatt1_" + TString(c), c, c+2);
       TH1D* h_sliceTKEatt2 = h2_anodeGrid2->ProjectionY("h_sliceTKEatt2_" + TString(c), c, c+2);

       double ampGuess, meanGuess, sigGuess;
       ampGuess = h_sliceTKEatt1->GetMaximum();
       meanGuess = h_sliceTKEatt1->GetMean();
       sigGuess = SIG_ANODE;
       // cout << ampGuess << " " << meanGuess << sigGuess << " " << endl;
       f_gaussYield1->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
       f_gaussYield1->SetParLimits(1, MIN_ANODE2, meanGuess);
       f_gaussYield1->SetParLimits(4, meanGuess, MAX_APH);
       h2_anodeGrid1->Fit(f_gaussYield1, "N Q", "", 0, MAX_APH);

       ampGuess = h_sliceTKEatt2->GetMaximum();
       meanGuess = h_sliceTKEatt2->GetMean();
       sigGuess = SIG_ANODE;
       // cout << ampGuess << " " << meanGuess << " " << sigGuess << endl;
       f_gaussYield2->SetParameters(ampGuess*0.8, meanGuess*0.8, sigGuess, ampGuess, meanGuess*1.2, sigGuess);
       f_gaussYield2->SetParLimits(1, MIN_ANODE2, meanGuess);
       f_gaussYield2->SetParLimits(4, meanGuess, MAX_APH);
       h2_anodeGrid2->Fit(f_gaussYield2, "N Q", "", 0, MAX_APH);


       centroids1[0][indC] = f_gaussYield1->GetParameter(1);
       centroids1[1][indC] = f_gaussYield1->GetParameter(4);

       centroids2[0][indC] = f_gaussYield2->GetParameter(1);
       centroids2[1][indC] = f_gaussYield2->GetParameter(4);

       cout << centroids1[0][indC] << ", " << centroids1[1][indC] << " | " << centroids2[0][indC] << ", " << centroids2[1][indC] << endl;

       indC++;

     }
     // create the graphs
     g_centAng1H = new TGraph(N_BINS_RATIO, angAxC, centroids1[0]);
     g_centAng1L = new TGraph(N_BINS_RATIO, angAxC, centroids1[1]);

     g_centAng2H = new TGraph(N_BINS_RATIO, angAxC, centroids2[0]);
     g_centAng2L = new TGraph(N_BINS_RATIO, angAxC, centroids2[1]);

     g_centAng1H->SetName("g_cent1H");
     g_centAng1L->SetName("g_cent1L");
     g_centAng2H->SetName("g_cent2H");
     g_centAng2L->SetName("g_cent2L");


     TGraph* g_invAng1H = new TGraph(N_BINS_RATIO, invAngC, centroids1[0]);
     TGraph* g_invAng1L = new TGraph(N_BINS_RATIO, invAngC, centroids1[1]);

     TGraph* g_invAng2H = new TGraph(N_BINS_RATIO, invAngC, centroids2[0]);
     TGraph* g_invAng2L = new TGraph(N_BINS_RATIO, invAngC, centroids2[1]);

     g_invAng1H->SetName("g_inv1H");
     g_invAng1L->SetName("g_inv1L");
     g_invAng2H->SetName("g_inv2H");
     g_invAng2L->SetName("g_inv2L");
   }



    /*
     ___ _     _
    | _ \ |___| |_
    |  _/ / _ \  _|
    |_| |_\___/\__|

    */

   TGraph* g_xAng1 = new TGraph(N_BINS_APH, chargeLength1, phAx);
   g_xAng1->SetName("g_xAng1");
   TGraph* g_xAng2 = new TGraph(N_BINS_APH, chargeLength2, phAx);
   g_xAng2->SetName("g_xAng2");

   // canvas with angle results
   TCanvas* c_angleAn = new TCanvas("c_angle1", "c_angle1", 600, 800);
   c_angleAn->Divide(1,3);

   c_angleAn->cd(1);
   h2_anodeGrid1->Draw();
   h2_anodeGrid1->SetOption("COLZ");
   g_xAng1->Draw("SAME");
   g_xAng1->SetLineColor(kRed);
   g_xAng1->SetLineWidth(2);

   if(fragSepAng)
   {
     // fragment lines
     g_centAng1H->SetLineColor(kGreen);
     g_centAng1L->SetLineColor(kGreen);
     g_centAng1H->SetLineWidth(2);
     g_centAng1L->SetLineWidth(2);
     g_centAng1H->Draw("SAME");
     g_centAng1L->Draw("SAME");
   }


   c_angleAn->cd(2);
   h2_anodeGrid2->Draw();
   h2_anodeGrid2->SetOption("COLZ");
   g_xAng2->Draw("SAME");
   g_xAng2->SetLineColor(kRed);
   g_xAng2->SetLineWidth(2);

   if(fragSepAng)
   {
     // fragment lines
     g_centAng2H->SetLineColor(kGreen);
     g_centAng2L->SetLineColor(kGreen);
     g_centAng2H->SetLineWidth(2);
     g_centAng2L->SetLineWidth(2);
     g_centAng2H->Draw("SAME");
     g_centAng2L->Draw("SAME");
   }

   c_angleAn->cd(3);
   g_Ang1->SetLineColor(kBlack);
   g_Ang2->SetLineColor(kRed);
   f_Ang1->SetLineColor(kBlack);
   f_Ang2->SetLineColor(kBlack);
   f_Ang1->SetLineStyle(kDashed);
   f_Ang2->SetLineStyle(kDashed);

   g_Ang1->Draw();
   g_Ang2->Draw("SAME");
   f_Ang1->Draw("SAME");
   f_Ang2->Draw("SAME");
   c_angleAn->Write();



   // cout << "Writing" << endl;
   fragDiagnostics->cd();
   cd_diagnostics->cd();
   c_angleAn->Write();

}
