/*
Author: Stefano Marin
Purpose: generalized storer for events in the digitizer. Some examples
         are particles, triggers, and accelerator.
Date:  May 12th, 2020, Ann Arbor.
*/

#ifndef GeneralEvent_h
#define GeneralEvent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
//#include "InfoSystem.h"
#include "InfoSystem.h"
#include "ParticleEvent.h"
#include "ProcessingConstants.h"


class GeneralEvent
{
private:

public:

	double depositedEnergy;
	double time;
	int detChannel;
	double psp;
	int type;
  double tailEnergy;

	// trigger default constructor
	GeneralEvent()
	{
		depositedEnergy = 0;
		time = 0;
		psp = 0;
		detChannel = 0;
		type = 0;
	}

	// trigger value constructor
	GeneralEvent(int detNumber, double timestamp, double energy, double energyShort)
	{
		depositedEnergy = energy * CHAN_VUS; //now in vms
		time = timestamp;
		detChannel = detNumber;
		psp = energyShort/energy;
		type = NO_PARTICLE;
	}

	// the next few functions are used to get the properties of the particle
	double getEnergy()
	{
		return depositedEnergy;
	}

	double getTime()
	{
		return time;
	}

	int getDetector()
	{
		return detChannel;
	}

	double getPsp()
	{
		return psp;
	}

	int getType()
	{
		return type;
	}

  int getTail()
	{
		return tailEnergy;
	}

};

#endif
