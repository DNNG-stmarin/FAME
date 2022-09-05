//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 23 12:11:00 2022 by ROOT version 6.24/06
// from TTree et/event tree from waveform analysis
// found on file: event12520.root
//////////////////////////////////////////////////////////

#ifndef fragFiss_h
#define fragFiss_h

#include "InfoSystem.h"

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>

// Header file for the classes stored in the TTree if any.

class fragFiss {

/*
 ___                       _
| _ \_ _ ___  __ ___ _____(_)_ _  __ _
|  _/ '_/ _ \/ _/ -_|_-<_-< | ' \/ _` |
|_| |_| \___/\__\___/__/__/_|_||_\__, |
                                 |___/

*/
public :
   TChain*         eventChain;
   TFile*          eventFile;
   Long64_t        nentries;

   int             numFiles;
   Long64_t*       nentriesTree;

   TTree*          fragTree;
   // TTree*          rfragTree;
   TFile*          fragFile;
   TFile*          postFragFile;
   TFile*          fragDiagnostics;

   InfoSystem*     infoSystem;

   /*

     ___
   / __|___ _ __  _ __  __ _ _ _ ___
  | (__/ _ \ '  \| '_ \/ _` | '_/ -_)
   \___\___/_|_|_| .__/\__,_|_| \___|
               |_|
  */

   TGraph*         g_sawtooth;
   TGraph*         g_bindErg;
   TGraph*         g_phd;
   TGraph*         g_fpy;
   TGraph*         g_tke;
   TGraph*         g_sigtke;

   TGraph2D*         g_neutATKE;
   TGraph2D*         g_deltATKE;

   TGraph*         g_slopeNu;
   TGraph*         g_interNu;

   Int_t           fCurrent; //!current Tree number in a TChain
   TString         inputTreeName = "et";
   TString         eventFilename = "default";
   TString         extEventfile = ".root";

/*
  ___                    _   _               ___     _
 / __|___ _ _ _ _ ___ __| |_(_)___ _ _  ___ | _ \_ _(_)___ _ _
| (__/ _ \ '_| '_/ -_) _|  _| / _ \ ' \(_-< |  _/ '_| / _ \ '_|
 \___\___/_| |_| \___\__|\__|_\___/_||_/__/ |_| |_| |_\___/_|

*/


   // angular analysis
   TGraph* g_Ang1;
   TGraph* g_Ang2;
   TGraph* g_AngMass1;
   TGraph* g_AngMass2;

   TF1* f_Ang1;
   TF1* f_Ang2;
   TF1* f_AngMass1;
   TF1* f_AngMass2;

   // attenuation analysis

   TF1* f_att1;
   TF1* f_att2;

   TF1* f_att1L;
   TF1* f_att1H;
   TF1* f_att2L;
   TF1* f_att2H;

   TF1* f_sepAtt1;
   TF1* f_sepAtt2;

   TF1* f_massAtt1;
   TF1* f_massAtt2;

   TGraph* g_interpMass1;
   TGraph* g_slopeMass1;
   TGraph* g_interpMass2;
   TGraph* g_slopeMass2;

   TGraph2D** g2_massAttSurf1;
   TGraph2D** g2_massAttSurf2;

   // calibration analysis
   TGraph* g_gainMatch;
   TGraph* g_calib1;
   TGraph* g_calib2;

   TGraph* g_massCalib1;
   TGraph* g_massCalib2;

   TGraph* g_phdCalib1;
   TGraph* g_phdCalib2;

   TH2D* h2_massAngle;
   TH1D* h_calibratedAn1;
   TH1D* h_calibratedAn2;
   TH1D* h_rawAn1;
   TH1D* h_rawAn2;
   TH1D* h_corAn1;
   TH1D* h_corAn2;
   TH1D* h_rawMass1;
   TH1D* h_rawMass2;
   TH1D* h_finalMass;
   TH2D* h2_finalTKE;




/*
     ___                _     _____
    |_ _|_ _  _ __ _  _| |_  |_   _| _ ___ ___
     | || ' \| '_ \ || |  _|   | || '_/ -_) -_)
    |___|_||_| .__/\_,_|\__|   |_||_| \___\___|
             |_|
*/

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Double_t        t0t;
   Double_t        ct;
   Double_t        dt;
   Double_t        cph;
   Double_t        tTrig[8];
   Double_t        aph[2];
   Double_t        gph[2];
   Double_t        sumph[2];
   Double_t        sumtTrig[2];
   Int_t           sumtPeak[2];
   Bool_t          ccoinc;
   Short_t         a0wf[1280];
   Short_t         a1wf[1280];
   Short_t         g0wf[1280];
   Short_t         g1wf[1280];
   Short_t         sum0wf[1280];
   Short_t         sum1wf[1280];
   Short_t         cwf[1280];

   // List of branches
   TBranch        *b_t0t;   //!
   TBranch        *b_ct;   //!
   TBranch        *b_dt;   //!
   TBranch        *b_cph;   //!
   TBranch        *b_tTrig;   //!
   TBranch        *b_aph;   //!
   TBranch        *b_gph;   //!
   TBranch        *b_sumph;   //!
   TBranch        *b_sumtTrig;   //!
   TBranch        *b_sumtPeak;   //!
   TBranch        *b_ccoinc;   //!
   TBranch        *b_a0wf;   //!
   TBranch        *b_a1wf;   //!
   TBranch        *b_g0wf;   //!
   TBranch        *b_g1wf;   //!
   TBranch        *b_sum0wf;   //!
   TBranch        *b_sum1wf;   //!
   TBranch        *b_cwf;   //!



   // fragment assigned quantities
   Double_t fT = 0;
   Double_t fAL, fAH;
   Double_t fKEL, fKEH;
   Double_t fThetaL, fThetaH;
   Double_t fEX;
   Double_t fA1, fA2;
   Double_t fAn1, fAn2, fGr1, fGr2;
   Double_t fCat;

   // fragment agnostic quantities
   Double_t fKE1, fKE2;
   Double_t fTheta1, fTheta2;


   // recursion analysis
   // double RECURSION_FRACTION_1;
   // double RECURSION_FRACTION_2;

   double MIN_MASS_RECURSION;
   double MAX_MASS_RECURSION;
   double MIN_ANG_RECURSION;
   double MAX_ANG_RECURSION;



  /*

   ___        _             _____
  | _ \___ __| |_ ___ _ _  |_   _| _ ___ ___
  |  _/ _ (_-<  _/ -_) '_|   | || '_/ -_) -_)
  |_| \___/__/\__\___|_|     |_||_| \___\___|

  */
  Double_t pT;
  Double_t pAL, pAH;
  Double_t pKEL, pKEH;
  Double_t pThetaL, pThetaH;
  Double_t pEX;
  Double_t pAn1, pAn2, pGr1, pGr2;
  Double_t pCat;
  Double_t pKE1, pKE2;
  Double_t pTheta1, pTheta2;
  Double_t pA1, pA2;

  TBranch *b_pT;
  TBranch *b_pAL, *b_pAH;
  TBranch *b_pKEL, *b_pKEH;
  TBranch *b_pThetaL, *b_pThetaH;
  TBranch *b_pEX;
  TBranch *b_pAn1, *b_pAn2, *b_pGr1, *b_pGr2;
  TBranch *b_pCat;
  TBranch *b_pKE1, *b_pKE2;
  TBranch *b_pTheta1, *b_pTheta2;
  TBranch *b_pA1, *b_pA2;


  /*
   __  __     _   _            _
  |  \/  |___| |_| |_  ___  __| |___
  | |\/| / -_)  _| ' \/ _ \/ _` (_-<
  |_|  |_\___|\__|_||_\___/\__,_/__/

  */

   fragFiss(InfoSystem* infoIn, TString fileName);
   virtual ~fragFiss();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init();

   TDirectory*      cd_diagnostics;
   TDirectory*      cd_recursion;

   virtual void     PlotWaves();

   // priori
   virtual void     AngleAnalysis();
   virtual void     ELossCorrection();
   virtual void     GainMatching();
   virtual void     InitFrag();
   virtual void     FillFragTree();

   // posteriori
   virtual void     InitPost(int iterationPost);
   virtual void     PostAngle(int iterationPost);
   virtual void     PostAttenuation(int iterationPost);
   virtual void     PostCalib(int iterationPost);
   virtual void     PostCalibPHD(int iterationPost);
   virtual void     PostFrag(int iterationPost);


   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif
