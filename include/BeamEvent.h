// Programmer: Stefano Marin, Isabel Hernandez
// Purpose: class intented to store the information related to a single detected particle
// Date: 02.20.2020

#ifndef BeamEvent_h
#define BeamEvent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "GeneralEvent.h"


class BeamEvent : public GeneralEvent
{

private:

public:

	int microIndex = 0;

	BeamEvent()
				: GeneralEvent()
				{
				}

	BeamEvent(int detNumber, double timestamp, double energy, double energyShort)
				: GeneralEvent(detNumber, timestamp, energy, energyShort)
				{
				}

	int getMicroIndex()
	{
		return microIndex;
	}
};

#endif
