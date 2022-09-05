/*
Author: Stefano Marin, Isabel Hernandez
Purpose: Store the constants used in the processing
*/

#ifndef ProcessingConstants_h_
#define ProcessingConstants_h_

const double FISS_RATE = 1e6;

const int ORGANIC = 0;
const int INORGANIC = 1;

// calibration of detectors for energy
const double CHAN_VUS = (1.0/1000.0)*(4.0/1024.0); // from digitizer channel to Vus
const double CSCOMPTEDGE = 0.477;
const double MASS_NEUTRONS = 939.565; // MeV/c^2
const double LIGHT_C = 29.979; // cm/ns

// particle types definitions
const short NO_PARTICLE = 0;
const short NEUTRON = 1;
const short PHOTON = 2;
const short ALPHA = 4;

// multiplicity max to store
const int MAX_MULTIPLICITY = 100;
const int MAX_EVENTS = 100;

// specifications of PSD bins
const double MIN_PSD_HIST = 0;
const double MAX_PSD_HIST = 1;
const int NUM_PSD_HIST = 100;

const double MIN_BEND_PSD = -2;
const double MAX_BEND_PSD = 5;

// double discrimination values
const double DISC_SHIFT_VALUE = 0.005;

// specifications of LO bins
const double MIN_LO_HIST = 0;
const double MAX_LO_HIST = 4;
const int NUM_LO_HIST = 400;

const int MIN_ENTRIES = 50;

const double FAKE_FISS_SIGNAL = 0.01;





#endif
