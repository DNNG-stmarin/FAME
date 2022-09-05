//
//

#include "SFAME.h"
#include "Constants.h"
#include <sstream>

// functions in simParameters

// converts from the cell value to the channel value
int sfame::cellToChannel(int cell)
{
    int channel;
    bool goodChan = false;
    for(int jC = 0; jC < cellToChanList->GetN(); jC++)
    {
      if(cell == cellToChanList->GetPointX(jC))
      {
         channel = cellToChanList->GetPointY(jC);
         goodChan = true;
         break;
      }
    }
    if(!goodChan) channel = -1;
    return channel;
}
int sfame::chanToCell(int chan)
{
    int cell = (int)(chanToCellList->Eval(chan));
    return cell;
}

// assigns a lightoutput value
double sfame::birks(int channel, double energy, int typeP, int zaid)
{
/*
    if((energy < MIN_EXP) || (energy > MAX_EXP))
    {
        lightout = 0.0;
    }
    // neutrons
    else if(typeP == NEUTRON & zaid == HYDROGEN)
    {
        // TGraph eval at (energy) for a certain detector
        lightout = lightoutFunctions[channel][energy]; // ? lightoutput functions is a weird mess
    }
    else if(typeP == NEUTRON & zaid == CARBON)
    {
        lightout = 0.02 * energy;
    }
    // photons
    else if(typeP == tp.PHOTON)
    {
        lightout = energy;
    }
    // edge cases
    else
    {
        lightout = energy;
    }
    if(lightout < 0.0)
    {
        lightout = 0.0;
    }
    return lightout;
*/
}

// V cuts
// cuts using energy
int sfame::cutLightOut(double height, int typeP)
{
/*
   int newType = 0;
   // cuts out the particles that have energies outside of the boundry
   if((height < ENERGYCUT) || (height > CLIPPED))
    {
       if(typeP == NEUTRON){
           newType = CUTNEUTRON;
       }
       elif(typeP == PHOTON){
           newType = CUTPHOTON;
       }

    }
    // otherwise the type stays the same
    else
    {
       newType = typeP;
    }
    return newType;
    */

}

// cuts using time
int sfame::cutTimePsd(double times, int typeP)
{
  /*
    int newType = 0;
    if((typeP == NEUTRON) & (times < MINTIMEN || times > MAXTIMEN))
    {
         newType = BADNEUTRON;
    }

    else if((typeP == PHOTON) & (times < MINTIMEP || times > MAXTIMEP))
    {
        newType = BADPHOTON;
    }
    else
    {
        newType = typeP;
    }

    return newType;
    */
}

/* non lightoutput functions

double sfame::findTn(double times, int channel):
        double v = ((DET_DIST[channel])/times);
        double Tn = 0.5*cn.MASS_NEUTRON* ( v/cn.C )**2;
        // cout << DETECTOR_DISTANCE[channel]) << endl;
        return Tn;
*/
