#include "readFiss.h"
using namespace std;

// read the information contained in the beam file
void readFiss::ReadBeamInfo()
{
  cout << "Extracting information from file" << endl;

  beamFile = new TFile((TString)nameBeam, "READ");
  if(!beamFile->IsOpen())
  {
    w->noBeamFile();
  }
  cd_beam->cd();

  // extract the histrograms from the file
  TH2D* alphaSpecAll = (TH2D*)beamFile->Get("alphaChanSpec");
	TH1I* h_macroPopFile = (TH1I*)beamFile->Get("macroPop");
	TH1D* h_fisDepFile = (TH1D*)beamFile->Get("fisSpec");

  // loop through the PPAC channels
  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
		int TRIGGER_CHANNEL = TRIGGERS[r];
    // cout << TRIGGER_CHANNEL << endl;
    h_alphaDep[r] = alphaSpecAll->ProjectionY((TString)"h_alphaDep_" + (TString)to_string(r), TRIGGER_CHANNEL + 1, TRIGGER_CHANNEL + 1);
		h_alphaDep[r]->Scale(1.0 / intWindowAlpha);		//Changing counts into count rate for alpha background
    h_alphaDep[r]->Scale(expTree->GetEntries() / h_fisDepFile->GetSum()); // Scale count rate by file size
    // h_alphaDep[r]->Write();
  }
  h_macroPop = (TH1I*)h_macroPopFile->Clone();
  cout << "Read in MacroStructure population, nentries = " << h_macroPop->GetEntries() << endl;
  //h_macroPop->Write();
  beamFile->Close();

}
