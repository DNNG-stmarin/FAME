////////////////////////////////////////////////////////////////////////////////
/// \file   definitions.h
/// \author Devin S. Connolly
/// \email  dconnolly@lanl.gov
/// \date   May 2020
///
/// Copyright (C) 2020 Devin S. Connolly
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_ 1

// definitions
#define WF_SIZE 240
// #define WF_SIZE 4096 // Jan 2018 data
#define MAXNBOARDS 1
#define MAXNCHANNELS 8
#define NBITSCLOCK 32
#define NBITSADC 12

// pre-processor flags
// #define SAVE_WAVEFORMS     // (un)comment to (keep)discard waveforms through all analysis stages (requires recompilation of all executables)
// #define WRITE_FILTERS     // (un)comment to (keep)discard sum waveform filters
#define GAMMAS_OFF     // (un)comment to (discard)use gamma detectors
// #define SMOOTHING // smooth anode waveforms

#endif // _DEFINITIONS_H_
