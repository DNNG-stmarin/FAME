//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Apr 27 16:37:26 2020 by ROOT version 6.18/04
// from TTree FissionTree/Fission TREE
// found on file: FissionOutput.root
//////////////////////////////////////////////////////////

#ifndef SystemAnalysis_h
#define SystemAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>

//#include "InfoSystem.h"
#include "InfoSystem.h"

using namespace std;

// Header file for the classes stored in the TTree if any.

class SystemAnalysis {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   //contain DetectorSystemClass

   TFile          *sysFile = 0; // file to store the system information

   // Declaration of leaf types
   int           tMult;
   double        tTime;
   double        tDep;
   double        tPsp;
   double        totToF[MAX_MULTIPLICITY];   //[tMult]
   double        totPSP[MAX_MULTIPLICITY];   //[tMult]
   double        totDep[MAX_MULTIPLICITY];   //[tMult]
   double        totTail[MAX_MULTIPLICITY];   //[tMult]
   int           totChan[MAX_MULTIPLICITY];   //[tMult]
   int           totFlag[MAX_MULTIPLICITY];   //[tMult]

   // List of branches
   TBranch        *b_tMult;   //!
   TBranch        *b_tPsp;   //!
   TBranch        *b_fissionTime;   //!
   TBranch        *b_fissionErg;   //!
   TBranch        *b_totToF;   //!
   TBranch        *b_totPSP;   //!
   TBranch        *b_totDep;   //!
   TBranch        *b_totTail;   //!
   TBranch        *b_totChan;   //!
   TBranch        *b_totFlag;   //!

   SystemAnalysis(TChain *tree, TFile* sysFileWrite);
   virtual ~SystemAnalysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SystemAnalysis_cxx
SystemAnalysis::SystemAnalysis(TChain *tree, TFile* sysFileWrite)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.

   sysFile = sysFileWrite;

   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("FissionOutput.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("FissionOutput.root");
      }
      f->GetObject("FissionTree",tree);
   }
   Init(tree);
}

SystemAnalysis::~SystemAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SystemAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SystemAnalysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void SystemAnalysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("tMult", &tMult, &b_tMult);
   fChain->SetBranchAddress("tTime", &tTime, &b_fissionTime);
   fChain->SetBranchAddress("tDep", &tDep, &b_fissionErg);
   fChain->SetBranchAddress("totToF", totToF, &b_totToF);
   fChain->SetBranchAddress("totPSP", totPSP, &b_totPSP);
   fChain->SetBranchAddress("totDep", totDep, &b_totDep);
   fChain->SetBranchAddress("totTail", totTail, &b_totTail);
   fChain->SetBranchAddress("totChan", totChan, &b_totChan);
   if(SIM_FILE) fChain->SetBranchAddress("totFlag", totFlag, &b_totFlag);
   Notify();

   cout << "Tree being read in correctly." << endl;
}

Bool_t SystemAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SystemAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SystemAnalysis::Cut(Long64_t entry)
{
   return 1;
}
#endif // #ifdef SystemAnalysis_cxx
