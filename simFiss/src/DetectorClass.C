#include "DetectorClass.h"

DetectorClass::DetectorClass()
{
  lightOutput = NULL;
  kinLight= NULL;

  x = 0;
  y = 0;
  z = 0;

  dist = 0;

  timeRes = 0;

  energyRes = NULL;

  channel = 0;
  cell = 0;

  meanNeutPSD = NULL;
  meanPhotPSD = NULL;
  sigNeutPSD = NULL;
  sigPhotPSD = NULL;

}

DetectorClass::~DetectorClass()
{
  cout << "destructing detectors" << endl;
  delete meanNeutPSD;
  delete meanPhotPSD;
  delete sigNeutPSD;
  delete sigPhotPSD;

  delete energyRes;

  delete lightOutput;
  delete kinLight;

}
