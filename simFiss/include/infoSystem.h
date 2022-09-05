// Programmer: Stefano Marin, Isabel Hernandez
// Purpose: header file containting useful constants
// Date: 03.20.2020

#ifndef InfoSystem_h
#define InfoSystem_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TGraph.h>

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>



using namespace std;

class InfoSystem
{
private:

public:
  int FIRST_FILE;
  int NUM_FILES;

  TString OUTPUT_NAME;

  // DETECTORS
  int NUM_DETECTORS;
  int TRIGGER_CHANNEL;

  // specs
  double* SOURCE_POSITION;
  double THRESHOLD;
  double COINCIDENCE_WINDOW;
  double PULSE_GENERATION_WINDOW;

  // parameters to accept the trigger in every case
  double ACCEPT_PSP;
  double ACCEPT_TRIG;
  int ACCEPT_TYPE;
    
  double TIME_RES_TRIGGER;

  // external files
  TString DET_DIST_FILE; // detector distances
  TString TIME_RESOLUTION_FILE; // time resolution
  TString CELL_NUMBERS_FILE; // cell numbers
  TString LIGHTOUTPUT_FILE; // conversion between energyDep and lightoutput
  double* RESOLUTION_COEFFICIENTS;

  // energy file
  TString MEAN_NEUTRON_PSD_FILE;
  TString MEAN_PHOTON_PSD_FILE;
  TString SIGMA_NEUTRON_PSD_FILE;
  TString SIGMA_PHOTON_PSD_FILE;


  InfoSystem()
  {

    cout << "generating deafult info" << endl;

    FIRST_FILE = 0;
    NUM_FILES = 1;

    OUTPUT_NAME = "CoincidenceTrees.root"; // leave it as default to avoid complications

    NUM_DETECTORS = 100;
    TRIGGER_CHANNEL = 0;

    SOURCE_POSITION  = new double[3];

    ACCEPT_PSP = 0.1;
    ACCEPT_TRIG = 0.01; // V us
    ACCEPT_TYPE = 0; // Spontaneous fission (?)

    THRESHOLD = 0.01; // MeVee
    COINCIDENCE_WINDOW = 200; // ns
    PULSE_GENERATION_WINDOW = 10; // ns
    TIME_RES_TRIGGER = 0.01; // ns

    RESOLUTION_COEFFICIENTS = new double[3];

    // external lightoutput files
    DET_DIST_FILE = "";
    TIME_RESOLUTION_FILE = "";
    CELL_NUMBERS_FILE = "";
    LIGHTOUTPUT_FILE = "";

    MEAN_NEUTRON_PSD_FILE = "";
    SIGMA_NEUTRON_PSD_FILE = "";
    MEAN_PHOTON_PSD_FILE = "";
    SIGMA_PHOTON_PSD_FILE = "";
  }

  void ReadInput(TString inputFile)
  {
    cout << "reading user input" << endl;
    string inputFileString = string(inputFile);
    ifstream file (inputFileString);

    if(!file.is_open())
    {
      cout << "Failed to open input file\n";
      exit(0);
    }
    else
    {
        cout << "file opened correctly" << endl;
    }

    string line;
    string tag, value;
    while(file >> tag)
    {
      // cout << "reading " << tag << endl;

      // main input
      if(tag == "<FIRST_FILE>:")
      {
        file >> value;
        FIRST_FILE = stoi(value);
      }
      else if(tag == "<NUM_FILES>:")
      {
        file >> value;
        NUM_FILES = stoi(value);
      }
      else if(tag == "<OUTPUT_NAME>:")
      {
        file >> value;
        OUTPUT_NAME = (TString)value;
      }
      else if(tag == "<TIME_RES_TRIGGER>:")
      {
          file >> value;
          TIME_RES_TRIGGER = stod(value);
          cout << "broadening trigger by " << TIME_RES_TRIGGER << " ns" << endl;
      }

      // geometry
      else if(tag == "<NUM_DETECTORS>:")
      {
        file >> value;
        NUM_DETECTORS = stoi(value);
      }
      else if(tag == "<TRIGGER_CHANNEL>:")
      {
        file >> value;
        TRIGGER_CHANNEL = stoi(value);
      }
      else if(tag == "<SOURCE_POSITION>:")
      {
        file >> value;
        SOURCE_POSITION[0] = stod(value);
        file >> value;
        SOURCE_POSITION[1] = stod(value);
        file >> value;
        SOURCE_POSITION[2] = stod(value);
      }

      // test passing values
      else if(tag == "<ACCEPT_PSP>:")
      {
        file >> value;
        ACCEPT_PSP = stod(value);
      }
      else if(tag == "<ACCEPT_TRIG>:")
      {
        file >> value;
        ACCEPT_TRIG = stod(value);
      }
      else if(tag == "<ACCEPT_TYPE>:")
      {
        file >> value;
        ACCEPT_TYPE = stoi(value);
      }

      // processing parameters
      else if(tag == "<THRESHOLD>:")
      {
        file >> value;
        THRESHOLD = stod(value);
      }
      else if(tag == "<COINCIDENCE_WINDOW>:")
      {
        file >> value;
        COINCIDENCE_WINDOW = stod(value);
      }
      else if(tag == "<PULSE_GENERATION_WINDOW>:")
      {
        file >> value;
        PULSE_GENERATION_WINDOW = stod(value);
      }

      // simulation files
      else if(tag == "<RESOLUTION_COEFFICIENTS>:")
      {
        file >> value;
        RESOLUTION_COEFFICIENTS[0] = stod(value);
        file >> value;
        RESOLUTION_COEFFICIENTS[1] = stod(value);
        file >> value;
        RESOLUTION_COEFFICIENTS[2] = stod(value);
      }
      else if(tag == "<DET_DIST_FILE>:")
      {
        file >> value;
        DET_DIST_FILE = (TString)value;
      }
      else if(tag == "<TIME_RESOLUTION_FILE>:")
      {
        file >> value;
        TIME_RESOLUTION_FILE = (TString)value;
      }
      else if(tag == "<CELL_NUMBERS_FILE>:")
      {
        file >> value;
        CELL_NUMBERS_FILE = (TString)value;
      }
      else if(tag == "<LIGHTOUTPUT_FILE>:")
      {
        file >> value;
        LIGHTOUTPUT_FILE = (TString)value;
      }

      // psd parameter
      else if(tag == "<MEAN_NEUTRON_PSD_FILE>:")
      {
        file >> value;
        MEAN_NEUTRON_PSD_FILE = (TString)value;
      }
      else if(tag == "<MEAN_PHOTON_PSD_FILE>:")
      {
        file >> value;
        MEAN_PHOTON_PSD_FILE = (TString)value;
      }
      else if(tag == "<SIGMA_NEUTRON_PSD_FILE>:")
      {
        file >> value;
        SIGMA_NEUTRON_PSD_FILE = (TString)value;
      }
      else if(tag == "<SIGMA_PHOTON_PSD_FILE>:")
      {
        file >> value;
        SIGMA_PHOTON_PSD_FILE = (TString)value;
      }
      // else
      // {
      //   cout << tag << " input could not be understood." << endl;
      // }

    }

    // TString pathT = (TString)calibrationPath;
    // ifstream fin(calibrationPath);
    // if(!fin.is_open()){
    //   cout << "Failed to open calibration file\n";
    // }
    // calibrationDet = new TGraph(pathT);
  }

  ~InfoSystem()
  {
    // delete FILE_LIST;
  }
};

#endif
