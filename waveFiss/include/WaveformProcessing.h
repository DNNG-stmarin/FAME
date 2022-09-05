////////////////////////////////////////////////////////////////////////////////
///  \file   WaveformProcessing.h
///  \author Devin S. Connolly
///  \email  dconnolly@lanl.gov
///  \author Shea M. Mosby
///  \email  smosby@lanl.gov
///  \brief  classes to process waveforms recoreded by CAEN digitizers
////////////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <cstdlib>
#include <cmath>

// ROOT includes
#include "Rtypes.h"
#include "TH1.h"
#include "TF1.h"
#include "TGraph.h"

// File includes
#include "definitions.h"

using namespace std;

class TKESettings
{
public:
  Int_t sampleDelta[MAXNBOARDS]
                   [MAXNCHANNELS]; // how far away to sample for each point of derivative
  Int_t derivePointDelta[MAXNBOARDS]
                        [MAXNCHANNELS]; // how far away to move to calculate derivative samples
  string   polarity[MAXNBOARDS][MAXNCHANNELS]; // polarity of input signal
  string   tMethod[MAXNBOARDS][MAXNCHANNELS];  // method to use to evaluate timing
  string   interp[MAXNBOARDS][MAXNCHANNELS];   // method to use to interpolate zero crossing
  string   eMethod[MAXNBOARDS][MAXNCHANNELS];  // method to use to evaluate peak height
  Int_t    Ns[MAXNBOARDS][MAXNCHANNELS];       // number of samples in waveform
  Int_t    ffRise[MAXNBOARDS][MAXNCHANNELS];
  Int_t    tRise[MAXNBOARDS][MAXNCHANNELS];
  Int_t    tGap[MAXNBOARDS][MAXNCHANNELS];
  Int_t    tPeak[MAXNBOARDS][MAXNCHANNELS];
  Int_t    nPeak[MAXNBOARDS][MAXNCHANNELS];     // number of samples used to calculate peak height
  Int_t    nZero[MAXNBOARDS][MAXNCHANNELS];     // number of samples used to interpolate zero Xing
  Int_t    delay[MAXNBOARDS][MAXNCHANNELS];     // delay for CFD
  Int_t    bLineOS[MAXNBOARDS][MAXNCHANNELS];   // # of samples to offset baseline
  Int_t    bLineNpts[MAXNBOARDS][MAXNCHANNELS]; // # samples used to calc baseline
  Int_t    tOffset[MAXNBOARDS][MAXNCHANNELS];   // # of samples preceding trigger
  Int_t    eOffset[MAXNBOARDS][MAXNCHANNELS]; // # of samples after trig time to begin ph extraction
  Double_t phGuess[MAXNBOARDS][MAXNCHANNELS]; // initial peak height for exp fit
  Double_t tau[MAXNBOARDS][MAXNCHANNELS];     // CSP decay constant
  Double_t frac[MAXNBOARDS][MAXNCHANNELS];    // frac for CFD
  Double_t thresh[MAXNBOARDS][MAXNCHANNELS];  // zero crossing threshold (for double derivative)

  Int_t    gOffset[MAXNBOARDS][MAXNCHANNELS]; // number of samples after grid min to begin averaging signal
public:
  void reset();
};

class CAEN_DGTZ_Event
{
public:
  UChar_t  bnum;
  UChar_t  chnum;
  ULong64_t   ts;            // 64 bit unsigned long
  Short_t  wf[WF_SIZE];   // raw waveform
  Double_t Eflt[WF_SIZE]; // filtered waveform 0 - mostly 1st derivative
  Double_t Tflt[WF_SIZE]; // filtered waveform 1 - mostly 2nd derivative
  TGraph*  gW = NULL;
  TGraph*  gD = NULL;
  TGraph*  gE = NULL;
  TGraph*  gWF = NULL;


public:
  Double_t calcBaseline(Int_t Ns, Int_t offset, Short_t* wf);
  Double_t calcPhChargeInt(Int_t Ns, Int_t offset, Short_t* wf); // charge integration
  Double_t calcPhGrid(Int_t Ns, Int_t offset, Int_t Npts, Short_t* wf);
  Double_t calcPhExp(Int_t Ns, Int_t Npts, Int_t offset, Double_t baseline, Short_t* wf,
                     Double_t ph_guess, Double_t tau); // exponential fitting
  void     CFD(Int_t Ns, Short_t* wf, Double_t* CFD, Int_t delay, Double_t frac);
  void     doubleDerivativeFilter(Int_t Ns, Short_t* wf, Double_t* outarray, Int_t sampleDelta,
                                  Int_t derivePointDelta, TGraph* gW, TGraph* gD);
  void     derivativeFilter(Int_t Ns, Short_t* wf, Double_t* outarray, Int_t sampleDelta,
                            Int_t derivePointDelta);
  void     fastFilter(Int_t Ns, Short_t* wf, Double_t* ff, Int_t T_r, Int_t T_clk, Double_t tau);
  void     ffCFD(Int_t Ns, Short_t* wf, Double_t* CFD, Int_t T_r, Int_t T_clk, Double_t tau,
                 Int_t delay, Double_t frac);
  Int_t    ffExtrapTs(Int_t Ns, Double_t* ff, Int_t offset, Int_t T_r, Double_t thresh);
  Double_t getPhTrapFilter(Int_t Ns, Double_t* tf, Int_t offset, Int_t tRise, Int_t tPeak,
                           Int_t nPeak); // trapezoidal filter
  Double_t cubicInterp0X(Int_t Ns, Double_t* wf, Double_t armthresh, Int_t offset, Int_t Npts);
  Double_t linearInterp0X(Int_t Ns, Double_t* wf, Double_t armthresh, Int_t offset, Int_t Npts);
  Double_t locate0X(Int_t Ns, Double_t* wf, Double_t armthresh, Int_t offset);
  Double_t locatePeak(Int_t Ns, Double_t* wf);
  Double_t cSplineInterp0X(Int_t Ns, Double_t* wf, Double_t armthresh = 20, Int_t Npts = 4,
                           Int_t offset = 0, Double_t tolerance = 1.e-4);
  Int_t    processWf(TKESettings tvar, double* tTrig, int* tPeak, double* peak, double* baseline, TGraph* gWF);
  void     reset();
  void     sdlFilter(Int_t Ns, Short_t* wf, Double_t* sdl, Int_t delay, Double_t tau);
  void     sdlCFD(Int_t Ns, Short_t* wf, Double_t* CFD, Int_t ff_rise = 64, Double_t tau = 2.5e4,
                  Int_t delay = 32, Double_t frac = 0.95);
  void     trapFilter(Int_t Ns, Short_t* wf, Double_t* tf, Int_t offset, Int_t T_r, Int_t T_g,
                      Int_t T_clk, Double_t tau);
};

class ProcessedRootEvent
{
public:
  int   bnum;
  int   chnum;
  ULong64_t ts;
  Double_t  tTrig;
  Int_t     tPeak;
  Double_t  peak;
  Double_t  baseline;
#ifdef SAVE_WAVEFORMS
  Short_t  wf[WF_SIZE];   // raw waveform
  Double_t Eflt[WF_SIZE]; // filtered waveform 0 - mostly 1st derivative
  Double_t Tflt[WF_SIZE]; // filtered waveform 1 - mostly 2nd derivative
#endif
public:
  void reset();
};

class GammaSingles
{
public:
  UChar_t  bnum;
  UChar_t  chnum;
  Double_t t0t; // t0
  Double_t dt;  // uncalibrated time of flight
  Double_t ge;
  Double_t baseline;

public:
  void reset();
};

class EventInfo
{
public:
  Double_t t0t;      // t0
  Double_t ct;       // cathode time
  Double_t dt;       // uncalibrated time of flight
  Double_t cph;      // cathode pulse height
  Double_t tTrig[8]; // trigger times
  Double_t aph[2];   // anode pulse height
  Double_t gph[2];   // grid pulse height
  Double_t sumph[2];
  Double_t sumtTrig[2];
  Int_t    sumtPeak[2];
  Double_t sumBl[2];
  bool     ccoinc; // have all charged particle channels fired
#ifdef SAVE_WAVEFORMS
  Short_t a0wf[WF_SIZE];   // raw waveform
  Short_t a1wf[WF_SIZE];   // raw waveform
  Short_t cwf[WF_SIZE];    // raw waveform
  Short_t g0wf[WF_SIZE];   // raw waveform
  Short_t g1wf[WF_SIZE];   // raw waveform
  Short_t sum0wf[WF_SIZE]; // sum of anode0 + grid0 waveforms
  Short_t sum1wf[WF_SIZE]; // sum of anode1 + grid1 waveforms
#ifdef WRITE_FILTERS
  Double_t sum0Eflt[WF_SIZE]; // energy filter of sum0 wf
  Double_t sum1Eflt[WF_SIZE]; // energy filter of sum1 wf
  Double_t sum0Tflt[WF_SIZE]; // timing filter of sum0 wf
  Double_t sum1Tflt[WF_SIZE]; // timing filter of sum1 wf
#endif
#endif
#ifndef GAMMAS_OFF
  Double_t gt[2];    // uncalibrated time of flight for gamma detectors
  Double_t ge[2];    // "pulse height" for gamma detectors
  Double_t gtl[10];  // extra big list for wide coinc gates - time
  Double_t gel[10];  // extra big list for wide coinc gates - energy
  Double_t gchl[10]; // extra big list for wide coinc gates - channel
  int      gmult;    // coincidence level for gamma detectors
#endif
public:
  void reset(); //#
};
