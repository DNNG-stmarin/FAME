#include "readFiss.h"
#include <TLegend.h>
#include <TCanvas.h>
#include <TRatioPlot.h>
#include <TPaveStats.h>
#include <TText.h>
#include <TLatex.h>

#include <iostream>
#include <fstream>
#include <sstream>

/*
  ___                   _               _
 | __|_ ___ __  ___ _ _(_)_ __  ___ _ _| |_
 | _|\ \ / '_ \/ -_) '_| | '  \/ -_) ' \  _|
 |___/_\_\ .__/\___|_| |_|_|_|_\___|_||_\__|
         |_|
*/
using namespace std;

void readFiss::PlotAll()
{
  // trigger
  PlotTrigger();

  // detector
  PlotLightOut();
  PlotTof();
  PlotErg();
  PlotMult();
  PlotPSD();
  PlotSingles();

  PlotIndiv();
}

void readFiss::PlotCorr()
{
  PlotMultCorExp();
  PlotMultCorBack();
  PlotMultLOExp();
  PlotEnergyLOExp();
  PlotN_LOPSD_Exp();
  PlotP_LOPSD_Exp();
  // PlotN_AngleCorr(); // buggy
}

void readFiss::CompareAll()
{
  cout << "Beginning comparison with simulation. Exp/Sim = " << ((double)expEntries)/((double)simEntries) << ". " << endl;

  CompareLightOut();
  CompareTof();
  CompareErg();
  CompareMult();
  ComparePSD();
  CompareSingles();
}

void readFiss::PlotLightOut()
{
    writeFile->cd();
    cd_basics->cd();
    cout << "Plotting Light Output." << endl;

    //create a canvas
    TCanvas* c_LO = new TCanvas("cLO", "Light Output", 800, 400);
    // cLO->Divide(1,2);

    c_LO->cd();

    h_photonLightOutputExp->SetLineColor(kRed);
    h_photonLightOutputExp->SetStats(0);
    h_photonLightOutputExp->Draw();

    h_photonLightOutputBack->SetLineColor(kRed);
    h_photonLightOutputBack->SetLineStyle(kDashed);
    h_photonLightOutputBack->SetStats(0);
    h_photonLightOutputBack->Draw("SAME");

    h_neutronLightOutputExp->SetLineColor(kBlue);
    h_neutronLightOutputExp->SetStats(0);
    h_neutronLightOutputExp->Draw("SAME");

    h_neutronLightOutputBack->SetLineColor(kBlue);
    h_neutronLightOutputBack->SetLineStyle(kDashed);
    h_neutronLightOutputBack->SetStats(0);
    h_neutronLightOutputBack->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronLightOutputExp", "Experimental Neutron");
    legend->AddEntry("h_photonLightOutputExp", "Experimental Photon");
    legend->AddEntry("h_neutronLightOutputBack", "Background Neutron");
    legend->AddEntry("h_photonLightOutputBack", "Background Photon");
    legend->Draw("SAME");

    // change size of axis objects - should be done on first histogram plotted
    // in this function
    h_photonLightOutputExp->GetXaxis()->SetTitleSize(x_labelSize);
    h_photonLightOutputExp->GetYaxis()->SetTitleSize(y_labelSize);
    h_photonLightOutputExp->GetXaxis()->SetTickSize(x_tickSize);
    h_photonLightOutputExp->GetYaxis()->SetTickSize(y_tickSize);

    c_LO->Write();
    //c_LO->SaveAs("fig/LightOutput.eps");
}

void readFiss::PlotTof()
{

    writeFile->cd();
    cd_basics->cd();
    cout << "Plotting Time of Flight." << endl;

    //create a canvas
    TCanvas* c_ToF = new TCanvas("cToF", "Time of Flight", 800, 400);
    // cLO->Divide(1,2);

    c_ToF->cd();

    h_photonTofExp->SetLineColor(kRed);
    h_photonTofExp->SetStats(0);
    h_photonTofExp->Draw();

    h_photonTofBack->SetLineColor(kRed);
    h_photonTofBack->SetLineStyle(kDashed);
    h_photonTofBack->SetStats(0);
    h_photonTofBack->Draw("SAME");

    h_neutronTofExp->SetLineColor(kBlue);
    h_neutronTofExp->SetStats(0);
    h_neutronTofExp->Draw("SAME");

    h_neutronTofBack->SetLineColor(kBlue);
    h_neutronTofBack->SetLineStyle(kDashed);
    h_neutronTofBack->SetStats(0);
    h_neutronTofBack->Draw("SAME");



    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronTofExp", "Experimental Neutron");
    legend->AddEntry("h_photonTofExp", "Experimental Photon");
    legend->AddEntry("h_neutronTofBack", "Background Neutron");
    legend->AddEntry("h_photonTofBack", "Background Photon");
    legend->Draw("SAME");

    // change size of axis objects - should be done on first histogram plotted
    // in this function
    h_photonTofExp->GetXaxis()->SetTitleSize(x_labelSize);
    h_photonTofExp->GetYaxis()->SetTitleSize(y_labelSize);
    h_photonTofExp->GetXaxis()->SetTickSize(x_tickSize);
    h_photonTofExp->GetYaxis()->SetTickSize(y_tickSize);

    c_ToF->Write();
    //c_ToF->SaveAs("fig/TimeOfFlight.eps");
}

void readFiss::PlotErg()
{

    writeFile->cd();
    cd_basics->cd();
    cout << "Plotting Neutron Energy." << endl;

    //create a canvas
    TCanvas* c_Erg = new TCanvas("cErg", "ToF Energy", 800, 400);
    // cLO->Divide(1,2);

    c_Erg->cd();
    h_neutronEnergyExp->SetLineColor(kBlue);
    h_neutronEnergyExp->SetStats(0);
    h_neutronEnergyExp->Draw();

    h_neutronEnergyBack->SetLineColor(kBlue);
    h_neutronEnergyBack->SetLineStyle(kDashed);
    h_neutronEnergyBack->SetStats(0);
    h_neutronEnergyBack->Draw("SAME");


    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronEnergyExp", "Experimental Neutron");
    legend->AddEntry("h_neutronEnergyBack", "Background Neutron");
    legend->Draw("SAME");

    // change size of axis objects - should be done on first histogram plotted
    // in this function
    h_neutronEnergyExp->GetXaxis()->SetTitleSize(x_labelSize);
    h_neutronEnergyExp->GetYaxis()->SetTitleSize(y_labelSize);
    h_neutronEnergyExp->GetXaxis()->SetTickSize(x_tickSize);
    h_neutronEnergyExp->GetYaxis()->SetTickSize(y_tickSize);

    c_Erg->Write();
    //c_Erg->SaveAs("fig/NeutronErg.eps");
}

void readFiss::PlotMult()
{
    writeFile->cd();
    cd_basics->cd();

    cout << "Plotting Multiplicities." << endl;

    //create a canvas
    TCanvas* c_Mult = new TCanvas("cMult", "Multiplicity", 800, 400);
    // cLO->Divide(1,2);

    c_Mult->cd();

    h_photonMultExp->SetLineColor(kRed);
    h_photonMultExp->SetStats(0);
    h_photonMultExp->Draw();

    h_photonMultBack->SetLineColor(kRed);
    h_photonMultBack->SetLineStyle(kDashed);
    h_photonMultBack->SetStats(0);
    h_photonMultBack->Draw("SAME");

    h_neutronMultExp->SetLineColor(kBlue);
    h_neutronMultExp->SetStats(0);
    h_neutronMultExp->Draw("SAME");

    h_neutronMultBack->SetLineColor(kBlue);
    h_neutronMultBack->SetLineStyle(kDashed);
    h_neutronMultBack->SetStats(0);
    h_neutronMultBack->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronMultExp", "Experimental Neutron");
    legend->AddEntry("h_photonMultExp", "Experimental Photon");
    legend->AddEntry("h_neutronMultBack", "Background Neutron");
    legend->AddEntry("h_photonMultBack", "Background Photon");
    legend->Draw("SAME");

    // change size of axis objects - should be done on first histogram plotted
    // in this function
    h_photonMultExp->GetXaxis()->SetTitleSize(x_labelSize);
    h_photonMultExp->GetYaxis()->SetTitleSize(y_labelSize);
    h_photonMultExp->GetXaxis()->SetTickSize(x_tickSize);
    h_photonMultExp->GetYaxis()->SetTickSize(y_tickSize);

    c_Mult->Write();
    //c_Mult->SaveAs("fig/Multiplicity.eps");
}

void readFiss::PlotPSD()
{
    writeFile->cd();
    cd_basics->cd();

    cout << "Plotting Particle Discrimination." << endl;

    //create a canvas
    TCanvas* c_PSD = new TCanvas("cPSD", "Particle Dicrimination", 800, 400);
    // cLO->Divide(1,2);

    c_PSD->cd();

    h_photonPSDExp->SetLineColor(kRed);
    h_photonPSDExp->SetStats(0);
    h_photonPSDExp->Draw();

    h_photonPSDBack->SetLineColor(kRed);
    h_photonPSDBack->SetLineStyle(kDashed);
    h_photonPSDBack->SetStats(0);
    h_photonPSDBack->Draw("SAME");

    h_neutronPSDExp->SetLineColor(kBlue);
    h_neutronPSDExp->SetStats(0);
    h_neutronPSDExp->Draw("SAME");

    h_neutronPSDBack->SetLineColor(kBlue);
    h_neutronPSDBack->SetLineStyle(kDashed);
    h_neutronPSDBack->SetStats(0);
    h_neutronPSDBack->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronPSDExp", "Experimental Neutron");
    legend->AddEntry("h_photonPSDExp", "Experimental Photon");
    legend->AddEntry("h_neutronPSDBack", "Background Neutron");
    legend->AddEntry("h_photonPSDBack", "Background Photon");
    legend->Draw("SAME");

    // change size of axis objects - should be done on first histogram plotted
    // in this function
    h_photonPSDExp->GetXaxis()->SetTitleSize(x_labelSize);
    h_photonPSDExp->GetYaxis()->SetTitleSize(y_labelSize);
    h_photonPSDExp->GetXaxis()->SetTickSize(x_tickSize);
    h_photonPSDExp->GetYaxis()->SetTickSize(y_tickSize);

    c_PSD->Write();
    //c_PSD->SaveAs("fig/ParticleDiscrimination.eps");
}

void readFiss::PlotSingles()
{
    writeFile->cd();
    cd_basics->cd();
    cout << "Plotting Singles." << endl;

    //create a canvas
    TCanvas* c_Sin = new TCanvas("cSin", "Singles Rates", 800, 400);
    // cLO->Divide(1,2);

    c_Sin->cd();

    h_photonSinglesExp->SetLineColor(kRed);
    h_photonSinglesExp->SetStats(0);
    h_photonSinglesExp->Draw();

    h_photonSinglesBack->SetLineColor(kRed);
    h_photonSinglesBack->SetLineStyle(kDashed);
    h_photonSinglesBack->SetStats(0);
    h_photonSinglesBack->Draw("SAME");

    h_neutronSinglesExp->SetLineColor(kBlue);
    h_neutronSinglesExp->SetStats(0);
    h_neutronSinglesExp->Draw("SAME");

    h_neutronSinglesBack->SetLineColor(kBlue);
    h_neutronSinglesBack->SetLineStyle(kDashed);
    h_neutronSinglesBack->SetStats(0);
    h_neutronSinglesBack->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronSinglesExp", "Experimental Neutron");
    legend->AddEntry("h_photonSinglesExp", "Experimental Photon");
    legend->AddEntry("h_neutronSinglesBack", "Background Neutron");
    legend->AddEntry("h_photonSinglesBack", "Background Photon");
    legend->Draw("SAME");

    // change size of axis objects - should be done on first histogram plotted
    // in this function
    h_photonSinglesExp->GetXaxis()->SetTitleSize(x_labelSize);
    h_photonSinglesExp->GetYaxis()->SetTitleSize(y_labelSize);
    h_photonSinglesExp->GetXaxis()->SetTickSize(x_tickSize);
    h_photonSinglesExp->GetYaxis()->SetTickSize(y_tickSize);

    c_Sin->Write();
    //c_Sin->SaveAs("fig/Singles.eps");
}

void readFiss::PlotMultCorExp()
{
  writeFile->cd();
  cd_correlated->cd();
  cout << "Plotting correlated multiplicity." << endl;

  //make canvas
  TCanvas* c_MultCorExp = new TCanvas("cMultCorExp", "Neutron-Gamma Multiplicity",
                                                                  800, 400);
  c_MultCorExp->cd();

  h2_neutronGammaMultExp->SetLineColor(kRed);
  h2_neutronGammaMultExp->Draw("COLZ");

  // change stat box
  c_MultCorExp->Update();
  TPaveStats *statsBox = (TPaveStats*)c_MultCorExp->GetPrimitive("stats");
  statsBox->SetName("mystats");
  TList* statsList = statsBox->GetListOfLines();

  TText* xline = statsBox->GetLineWith("Mean x");
  TString xtext = xline->GetTitle();
  xtext.Replace(0, xtext.First('='), "Mean Neutrons ");
  TLatex* xlatex = new TLatex(0, 0, xtext);
  xlatex->SetTextFont(42);
  xlatex->SetTextSize(0.035);
  statsList->Remove(xline);
  statsList->Add(xlatex);

  TText* yline = statsBox->GetLineWith("Mean y");
  TString ytext = yline->GetTitle();
  ytext.Replace(0, ytext.First('='), "Mean Gammas ");
  TLatex* ylatex = new TLatex(0, 0, ytext);
  ylatex->SetTextFont(42);
  ylatex->SetTextSize(0.035);
  statsList->Remove(yline);
  statsList->Add(ylatex);

  h2_neutronGammaMultExp->SetStats(0);
  c_MultCorExp->Modified();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  h2_neutronGammaMultExp->GetXaxis()->SetTitleSize(x_labelSize);
  h2_neutronGammaMultExp->GetYaxis()->SetTitleSize(y_labelSize);
  h2_neutronGammaMultExp->GetXaxis()->SetTickSize(x_tickSize);
  h2_neutronGammaMultExp->GetYaxis()->SetTickSize(y_tickSize);

  c_MultCorExp->Write();
  //c_MultCorExp->SaveAs("MultiplicityCorrelationExp.eps");
}

void readFiss::PlotMultCorBack()
{
  writeFile->cd();
  cd_correlated->cd();
  cout << "Plotting correlated back multiplicity." << endl;

  //make canvas
  TCanvas* c_MultCorBack = new TCanvas("cMultCorBack", "Neutron-Gamma Back Multiplicity",
                                                                  800, 400);
  c_MultCorBack->cd();

  h2_neutronGammaMultBack->SetLineColor(kRed);
  h2_neutronGammaMultBack->Draw("COLZ");

  // change stat box
  c_MultCorBack->Update();
  TPaveStats *statsBox = (TPaveStats*)c_MultCorBack->GetPrimitive("stats");
  statsBox->SetName("mystats");
  TList* statsList = statsBox->GetListOfLines();

  TText* xline = statsBox->GetLineWith("Mean x");
  TString xtext = xline->GetTitle();
  xtext.Replace(0, xtext.First('='), "Mean Neutrons ");
  TLatex* xlatex = new TLatex(0, 0, xtext);
  xlatex->SetTextFont(42);
  xlatex->SetTextSize(0.035);
  statsList->Remove(xline);
  statsList->Add(xlatex);

  TText* yline = statsBox->GetLineWith("Mean y");
  TString ytext = yline->GetTitle();
  ytext.Replace(0, ytext.First('='), "Mean Gammas ");
  TLatex* ylatex = new TLatex(0, 0, ytext);
  ylatex->SetTextFont(42);
  ylatex->SetTextSize(0.035);
  statsList->Remove(yline);
  statsList->Add(ylatex);

  h2_neutronGammaMultBack->SetStats(0);
  c_MultCorBack->Modified();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  h2_neutronGammaMultBack->GetXaxis()->SetTitleSize(x_labelSize);
  h2_neutronGammaMultBack->GetYaxis()->SetTitleSize(y_labelSize);
  h2_neutronGammaMultBack->GetXaxis()->SetTickSize(x_tickSize);
  h2_neutronGammaMultBack->GetYaxis()->SetTickSize(y_tickSize);

  c_MultCorBack->Write();
  //c_MultCorBack->SaveAs("MultiplicityCorrelationBack.eps");
}

void readFiss::PlotMultLOExp()
{
  writeFile->cd();
  cd_correlated->cd();
  cout << "Plotting correlated mult/LO." << endl;

  //make canvas
  TCanvas* c_MultLOExp = new TCanvas("cMultLOExp", "Neutron Mult vs. Photon LO",
                                                                800, 400);
  c_MultLOExp->cd();

  h2_neutronMultPhotonLOExp->SetLineColor(kRed);
  h2_neutronMultPhotonLOExp->SetStats(0);
  h2_neutronMultPhotonLOExp->Draw("COLZ");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  h2_neutronMultPhotonLOExp->GetXaxis()->SetTitleSize(x_labelSize);
  h2_neutronMultPhotonLOExp->GetYaxis()->SetTitleSize(y_labelSize);
  h2_neutronMultPhotonLOExp->GetXaxis()->SetTickSize(x_tickSize);
  h2_neutronMultPhotonLOExp->GetYaxis()->SetTickSize(y_tickSize);

  c_MultLOExp->Write();
  //c_MultLOExp->SaveAs("NeutronMultiplicityPhotonLOExp.eps");
}

void readFiss::PlotEnergyLOExp()
{
  writeFile->cd();
  cd_correlated->cd();
  cout << "Plotting correlated neutron Energy/LO" << endl;

  //make canvas
  TCanvas* c_EnergyLOExp = new TCanvas("cEnergyLOExp",
                                  "Neutron Energy vs. Neutron LO", 800, 400);
  c_EnergyLOExp->cd();

  h2_neutronEnergyLOExp->SetLineColor(kRed);
  h2_neutronEnergyLOExp->SetStats(0);
  h2_neutronEnergyLOExp->Draw("COLZ");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  h2_neutronEnergyLOExp->GetXaxis()->SetTitleSize(x_labelSize);
  h2_neutronEnergyLOExp->GetYaxis()->SetTitleSize(y_labelSize);
  h2_neutronEnergyLOExp->GetXaxis()->SetTickSize(x_tickSize);
  h2_neutronEnergyLOExp->GetYaxis()->SetTickSize(y_tickSize);

  c_EnergyLOExp->Write();
  //c_EnergyLOExp->SaveAs("neutronEnergyLightOut.eps");
}

void readFiss::PlotN_LOPSD_Exp()
{
  writeFile->cd();
  cd_correlated->cd();
  cout << "Plotting correlated neutron LO/PSD" << endl;

  //make canvas
  TCanvas* c_N_LOPSD_Exp = new TCanvas("cN_LOPSD_Exp",
                                  "Neutron LO vs. Neutron PSD", 800, 400);
  c_N_LOPSD_Exp->cd();

  h2_neutronLightOutPSDExp->SetLineColor(kRed);
  h2_neutronLightOutPSDExp->SetStats(0);
  h2_neutronLightOutPSDExp->Draw("SURF2");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  h2_neutronLightOutPSDExp->GetXaxis()->SetTitleSize(x_labelSize);
  h2_neutronLightOutPSDExp->GetYaxis()->SetTitleSize(y_labelSize);
  h2_neutronLightOutPSDExp->GetXaxis()->SetTickSize(x_tickSize);
  h2_neutronLightOutPSDExp->GetYaxis()->SetTickSize(y_tickSize);

  c_N_LOPSD_Exp->Write();
  //c_N_LOPSD_Exp->SaveAs("h2_neutronLightOutPSDExp.eps");
}

void readFiss::PlotP_LOPSD_Exp()
{
  writeFile->cd();
  cd_correlated->cd();
  cout << "Plotting correlated photon LO/PSD" << endl;

  //make canvas
  TCanvas* c_P_LOPSD_Exp = new TCanvas("cP_LOPSD_Exp", "Photon LO vs. Photon PSD", 800, 400);
  c_P_LOPSD_Exp->cd();

  cout << h2_photonLightOutPSDExp << endl;

  h2_photonLightOutPSDExp->SetLineColor(kRed);
  h2_photonLightOutPSDExp->SetStats(0);
  h2_photonLightOutPSDExp->Draw("SURF2");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  h2_photonLightOutPSDExp->GetXaxis()->SetTitleSize(x_labelSize);
  h2_photonLightOutPSDExp->GetYaxis()->SetTitleSize(y_labelSize);
  h2_photonLightOutPSDExp->GetXaxis()->SetTickSize(x_tickSize);
  h2_photonLightOutPSDExp->GetYaxis()->SetTickSize(y_tickSize);
  
  c_P_LOPSD_Exp->Write();
  //c_P_LOPSD_Exp->SaveAs("h2_photonLightOutPSDExp.eps");
}

void readFiss::PlotN_AngleCorr()
{
  writeFile->cd();
  cd_correlated->cd();
  cout << "Plotting neutron angular correlations." << endl;

  // make canvas
  TCanvas* c_N_AngleCorr = new TCanvas("cN_AngleCorr", "Cos(Theta) in Neutron Doubles", 800, 400);
  c_N_AngleCorr->cd();

  g_neutronAngleCorr->SetLineColor(kRed);
  g_neutronAngleCorr->Draw("A*");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronAngleCorr->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronAngleCorr->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronAngleCorr->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronAngleCorr->GetYaxis()->SetTickSize(y_tickSize);

  c_N_AngleCorr->Write();
  //c_N_AngleCorr->SaveAs("g_neutronAngleCorrExp.eps");

  cout << "Plotting photon angular correlations." << endl;

  // make photon canvas
  TCanvas* c_P_AngleCorr = new TCanvas("cP_AngleCorr", "Cos(Theta) in Photon Doubles", 800, 400);
  c_P_AngleCorr->cd();

  g_photonAngleCorr->SetLineColor(kRed);
  g_photonAngleCorr->Draw("A*");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_photonAngleCorr->GetXaxis()->SetTitleSize(x_labelSize);
  g_photonAngleCorr->GetYaxis()->SetTitleSize(y_labelSize);
  g_photonAngleCorr->GetXaxis()->SetTickSize(x_tickSize);
  g_photonAngleCorr->GetYaxis()->SetTickSize(y_tickSize);

  c_P_AngleCorr->Write();

  cout << "Plotting neutron-photon angular correlations." << endl;

  // make neutorn-photon canvas
  TCanvas* c_NP_AngleCorr = new TCanvas("cNP_AngleCorr", "Cos(Theta) in Neutron-Photon Doubles", 800, 400);
  c_NP_AngleCorr->cd();

  g_neutronPhotonAngleCorr->SetLineColor(kRed);
  g_neutronPhotonAngleCorr->Draw("A*");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronPhotonAngleCorr->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronPhotonAngleCorr->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronPhotonAngleCorr->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronPhotonAngleCorr->GetYaxis()->SetTickSize(y_tickSize);

  c_NP_AngleCorr->Write();

  // Background
  cout << "Plotting background neutron angular correlations." << endl;

  // make canvas
  TCanvas* c_NB_AngleCorr = new TCanvas("cNB_AngleCorr", "Cos(Theta) in Background Neutron Doubles", 800, 400);
  c_NB_AngleCorr->cd();

  g_neutronBackAngleCorr->SetLineColor(kRed);
  g_neutronBackAngleCorr->Draw("A*");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronBackAngleCorr->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronBackAngleCorr->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronBackAngleCorr->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronBackAngleCorr->GetYaxis()->SetTickSize(y_tickSize);

  c_NB_AngleCorr->Write();

  cout << "Plotting background photon angular correlations." << endl;

  // make background photon canvas
  TCanvas* c_PB_AngleCorr = new TCanvas("cPB_AngleCorr", "Cos(Theta) in Background Photon Doubles", 800, 400);
  c_PB_AngleCorr->cd();

  g_photonBackAngleCorr->SetLineColor(kRed);
  g_photonBackAngleCorr->Draw("A*");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_photonBackAngleCorr->GetXaxis()->SetTitleSize(x_labelSize);
  g_photonBackAngleCorr->GetYaxis()->SetTitleSize(y_labelSize);
  g_photonBackAngleCorr->GetXaxis()->SetTickSize(x_tickSize);
  g_photonBackAngleCorr->GetYaxis()->SetTickSize(y_tickSize);

  c_PB_AngleCorr->Write();

  cout << "Plotting background neutron-photon angular correlations." << endl;

  // make background neutorn-photon canvas
  TCanvas* c_NPB_AngleCorr = new TCanvas("cNPB_AngleCorr", "Cos(Theta) in Background Neutron-Photon Doubles", 800, 400);
  c_NPB_AngleCorr->cd();

  g_neutronPhotonBackAngleCorr->SetLineColor(kRed);
  g_neutronPhotonBackAngleCorr->Draw("A*");

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronPhotonBackAngleCorr->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronPhotonBackAngleCorr->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronPhotonBackAngleCorr->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronPhotonBackAngleCorr->GetYaxis()->SetTickSize(y_tickSize);

  c_NPB_AngleCorr->Write();

  TCanvas* C_N_AngleCorrAvg = new TCanvas("cN_AngleCorrAvg", "Average line of Cos(Theta) in Neutron Doubles", 800, 400);
  C_N_AngleCorrAvg->cd();

  g_neutronAngleCorrAvg->SetLineColor(kBlue);
  g_neutronAngleCorrAvg->Draw();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronAngleCorrAvg->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronAngleCorrAvg->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronAngleCorrAvg->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronAngleCorrAvg->GetYaxis()->SetTickSize(y_tickSize);

  C_N_AngleCorrAvg->Write();

    // gamma-gamma
  TCanvas* C_P_AngleCorrAvg = new TCanvas("cP_AngleCorrAvg", "Average line of Cos(Theta) in Photon Doubles", 800, 400);
  C_P_AngleCorrAvg->cd();

  g_photonAngleCorrAvg->SetLineColor(kBlue);
  g_photonAngleCorrAvg->Draw();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_photonAngleCorrAvg->GetXaxis()->SetTitleSize(x_labelSize);
  g_photonAngleCorrAvg->GetYaxis()->SetTitleSize(y_labelSize);
  g_photonAngleCorrAvg->GetXaxis()->SetTickSize(x_tickSize);
  g_photonAngleCorrAvg->GetYaxis()->SetTickSize(y_tickSize);

  C_P_AngleCorrAvg->Write();

    // n-gamma
  TCanvas* C_NP_AngleCorrAvg = new TCanvas("cNP_AngleCorrAvg", "Average line of Cos(Theta) in Neutron-Photon Doubles", 800, 400);
  C_NP_AngleCorrAvg->cd();

  g_neutronPhotonAngleCorrAvg->SetLineColor(kBlue);
  g_neutronPhotonAngleCorrAvg->Draw();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronPhotonAngleCorrAvg->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronPhotonAngleCorrAvg->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronPhotonAngleCorrAvg->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronPhotonAngleCorrAvg->GetYaxis()->SetTickSize(y_tickSize);

  C_NP_AngleCorrAvg->Write();

    // n-n background
  TCanvas* C_NB_AngleCorrAvg = new TCanvas("cNB_AngleCorrAvg", "Average line of Cos(Theta) in Background Neutron Doubles", 800, 400);
  C_NB_AngleCorrAvg->cd();

  g_neutronBackAngleCorrAvg->SetLineColor(kBlue);
  g_neutronBackAngleCorrAvg->Draw();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronBackAngleCorrAvg->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronBackAngleCorrAvg->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronBackAngleCorrAvg->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronBackAngleCorrAvg->GetYaxis()->SetTickSize(y_tickSize);

  C_NB_AngleCorrAvg->Write();

    // gamma-gamma background
  TCanvas* C_PB_AngleCorrAvg = new TCanvas("cPB_AngleCorrAvg", "Average line of Cos(Theta) in Background Photon Doubles", 800, 400);
  C_PB_AngleCorrAvg->cd();

  g_photonBackAngleCorrAvg->SetLineColor(kBlue);
  g_photonBackAngleCorrAvg->Draw();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_photonBackAngleCorrAvg->GetXaxis()->SetTitleSize(x_labelSize);
  g_photonBackAngleCorrAvg->GetYaxis()->SetTitleSize(y_labelSize);
  g_photonBackAngleCorrAvg->GetXaxis()->SetTickSize(x_tickSize);
  g_photonBackAngleCorrAvg->GetYaxis()->SetTickSize(y_tickSize);

  C_PB_AngleCorrAvg->Write();

    // n-gamma background
  TCanvas* C_NPB_AngleCorrAvg = new TCanvas("cNPB_AngleCorrAvg", "Average line of Cos(Theta) in Background Neutron-Photon Doubles", 800, 400);
  C_NPB_AngleCorrAvg->cd();

  g_neutronPhotonBackAngleCorrAvg->SetLineColor(kBlue);
  g_neutronPhotonBackAngleCorrAvg->Draw();

  // change size of axis objects - should be done on first histogram plotted
  // in this function
  g_neutronPhotonBackAngleCorrAvg->GetXaxis()->SetTitleSize(x_labelSize);
  g_neutronPhotonBackAngleCorrAvg->GetYaxis()->SetTitleSize(y_labelSize);
  g_neutronPhotonBackAngleCorrAvg->GetXaxis()->SetTickSize(x_tickSize);
  g_neutronPhotonBackAngleCorrAvg->GetYaxis()->SetTickSize(y_tickSize);

  C_NPB_AngleCorrAvg->Write();

  //c_N_AngleCorrAvg->SaveAs("g_neutronAngleCorrExpAvg.eps");

  h2_neutronSinglesMat->Write();
  h2_neutronDoublesMat->Write();
  h2_neutronScaledDoubles->Write();

  h2_photonSinglesMat->Write();
  h2_photonDoublesMat->Write();
  h2_photonScaledDoubles->Write();

  h2_neutronPhotonSinglesMat->Write();
  h2_neutronPhotonDoublesMat->Write();
  h2_neutronPhotonScaledDoubles->Write();

  h2_neutronBackSinglesMat->Write();
  h2_neutronBackDoublesMat->Write();
  h2_neutronBackScaledDoubles->Write();

  h2_photonBackSinglesMat->Write();
  h2_photonBackDoublesMat->Write();
  h2_photonBackScaledDoubles->Write();

  h2_neutronPhotonBackSinglesMat->Write();
  h2_neutronPhotonBackDoublesMat->Write();
  h2_neutronPhotonBackScaledDoubles->Write();
}

void readFiss::PlotIndiv()
{
  writeFile->cd();
  cd_individual->cd();
  cout << "Plotting individual detectors" << endl;

  cd_LightOutExp = cd_individual->mkdir("LightOutExp");
  cd_ToFExp = cd_individual->mkdir("ToFExp");
  cd_ErgExp = cd_individual->mkdir("ErgExp");
  cd_PSDExp = cd_individual->mkdir("PSDExp");
  cd_IndivNeutronEnergyLOExp = cd_individual->mkdir("neutronEnergyLOExp");
  cd_IndivNeutronLightOutPSDExp = cd_individual->mkdir("neutronLightOutPSDExp");
  cd_IndivPhotonLightOutPSDExp = cd_individual->mkdir("photonLightOutPSDExp");

  cd_LightOutExp->cd();
  TCanvas** c_LightOutExp = new TCanvas*[NUM_DETECTORS];

  cd_ToFExp->cd();
  TCanvas** c_ToFExp = new TCanvas*[NUM_DETECTORS];

  cd_ErgExp->cd();
  TCanvas** c_ErgExp = new TCanvas*[NUM_DETECTORS];

  cd_PSDExp->cd();
  TCanvas** c_PSDExp = new TCanvas*[NUM_DETECTORS];

  cd_IndivNeutronEnergyLOExp->cd();
  TCanvas** c_h2_IndivNeutronEnergyLOExp = new TCanvas*[NUM_DETECTORS];

  cd_IndivNeutronLightOutPSDExp->cd();
  TCanvas** c_h2_IndivNeutronLightOutPSDExp = new TCanvas*[NUM_DETECTORS];

  cd_IndivPhotonLightOutPSDExp->cd();
  TCanvas** c_h2_IndivPhotonLightOutPSDExp = new TCanvas*[NUM_DETECTORS];

  for(int i = 0; i < NUM_DETECTORS; ++i)
  {
    TString index = (TString)to_string(i);

    cd_LightOutExp->cd();
    c_LightOutExp[i] = new TCanvas((TString)"LightOutExp" + (TString)to_string(i), "Light Output", 800, 400);
    c_LightOutExp[i]->cd();

    h_IndivPhotonLightOutputExp[i]->SetLineColor(kRed);
    h_IndivPhotonLightOutputExp[i]->SetStats(0);
    h_IndivPhotonLightOutputExp[i]->Draw();

    h_IndivPhotonLightOutputBack[i]->SetLineColor(kRed);
    h_IndivPhotonLightOutputBack[i]->SetLineStyle(kDashed);
    h_IndivPhotonLightOutputBack[i]->SetStats(0);
    h_IndivPhotonLightOutputBack[i]->Draw("SAME");

    h_IndivNeutronLightOutputExp[i]->SetLineColor(kBlue);
    h_IndivNeutronLightOutputExp[i]->SetStats(0);
    h_IndivNeutronLightOutputExp[i]->Draw("SAME");

    h_IndivNeutronLightOutputBack[i]->SetLineColor(kBlue);
    h_IndivNeutronLightOutputBack[i]->SetLineStyle(kDashed);
    h_IndivNeutronLightOutputBack[i]->SetStats(0);
    h_IndivNeutronLightOutputBack[i]->Draw("SAME");

    TLegend *legend_LightOutput = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend_LightOutput->AddEntry((TString)"h_IndivNeutronLightOutputExp" + index, "Experimental Neutron");
    legend_LightOutput->AddEntry((TString)"h_IndivPhotonLightOutputExp" + index, "Experimental Photon");
    legend_LightOutput->AddEntry((TString)"h_IndivNeutronLightOutputBack" + index, "Background Neutron");
    legend_LightOutput->AddEntry((TString)"h_IndivPhotonLightOutputBack" + index, "Background Photon");
    legend_LightOutput->Draw("SAME");

    h_IndivPhotonLightOutputExp[i]->GetXaxis()->SetTitleSize(x_labelSize);
    h_IndivPhotonLightOutputExp[i]->GetYaxis()->SetTitleSize(y_labelSize);
    h_IndivPhotonLightOutputExp[i]->GetXaxis()->SetTickSize(x_tickSize);
    h_IndivPhotonLightOutputExp[i]->GetYaxis()->SetTickSize(y_tickSize);

    c_LightOutExp[i]->Write();

    cd_ToFExp->cd();
    c_ToFExp[i] = new TCanvas((TString)"ToFExp" + (TString)to_string(i), "Time of Flight", 800, 400);
    c_ToFExp[i]->cd();

    h_IndivPhotonTofExp[i]->SetLineColor(kRed);
    h_IndivPhotonTofExp[i]->SetStats(0);
    h_IndivPhotonTofExp[i]->Draw();

    h_IndivPhotonTofBack[i]->SetLineColor(kRed);
    h_IndivPhotonTofBack[i]->SetLineStyle(kDashed);
    h_IndivPhotonTofBack[i]->SetStats(0);
    h_IndivPhotonTofBack[i]->Draw("SAME");

    h_IndivNeutronTofExp[i]->SetLineColor(kBlue);
    h_IndivNeutronTofExp[i]->SetStats(0);
    h_IndivNeutronTofExp[i]->Draw("SAME");

    h_IndivNeutronTofBack[i]->SetLineColor(kBlue);
    h_IndivNeutronTofBack[i]->SetLineStyle(kDashed);
    h_IndivNeutronTofBack[i]->SetStats(0);
    h_IndivNeutronTofBack[i]->Draw("SAME");

    TLegend *legend_ToF = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend_ToF->AddEntry((TString)"h_IndivNeutronTofExp" + index, "Experimental Neutron");
    legend_ToF->AddEntry((TString)"h_IndivPhotonTofExp" + index, "Experimental Photon");
    legend_ToF->AddEntry((TString)"h_IndivNeutronTofBack" + index, "Background Neutron");
    legend_ToF->AddEntry((TString)"h_IndivPhotonTofBack" + index, "Background Photon");
    legend_ToF->Draw("SAME");

    h_IndivPhotonTofExp[i]->GetXaxis()->SetTitleSize(x_labelSize);
    h_IndivPhotonTofExp[i]->GetYaxis()->SetTitleSize(y_labelSize);
    h_IndivPhotonTofExp[i]->GetXaxis()->SetTickSize(x_tickSize);
    h_IndivPhotonTofExp[i]->GetYaxis()->SetTickSize(y_tickSize);

    c_ToFExp[i]->Write();

    cd_ErgExp->cd();
    c_ErgExp[i] = new TCanvas((TString)"ErgExp" + (TString)to_string(i), "ToF Energy", 800, 400);
    c_ErgExp[i]->cd();

    h_IndivNeutronEnergyExp[i]->SetLineColor(kBlue);
    h_IndivNeutronEnergyExp[i]->SetStats(0);
    h_IndivNeutronEnergyExp[i]->Draw();

    h_IndivNeutronEnergyBack[i]->SetLineColor(kBlue);
    h_IndivNeutronEnergyBack[i]->SetLineStyle(kDashed);
    h_IndivNeutronEnergyBack[i]->SetStats(0);
    h_IndivNeutronEnergyBack[i]->Draw("SAME");


    TLegend *legend_Energy = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend_Energy->AddEntry((TString)"h_IndivNeutronEnergyExp" + index, "Experimental Neutron");
    legend_Energy->AddEntry((TString)"h_IndivNeutronEnergyBack" + index, "Background Neutron");
    legend_Energy->Draw("SAME");

    h_IndivNeutronEnergyExp[i]->GetXaxis()->SetTitleSize(x_labelSize);
    h_IndivNeutronEnergyExp[i]->GetYaxis()->SetTitleSize(y_labelSize);
    h_IndivNeutronEnergyExp[i]->GetXaxis()->SetTickSize(x_tickSize);
    h_IndivNeutronEnergyExp[i]->GetYaxis()->SetTickSize(y_tickSize);

    c_ErgExp[i]->Write();

    cd_PSDExp->cd();
    c_PSDExp[i] = new TCanvas((TString)"PSDExp" + (TString)to_string(i), "Particle Discrimination", 800, 400);
    c_PSDExp[i]->cd();

    h_IndivPhotonPSDExp[i]->SetLineColor(kRed);
    h_IndivPhotonPSDExp[i]->SetStats(0);
    h_IndivPhotonPSDExp[i]->Draw();

    h_IndivPhotonPSDBack[i]->SetLineColor(kRed);
    h_IndivPhotonPSDBack[i]->SetLineStyle(kDashed);
    h_IndivPhotonPSDBack[i]->SetStats(0);
    h_IndivPhotonPSDBack[i]->Draw("SAME");

    h_IndivNeutronPSDExp[i]->SetLineColor(kBlue);
    h_IndivNeutronPSDExp[i]->SetStats(0);
    h_IndivNeutronPSDExp[i]->Draw("SAME");

    h_IndivNeutronPSDBack[i]->SetLineColor(kBlue);
    h_IndivNeutronPSDBack[i]->SetLineStyle(kDashed);
    h_IndivNeutronPSDBack[i]->SetStats(0);
    h_IndivNeutronPSDBack[i]->Draw("SAME");

    TLegend *legend_PSD = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend_PSD->AddEntry((TString)"h_IndivNeutronPSDExp" + index, "Experimental Neutron");
    legend_PSD->AddEntry((TString)"h_IndivPhotonPSDExp" + index, "Experimental Photon");
    legend_PSD->AddEntry((TString)"h_IndivNeutronPSDBack" + index, "Background Neutron");
    legend_PSD->AddEntry((TString)"h_IndivPhotonPSDBack" + index, "Background Photon");
    legend_PSD->Draw("SAME");

    h_IndivPhotonPSDExp[i]->GetXaxis()->SetTitleSize(x_labelSize);
    h_IndivPhotonPSDExp[i]->GetYaxis()->SetTitleSize(y_labelSize);
    h_IndivPhotonPSDExp[i]->GetXaxis()->SetTickSize(x_tickSize);
    h_IndivPhotonPSDExp[i]->GetYaxis()->SetTickSize(y_tickSize);

    c_PSDExp[i]->Write();

    cd_IndivNeutronEnergyLOExp->cd();
    c_h2_IndivNeutronEnergyLOExp[i] = new TCanvas((TString)"h2_neutronEnergyLOExp" + (TString)to_string(i), "Neutron Energy vs. LO", 800, 400);
    c_h2_IndivNeutronEnergyLOExp[i]->cd();

    h2_IndivNeutronEnergyLOExp[i]->SetLineColor(kRed);
    h2_IndivNeutronEnergyLOExp[i]->SetStats(0);
    h2_IndivNeutronEnergyLOExp[i]->Draw("COLZ");

    h2_IndivNeutronEnergyLOExp[i]->GetXaxis()->SetTitleSize(x_labelSize);
    h2_IndivNeutronEnergyLOExp[i]->GetYaxis()->SetTitleSize(y_labelSize);
    h2_IndivNeutronEnergyLOExp[i]->GetXaxis()->SetTickSize(x_tickSize);
    h2_IndivNeutronEnergyLOExp[i]->GetYaxis()->SetTickSize(y_tickSize);

    c_h2_IndivNeutronEnergyLOExp[i]->Write();

    cd_IndivNeutronLightOutPSDExp->cd();
    c_h2_IndivNeutronLightOutPSDExp[i] = new TCanvas((TString)"h2_neutronLightOutPSDExp" + (TString)to_string(i), "Neutron Light Output vs. PSD", 800, 400);
    c_h2_IndivNeutronLightOutPSDExp[i]->cd();

    h2_IndivNeutronLightOutPSDExp[i]->SetLineColor(kRed);
    h2_IndivNeutronLightOutPSDExp[i]->SetStats(0);
    h2_IndivNeutronLightOutPSDExp[i]->Draw("SURF2");

    h2_IndivNeutronLightOutPSDExp[i]->GetXaxis()->SetTitleSize(x_labelSize);
    h2_IndivNeutronLightOutPSDExp[i]->GetYaxis()->SetTitleSize(y_labelSize);
    h2_IndivNeutronLightOutPSDExp[i]->GetXaxis()->SetTickSize(x_tickSize);
    h2_IndivNeutronLightOutPSDExp[i]->GetYaxis()->SetTickSize(y_tickSize);

    c_h2_IndivNeutronLightOutPSDExp[i]->Write();

    cd_IndivPhotonLightOutPSDExp->cd();
    c_h2_IndivPhotonLightOutPSDExp[i] = new TCanvas((TString)"h2_photonLightOutPSDExp" + (TString)to_string(i), "Photon Light Output vs. PSD", 800, 400);
    c_h2_IndivPhotonLightOutPSDExp[i]->cd();

    h2_IndivPhotonLightOutPSDExp[i]->SetLineColor(kRed);
    h2_IndivPhotonLightOutPSDExp[i]->SetStats(0);
    h2_IndivPhotonLightOutPSDExp[i]->Draw("SURF2");

    h2_IndivPhotonLightOutPSDExp[i]->GetXaxis()->SetTitleSize(x_labelSize);
    h2_IndivPhotonLightOutPSDExp[i]->GetYaxis()->SetTitleSize(y_labelSize);
    h2_IndivPhotonLightOutPSDExp[i]->GetXaxis()->SetTickSize(x_tickSize);
    h2_IndivPhotonLightOutPSDExp[i]->GetYaxis()->SetTickSize(y_tickSize);

    c_h2_IndivPhotonLightOutPSDExp[i]->Write();
  }
}

/*
   ___                          _                ___ _         _____
  / __|___ _ __  _ __  __ _ _ _(_)___ ___ _ _   / __(_)_ __   / / __|_ ___ __
 | (__/ _ \ '  \| '_ \/ _` | '_| (_-</ _ \ ' \  \__ \ | '  \ / /| _|\ \ / '_ \
  \___\___/_|_|_| .__/\__,_|_| |_/__/\___/_||_| |___/_|_|_|_/_/ |___/_\_\ .__/
                |_|                                                     |_|

*/

void readFiss::CompareLightOut()
{

    writeFile->cd();
    cd_simComparison->cd();

    cout << "Comparing Light Output." << endl;


    h_photonLightOutputExp->SetLineColor(kRed);

    h_neutronLightOutputExp->SetLineColor(kBlue);

    h_neutronLightOutputSim->Scale((double)expEntries/(double)simEntries);
    h_neutronLightOutputSim->SetLineColor(kBlue);
    h_neutronLightOutputSim->SetLineStyle(kDashed);

    h_photonLightOutputSim->Scale((double)expEntries/(double)simEntries);
    h_photonLightOutputSim->SetLineColor(kRed);
    h_photonLightOutputSim->SetLineStyle(kDashed);



    //create a canvas
    TCanvas* c_LOs = new TCanvas("cLOs", "Light Output", 800, 400);
    // cLO->Divide(1,2);
    c_LOs->cd();

    h_photonLightOutputExp->Draw();
    h_neutronLightOutputExp->Draw("SAME");
    h_neutronLightOutputSim->Draw("SAME");
    h_photonLightOutputSim->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronLightOutputExp", "Experimental Neutron");
    legend->AddEntry("h_photonLightOutputExp", "Experimental Photon");
    legend->AddEntry("h_neutronLightOutputSim", "Simulated Neutron");
    legend->AddEntry("h_photonLightOutputSim", "Simulated Photon");
    legend->Draw("SAME");

    c_LOs->Write();
    //c_LOs->SaveAs("fig/LightOutputSim.eps");


    //
    // //neutrons only
    // TCanvas* c_LOns = new TCanvas("cLOns", "Light Output Neutrons", 800,400);
    // // cLO->Divide(1,2);
    // c_LOns->cd();
    //
    // h_neutronLightOutputExp->Draw();
    // h_neutronLightOutputSim->Draw("SAME");
    //
    // TLegend *legendN = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    // legendN->AddEntry("h_neutronLightOutputExp", "Experimental Neutron");
    // legendN->AddEntry("h_neutronLightOutputSim", "Simulated Neutron");
    // legendN->Draw("SAME");
    //
    // TRatioPlot *ratN = new TRatioPlot(h_neutronLightOutputSim, h_neutronLightOutputExp);
    // ratN->Draw("SAME");
    //
    // c_LOns->Write();
    //
    //
    // //photons only
    // TCanvas* c_LOps = new TCanvas("cLOps", "Light Output Photons", 800,400);
    // // cLO->Divide(1,2);
    // c_LOps->cd();
    //
    // h_photonLightOutputExp->Draw();
    // h_photonLightOutputSim->Draw("SAME");
    //
    // TLegend *legendP = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    // legendP->AddEntry("h_photonLightOutputExp", "Experimental Photon");
    // legendP->AddEntry("h_photonLightOutputSim", "Simulated Photon");
    // legendP->Draw("SAME");
    //
    // TRatioPlot *ratP = new TRatioPlot(h_photonLightOutputSim, h_photonLightOutputExp);
    // ratP->Draw("SAME");
    //
    // c_LOps->Write();

}

void readFiss::CompareTof()
{

    writeFile->cd();
    cd_simComparison->cd();

    cout << "Comparing Time of Flight." << endl;

    //create a canvas

     //create a canvas
    TCanvas* c_ToFs = new TCanvas("cToFs", "Time of Flight", 800, 400);
    // cLO->Divide(1,2);
    c_ToFs->cd();


    h_neutronTofExp->SetLineColor(kBlue);
    h_neutronTofExp->Draw();

    h_photonTofExp->SetLineColor(kRed);
    h_photonTofExp->Draw("SAME");

    h_neutronTofSim->Scale((double)expEntries/(double)simEntries);
    h_neutronTofSim->SetLineStyle(kDashed);
    h_neutronTofSim->SetLineColor(kBlue);
    h_neutronTofSim->Draw("SAME");

    h_photonTofSim->Scale((double)expEntries/(double)simEntries);
    h_photonTofSim->SetLineStyle(kDashed);
    h_photonTofSim->SetLineColor(kRed);
    h_photonTofSim->Draw("SAME");

    // TRatioPlot* rat = new TRatioPlot(h_neutronTofExp, h_neutronTofSim);
    // rat->Draw("SAME");

    TLegend* legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronTofExp", "Experimental Neutron");
    legend->AddEntry("h_photonTofExp", "Experimental Photon");
    legend->AddEntry("h_neutronTofSim", "Simulated Neutron");
    legend->AddEntry("h_photonTofSim", "Simulated Photon");
    legend->Draw("SAME");

    c_ToFs->Write();
    //c_ToFs->SaveAs("fig/TimeOfFlightSim.eps");
}

void readFiss::CompareErg()
{
    writeFile->cd();
    cd_simComparison->cd();

    cout << "Comparing Energy." << endl;

    //create a canvas
    TCanvas* c_Ergs = new TCanvas("cErgs", "Energy", 800, 400);
    // cLO->Divide(1,2);
    c_Ergs->cd();

    h_neutronEnergyExp->SetLineColor(kBlue);
    h_neutronEnergyExp->Draw();

    h_neutronEnergySim->Scale((double)expEntries/(double)simEntries);
    h_neutronEnergySim->SetLineColor(kBlue);
    h_neutronEnergySim->SetLineStyle(kDashed);
    h_neutronEnergySim->Draw("SAME");

    TLegend* legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronEnergyExp", "Experimental Neutron");
    legend->AddEntry("h_neutronEnergySim", "Simulated Neutron");
    legend->Draw("SAME");

    c_Ergs->Write();
    // c_Ergs->SaveAs("fig/NeutronErgSim.eps");
}

void readFiss::CompareMult()
{
    writeFile->cd();
    cd_simComparison->cd();
    cout << "Comparing Multiplicity." << endl;

    //create a canvas
    TCanvas* c_Mults = new TCanvas("cMults", "Multiplicity", 800, 400);
    // cLO->Divide(1,2);

    c_Mults->cd();

    h_photonMultExp->SetLineColor(kRed);
    h_photonMultExp->Draw();

    h_photonMultSim->Scale((double)expEntries/(double)simEntries);
    h_photonMultSim->SetLineColor(kRed);
    h_photonMultSim->SetLineStyle(kDashed);
    h_photonMultSim->Draw("SAME");

    h_neutronMultExp->SetLineColor(kBlue);
    h_neutronMultExp->Draw("SAME");

    h_neutronMultSim->Scale((double)expEntries/(double)simEntries);
    h_neutronMultSim->SetLineColor(kBlue);
    h_neutronMultSim->SetLineStyle(kDashed);
    h_neutronMultSim->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronMultSim", "Experimental Neutron");
    legend->AddEntry("h_photonMultSim", "Experimental Photon");
    legend->AddEntry("h_neutronMultSim", "Simulated Neutron");
    legend->AddEntry("h_photonMultSim", "Simulated Photon");
    legend->Draw("SAME");

    c_Mults->Write();
    //c_Mults->SaveAs("fig/MultiplicitySim.eps");
}


void readFiss::ComparePSD()
{
    writeFile->cd();
    cd_simComparison->cd();
    cout << "Comparing Discrimination." << endl;

    //create a canvas
    TCanvas* c_PSDs = new TCanvas("cPSDs", "Particle Dicrimination", 800, 400);
    // cLO->Divide(1,2);

    c_PSDs->cd();

    h_photonPSDExp->SetLineColor(kRed);
    h_photonPSDExp->Draw();

    h_photonPSDSim->Scale((double)expEntries/(double)simEntries);
    h_photonPSDSim->SetLineColor(kRed);
    h_photonPSDSim->SetLineStyle(kDashed);
    h_photonPSDSim->Draw("SAME");

    h_neutronPSDExp->SetLineColor(kBlue);
    h_neutronPSDExp->Draw("SAME");

    h_neutronPSDSim->Scale((double)expEntries/(double)simEntries);
    h_neutronPSDSim->SetLineColor(kBlue);
    h_neutronPSDSim->SetLineStyle(kDashed);
    h_neutronPSDSim->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronPSDExp", "Experimental Neutron");
    legend->AddEntry("h_photonPSDExp", "Experimental Photon");
    legend->AddEntry("h_neutronPSDSim", "Simulated Neutron");
    legend->AddEntry("h_photonPSDSim", "Simulated Photon");
    legend->Draw("SAME");

    c_PSDs->Write();
    //c_PSDs->SaveAs("fig/ParticleDiscriminationSim.eps");
}

void readFiss::CompareSingles()
{
    writeFile->cd();
    cd_simComparison->cd();
    cout << "Comparing Singles." << endl;

    //create a canvas
    TCanvas* c_Sins = new TCanvas("cSins", "Singles Rates", 800, 400);
    // cLO->Divide(1,2);

    c_Sins->cd();

    h_photonSinglesExp->SetLineColor(kRed);
    h_photonSinglesExp->Draw();

    h_photonSinglesSim->Scale((double)expEntries/(double)simEntries);
    h_photonSinglesSim->SetLineColor(kRed);
    h_photonSinglesSim->SetLineStyle(kDashed);
    h_photonSinglesSim->Draw("SAME");

    h_neutronSinglesExp->SetLineColor(kBlue);
    h_neutronSinglesExp->Draw("SAME");

    h_neutronSinglesSim->Scale((double)expEntries/(double)simEntries);
    h_neutronSinglesSim->SetLineColor(kBlue);
    h_neutronSinglesSim->SetLineStyle(kDashed);
    h_neutronSinglesSim->Draw("SAME");

    TLegend *legend = new TLegend(legend_x1, legend_y1, legend_x2, legend_y2);
    legend->AddEntry("h_neutronSinglesExp", "Experimental Neutron");
    legend->AddEntry("h_photonSinglesExp", "Experimental Photon");
    legend->AddEntry("h_neutronSinglesSim", "Simulated Neutron");
    legend->AddEntry("h_photonSinglesSim", "Simulated Photon");
    legend->Draw("SAME");

    c_Sins->Write();
    //c_Sins->SaveAs("fig/SinglesSim.eps");
}
