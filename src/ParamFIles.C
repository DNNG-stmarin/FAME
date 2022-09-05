/*
Author: Stefano Marin, Katie Ballard
Purpose: Output the data on the fits of the parameters so that they can be used for simulation.
*/

#define DetectionAnalysis_cxx

#include "DetectorSystemClass.h"

#include <TH3.h>
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TCut.h>
#include <TFitResult.h>
#include <THStack.h>

#include <TProfile.h>
#include <TGraph.h>
#include <TLine.h>
#include <TFormula.h>

#include <stdlib.h>
#include <stdio.h>
#include <queue>

#include <iostream>
#include <fstream>

#include "ParticleEvent.h"
#include "TriggerEvent.h"
#include "ProcessingConstants.h"

using namespace std;

// stores the parameters for the parameter fits into seperate text documents
int DetectorSystemClass::ParamFiles()
{

cout << "Writing parameter files" << endl;

// creates the output files and opens them
ofstream meanNeutFile;
meanNeutFile.open("meanNeutParams");
ofstream meanPhotFile;
meanPhotFile.open("meanPhotParams");
ofstream sigNeutFile;
sigNeutFile.open("sigNeutParams");
ofstream sigPhotFile;
sigPhotFile.open("sigPhotParams");

// loops through and saves the data for each detector
for(int det = 0; det < NUM_DETS; ++det)
{
    if(detectors[det].type == ORGANIC)
    {
      meanNeutFile << det << " " << detectors[det].meanNeut->GetParameter(0) << " " << detectors[det].meanNeut->GetParameter(1)  << " " << detectors[det].meanNeut->GetParameter(2)  << " " << detectors[det].meanNeut->GetParameter(3)  << endl;
      meanPhotFile << det << " " << detectors[det].meanPhot->GetParameter(0) << " " << detectors[det].meanPhot->GetParameter(1)  << " " << detectors[det].meanPhot->GetParameter(2)  << " " << detectors[det].meanPhot->GetParameter(3)  << endl;
      sigNeutFile << det << " " << detectors[det].sigNeut->GetParameter(0) << " " << detectors[det].sigNeut->GetParameter(1)  << " " << detectors[det].sigNeut->GetParameter(2)  << " " << detectors[det].sigNeut->GetParameter(3)  << endl;
      sigPhotFile << det << " " << detectors[det].sigPhot->GetParameter(0) << " " << detectors[det].sigPhot->GetParameter(1)  << " " << detectors[det].sigPhot->GetParameter(2)  << " " << detectors[det].sigPhot->GetParameter(3)  << endl;
    }
    else
    {
      meanNeutFile << det << " " << 0;
      meanPhotFile << det << " " << 0;
      sigNeutFile << det << " " << 0;
      sigPhotFile << det << " " << 0;
    }

}

// closes the files
meanNeutFile.close();
meanPhotFile.close();
sigNeutFile.close();
sigPhotFile.close();

return 1;

}
