#include "SFAME.h"
#include "Constants.h"
#include "DetectorClass.h"

void sfame::initializeCoincTree()
{
/*
  _____      _            _     _                       _______
 / ____|    (_)          (_)   | |                     |__   __|
| |     ___  _ _ __   ___ _  __| | ___ _ __   ___ ___     | |_ __ ___  ___
| |    / _ \| | '_ \ / __| |/ _` |/ _ \ '_ \ / __/ _ \    | | '__/ _ \/ _ \
| |___| (_) | | | | | (__| | (_| |  __/ | | | (_|  __/    | | | |  __/  __/
 \_____\___/|_|_| |_|\___|_|\__,_|\___|_| |_|\___\___|    |_|_|  \___|\___|

*/

  cout << "Initializing coincidence TTrees " << endl;
  SfameFiss = new TFile(OUTPUT_NAME, "RECREATE");

	SfameFiss->cd();

	coincTree = new TTree("CoincidenceTree", "Tree of Simulated Coincidences");
	coincTree->SetFileNumber(0);

	// trigger variables
	coincTree->Branch("tMult", &tMult, "tMult/I");
	coincTree->Branch("tTime", &tTime, "fissionTime/D");
	coincTree->Branch("tDep", &tDep, "fissionErg/D");
	coincTree->Branch("tChan", &tChan, "fissionChan/I");
	coincTree->Branch("tPSP", &tPSP, "fissionPSP/D");
	coincTree->Branch("tTail", &tTail, "fissionTail/D");
	coincTree->Branch("tType", &tType, "fissionType/I");

	// // beam variables
	// if(NUM_BEAMS > 0)
	// {
	// 	coincTree->Branch("bTime", &bTime, "beamTime/D");
	// 	coincTree->Branch("bErg", &bErg, "beamEnergy/D");
	// 	coincTree->Branch("bChan", &bChan, "beamChan/I");
	// 	coincTree->Branch("bIndex", &bIndex, "beamIndex/I");
	// 	coincTree->Branch("bPSP", &bPSP, "beamPSP/D");
	// 	coincTree->Branch("bTail", &bTail, "beamTail/D");
	// }

	// list variables
	coincTree->Branch("totToF", totToF, "totToF[tMult]/D");
	coincTree->Branch("totType", totType, "totType[tMult]/I");

	coincTree->Branch("totPSP", totPSP, "totPSP[tMult]/D");
	coincTree->Branch("totDep", totDep, "totDep[tMult]/D");
	coincTree->Branch("totChan", totChan, "totChan[tMult]/I");
	coincTree->Branch("totTail", totTail, "totTail[tMult]/D");
  coincTree->Branch("totFlag", totFlag, "totFlag[tMult]/I");
  cout << "setting flag branch" << endl;
	coincTree->SetMaxTreeSize(1000000000LL);

}
