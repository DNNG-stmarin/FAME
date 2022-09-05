#include "fragFiss.h"
#include "constants.h"

fragFiss::fragFiss(InfoSystem* infoIn, TString fileName) : eventChain(0)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  infoSystem = infoIn;
  cout << "Creating output file" << endl;
  fragFile = new TFile("frag.root", "RECREATE");
  postFragFile = new TFile("postFrag.root", "RECREATE");
  fragDiagnostics = new TFile("fragDiagnostics.root", "RECREATE");
  eventChain = new TChain(inputTreeName, inputTreeName);

  cd_diagnostics = fragDiagnostics->mkdir("diagnostics");
  cd_recursion = fragDiagnostics->mkdir("recursion");

   numFiles = 0;
   nentriesTree = new Long64_t [infoSystem->NUM_FILES];

   for(int i = 0; i < infoSystem->NUM_FILES; i++)
   {


      int fileNum = (infoSystem->FILE_LIST)[i];

      cout << "Reading file number " << fileNum << endl;
      // find the file

      TString s_eventFile =  fileName + TString(to_string(fileNum)) + extEventfile;
      eventFile = (TFile*)gROOT->GetListOfFiles()->FindObject(s_eventFile);

      if (!eventFile || !eventFile ->IsOpen())
      {
        eventFile = new TFile(s_eventFile);
      }
      else
      {
        cout << "ERROR: Failed to open event file" << endl;
      }

      TTree* eventTree;
      eventFile->GetObject(inputTreeName, eventTree);

      if(eventTree != 0)
      {
        eventChain->Add(s_eventFile + "/" + inputTreeName);
        nentriesTree[numFiles] = eventTree->GetEntries();
        cout << nentriesTree[numFiles] << " events" << endl;
        numFiles++;

      }
      //eventFile->Close();
      delete eventFile;
   }
   Init();

   nentries = eventChain->GetEntries();
   cout << "Analyzing " << nentries << " events" << endl;

   // Initialize sawtooth pointer
   g_sawtooth = new TGraph(infoIn->SAWTOOTH_FILENAME);
   g_bindErg = new TGraph(infoIn->BIND_ERG_FILENAME);
   g_phd = new TGraph(infoIn->PHD_FILENAME);
   g_fpy = new TGraph(infoIn->FPY_FILENAME);
   g_tke = new TGraph(infoIn->TKE_FILENAME);
   g_sigtke = new TGraph(infoIn->SIG_TKE_FILENAME);
   g_neutATKE = new TGraph2D(infoIn->NU_A_TKE_FILENAME);
   g_neutATKE->SetName("g_neutATKE");
   g_deltATKE = new TGraph2D(infoIn->DELT_A_TKE_FILENAME);
   g_deltATKE->SetName("g_deltATKE");

   g_slopeNu = new TGraph(infoIn->SLOPE_NU_FILENAME);
   g_slopeNu->SetName("g_slopeNu");
   g_interNu = new TGraph(infoIn->INTERCEPT_NU_FILENAME);
   g_interNu->SetName("g_interNu");



   g2_massAttSurf1 = new TGraph2D* [infoIn->NUM_RECURSIONS];
   g2_massAttSurf2 = new TGraph2D* [infoIn->NUM_RECURSIONS];

}

fragFiss::~fragFiss()
{
   if (!eventChain) return;
   delete eventChain->GetCurrentFile();
   // fragFile->Close();
   delete eventChain;
   delete eventFile;

   delete fragTree;
   delete fragFile;
   delete postFragFile;
   delete fragDiagnostics;

   delete infoSystem;

   delete g2_massAttSurf1;
   delete g2_massAttSurf2;

   cout << "Finished deleting" << endl;

}

Int_t fragFiss::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!eventChain) return 0;
   return eventChain->GetEntry(entry);
}
Long64_t fragFiss::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!eventChain) return -5;
   Long64_t centry = eventChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (eventChain->GetTreeNumber() != fCurrent) {
      fCurrent = eventChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void fragFiss::Init()
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   cout << "Init has been called" << endl;
   // if (!tree) return;
   // eventChain = tree;
   fCurrent = -1;
   eventChain->SetMakeClass(1);

   eventChain->SetBranchAddress("t0t", &t0t, &b_t0t);
   eventChain->SetBranchAddress("ct", &ct, &b_ct);
   eventChain->SetBranchAddress("dt", &dt, &b_dt);
   eventChain->SetBranchAddress("cph", &cph, &b_cph);
   eventChain->SetBranchAddress("tTrig[8]", tTrig, &b_tTrig);
   eventChain->SetBranchAddress("aph[2]", aph, &b_aph);
   eventChain->SetBranchAddress("gph[2]", gph, &b_gph);
   eventChain->SetBranchAddress("sumph[2]", sumph, &b_sumph);
   eventChain->SetBranchAddress("sumtTrig[2]", sumtTrig, &b_sumtTrig);
   eventChain->SetBranchAddress("sumtPeak[2]", sumtPeak, &b_sumtPeak);
   eventChain->SetBranchAddress("ccoinc", &ccoinc, &b_ccoinc);

   if(WAVE_MODE)
   {
     eventChain->SetBranchAddress("a0wf[1280]", a0wf, &b_a0wf);
     eventChain->SetBranchAddress("a1wf[1280]", a1wf, &b_a1wf);
     eventChain->SetBranchAddress("g0wf[1280]", g0wf, &b_g0wf);
     eventChain->SetBranchAddress("g1wf[1280]", g1wf, &b_g1wf);
     eventChain->SetBranchAddress("sum0wf[1280]", sum0wf, &b_sum0wf);
     eventChain->SetBranchAddress("sum1wf[1280]", sum1wf, &b_sum1wf);
     eventChain->SetBranchAddress("cwf[1280]", cwf, &b_cwf);
   }

   Notify();
}

Bool_t fragFiss::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void fragFiss::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!eventChain) return;
   eventChain->Show(entry);
}
Int_t fragFiss::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void fragFiss::InitFrag()
{
  cout << "Creating fragment tree " << endl;
  fragFile->cd();
  // initialize fragment tree
  fragTree = new TTree("FragmentTree", "Tree of Fragments");
  fragTree->SetFileNumber(0);
  // fragTree->SetMaxTreeSize(10000000000LL);

  fragTree->Branch("fT", &fT, "fT/D");
  fragTree->Branch("fAL", &fAL, "fAL/D");
  fragTree->Branch("fAH", &fAH, "fAH/D");
  fragTree->Branch("fKEL", &fKEL, "fKEL/D");
  fragTree->Branch("fKEH", &fKEH, "fKEH/D");
  fragTree->Branch("fThetaL", &fThetaL, "fThetaL/D");
  fragTree->Branch("fThetaH", &fThetaH, "fThetaH/D");
  fragTree->Branch("fEX", &fEX, "fEX/D");

  fragTree->Branch("fAn1", &fAn1, "fAn1/D");
  fragTree->Branch("fAn2", &fAn2, "fAn2/D");
  fragTree->Branch("fGr1", &fGr1, "fGr1/D");
  fragTree->Branch("fGr2", &fGr2, "fGr2/D");
  fragTree->Branch("fCat", &fCat, "fCat/D");
  fragTree->Branch("fA1", &fA1, "fA1/D");
  fragTree->Branch("fA2", &fA2, "fA2/D");

  fragTree->Branch("fKE1", &fKE1, "fKE1/D");
  fragTree->Branch("fKE2", &fKE2, "fKE2/D");
  fragTree->Branch("fTheta1", &fTheta1, "fTheta1/D");
  fragTree->Branch("fTheta2", &fTheta2, "fTheta2/D");
}

void fragFiss::InitPost(int iterationPost)
{
  // Set branch addresses and branch pointers
  cout << "InitPost has been called with index " << iterationPost << endl;
  // if (!tree) return;
  // eventChain = tree;

  // RECURSION_FRACTION_1 = ((double)iterationPost+1)/((double)infoSystem->NUM_RECURSIONS);
  // RECURSION_FRACTION_2 = ((double)iterationPost+1)/((double)infoSystem->NUM_RECURSIONS);

  // MIN_MASS_RECURSION = MIN_MASS + RECURSION_FRACTION_1*(MIN_MASS_ANALYSIS - MIN_MASS);
  // MAX_MASS_RECURSION = MAX_MASS + RECURSION_FRACTION_1*(MAX_MASS_ANALYSIS - MAX_MASS);
  // MIN_ANG_RECURSION =  MIN_ANG1  +  RECURSION_FRACTION_1*(MIN_ANG_ANALYSIS - MIN_ANG1);
  // MAX_ANG_RECURSION =  MAX_ANG1  +  RECURSION_FRACTION_1*(MAX_ANG_ANALYSIS - MAX_ANG1);

  MIN_MASS_RECURSION = MIN_MASS_ANALYSIS;
  MAX_MASS_RECURSION = MAX_MASS_ANALYSIS;
  MIN_ANG_RECURSION =  MIN_ANG_ANALYSIS;
  MAX_ANG_RECURSION =  MAX_ANG_ANALYSIS;

  // if(iterationPost > 4)
  // {
  //   MASS_DEP_ATT = true;
  // }


  cout << "recursion mass: " << MIN_MASS_RECURSION << " " << MAX_MASS_RECURSION << endl;
  cout << "recursion angl: " << MIN_ANG_RECURSION << " " << MAX_ANG_RECURSION << endl;


  // cout << fragTree << endl;

  fragTree->SetFileNumber(0);

  fCurrent = -1;
  fragTree->SetMakeClass(1);

  fragTree->SetBranchAddress("fT", &pT, &b_pT);
  fragTree->SetBranchAddress("fAL", &pAL, &b_pAL);
  fragTree->SetBranchAddress("fAH", &pAH, &b_pAH);
  fragTree->SetBranchAddress("fKEL", &pKEL, &b_pKEL);
  fragTree->SetBranchAddress("fKEH", &pKEH, &b_pKEH);
  fragTree->SetBranchAddress("fThetaL", &pThetaL, &b_pThetaL);
  fragTree->SetBranchAddress("fThetaH", &pThetaH, &b_pThetaH);
  fragTree->SetBranchAddress("fEX", &pEX, &b_pEX);
  fragTree->SetBranchAddress("fA1", &pA1, &b_pA1);
  fragTree->SetBranchAddress("fA2", &pA2, &b_pA2);
  fragTree->SetBranchAddress("fAn1", &pAn1, &b_pAn1);
  fragTree->SetBranchAddress("fAn2", &pAn2, &b_pAn2);
  fragTree->SetBranchAddress("fAn1", &pAn1, &b_pAn1);
  fragTree->SetBranchAddress("fGr1", &pGr1, &b_pGr1);
  fragTree->SetBranchAddress("fGr2", &pGr2, &b_pGr2);
  fragTree->SetBranchAddress("fCat", &pCat, &b_pCat);
  fragTree->SetBranchAddress("fKE1", &pKE1, &b_pKE1);
  fragTree->SetBranchAddress("fKE2", &pKE2, &b_pKE2);
  fragTree->SetBranchAddress("fTheta1", &pTheta1, &b_pTheta1);
  fragTree->SetBranchAddress("fTheta2", &pTheta2, &b_pTheta2);

}
