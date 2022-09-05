// Programmer: Stefano Marin
// Purpose: Reading the fission events generated frmo ROOT_FAME
// Date: April 2021

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TBrowser.h>

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "SFAME.h"
#include "infoSystem.h"

using namespace std;

int main(int argc, char** argv)
{

  cout << "\n Welcome to SIM_FAME \n" << endl;

  TString simFile;
  TString dataFileName;
  TString inputFileName;
  int firstFile;
  int numFiles;
  InfoSystem* infoSys;

  inputFileName = TString(argv[1]);

  dataFileName = TString(argv[2]);

  cout << "reading input from: " << inputFileName << endl;
  cout << "reading data from: " << dataFileName << endl;

  if(argc == 1)
  {
    cout << "USAGE STATEMENT:" << endl;
    cout << "Provide name of input file and name of .d file" << endl;
    return 1;
  }

  cout << "creating info " << endl;
  InfoSystem* info = new InfoSystem();
  info->ReadInput(inputFileName);

  cout << info << endl;

  cout << "creating instance of simFame: " << endl;
  
  sfame* simFame = new sfame(info);
  simFame->nameColFile = dataFileName;

  cout << "finished constructing" << endl;

  simFame->readFromCol();
  simFame->LoopColl();


  return 0;
}
