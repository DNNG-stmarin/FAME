// copies psderg canvas from a root file and replicates it in another file 

{
// constants
int numDetectors = 10;
//- Create a TFile to store the output
TFile* readFile = new TFile("~/Downloads/Detectors.root","READ");

// tests that the correct file has been grabbed by printing out the size of the file
//cout << readFile->GetSize() << endl;

// Creates a TFile to store the output
TFile* outFile = new TFile("~/Documents/Research/ROOTFAMEwork/psdErgDiscrim.root", "RECREATE");

// defines varibles for the file name of the canvases being copied
TString fileNameBase = "PSD/PSDErg_discrimination/psdErg";
TString fileName;

// loops through all of the detectors
for(int det = 0; det < numDetectors; ++det){
    // create a canvas to store the data (double)
    TCanvas* c_psdErg;
    TF1* f_psdDisc;

    // sets the file name to the base name and the detector 
    fileName =  fileNameBase + (TString)to_string(det);
    //sets the canvas equal to the psdErg one for that detector 
    c_psdErg = (TCanvas*)readFile->Get(fileName);

    f_psdDisc = (TF1*)c_psdErg->GetPrimitive("psdDisc");
    //cout << f_psdDisc << endl;
    cout << f_psdDisc->GetParameter(2) << endl;



    //get info about something in the plot and change it 
    //grab a canvas object or graph object from the plot 
    //should be able to edit it and draw it back onto the canvas before saving it

    // writes the grabbed and edited canvas to the new file
    outFile->cd();
    c_psdErg->Write();
    //f_psdDisc->Write();
}

}




