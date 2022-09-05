#ifndef Parameters_h
#define Parameters_h

const int numTofBins = 240;
const double minTof = -20;
const double maxTof = 100;

const int numLObins = 250;
const double minLO = 0;
const double maxLO = 5;

const int numErgBins = 100;
const double minErg = 0;
const double maxErg = 10;

const int numPSDBins = 100;
const double minPSP = 0;
const double maxPSP = 1;

const int minMult = 0;
const int maxMult = 10;

const int numCosBins = 100;

const int numfisDepBins = 500; //JJ import these numbers from the beamInfo.root file
const double minDep = 0;
const double maxDep = 0.05;

const int numfisBeamTimeBins= 2000;
const double minBeamTime = -200;
const double maxBeamTime = 1800;

const int numDeltaT = 200;
const double minDeltaT = 0;
const double maxDeltaT = 10000;

const int numTKEbins = 50;
const double minTKE = 140;
const double maxTKE = 240;

// cov EM parameters
// const double sizeTheta = (MAX_THETA - MIN_THETA)/BTH;
// const double sizePhi = (MAX_PHI - MIN_PHI)/BPH;

const int MAX_MULT_DET = 2; // counting 0

const int NUM_DIFF_N = 2; // detN, BN (no degeneracy)
const int NUM_DIFF_P = 2; // detP, BP (no degeneracy)

const double FORWARD_FRAG = 0.8;
const double SEP_NEUT_FRAG = 0.9;
const double MIN_NEUT_ECOM = 0.5;

#endif
