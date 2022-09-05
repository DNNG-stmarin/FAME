#include "readFiss.h"

#include <TLegend.h>
#include <TCanvas.h>
#include <TRatioPlot.h>
#include <TPaveStats.h>

using namespace std;

void readFiss::PlotBeamTime()
{
  cd_beamTime->cd();
  cout << "Plotting time behavior of beam" << endl;

  TCanvas** c_beamTime = new TCanvas* [NUM_TRIGGERS];

  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
    TString s_TRIG_NUM = (TString)to_string(r);

    c_beamTime[r] = new TCanvas("beamTimeCh"+ s_TRIG_NUM, "beamTime", 1200, 800);

    h_beamTime[r]->SetLineColor(kBlue);
    h_alphaDep[r]->SetLineColor(kGreen);
    h_fisSubtract[r]->SetLineColor(kBlack);


    //Draw both fits on general fisDep hisgram
     c_beamTime[r] ->cd();
     gPad->SetLogy();
     h_beamTime[r]->Draw();

     c_beamTime[r]->Write();
  }
}

void readFiss::PlotDepSubtraction()
{
  cd_alphaSub->cd();
  cout << "Plotting subtracted spectra " << endl;

  TCanvas** c_Alpha = new TCanvas* [NUM_TRIGGERS];

  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
    TString s_TRIG_NUM = (TString)to_string(r);

    c_Alpha[r] = new TCanvas("AlphaSubtraction_Channel_"+ s_TRIG_NUM, "Fission Alpha Ratios", 1200, 800);
    c_Alpha[r]->Divide(2,2);

    h_fisDep[r]->SetLineColor(kBlue);
    h_alphaDep[r]->SetLineColor(kGreen);
    h_fisSubtract[r]->SetLineColor(kBlack);


    //Draw both fits on general fisDep hisgram
     c_Alpha[r]->cd(1);
     gPad->SetLogy(1);
     h_fisDep[r]->Draw();											//General plot of fisDep
     h_fisSubtract[r]->Draw("SAME");
     f_gauss[r]->Draw("SAME");								//Gaussian fis of fissions
     f_expo[r]->Draw("SAME");								//Exponential fit of alphas

     TLegend *leg = new TLegend(0.9,0.6,0.5,0.75);
     leg->AddEntry("h_fisDep" + s_TRIG_NUM, "Total", "l");
     leg->AddEntry("f_gaus" +  s_TRIG_NUM, "Fission Products", "l");
     leg->AddEntry("f_expo" + s_TRIG_NUM, "Alpha Background", "l");
     leg->Draw();

    //Draw Vertical Line at intersection
     //TLine *intersection = new TLine(0.00623,0,0.00623,100000);	//Define intersections line of gaussian (fissions) and exponential (alphas)
     //intersection->Draw("SAME");

    //Draw other canvas entries
     c_Alpha[r]->cd(2);												//Second canvas assignment
     h_fisSubtract[r]->Draw("HIST");							//Isolated fissions plot
     f_gauss[r]->Draw("SAME");							//Fissions fit
     h_fisSubtract[r]->SetTitle("Fissions without Alphas;Pulse Integral (V us); Count Rate");	//Title

     c_Alpha[r]->cd(3);							//Third canvas assignment
     gPad->SetLogy(3);
     h_alphaDep[r]->Draw("HIST");			//Isolated alpha background plot
     f_expo[r]->Draw("SAME");		//Alpha background fit
     h_alphaDep[r]->SetTitle("Alpha spec vs. chan; Pulse Integral (V us); Count Rate");

     // c_Alpha->cd(4);																			//Fourth canvas assignment
     //
     // TLine *beamRangeFis1 = new TLine(FIS_RANGE_LOW,0,FIS_RANGE_LOW,10000);				//Define line indicating lower limit of fissions interval
     // TLine *beamRangeFis2 = new TLine(FIS_RANGE_HIGH,0,FIS_RANGE_HIGH,10000);			//Define line indicating upper limit of fissions interval
     // beamRangeFis1->Draw("SAME");														//Draw lower limit fissions line
     // beamRangeFis2->Draw("SAME");														//Draw upper limit fissions line
     // beamRangeFis1->SetLineColor(kGreen);
     // beamRangeFis2->SetLineColor(kGreen);

     c_Alpha[r]->Write();
  }
}

void readFiss::PlotRatioMult()
{
  cd_multRatio->cd();
  cout << "Plotting corrected multiplicities " << endl;

  TCanvas** c_depRatio = new TCanvas* [NUM_TRIGGERS];

  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
    TString s_TRIG_NUM = (TString)to_string(r);

    c_depRatio[r] = new TCanvas("MultiplicityRatio_Channel_" + s_TRIG_NUM, "Neutron/Gamma Multiplicity Comparision to Fission Fraction", 600, 800);
    c_depRatio[r]->Divide(1,2);

    c_depRatio[r]->cd(1);
    g_fisRatioSelect[r]->Draw();										//Draw segmented energy fission alpha ratios
    p_neutronMultDep[r]->Draw("SAME");								//Draw profile of neutronMult with ratio of fissions to alphas
    p_gammaMultDep[r]->Draw("SAME");									//Draw profile of gammaMult with ratio of fissions to alphas

    g_fisRatioSelect[r]->SetLineColor(kBlack);		//**
    p_neutronMultDep[r]->SetLineColor(kBlue);	//**
    p_gammaMultDep[r]->SetLineColor(kRed);		//**

    TLegend *segLeg = new TLegend(0.4,0.7,0.15,0.88);			//Define Legend
    segLeg->AddEntry("fissRatioSelect" + s_TRIG_NUM,"Fission Fraction","l");
    segLeg->AddEntry("p_neutronMultDep" + s_TRIG_NUM,"Mean Neutron Mult","l");
    segLeg->AddEntry("p_gammaMultDep"+ s_TRIG_NUM,"Mean Gamma Mult","l");
    segLeg->Draw();												//Draw Legend

    c_depRatio[r]->cd(2);
    g_neutronMultRatioDep[r]->Draw();
    g_gammaMultRatioDep[r]->Draw("SAME");

    g_neutronMultRatioDep[r]->SetLineColor(kBlue);	//**
    g_gammaMultRatioDep[r]->SetLineColor(kRed);	//**

    TLegend *ratLeg = new TLegend(0.4,0.7,0.15,0.88);			//Define Legend
    ratLeg->AddEntry("g_neutronMultRatioDep"+ s_TRIG_NUM,"Mean Fiss Neutron Mult","l");
    ratLeg->AddEntry("g_gammaMultRatioDep"+ s_TRIG_NUM,"Mean Fiss Gamma Mult","l");
    ratLeg->Draw();


    c_depRatio[r]->Write();
  }
}

void readFiss::PlotMultErg()
{
  cd_multErg->cd();
  cout << "Plotting incident-dependent multiplicities " << endl;

  TCanvas** c_incMult = new TCanvas* [NUM_TRIGGERS];


  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
    TString s_TRIG_NUM = (TString)to_string(r);

    c_incMult[r] = new TCanvas("MultCorrectedErg_Channel_" + s_TRIG_NUM, "Neutron/Gamma Multiplicity Comparision to Fission Fraction", 600, 800);
    c_incMult[r]->Divide(1,2);

    c_incMult[r]->cd(1);
    g_fisRatioErg[r]->Draw("ALP");
    p_neutronMultErg[r]->Draw("SAME");
    p_gammaMultErg[r]->Draw("SAME");
    p_backNeutronMultErg[r]->Draw("SAME");
    p_backGammaMultErg[r]->Draw("SAME");

    g_fisRatioErg[r]->SetLineColor(kBlack);
    g_fisRatioErg[r]->SetMarkerColor(kBlack);
    g_fisRatioErg[r]->SetMarkerSize(0.5);
    g_fisRatioErg[r]->SetMarkerStyle(21);
    g_fisRatioErg[r]->GetYaxis()->SetRangeUser(0., 1.);

    p_neutronMultErg[r]->SetLineColor(kBlue);

    p_gammaMultErg[r]->SetLineColor(kRed);
    p_backNeutronMultErg[r]->SetLineColor(kBlue);
    p_backNeutronMultErg[r]->SetLineStyle(kDashed);
    p_backGammaMultErg[r]->SetLineColor(kRed);
    p_backGammaMultErg[r]->SetLineStyle(kDashed);

    TLegend *segLeg = new TLegend(0.4,0.7,0.15,0.88);			//Define Legend
    segLeg->AddEntry("g_fisRatioErg" + s_TRIG_NUM, "Fission Fraction","l");
    segLeg->AddEntry("p_neutronMultErg" + s_TRIG_NUM,"Mean Neutron Mult","l");
    segLeg->AddEntry("p_gammaMultErg"+ s_TRIG_NUM,"Mean Gamma Mult","l");
    segLeg->AddEntry("p_backNeutronMultErg" + s_TRIG_NUM,"Mean Background Neutron Mult","l");
    segLeg->AddEntry("p_backGammaMultErg"+ s_TRIG_NUM,"Mean Background Gamma Mult","l");
    segLeg->Draw();												//Draw Legend

    c_incMult[r]->cd(2);
    g_nMultErg[r]->Draw("ALP");
    g_nMultErg[r]->GetYaxis()->SetRangeUser(0., 0.2);
    g_gMultErg[r]->Draw("SAME LP");
    g_nMultBackErg[r]->Draw("SAME LP");
    g_gMultBackErg[r]->Draw("SAME LP");

    g_nMultErg[r]->SetLineColor(kBlue);
    g_nMultErg[r]->SetMarkerColor(kBlue);
    g_nMultErg[r]->SetMarkerSize(0.5);
    g_nMultErg[r]->SetMarkerStyle(21);

    g_gMultErg[r]->SetLineColor(kRed);
    g_gMultErg[r]->SetMarkerColor(kRed);
    g_gMultErg[r]->SetMarkerSize(0.5);
    g_gMultErg[r]->SetMarkerStyle(22);

    g_nMultBackErg[r]->SetLineColor(kBlue);
    g_nMultBackErg[r]->SetLineStyle(kDashed);
    g_nMultBackErg[r]->SetMarkerColor(kBlue);
    g_nMultBackErg[r]->SetMarkerSize(0.5);
    g_nMultBackErg[r]->SetMarkerStyle(21);

    g_gMultBackErg[r]->SetLineColor(kRed);
    g_gMultBackErg[r]->SetLineStyle(kDashed);
    g_gMultBackErg[r]->SetMarkerColor(kRed);
    g_gMultBackErg[r]->SetMarkerSize(0.5);
    g_gMultBackErg[r]->SetMarkerStyle(22);

    c_incMult[r]->Update();

    TLegend *ratLeg = new TLegend(0.4,0.7,0.15,0.88);			//Define Legend
    ratLeg->AddEntry("g_nMultErg" + s_TRIG_NUM, "Fission Neutrons","l");
    ratLeg->AddEntry("g_gMultErg" + s_TRIG_NUM,"Fission Gammas","l");
    ratLeg->AddEntry("g_nMultBackErg"+ s_TRIG_NUM,"Background Neutrons","l");
    ratLeg->AddEntry("g_gMultBackErg" + s_TRIG_NUM,"Background Gammas","l");
    ratLeg->Draw();												//Draw Legend

    c_incMult[r]->Write();
  }

  cout << "Plotting gamma vs. neutron multiplicities " << endl;

  TCanvas** c_ngMult =  new TCanvas* [NUM_TRIGGERS];
  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
    TString s_TRIG_NUM = (TString)to_string(r);

    c_ngMult[r] = new TCanvas("MultGammaPerNeutronErg_Channel_" + s_TRIG_NUM, "Gamma/Neutron Multiplicity", 600, 400);
    c_ngMult[r]->cd();

    g_gMultnMult[r]->Draw("AP");
    g_gMultnMult[r]->SetLineColor(kBlack);
    g_gMultnMult[r]->SetMarkerColor(kBlack);
    g_gMultnMult[r]->SetMarkerSize(1);
    g_gMultnMult[r]->SetMarkerStyle(20);

    c_ngMult[r]->Write();
  }

  cout << "Plotting gammas per neutron vs. PPAC Channel" << endl;

  TCanvas* c_gnRatio = new TCanvas;
  c_gnRatio = new TCanvas("GammasPerNeutronForPPACs", "GammasPerNeutronDueToPPAC", 600, 800);
  c_gnRatio->Divide(1,2);
  c_gnRatio->cd(1);

  g_gnRatio->Draw("AP");
  g_gnRatio->SetTitle("Gammas Per Neutron vs. PPAC Channel; PPAC Channel; g/n Fraction");
  g_gnRatio->SetMarkerColor(kBlack);
  g_gnRatio->SetMarkerSize(1);
  g_gnRatio->SetMarkerStyle(20);

  c_gnRatio->cd(2);
  g_nRatioSlopeInt->SetMarkerSize(2);
  g_nRatioSlopeInt->SetMarkerStyle(34);
  g_nRatioSlopeInt->SetMarkerColor(kBlue);

  g_gRatioSlopeInt->SetMarkerSize(2);
  g_gRatioSlopeInt->SetMarkerStyle(21);
  g_gRatioSlopeInt->SetMarkerColor(kRed);

  g_nRatioSlopeInt->Draw("AP");
  g_gRatioSlopeInt->Draw("SAMEP");
  g_nRatioSlopeInt->SetTitle("Neutron Slope/Int; PPAC Channel; Slope/Int");
  g_gRatioSlopeInt->SetTitle("Gamma Slope/Int");
  gPad->BuildLegend();
  g_nRatioSlopeInt->SetTitle("Neutron and Gamma Slope/Int vs. PPAC Channel");

  c_gnRatio->Write();
}
void readFiss::PlotStack()
{
  cd_stack->cd();
  cout << "Plotting stack" << endl;

  TCanvas** c_stack =  new TCanvas* [NUM_TRIGGERS];
  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
    TString s_TRIG_NUM = (TString)to_string(r);

  c_stack[r] = new TCanvas("FissionSpectrumDueToBeamErg_Chan_" + s_TRIG_NUM, "Fission Spectrum due to Beam Energies", 600, 400);
  c_stack[r]->Divide(1,1);
  c_stack[r]->cd(1);
  stack[r]->Draw("PADS");

  c_stack[r]->Write();
  }
}

void readFiss::PlotLO()
{
  cd_LO->cd();
  cout << "Plotting neutronLightOutput, neutronToF, and photonLightOut" << endl;

  TCanvas** c_vsbeamErg =  new TCanvas* [NUM_TRIGGERS];
  for (int r = 0; r < NUM_TRIGGERS; r++)
	{
    TString s_TRIG_NUM = (TString)to_string(r);

    c_vsbeamErg[r] = new TCanvas("nLightOutErgProj_pLightOutErgProj_Channel_" + s_TRIG_NUM, "nLO and photonLO vs beamEnergy for Channel " + s_TRIG_NUM, 800, 1000);
    c_vsbeamErg[r]->Divide(1,2);

    c_vsbeamErg[r]->cd(1);

    double separationErg = 4*(BEAM_ERG_MAX - BEAM_ERG_MIN)/BEAM_ERG_BINNUM;
    pj_pLightOutErg[r][0]->Rebin(5);
    pj_pLightOutErg[r][0]->Draw();
    pj_pLightOutErg[r][0]->SetLineColor(1);
    pj_pLightOutErg[r][0]->SetTitle("IncErg Range " + (TString)to_string(BEAM_ERG_MIN) + "-" + (TString)to_string(BEAM_ERG_MIN + separationErg)  + " (MeV)" );
    for (int i = 1; i < BEAM_ERG_BINNUM/4; i++)
    {
      TString ergRLow = (TString)to_string(BEAM_ERG_MIN + separationErg*i);
      TString ergRHigh = (TString)to_string(BEAM_ERG_MIN + separationErg*(i+1));
      pj_pLightOutErg[r][i]->Rebin(5);
      pj_pLightOutErg[r][i]->Draw("SAME");
      pj_pLightOutErg[r][i]->SetLineColor(i + 1);
      pj_pLightOutErg[r][i]->SetTitle("IncErg Range: " + ergRLow + "-" + ergRHigh + " (MeV)");
    }
    gPad->BuildLegend();
    pj_pLightOutErg[r][0]->SetTitle("Incident Neutron Energy Dependent Photon Light Output; Photon Light Output (MeVee); Integral Normalized Counts From Projection (Counts/area)");

    c_vsbeamErg[r]->cd(2);

    pj_nLightOutErg[r][0]->Rebin(5);
    pj_nLightOutErg[r][0]->Draw();
    pj_nLightOutErg[r][0]->SetLineColor(1);
    pj_nLightOutErg[r][0]->SetTitle("IncErg Range " + (TString)to_string(BEAM_ERG_MIN) + "-" + (TString)to_string(BEAM_ERG_MIN + separationErg) + " (MeV)" );
    for (int i = 1; i < BEAM_ERG_BINNUM/4; i++)
    {
      TString ergRLow = (TString)to_string(BEAM_ERG_MIN + separationErg*i);
      TString ergRHigh = (TString)to_string(BEAM_ERG_MIN + separationErg*(i+1));
      pj_nLightOutErg[r][i]->Rebin(5);
      pj_nLightOutErg[r][i]->Draw("SAME");
      pj_nLightOutErg[r][i]->SetLineColor(i + 1);
      pj_nLightOutErg[r][i]->SetTitle("IncErg Range: " + ergRLow + "-" + ergRHigh + " (MeV)");
    }
    gPad->BuildLegend();
    pj_nLightOutErg[r][0]->SetTitle("Incident Neutron Energy Dependent Neutron Light Output; Neutron Light Output (MeVee); Integral Normalized Counts From Projection (Counts/area)");

    c_vsbeamErg[r]->Write();
  }
}

void readFiss::PlotBeamLO()
{
    cd_gammaSpecBeam->cd();
    cout << "Plotting Beam PhotonLO Energy" << endl;

    TCanvas** c_LOvsBeamErg =  new TCanvas* [NUM_TRIGGERS];
    // cout << NUM_TRIGGERS << endl;
    for (int r = 0; r < NUM_TRIGGERS; r++)
  	{
      TString s_TRIG_NUM = (TString)to_string(r);

      c_LOvsBeamErg[r] = new TCanvas("LOScaledBeam_Channel_" + s_TRIG_NUM, "photonLO vs beamEnergy for Channel " + s_TRIG_NUM, 2000, 1000);
      //c_LOvsBeamErg[r]->Divide(3,1);

      c_LOvsBeamErg[r]->cd();

      h2_gammaLightOutErg[r]->Draw("COLZ");
      c_LOvsBeamErg[r]->Write();
      // cout << "Written canvas " << r << endl;
    }



    cout << "Plotting Beam vs NeutronEn" << endl;

    TCanvas** c_EnvsBeamErg =  new TCanvas* [NUM_TRIGGERS];
    for (int r = 0; r < NUM_TRIGGERS; r++)
  	{
      TString s_TRIG_NUM = (TString)to_string(r);

      c_EnvsBeamErg[r] = new TCanvas("EnScaledBeam_Channel_" + s_TRIG_NUM, "Neutron Energy vs beamEnergy for Channel " + s_TRIG_NUM, 2000, 1000);
      // c_LOvsBeamErg[r]->Divide(3,1);

      c_EnvsBeamErg[r]->cd();
      h2_nToFErg[r]->Draw("COLZ");
      c_EnvsBeamErg[r]->Write();
      // cout << "Written canvas " << r << endl;

    }
}
