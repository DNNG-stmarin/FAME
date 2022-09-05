#include "readFiss.h"
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TCanvas.h>
#include <TPaveStats.h>
#include <TLatex.h>
#include <TLine.h>

#include <algorithm>
#include <vector>
#include <utility>

using namespace std;


void readFiss::CorrAnalysis()
{
  cout << "Generating correlated neutron Energy/LO fits" << endl;
  Slice();

  cout << "Generating angular correlations" << endl;
  AngCorr();
}

void readFiss::Slice()
{
  int projectionNum = 30;
  int startBin = 1;

  // choose where to start and stop
  bool found = false;
  for(int i = 1; i < h2_neutronEnergyLOExp->GetNbinsX(); ++i)
  {
    for(int j = 1; j < h2_neutronEnergyLOExp->GetNbinsY(); ++j)
    {
      if(!found && (h2_neutronEnergyLOExp->GetBinContent(i, j) > 0) &&
      (h2_neutronEnergyLOExp->GetBinContent(i, j+1) > h2_neutronEnergyLOExp->GetBinContent(i, j)) &&
      (h2_neutronEnergyLOExp->Integral(i, i, 1, h2_neutronEnergyLOExp->GetNbinsY()) > (h2_neutronEnergyLOExp->Integral() / 100)))
      {
        startBin = i;
        found = true;
        break;
      }
    }
    if(found && (h2_neutronEnergyLOExp->Integral(i, i, 1, h2_neutronEnergyLOExp->GetNbinsY()) < (h2_neutronEnergyLOExp->Integral() / 100)))
    {
      projectionNum = i - startBin;
      break;
    }
  }

  TF1** myfunc = new TF1*[projectionNum];
  TFitResultPtr* Results = new TFitResultPtr[projectionNum]; // currently unused

  TH1D** Projections = new TH1D*[projectionNum];
  TH1D** Derivatives = new TH1D*[projectionNum];
  double* left_bound = new double[projectionNum];
  TH1D** Seconds = new TH1D*[projectionNum];
  TGraph* final = new TGraph(projectionNum);

  for(int i = 0; i < projectionNum; ++i)
  {
    Projections[i] = h2_neutronEnergyLOExp->ProjectionY((TString)"projection" + (TString)to_string(i), startBin + i, startBin + i);
    Projections[i]->Rebin(5);
    Derivatives[i] = new TH1D((TString)"derivative"+ (TString)to_string(i), "Derivative of Projection;Neutron Light Output [MeVee];Rate of Change",
                                      Projections[i]->GetNbinsX(), Projections[i]->GetXaxis()->GetXmin(), Projections[i]->GetXaxis()->GetXmax());
    Seconds[i] = new TH1D((TString)"2nd_derivative" + (TString)to_string(i), "2nd Derivative of Projection;Neutron Light Output [MeVee];Rate of Change",
                                      Derivatives[i]->GetNbinsX(), Derivatives[i]->GetXaxis()->GetXmin(), Derivatives[i]->GetXaxis()->GetXmax());
    myfunc[i] = new TF1((TString)"myfunc" + (TString)to_string(i), "gaus");

    for(int j = 1; j <= Projections[i]->GetNbinsX(); ++j)
    {
      Derivatives[i]->SetBinContent(j, abs((Projections[i]->GetBinContent(j+1) - Projections[i]->GetBinContent(j)) /
                            (Projections[i]->GetBinCenter(j+1) - Projections[i]->GetBinCenter(j))));
    }

    for(int k = 1; k <= Derivatives[i]->GetNbinsX(); ++k)
    {
      Seconds[i]->SetBinContent(k, (Derivatives[i]->GetBinContent(k+1) - Derivatives[i]->GetBinContent(k)) /
                              (Derivatives[i]->GetBinCenter(k+1) - Derivatives[i]->GetBinCenter(k)));
    }
    Seconds[i]->Rebin(2);

    //Derivatives[i]->Rebin(5); // makes structure more visible
    left_bound[i] = THRESHOLD;

    for(int k = Seconds[i]->GetNbinsX(); k >= 1; --k)
    {
      if(Seconds[i]->GetBinContent(k - 1) > 0 &&
         Seconds[i]->GetBinContent(k) < 0 &&
        (Projections[i]->GetBinContent(2 * k) > (Projections[i]->GetMaximum() / 10)))
        // (Seconds[i]->GetBinContent(k - 1) > (1000 * (1 - Derivatives[i]->GetMean(1))) &&
        //  Seconds[i]->GetBinContent(k) < (-1000 * (1 - Derivatives[i]->GetMean(1)))))
      {
        left_bound[i] = Seconds[i]->GetBinCenter(k);
        break;
      }
    }


    myfunc[i]->SetParameters(100000, left_bound[i], 0.25);
    double leftLimit;
    if(left_bound[i] - 0.1 > THRESHOLD)
    {
      leftLimit = left_bound[i] - 0.1;
    }
    else
    {
      leftLimit = THRESHOLD - 0.1;
    }
    myfunc[i]->SetParLimits(1, leftLimit, left_bound[i] + 0.1);
    Results[i] = Derivatives[i]->Fit((TString)"myfunc" + (TString)to_string(i), "B0QS", "", left_bound[i], 5);

  }

  writeFile->cd();
  cd_correlated->cd();
  cd_projs = cd_correlated->mkdir("ErgDepLO");
  cd_projs->cd();
  TCanvas** c_Proj = new TCanvas*[projectionNum];

  for(int r = 0; r < projectionNum; ++r)
  {
    // prepare to draw fit
    TF1 fitDraw("fitDraw", "gaus", 0, 5);
    double mean = myfunc[r]->GetParameter(1);
    fitDraw.SetParameters(myfunc[r]->GetParameter(0), mean, myfunc[r]->GetParameter(2));

    final->SetPoint(r, ((startBin + r) * 0.1) - 0.05, mean);

    // draw projection
    c_Proj[r] = new TCanvas((TString)"Proj" + (TString)to_string(r), "Projection", 800, 400);
    c_Proj[r]->Divide(1, 3);

    c_Proj[r]->cd(1);
    Projections[r]->Draw();

    TLine* line = new TLine(mean, 0, mean, Projections[r]->GetMaximum());
    line->SetLineColor(kRed);
    line->Draw("SAME");

    c_Proj[r]->cd(2);
    Derivatives[r]->Draw();

    TLine* line2 = new TLine(left_bound[r], 0, left_bound[r], Derivatives[r]->GetMaximum());
    line2->SetLineColor(kBlack);
    line2->Draw("SAME");

    c_Proj[r]->Update();
    TPaveStats *stats = (TPaveStats*)Derivatives[r]->GetListOfFunctions()->FindObject("stats");
    stats->SetName("mystats");
    TList* list = stats->GetListOfLines();
    TLatex* mytext = new TLatex(0, 0, (TString)"Fit Mean = " + (TString)to_string(mean));
    mytext->SetTextFont(42);
    mytext->SetTextSize(0.03);
    list->Add(mytext);
    Derivatives[r]->SetStats(0);

    c_Proj[r]->Modified();
    c_Proj[r]->Update();

    fitDraw.Draw("SAME");

    c_Proj[r]->cd(3);
    Seconds[r]->Draw();

    c_Proj[r]->Write();
  }

  TCanvas* c_Final = new TCanvas("ErgDepLO", "Neutron Energy vs. Neutron LO", 800, 400);
  c_Final->cd();

  final->SetTitle("Neutron Energy vs. Neutron Light Out;Neutron Energy [MeV];Neutron Light Out [MeVee]");
  final->GetXaxis()->SetLimits(0, 5);
  final->GetYaxis()->SetRangeUser(0, 3);
  final->Draw();

  TF1* finalfit = new TF1("finalfit", "[0] * x + [1] * (1-exp([2] * pow(x, [3])))", 0, 5);
  finalfit->SetParameters(0.8, -2.5, -0.3, 1.001);
  finalfit->SetParLimits(0, 0.5, 1);
  finalfit->SetParLimits(1, -5, 0);
  // finalfit->FixParameter(0, 0.817);
  // finalfit->FixParameter(1, -2.63);
  // finalfit->FixParameter(2, -0.297);
  finalfit->FixParameter(3, 1.001);
  final->Fit("finalfit", "BQ");

  c_Final->Write();
}

void readFiss::AngCorr()
{
  // computing efficiency matrix
  for(int i = 1; i <= h_neutronSinglesExp->GetNbinsX(); ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      h2_neutronSinglesMat->SetBinContent(i, j, (h_neutronSinglesExp->GetBinContent(i) * h_neutronSinglesExp->GetBinContent(j)) / h_neutronSinglesExp->Integral());
    }
  }

    // gamma-gamma
  for(int i = 1; i <= h_photonSinglesExp->GetNbinsX(); ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      h2_photonSinglesMat->SetBinContent(i, j, (h_photonSinglesExp->GetBinContent(i) * h_photonSinglesExp->GetBinContent(j)) / h_photonSinglesExp->Integral());
    }
  }
    // n-gamma
  for(int i = 1; i <= h_neutronSinglesExp->GetNbinsX(); ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      h2_neutronPhotonSinglesMat->SetBinContent(i, j, (h_neutronSinglesExp->GetBinContent(i) * h_photonSinglesExp->GetBinContent(j)) / h_neutronSinglesExp->Integral());
    }
  }
    // n-n background
    for(int i = 1; i <= h_neutronSinglesBack->GetNbinsX(); ++i)
    {
      for(int j = 1; j < i; ++j)
      {
        h2_neutronBackSinglesMat->SetBinContent(i, j, (h_neutronSinglesBack->GetBinContent(i) * h_neutronSinglesBack->GetBinContent(j)) / h_neutronSinglesBack->Integral());
      }
    }
    // gamma-gamma background
  for(int i = 1; i <= h_photonSinglesBack->GetNbinsX(); ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      h2_photonBackSinglesMat->SetBinContent(i, j, (h_photonSinglesBack->GetBinContent(i) * h_photonSinglesBack->GetBinContent(j)) / h_photonSinglesBack->Integral());
    }
  }
    // n-gamma background
  for(int i = 1; i <= h_neutronSinglesBack->GetNbinsX(); ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      h2_neutronPhotonBackSinglesMat->SetBinContent(i, j, (h_neutronSinglesBack->GetBinContent(i) * h_photonSinglesBack->GetBinContent(j)) / h_neutronSinglesBack->Integral());
    }
  }


  // scale doubles matrix by eff matrix
  h2_neutronScaledDoubles = (TH2I*)h2_neutronDoublesMat->Clone();
  h2_neutronScaledDoubles->SetName("h2_neutronScaledDoubles");
  h2_neutronScaledDoubles->Divide(h2_neutronSinglesMat);

  h2_photonScaledDoubles = (TH2I*)h2_photonDoublesMat->Clone();
  h2_photonScaledDoubles->SetName("h2_photonScaledDoubles");
  h2_photonScaledDoubles->Divide(h2_photonSinglesMat);

  h2_neutronPhotonScaledDoubles = (TH2I*)h2_neutronPhotonDoublesMat->Clone();
  h2_neutronPhotonScaledDoubles->SetName("h2_neutronPhotonScaledDoubles");
  h2_neutronPhotonScaledDoubles->Divide(h2_neutronPhotonSinglesMat);

    // Background
  h2_neutronBackScaledDoubles = (TH2I*)h2_neutronBackDoublesMat->Clone();
  h2_neutronBackScaledDoubles->SetName("h2_neutronBackScaledDoubles");
  h2_neutronBackScaledDoubles->Divide(h2_neutronBackSinglesMat);

  h2_photonBackScaledDoubles = (TH2I*)h2_photonBackDoublesMat->Clone();
  h2_photonBackScaledDoubles->SetName("h2_photonBackScaledDoubles");
  h2_photonBackScaledDoubles->Divide(h2_photonBackSinglesMat);

  h2_neutronPhotonBackScaledDoubles = (TH2I*)h2_neutronPhotonBackDoublesMat->Clone();
  h2_neutronPhotonBackScaledDoubles->SetName("h2_neutronPhotonBackScaledDoubles");
  h2_neutronPhotonBackScaledDoubles->Divide(h2_neutronPhotonBackSinglesMat);

  // make arrays of angles and counts
  int arraySize = pow(NUM_DETECTORS, 2);
  double* a_angles = new double[arraySize];
  double* a_counts = new double[arraySize];
  double* a_errors = new double[arraySize];
  int n_points = 0;
  for(int i = 1; i <= NUM_DETECTORS; ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      if(h2_neutronScaledDoubles->GetBinContent(i, j) == 0)
      {
        continue;
      }

      a_angles[n_points] = angles[i-1][j-1];
      a_counts[n_points] = h2_neutronScaledDoubles->GetBinContent(i, j);
      a_errors[n_points] = h2_neutronScaledDoubles->GetBinError(i, j);
      n_points++;
    }
  }

    // gamma-gamma
  double* p_angles = new double[arraySize];
  double* p_counts = new double[arraySize];
  double* p_errors = new double[arraySize];
  int p_points = 0;
  for(int i = 1; i <= NUM_DETECTORS; ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      if(h2_photonScaledDoubles->GetBinContent(i, j) == 0)
      {
        continue;
      }

      p_angles[p_points] = angles[i-1][j-1];
      p_counts[p_points] = h2_photonScaledDoubles->GetBinContent(i, j);
      p_errors[p_points] = h2_photonScaledDoubles->GetBinError(i, j);
      p_points++;
    }
  }
    // n-gamma
  double* np_angles = new double[arraySize];
  double* np_counts = new double[arraySize];
  double* np_errors = new double[arraySize];
  int np_points = 0;
  for(int i = 1; i <= NUM_DETECTORS; ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      if(h2_neutronPhotonScaledDoubles->GetBinContent(i, j) == 0)
      {
        continue;
      }

      np_angles[np_points] = angles[i-1][j-1];
      np_counts[np_points] = h2_neutronPhotonScaledDoubles->GetBinContent(i, j);
      np_errors[np_points] = h2_neutronPhotonScaledDoubles->GetBinError(i, j);
      np_points++;
    }
  }
    // n-n background
  double* nB_angles = new double[arraySize];
  double* nB_counts = new double[arraySize];
  double* nB_errors = new double[arraySize];
  int nB_points = 0;
  for(int i = 1; i <= NUM_DETECTORS; ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      if(h2_neutronBackScaledDoubles->GetBinContent(i, j) == 0)
      {
        continue;
      }

      nB_angles[nB_points] = angles[i-1][j-1];
      nB_counts[nB_points] = h2_neutronBackScaledDoubles->GetBinContent(i, j);
      nB_errors[nB_points] = h2_neutronBackScaledDoubles->GetBinError(i, j);
      nB_points++;
    }
  }
    // gamma-gamma background
  double* pB_angles = new double[arraySize];
  double* pB_counts = new double[arraySize];
  double* pB_errors = new double[arraySize];
  int pB_points = 0;
  for(int i = 1; i <= NUM_DETECTORS; ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      if(h2_photonBackScaledDoubles->GetBinContent(i, j) == 0)
      {
        continue;
      }

      pB_angles[pB_points] = angles[i-1][j-1];
      pB_counts[pB_points] = h2_photonBackScaledDoubles->GetBinContent(i, j);
      pB_errors[pB_points] = h2_photonBackScaledDoubles->GetBinError(i, j);
      pB_points++;
    }
  }
    // n-gamma background
  double* npB_angles = new double[arraySize];
  double* npB_counts = new double[arraySize];
  double* npB_errors = new double[arraySize];
  int npB_points = 0;
  for(int i = 1; i <= NUM_DETECTORS; ++i)
  {
    for(int j = 1; j < i; ++j)
    {
      if(h2_neutronPhotonBackScaledDoubles->GetBinContent(i, j) == 0)
      {
        continue;
      }

      npB_angles[npB_points] = angles[i-1][j-1];
      npB_counts[npB_points] = h2_neutronPhotonBackScaledDoubles->GetBinContent(i, j);
      npB_errors[npB_points] = h2_neutronPhotonBackScaledDoubles->GetBinError(i, j);
      npB_points++;
    }
  }

  // make graphs
  g_neutronAngleCorr = new TGraphErrors(n_points, a_angles, a_counts, 0, a_errors);
  g_neutronAngleCorr->Sort();
  g_neutronAngleCorr->SetName("g_neutronAngleCorr");
  g_neutronAngleCorr->SetTitle("Neutron Double Angle Correlations;Cos T;Counts");
  g_neutronAngleCorr->GetXaxis()->SetLimits(-1, 1);
  //g_neutronAngleCorr->GetYaxis()->SetRangeUser(0, ?);

  g_photonAngleCorr = new TGraphErrors(p_points, p_angles, p_counts, 0, p_errors);
  g_photonAngleCorr->Sort();
  g_photonAngleCorr->SetName("g_photonAngleCorr");
  g_photonAngleCorr->SetTitle("Photon Double Angle Correlations;Cos T;Counts");
  g_photonAngleCorr->GetXaxis()->SetLimits(-1, 1);

  g_neutronPhotonAngleCorr = new TGraphErrors(np_points, np_angles, np_counts, 0, np_errors);
  g_neutronPhotonAngleCorr->Sort();
  g_neutronPhotonAngleCorr->SetName("g_neutronPhotonAngleCorr");
  g_neutronPhotonAngleCorr->SetTitle("Neutron Photon Angle Correlations;Cos T;Counts");
  g_neutronPhotonAngleCorr->GetXaxis()->SetLimits(-1, 1);

    // Background
  g_neutronBackAngleCorr = new TGraphErrors(nB_points, nB_angles, nB_counts, 0, nB_errors);
  g_neutronBackAngleCorr->Sort();
  g_neutronBackAngleCorr->SetName("g_neutronBackAngleCorr");
  g_neutronBackAngleCorr->SetTitle("Background Neutron Double Angle Correlations;Cos T;Counts");
  g_neutronBackAngleCorr->GetXaxis()->SetLimits(-1, 1);

  g_photonBackAngleCorr = new TGraphErrors(pB_points, pB_angles, pB_counts, 0, pB_errors);
  g_photonBackAngleCorr->Sort();
  g_photonBackAngleCorr->SetName("g_photonBackAngleCorr");
  g_photonBackAngleCorr->SetTitle("Background Photon Double Angle Correlations;Cos T;Counts");
  g_photonBackAngleCorr->GetXaxis()->SetLimits(-1, 1);

  g_neutronPhotonBackAngleCorr = new TGraphErrors(npB_points, npB_angles, npB_counts, 0, npB_errors);
  g_neutronPhotonBackAngleCorr->Sort();
  g_neutronPhotonBackAngleCorr->SetName("g_neutronPhotonBackAngleCorr");
  g_neutronPhotonBackAngleCorr->SetTitle("Background Neutron Photon Angle Correlations;Cos T;Counts");
  g_neutronPhotonBackAngleCorr->GetXaxis()->SetLimits(-1, 1);

  // make average lines
  int numAvgBins = 20;
  int numBin = 0;
  int numPoints = 0;
  double xAvg[n_points];
  double yAvg[n_points];

  g_neutronAngleCorrAvg = new TGraphErrors(numAvgBins);
  g_photonAngleCorrAvg = new TGraphErrors(numAvgBins);
  g_neutronPhotonAngleCorrAvg = new TGraphErrors(numAvgBins);

  g_neutronBackAngleCorrAvg = new TGraphErrors(numAvgBins);
  g_photonBackAngleCorrAvg = new TGraphErrors(numAvgBins);
  g_neutronPhotonBackAngleCorrAvg = new TGraphErrors(numAvgBins);


  for(int i = 0; i < n_points; ++i)
  {
    double rightBound = (2.0 / numAvgBins) * (numBin + 1);
    if(((g_neutronAngleCorr->GetPointX(i) + 1) >= rightBound) || i == n_points - 1)
    {
      g_neutronAngleCorrAvg->SetPoint(numBin, TMath::Mean(numPoints, xAvg), TMath::Mean(numPoints, yAvg));
      g_neutronAngleCorrAvg->SetPointError(numBin, TMath::StdDev(numPoints, xAvg), TMath::StdDev(numPoints, yAvg));

      for(int j = 0; j < numPoints; ++j)
      {
        xAvg[j] = 0;
        yAvg[j] = 0;
      }
      numPoints = 0;
      ++numBin;
    }
    xAvg[numPoints] = g_neutronAngleCorr->GetPointX(i);
    yAvg[numPoints] = g_neutronAngleCorr->GetPointY(i);
    ++numPoints;
  }

    // gamma-gamma
  numBin = 0;
  numPoints = 0;
  double xAvg_p[p_points];
  double yAvg_p[p_points];
  for(int i = 0; i < p_points; ++i)
  {
    double rightBound = (2.0 / numAvgBins) * (numBin + 1);
    if(((g_photonAngleCorr->GetPointX(i) + 1) >= rightBound) || i == p_points - 1)
    {
      g_photonAngleCorrAvg->SetPoint(numBin, TMath::Mean(numPoints, xAvg_p), TMath::Mean(numPoints, yAvg_p));
      g_photonAngleCorrAvg->SetPointError(numBin, TMath::StdDev(numPoints, xAvg_p), TMath::StdDev(numPoints, yAvg_p));

      for(int j = 0; j < numPoints; ++j)
      {
        xAvg_p[j] = 0;
        yAvg_p[j] = 0;
      }
      numPoints = 0;
      ++numBin;
    }
    xAvg_p[numPoints] = g_photonAngleCorr->GetPointX(i);
    yAvg_p[numPoints] = g_photonAngleCorr->GetPointY(i);
    ++numPoints;
  }
    // n-gamma
    numBin = 0;
    numPoints = 0;
    double xAvg_np[np_points];
    double yAvg_np[np_points];
    for(int i = 0; i < np_points; ++i)
    {
      double rightBound = (2.0 / numAvgBins) * (numBin + 1);
      if(((g_neutronPhotonAngleCorr->GetPointX(i) + 1) >= rightBound) || i == np_points - 1)
      {
        g_neutronPhotonAngleCorrAvg->SetPoint(numBin, TMath::Mean(numPoints, xAvg_np), TMath::Mean(numPoints, yAvg_np));
        g_neutronPhotonAngleCorrAvg->SetPointError(numBin, TMath::StdDev(numPoints, xAvg_np), TMath::StdDev(numPoints, yAvg_np));

        for(int j = 0; j < numPoints; ++j)
        {
          xAvg_np[j] = 0;
          yAvg_np[j] = 0;
        }
        numPoints = 0;
        ++numBin;
      }
      xAvg_np[numPoints] = g_neutronPhotonAngleCorr->GetPointX(i);
      yAvg_np[numPoints] = g_neutronPhotonAngleCorr->GetPointY(i);
      ++numPoints;
    }
    // n-n background
    numBin = 0;
    numPoints = 0;
    double xAvg_nB[nB_points];
    double yAvg_nB[nB_points];
    for(int i = 0; i < nB_points; ++i)
    {
      double rightBound = (2.0 / numAvgBins) * (numBin + 1);
      if(((g_neutronBackAngleCorr->GetPointX(i) + 1) >= rightBound) || i == nB_points - 1)
      {
        g_neutronBackAngleCorrAvg->SetPoint(numBin, TMath::Mean(numPoints, xAvg_nB), TMath::Mean(numPoints, yAvg_nB));
        g_neutronBackAngleCorrAvg->SetPointError(numBin, TMath::StdDev(numPoints, xAvg_nB), TMath::StdDev(numPoints, yAvg_nB));

        for(int j = 0; j < numPoints; ++j)
        {
          xAvg_nB[j] = 0;
          yAvg_nB[j] = 0;
        }
        numPoints = 0;
        ++numBin;
      }
      xAvg_nB[numPoints] = g_neutronBackAngleCorr->GetPointX(i);
      yAvg_nB[numPoints] = g_neutronBackAngleCorr->GetPointY(i);
      ++numPoints;
    }
    // gamma-gamma background
  numBin = 0;
  numPoints = 0;
  double xAvg_pB[pB_points];
  double yAvg_pB[pB_points];
  for(int i = 0; i < pB_points; ++i)
  {
    double rightBound = (2.0 / numAvgBins) * (numBin + 1);
    if(((g_photonBackAngleCorr->GetPointX(i) + 1) >= rightBound) || i == pB_points - 1)
    {
      g_photonBackAngleCorrAvg->SetPoint(numBin, TMath::Mean(numPoints, xAvg_pB), TMath::Mean(numPoints, yAvg_pB));
      g_photonBackAngleCorrAvg->SetPointError(numBin, TMath::StdDev(numPoints, xAvg_pB), TMath::StdDev(numPoints, yAvg_pB));

      for(int j = 0; j < numPoints; ++j)
      {
        xAvg_pB[j] = 0;
        yAvg_pB[j] = 0;
      }
      numPoints = 0;
      ++numBin;
    }
    xAvg_pB[numPoints] = g_photonBackAngleCorr->GetPointX(i);
    yAvg_pB[numPoints] = g_photonBackAngleCorr->GetPointY(i);
    ++numPoints;
  }
    // n-gamma background
  numBin = 0;
  numPoints = 0;
  double xAvg_npB[npB_points];
  double yAvg_npB[npB_points];
  for(int i = 0; i < npB_points; ++i)
  {
    double rightBound = (2.0 / numAvgBins) * (numBin + 1);
    if(((g_neutronPhotonBackAngleCorr->GetPointX(i) + 1) >= rightBound) || i == npB_points - 1)
    {
      g_neutronPhotonBackAngleCorrAvg->SetPoint(numBin, TMath::Mean(numPoints, xAvg_npB), TMath::Mean(numPoints, yAvg_npB));
      g_neutronPhotonBackAngleCorrAvg->SetPointError(numBin, TMath::StdDev(numPoints, xAvg_npB), TMath::StdDev(numPoints, yAvg_npB));

      for(int j = 0; j < numPoints; ++j)
      {
        xAvg_npB[j] = 0;
        yAvg_npB[j] = 0;
      }
      numPoints = 0;
      ++numBin;
    }
    xAvg_npB[numPoints] = g_neutronPhotonBackAngleCorr->GetPointX(i);
    yAvg_npB[numPoints] = g_neutronPhotonBackAngleCorr->GetPointY(i);
    ++numPoints;
  }

  cd_AngleCorr = cd_correlated->mkdir("AngleCorr");
}
