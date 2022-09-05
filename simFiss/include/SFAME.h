// this class takes in simulated data and creates trees to be analyzed using root fame
// inputs
// Stefano, Kate August 4th, 2021

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TMatrixD.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <THStack.h>

#include "infoSystem.h"

#include <iostream>
#include <fstream>
// Header file for the classes stored in the TTree if any.

#include <string.h>
#include "Constants.h"
#include "DetectorClass.h"

using namespace std;

#ifndef sfame_h
#define sfame_h

class sfame {
public:
    sfame(InfoSystem* inInfo);

    InfoSystem* info;

    // readfromCol vars
    TString nameColFile;
    TString nameCollTree;

    // read from sim vars
    TString simFile;
    int FIRST_FILE;
    int NUM_FILES;

    // output file of SFAME
    TString OUTPUT_NAME;

    int numCollTreeFiles;
    int REUSE_DATA;




/*
  ___     _         _    _                   _____
 / __|___(_)_ _  __(_)__| |___ _ _  __ ___  |_   _| _ ___ ___
| (__/ _ \ | ' \/ _| / _` / -_) ' \/ _/ -_)   | || '_/ -_) -_)
 \___\___/_|_||_\__|_\__,_\___|_||_\__\___|   |_||_| \___\___|
*/

    TFile* SfameFiss;
    TTree* coincTree;
    TChain* rCollisionTree;
    // declatre the variables to store the fission branches
    // fission trigger
    int tMult = 0;
    double tTime = 0;
    double tDep = 0;
    int tChan = 0;
    double tPSP = 0;
    double tTail = 0;
    int tType = 0;

    // // fission beam
    // double bTime = 0;
    // double bErg = 0;
    // int bChan = 0;
    // int bIndex = 0;
    // double bPSP = 0;
    // double bTail = 0;

    // particles
    double totToF[MAX_MULTIPLICITY] = {0};
    double totPSP[MAX_MULTIPLICITY] = {0};
    double totDep[MAX_MULTIPLICITY] = {0};
    int totChan[MAX_MULTIPLICITY] = {0};
    double totTail[MAX_MULTIPLICITY] = {0};
    int totFlag[MAX_MULTIPLICITY] = {0};
    int totType[MAX_MULTIPLICITY] = {0};

/*
  ___     _ _ _    _            _____
 / __|___| | (_)__(_)___ _ _   |_   _| _ ___ ___
| (__/ _ \ | | (_-< / _ \ ' \    | || '_/ -_) -_)
 \___\___/_|_|_/__/_\___/_||_|   |_||_| \___\___|
*/
    TFile* outFile;
    TTree *collisionTree;
    Int_t fCurrent;
    // variable used for defining lengths of histories
    //attributes of each interaction
    int f_history; // stores the current history number
    int f_numLines; // this is a dynamic number (changes for every history) that stores the number of lines in the present history. If the history is not in the file, this number should be 0

    // variables of the tree as such they are arrays since a single history might contain more than one interaction
    // V
    int f_type[MAX_LINES] = {0};
    double f_energyDep[MAX_LINES] = {0}; // deposited
    double f_energyInc[MAX_LINES] = {0}; // incident
    int f_interaction[MAX_LINES] = {0};
    int f_zaid[MAX_LINES] = {0};
    int f_cell[MAX_LINES] = {0};
    double f_time[MAX_LINES] = {0};
    double f_weight[MAX_LINES] = {0};
    double f_posX[MAX_LINES] = {0};
    double f_posY[MAX_LINES] = {0};
    double f_posZ[MAX_LINES] = {0};
    int f_particle[MAX_LINES] = {0};
    // V
    int f_scatters[MAX_LINES] = {0};
    int f_code[MAX_LINES] = {0}; // fission or another interaction
    int f_generation[MAX_LINES] = {0};
    int f_flag[MAX_LINES] = {0};

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
    TBranch        *b_Flag;   //!

    /*
    ___                         _
   | _ \__ _ _ _ __ _ _ __  ___| |_ ___ _ _ ___
   |  _/ _` | '_/ _` | '  \/ -_)  _/ -_) '_(_-<
   |_| \__,_|_| \__,_|_|_|_\___|\__\___|_| /__/

   */

    // DETECTORS
    int NUM_DETECTORS;
    int TRIGGER_CHANNEL;
    double* SOURCE_POSITION;
    double THRESHOLD;
    double COINCIDENCE_WINDOW;
    double PULSE_GENERATION_WINDOW;

    // parameters to accept the trigger in every case
    double ACCEPT_PSP;
    double ACCEPT_TRIG;
    int ACCEPT_TYPE;
    
    double TIME_RES_TRIGGER;


    // external files
    TString DET_DIST_FILE; // detector distances
    TString TIME_RESOLUTION_FILE; // time resolution
    TString CELL_NUMBERS_FILE; // cell numbers
    TString LIGHTOUTPUT_FILE; // conversion between energyDep and lightoutput
    double* RESOLUTION_COEFFICIENTS;
    // energy file
    TString MEAN_NEUTRON_PSD_FILE;
    TString MEAN_PHOTON_PSD_FILE;
    TString SIGMA_NEUTRON_PSD_FILE;
    TString SIGMA_PHOTON_PSD_FILE;

    // supplemental files
    TString dEdX_FILE; // birks stopping power
    TString DET_BIRKCOEFF_FILE; // birks coeffs
    //
    TGraph* chanToCellList;
    TGraph* cellToChanList;
    TGraph* posXList;
    TGraph* posYList;
    TGraph* posZList;
    TGraph* timeResList;

    TGraph** LightOutList;
    TGraph** KinLightList;

    double** meanNeutPSDArray;
    double** meanPhotPSDArray;
    double** sigNeutPSDArray;
    double** sigPhotPSDArray;

    // initialization functions
    void getInfo();
    void initializeDetectors();
    void initializeInputFiles();

    // functions
    void readFromCol();
    void readFromSimFile(TString nameSim, int firstFile, int numFiles);
    void initializeCoincTree();
    void initializeCollisionTree();

    void readCollisionTree();
    Int_t GetCollEntry(Long64_t entry);
    Long64_t LoadCollTree(Long64_t entry);
    void LoopColl();


    // lightoutput functions
    int cellToChannel(int cell);
    int chanToCell(int chan);
    // void lightOutput();

    double birks(int channel, double energy, int typeP, int zaid);
    int cutLightOut(double height, int typeP);
    int cutTimePsd(double times, int typeP);

    // array of the detectors
    DetectorClass* detectors;

    ~sfame();
};

#endif
