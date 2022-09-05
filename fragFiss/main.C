// Programmer: Nathan and Stefano
// Purpose: Compass post-process and forming fission coincidences in fission
// Date: February 2022

#include "InfoSystem.h"
#include "fragFiss.h"

#include <stdlib.h>
#include <stdio.h>
#include <TString.h>
#include <constants.h>

using namespace std;

int main(int argc, char** argv)
{

  cout << "\nWelcome to FRAG_FAME" << endl;

  TString fileName;
  TString inputFileName;

  if(argc == 1) {
    cout << "ERROR: input file not given\n";
    return 0;
  }

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

  cout << "You have selected to run " << info->NUM_RECURSIONS << " recursions." << endl;

  fragFiss* frag = new fragFiss(info, fileName);
  frag->PlotWaves();
  frag->AngleAnalysis();
  frag->ELossCorrection();
  frag->GainMatching();
  frag->FillFragTree();

  if(info->NUM_RECURSIONS > 0) cout << "Beginning recursive improvements" << endl;

  for(int i = 0; i < info->NUM_RECURSIONS; i++)
  {

    frag->InitPost(i);
    frag->PostAngle(i);
    frag->PostAttenuation(i);
    frag->PostCalib(i);
    frag->PostCalibPHD(i);
    frag->PostFrag(i);
  }






  frag->fragFile->Close();
  frag->fragDiagnostics->Close();

  if(info->NUM_RECURSIONS > 0)
  {
    frag->postFragFile->Close();
  }


  cout << "Thank you for using FRAG_FISS" << endl;
  cout << "Please be thankful to Dana Duke at LANL" << endl;



  return 0;
}
