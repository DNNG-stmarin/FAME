#include "readFiss.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Parameters.h"

using namespace std;


void readFiss::LoopExp()
{

   cout << "Now looping through experiment. " << endl;
   cout << "ANN mode set to: " << ANN_mode << endl;
   if (expTree == 0) return;

   int badEvents = 0;

   if(mode == BEAM_MODE)
   {
     cout << "beam acceptance between " << BEAM_ERG_MIN << " and " << BEAM_ERG_MAX << " MeV, with " << BEAM_ERG_BINNUM << " bins. " << endl;
   }

   if(mode == FRAG_MODE)
   {
     cout << "fragment acceptance between angles " << MIN_ANGLE << " and " << MAX_ANGLE  << endl;
     cout << "and masses between " << MIN_MASS << " " << 252-MIN_MASS  << endl;

   }

   expEntries = expTree->GetEntries();
   // expEntries = 10000000;
   Long64_t firstEvent = 0;
   if(expEntries == 0)
   {
     w->noExpFile();
   }
   cout << "Analyzing " << expEntries << " experimental events \n";
   long int numFissIter = 0;

   int nMult, gMult, nMultBack, gMultBack, indexChannel, indexDet;
   Double_t currTime;
   Long64_t nbytes = 0, nb = 0;
   // *************
   // int ergCheck, ergFlag, ergCount, ergNum;
   // int i,j,k,y,d;
   // ***************

   int encN, encP, encBeamErg;
   short** shortListErgN;
   short** shortListErgP;
   short** shortListErgBackN;
   short** shortListErgBackP;
   // *****************
   int** ergMultListN;
   int** ergMultListP;
   // ****************

   // fragment variables
   double avAng;

   if(CovEM_in)
   {
     cout << "initilaizing cov EM short lists" << endl;
     shortListErgN = new short* [NUM_DETECTORS];
     shortListErgP = new short* [NUM_DETECTORS];
     shortListErgBackN = new short* [NUM_DETECTORS];
     shortListErgBackP = new short* [NUM_DETECTORS];
    // ************
    ergMultListN = new int* [NUM_DETECTORS];
    ergMultListP = new int* [NUM_DETECTORS];
    // *************

     for(int d = 0; d < NUM_DETECTORS; d++)
     {
       shortListErgN[d] = new short [NUM_DIFF_N];
       shortListErgP[d] = new short [NUM_DIFF_P];
       shortListErgBackN[d] = new short [NUM_DIFF_N];
       shortListErgBackP[d] = new short [NUM_DIFF_P];
     }

     // *****************
     for (int m = 0; m < NUM_DETECTORS; m++)
     {
       ergMultListN[m] = new int [2];
       ergMultListN[m][0] = 0;                      // energy
       ergMultListN[m][1] = 0;                      // mult
     }
     for (int p = 0; p < NUM_DETECTORS; p++)
     {
       ergMultListP[p] = new int [2];
       ergMultListP[p][0] = 0;                      // energy
       ergMultListP[p][1] = 0;                      // mult
     }
     // ****************

     cout << "BN: " << BN << ", BP: " << BP << ", BA: " << BA << endl;
     cout << "ranges, N: " << MIN_N_ERG <<"-" <<MAX_N_ERG <<  ", P: " << MIN_P_ERG << "-" << MAX_P_ERG << endl;
   }
/*
 __  __       _         _
|  \/  |     (_)       | |
| \  / | __ _ _ _ __   | |     ___   ___  _ __
| |\/| |/ _` | | '_ \  | |    / _ \ / _ \| '_ \
| |  | | (_| | | | | | | |___| (_) | (_) | |_) |
|_|  |_|\__,_|_|_| |_| |______\___/ \___/| .__/
                                        | |
                                        |_|
*/
   cout << "beginning loop" << endl;
   // main loop
   // expEntries = 10000000;

   for (Long64_t jentry=firstEvent; jentry<expEntries + firstEvent; jentry++)
   {
      // updates READ_FAME progress bar (negligible impact on runtime)
      w->setProgress((int)(((double)runNum * (100.0 / (double)NUM_RUNS)) + 100.0 * (((double)jentry / (double)expEntries) / (double)NUM_RUNS)));

      Long64_t ientry = LoadExpTree(jentry);
      if (ientry < 0) break;
      nb = expTree->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      indexChannel = isTrigger(fisChan); // this should be a function of fisChan
      // cout << jentry << endl;

      /*
        ___     _
       / __|  _| |_ ___
      | (_| || |  _(_-<
      \___\_,_|\__/__/

      */
      // broken channel cut
      if(indexChannel < 0)
      {
        cout << "Trigger number " << fisChan << " not recognized." << endl;
        badEvents++;
        continue;
      }


       // beam energy and index cuts
      bool validBeam = (mode == BEAM_MODE) &&
                       (beamEnergy > BEAM_ERG_MIN && beamEnergy < BEAM_ERG_MAX);
                       // && (beamIndex > MIN_BEAM_INDEX && beamIndex < MAX_BEAM_INDEX);
      if((mode == BEAM_MODE) && !(beamEnergy > BEAM_ERG_MIN && beamEnergy < BEAM_ERG_MAX))
      {
        h_fissRej->Fill(CUT_BEAM);
        continue;
      }
      // if((mode == BEAM_MODE) && !(beamIndex > MIN_BEAM_INDEX && beamIndex < MAX_BEAM_INDEX))
      // {
      //   h_fissRej->Fill(CUT_BEAM_INDEX);
      //   continue;
      // }


      // Fragment cuts
      if(mode == FRAG_MODE)
      {
        if(AL < MIN_MASS) continue;
        if(abs(ThetaL) < MIN_ANGLE || abs(ThetaL) > MAX_ANGLE) continue;
        if(abs(ThetaH) < MIN_ANGLE || abs(ThetaH) > MAX_ANGLE) continue;
        if(0.5*abs(ThetaL - ThetaH) < MIN_ANGLE || 0.5*abs(ThetaL - ThetaH) > MAX_ANGLE) continue;
        if(abs(ThetaL + ThetaH) > ANGLE_RESOLUTION) continue;
        // if(ThetaL - ThetaH < 0.05) continue;
      }


      // trigger threshold cut
      if(!(fisDep > THRESHOLD_DEP && fisDep < CLIPPING_DEP))
      {
        h_fissRej->Fill(CUT_DEP);
        continue;
      }



      // fission pile-up test
      currTime = fisTime;
      expTree->GetEntry(jentry+1);
      h_timeDiffTrig[indexChannel]->Fill(fisTime - currTime);
      if(fisTime - currTime < FISS_PILEUP_TIME)
      {
        h_fissRej->Fill(CUT_PILEUP);
        continue;
      }
      else
      {
        expTree->GetEntry(jentry);
      }

      // cout << jentry << endl;



      /*
       ___      _
      / __| ___| |_ _  _ _ __
      \__ \/ -_)  _| || | '_ \
      |___/\___|\__|\_,_| .__/
                       |_|
      */
      // fissions passed all the tests, delete
      numFissIter++;
      h_fissTriggers->Fill(indexChannel);
      // cout << numFissIter << endl;
      h_fissRej->Fill(ACCEPTED_SIGNAL);

      if(numFissIter%1000000 == 0)
      {
        cout << "finished processing " << numFissIter << " valid fissions, " << (Long64_t)jentry + 1 << " entry number" << endl;
      }
      nMult = 0;
      gMult = 0;
      nMultBack = 0;
      gMultBack = 0;


      // Add beamTime histogram only if fission is used in final analysis (i.e. is valid)
      if(mode == BEAM_MODE)
      {
        // Store histogram values
        h_fisDep[indexChannel]->Fill(fisDep);
        h_beamTime[indexChannel]->Fill(beamTime);
        h2_fisDepErg[indexChannel]->Fill(fisDep, beamEnergy);

        encBeamErg = (beamEnergy - BEAM_ERG_MIN)/sizeBerg;
        if(encBeamErg >= BEAM_ERG_BINNUM) encBeamErg = BEAM_ERG_BINNUM-1;
        else if(encBeamErg < 0) encBeamErg = 0;
      }

      if(mode == FRAG_MODE)
      {
        h2_MassTKE->Fill(AL, KEL + KEH);
        h2_MassTKE->Fill(AH, KEL + KEH);

        h2_MassExc->Fill(AL, KEL + KEH);
        h2_MassExc->Fill(AH, KEL + KEH);

        avAng = 0.5*(ThetaL - ThetaH);



        // h2_MassTKE->Fill(AL, Ex);
        // h2_MassTKE->Fill(AH, Ex);
        // h1_exc->Fill(EX);
      }


      // cout << neutronMult << " " << gammaMult << " " << neutronBackMult << " " << gammaBackMult << endl;

      /*
                    _
      _ _  ___ _  _| |_ _ _ ___ _ _
      | ' \/ -_) || |  _| '_/ _ \ ' \
      |_||_\___|\_,_|\__|_| \___/_||_|

      */
      // loop through neutrons
      int n1 = -1, n2 = -1; // For nn angular correlations
      int neutFragL = 0;
      int neutFragH = 0;
      for (int i = 0; i < neutronMult; i++)
      {

        // ann mode cut neutrons
        if(ANN_mode)
        {
          Double_t ANNFlag = crossTalkANN->Value(0, neutronDetTimes[i], neutronLightOut[i]);
          //cout << ANNFlag << "\n";
          if(ANNFlag < thresholdANN) continue;
        }

        if ((neutronLightOut[i] > THRESHOLD) && (neutronLightOut[i] < CLIPPING)
             && (neutronDetTimes[i] < MAX_TIME_N) && (neutronDetTimes[i] >= MIN_TIME_N))
        {

            nMult++;
            h_neutronLightOutputExp->Fill(neutronLightOut[i]);
            h_neutronTofExp->Fill(neutronDetTimes[i]);
            h_neutronEnergyExp->Fill(neutronToFErg[i]);
            h_neutronPSDExp->Fill(neutronPSD[i]);
            h_neutronSinglesExp->Fill(neutronDet[i]);
            h2_neutronEnergyLOExp->Fill(neutronToFErg[i], neutronLightOut[i]);
            h2_neutronLightOutPSDExp->Fill(neutronLightOut[i], neutronPSD[i]);

            indexDet = neutronDet[i];
            if(indexDet < 0 || indexDet > (NUM_DETECTORS - 1))
            {
              cout << "Detector number " << neutronDet[i] << " not recognized from neutronDet." << endl;
              badEvents++;
              continue;
            }

            h_IndivNeutronLightOutputExp[indexDet]->Fill(neutronLightOut[i]);
            h_IndivNeutronTofExp[indexDet]->Fill(neutronDetTimes[i]);
            h_IndivNeutronEnergyExp[indexDet]->Fill(neutronToFErg[i]);
            h_IndivNeutronPSDExp[indexDet]->Fill(neutronPSD[i]);

            h2_IndivNeutronEnergyLOExp[indexDet]->Fill(neutronToFErg[i], neutronLightOut[i]);
            h2_IndivNeutronLightOutPSDExp[indexDet]->Fill(neutronLightOut[i], neutronPSD[i]);



            if (validBeam)
            {
              h2_nLightOutErg[indexChannel]->Fill(beamEnergy, neutronLightOut[i]);
              h2_nToFErg[indexChannel]->Fill(beamEnergy, neutronToFErg[i]);
            }

            if(mode == FRAG_MODE)
            {
              // h2_nSpecExc->Fill(EX, neutronToFErg[i]);

              h3_nSpecMassTKE->Fill(AL, KEL+KEH, neutronToFErg[i]);
              h3_nSpecMassTKE->Fill(AH, KEL+KEH, neutronToFErg[i]);

              h3_nSpecMassTKEdet[indexDet]->Fill(AL, KEL+KEH, neutronToFErg[i]);
              h3_nSpecMassTKEdet[indexDet]->Fill(AH, KEL+KEH, neutronToFErg[i]);


              h3_nSpecMassExc->Fill(AL, EX, neutronToFErg[i]);
              h3_nSpecMassExc->Fill(AH, EX, neutronToFErg[i]);

              h3_nSpecMassExcDet[indexDet]->Fill(AL, EX, neutronToFErg[i]);
              h3_nSpecMassExcDet[indexDet]->Fill(AH, EX, neutronToFErg[i]);


              if(abs(avAng) > FORWARD_FRAG)
              {
                if(neutronCosF[i] > SEP_NEUT_FRAG)
                {
                  h1_neutronComSpec->Fill(neutronEnergyCom[i]);

                  if(neutronEnergyCom[i] > MIN_NEUT_ECOM)
                  {
                    neutronFragment[i] = 1;
                    neutronEnergyCom[i] = neutronToFErg[i] - 1/AL*KEL - 2*sqrt((neutronToFErg[i]*KEL)/(AL))*neutronCosF[i];

                    neutFragL++;
                  }
                  else
                  {
                    neutronFragment[i] = 0;
                  }

                }
                else if((neutronCosF[i] < -SEP_NEUT_FRAG))
                {
                  h1_neutronComSpec->Fill(neutronEnergyCom[i]);

                  if(neutronEnergyCom[i] > MIN_NEUT_ECOM)
                  {
                    neutronFragment[i] = 2;
                    neutronEnergyCom[i] = neutronToFErg[i] - 1/AH*KEH - 2*sqrt((neutronToFErg[i]*KEH)/(AH))*abs(neutronCosF[i]);

                    neutFragH++;
                  }
                  else
                  {
                    neutronFragment[i] = 0;
                  }

                }
                else
                {
                  neutronFragment[i] = 0;
                }
              }

            }



            if(CovEM_in)
            {
              encN = int((neutronToFErg[i] - MIN_N_ERG)/sizeNerg);
              if(encN >= BN)
              {
                encN = BN-1;
              }
              else if(encN < 0)
              {
                encN = 0;
              }
              //cout << NUM_DIFF_N*(nMult-1)<< endl;
              shortListErgN[nMult-1][0] = neutronDet[i];
              shortListErgN[nMult-1][1] = encN;
            }

        }

        // double neutrons
        if(nMult == 1)
        {
          n1 = i;
        }
        else if(nMult == 2)
        {
          n2 = i;
        }
      }

      h_neutronMultExp->Fill(nMult);

      if(mode == FRAG_MODE)
      {
        h3_nMassTKE->Fill(AL, KEL+KEH, nMult);
        h3_nMassTKE->Fill(AH, KEL+KEH, nMult);

        if(abs(avAng) > FORWARD_FRAG)
        {
          // cout << neutFragL << " " << neutFragH << endl;
          h2_neutSawtooth->Fill(AL, neutFragL);
          h2_neutSawtooth->Fill(AH, neutFragH);
        }
      }

      if(validBeam) h2_neutronMultDep[indexChannel]->Fill(fisDep, nMult);

      // angular correlations
      if(nMult == 2)
      {
        if(neutronDet[n1] > neutronDet[n2])
        {
          h2_neutronDoublesMat->Fill((int)neutronDet[n1], (int)neutronDet[n2]);
          // g_neutronAngleCorr->Fill(angles[(int)neutronDet[n1]][(int)neutronDet[n2]]);
        }
        else
        {
          h2_neutronDoublesMat->Fill((int)neutronDet[n2], (int)neutronDet[n1]);
          // g_neutronAngleCorr->Fill(angles[(int)neutronDet[n2]][(int)neutronDet[n1]]);
        }

        if(mode == FRAG_MODE)
        {
          double neutronCosFcom = neutronCosF[n1];

          if(neutFragL == 2 & neutFragH == 0)
          {

          }

        }
      }


      /*
        ___
      / __|__ _ _ __  _ __  __ _
      | (_ / _` | '  \| '  \/ _` |
      \___\__,_|_|_|_|_|_|_\__,_|

      */
      // loop through gamma rays
      int g1 = -1, g2 = -1;
      for (int i = 0; i < gammaMult; i++)
      {
        //cout << "a" << endl;
        if (photonLightOut[i] > THRESHOLD && photonLightOut[i] < CLIPPING
           && photonDetTimes[i] < MAX_TIME_P &&  photonDetTimes[i] >= MIN_TIME_P)
        {
          gMult++;
          h_photonLightOutputExp->Fill(photonLightOut[i]);
          h_photonTofExp->Fill(photonDetTimes[i]);
          h_photonPSDExp->Fill(photonPSD[i]);
          h_photonSinglesExp->Fill(photonDet[i]);
          h2_neutronMultPhotonLOExp->Fill(nMult, photonLightOut[i]);
          h2_photonLightOutPSDExp->Fill(photonLightOut[i], photonPSD[i]);

          indexDet = photonDet[i];
          if(indexDet < 0 || indexDet > (NUM_DETECTORS - 1))
          {
            cout << "Detector number " << photonDet[i] << " not recognized from photonDet." << endl;
            badEvents++;
            continue;
          }

          h_IndivPhotonLightOutputExp[indexDet]->Fill(photonLightOut[i]);
          h_IndivPhotonTofExp[indexDet]->Fill(photonDetTimes[i]);
          h_IndivPhotonPSDExp[indexDet]->Fill(photonPSD[i]);

          h2_IndivPhotonLightOutPSDExp[indexDet]->Fill(photonLightOut[i], photonPSD[i]);

          if (validBeam)
          {
            h2_photonLightOutErg[indexChannel]->Fill(beamEnergy, photonLightOut[i]);
            h2_gammaLightOutErg[indexChannel]->Fill(beamEnergy, photonLightOut[i]);
          }

          if(mode == FRAG_MODE)
          {
            // h2_gSpecExc->Fill(EX, photonLightOut[i]);


            h3_gSpecMassTKE->Fill(AL, KEL+KEH, photonLightOut[i]);
            h3_gSpecMassTKE->Fill(AH, KEL+KEH, photonLightOut[i]);

            h3_gSpecMassTKEdet[indexDet]->Fill(AL, KEL+KEH, photonLightOut[i]);
            h3_gSpecMassTKEdet[indexDet]->Fill(AH, KEL+KEH, photonLightOut[i]);

            h3_gSpecMassExc->Fill(AL, EX, photonLightOut[i]);
            h3_gSpecMassExc->Fill(AH, EX, photonLightOut[i]);

            h3_gSpecMassExcDet[indexDet]->Fill(AL, EX, photonLightOut[i]);
            h3_gSpecMassExcDet[indexDet]->Fill(AH, EX, photonLightOut[i]);

          }

          if(CovEM_in)
          {
            encP = int((photonLightOut[i] - MIN_P_ERG)/sizePerg);
            if(encP >= BP)
            {
              encN = BP-1;
            }
            else if(encP < 0)
            {
              encP = 0;
            }
            shortListErgP[gMult-1][0] = photonDet[i];
            shortListErgP[gMult-1][1] = encP;
          }
        }

        if(gMult == 1)
        {
          g1 = i;
        }
        else if(gMult == 2)
        {
          g2 = i;
        }

      }
      h_photonMultExp->Fill(gMult);
      if(validBeam) h2_gammaMultDep[indexChannel]->Fill(fisDep, gMult);
      if(mode == FRAG_MODE)
      {
        h3_gMassTKE->Fill(AL, KEL+KEH, gMult);
        h3_gMassTKE->Fill(AH, KEL+KEH, gMult);
      }


      if(gMult == 2)
      {
        if(photonDet[g1] > photonDet[g2])
        {
          h2_photonDoublesMat->Fill((int)photonDet[g1], (int)photonDet[g2]);
        }
        else
        {
          h2_photonDoublesMat->Fill((int)photonDet[g2], (int)photonDet[g1]);
        }
      }



      if(gMult == 1 && nMult == 1)
      {
        if(photonDet[g1] > neutronDet[n1])
        {
          h2_neutronPhotonDoublesMat->Fill((int)photonDet[g1], (int)neutronDet[n1]);
        }
        else
        {
          h2_neutronPhotonDoublesMat->Fill((int)neutronDet[n1], (int)photonDet[g1]);
        }


      }

      /*
       _                                     _   _    _
      | |__  ___ __ _ _ __    _ __  __ _ _ _| |_(_)__| |___
      | '_ \/ -_) _` | '  \  | '_ \/ _` | '_|  _| / _| / -_)
      |_.__/\___\__,_|_|_|_| | .__/\__,_|_|  \__|_\__|_\___|
                            |_|
      */
      if(validBeam)
      {
        h_beamErg[indexChannel]->Fill(beamEnergy);
        h2_neutronMultErg[indexChannel]->Fill(beamEnergy, nMult);
        h2_gammaMultErg[indexChannel]->Fill(beamEnergy, gMult);
      }


      /*
       _             _                   _
      | |__  __ _ __| |__  _ _  ___ _  _| |_ _ _ ___ _ _  ___
      | '_ \/ _` / _| / / | ' \/ -_) || |  _| '_/ _ \ ' \(_-<
      |_.__/\__,_\__|_\_\ |_||_\___|\_,_|\__|_| \___/_||_/__/

      */
      // loop through back neutrons
      int nB1 = -1, nB2 = -1;
      for (int i = 0; i < neutronBackMult; i++)
      {
        if ((backNeutronLightOut[i] > THRESHOLD) && (backNeutronLightOut[i] < CLIPPING )
             && (backNeutronDetTimes[i] + BACKGROUND_DELAY < MAX_TIME_N)
             && (backNeutronDetTimes[i] + BACKGROUND_DELAY >= MIN_TIME_N))
        {
            nMultBack++;
            h_neutronLightOutputBack->Fill(backNeutronLightOut[i]);
            h_neutronTofBack->Fill(backNeutronDetTimes[i] + BACKGROUND_DELAY);
            h_neutronEnergyBack->Fill(backNeutronToFErg[i]);
            h_neutronPSDBack->Fill(backNeutronPSD[i]);
            h_neutronSinglesBack->Fill(backNeutronDet[i]);

            indexDet = backNeutronDet[i];
            if(indexDet < 0 || indexDet > (NUM_DETECTORS - 1))
            {
              cout << "Detector number " << backNeutronDet[i] << " not recognized from backNeutronDet." << endl;
              badEvents++;
              continue;
            }

            h_IndivNeutronLightOutputBack[indexDet]->Fill(backNeutronLightOut[i]);
            h_IndivNeutronTofBack[indexDet]->Fill(backNeutronDetTimes[i] + BACKGROUND_DELAY);
            h_IndivNeutronEnergyBack[indexDet]->Fill(backNeutronToFErg[i]);
            h_IndivNeutronPSDBack[indexDet]->Fill(backNeutronPSD[i]);

            if(nMultBack == 1)
            {
              nB1 = i;
            }
            else if(nMultBack == 2)
            {
              nB2 = i;
            }

            if(validBeam)
            {
              h2_nBackToFErg[indexChannel]->Fill(beamEnergy, backNeutronToFErg[i]);
              h2_nBackLightOutErg[indexChannel]->Fill(beamEnergy, backNeutronLightOut[i]);
              //get LO background
            }

            if(CovEM_in)
            {
              encN = int((backNeutronToFErg[i] - MIN_N_ERG)/sizeNerg);
              if(encN >= BN)
              {
                encN = BN-1;
              }
              else if(encN < 0)
              {
                encN = 0;
              }
              //cout << NUM_DIFF_N*(nMult-1)<< endl;
              shortListErgBackN[nMultBack-1][0] = backNeutronDet[i];
              shortListErgBackN[nMultBack-1][1] = encN;
            }
        }
      }
      h_neutronMultBack->Fill(nMultBack);
      if(validBeam) h2_backNeutronMultDep[indexChannel]->Fill(fisDep, nMultBack);

      if(nMultBack == 2)
      {
        if(backNeutronDet[n1] > backNeutronDet[nB2])
        {
          h2_neutronBackDoublesMat->Fill((int)backNeutronDet[nB1], (int)backNeutronDet[nB2]);
        }
        else
        {
          h2_neutronBackDoublesMat->Fill((int)backNeutronDet[nB2], (int)backNeutronDet[nB1]);
        }
      }

      /*
       _             _
      | |__  __ _ __| |__  __ _ __ _ _ __  _ __  __ _
      | '_ \/ _` / _| / / / _` / _` | '  \| '  \/ _` |
      |_.__/\__,_\__|_\_\ \__, \__,_|_|_|_|_|_|_\__,_|
                          |___/
      */
      // loop through back photons
      int gB1 = -1, gB2 = -1;
      for (int i = 0; i < gammaBackMult; i++)
      {
        if (backPhotonLightOut[i] > THRESHOLD && backPhotonLightOut[i] < CLIPPING
            && (backPhotonDetTimes[i] + BACKGROUND_DELAY < MAX_TIME_P)
            && (backPhotonDetTimes[i] + BACKGROUND_DELAY >= MIN_TIME_P))
        {
          gMultBack++;
          h_photonLightOutputBack->Fill(backPhotonLightOut[i]);
          h_photonTofBack->Fill(backPhotonDetTimes[i] + BACKGROUND_DELAY);
          h_photonPSDBack->Fill(backPhotonPSD[i]);
          h_photonSinglesBack->Fill(backPhotonDet[i]);

          indexDet = backPhotonDet[i];
          if(indexDet < 0 || indexDet > (NUM_DETECTORS - 1))
          {
            cout << "Detector number " << backPhotonDet[i] << " not recognized from backPhotonDet." << endl;
            badEvents++;
            continue;
          }

          h_IndivPhotonLightOutputBack[indexDet]->Fill(backPhotonLightOut[i]);
          h_IndivPhotonTofBack[indexDet]->Fill(backPhotonDetTimes[i] + BACKGROUND_DELAY);
          h_IndivPhotonPSDBack[indexDet]->Fill(backPhotonPSD[i]);

          if(gMultBack == 1)
          {
            gB1 = i;
          }
          else if(gMultBack == 2)
          {
            gB2 = i;
          }

          if (validBeam)
          {
            h2_photonBackLightOutErg[indexChannel]->Fill(beamEnergy, backPhotonLightOut[i]);
          }

          if(CovEM_in)
          {
            encP = int((backPhotonLightOut[i] - MIN_P_ERG)/sizePerg);
            if(encP >= BP)
            {
              encN = BP-1;
            }
            else if(encP < 0)
            {
              encP = 0;
            }
            //cout << NUM_DIFF_N*(nMult-1)<< endl;
            shortListErgBackP[gMultBack-1][0] = backPhotonDet[i];
            shortListErgBackP[gMultBack-1][1] = encP;
          }

        }
      }
      h_photonMultBack->Fill(gMultBack);
      if(validBeam) h2_backGammaMultDep[indexChannel]->Fill(fisDep, gMultBack);

      // photon doubles
      if(gMultBack == 2)
      {
        if(backPhotonDet[gB1] > backPhotonDet[gB2])
        {
          h2_photonBackDoublesMat->Fill((int)backPhotonDet[gB1], (int)backPhotonDet[gB2]);
        }
        else
        {
          h2_photonBackDoublesMat->Fill((int)backPhotonDet[gB2], (int)backPhotonDet[gB1]);
        }
      }

      if(validBeam)
      {
        h2_backGammaMultErg[indexChannel]->Fill(beamEnergy, gMultBack);
        h2_backNeutronMultErg[indexChannel]->Fill(beamEnergy, nMultBack);
      }



      // n-gamma doubles
      if(gMultBack == 1 && nMultBack == 1)
      {
        if(backPhotonDet[gB1] > backNeutronDet[nB1])
        {
          h2_neutronPhotonBackDoublesMat->Fill((int)backPhotonDet[gB1], (int)backNeutronDet[nB1]);
        }
        else
        {
          h2_neutronPhotonBackDoublesMat->Fill((int)backNeutronDet[nB1], (int)backPhotonDet[gB1]);
        }
      }

      /*
                     _   _    _                        _   _    _
      _ __  __ _ _ _| |_(_)__| |___ ___ _ __  __ _ _ _| |_(_)__| |___
      | '_ \/ _` | '_|  _| / _| / -_)___| '_ \/ _` | '_|  _| / _| / -_)
      | .__/\__,_|_|  \__|_\__|_\___|   | .__/\__,_|_|  \__|_\__|_\___|
      |_|                               |_|
      */
      h2_neutronGammaMultExp->Fill(nMult, gMult);
      h2_neutronGammaMultBack->Fill(nMultBack, gMultBack);

      /*
                  ___ __  __
       __ _____ _| __|  \/  |
      / _/ _ \ V / _|| |\/| |
      \__\___/\_/|___|_|  |_|

      */
      // covEM filling
      if(CovEM_in)
      {
        // ****************
        int ergCheck, ergFlag, ergCount, ergNum;
        int i,j,k,y,d;
        // *************
        int detN, ergN, detP, ergP;
        bool crossingPoint = false;

        // exp
        for(int iN = 0; iN < nMult; iN ++)
        {
          detN = shortListErgN[iN][0];
          ergN = shortListErgN[iN][1];

          for(int ldetP = 0; ldetP < NUM_DETECTORS; ldetP++)
          {
            for(int lergP = 0; lergP < BP; lergP++)
            {
              for(int iP = 0; iP < gMult; iP++)
              {
                detP = shortListErgP[iP][0];
                ergP = shortListErgP[iP][1];

                if((detP == ldetP) & (ergP == lergP))
                {
                  crossingPoint = true;
                  arrayExp[indexChannel][detN][detP][ergN][ergP][1][1]++;
                  // arrayExpErg[ergN][ergP][1][1]++;
                  if(validBeam)
                  {
                    arrayExpBeam[indexChannel][encBeamErg][detN][detP][ergN][ergP][1][1]++;
                    // *******************
                    // arrayExpErg[indexChannel][encBeamErg][ergN][ergP][1][1]++;
                    // *********************
                  }
                  //cout << "coinc" << endl;
                  continue;
                }
              }
              if(crossingPoint)
              {
                // already filled this combination
                crossingPoint = false;
                continue;
              }
              else
              {
                arrayExp[indexChannel][detN][ldetP][ergN][lergP][1][0]++;
                if(validBeam)
                {
                  arrayExpBeam[indexChannel][encBeamErg][detN][ldetP][ergN][lergP][1][0]++;
                  // *******************
                  // arrayExpErg[indexChannel][encBeamErg][ergN][lergP]++;
                  // *********************
                }
              }

            }
          }
        }

        for(int iP = 0; iP < gMult; iP ++)
        {
          detP = shortListErgP[iP][0];
          ergP = shortListErgP[iP][1];

          for(int ldetN = 0; ldetN < NUM_DETECTORS; ldetN++)
          {
            for(int lergN = 0; lergN < BN; lergN++)
            {
              for(int iN = 0; iN < nMult; iN++)
              {
                detN = shortListErgN[iN][0];
                ergN = shortListErgN[iN][1];

                if((detN == ldetN) & (ergN == lergN))
                {
                  crossingPoint = true;
                  continue;
                }
              }
              if(crossingPoint)
              {
                // already filled this combination
                crossingPoint = false;
                continue;
              }
              else
              {
                arrayExp[indexChannel][ldetN][detP][lergN][ergP][0][1]++;
                if(validBeam)
                {
                  arrayExpBeam[indexChannel][encBeamErg][ldetN][detP][lergN][ergP][0][1]++;
                  // *******************
                  // arrayExpErg[indexChannel][encBeamErg][lergN][ergP][0][1]++;
                  // *********************
                }
              }

            }
          }
        }

        // back
        for(int iN = 0; iN < nMultBack; iN ++)
        {
          detN = shortListErgBackN[iN][0];
          ergN = shortListErgBackN[iN][1];

          for(int ldetP = 0; ldetP < NUM_DETECTORS; ldetP++)
          {
            for(int lergP = 0; lergP < BP; lergP++)
            {
              for(int iP = 0; iP < gMultBack; iP++)
              {
                detP = shortListErgBackP[iP][0];
                ergP = shortListErgBackP[iP][1];

                if((detP == ldetP) & (ergP == lergP))
                {
                  crossingPoint = true;
                  arrayBack[indexChannel][detN][detP][ergN][ergP][1][1]++;
                  if(validBeam) arrayBackBeam[indexChannel][encBeamErg][detN][detP][ergN][ergP][1][1]++;
                  //cout << "coinc" << endl;
                  continue;
                }
              }
              if(crossingPoint)
              {
                // already filled this combination
                crossingPoint = false;
                continue;
              }
              else
              {
                arrayBack[indexChannel][detN][ldetP][ergN][lergP][1][0]++;
                if(validBeam) arrayBackBeam[indexChannel][encBeamErg][detN][ldetP][ergN][lergP][1][0]++;
              }

            }
          }
        }

        for(int iP = 0; iP < gMultBack; iP ++)
        {
          detP = shortListErgBackP[iP][0];
          ergP = shortListErgBackP[iP][1];

          for(int ldetN = 0; ldetN < NUM_DETECTORS; ldetN++)
          {
            for(int lergN = 0; lergN < BN; lergN++)
            {
              for(int iN = 0; iN < nMultBack; iN++)
              {
                detN = shortListErgBackN[iN][0];
                ergN = shortListErgBackN[iN][1];

                if((detN == ldetN) & (ergN == lergN))
                {
                  crossingPoint = true;
                  continue;
                }
              }
              if(crossingPoint)
              {
                // already filled this combination
                crossingPoint = false;
                continue;
              }
              else
              {
                arrayBack[indexChannel][ldetN][detP][lergN][ergP][0][1]++;
                if(validBeam) arrayBackBeam[indexChannel][encBeamErg][ldetN][detP][lergN][ergP][0][1]++;
              }

            }
          }
        }

        // Sorting shortListErgN into energy multiplicty matrix
      //*******************************
          ergNum = 0;
          for (int i = 0; i < nMult; i++)
          {
            ergCheck = 0;
            ergFlag = 0;
            ergCount = 1;
            ergCheck = shortListErgN[i][1];

            // Check if repeat energy
            for (int k = 1; k <= i; k++)
            {
              if (ergCheck == shortListErgN[i-k][1])
              {
                // cout << ergCheck << " is a repeat energy" << endl;
                ergFlag++;
              }
            }
            if (ergFlag > 0)
            {
              continue;
            }

            // determine multiplicity of energy
            for (int j = i+1; j < nMult; j++)
            {
              if (ergCheck == shortListErgN[j][1])
              {
                ergCount++;
                // cout << ergCheck << " has multiplicity " << ergCount << endl;
              }
            }

            // Assign energy and mult
            ergMultListN[ergNum][0] = shortListErgN[i][1];  // energy
            ergMultListN[ergNum][1] = ergCount;             // mult

            // Iterate next entry number
            ergNum++;
          }

          int lengthErgListN = ergNum;
          // cout << "lengthErgListN = " << lengthErgListN << endl;

          // Sorting Gammas
          ergNum = 0;
          for (int i = 0; i < gMult; i++)
          {
            ergCheck = 0;
            ergFlag = 0;
            ergCount = 1;
            ergCheck = shortListErgP[i][1];

            // Check if repeat energy
            for (int k = 1; k <= i; k++)
            {
              if (ergCheck == shortListErgP[i-k][1])
              {
                // cout << ergCheck << " is a repeat energy" << endl;
                ergFlag++;
              }
            }
            if (ergFlag > 0)
            {
              continue;
            }

            // determine multiplicity of energy
            for (int j = i+1; j < gMult; j++)
            {
              if (ergCheck == shortListErgP[j][1])
              {
                ergCount++;
                // cout << ergCheck << " has multiplicity " << ergCount << endl;
              }
            }

            // Assign energy and mult
            ergMultListP[ergNum][0] = shortListErgP[i][1];  // energy
            ergMultListP[ergNum][1] = ergCount;             // mult

            // Iterate next entry number
            ergNum++;
          }

          int lengthErgListP = ergNum;
          // cout << "lengthErgListP = " << lengthErgListP << endl;

          // Fill arrayExpErg
          int nMultE = 0;
          int gMultE= 0;

          for (int nErg = 0; nErg < BN; nErg++)
          {
            // Check in neutron lists energy and mult
            nMultE = 0;
            for (int i = 0; i < lengthErgListN; i++)
            {
              if (nErg == ergMultListN[i][0])
              {
                nMultE = ergMultListN[i][1];
              }
            }

            // Check in gamma lists energy and mult
            for (int gErg = 0; gErg < BP; gErg++)
            {
              // Check in neutron lists energy and mult
              gMultE = 0;
              for (int i = 0; i < lengthErgListP; i++)
              {
                if (gErg == ergMultListP[i][0])
                {
                  gMultE = ergMultListP[i][1];
                }
              }
              arrayExpErg[nErg][gErg][nMultE][gMultE]++;
            }
          }
          // cout << "made it" << endl;
// ***************************

      }


   }

   cout << "Unfortunately I found: " << badEvents << " bad events..." << endl;



   expEntries = numFissIter;
   cout << "We found " << expEntries << " valid measured fissions" << endl;

   h_fissRej->Draw();
}

void readFiss::LoopSim()
{
    cout << "Now looping through simulation. " << endl;
    int badEvents = 0;

    if (expTree == 0) return;
    //CHANGE BACK TO SIM TREE
    simEntries = simTree->GetEntries();
    if(simEntries == 0)
    {
      w->noSimFile();
    }
    cout << "Analyzing " << simEntries << " simulated events \n ";

    long int numFissIter = 0;
    int nMult, gMult, nMultBack, gMultBack, indexDet;
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry = 0; jentry < simEntries; jentry++)
    {
        Long64_t ientry = LoadSimTree(jentry);
        if (ientry < 0) break;
        nb = simTree->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;

        nMult = 0;
        gMult = 0;
        nMultBack = 0;
        gMultBack = 0;
        numFissIter++;
        if(numFissIter%1000000 == 0)
        {
          cout << "finished processing " << numFissIter << " simulated fissions" << endl;
        }


        // loop through neutrons
        for (int i = 0; i < neutronMult; i++)
        {
          if (neutronLightOut[i] > THRESHOLD && neutronLightOut[i] < CLIPPING && neutronDetTimes[i] < MAX_TIME_N  && neutronDetTimes[i] >= MIN_TIME_N)
          {
              nMult++;
              h_neutronLightOutputSim->Fill(neutronLightOut[i]);
              h_neutronTofSim->Fill(neutronDetTimes[i]);
              h_neutronEnergySim->Fill(neutronToFErg[i]);
              h_neutronSinglesSim->Fill(neutronDet[i]);

              indexDet = neutronDet[i];
              if(indexDet < 0 || indexDet > (NUM_DETECTORS - 1))
              {
                cout << "Detector number " << neutronDet[i] << " not recognized from neutronChannel." << endl;
                badEvents++;
                continue;
              }

              h_IndivNeutronLightOutputSim[indexDet]->Fill(neutronLightOut[i]);
              h_IndivNeutronTofSim[indexDet]->Fill(neutronDetTimes[i]);
              h_IndivNeutronEnergySim[indexDet]->Fill(neutronToFErg[i]);
              h_IndivNeutronPSDSim[indexDet]->Fill(neutronPSD[i]);
          }
        }
        h_neutronMultSim->Fill(nMult);

        // loop through gamma rays
        for (int i = 0; i < gammaMult; i++)
        {
          if (photonLightOut[i] > THRESHOLD && photonLightOut[i] < CLIPPING && photonDetTimes[i] < MAX_TIME_P  && photonDetTimes[i] >= MIN_TIME_P)
          {
              gMult++;
              h_photonLightOutputSim->Fill(photonLightOut[i]);
              h_photonTofSim->Fill(photonDetTimes[i]);
              h_photonSinglesSim->Fill(photonDet[i]);

              indexDet = photonDet[i];
              if(indexDet < 0 || indexDet > (NUM_DETECTORS - 1))
              {
                cout << "Detector number " << photonDet[i] << " not recognized from photonChannel." << endl;
                badEvents++;
                continue;
              }

              h_IndivPhotonLightOutputSim[indexDet]->Fill(photonLightOut[i]);
              h_IndivPhotonTofSim[indexDet]->Fill(photonDetTimes[i]);
              h_IndivPhotonPSDSim[indexDet]->Fill(photonPSD[i]);
          }
        }
        h_photonMultSim->Fill(gMult);

    }

    cout << "We found " << simEntries << " valid simulated fissions" << endl;
}

void readFiss::LoopBeam()
{
    // cout << "Now looping through beam. " << endl;
    //
    // if (expTree == 0) return;
    // //CHANGE BACK TO SIM TREE
    // expEntries = expTree->GetEntries();
    //
    // cout << "Analyzing (again)" << expEntries << " experimental events \n ";
    //
    // int nMult, gMult, nMultBack, gMultBack, indexChannel;
    // Long64_t nbytes = 0, nb = 0;
    // for (Long64_t jentry = 0; jentry < expEntries; jentry++)
    // {
    //     Long64_t ientry = LoadExpTree(jentry);
    //     if (ientry < 0) break;
    //     nb = expTree->GetEntry(jentry);   nbytes += nb;
    //     // if (Cut(ientry) < 0) continue;
    //     indexChannel = isTrigger(fisChan); // this should be a function of fisChan
    //
    //     if(indexChannel < 0)
    //     {
    //       cout << "Trigger number " << fisChan << " not recognized." << endl;
    //       exit(10);
    //     }
    //
    //     // skip if the energy of the beam is outside the range
    //     // nathan remove
    //
    //     if(beamEnergy > BEAM_ERG_MIN && beamEnergy < BEAM_ERG_MAX)
    //     {
    //       h_fisDep[indexChannel]->Fill(fisDep);
    //       h_beamTime[indexChannel]->Fill(beamTime);
    //       h2_fisDepErg[indexChannel]->Fill(fisDep, beamEnergy);
    //
    //     }
    //     else
    //     {
    //       // cout << "energy not recognized: " << beamEnergy << endl;
    //       continue;
    //     }
    //
    //     nMult = 0;
    //     gMult = 0;
    //     nMultBack = 0;
    //     gMultBack = 0;
    //
    //     // loop through neutrons
    //     for (int i = 0; i < neutronMult; i++)
    //     {
    //       if ((neutronLightOut[i] > THRESHOLD) && (neutronDetTimes[i] < MAX_TIME_N) )
    //       {
    //         nMult++;
    //       }
    //     }
    //     h2_neutronMultDep[indexChannel]->Fill(fisDep, nMult);
    //
    //     if (fisDep > THRESHOLD_DEP)
    //     {
    //       h2_neutronMultErg[indexChannel]->Fill(beamEnergy, nMult);
    //     }
    //
    //     // loop through gamma rays
    //     for (int i = 0; i < gammaMult; i++)
    //     {
    //       if (photonLightOut[i] > THRESHOLD)
    //       {
    //         gMult++;
    //       }
    //     }
    //     h2_gammaMultDep[indexChannel]->Fill(fisDep, gMult);
    //
    //     if (fisDep > THRESHOLD_DEP)
    //     {
    //       h2_gammaMultErg[indexChannel]->Fill(beamEnergy, gMult);
    //     }
    //
    //     // loop through back neutrons
    //     for (int i = 0; i < neutronBackMult; i++)
    //     {
    //       if ((backNeutronLightOut[i] > THRESHOLD) && (backNeutronDetTimes[i] + BACKGROUND_DELAY < MAX_TIME_N))
    //       {
    //         nMultBack++;
    //       }
    //     }
    //     h2_backNeutronMultDep[indexChannel]->Fill(fisDep, nMultBack);
    //
    //     if (fisDep > THRESHOLD_DEP)
    //     {
    //       h2_backNeutronMultErg[indexChannel]->Fill(beamEnergy, nMultBack);
    //     }
    //
    //
    //     // loop through back photons
    //     for (int i = 0; i < gammaBackMult; i++)
    //     {
    //       if (backPhotonLightOut[i] > THRESHOLD)
    //       {
    //         gMultBack++;
    //       }
    //     }
    //     h2_backGammaMultDep[indexChannel]->Fill(fisDep, gMultBack);
    //
    //     if (fisDep > THRESHOLD_DEP)
    //     {
    //       h2_backGammaMultErg[indexChannel]->Fill(beamEnergy, gMultBack);
    //     }
    // }
}
