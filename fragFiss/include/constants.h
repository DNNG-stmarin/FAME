#ifndef constants_h
#define constants_h

const bool WAVE_MODE = false;

// neutron emission
const bool TKE_SAWTOOTH = false;
const bool INTERP_SLOPE_SAWTOOTH = true;

// backing attenuation
const bool MASS_DEP_ATT = false;
const bool MASS_DEP_ATT_SURF = false;
const bool MIXED_DEP = false;
const bool RECURSIVE_DEP = false;


const bool DOUBLE_ATT_LINE = false;
const bool QUADRATIC_ATT = false;

const double MIN_MASS_ANALYSIS = 70;
const double MAX_MASS_ANALYSIS = 180;
const double MIN_ANG_ANALYSIS = 0.3;
const double MAX_ANG_ANALYSIS = 1.2;
const double MIN_ANG_CALIB = 0.85;
const double MAX_ANG_CALIB = 0.95;

const double RECURSION_FRACTION_1 = 0.5;
const double RECURSION_FRACTION_2 = 0.5;

// operational
const double GRID_INEFFICIENCY = 0.0321;

// energy acceptance region
const double MIN_ANODE1 = 100;
const double MIN_ANODE2 = 100;

// angle acceptance region
const double MIN_ANG1 = 0.3;
const double MIN_ANG2 = 0.3;
const double MAX_ANG1 = 1.9;
const double MAX_ANG2 = 1.9;
const int N_BINS_ANGLE = 100;
const double MIN_ANG_EDGE = 0.9;

// angle finding parameters
const int N_BINS_RATIO = 100;
const double MIN_RATIO = 0.05;
const double MAX_RATIO = 0.20;
const double ALL_RATIO = 0.25;
const int MIN_ENTRIES_PROJ = 400;


const int N_BINS_APH = 160;
const int MAX_APH = 800;

const int MIN_MASS = 50;
const int MAX_MASS = 200;
const int N_BINS_MASS = 150;
const int N_BINS_MASS_TH = 50;

// const int MIN_MASS_ATT = 70;
// const int MAX_MASS_ATT =


// breakpoint guesses for angle reconstruction
const double MIN_BR_1 = 150;
const double MAX_BR_1 = 700;
const double MIN_BR_2 = 150;
const double MAX_BR_2 = 700;
const double MID_BR_1 = 400;
const double MID_BR_2 = 400;
const double BREAK_POINT_RATIO = 0.015;

const double MIN_MASS_BR = 60;
const double MAX_MASS_BR = 170;
const double BREAK_POINT_MASS = 0.2;

const double MIN_ANG_POST = 0.8;
const double MAX_ANG_POST = 1.5;

// fitting parameter for attenuation analysis
const double MIN_INV_ANG_FIT = 1.1;
const double MAX_INV_ANG_FIT = 2.0;

const double MIN_INV_ANG_FIT_TIP = 1.2;
const double MAX_INV_ANG_FIT_TIP = 2.0;


const double MAX_KE = 150;

// for fitting anode pulses
const double SIG_ANODE = 40;

// phd for calibration
const double PHD_L = 4.122; // +/- 0.05 MeV
const double PHD_H = 4.313; // +/- 0.03 MeV


// converging parameter
const int A_TOT = 252;
const double NU_BAR = 3.76;
const int MAX_CONV_ITER = 252;
const double CONVERGEANCE_CONST = 0.002;


// wave stuff
const int RECORD_LENGTH = 1280;
const int NUM_WAVES = 100;

const int TIME_SAMPLE_CONVERSION = 16.0;

const int DIGITIZER_BITS = pow(2, 12);


const int MIN_COUNT_MASS_ATT = 1000;




#endif
