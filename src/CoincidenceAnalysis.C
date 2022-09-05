#define CoincidenceAnalysis_cxx

#include "CoincidenceAnalysis.h"

#include <TH2.h>
#include <TH3.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TCut.h>
#include <TFitResult.h>
#include <THStack.h>
#include <TRandom.h>

#include <stdlib.h>
#include <stdio.h>
#include <queue>

#include "InfoSystem.h"
#include "ProcessingConstants.h"

#include "CoincidenceEvent.h"
#include "ParticleEvent.h"
#include "TriggerEvent.h"
#include "BeamEvent.h"

#include "mappingFunctions.h"

using namespace std;

int CoincidenceAnalysis::CreateCoincidenceTree(Long64_t entriesToProc)
{
	/*
	  _  _ _    _
	 | || (_)__| |_ ___  __ _ _ _ __ _ _ __  ___
	 | __ | (_-<  _/ _ \/ _` | '_/ _` | '  \(_-<
	 |_||_|_/__/\__\___/\__, |_| \__,_|_|_|_/__/
	                    |___/
	*/
	cout << "Initializing coincidence THistogram " << endl;

	//TH1F* h_Dt = new TH1F("delT", "delT", 1000, 0, 5e12);
	TH1D* h_microTimeDiff = new TH1D("microDelta", "Time Interval Micro; Time Difference (ns); Counts", 1000, 1750, 1800);
	TH1D* h_macroTimeDiff = new TH1D("macroDelta", "Time Interval Macro; Time Difference (ns); Counts", 1000, 0, 5e7);
	TH1D* h_microTime = new TH1D("microTime", "Time in Micro; Time (ns); Counts", 10000, 0, 1e6);
	TH1I* h_macroPop = new TH1I("macroPop", "Population in Macro; Number of Micropulses; Counts", 500, 0, 500);

  TH1D* h_alphaSpec = new TH1D("alphaSpec", "Alpha spectrum; Pulse Integral (V us); Counts", 500, 0, 0.05);
  TH1D* h_alphaTime = new TH1D("alphaTime", "Alpha time; t; Counts", 200000, -20000000, 0);

  TH1D* h_fisSpec = new TH1D("fisSpec", "Fission window spectrum; Pulse Integral (V us); Counts", 500, 0, 0.05);

  TH2D* h2_alphaTimeSpec = new TH2D("alphaTimeSpec", "Alpha spec vs. time; t (ns); Pulse Integral (V us)", 100, -120000, -20000, 500, 0, 0.05);
  TH2D* h2_alphaChanSpec = new TH2D("alphaChanSpec", "Alpha spec vs. chan; Channel; Pulse Integral (V us)", 40, 0, 40, 500, 0, 0.05);
  TH2D* h2_alphaTimeChan = new TH2D("alphaTimeChan", "Alpha chan vs. time; t (ns); Channel", 1000, -10000000, 0, 40, 0, 40);


  TH1D* h_triggerTime = new TH1D("triggerTime", "Trigger Time; t (ns); Counts", 1e5, 0 + .5e6, 20e6 + .5e6);
  TH1D* h_beamTime = new TH1D("beamTime", "Beam Time; t (ns); Counts", 1e5, 0 + .5e6, 20e6 + .5e6);
  TH2D* h2_triggerTimeChan = new TH2D("triggerTimeChan", "Trigger chan vs. time; t (ns); Channel", 1e5, 0 + .5e6, 40e6 + .5e6, 4, 5, 9);

	TH3D* h3_fileDesc = new TH3D("h3_fileDesc", "File clustering; Number of Beams; Number of Fissions; Number of Particles;counts", 100, 0,  1e9, 100, 0,  1e9, 100, 0,  1e9);

	TH1I* h1_fissNumPerBeam = new TH1I("h1_fissNumPerBeam", "Number of fissions per micropulse; Number of fissions; counts", 100, -0.5, 99.5);
	TH1D* h1_fissTimeInBeam = new TH1D("h1_fissTimeInBeam", "Time of fissions in micropulse; Number of fissions; counts", 2000, 0, 2000);

	TH1D* h1_fissFragDelt = new TH1D("h1_fissFragDelt", "Coincidence Window", 2000, 0, 200000);
	TH1D* h1_fragSorting = new TH1D("h1_fragSorting", "FragmentSorting", 2000, -1e7, 3*FRAGMENT_WINDOW);
	TH1D* h1_lostFrags = new TH1D("h1_lostFrag", "lostFrag", 5,0,5);

	queue<BeamEvent> microStructure;

	TRandom* randomTimeCoinc = new TRandom();

	if(RANDOM_COINCIDENCE == 1)
	{
		cout << "Producing random coincidence fission events" << endl;
	}



	/*
	  _______              _____            _                 _   _
	 |__   __|            |  __ \          | |               | | (_)
	    | |_ __ ___  ___  | |  | | ___  ___| | __ _ _ __ __ _| |_ _  ___  _ __
	    | |  __/ _ \/ _ \ | |  | |/ _ \/ __| |/ _  |  __/ _  | __| |/ _ \|  _ \
	    | | | |  __/  __/ | |__| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | |
	    |_|_|  \___|\___| |_____/ \___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|
  */

	cout << "Initializing coincidence TTrees " << endl;

	expFile->cd();

	coincTree = new TTree("CoincidenceTree", "Tree of Experimental Coincidences");
	coincTree->SetFileNumber(0);

	Long64_t fragEntry;

	// declatre the variables to store the fission branches
	// fission trigger
	int tMult = 0;
	double tTime = 0;
	double tDep = 0;
	int tChan = 0;
	double tPSP = 0;
	double tTail = 0;

	// fission beam
	double bTime = 0;
	double bErg = 0;
	int bChan = 0;
	int bIndex = 0;
	double bPSP = 0;
	double bTail = 0;

	// fragment branches
	double rDelt = 0;
	double rAL = 0;
	double rAH = 0;
	double rKEL = 0;
	double rKEH = 0;
	double rThetaL = 0;
	double rThetaH = 0;
	double rEX = 0;

	double rAn1 = 0;
	double rAn2 = 0;
	double rGr1 = 0;
	double rGr2 = 0;
	double rCat = 0;

	double rA1 = 0;

	// particles
	double totToF[MAX_MULTIPLICITY] = {0};
	double totPSP[MAX_MULTIPLICITY] = {0};
	double totDep[MAX_MULTIPLICITY] = {0};
	int totChan[MAX_MULTIPLICITY] = {0};
	int totTail[MAX_MULTIPLICITY] = {0};

	// trigger variables
	coincTree->Branch("tMult", &tMult, "tMult/I");
	coincTree->Branch("tTime", &tTime, "fissionTime/D");
	coincTree->Branch("tDep", &tDep, "fissionErg/D");
	coincTree->Branch("tChan", &tChan, "fissionChan/I");
	coincTree->Branch("tPSP", &tPSP, "fissionPSP/D");
	coincTree->Branch("tTail", &tTail, "fissionTail/D");

	// beam variables
	if(NUM_BEAMS > 0)
	{
		coincTree->Branch("bTime", &bTime, "beamTime/D");
		coincTree->Branch("bErg", &bErg, "beamEnergy/D");
		coincTree->Branch("bChan", &bChan, "beamChan/I");
		coincTree->Branch("bIndex", &bIndex, "beamIndex/I");
		coincTree->Branch("bPSP", &bPSP, "beamPSP/D");
		coincTree->Branch("bTail", &bTail, "beamTail/D");
	}

	if(FRAGMENT_MODE == 1)
	{
		cout << "Creating fragment chain" << endl;
		fragTreeChain = new TChain(nameFragTree, "Fragment Tree");
		coincTree->Branch("rAL", &rAL, "rAL/D");
		coincTree->Branch("rAH", &rAH, "rAH/D");
		coincTree->Branch("rKEL", &rKEL, "rKEL/D");
		coincTree->Branch("rKEH", &rKEH, "rKEH/D");
		coincTree->Branch("rThetaL", &rThetaL, "rThetaL/D");
		coincTree->Branch("rThetaH", &rThetaH, "rThetaH/D");
		coincTree->Branch("rEX", &rEX, "rEX/D");
		coincTree->Branch("rDelt", &rDelt, "rDelt/D");

		coincTree->Branch("rAn1", &rAn1, "rAn1/D");
		coincTree->Branch("rAn2", &rAn2, "rAn2/D");
		coincTree->Branch("rGr1", &rGr1, "rGr1/D");
		coincTree->Branch("rGr2", &rGr2, "rGr2/D");
		coincTree->Branch("rCat", &rCat, "rCat/D");

		coincTree->Branch("rA1", &rA1, "rA1/D");

		// open the fragment tree
		gROOT->cd();

		cout << "Reading fragment tree from " << FRAGMENT_PATH + rootEnding + "/" + nameFragTree << endl;

		fragTreeChain->Add(FRAGMENT_PATH + rootEnding + "/" + nameFragTree);
		bool fileFound = true;
		int fileNum = 1;

		while(gSystem->AccessPathName(FRAGMENT_PATH + "_" + to_string(fileNum) + rootEnding) == false)
		{
			fragTreeChain->Add(FRAGMENT_PATH + "_" + to_string(fileNum) + rootEnding + "/" + nameFragTree);
			cout << "And file " << FRAGMENT_PATH + "_" + to_string(fileNum) + rootEnding + "/" + nameFragTree << endl;
			fileNum++;
		}
		cout << "Completed reading from " << fileNum << " with " << fragTreeChain->GetEntries() << endl;

		cout << "initializing fragment tree " << endl;
		fragTreeChain->SetMakeClass(1);
		fragTreeChain->SetBranchAddress("fT", &fT, &b_fT);
    fragTreeChain->SetBranchAddress("fAL", &fAL, &b_fAL);
    fragTreeChain->SetBranchAddress("fAH", &fAH, &b_fAH);
    fragTreeChain->SetBranchAddress("fKEL", &fKEL, &b_fKEL);
    fragTreeChain->SetBranchAddress("fKEH", &fKEH, &b_fKEH);
    fragTreeChain->SetBranchAddress("fThetaL", &fThetaL, &b_fThetaL);
    fragTreeChain->SetBranchAddress("fThetaH", &fThetaH, &b_fThetaH);
		fragTreeChain->SetBranchAddress("fEX", &fEX, &b_fEX);

    fragTreeChain->SetBranchAddress("fAn1", &fAn1, &b_fAn1);
		fragTreeChain->SetBranchAddress("fAn2", &fAn2, &b_fAn2);
		fragTreeChain->SetBranchAddress("fGr1", &fGr1, &b_fGr1);
		fragTreeChain->SetBranchAddress("fGr2", &fGr2, &b_fGr2);
		fragTreeChain->SetBranchAddress("fCat", &fCat, &b_fCat);

		fragTreeChain->SetBranchAddress("fA1", &fA1, &b_fA1);

		cout << fragTreeChain->GetEntries() << " fragment events" << endl;

		// index of fragment tree
		fragEntry = 1;

		fragTreeChain->GetEntry(1);
		cout << "First time is " << fT << endl;

	}

	// list variables
	coincTree->Branch("totToF", totToF, "totToF[tMult]/D");
	coincTree->Branch("totPSP", totPSP, "totPSP[tMult]/D");
	coincTree->Branch("totDep", totDep, "totDep[tMult]/D");
	coincTree->Branch("totChan", totChan, "totChan[tMult]/I");
	coincTree->Branch("totTail", totTail, "totTail[tMult]/D");

	coincTree->SetMaxTreeSize(5000000000LL);


	int fissNcount = 0;


	/*
		   _____      _            _     _                       _
		  / ____|    (_)          (_)   | |                     | |
		 | |     ___  _ _ __   ___ _  __| | ___ _ __   ___ ___  | |     ___   ___  _ __
		 | |    / _ \| | '_ \ / __| |/ _` |/ _ \ '_ \ / __/ _ \ | |    / _ \ / _ \| '_ \
		 | |___| (_) | | | | | (__| | (_| |  __/ | | | (_|  __/ | |___| (_) | (_) | |_) |
		  \_____\___/|_|_| |_|\___|_|\__,_|\___|_| |_|\___\___| |______\___/ \___/| .__/
		                                                                          | |
		                                                                          |_|
	*/
		cout << "Initializing coincidence queue " << endl;


		// keep track of the iterator in each of the channels
		Long64_t *indexTrig = new Long64_t[NUM_TRIGGERS];

		// keep track of the earliest times in each of the channels
		double *DetectorLastTime = new double[NUM_DETS];

		// distribution of chamber times and energy
		double *chamberTimes = new double[NUM_TRIGGERS];
		double *chamberErgs = new double[NUM_TRIGGERS];
		double *chamberPSP = new double[NUM_TRIGGERS];
		int chamberChan = 0;

		// initialize the new particle
		TriggerEvent qTrigger = TriggerEvent();
		BeamEvent qBeam = BeamEvent();
		CoincidenceEvent newFission = CoincidenceEvent(0, 0, 0, 0, 0);

		// start at the beginning of the array and also keep track of the the number of coincidence events found.
		ULong64_t numFissEvents = 0;

		// initialize fission tracker
		bool validFiss = true;
		double averageTrigTime = 0;
		double sumTrigErg = 0;
		double averageTrigPSP = 0;

		long double beginTime = 0;

		double firstMicro;
		double lastMicro;
		double timeDiffMacro;
		double timeDiffMicro;

		double microTime;
		double timeHead;

	  int microIndex;

		// variables for validation
		CoincidenceEvent curFis;
		CoincidenceEvent goodFis;
		BeamEvent curBeam;

		double deltaFissBeam;
		double curFisTime, curBeamTime;

		int numFissBeam = 0;
		/*
			______ _         _               _
		 |  ____(_)       (_)             | |
		 | |__   _ ___ ___ _  ___  _ __   | |     ___   ___  _ __
		 |  __| | / __/ __| |/ _ \| '_ \  | |    / _ \ / _ \| '_ \
		 | |    | \__ \__ \ | (_) | | | | | |___| (_) | (_) | |_) |
		 |_|    |_|___/___/_|\___/|_| |_| |______\___/ \___/| .__/
																												| |
																												|_|
		*/
		cout << "Initializing fission queue. " << endl;

		// now loop thrugh fission events to find valid fission events
		CoincidenceEvent qFission = CoincidenceEvent(0, 0, 0, 0, 0);

		// trigger variables
		double fissionTime = 0;
		double fissionEnergy = 0;
		int fissionChan = 0;
		double fissionPSP = 0;
		double fissionTail = 0;

		// same but for beam variables
		double beamPSP, beamTime, beamEnergy, beamTail;
		int beamChan, beamIndex;

		// dynamical variables
		double detTime = 0;
		double detPSP = 0;

		// store multiplicities
		int totMult = 0;

		// find coincidences option
		ParticleEvent qParticle = ParticleEvent();

		// time to compare to fission
		double compDetTime;
		double deltaT;

		// keep track of the fission index
		long int fisTracker = 0;

		long int countBeams, countFiss, countDet;

		// fragment diff times
		Double_t tDiff;
		double oldTime;



	/*
	  __  __       _         _
	 |  \/  |     (_)       | |
	 | \  / | __ _ _ _ __   | |     ___   ___  _ __
	 | |\/| |/ _` | | '_ \  | |    / _ \ / _ \| '_ \
	 | |  | | (_| | | | | | | |___| (_) | (_) | |_) |
	 |_|  |_|\__,_|_|_| |_| |______\___/ \___/| .__/
	                                          | |
	                                          |_|
	*/

	bool noFrag;
	if(FRAGMENT_MODE)
	{
		noFrag = false;


	}

	// get the number of entries
	Long64_t nentries = fChain->GetEntries();
	//nentries = 1000000; // for debug
	cout << "For this file there are: " << nentries << " entries." << endl;

	if(entriesToProc > 0)
	{
		cout << "Reading the first " << entriesToProc << " events." << endl;
		nentries = entriesToProc;
	}

	// keep track of the number of bytes in the chain
	Long64_t nbytes = 0, nb = 0;

	// initialize the particle
	ParticleEvent newParticle = ParticleEvent();
	TriggerEvent newTrigger = TriggerEvent();
	BeamEvent newBeam = BeamEvent();

	// initialize the detector channel
	int detChannel = 0;
	double timeDet = 0;
	double energyDep = 0;
	double energyTail = 0;
	int entryChannel = 0;

	int refTrig = 0;
	double refTrigTime = 0;
	double compTrigTime = 0;

	bool pastRef, trigReady;

	double timeDel = 0;
	int fileInd = 0;

	if(NUM_BEAMS > 0) cout << "Beam delay set to: " << BEAM_DELAY << endl;

	cout << "Fragment mode set to: " << FRAGMENT_MODE << endl;


	// cout << TRIGGER_THRESHOLD << " " << TRIGGER_CLIP << endl;
	bool doneFrag = false;

	// loop through the raw data
	for (Long64_t jentry = 0; jentry < nentries; jentry++)
	{

		// if(jentry%100000 == 0)
		// {
		// 	cout << jentry << endl;
		// }

		Long64_t ientry = LoadTree(jentry);
	  if (ientry < 0) break;

		if(FRAGMENT_MODE == 1)
		{
	  	Long64_t fentry	= fragTreeChain->LoadTree(fragEntry);
			if(fragEntry >= fragTreeChain->GetEntries() - 10)
			{
				doneFrag = true;
				break;
			}
		}



		// load current entry
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		//correct for time delay
		if(ientry == 0)
		{
			// coincTree->SetFileNumber(fileInd);
			cout << "Loading file number " << fileInd++ << endl;

      if(DATA_TYPE == 0) // compass
      {
        timeDel = 0;
      }
      else if(DATA_TYPE == 1) // midas
      {
        timeDel = timeDet;
      }

			countBeams = 0;
			countFiss = 0;
			countDet = 0;

			h3_fileDesc->Fill(countBeams, countFiss, countDet);
		}

		//  ___ _ _ _ _
		// | __(_) | (_)_ _  __ _
		// | _|| | | | | ' \/ _` |
		// |_| |_|_|_|_|_||_\__, |
		// 							   	|___/

		// choose the correct data format
		if(DATA_TYPE == 0) //compass type digitizer
		{
			detChannel = cp->getDetector();
			timeDet = cp->getTime() + timeDel;
			energyDep = cp->getEnergy();
			energyTail = cp->getTail();

		}
		else if(DATA_TYPE == 1) //midas type digitizer
		{
			detChannel = md->getDetector();
			timeDet = md->getTime() + timeDel;
			energyDep = md->getEnergy();
			energyTail = md->getTail();
		}

		// new events is in a detector
		if(isDetector(detChannel, NUM_DETS, DETECTORS) >= 0)
		{
			entryChannel = isDetector(detChannel, NUM_DETS, DETECTORS);
			newParticle = ParticleEvent(detChannel, timeDet - BEAM_DELAY, energyDep, energyTail);
			DetectorBuffer[entryChannel].push(newParticle);
			countDet++;
		}

		// new event is in a trigger
		else if(isTrigger(detChannel, NUM_TRIGGERS, FISSION_TRIGGERS) >= 0)
		{
			entryChannel = isTrigger(detChannel, NUM_TRIGGERS, FISSION_TRIGGERS);
			newTrigger = TriggerEvent(detChannel, timeDet - BEAM_DELAY, energyDep, energyTail);

			// energy discrimination of fission
			if((newTrigger.getEnergy() > TRIGGER_THRESHOLD) & (newTrigger.getEnergy() < TRIGGER_CLIP) & (newTrigger.getPsp() > TRIGGER_MIN_PSP) & (newTrigger.getPsp() < TRIGGER_MAX_PSP) )
			{
				// cout << newTrigger.getEnergy() << endl;
				TriggerBuffer[entryChannel].push(newTrigger);
				countFiss++;
				// cout << "trigger " << endl;
				// cout << TriggerBuffer[entryChannel].size() << endl;
			}
			else
			{
				// cout << "discarded trigger at " << newTrigger.getEnergy() << " " << newTrigger.getPsp() << endl;
			}

      h_triggerTime->Fill(timeDet - BEAM_DELAY); // Why is this a generic beam delay and not the calculated delay?
      h2_triggerTimeChan->Fill(timeDet - BEAM_DELAY, detChannel);
			// cout << "trigger " << endl;
		}

		else if(isBeam(detChannel, NUM_BEAMS, BEAM) >= 0)
		{
			entryChannel = isBeam(detChannel, NUM_BEAMS, BEAM);
			newBeam = BeamEvent(detChannel, timeDet, energyDep, energyTail);
			BeamBuffer[entryChannel].push(newBeam);
			countBeams++;
      h_beamTime->Fill(timeDet);
			// cout << newBeam.getEnergy() << endl;
		}

		else
		{
			continue;
		}

		// cout << DetectorBuffer[0].size() << endl;


		/*
		 ____                         _
		|  _ \                       | |
		| |_) | ___  __ _ _ __ ___   | |     ___   ___  _ __
		|  _ < / _ \/ _` | '_ ` _ \  | |    / _ \ / _ \| '_ \
		| |_) |  __/ (_| | | | | | | | |___| (_) | (_) | |_) |
		|____/ \___|\__,_|_| |_| |_| |______\___/ \___/| .__/
																									| |
																									|_|
		*/
		// cout << "microStructure size: " << microStructure.size() << endl;

		// loop through the beams (usually only one)
		for(int beamIndex = 0; beamIndex < NUM_BEAMS; beamIndex++)
		{

			// if there is information stored in the beam queue, proceed to analyze
			if(!BeamBuffer[beamIndex].empty())
			{
				curBeam = BeamBuffer[beamIndex].front();
				curBeamTime = curBeam.getTime();

				// micropulse structure is empty, assume new beam is part of a macro
				if(microStructure.size() == 0)
				{
					microStructure.push(curBeam);
					BeamBuffer[beamIndex].pop();
				}

				// a macro already exists, compare current beam to the existing one
				else
				{
					firstMicro = microStructure.front().getTime();
					lastMicro = microStructure.back().getTime();
					timeDiffMacro = 0;
					timeDiffMicro = 0;

					while(!BeamBuffer[beamIndex].empty())
					{
						timeDiffMacro =  curBeamTime - firstMicro;

						if(timeDiffMacro < MACRO_SEP) // within macro pulse
						{
							timeDiffMicro = curBeamTime - lastMicro;
							h_microTimeDiff->Fill(timeDiffMicro);
							microStructure.push(curBeam);
						}

						else // next macropulse, now analyze the present microstructure
						{

							h_macroTimeDiff->Fill(timeDiffMacro);

							timeHead = microStructure.front().getTime(); // this is the last micro in the macropulse

							h_macroPop->Fill(microStructure.size()); // population of macropulse

							// if(microStructure.size() > MICRO_NUM)
							// {
							// 	microStructureCoinc = microStructure;
							// }
							microIndex = 0;
							while(!microStructure.empty())
							{
								qBeam = microStructure.front();
								qBeam.microIndex = microIndex; microIndex++;

								microTime = qBeam.getTime() - timeHead;
								h_microTime->Fill(microTime);

								ValidBeamBuffer.push(qBeam);
								microStructure.pop();

							}

							microStructure.push(curBeam);
						}

						BeamBuffer[beamIndex].pop();
						// cout << "B" << endl;
					}
				}

			}

		}


		//  _____    _
		// |_   _| _(_)__ _ __ _ ___ _ _
		//  | || '_| / _` / _` / -_) '_|
		//  |_||_| |_\__, \__, \___|_|
		// 					 |___/|___/

		// if trigger is not split, check if you can store in a fission.
		// This is the standard operational mode
		if(!TRIGGER_SPLIT)
		{
			// check whether all trigs queues are populated
			bool allFill = true;

			while(allFill)
			{
				for(int trigIndex = 0; trigIndex < NUM_TRIGGERS; trigIndex++)
				{
					if(TriggerBuffer[trigIndex].empty())
					{
						allFill = false;
						// cout << trigIndex << " " << TriggerBuffer[trigIndex].empty() <<  endl;
					}
				}

				// find minimum trigger
				if(allFill)
				{
					// cout << "oh hello" << endl;
					int recentIndex = 0;
					for(int trigIndex=0; trigIndex<NUM_TRIGGERS; trigIndex++)
					{
						if(TriggerBuffer[trigIndex].front().getTime() < TriggerBuffer[recentIndex].front().getTime())
						{
						   recentIndex = trigIndex;
						}
					}

					newFission = CoincidenceEvent(TriggerBuffer[recentIndex].front().getTime(),
					 															TriggerBuffer[recentIndex].front().getEnergy(),
																				TriggerBuffer[recentIndex].front().getDetector(),
																				TriggerBuffer[recentIndex].front().getPsp(),
																		   	TriggerBuffer[recentIndex].front().getTail());


					// we have found the first fission event, check whether this can be put in coincidence with beam

					ValidTriggerBuffer.push(newFission);

					TriggerBuffer[recentIndex].pop();
					// cout << "popped, now " << recentIndex << ": " << TriggerBuffer[recentIndex].size() << endl;
				}

				// cout << "T" << endl;
			}
		}

		// if trigger split, check if you can form a coincidence between the triggers
		else if(TRIGGER_SPLIT)
		{

			trigReady = true;
			pastRef = false;

			if(!TriggerBuffer[refTrig].empty())
			{
				refTrigTime = TriggerBuffer[refTrig].front().getTime();

				// cycle through the other triggers and make sure that they are empty
				for(int trigIndex = 1; trigIndex < NUM_TRIGGERS; trigIndex++)
				{
					// check whether more data needs to be read in
					if(!TriggerBuffer[trigIndex].empty())
					{
						compTrigTime = TriggerBuffer[trigIndex].front().getTime();
					}

					// eliminate bad coincidence events
					while( (!TriggerBuffer[trigIndex].empty() ) &  (refTrigTime - compTrigTime > MAX_TRIGGER_DRIFT) )
					{
						TriggerBuffer[trigIndex].pop();
						compTrigTime = TriggerBuffer[trigIndex].front().getTime();
					}

					// check whether more data needs to be read in
					if(TriggerBuffer[trigIndex].empty())
					{
						// split fission not reached yet
						trigReady = false;
					}

					// coincidence can be formed
					else if(abs(compTrigTime - refTrigTime) < MAX_TRIGGER_DRIFT)
					{
						// h_Dt->Fill(compTrigTime - refTrigTime);
						trigReady = true; // does this work, or does it cancel 2/3
					}

					// reference channel is lagging
					else if(compTrigTime - refTrigTime > MAX_TRIGGER_DRIFT)
					{
						trigReady = false;
						pastRef = true;
					}
				}
			}

			// if the time is right for coincidence, create a new fission
			if(trigReady)
			{
				newFission = CoincidenceEvent(TriggerBuffer[refTrig].front().getTime(),
				 															TriggerBuffer[refTrig].front().getEnergy(),
																			TriggerBuffer[refTrig].front().getDetector(),
																			TriggerBuffer[refTrig].front().getPsp(),
																		  TriggerBuffer[refTrig].front().getTail());
				ValidTriggerBuffer.push(newFission);
				TriggerBuffer[refTrig].pop();
			}

			// if the reference channel is outdated, pop and proceed
			else if(pastRef)
			{
				TriggerBuffer[refTrig].pop();
			}

		}

//  ___ _           ___                  __   __    _ _    _      _   _
// | __(_)_________| _ ) ___ __ _ _ __   \ \ / /_ _| (_)__| |__ _| |_(_)___ _ _
// | _|| (_-<_-<___| _ \/ -_) _` | '  \   \ V / _` | | / _` / _` |  _| / _ \ ' \
// |_| |_/__/__/   |___/\___\__,_|_|_|_|   \_/\__,_|_|_\__,_\__,_|\__|_\___/_||_|


		// At the end of the trigger loop we have found a valid, chronological fission event

		// if no beam are present, just set fission events to loop events
		if(NUM_BEAMS == 0)
		{
			// depopulate the valid trigger and populate the fission trigger
			while(ValidTriggerBuffer.size() > 0)
			{
				curFis = ValidTriggerBuffer.front();
				FissionBuffer.push(curFis);
				ValidTriggerBuffer.pop();
			}
		}

		// if beams are present, compare the earliest events
		else
		{
			// cout << ValidBeamBuffer.size() << " " << ValidTriggerBuffer.size() << endl;

			while(!ValidBeamBuffer.empty() & !ValidTriggerBuffer.empty())
			{
				// cout << ValidBeamBuffer.size() << " " << ValidTriggerBuffer.size() << endl;

				// get the front of the queue
				curFis = ValidTriggerBuffer.front();
				curBeam = ValidBeamBuffer.front();;

				// get the times
				curFisTime = curFis.triggerTime;
				curBeamTime = curBeam.getTime();

				deltaFissBeam = curFisTime - curBeamTime;
				// if the background is to be characterized, turn on this function and produce a random signal to characterize background
				if(RANDOM_COINCIDENCE == 1)
				{
					deltaFissBeam = randomTimeCoinc->Uniform(-1*BEAM_WINDOW, BEAM_WINDOW);

					if(ValidBeamBuffer.empty())
					{
						cout << "matched fissions to beams." << endl;
						while(ValidTriggerBuffer.size() > 0)
						{
							FissionBuffer.pop();
						}
						break;
					}


				}

				// cout << deltaFissBeam << endl;

				if(deltaFissBeam >= BEAM_WINDOW)
				{
						// the beam store in memory did not trigger a fission reaction
						h1_fissNumPerBeam->Fill(numFissBeam);
						numFissBeam = 0;
						ValidBeamBuffer.pop();
				}

				else if(deltaFissBeam <= -1*BEAM_WINDOW)
				{
					// the fission stored in memory does not belong to a beam event
          h_alphaSpec->Fill(curFis.triggerEnergy);
          h_alphaTime->Fill(deltaFissBeam);
          h2_alphaTimeSpec->Fill(deltaFissBeam, curFis.triggerEnergy);
          h2_alphaTimeChan->Fill(deltaFissBeam, curFis.triggerChannel);

          if (deltaFissBeam < -50000 && deltaFissBeam > -150000) // Replace hard-coded values
          {
            h2_alphaChanSpec->Fill(curFis.triggerChannel, curFis.triggerEnergy);
          }

					ValidTriggerBuffer.pop();
				}

				else if(abs(deltaFissBeam) < BEAM_WINDOW)
				{
					// the fission reaction is associated with the current beam and a new
					// accepted fission is created

					goodFis = CoincidenceEvent(curFis.triggerTime,
					 													 curFis.triggerEnergy,
																		 curFis.triggerChannel,
																		 curFis.triggerPSP,
																		 curFis.triggerTail);

					goodFis.beamTime = deltaFissBeam;
					goodFis.beamEnergy = curBeam.getEnergy();
					goodFis.beamPSP = curBeam.getPsp();
					goodFis.beamChannel = curBeam.getDetector();
					goodFis.beamMicroIndex = curBeam.getMicroIndex();

          h_fisSpec->Fill(curFis.triggerEnergy);


					if(RANDOM_COINCIDENCE == 1)
					{
						goodFis.triggerTime = deltaFissBeam + curBeam.getTime();
						goodFis.triggerEnergy = FAKE_FISS_SIGNAL; // set this so to create
						if(!ValidBeamBuffer.empty())
						{
							ValidBeamBuffer.pop();
						}
						else
						{
							break;
						}
					}

					FissionBuffer.push(goodFis);
					ValidTriggerBuffer.pop(); // discard only the fission, there could be several fiss for same beam.

					h1_fissTimeInBeam->Fill(goodFis.beamTime);
					numFissBeam++;
				}

				else
				{
					cout << "Computation failed on " << deltaFissBeam << endl;
					exit(4);
				}

				// cout << "FB" << endl;
			}
		}






		//  ___ _       _
		// | __(_)_____(_)___ _ _
		// | _|| (_-<_-< / _ \ ' \
		// |_| |_/__/__/_\___/_||_|
		//

		// before we can fill the detectors, we need to make sure that there are
		// enough events to start populating the fissions
		bool readyDet = true;
		// look at the detection events
		for(int detIndex = 0; detIndex < NUM_DETS; detIndex++)
		{
			// assign detection time
			if(DetectorBuffer[detIndex].empty())
			{
				readyDet = false;
				// cout << detIndex << endl;
			}
		}

		// cout << FissionBuffer.size() << endl;

		// if the detector buffers are ready, proceed to go through the fission and check
		// whether there are particle-trigger coincidences
		while(!FissionBuffer.empty() & readyDet)
		{

			if(FRAGMENT_MODE)
			{
				if(doneFrag) break;
			}


			// cout << "picking up new events" << endl;

			// cout << "F" << endl;

			totMult = 0;

			// reset all the indices to 0
			for(int jMult = 0; jMult < MAX_MULTIPLICITY; jMult++)
			{
				totToF[jMult] = 0;
				totDep[jMult] = 0;
				totPSP[jMult] = 0;
				totChan[jMult] = 0;
			}

			// assign fission event from first list
			qFission = FissionBuffer.front();

			// assign the detector channel
			fissionChan = qFission.getTriggerChan();
			fissionTime = qFission.getTriggerTime();
			fissionEnergy = qFission.getEnergy();
			fissionPSP = qFission.getTriggerPSP();
			fissionTail = qFission.getTriggerTail();


			beamTime = qFission.getBeamTime();
			beamChan = qFission.getBeamChan();
			beamEnergy = qFission.getBeamEnergy();
			beamPSP = qFission.getBeamPSP();
			beamIndex = qFission.getBeamIndex();
			beamTail = qFission.getBeamTail();

			// cout << FissionBuffer.size() << endl;

			// cout << "forming fissiona" << endl;

			// look at the detection events
			for(int detIndex = 0; detIndex < NUM_DETS; detIndex++)
			{
				// assign detection time
				if(!DetectorBuffer[detIndex].empty())
				{
					compDetTime = DetectorBuffer[detIndex].front().getTime();
					// cout << fissionTime << " - " << compDetTime << " = " << fissionTime - compDetTime <<  endl;
				}

				// eliminate bad coincidence events
				while((!DetectorBuffer[detIndex].empty() ) & (fissionTime - compDetTime > COINC_WINDOW) )
				{
					// cout << fissionTime - compDetTime << endl;
					DetectorBuffer[detIndex].pop();
					compDetTime = DetectorBuffer[detIndex].front().getTime();
					// cout << "time checked" << endl;
				}

				// if the next event is empty, we don't know for sure if it was in coincidence, back to the drawing board

				if(DetectorBuffer[detIndex].empty())
				{
					readyDet = false;
					//cout << detIndex << endl;
				}

			}

			// now we have made sure that all the detectors contributed an event if they had one

			if(readyDet)
			{

				for(int detIndex = 0; detIndex < NUM_DETS; detIndex++)
				{
					qParticle = DetectorBuffer[detIndex].front();
					detTime = qParticle.getTime();
					deltaT = detTime - fissionTime;

					// cout << deltaT << endl;

					// create the coincidence event
					if(abs(deltaT) < COINC_WINDOW)
					{
						totToF[totMult] = deltaT;
						totPSP[totMult] = qParticle.getPsp();
						totDep[totMult] = qParticle.getEnergy();
						totTail[totMult] = qParticle.getTail();
						totChan[totMult] = qParticle.getDetector();
						totMult++;
					}
				}

				// get rid of analyzed fission event
				FissionBuffer.pop();
				// cout << "pop" <<  FissionBuffer.size() << endl;


				// now fill the histogram of particle-particle coincidences
				tMult = totMult;
				tTime = fissionTime;
				tDep = fissionEnergy;
				tPSP = fissionPSP;
				tChan = fissionChan;
				tTail = fissionTail;

				if(NUM_BEAMS > 0)
				{
					bErg = beamEnergy;
					bPSP = beamPSP;
					bChan = beamChan;
					bIndex = beamIndex;
					bTime = beamTime;
					bTail = beamTail;
				}

				fissNcount++;


				if(FRAGMENT_MODE == 1)
				{
					// fragTreeChain->GetEntry(fragEntry);

					// cout << fT - tTime << endl;
					// if(fT > 3e13) cout << tTime << " " << fT << " " << fragEntry << endl;

					fragTreeChain->GetEntry(fragEntry-1);
					oldTime = fT;
					fragTreeChain->GetEntry(fragEntry);

					// cout << tTime -  fT << " " << fragEntry << endl;

					if(fT - oldTime > 1000*FISS_RATE)
					{
						// cout << "discontinuation in time found at " << fT << ", " << fT - oldTime << endl;
						fragEntry++;
						fragTreeChain->GetEntry(fragEntry);
						h1_lostFrags->Fill(4);

					}
					// store the time difference from last event
					h1_fissFragDelt->Fill(fT - tTime);

					// eliminate bad events
					if(!(fT >= 0))
					{
						fragEntry++;
						// cout << "uh oh: " << fT << endl;
						// fragTreeChain->LoadTree(fragEntry);
						fragTreeChain->GetEntry(fragEntry);
						h1_fragSorting->Fill(fT-oldTime);
						h1_lostFrags->Fill(0);
					}

					// compute the time difference
					// tDiff = fT - tTime;
					// cout << tDiff << endl;

					//
					while(fT - tTime < -1*FRAGMENT_WINDOW)
					{
						// cout << "pruning" << endl;
						// if(fragEntry >= fragTreeChain->GetEntries()) doneFrag = true; break;
						fragEntry++;
						// fragTreeChain->LoadTree(fragEntry);
						fragTreeChain->GetEntry(fragEntry);
						h1_fragSorting->Fill(fT-oldTime);
						h1_lostFrags->Fill(1);
					}

					if(abs(double(fT - tTime)) <= FRAGMENT_WINDOW )
					{
						// cout << tTime << " " << fT << " " << fragEntry << endl;
						// cout << "fragment coincidence found: " << (fT - tTime) << endl;
						// cout << "coinc" << endl;
						rAL = fAL;
						rAH = fAH;
						rKEL = fKEL;
						rKEH = fKEH;
						rThetaL = fThetaL;
						rThetaH = fThetaH;
						rEX = fEX;
						rDelt = fT - tTime;

						rAn1 = fAn1;
						rAn2 = fAn2;
						rGr1 = fGr1;
						rGr2 = fGr2;
						rCat = fCat;

						rA1 = fA1;

						// cout << "in: " << fT - tTime << endl;

						fragEntry++;
						// fragTreeChain->LoadTree(fragEntry);
						fragTreeChain->GetEntry(fragEntry);
						h1_fragSorting->Fill(fT-oldTime);

						// cout << fragEntry << endl;


					}

					else if (fT - tTime > FRAGMENT_WINDOW)
					{
						// cout << "weird event with tdiff: " << fT-tTime << endl;
						// cout << "catching up" << endl;
						// fragEntry++;
						// check for discontinuities
						// cout << "no frag" << endl;

						noFrag = true;
						h1_lostFrags->Fill(2);
						// cout << "out: " << fT - tTime << endl;
						// continue;
					}
					else
					{
						// cout << "weird event with tdiff: " << fT-tTime << endl;
						noFrag = true;
						fragEntry++;
						// fragTreeChain->LoadTree(fragEntry);
						fragTreeChain->GetEntry(fragEntry);
						h1_lostFrags->Fill(3);
						continue;
					}

					if(fragEntry >= fragTreeChain->GetEntries() - 10)
				  {
						cout << "finished fragments" << endl;
						doneFrag = true;
					}


					// cout << fragEntry << "/" << fragTreeChain->GetEntries() << endl;



					// cout << fragEntry << endl;

					// finished the fragment tree
					// if(fragEntry > fragTreeChain->GetEntries()) break;

					// tTime = fT;
				}

				// cout << "A" << endl;

				// fill the tree branches

				if(!(tTime >= 0))
				{
					cout << "problem with fragment time at " << tTime << endl;
					continue;
				}

				// fill event
				if(FRAGMENT_MODE == 1)
				{
					if(!noFrag)
					{
						// cout << "coinc" << endl;
						coincTree->Fill();
						fisTracker++;
					}
					else
					{
						// cout << "noFrag" << endl;
						noFrag = false;
					}
				}
				else // not in fragment mode
				{
					coincTree->Fill();
					fisTracker++;
				}

				// cout << tTime << " " << tMult << " " << tDep << " " << tPSP << " " << tChan << endl;

				// cout << fisTracker << endl;
				// update user on status of processing
				if(fisTracker%100000 == 0)
				{
					cout << fisTracker << " fissions in " << tTime/1e9 << " seconds " << endl;
				}

			}

		 }

		}
		cout << "Finished looping through data. " << endl;

		if(NUM_BEAMS > 0)
		{

      beamFile->cd();
			h_microTimeDiff->Write();
			h_macroTimeDiff->Write();
			h_microTime->Write();
			h_macroPop->Write();
      h_alphaSpec->Write();
      h_fisSpec->Write();
      h_alphaTime->Write();

      h2_alphaTimeSpec->Write();
      h2_alphaChanSpec->Write();
      h2_alphaTimeChan->Write();

      h_triggerTime->Write();
      h_beamTime->Write();
      h2_triggerTimeChan->Write();
			h3_fileDesc->Write();

			h1_fissNumPerBeam->Write();
			h1_fissTimeInBeam->Write();

			cout << "Written histograms to beam info file." << endl;
		}





	 //   _____             _               _____        _
	 //  / ____|           (_)             |  __ \      | |
	 // | (___   __ ___   ___ _ __   __ _  | |  | | __ _| |_ __ _
	 //  \___ \ / _` \ \ / / | '_ \ / _` | | |  | |/ _` | __/ _` |
	 //  ____) | (_| |\ V /| | | | | (_| | | |__| | (_| | || (_| |
	 // |_____/ \__,_| \_/ |_|_| |_|\__, | |_____/ \__,_|\__\__,_|
	 //                              __/ |
	 //                             |___/


	// cd back into the main file
	cout << "Saving the tree to file. " << endl;

	cout << "In total: " << fisTracker << " fissions formed. " << endl;
	cout << "of which " << fissNcount << " without enforcing fragment coincidence" << endl;

	expFile = coincTree->GetCurrentFile();
	expFile->Write();

	if(FRAGMENT_MODE)
	{
		expFile->cd();
		h1_fissFragDelt->Write();
		h1_fragSorting->Write();
		h1_lostFrags->Write();
	}

	numCoincFiles = coincTree->GetFileNumber();

	cout << "Tree has been written " << endl;

	// expFile->Close();

  delete beamFile;


	// coincTree->Write();
	//expFile->Close();
	//h_Dt->Write();

	return 1;
}
