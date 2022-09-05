//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug  9 17:52:09 2021 by ROOT version 6.22/08
// from TTree CollisionFile/CollisionFile
// found on file: CollisionFile.root
//////////////////////////////////////////////////////////

#ifndef readColl_h
#define readColl_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class readColl {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           History;
   Int_t           NumLines;
   Int_t           Type[15];   //[NumLines]
   Double_t        EnergyDeposited[15];   //[NumLines]
   Double_t        EnergyIncident[15];   //[NumLines]
   Int_t           Interaction[15];   //[NumLines]
   Int_t           Zaid[15];   //[NumLines]
   Int_t           Cell[15];   //[NumLines]
   Double_t        Time[15];   //[NumLines]
   Double_t        Weight[15];   //[NumLines]
   Double_t        PositionX[15];   //[NumLines]
   Double_t        PositionY[15];   //[NumLines]
   Double_t        PositionZ[15];   //[NumLines]
   Int_t           Particle[15];   //[NumLines]
   Int_t           Scatters[15];   //[NumLines]
   Int_t           Code[15];   //[NumLines]
   Int_t           Generation[15];   //[NumLines]

   // List of branches
   TBranch        *b_History;   //!
   TBranch        *b_NumLines;   //!
   TBranch        *b_Type;   //!
   TBranch        *b_EnergyDeposited;   //!
   TBranch        *b_EnergyIncident;   //!
   TBranch        *b_Interaction;   //!
   TBranch        *b_Zaid;   //!
   TBranch        *b_Cell;   //!
   TBranch        *b_Time;   //!
   TBranch        *b_Weight;   //!
   TBranch        *b_PositionX;   //!
   TBranch        *b_PositionY;   //!
   TBranch        *b_PositionZ;   //!
   TBranch        *b_Particle;   //!
   TBranch        *b_Scatters;   //!
   TBranch        *b_Code;   //!
   TBranch        *b_Generation;   //!

   readColl(TCchain *tree=0);
   virtual ~readColl();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef readColl_cxx
// readColl::readColl(TTree *tree) : fChain(0)
// {
// // if parameter tree is not specified (or zero), connect the file
// // used to generate this class and read the Tree.
//    if (tree == 0) {
//       TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("CollisionFile.root");
//       if (!f || !f->IsOpen()) {
//          f = new TFile("CollisionFile.root");
//       }
//       f->GetObject("CollisionFile", tree);
//
//    }
//    Init(tree);
// }
//
// readColl::~readColl()
// {
//    if (!fChain) return;
//    delete fChain->GetCurrentFile();
// }
//


Long64_t readColl::LoadTree(Long64_t entry)
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

void readColl::Init(TChain *tree)
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

   fChain->SetBranchAddress("History", &History, &b_History);
   fChain->SetBranchAddress("NumLines", &NumLines, &b_NumLines);
   fChain->SetBranchAddress("Type", Type, &b_Type);
   fChain->SetBranchAddress("EnergyDeposited", EnergyDeposited, &b_EnergyDeposited);
   fChain->SetBranchAddress("EnergyIncident", EnergyIncident, &b_EnergyIncident);
   fChain->SetBranchAddress("Interaction", Interaction, &b_Interaction);
   fChain->SetBranchAddress("Zaid", Zaid, &b_Zaid);
   fChain->SetBranchAddress("Cell", Cell, &b_Cell);
   fChain->SetBranchAddress("Time", Time, &b_Time);
   fChain->SetBranchAddress("Weight", Weight, &b_Weight);
   fChain->SetBranchAddress("PositionX", PositionX, &b_PositionX);
   fChain->SetBranchAddress("PositionY", PositionY, &b_PositionY);
   fChain->SetBranchAddress("PositionZ", PositionZ, &b_PositionZ);
   fChain->SetBranchAddress("Particle", Particle, &b_Particle);
   fChain->SetBranchAddress("Scatters", Scatters, &b_Scatters);
   fChain->SetBranchAddress("Code", Code, &b_Code);
   fChain->SetBranchAddress("Generation", Generation, &b_Generation);
   Notify();
}

Bool_t readColl::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void readColl::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t readColl::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef readColl_cxx
