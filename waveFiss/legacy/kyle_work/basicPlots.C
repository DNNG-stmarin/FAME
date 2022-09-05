using namespace std;

void BinLogX(TH1 *h){
  TAxis *axis = h->GetXaxis();
  int bins = axis->GetNbins();
  Axis_t from = axis->GetXmin();
  Axis_t to = axis->GetXmax();
  Axis_t width = (to-from)/bins;
  Axis_t *new_bins = new Axis_t[bins+1];

  for (int i=0; i<=bins; i++){
    new_bins[i]=TMath::Power(10, from+i*width);
  }
  axis->Set(bins,new_bins);
//  delete new_bins;
}

void basicPlots(const char* infile_list = "NpNoFilter.txt"){
//void basicPlots(const char* infile_list = "NpCarbonFilter.txt"){

//Construct TChain---------------------------------------------------
  TChain* et = new TChain("et");

  ifstream fileList(infile_list);
  cout << "Name of evt.root file list: " << infile_list << endl;

  string rootFileName;

  if (fileList.is_open()){
    cout << "file list opened" << endl;
    Int_t control = 0;

    while (!fileList.eof()){
      getline(fileList,rootFileName);
      if (!fileList.eof()){
        cout << "reading file: " << rootFileName.c_str() << endl;
        et->Add(rootFileName.c_str());
      }
      else fileList.close();
    }
  }
  else cout << "bad file list" << endl;

//Initialize variables from tree--------------------------------------

  Double_t dt=0;
  Double_t cph=0;
  Double_t aph[2]={0,0};
  Double_t gph[2]={0,0};
  Double_t nToF;
  Double_t neutronEnergy;

  Double_t massOfNeutron = 939.56563; //MeV/c2
  Double_t speedOfLight = 0.299792458; //m/ns
  Double_t flightPathLength = 11.86; //m

  et->SetBranchAddress("dt",&dt);
  et->SetBranchAddress("cph",&cph);
  et->SetBranchAddress("aph[2]",aph);
  et->SetBranchAddress("gph[2]",gph);

//Define histograms---------------------------------------------------

  TH1D* hAPH0 = new TH1D("hAPH0","Anode Pulse Height",200,0,1000);
  TH1D* hAPH1 = new TH1D("hAPH1","Anode Pulse Height",200,0,1000);
  TH1D* hGPH0 = new TH1D("hGPH0","Grid Pulse Height",300,0,30000);
  TH1D* hGPH1 = new TH1D("hGPH1","Grid Pulse Height",300,0,30000);

  TH1D* hCPH = new TH1D("hCPH","Cathode Pulse Height",500,0,50000);
  TH1D* hTOF = new TH1D("hTOF","Time of Flight (ns)",8000,0,2000);
  TH1D* hEn = new TH1D("hEn","Neutron energy (MeV)",4000,-1,3);
  BinLogX(hEn);

  TH2D* hAA = new TH2D("hAA","Anode versus Anode", 200,0,1000,200,0,1000);
  TH2D* hECosTheta0 = new TH2D("hECosTheta0","Energy vs x/dcos(theta)(raw)",200,0,80,200,0,800);
  TH2D* hECosTheta1 = new TH2D("hECosTheta1","Energy vs x/dcos(theta)(raw)",200,0,80,200,0,800);

  TH1D* proj0[25];
  TH1D* proj1[25];

  unsigned int NumOfEntries = et->GetEntries();

  for (unsigned int i=0;i<NumOfEntries; i++){
    et->GetEntry(i);
    time_t curr=time(0);
    if (i%1000000==0) cout << "Event#: " << i << endl << "Time: " << ctime(&curr) << endl;

    if (aph[0]<-50 && cph<-20000){
      hAPH0->Fill(fabs(aph[0]));
      hAPH1->Fill(fabs(aph[1]));
      hGPH0->Fill(gph[0]);
      hGPH1->Fill(gph[1]);
      hCPH->Fill(fabs(cph));

      nToF =fmod(100000.*(dt-1150.-49.7062),(100000*1788.819875776))/100000.; //ns
//      nToF =fmod(100000.*(dt-1150.-49.78),(100000*1788.819875776))/100000.; //ns
      hTOF->Fill(nToF); //ns

      if (nToF>42.){
        neutronEnergy = massOfNeutron*(1./sqrt(1.-pow(flightPathLength/nToF/speedOfLight,2))-1.);
        hEn->Fill(neutronEnergy);
      }

      hECosTheta0->Fill(gph[0]/fabs(aph[0]),fabs(aph[0]));
      hECosTheta1->Fill(gph[1]/fabs(aph[1]),fabs(aph[1]));

      hAA->Fill(fabs(aph[0]),fabs(aph[1]));
    }
  }

  TFile *f = new TFile("basicPlots.root","RECREATE");
  if (f->IsOpen()) printf("File opened successfully\n");
  hECosTheta0->Write("hECosTheta0_1");
  hECosTheta1->Write("hECosTheta1_1");
  f->Close();
}
