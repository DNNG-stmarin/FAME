#include "readFiss.h"
using namespace std;
void readFiss::InitializeHistograms()
{
  cout << "initializing histograms" << endl;

  const int numTofBins = 240;
	const double minTof = -20;
	const double maxTof = 100;

	const int numLObins = 250;
	const double minLO = 0;
	const double maxLO = 5;

	const int numErgBins = 100;
	const double minErg = 0;
	const double maxErg = 10;

  const int numPSDBins = 100;
  const double minPSP = 0;
  const double maxPSP = 1;

  const int minMult = 0;
  const int maxMult = 10;

  const int numDets = NUM_DETECTORS;

  const int numCosBins = 100;

  const int numfisDepBins = 500; //JJ import these numbers from the beamInfo.root file
  const double minDep = 0;
  const double maxDep = 0.05;

  const int numfisBeamTimeBins= 2000;
  const double minBeamTime = -200;
  const double maxBeamTime = 1800;

  const int numDeltaT = 200;
  const double minDeltaT = 0;
  const double maxDeltaT = 10000;

  const int numTKEbins = 70;
  const double minTKE = 120;
  const double maxTKE = 260;

  const int numExcBin = 50;
  const double minExc = 0;
  const double maxExc = 100;




  /*
   _   _                         _      _          _
  | | | |_ _  __ ___ _ _ _ _ ___| |__ _| |_ ___ __| |
  | |_| | ' \/ _/ _ \ '_| '_/ -_) / _` |  _/ -_) _` |
   \___/|_||_\__\___/_| |_| \___|_\__,_|\__\___\__,_|

   */

  h_fissRej = new TH1D("h_fissRej", "Composition of Fission Rejection; Fission Type; Counts", 10, -0.5, 9.5);
  h_timeDiffTrig = new TH1D* [NUM_TRIGGERS];
  for(int j = 0; j < NUM_TRIGGERS; j++)
  {
    h_timeDiffTrig[j] = new TH1D("h_timeDiffTrig_" + TString(to_string(j)), "Time difference Triggers; Delta Time (ns); Counts", numDeltaT, minDeltaT, maxDeltaT);
  }
  h_fissTriggers = new TH1D("h_fissTriggers", "Triggers by Channel; Trigger Channel; Counts", NUM_TRIGGERS, 0, NUM_TRIGGERS);

  h_neutronLightOutputExp = new TH1D("h_neutronLightOutputExp", "Light Output Experiment;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
  h_photonLightOutputExp = new TH1D("h_photonLightOutputExp", "Light Output Experiment;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
  h_neutronTofExp = new TH1D("h_neutronTofExp", "Time-of-Flight Experiment;Time [ns];Counts", numTofBins, minTof, maxTof);
  h_photonTofExp = new TH1D("h_photonTofExp", "Time-of-Flight Experiment;Time [ns];Counts", numTofBins, minTof, maxTof);
  h_neutronEnergyExp = new TH1D("h_neutronEnergyExp", "Neutron Energy Experiment;Energy [MeV];Counts", numErgBins, minErg, maxErg);
  h_neutronMultExp = new TH1D("h_neutronMultExp", "Neutron Multiplicity; multiplicity; counts", maxMult, minMult, maxMult);
  h_photonMultExp = new TH1D("h_photonMultExp", "Photon Multiplicity; multiplicity; counts", maxMult, minMult, maxMult);
  h_neutronPSDExp = new TH1D("h_neutronPSDExp", "Neutron PSP; PSP (tail/total); counts",  numPSDBins, minPSP, maxPSP);
  h_photonPSDExp = new TH1D("h_photonPSDExp", "Photon PSP; PSP (tail/total); counts", numPSDBins, minPSP, maxPSP);
  h_neutronSinglesExp = new TH1D("h_neutronSinglesExp", "Neutron Singles; Detector1; counts", numDets, 0, numDets);
  h_photonSinglesExp = new TH1D("h_photonSinglesExp", "Photon Singles; Detector1; counts", numDets, 0, numDets);

  h_neutronLightOutputBack = new TH1D("h_neutronLightOutputBack", "Light Output Background;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
  h_photonLightOutputBack = new TH1D("h_photonLightOutputBack", "Light Output Background;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
  h_neutronTofBack = new TH1D("h_neutronTofBack", "Time-of-Flight Background;Time [ns];Counts", numTofBins, minTof, maxTof);
  h_photonTofBack = new TH1D("h_photonTofBack", "Time-of-Flight Background;Time [ns];Counts", numTofBins, minTof, maxTof);
  h_neutronEnergyBack = new TH1D("h_neutronEnergyBack", "Neutron Energy Background;Energy [MeV];Counts", numErgBins, minErg, maxErg);
  h_neutronMultBack = new TH1D("h_neutronMultBack", "Neutron Multiplicity; multiplicity; counts", maxMult, minMult, maxMult);
  h_photonMultBack = new TH1D("h_photonMultBack", "Photon Multiplicity; multiplicity; counts", maxMult, minMult, maxMult);
  h_neutronPSDBack = new TH1D("h_neutronPSDBack", "Neutron PSP; PSP (tail/total); counts",  numPSDBins, minPSP, maxPSP);
  h_photonPSDBack = new TH1D("h_photonPSDBack", "Photon PSP; PSP (tail/total); counts", numPSDBins, minPSP, maxPSP);
  h_neutronSinglesBack = new TH1D("h_neutronSinglesBack", "Neutron Singles; Detector1; counts", numDets, 0, numDets);
  h_photonSinglesBack = new TH1D("h_photonSinglesBack", "Photon Singles; Detector1; counts", numDets, 0, numDets);



  if(simTree != 0)
  {
    h_neutronLightOutputSim = new TH1D("h_neutronLightOutputSim", "Light Output Simulation;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
    h_photonLightOutputSim = new TH1D("h_photonLightOutputSim", "Light Output Simulation;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
    h_neutronTofSim = new TH1D("h_neutronTofSim", "Time-of-Flight Simulation;Time [ns];Counts", numTofBins, minTof, maxTof);
    h_photonTofSim = new TH1D("h_photonTofSim", "Time-of-Flight Simulation;Time [ns];Counts", numTofBins, minTof, maxTof);
    h_neutronEnergySim = new TH1D("h_neutronEnergySim", "Neutron Energy Simulation;Energy [MeV];Counts", numErgBins, minErg, maxErg);
    h_neutronMultSim = new TH1D("h_neutronMultSim", "Neutron Multiplicity; multiplicity; counts", maxMult, minMult, maxMult);
    h_photonMultSim = new TH1D("h_photonMultSim", "Photon Multiplicity; multiplicity; counts", maxMult, minMult, maxMult);
    h_neutronPSDSim = new TH1D("h_neutronPSDSim", "Neutron PSP; PSP (tail/total); counts",  numPSDBins, minPSP, maxPSP);
    h_photonPSDSim = new TH1D("h_photonPSDSim", "Photon PSP; PSP (tail/total); counts", numPSDBins, minPSP, maxPSP);
    h_neutronSinglesSim = new TH1D("h_neutronSinglesSim", "Neutron Singles; Detector1; counts", numDets, 0, numDets);
    h_photonSinglesSim = new TH1D("h_photonSinglesSim", "Photon Singles; Detector1; counts", numDets, 0, numDets);
  }

/*
  ___                 _      _          _
 / __|___ _ _ _ _ ___| |__ _| |_ ___ __| |
| (__/ _ \ '_| '_/ -_) / _` |  _/ -_) _` |
 \___\___/_| |_| \___|_\__,_|\__\___\__,_|

 */
  // correlated multiplicity
  h2_neutronGammaMultExp = new TH2I("h2_neutronGammaMultExp", "Neutron-Gamma Multiplicity;Neutron Multiplicity; Gamma Multiplicity; Counts",maxMult, minMult, maxMult, maxMult, minMult, maxMult);
  h2_neutronGammaMultBack = new TH2I("h2_neutronGammaMultBack", "Neutron-Gamma Back Multiplicity;Neutron Multiplicity; Gamma Multiplicity; Counts",maxMult, minMult, maxMult, maxMult, minMult, maxMult);

// neutron mult vs photon light output
  h2_neutronMultPhotonLOExp = new TH2D("h2_neutronMultPhotonLOExp", "Neutron Multiplicity vs. Photon Light Output; Neutron Multiplicity; Photon Light Output [MeVee]; Counts", maxMult, minMult-0.5, maxMult-0.5, numLObins, minLO, maxLO);

// neutron ToFErg vs neutron Light Ouput
  h2_neutronEnergyLOExp = new TH2D("h2_neutronEnergyLOExp", "Neutron Energy vs. Neutron Light Output; Neutron Energy [MeV]; Neutron Light Output [MeVee]; Counts", numErgBins, minErg, maxErg, numLObins, minLO, maxLO);

// neutron Light Output vs neutron PSD
  h2_neutronLightOutPSDExp = new TH2D("h2_neutronLightOutPSDExp", "Neutron Light Output vs. Neutron PSD; Neutron Light Output [MeVee]; Neutron PSP [tail/total]; Counts", numLObins, minLO, maxLO, numPSDBins, minPSP, maxPSP);

// photon Light Output vs neutron PSD
  h2_photonLightOutPSDExp = new TH2D("h2_photonLightOutPSDExp", "Photon Light Output vs. Photon PSD; Photon Light Output [MeVee]; Photon PSP [tail/total]; Counts", numLObins, minLO, maxLO, numPSDBins, minPSP, maxPSP);

// angular coorrelations
  h2_neutronDoublesMat = new TH2D("neutronDoublesExp", "Neutron Doubles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  h2_neutronSinglesMat = new TH2D("h2_neutronSinglesMatExp", "Neutron Singles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);

  //*********
  h2_photonDoublesMat = new TH2D("photonDoublesExp", "Photon Doubles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  h2_photonSinglesMat = new TH2D("h2_photonSinglesMatExp", "Photon Singles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);

  h2_neutronPhotonDoublesMat = new TH2D("neutronPhotonDoublesExp", "Neutron-Photon Doubles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  h2_neutronPhotonSinglesMat = new TH2D("neutronh_photonSinglesExp", "Neutron-Photon Singles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  // Background
  h2_neutronBackDoublesMat = new TH2D("neutronBackDoublesExp", "Backgorund Neutron Doubles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  h2_neutronBackSinglesMat = new TH2D("h2_neutronBackSinglesMatExp", "Backgorund Neutron Singles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);

  h2_photonBackDoublesMat = new TH2D("photonBackDoublesExp", "Backgorund Photon Doubles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  h2_photonBackSinglesMat = new TH2D("h2_photonBackSinglesMatExp", "Backgorund Photon Singles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);

  h2_neutronPhotonBackDoublesMat = new TH2D("neutronPhotonBackDoublesExp", "Backgorund Neutron-Photon Doubles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  h2_neutronPhotonBackSinglesMat = new TH2D("neutronPhotonBackSinglesExp", "Backgorund Neutron-Photon Singles; Detector1; Detector2; counts", numDets, 0, numDets, numDets, 0, numDets);
  //*********



  //
  /*
  ___         _ _     _    _           _
 |_ _|_ _  __| (_)_ _(_)__| |_  _ __ _| |
  | || ' \/ _` | \ V / / _` | || / _` | |
 |___|_||_\__,_|_|\_/|_\__,_|\_,_\__,_|_|

 */
 cout << "initializing individual" << endl;
 // basic
 h_IndivNeutronLightOutputExp = new TH1D*[NUM_DETECTORS];
 h_IndivNeutronLightOutputBack = new TH1D*[NUM_DETECTORS];

 h_IndivPhotonLightOutputExp = new TH1D*[NUM_DETECTORS];
 h_IndivPhotonLightOutputBack = new TH1D*[NUM_DETECTORS];

 h_IndivNeutronTofExp = new TH1D*[NUM_DETECTORS];
 h_IndivNeutronTofBack = new TH1D*[NUM_DETECTORS];

 h_IndivPhotonTofExp = new TH1D*[NUM_DETECTORS];
 h_IndivPhotonTofBack = new TH1D*[NUM_DETECTORS];

 h_IndivNeutronEnergyExp = new TH1D*[NUM_DETECTORS];
 h_IndivNeutronEnergyBack = new TH1D*[NUM_DETECTORS];

 h_IndivNeutronPSDExp = new TH1D*[NUM_DETECTORS];
 h_IndivNeutronPSDBack = new TH1D*[NUM_DETECTORS];

 h_IndivPhotonPSDExp = new TH1D*[NUM_DETECTORS];
 h_IndivPhotonPSDBack = new TH1D*[NUM_DETECTORS];

 // simulated
 if(mode == SIM_MODE)
 {
   h_IndivNeutronLightOutputSim = new TH1D*[NUM_DETECTORS];
   h_IndivPhotonLightOutputSim = new TH1D*[NUM_DETECTORS];
   h_IndivNeutronTofSim = new TH1D*[NUM_DETECTORS];
   h_IndivPhotonTofSim = new TH1D*[NUM_DETECTORS];
   h_IndivNeutronEnergySim = new TH1D*[NUM_DETECTORS];
   h_IndivNeutronPSDSim = new TH1D*[NUM_DETECTORS];
   h_IndivPhotonPSDSim = new TH1D*[NUM_DETECTORS];
 }

 // correlated
 h2_IndivNeutronEnergyLOExp = new TH2D*[NUM_DETECTORS];
 h2_IndivNeutronLightOutPSDExp = new TH2D*[NUM_DETECTORS];
 h2_IndivPhotonLightOutPSDExp = new TH2D*[NUM_DETECTORS];


 for(int i = 0; i < NUM_DETECTORS; ++i)
 {
   // basic
   h_IndivNeutronLightOutputExp[i] = new TH1D((TString)"h_IndivNeutronLightOutputExp" + (TString)to_string(i), "Light Output Experiment;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
   h_IndivNeutronLightOutputBack[i] = new TH1D((TString)"h_IndivNeutronLightOutputBack" + (TString)to_string(i), "Light Output Background;Light Output [MeVee];Counts", numLObins, minLO, maxLO);

   h_IndivPhotonLightOutputExp[i] = new TH1D((TString)"h_IndivPhotonLightOutputExp" + (TString)to_string(i), "Light Output Experiment;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
   h_IndivPhotonLightOutputBack[i] = new TH1D((TString)"h_IndivPhotonLightOutputBack" + (TString)to_string(i), "Light Output Background;Light Output [MeVee];Counts", numLObins, minLO, maxLO);

   h_IndivNeutronTofExp[i] = new TH1D((TString)"h_IndivNeutronTofExp" + (TString)to_string(i), "Time-of-Flight Experiment;Time [ns];Counts", numTofBins, minTof, maxTof);
   h_IndivNeutronTofBack[i] = new TH1D((TString)"h_IndivNeutronTofBack" + (TString)to_string(i), "Time-of-Flight Background;Time [ns];Counts", numTofBins, minTof, maxTof);

   h_IndivPhotonTofExp[i] = new TH1D((TString)"h_IndivPhotonTofExp" + (TString)to_string(i), "Time-of-Flight Experiment;Time [ns];Counts", numTofBins, minTof, maxTof);
   h_IndivPhotonTofBack[i] = new TH1D((TString)"h_IndivPhotonTofBack" + (TString)to_string(i), "Time-of-Flight Background;Time [ns];Counts", numTofBins, minTof, maxTof);

   h_IndivNeutronEnergyExp[i] = new TH1D((TString)"h_IndivNeutronEnergyExp" + (TString)to_string(i), "Neutron Energy Experiment;Energy [MeV];Counts", numErgBins, minErg, maxErg);
   h_IndivNeutronEnergyBack[i] = new TH1D((TString)"h_IndivNeutronEnergyBack" + (TString)to_string(i), "Neutron Energy Background;Energy [MeV];Counts", numErgBins, minErg, maxErg);

   h_IndivNeutronPSDExp[i] = new TH1D((TString)"h_IndivNeutronPSDExp" + (TString)to_string(i), "Neutron PSP; PSP (tail/total); counts",  numPSDBins, minPSP, maxPSP);
   h_IndivNeutronPSDBack[i] = new TH1D((TString)"h_IndivNeutronPSDBack" + (TString)to_string(i), "Neutron PSP; PSP (tail/total); counts",  numPSDBins, minPSP, maxPSP);

   h_IndivPhotonPSDExp[i] = new TH1D((TString)"h_IndivPhotonPSDExp" + (TString)to_string(i), "Photon PSP; PSP (tail/total); counts", numPSDBins, minPSP, maxPSP);
   h_IndivPhotonPSDBack[i] = new TH1D((TString)"h_IndivPhotonPSDBack" + (TString)to_string(i), "Photon PSP; PSP (tail/total); counts", numPSDBins, minPSP, maxPSP);

   // simulated
   if(mode == SIM_MODE)
   {
     h_IndivNeutronLightOutputSim[i] = new TH1D((TString)"h_IndivNeutronLightOutputSim" + (TString)to_string(i), "Light Output Simulation;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
     h_IndivPhotonLightOutputSim[i] = new TH1D((TString)"h_IndivPhotonLightOutputSim" + (TString)to_string(i), "Light Output Simulation;Light Output [MeVee];Counts", numLObins, minLO, maxLO);
     h_IndivNeutronTofSim[i] = new TH1D((TString)"h_IndivNeutronTofSim" + (TString)to_string(i), "Time-of-Flight Simulation;Time [ns];Counts", numTofBins, minTof, maxTof);
     h_IndivPhotonTofSim[i] = new TH1D((TString)"h_IndivPhotonTofSim" + (TString)to_string(i), "Time-of-Flight Simulation;Time [ns];Counts", numTofBins, minTof, maxTof);
     h_IndivNeutronEnergySim[i] = new TH1D((TString)"h_IndivNeutronEnergySim" + (TString)to_string(i), "Neutron Energy Simulation;Energy [MeV];Counts", numErgBins, minErg, maxErg);
     h_IndivNeutronPSDSim[i] = new TH1D((TString)"h_IndivNeutronPSDSim" + (TString)to_string(i), "Neutron PSP; PSP (tail/total); counts",  numPSDBins, minPSP, maxPSP);
     h_IndivPhotonPSDSim[i] = new TH1D((TString)"h_IndivPhotonPSDSim" + (TString)to_string(i), "Photon PSP; PSP (tail/total); counts", numPSDBins, minPSP, maxPSP);
   }

   // correlated
   h2_IndivNeutronEnergyLOExp[i] = new TH2D((TString)"h2_IndivNeutronEnergyLOExp" + (TString)to_string(i), "Neutron Energy vs. Neutron Light Output; Neutron Energy [MeV]; Neutron Light Output [MeVee]; Counts", numErgBins, minErg, maxErg, numLObins, minLO, maxLO);
   h2_IndivNeutronLightOutPSDExp[i] = new TH2D((TString)"h2_IndivNeutronLightOutPSDExp" + (TString)to_string(i), "Neutron Light Output vs. Neutron PSD; Neutron Light Output [MeVee]; Neutron PSP [tail/total]; Counts", numLObins, minLO, maxLO, numPSDBins, minPSP, maxPSP);
   h2_IndivPhotonLightOutPSDExp[i] = new TH2D((TString)"h2_IndivPhotonLightOutPSDExp" + (TString)to_string(i), "Photon Light Output vs. Photon PSD; Photon Light Output [MeVee]; Photon PSP [tail/total]; Counts", numLObins, minLO, maxLO, numPSDBins, minPSP, maxPSP);

 }


 /*
  ___
 | _ ) ___ __ _ _ __
 | _ \/ -_) _` | '  \
 |___/\___\__,_|_|_|_|

 */


 if(mode == BEAM_MODE)
 {
   cout << "initializing beam" << endl;
  // alphaFile histograms
  h_alphaDep = new TH1D* [NUM_TRIGGERS];

  //Projection histograms

  pj_pLightOutErg = new TH1D** [(int)BEAM_ERG_BINNUM];
  pj_nLightOutErg = new TH1D** [(int)BEAM_ERG_BINNUM];
  pj_scaledGammaLOErg = new TH1D** [(int)NUM_TRIGGERS];
  pj_meanGammaLOErg = new TH1D** [(int)BEAM_ERG_BINNUM];
  pj_scaledNeutronEnErg = new TH1D** [(int)NUM_TRIGGERS];
  pj_meanNeutronEnErg = new TH1D** [(int)BEAM_ERG_BINNUM];


  // beam histograms
  h_fisDep = new TH1D* [NUM_TRIGGERS];
  h_fisSubtract = new TH1D* [NUM_TRIGGERS];
  h2_fisDepErg = new TH2D* [NUM_TRIGGERS];
  h_beamTime = new TH1D* [NUM_TRIGGERS];
  h_beamErg = new TH1D* [NUM_TRIGGERS];

  h2_neutronMultDep = new TH2D* [NUM_TRIGGERS];
  h2_gammaMultDep = new TH2D* [NUM_TRIGGERS];
  h2_backNeutronMultDep = new TH2D* [NUM_TRIGGERS];
  h2_backGammaMultDep = new TH2D* [NUM_TRIGGERS];

  h2_neutronMultErg = new TH2D* [NUM_TRIGGERS];
  h2_gammaMultErg = new TH2D* [NUM_TRIGGERS];
  h2_backNeutronMultErg = new TH2D* [NUM_TRIGGERS];
  h2_backGammaMultErg = new TH2D* [NUM_TRIGGERS];

  h2_photonLightOutErg = new TH2D* [NUM_TRIGGERS];
  h2_gammaLightOutErg = new TH2D* [NUM_TRIGGERS];
  h2_nLightOutErg = new TH2D* [NUM_TRIGGERS];
  h2_nToFErg = new TH2D* [NUM_TRIGGERS];

  h2_nBackToFErg = new TH2D* [NUM_TRIGGERS];
  h2_photonBackLightOutErg = new TH2D* [NUM_TRIGGERS];
  h2_nBackLightOutErg = new TH2D* [NUM_TRIGGERS];

  cout << "looper" << endl;

  for(int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    cout << indexChannel << endl;
    // h_alphaDep[indexChannel] = new TH1D("h_alphaDep","Total PPAC events; Event energy (V us); counts", numfisDepBins, minDep, maxDep);
    // beam histograms
    h_fisDep[indexChannel]  = new TH1D((TString)"h_fisDep" + (TString)to_string(indexChannel), "Total Fission Spectrum; Event Energy (V us); counts",  numfisDepBins, minDep, maxDep);
    h_fisSubtract[indexChannel]  = new TH1D((TString)"h_fisSubtract" + (TString)to_string(indexChannel), "Total Fission Spectrum; Event Energy (V us); counts",  numfisDepBins, minDep, maxDep);
    h2_fisDepErg[indexChannel]  = new TH2D((TString)"h2_fisDepErg" + (TString)to_string(indexChannel), "Total Fission Spectrum vs Ei; Event Energy (V us); Incident Neutron Energy (MeV); counts", numfisDepBins, minDep, maxDep, BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX);
    h_beamTime[indexChannel]  = new TH1D((TString)"h_beamTime" + (TString)to_string(indexChannel), "Fission rate in Beam Window; Time within micro beam Index (ns); counts",  numfisBeamTimeBins, minBeamTime, maxBeamTime);
    h_beamErg[indexChannel] = new TH1D((TString)"h_beamErg" + (TString)to_string(indexChannel), "Number of Fissions; Beam Energy (MeV); counts", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX);
    //cout << "h0" << endl;
    h2_neutronMultDep[indexChannel]  = new TH2D((TString)"h2_neutronMultDep" + (TString)to_string(indexChannel), "Energy Dependent Neutron Multiplicity; Event Energy (V us); Neutron Multiplicity; counts", numfisDepBins, minDep, maxDep, maxMult, minMult-0.5, maxMult-0.5);
    h2_gammaMultDep[indexChannel]  =  new TH2D((TString)"h2_gammaMultDep" + (TString)to_string(indexChannel), "Energy Dependent Gamma Multiplicity; Event Energy (V us); Gamma Multiplicity; counts", numfisDepBins, minDep, maxDep, maxMult, minMult-0.5, maxMult-0.5);
    h2_backNeutronMultDep[indexChannel]  = new TH2D((TString)"h2_backNeutronMultDep"+ (TString)to_string(indexChannel), "Energy Dependent Background Neutron Multiplicity; Event Energy (V us); Neutron Multiplicity; counts", numfisDepBins, minDep, maxDep, maxMult, minMult-0.5, maxMult-0.5);
    h2_backGammaMultDep[indexChannel]  =  new TH2D((TString)"h2_backGammaMultDep"+ (TString)to_string(indexChannel), "Energy Dependent Background Gamma Multiplicity; Event Energy (V us); Gamma Multiplicity; counts", numfisDepBins, minDep, maxDep, maxMult, minMult-0.5, maxMult-0.5);
    //cout << "h1" << endl;
    h2_neutronMultErg[indexChannel]  = new TH2D((TString)"h2_neutronMultErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Neutron Multiplicity; Incident Neutron Energy (MeV); Neutron Multiplicity; counts", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, maxMult, minMult-0.5, maxMult-0.5);
    h2_gammaMultErg[indexChannel]  = new TH2D((TString)"h2_gammaMultErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Gamma Multiplicity; Event Energy (V us); Gamma Multiplicity; counts", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, maxMult, minMult-0.5, maxMult-0.5);
    h2_backNeutronMultErg[indexChannel] =  new TH2D((TString)"h2_backNeutronMultErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Neutron Multiplicity; Incident Neutron Energy (MeV); Neutron Multiplicity; counts", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, maxMult, minMult-0.5, maxMult-0.5);
    h2_backGammaMultErg[indexChannel]  = new TH2D((TString)"h2_backGammaMultErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Gamma Multiplicity; Event Energy (V us); Gamma Multiplicity; counts", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, maxMult, minMult-0.5, maxMult-0.5);
    // cout << "h2" << endl;
    pj_pLightOutErg[indexChannel] = new TH1D* [(int)BEAM_ERG_BINNUM];
    pj_nLightOutErg[indexChannel] = new TH1D* [(int)BEAM_ERG_BINNUM];
    // cout << "h3" << endl;
    pj_scaledGammaLOErg[indexChannel] = new TH1D* [(int)BEAM_ERG_BINNUM];
    pj_meanGammaLOErg[indexChannel] = new TH1D* [(int)numLObins];
    // cout << "h4" << endl;
    pj_scaledNeutronEnErg[indexChannel] = new TH1D* [(int)BEAM_ERG_BINNUM];
    pj_meanNeutronEnErg[indexChannel] = new TH1D* [(int)numErgBins];
    // cout << "h5" << endl;
    h2_photonLightOutErg[indexChannel] = new TH2D((TString)"h2_photonLightOutErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Photon Light Output; Incident Neutron Energy (MeV); Photon Light Output (MeVee)", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, numLObins, minLO, maxLO);
    h2_gammaLightOutErg[indexChannel] = new TH2D((TString)"h2_gammaLightOutErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Gamma Light Output; Incident Neutron Energy (MeV); Photon Light Output (MeVee)", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, numLObins, minLO, maxLO);
    h2_nLightOutErg[indexChannel] = new TH2D((TString)"h2_nLightOutErg"+ (TString)to_string(indexChannel), "Incident Energy Depnedent Neutron Light Output; Incident Neutron Energy (MeV); Neutron Light Output (MeVee)", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, numLObins, minLO, maxLO);
    h2_nToFErg[indexChannel] = new TH2D((TString)"h2_nToFErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Neutron Time of Flight Energy; Incident Neutron Energy (MeV); Neutron Time of Flight Energy (MeV)", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, numErgBins, minErg, maxErg);
    // cout << "h6" << endl;
    h2_nBackToFErg[indexChannel] = new TH2D((TString)"h2_nBackToFErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Background Neutron Time of Flight Energy; Incident Neutron Energy (MeV); Background Neutron Time of Flight Energy (MeV)", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, numErgBins, minErg, maxErg);
    h2_photonBackLightOutErg[indexChannel] = new TH2D((TString)"h2_photonBackLightOutErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Background Photon Light Output; Incident Neutron Energy (MeV); Background Photon Light Output", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, numLObins, minLO, maxLO);
    h2_nBackLightOutErg[indexChannel] = new TH2D((TString)"h2_nBackLightOutErg"+ (TString)to_string(indexChannel), "Incident Energy Dependent Background Neutron Light Output; Incident Neutron Energy (MeV); Background Neutron Light Output", BEAM_ERG_BINNUM, BEAM_ERG_MIN, BEAM_ERG_MAX, numLObins, minLO, maxLO);

  }
}

  //beam stack
  stack = new THStack* [NUM_TRIGGERS];

  // declare the profiles for the analysis
  p_neutronMultDep = new TProfile* [NUM_TRIGGERS];
  p_gammaMultDep = new TProfile* [NUM_TRIGGERS];
  p_backNeutronMultDep = new TProfile* [NUM_TRIGGERS];
  p_backGammaMultDep = new TProfile* [NUM_TRIGGERS];

  p_nToFErg = new TProfile* [NUM_TRIGGERS];
  p_photonLightOutErg = new TProfile* [NUM_TRIGGERS];
  p_nLightOutErg = new TProfile* [NUM_TRIGGERS];


  g_fisRatioThreshold = new TGraph* [NUM_TRIGGERS];
  g_fisRatioSelect = new TGraph* [NUM_TRIGGERS];

  g_neutronMultRatioDep = new TGraph* [NUM_TRIGGERS];
  g_gammaMultRatioDep = new TGraph* [NUM_TRIGGERS];

  for(int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    g_fisRatioThreshold[indexChannel] = new TGraph(numfisDepBins);
    g_fisRatioSelect[indexChannel] = new TGraph(numfisDepBins);

    g_neutronMultRatioDep[indexChannel] = new TGraph(numfisDepBins);
    g_gammaMultRatioDep[indexChannel] = new TGraph(numfisDepBins);
  }

  // BeamErgAnalysis profiles and graphs
  p_neutronMultErg =     new TProfile* [NUM_TRIGGERS];
  p_gammaMultErg =       new TProfile* [NUM_TRIGGERS];
  p_backNeutronMultErg = new TProfile* [NUM_TRIGGERS];
  p_backGammaMultErg =   new TProfile* [NUM_TRIGGERS];

  g_fisRatioErg =  new TGraph* [NUM_TRIGGERS];
  g_nMultErg =     new TGraph* [NUM_TRIGGERS];
  g_gMultErg =     new TGraph* [NUM_TRIGGERS];
  g_nMultBackErg = new TGraph* [NUM_TRIGGERS];
  g_gMultBackErg = new TGraph* [NUM_TRIGGERS];

  g_gMultnMult =   new TGraph* [NUM_TRIGGERS];

  g_gnRatio = new TGraphErrors(NUM_TRIGGERS);
  g_nRatioSlopeInt = new TGraphErrors(NUM_TRIGGERS);
  g_gRatioSlopeInt = new TGraphErrors(NUM_TRIGGERS);

  for(int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    g_fisRatioErg[indexChannel] =  new TGraph(BEAM_ERG_BINNUM);
    g_nMultErg[indexChannel] =     new TGraph(BEAM_ERG_BINNUM);
    g_gMultErg[indexChannel] =     new TGraph(BEAM_ERG_BINNUM);
    g_nMultBackErg[indexChannel] = new TGraph(BEAM_ERG_BINNUM);
    g_gMultBackErg[indexChannel] = new TGraph(BEAM_ERG_BINNUM);

    g_gMultnMult[indexChannel] =   new TGraph(BEAM_ERG_BINNUM);
  }


  /*
       ___                            _
      | __| _ __ _ __ _ _ __  ___ _ _| |_
      | _| '_/ _` / _` | '  \/ -_) ' \  _|
      |_||_| \__,_\__, |_|_|_\___|_||_\__|
                  |___/

  */
  if(FRAG_MODE)
  {
    cout << "initializing fragment histograms" << endl;

    cout << MASS_BINNUM << " mass bins" << endl;
    cout << numTKEbins << " ke bins" << endl;

    // fragment histograms
    h3_gMassTKE  = new TH3D("h3_gMassTKE", "Gamma yield; Mass (AMU); TKE (MeV); Mean Gamma Multiplicity; counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE, maxMult, minMult-0.5, maxMult-0.5);
    h3_nMassTKE  = new TH3D("h3_gMassTKE", "Neutron yield; Mass (AMU); TKE (MeV); Mean Neutron Multiplicity; counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE, maxMult, minMult-0.5, maxMult-0.5);
    // h3_gSpecMassTKE = new TH3D("h3_gMassTKE", "Gamma Energy yield; Mass (AMU); TKE (MeV); Mean Gamma Energy; counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE, numLObins, 0, maxLO*maxMult);

    // actual final plots
    h2_MassTKE = new TH2D("h2_MassTKE", "TKE_Mass distribution; Mass (AMU); TKE (MeV);counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE);
    h2_MassExc = new TH2D("h2_MassExc", "Exc distribution; Mass (AMU); TKE (MeV);counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numExcBin, minExc, maxExc);

    h3_gSpecMassTKE = new TH3D("h3_gSpecMassTKE", "Gamma yield; Mass (AMU); TKE (MeV); Gamma-ray Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE, numLObins, 0, maxLO);
    h3_nSpecMassTKE = new TH3D("h3_nSpecMassTKE", "Neutron yield; Mass (AMU); TKE (MeV); Neutron Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE, numErgBins, 0, maxErg);

    h3_gSpecMassExc = new TH3D("h3_gSpecMassExc", "Gamma yield; Mass (AMU); TKE (MeV); Gamma-ray Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numExcBin, minExc, maxExc, numLObins, 0, maxLO);
    h3_nSpecMassExc = new TH3D("h3_nSpecMassExc", "Neutron yield; Mass (AMU); TKE (MeV); Neutron Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numExcBin, minExc, maxExc, numErgBins, 0, maxErg);

    h3_gSpecMassTKEdet = new TH3D*[NUM_DETECTORS];
    h3_nSpecMassTKEdet = new TH3D*[NUM_DETECTORS];

    h3_gSpecMassExcDet = new TH3D*[NUM_DETECTORS];
    h3_nSpecMassExcDet = new TH3D*[NUM_DETECTORS];

    for(int i = 0; i < NUM_DETECTORS; ++i)
    {
      h3_gSpecMassTKEdet[i] = new TH3D("h3_gSpecMassTKE_" + (TString)to_string(i), "Gamma yield; Mass (AMU); TKE (MeV); Gamma-ray Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE, numLObins, 0, maxLO);
      h3_nSpecMassTKEdet[i] = new TH3D("h3_nSpecMassTKE_" + (TString)to_string(i), "Gamma yield; Mass (AMU); TKE (MeV); Gamma-ray Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numTKEbins, minTKE, maxTKE, numLObins, 0, maxLO);

      h3_gSpecMassExcDet[i] = new TH3D("h3_gSpecMassExc_" + (TString)to_string(i), "Gamma yield; Mass (AMU); TKE (MeV); Gamma-ray Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numExcBin, minExc, maxExc, numLObins, 0, maxLO);
      h3_nSpecMassExcDet[i] = new TH3D("h3_nSpecMassExc_" + (TString)to_string(i), "Gamma yield; Mass (AMU); TKE (MeV); Gamma-ray Energy (MeV); counts", MASS_BINNUM, MIN_MASS, MAX_MASS, numExcBin, minExc, maxExc, numLObins, 0, maxLO);
    }

    h2_neutSawtooth = new TH2D("h2_neutSawtooth", "h2_neutSawtooth; Mass number; Nu", MASS_BINNUM, MIN_MASS, MAX_MASS, maxMult, minMult-0.5, maxMult-0.5);
    h1_neutronComSpec = new TH1D("h1_neutronComSpec", "h1_neutronComSpec; Energy CoM (MeV); counts", numErgBins, minErg, maxErg);

    h2_nnAngCorrFragL = new TH2D("h2_nnAngCorrFragL", "h2_nnAngCorrFragL; cos1; cos2; counts", 50, -1, 1, 50, -1, 1);
    h2_nnAngCorrFragH = new TH2D("h2_nnAngCorrFragH", "h2_nnAngCorrFragH; cos1; cos2; counts", 50, -1, 1, 50, -1, 1);

  }


}
