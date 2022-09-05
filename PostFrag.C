#include "fragFiss.h"
#include "constants.h"

#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>


void fragFiss::PostAngle()
{
  TTree* cloneFrag = (TTree*)fragTree->Clone();
  // InitFrag();

  Long64_t nentries = cloneFrag->GetEntries();

  cloneFrag->Show(10);

  for (Long64_t jentry = cumulatEntries; jentry > cumulatEntries - nentriesTree[fil]; jentry--)
  {


  }

}
