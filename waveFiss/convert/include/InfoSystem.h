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
  int NUM_FILES;
  int* FILE_LIST;
  TString FILE_NAME;
  int BOARD_NUM;

  InfoSystem()
  {
    NUM_FILES = 1;
    FILE_LIST = NULL;
    FILE_NAME = "DataF_runW";
    BOARD_NUM = 3;
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
      if(tag == "<FILE_NAME>:")
      {
        file >> value;
        FILE_NAME = value;
      }
      else if(tag == "<NUM_FILES>:")
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
      else if(tag == "<BOARD_NUM>:")
      {
        file >> value;
        BOARD_NUM = stoi(value);
      }
    }
    file.close();
  }

  ~InfoSystem()
  {
    delete FILE_LIST;
  }
};

#endif
