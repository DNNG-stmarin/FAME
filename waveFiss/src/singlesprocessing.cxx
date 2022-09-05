////////////////////////////////////////////////////////////////////////////////
///  \file   singles_processing.cxx
///  \author Shea M. Mosby
///  \email  smosby@lanl.gov
///  \author Devin S. Connolly
///  \email  dconnolly@lanl.gov
////////////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h> /* atoi */
#include <iomanip>
#include <zlib.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>

// ROOT includes
#include "TTree.h"
#include "TFile.h"

// File includes
#include "Structures.h"
#include "WaveformProcessing.h"

int main(int argc, char* argv[])
{
  gErrorIgnoreLevel = 6001;
  // Creation of variables section
  char   ipath[512];
  char   opath[512];
  char   buf[512];
  string cfgfile;

  Int_t EvtBreakout = 1e8; // stop analyzing after this many events

  printf("\n");
  printf(" _______________________________________________________\n");
  printf("|                                                       |\n");
  printf("|                CAEN Singles Processing                |\n");
  printf("|-------------------------------------------------------|\n");
  printf("|      Wave Process DPP-enabled CAEN digital ouput      |\n");
  printf("|                                                       |\n");
  printf("|           (send bugs to dconnolly@lanl.gov)           |\n");
  printf("|_______________________________________________________|\n");
  printf("\n");

  // Initialize the waveform processing settings
  TKESettings tke;
  /////////////////////////////////////////////////////
  // SET UP YOUR WAVEFORM PROCESSING PARAMETERS HERE //
  /////////////////////////////////////////////////////
  // defaults
  tke.reset();
  Int_t i_bnum  = 0;


  // Board 0 T0
  Int_t i_chnum = 5;
  tke.sampleDelta[i_bnum][i_chnum]      = 1;
  tke.derivePointDelta[i_bnum][i_chnum] = 3;
  tke.Ns[i_bnum][i_chnum]               = 60;
  tke.nZero[i_bnum][i_chnum]            = 4;
  tke.tMethod[i_bnum][i_chnum]          = "derivative";
  tke.interp[i_bnum][i_chnum]           = "cubic";
  tke.thresh[i_bnum][i_chnum]           = 5;
  tke.polarity[i_bnum][i_chnum]         = "positive";
  tke.bLineOS[i_bnum][i_chnum]          = 1;
  tke.bLineNpts[i_bnum][i_chnum]        = 10;
  tke.eMethod[i_bnum][i_chnum]          = "chargeInt";
  tke.nPeak[i_bnum][i_chnum]            = 10;
  tke.tOffset[i_bnum][i_chnum]          = 0;
  tke.eOffset[i_bnum][i_chnum]          = -1;

  // Fast Cathode
  i_chnum                        = 0;
  tke.bLineNpts[i_bnum][i_chnum] = 16;
  tke.bLineOS[i_bnum][i_chnum]   = 1;
  tke.eMethod[i_bnum][i_chnum]   = "chargeInt";
  tke.interp[i_bnum][i_chnum]    = "linear";
  tke.nPeak[i_bnum][i_chnum]     = 16;
  tke.Ns[i_bnum][i_chnum]        = WF_SIZE;
  tke.nZero[i_bnum][i_chnum]     = 4;
  tke.tOffset[i_bnum][i_chnum]   = 10;
  tke.polarity[i_bnum][i_chnum]  = "negative";
  tke.thresh[i_bnum][i_chnum]    = -1;
  tke.tau[i_bnum][i_chnum]       = 9375;
  tke.tMethod[i_bnum][i_chnum]   = "derivative";
  tke.derivePointDelta[i_bnum][i_chnum]   = 3;
  tke.frac[i_bnum][i_chnum]      = 0.98;
  tke.eOffset[i_bnum][i_chnum]   = 50;

  // ion chamber anodes
  for (i_chnum = 1; i_chnum < 3; ++i_chnum) {
    tke.Ns[i_bnum][i_chnum]        = WF_SIZE;
    tke.thresh[i_bnum][i_chnum]    = -1;
    tke.polarity[i_bnum][i_chnum]  = "positive";
    tke.bLineOS[i_bnum][i_chnum]   = 1;
    tke.bLineNpts[i_bnum][i_chnum] = 8;
    tke.eMethod[i_bnum][i_chnum]   = "trapFilter";
    tke.tRise[i_bnum][i_chnum]     = 20;
    tke.tGap[i_bnum][i_chnum]      = 50;
    tke.tPeak[i_bnum][i_chnum]     = tke.tGap[i_bnum][i_chnum] - 1;
    tke.nPeak[i_bnum][i_chnum]     = 16;//2;
    tke.tMethod[i_bnum][i_chnum]   = "derivative";
    tke.ffRise[i_bnum][i_chnum]    = 16;
    // tke.delay[i_bnum][i_chnum]     = 4;
    // tke.frac[i_bnum][i_chnum]      = 0.98;
    tke.interp[i_bnum][i_chnum]    = "linear";
    tke.nZero[i_bnum][i_chnum]     = 4;
    tke.tOffset[i_bnum][i_chnum]   = 4;
    tke.tau[i_bnum][i_chnum]       = 9375;
    tke.phGuess[i_bnum][i_chnum]   = 3000;
    tke.sampleDelta[i_bnum][i_chnum] = 1;
    tke.derivePointDelta[i_bnum][i_chnum] = 3;
    // tke.nPeak[i_bnum][i_chnum] = 300; // for U5
    // tke.tOffset[i_bnum][i_chnum] = 400; // for U5
    // tke.bLineNpts[i_bnum][i_chnum] = 150; // good for U5
  }

  // ion chamber grids
  for (i_chnum = 3; i_chnum < 5; ++i_chnum) {
    tke.Ns[i_bnum][i_chnum]        = WF_SIZE;
    tke.polarity[i_bnum][i_chnum]  = "positive";
    tke.tMethod[i_bnum][i_chnum]   = "derivative";
    tke.thresh[i_bnum][i_chnum]    = -1;
    tke.bLineOS[i_bnum][i_chnum]   = 10;
    tke.bLineNpts[i_bnum][i_chnum] = 16;
    tke.eMethod[i_bnum][i_chnum]   = "chargeIntGrid";
    tke.nPeak[i_bnum][i_chnum]     = 16;
    tke.nZero[i_bnum][i_chnum]     = 4;
    tke.interp[i_bnum][i_chnum]    = "cubic";
    tke.tOffset[i_bnum][i_chnum]   = 4;
    tke.gOffset[i_bnum][i_chnum]   = 30;

    // tke.nPeak[i_bnum][i_chnum] = 200; // good for U5
    // tke.bLineNpts[i_bnum][i_chnum] = 200; /// good for U5
  }

  /////////////////////////////////////////////
  // END WAVEFORM PROCESSING PARAMETER SETUP //
  /////////////////////////////////////////////

  // Parsing of input parameters
  if (argc == 3) {
    sprintf(ipath, "%s", argv[1]);
    sprintf(opath, "%s", argv[2]);
  } else if (argc == 4) {
    sprintf(ipath, "%s", argv[1]);
    sprintf(opath, "%s", argv[2]);
    cfgfile = argv[3];
  } else {
    printf("--> EXIT:\nPlease, start singles processing as follows: \n");
    printf("--> EXIT: singlesprocessing string=PATH_TO_FILE string=PATH_TO_OUTPUT\n");
    return -1;
  }

  cout << "Reading config file\n";
  ifstream cfgf;
  cfgf.open(cfgfile.c_str());

  if (!(cfgf.is_open())) {
    cout << "!!!Error!!! could not open config file - using default analysis parameters\n";
  } else {
    // mmsg.str("");
    // mmsg << "Configuration File " << cfgfile.c_str() << " is Open";
    // Spider_Success("Main", mmsg.str());
    string item;
    string line;
    while (getline(cfgf, line)) {
      if (line.compare(0, 1, "#") == 0) {
        continue;
      } else {
        cfgf >> item;
        if (item.compare("breakout") == 0) { cfgf >> EvtBreakout; }
        if (item.compare("anode_eMethod") == 0) { cfgf >> tke.eMethod[i_bnum][2]; }
        if (item.compare("anode_tMethod") == 0) { cfgf >> tke.tMethod[i_bnum][2]; }
        if (item.compare("anode_interp") == 0) { cfgf >> tke.interp[i_bnum][2]; }
        if (item.compare("anode_Ns") == 0) { cfgf >> tke.Ns[i_bnum][2]; }
        if (item.compare("anode_thresh") == 0) {
          cfgf >> tke.thresh[i_bnum][2];
          cfgf >> tke.thresh[i_bnum][3];
        }
        if (item.compare("anode_bLineOS") == 0) {
          cfgf >> tke.bLineOS[i_bnum][2];
          cfgf >> tke.bLineOS[i_bnum][3];
        }
        if (item.compare("anode_bLineNpts") == 0) {
          cfgf >> tke.bLineNpts[i_bnum][2];
          cfgf >> tke.bLineNpts[i_bnum][3];
        }
        if (item.compare("anode_nZero") == 0) {
          cfgf >> tke.nZero[i_bnum][2];
          cfgf >> tke.nZero[i_bnum][3];
        }
        if (item.compare("anode_tOffset") == 0) {
          cfgf >> tke.tOffset[i_bnum][2];
          cfgf >> tke.tOffset[i_bnum][3];
        }
        if (item.compare("anode_tau") == 0) {
          cfgf >> tke.tau[i_bnum][2];
          cfgf >> tke.tau[i_bnum][3];
        }
        if (item.compare("anode_ffRise") == 0) {
          cfgf >> tke.ffRise[i_bnum][2];
          cfgf >> tke.ffRise[i_bnum][3];
        }
        if (item.compare("anode_tRise") == 0) {
          cfgf >> tke.tRise[i_bnum][2];
          cfgf >> tke.tRise[i_bnum][3];
        }
        if (item.compare("anode_tGap") == 0) {
          cfgf >> tke.tGap[i_bnum][2];
          cfgf >> tke.tGap[i_bnum][3];
        }
        if (item.compare("anode_tPeak") == 0) {
          cfgf >> tke.tPeak[i_bnum][2];
          cfgf >> tke.tPeak[i_bnum][3];
        }
        if (item.compare("anode_nPeak") == 0) {
          cfgf >> tke.nPeak[i_bnum][3];
          cfgf >> tke.nPeak[i_bnum][2];
        }
        if (item.compare("anode_delay") == 0) {
          cfgf >> tke.delay[i_bnum][2];
          cfgf >> tke.delay[i_bnum][3];
        }
        if (item.compare("anode_frac") == 0) {
          cfgf >> tke.frac[i_bnum][2];
          cfgf >> tke.frac[i_bnum][3];
        }
        if (item.compare("anode_phGuess") == 0) { cfgf >> tke.phGuess[i_bnum][2]; }
        if (item.compare("grid_Ns") == 0) { cfgf >> tke.Ns[i_bnum][4]; }
        if (item.compare("grid_polarity") == 0) { cfgf >> tke.polarity[i_bnum][4]; }
        if (item.compare("grid_eMethod") == 0) { cfgf >> tke.eMethod[i_bnum][4]; }
        if (item.compare("grid_bLineOS") == 0) { cfgf >> tke.bLineOS[i_bnum][4]; }
        if (item.compare("grid_bLineNpts") == 0) { cfgf >> tke.bLineNpts[i_bnum][4]; }
        if (item.compare("grid_nPeak") == 0) {
          cfgf >> tke.nPeak[i_bnum][4];
          tke.tOffset[i_bnum][4] = tke.Ns[i_bnum][4] - tke.nPeak[i_bnum][4];
        }
        if (item.compare("cathode_Ns") == 0) { cfgf >> tke.Ns[i_bnum][6]; }
        if (item.compare("cathode_polarity") == 0) { cfgf >> tke.polarity[i_bnum][6]; }
        if (item.compare("cathode_eMethod") == 0) { cfgf >> tke.eMethod[i_bnum][6]; }
        if (item.compare("cathode_tMethod") == 0) { cfgf >> tke.tMethod[i_bnum][6]; }
        if (item.compare("cathode_interp") == 0) { cfgf >> tke.interp[i_bnum][6]; }
        if (item.compare("cathode_nZero") == 0) { cfgf >> tke.nZero[i_bnum][6]; }
        if (item.compare("cathode_thresh") == 0) { cfgf >> tke.thresh[i_bnum][6]; }
        if (item.compare("cathode_tau") == 0) { cfgf >> tke.tau[i_bnum][6]; }
        if (item.compare("cathode_ffRise") == 0) { cfgf >> tke.ffRise[i_bnum][6]; }
        if (item.compare("cathode_nPeak") == 0) { cfgf >> tke.tPeak[i_bnum][6]; }
        if (item.compare("cathode_delay") == 0) { cfgf >> tke.delay[i_bnum][6]; }
        if (item.compare("cathode_frac") == 0) { cfgf >> tke.frac[i_bnum][6]; }
        if (item.compare("cathode_bLineOS") == 0) { cfgf >> tke.bLineOS[i_bnum][6]; }
        if (item.compare("cathode_bLineNpts") == 0) { cfgf >> tke.bLineNpts[i_bnum][6]; }
        if (item.compare("cathode_tOffset") == 0) { cfgf >> tke.tOffset[i_bnum][6]; }
      }
    }
  }

  cfgf.close();

  tke.polarity[i_bnum][2] = tke.polarity[i_bnum][1];
  tke.tMethod[i_bnum][2]  = tke.tMethod[i_bnum][1];
  tke.eMethod[i_bnum][2]  = tke.eMethod[i_bnum][1];
  tke.interp[i_bnum][2]   = tke.interp[i_bnum][1];
  tke.Ns[i_bnum][2]       = tke.Ns[i_bnum][1];
  // tke.thresh[i_bnum][2]    = tke.thresh[i_bnum][1];
  // tke.bLineOS[i_bnum][2]   = tke.bLineOS[i_bnum][1];
  // tke.bLineNpts[i_bnum][2] = tke.bLineNpts[i_bnum][1];
  // tke.tRise[i_bnum][2]     = tke.tRise[i_bnum][1];
  // tke.tGap[i_bnum][2]      = tke.tGap[i_bnum][1];
  // tke.tPeak[i_bnum][2]     = tke.tPeak[i_bnum][1];
  // tke.nPeak[i_bnum][2]     = tke.nPeak[i_bnum][1];
  // tke.nZero[i_bnum][2]     = tke.nZero[i_bnum][1];
  // tke.ffRise[i_bnum][2]    = tke.ffRise[i_bnum][1];
  // tke.delay[i_bnum][2]     = tke.delay[i_bnum][1];
  // tke.frac[i_bnum][2]      = tke.frac[i_bnum][1];
  // tke.tOffset[i_bnum][2]   = tke.tOffset[i_bnum][1];
  // tke.tau[i_bnum][2]       = tke.tau[i_bnum][1];

  tke.Ns[i_bnum][4]        = tke.Ns[i_bnum][3];
  tke.polarity[i_bnum][4]  = tke.polarity[i_bnum][3];
  tke.thresh[i_bnum][4]    = tke.thresh[i_bnum][3];
  tke.bLineOS[i_bnum][4]   = tke.bLineOS[i_bnum][3];
  tke.bLineNpts[i_bnum][4] = tke.bLineNpts[i_bnum][3];
  tke.eMethod[i_bnum][4]   = tke.eMethod[i_bnum][3];
  tke.nPeak[i_bnum][4]     = tke.nPeak[i_bnum][3];
  tke.tOffset[i_bnum][4]   = tke.tOffset[i_bnum][3];


  // printing out

  cout << setw(32) << left << "Event limit: " << left << EvtBreakout << "\n";
  cout << setw(32) << left << "Anodes: "
       << "\n";
  cout << setw(32) << left << "\tpolarity: " << left << tke.polarity[i_bnum][2] << "\n";
  cout << setw(32) << left << "\tE method: " << left << tke.eMethod[i_bnum][2] << "\n";
  cout << setw(32) << left << "\tT method: " << left << tke.tMethod[i_bnum][2] << "\n";
  cout << setw(32) << left << "\tZero interpolation: " << left << tke.interp[i_bnum][2] << "\n";
  cout << setw(32) << left << "\t# interpolation pts: " << left << tke.nZero[i_bnum][2] << "\n";
  cout << setw(32) << left << "\tN samples: " << left << tke.Ns[i_bnum][2] << "\n";
  cout << setw(32) << left << "\tthreshold: " << setw(16) << left << tke.thresh[i_bnum][2] << left
       << tke.thresh[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tbaseline offset: " << setw(16) << left << tke.bLineOS[i_bnum][2]
       << left << tke.bLineOS[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tbaseline N pts: " << setw(16) << left << tke.bLineNpts[i_bnum][2]
       << left << tke.bLineNpts[i_bnum][3] << "\n";
  cout << setw(32) << left << "\ttiming offset: " << setw(16) << left << tke.tOffset[i_bnum][2]
       << left << tke.tOffset[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tdecay constant: " << setw(16) << left << tke.tau[i_bnum][2] << left
       << tke.tau[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tfast filter rise time: " << setw(16) << left
       << tke.ffRise[i_bnum][2] << left << tke.ffRise[i_bnum][3] << "\n";
  cout << setw(32) << left << "\ttrapezoid rise time: " << setw(16) << left << tke.tRise[i_bnum][2]
       << left << tke.tRise[i_bnum][3] << "\n";
  cout << setw(32) << left << "\ttrapezoid gap time: " << setw(16) << left << tke.tGap[i_bnum][2]
       << left << tke.tGap[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tpeak npts: " << setw(16) << left << tke.nPeak[i_bnum][2] << left
       << tke.nPeak[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tpeaking time: " << setw(16) << left << tke.tPeak[i_bnum][2] << left
       << tke.tPeak[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tcfd delay: " << setw(16) << left << tke.delay[i_bnum][2] << left
       << tke.delay[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tcfd fraction: " << setw(16) << left << tke.frac[i_bnum][2] << left
       << tke.frac[i_bnum][3] << "\n";
  cout << setw(32) << left << "\tpeak height guess: " << left << tke.phGuess[i_bnum][2] << "\n";
  cout << setw(32) << left << "Grids: "
       << "\n";
  cout << setw(32) << left << "\tpolarity: " << left << tke.polarity[i_bnum][4] << "\n";
  cout << setw(32) << left << "\tE method: " << left << tke.eMethod[i_bnum][4] << "\n";
  cout << setw(32) << left << "\tT method: " << left << tke.tMethod[i_bnum][4] << "\n";
  cout << setw(32) << left << "\tN samples: " << left << tke.Ns[i_bnum][4] << "\n";
  cout << setw(32) << left << "\tbaseline offset: " << left << tke.bLineOS[i_bnum][4] << "\n";
  cout << setw(32) << left << "\tbaseline N pts: " << left << tke.bLineNpts[i_bnum][4] << "\n";
  cout << setw(32) << left << "\tpeak npts: " << left << tke.nPeak[i_bnum][4] << "\n";
  cout << setw(32) << left << "\tpeak offset: " << left << tke.tOffset[i_bnum][4] << "\n";
  cout << setw(32) << left << "Cathode: "
       << "\n";
  cout << setw(32) << left << "\tpolarity: " << left << tke.polarity[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tE method: " << left << tke.eMethod[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tT method: " << left << tke.tMethod[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tZero interpolation: " << left << tke.interp[i_bnum][0] << "\n";
  cout << setw(32) << left << "\t# interpolation pts: " << left << tke.nZero[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tN samples: " << left << tke.Ns[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tthreshold: " << left << tke.thresh[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tcfd delay: " << left << tke.delay[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tcfd fraction: " << left << tke.frac[i_bnum][0] << "\n";
  cout << setw(32) << left << "\ttau: " << left << tke.tau[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tfast filter rise time: " << left << tke.ffRise[i_bnum][0] << "\n";
  cout << setw(32) << left << "\ttrap filter rise time: " << left << tke.tRise[i_bnum][0] << "\n";
  cout << setw(32) << left << "\ttrap filter gap time: " << left << tke.tGap[i_bnum][0] << "\n";
  cout << setw(32) << left << "\ttrap filter peak time: " << left << tke.tPeak[i_bnum][0] << "\n";
  cout << setw(32) << left << "\ttrap filter N peak: " << left << tke.nPeak[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tbaseline offset: " << left << tke.bLineOS[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tbaseline N pts: " << left << tke.bLineNpts[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tpeak npts: " << left << tke.nPeak[i_bnum][0] << "\n";
  cout << setw(32) << left << "\tpeak offset: " << left << tke.tOffset[i_bnum][8] << "\n";
  // cout << setw(32) << left << "\tcfd delay: " << left << tke.delay << "\n";
  // cout << setw(32) << left << "\tcfd fraction: " << left << tke.frac << "\n";
  // cout << setw(32) << left << "Sum: " << "\n";
  // cout << setw(32) << left << "\tmethod: " << left << sum.method << "\n";
  // cout << setw(32) << left << "\tN samples: " << left << sum.Ns << "\n";
  // cout << setw(32) << left << "\tthreshold: " << left << sum.thresh << "\n";
  // cout << setw(32) << left << "\tbaseline offset: " << left << sum.bLineOS << "\n";
  // cout << setw(32) << left << "\tbaseline N pts: " << left << sum.bLineNpts << "\n";
  // cout << setw(32) << left << "\tpeak npts: " << left << sum.nPeak << "\n";
  // cout << setw(32) << left << "\tpeak offset: " << left << sum.tOffset << "\n";
  // cout << setw(32) << left << "\tpeak height guess: " << left << sum.ph << "\n";
  // cout << setw(32) << left << "\tdecay constant: " << left << sum.tau << "\n";
  // cout << setw(32) << left << "\tfast filter rise time: " << left << sum.ffRise << "\n";
  // cout << setw(32) << left << "\ttrapezoid rise time: " << left << sum.tRise << "\n";
  // cout << setw(32) << left << "\ttrapezoid gap time: " << left << sum.tGap << "\n";
  // cout << setw(32) << left << "\tcfd delay: " << left << sum.delay << "\n";
  // cout << setw(32) << left << "\tcfd fraction: " << left << sum.frac << "\n";

  cout << "Opening input file " + (TString)ipath + "\n";
  // Get file / tree paths set
  TFile* ifile = new TFile(ipath);
  TTree* itree = (TTree*)ifile->Get("t");
  TFile* ofile = new TFile(opath, "recreate");
  // TTree *otree = new TTree("pt","Pulse tree from waveform analysis");
  TTree* otree[MAXNBOARDS][MAXNCHANNELS]; // = new TTree("pt","Pulse tree from waveform analysis");

  cout << "Files opened: " << ifile << " " << ofile << endl;

  // set up tree representing classes
  CAEN_DGTZ_Event raw_evt;
  raw_evt.reset();
  ProcessedRootEvent proc_evt;
  proc_evt.reset();


  // create branches
  itree->SetBranchAddress("bnum", &raw_evt.bnum);
  itree->SetBranchAddress("chnum", &raw_evt.chnum);
  itree->SetBranchAddress("ts", &raw_evt.ts);
  // sprintf(buf, "wf[%i]", WF_SIZE);
  itree->SetBranchAddress("wf", raw_evt.wf);



  // cout << "oh wow i made it to the output loop\n";
  cout << "Creating output tree...\n";
  for (int i = 0; i < MAXNBOARDS; ++i) {
    for (int j = 0; j < MAXNCHANNELS; ++j) {
      sprintf(buf, "pt%i_%i", i, j);
      otree[i][j] = new TTree(buf, "Pulse tree from waveform analysis");
      otree[i][j]->Branch("bnum", &proc_evt.bnum);
      otree[i][j]->Branch("chnum", &proc_evt.chnum);
      otree[i][j]->Branch("ts", &proc_evt.ts);
      otree[i][j]->Branch("tTrig", &proc_evt.tTrig);
      otree[i][j]->Branch("tPeak", &proc_evt.tPeak);
      otree[i][j]->Branch("peak", &proc_evt.peak);
      otree[i][j]->Branch("baseline", &proc_evt.baseline);
#ifdef SAVE_WAVEFORMS
      // sprintf(buf, "wf[%i]", WF_SIZE);
      otree[i][j]->Branch("wf", proc_evt.wf);
      // sprintf(buf, "Eflt[%i]", WF_SIZE);
      otree[i][j]->Branch("Eflt", proc_evt.Eflt);
      // sprintf(buf, "Tflt[%i]", WF_SIZE);
      otree[i][j]->Branch("Tflt", proc_evt.Tflt);
#endif
    }
  }
  cout << "...great success!\n";

//   cout << tke.tMethod[0][0] << " " << tke.tMethod[0][1]
// << " " << tke.tMethod[0][2] << " " << tke.tMethod[0][3] << endl;
//
// cout << tke.interp[0][0] << " " << tke.interp[0][1]
// << " " << tke.interp[0][2] << " " << tke.interp[0][3] << endl;

  // logistical stuff to sort out clock rollover
  ULong64_t    last_ts[MAXNBOARDS][MAXNCHANNELS];
  ULong64_t ts_base[MAXNBOARDS][MAXNCHANNELS];
  for (int i = 0; i < MAXNBOARDS; ++i) {
    for (int j = 0; j < MAXNCHANNELS; ++j) {
      last_ts[i][j] = 0;
      ts_base[i][j] = 0;
    }
  }

  // loop over the input tree
  cout << "Processing raw entries \n";
  Long64_t eye;

  Long64_t nentries = itree->GetEntries();
  cout << nentries << " entries total\n";
  // nentries = 100000;

  for (eye = 0; eye < nentries; eye++)
  {
    // cout << "on loop " << eye << endl;
    // itree->Show(eye);
    raw_evt.reset();
    proc_evt.reset();
    itree->GetEntry(eye);
    // cout << "read entry " << eye << endl;
    // cout << raw_evt.bnum << " " << raw_evt.chnum << endl;
    // check timestamps for rollover
    if (raw_evt.ts < last_ts[raw_evt.bnum][raw_evt.chnum]) {
      printf("hey i found rollover on board %i channel %i event // %i\n",raw_evt.bnum,raw_evt.chnum,eye);
      ts_base[raw_evt.bnum][raw_evt.chnum] =
          ts_base[raw_evt.bnum][raw_evt.chnum] + pow(2, NBITSCLOCK); ////// / STEFANO CHANGE THIS

      // std::cout << "My ts base is now " << ts_base[raw_evt.bnum][raw_evt.chnum] << std::endl;
    }
    proc_evt.ts                          = ts_base[raw_evt.bnum][raw_evt.chnum] + raw_evt.ts;
    last_ts[raw_evt.bnum][raw_evt.chnum] = raw_evt.ts;
    // build rest of tree
    proc_evt.bnum  = (int)raw_evt.bnum;
    proc_evt.chnum = (int)raw_evt.chnum;
    // if ((proc_evt.bnum == 0) && (proc_evt.chnum == 0))
    // if ((proc_evt.bnum == 0) && (proc_evt.chnum == 0 || proc_evt.chnum == 1 || proc_evt.chnum==2
    // || proc_evt.chnum==3 || proc_evt.chnum == 4 || proc_evt.chnum == 5 || proc_evt.chnum == 6 ||
    // proc_evt.chnum == 5)) if ((proc_evt.bnum == 0 && proc_evt.chnum!=7)) if ((proc_evt.bnum == 0
    // && proc_evt.chnum!=2 && proc_evt.chnum!=3))
    if (proc_evt.bnum == 0) {
      // std::cout << "board / channel numbers " << (Int_t)proc_evt.bnum << " " <<
      // (Int_t)proc_evt.chnum << std::endl;
      // mstd::cout << (int)proc_evt.chnum << std::endl;
      // cout << eye << " " << (int)raw_evt.chnum << endl;
      Int_t retval = raw_evt.processWf(tke, &proc_evt.tTrig, &proc_evt.tPeak, &proc_evt.peak,
                                       &proc_evt.baseline, raw_evt.gWF);

      // cout << "baseline " << proc_evt.baseline << endl;
#ifdef SAVE_WAVEFORMS
      for (int eye = 0; eye < WF_SIZE; eye++) {
        proc_evt.wf[eye]   = raw_evt.wf[eye];
        proc_evt.Eflt[eye] = raw_evt.Eflt[eye];
        proc_evt.Tflt[eye] = raw_evt.Tflt[eye];
      }
#endif
      otree[proc_evt.bnum][proc_evt.chnum]->Fill();
    }
    if (eye % 100000 == 0) {
      printf("\rProcessing entry %i \n", eye);
      fflush(stdout);
    }
    if (eye > EvtBreakout) {
      cout << "!!!Warning!!! - Event limit exceeded; exiting entry processing loop!\n";
      break;
    }
    // if((int)raw_evt.chnum == 0) break; // Select channel to debug
    // if(eye == 328) break; // Get pulse that fails
  }
  cout << "\ndone!\n"
       << "Processed " << eye << " entries\n";
  cout << "Writing output file...\n";

  ofile->cd();
  for (int eye = 0; eye < MAXNBOARDS; eye++) {
    for (int jay = 0; jay < MAXNCHANNELS; jay++) { otree[eye][jay]->Write(); }
  }
  raw_evt.gW->Write();
  raw_evt.gD->Write();
  raw_evt.gE->Write();
  raw_evt.gWF->Write();

  cout << "...great success!\n";
  ofile->Close();
  ifile->Close();
  return 0;
}
