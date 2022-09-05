#include "readFiss.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// set the the neutron cut thresholds
void readFiss::SetRunThresholds(double threshold, double max_time_n)
{

   THRESHOLD = threshold;
   MAX_TIME_N = max_time_n;

   cout << "Using: threshold = " << THRESHOLD << " MeVee, " << " Tmax = " << MAX_TIME_N << " ns." << endl;
}

// set the coarseness of the covem analysis
void readFiss::SetBNBP(int bn, int bp)
{

   BN = bn;
   BP = bp;

   cout << "Using BN = " << BN << " and BP = " << BP << ". \n";
}

// set the energy limits
void readFiss::SetEnergyLimits(double min_n_erg, double max_n_erg, double min_p_erg, double max_p_erg)
{
  MIN_N_ERG = min_n_erg;
  MAX_N_ERG = max_n_erg;

  MIN_P_ERG = min_p_erg;
  MAX_P_ERG = max_p_erg;

  cout << "Using neutron energy range " << min_n_erg << " to " << max_n_erg <<
          " and photon energy range " << min_p_erg << " to " << max_p_erg << endl;
}
