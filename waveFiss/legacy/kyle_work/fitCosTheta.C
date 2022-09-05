using namespace std;


double errorFunction(double *x, double *par){
  return par[2]*0.5*(1.+erf(-(x[0]-par[0])/par[1]));
  //  return *0.5*(1.+erf(-(x[0]-37.)/3.));
}


void fitCosTheta(){

  //  TF1* fitfunc2 = new TF1("fitfunc2",errorFunction,20.,40.,0);

  //  fitfunc2->Draw();

  //Define histograms---------------------------------------------------

  TH2D* hECosTheta0 = new TH2D("hECosTheta0","Energy vs x/dcos(theta)(raw)",200,0,80,200,0,800);
  TH2D* hECosTheta1 = new TH2D("hECosTheta1","Energy vs x/dcos(theta)(raw)",200,0,80,200,0,800);

  TH1D* proj0[25];
  TH1D* proj1[25];

  TFile *f = new TFile("./basicPlots.root","READ");

  gDirectory->GetObject("hECosTheta0_1",hECosTheta0);
  gDirectory->GetObject("hECosTheta1_1",hECosTheta1);

  double lowerBound,upperBound;

  double edge0[25],edge1[25],bin[25];

  for(Int_t i=0;i<25;i++){
    //    Int_t i=10;
    proj0[i] = hECosTheta0->ProjectionX(Form("proj0%d",i),25+i*5,25+(i+1)*5);
    proj1[i] = hECosTheta1->ProjectionX(Form("proj1%d",i),25+i*5,25+(i+1)*5);

    bin[i]=25.+((double)i+0.5)*5.;

    lowerBound = ((double)i)*1.4+15.;
    upperBound = lowerBound+20.;

    TF1* fitfunc0 = new TF1("fitfunc0",errorFunction,lowerBound,upperBound,3);
    fitfunc0->SetParameters(40.,3.,330.);
    proj0[i]->Fit("fitfunc0","R");
    edge0[i]=fitfunc0->GetParameter(0);

    TF1* fitfunc1 = new TF1("fitfunc1",errorFunction,lowerBound,upperBound,3);
    fitfunc1->SetParameters(37.,3.,546.);
    proj1[i]->Fit("fitfunc1","R");
    edge1[i]=fitfunc1->GetParameter(0);
  }

  TGraph* edges0 = new TGraph(25,edge0,bin);
  TGraph* edges1 = new TGraph(25,edge1,bin);
  edges0->Draw("H");
}
