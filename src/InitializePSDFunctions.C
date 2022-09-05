/*
Author: Stefano Marin
Purpose: Initialize the psd functions used in the analysis of the data
Date: Ann Arbor, June 1st, 2020
*/
#define InitializePSDFunctions_cxx
#include "DetectorSystemClass.h"
#include "ProcessingConstants.h"
void DetectorSystemClass::InitializePSDFunctions()
{
  // define the fitting function for the psd discrimination
  TF1* gaussian = new TF1("gausPSD", "[0]/(1 + ((x - [1])/([2]))^2)", MINPSD_FIT, MAXPSD_FIT);
  TF1* cauchy = new TF1("cauchyPSD", "[0]*e^(-(x - [1])^2/(2*[2]^2))", MINPSD_FIT, MAXPSD_FIT);

  // fitting functions for the two
  fitPSD_p = new TF1("fitPSDp", "gausPSD");
  fitPSD_n = new TF1("fitPSDn", "gausPSD");


  // double fitting
  fitPSD = new TF1("fitPSDnp", "fitPSDn + fitPSDp");
  intersection = new TF1("intersect", "abs(fitPSDn - fitPSDp)");

  // line colors of the fits
  fitPSD->SetLineColor(kRed);
  intersection->SetLineColor(kBlue);
  fitPSD_p->SetLineColor(kGreen);
  fitPSD_n->SetLineColor(kGreen);

  // parameters
  fitPSD->SetParNames("AP", "mP", "sP", "AN", "mN", "sN");
  fitPSD_n->SetParNames("AN", "mN", "sN");
  fitPSD_n->SetParNames("AP", "mP", "sP");
  intersection->SetParNames("AP", "mP", "sP", "AN", "mN", "sN");

  // guesses for the limit of the PSD distribution
  fitPSD->SetParLimits(1, MINPSD_FIT, DIVPSD_FIT);
  fitPSD->SetParLimits(4, DIVPSD_FIT, MAXPSD_FIT);

  // fitPSD->SetParLimits(5, 0.01, 0.05);
}
