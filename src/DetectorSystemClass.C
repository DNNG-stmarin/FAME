/*
Detector System Class Methods,
Author: Stefano Marin
Purpose: Methods associated with detector system class
Date: May 24th, 2020
*/

#include "DetectorSystemClass.h"

#define DetectorSystemClass_cxx

// constructor of the detector system class
DetectorSystemClass::DetectorSystemClass(TChain* treeIn, TFile* writeFile, InfoSystem* info)
{
	DETECTOR_THRESHOLD = info->DETECTOR_THRESHOLD;
	DETECTOR_CLIP = info->DETECTOR_CLIP;
	COINC_WINDOW = info->COINC_WINDOW;
	BEAM_WINDOW = info->BEAM_WINDOW;
  BEAM_DISTANCE = info->BEAM_DISTANCE;
	MIN_TIME_P = info->MIN_TIME_P;
	MAX_TIME_P = info->MAX_TIME_P;
	MIN_TIME_N = info->MIN_TIME_N;
	MAX_TIME_N = info->MAX_TIME_N;
	MINPSD_FIT = info->MINPSD_FIT;
	DIVPSD_FIT = info->DIVPSD_FIT;
	MAXPSD_FIT = info->MAXPSD_FIT;
	MINERG_FIT = info->MINERG_FIT;
	MAXERG_FIT = info->MAXERG_FIT;
	DELTA_BACK_SIG = info->DELTA_BACK_SIG;
	BACKGROUND_SHIFT = MAX_TIME_N - MIN_TIME_P + DELTA_BACK_SIG;
	DEBUG = info->DEBUG;
	PSD_ERG = info->PSD_ERG;
	STEP_SIZE = info->STEP_SIZE;


	NUM_TRIGGERS = info->NUM_TRIGGERS;
	NUM_DETS = info->NUM_DETS;
	NUM_EXCLUDED = info->NUM_EXCLUDED;
	NUM_BEAMS = info->NUM_BEAMS;

	FISSION_TRIGGERS = info->FISSION_TRIGGERS;
	DETECTORS = info->DETECTORS;
	EXCLUDE_DETECTORS = info->EXCLUDE_DETECTORS;
	DETECTOR_TYPE = info->DETECTOR_TYPE;

	TRIGGER_PATH = info->triggerPath;
	TRIGGER_DIR_PATH = info->triggerDirPath;
	DETECTOR_PATH = info->detectorPath;
	DET_CALIBRATION = new TGraph(*(info->calibrationDet));
	REUSE_DETECTOR = info->REUSE_DETECTOR;
	DOUBLE_DISC = info->DOUBLE_DISC;
	MISC_MAX = info->MISC_MAX;
	PARAM_FILES = info->PARAM_FILES;
	SIM_FILE = info->SIM_FILE;
	REMOVE_FLAG = info->REMOVE_FLAG;
	//BEAM = info->BEAM;
	FRAGMENT_MODE = info->FRAGMENT_MODE;
	FRAGMENT_PATH = info->FRAGMENT_PATH;

	// create the dynamically allocated array of detectors and triggers
	triggers = new TriggerClass[NUM_TRIGGERS];
	detectors = new DetectorClass[NUM_DETS];
	cout << "Detectors and triggers have been created" << endl;

  cout << "Background shift set to " << BACKGROUND_SHIFT << " ns." << endl;
	cout << "Collecting background between: " << MIN_TIME_N - BACKGROUND_SHIFT;
	cout << "ns and " << MAX_TIME_N - BACKGROUND_SHIFT << " ns." << endl;

	// initializing all delays and resolutions
	for(int det = 0; det < NUM_DETS; det ++)
	{
		detectors[det].timeDelay = new double[NUM_TRIGGERS];
		detectors[det].timeResolution = new double[NUM_TRIGGERS];

		detectors[det].type = DETECTOR_TYPE[det];
		
		cout << "detector det is of type " << detectors[det].type << endl;
	}

	cout << "setting coordinates " << endl;

  // Setting detector distances
	string line;
	string x, y, z;
	string u, v, w;
	ifstream inDet (DETECTOR_PATH);
	for(int i=0; i<NUM_DETS; i++)
	{
		// cout << "reading detector coordinates from file" << endl;
		// cout << i << endl;
		getline(inDet, line);
		istringstream iss(line);
		iss >> x >> y >> z;
		detectors[i].X = stod(x)*100;
		detectors[i].Y = stod(y)*100;
		detectors[i].Z = stod(z)*100;
		detectors[i].distance = sqrt(pow(stod(x),2)+pow(stod(y),2)+pow(stod(z),2))*100;
		cout << "Detector " << i << " at: " << detectors[i].X << " " << detectors[i].Y << " " << detectors[i].Z << endl;
	}
  inDet.close();

  // Setting trigger offsets
  string trigLine;
	ifstream inTrig (TRIGGER_PATH);
	ifstream inTrigDir;
	if(FRAGMENT_MODE)
	{
		inTrigDir.open(TRIGGER_DIR_PATH);
	}

	for(int i=0; i<NUM_TRIGGERS; i++)
	{
		// cout << "reading detector coordinates from file" << endl;
		// cout << i << endl;
    if(!inTrig.is_open())
    {
      triggers[i].X = 0.0;
      triggers[i].Y = 0.0;
      triggers[i].Z = 0.0;
			cout << "Can't open trigger cartesian file, using default trigger at origin."  <<endl;
    }
    else
    {
  		getline(inTrig, trigLine);
  		istringstream iss(trigLine);
  		iss >> x >> y >> z;
      triggers[i].X = stod(x) * 100;
      triggers[i].Y = stod(y) * 100;
      triggers[i].Z = stod(z) * 100;
      //cout << info->triggerPath << endl;
      cout << "Trigger " << i << " at: " << triggers[i].X << " " << triggers[i].Y << " " << triggers[i].Z << endl;
    }

		if(FRAGMENT_MODE)
		{
			if(!inTrigDir.is_open())
			{
				triggers[i].U = 0.0;
				triggers[i].V = 0.0;
				triggers[i].W = 1.0;
				cout << "Can't open trigger dire, using default trigger direction toward Z axis."  <<endl;
			}
			else
			{
				getline(inTrigDir, trigLine);
	  		istringstream iss(trigLine);
	  		iss >> u >> v >> w;
	      triggers[i].U = stod(u);
	      triggers[i].V = stod(v);
	      triggers[i].W = stod(w);

				double totDir = pow(pow(triggers[i].U,2) + pow(triggers[i].V,2) + pow(triggers[i].W,2),0.5);

				triggers[i].U = triggers[i].U/totDir;
	      triggers[i].V = triggers[i].V/totDir;
	      triggers[i].W = triggers[i].W/totDir;

	      //cout << info->triggerPath << endl;
	      cout << "Trigger " << i << " pointing: " << triggers[i].U << " " << triggers[i].V << " " << triggers[i].W << endl;
			}
		}



	}
  inTrig.close();
	if(FRAGMENT_MODE) inTrigDir.close();


	//calibration for only chinu system
	for(int i=0; i<NUM_DETS; i++)
	{
		// cout << "Reading detector calibration from file" << endl;
		detectors[i].calibration = DET_CALIBRATION->Eval(i)/CSCOMPTEDGE;
	}

	cout << "Detector calibration complete\n" << endl;

	// initialize the tree and the file to write to
	detFile = writeFile;
	cout << "Tree passed at " << treeIn << endl;
	Init(treeIn);

	if(REUSE_DETECTOR == 0){
		//create folders and write things to correct folder
		cdPsd = detFile->mkdir("PSD");
		cdToF = detFile->mkdir("TOF");
		cdKin = detFile->mkdir("Kinematics");
		// cdMult =  detFile->mkdir("Multiplicity");
		// cdCoinc =  detFile->mkdir("Coincidences");
		// cdFigCoinc = detFile->mkdir("CoincFigs");
		// cdBicorr = detFile->mkdir("Bicorr");
		// cdRef = detFile->mkdir("Reflections");
		cdBeam = detFile->mkdir("Beam");

		// create the folders for psd
		cdPsdSlices = cdPsd->mkdir("PSD_slices");
		cdPsdIndividual = cdPsd->mkdir("PSD_individual");
		cdPsdErg = cdPsd->mkdir("PSDErg_discrimination");
		cdParam = cdPsd->mkdir("PSD_parameters");

		// create folders for tof
		cdTofIndividual = cdToF->mkdir("TOF_individual");
		cdTOFPSD = cdPsd->mkdir("TOF_PSD");
		if(TOF_ERG == 1){
			cdTofSlices = cdToF->mkdir("TOF_slices");
			cdTofErg = cdToF->mkdir("TOFErg_discrimination");
		}
		cdTOFCorr = cdToF->mkdir("TOF_Corrected");
	}
}

DetectorSystemClass::~DetectorSystemClass()
{
   delete tree;

	 delete triggers;
	 delete detectors;

	 delete DETECTORS;
	 delete FISSION_TRIGGERS;
	 delete EXCLUDE_DETECTORS;
	 delete DET_CALIBRATION;
}

Int_t DetectorSystemClass::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!tree) return 0;
   return tree->GetEntry(entry);
}
Long64_t DetectorSystemClass::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!tree) return -5;
   Long64_t centry = tree->LoadTree(entry);
   if (centry < 0) return centry;
   if (tree->GetTreeNumber() != fCurrent) {
      fCurrent = tree->GetTreeNumber();
      Notify();
   }
   return centry;
}

void DetectorSystemClass::Init(TChain* treeIn)
{
   // Set branch addresses and branch pointers for the coincidence tree
   if (!treeIn) {
		 cout << "No tree!" << endl;
		 return;
	 }
	 else
	 {
		 tree = treeIn;
		 fCurrent = -1;
		 tree->SetMakeClass(1);

		 tree->SetBranchAddress("tMult", &tMult, &b_tMult);

		 tree->SetBranchAddress("tTime", &tTime, &b_tTime);
		 tree->SetBranchAddress("tDep", &tDep, &b_tDep);
		 tree->SetBranchAddress("tPSP", &tPSP, &b_tPSP);
		 tree->SetBranchAddress("tChan", &tChan, &b_tChan);

		 if(NUM_BEAMS)
		 {
			 tree->SetBranchAddress("bTime", &bTime, &b_bTime);
			 tree->SetBranchAddress("bErg", &bErg, &b_bErg);
			 tree->SetBranchAddress("bPSP", &bPSP, &b_bPSP);
			 tree->SetBranchAddress("bChan", &bChan, &b_bChan);
			 tree->SetBranchAddress("bIndex", &bIndex, &b_bIndex);
		 }
		 if(FRAGMENT_MODE)
		 {
			 tree->SetBranchAddress("rAL", &rAL, &b_rAL);
			 tree->SetBranchAddress("rAH", &rAH, &b_rAH);
			 tree->SetBranchAddress("rKEL", &rKEL, &b_rKEL);
			 tree->SetBranchAddress("rKEH", &rKEH, &b_rKEH);
			 tree->SetBranchAddress("rThetaL", &rThetaL, &b_rThetaL);
			 tree->SetBranchAddress("rThetaH", &rThetaH, &b_rThetaH);
			 tree->SetBranchAddress("rEX", &rEX, &b_rEX);

			 tree->SetBranchAddress("rAn1", &rAn1, &b_rAn1);
			 tree->SetBranchAddress("rAn2", &rAn2, &b_rAn2);
			 tree->SetBranchAddress("rGr1", &rGr1, &b_rGr1);
			 tree->SetBranchAddress("rGr2", &rGr2, &b_rGr2);
		 	 tree->SetBranchAddress("rCat", &rCat, &b_rCat);

			 tree->SetBranchAddress("rA1", &rA1, &b_rA1);

		 }

		 tree->SetBranchAddress("totToF", totToF, &b_totToF);
		 tree->SetBranchAddress("totPSP", totPSP, &b_totPSP);
		 tree->SetBranchAddress("totDep", totDep, &b_totDep);
		 tree->SetBranchAddress("totChan", totChan, &b_totChan);
		 // tree->SetBranchAddress("totTail", totTail, &b_totTail);
		 if(SIM_FILE) tree->SetBranchAddress("totFlag", totFlag, &b_totFlag);


		 Notify();

		 cout << "Tree being read in correctly " << tree << endl;;
	 }

}

void DetectorSystemClass::InitFiss()
{
	 // fission variables
	 fissionTree->Branch("fisTime", &f_fisTime, "fisTime/D");
	 fissionTree->Branch("fisDep", &f_fisDep, "fisDep/D");
	 fissionTree->Branch("fisType", &f_fisType, "fisType/I");
	 fissionTree->Branch("fisChan", &f_fisChan, "fisChan/I");
	 fissionTree->Branch("fisPSP", &f_fisPSP, "fisPSP/D");

	 // beam variables
	 if(NUM_BEAMS > 0)
	 {
		 fissionTree->Branch("beamTime", &f_beamTime, "beamTime/D");
		 fissionTree->Branch("beamEnergy", &f_beamEnergy, "beamEnergy/D");
		 fissionTree->Branch("beamDep", &f_beamDep, "beamDep/D");
		 fissionTree->Branch("beamPSP", &f_beamPSP, "beamPSP/D");
		 fissionTree->Branch("beamChan", &f_beamChan, "beamChan/I");
		 fissionTree->Branch("beamIndex", &f_beamIndex, "beamIndex/I");
	 }

	 // fragment variables
	 if(FRAGMENT_MODE == 1)
	 {
		 fissionTree->Branch("AL", &f_AL, "AL/D");
		 fissionTree->Branch("AH", &f_AH, "AH/D");
		 fissionTree->Branch("KEL", &f_KEL, "beamDep/D");
		 fissionTree->Branch("KEH", &f_KEH, "beamPSP/D");
		 fissionTree->Branch("ThetaL", &f_ThetaL, "ThetaL/D");
		 fissionTree->Branch("ThetaH", &f_ThetaH, "ThetaH/D");
		 fissionTree->Branch("EX", &f_EX, "EX/D");


		 fissionTree->Branch("Anode1", &f_An1, "An1/D");
		 fissionTree->Branch("Anode2", &f_An2, "An2/D");
		 fissionTree->Branch("Grid1", &f_Gr1, "Gr1/D");
		 fissionTree->Branch("Grid2", &f_Gr2, "Gr2/D");
		 fissionTree->Branch("Cathode", &f_Cat, "Cat/D");
	 }

	 // multiplicity variables
	 fissionTree->Branch("neutronMult", &f_neutronMult, "neutronMult/I");
	 fissionTree->Branch("gammaMult", &f_gammaMult, "gammaMult/I");
	 fissionTree->Branch("neutronBackMult", &f_neutronBackMult, "neutronBackMult/I");
	 fissionTree->Branch("gammaBackMult", &f_gammaBackMult, "gammaBackMult/I");

	 // neutron attribues
	 fissionTree->Branch("neutronDetTimes", neutronDetTimes, "neutronDetTimes[neutronMult]/D");
	 fissionTree->Branch("neutronLightOut", neutronLightOut, "neutronLightOut[neutronMult]/D");
	 fissionTree->Branch("neutronPSD", neutronPSD, "neutronPSD[neutronMult]/D");
	 fissionTree->Branch("neutronToFErg", neutronToFErg, "neutronToFErg[neutronMult]/D");
	 fissionTree->Branch("neutronDet", neutronDet, "neutronDet[neutronMult]/D");
	 fissionTree->Branch("neutronVx", neutronVx, "neutronVx[neutronMult]/D");
	 fissionTree->Branch("neutronVy", neutronVy, "neutronVy[neutronMult]/D");
	 fissionTree->Branch("neutronVz", neutronVz, "neutronVz[neutronMult]/D");
	 if(SIM_FILE) fissionTree->Branch("neutronFlag", neutronFlag, "neutronFlag[neutronMult]/I");

	 // photon attributes
	 fissionTree->Branch("photonDetTimes", photonDetTimes, "photonDetTimes[gammaMult]/D");
	 fissionTree->Branch("photonLightOut", photonLightOut, "photonLightOut[gammaMult]/D");
	 fissionTree->Branch("photonPSD", photonPSD, "photonPSD[gammaMult]/D");
	 fissionTree->Branch("photonDet", photonDet, "photonDet[gammaMult]/D");
	 fissionTree->Branch("photonVx", photonVx, "photonVx[gammaMult]/D");
	 fissionTree->Branch("photonVy", photonVy, "photonVy[gammaMult]/D");
	 fissionTree->Branch("photonVz", photonVz, "photonVz[gammaMult]/D");
	 if(SIM_FILE) fissionTree->Branch("photonFlag", photonFlag, "photonFlag[gammaMult]/I");

	 // background neutron attributes
	 fissionTree->Branch("backNeutronDetTimes", backNeutronDetTimes, "backNeutronDetTimes[neutronBackMult]/D");
	 fissionTree->Branch("backNeutronLightOut", backNeutronLightOut, "backNeutronLightOut[neutronBackMult]/D");
	 fissionTree->Branch("backNeutronPSD", backNeutronPSD, "backNeutronPSD[neutronBackMult]/D");
	 fissionTree->Branch("backNeutronToFErg", backNeutronToFErg, "backNeutronToFErg[neutronBackMult]/D");
	 fissionTree->Branch("backNeutronDet", backNeutronDet, "backNeutronDet[neutronBackMult]/D");
	 fissionTree->Branch("backNeutronVx", backNeutronVx, "backNeutronVx[neutronBackMult]/D");
	 fissionTree->Branch("backNeutronVy", backNeutronVy, "backNeutronVy[neutronBackMult]/D");
	 fissionTree->Branch("backNeutronVz", backNeutronVz, "backNeutronVz[neutronBackMult]/D");

	 fissionTree->Branch("backPhotonDetTimes", backPhotonDetTimes, "backPhotonDetTimes[gammaBackMult]/D");
	 fissionTree->Branch("backPhotonLightOut", backPhotonLightOut, "backPhotonLightOut[gammaBackMult]/D");
	 fissionTree->Branch("backPhotonPSD", backPhotonPSD, "backPhotonPSD[gammaBackMult]/D");
	 fissionTree->Branch("backPhotonDet", backPhotonDet, "backPhotonDet[gammaBackMult]/D");
	 fissionTree->Branch("backPhotonVx", backPhotonVx, "backPhotonVx[gammaBackMult]/D");
	 fissionTree->Branch("backPhotonVy", backPhotonVy, "backPhotonVy[gammaBackMult]/D");
	 fissionTree->Branch("backPhotonVz", backPhotonVz, "backPhotonVz[gammaBackMult]/D");

	 if(FRAGMENT_MODE>0)
	 {
	 		 fissionTree->Branch("neutronCosF", neutronCosF, "neutronCosF[neutronMult]/D");
			 fissionTree->Branch("neutronPhiF", neutronPhiF, "neutronPhiF[neutronMult]/D");
			 fissionTree->Branch("neutronCosCom", neutronCosCom, "neutronCosCom[neutronMult]/D");
			 fissionTree->Branch("neutronPhiCom", neutronPhiCom, "neutronPhiCom[neutronMult]/D");
			 fissionTree->Branch("neutronEnergyCom", neutronEnergyCom, "neutronEnergyCom[neutronMult]/D");

	 		 fissionTree->Branch("photonCosF", photonCosF, "photonCosF[gammaMult]/D");
			 fissionTree->Branch("photonPhiF", photonPhiF, "photonPhiF[gammaMult]/D");
			 fissionTree->Branch("photonCosCom", photonCosCom, "photonCosCom[gammaMult]/D");

	 		 fissionTree->Branch("backNeutronCosF", backNeutronCosF, "backNeutronCosF[neutronBackMult]/D");
			 fissionTree->Branch("backNeutronPhiF", backNeutronPhiF, "backNeutronPhiF[neutronBackMult]/D");
			 fissionTree->Branch("backNeutronCosCom", backNeutronCosCom, "backNeutronCosCom[neutronBackMult]/D");

	 		 fissionTree->Branch("backPhotonCosF", backPhotonCosF, "backPhotonCosF[gammaBackMult]/D");
			 fissionTree->Branch("backPhotonPhiF", backPhotonPhiF, "backPhotonPhiF[gammaBackMult]/D");
			 fissionTree->Branch("backPhotonCosCom", backPhotonCosCom, "backPhotonCosCom[gammaBackMult]/D");

	 }

	 fissionTree->SetMaxTreeSize(5000000000LL);
	 // fissionTree->SetFileNumber(-1);

   cout << "Fission tree has been created." << endl;
}

Bool_t DetectorSystemClass::Notify()
{
   return kTRUE;
}

void DetectorSystemClass::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!tree) return;
   tree->Show(entry);
}

Int_t DetectorSystemClass::Cut(Long64_t entry)
{
   return 1;
}


// returns the index of the detector or -1 if not a detector
int DetectorSystemClass::isDetector(int detectorNumber)
{
	int detIndex = -1;

	for(int index = 0; index < NUM_DETS; index++)
	{
		//cout << "In loop!!\n";
		if(detectorNumber == DETECTORS[index])
		{
			detIndex = index;
			break;
		}
	}
	return detIndex;
}

int DetectorSystemClass::isTrigger(int detectorNumber)
{
	int chamberIndex = -1;
	for(int index = 0; index < NUM_TRIGGERS; index++)
	{
		if(detectorNumber == FISSION_TRIGGERS[index])
		{
			chamberIndex = index;
			break;
		}
	}
	return chamberIndex;
}
