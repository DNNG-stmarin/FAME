#include "readFiss.h"
using namespace std;
void readFiss::InitializeFunctions()
{
  cout << "initializing functions" << endl;
  f_alpha = new TF1* [NUM_TRIGGERS];
  f_expo = new TF1* [NUM_TRIGGERS];
  f_fisProducts = new TF1* [NUM_TRIGGERS];
  f_gauss = new TF1* [NUM_TRIGGERS];
  f_aveGmult = new TF1* [NUM_TRIGGERS];
  f_aveNmult = new TF1* [NUM_TRIGGERS];
  f_meanGammaLOErg = new TF1* [NUM_TRIGGERS];

  for(int indexChannel = 0; indexChannel < NUM_TRIGGERS; indexChannel++)
  {
    f_alpha[indexChannel]  = new TF1((TString)"f_alpha" + (TString)to_string(indexChannel), "expo", DEP_MIN, DEP_MAX);
    f_expo[indexChannel]  = new TF1((TString)"f_expo" + (TString)to_string(indexChannel), "expo");
    f_fisProducts[indexChannel]  = new TF1((TString)"f_fisProducts" + (TString)to_string(indexChannel), "gaus", FIS_MIN, FIS_MAX);
    f_gauss[indexChannel]  = new TF1((TString)"f_gauss" + (TString)to_string(indexChannel), "gaus");
    f_aveNmult[indexChannel] = new TF1((TString)"f_aveNmult" + (TString)to_string(indexChannel), "pol1");
    f_aveGmult[indexChannel] = new TF1((TString)"f_aveGmult" + (TString)to_string(indexChannel), "pol1");
    f_meanGammaLOErg[indexChannel] = new TF1((TString)"f_meanGammaLOErg" + (TString)to_string(indexChannel), "pol1");
  }

}
