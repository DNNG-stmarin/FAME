#include "fragFiss.h"
#include "constants.h"

#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>


void fragFiss::PostFrag(int iterationPost)
{
  postFragFile->cd();
  // if(iterationPost == infoSystem->NUM_RECURSIONS - 1) postFragFile->cd();

  TH1D* h1_iteratedMass = new TH1D("h1_iteratedMass_" +  (TString)to_string(iterationPost), "h1_iteratedMass; amu; counts", A_TOT, 0, A_TOT);
  TH1D* h1_iteratedTKE = new TH1D("h1_iteratedTKE_" + (TString)to_string(iterationPost), "h1_iteratedTKE; TKE (MeV); counts", 2*N_BINS_APH, 0, 2*MAX_KE);
  TH2D* h2_iteratedMassTKE = new TH2D("h2_iteratedMassTKE_" + (TString)to_string(iterationPost), "h2_iteratedMassTKE; amu; TKE (MeV); counts",A_TOT, 0, A_TOT,2*N_BINS_APH, 0, 2*MAX_KE );


  TH1D* h1_iteratedAngleRes = new TH1D("h1_iteratedAngleRes_" +  (TString)to_string(iterationPost), "h1_iteratedAngleRes; differenceAngle; counts", 500, -1.0, 1.0);

  TTree* cloneFrag = new TTree("cloneTree_"+ TString(to_string(iterationPost)), "Clone Tree of Fragments");
  cloneFrag->SetFileNumber(0);
  // cloneFrag->SetMaxTreeSize(20000000000LL);

  cloneFrag->Branch("fT", &fT, "fT/D");
  cloneFrag->Branch("fAL", &fAL, "fAL/D");
  cloneFrag->Branch("fAH", &fAH, "fAH/D");
  cloneFrag->Branch("fKEL", &fKEL, "fKEL/D");
  cloneFrag->Branch("fKEH", &fKEH, "fKEH/D");
  cloneFrag->Branch("fThetaL", &fThetaL, "fThetaL/D");
  cloneFrag->Branch("fThetaH", &fThetaH, "fThetaH/D");
  cloneFrag->Branch("fEX", &fEX, "fEX/D");

  cloneFrag->Branch("fAn1", &fAn1, "fAn1/D");
  cloneFrag->Branch("fAn2", &fAn2, "fAn2/D");
  cloneFrag->Branch("fGr1", &fGr1, "fGr1/D");
  cloneFrag->Branch("fGr2", &fGr2, "fGr2/D");
  cloneFrag->Branch("fCat", &fCat, "fCat/D");

  cloneFrag->Branch("fKE1", &fKE1, "fKE1/D");
  cloneFrag->Branch("fKE2", &fKE2, "fKE2/D");
  cloneFrag->Branch("fTheta1", &fTheta1, "fTheta1/D");
  cloneFrag->Branch("fTheta2", &fTheta2, "fTheta2/D");
  cloneFrag->Branch("fA1", &fA1, "fA1/D");
  cloneFrag->Branch("fA2", &fA2, "fA2/D");

  double preA1, preA2, preA1b, preA2b, postA1, postA2, preE1, preE2, postE1, postE2;
  double fPH1, fPH2;

  // InitFrag();

  Long64_t nentries = fragTree->GetEntries();
  cout << "analyzing " << nentries << " entries." << endl;
  long int nFiss = 0;
  double pThetaAv;

  for (Long64_t jentry =0; jentry < nentries; jentry++)
  {
    Long64_t ientry = fragTree->LoadTree(jentry);
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


    fT = pT;
    fAn1 = pAn1;
    fAn2 = pAn2;
    fGr1 = pGr1;
    fGr2 = pGr2;
    fCat = pCat;

     // recursive corrections
     pAn1 = (pAn1 - GRID_INEFFICIENCY*(pAn1 + pGr1))/(1 - GRID_INEFFICIENCY);
     pAn2 = (pAn2 - GRID_INEFFICIENCY*(pAn2 + pGr2))/(1 - GRID_INEFFICIENCY);

     pTheta1 /= g_AngMass1->Eval(pA1);
     pTheta2 /= g_AngMass2->Eval(pA2);

     h1_iteratedAngleRes->Fill(pTheta1 - pTheta2);

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

     pKE1 = g_phdCalib1->Eval(pKE1) - g_phd->Eval(pA1);
     pKE2 = g_phdCalib2->Eval(pKE2) - g_phd->Eval(pA2);

     // save recursively corrected branches
     fTheta1 = pTheta1;
     fTheta2 = pTheta2;

     fKE1 = pKE1;
     fKE2 = pKE2;

     fPH1 = pAn1;
     fPH2 = pAn2;

     // mass calculation
     preA1 = A_TOT * fKE2 / (fKE1 + fKE2);
     preA2 = A_TOT * fKE1 / (fKE1 + fKE2);
     postA1 = preA1;
     postA2 = preA2;

     // h_rawMass1->Fill(preA1);
     // h_rawMass2->Fill(preA2);

     preE1 = fKE1;
     postE1 = fKE1;
     preE2 = fKE2;
     postE2 = fKE2;

     int iterCounter = 0;
     // cout << endl;
     while(iterCounter < MAX_CONV_ITER)
     {

       // cout << preE[0] << " " << preE[1] << postE[0] << " " << postE[1] << endl;

       preA1b = preA1;
       preA2b = preA2;

       if(TKE_SAWTOOTH)
       {
         postA1 = preA1 - g_neutATKE->Interpolate(preA1, preE1+preE2);
         postA2 = preA2 - g_neutATKE->Interpolate(preA2, preE1+preE2);
       }
       else if(INTERP_SLOPE_SAWTOOTH)
       {
         postA1 = preA1 - ((preE1 + preE2) - g_interNu->Eval(preA1))*g_slopeNu->Eval(preA1);
         postA2 = preA2 - ((preE1 + preE2) - g_interNu->Eval(preA2))*g_slopeNu->Eval(preA2);
       }
       else
       {
         postA1 = preA1 - g_sawtooth->Eval(preA1);
         postA2 = preA2 - g_sawtooth->Eval(preA2);
       }

       // Correct for PHD
       postE1 = fKE1 + g_phd->Eval(postA1);
       postE2 = fKE2 + g_phd->Eval(postA2);

       double B = preA2 * postA1 / (preA1 * postA2);

       preA1 = A_TOT * postE2 / (postE1 / B + postE2);
       preA2 = A_TOT * postE1 / (postE2 * B + postE1);

       preE1 = postE1 * preA1 / postA1;
       preE2 = postE2 * preA2 / postA2;


       if (abs(preA1 - preA1b) < CONVERGEANCE_CONST &&
           abs(preA2 - preA2b) < CONVERGEANCE_CONST)
       {
         break;
       }
       iterCounter++;
     }

     // save the corrected anode kinetic energies
     fKE1 = preE1;
     fKE2 = preE2;

     fA1 = preA1;
     fA2 = preA2;

     // assign to light and heavy
     if (preA1 > preA2)
     {
       fAH = preA1;
       fKEH = preE1;
       fThetaH = fTheta1;

       fAL = preA2;
       fKEL = preE2;
       fThetaL = fTheta2;

     }
     else
     {
       fAL = preA1;
       fKEL = preE1;
       fThetaL = fTheta1;

       fAH = preA2;
       fKEH = preE2;
       fThetaH = fTheta2;
     }

     if (iterCounter == MAX_CONV_ITER - 1) {cout << "Reached max. iterations"<< endl;}

     // if (jentry % 1000 == 0) cout << fAH << " " << fAL << endl;


     // extract excitation energy
     fEX = g_bindErg->Eval(fAL) - (fKEL + fKEH);

     // cout << fKEL + fKEH << endl;


     // h2_massAngle->Fill(fThetaL, fAL);
     // h2_massAngle->Fill(fThetaH, fAH);


     // if event passes the test, fill the tree
     if( (fPH1 > MIN_ANODE1) && (fPH2 > MIN_ANODE2) && (fTheta1 > MIN_ANG1) && (fTheta2 > MIN_ANG2)
          && (fTheta1 < MAX_ANG1) && (fTheta2 < MAX_ANG2))
     {
       cloneFrag->Fill();
       nFiss++;
     }

     if(fAL > MIN_MASS_ANALYSIS & fAH < MAX_MASS_ANALYSIS
        & fTheta1 > MIN_ANG_ANALYSIS & fTheta1 < MAX_ANG_ANALYSIS
        & fTheta2 > MIN_ANG_ANALYSIS & fTheta2 < MAX_ANG_ANALYSIS
        )
        {
          h1_iteratedMass->Fill(fAL);
          h1_iteratedMass->Fill(fAH);
          h1_iteratedTKE->Fill(fKEL + fKEH);
          h2_iteratedMassTKE->Fill(fAL, fKEL + fKEH);
          h2_iteratedMassTKE->Fill(fAH, fKEL + fKEH);
        }

     if((jentry%100000 == 0) & (iterationPost == infoSystem->NUM_RECURSIONS - 1)) cout << "Filled " << nFiss << " fissions in " << fT/1e9 <<  " seconds." <<  endl;
  }

  // set the frag tree to the iterated value

  fragTree = (TTree*)cloneFrag->Clone();
  fragTree->SetName("FragmentTree");
  // fragTree->Show(10);
  if(iterationPost == infoSystem->NUM_RECURSIONS - 1)
  {
    // fragTree->SetMaxTreeSize(10000000000LL);
    fragTree->Write();
  }




  double massAx[252] = {0};
  double sigTKE[252] = {0};
  // double massErr[252] = {0};
  // double sigTkeErr[252] = {0};

  fragDiagnostics->cd();
  for(int i = 1; i <= A_TOT; i++)
  {
    TH1D* prTKE = h2_iteratedMassTKE->ProjectionY("tkeSlice_"+TString(to_string(i)),i,i);
    massAx[i-1] = h2_iteratedMassTKE->GetXaxis()->GetBinCenter(i);

    sigTKE[i-1] = prTKE->GetStdDev();
  }
  TGraph* g_sig = new TGraph(252, massAx, sigTKE);
  g_sig->SetName("g_sigIter");




  TCanvas* c_iteratedResults = new TCanvas("c_iteratedResults_" + (TString)to_string(iterationPost),"c_iteratedResults",600,400);
  c_iteratedResults->Divide(3,2);

  c_iteratedResults->cd(1);
  h1_iteratedMass->Draw();
  double scale = h1_iteratedMass->GetMaximum() / g_fpy->Eval(h1_iteratedMass->GetMaximumBin());
  for (int i = 0; i < g_fpy->GetN(); i++) g_fpy->GetY()[i] *= scale;
  g_fpy->Draw("SAME");


  c_iteratedResults->cd(2);
  h1_iteratedTKE->Draw();


  c_iteratedResults->cd(3);
  h2_iteratedMassTKE->ProfileX()->Draw();
  g_tke->Draw("SAME");


  c_iteratedResults->cd(4);
  g_sig->Draw();
  g_sigtke->Draw("SAME");

  c_iteratedResults->cd(5);
  h2_iteratedMassTKE->Draw("COLZ");

  c_iteratedResults->cd(6);
  h1_iteratedAngleRes->Draw("COLZ");


  c_iteratedResults->Write();



}
