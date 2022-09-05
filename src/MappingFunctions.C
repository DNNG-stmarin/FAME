// Programmer: Stefano Marin, Isabel Hernandez
// Purpose: functions to map channel numbers to detectors and identify the type of signal
// Date: 03.20.2020

#define mappingFunctions_h

#include "InfoSystem.h"
#include "mappingFunctions.h"

// returns the index of the detector or -1 if not a detector
int isDetector(int detectorNumber, int NUM_DETS, int* DETECTORS)
{
	int detIndex = -1;

	for(int index = 0; index < NUM_DETS; index++)
	{
		if(detectorNumber == DETECTORS[index])
		{
			detIndex = index;
			break;
		}
	}
	return detIndex;
}


int isBeam(int detectorNumber, int NUM_BEAMS, int* BEAM)
{
	int beamIndex = -1;
	for(int index = 0; index < NUM_BEAMS; index++)
	{
		if(detectorNumber == BEAM[index])
		{
			beamIndex = index;
			break;
		}
	}
	return beamIndex;
}

int isTrigger(int detectorNumber, int NUM_TRIGGERS, int* FISSION_TRIGGERS)
{
	int chamberIndex = -1;
	for(int index = 0; index < NUM_TRIGGERS; index++)
	{
		if(detectorNumber == FISSION_TRIGGERS[index])
		{
			chamberIndex = index;
			break;
		}
	}
	return chamberIndex;
}

// returns the index of the detector or -1 if not a detector
int isExcluded(int detectorNumber, int NUM_EXCLUDED, int* EXCLUDE_DETECTORS)
{
	int detIndex = -1;

	for(int index = 0; index < NUM_EXCLUDED; index++)
	{
		if(detectorNumber == EXCLUDE_DETECTORS[index])
		{
			detIndex = index;
			break;
		}
	}
	return detIndex;
}
