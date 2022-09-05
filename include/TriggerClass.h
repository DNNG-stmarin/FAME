/*
Author: Stefano Marin, Isabel Hernandez
Purpose: Class with all of the properties of the triggering system

Date: Ann Arbor, MI, May 3rd, 2020
*/

#ifndef TriggerClass_h
#define TriggerClass_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


class TriggerClass
{
public:
  TString name = "trig";

  double crossSection;

  double energyResolution;
  double timeResolution;

  double beamDelay = 0; // keeps track of the delay of the fission triggers with respect to the beam
  double backgroundActivity = 0; // activity before beam strikes


  double calibration; // store the energy calibration for each detector (was called energyCalibration)

  double efficiency; // efficiency of this detector

  double X, Y, Z; // position of the center of the trigger
  // double offset; // distance of detector from source
  double U, V, W; // direction of detector assembly

  TF1* fisBeamActivity;

};

#endif
