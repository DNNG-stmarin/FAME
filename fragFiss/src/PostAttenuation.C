
#include "fragFiss.h"
#include "constants.h"

#include <TH2.h>
#include <TF1.h>
#include <TH3.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TFitResult.h>
#include <TFitResultPtr.h>


void fragFiss::PostAttenuation(int iterationPost)
{
  TH2D* h2_massAttU1 = new TH2D("h2_massAttU1_" + TString(to_string(iterationPost)),"h2_massAttU1;attenuation;kinetic energy",N_BINS_RATIO,-1, 5,N_BINS_APH, 0, MAX_APH);
  TH2D* h2_massAttU2 = new TH2D("h2_massAttU2_" + TString(to_string(iterationPost)),"h2_massAttU2;attenuation;kinetic energy",N_BINS_RATIO,-1, 5,N_BINS_APH, 0, MAX_APH);

  TH2D* h2_massAtt1 = new TH2D("h2_massAtt1_" + TString(to_string(iterationPost)),"h2_massAtt1;attenuation;kinetic energy",N_BINS_RATIO,-1, 5,N_BINS_APH, 0, MAX_APH);
  TH2D* h2_massAtt2 = new TH2D("h2_massAtt2_" + TString(to_string(iterationPost)),"h2_massAtt2;attenuation;kinetic energy",N_BINS_RATIO,-1, 5,N_BINS_APH, 0, MAX_APH);

  TH3D* h3_massKEAtt1 = new TH3D("h3_massKEAtt1_"+ TString(to_string(iterationPost)),"h3_massKEAtt1; mass;  1/angle; kinetic energy", N_BINS_MASS_TH, MIN_MASS, MAX_MASS, N_BINS_RATIO,-1, 5, N_BINS_APH,0,MAX_APH);
  TH3D* h3_massKEAtt2 = new TH3D("h3_massKEAtt2_"+ TString(to_string(iterationPost)),"h3_massKEAtt2; mass; 1/angle; kinetic energy", N_BINS_MASS_TH, MIN_MASS, MAX_MASS, N_BINS_RATIO,-1, 5, N_BINS_APH,0,MAX_APH);

  cout << "Filling mass attenuation histograms" << endl;
  Long64_t nentries = fragTree->GetEntries();

  double pThetaAv;

  for (Long64_t jentry = 0; jentry < nentries; jentry++)
  {
     Long64_t ientry = fragTree->LoadTree(jentry);
     if(ientry < 0) break; // reached the end of the file

     fragTree->GetEntry(jentry);

     if( !((pAn1 > MIN_ANODE1) && (pAn2 > MIN_ANODE2) && (pTheta1 > MIN_ANG1) && (pTheta2 > MIN_ANG2)) ) continue;
     if(iterationPost == infoSystem->NUM_RECURSIONS - 1)
     {
       if(pA1 < MIN_MASS_ANALYSIS || pA1 > MAX_MASS_ANALYSIS || pA2 < MIN_MASS_ANALYSIS || pA2 > MAX_MASS_ANALYSIS) continue;
       if(pTheta1 < MIN_ANG_ANALYSIS || pTheta1 > MAX_ANG_ANALYSIS || pTheta2 < MIN_ANG_ANALYSIS || pTheta2 > MAX_ANG_ANALYSIS) continue;
     }
     else
     {
       if(pA1 < MIN_MASS_RECURSION|| pA1 > MAX_MASS_RECURSION || pA2 < MIN_MASS_RECURSION || pA2 > MAX_MASS_RECURSION) continue;
       if(pTheta1 < MIN_ANG_RECURSION || pTheta1 > MAX_ANG_RECURSION || pTheta2 < MIN_ANG_RECURSION || pTheta2 > MAX_ANG_RECURSION) continue;
     }

     // grid inefficiency corrections
     pAn1 = (pAn1 - GRID_INEFFICIENCY*(pAn1 + pGr1))/(1 - GRID_INEFFICIENCY);
     pAn2 = (pAn2 - GRID_INEFFICIENCY*(pAn2 + pGr2))/(1 - GRID_INEFFICIENCY);

     h2_massAttU1->Fill(1.0/pTheta1, pAn1);
     h2_massAttU2->Fill(1.0/pTheta2, pAn2);

     pTheta1 /= g_AngMass1->Eval(pA1);
     pTheta2 /= g_AngMass2->Eval(pA2);

     pThetaAv = 0.5*(pTheta1 + pTheta2);

     // pTheta1 = pThetaAv;
     // pTheta2 = pThetaAv;

     h2_massAtt1->Fill(1.0/pTheta1, pAn1);
     h2_massAtt2->Fill(1.0/pTheta2, pAn2);

     h3_massKEAtt1->Fill(pA1, 1.0/pTheta1, pAn1);
     h3_massKEAtt2->Fill(pA2, 1.0/pTheta2, pAn2);

  }


  // attenuation surfaces

  cout << "declaring attenuation surfaces with dimension " << N_BINS_MASS_TH*N_BINS_MASS_TH << endl;
  g2_massAttSurf1[iterationPost] = new TGraph2D(N_BINS_MASS_TH*N_BINS_MASS_TH);
  g2_massAttSurf2[iterationPost] = new TGraph2D(N_BINS_MASS_TH*N_BINS_MASS_TH);
  g2_massAttSurf1[iterationPost]->SetName("g2_massAttSurf1_" + (TString)to_string(iterationPost));
  g2_massAttSurf2[iterationPost]->SetName("g2_massAttSurf2_" + (TString)to_string(iterationPost));

  cout << "creating profiles for standard analysis" << endl;

  // find the profiles
  TProfile* p1_massBacking1 = h2_massAtt1->ProfileX();
  TProfile* p1_massBacking2 = h2_massAtt2->ProfileX();

  TProfile* p1_massBackingU1 = h2_massAttU1->ProfileX();
  TProfile* p1_massBackingU2 = h2_massAttU2->ProfileX();

  // fits mass ind
  f_massAtt1 = new TF1("f_massAtt1", "[0] + [1]*x", -10, 10);
  f_massAtt2 = new TF1("f_massAtt2", "[0] + [1]*x", -10, 10);

  p1_massBacking1->Fit(f_massAtt1, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);
  p1_massBacking2->Fit(f_massAtt2, "Q", "", MIN_INV_ANG_FIT, MAX_INV_ANG_FIT);

  // fits mass dep
  TFitResultPtr depMass1 = 0;
  TFitResultPtr depMass2 = 0;

  TF1* f_massAttProj1 = new TF1("f_massAttProj1", "[0] + [1]*x", -10, 10);
  TF1* f_massAttProj2 = new TF1("f_massAttProj2", "[0] + [1]*x", -10, 10);


  double *interpMass1, *slopeMass1;
  double *interpMass2, *slopeMass2;
  interpMass1 = new double[N_BINS_MASS_TH];
  interpMass2 = new double[N_BINS_MASS_TH];
  slopeMass1 = new double[N_BINS_MASS_TH];
  slopeMass2 = new double[N_BINS_MASS_TH];
  double  *massCenters1, *massCenters2;
  massCenters1 = new double[N_BINS_MASS_TH];
  massCenters2 = new double[N_BINS_MASS_TH];


  TProfile** p1_massAtt1;
  TProfile** p1_massAtt2;

  p1_massAtt1 = new TProfile*[N_BINS_MASS_TH];
  p1_massAtt2 = new TProfile*[N_BINS_MASS_TH];

  int numMass1 = 0;
  int numMass2 = 0;

  int surfPoint1 = 0;

  int minBinFit, maxBinFit;

  for(int m = 1; m <= N_BINS_MASS_TH; m++)
  {

    h3_massKEAtt1->GetXaxis()->SetRange(m,m);

    TH2D* h2_mKEAtt1  = ((TH2D*)h3_massKEAtt1->Project3D("zy"));

    minBinFit = h2_mKEAtt1->GetXaxis()->FindBin(MIN_INV_ANG_FIT_TIP);
    maxBinFit = h2_mKEAtt1->GetXaxis()->FindBin(MAX_INV_ANG_FIT_TIP);


    if(h2_mKEAtt1->Integral(minBinFit, maxBinFit, 1,-1) < MIN_COUNT_MASS_ATT) continue;
    cout << h2_mKEAtt1->GetEntries() << " ";

    TProfile* p1_temp1 = h2_mKEAtt1->ProfileX();

    p1_massAtt1[numMass1] = (TProfile*)p1_temp1->Clone();
    p1_massAtt1[numMass1]->SetName("p1_massAtt1_" + (TString)to_string(m));

    depMass1 = p1_temp1->Fit(f_massAttProj1, "Q S", "", MIN_INV_ANG_FIT_TIP, MAX_INV_ANG_FIT_TIP);

    massCenters1[numMass1] = h3_massKEAtt1->GetXaxis()->GetBinCenter(m);
    interpMass1[numMass1] = depMass1->Parameter(0);
    slopeMass1[numMass1] = depMass1->Parameter(1);

    for(int anI = 1; anI <= N_BINS_RATIO; anI++)
    {
      // cout << surfPoint1 << " " <<  h3_massKEAtt1->GetXaxis()->GetBinCenter(m) << " " << p1_massAtt1[numMass1]->GetBinCenter(anI) << " " << p1_massAtt1[numMass1]->GetBinContent(anI) << endl;
      g2_massAttSurf1[iterationPost]->SetPoint(surfPoint1, h3_massKEAtt1->GetXaxis()->GetBinCenter(m), p1_massAtt1[numMass1]->GetBinCenter(anI), p1_massAtt1[numMass1]->GetBinContent(anI));

      surfPoint1++;
    }

    numMass1++;
  }
  h3_massKEAtt1->GetXaxis()->SetRange(1,N_BINS_MASS_TH);
  cout << endl;

  int surfPoint2 = 0;

  for(int m = 1; m <= N_BINS_MASS_TH; m++)
  {

    h3_massKEAtt2->GetXaxis()->SetRange(m,m);

    TH2D* h2_mKEAtt2  = ((TH2D*)h3_massKEAtt2->Project3D("zy"));
    minBinFit = h2_mKEAtt2->GetXaxis()->FindBin(MIN_INV_ANG_FIT_TIP);
    maxBinFit = h2_mKEAtt2->GetXaxis()->FindBin(MAX_INV_ANG_FIT_TIP);


    if(h2_mKEAtt2->Integral(minBinFit, maxBinFit, 1,-1) < MIN_COUNT_MASS_ATT) continue;
    cout << h2_mKEAtt2->GetEntries() << " ";

    TProfile* p1_temp2 = h2_mKEAtt2->ProfileX();

    p1_massAtt2[numMass2] = (TProfile*)p1_temp2->Clone();
    p1_massAtt2[numMass2]->SetName("p1_massAtt2_" + (TString)to_string(m));

    depMass2 = p1_temp2->Fit(f_massAttProj2, "Q S", "", MIN_INV_ANG_FIT_TIP, MAX_INV_ANG_FIT_TIP);

    massCenters2[numMass2] = h3_massKEAtt2->GetXaxis()->GetBinCenter(m);
    interpMass2[numMass2] = depMass2->Parameter(0);
    slopeMass2[numMass2] = depMass2->Parameter(1);

    for(int anI = 1; anI <= N_BINS_RATIO; anI++)
    {

      g2_massAttSurf2[iterationPost]->SetPoint(surfPoint2, h3_massKEAtt2->GetXaxis()->GetBinCenter(m), p1_massAtt2[numMass2]->GetBinCenter(anI), p1_massAtt2[numMass2]->GetBinContent(anI));

      surfPoint2++;
    }

    numMass2++;
  }
  h3_massKEAtt2->GetXaxis()->SetRange(1,N_BINS_MASS_TH);
  cout << endl;


  g_interpMass1 = new TGraph(numMass1, massCenters1, interpMass1);
  g_interpMass2 = new TGraph(numMass2, massCenters2, interpMass2);
  g_slopeMass1 = new TGraph(numMass1, massCenters1, slopeMass1);
  g_slopeMass2 = new TGraph(numMass2, massCenters2, slopeMass2);

  g_interpMass1->SetName("g_interpMass1" + (TString)to_string(iterationPost));
  g_interpMass2->SetName("g_interpMass2" + (TString)to_string(iterationPost));
  g_slopeMass1->SetName("g_slopeMass1" + (TString)to_string(iterationPost));
  g_slopeMass2->SetName("g_slopeMass2" + (TString)to_string(iterationPost));

  // // analysis of fits
  // // ask fredrik about this
  // double crossPoint = (f_att2->GetParameter(0) - f_att1->GetParameter(0))/(f_att1->GetParameter(1) - f_att2->GetParameter(1));
  // double heightPoint = f_att1->Eval(crossPoint);
  //
  // cout << "The intersect point is " << crossPoint << " and the height is " << heightPoint << endl;

  // canvas with angle results
  TCanvas* c_massAtt = new TCanvas("c_massAtt_" + TString(to_string(iterationPost)), "c_massAtt", 400, 500);
  c_massAtt->Divide(3,1);

  c_massAtt->cd(1);
  p1_massBacking1->Draw();
  p1_massBacking1->SetName("p1_massBacking1");
  p1_massBacking1->SetLineColor(kBlue);
  f_massAtt1->Draw("SAME");
  p1_massBackingU1->SetLineColor(kBlue);
  p1_massBackingU1->SetLineStyle(kDashed);
  p1_massBackingU1->Draw("SAME");

  p1_massBacking2->Draw("SAME");
  p1_massBacking2->SetName("p1_massBacking2");
  p1_massBacking2->SetLineColor(kRed);
  f_massAtt2->Draw("SAME");
  p1_massBackingU2->SetLineColor(kRed);
  p1_massBackingU2->SetLineStyle(kDashed);
  p1_massBackingU2->Draw("SAME");

  c_massAtt->cd(2);
  g_interpMass1->SetLineColor(kBlue);
  g_interpMass1->Draw();
  g_interpMass2->SetLineColor(kRed);
  g_interpMass2->Draw("SAME");

  c_massAtt->cd(3);
  g_slopeMass1->SetLineColor(kBlue);
  g_slopeMass1->Draw();
  g_slopeMass2->SetLineColor(kRed);
  g_slopeMass2->Draw("SAME");

  TCanvas* c_massAttSing = new TCanvas("c_massAttSing_" + TString(to_string(iterationPost)), "c_massAttSing", 400, 500);
  c_massAttSing->Divide(5,2);

  int jPan = 0;
  int canPan = 1;
  while(jPan < numMass1 & jPan < numMass2)
  {
    c_massAttSing->cd(canPan);
    p1_massAtt1[jPan]->Draw();
    p1_massAtt2[jPan]->Draw("SAME");
    jPan += 5;
    canPan++;
  }

  // TGraph2D *g2_massAttSurf1C, *g2_massAttSurf2C;
  //
  // g2_massAttSurf1C = (TGraph2D*)g2_massAttSurf1->Clone();
  // g2_massAttSurf2C = (TGraph2D*)g2_massAttSurf2->Clone();
  // g2_massAttSurf1C->SetName("g2_massAttSurf1C_" + (TString)to_string(iterationPost));
  // g2_massAttSurf2C->SetName("g2_massAttSurf2C_" + (TString)to_string(iterationPost));



  fragDiagnostics->cd();
  cd_recursion->cd();
  c_massAtt->Write();

  c_massAttSing->Write();

  h3_massKEAtt1->Write();
  h3_massKEAtt2->Write();

  g2_massAttSurf1[iterationPost]->Write();
  g2_massAttSurf2[iterationPost]->Write();





}
