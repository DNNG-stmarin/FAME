/*
Author: Stefano Marin
Purpose: Loop through the entire the coincidence tree and create the fission
         tree.
Date: May 14th, Ann Arbor
*/

#include "DetectorSystemClass.h"
// #include "mappingFunctions.h"
#include <fstream>
#include <TVector3.h>
#include <TRotation.h>
#include <TRandom3.h>

void DetectorSystemClass::FissionAnalysis()
{

  fissionFile = new TFile(nameFission + ".root", "RECREATE");
  fissionTree = new TTree(nameFission, nameFission);
  InitFiss();

  fissionFile->cd();

  Long64_t nentries = tree->GetEntriesFast();
  // nentries = 10000;
  Long64_t nbytes = 0, nb = 0;

  // neutron and photon multiplicities
  int nMult, pMult;
  int nBackMult, pBackMult;

  // detection time
  double timeDet;
  int numDet, numTrig;
  double engDet;

  double neutVelocity, neutErg;

  TVector3 trigDir;
  TVector3 fragLDir;
  TVector3 fragHDir;
  TVector3 trigNorm;
  TVector3 fragNorm;

  TRandom3* randPhi = new TRandom3();

  TVector3 neutDir;
  TVector3 gamDir;


  TRandom3* killFlag = new TRandom3();

  // cout << "NUM_DET: " << NUM_DETS << endl;
  // for(int d =0; d < NUM_DETS; d++)
  // {
  //     cout << DETECTORS[d] << endl;
  // }

  bool psdNcut, psdPcut, timeNcut, timePcut, timeBackNcut, timeBackPcut, ergCut;




  for (Long64_t jentry = 0; jentry < nentries; jentry++)
  {
    // load tree
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = tree->GetEntry(jentry);   nbytes += nb;

    if(jentry % 1000000 == 0)
    {
      cout << jentry << " fissions processed." << endl;
    }

    // cout << jentry << endl;


	  if(tMult >= NUM_DETS)
	  {
		 continue;
	  }

    numTrig = isTrigger(tChan);
    //cout << tChan << " " << numTrig << endl;

    // Define offset based on targetCoord.txt
    if (NUM_BEAMS > 0)
    {
      double distance = BEAM_DISTANCE + triggers[numTrig].Z; // Include offset for trigger location along beam path
      f_beamTime = bTime - triggers[numTrig].beamDelay + distance / LIGHT_C;
      f_beamEnergy = MASS_NEUTRONS *
        (1 / pow(1 - pow(distance / f_beamTime / LIGHT_C, 2.), 0.5) - 1);
      // f_beamEnergy = 0.5 * MASS_NEUTRONS / pow(LIGHT_C, 2)
      //   * pow(BEAM_DISTANCE / f_beamTime, 2); Too classical for Ionel

      f_beamDep = bErg;
      f_beamPSP = bPSP;
      f_beamChan = bChan;
      f_beamIndex = bIndex;

    }

    if(FRAGMENT_MODE == 1)
    {
      f_AL = rAL;
      f_AH = rAH;
      f_KEL = rKEL;
      f_KEH = rKEH;
      f_ThetaL = rThetaL;
      f_ThetaH = rThetaH;
      f_EX = rEX;

      f_An1 = rAn1;
      f_An2 = rAn2;
      f_Gr1 = rGr1;
      f_Gr2 = rGr2;
      f_Cat = rCat;



      // fragment direction routine

      // set the fragment direction
      trigDir.SetXYZ(triggers[numTrig].U, triggers[numTrig].V, triggers[numTrig].W);
      trigNorm.SetXYZ(0, -1*triggers[numTrig].W, triggers[numTrig].V);

      fragLDir = trigDir;
      fragHDir = trigDir;

      // make sure the chamber is oriented correctly
      if(abs(rA1 - rAL) < 0.01) f_ThetaL *= -1;
      else f_ThetaH *= -1;

      double fragPhi = randPhi->Uniform(-TMath::Pi(), TMath::Pi());

      fragLDir.Rotate(TMath::ACos(f_ThetaL), trigNorm);
      fragLDir.Rotate(fragPhi, trigDir);

      fragHDir.Rotate(TMath::ACos(f_ThetaH), trigNorm);
      fragHDir.Rotate(-fragPhi, trigDir);

      fragLDir *= 1/fragLDir.Mag();
      fragHDir *= 1/fragHDir.Mag();

      fragLDir *= sqrt(2*f_KEL/(f_AL*MASS_NEUTRONS));
      fragHDir *= sqrt(2*f_KEH/(f_AH*MASS_NEUTRONS));


      fragNorm = fragLDir.Orthogonal();
      fragNorm *= 1/fragNorm.Mag();

    }





    // allocating the fission info
    f_fisTime = tTime;
    f_fisDep = tDep;
    f_fisChan = tChan;
    f_fisPSP = tPSP;



    // Assign fission trigger type based on beamTime
    // Times are hard-coded for now since I anticipate needing more complex logic later



    // reset the neutron and photon multiplicities
    nMult = 0;
    pMult = 0;
    nBackMult = 0;
    pBackMult = 0;

    // if(tMult > 0)
    // {
    //   cout << tMult << endl;
    // }
    // cout << tMult << endl;

    for(int j = 0; j < tMult; j++)
    {
      // find the number of the detector
      numDet = isDetector(totChan[j]);
      if(SIM_FILE == 1)
      {
        numDet = totChan[j];
      }



      // Calculate distance to detector
      double adjX = detectors[numDet].X - triggers[numTrig].X;
      double adjY = detectors[numDet].Y - triggers[numTrig].Y;
      double adjZ = detectors[numDet].Z - triggers[numTrig].Z;
      //cout << adjX << " " << adjY << " " <<  adjZ << endl;
      detectors[numDet].distance = sqrt(pow(adjX, 2.) + pow(adjY, 2.) + pow(adjZ, 2.));
      double detDist = detectors[numDet].distance;

      // detection time corrected for delay
      timeDet = totToF[j] -  detectors[numDet].timeDelay[numTrig];
      if(SIM_FILE == 1) timeDet = totToF[j];

      engDet = totDep[j]/detectors[numDet].calibration;
      if(SIM_FILE == 1) engDet = totDep[j];

      // cout << engDet << " " << timeDet << endl;

      //if numdet is broken, continue and skip this detector
      bool quit = 0;
      for(int k=0; k< NUM_EXCLUDED; k++) {
        if(numDet == EXCLUDE_DETECTORS[k]) {
          quit = 1;
        }
      }
      if(quit) {
        continue;
      }



      // cuts for neutrons & lower than something
      if((detectors[numDet].type == ORGANIC) & (totPSP[j] < 0 || totPSP[j] > 1))
      {
        continue;
      }


      if(detectors[numDet].type == INORGANIC)
      {
        psdNcut = false;
        psdPcut = true;
      }
      else
      {
        psdNcut = ((((DOUBLE_DISC == 1) & (totPSP[j] > detectors[numDet].discPSDNeut->Eval(engDet)))
                  || ((DOUBLE_DISC != 1) & (totPSP[j] > detectors[numDet].discPSD->Eval(engDet)))));



        psdPcut = ((((DOUBLE_DISC == 1) & (totPSP[j] < detectors[numDet].discPSDNeut->Eval(engDet)))
                      ||
                      ((DOUBLE_DISC != 1) & (totPSP[j] < detectors[numDet].discPSD->Eval(engDet)))));
      }




      timeNcut = ((timeDet > MIN_TIME_N) & (timeDet < MAX_TIME_N));
      timePcut = ((timeDet > MIN_TIME_P) & (timeDet < MAX_TIME_P));

      timeBackNcut = ((timeDet > MIN_TIME_N - BACKGROUND_SHIFT) & (timeDet < MAX_TIME_N - BACKGROUND_SHIFT));
      timeBackPcut = ((timeDet > MIN_TIME_P - BACKGROUND_SHIFT) & (timeDet < MAX_TIME_P - BACKGROUND_SHIFT));

      ergCut = ((engDet > DETECTOR_THRESHOLD));






      if(psdNcut & timeNcut & ergCut)
      {

        // kill the flag
        if((SIM_FILE == 1) &  (REMOVE_FLAG > 0) & (totFlag[j] == 0))
        {
          if( (killFlag->Uniform(0,1) < REMOVE_FLAG) & (totFlag[j] == 0) ) continue;
        }

        neutronDetTimes[nMult] = timeDet;
        neutronLightOut[nMult] = engDet;
        neutronPSD[nMult] = totPSP[j];
        neutVelocity = (1.0/LIGHT_C)*detDist/timeDet;
        // if (timeDet == 0) {cout << "Wee woo" << endl;}
        // if (LIGHT_C == 0) {cout << "wtf" << endl;}
        // cout << "";
        // cout << numDet << " " << detectors[numDet].distance << endl;
        // cout << (1.0/2.0)*MASS_NEUTRONS*pow(neutVelocity,2) << endl;
        neutErg = 0.5*MASS_NEUTRONS*pow(neutVelocity, 2);
        neutronToFErg[nMult] = neutErg;
        neutronDet[nMult] = numDet;
        neutronVx[nMult] = adjX / detDist*neutVelocity;
        neutronVy[nMult] = adjY / detDist*neutVelocity;
        neutronVz[nMult] = adjZ / detDist*neutVelocity;
        if(SIM_FILE == 1) neutronFlag[nMult] = totFlag[j];

        neutDir.SetXYZ(neutronVx[nMult], neutronVy[nMult], neutronVz[nMult]);
        // neutDir *= 1/neutDir.Mag();

        if(FRAGMENT_MODE == 1)
        {
          neutronCosF[nMult] = neutDir.Dot(fragLDir)/(neutDir.Mag() * fragLDir.Mag());
          neutronPhiF[nMult] = neutDir.Dot(fragNorm)/(neutDir.Mag() * fragNorm.Mag());

          if(neutronCosF[nMult] > 0)
          {
            neutronFragment[nMult] = 1;
            neutDir -= fragLDir;
          }
          else if(neutronCosF[nMult] < 0)
          {
            neutronFragment[nMult] = 2;
            neutDir -= fragHDir;
          }

          neutronEnergyCom[nMult] = 0.5*MASS_NEUTRONS*(neutDir.Mag(),2);
          neutronCosCom[nMult] = neutDir.Dot(fragLDir)/(neutDir.Mag() * fragLDir.Mag());
          neutronPhiCom[nMult] = neutDir.Dot(fragNorm)/(neutDir.Mag() * fragNorm.Mag());
        }


        nMult++;
      }


      // cuts for gammas
      else if(psdPcut & timePcut & ergCut)
      {
        // kill the flag
        if((SIM_FILE == 1) & (REMOVE_FLAG > 0) & (totFlag[j] == 0))
        {
          if( (killFlag->Uniform(0,1) < REMOVE_FLAG) & (totFlag[j] == 0) ) continue;
        }

        photonDetTimes[pMult] = timeDet;
        photonLightOut[pMult] = engDet;
        photonPSD[pMult] = totPSP[j];
        photonDet[pMult] = numDet;
        photonVx[pMult] = adjX / detDist*LIGHT_C;
        photonVy[pMult] = adjY / detDist*LIGHT_C;
        photonVz[pMult] = adjZ / detDist*LIGHT_C;
        if(SIM_FILE == 1) photonFlag[pMult] = totFlag[j];


        gamDir.SetXYZ(photonVx[pMult], photonVy[pMult], photonVz[pMult]);
        // gamDir *= 1/gamDir.Mag();

        if(FRAGMENT_MODE == 1)
        {
          photonCosF[pMult] = gamDir.Dot(fragLDir)/(gamDir.Mag() * fragLDir.Mag());
          photonPhiF[pMult] = gamDir.Dot(fragNorm)/(gamDir.Mag() * fragNorm.Mag());
        }


        pMult++;
      }

      // cuts for background neutrons
      else if(psdNcut & timeBackNcut & ergCut)
      {
        backNeutronDetTimes[nBackMult] = timeDet;
        backNeutronLightOut[nBackMult] = engDet;
        backNeutronPSD[nBackMult] = totPSP[j];
        neutVelocity = (1.0/LIGHT_C)*detDist/(timeDet + BACKGROUND_SHIFT);
        backNeutronToFErg[nBackMult] = (1.0/2.0)*MASS_NEUTRONS*pow(neutVelocity,2);
        backNeutronDet[nBackMult] = numDet;
        backNeutronVx[nBackMult] = adjX/detDist*neutVelocity;
        backNeutronVy[nBackMult] = adjY/detDist*neutVelocity;
        backNeutronVz[nBackMult] = adjZ/detDist*neutVelocity;

        neutDir.SetXYZ(backNeutronVx[nBackMult], backNeutronVy[nBackMult], backNeutronVz[nBackMult]);
        // neutDir *= 1/neutDir.Mag();

        if(FRAGMENT_MODE == 1)
        {
          backNeutronCosF[nBackMult] = neutDir.Dot(fragLDir)/(neutDir.Mag() * fragLDir.Mag());
          backNeutronPhiF[nBackMult] = neutDir.Dot(fragNorm)/(neutDir.Mag() * fragNorm.Mag());
        }

        nBackMult++;
      }

      // cuts for background photons
      else if(psdPcut & timeBackPcut & ergCut)
      {
        backPhotonDetTimes[pBackMult] = timeDet;
        backPhotonLightOut[pBackMult] = engDet;
        backPhotonPSD[pBackMult] = totPSP[j];
        backPhotonDet[pBackMult] = numDet;
        backPhotonVx[pBackMult] = adjX/detDist*LIGHT_C;
        backPhotonVy[pBackMult] = adjY/detDist*LIGHT_C;
        backPhotonVz[pBackMult] = adjZ/detDist*LIGHT_C;

        gamDir.SetXYZ(adjX, adjY, adjZ);
        // gamDir *= 1/gamDir.Mag();

        if(FRAGMENT_MODE == 1)
        {
          backPhotonCosF[pMult] = gamDir.Dot(fragLDir)/(gamDir.Mag() * fragLDir.Mag());
          backPhotonPhiF[pMult] = gamDir.Dot(fragNorm)/(gamDir.Mag() * fragNorm.Mag());
        }

        pBackMult++;
      }
      // else
    }

    // set branches of final tree
    f_neutronMult = nMult;
    f_gammaMult = pMult;
    f_neutronBackMult = nBackMult;
    f_gammaBackMult = pBackMult;

    fissionTree->Fill();
  }

  fissionFile = fissionTree->GetCurrentFile();
  fissionFile->Write();
  fissionFile->Close();
}


//deltapsd, save covariances,
void DetectorSystemClass::FissionAnalysisLoop()
{
  fissionFile->cd();
  ofstream myfile;
  string filename = "file" + to_string(DETECTOR_THRESHOLD) + ".csv";
  myfile.open(filename);

  int times = 10;
  double startpsd = -0.03;
  double stoppsd = 0.03;
  double step = (abs(startpsd)+abs(stoppsd))/times;
  double deltapsd = startpsd;

  for(int delta=0; delta<times+1; delta++)
  {
    //create 2d histogram th2i
    TString deltaname= "multHist" + to_string(deltapsd);
    TH2I* multHist = new TH2I(deltaname, "MultHist; Mult N; Mult P", 10, 0, 10, 10, 0, 10);

    Long64_t nentries = tree->GetEntriesFast();
    Long64_t nbytes = 0, nb = 0;

    // neutron and photon multiplicities
    int nMult, pMult;
    int nBackMult, pBackMult;

    // detection time
    double timeDet;
    int numDet, numTrig;
    int engDet;

    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
       // load tree
       Long64_t ientry = LoadTree(jentry);
       if (ientry < 0) break;
       nb = tree->GetEntry(jentry);   nbytes += nb;

       // allocating the fission info
       f_fisTime = tTime;
       f_fisDep = tDep;

       // reset the neutron and photon multiplicities
       nMult = 0;
       pMult = 0;
       nBackMult = 0;
       pBackMult = 0;

       numTrig = isTrigger(tChan);

       for(int j = 0; j < tMult; j++)
       {
         // find the number of the detector
        numDet = isDetector(totChan[j]);
        if(SIM_FILE == 1) numDet = totChan[j];

        // detection time corrected for delay
        timeDet = totToF[j] -  detectors[numDet].timeDelay[numTrig];

        engDet = totDep[j]/detectors[numDet].calibration;
        if(SIM_FILE == 1) engDet =  totDep[j];


        // cuts for neutrons
        if(
          (totPSP[j] > detectors[numDet].discPSD->Eval(engDet) + deltapsd)  //+deltapsd
          &
          (timeDet > MIN_TIME_N)
          &
          (timeDet < MAX_TIME_N)
          &
          (engDet > DETECTOR_THRESHOLD)
          )
        {
             nMult++;
        }

        // cuts for gammas
        else if(
          (totPSP[j] < detectors[numDet].discPSD->Eval(engDet) + deltapsd)  //^^ -0.03 +0.03
          &
          (timeDet > MIN_TIME_P)
          &
          (timeDet < MAX_TIME_P)
          &
          (engDet > DETECTOR_THRESHOLD)
          )
        {
             pMult++;
        }

        // cuts for background neutrons
        else if(
          (totPSP[j] > detectors[numDet].discPSD->Eval(engDet) + deltapsd)
          &
          (timeDet > BACKGROUND_SHIFT + MIN_TIME_N)
          &
          (timeDet < BACKGROUND_SHIFT + MAX_TIME_N)
          &
          (engDet > DETECTOR_THRESHOLD)
          )
        {
             nBackMult++;
        }

        // cuts for background photons
        else if(
          (totPSP[j] < detectors[numDet].discPSD->Eval(engDet) + deltapsd)
          &
          (timeDet> BACKGROUND_SHIFT + MIN_TIME_P)
          &
          (timeDet< BACKGROUND_SHIFT + MAX_TIME_P)
          &
          (engDet > DETECTOR_THRESHOLD)
          )
        {
             pBackMult++;
        }

       }

       // set branches of final tree
       f_neutronMult = nMult;
       f_gammaMult = pMult;
       f_neutronBackMult = nBackMult;
       f_gammaBackMult = pBackMult;

       multHist->Fill(nMult, pMult);
       fissionTree->Fill();
    }
    cout << deltapsd << " " << multHist->GetCovariance() << " " << multHist->GetMean(1) << " " << multHist->GetMean(2) << endl;
    myfile << deltapsd << "," << multHist->GetCovariance() << "," << multHist->GetMean(1) << "," << multHist->GetMean(2) << "\n";
    multHist->Write();
    deltapsd += step;
  }

  myfile.close();
}
