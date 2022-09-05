#include "ConvertCompass.h"

#include <iostream>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

using namespace std;

ConvertCompass::ConvertCompass(InfoSystem* infoIn, TString fileName) : runChain(0)
{
  // Create output file
  TString outfileName = "danaFile.root";
  cout << "Creating output file:" << outfileName << endl;
  danaFile = new TFile(outfileName, "RECREATE");

  // Get runChain
  runChain = new TChain(inputTreeName, inputTreeName);

  // Load info
  infoSystem = infoIn;

   for(int i = 0; i < infoSystem->NUM_FILES; i++)
   {
      int fileNum = (infoSystem->FILE_LIST)[i];

      TString s_runFile;

      s_runFile = infoSystem->FILE_NAME + "_" + TString(to_string(fileNum)) + extRunFile;

      cout << "Reading file number " << fileNum << ": " << s_runFile << endl;

      runFile = (TFile*)gROOT->GetListOfFiles()->FindObject(s_runFile);

      if (!runFile || !runFile->IsOpen())
      {
        runFile = new TFile(s_runFile);
      }
      else
      {
        cout << "ERROR: Failed to open run file" << endl;
      }

      TTree* runTree;
      runFile->GetObject(inputTreeName, runTree);

      if(runTree != 0)
      {
        runChain->Add(s_runFile + "/" + inputTreeName);
      }
      runFile->Close();
      // delete runFile;
   }

   Init();
   nentries = runChain->GetEntries();

   // nentries = 100;
   cout << "Analyzing " << nentries << " entries" << endl;

   // initializing board

   boardIn = infoSystem->BOARD_NUM;
   boardDefault = 0;
   cout << "Reading fragments from board " <<  boardIn << " and writing to " << boardDefault << endl;
}

void ConvertCompass::Loop()
{
  danaFile->cd();
  cout << "Creating Dana tree" << endl;
  danaTree = new TTree("t", "DanaCompatible TTree");
  danaTree->SetFileNumber(0);
  danaTree->SetMaxTreeSize(500000000LL);


   // setup info
   int recordLength = 1280;
   // const int NUM_ELEC = 7;
   // const int channelChamber[NUM_ELEC] = {1,2,3,4,5,6,21};

   UChar_t tBoard;
   UChar_t tChan;
   ULong64_t tTime;
   Short_t* tWave;

   int iEnt = 0;
   do
   {
     runChain->GetEntry(iEnt);
   }
   while(Board != boardIn);

   recordLength = Samples->GetSize();
   tWave = new Short_t [recordLength];

   cout << "___________" << endl;
   cout << "RECORD LENGTH: " << recordLength << endl;
   cout << "___________" << endl;

   danaTree->Branch("bnum", &tBoard, "bnum/b");
   danaTree->Branch("chnum", &tChan, "chnum/b");
   danaTree->Branch("ts", &tTime, "ts/l");

   char buf[100];
   sprintf(buf, "wf[%i]/S", recordLength);

   danaTree->Branch("wf", tWave, buf);

   // danaTree->SetMaxTreeSize(1000000000LL);
   // danaTree->SetFileNumber(0);


   // random filling options
   // TF1* electrodeFall = new TF1("electrode", "1/(1 + Exp[-x])", )
   // TRandom3* randGen = new TRandom3();


   if (runChain == 0) return;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);

      if (ientry < 0) break;
      nb = runChain->GetEntry(jentry);   nbytes += nb;

      if(Board != boardIn) continue;

      // tBoard = (UChar_t)Board;
      tBoard = (UChar_t)boardDefault; // convert back to 0, for ease of use of next codes
      tChan = (UChar_t)Channel;
      if (tChan == 8) // cathode channel
      {
        tChan = 0;
      }

      tTime = (ULong64_t)Timestamp / 1000.; // Convert from ps to ns

      for (int i = 0; i < recordLength; i++)
      {
        tWave[i] = (Short_t)Samples->GetAt(i);
      }
      danaTree->Fill();

      if(jentry%10000000 == 0)
      {
        cout << jentry << " entries done" << endl;;
      }

   }



   danaFile = danaTree->GetCurrentFile();
 	 danaFile->Write();


   // danaTree->Write();
   // danaFile->Write();
   danaFile->Close();
}


ConvertCompass::~ConvertCompass()
{
   if (!runChain) return;
   delete runChain->GetCurrentFile();
}

Int_t ConvertCompass::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!runChain) return 0;
   return runChain->GetEntry(entry);
}
Long64_t ConvertCompass::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!runChain) return -5;
   Long64_t centry = runChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (runChain->GetTreeNumber() != fCurrent) {
      fCurrent = runChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ConvertCompass::Init()
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   Samples = 0;
   fCurrent = -1;
   runChain->SetMakeClass(1);

   runChain->SetBranchAddress("Channel", &Channel, &b_Channel);
   runChain->SetBranchAddress("Timestamp", &Timestamp, &b_Timestamp);
   runChain->SetBranchAddress("Board", &Board, &b_Board);
   runChain->SetBranchAddress("Energy", &Energy, &b_Energy);
   runChain->SetBranchAddress("Flags", &Flags, &b_Flags);
   runChain->SetBranchAddress("Probe", &Probe, &b_Probe);
   runChain->SetBranchAddress("Samples", &Samples, &b_Samples);

   Notify();
}

Bool_t ConvertCompass::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.
   return kTRUE;
}

void ConvertCompass::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!runChain) return;
   runChain->Show(entry);
}
Int_t ConvertCompass::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
