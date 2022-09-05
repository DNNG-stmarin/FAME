// copies a canvas from a root file and replicates it in another file 



{
//- Create a TFile to store the output

TFile* readFile = new TFile("~/Downloads/Detectors.root","READ");

cout << readFile->GetSize() << endl;

// Creates a TFile to store the output
TFile* outFile = new TFile("~/Documents/Research/ROOTFAMEwork/psdErgSlice.root", "RECREATE");

// create a canvas to store the data (double)
TCanvas* c_psdSlice;
//sets the canvas equal to psd0Proj1
c_psdSlice = (TCanvas*)readFile->Get("PSD/PSD_slices/psd0Proj1");

//get info about something in the plot and change it 

// writes the canvas to the new file
outFile->cd();
c_psdSlice->Write();

}




