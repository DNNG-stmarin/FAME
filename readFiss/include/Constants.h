#ifndef Constants_h
#define Constants_h

// Fixed size dimensions of array or collections stored in the TTree if any.
const int MAX_MULT = 20;
const int MAX_TRIGGERS = 1000;

const int STANDARD_MODE = 0;
const int SIM_MODE = 1;
const int BEAM_MODE = 2;
const int FRAG_MODE = 3;
const int FRAG_BEAM_MODE = 10;

// Set physical constants
const double BEAM_PATH = 2150;//cm
const double N_MASS = 939.565; // MeV
const double LIGHT_C = 29.98; // cm/ns

// types of cut particles
const int ACCEPTED_SIGNAL = 0;
const int CUT_BEAM = 1;
const int CUT_DEP = 2;
const int CUT_PILEUP = 3;
const int CUT_BEAM_INDEX = 4;

// covEM chunks
const int NUM_CHUNKS = 10;

// Beam index acceptances
const int MIN_BEAM_INDEX = 10;
const int MAX_BEAM_INDEX = 340;

const double ANGLE_RESOLUTION = 0.05;

// Frag angles
const double MIN_THETA = -1;
const double MAX_THETA = 1;

const double MIN_PHI = -1*TMath::Pi();
const double MAX_PHI = 1*TMath::Pi();

#endif
