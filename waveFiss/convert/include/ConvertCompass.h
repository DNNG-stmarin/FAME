//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Feb 21 18:02:39 2022 by ROOT version 6.22/08
// from TTree Data_F/CoMPASS filtered events TTree
// found on file: compassF_CfOvernight_1_0.root
//////////////////////////////////////////////////////////

#ifndef ConvertCompass_h
#define ConvertCompass_h

#include "InfoSystem.h"

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "TArrayS.h"


class ConvertCompass {
public :
   InfoSystem*     infoSystem;

   TChain*         runChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   TFile*          runFile;
   Long64_t        nentries;

   TTree*          danaTree;
   TFile*          danaFile;

   TString         inputTreeName = "Data_F";
   TString         runFilename = "default";
   TString         extRunFile = ".root";

   int             boardIn;
   int             boardDefault;

// Fixed size dimensions of array or collections stored in the TTree if any.

// Declaration of leaf types
   UShort_t        Channel;
   ULong64_t       Timestamp;
   UShort_t        Board;
   UShort_t        Energy;
   UInt_t          Flags;
   Int_t           Probe;
   TArrayS*        Samples;

    // List of branches
   TBranch*        b_Channel;   //!
   TBranch*        b_Timestamp;   //!
   TBranch*        b_Board;   //!
   TBranch*        b_Energy;   //!
   TBranch*        b_Flags;   //!
   TBranch*        b_Probe;   //!
   TBranch*        b_Samples;   //!

   ConvertCompass(InfoSystem* infoIn, TString fileName);
   virtual ~ConvertCompass();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init();

   virtual void     Loop();

   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif
