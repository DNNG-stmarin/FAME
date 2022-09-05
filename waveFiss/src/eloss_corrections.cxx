////////////////////////////////////////////////////////////////////////////////
///  Shea M. Mosby
///  smosby@lanl.gov
///  Devin S. Connolly
///  dconnolly@lanl.gov
///  eloss_corrections.cxx
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
  char ipath0[512];
  char ipath1[512];
  char opath[512];
  char buf[512];

  printf("\n");
  printf(" _______________________________________________________\n");
  printf("|                                                       |\n");
  printf("|                  CAEN Eloss Corr                      |\n");
  printf("|-------------------------------------------------------|\n");
  printf("|        Analyze DPP-enabled CAEN digital ouput         |\n");
  printf("|                                                       |\n");
  printf("|          (send bugs to smosby@lanl.gov)               |\n");
  printf("|_______________________________________________________|\n");
  printf("\n");
  // Parsing of input parameters
  if(argc == 4){
    sprintf(ipath0,"%s",argv[1]);
    sprintf(ipath1,"%s",argv[2]);
    sprintf(opath,"%s",argv[3]);
  }
  else {
    printf("--> EXIT:\nPlease, start singles processing as follows: \n");
    printf("--> EXIT: elosscorrection string=PATH_TO_FILE string=PATH_TO_FILE string=PATH_TO_OUTPUT\n");
    return -1;
  }
  // Get file / tree paths set
  TFile *ifile0 = new TFile(ipath0);
  TFile *ifile1 = new TFile(ipath1);
  TFile *ofile = new TFile(opath,"recreate");
  TTree *itree0 = (TTree*)ifile0->Get("et");
  EventInfo coinc_evt;
  itree0->SetBranchAddress("aph[2]",coinc_evt.aph);
  TTree *itree1 = (TTree*)ifile1->Get("at");
  Float_t cosTheta[2];
  itree1->SetBranchAddress("costheta[2]",cosTheta);
  TTree *otree = new TTree("ipt","ideal pulse height tree");
  Float_t iph[2];
  otree->Branch("iph[2]",iph);

  // calibration functions
  TF1 *eloss[2];
  eloss[0] = new TF1("eloss0","[0]+[1]*x",-10,10);
  eloss[1] = new TF1("eloss1","[0]+[1]*x",-10,10);
  // and the parameters
  eloss[0]->SetParameter(0,437.863);
  eloss[0]->SetParameter(1,-16.3465);
  eloss[1]->SetParameter(0,441.07);
  eloss[1]->SetParameter(1,-57.6876);

  Double_t xeval = -(eloss[1]->GetParameter(0)-eloss[0]->GetParameter(0))/(eloss[1]->GetParameter(1)-eloss[0]->GetParameter(1));
  Double_t y0 = eloss[0]->Eval(xeval);
  cout << "The intersect point is " << xeval << " and max peak " << y0 << endl;

  for (Int_t i = 0;i < itree0->GetEntries();++i)
    {
      itree0->GetEntry(i);
      itree1->GetEntry(i);

      for (Int_t j=0; j<2; ++j)
        {
          Double_t deltap = y0-eloss[j]->Eval(1./cosTheta[j]);
          iph[j] = (-1.0*coinc_evt.aph[j]) + deltap;
        }

      otree->Fill();
    }
  ofile->Write();

  return 0;
}
