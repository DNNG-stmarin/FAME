// Programmer: Stefano Marin, Isabel Hernandez
// Purpose: header file containting useful constants
// Date: 03.20.2020

#ifndef mappingFunctions_h
#define mappingFunctions_h

// returns the index of the detector or -1 if not a detector
int isDetector(int detectorNumber, int NUM_DETS, int* DETECTORS);

int isTrigger(int detectorNumber, int NUM_TRIGGERS, int* FISSION_TRIGGERS);

int isBeam(int detectorNumber, int NUM_BEAMS, int* BEAM);

int isExcluded(int detectorNumber, int NUM_EXCLUDED, int* EXCLUDE_DETECTORS);

#endif
