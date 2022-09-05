////////////////////////////////////////////////////////////////////////////////
///  Shea M. Mosby
///  smosby@lanl.gov
///  Devin S. Connolly
///  dconnolly@lanl.gov
///  iterate.cxx
////////////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <stdio.h>
#include <string.h>
#include <fstream>

// ROOT includes
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"

// File includes
#include "WaveformProcessing.h"

Double_t phd_lookup(Float_t mpost,TGraph *hist)
{
  return hist->Eval(mpost)/0.188;
}

Double_t sawtooth_lookup(Float_t mpost,TGraph *hist)
{
  return hist->Eval(mpost);
}

int main(int argc,char *argv[])
{
  // Creation of variables section
  char ipath[512];
  char opath[512];
  char buf[512];

  printf("\n");
  printf(" _______________________________________________________\n");
  printf("|                                                       |\n");
  printf("|		                CAEN Eloss Corr                   |\n");
  printf("|-------------------------------------------------------|\n");
  printf("|        Analyze DPP-enabled CAEN digital ouput		  |\n");
  printf("|                                                       |\n");
  printf("|          (send bugs to smosby@lanl.gov)	              |\n");
  printf("|_______________________________________________________|\n");
  printf("\n");
  // Parsing of input parameters
  if(argc == 3){
    sprintf(ipath,"%s",argv[1]);
    sprintf(opath,"%s",argv[2]);
  }
  else {
    printf("--> EXIT:\nPlease, start singles processing as follows: \n");
    printf("--> EXIT: iterate string=PATH_TO_FILE string=PATH_TO_OUTPUT\n");
    return -1;
  }

  // Global Parameters
  Double_t qt = 0.002;
  Double_t mcn = 236; // compound nucleus mass - here its 236U
  Double_t A0 = 0.188; // absolute energy scale

  // neutron emission lookup table
  //TGraph *neutemission = new TGraph("nu_a_lookup_m.txt"); // messing around to help things look "good"
  TGraph *neutemission = new TGraph("nu_a_lookup.txt"); // messing around to help things look "good"
  TGraph *pulseheight = new TGraph("phd_lookup.txt");

  // Get file / tree paths set
  TFile *ifile = new TFile(ipath);
  TTree *itree = (TTree*)ifile->Get("ipt");
  Float_t iph[2];
  itree->SetBranchAddress("iph[2]",iph);
  TFile *ofile = new TFile(opath,"recreate");
  TTree *otree = new TTree("irt","iterative procedure results tree");
  Float_t mpre[2],mpost[2],epost[2],epre[2],mpreb[2];
  otree->Branch("mpre[2]",mpre);
  otree->Branch("mpost[2]",mpost);
  otree->Branch("epre[2]",epre);
  otree->Branch("epost[2]",epost);

  for (Int_t i = 0;i < itree->GetEntries();++i)
    {
      itree->GetEntry(i);

      // Begin Iterative Procedure: initial conditions
      mpreb[0] = mcn/2.;
      mpreb[1] = mcn/2.;
      mpre[0] = mcn/2.;
      mpre[1] = mcn/2.;
      mpost[0] = mcn/2.;
      mpost[1] = mcn/2.;
      // Execute the iteration
      Bool_t comp0;
      Bool_t comp1;
      Bool_t logic;
      Int_t counter = 0;
      do
        {
          mpreb[0] = mpre[0];
          mpreb[1] = mpre[1];
          //cout << "\n\nMpreb[0] set to " << mpreb[0] << " mpreb[1] " << mpreb[1] << endl;

          // Step 1 in my list
          for (Int_t j=0;j<2;++j)
            {
              mpost[j] = mpre[j] - sawtooth_lookup(mpost[j],neutemission);
            }
          // Step 2 on my list
          for (Int_t j=0;j<2;++j)
            {
              epost[j] = A0*(iph[j] + phd_lookup(mpost[j],pulseheight));
            }
          // Step 3 on my list
          Double_t onePlusBeta = (mpre[1]/mpost[1])*(mpost[0]/mpre[0]);
          // Step 4 on my list
          mpre[0] = mcn*(epost[1])/((epost[0]/onePlusBeta)+epost[1]);
          mpre[1] = mcn*(epost[0])/((epost[1]*onePlusBeta)+epost[0]);
          //cout << "mass " << mpre[0] << " other " << mpre[1] << endl;
          //cout << " compo " << abs(mpre[0]-mpreb[0]) << " comp1 " << abs(mpre[1]-mpreb[1]) << endl;
          Bool_t comp0 = (abs(mpre[0]-mpreb[0])<qt);
          Bool_t comp1 = (abs(mpre[1]-mpreb[1])<qt);
          Bool_t logic = comp0&&comp1;
          //cout << " bool0 " << comp0 << " bool1 " << comp1 << " logic " << logic << endl;
          for (Int_t j = 0; j < 1; ++j)
            {
              epre[j] = mpre[j]*epost[j]/mpost[j];
            }

          counter += 1;
          if (logic) break;
        } while(counter<1000);
      otree->Fill();
      //break;
    }
  otree->Write();
  ofile->Close();

  return 0;
}
