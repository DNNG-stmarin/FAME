//
//

#include <fstream>
#include <TGraph.h>
#include <string.h>
#include <TF1.h>

#include "Constants.h"

using namespace std;

#ifndef DetectorClass_h
#define DetectorClass_h

class DetectorClass
{
public:

    DetectorClass();
    ~DetectorClass();

    TGraph* lightOutput;
    TGraph* kinLight;

    double x, y, z;
    double dist;

    double timeRes;

    TF1* energyRes;

    int channel;
    int cell;

    TF1* meanNeutPSD;
    TF1* meanPhotPSD;
    TF1* sigNeutPSD;
    TF1* sigPhotPSD;
};

#endif
