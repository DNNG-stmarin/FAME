#include "SFAME.h"

void sfame::readCollisionTree()
{

  // TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(nameCollTree);
  // if (!f || !f->IsOpen())
  // {
  //   f = new TFile(nameCollTree);
  // }

  rCollisionTree = new TChain("CollisionFile");
  rCollisionTree->Add("collTree.root");

  cout << "added collTree.root" << endl;


  for(int i = 1; i < numCollTreeFiles; i++)
  {
    rCollisionTree->Add("collTree_" + (TString)to_string(i) + ".root");
    cout << "added " << "collTree_" + (TString)to_string(i) + ".root" << endl;
  }
  cout << rCollisionTree->GetEntries() << endl;



  fCurrent = -1;
  // rCollisionTree->SetMakeClass(1);

  rCollisionTree->SetBranchAddress("History", &f_history, &b_History);
  rCollisionTree->SetBranchAddress("NumLines", &f_numLines, &b_NumLines);
  rCollisionTree->SetBranchAddress("Type", f_type, &b_Type);
  rCollisionTree->SetBranchAddress("EnergyDeposited", f_energyDep, &b_EnergyDeposited);
  rCollisionTree->SetBranchAddress("EnergyIncident", f_energyInc, &b_EnergyIncident);
  rCollisionTree->SetBranchAddress("Interaction", f_interaction, &b_Interaction);
  rCollisionTree->SetBranchAddress("Zaid", f_zaid, &b_Zaid);
  rCollisionTree->SetBranchAddress("Cell", f_cell, &b_Cell);
  rCollisionTree->SetBranchAddress("Time", f_time, &b_Time);
  rCollisionTree->SetBranchAddress("Weight", f_weight, &b_Weight);
  rCollisionTree->SetBranchAddress("PositionX", f_posX, &b_PositionX);
  rCollisionTree->SetBranchAddress("PositionY", f_posY, &b_PositionY);
  rCollisionTree->SetBranchAddress("PositionZ", f_posZ, &b_PositionZ);
  rCollisionTree->SetBranchAddress("Particle", f_particle, &b_Particle);
  rCollisionTree->SetBranchAddress("Scatters", f_scatters, &b_Scatters);
  rCollisionTree->SetBranchAddress("Code", f_code, &b_Code);
  rCollisionTree->SetBranchAddress("Generation", f_generation, &b_Generation);
}


Int_t sfame::GetCollEntry(Long64_t entry)
{
// Read contents of entry.
   if (!rCollisionTree) return 0;
   return rCollisionTree->GetEntry(entry);
}

Long64_t sfame::LoadCollTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!rCollisionTree) return -5;
   Long64_t centry = rCollisionTree->LoadTree(entry);
   if (centry < 0) return centry;
   if (rCollisionTree->GetTreeNumber() != fCurrent) {
      fCurrent = rCollisionTree->GetTreeNumber();
   }
   return centry;
}
