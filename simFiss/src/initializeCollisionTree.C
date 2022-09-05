#include "SFAME.h"
#include "Constants.h"
#include "DetectorClass.h"

void sfame::initializeCollisionTree()
{

  outFile = new TFile("collTree.root", "RECREATE");
  outFile->cd();

  cout << "Initializing Collision Tree" << endl;

  collisionTree = new TTree("CollisionFile", "CollisionFile");
  collisionTree->SetFileNumber(0);
  collisionTree->SetMaxTreeSize(1000000000LL);


  collisionTree->Branch("History", &f_history, "History/I");
  collisionTree->Branch("NumLines", &f_numLines, "NumLines/I");
  // V check the varibles, some have to change format (doubles change to /D ?)
  collisionTree->Branch("Type", f_type, "Type[NumLines]/I");
  collisionTree->Branch("EnergyDeposited", f_energyDep, "EnergyDeposited[NumLines]/D");
  collisionTree->Branch("EnergyIncident", f_energyInc, "EnergyIncident[NumLines]/D");
  collisionTree->Branch("Interaction", f_interaction, "Interaction[NumLines]/I");
  collisionTree->Branch("Zaid", f_zaid, "Zaid[NumLines]/I");
  collisionTree->Branch("Cell", f_cell, "Cell[NumLines]/I");
  collisionTree->Branch("Time", f_time, "Time[NumLines]/D");
  collisionTree->Branch("Weight", f_weight, "Weight[NumLines]/D");
  collisionTree->Branch("PositionX", f_posX, "PositionX[NumLines]/D");
  collisionTree->Branch("PositionY", f_posY, "PositionY[NumLines]/D");
  collisionTree->Branch("PositionZ", f_posZ, "PositionZ[NumLines]/D");
  collisionTree->Branch("Particle", f_particle, "Particle[NumLines]/I");
  collisionTree->Branch("Scatters", f_scatters, "Scatters[NumLines]/I");
  collisionTree->Branch("Code", f_code, "Code[NumLines]/I");
  collisionTree->Branch("Generation", f_generation, "Generation[NumLines]/I");
}
