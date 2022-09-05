#include "readFiss.h"

#include <TLegend.h>
#include <TCanvas.h>
#include <TRatioPlot.h>
#include <TPaveStats.h>
#include <TProfile2D.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

void readFiss::PlotFragmentEmission()
{
  cd_fragment->cd();

  cout << "Plotting Fragment results" << endl;

  TCanvas* c_FragYield = new TCanvas("c_fragYield","c_fragYield", 1200, 800);
  c_FragYield->Divide(3,1);

  c_FragYield->cd(1);
  TProfile2D* h2_gMassTKE = h3_gMassTKE->Project3DProfile("xy");
  h2_gMassTKE->SetName("h2_gMassTKE");
  h2_gMassTKE->Draw("COLZ");

  c_FragYield->cd(2);
  TProfile2D* h2_nMassTKE = h3_nMassTKE->Project3DProfile("xy");
  h2_nMassTKE->SetName("h2_nMassTKE");
  h2_nMassTKE->Draw("COLZ");

  c_FragYield->cd(3);
  h2_MassTKE->Draw("COLZ");

  c_FragYield->Write();



  TCanvas* c_FragSpec = new TCanvas("c_fragSpec","c_fragSpec", 1200, 800);
  c_FragSpec->Divide(3,1);

  c_FragSpec->cd(1);
  h3_gSpecMassTKE->Draw("COLZ");

  c_FragSpec->cd(2);
  h3_nSpecMassTKE->Draw("COLZ");

  c_FragSpec->cd(3);
  h2_MassTKE->Draw("COLZ");

  c_FragSpec->Write();




  cout << "writing fragment results to csv" << endl;
  mkdir("fragResults/", 0777);

  h3_gSpecMassTKE->RebinZ(2);
  for(int i = 0; i < NUM_DETECTORS; i++)
  {
    h3_gSpecMassTKEdet[i]->RebinZ(2);
  }

  int numMASSbins = h3_gSpecMassTKE->GetXaxis()->GetNbins();
  int numTKEbins = h3_gSpecMassTKE->GetYaxis()->GetNbins();
  int numGEbins = h3_gSpecMassTKE->GetZaxis()->GetNbins();
  // cout << h3_gSpecMassTKE->GetZaxis()->GetNbins() << endl;
  int numNEbins = h3_nSpecMassTKE->GetZaxis()->GetNbins();

  ofstream gamFragFile;
  ofstream neutFragFile;
  ofstream yieldFragFile;

  TString nameFileGam;
  TString nameFileNeut;

  TString baseFileGam = "fragResults/gamFrag_";
  TString baseFileNeut = "fragResults/neutFrag_";

  TString csvExt = ".csv";
  TString sepStr = "_";


  yieldFragFile.open("fragResults/yieldFrag.csv");

  // write yield
  cout << "writing yield to csv" << endl;
  for(int m = 1; m <= numMASSbins; m++)
  {
    for(int k = 1; k <= numTKEbins; k++)
    {
      yieldFragFile << h2_MassTKE->GetBinContent(m, k);

      if(k < numTKEbins) yieldFragFile << ", ";
      else yieldFragFile << "\n ";
    }
  }
  yieldFragFile.close();



  cout << "writing gam to csv" << endl;
  for(int eg = 1; eg <= numGEbins; eg++)
  {
    nameFileGam = baseFileGam + (TString)to_string(eg) + csvExt;
    gamFragFile.open(nameFileGam);
    // write yield
    for(int m = 1; m <= numMASSbins; m++)
    {
      for(int k = 1; k <= numTKEbins; k++)
      {
        gamFragFile << h3_gSpecMassTKE->GetBinContent(m, k, eg);

        if(k < numTKEbins) gamFragFile << ", ";
        else gamFragFile << "\n ";
      }
    }
    gamFragFile.close();

  }

  cout << "writing neut to csv" << endl;
  for(int en = 1; en <= numNEbins; en++)
  {
    nameFileNeut = baseFileNeut + (TString)to_string(en) + csvExt;
    neutFragFile.open(nameFileNeut);
    // write yield
    for(int m = 1; m <= numMASSbins; m++)
    {
      for(int k = 1; k <= numTKEbins; k++)
      {
        neutFragFile << h3_nSpecMassTKE->GetBinContent(m, k, en);

        if(k < numTKEbins) neutFragFile << ", ";
        else neutFragFile << "\n ";
      }
    }
    neutFragFile.close();

  }

  cout << "writig all detectrors separately" << endl;
  for(int deT = 0; deT < NUM_DETECTORS; deT++)
  {

    for(int eg = 1; eg <= numGEbins; eg++)
    {
      nameFileGam = baseFileGam + (TString)to_string(eg) + sepStr + (TString)to_string(deT)  + csvExt;
      gamFragFile.open(nameFileGam);
      // write yield
      for(int m = 1; m <= numMASSbins; m++)
      {
        for(int k = 1; k <= numTKEbins; k++)
        {
          gamFragFile << h3_gSpecMassTKEdet[deT]->GetBinContent(m, k, eg);

          if(k < numTKEbins) gamFragFile << ", ";
          else gamFragFile << "\n ";
        }
      }
      gamFragFile.close();

    }

    for(int en = 1; en <= numNEbins; en++)
    {
      nameFileNeut = baseFileNeut + (TString)to_string(en) + sepStr + (TString)to_string(deT) + csvExt;
      neutFragFile.open(nameFileNeut);
      // write yield
      for(int m = 1; m <= numMASSbins; m++)
      {
        for(int k = 1; k <= numTKEbins; k++)
        {
          neutFragFile << h3_nSpecMassTKEdet[deT]->GetBinContent(m, k, en);

          if(k < numTKEbins) neutFragFile << ", ";
          else neutFragFile << "\n ";
        }
      }
      neutFragFile.close();

    }
  }

}

void readFiss::PlotNeutronSawtooth()
{
  cd_fragment->cd();

  cout << "Plotting Sawtooths" << endl;

  TProfile* p1_nSawtooth = h2_neutSawtooth->ProfileX();
  p1_nSawtooth->SetName("p1_nSawtooth");
  p1_nSawtooth->Write();

  h1_neutronComSpec->Write();
}
