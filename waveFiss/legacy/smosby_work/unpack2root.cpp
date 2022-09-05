// C/C++ includes
#include <cstdio>

// ROOT includes
#include "TFile.h"
#include "TTree.h"

// File includes
#include "CFile.h"
#include "CTrack.h"

void decimate(CTrack* inarray, float* outarray, int decimation, int outlength)
{
  for (int i = 0; i < outlength; ++i) {
    float temp = 0.;
    for (int j = 0; j < decimation; ++j) { temp += inarray->GetBinContent(decimation * i + j); }
    outarray[i] = temp;
  }
};

int main(int argc, char* argv[])
{
  // set up logistical variables
  char      ifname[1000]; // path to input file
  char      ofname[1000]; // path to output file
  CFile*    mCFile;       // raw data file
  CTrack*** cCTracks;     // waveform information
  Int_t     nevt;         // number of events
  Int_t     ntracks;      // number of "tracks"
  TFile*    ofile;        // output file
  TTree*    otree;        // output tree

  // check that we have right number of arguments
  // argc=3 is correct - name of program + file to read in + file to write out
  if (argc != 3) {
    printf("--> EXIT: Please start unpack2root as follows:\n");
    printf("--> EXIT: unpack2root string=PATH_TO_DATA string=PATH_TO_OUTPUT\n");
    return -1;
  }
  // we've been given the information we need, so let's get to work
  // set variables to input/output file names
  sprintf(ifname, "%s", argv[1]);
  sprintf(ofname, "%s", argv[2]);
  // Read the input file
  mCFile =
      new CFile("spectrum",
                ifname); // note that this assumes a particular kind of cfile - .dat file extension

  // get information out of the file
  cCTracks = mCFile->GetTracks(); // 2d array - [event][channel] where index is t0,c,a1,g1,a2,g2
  ntracks  = mCFile->GetNrTracks();
  nevt     = mCFile->GetNrEvents();

  // figure out how big my arrays are
  int        decimation       = 5;
  static int nsamples_cathode = cCTracks[0][0]->GetNbinsX() / decimation;
  static int nsamples_anode   = cCTracks[0][2]->GetNbinsX() / decimation;

  // make arrays to hold waveform information
  // NOTE: we're using floats because the provided unpacker uses a class derived
  // from TH1F - this is a wasteful way of doing business but we're stuck with it
  // unless we rewrite the unpacker
  float t0[nsamples_cathode];
  float c[nsamples_cathode];
  float a1[nsamples_anode];
  float g1[nsamples_anode];
  float a2[nsamples_anode];
  float g2[nsamples_anode];

  // Open output file, set up root tree for raw waveforms
  ofile = new TFile(ofname, "recreate");
  otree = new TTree("wt", "Tree of Waveforms");
  // make the actual branches
  otree->Branch(TString::Format("t0[%i]", nsamples_cathode), t0);
  otree->Branch(TString::Format("c[%i]", nsamples_cathode), c);
  otree->Branch(TString::Format("a1[%i]", nsamples_anode), a1);
  otree->Branch(TString::Format("g1[%i]", nsamples_anode), g1);
  otree->Branch(TString::Format("a2[%i]", nsamples_anode), a2);
  otree->Branch(TString::Format("g2[%i]", nsamples_anode), g2);

  // Loop through all events to write waveforms out
  for (int i = 0; i < nevt; ++i) {
    // for each event, loop through waveforms to copy information into tree
    /*
    for (int j=0;j<nsamples_cathode;++j)
    {
        t0[j]=cCTracks[i][0]->GetBinContent(j);
        c[j]=cCTracks[i][1]->GetBinContent(j);
    }
    */
    decimate(cCTracks[i][0], t0, decimation, nsamples_cathode);
    decimate(cCTracks[i][1], c, decimation, nsamples_cathode);

    decimate(cCTracks[i][2], a1, decimation, nsamples_anode);
    decimate(cCTracks[i][3], g1, decimation, nsamples_anode);
    decimate(cCTracks[i][4], a2, decimation, nsamples_anode);
    decimate(cCTracks[i][5], g2, decimation, nsamples_anode);
    /*
    for (int j=0;j<nsamples_anode;++j)
    {
        a1[j]=cCTracks[i][2]->GetBinContent(j);
        g1[j]=cCTracks[i][3]->GetBinContent(j);
        a2[j]=cCTracks[i][4]->GetBinContent(j);
        g2[j]=cCTracks[i][5]->GetBinContent(j);
    }
    */
    // And we fill the tree since we're just copying waveforms
    otree->Fill();
  }
  // Write the tree and close things out
  otree->Write();
  ofile->Close();

  return 0;
}
