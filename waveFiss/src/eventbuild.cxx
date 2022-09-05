////////////////////////////////////////////////////////////////////////////////
///  Shea M. Mosby
///  smosby@lanl.gov
///  Devin S. Connolly
///  dconnolly@lanl.gov
///  eventbuild.cxx
////////////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

// ROOT includes
#include "TTree.h"
#include "TLeaf.h"
#include "TFile.h"

// File includes
#include "WaveformProcessing.h"

int main(int argc, char* argv[])
{
  // Creation of variables section
  char   ipath[512];
  char   opath[512];
  char   buf[512];
  string cfgfile;


  printf("\n");
  printf(" _______________________________________________________\n");
  printf("|                                                       |\n");
  printf("|                 CAEN Event Building                   |\n");
  printf("|-------------------------------------------------------|\n");
  printf("|          Analyze DPP-enabled CAEN digital ouput       |\n");
  printf("|                                                       |\n");
  printf("|           (send bugs to dconnolly@lanl.gov)           |\n");
  printf("|_______________________________________________________|\n");
  printf("\n");
  // Parsing of input parameters
  if (argc == 3) {
    sprintf(ipath, "%s", argv[1]);
    sprintf(opath, "%s", argv[2]);
  } else if (argc == 4) {
    sprintf(ipath, "%s", argv[1]);
    sprintf(opath, "%s", argv[2]);
    cfgfile = argv[3];

  } else {
    printf("--> EXIT:\nPlease, start event building as follows: \n");
    printf("--> EXIT: eventbuild string=PATH_TO_INPUT_FILE string=PATH_TO_OUTPUT_FILE\n");
    return -1;
  }

  // Get file / tree paths set
  TFile*             ifile       = new TFile(ipath);
  const Short_t      board_ana   = 0; // what board to be analyzing
  const Short_t      channel_ana = 8; // number of channels to analyze
  TTree*             itree[channel_ana];
  ProcessedRootEvent proc_evt[channel_ana];
  if (ifile->IsZombie()) {
    cout << "Error opening file " << ifile << "Aborting\n";
    exit(-1);
  }
  for (int eye = 0; eye < channel_ana; eye++) {
    sprintf(buf, "pt%i_%i", board_ana, eye);
    if (!(ifile->GetListOfKeys()->Contains(buf))) {
      printf("Error in file %s - does not contain singles tree pt%d_%d! Aborting\n", ipath,
             board_ana, channel_ana);
      exit(-1);
    }
  }
  TFile* ofile = new TFile(opath, "recreate");
  TTree* otree[2];
  otree[0] = new TTree("et", "event tree from waveform analysis");
  // set up tree representing classes
  for (int eye = 0; eye < channel_ana; eye++) {
    sprintf(buf, "pt%i_%i", board_ana, eye);
    itree[eye] = (TTree*)ifile->Get(buf);
    proc_evt[eye].reset();
    itree[eye]->SetBranchAddress("ts", &proc_evt[eye].ts);
    itree[eye]->SetBranchAddress("tTrig", &proc_evt[eye].tTrig);
    itree[eye]->SetBranchAddress("peak", &proc_evt[eye].peak);
    itree[eye]->SetBranchAddress("baseline", &proc_evt[eye].baseline);
  }
  EventInfo coinc_evt[2];
  // create branches
  cout << "Creating output tree\n";
  for (int eye = 0; eye < 1; ++eye) {
    coinc_evt[eye].reset();
    otree[eye]->Branch("t0t", &coinc_evt[eye].t0t); // beam time
    otree[eye]->Branch("ct", &coinc_evt[eye].ct); // cathode time
    otree[eye]->Branch("dt", &coinc_evt[eye].dt); // delta beam cathode
    otree[eye]->Branch("cph", &coinc_evt[eye].cph); // cathode pulse height
    otree[eye]->Branch("tTrig[8]", coinc_evt[eye].tTrig); // time trigger for all channels
    otree[eye]->Branch("aph[2]", coinc_evt[eye].aph); // anode pulse height
    otree[eye]->Branch("gph[2]", coinc_evt[eye].gph); // grid pulse height
    otree[eye]->Branch("sumph[2]", coinc_evt[eye].sumph); // sum pulse height
    otree[eye]->Branch("sumtTrig[2]", coinc_evt[eye].sumtTrig);
    otree[eye]->Branch("sumtPeak[2]", coinc_evt[eye].sumtPeak);
    otree[eye]->Branch("ccoinc", &coinc_evt[eye].ccoinc);
  }
  /////////////////////////////////////////////////
  // SET UP EVENT RECONSTRUCTION PARAMETERS HERE //
  /////////////////////////////////////////////////

  // General note, we're still working in dig units - sample rate is 4 ns
  // Also, we'll do the event reconstruction based on the trigger time tag
  // but tof based on everything
  // the written tof output will be in ns
  // also, remember we reconstruct relative to the macropulse right now
  // and finally, we've got 2 trees. The first one is for the TKE experiment
  // and the 2nd is for gamma rays that don't coincide with any fission event

  // event reconstruction offsets (dig units)
  Long64_t rt_offset[channel_ana];
  rt_offset[0] = 0;    // cathode
  rt_offset[1] = 0;    //-250; // anode 0
  rt_offset[2] = 0; //-250; // anode 1
  rt_offset[3] = 0; //-25;  // grid 0
  rt_offset[4] = 0; //-25;  // grid 1
  rt_offset[5] = 0; // empty
  rt_offset[6] = 0; // empty
  rt_offset[7] = 0;  // empty

  // reconstruction window (dig units, goes on either side)
  Long64_t recon_window = 4000;

  Double_t dig2ns = 1.; // Already set to ns

  // time of flight calculation offsets (ns)
  Double_t tof_offset[channel_ana];
  tof_offset[0] = 0; // gamma detector 0
  tof_offset[1] = 0; // gamma detector 1
  tof_offset[2] = 0; // t0
  tof_offset[3] = 0; // anode 0
  tof_offset[4] = 0; // anode 1
  tof_offset[5] = 0; // grid 0
  tof_offset[6] = 0; // grid 1
  tof_offset[7] = 0; // cathode

  ULong64_t current_index[channel_ana];
  for (int eye = 0; eye < channel_ana; eye++) { current_index[eye] = itree[eye]->GetEntries() - 1; }
  Int_t t0_channel      = 0;
  Int_t cathode_channel = 0;

  const Int_t n_other_ic_channels                    = 4;
  Int_t       other_ic_channels[n_other_ic_channels] = {1, 2, 3, 4};
  // Int_t other_ic_channels[n_other_ic_channels] = {3};

  ////////////////////////////////////////////////
  // END SET UP EVENT RECONSTRUCTION PARAMETERS //
  ////////////////////////////////////////////////
  // we reverse loop over everything for "simplicity"
  cout << "Building events\n";
  for (int t0i = current_index[t0_channel]; t0i > -1; --t0i)
  // for (int t0i = 0; t0i < 1000; ++t0i)
  {


    if (t0i % 100 == 0)
    {
      cout << "\r"
           << "t0 index " << t0i;
    }


    itree[t0_channel]->GetEntry(t0i);
    // if (proc_evt[t0_channel].peak>-2500.) continue;
    // if (proc_evt[t0_channel].peak > -500) continue;


    int cathode_i = current_index[cathode_channel];
    while (true) // this looks for fission events - cathode is the reference
    {
      if (cathode_i < 0) break;
      coinc_evt[0].reset();

      ////////////////////////////////////
      // CATHODE / T0 COINCIDENCE BLOCK //
      ////////////////////////////////////
      itree[cathode_channel]->GetEntry(cathode_i);
      // break out if we are less than the t0

      // if (proc_evt[cathode_channel].ts + rt_offset[cathode_channel] <
      //     proc_evt[t0_channel].ts + rt_offset[t0_channel]) {
      //   break;
      // }

      // itree[cathode_channel]->Show();
      // cout << "t0 time " << proc_evt[0].ts+rt_offset[0] << " cathode time " <<
      // proc_evt[cathode_channel].ts+rt_offset[cathode_channel] << " difference " <<  endl;
      // otherwise keep trying to reconstruct
      coinc_evt[0].tTrig[t0_channel]      = proc_evt[t0_channel].tTrig;
      coinc_evt[0].tTrig[cathode_channel] = proc_evt[cathode_channel].tTrig;
      coinc_evt[0].t0t = dig2ns * ((double)(proc_evt[t0_channel].ts + rt_offset[t0_channel]) +
                                   proc_evt[t0_channel].tTrig) +
                         tof_offset[t0_channel];
      coinc_evt[0].ct =
          dig2ns * ((double)(proc_evt[cathode_channel].ts + rt_offset[cathode_channel]) +
                    proc_evt[cathode_channel].tTrig) +
          tof_offset[cathode_channel];
      coinc_evt[0].cph = proc_evt[cathode_channel].peak;
      coinc_evt[0].dt  = coinc_evt[0].ct - coinc_evt[0].t0t;
      // coinc_evt[0].dt = proc_evt[cathode_channel].ts-proc_evt[t0_channel].ts;
      cathode_i -= 1;
      ///////////////////////
      // OTHER IC CHANNELS //
      ///////////////////////
      for (int ic_loop = 0; ic_loop < n_other_ic_channels; ++ic_loop)
      {
        int ic_channel = other_ic_channels[ic_loop];
        int ic_i       = current_index[ic_channel];

        while (true)
        {
          if (ic_i < 0) break;
          itree[ic_channel]->GetEntry(ic_i);
          // break out if we are less than low limit of coinc window
          Long64_t ic_channel_time = proc_evt[ic_channel].ts + rt_offset[ic_channel];
          Long64_t cathode_time    = proc_evt[cathode_channel].ts + rt_offset[cathode_channel];
          // cout << "channel " << ic_channel;
          if (ic_channel_time < (cathode_time - recon_window))
          {
            break;
          }
          else if (ic_channel_time > (cathode_time + recon_window)) {
            // continue on if we are greater than high limit of coinc window
            switch (ic_channel) {
            default: break;
            }
            ic_i -= 1;
            current_index[ic_channel] = ic_i;
            continue;
          }
          else {
            // if we are inside the coinc window we actually do something
            // if ((cathode_time - recon_window) < ic_channel_time < (cathode_time + recon_window))
            // {
            switch (ic_channel)
            {
              case 1:
                coinc_evt[0].aph[0]            = proc_evt[ic_channel].peak;
                coinc_evt[0].tTrig[ic_channel] = proc_evt[ic_channel].tTrig;
                break;
              case 2:
                coinc_evt[0].aph[1]            = proc_evt[ic_channel].peak;
                coinc_evt[0].tTrig[ic_channel] = proc_evt[ic_channel].tTrig;
                break;
              case 3:
                coinc_evt[0].gph[0]            = proc_evt[ic_channel].peak;
                coinc_evt[0].tTrig[ic_channel] = proc_evt[ic_channel].tTrig;
                break;
              case 4:
                coinc_evt[0].gph[1]            = proc_evt[ic_channel].peak;
                coinc_evt[0].tTrig[ic_channel] = proc_evt[ic_channel].tTrig;
                break;
              default: break;
            }
          }
          ic_i -= 1;
          current_index[ic_channel] = ic_i;
        }
      }
      if (coinc_evt[0].aph[0] > -1 && coinc_evt[0].aph[1] > -1 &&
          coinc_evt[0].gph[0] > -1 && coinc_evt[0].gph[1] > -1)
      {
        coinc_evt[0].ccoinc = 1;
      }
      else {coinc_evt[0].ccoinc = 0;}

      otree[0]->Fill();
      current_index[cathode_channel] = cathode_i;
    }
  }
  cout << "\n";
  cout << "Writing tke event tree to rootfile...\n";
  otree[0]->Write();
#ifndef GAMMAS_OFF
  cout << "Writing gamma singles event tree to rootfile...\n";
  otree[1]->Write();
#endif
  cout << "...great success!\n";
  ofile->Close();
  ifile->Close();
  return 0;
}
