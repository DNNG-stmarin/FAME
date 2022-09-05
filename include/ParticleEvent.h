// Programmer: Stefano Marin, Isabel Hernandez
// Purpose: class intented to store the information related to a single detected particle
// Date: 02.20.2020

#ifndef ParticleEvent_h
#define ParticleEvent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
//#include "InfoSystem.h"
#include "InfoSystem.h"

#include "GeneralEvent.h"


class ParticleEvent : public GeneralEvent
{

private:

public:
	ParticleEvent()
				: GeneralEvent()
				{
				}

	ParticleEvent(int detNumber, double timestamp, double energy, double energyShort)
				: GeneralEvent(detNumber, timestamp, energy, energyShort)
				{
				}
};

#endif
