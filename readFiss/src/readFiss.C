#include "readFiss.h"
#include <TLegend.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFitResult.h>
#include <THStack.h>
#include <TFolder.h>
#include <TSystem.h>

#include <iostream>
#include <fstream>
#include <sstream>



using namespace std;

//Destructor
readFiss::~readFiss()
{


    if (expFile != nullptr)
    {
      delete expFile;
    }

    if(mode == SIM_MODE && simFile != nullptr)
    {
      delete simFile;
    }

    if(mode == BEAM_MODE && beamFile != nullptr)
    {
      delete beamFile;
    }

    if(writeFile != nullptr)
    {
      delete writeFile;
    }

    if(TRIGGERS != nullptr)
    {
      delete[] TRIGGERS;
    }

    writeFile->Close();
}

//Get entry at integer <entry>
Int_t readFiss::GetExpEntry(Long64_t entry)
{
    // Read contents of entry.
    if (!expTree) return 0;
    return expTree->GetEntry(entry);
}
Int_t readFiss::GetSimEntry(Long64_t entry)
{
    // Read contents of entry.
    if (!simTree) return 0;
    return simTree->GetEntry(entry);
}
//Load root Trees
Long64_t readFiss::LoadExpTree(Long64_t entry)
{
    // Set the environment to read one entry
    if (!expTree) return -5;
    Long64_t centry = expTree->LoadTree(entry);
    if (centry < 0) return centry;
    if (expTree->GetTreeNumber() != fCurrent) {
        fCurrent = expTree->GetTreeNumber();
        Notify();
    }
    return centry;
}
Long64_t readFiss::LoadSimTree(Long64_t entry)
{
    // Set the environment to read one entry
    if (!simTree) return -5;
    Long64_t centry = simTree->LoadTree(entry);
    if (centry < 0) return centry;
    if (simTree->GetTreeNumber() != fCurrent) {
        fCurrent = simTree->GetTreeNumber();
        Notify();
    }
    return centry;
}

void readFiss::InitExp(TChain* tree)
{

    //INITIALIZE EXPERIMENT TREE
    if (!tree) return;
    expTree = tree;
    fCurrent = -1;
    expTree->SetMakeClass(1);

    expTree->SetBranchAddress("fisTime", &fisTime, &b_fisTime);
    expTree->SetBranchAddress("fisDep", &fisDep, &b_fisDep);
    expTree->SetBranchAddress("fisChan", &fisChan, &b_fisChan);
    expTree->SetBranchAddress("fisPSP", &fisPSP, &b_fisPSP);
    expTree->SetBranchAddress("neutronMult", &neutronMult, &b_neutronMult);
    expTree->SetBranchAddress("gammaMult", &gammaMult, &b_gammaMult);
    expTree->SetBranchAddress("neutronBackMult", &neutronBackMult, &b_neutronBackMult);
    expTree->SetBranchAddress("gammaBackMult", &gammaBackMult, &b_gammaBackMult);
    expTree->SetBranchAddress("neutronDetTimes", neutronDetTimes, &b_neutronDetTimes);
    expTree->SetBranchAddress("neutronLightOut", neutronLightOut, &b_neutronLightOut);
    expTree->SetBranchAddress("neutronPSD", neutronPSD, &b_neutronPSD);
    expTree->SetBranchAddress("neutronToFErg", neutronToFErg, &b_neutronToFErg);
    expTree->SetBranchAddress("neutronDet", neutronDet, &b_neutronDet);
    expTree->SetBranchAddress("neutronVx", neutronVx, &b_neutronVx);
    expTree->SetBranchAddress("neutronVy", neutronVy, &b_neutronVy);
    expTree->SetBranchAddress("neutronVz", neutronVz, &b_neutronVz);

    expTree->SetBranchAddress("photonDetTimes", photonDetTimes, &b_photonDetTimes);
    expTree->SetBranchAddress("photonLightOut", photonLightOut, &b_photonLightOut);
    expTree->SetBranchAddress("photonPSD", photonPSD, &b_photonPSD);
    expTree->SetBranchAddress("photonDet", photonDet, &b_photonDet);
    expTree->SetBranchAddress("photonVx", photonVx, &b_photonVx);
    expTree->SetBranchAddress("photonVy", photonVy, &b_photonVy);
    expTree->SetBranchAddress("photonVz", photonVz, &b_photonVz);

    expTree->SetBranchAddress("backNeutronDetTimes", backNeutronDetTimes, &b_backNeutronDetTimes);
    expTree->SetBranchAddress("backNeutronLightOut", backNeutronLightOut, &b_backNeutronLightOut);
    expTree->SetBranchAddress("backNeutronPSD", backNeutronPSD, &b_backNeutronPSD);
    expTree->SetBranchAddress("backNeutronToFErg", backNeutronToFErg, &b_backNeutronToFErg);
    expTree->SetBranchAddress("backNeutronDet", backNeutronDet, &b_backNeutronDet);
    expTree->SetBranchAddress("backNeutronVx", backNeutronVx, &b_backNeutronVx);
    expTree->SetBranchAddress("backNeutronVy", backNeutronVy, &b_backNeutronVy);
    expTree->SetBranchAddress("backNeutronVz", backNeutronVz, &b_backNeutronVz);

    expTree->SetBranchAddress("backPhotonDetTimes", backPhotonDetTimes, &b_backPhotonDetTimes);
    expTree->SetBranchAddress("backPhotonLightOut", backPhotonLightOut, &b_backPhotonLightOut);
    expTree->SetBranchAddress("backPhotonPSD", backPhotonPSD, &b_backPhotonPSD);
    expTree->SetBranchAddress("backPhotonDet", backPhotonDet, &b_backPhotonDet);
    expTree->SetBranchAddress("backPhotonVx", backPhotonVx, &b_backPhotonVx);
    expTree->SetBranchAddress("backPhotonVy", backPhotonVy, &b_backPhotonVy);
    expTree->SetBranchAddress("backPhotonVz", backPhotonVz, &b_backPhotonVz);

    if(mode == BEAM_MODE)
    {
      expTree->SetBranchAddress("beamTime", &beamTime, &b_beamTime);
      expTree->SetBranchAddress("beamEnergy", &beamEnergy, &b_beamEnergy);
      expTree->SetBranchAddress("beamDep", &beamDep, &b_beamDep);
      expTree->SetBranchAddress("beamPSP", &beamPSP, &b_beamPSP);
      expTree->SetBranchAddress("beamChan", &beamChan, &b_beamChan);
      expTree->SetBranchAddress("beamIndex", &beamIndex, &b_beamIndex);
    }

    if(mode == FRAG_MODE)
    {
      expTree->SetBranchAddress("AL", &AL, &b_AL);
      expTree->SetBranchAddress("AH", &AH, &b_AH);
      expTree->SetBranchAddress("KEL", &KEL, &b_KEL);
      expTree->SetBranchAddress("KEH", &KEH, &b_KEH);
      expTree->SetBranchAddress("ThetaL", &ThetaL, &b_ThetaL);
      expTree->SetBranchAddress("ThetaH", &ThetaH, &b_ThetaH);
      expTree->SetBranchAddress("EX", &EX, &b_EX);
      expTree->SetBranchAddress("Anode1", &Anode1, &b_Anode1);
      expTree->SetBranchAddress("Anode2", &Anode2, &b_Anode2);
      expTree->SetBranchAddress("Grid1", &Grid1, &b_Grid1);
      expTree->SetBranchAddress("Grid2", &Grid2, &b_Grid2);
      expTree->SetBranchAddress("Cathode", &Cathode, &b_Cathode);

      expTree->SetBranchAddress("photonCosF", photonCosF, &b_photonCosF);
      expTree->SetBranchAddress("neutronCosF", neutronCosF, &b_neutronCosF);
      expTree->SetBranchAddress("backPhotonCosF", backPhotonCosF, &b_backPhotonCosF);
      expTree->SetBranchAddress("backNeutronCosF", backNeutronCosF, &b_backNeutronCosF);
    }
    Notify();


}

void readFiss::InitSim(TChain* tree)
{
    //INTITIALZE SIM TREE
    if (!tree) return;
    simTree = tree;
    fCurrent = -1;
    simTree->SetMakeClass(1);

    simTree->SetBranchAddress("neutronFlag", neutronFlag, &b_neutronFlagSim);
    simTree->SetBranchAddress("photonFlag", photonFlag, &b_photonFlagSim);

    simTree->SetBranchAddress("fisTime", &fisTime, &b_fisTimeSim);
    simTree->SetBranchAddress("fisDep", &fisDep, &b_fisDepSim);
    simTree->SetBranchAddress("fisChan", &fisChan, &b_fisChanSim);
    simTree->SetBranchAddress("fisPSP", &fisPSP, &b_fisPSPSim);
    simTree->SetBranchAddress("neutronMult", &neutronMult, &b_neutronMultSim);
    simTree->SetBranchAddress("gammaMult", &gammaMult, &b_gammaMultSim);
    simTree->SetBranchAddress("neutronBackMult", &neutronBackMult, &b_neutronBackMultSim);
    simTree->SetBranchAddress("gammaBackMult", &gammaBackMult, &b_gammaBackMultSim);
    simTree->SetBranchAddress("neutronDetTimes", neutronDetTimes, &b_neutronDetTimesSim);
    simTree->SetBranchAddress("neutronLightOut", neutronLightOut, &b_neutronLightOutSim);
    simTree->SetBranchAddress("neutronPSD", neutronPSD, &b_neutronPSDSim);
    simTree->SetBranchAddress("neutronToFErg", neutronToFErg, &b_neutronToFErgSim);
    simTree->SetBranchAddress("neutronDet", neutronDet, &b_neutronDetSim);
    simTree->SetBranchAddress("neutronVx", neutronVx, &b_neutronVxSim);
    simTree->SetBranchAddress("neutronVy", neutronVy, &b_neutronVySim);
    simTree->SetBranchAddress("neutronVz", neutronVz, &b_neutronVzSim);
    simTree->SetBranchAddress("photonDetTimes", photonDetTimes, &b_photonDetTimesSim);
    simTree->SetBranchAddress("photonLightOut", photonLightOut, &b_photonLightOutSim);
    simTree->SetBranchAddress("photonPSD", photonPSD, &b_photonPSDSim);
    simTree->SetBranchAddress("photonDet", photonDet, &b_photonDetSim);
    simTree->SetBranchAddress("photonVx", photonVx, &b_photonVxSim);
    simTree->SetBranchAddress("photonVy", photonVy, &b_photonVySim);
    simTree->SetBranchAddress("photonVz", photonVz, &b_photonVzSim);
    simTree->SetBranchAddress("backNeutronDetTimes", backNeutronDetTimes, &b_backNeutronDetTimesSim);
    simTree->SetBranchAddress("backNeutronLightOut", backNeutronLightOut, &b_backNeutronLightOutSim);
    simTree->SetBranchAddress("backNeutronPSD", backNeutronPSD, &b_backNeutronPSDSim);
    simTree->SetBranchAddress("backNeutronToFErg", backNeutronToFErg, &b_backNeutronToFErgSim);
    simTree->SetBranchAddress("backNeutronDet", backNeutronDet, &b_backNeutronDetSim);
    simTree->SetBranchAddress("backNeutronVx", backNeutronVx, &b_backNeutronVxSim);
    simTree->SetBranchAddress("backNeutronVy", backNeutronVy, &b_backNeutronVySim);
    simTree->SetBranchAddress("backNeutronVz", backNeutronVz, &b_backNeutronVzSim);
    simTree->SetBranchAddress("backPhotonDetTimes", backPhotonDetTimes, &b_backPhotonDetTimesSim);
    simTree->SetBranchAddress("backPhotonLightOut", backPhotonLightOut, &b_backPhotonLightOutSim);
    simTree->SetBranchAddress("backPhotonPSD", backPhotonPSD, &b_backPhotonPSDSim);
    simTree->SetBranchAddress("backPhotonDet", backPhotonDet, &b_backPhotonDetSim);
    simTree->SetBranchAddress("backPhotonVx", backPhotonVx, &b_backPhotonVxSim);
    simTree->SetBranchAddress("backPhotonVy", backPhotonVy, &b_backPhotonVySim);
    simTree->SetBranchAddress("backPhotonVz", backPhotonVz, &b_backPhotonVzSim);


    Notify();
}
Bool_t readFiss::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

void readFiss::ShowExp(Long64_t entry)
{
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!expTree) return;
    expTree->Show(entry);
}
void readFiss::ShowSim(Long64_t entry)
{
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!simTree) return;
    simTree->Show(entry);
}
Int_t readFiss::Cut(Long64_t entry)
{
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    return 1;
}
