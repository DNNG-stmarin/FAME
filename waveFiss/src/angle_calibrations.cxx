////////////////////////////////////////////////////////////////////////////////
///  Shea M. Mosby
///  smosby@lanl.gov
///  Devin S. Connolly
///  dconnolly@lanl.gov
///  angle_calibrations.cxx
////////////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <stdio.h>
#include <string.h>

// ROOT includes
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"

// File includes
#include "WaveformProcessing.h"


int main(int argc,char *argv[])
{
  // Creation of variables section
  char ipath[512];
  char opath[512];
  char buf[512];

  printf("\n");
  printf(" _______________________________________________________\n");
  printf("|                               |\n");
  printf("|		   CAEN Angle Calib         |\n");
  printf("|-------------------------------------------------------|\n");
  printf("|        Analyze DPP-enabled CAEN digital ouput		|\n");
  printf("|                               |\n");
  printf("|          (send bugs to smosby@lanl.gov)       |\n");
  printf("|_______________________________________________________|\n");
  printf("\n");
  // Parsing of input parameters
  if(argc == 3){
    sprintf(ipath,"%s",argv[1]);
    sprintf(opath,"%s",argv[2]);
  }
  else {
    printf("--> EXIT:\nPlease, start singles processing as follows: \n");
    printf("--> EXIT: anglecalibrations string=PATH_TO_FILE string=PATH_TO_OUTPUT\n");
    return -1;
  }
  // Get file / tree paths set
  TFile *ifile = new TFile(ipath);
  TFile *ofile = new TFile(opath,"recreate");
  TTree *itree = (TTree*)ifile->Get("et");
  EventInfo coinc_evt;
  Float_t cosTheta[2];
  itree->SetBranchAddress("aph[2]",coinc_evt.aph);
  itree->SetBranchAddress("gph[2]",coinc_evt.gph);
  TTree *otree = new TTree("at","angle tree");
  otree->Branch("costheta[2]",cosTheta);


  // calibration functions
  TF1 *lightf[2];
  TF1 *heavyf[2];
  lightf[0] = new TF1("lightf0","[0]+[1]*x",0,1000);
  lightf[1] = new TF1("lightf1","[0]+[1]*x",0,1000);
  heavyf[0] = new TF1("heavyf0","[0]+[1]*x",0,1000);
  heavyf[1] = new TF1("heavyf1","[0]+[1]*x+[2]*x*x",0,1000);
  // and their parameters
  lightf[0]->SetParameter(0,83.9106);
  lightf[0]->SetParameter(1,0.10187);
  lightf[1]->SetParameter(0,84.);
  lightf[1]->SetParameter(1,0.13005);
  heavyf[0]->SetParameter(0,104.57);
  heavyf[0]->SetParameter(1,0.125742);
  heavyf[1]->SetParameter(0,-186.545);
  heavyf[1]->SetParameter(1,1.205);
  heavyf[1]->SetParameter(2,-0.00094637);
  // Calibration Points for interpolation
  TGraph *g[2];
  g[0] = new TGraph("./hists/xd0.txt");
  g[1] = new TGraph("./hists/xd1.txt");

  // Break Points
  Double_t lolim[2] = {261.,188.};
  Double_t hilim[2] = {585.,538.};

  for (Int_t i = 0;i < itree->GetEntries();++i)
    {
      itree->GetEntry(i);

      Double_t aph[2];
      Double_t xdcos[2];
      for (Int_t j = 0; j<2;++j)
        {
          aph[j] = -coinc_evt.aph[j];
          xdcos[j] = coinc_evt.gph[j]/aph[j];
          Double_t xd = -1;
          if (aph[j]>hilim[j])
            {
              xd = heavyf[j]->Eval(aph[j]);
            } else if (aph[j]<lolim[j])
            {
              xd = lightf[j]->Eval(aph[j]);
            }
          else
            {
              xd = g[j]->Eval(aph[j]);
            }
          cosTheta[j] = xdcos[j]/xd;
        }
      otree->Fill();
    }
  ofile->Write();


  return 0;
}
