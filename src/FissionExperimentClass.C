/*
Author: Stefano Marin, Isabel Hernandez
Date: May 1st, 2020
Purpose: Methods of the Fission Experiment Class
*/

#define FissionExperimentClass_cxx

#include "FissionExperimentClass.h"

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TCut.h>
#include <TFitResult.h>
#include <THStack.h>
#include <TFolder.h>
#include <TSystem.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <queue>

#include "ParticleEvent.h"
#include "TriggerEvent.h"
#include "InfoSystem.h"

using namespace std;


FissionExperimentClass::FissionExperimentClass(TString inputFileName)
{
	//call info System
	info = new InfoSystem;
	cout << "reading input file" << endl;
	info->ReadInput(inputFileName);

	nameOfExp = info->nameOfExp;
	MIN_FILE = info->MIN_FILE;
	NUM_FILES = info->NUM_FILES;
	DATA_TYPE = info->DATA_TYPE;
	REUSE_DATA = info->REUSE_DATA;
	PARAM_FILES = info->PARAM_FILES;

	FILE_LIST_MODE = info->FILE_LIST_MODE;
	FILE_LIST = new int[NUM_FILES];
	RANDOM_COINCIDENCE = info->RANDOM_COINCIDENCE;

	REUSE_DETECTOR = info->REUSE_DETECTOR;
	FISSION_MODE = info->FISSION_MODE;
	DEBUG = info->DEBUG;

	NUM_BEAMS = info->NUM_BEAMS;

	FRAGMENT_MODE = info->FRAGMENT_MODE;
	FRAGMENT_PATH = info->FRAGMENT_PATH;
	REMOVE_FLAG = info->REMOVE_FLAG;


	if(REMOVE_FLAG > 0)
	{
		cout << "* REMOVING SIMULATED FLAGGED EVENTS WITH " << REMOVE_FLAG << endl;
	}

	cout << "Advanced modes being instructed" << endl;
	
	resultFold = new TFolder(nameOfExp, nameOfExp);

	if(REUSE_DATA == 0) {
        cout << "creating new coincidences." << endl;
		expFile = new TFile(treeFileT + rootEnding, "RECREATE");
	}
	else {
        cout << "reading old coincidences." << endl;
		expFile = new TFile(treeFileT + rootEnding, "READ");
	}
/////
	//if reuse then read, otherwise recreate (only detFile)
	cout << "about to start detector import" << endl;
	if(REUSE_DETECTOR == 1){
		cout << "in the detector import loop" << endl;
		detFile = new TFile(detFileT, "READ");
	}
	else if(REUSE_DETECTOR == 0){
		detFile = new TFile(detFileT, "RECREATE");
	}
	else{
	}
	if(NUM_BEAMS > 0)
	{
		if(REUSE_DATA == 0) beamFile = new TFile(beamFileT, "RECREATE");
		// else beamFile = new TFile(beamFileT, "READ");
	}

  // create the chain with all the entries to analyze for the raw coincidence mode
	rawTreeChain = new TChain();
  coincTreeChain = new TChain();
}

// destructor closes all the remaining loose ends
FissionExperimentClass::~FissionExperimentClass()
{
	delete info;
}

// Now go ahead and create all the fission tree
int FissionExperimentClass::CreateCoincidenceTree(TString filename, TFile* expFileWrite, int numEntries)
{

	// create a file list to iterate over
	if(FILE_LIST_MODE == 1)
	{
		cout << "Using defined file numbers. " << endl;
		FILE_LIST = info->FILE_LIST;
	}
	else
	{
		cout << "Using consecutive file numbers. " << endl;
		int currFile = MIN_FILE;
		for(int i = 0; i < NUM_FILES; i++)
		{
			FILE_LIST[i] = currFile;
			currFile++;
		}
	}

	// attach the data file to the chain
	if(REUSE_DATA == 0)
	{
		cout << "Writing coincidence trees to " << expFileWrite->GetName() << "." << endl;

		// iterator for adding data to the fchain
		TFile *fileRaw;
		TTree *treeRaw;
		TString s_fileRaw;

		// set the input tree
		if(DATA_TYPE == 0)
		{
			 inputTreeName = compassName;
		}
		else if(DATA_TYPE == 1)
		{
			 inputTreeName = midasName;
		}

		for(int i = 0; i < NUM_FILES; i++)
		{
			 int fileNum = FILE_LIST[i];

			 cout << "reading file number " << fileNum << endl;
			 // find the file
			 s_fileRaw =  filename + TString(to_string(fileNum)) + extExpFile;
       fileRaw = (TFile*)gROOT->GetListOfFiles()->FindObject(s_fileRaw);
       if (!fileRaw || !fileRaw->IsOpen()) {
          fileRaw = new TFile(s_fileRaw);
       }
       fileRaw->GetObject(inputTreeName, treeRaw);

			 if(treeRaw != 0)
			 {
	 			 rawTreeChain->Add(s_fileRaw + "/" + inputTreeName);
				 cout << "tree found at " << treeRaw << " added to chain" << endl;
			 }

			 //fileRaw->Close();
			 delete fileRaw;
		}

		CoincidenceAnalysis* inputData = new CoincidenceAnalysis(expFileWrite, rawTreeChain, beamFile, info); //replace digType w info->DATA_TYPE
		inputData->CreateCoincidenceTree(numEntries);
	}

	gROOT->cd();

	cout << "Reading coincidence tree from " << treeFileT + rootEnding + "/" + nameCoincTree << endl;

	coincTreeChain->Add(treeFileT + rootEnding + "/" + nameCoincTree);
	bool fileFound = true;
	int fileNum = 1;

	while(gSystem->AccessPathName(treeFileT + "_" + to_string(fileNum) + rootEnding) == false)
	{
		coincTreeChain->Add(treeFileT + "_" + to_string(fileNum) + rootEnding + "/" + nameCoincTree);
		cout << "And file " << treeFileT + "_" + to_string(fileNum) + rootEnding + "/" + nameCoincTree << endl;
		fileNum++;
	}
	cout << "Completed reading from " << fileNum << endl;
	return 1;
}

int FissionExperimentClass::CreateDetectionAnalysis()
{

	cout << "Analyzing " << coincTreeChain->GetEntries() << " events." << endl;
	// cout << "tree: " << coincTreeChain << endl;
	// cout << "detFile: " << detFile << endl;
	// cout << "info: " << info << endl;
	detectorData = new DetectorSystemClass(coincTreeChain, detFile, info);

	cout << "Creating the functions to analyze the data. " << endl;
	detectorData->InitializePSDFunctions();

	cout << "Creating the histograms to store the data. " << endl;
	detectorData->InitializeDetectorHistograms();

	cout << "Entering detector analysis mode" << endl;

	if(REUSE_DETECTOR == 1){
	detectorData->DetectionImport();
	}
	else if(REUSE_DETECTOR == 0){
	detectorData->DetectionAnalysis();
	}
	else{
	}

	// V
	if(PARAM_FILES == 1){
		detectorData->ParamFiles();
	}


	if(FISSION_MODE == 1)
	{
		cout << "Entering fission analysis mode" << endl;
		if(DEBUG==1){
			cout << "debug is on" << endl;
			detectorData->FissionAnalysisLoop();
		}
		else{
			cout << "starndard fission analysis starting" << endl;
			detectorData->FissionAnalysis();
		}
		}
		return 1;

}

// getSystemInfo
int FissionExperimentClass::getStartFile()
{
	return MIN_FILE;
}

int FissionExperimentClass::getEndFile()
{
	return MIN_FILE + NUM_FILES;
}

void FissionExperimentClass::saveAll()
{
	// expFile->Close();
	// detFile->Close();
	delete expFile;
	delete detFile;
  // Close more things?
}

TString FissionExperimentClass::getExpName()
{
	return nameOfExp;
}
