// Programmer: Stefano Marin, Isabel Hernandez
// Purpose: class intented to store the information related to a single detected particle
// Date: 02.20.2020

#ifndef TriggerEvent_h
#define TriggerEvent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "GeneralEvent.h"


class TriggerEvent : public GeneralEvent
{

private:

public:
	TriggerEvent()
				: GeneralEvent()
				{
				}

	TriggerEvent(int detNumber, double timestamp, double energy, double energyShort)
				: GeneralEvent(detNumber, timestamp, energy, energyShort)
				{
				}
};

#endif
