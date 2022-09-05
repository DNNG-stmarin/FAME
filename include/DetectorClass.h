/*
Author: Stefano Marin, Isabel Hernandez
Purpose: Class with all of the properties of the detection system

Date: Ann Arbor, MI, May 3rd, 2020
*/

#ifndef DetectorClass_h
#define DetectorClass_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TF1.h>

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


class DetectorClass
{
private:

public:

	TString name = "det";

	int type;

	double energyThreshold; // threshold in keVee

	double* timeDelay; // store the time delay with respect to the trigger(s)
	double* timeResolution; // store the time resolution from the fitted gamma peak


	double energyResolution; // store the energy resolution of this detector

	TF1* discPSD; // psd discrimination line
	TF1* meanNeut;
	TF1* meanPhot;
	TF1* sigNeut;
	TF1* sigPhot;

	// double discrimination lines
	TGraph* discPSDNeut;
	TGraph* discPSDPhot;

	// double discTOFPoint; //tof discrimination
	//TF1* discPSDPoint; //psd discrimination //have just one s

	double calibration; // store the energy calibration for each detector (was called energyCalibration)

	double efficiency; // efficiency of this detector

	double X, Y, Z; // position of the center of the detector
	double distance; // distance of detector from source

};

#endif
