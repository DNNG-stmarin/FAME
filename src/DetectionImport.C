/*
Author: Stefano Marin, Katie Ballard
Purpose: Perform the analyisis of the collected data on a detector-by-detector basis.
The result of this code will be a series of cuts to clean the data.
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

int DetectorSystemClass::DetectionImport()
{
cout << "Detection Import Start" << endl;


/*

_______   _
|__   __| (_)
  | |_ __ _  __ _  __ _  ___ _ __ ___
  | | '__| |/ _` |/ _` |/ _ \ '__/ __|
  | | |  | | (_| | (_| |  __/ |  \__ \
  |_|_|  |_|\__, |\__, |\___|_|  |___/
             __/ | __/ |
            |___/ |___/

*/

TString fileNameTrigBeamBase = "Beam/beamTimeTrig";
TString fileNameTrigBeam;

for(int trig = 0; trig < NUM_TRIGGERS; trig++)
{
  if(NUM_BEAMS > 0)
  {
    cout << "importing beam fiss" << endl;

    // discrmPSD
    // create a canvas to store the data being accessed
    TCanvas* c_trigBeam;
    TF1* f_trigBeam;

    // sets the file name to the base name and the detector
    fileNameTrigBeam =  fileNameTrigBeamBase + (TString)to_string(trig);

    cout << "reading from " << fileNameTrigBeam << endl;

    //sets the canvas equal to the psdErg one for that detector
    c_trigBeam = (TCanvas*)detFile->Get(fileNameTrigBeam);
    f_trigBeam = (TF1*)c_trigBeam->GetPrimitive("fisBeamActivity");

    triggers[trig].backgroundActivity = f_trigBeam->GetParameter(0);
    triggers[trig].beamDelay = f_trigBeam->GetParameter(1);
  }
}

cout << "importing detectors" << endl;



// defines varibles for the file name of the canvases being accessed
// PSD
TString fileNameBasePsd= "PSD/PSDErg_discrimination/psdErg";
TString fileNamePsd;

// TOF
TString fileNameStartTOF= "TOF/TOF_individual/ToFDiscrimination";
TString fileNameEndTOF = "trig";
TString fileNameTOF;

// Double Discrim
TString fileNameBasePsdParam = "PSD/PSD_parameters/param";
TString fileNamePsdParam;


/*

_____       _            _
|  __ \     | |          | |
| |  | | ___| |_ ___  ___| |_ ___  _ __ ___
| |  | |/ _ \ __/ _ \/ __| __/ _ \| '__/ __|
| |__| |  __/ ||  __/ (__| || (_) | |  \__ \
|_____/ \___|\__\___|\___|\__\___/|_|  |___/



*/

// loops through all of the detectors
for(int det = 0; det < NUM_DETS; ++det)
{

    // TOF
    for(int trig = 0; trig < NUM_TRIGGERS; ++trig)
    {
        // create a canvas to store the data being accessed
        TCanvas* c_TOF;
        TF1* f_TOF;
        // sets the file name to the base name and the detector
        fileNameTOF =  fileNameStartTOF + (TString)to_string(det) + fileNameEndTOF + (TString)to_string(trig);
        //sets the canvas equal to the psdErg one for that detector
        c_TOF = (TCanvas*)detFile->Get(fileNameTOF);
        f_TOF = (TF1*)c_TOF->GetPrimitive("peakFit");
        //time delay
        detectors[det].timeDelay[trig] = f_TOF->GetParameter(1) - detectors[det].distance/LIGHT_C;
        //time resolution
        detectors[det].timeResolution[trig] = f_TOF->GetParameter(2);

    }

    if(detectors[det].type == INORGANIC) continue;
    // discrmPSD
    // create a canvas to store the data being accessed
    TCanvas* c_psdErg;
    TF1* f_psdDisc;
    // sets the file name to the base name and the detector
    fileNamePsd =  fileNameBasePsd + (TString)to_string(det);

    // cout << detFile->GetName() << endl;

    //sets the canvas equal to the psdErg one for that detector
    c_psdErg = (TCanvas*)detFile->Get(fileNamePsd);
    f_psdDisc = (TF1*)c_psdErg->GetPrimitive("expLinPsd");

    detectors[det].discPSD = (TF1*)f_psdDisc->Clone();
    // DOUBLE DISCRIM
    if(DOUBLE_DISC == 1)
    {
        TGraph* f_psdDiscNeut;
        TGraph* f_psdDiscPhot;
        //neutron line
        f_psdDiscNeut = (TGraph*)c_psdErg->GetPrimitive("psdNeut");
        detectors[det].discPSDNeut = (TGraph*)f_psdDiscNeut->Clone();
        //photon line
        f_psdDiscPhot = (TGraph*)c_psdErg->GetPrimitive("psdPhot");
        detectors[det].discPSDPhot = (TGraph*)f_psdDiscPhot->Clone();
    }
    else if(DOUBLE_DISC != 1)
    {
        TGraph* f_psdDiscNeut;
        TGraph* f_psdDiscPhot;
        //neutron line
        f_psdDiscNeut = (TGraph*)c_psdErg->GetPrimitive("discLines");
        detectors[det].discPSDNeut = (TGraph*)f_psdDiscNeut->Clone();
        //photon line
        f_psdDiscPhot = (TGraph*)c_psdErg->GetPrimitive("discLines");
        detectors[det].discPSDPhot = (TGraph*)f_psdDiscPhot->Clone();

        cout << detectors[det].discPSDNeut->Eval(0.8) << endl;
    }

    // Psd parameters
    // create a canvas to store the data being accessed
    TCanvas* c_psdParam;
    TF1* f_meanNeut;
    TF1* f_meanPhot;
    TF1* f_sigNeut;
    TF1* f_sigPhot;
    // sets the file name to the base name and the detector
    fileNamePsdParam =  fileNameBasePsdParam + (TString)to_string(det);
    //sets the canvas equal to the psdErg one for that detector
    c_psdParam = (TCanvas*)detFile->Get(fileNamePsdParam);

    // neutron
    // mean
    f_meanNeut = (TF1*)c_psdParam->GetPrimitive("expLinMN");
    detectors[det].meanNeut = (TF1*)f_meanNeut->Clone();
    //sigma
    f_sigNeut = (TF1*)c_psdParam->GetPrimitive("expLinSN");
    detectors[det].sigNeut = (TF1*)f_sigNeut->Clone();

    // photon
    // mean
    f_meanPhot = (TF1*)c_psdParam->GetPrimitive("expLinMP");
    detectors[det].meanPhot = (TF1*)f_meanNeut->Clone();
    //sigma
    f_sigPhot = (TF1*)c_psdParam->GetPrimitive("expLinSP");
    detectors[det].sigPhot = (TF1*)f_sigPhot->Clone();

}

return 1;

}
