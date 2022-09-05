// Programmers: Nathan Giha (giha@umich.edu)
// Purpose: Convert CoMPASS output to a format compatible with Dana's scripts
// Date: March 2022

#include "InfoSystem.h"
#include "ConvertCompass.h"

#include <stdlib.h>
#include <stdio.h>
#include <TString.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{

  cout << "\nWelcome to CONVERT_COMPASS" << endl;

  if(argc == 1) {
    cout << "ERROR: input file not given\n";
    return 0;
  }

  TString inputFileName;
  TString fileName;

  inputFileName = TString(argv[1]);

  if(argc >= 3) {
    fileName = TString(argv[2]);
  }
  else
  {
    fileName = "compassCf_";
  }

  InfoSystem* info = new InfoSystem();
  info->ReadInput(inputFileName);

  ConvertCompass* convert = new ConvertCompass(info, fileName);
  convert->Loop();

  return 0;
}
