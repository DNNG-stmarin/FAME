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
#include <TGraph2D.h>

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
  int NUM_FILES;
  int* FILE_LIST;
  double KINETIC_PEAKS[2] = {0,0};
  TString SAWTOOTH_FILENAME;
  TString BIND_ERG_FILENAME;
  TString PHD_FILENAME;
  TString FPY_FILENAME;
  TString TKE_FILENAME;
  TString SIG_TKE_FILENAME;
  TString NU_A_TKE_FILENAME;
  TString DELT_A_TKE_FILENAME;

  TString SLOPE_NU_FILENAME;
  TString INTERCEPT_NU_FILENAME;



  int NUM_RECURSIONS;

  InfoSystem()
  {
    NUM_FILES = 1;
    FILE_LIST = NULL;
    SAWTOOTH_FILENAME = "cf252sawtooth.csv";
    BIND_ERG_FILENAME = "L";
    PHD_FILENAME = "phd.txt";
    FPY_FILENAME = "cf252fpy.txt";
    TKE_FILENAME = "cf252tke.txt";
    TKE_FILENAME = "sigTKE.txt";
    NU_A_TKE_FILENAME = "nuTKE.txt";
    DELT_A_TKE_FILENAME = "delTKE.txt";
    NUM_RECURSIONS = 0;

    SLOPE_NU_FILENAME = "slopetNu.txt";
    INTERCEPT_NU_FILENAME = "interceptNu.txt";

  }

  void ReadInput(TString inputFile)
  {
    string inputFileString = string(inputFile);
    ifstream file (inputFileString);

    if(!file.is_open())
    {
      cout << "Failed to open input file\n";
      exit(0);
    }

    string line;
    string tag, value;
    while(file >> tag)
    {
      if(tag == "<NUM_FILES>:")
      {
        file >> value;
        NUM_FILES = stoi(value);
        FILE_LIST = new int[NUM_FILES];
        for(int i=0; i<NUM_FILES; i++)
        {
          FILE_LIST[i] = i;
        }
      }
      else if(tag == "<FILE_LIST>:")
      {
        for(int i=0; i<NUM_FILES; i++)
        {
          file >> value;
          FILE_LIST[i] = stoi(value);
        }
      }
      else if(tag == "<KINETIC_PEAKS>:")
      {
        file >> value;
        KINETIC_PEAKS[0] = stod(value);
        file >> value;
        KINETIC_PEAKS[1] = stod(value);
      }
      else if(tag == "<SAWTOOTH_FILENAME>:")
      {
        file >> value;
        SAWTOOTH_FILENAME = value;
      }
      else if(tag == "<BIND_ERG_FILENAME>:")
      {
        file >> value;
        BIND_ERG_FILENAME = value;
      }
      else if(tag == "<PHD_FILENAME>:")
      {
        file >> value;
        PHD_FILENAME = value;
      }
      else if(tag == "<FPY_FILENAME>:")
      {
        file >> value;
        FPY_FILENAME = value;
      }
      else if(tag == "<TKE_FILENAME>:")
      {
        file >> value;
        TKE_FILENAME = value;
      }
      else if(tag == "<SIG_TKE_FILENAME>:")
      {
        file >> value;
        SIG_TKE_FILENAME = value;
      }

      else if(tag == "<NU_A_TKE_FILENAME>:")
      {
        file >> value;
        NU_A_TKE_FILENAME = value;
      }

      else if(tag == "<DELT_A_TKE_FILENAME>:")
      {
        file >> value;
        DELT_A_TKE_FILENAME = value;
      }

      else if(tag == "<SLOPE_NU_FILENAME>:")
      {
        file >> value;
        SLOPE_NU_FILENAME = value;
      }

      else if(tag == "<INTERCEPT_NU_FILENAME>:")
      {
        file >> value;
        INTERCEPT_NU_FILENAME = value;
      }

      else if(tag == "<NUM_RECURSIONS>:")
      {
        file >> value;
        NUM_RECURSIONS = stoi(value);
      }

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
    delete FILE_LIST;
  }
};

#endif
