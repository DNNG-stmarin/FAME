//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 17 09:51:08 2021 by ROOT version 5.34/38
// from TTree Fiss/Fiss
// found on file: Fiss.root
//////////////////////////////////////////////////////////

#ifndef readFiss_h
#define readFiss_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TString.h>
#include <TMatrixD.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <THStack.h>

#include <QApplication>

#include <iostream>
#include <fstream>
#include <string>

#include "mainwindow.h"
// Header file for the classes stored in the TTree if any.

#include "Constants.h"
#include "myANN.h"

extern QApplication* app; // pointer to application

class readFiss {
public :
   MainWindow*      w; // pointer to GUI

/*
  _____                _      _    _
 |_   _| _ ___ ___    /_\  __| |__| |_ _ ___ ______ ___ ___
   | || '_/ -_) -_)  / _ \/ _` / _` | '_/ -_|_-<_-</ -_|_-<
   |_||_| \___\___| /_/ \_\__,_\__,_|_| \___/__/__/\___/__/
                                                           */
   TChain*           expTree;   //!pointer to the analyzed TTree or TChain
   TChain*           simTree;
   // TTree*           beamTree;

   //neural network
   myANN*           crossTalkANN;
   Double_t         thresholdANN = 0.80;
   bool             ANN_mode = 0;

   std::string      nameExp; // String name of where to find experiment
   std::string      nameSim;
   std::string      nameBeam;
   std::string      nameCoords;
   std::string      nameWrite;

   TFile*           expFile;
   TFile*           simFile;
   TFile*           beamFile;

   int              numExpFiles;
   int              numSimFiles;

   // neural network

   Int_t            fCurrent; //!current Tree number in a TChain

   TFile*           writeFile;
   TDirectory*      cd_basics;
   TDirectory*      cd_individual;
     TDirectory*      cd_LightOutExp;
     TDirectory*      cd_ToFExp;
     TDirectory*      cd_ErgExp;
     TDirectory*      cd_PSDExp;
     TDirectory*      cd_IndivNeutronEnergyLOExp;
     TDirectory*      cd_IndivNeutronLightOutPSDExp;
     TDirectory*      cd_IndivPhotonLightOutPSDExp;
   TDirectory*      cd_simComparison;
   TDirectory*      cd_FAME;
   TDirectory*      cd_correlated;
     TDirectory*      cd_projs;
     TDirectory*      cd_AngleCorr;
   TDirectory*      cd_beam;
     TDirectory*      cd_beamTime;
     TDirectory*      cd_alphaSub;
     TDirectory*      cd_multRatio;
     TDirectory*      cd_multErg;
     TDirectory*      cd_stack;
     TDirectory*      cd_LO;
     // **********
     TDirectory*      cd_gammaSpecBeam;
     // **********
   TDirectory*      cd_trigger;

   TDirectory*      cd_fragment;

   int NUM_TRIGGERS; // number of trigger channels
   int* TRIGGERS;

   int NUM_DETECTORS; // number of detectors
   int* DETECTORS; // currently unused


  /*
   ___                       _
  | _ \_ _ ___  __ ___ _____(_)_ _  __ _
  |  _/ '_/ _ \/ _/ -_|_-<_-< | ' \/ _` |
  |_| |_| \___/\__\___/__/__/_|_||_\__, |
                                   |___/
  */
  // objects specific to this run of the analysis
  int mode;                     // what mode this run is using
  int NUM_RUNS;                 // how many runs READ_FAME is doing
  int runNum;                   // what run READ_FAME is on
  bool CovEM_in;                // whether or not this run uses CovEM

  int BN;                       // CovEM setting (number of neutron bins)
  int BP;                       // CovEM setting (number of photon bins)
  int BA;                       // number of angular bins
  // int BTH = 8;                   // CovEM setting (number of Lab theta bins)
  // int BPH = 8;                   // CovEM setting (number of Lab theta bins)
  // int BA = 20;                   // CovEM setting (number of Lab theta bins)

  // int BP = 8;                   // CovEM setting (number of phi bins)
  double MIN_TIME_P, MAX_TIME_P;
  double MIN_TIME_N, MAX_TIME_N;
  double THRESHOLD, CLIPPING;
  double THRESHOLD_DEP, CLIPPING_DEP;

  double BACKGROUND_DELAY;
  double FISS_PILEUP_TIME;

  // beam informations
  double BEAM_ERG_MIN, BEAM_ERG_MAX; // range of the beam energies to be employed
  int BEAM_ERG_BINNUM = 10;          // number of energy bins in range (default so no breaking)

  // fragment information
  double MIN_ANGLE, MAX_ANGLE;
  double MIN_MASS, MAX_MASS;
  int MASS_BINNUM;

  // cov EM settings
  double MIN_N_ERG, MAX_N_ERG;  // CovEM setting
  double MIN_P_ERG, MAX_P_ERG;  // CovEM setting
  double sizeNerg, sizePerg, sizeNgAng;
  double sizeBerg;
  int iN1, iN2, iD1, iD2, iE1, iE2, iTot;


  TString rootEnding = ".root";
  TString nameExpTree = "Fiss";
  TString nameSimTree = "fissionTree";


  // non-user information
  long int expEntries;
  long int simEntries;

  // change these into inputs
  double intWindowAlpha = 1e5; // ns integration window
  double intWindowFiss;

  bool SELF_ERROR = true;


 /*
  _  _ _    _
 | || (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
 | __ | (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
 |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
                    |___/                    */

   // cov em matrices
   int******* arrayExp;
   int******* arrayBack;

   // differentiated lists
   int ******** arrayExpBeam;
   int ******** arrayBackBeam;
   // *****************
   int **** arrayExpErg;
   // *****************

   short int ********* arrayExpFrag;
   short int ********* arrayBackFrag;

   // matrix variables
   TH3D** h3_arrayCorrExp;
   TH3D** h3_arraySpecExp;
   TH3D** h3_arrayCorrBack;
   TH3D** h3_arraySpecBack;

   TH3D*** h3_arrayCorrExpBeam;
   TH3D*** h3_arraySpecExpBeam;
   TH3D*** h3_arrayCorrBackBeam;
   TH3D*** h3_arraySpecBackBeam;
   // **********************
   TH2I*** h2_arrayMultErg;
   // **********************

   double**** arrayCorrExp;
   double**** arraySpecExp;
   double**** arrayCorrBack;
   double**** arraySpecBack;

   double***** arrayCorrExpBeam;
   double***** arraySpecExpBeam;
   double***** arrayCorrBackBeam;
   double***** arraySpecBackBeam;

   //TH2D* h2_arrayDiff;

   // loop histograms quality of data
   TH1D* h_fissRej;

   // trigger histograms
   TH1D** h_timeDiffTrig;
   TH1D* h_fissTriggers;

   // n LO
   TH1D* h_neutronLightOutputSim;
   TH1D* h_neutronLightOutputExp;
   TH1D* h_neutronLightOutputBack;
   // p LO
   TH1D* h_photonLightOutputSim;
   TH1D* h_photonLightOutputExp;
   TH1D* h_photonLightOutputBack;

   // n ToF
   TH1D* h_neutronTofSim;
   TH1D* h_neutronTofExp;
   TH1D* h_neutronTofBack;

   // p ToF
   TH1D* h_photonTofSim;
   TH1D* h_photonTofExp;
   TH1D* h_photonTofBack;

   // n Erg
   TH1D* h_neutronEnergySim;
   TH1D* h_neutronEnergyExp;
   TH1D* h_neutronEnergyBack;

   // n Mult
   TH1D* h_neutronMultExp;
   TH1D* h_neutronMultSim;
   TH1D* h_neutronMultBack;

   // p Mult
   TH1D* h_photonMultExp;
   TH1D* h_photonMultSim;
   TH1D* h_photonMultBack;

   // n PSD
   TH1D* h_neutronPSDExp;
   TH1D* h_neutronPSDSim;
   TH1D* h_neutronPSDBack;

   // p PSD
   TH1D* h_photonPSDExp;
   TH1D* h_photonPSDSim;
   TH1D* h_photonPSDBack;

   // n det
   TH1D* h_neutronSinglesExp;
   TH1D* h_neutronSinglesSim;
   TH1D* h_neutronSinglesBack;

   // p det
   TH1D* h_photonSinglesExp;
   TH1D* h_photonSinglesSim;
   TH1D* h_photonSinglesBack;

   // individual basic hists
   TH1D** h_IndivNeutronLightOutputSim;
   TH1D** h_IndivNeutronLightOutputExp;
   TH1D** h_IndivNeutronLightOutputBack;

   TH1D** h_IndivPhotonLightOutputSim;
   TH1D** h_IndivPhotonLightOutputExp;
   TH1D** h_IndivPhotonLightOutputBack;

   TH1D** h_IndivNeutronTofSim;
   TH1D** h_IndivNeutronTofExp;
   TH1D** h_IndivNeutronTofBack;

   TH1D** h_IndivPhotonTofSim;
   TH1D** h_IndivPhotonTofExp;
   TH1D** h_IndivPhotonTofBack;

   TH1D** h_IndivNeutronEnergySim;
   TH1D** h_IndivNeutronEnergyExp;
   TH1D** h_IndivNeutronEnergyBack;

   TH1D** h_IndivNeutronPSDExp;
   TH1D** h_IndivNeutronPSDSim;
   TH1D** h_IndivNeutronPSDBack;

   TH1D** h_IndivPhotonPSDExp;
   TH1D** h_IndivPhotonPSDSim;
   TH1D** h_IndivPhotonPSDBack;

   // correlated histograms - add Exp to the end of these
   TH2I* h2_neutronGammaMultExp;
   TH2I* h2_neutronGammaMultBack;
   TH2D* h2_neutronMultPhotonLOExp;
   TH2D* h2_neutronEnergyLOExp;
   TH2D* h2_neutronLightOutPSDExp;
   TH2D* h2_photonLightOutPSDExp;

   TH2D* h2_neutronDoublesMat;
   TH2D* h2_neutronSinglesMat;
   TH2I* h2_neutronScaledDoubles;

   TH2D* h2_photonDoublesMat;
   TH2D* h2_photonSinglesMat;
   TH2I* h2_photonScaledDoubles;

   TH2D* h2_neutronPhotonDoublesMat;
   TH2D* h2_neutronPhotonSinglesMat;
   TH2I* h2_neutronPhotonScaledDoubles;

   TH2D* h2_neutronBackDoublesMat;
   TH2D* h2_neutronBackSinglesMat;
   TH2I* h2_neutronBackScaledDoubles;

   TH2D* h2_photonBackDoublesMat;
   TH2D* h2_photonBackSinglesMat;
   TH2I* h2_photonBackScaledDoubles;

   TH2D* h2_neutronPhotonBackDoublesMat;
   TH2D* h2_neutronPhotonBackSinglesMat;
   TH2I* h2_neutronPhotonBackScaledDoubles;

   TGraphErrors* g_neutronAngleCorr;
   TGraphErrors* g_neutronAngleCorrAvg;

   TGraphErrors* g_photonAngleCorr;
   TGraphErrors* g_photonAngleCorrAvg;

   TGraphErrors* g_neutronPhotonAngleCorr;
   TGraphErrors* g_neutronPhotonAngleCorrAvg;

   TGraphErrors* g_neutronBackAngleCorr;
   TGraphErrors* g_neutronBackAngleCorrAvg;

   TGraphErrors* g_photonBackAngleCorr;
   TGraphErrors* g_photonBackAngleCorrAvg;

   TGraphErrors* g_neutronPhotonBackAngleCorr;
   TGraphErrors* g_neutronPhotonBackAngleCorrAvg;

   TGraphErrors** g_gammaBeamSlope;
   TGraphErrors** g_gammaBeamInt;

   double** angles;

   // individual correlated hists
   TH2D** h2_IndivNeutronEnergyLOExp;
   TH2D** h2_IndivNeutronLightOutPSDExp;
   TH2D** h2_IndivPhotonLightOutPSDExp;

   // alphaFile histograms
   TH1D** h_alphaDep;
   TH1I* h_macroPop;

   //Projection histograms
   TH1D*** pj_pLightOutErg;
   TH1D*** pj_nLightOutErg;
   TH1D*** pj_scaledGammaLOErg;
   TH1D*** pj_meanGammaLOErg;
   TH1D*** pj_scaledNeutronEnErg;
   TH1D*** pj_meanNeutronEnErg;

   // beam histograms
   TH1D** h_fisDep;
   TH1D** h_fisSubtract;
   TH2D** h2_fisDepErg;
   TH1D** h_beamTime;
   TH1D** h_beamErg;

   TH2D** h2_neutronMultDep;
   TH2D** h2_gammaMultDep;
   TH2D** h2_backNeutronMultDep;
   TH2D** h2_backGammaMultDep;

   TH2D** h2_neutronMultErg;
   TH2D** h2_gammaMultErg;
   TH2D** h2_backNeutronMultErg;
   TH2D** h2_backGammaMultErg;

   TH2D** h2_photonLightOutErg;
   TH2D** h2_gammaLightOutErg;
   TH2D** h2_nLightOutErg;
   TH2D** h2_nToFErg;

   TH2D** h2_nBackToFErg;
   TH2D** h2_photonBackLightOutErg;
   TH2D** h2_nBackLightOutErg;

   // fragment histograms

   TH2D* h2_MassTKE;
   TH2D* h2_MassExc;

   TH3D* h3_gMassTKE;

   TH3D* h3_nMassTKE;
   TH3D* h3_geMassTKE;

   TH3D* h3_nSpecMassTKE;
   TH3D* h3_gSpecMassTKE;

   TH3D* h3_nSpecMassExc;
   TH3D* h3_gSpecMassExc;

   TH3D** h3_nSpecMassTKEdet;
   TH3D** h3_gSpecMassTKEdet;

   TH3D** h3_nSpecMassExcDet;
   TH3D** h3_gSpecMassExcDet;

   TH2D* h2_nnAngCorrFragL;
   TH2D* h2_nnAngCorrFragH;

   TH1D* h1_neutronComSpec;
   TH2D* h2_neutSawtooth;

   // TH2D* h2_fragYield;

   // TH1D* h1_exc;
   // TH2D* h2_nSpecExc;
   // TH2D* h2_gSpecExc;



   //beam stack
   THStack** stack;

   // visual settings
   Float_t legend_x1;
   Float_t legend_x2;
   Float_t legend_y1;
   Float_t legend_y2;

   Float_t x_labelSize;
   Float_t y_labelSize;

   Float_t x_tickSize;
   Float_t y_tickSize;


/*
   ___          __ _ _
  | _ \_ _ ___ / _(_) |___ ___
  |  _/ '_/ _ \  _| | / -_|_-<
  |_| |_| \___/_| |_|_\___/__/

*/

  TProfile** p_neutronMultDep;				                   //Profile neutronMult vs fisDep
  TProfile** p_gammaMultDep;				                     //Profile gammaMult vs fisDep
  TProfile** p_backNeutronMultDep;                       //Profile backNeutronMult vs fisDep
	TProfile** p_backGammaMultDep;                         //Profile backGammaMult vs fisDep

  TProfile** p_nToFErg;
  TProfile** p_photonLightOutErg;
  TProfile** p_nLightOutErg;

  TGraph** g_fisRatioThreshold;                          // fis ratio for different dep threshold
  TGraph** g_fisRatioSelect;                             // fis ratio for different dep threshold

  TGraph** g_neutronMultRatioDep;
  TGraph** g_gammaMultRatioDep;


  TProfile** p_neutronMultErg;				                   //Profile neutronMult vs beamEnergy
  TProfile** p_gammaMultErg;				                     //Profile gammaMult vs beamEnergy
  TProfile** p_backNeutronMultErg;                       //Profile backNeutronMult vs beamEnergy
	TProfile** p_backGammaMultErg;                         //Profile backGammaMult vs beamEnergy

  TGraph** g_fisRatioErg;                                // fission triggers / total vs. beamEnergy
  TGraph** g_nMultErg;                                   // Background-subtracted nMult vs. beamEnergy
  TGraph** g_gMultErg;                                   // Background-subtracted gMult vs. beamEnergy
  TGraph** g_nMultBackErg;                               // Background-subtracted nBackMult vs. beamEnergy
  TGraph** g_gMultBackErg;                               // Background-subtracted gBackMult vs. beamEnergy
  TGraph** g_gMultnMult;                                 // Background-subtracted gBackMult vs. beamEnergy

  TGraphErrors* g_gnRatio;                               // ave gamma-neutron slope ratio due to PPAC
  TGraphErrors* g_nRatioSlopeInt;                                // ave neutron slope due to PPAC
  TGraphErrors* g_gRatioSlopeInt;                                // ave gamma slope due to PPAC


/*
   ___                   ___
  | _ ) ___ __ _ _ __   | _ \__ _ _ _  __ _ ___ ___
  | _ \/ -_) _` | '  \  |   / _` | ' \/ _` / -_|_-<
  |___/\___\__,_|_|_|_| |_|_\__,_|_||_\__, \___/__/
                                      |___/
*/

  double DEP_MIN = 0;       // Min energy for alpha fit
  double DEP_MAX = 0.05;    // Max energy for alpha fit

  double FIS_MIN = 0.005;   // Min energy for fission fit
  double FIS_MAX = 0.05;    // Max energy for fission fit

/*
 ___             _   _
| __|  _ _ _  __| |_(_)___ _ _  ___
| _| || | ' \/ _|  _| / _ \ ' \(_-<
|_| \_,_|_||_\__|\__|_\___/_||_/__/

*/

   TF1* f_TimeFromErg = new TF1("f_TimeFromErg", "[0] / [1] * ([2] + x) / sqrt(x * (2 * [2] + x))", 0, 20);

   // Fit Functions
   TF1** f_alpha;// = new TF1("f_alpha", "expo", DEP_MIN, DEP_MAX);  // Define expo fit for alpha background to start at middle bin with most counts
   TF1** f_expo;// = new TF1("f_expo", "expo");
   TF1** f_fisProducts;// = new TF1("f_fisProducts", "gaus", FIS_MIN, FIS_MAX);  // Define gaus fit function for after binning errors in low values of fisDep
   TF1** f_gauss;// = new TF1("f_gauss", "gaus");
   TF1** f_aveGmult; // = new TF1((TString)"f_aveGmult" + (TString)to_string(indexChannel), "pol1");
   TF1** f_aveNmult; // = new TF1((TString)"f_aveNmult" + (TString)to_string(indexChannel), "pol1");meanGammaLOErgFit[eG]
   TF1** f_meanGammaLOErg;



/*
              _____              ___                  _
  _____ ___ _|_   _| _ ___ ___  | _ )_ _ __ _ _ _  __| |_  ___ ___
 / -_) \ / '_ \| || '_/ -_) -_) | _ \ '_/ _` | ' \/ _| ' \/ -_|_-<
 \___/_\_\ .__/|_||_| \___\___| |___/_| \__,_|_||_\__|_||_\___/__/
         |_|                                                       */
   // Declaration of leaf types
   Double_t        fisTime;
   Double_t        fisDep;
   Int_t           fisChan;
   Double_t        fisPSP;
   Int_t           neutronMult;
   Int_t           gammaMult;
   Int_t           neutronBackMult;
   Int_t           gammaBackMult;
   Double_t        neutronDetTimes[MAX_MULT];   //[neutronMult]
   Double_t        neutronLightOut[MAX_MULT];   //[neutronMult]
   Double_t        neutronPSD[MAX_MULT];   //[neutronMult]
   Double_t        neutronToFErg[MAX_MULT];   //[neutronMult]
   Double_t        neutronDet[MAX_MULT];   //[neutronMult]
   Double_t        neutronVx[MAX_MULT];   //[neutronMult]
   Double_t        neutronVy[MAX_MULT];   //[neutronMult]
   Double_t        neutronVz[MAX_MULT];   //[neutronMult]
   Double_t        neutronCosF[MAX_MULT];   //[neutronMult]
   Double_t        photonDetTimes[MAX_MULT];   //[gammaMult]
   Double_t        photonLightOut[MAX_MULT];   //[gammaMult]
   Double_t        photonPSD[MAX_MULT];   //[gammaMult]
   Double_t        photonDet[MAX_MULT];   //[gammaMult]
   Double_t        photonVx[MAX_MULT];   //[gammaMult]
   Double_t        photonVy[MAX_MULT];   //[gammaMult]
   Double_t        photonVz[MAX_MULT];   //[gammaMult]
   Double_t        photonCosF[MAX_MULT];   //[neutronMult]
   Double_t        backNeutronDetTimes[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronLightOut[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronPSD[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronToFErg[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronDet[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronVx[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronVy[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronVz[MAX_MULT];   //[neutronBackMult]
   Double_t        backNeutronCosF[MAX_MULT];   //[neutronBackMult]
   Double_t        backPhotonDetTimes[MAX_MULT];   //[gammaBackMult]
   Double_t        backPhotonLightOut[MAX_MULT];   //[gammaBackMult]
   Double_t        backPhotonPSD[MAX_MULT];   //[gammaBackMult]
   Double_t        backPhotonDet[MAX_MULT];   //[gammaBackMult]
   Double_t        backPhotonVx[MAX_MULT];   //[gammaBackMult]
   Double_t        backPhotonVy[MAX_MULT];   //[gammaBackMult]
   Double_t        backPhotonVz[MAX_MULT];   //[gammaBackMult]
   Double_t        backPhotonCosF[MAX_MULT];   //[gammaBackMult]


   // loop variables
   Double_t        neutronFragment[MAX_MULT]; // 1: light, 2: heavy
   Double_t        neutronThetaCom[MAX_MULT];
   Double_t        neutronPhiCom[MAX_MULT];
   Double_t        neutronEnergyCom[MAX_MULT];

   // Double_t        neutronCosF[MAX_MULT];   //[gammaBackMult]
   // Double_t        photonCosF[MAX_MULT];   //[gammaBackMult]
   // Double_t        backNeutronCosF[MAX_MULT];   //[gammaBackMult]
   // Double_t        backPhotonCosF[MAX_MULT];   //[gammaBackMult]

   // beam specific branches
   Double_t        beamTime;
   Double_t        beamEnergy;
   Double_t        beamDep;
   Double_t        beamPSP;
   Int_t           beamChan;
   Int_t           beamIndex;

   // fragment specific branches
   Double_t        AL;
   Double_t        AH;
   Double_t        KEL;
   Double_t        KEH;
   Double_t        ThetaL;
   Double_t        ThetaH;
   Double_t        EX;
   Double_t        Anode1;
   Double_t        Anode2;
   Double_t        Grid1;
   Double_t        Grid2;
   Double_t        Cathode;


   // List of branches
   TBranch        *b_fisTime;   //!
   TBranch        *b_fisDep;   //!
   TBranch        *b_fisChan;   //!
   TBranch        *b_fisPSP;   //!
   TBranch        *b_neutronMult;   //!
   TBranch        *b_gammaMult;   //!
   TBranch        *b_neutronBackMult;   //!
   TBranch        *b_gammaBackMult;   //!
   TBranch        *b_neutronDetTimes;   //!
   TBranch        *b_neutronLightOut;   //!
   TBranch        *b_neutronPSD;   //!
   TBranch        *b_neutronToFErg;   //!
   TBranch        *b_neutronDet;   //!
   TBranch        *b_neutronVx;   //!
   TBranch        *b_neutronVy;   //!
   TBranch        *b_neutronVz;   //!
   TBranch        *b_neutronCosF;   //!
   TBranch        *b_photonDetTimes;   //!
   TBranch        *b_photonLightOut;   //!
   TBranch        *b_photonPSD;   //!
   TBranch        *b_photonDet;   //!
   TBranch        *b_photonVx;   //!
   TBranch        *b_photonVy;   //!
   TBranch        *b_photonVz;   //!
   TBranch        *b_photonCosF;   //!
   TBranch        *b_backNeutronDetTimes;   //!
   TBranch        *b_backNeutronLightOut;   //!
   TBranch        *b_backNeutronPSD;   //!
   TBranch        *b_backNeutronToFErg;   //!
   TBranch        *b_backNeutronDet;   //!
   TBranch        *b_backNeutronVx;   //!
   TBranch        *b_backNeutronVy;   //!
   TBranch        *b_backNeutronVz;   //!
   TBranch        *b_backNeutronCosF;   //!
   TBranch        *b_backPhotonDetTimes;   //!
   TBranch        *b_backPhotonLightOut;   //!
   TBranch        *b_backPhotonPSD;   //!
   TBranch        *b_backPhotonDet;   //!
   TBranch        *b_backPhotonVx;   //!
   TBranch        *b_backPhotonVy;   //!
   TBranch        *b_backPhotonVz;   //!
   TBranch        *b_backPhotonCosF;   //!

   // beam specific branches
   TBranch        *b_beamTime;   //!
   TBranch        *b_beamEnergy;   //!
   TBranch        *b_beamDep;   //!
   TBranch        *b_beamPSP;   //!
   TBranch        *b_beamChan;   //!
   TBranch        *b_beamIndex;   //!

   // fragment specific branches
   TBranch        *b_AL;
   TBranch        *b_AH;
   TBranch        *b_KEL;
   TBranch        *b_KEH;
   TBranch        *b_ThetaL;
   TBranch        *b_ThetaH;
   TBranch        *b_EX;
   TBranch        *b_Anode1;
   TBranch        *b_Anode2;
   TBranch        *b_Grid1;
   TBranch        *b_Grid2;
   TBranch        *b_Cathode;


   /*
     _     _____              ___                  _
  __(_)_ _|_   _| _ ___ ___  | _ )_ _ __ _ _ _  __| |_  ___ ___
 (_-< | '  \| || '_/ -_) -_) | _ \ '_/ _` | ' \/ _| ' \/ -_|_-<
 /__/_|_|_|_|_||_| \___\___| |___/_| \__,_|_||_\__|_||_\___/__/ */
 // Declaration of leaf types
   Int_t           neutronFlag[MAX_MULT];   //[neutronMult] not defined
   Int_t           photonFlag[MAX_MULT];   //[gammaMult]   not defined

   // List of branches ADD EXP OR SIM AT THE END
   TBranch        *b_neutronFlagSim;   //!
   TBranch        *b_photonFlagSim;   //!
   TBranch        *b_fisTimeSim;   //!
   TBranch        *b_fisDepSim;   //!
   TBranch        *b_fisChanSim;   //!
   TBranch        *b_fisPSPSim;   //!
   TBranch        *b_neutronMultSim;   //!
   TBranch        *b_gammaMultSim;   //!
   TBranch        *b_neutronBackMultSim;   //!
   TBranch        *b_gammaBackMultSim;   //!
   TBranch        *b_neutronDetTimesSim;   //!
   TBranch        *b_neutronLightOutSim;   //!
   TBranch        *b_neutronPSDSim;   //!
   TBranch        *b_neutronToFErgSim;   //!
   TBranch        *b_neutronDetSim;   //!
   TBranch        *b_neutronVxSim;   //!
   TBranch        *b_neutronVySim;   //!
   TBranch        *b_neutronVzSim;   //!
   TBranch        *b_photonDetTimesSim;   //!
   TBranch        *b_photonLightOutSim;   //!
   TBranch        *b_photonPSDSim;   //!
   TBranch        *b_photonDetSim;   //!
   TBranch        *b_photonVxSim;   //!
   TBranch        *b_photonVySim;   //!
   TBranch        *b_photonVzSim;   //!
   TBranch        *b_backNeutronDetTimesSim;   //!
   TBranch        *b_backNeutronLightOutSim;   //!
   TBranch        *b_backNeutronPSDSim;   //!
   TBranch        *b_backNeutronToFErgSim;   //!
   TBranch        *b_backNeutronDetSim;   //!
   TBranch        *b_backNeutronVxSim;   //!
   TBranch        *b_backNeutronVySim;   //!
   TBranch        *b_backNeutronVzSim;   //!
   TBranch        *b_backPhotonDetTimesSim;   //!
   TBranch        *b_backPhotonLightOutSim;   //!
   TBranch        *b_backPhotonPSDSim;   //!
   TBranch        *b_backPhotonDetSim;   //!
   TBranch        *b_backPhotonVxSim;   //!
   TBranch        *b_backPhotonVySim;   //!
   TBranch        *b_backPhotonVzSim;   //!

/*
  __  __     _   _            _
 |  \/  |___| |_| |_  ___  __| |___
 | |\/| / -_)  _| ' \/ _ \/ _` (_-<
 |_|  |_\___|\__|_||_\___/\__,_/__/*/

  // constructors
   readFiss();
   // readFiss(int &argc, char** &argv);
   virtual ~readFiss();

   // create a menu
   virtual void Run();
   virtual void Save(MainWindow* main_in);
   virtual void Load(MainWindow* main_in);
   virtual void Print(std::ostream &out);
   virtual void LoadInput(std::istream &in);
   virtual void SetInfo(MainWindow* main_in);
   // virtual void GetInfo(std::istream &inputStream);
   // virtual void BadInputMessage();

   // set thresholds and time limit
   virtual void SetRunThresholds(double threshold, double max_time_n);
   virtual void SetEnergyLimits(double min_n_erg, double max_n_erg, double min_p_erg, double max_p_erg);

   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetExpEntry(Long64_t entry);
   virtual Int_t    GetSimEntry(Long64_t entry);
   virtual Long64_t LoadExpTree(Long64_t entry);
   virtual Long64_t LoadSimTree(Long64_t entry);

   virtual void     InitExp(TChain *tree);
   virtual void     InitSim(TChain *tree);

   virtual Bool_t   Notify();
   virtual void     ShowExp(Long64_t entry = -1);
   virtual void     ShowSim(Long64_t entry = -1);

   virtual void     ReadBeamInfo();

   // loop through data
   virtual void     LoopExp();
   virtual void     LoopSim();
   virtual void     LoopBeam();

   // perform FAME analysis
   virtual void     SetBNBP(int BN, int BP); // set the number of bins for the covariance analysis
   virtual void     CovEM(); // number of energy bins in neutron and photon energies
   virtual void     WriteCovEM();
   virtual void     AnalyseCovEM();
   virtual void     ExtractCov();

   // perform beam analysis
   virtual void     BeamDepAnalysis();
   virtual void     BeamErgAnalysis();
   virtual void     FitMult();

   // perform correlated analysis
   virtual void     CorrAnalysis();
   virtual void     Slice();
   virtual void     AngCorr();


   // initialization functions
   virtual void     GenerateAngles();
   virtual void     InitializeHistograms();
   virtual void     InitializeCovHistograms();
   virtual void     InitializeFunctions();
   virtual int      isTrigger(int triggerNumber);
   virtual int      isDetector(int detectorNumber);

   // callable functions
   virtual void     PlotAll();
   virtual void     PlotCorr();
   virtual void     CompareAll();

   // plot the trigger properties
   virtual void     PlotTrigger();

   // plot the experiment branches
   virtual void     PlotTof();
   virtual void     PlotLightOut();
   virtual void     PlotErg();
   virtual void     PlotPSD();
   virtual void     PlotMult();
   virtual void     PlotSingles();
   // correlated
   virtual void     PlotMultCorExp();
   virtual void     PlotMultCorBack();
   virtual void     PlotMultLOExp();
   virtual void     PlotEnergyLOExp();
   virtual void     PlotN_LOPSD_Exp();
   virtual void     PlotP_LOPSD_Exp();
   virtual void     PlotN_AngleCorr();

   // plot individual detectors
   virtual void     PlotIndiv();

   // plot the beam parameters
   virtual void     PlotBeamTime();
   virtual void     PlotDepSubtraction();
   virtual void     PlotRatioMult();
   virtual void     PlotMultErg();
   virtual void     PlotStack();
   virtual void     PlotLO();
   virtual void     PlotBeamLO();

   // plot the fragment results
   virtual void     PlotFragmentEmission();
   virtual void     PlotNeutronSawtooth();

   // plot the experiment vs simulated branches
   virtual void     CompareTof();
   virtual void     CompareLightOut();
   virtual void     CompareErg();
   virtual void     ComparePSD();
   virtual void     CompareMult();
   virtual void     CompareSingles();
};

#endif
