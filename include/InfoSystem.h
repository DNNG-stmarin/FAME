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

  // store the channel multiplcities
  int NUM_BEAMS;
  int NUM_DETS;
  int NUM_TRIGGERS;

  // store the channels
  int *BEAM;
  int *DETECTORS;
  int *FISSION_TRIGGERS;

  int* DETECTOR_TYPE;

  //(optional) broken detectors
  int *EXCLUDE_DETECTORS;
  int NUM_EXCLUDED;

  // fragment mode and path to fragment
  int FRAGMENT_MODE;
  TString FRAGMENT_PATH;

  //calibration attributes
  TGraph *calibrationDet;
  TString calibrationPath;
  TString detectorPath;
  TString triggerPath;
  TString triggerDirPath;
  TString nameOfExp;

  //input file
  int MIN_FILE;
  int NUM_FILES;

  int FILE_LIST_MODE;
  int* FILE_LIST;

  int DATA_TYPE;
  int REUSE_DATA;
  int REUSE_DETECTOR;
  int FISSION_MODE;
  int DOUBLE_DISC;
  double MISC_MAX;
  int PARAM_FILES;

  int SIM_FILE;
  double REMOVE_FLAG;


  int RANDOM_COINCIDENCE;

  double COINC_WINDOW;
  double BEAM_WINDOW;
  double FRAGMENT_WINDOW;

  double DETECTOR_THRESHOLD;
  double DETECTOR_CLIP;
  double BEAM_DISTANCE;

  double TRIGGER_THRESHOLD;
  double TRIGGER_CLIP;

  double TRIGGER_MIN_PSP;
  double TRIGGER_MAX_PSP;

  bool TRIGGER_SPLIT;

  double MAX_TRIGGER_DRIFT;
  double MIN_TIME_P;
  double MAX_TIME_P;
  double MIN_TIME_N;
  double MAX_TIME_N;
  double MINPSD_FIT;
  double DIVPSD_FIT;
  double MAXPSD_FIT;

  double MINERG_FIT;
  double MAXERG_FIT;

  double DELTA_BACK_SIG;

  int DEBUG;
  int PSD_ERG;
  int TOF_ERG;
  int STEP_SIZE;

  double MICRO_SEP;
  double MACRO_SEP;
  int MICRO_NUM;

  double BEAM_DELAY;

  InfoSystem()
  {
    detectorPath = "";
    triggerPath = "";
    triggerDirPath = "";
    calibrationPath = "";
    nameOfExp = "";

    calibrationDet = NULL;

    FRAGMENT_MODE = 0;
    FRAGMENT_PATH = "";

    NUM_BEAMS = 0;
    NUM_DETS = 0;
    NUM_TRIGGERS = 0;

    BEAM = NULL;
    FISSION_TRIGGERS =  NULL;
    DETECTORS = NULL;
    EXCLUDE_DETECTORS = NULL;

    DETECTOR_TYPE = NULL;

    MIN_FILE = 0;
    NUM_FILES = 0;
    FILE_LIST_MODE = 0;
    FILE_LIST = NULL;

    DATA_TYPE = 0;
    REUSE_DATA = 0;
    REUSE_DETECTOR = 0;
    FISSION_MODE = 1;
    DOUBLE_DISC = 0;
    MISC_MAX = 0.005;
    PARAM_FILES = 0;

    SIM_FILE = 0;
    REMOVE_FLAG = 0.0;

    RANDOM_COINCIDENCE = 0;

    COINC_WINDOW = 200;
    FRAGMENT_WINDOW = 2000;
    BEAM_WINDOW = 200;
    BEAM_DISTANCE = 2150;

    DETECTOR_THRESHOLD = 0.05;
    DETECTOR_CLIP = 10;
    TRIGGER_THRESHOLD = 0.1;
    TRIGGER_CLIP = 100.0;
    TRIGGER_MIN_PSP = 0.0;
    TRIGGER_MAX_PSP = 1.0;

    TRIGGER_SPLIT = 0;
    MAX_TRIGGER_DRIFT = 1.0;

    MIN_TIME_P = 0.0;
    MAX_TIME_P = 100.0;
    MIN_TIME_N = 0.0;
    MAX_TIME_N = 100.0;

    MINPSD_FIT = 0.0;
    DIVPSD_FIT = 0.25;
    MAXPSD_FIT = 1.0;
    MINERG_FIT = 0.0;
    MAXERG_FIT = 10.0;

    DELTA_BACK_SIG = 10.0;

    DEBUG = 0;

    PSD_ERG = 0;
    TOF_ERG = 0;
    STEP_SIZE = 0;

    MICRO_SEP = 2000;
    MACRO_SEP = 2e6;
    MICRO_NUM = 330;

    BEAM_DELAY = 0;
  }

  void ReadInput(TString inputFile) {
    string inputFileString = string(inputFile);
    ifstream file (inputFileString);

    if(!file.is_open()) {
      cout << "Failed to open input file\n";
      exit(0);
    }

    string line;
    string tag, value;
    while(file >> tag) {
      if(tag == "<NAME>:") {
        file >> value;
        nameOfExp = TString(value);
      }
      if(tag == "<MIN_FILE>:") {
        file >> value;
        MIN_FILE = stoi(value);
      }
      else if(tag == "<FILE_LIST_MODE>:") {
        file >> value;
        FILE_LIST_MODE = stoi(value);
      }
      else if(tag == "<NUM_FILES>:") {
        file >> value;
        NUM_FILES = stoi(value);
      }
      else if(tag == "<FILE_LIST>:") {
        FILE_LIST = new int[NUM_FILES];
        for(int i=0; i<NUM_FILES; i++)
        {
          file >> value;
          FILE_LIST[i] = stoi(value);
        }
      }
      else if(tag == "<DATA_TYPE>:") {
        file >> value;
        DATA_TYPE = stoi(value);
      }
      else if(tag == "<REUSE_DATA>:") {
        file >> value;
        REUSE_DATA = stoi(value);
      }
      else if(tag == "<RANDOM_COINCIDENCE>:"){
        file >> value;
        RANDOM_COINCIDENCE = stoi(value);
      }
      else if(tag == "<REUSE_DETECTOR>:") {
        file >> value;
        REUSE_DETECTOR = stoi(value);
      }
      else if(tag == "<FISSION_MODE>:") {
        file >> value;
        FISSION_MODE = stoi(value);
      }
      else if(tag == "<DOUBLE_DISC>:") {
        file >> value;
        DOUBLE_DISC = stoi(value);
      }
      else if(tag == "<MISC_MAX>:") {
        file >> value;
        MISC_MAX = stod(value);
      }
      else if(tag == "<PARAM_FILES>:") {
        file >> value;
        PARAM_FILES = stoi(value);
      }
      else if(tag == "<SIM_FILE>:") {
        file >> value;
        SIM_FILE = stoi(value);
      }
      else if(tag == "<REMOVE_FLAG>:") {
        file >> value;
        REMOVE_FLAG = stod(value);
      }
      else if(tag == "<DETECTOR_PATH>:") {
        file >> value;
        detectorPath = TString(value);
      }
      else if(tag == "<TRIGGER_PATH>:") {
        file >> value;
        triggerPath = TString(value);
      }
      else if(tag == "<TRIGGER_DIR_PATH>:") {
        file >> value;
        triggerDirPath = TString(value);
      }
      else if(tag == "<CALIBRATION_PATH>:") {
        file >> value;
        calibrationPath = TString(value);
      }
      else if(tag == "<NUM_BEAMS>:") {
        file >> value;
        NUM_BEAMS = stoi(value);
      }
      else if(tag == "<BEAMS>:") {
        BEAM = new int[NUM_BEAMS];
        for(int i=0; i<NUM_BEAMS; i++) {
          file >> value;
          BEAM[i] = stoi(value);
        }
      }
      else if(tag == "<NUM_DETS>:") {
        file >> value;
        NUM_DETS = stoi(value);
      }
      else if(tag == "<DETECTORS>:") {
        DETECTORS = new int[NUM_DETS];
        DETECTOR_TYPE = new int[NUM_DETS];
        for(int i=0; i<NUM_DETS; i++) {
          file >> value;
          DETECTORS[i] = stoi(value);
          DETECTOR_TYPE[i] = 0; // assume organic scintillators
        }
      }
      else if(tag == "<DETECTOR_TYPE>:")
      {
        for(int i=0; i<NUM_DETS; i++)
        {
          file >> value;
          DETECTOR_TYPE[i] = stoi(value); // assume organic scintillators
        }
      }
      else if(tag == "<NUM_TRIGGERS>:") {
        file >> value;
        NUM_TRIGGERS = stoi(value);
      }
      else if(tag == "<TRIGGERS>:") {
        FISSION_TRIGGERS = new int[NUM_TRIGGERS];
        for(int i=0; i<NUM_TRIGGERS; i++) {
          file >> value;
          FISSION_TRIGGERS[i] = stoi(value);
        }
      }
      else if(tag == "<NUM_EXCLUDED>:") {
        file >> value;
        NUM_EXCLUDED = stoi(value);
      }
      else if(tag == "<EXCLUDE_DETECTORS>:") {
        EXCLUDE_DETECTORS = new int[NUM_EXCLUDED];
        for(int i=0; i<NUM_EXCLUDED; i++) {
          file >> value;
          EXCLUDE_DETECTORS[i] = stoi(value);
        }
      }
      else if(tag == "<COINC_WINDOW>:") {
        file >> value;
        COINC_WINDOW = stod(value);
      }

      else if(tag == "<BEAM_WINDOW>:") {
        file >> value;
        BEAM_WINDOW = stod(value);
      }

      else if(tag == "<FRAGMENT_WINDOW>:") {
        file >> value;
        FRAGMENT_WINDOW = stod(value);
      }

      else if(tag == "<BEAM_DISTANCE>:") {
        file >> value;
        BEAM_DISTANCE = stod(value);
      }

      else if(tag == "<DETECTOR_THRESHOLD>:") {
        file >> value;
        DETECTOR_THRESHOLD = stod(value);
      }
      else if(tag == "<DETECTOR_CLIP>:") {
        file >> value;
        DETECTOR_CLIP = stod(value);
      }
      else if(tag == "<TRIGGER_THRESHOLD>:") {
        file >> value;
        TRIGGER_THRESHOLD = stod(value);
      }
      else if(tag == "<TRIGGER_CLIP>:") {
        file >> value;
        TRIGGER_CLIP = stod(value);
      }
      else if(tag == "<TRIGGER_MIN_PSP>:")
      {
        file >> value;
        TRIGGER_MIN_PSP = stod(value);
      }
      else if(tag == "<TRIGGER_MAX_PSP>:")
      {
        file >> value;
        TRIGGER_MAX_PSP = stod(value);
      }
      else if(tag == "<TRIGGER_SPLIT>:")
      {
        file >> value;
        if(stoi(value) == 0) {
          TRIGGER_SPLIT = 0;
        }
        if(stoi(value) == 1) {
          TRIGGER_SPLIT = 1;
        }
      }
      else if(tag == "<MAX_TRIGGER_DRIFT>:") {
        file >> value;
        MAX_TRIGGER_DRIFT = stod(value);
      }
      else if(tag == "<MIN_TIME_P>:") {
        file >> value;
        MIN_TIME_P = stod(value);
      }
      else if(tag == "<MAX_TIME_P>:") {
        file >> value;
        MAX_TIME_P = stod(value);
      }
      else if(tag == "<MIN_TIME_N>:") {
        file >> value;
        MIN_TIME_N = stod(value);
      }
      else if(tag == "<MAX_TIME_N>:") {
        file >> value;
        MAX_TIME_N = stod(value);
      }
      else if(tag == "<DELTA_BACK_SIG>:") {
        file >> value;
        DELTA_BACK_SIG = stod(value);
      }
      else if(tag == "<MINPSD_FIT>:") {
        file >> value;
        MINPSD_FIT = stod(value);
      }
      else if(tag == "<DIVPSD_FIT>:") {
        file >> value;
        DIVPSD_FIT = stod(value);
      }
      else if(tag == "<MAXPSD_FIT>:") {
        file >> value;
        MAXPSD_FIT = stod(value);
      }
      else if(tag == "<MINERG_FIT>:") {
        file >> value;
        MINERG_FIT = stod(value);
      }
      else if(tag == "<MAXERG_FIT>:") {
        file >> value;
        MAXERG_FIT = stod(value);
      }
      else if(tag == "<DEBUG>:") {
        file >> value;
        DEBUG = stoi(value);
      }
      else if(tag == "<PSD_ERG>:") {
        file >> value;
        PSD_ERG = stoi(value);
      }

      else if(tag == "<TOF_ERG>:") {
        file >> value;
        TOF_ERG = stoi(value);
      }

      else if(tag == "<STEP_SIZE>:") {
        file >> value;
        STEP_SIZE = stoi(value);
      }

      else if(tag == "<MICRO_SEP>:") {
        file >> value;
        MICRO_SEP = stoi(value);
      }

      else if(tag == "<MACRO_SEP>:") {
        file >> value;
        MACRO_SEP = stoi(value);
      }

      else if(tag == "<MICRO_NUM>:") {
        file >> value;
        MICRO_NUM = stoi(value);
      }

      else if(tag == "<BEAM_DELAY>:") {
        file >> value;
        BEAM_DELAY = stod(value);
      }

      else if(tag == "<FRAGMENT_PATH>:")
      {
        FRAGMENT_MODE = 1;
        file >> value;
        FRAGMENT_PATH = TString(value);
      }
    }

    TString pathT = (TString)calibrationPath;
    ifstream fin(calibrationPath);
    if(!fin.is_open()){
      cout << "Failed to open calibration file\n";
    }
    calibrationDet = new TGraph(pathT);


    fin.close();
    file.close();
  }

  ~InfoSystem()
  {
    delete BEAM;
    delete FISSION_TRIGGERS;
    delete DETECTORS;
    delete EXCLUDE_DETECTORS;
    delete calibrationDet;
  }
};

#endif
