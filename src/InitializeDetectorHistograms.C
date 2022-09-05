/*
Author: Stefano Marin
Purpose: Initialize all the histograms that are used for the analysis of the
         detectors histogram
Date: Ann Arbor, May 14th 2020
*/

#define InitializeDetectorHistograms_cxx
#include "DetectorSystemClass.h"
#include "ProcessingConstants.h"

using namespace std;

void DetectorSystemClass::InitializeDetectorHistograms()
{
    // important axes to have
    TString timeAxis = "time (ns)";
    TString energyAxis = "energy (MeVee)";
    TString countAxis = "counts";

    // particle names
    TString neutronName = "n";
    TString photonName = "p";
    TString allName = "all";

    // name generators
    TString numDet;

    // psd
  	TString psdName = "PSD";
    TString psdHistNameT;
    psdhists = new TH1F* [NUM_DETS];

    // energy
  	TString ergName = "Erg";
    TString ergHistNameT;
    erghists = new TH1F* [NUM_DETS];

    // psd energy
  	TString psdErgName = "PSD_Erg";
    TString psdErgHistNameT;
    psdErgHists = new TH2F* [NUM_DETS];

    // TofPSD
    TString tofPsdNameT = "ToF_Psd";
    TString tofPsdHistNameT;
    tofPsdHists = new TH2F* [NUM_DETS];

    // TofPSD
    TString tofErgNameT = "ToF_Erg";
    TString tofErgHistNameT;
    tofErgHists = new TH2F* [NUM_DETS];

    // time of flight
  	TString tofName = "TOF";
    TString tofNHistNameT;
    TString tofPHistNameT;
    TString delayTimeT = "del";
    tofNhists = new TH1F* [NUM_DETS];
    tofPhists = new TH1F* [NUM_DETS];

    // delay histogram
    tofDelPhists = new TH1F** [NUM_DETS];

    //corrected ToF
    TString delp = "DelP";
    TString corr = "Corr";
    tofDelPhistsCorr = new TH1F* [NUM_DETS];
    tofPsdHistsCorr = new TH2F* [NUM_DETS];
    tofErgHistsCorr = new TH2F* [NUM_DETS];

    // kinematic
  	TString kinematicName = "Kin";
  	TString kinematicNHistNameT, kinematicPHistNameT, kinematicAllHistNameT;
    kinematicN = new TH2F* [NUM_DETS];
    kinematicP = new TH2F* [NUM_DETS];
    kinematicAll = new TH2F* [NUM_DETS];

    for(int i = 0; i < NUM_DETS; i++)
  	{

      cout << "initializing histograms for detector number " << i << endl;
  		// find the string name of the detector
  		numDet = to_string(DETECTORS[i]);

  		// psd histograms
  		psdHistNameT = psdName + numDet;
  		psdhists[i] = new TH1F(psdHistNameT, psdHistNameT, NUM_PSD_HIST, MIN_PSD_HIST, MAX_PSD_HIST);

      // energy histograms
      ergHistNameT = ergName + numDet;
      erghists[i] = new TH1F(ergHistNameT, ergHistNameT, NUM_LO_HIST, MIN_LO_HIST, MAX_LO_HIST);

      // time of flight histograms
  		tofNHistNameT = tofName + neutronName +  numDet;
  		tofPHistNameT = tofName + photonName + numDet;

      tofDelPhists[i] = new TH1F* [NUM_TRIGGERS];
      for(int tr = 0; tr < NUM_TRIGGERS; tr++)
      {
        TString trigString = (TString)to_string(tr);
        tofDelPhists[i][tr] = new TH1F(tofPHistNameT + delayTimeT + trigString, tofPHistNameT + delayTimeT + trigString, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW);
      }

      tofDelPhistsCorr[i] = new TH1F(tofName+delp+corr+numDet, tofName+delp+corr+numDet, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW);

      // split by particles
  		tofNhists[i] = new TH1F(tofNHistNameT, tofNHistNameT, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW);
      tofNhists[i]->SetLineColor(kBlue);
  		tofPhists[i] = new TH1F(tofPHistNameT, tofPHistNameT, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW);
      tofPhists[i]->SetLineColor(kRed);

      // tofPSd histograms
      tofPsdHistNameT = tofPsdNameT + numDet;
      tofPsdHists[i] = new TH2F(tofPsdHistNameT, tofPsdHistNameT, NUM_PSD_HIST, MIN_PSD_HIST, MAX_PSD_HIST, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW);
      tofPsdHists[i]->SetOption("COLZ");
      tofPsdHistsCorr[i] = new TH2F(tofName+psdName+corr+numDet, tofName+psdName+corr+numDet, NUM_PSD_HIST, MIN_PSD_HIST, MAX_PSD_HIST, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW);
      tofPsdHistsCorr[i]->SetOption("COLZ");

      // tofErg histograms
      tofErgHistNameT = tofErgNameT + numDet;
      tofErgHists[i] = new TH2F(tofErgHistNameT, tofErgHistNameT, NUM_LO_HIST, MIN_LO_HIST, MAX_LO_HIST, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW); //each slice is 10 kev
      tofErgHists[i]->SetOption("COLZ");
      tofErgHistsCorr[i] = new TH2F(tofName+ergName+corr+numDet, tofName+ergName+corr+numDet, NUM_LO_HIST, MIN_LO_HIST, MAX_LO_HIST, 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW);
      tofErgHistsCorr[i]->SetOption("COLZ");

      // energy-psd
      psdErgHistNameT = psdErgName + numDet;
      psdErgHists[i] = new TH2F(psdErgHistNameT, psdErgHistNameT, 1000, 0, 10, NUM_PSD_HIST, MIN_PSD_HIST, MAX_PSD_HIST); //each slice is 10 kev
      psdErgHists[i]->SetOption("COLZ");


      // kinematic histograms
      kinematicNHistNameT = kinematicName + neutronName + numDet;
      kinematicPHistNameT = kinematicName + photonName + numDet;
      kinematicAllHistNameT = kinematicName + allName + numDet;
  		kinematicN[i] = new TH2F(kinematicNHistNameT, kinematicNHistNameT + ";Time (ns); Integral (MeVee); Counts", 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW, 10000, 0, 10);
      kinematicN[i]->SetOption("COLZ");
      kinematicP[i] = new TH2F(kinematicPHistNameT, kinematicPHistNameT + ";Time (ns); Integral (MeVee); Counts", 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW, 10000, 0, 10);
  		kinematicP[i]->SetOption("COLZ");
      kinematicAll[i] = new TH2F(kinematicAllHistNameT, kinematicAllHistNameT + ";Time (ns); Integral (MeVee); Counts", 2*(int)COINC_WINDOW, -COINC_WINDOW, +COINC_WINDOW, 10000, 0, 10);
  		kinematicAll[i]->SetOption("COLZ");

  	}

    cout << "Detector histograms have been initialized. " << endl;



  /*
    __  __      _ _   _      _ _    _ _
   |  \/  |_  _| | |_(_)_ __| (_)__(_) |_ _  _
   | |\/| | || | |  _| | '_ \ | / _| |  _| || |
   |_|  |_|\_,_|_|\__|_| .__/_|_\__|_|\__|\_, |
                       |_|                |__/
  */

  // multiplicity histogram
  hMult = new TH2I("mult", "mult", 10, 0, 10, 10, 0, 10);

  // bicorrelation histogram
  TString bicorrT = "bicorr_det_";
  TString bicorrHistName, bicorrHistTitle;

  hBicorr = new TH2F** [NUM_DETS]; // initialize rows
  for(int det1 = 0; det1 < NUM_DETS; det1++)
  {
    hBicorr[det1] = new TH2F* [NUM_DETS]; // initialize columns

     for(int det2 = 0; det2 < NUM_DETS; det2++)
     {
        bicorrHistName = bicorrT + to_string(det1) + "_" + to_string(det2);
        bicorrHistTitle = bicorrHistName + ";" + timeAxis + ";" + timeAxis;

        hBicorr[det1][det2] = new TH2F(bicorrHistName, bicorrHistTitle, 100, -10, 100, 100, -10, 100);
        // hBicorr[det1][det2]->SetOption("COLZ");
     }
  }

  /*
    ___ _                    _      _   _
   | _ |_)__ ___ _ _ _ _ ___| |__ _| |_(_)___ _ _
   | _ \ / _/ _ \ '_| '_/ -_) / _` |  _| / _ \ ' \
   |___/_\__\___/_| |_| \___|_\__,_|\__|_\___/_||_|

  */

  cout << "bicorrelation" << endl;

  cout << "Bicorrelation histograms have been created" << endl;

  // singles and doubles histograms
  hSingles = new TH1I("singles", "singles; detector; counts", NUM_DETS + 1, 0, NUM_DETS);
  hDoubles = new TH2I("doubles", "doubles; detector; detector", NUM_DETS + 1, 0, NUM_DETS, NUM_DETS + 1, 0, NUM_DETS);

  cout << "Detection histograms have been created" << endl;

  /*
           _         _    _
    __ ___(_)_ _  __(_)__| |___ _ _  __ ___ ___
   / _/ _ \ | ' \/ _| / _` / -_) ' \/ _/ -_|_-<
   \__\___/_|_||_\__|_\__,_\___|_||_\__\___/__/

  */

  cout << "coincidences" << endl;
  // 
  // // create titles
  // TString nnCoincT = "nn_det_";
  // TString ngCoincT = "ng_det_";
  // TString gnCoincT = "gn_det_";
  // TString ggCoincT = "gg_det_";
  // TString allCoincT = "all_det_";
  //
  // double minTimeCrossCorr, maxTimeCrossCorr, numBinsCrossCorr;
  // minTimeCrossCorr = -50;
  // maxTimeCrossCorr = +50;
  // numBinsCrossCorr = 800;
  //
  //
  // TString nnHistName, ngHistName, gnHistName, ggHistName, allCoincName;
  // TString nnHistTitle, ngHistTitle, gnHistTitle, ggHistTitle, allCoincTitle;
  //
  // // populate histograms with pointers
  // allCoinc = new THStack** [NUM_DETS]; // initialize rows
  // nnMult = new TH1F** [NUM_DETS]; // initialize rows
  // ngMult = new TH1F** [NUM_DETS]; // initialize rows
  // gnMult = new TH1F** [NUM_DETS]; // initialize rows
  // ggMult = new TH1F** [NUM_DETS]; // initialize rows
  //
  // for(int det1 = 0; det1 < NUM_DETS; det1++)
  // {
  //     allCoinc[det1] = new THStack* [NUM_DETS]; // initialize rows
  //     nnMult[det1] = new TH1F* [NUM_DETS]; // initialize rows
  //     ngMult[det1] = new TH1F* [NUM_DETS]; // initialize rows
  //     gnMult[det1] = new TH1F* [NUM_DETS]; // initialize rows
  //     ggMult[det1] = new TH1F* [NUM_DETS]; // initialize rows
  //
  //    for(int det2 = 0; det2 < NUM_DETS; det2++)
  //    {
  //       allCoincName = allCoincT + to_string(det1) + "_" + to_string(det2);
  //       allCoincTitle = allCoincName + ";" + timeAxis + ";" + countAxis;
  //       allCoinc[det1][det2] = new THStack(allCoincName, "");
  //
  //       nnHistName = nnCoincT + to_string(det1) + "_" + to_string(det2);
  //       nnHistTitle = nnHistName + ";" + timeAxis + ";" + countAxis;
  //       nnMult[det1][det2] = new TH1F(nnHistName, nnHistTitle, numBinsCrossCorr, minTimeCrossCorr, maxTimeCrossCorr);
  //       nnMult[det1][det2]->SetLineColor(kBlue);
  //
  //       allCoinc[det1][det2]->Add(nnMult[det1][det2]);
  //
  //       ngHistName = ngCoincT + to_string(det1) + "_" + to_string(det2);
  //       ngHistTitle = ngHistName + ";" + timeAxis + ";" + countAxis;
  //       ngMult[det1][det2] = new TH1F(ngHistName, ngHistTitle, numBinsCrossCorr, minTimeCrossCorr, maxTimeCrossCorr);
  //       ngMult[det1][det2]->SetLineColor(kOrange);
  //
  //       allCoinc[det1][det2]->Add(ngMult[det1][det2]);
  //
  //       gnHistName = gnCoincT + to_string(det1) + "_" + to_string(det2);
  //       gnHistTitle = gnHistName + ";" + timeAxis + ";" + countAxis;
  //       gnMult[det1][det2] = new TH1F(gnHistName, gnHistTitle, numBinsCrossCorr, minTimeCrossCorr, maxTimeCrossCorr);
  //       gnMult[det1][det2]->SetLineColor(kOrange);
  //
  //       allCoinc[det1][det2]->Add(gnMult[det1][det2]);
  //
  //       ggHistName = ggCoincT + to_string(det1) + "_" + to_string(det2);
  //       ggHistTitle = ggHistName + ";" + timeAxis + ";" + countAxis;
  //       ggMult[det1][det2] = new TH1F(ggHistName, ggHistTitle, numBinsCrossCorr, minTimeCrossCorr, maxTimeCrossCorr);
  //       ggMult[det1][det2]->SetLineColor(kRed);
  //
  //       allCoinc[det1][det2]->Add(ggMult[det1][det2]);
  //       allCoinc[det1][det2]->SetDrawOption("nostack");
  //    }
  // }

  cout << "Cross-correlations histograms have been created" << endl;

  /*
    ___      __ _        _   _
   | _ \___ / _| |___ __| |_(_)___ _ _  ___
   |   / -_)  _| / -_) _|  _| / _ \ ' \(_-<
   |_|_\___|_| |_\___\__|\__|_\___/_||_/__/
  */

  // // create the reflections
  // TString refT = "ref_det_";
  // TString refHistName, refHistTitle;
  // reflections = new TH2F** [NUM_DETS]; // initialize rows
  // for(int det1 = 0; det1 < NUM_DETS; det1++)
  // {
  //    reflections[det1] = new TH2F* [NUM_DETS]; // initialize rows
  //    for(int det2 = 0; det2 < NUM_DETS; det2++)
  //    {
  //       refHistName = refT + to_string(det1) + "_" + to_string(det2);
  //       refHistTitle = refHistName + ";" + timeAxis + ";" + energyAxis + ";" + countAxis;
  //       reflections[det1][det2] = new TH2F(refHistName, refHistTitle, 100, -50, 50, 10000, 0, 10);
  //    }
  // }
  //
  // cout << "Reflection histograms have been created" << endl;


  /*
   ___                   _  _ _    _
  | _ ) ___ __ _ _ __   | || (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
  | _ \/ -_) _` | '  \  | __ | (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
  |___/\___\__,_|_|_|_| |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
                                          |___/
  */
  cout << "bicorrelation" << endl;
  TString nameBeamHist;
  TString bHistStart = "b_";
  beamTimeHist = new TH1D* [NUM_TRIGGERS]; // initialize rows

  for(int trigNum = 0; trigNum < NUM_TRIGGERS; trigNum++)
  {
    nameBeamHist = bHistStart + to_string(trigNum);
    beamTimeHist[trigNum] = new TH1D(nameBeamHist, nameBeamHist, 4000, -2000, 2000);
  }

  cout << "Beam histograms have been created" << endl;

  cout << "All histograms have been initialized" << endl;

}
