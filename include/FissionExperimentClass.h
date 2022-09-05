/*
Author: Stefano Marin, Isabel Hernandez
Purpose: Class that analyzes experiments in fission from raw data to observed correlations.

The object contains several classes each representing a stage of the analysis process.

Date: late April 2020, Ann Arbor, Michigan
*/

#ifndef FissionExperimentClass_h
#define FissionExperimentClass_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "CoincidenceAnalysis.h"
#include "DetectorSystemClass.h"

using namespace std;


class FissionExperimentClass
{

private:
	TString nameOfExp;
	int MIN_FILE = 0;
	int NUM_FILES = 1;

	int DATA_TYPE;
	int REUSE_DATA;
	int REUSE_DETECTOR;
	int DEBUG;
	int FISSION_MODE;
	int PARAM_FILES;

	int FRAGMENT_MODE;
	TString FRAGMENT_PATH;

	int RANDOM_COINCIDENCE;

	int FILE_LIST_MODE;
	int* FILE_LIST;

	int NUM_BEAMS;

	double REMOVE_FLAG;

	TString nameExpFile = "fiss";
	TString extExpFile = ".root";

	TString compassName = "Data_F";
	TString midasName = "midas_data";

	TString inputTreeName;

	TString nameCoincTree = "CoincidenceTree"; // coincidence pre-clean tree
	TString nameFissionTree = "FissionTree"; // fission post-cleaning tree

	// names of files to write
	TString treeFileT = "CoincidenceTrees";
	TString rootEnding = ".root";
	TString detFileT = "Detectors.root";
	TString sysFileT = "Fission.root";
  TString beamFileT = "BeamInfo.root";

public:

	/*
   	     _   _       _ _         _
	 __ _| |_| |_ _ _(_) |__ _  _| |_ ___ ___
	/ _` |  _|  _| '_| | '_ \ || |  _/ -_|_-<
	\__,_|\__|\__|_| |_|_.__/\_,_|\__\___/__/
	*/

	// folder which contains all the results
	TFolder* resultFold = 0;

	// file to write all results
	TFile* expFile = 0;
	TFile* detFile = 0;
	TFile* beamFile = 0;
	// TFile* fragFile = 0;

	// pointer attributes
	CoincidenceAnalysis* inputData;
	DetectorSystemClass* detectorData;

	// the information about the system
	InfoSystem* info;

	// chain of raw tree files
	TChain* rawTreeChain = 0;
	TChain* coincTreeChain = 0;


/*
	           _   _            _
	 _ __  ___| |_| |_  ___  __| |___
	| '  \/ -_)  _| ' \/ _ \/ _` (_-<
	|_|_|_\___|\__|_||_\___/\__,_/__/

*/

	// class constructor
	FissionExperimentClass(TString inputfilename);
	~FissionExperimentClass();

	// operations
	int CreateCoincidenceTree(TString filename, TFile* expFile, int numEntries = - 1);
	int CreateDetectionAnalysis();

	void saveAll();

	// get attributes
	int getStartFile();
	int getEndFile();
	TString getExpName();
};

#endif
