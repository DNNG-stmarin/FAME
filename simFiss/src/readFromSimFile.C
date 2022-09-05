#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <string>
#include "Constants.h"

#include "SFAME.h"

void sfame::readFromSimFile(TString nameSim, int firstFile, int numFiles)
{

  // V
  cout << "read from sim file is running" << endl;
  const int MAX_MULT = 50;

  //declare dynamic array parameters
  int f_neutronMult;
  int f_gammaMult;
  int totMult;

  //declare branch arrays
  int neutronChannel[MAX_MULT] = {0};
  double neutronDetTimes[MAX_MULT] = {0};
  double neutronEnergy[MAX_MULT] = {0};
  double neutronIntegral[MAX_MULT] = {0};
  double neutronHeight[MAX_MULT] = {0};
  double neutronPSD[MAX_MULT] = {0};

  int photonChannel[MAX_MULT] = {0};
  double photonDetTimes[MAX_MULT] = {0};
  double photonEnergy[MAX_MULT] = {0};
  double photonIntegral[MAX_MULT] = {0};
  double photonHeight[MAX_MULT] = {0};
  double photonPSD[MAX_MULT] = {0};

  //int Flag[MAX_MULT] = {0};
  int neutronFlag[MAX_MULT] = {0};
  int photonFlag[MAX_MULT] = {0};

  //initalize fission tree
  TFile *outfile = TFile::Open("FissSim.root", "RECREATE");

  TTree *fissionTree = new TTree("fissionTree", "fissionTree");
  fissionTree->SetFileNumber(0);
  fissionTree->SetMaxTreeSize(1000000000LL);


  // initialie the branches
  fissionTree->Branch("neutronMult", &f_neutronMult, "neutronMult/I");
  fissionTree->Branch("gammaMult", &f_gammaMult, "gammaMult/I");

  fissionTree->Branch("neutronChannel", neutronChannel, "neutronChannel[neutronMult]/I");
  fissionTree->Branch("neutronDetTimes", neutronDetTimes, "neutronDetTimes[neutronMult]/D");
  fissionTree->Branch("neutronEnergy", neutronEnergy, "neutronEnergy[neutronMult]/D");
  fissionTree->Branch("neutronIntegral", neutronIntegral, "neutronIntegral[neutronMult]/D");
  fissionTree->Branch("neutronHeight", neutronHeight, "neutronHeight[neutronMult]/D");
  fissionTree->Branch("neutronPSD", neutronPSD, "neutronPSD[neutronMult]/D");
  fissionTree->Branch("photonChannel", photonChannel, "photonChannel[gammaMult]/I");
  fissionTree->Branch("photonDetTimes", photonDetTimes, "photonDetTimes[gammaMult]/D");
  fissionTree->Branch("photonEnergy", photonEnergy, "photonEnergy[gammaMult]/D");
  fissionTree->Branch("photonIntegral", photonIntegral, "photonIntegral[gammaMult]/D");
  fissionTree->Branch("photonHeight", photonHeight, "photonHeight[gammaMult]/D");
  fissionTree->Branch("photonPSD", photonPSD, "photonPSD[gammaMult]/D");

  //make 2 different flags for neutron and photon
  //fissionTree->Branch("Flag", Flag, "Flag[totMult]/I");
  fissionTree->Branch("neutronFlag", neutronFlag, "neutronFlag[neutronMult]/I");
  fissionTree->Branch("photonFlag", photonFlag, "photonFlag[gammaMult]/I");


  //iterate sim file
  for(int fileNum = firstFile; fileNum < firstFile + numFiles; fileNum++)
  {

    TString nameFile = nameSim + to_string(fileNum);
    cout << "reading from " << nameFile << endl;

    ifstream file(nameFile);
    if(!file.is_open())
    {
      cout << "error, file not found \n";
    }

    string line;
    double val;
    int col = 0;
    int nMult = 0;
    int pMult = 0;
    int prevhist = 0;
    double type, hist;

    int numFiss = 0;

    //iterate through lines
    while(getline(file, line))
    {

      stringstream ss(line);
      col = 2;

      //read in history of line (first column)
      ss >> hist;
      if(ss.peek() == ',')
        ss.ignore();
      // cout << "lineHist: " << hist << endl;

      //if new history
      if(int(hist) != prevhist)
      {
        if(prevhist != 0)
        {
          f_neutronMult = nMult;
          f_gammaMult = pMult;
          //totMult = pMult + nMult;
          fissionTree->Fill();
          numFiss++;
        }

        //set new history
        prevhist = int(hist);

        nMult = 0;
        pMult = 0;
      }

      while(int(hist)-1 != numFiss)
      {
        f_neutronMult = 0;
        f_gammaMult = 0;
        fissionTree->Fill();
        numFiss++;
      }

      //ignore comma and read type
      ss >> type;
      if(ss.peek() == ',')
        ss.ignore();

      //read in rest of line - regardless of new history or not
      while(ss >> val)
      {

        //neutron
        if(int(type) == 1) {
          if(col == 2)
            neutronChannel[nMult] = int(val);
          else if(col == 3)
            neutronDetTimes[nMult] = val;
          else if(col == 4)
            neutronEnergy[nMult] = val;
          else if(col == 5)
            neutronIntegral[nMult] = val;
          else if(col == 6)
            neutronHeight[nMult] = val;
          else if(col == 7)
            neutronPSD[nMult] = val;
          else if(col == 8)
            neutronFlag[nMult] = abs(1 - int(val)); // switch value of flag

          if(ss.peek() == ',')
            ss.ignore();
          col++;
        }

        //photon
        else if(int(type) == 2)
        {
          if(col == 2)
            photonChannel[pMult] = int(val);
          else if(col == 3)
            photonDetTimes[pMult] = val;
          else if(col == 4)
            photonEnergy[pMult] = val;
          else if(col == 5)
            photonIntegral[pMult] = val;
          else if(col == 6)
            photonHeight[pMult] = val;
          else if(col == 7)
            photonPSD[pMult] = val;
          else if(col == 8)
            photonFlag[pMult] = abs(1-int(val));  // switch value of flag

          if(ss.peek() == ',')
            ss.ignore();
          col++;
        }
      }

      //record event type
      if(int(type) == 1)
      {
        nMult++;
      }
      else if(int(type) == 2)
      {
        pMult++;
      }


    }

    // record the last histories in the buffer of this file
    f_neutronMult = nMult;
    f_gammaMult = pMult;
    //totMult = pMult + nMult;
    fissionTree->Fill();
    numFiss++;
    nMult = 0;
    pMult = 0;

  }


  outfile = fissionTree->GetCurrentFile();
  fissionTree->Write();
  delete outfile;

}













//
