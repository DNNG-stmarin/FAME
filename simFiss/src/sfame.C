//
//

#include "SFAME.h"
#include "TString.h"

sfame::sfame(InfoSystem* inInfo)
{

    // read from colliosn tree
    nameCollTree = "collTree.root";
    REUSE_DATA = 1;
    if(REUSE_DATA == 1) numCollTreeFiles = 1;

    info = inInfo;

    getInfo();
    initializeInputFiles();
    initializeDetectors();
    initializeCollisionTree();
    initializeCoincTree();
}

sfame::~sfame()
{
    cout << "Cleaning up " << endl;

    delete info;

    delete detectors;
    delete collisionTree->GetCurrentFile();
    delete coincTree->GetCurrentFile();
    delete SfameFiss;

    delete chanToCellList;
    delete cellToChanList;
    delete posXList;
    delete posYList;
    delete posZList;
    delete timeResList;

    delete LightOutList;
    delete KinLightList;

    delete meanNeutPSDArray;
    delete meanPhotPSDArray;
    delete sigNeutPSDArray;
    delete sigPhotPSDArray;
}
