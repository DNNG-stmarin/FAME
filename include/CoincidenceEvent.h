// Programmer: Stefano Marin, Isabel Hernandez
// Purpose: class intented to store a complete coincidence event with all the particle information
// Date: 02.20.2020 Ann Arbor

#ifndef CoincidenceEvent_h
#define CoincidenceEvent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "ParticleEvent.h"
#include "InfoSystem.h"

#include "ProcessingConstants.h"

class CoincidenceEvent {

private:

public:
	double beamTime;
	double beamEnergy;
	int beamChannel;
	double beamPSP;
	int beamMicroIndex;
	double beamTail;

	// attributes of the coincidence class
	int totalMultiplicity;

	// add the channel of the target
	int triggerChannel;
	double triggerEnergy;
	double triggerTime;
	bool isValidFission;
	double triggerPSP;
	double triggerTail;

	ParticleEvent particles[MAX_MULTIPLICITY];
	ParticleEvent neutrons[MAX_MULTIPLICITY];
	ParticleEvent photons[MAX_MULTIPLICITY];



	CoincidenceEvent()
	{

		// initialize beam properties
		beamTime = 0;
		beamEnergy = 0;
		beamChannel = 0;
		beamPSP = 0;
		beamMicroIndex = 0;
		beamTail = 0;

		// initialize trigger properties
		totalMultiplicity = 0;
		triggerTime = 0;
		triggerEnergy = 0;
		triggerChannel = 0;
		triggerPSP = 0;
		triggerTail = 0;
	}

	CoincidenceEvent(double triggerTimein, double triggerEnergyin, int triggerChannelin, double triggerPSPin, double triggerTailin) {
		totalMultiplicity = 0;
		triggerTime = triggerTimein;
		triggerEnergy = triggerEnergyin;
		triggerChannel = triggerChannelin;
		triggerPSP = triggerPSPin;
		triggerTail = triggerTailin;
	}

	void AddBeam(double beamTimein, double beamEnergyin, double beamChannelin, double beamPSPin, int beamMicroIndexin)
	{
		beamTime = beamTimein;
		beamEnergy = beamEnergyin;
		beamChannel = beamChannelin;
		beamPSP = beamPSPin;
		beamMicroIndex = beamMicroIndexin;
	}

	// default destructor
	~CoincidenceEvent(){
	}

	// get the trigger time
	double getTriggerTime()
	{
		return triggerTime;
	}

	// get the trigger time
	double getEnergy()
	{
		return triggerEnergy;
	}

	int getTriggerChan()
	{
		return triggerChannel;
	}

	double getTriggerPSP()
	{
		return triggerPSP;
	}

	double getTriggerTail()
	{
		return triggerTail;
	}

	double getBeamTime()
	{
		return beamTime;
	}

	double getBeamEnergy()
	{
		return beamEnergy;
	}

	double getBeamPSP()
	{
		return beamPSP;
	}

	int getBeamChan()
	{
		return beamChannel;
	}

	int getBeamIndex()
	{
		return beamMicroIndex;
	}

	double getBeamTail()
	{
		return beamTail;
	}

};

#endif // #ifdef FissionEvent.cxx
