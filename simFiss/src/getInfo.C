
#include "SFAME.h"
#include "Constants.h"
#include "DetectorClass.h"
#include "infoSystem.h"

void sfame::getInfo()
{

  // main output
  OUTPUT_NAME =  info->OUTPUT_NAME;

  // read from collision files
  // nameColFile = "cn";
  FIRST_FILE =  info->FIRST_FILE;
  NUM_FILES =  info->NUM_FILES;

  //
  NUM_DETECTORS =  info->NUM_DETECTORS;
  TRIGGER_CHANNEL =  info->TRIGGER_CHANNEL;
  SOURCE_POSITION =  info->SOURCE_POSITION;

  ACCEPT_PSP =  info->ACCEPT_PSP;
  ACCEPT_TRIG =  info->ACCEPT_TRIG; // V us
  ACCEPT_TYPE =  info->ACCEPT_TYPE; // Spontaneous fission (?)
    
  TIME_RES_TRIGGER = info->TIME_RES_TRIGGER; // in ns

  THRESHOLD =  info->THRESHOLD; // MeVee
  COINCIDENCE_WINDOW =  info->COINCIDENCE_WINDOW; // ns
  PULSE_GENERATION_WINDOW =  info->PULSE_GENERATION_WINDOW; // ns

  RESOLUTION_COEFFICIENTS =  info->RESOLUTION_COEFFICIENTS;

  // external lightoutput files
  DET_DIST_FILE =  info->DET_DIST_FILE;
  TIME_RESOLUTION_FILE =  info->TIME_RESOLUTION_FILE;
  CELL_NUMBERS_FILE =  info->CELL_NUMBERS_FILE;
  LIGHTOUTPUT_FILE =  info->LIGHTOUTPUT_FILE;

  // * could be an energy resolution file
  MEAN_NEUTRON_PSD_FILE =  info->MEAN_NEUTRON_PSD_FILE;
  SIGMA_NEUTRON_PSD_FILE =  info->SIGMA_NEUTRON_PSD_FILE;
  MEAN_PHOTON_PSD_FILE = info->MEAN_PHOTON_PSD_FILE;
  SIGMA_PHOTON_PSD_FILE = info->SIGMA_PHOTON_PSD_FILE;
}
