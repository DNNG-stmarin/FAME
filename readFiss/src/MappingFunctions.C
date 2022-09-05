#include "readFiss.h"

int readFiss::isTrigger(int detectorNumber)
{
	int chamberIndex = -1;
	for(int index = 0; index < NUM_TRIGGERS; index++)
	{
		if(detectorNumber == TRIGGERS[index])
		{
			chamberIndex = index;
			break;
		}
	}
	return chamberIndex;
}

int readFiss::isDetector(int detectorNumber)
{
	int detIndex = -1;
	for(int index = 0; index < NUM_DETECTORS; index++)
	{
		if(detectorNumber == DETECTORS[index])
		{
			detIndex = index;
			break;
		}
	}
	return detIndex;
}
