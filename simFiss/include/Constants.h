//
//
#include <TF1.h>

#ifndef Constants_h
#define Constants_h

const int MAX_MULTIPLICITY = 100;

// Fixed size dimensions of array or collections stored in the TTree if any.
const int MAX_LINES = 1000; // put a large number representing the maximum number of interactions a single history can generate (i.e. the number of lines all having the same history)
const int NUM_ROWS = 16;
const int MAX_TRACKS = 200;

// particle types
const int NEUTRONS = 1;
const int PHOTONS = 2;
const int BADNEUTRON = 11;
const int BADPHOTON = 12;
const int CUTNEUTRON = 21;
const int CUTPHOTON = 22;

// fission types (ARE THESE CONSISTENT W PREVIOUS?)
const int SPONTANEOUS = 0;
const int INDUCED_N = 1;
const int INDUCED_ION = 2;

// zaid values
const int HYDROGEN = 1001;
const int CARBON = 6000;

// light output constants
const double MIN_DEP_ENERGY = 0.001;
const double MAX_DEP_ENERGY = 8.00;

const double HISTORY_SPACING = 1000;

// PSD parameters
const int NUM_PSD_PARAMS = 4;
const TF1 expLin = TF1("expLin", "[0]*e^(-[1]*x) + [2]*x + [3]", MIN_DEP_ENERGY, MAX_DEP_ENERGY);

// physical constants
const double C = 29.9792; // speed of light in cm/ns
const double MASS_NEUTRON = 939.565;   // mass of the neutron in MeV/c^2

const double N_PER_FISSION = 3.757;
const double P_PER_FISSION = 8.29;

#endif
