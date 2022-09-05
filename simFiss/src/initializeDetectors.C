//
//

#include "SFAME.h"
#include "Constants.h"
#include "DetectorClass.h"

void sfame::initializeDetectors()
{
  cout << "Initializing detectors properties. " << endl;
// array of x detectors (40 here)
  detectors = new DetectorClass [NUM_DETECTORS];
  // fill all of the properties
  // ? I think this needed to potentially go somewhere else eventually
  for(int i = 0; i < NUM_DETECTORS; i++)
  {
      // LO_function
      detectors[i].lightOutput = LightOutList[i];
      detectors[i].kinLight = KinLightList[i];
      detectors[i].lightOutput->SetName("LightOut_" + (TString)to_string(i));
      detectors[i].kinLight->SetName("KinematicLight_" + (TString)to_string(i));


      // distance
      detectors[i].x = posXList->Eval(i)*100;
      detectors[i].y = posYList->Eval(i)*100;
      detectors[i].z = posZList->Eval(i)*100;
      detectors[i].dist = sqrt(pow(detectors[i].x - SOURCE_POSITION[0], 2) + pow(detectors[i].y - SOURCE_POSITION[1], 2) + pow(detectors[i].z- SOURCE_POSITION[2], 2));

      // time resolution
      detectors[i].timeRes = timeResList->Eval(i);

      // energy resolution
      detectors[i].energyRes = new TF1("energyRes_" + (TString)to_string(i), "([0]*x + [1]*sqrt(x) + [2])/(2.35*100)", MIN_DEP_ENERGY, MAX_DEP_ENERGY);
      detectors[i].energyRes->SetParameters(RESOLUTION_COEFFICIENTS[0], RESOLUTION_COEFFICIENTS[1], RESOLUTION_COEFFICIENTS[2]);
      // cout << detectors[i].energyRes->Eval(0.662)/0.662*2.35 << endl;

      // cell
      detectors[i].cell = chanToCell(i);

      // channel
      detectors[i].channel = i;

      // psd parameters
      detectors[i].meanNeutPSD = new TF1(expLin);
      detectors[i].meanPhotPSD = new TF1(expLin);
      detectors[i].sigNeutPSD = new TF1(expLin);
      detectors[i].sigPhotPSD = new TF1(expLin);

      for(int j = 0; j < NUM_PSD_PARAMS; j++)
      {
        detectors[i].meanNeutPSD->SetParameter(j, meanNeutPSDArray[i][j]);
        detectors[i].meanPhotPSD->SetParameter(j, meanPhotPSDArray[i][j]);
        detectors[i].sigNeutPSD->SetParameter(j, sigNeutPSDArray[i][j]);
        detectors[i].sigPhotPSD->SetParameter(j, sigPhotPSDArray[i][j]);
      }
      detectors[i].meanNeutPSD->SetName("meanNeutPSD_" + (TString)to_string(i));
      detectors[i].meanPhotPSD->SetName("meanPhotPSD_" + (TString)to_string(i));
      detectors[i].sigNeutPSD->SetName("sigNeutPSD_" + (TString)to_string(i));
      detectors[i].sigPhotPSD->SetName("sigPhotPSD_" + (TString)to_string(i));


      // cout << detectors[i].meanPhotPSD->Eval(0.4) << "+/-" << detectors[i].sigPhotPSD->Eval(0.4) << endl;
      // cout << detectors[i].meanNeutPSD->Eval(0.4) << "+/-" << detectors[i].sigNeutPSD->Eval(0.4) << endl;
      // cout << endl;

   }




}
