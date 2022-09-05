#include "readFiss.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMatrixD.h>
#include <TMath.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Parameters.h"

#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

void readFiss::ExtractCov()
{
  cd_FAME->cd();
  TH1D* h_Angles = new TH1D("Angles","angles between detectors",BA,MIN_THETA,MAX_THETA);
  cout << "extracting cov " << endl;

  arrayCorrExp = new double*** [NUM_TRIGGERS];
  arraySpecExp = new double*** [NUM_TRIGGERS];
  arrayCorrBack = new double*** [NUM_TRIGGERS];
  arraySpecBack = new double*** [NUM_TRIGGERS];

  if (mode == BEAM_MODE)
  {
    arrayCorrExpBeam = new double**** [NUM_TRIGGERS];
    arraySpecExpBeam = new double**** [NUM_TRIGGERS];
    arrayCorrBackBeam = new double**** [NUM_TRIGGERS];
    arraySpecBackBeam = new double**** [NUM_TRIGGERS];
  }

  for (int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    arrayCorrExp[indexChannel] = new double** [BA];
    arraySpecExp[indexChannel] = new double** [BA];
    arrayCorrBack[indexChannel] = new double** [BA];
    arraySpecBack[indexChannel] = new double** [BA];
    if (mode == BEAM_MODE)
    {
      arrayCorrExpBeam[indexChannel] = new double*** [BEAM_ERG_BINNUM];
      arraySpecExpBeam[indexChannel] = new double*** [BEAM_ERG_BINNUM];
      arrayCorrBackBeam[indexChannel] = new double*** [BEAM_ERG_BINNUM];
      arraySpecBackBeam[indexChannel] = new double*** [BEAM_ERG_BINNUM];
      for (int bb = 0; bb < BEAM_ERG_BINNUM; bb++)
      {
        arrayCorrExpBeam[indexChannel][bb] = new double** [BA];
        arraySpecExpBeam[indexChannel][bb] = new double** [BA];
        arrayCorrBackBeam[indexChannel][bb] = new double** [BA];
        arraySpecBackBeam[indexChannel][bb] = new double** [BA];
      }
    }

    for(int ba = 0; ba < BA; ba++)
    {
      arrayCorrExp[indexChannel][ba] = new double* [BN];
      arraySpecExp[indexChannel][ba] = new double* [BN];
      arrayCorrBack[indexChannel][ba] = new double* [BN];
      arraySpecBack[indexChannel][ba] = new double* [BN];

      if (mode == BEAM_MODE)
      {
        for (int bb = 0; bb < BEAM_ERG_BINNUM; bb++)
        {
          arrayCorrExpBeam[indexChannel][bb][ba] = new double* [BN];
          arraySpecExpBeam[indexChannel][bb][ba] = new double* [BN];
          arrayCorrBackBeam[indexChannel][bb][ba] = new double* [BN];
          arraySpecBackBeam[indexChannel][bb][ba] = new double* [BN];
        }
      }

      for(int bn = 0; bn < BN; bn++)
      {
        arrayCorrExp[indexChannel][ba][bn] = new double [BP];
        arraySpecExp[indexChannel][ba][bn] = new double [BP];
        arrayCorrBack[indexChannel][ba][bn] = new double [BP];
        arraySpecBack[indexChannel][ba][bn] = new double [BP];

        if (mode == BEAM_MODE)
        {
          for (int bb = 0; bb < BEAM_ERG_BINNUM; bb++)
          {
          arrayCorrExpBeam[indexChannel][bb][ba][bn] = new double [BP];
          arraySpecExpBeam[indexChannel][bb][ba][bn] = new double [BP];
          arrayCorrBackBeam[indexChannel][bb][ba][bn] = new double [BP];
          arraySpecBackBeam[indexChannel][bb][ba][bn] = new double [BP];
          }
        }

        for(int bp = 0; bp < BP; bp++)
        {
          arrayCorrExp[indexChannel][ba][bn][bp] = 0.;
          arraySpecExp[indexChannel][ba][bn][bp] = 0.;
          arrayCorrBack[indexChannel][ba][bn][bp] = 0.;
          arraySpecBack[indexChannel][ba][bn][bp] = 0.;

          if (mode == BEAM_MODE)
          {
            for (int bb = 0; bb < BEAM_ERG_BINNUM; bb++)
            {
            arrayCorrExpBeam[indexChannel][bb][ba][bn][bp] = 0.;
            arraySpecExpBeam[indexChannel][bb][ba][bn][bp] = 0.;
            arrayCorrBackBeam[indexChannel][bb][ba][bn][bp] = 0.;
            arraySpecBackBeam[indexChannel][bb][ba][bn][bp] = 0.;
            }
          }
        }
      }
    }
  }

  double covVal, specValN, specValP, specVal;
  int ngEnc;
  double totCovCheck = 0;
  double totSpecCheck = 0;
  double covValBack, specValNBack, specValPBack, specValBack;
  int* multPosBeam;
  double* covValBeam;
  double* specValNBeam;
  double* specValPBeam;
  double* specValBeam;
  int* multPosBeamBack;
  double* covValBeamBack;
  double* specValNBeamBack;
  double* specValPBeamBack;
  double* specValBeamBack;
  // int* listPerg;
  //    listNerg = new int[BN];
  multPosBeam = new int[BEAM_ERG_BINNUM];
  covValBeam = new double[BEAM_ERG_BINNUM];
  specValNBeam = new double[BEAM_ERG_BINNUM];
  specValPBeam = new double[BEAM_ERG_BINNUM];
  specValBeam = new double[BEAM_ERG_BINNUM];
  multPosBeamBack = new int[BEAM_ERG_BINNUM];
  covValBeamBack = new double[BEAM_ERG_BINNUM];
  specValNBeamBack = new double[BEAM_ERG_BINNUM];
  specValPBeamBack = new double[BEAM_ERG_BINNUM];
  specValBeamBack = new double[BEAM_ERG_BINNUM];

  for (int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    for(int d1 = 0; d1 < NUM_DETECTORS; d1++)
    {
      for(int d2 = 0; d2 < NUM_DETECTORS; d2++)
      {
        if(d1 == d2) continue;

        h_Angles->Fill(angles[d1][d2]);

        ngEnc = int ((angles[d1][d2] - MIN_THETA)/sizeNgAng);
        if(ngEnc >= BA) ngEnc = BA-1;
        else if(ngEnc < 0) ngEnc = 0;

        for(int e1 = 0; e1 < BN; e1++)
        {
          for(int e2 = 0; e2 < BP; e2++)
          {
            covVal = 0;
            specValN = 0;
            specValP = 0;
            specVal = 0;

            covValBack = 0;
            specValNBack = 0;
            specValPBack = 0;
            specValBack = 0;

            for(int n1 = 0; n1 < MAX_MULT_DET; n1++)
            {
              for(int n2 = 0; n2 < MAX_MULT_DET; n2++)
              {
                int multPos = arrayExp[indexChannel][d1][d2][e1][e2][n1][n2];
                covVal += multPos*n1*n2;
                specValN += multPos*n1;
                specValP += multPos*n2;
                //cout << multPos << " ";

                // background
                int multPosBack = arrayBack[indexChannel][d1][d2][e1][e2][n1][n2];
                covValBack += multPosBack*n1*n2;
                specValNBack += multPosBack*n1;
                specValPBack += multPosBack*n2;

                if (mode == BEAM_MODE)
                {
                  for (int eB = 0; eB < BEAM_ERG_BINNUM; eB++)
                  {
                    multPosBeam[eB] = arrayExpBeam[indexChannel][eB][d1][d2][e1][e2][n1][n2];
                    covValBeam[eB] += multPosBeam[eB]*n1*n2;
                    specValNBeam[eB] += multPosBeam[eB]*n1;
                    specValPBeam[eB] += multPosBeam[eB]*n2;

                    // Background
                    multPosBeamBack[eB] = arrayBackBeam[indexChannel][eB][d1][d2][e1][e2][n1][n2];
                    covValBeamBack[eB] += multPosBeamBack[eB]*n1*n2;
                    specValNBeamBack[eB] += multPosBeamBack[eB]*n1;
                    specValPBeamBack[eB] += multPosBeamBack[eB]*n2;

                  }
                }
              }
            }
            covVal /= expEntries;
            specValN /= expEntries;
            specValP /= expEntries;

            covVal = (covVal - specValN*specValP);
            specVal = (specValN*specValP);

            //cout << covVal << endl;
            arrayCorrExp[indexChannel][ngEnc][e1][e2] += covVal;
            arraySpecExp[indexChannel][ngEnc][e1][e2] += specVal;
            totCovCheck += covVal;
            totSpecCheck += specVal;

            // Background
            covValBack /= expEntries;
            specValNBack /= expEntries;
            specValPBack /= expEntries;

            covValBack = (covValBack - specValNBack*specValPBack);
            specValBack = (specValNBack*specValPBack);


            arrayCorrBack[indexChannel][ngEnc][e1][e2] += covValBack;
            arraySpecBack[indexChannel][ngEnc][e1][e2] += specValBack;
            // totCovCheck += covVal;
            // totSpecCheck += specVal;

            if (mode == BEAM_MODE)
            {
              double beamExpEntries;
              for (int eB = 0; eB < BEAM_ERG_BINNUM; eB++)
              {
                beamExpEntries = 0; // Delete this and switch 447 to "=" instead of "+="
                // for (int chan = 0; chan < NUM_TRIGGERS; chan++)
                // {
                  beamExpEntries += h_beamErg[indexChannel]->GetBinContent(eB+1);
                // }
                covValBeam[eB] /= beamExpEntries;
                specValNBeam[eB] /= beamExpEntries;
                specValPBeam[eB] /= beamExpEntries;

                covValBeam[eB] = (covValBeam[eB] - specValNBeam[eB]*specValPBeam[eB]);
                specValBeam[eB] = (specValNBeam[eB]*specValPBeam[eB]);

                arrayCorrExpBeam[indexChannel][eB][ngEnc][e1][e2] += covValBeam[eB];
                arraySpecExpBeam[indexChannel][eB][ngEnc][e1][e2] += specValBeam[eB];

                // Background
                covValBeamBack[eB] /= beamExpEntries;
                specValNBeamBack[eB] /= beamExpEntries;
                specValPBeamBack[eB] /= beamExpEntries;

                covValBeamBack[eB] = (covValBeamBack[eB] - specValNBeamBack[eB]*specValPBeamBack[eB]);
                specValBeamBack[eB] = (specValNBeamBack[eB]*specValPBeamBack[eB]);
                arrayCorrBackBeam[indexChannel][eB][ngEnc][e1][e2] += covValBeamBack[eB];
                arraySpecBackBeam[indexChannel][eB][ngEnc][e1][e2] += specValBeamBack[eB];
              }
            }

            // Add Correlation histogram? Ratio of cov/spec

          }
        }
      }
    }
  }

  cout << "Cov macro checks:" << endl;
  cout << "cov = "<< totCovCheck << ", spec =  " << totSpecCheck << endl;
  cout << "corrected ratio: " << totCovCheck/totSpecCheck << endl;

  h_Angles->Write();
}


// write the content of the matrices
void readFiss::WriteCovEM()
{
  writeFile->cd();
  cd_FAME->cd();

  cout <<  "wirting covEM resutls to root"  << endl;

  TCanvas** c_CovExp = new TCanvas* [NUM_TRIGGERS];
  TCanvas** c_CovBack = new TCanvas* [NUM_TRIGGERS];
  h3_arrayCorrExp = new TH3D* [NUM_TRIGGERS];
  h3_arraySpecExp = new TH3D* [NUM_TRIGGERS];
  h3_arrayCorrBack = new TH3D* [NUM_TRIGGERS];
  h3_arraySpecBack = new TH3D* [NUM_TRIGGERS];

  // **********************
  TCanvas*** c_arrayMultErg = new TCanvas** [BN];
  h2_arrayMultErg = new TH2I** [BN];
  for (int n = 0; n < BN; n++)
  {
    c_arrayMultErg[n] = new TCanvas* [BP];
    h2_arrayMultErg[n] = new TH2I* [BP];
    for (int p = 0; p < BP; p++)
    {
      c_arrayMultErg[n][p] = new TCanvas("c_arrayMultErg_" + (TString)to_string(n) + "_" + (TString)to_string(p), "arrayMultErg", 400, 500);
      c_arrayMultErg[n][p]->Divide(1,1);
      c_arrayMultErg[n][p]->cd(1);

      h2_arrayMultErg[n][p] = new TH2I("h2_arrayMultErg", "Experimental Detected Mult with Erg Differentiation; Detected Neutron Mult; Detected Gamma Mult", NUM_DETECTORS, 0, NUM_DETECTORS, NUM_DETECTORS, 0, NUM_DETECTORS);
    }
  }
  // **********************

  if (mode == BEAM_MODE)
  {
    h3_arrayCorrExpBeam = new TH3D**[NUM_TRIGGERS];
    h3_arraySpecExpBeam = new TH3D**[NUM_TRIGGERS];
    h3_arrayCorrBackBeam = new TH3D**[NUM_TRIGGERS];
    h3_arraySpecBackBeam = new TH3D**[NUM_TRIGGERS];
  }

  for (int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    c_CovExp[indexChannel] = new TCanvas("c_CovExp_" + (TString)to_string((int)indexChannel), "arrayCorr and arraySpec", 800, 1000);
    c_CovBack[indexChannel] = new TCanvas("c_CovBack_" + (TString)to_string((int)indexChannel), "arrayCorrBack and arraySpecBack", 800, 1000);
    c_CovExp[indexChannel]->Divide(1,2);
    c_CovBack[indexChannel]->Divide(1,2);
    c_CovBack[indexChannel]->cd(1);
    c_CovExp[indexChannel]->cd(1);


    h3_arrayCorrExp[indexChannel] = new TH3D("h3_arrayCorrExp_" + (TString)to_string(indexChannel), "Experimental Corr Matrix; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);
    h3_arraySpecExp[indexChannel] = new TH3D("h3_arraySpecExp_" + (TString)to_string(indexChannel), "Experimental Spec Matrix; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);
    h3_arrayCorrBack[indexChannel] = new TH3D("h3_arrayCorrBack_" + (TString)to_string(indexChannel), "Background Corr Matrix; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);
    h3_arraySpecBack[indexChannel] = new TH3D("h3_arraySpecBack_" + (TString)to_string(indexChannel), "Background Spec Matrix; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);

    if (mode == BEAM_MODE)
    {
      h3_arrayCorrExpBeam[indexChannel] = new TH3D*[BEAM_ERG_BINNUM];
      h3_arraySpecExpBeam[indexChannel] = new TH3D*[BEAM_ERG_BINNUM];
      h3_arrayCorrBackBeam[indexChannel] = new TH3D*[BEAM_ERG_BINNUM];
      h3_arraySpecBackBeam[indexChannel] = new TH3D*[BEAM_ERG_BINNUM];

      for (int eB = 0; eB < BEAM_ERG_BINNUM; eB++)
      {
        h3_arrayCorrExpBeam[indexChannel][eB] = new TH3D("h3_arrayCorrExpBeam_" + (TString)to_string(indexChannel), "Experimental Corr Matrix with Beam Energy MeV; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);
        h3_arraySpecExpBeam[indexChannel][eB] = new TH3D("h3_arraySpecExpBeam_" + (TString)to_string(indexChannel), "Experimental Spec Matrix with Beam Energy MeV; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);
        h3_arrayCorrBackBeam[indexChannel][eB] = new TH3D("h3_arrayCorrBackBeam_" + (TString)to_string(indexChannel), "Background Corr Matrix with Beam Energy MeV; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);
        h3_arraySpecBackBeam[indexChannel][eB] = new TH3D("h3_arraySpecBackBeam_" + (TString)to_string(indexChannel), "Background Spec Matrix with Beam Energy MeV; Neutron Energy (MeV); Gamma Light Out (MeVee); Neutron-Gamma Angle", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG, BA, MIN_THETA, MAX_THETA);
      }
    }

    // Fill TH3Ds with respective matrixices and set proper histogram titles when drawing to canvas
    for (int bP = 0; bP < BP; bP++)
    {
      for (int bN = 0; bN < BN; bN++)
      {
        for (int ngAng = 0; ngAng < BA; ngAng++)
        {
          h3_arrayCorrExp[indexChannel]->SetBinContent(bN+1,bP+1,ngAng+1,arrayCorrExp[indexChannel][ngAng][bN][bP]);  // neutrons = x-axis , photons = y-axis ; n-p angle = z-axis
          h3_arraySpecExp[indexChannel]->SetBinContent(bN+1,bP+1,ngAng+1,arraySpecExp[indexChannel][ngAng][bN][bP]);
          h3_arrayCorrBack[indexChannel]->SetBinContent(bN+1,bP+1,ngAng+1,arrayCorrBack[indexChannel][ngAng][bN][bP]);
          h3_arraySpecBack[indexChannel]->SetBinContent(bN+1,bP+1,ngAng+1,arraySpecBack[indexChannel][ngAng][bN][bP]);

          if (mode == BEAM_MODE){
            for (int eB = 0; eB < BEAM_ERG_BINNUM; eB++)
            {
              h3_arrayCorrExpBeam[indexChannel][eB]->SetBinContent(bN+1,bP+1,ngAng+1,arrayCorrExpBeam[indexChannel][eB][ngAng][bN][bP]);
              h3_arraySpecExpBeam[indexChannel][eB]->SetBinContent(bN+1,bP+1,ngAng+1,arraySpecExpBeam[indexChannel][eB][ngAng][bN][bP]);
              h3_arrayCorrBackBeam[indexChannel][eB]->SetBinContent(bN+1,bP+1,ngAng+1,arrayCorrBackBeam[indexChannel][eB][ngAng][bN][bP]);
              h3_arraySpecBackBeam[indexChannel][eB]->SetBinContent(bN+1,bP+1,ngAng+1,arraySpecBackBeam[indexChannel][eB][ngAng][bN][bP]);
            }
          }
        }
      }
    }
    // *****************
    for (int nN = 0; nN < NUM_DETECTORS; nN++)
    {
      for (int nP = 0; nP < NUM_DETECTORS; nP++)
      {
        for (int bP = 0; bP < BP; bP++)
        {
          for (int bN = 0; bN < BN; bN++)
          {
            h2_arrayMultErg[bN][bP]->SetBinContent(nN+1,nP+1,arrayExpErg[bN][bP][nN][nP]);  //Check this b/c not confident on filling
          }
        }
      }
    }
    // ************************


    h3_arrayCorrExp[indexChannel]->Draw("LEGO2");
    h3_arrayCorrExp[indexChannel]->SetTitle("Experiment Covariance Matrix");

    c_CovExp[indexChannel]->cd(2);
    h3_arraySpecExp[indexChannel]->Draw("LEGO2");
    h3_arraySpecExp[indexChannel]->SetTitle("Experiment Spectrum");
    c_CovExp[indexChannel]->Write();

    c_CovBack[indexChannel]->cd(1);
    h3_arrayCorrBack[indexChannel]->Draw("LEGO2");
    h3_arrayCorrBack[indexChannel]->SetTitle("Background Covariance Matrix");

    c_CovBack[indexChannel]->cd(2);
    h3_arraySpecBack[indexChannel]->Draw("LEGO2");
    h3_arraySpecBack[indexChannel]->SetTitle("Background Spectrum");
    c_CovBack[indexChannel]->Write();

    // *********************
    // Plot arrayMultErg
    for (int bP = 0; bP < BP; bP++)
    {
      for (int bN = 0; bN < BN; bN++)
      {
        c_arrayMultErg[bN][bP]->cd(1);
        h2_arrayMultErg[bN][bP]->Draw("LEGO1");
        h2_arrayMultErg[bN][bP]->SetTitle("Experiment Multiplicity Matrix");
        c_arrayMultErg[bN][bP]->Write();
      }
    }
    // ********************


    if (mode == BEAM_MODE)
    {
      TCanvas*** c_corrSpecBeam =  new TCanvas** [NUM_TRIGGERS];
      TCanvas*** c_corrSpecBeamBack = new TCanvas** [NUM_TRIGGERS];
      // *****************
      TCanvas*** c_corrMultBeam =  new TCanvas** [NUM_TRIGGERS];
      // ******************

      c_corrSpecBeam[indexChannel] = new TCanvas* [BEAM_ERG_BINNUM];
      c_corrSpecBeamBack[indexChannel] = new TCanvas* [BEAM_ERG_BINNUM];
      // ******************
      c_corrMultBeam[indexChannel] =  new TCanvas* [BEAM_ERG_BINNUM];
      // *******************
      for (int eB = 0; eB < BEAM_ERG_BINNUM; eB++)
    	{
        TString s_BEAM_Erg_Low = (TString)to_string(eB + BEAM_ERG_MIN);
        TString s_BEAM_Erg_High = (TString)to_string(eB + BEAM_ERG_MIN + 1);

        // Experiment
        c_corrSpecBeam[indexChannel][eB] = new TCanvas("c_corrSpecBeam_" + (TString)to_string((int)indexChannel) + "_" + s_BEAM_Erg_Low, "arrayCorr and arraySpec for BeamErg " + s_BEAM_Erg_Low + "-" + s_BEAM_Erg_High, 800, 1000);
        c_corrSpecBeam[indexChannel][eB]->Divide(1,2);
        // *****************
        c_corrMultBeam[indexChannel][eB] = new TCanvas("c_corrMultBeam_" + (TString)to_string((int)indexChannel) + "_" + s_BEAM_Erg_Low, "arrayExpErg for BeamErg " + s_BEAM_Erg_Low + "-" + s_BEAM_Erg_High, 400, 500);
        c_corrMultBeam[indexChannel][eB]->Divide(1,1);
        // *****************

        c_corrSpecBeam[indexChannel][eB]->cd(1);
        h3_arrayCorrExpBeam[indexChannel][eB]->Draw("LEGO2");

        c_corrSpecBeam[indexChannel][eB]->cd(2);
        h3_arraySpecExpBeam[indexChannel][eB]->Draw("LEGO2");
        h3_arrayCorrExpBeam[indexChannel][eB]->SetTitle("Experiment Covariance Matrix for BeamErg " + s_BEAM_Erg_Low + "-" + s_BEAM_Erg_High + " MeV");
        h3_arraySpecExpBeam[indexChannel][eB]->SetTitle("Experiment Spectrum for BeamErg " + s_BEAM_Erg_Low + "-" + s_BEAM_Erg_High + " MeV");

        c_corrSpecBeam[indexChannel][eB]->Write();

        // Background
        c_corrSpecBeamBack[indexChannel][eB] = new TCanvas("c_corrSpecBeamBack_" + (TString)to_string((int)indexChannel) + "_" + s_BEAM_Erg_Low, "arrayCorrBack and arraySpecBack for BeamErg " + s_BEAM_Erg_Low + "-" + s_BEAM_Erg_High, 800, 1000);
        c_corrSpecBeamBack[indexChannel][eB]->Divide(1,2);

        c_corrSpecBeamBack[indexChannel][eB]->cd(1);
        h3_arrayCorrBackBeam[indexChannel][eB]->Draw("LEGO2");
        h3_arrayCorrBackBeam[indexChannel][eB]->SetTitle("Background Covariance Matrix for BeamErg " + s_BEAM_Erg_Low + "-" + s_BEAM_Erg_High + " MeV");

        c_corrSpecBeamBack[indexChannel][eB]->cd(2);
        h3_arraySpecBackBeam[indexChannel][eB]->Draw("LEGO2");
        h3_arraySpecBackBeam[indexChannel][eB]->SetTitle("Background Spectrum for BeamErg " + s_BEAM_Erg_Low + "-" + s_BEAM_Erg_High + " MeV");

        c_corrSpecBeamBack[indexChannel][eB]->Write();

      }
    }
  }

  cout << "Writing CovEM results to .csv" << endl;

  mkdir("covResults", 0777);
  cout << "creating" << endl;

  // print the content of the matrices
  ofstream covMatFire;
  ofstream specMatFire;
  string* fileCovFire = new string[NUM_TRIGGERS];
  string* fileSpecFire = new string[NUM_TRIGGERS];

  ofstream covMatFireBack;
  ofstream specMatFireBack;
  string* fileCovFireBack = new string[NUM_TRIGGERS];
  string* fileSpecFireBack = new string[NUM_TRIGGERS];

  // ********************
  ofstream multMatFire;
  string fileMultArray;
  mkdir("covResults/MultArrays", 0777);

  for(int eN = 0; eN < BN; eN ++)
  {
   for(int eP = 0; eP < BP; eP ++)
    {
        // create new files
      fileMultArray = "covResults/MultArrays/MultErg_" + to_string(eN) + "_" + to_string(eP) + ".csv";
      multMatFire.open(fileMultArray);

        // loop through multiplicities
      for(int nN = 0; nN < NUM_DETECTORS; nN++)
      {
       for(int nP = 0; nP < NUM_DETECTORS; nP++)
        {
           multMatFire << arrayExpErg[eN][eP][nN][nP];

           if(nP < NUM_DETECTORS - 1)
           {
             multMatFire << ",";

           }
           else if(nP == NUM_DETECTORS - 1)
           {
             multMatFire << "\n";
           }
        }
      }
      multMatFire.close();
    }
  }
  // ********************

  for (int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    for(int eA = 0; eA < BA; eA++)
    {
      fileCovFire[indexChannel] = "covResults/covEM_" + to_string(indexChannel) + "_" + to_string(eA) + ".csv";
      covMatFire.open(fileCovFire[indexChannel]);

      fileSpecFire[indexChannel] = "covResults/specEM_" + to_string(indexChannel) + "_" + to_string(eA) + ".csv";
      specMatFire.open(fileSpecFire[indexChannel]);

      fileCovFireBack[indexChannel] = "covResults/covEMBack_" + to_string(indexChannel) + "_" + to_string(eA) + ".csv";
      covMatFireBack.open(fileCovFireBack[indexChannel]);

      fileSpecFireBack[indexChannel] = "covResults/specEMBack_" + to_string(indexChannel) + "_" + to_string(eA) + ".csv";
      specMatFireBack.open(fileSpecFireBack[indexChannel]);

      // cout << eA << endl;

      for(int eN = 0; eN < BN; eN ++)
      {
       for(int eP = 0; eP < BP; eP ++)
        {
           covMatFire << arrayCorrExp[indexChannel][eA][eN][eP];
           specMatFire << arraySpecExp[indexChannel][eA][eN][eP];
           covMatFireBack << arrayCorrBack[indexChannel][eA][eN][eP];
           specMatFireBack << arraySpecBack[indexChannel][eA][eN][eP];

           if(eP < BP - 1)
           {
             covMatFire << ",";
             specMatFire << ",";
             covMatFireBack << ",";
             specMatFireBack << ",";
           }
           else if(eP == BP - 1)
           {
             covMatFire << "\n";
             specMatFire << "\n";
             covMatFireBack << "\n";
             specMatFireBack << "\n";
           }
        }
      }

      covMatFire.close();
      specMatFire.close();
      covMatFireBack.close();
      specMatFireBack.close();

    }

    if (mode == BEAM_MODE)
    {
      ofstream covMatBeamFire;
      ofstream specMatBeamFire;
      string* fileCovBeamFire = new string[NUM_TRIGGERS];
      string* fileSpecBeamFire = new string[NUM_TRIGGERS];

      ofstream covMatBeamFireBack;
      ofstream specMatBeamFireBack;
      string* fileCovBeamFireBack = new string[NUM_TRIGGERS];
      string* fileSpecBeamFireBack = new string[NUM_TRIGGERS];

      string beamFolderName;

      for (int eB = 0; eB < BEAM_ERG_BINNUM; eB++)
      {
        beamFolderName = "covResultsBeam_" + to_string(indexChannel) + "_" + to_string(eB);
        mkdir(beamFolderName.c_str(), 0777);

        for(int eA = 0; eA < BA; eA++)
        {

        fileCovBeamFire[indexChannel] = beamFolderName + "/covEMbeam_" + to_string(eA) + "_" + to_string(eB) + ".csv";
        covMatBeamFire.open(fileCovBeamFire[indexChannel]);

        fileSpecBeamFire[indexChannel] = beamFolderName + "/specEMbeam_" + to_string(eA) + "_" + to_string(eB) + ".csv";
        specMatBeamFire.open(fileSpecBeamFire[indexChannel]);

        fileCovBeamFireBack[indexChannel] = beamFolderName + "/covEMBackbeam_" + to_string(eA) + "_" + to_string(eB) + ".csv";
        covMatBeamFireBack.open(fileCovBeamFireBack[indexChannel]);

        fileSpecBeamFireBack[indexChannel] = beamFolderName + "/specEMBackbeam_" + to_string(eA) + "_" + to_string(eB) + ".csv";
        specMatBeamFireBack.open(fileSpecBeamFireBack[indexChannel]);

          for(int eN = 0; eN < BN; eN ++)
          {
           for(int eP = 0; eP < BP; eP ++)
            {
               covMatBeamFire << arrayCorrExpBeam[indexChannel][eB][eA][eN][eP];
               specMatBeamFire << arraySpecExpBeam[indexChannel][eB][eA][eN][eP];
               covMatBeamFireBack << arrayCorrBackBeam[indexChannel][eB][eA][eN][eP];
               specMatBeamFireBack << arraySpecBackBeam[indexChannel][eB][eA][eN][eP];

               if(eP < BP - 1)
               {
                 covMatBeamFire << ",";
                 specMatBeamFire << ",";
                 covMatBeamFireBack << ",";
                 specMatBeamFireBack << ",";
               }
               else if(eP == BP - 1)
               {
                 covMatBeamFire << "\n";
                 specMatBeamFire << "\n";
                 covMatBeamFireBack << "\n";
                 specMatBeamFireBack << "\n";
               }
            }
          }
          covMatBeamFire.close();
          specMatBeamFire.close();
          covMatBeamFireBack.close();
          specMatBeamFireBack.close();
        }
      }
    }
  }
}

void readFiss::AnalyseCovEM()
{

     //  //open file
     // ofstream covFire;
     // string fileFire = "covFire" + to_string(BN) + "_" + to_string(BP) +  ".csv";
     // covFire.open(fileFire);


     //   // compute the marginals
     //   double ergN[BN] = {0};
     //   double ergP[BP] = {0};

     //   double covNmarg[BN] = {0};
     //   double covPmarg[BP] = {0};

     //   double specN[BN] = {0};
     //   double specP[BP] = {0};


     //   // compute the marginals for neutrons
     //   for(int eN = 0; eN < BN; eN ++)
     //   {
     //    ergN[eN] = (eN + 0.5)*sizeNerg + minNerg;

     //    for(int eP = 0; eP < BP; eP ++)
     //     {
     //      covNmarg[eN] += h2_arrayCorr[eN][eP];
     //      specN[eN] += h2_arraySpec[eN][eP];
     //     }

     //     covFire << covNmarg[eN] << "," << specN[eN] << endl;
     //   }

     //   // compute the marginals for photons
     //   for(int eP = 0; eP < BP; eP++)
     //   {
     //    ergP[eP] = (eP + 0.5)*sizePerg + minPerg;

     //    for(int eN = 0; eN < BN; eN++)
     //     {
     //      covPmarg[eP] += h2_arrayCorr[eN][eP];
     //      specP[eP] += h2_arraySpec[eN][eP];
     //     }
     //     covFire << covPmarg[eP] << "," << specP[eP] << endl;
     //   }


     //   double normCovN[BN] = {0};
     //   double normCovP[BP] = {0};

     //   for(int eN = 0; eN < BN; eN ++)
     //   {
     //    normCovN[eN] = covNmarg[eN]/specN[eN];
     //   }

     //   for(int eP = 0; eP < BP; eP++)
     //   {
     //    normCovP[eP] = covPmarg[eP]/specP[eP];
     //   }

     //   double sumNeut = 0;
     //   double sumPhot = 0;

     //   for(int eN = 0; eN < BN; eN ++)
     //   {
     //    sumNeut += specN[eN];
     //   }

     //   for(int eP = 0; eP < BP; eP++)
     //   {
     //    sumPhot += specP[eP];
     //   }

     //   cout << sumNeut << " " << sumPhot << endl;


     //   // save as graphs
     //   TGraph* g_margN = new TGraph(BN, ergN, covNmarg);
     //   TGraph* g_margP = new TGraph(BP, ergP, covPmarg);
     //   TGraph* g_specN = new TGraph(BN, ergN, specN);
     //   TGraph* g_specP = new TGraph(BP, ergP, specP);
     //   TGraph* g_normN = new TGraph(BN, ergN, normCovN);
     //   TGraph* g_normP = new TGraph(BP, ergP, normCovP);

     //   g_margN->SetName("neutCov");
     //   g_margP->SetName("photCov");
     //   g_specN->SetName("neutSpec");
     //   g_specP->SetName("photSpec");
     //   g_normN->SetName("neutNorm");
     //   g_normP->SetName("photNorm");

     //   g_normN->SetTitle("Neutron dependent correlations; neutron energy (MeV); cov");
     //   g_normP->SetTitle("Photon dependent correlations; photon energy (MeV); cov");

     //   // write the graphs to file
     //   g_margN->Write();
     //   g_margP->Write();
     //   g_specN->Write();
     //   g_specP->Write();
     //   g_normN->Write();
     //   g_normP->Write();

     //   //h2_arrayCorr.Write();

     //   fileCorr->Write();
     //   fileCorr->Close();

       // covMatFire->Close();
       // covFire->Close();
}

void readFiss::CovEM()
{
//
//    cout << "Beginning the great CovEM loop" << endl;
//    // place to store all the result
//
//    h2_arrayCorr = new TH2D("CovMat", "Covariance Matrix; Neutron ToF Energy (MeV); Photon Deposited Energy (MeVee); counts", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG);
//    h2_arraySpec = new TH2D("SpecMat", "Spectrum Matrix; Neutron ToF Energy (MeV); Photon Deposited Energy (MeVee); counts", BN, MIN_N_ERG, MAX_N_ERG, BP, MIN_P_ERG, MAX_P_ERG);
//
//    // make sure tree is open
//    if (expTree == 0) return;
//
//    // updated event-by-event history
//    int* listNerg;
//    int* listPerg;
//    listNerg = new int[BN];
//    listPerg = new int[BP];
//
//    // initialize the EM histograms
//    TString t_nameH;
//    TString t_rootName = "em_";
//    TString titleH = "EnergyMode Correlations";
//
//    TH2I*** emMode;
//    emMode = new TH2I** [BN];
//    for(int eN = 0; eN < BN; eN++)
//    {
//    	emMode[eN] = new TH2I* [BP];
//
//       for(int eP = 0; eP < BP; eP++)
//       {
//          t_nameH = t_rootName + to_string(eN) + " " + to_string(eP);
//          emMode[eN][eP] = new TH2I(t_nameH, titleH, 10,0,10, 10,0,10);
//       }
//    }
//
//    TH2I**** emModeChunks;
//    emModeChunks = new TH2I*** [NUM_CHUNKS];
//    for(int chunk = 0; chunk < NUM_CHUNKS; chunk++)
//    {
//      emModeChunks[chunk] = new TH2I** [BN];
//      for(int eN = 0; eN < BN; eN++)
//      {
//      	emModeChunks[chunk][eN] = new TH2I* [BP];
//
//         for(int eP = 0; eP < BP; eP++)
//         {
//            t_nameH = t_rootName + to_string(eN) + " " + to_string(eP) + " " + to_string(chunk);
//            emModeChunks[chunk][eN][eP] = new TH2I(t_nameH, titleH, 10,0,10, 10,0,10);
//         }
//      }
//    }
//
//
//    int encN, encP;
//    // read the entries
//    Long64_t nentries = expTree->GetEntriesFast();
//    nentries = 1000000;
//    Long64_t nbytes = 0, nb = 0;
//
//    for (Long64_t jentry=0; jentry<nentries;jentry++)
//    {
//
//       if(jentry%1000000 == 0)
//       {
//         cout << jentry << " entries done. " << endl;
//       }
//
//
//
//    	  // load the new entry
//       Long64_t ientry = LoadExpTree(jentry);
//       if (ientry < 0) break;
//       nb = expTree->GetEntry(jentry);   nbytes += nb;
//
//      // cout << "loaded the new entry " << endl;
//
//      // reset the event-by-event list to 0
//      for(int eN = 0; eN < BN; eN ++)
//      {
//       listNerg[eN] = 0;
//      }
//      for(int eP = 0; eP < BP; eP ++)
//      {
//       listPerg[eP] = 0;
//      }
//
//      // cout << "resetted lists" << endl;
//
//
//      // Neutron event-by-event spectrum
//      for(int nN = 0; nN < neutronMult; nN++)
//      {
//
//        if(neutronLightOut[nN] > THRESHOLD && neutronDetTimes[nN] < MAX_TIME_N)
//        {
//          encN = int((neutronToFErg[nN] - MIN_N_ERG)/sizeNerg);
//
//          if(encN >= BN)
//          {
//            encN = BN-1;
//          }
//          else if(encN < 0)
//          {
//            encN = 0;
//          }
//
//          listNerg[encN] += 1;
//        }
//
//      }
//
//      // cout << "encoded neutrons" << endl;
//
//      // Photon event-by-event spectrum
//      for(int nP = 0; nP < gammaMult; nP++)
//      {
//        encP = int((photonLightOut[nP] - MIN_P_ERG)/sizePerg);
//
//        if(encP >= BP)
//        {
//          encP = BP-1;
//        }
//        else if(encP < 0)
//        {
//          encP = 0;
//        }
//
//        listPerg[encP] += 1;
//      }
//
//      // cout << "encoded gammas" << endl;
//
//
//      // populate the em histograms
//      for(int eN = 0; eN < BN; eN ++)
//      {
//       for(int eP = 0; eP < BP; eP ++)
//        {
//         emMode[eN][eP]->Fill(listNerg[eN], listPerg[eP]);
//
//         if(SELF_ERROR)
//         {
//           emModeChunks[jentry%NUM_CHUNKS][eN][eP]->Fill(listNerg[eN], listPerg[eP]);
//         }
//        }
//      }
//
//      // cout << "filled EM histograms" << endl;
//
//     }
//
//    // find the covariance and spec at each point
//      for(int eN = 0; eN < BN; eN ++)
//      {
//       for(int eP = 0; eP < BP; eP ++)
//        {
//         h2_arrayCorr->SetBinContent(eN+1, eP+1, emMode[eN][eP]->GetCovariance());
//         h2_arraySpec->SetBinContent(eN+1, eP+1, (emMode[eN][eP]->GetMean(1))*(emMode[eN][eP]->GetMean(2)));
//
//        }
//      }
//
//      cout << "finished setting EM matrices " << endl;
//
//
//
//
//      if(SELF_ERROR)
//      {
//        double listCov[NUM_CHUNKS] = {0};
//        double listSpec[NUM_CHUNKS] = {0};
//
//        double totCov = 0;
//        double totSpec = 0;
//
//        cout << "computing self error from data fluctuations" << endl;
//        // find the covariance at each point
//          for(int eN = 0; eN < BN; eN ++)
//          {
//           for(int eP = 0; eP < BP; eP ++)
//            {
//              for(int chunk = 0; chunk < NUM_CHUNKS; chunk++)
//              {
//                double thisCov = emModeChunks[chunk][eN][eP]->GetCovariance();
//                double thisSpec = (emModeChunks[chunk][eN][eP]->GetMean(1))*(emMode[eN][eP]->GetMean(2));
//
//                listCov[chunk] = thisCov;
//                listSpec[chunk] = thisSpec;
//
//              }
//
//              h2_arrayCorr->SetBinError(eN+1, eP+1, TMath::StdDev(NUM_CHUNKS, listCov)/sqrt(NUM_CHUNKS-1) );
//              h2_arraySpec->SetBinError(eN+1, eP+1, TMath::StdDev(NUM_CHUNKS, listSpec)/sqrt(NUM_CHUNKS-1) );
//            }
//          }
//      }
//
//
}
