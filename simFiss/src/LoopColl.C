#include "SFAME.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRandom3.h>

void sfame::LoopColl()
{
   readCollisionTree();

   SfameFiss->cd();

   cout << "Looping through collision file. " << endl;
   if (rCollisionTree == 0) return;

   cout << rCollisionTree << endl;
   Long64_t nentries = rCollisionTree->GetEntries();
   cout << "Processing " << nentries << " events" << endl;

   double historyTime;

   // vertex properties
   int vertexChannel;
   int vertexID;
   int vertexType;
   double vertexLightOut;
   double vertexTail;
   double vertexTime;
   int vertexRR;
   TRandom3* randGen = new TRandom3();

   // track properties
   int numVertices[MAX_TRACKS] = {0};
   int trackChannel[MAX_TRACKS] = {0};
   int trackID[MAX_TRACKS] = {0};
   double trackLightOut[MAX_TRACKS] = {0};
   double trackTail[MAX_TRACKS] = {0};
   double trackPSP[MAX_TRACKS] = {0};
   double trackTime[MAX_TRACKS] = {0};
   double trackType[MAX_TRACKS] = {0};

   // track flags
   double trackRR_flag[MAX_TRACKS] = {0};
   double trackPU_flag[MAX_TRACKS] = {0};

   int numTracks = 0;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadCollTree(jentry);
      if (ientry < 0) break;
      nb = rCollisionTree->GetEntry(jentry);   nbytes += nb;

      // reset track
      for(int tr = 0; tr < numTracks; tr++)
      {
        numVertices[tr] = 0;
        trackChannel[tr] = -1;
        trackID[tr] = -1;
        trackLightOut[tr] = 0;
        trackTail[tr] = 0;
        trackPSP[tr] = 0;
        trackTime[tr] = 0;
        trackType[tr] = 0;

        trackRR_flag[tr] = 0;
        trackPU_flag[tr] = 0;
      }
      numTracks = 0;

      tTime = f_history*HISTORY_SPACING;
      tPSP = ACCEPT_PSP;
      tDep = ACCEPT_TRIG;
      tChan = TRIGGER_CHANNEL;
      tType = ACCEPT_TYPE;

      historyTime = randGen->Gaus(0, TIME_RES_TRIGGER);


      for(int l = 0; l < f_numLines; l++)
      {
        // important properties of the vertex
        cout << l << endl;
        vertexChannel = cellToChannel(f_cell[l]);

        if(vertexChannel < 0)
        {

          // cout << "did not recognize channel " << vertexChannel << endl;
          continue;
        }
        vertexID = f_particle[l];
        vertexType = f_type[l];
        // vertexLightOut = detectors[vertexChannel].lightOutput->Eval(f_energyDep[l]); // Does this ever check the particle type? (giha)
        vertexTime = f_time[l]*10.0 - historyTime; // shakes to ns conversion

        // psd simulation
        if(vertexType == NEUTRONS)
        {
          if(f_zaid[l] == HYDROGEN)
          {
            vertexLightOut = detectors[vertexChannel].lightOutput->Eval(f_energyDep[l]);
          }
          else if(f_zaid[l] == CARBON)
          {
            vertexLightOut = 0.02 * f_energyDep[l];
          }
          else
          {
            // cout << "Not carbon or hydrogen!" << endl;
            // vertexLightOut = -99;
          }
          vertexTail = randGen->Gaus(detectors[vertexChannel].meanNeutPSD->Eval(vertexLightOut), detectors[vertexChannel].sigNeutPSD->Eval(vertexLightOut));
          if(f_scatters[l] == 0)
          {
            vertexRR = 1;
          }
          else // neutron has scattered before
          {
            vertexRR = 0;
          }

        }
        else if(vertexType == PHOTONS)
        {
          vertexLightOut = f_energyDep[l];
          vertexTail = randGen->Gaus(detectors[vertexChannel].meanPhotPSD->Eval(vertexLightOut), detectors[vertexChannel].sigPhotPSD->Eval(vertexLightOut));

          if(f_scatters[l] == 1) // yes this is correct... a bug in MCNP makes photons start at 1
          {
            vertexRR = 1;
          }
          else // photon has scattered before
          {
            vertexRR = 0;
          }

        }
        vertexTail *= vertexLightOut;


        // check if the history is new
        if( (numTracks == 0) ||
            (vertexChannel != trackChannel[numTracks - 1]) ||
            (vertexID != trackID[numTracks - 1]) ||
            (vertexType != trackType[numTracks - 1]) ) // new history if current track has a different type, since vertexID may be corrupted
        {
          numTracks++;

          trackChannel[numTracks-1] = vertexChannel;
          trackType[numTracks-1] = vertexType; // Put particle type in CoincTree
          trackID[numTracks-1] = vertexID;
          trackRR_flag[numTracks-1] = vertexRR;
          numVertices[numTracks-1] = 1;
          trackLightOut[numTracks-1] = vertexLightOut;
          trackTail[numTracks-1] = vertexTail;
          trackTime[numTracks-1] = vertexTime * vertexLightOut;

        }
        // add to history
        else
        {
          if(abs(vertexTime - trackTime[numTracks-1]/trackLightOut[numTracks-1]) < PULSE_GENERATION_WINDOW)
          {
            numVertices[numTracks-1]++;
            trackLightOut[numTracks-1] += vertexLightOut;
            trackTail[numTracks-1] += vertexTail;
            trackTime[numTracks-1] += vertexTime * vertexLightOut;
          }
        }

      }

      // clean up the tracks
      for (int tr = 0; tr < numTracks; tr++)
      {
        // time estimation
        trackTime[tr] /= trackLightOut[tr];
        trackTime[tr] = randGen->Gaus(trackTime[tr], detectors[trackChannel[tr]].timeRes);
        // psd estimation
        trackPSP[tr] = trackTail[tr]/trackLightOut[tr];
        // energy broadening
        trackLightOut[tr] = randGen->Gaus(trackLightOut[tr], detectors[trackChannel[tr]].energyRes->Eval(trackLightOut[tr]));
      }

      // determine pile-up
      for (int tr1 = 0; tr1 < numTracks; tr1++)
      {
        for (int tr2 = 0; tr2 < tr1; tr2++)
        {
          if((trackChannel[tr1] == trackChannel[tr2]) & (abs(trackTime[tr1] - trackTime[tr2]) < COINCIDENCE_WINDOW))
          {
            trackPU_flag[tr1] = 1;
            trackPU_flag[tr2] = 1;
          }
        }
      }

      // determine observables
      int acc = 0;
      for (int tr = 0; tr < numTracks; tr++)
      {
        if((trackLightOut[tr] > THRESHOLD) & (trackTime[tr] < COINCIDENCE_WINDOW))
        {
          totType[acc] = trackType[tr]; // What is totType? What if there are many different particles?
          totChan[acc] = trackChannel[tr];
          totDep[acc] = trackLightOut[tr];
          totTail[acc] = trackTail[tr];
          totPSP[acc] = trackPSP[tr];
          totToF[acc] = trackTime[tr];
          totFlag[acc] = trackRR_flag[tr];
          acc++;
        }
      }
      tMult = acc;

      coincTree->Fill();

   }

   // close out of everything
   SfameFiss = coincTree->GetCurrentFile();
   SfameFiss->Write();

   SfameFiss->Close();


}
