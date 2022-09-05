//
//

#include <iostream>
#include <fstream>
#include <sstream>


#include "SFAME.h"
#include "Constants.h"
#include "DetectorClass.h"

using namespace std;

void sfame::initializeInputFiles()
{
    cout << "Initializing input files. " << endl;
    const int MAX_DET_NUMBER = 1000;
    const int ENERGY_AXIS_LENGTH = 10000;

    // read the file and create a TGraph
    ifstream fileCell, filePos, fileLight, fileTimeRes;
    ifstream fileNmeanPSD, filePmeanPSD, fileNsigmaPSD, filePsigmaPSD;

    // detector axis
    double detNumArray[MAX_DET_NUMBER];

    // channel to cell
    double chanArray[MAX_DET_NUMBER];

    // positions
    double xPosArray[MAX_DET_NUMBER];
    double yPosArray[MAX_DET_NUMBER];
    double zPosArray[MAX_DET_NUMBER];

    // light output
    int numEnergyPoints = 0;
    double energyDepArray[ENERGY_AXIS_LENGTH];
    double** lightOutputArray;
    lightOutputArray = new double* [NUM_DETECTORS];
    LightOutList = new TGraph* [NUM_DETECTORS];
    KinLightList = new TGraph* [NUM_DETECTORS];
    for(int Dj = 0; Dj < NUM_DETECTORS; Dj++)
    {
      lightOutputArray[Dj] = new double [ENERGY_AXIS_LENGTH];
    }
    string line;
    double val;

    // time resolution values
    double timeResArray[MAX_DET_NUMBER];

    // psd parameters
    meanNeutPSDArray = new double* [NUM_DETECTORS];
    meanPhotPSDArray = new double* [NUM_DETECTORS];
    sigNeutPSDArray = new double* [NUM_DETECTORS];
    sigPhotPSDArray = new double* [NUM_DETECTORS];

    for(int Dj = 0; Dj < NUM_DETECTORS; Dj++)
    {
      meanNeutPSDArray[Dj] = new double [NUM_PSD_PARAMS];
      meanPhotPSDArray[Dj] = new double [NUM_PSD_PARAMS];
      sigNeutPSDArray[Dj] = new double [NUM_PSD_PARAMS];
      sigPhotPSDArray[Dj] = new double [NUM_PSD_PARAMS];
    }


    // open the files
    fileCell.open(CELL_NUMBERS_FILE);
    filePos.open(DET_DIST_FILE);
    fileLight.open(LIGHTOUTPUT_FILE);
    fileTimeRes.open(TIME_RESOLUTION_FILE);

    fileNmeanPSD.open(MEAN_NEUTRON_PSD_FILE);
    filePmeanPSD.open(MEAN_PHOTON_PSD_FILE);
    fileNsigmaPSD.open(SIGMA_NEUTRON_PSD_FILE);
    filePsigmaPSD.open(SIGMA_PHOTON_PSD_FILE);


    // start by reading the energy axis
    // cout << 0 << endl;
    getline(fileLight, line);
    // cout << line << endl;
    stringstream ss(line);
    bool readLineLight = true;
    int Ej = 0;
    while(readLineLight)
    {
      if(ss.peek() == ',') ss.ignore();
      ss >> val;

      if(val > 6.0)
      {
        readLineLight = false;
        break;
      }

      energyDepArray[Ej] = val;
      // cout << Ej << " " << energyDepArray[Ej] << endl;
      Ej++;
    }
    // cout << 0 << endl;
    // loop through the files
    for(int i = 0; i < NUM_DETECTORS; i++)
    {
        // cout << i << endl;
        detNumArray[i] = i;
        fileCell >> chanArray[i];
        filePos >> xPosArray[i] >> yPosArray[i] >> zPosArray[i];
        fileTimeRes >> timeResArray[i];

        getline(fileLight, line);
        stringstream ss(line);
        for(int jj = 0; jj < Ej; jj++)
        {
          if(ss.peek() == ',') ss.ignore();
          ss >> val;
          lightOutputArray[i][jj] = val;
        }
        LightOutList[i] = new TGraph(Ej, energyDepArray, lightOutputArray[i]);
        KinLightList[i] = new TGraph(Ej, lightOutputArray[i], energyDepArray);

        int det;

        fileNmeanPSD >> det;
        filePmeanPSD >> det;
        fileNsigmaPSD >> det;
        filePsigmaPSD >> det;

        for(int psdJ = 0; psdJ < NUM_PSD_PARAMS; psdJ++)
        {
          fileNmeanPSD >> meanNeutPSDArray[i][psdJ];
          filePmeanPSD >> meanPhotPSDArray[i][psdJ];
          fileNsigmaPSD >> sigNeutPSDArray[i][psdJ];
          filePsigmaPSD >> sigPhotPSDArray[i][psdJ];
        }
    }

    // create the graphs
    chanToCellList = new TGraph(NUM_DETECTORS, detNumArray, chanArray);
    cellToChanList = new TGraph(NUM_DETECTORS, chanArray, detNumArray);
    posXList = new TGraph(NUM_DETECTORS, detNumArray, xPosArray);
    posYList = new TGraph(NUM_DETECTORS, detNumArray, yPosArray);
    posZList = new TGraph(NUM_DETECTORS, detNumArray, zPosArray);
    timeResList = new TGraph(NUM_DETECTORS, detNumArray, timeResArray);



    // close the files
    fileCell.close();
    filePos.close();
    fileLight.close();
    fileTimeRes.close();

    fileNmeanPSD.close();
    filePmeanPSD.close();
    fileNsigmaPSD.close();
    filePsigmaPSD.close();

}
