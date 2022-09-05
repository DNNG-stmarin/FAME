/*
Author: Stefano Marin, Isabel Hernandez
Purpose: Perform the analyisis of the collected data on a detector-by-detector basis.
The result of this code will be a series of cuts to clean the data.

So far, we look at:
- PSD characterization
- Timing delays
*/

#define DetectionAnalysis_cxx

#include "DetectorSystemClass.h"

#include <TH3.h>
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TCut.h>
#include <TFitResult.h>
#include <THStack.h>
#include <TROOT.h>
#include "Math/ProbFunc.h"

#include <TProfile.h>
#include <TGraph.h>
#include <TLine.h>
#include <TFormula.h>

#include <stdlib.h>
#include <stdio.h>
#include <queue>


#include "ParticleEvent.h"
#include "TriggerEvent.h"
#include "ProcessingConstants.h"


using namespace std;
using namespace ROOT::Math;

int DetectorSystemClass::DetectionAnalysis()
{
	/*
	    _____      _
	  / ____|    | |
	 | (___   ___| |_ _   _ _ __
	  \___ \ / _ \ __| | | | '_ \
	  ____) |  __/ |_| |_| | |_) |
	 |_____/ \___|\__|\__,_| .__/
	                       | |
	                       |_|
	*/


	// openFile and create tree
	TString filePrefix = "FissionOutput";
	TString fileInName;

	/*
	  __  __      _        _
	 |  \/  |__ _(_)_ _   | |   ___  ___ _ __
	 | |\/| / _` | | ' \  | |__/ _ \/ _ \ '_ \
	 |_|  |_\__,_|_|_||_| |____\___/\___/ .__/
	                                    |_|
	*/
	cout << "opening det file " << endl;
	detFile->cd();
	int channelDet, channelTrig;
	double channelCalib;

	if (tree == 0) return -1;

	Long64_t nentries = tree->GetEntriesFast();
	// nentries = 10000000;
	Long64_t nbytes = 0, nb = 0;

	cout << "beginnning loop det " << endl;

	// loop through and compute delays and properties
	for (Long64_t jentry=0; jentry<nentries;jentry++)
	{
	 // load tree
	 Long64_t ientry = LoadTree(jentry);
	 if (ientry < 0) break;
	 nb = tree->GetEntry(jentry);   nbytes += nb;


	 // store the channel of the fission trigger
	 channelTrig = isTrigger(tChan);

	 if(jentry % 1000000 == 0)
	 {
	 	cout << jentry << " entries processed." << endl;
	 }

	 // broken event has been found, skip it.
	 if(tMult > NUM_DETS)
	 {
		 cout << "Encountered a corrupt event at history: " << jentry << endl;
		 cout << tMult << endl;
		 continue;
	 }


	 // beam parameters are used here
	 if(NUM_BEAMS > 0)
	 {
		 beamTimeHist[channelTrig]->Fill(bTime);
	 }

	 //
	 for(int part = 0; part < tMult; part++)
	 {
		 // store the channel number
		 channelDet = isDetector(totChan[part]);
		 channelCalib = detectors[channelDet].calibration;
		 if(SIM_FILE)
		 {
			 channelDet = totChan[part];
			 channelCalib = 1;
		 }
		 // 1D Histograms
		 psdhists[channelDet]->Fill(totPSP[part]); // psd histogram
		 erghists[channelDet]->Fill(totDep[part]/channelCalib); // energy histogram
		 tofDelPhists[channelDet][channelTrig]->Fill(totToF[part]); // tof histograms

		 // 2D histograms
		 psdErgHists[channelDet]->Fill(totDep[part]/channelCalib, totPSP[part]); // energy-psd histogram
		 tofPsdHists[channelDet]->Fill(totPSP[part], totToF[part]); // psd-tof histograms
		 tofErgHists[channelDet]->Fill(totDep[part]/channelCalib, totToF[part]); // kinematic histogram
	 }
  }
	cout << "Finished psd filling loop" << endl;


/*
 ___                   _
| _ ) ___ __ _ _ __   | |   ___  ___ _ __
| _ \/ -_) _` | '  \  | |__/ _ \/ _ \ '_ \
|___/\___\__,_|_|_|_| |____\___/\___/ .__/
																	 |_|

*/
  if(NUM_BEAMS > 0)
	{

		cdBeam->cd();
		TF1* fisBeamActivity = new TF1("fisBeamActivity", "landau + [0]");
		fisBeamActivity->SetParNames("baseline", "center", "width");

		const double WINDOW_FIT_FIS_BEAM = 200;
		const double WIDTH_FIS_BEAM = 10;
		for(int trig = 0; trig < NUM_TRIGGERS; trig++)
		{
			// find the macroscopic properties
			double maxCounts = beamTimeHist[trig]->GetMaximum();
			int maxBin = beamTimeHist[trig]->GetMaximumBin();

			// place an iterator
			int maxShift = 100; // put in input file
			int currentBin = maxBin - maxShift;
			double currentCount = beamTimeHist[trig]->GetBinContent(currentBin);


			// calculate the baseline
			double baselineAverage = 0; // think of doing a TGraph, continuum subtraction and nice visual
			int numBaselinePoints = 10; // put in input file

			for(int binBase = currentBin; binBase < currentBin + numBaselinePoints; binBase++)
			{
				baselineAverage += beamTimeHist[trig]->GetBinContent(binBase);
			}
			baselineAverage /= numBaselinePoints;

			triggers[trig].backgroundActivity = baselineAverage;

			cout << "baseline count for trigger" << trig <<" is " << triggers[trig].backgroundActivity << "counts/ns." << endl;


			currentCount -= baselineAverage;
			maxCounts -= baselineAverage;

			while(currentCount < maxCounts/2)
			{
				currentBin++;
				currentCount = beamTimeHist[trig]->GetBinContent(currentBin) - baselineAverage;
			}

			triggers[trig].beamDelay = beamTimeHist[trig]->GetBinCenter(currentBin);

			cout << "delay of trigger " << trig << " is " << 	triggers[trig].beamDelay << " ns." << endl;

			fisBeamActivity->SetRange(triggers[trig].beamDelay - WINDOW_FIT_FIS_BEAM, triggers[trig].beamDelay + WINDOW_FIT_FIS_BEAM);
			fisBeamActivity->SetParameter(0, baselineAverage);
			fisBeamActivity->SetParameter(1, triggers[trig].beamDelay);
			fisBeamActivity->SetParameter(2, WIDTH_FIS_BEAM);
			fisBeamActivity->SetLineColor(kGreen + 3);

			// beamTimeHist[trig]->Fit(fisBeamActivity, "S Q R");
			triggers[trig].fisBeamActivity = (TF1*)fisBeamActivity->Clone();

			// visualizations

			TString nameCanvBeam;
			TString s_canvBeam = "beamTimeTrig";
			nameCanvBeam = s_canvBeam + to_string(trig);

			TCanvas* canvBeam = new TCanvas(nameCanvBeam, nameCanvBeam, 800, 500);
			canvBeam->cd();

			beamTimeHist[trig]->SetTitle("Time difference with beam; time (ns); counts");
			beamTimeHist[trig]->Draw();

			TLine* g_baseLine = new TLine(-1*BEAM_WINDOW, baselineAverage, BEAM_WINDOW, baselineAverage);
			g_baseLine->SetLineColor(kRed);
			g_baseLine->Draw("SAME");


			TLine* g_delay = new TLine(triggers[trig].beamDelay, 0, triggers[trig].beamDelay, 10*maxCounts);
			g_delay->SetLineColor(kGreen);
			g_delay->Draw("SAME");

			cout << "writing activity with " << fisBeamActivity->GetParameter(1) << endl;
			triggers[trig].fisBeamActivity->Draw("SAME");

			canvBeam->Write();

		}

	}


	/*
	            )   (         (    (    (
	  *   )  ( /(   )\ )      )\ ) )\ ) )\ )
	` )  /(  )\()) (()/(     (()/((()/((()/(
	 ( )(_))((_)\   /(_))  __ /(_))/(_))/(_))
	(_(_())   ((_) (_))_| / /(_)) (_)) (_))_
	|_   _|  / _ \ | |_  / / | _ \/ __| |   \
	  | |   | (_) || __|/_/  |  _/\__ \ | |) |
	  |_|    \___/ |_|       |_|  |___/ |___/
	*/

		cdToF->cd();
		// individual
		TFitResultPtr tofphotpeak_opt;
		TFitResultPtr tofneutpeak_opt;
		double tracktof[NUM_DETS];
		double trackpsd[NUM_DETS];


		cout << "Applying standard ToF PSD analysis" << endl;


		//individual discrimination for each detector loop
		for(int tr = 0; tr < NUM_TRIGGERS; tr++)
		{
			for(int i=0; i<NUM_DETS; i++)
			{
				// _____
				// Find the delay in the ToF distribution
				// _____

				// cout << "Detector " << i << endl;
				TString tofpsdname = "TOF_PSD" + to_string(i) + "trig" + to_string(tr);
				TCanvas* tofpsdcanvas = new TCanvas(tofpsdname, tofpsdname, 800, 500);

				int totbin = tofDelPhists[i][tr]->GetNbinsX();
				int binlim = tofDelPhists[i][tr]->FindLastBinAbove(0);
				int binmin = tofDelPhists[i][tr]->FindFirstBinAbove(0);
				double xlim = tofDelPhists[i][tr]->GetXaxis()->GetBinCenter(binlim);
				double xmin = tofDelPhists[i][tr]->GetXaxis()->GetBinCenter(binmin);

				TF1* tofphotpeak = new TF1("peakFit", "[0]*e^(-(x - [1])^2/(2*[2]^2)) + [3]", xmin, xlim);
				TF1* tofneutpeak = new TF1("npeakFit", "[0]/(1 + ((x - [1])/([2]))^2) + [4]", xmin, xlim);
				TF1* tofintersection = new TF1("TOFintersect", "abs(npeakFit - peakFit)", xmin, xlim);

				//initial parameters photon peak
				int ctMax = tofDelPhists[i][tr]->GetMaximum();
				int binMax = tofDelPhists[i][tr]->GetMaximumBin();
				double timePeak = tofDelPhists[i][tr]->GetBinCenter(binMax);

				//fit and optimize photon peak
				tofphotpeak->SetParameter(0, ctMax);
				tofphotpeak->SetParameter(1, timePeak);
				tofphotpeak->SetParameter(2, 1);
				tofphotpeak_opt = tofDelPhists[i][tr]->Fit(tofphotpeak, "SQ");
				tofphotpeak->SetParameter(0, tofphotpeak_opt->Parameter(0));
				tofphotpeak->SetParameter(1, tofphotpeak_opt->Parameter(1));
				tofphotpeak->SetParameter(2, tofphotpeak_opt->Parameter(2));

				tofDelPhists[i][tr]->GetXaxis()->SetRangeUser(timePeak+10, xlim);

				//initial parameters neutron peak
				double neuMax = tofDelPhists[i][tr]->GetMaximum();
				double nbinMax = tofDelPhists[i][tr]->GetMaximumBin();
				double ntimePeak = tofDelPhists[i][tr]->GetBinCenter(nbinMax);

				//fit and optimize neutron peak
				tofneutpeak->SetParameter(0, neuMax);
				tofneutpeak->SetParameter(1, ntimePeak);
				tofneutpeak->SetParameter(2, tofDelPhists[i][tr]->GetStdDev());
				tofneutpeak_opt = tofDelPhists[i][tr]->Fit(tofneutpeak, "+SQ");
				tofneutpeak->SetParameter(0, tofneutpeak_opt->Parameter(0));
				tofneutpeak->SetParameter(1, tofneutpeak_opt->Parameter(1));
				tofneutpeak->SetParameter(2, tofneutpeak_opt->Parameter(2));

				tofDelPhists[i][tr]->GetXaxis()->SetRangeUser(xmin, xlim);

				//intersection
				tofintersection->SetParameters(tofphotpeak->GetParameter(0), tofphotpeak->GetParameter(1), tofphotpeak->GetParameter(2), tofneutpeak->GetParameter(0), tofneutpeak->GetParameter(1), tofneutpeak->GetParameter(2));
				tofintersection->SetLineColor(kGreen);

				double toftempPSD = tofintersection->GetMinimumX(tofDelPhists[i][tr]->GetXaxis()->GetBinCenter(binMax), tofDelPhists[i][tr]->GetXaxis()->GetBinCenter(nbinMax));
				// cout << tofDelPhists[i]->GetXaxis()->GetBinCenter(binMax) << " " << toftempPSD << " " << tofDelPhists[i]->GetXaxis()->GetBinCenter(nbinMax) << endl;

				TString canDiscTOFName = "ToFDiscrimination" + to_string(i) + "trig" + to_string(tr);
				//cout << canDiscTOFName << endl;
				TCanvas* canvasDiscTOF = new TCanvas(canDiscTOFName, canDiscTOFName, 800, 500);

				TLine* TOFline = new TLine(toftempPSD, 0, toftempPSD, ctMax);
				TOFline->SetLineColor(kBlack);
				TLine* TOFPSDlinetof = new TLine(0, toftempPSD, 1, toftempPSD);
				TOFPSDlinetof->SetLineColor(kRed);

				canvasDiscTOF->cd();
				tofDelPhists[i][tr]->Draw();
				TOFline->Draw("SAME");
				tofphotpeak->Draw("SAME");
				tofneutpeak->SetLineColor(kOrange);
				tofneutpeak->Draw("SAME");
				tofintersection->Draw("SAME");
				cdTofIndividual->cd();
				canvasDiscTOF->Write();

				// find the delay of photon peak for each detector
				double sourceDetDist = sqrt( pow(detectors[channelDet].X - triggers[tr].X, 2) +
																		 pow(detectors[channelDet].Y - triggers[tr].Y, 2) +
																		 pow(detectors[channelDet].Z - triggers[tr].Z, 2));

				detectors[i].timeDelay[tr] = tofphotpeak_opt->Parameter(1) - sourceDetDist/LIGHT_C;
				detectors[i].timeResolution[tr] = tofphotpeak_opt->Parameter(2);

				// cout << "det " << i << " and trig" << tr << " timing: " << detectors[i].timeDelay[tr] << " ns" << endl;

				tracktof[i] = toftempPSD;
			}

		}



		for(int i=0; i<NUM_DETS; i++)
		{
			// _________
			// PSD discrimination individual
			// _________

			if(detectors[i].type == INORGANIC) continue;

			// move to the outside of the distribution
			TF1* psdphotpeak = new TF1("psdphotpeak", "[0]*e^(-(x - [1])^2/(2*[2]^2))");
			TF1* psdneutpeak = new TF1("psdneutpeak", "[0]*e^(-(x - [1])^2/(2*[2]^2))");
			TF1* psdcombined = new TF1("psdcombined", "psdphotpeak + psdneutpeak");
			TF1* psdintersection = new TF1("psdintersection", "abs(psdneutpeak - psdphotpeak)");
			TFitResultPtr optimized;

			// set limits to the psd means


			TH1F* smoothN = (TH1F*) psdhists[i]->Clone();
			TH1F* smoothP = (TH1F*) psdhists[i]->Clone();
			smoothN->SetLineColor(kRed);
			smoothP->SetLineColor(kGreen);
			// smoothpsd->Smooth(1);

			double tempLocMaxPhot = psdhists[i]->GetBinCenter(psdhists[i]->GetMaximumBin());

			psdcombined->SetParLimits(1, MINPSD_FIT, tempLocMaxPhot + DIVPSD_FIT);
			psdcombined->SetParLimits(4, tempLocMaxPhot + DIVPSD_FIT, MAXPSD_FIT);

			//photon fit
			smoothP->GetXaxis()->SetRangeUser(MINPSD_FIT, tempLocMaxPhot + DIVPSD_FIT);
			Double_t psdmaxP = smoothP->GetMaximum();
			Double_t binpsdmaxP = smoothP->GetMaximumBin();
			Double_t xpsdmaxP = smoothP->GetXaxis()->GetBinCenter(binpsdmaxP);
			Double_t meanP = smoothP->GetMean();
			Double_t rmsP = smoothP->GetRMS();
			Double_t stdDevP = smoothP->GetStdDev();

			//neutron fit
			smoothN->GetXaxis()->SetRangeUser(tempLocMaxPhot + DIVPSD_FIT, MAXPSD_FIT);
			Double_t psdmaxN = smoothN->GetMaximum();
			Double_t binpsdmaxN = smoothN->GetMaximumBin();
			Double_t xpsdmaxN = smoothN->GetXaxis()->GetBinCenter(binpsdmaxN);
			Double_t meanN = smoothN->GetMean();
			Double_t rmsN = smoothN->GetRMS();
			Double_t stdDevN = smoothN->GetStdDev();

			// cout << meanN << " " << meanP << endl;

			// smoothpsd->GetXaxis()->SetRangeUser(MINPSD_FIT, MAXPSD_FIT);
			smoothP->GetXaxis()->SetRangeUser(MINPSD_FIT, MAXPSD_FIT);
			smoothN->GetXaxis()->SetRangeUser(MINPSD_FIT, MAXPSD_FIT);

			//combined fit
			psdcombined->SetParameter(0, psdmaxP);
			psdcombined->SetParameter(1, meanP);
			psdcombined->SetParameter(2, stdDevP);

			psdcombined->SetParameter(3, psdmaxN);
			psdcombined->SetParameter(4, meanN);
			psdcombined->SetParameter(5, stdDevN);

			psdhists[i]->GetXaxis()->SetRangeUser(MINPSD_FIT, MAXPSD_FIT);

			//cout << i << " psd: " << meanP << " " << meanN << endl;
			// cout << MINPSD_FIT << " " << MAXPSD_FIT << endl;

			// close out of the loop if there is not enough info in the slice. Save fake info for them.
			if(psdhists[i]->Integral() < MIN_ENTRIES)
			{
				TString nameBrokenPSD;
				nameBrokenPSD = "brokenPSD" + to_string(i);
				TFormula *f1 = new TFormula("f1", "[0]");
				f1->SetParameter(0, tempLocMaxPhot + DIVPSD_FIT);
				detectors[i].discPSD = new TF1(nameBrokenPSD, "f1");
				continue;
			}

			//optimize combined fit
			optimized = psdhists[i]->Fit(psdcombined, "SQ");

			//set optimized parameters to photon and neutron fit
			psdphotpeak->SetParameter(0, optimized->Parameter(0));
			psdphotpeak->SetParameter(1, optimized->Parameter(1));
			psdphotpeak->SetParameter(2, optimized->Parameter(2));
			psdneutpeak->SetParameter(0, optimized->Parameter(3));
			psdneutpeak->SetParameter(1, optimized->Parameter(4));
			psdneutpeak->SetParameter(2, optimized->Parameter(5));

			//set optimized parameters to combined fit
			psdcombined->SetParameter(0, optimized->Parameter(0));
			psdcombined->SetParameter(1, optimized->Parameter(1));
			psdcombined->SetParameter(2, optimized->Parameter(2));
			psdcombined->SetParameter(3, optimized->Parameter(3));
			psdcombined->SetParameter(4, optimized->Parameter(4));
			psdcombined->SetParameter(5, optimized->Parameter(5));

			//set optimized parameters to intersection fit
			psdintersection->SetParameter(0, optimized->Parameter(0));
			psdintersection->SetParameter(1, optimized->Parameter(1));
			psdintersection->SetParameter(2, optimized->Parameter(2));
			psdintersection->SetParameter(3, optimized->Parameter(3));
			psdintersection->SetParameter(4, optimized->Parameter(4));
			psdintersection->SetParameter(5, optimized->Parameter(5));

			double psdtempPSD = psdintersection->GetMinimumX(optimized->Parameter(1), optimized->Parameter(4));

			//in prediction completely off, use default intersection
			if(psdtempPSD < optimized->Parameter(1) || psdtempPSD > optimized->Parameter(4) || psdtempPSD >= 1) {
				psdtempPSD = tempLocMaxPhot + DIVPSD_FIT;
			}

			TLine* psdline = new TLine(psdtempPSD, 0, psdtempPSD, optimized->Parameter(0));
			psdline->SetLineColor(kBlack);
			TLine* TOFPSDlinepsd = new TLine(psdtempPSD, -200, psdtempPSD, 200);
			TOFPSDlinepsd->SetLineColor(kRed);

			TString tempName = "PSD" + to_string(i);
			TCanvas* canvaspsd = new TCanvas(tempName, tempName, 800, 500);

			trackpsd[i] = psdtempPSD;

			TString namePointPSD = "detPSD" + to_string(i);
			TFormula *f1 = new TFormula("f1", "[0]");
			f1->SetParameter(0, psdtempPSD);
			//detectors[i].discPSDPoint = new TF1(namePointPSD, "f1"); //detectors[i].discPSD
			detectors[i].discPSD = new TF1(namePointPSD, "f1");
			// detectors[i].discTOFPoint = toftempPSD - detectors[i].timeDelay;

			if(PSD_ERG==0)
			{
				TString canDiscErgName = "psdErg" + to_string(i);
				TCanvas* canvasDiscErg = new TCanvas(canDiscErgName, canDiscErgName, 800, 500);
				canvasDiscErg->cd();
				psdErgHists[i]->Draw();
				detectors[i].discPSD->Draw("SAME");
				//detectors[i].discPSDPoint->Draw("SAME");
				cdPsdErg->cd();
				canvasDiscErg->Write();
			}

			canvaspsd->cd();
			psdhists[i]->Draw();
			psdcombined->Draw("SAME");
			psdintersection->SetLineColor(kViolet+1);
			psdintersection->Draw("SAME");
			psdphotpeak->SetLineColor(kYellow);
			psdneutpeak->SetLineColor(kOrange);
			psdphotpeak->Draw("SAME");
			psdneutpeak->Draw("SAME");
			psdline->Draw("SAME");
			cdPsdIndividual->cd();
			canvaspsd->Write();

			// cdPsd->cd();
			// tofpsdcanvas->cd();
			// tofPsdHists[i]->Draw();
			// TOFPSDlinetof->Draw("SAME");
			// TOFPSDlinepsd->Draw("SAME");
			// cdTOFPSD->cd();
			// tofpsdcanvas->Write();
		}





 //  ___                          ___  ___ ___
 // | __|_ _  ___ _ _ __ _ _  _  | _ \/ __|   \
 // | _|| ' \/ -_) '_/ _` | || | |  _/\__ \ |) |
 // |___|_||_\___|_| \__, |\_, | |_|  |___/___/
 //                  |___/ |__/
 //
 if(DOUBLE_DISC == 1){
	 cout << "Using double discrimination with maximum misclassification of: " << MISC_MAX << endl;
 }
 else{
	 cout << "Using single misclassification" << endl;
 }

 // declare a TF1 function to be used for the fits
 const TF1 expLin = TF1("expLin", "[0]*e^(-[1]*x) + [2]*x + [3]", MINERG_FIT, MAXERG_FIT);
 cout << "Fitting psd vs energy in the range: " << MINERG_FIT << ", " << MAXERG_FIT << " MeV \n";

// if this doesn't work use a clone
 TF1* expLinPsd = new TF1(expLin);
 expLinPsd->SetName("expLinPsd");
 TF1* expLinMN = new TF1(expLin);
 expLinMN->SetName("expLinMN");
 TF1* expLinMP= new TF1(expLin);
 expLinMP->SetName("expLinMP");
 TF1* expLinSN = new TF1(expLin);
 expLinSN->SetName("expLinSN");
 TF1* expLinSP = new TF1(expLin);
 expLinSP->SetName("expLinSP");
 TF1* expLinPsdN = new TF1(expLin);
 expLinPsdN->SetName("expLinPsdN");
 TF1* expLinPsdP = new TF1(expLin);
 expLinPsdP->SetName("expLinPsdP");

	if(PSD_ERG==1)
	{

		// defines arrays used in and for the slicing process
		int numGoodSlicespsd;
		int energyBinspsd;
		double tempPSD, tempErgPSD;
		double psdPhotCounts, psdPhotMean, psdPhotStd;
		double psdNeutCounts, psdNeutMean, psdNeutStd;
		double *meanNeutPoint, *meanPhotPoint, *sigNeutPoint, *sigPhotPoint;
		double *energySliceInd, *discLinePoint, *discLineNeutPoint, *discLinePhotPoint;

		// defines TGraphs for all of the paramter and discrim lines
		TGraph** discLines;
		discLines = new TGraph* [NUM_DETS];
		double p0Disc, p1Disc, p2Disc;
		// parameters
		TGraph** meanNeut;
		meanNeut = new TGraph* [NUM_DETS];
		double m0Neut, m1Neut, m2Neut, m3Neut;
		TGraph** meanPhot;
		meanPhot = new TGraph* [NUM_DETS];
		double m0Phot, m1Phot, m2Phot, m3Phot;
		TGraph** sigNeut;
		sigNeut = new TGraph* [NUM_DETS];
		double s0Neut, s1Neut, s2Neut, s3Neut;
		TGraph** sigPhot;
		sigPhot = new TGraph* [NUM_DETS];
		double s0Phot, s1Phot, s2Phot, s3Phot;
		// double discrim
		TGraph** discLinesNeut;
		discLinesNeut = new TGraph* [NUM_DETS];
		double p0DiscN, p1DiscN, p2DiscN;
		TGraph** discLinesPhot;
		discLinesPhot = new TGraph* [NUM_DETS];
		double p0DiscP, p1DiscP, p2DiscP;

		// defines names for the fit results for each paramter and discrim line
		TFitResultPtr fitPSD_opt;
		TFitResultPtr psdDisc_opt;
		TFitResultPtr meanNeut_opt;
		TFitResultPtr meanPhot_opt;
		TFitResultPtr sigNeut_opt;
		TFitResultPtr sigPhot_opt;

		// psd slice discrimination
		cout << "Performing energy dependent PSD analysis" << endl;
		for(int det = 0; det < NUM_DETS; det++)
		{
			cdPsd->cd();
			// cout << "For detector " << det << ":" << endl;

			if(detectors[det].type == INORGANIC) continue;

			// sets the number of bins for each array
			energyBinspsd = psdErgHists[det]->GetNbinsX(); // get the number of slices
			energySliceInd = new double [energyBinspsd]; //energy slice
			discLinePoint = new double [energyBinspsd]; //discrimination
			// paramater arrays
			meanNeutPoint = new double [energyBinspsd];
			meanPhotPoint = new double [energyBinspsd];
			sigNeutPoint = new double [energyBinspsd];
			sigPhotPoint = new double [energyBinspsd];
			// double discrim arrays
			discLineNeutPoint = new double [energyBinspsd];
			discLinePhotPoint = new double [energyBinspsd];

			numGoodSlicespsd = -1; // reset the number of good slices

			// find the profile to get the center
			TProfile* histErgProfile = psdErgHists[det]->ProfileX("profileEnergy");

			cdPsdSlices->cd();
			// loop over the slices
			for(int energySlice = 1; energySlice <= energyBinspsd; energySlice += STEP_SIZE)
			{
				// define the slice
				TH1D* psdErgSlice = psdErgHists[det]->ProjectionY("psdErgSlice", energySlice, energySlice + STEP_SIZE-1);
				int numEntriesInSlice = psdErgSlice->GetEntries();
				double stdSlice = psdErgSlice->GetStdDev();
				psdErgSlice->SetLineColor(kBlack);

				// close out of the loop if there is not enough info in the slice
				if(numEntriesInSlice < MIN_ENTRIES || stdSlice == 0) //send pic of files that have std=0
				{
					continue;
				}

				// move to the outside of the distribution
				TF1* psdphotpeak = new TF1("psdphotpeak", "[0]*e^(-(x - [1])^2/(2*[2]^2))");
				TF1* psdneutpeak = new TF1("psdneutpeak", "[0]*e^(-(x - [1])^2/(2*[2]^2))");
				TF1* psdcombined = new TF1("psdcombined", "psdphotpeak + psdneutpeak");
				TF1* psdintersection = new TF1("psdintersection", "abs(psdneutpeak - psdphotpeak)");
				TFitResultPtr optimized;

				// set limits to the psd means
				// psdcombined->SetParLimits(1, MINPSD_FIT, DIVPSD_FIT);
			  // psdcombined->SetParLimits(4, DIVPSD_FIT, MAXPSD_FIT);



				TH1F* smoothN = (TH1F*) psdErgSlice->Clone();
				TH1F* smoothP = (TH1F*) psdErgSlice->Clone();
				smoothN->SetLineColor(kRed);
				smoothP->SetLineColor(kGreen);

				double tempLocMaxPhotSlice = psdErgSlice->GetBinCenter(psdErgSlice->GetMaximumBin());
				// smoothpsd->Smooth(1);

				//photon fit
				smoothP->GetXaxis()->SetRangeUser(MINPSD_FIT, tempLocMaxPhotSlice + DIVPSD_FIT);
				Double_t psdmaxP = smoothP->GetMaximum();
				Double_t binpsdmaxP = smoothP->GetMaximumBin();
				Double_t xpsdmaxP = smoothP->GetXaxis()->GetBinCenter(binpsdmaxP);
				Double_t meanP = smoothP->GetMean();
				Double_t rmsP = smoothP->GetRMS();
				Double_t stdDevP = smoothP->GetStdDev();

				//neutron fit
				smoothN->GetXaxis()->SetRangeUser(tempLocMaxPhotSlice + DIVPSD_FIT, MAXPSD_FIT);
				Double_t psdmaxN = smoothN->GetMaximum();
				Double_t binpsdmaxN = smoothN->GetMaximumBin();
				Double_t xpsdmaxN = smoothN->GetXaxis()->GetBinCenter(binpsdmaxN);
				Double_t meanN = smoothN->GetMean();
				Double_t rmsN = smoothN->GetRMS();
				Double_t stdDevN = smoothN->GetStdDev();

				// cout << meanN << " " << meanP << endl;

				//combined fit
				psdcombined->SetParameter(0, psdmaxP);
				psdcombined->SetParameter(1, meanP);
				psdcombined->SetParameter(2, stdDevP);

				psdcombined->SetParameter(3, psdmaxN);
				psdcombined->SetParameter(4, meanN);
				psdcombined->SetParameter(5, stdDevN);

				psdErgSlice->GetXaxis()->SetRangeUser(MINPSD_FIT, MAXPSD_FIT);
				//optimize combined fit

				if(psdErgSlice->Integral() < MIN_ENTRIES)
				{
					continue;
				}

				// cout << "in slice " << energySlice << " there are " << psdErgSlice->Integral() << " events" << endl;

				optimized = psdErgSlice->Fit(psdcombined, "SQ");

				//set optimized parameters to photon and neutron fit
				psdphotpeak->SetParameter(0, optimized->Parameter(0));
				psdphotpeak->SetParameter(1, optimized->Parameter(1));
				psdphotpeak->SetParameter(2, optimized->Parameter(2));
				psdneutpeak->SetParameter(0, optimized->Parameter(3));
				psdneutpeak->SetParameter(1, optimized->Parameter(4));
				psdneutpeak->SetParameter(2, optimized->Parameter(5));

				// find the number of neutrons and photons
				double photonArea = psdphotpeak->Integral(MINPSD_FIT, MAXPSD_FIT);
				double neutronArea = psdneutpeak->Integral(MINPSD_FIT, MAXPSD_FIT);

				//set optimized parameters to combined fit
				psdcombined->SetParameter(0, optimized->Parameter(0));
				psdcombined->SetParameter(1, optimized->Parameter(1));
				psdcombined->SetParameter(2, optimized->Parameter(2));
				psdcombined->SetParameter(3, optimized->Parameter(3));
				psdcombined->SetParameter(4, optimized->Parameter(4));
				psdcombined->SetParameter(5, optimized->Parameter(5));

				//set optimized parameters to intersection fit
				psdintersection->SetParameter(0, optimized->Parameter(0));
				psdintersection->SetParameter(1, optimized->Parameter(1));
				psdintersection->SetParameter(2, optimized->Parameter(2));
				psdintersection->SetParameter(3, optimized->Parameter(3));
				psdintersection->SetParameter(4, optimized->Parameter(4));
				psdintersection->SetParameter(5, optimized->Parameter(5));

				// find the optimal psd line
				tempPSD = psdintersection->GetMinimumX(meanP, meanN);
				tempErgPSD = (histErgProfile->GetBinCenter(energySlice) + histErgProfile->GetBinCenter(energySlice + STEP_SIZE))/2.0;
				// cout << psdPhotMean << "    " << tempPSD << "    " << psdNeutMean;

				// cout << det << "." << energySlice << " psd: " << meanP << " " << meanN << " -> " << tempPSD << endl;

				if(tempPSD < meanP || tempPSD > meanN || tempPSD >= 1)
				{
					// cout << "     throw" << endl;
					continue;
				}

				else
				{
					numGoodSlicespsd++;
					// cout << endl;
				}



				// stores the points for the graphs
				energySliceInd[numGoodSlicespsd] = tempErgPSD; //xvalue
				discLinePoint[numGoodSlicespsd] = tempPSD; //yvalue
				// paramater values
				meanNeutPoint[numGoodSlicespsd] = psdcombined->GetParameter(4);
				meanPhotPoint[numGoodSlicespsd] = psdcombined->GetParameter(1);
				sigNeutPoint[numGoodSlicespsd] = psdcombined->GetParameter(5);
				sigPhotPoint[numGoodSlicespsd] = psdcombined->GetParameter(2);



				// check if there is a natural minimum. If yes, rewrite the values
				double maxP, maxPosP;
				double maxN, maxPosN;
				double  tempMaxP,  tempMaxPosP;
				double minNP, minPosNP;
				maxP = 0;
				maxN = 0;
				tempMaxP = 0;

				double ratioMin = 100;

				bool valleyFound = false;

				for(int psV = 1; psV <= NUM_PSD_HIST; psV++)
				{
					double binC = psdErgSlice->GetBinContent(psV);
					// find the photon maximum
					if((!valleyFound) && (binC >= tempMaxP))
					{
						tempMaxP = binC;
						tempMaxPosP = psdErgSlice->GetBinCenter(psV);
					}

					// find the valley dip
					if((!valleyFound) && (binC < tempMaxP/ratioMin) & (tempMaxP > 0.5*psdErgSlice->GetMaximum())) // make sure it's an actual viable peak
					{
						minNP = binC;
						maxP = tempMaxP;
						maxPosP = tempMaxPosP;
						valleyFound = true;
					}

					// found the valley, now find the neutron
					if((valleyFound) & (binC > maxN))
					{
						maxN = binC;
						maxPosN = psdErgSlice->GetBinCenter(psV);
					}
				}


				// loops through and finds the double discrim lines when turned on
				if(DOUBLE_DISC == 1)
				{
					//defining and setting requred values
					discLineNeutPoint[numGoodSlicespsd] = tempPSD - DISC_SHIFT_VALUE;
					discLinePhotPoint[numGoodSlicespsd] = tempPSD + DISC_SHIFT_VALUE;

					double goodPhot, goodNeut;
					double badPhot, badNeut;
					double miscPhot, miscNeut;

					// loops through to find the new photon based discrimline
					do
					{
						discLinePhotPoint[numGoodSlicespsd] -= DISC_SHIFT_VALUE;

						// finds the photon misclafficiation based on total area
						badNeut = neutronArea * normal_cdf(discLinePhotPoint[numGoodSlicespsd], sigNeutPoint[numGoodSlicespsd], meanNeutPoint[numGoodSlicespsd]);
						goodPhot = photonArea * normal_cdf(discLinePhotPoint[numGoodSlicespsd], sigPhotPoint[numGoodSlicespsd], meanPhotPoint[numGoodSlicespsd]);
						miscNeut = badNeut / goodPhot;

						// stops the while loop if needed
						if(discLinePhotPoint[numGoodSlicespsd] <= MINPSD_FIT)
						{
							cout << det << ": desired phot discrim not found" << endl;
							break;
						}
					}
					while(miscNeut >= MISC_MAX);

					// loops through to find the new neutron based discrimline
					do
					{
						discLineNeutPoint[numGoodSlicespsd] += DISC_SHIFT_VALUE;

						// finds the neutron misclassification based on total area
						badPhot = photonArea * (1 - normal_cdf(discLineNeutPoint[numGoodSlicespsd], sigPhotPoint[numGoodSlicespsd], meanPhotPoint[numGoodSlicespsd]));
						goodNeut = neutronArea * (1 - normal_cdf(discLineNeutPoint[numGoodSlicespsd], sigNeutPoint[numGoodSlicespsd], meanNeutPoint[numGoodSlicespsd]));
						miscPhot = badPhot / goodNeut;

						// stops the while loop if needed
						if(discLineNeutPoint[numGoodSlicespsd] >= MAXPSD_FIT)
						{
							cout << det << ": desired neut discrim not found" << endl;
							break;
						}
					}
					while(miscPhot >= MISC_MAX);
				}

				// if we found the valley, store the halfway point

				if(valleyFound == true)
				{
					// cout << "I used the valley!" << endl;
					energySliceInd[numGoodSlicespsd] = tempErgPSD; //xvalue
					discLinePoint[numGoodSlicespsd] = 0.5*(maxPosN + maxPosP); //yvalue
					meanNeutPoint[numGoodSlicespsd] = maxPosN;
					meanPhotPoint[numGoodSlicespsd] = maxPosP;

					sigPhotPoint[numGoodSlicespsd] = 0.01;
					sigNeutPoint[numGoodSlicespsd] = 0.01;
				}


				TString canSliceName = "psd" + to_string(det) +  "Proj" + to_string(numGoodSlicespsd);

				// canvas with the psd
				TCanvas* canvasSlice = new TCanvas(canSliceName, canSliceName, 800, 500);

				double windowHeight = psdcombined->GetParameter(0)*2;

				psdErgSlice->GetYaxis()->SetRangeUser(0, windowHeight); //maxcounts

				canvasSlice->cd();
				psdErgSlice->Draw(); // draw the histogram
				psdcombined->Draw("SAME"); // draw the combined fit
				psdintersection->Draw("SAME");
				psdphotpeak->SetLineColor(kYellow);
				psdneutpeak->SetLineColor(kOrange);
				psdphotpeak->Draw("SAME");
				psdneutpeak->Draw("SAME");

				// discrimination line
				TLine* line = new TLine(tempPSD, 0, tempPSD, windowHeight);
				line->SetLineColor(kBlack);
				line->Draw("SAME");

				canvasSlice->Write();

				// cout << "  n: " <<  tempPSD << endl;

			}

			cdPsd->cd();
			// cout << det << "has " << numGoodSlicespsd << " good slices." << endl;
			// cout << "Finished looping through slices" << endl;

			if(numGoodSlicespsd > 0)
			{
					// create a canvas with the psd discrimination
					TString canDiscErgName = "psdErg" + to_string(det);
					TCanvas* canvasDiscErg = new TCanvas(canDiscErgName, canDiscErgName, 800, 500);


					// remove discontinuities in the psd points
					for(int smS = 1; smS < numGoodSlicespsd; smS++)
					{
						if((energySliceInd[smS] > 2*MINERG_FIT))
						{
							if(abs(discLinePoint[smS] - discLinePoint[smS-1]) > DIVPSD_FIT)
							{
								cout << "disc at " << det << ": " << smS << endl;

								discLinePoint[smS] = discLinePoint[smS-1];
							}

							if(abs(meanNeutPoint[smS] - meanNeutPoint[smS-1]) > DIVPSD_FIT) meanNeutPoint[smS] = meanNeutPoint[smS-1];

							if(abs(meanPhotPoint[smS] - meanPhotPoint[smS-1]) > DIVPSD_FIT) meanPhotPoint[smS] = meanPhotPoint[smS-1];

							if(abs(sigNeutPoint[smS] - sigNeutPoint[smS-1]) > DIVPSD_FIT) sigNeutPoint[smS] = sigNeutPoint[smS-1];

							if(abs(sigPhotPoint[smS] - sigPhotPoint[smS-1]) > DIVPSD_FIT) meanPhotPoint[smS] = sigPhotPoint[smS-1];
						}
					}

					// smooth out psd curves
					for(int smS = 1; smS < numGoodSlicespsd; smS++)
					{
						if((energySliceInd[smS] > 2*MINERG_FIT))
						{
							discLinePoint[smS] = 0.5*(discLinePoint[smS-1] + discLinePoint[smS]);
							meanNeutPoint[smS] = 0.5*(meanNeutPoint[smS-1] + meanNeutPoint[smS]);
							meanPhotPoint[smS] = 0.5*(meanPhotPoint[smS-1] + meanPhotPoint[smS]);
							sigNeutPoint[smS] = 0.5*(sigNeutPoint[smS-1] + sigNeutPoint[smS]);
							sigPhotPoint[smS] = 0.5*(sigPhotPoint[smS-1] + sigPhotPoint[smS]);

							energySliceInd[smS] = 0.5*(energySliceInd[smS-1] + energySliceInd[smS]);
						}
					}

					// stores the psd discrimination line by detector
				  discLines[det] = new TGraph(numGoodSlicespsd+1, energySliceInd, discLinePoint);
					discLines[det]->SetLineColor(kBlack);
					discLines[det]->SetLineWidth(3);
					discLines[det]->SetName("discLines");

					// stores the paramater data by detector
					meanNeut[det] = new TGraph(numGoodSlicespsd+1, energySliceInd, meanNeutPoint);
					meanNeut[det]->SetLineColor(kBlue);
					meanNeut[det]->SetLineWidth(3);
					meanNeut[det]->SetName("meanNeut");

					meanPhot[det] = new TGraph(numGoodSlicespsd+1, energySliceInd, meanPhotPoint);
					meanPhot[det]->SetLineColor(kRed);
					meanPhot[det]->SetLineWidth(3);
					meanPhot[det]->SetName("meanPhot");

					sigNeut[det] = new TGraph(numGoodSlicespsd+1, energySliceInd, sigNeutPoint);
					sigNeut[det]->SetLineColor(kBlue);
					sigNeut[det]->SetLineStyle(kDashed);
					sigNeut[det]->SetLineWidth(3);
					sigNeut[det]->SetName("sigNeut");

					sigPhot[det] = new TGraph(numGoodSlicespsd+1, energySliceInd, sigPhotPoint);
					sigPhot[det]->SetLineColor(kRed);
					sigPhot[det]->SetLineStyle(kDashed);
					sigPhot[det]->SetLineWidth(3);
					sigPhot[det]->SetName("sigPhot");

					// stores the double discrimination line data by detector
					if(DOUBLE_DISC == 1)
					{
						discLinesNeut[det] = new TGraph(numGoodSlicespsd+1, energySliceInd, discLineNeutPoint);
						discLinesNeut[det]->SetLineColor(kGreen);
						discLinesNeut[det]->SetLineWidth(3);
						discLinesNeut[det]->SetName("psdNeut");

						discLinesPhot[det] = new TGraph(numGoodSlicespsd+1, energySliceInd, discLinePhotPoint);
						discLinesPhot[det]->SetLineColor(kOrange);
						discLinesPhot[det]->SetLineWidth(3);
						discLinesPhot[det]->SetName("psdPhot");
					}

					expLinPsd->SetParLimits(0, MIN_PSD_HIST, MAX_PSD_HIST); expLinPsd->SetParLimits(1, MIN_BEND_PSD, MAX_BEND_PSD);
					expLinMN->SetParLimits(0, MIN_PSD_HIST, MAX_PSD_HIST); expLinMN->SetParLimits(1, MIN_BEND_PSD, MAX_BEND_PSD);
					expLinMP->SetParLimits(0, MIN_PSD_HIST, MAX_PSD_HIST); expLinMP->SetParLimits(1, MIN_BEND_PSD, MAX_BEND_PSD);
					expLinSN->SetParLimits(0, MIN_PSD_HIST, MAX_PSD_HIST); expLinSN->SetParLimits(1, MIN_BEND_PSD, MAX_BEND_PSD);
					expLinSP->SetParLimits(0, MIN_PSD_HIST, MAX_PSD_HIST); expLinSP->SetParLimits(1, MIN_BEND_PSD, MAX_BEND_PSD);


					// fit the different parameters by detector
					// fits the dirsim line
					psdDisc_opt = discLines[det]->Fit(expLinPsd, "SQ", "",  MINERG_FIT, MAXERG_FIT);
					// fits the parameters
					meanNeut_opt = meanNeut[det]->Fit(expLinMN, "SQ", "", MINERG_FIT, MAXERG_FIT);
					meanPhot_opt = meanPhot[det]->Fit(expLinMP, "SQ", "", MINERG_FIT, MAXERG_FIT);
					sigNeut_opt = sigNeut[det]->Fit(expLinSN, "SQ", "", MINERG_FIT, MAXERG_FIT);
					sigPhot_opt = sigPhot[det]->Fit(expLinSP, "SQ", "", MINERG_FIT, MAXERG_FIT);

					// stores the parameter fits as a TF1
				  detectors[det].discPSD = (TF1*)expLinPsd->Clone();
					detectors[det].meanNeut = (TF1*)expLinMN->Clone();
					detectors[det].meanPhot = (TF1*)expLinMP->Clone();
					detectors[det].sigNeut = (TF1*)expLinSN->Clone();
					detectors[det].sigPhot = (TF1*)expLinSP->Clone();

					if(DOUBLE_DISC == 1)
					{
						detectors[det].discPSDNeut = discLinesNeut[det];
						detectors[det].discPSDPhot = discLinesPhot[det];
					}
					else if(DOUBLE_DISC != 1)
					{
						detectors[det].discPSDNeut = discLines[det];
						detectors[det].discPSDPhot = discLines[det];
					}


					// adds axis labels to the energy plots
					psdErgHists[det]->GetXaxis()->SetTitle("Total (MeVee)");
					psdErgHists[det]->GetYaxis()->SetTitle("Tail/Total");

					// draw the parameter points on top of the histogram
					canvasDiscErg->cd();
					psdErgHists[det]->Draw();
					meanNeut[det]->Draw("SAME");
					meanPhot[det]->Draw("SAME");
					if(DOUBLE_DISC == 1)
					{
						discLinesNeut[det]->Draw("SAME");
						discLinesPhot[det]->Draw("SAME");
					}
					discLines[det]->Draw("SAME");

					// draws the paramater lines onto the histogram
					detectors[det].discPSD->Draw("SAME");
					detectors[det].meanNeut->Draw("SAME");
					detectors[det].meanPhot->Draw("SAME");
					cdPsdErg->cd();
					canvasDiscErg->Write();

					// changes line colors for the pure paramter plots
					detectors[det].discPSD->SetLineColor(kBlack);
					detectors[det].meanNeut->SetLineColor(kBlue);
					detectors[det].meanPhot->SetLineColor(kRed);
					detectors[det].sigNeut->SetLineColor(kGreen);
					detectors[det].sigPhot->SetLineColor(kOrange);

					// creates a canvas for all of the parameter plots
					TString canParamName = "param" + to_string(det);
					TCanvas* canvasParam = new TCanvas(canParamName, canParamName, 800, 500);
					detectors[det].discPSD->SetTitle("PSD Paramters; Light Output (MeVee); PSD");

					// draws the points and fits onto the paramater plots
					canvasParam->cd();
					detectors[det].discPSD->Draw();
					detectors[det].meanNeut->Draw("SAME");
					detectors[det].meanPhot->Draw("SAME");
					detectors[det].sigNeut->Draw("SAME");
					detectors[det].sigPhot->Draw("SAME");
					discLines[det]->Draw("SAME");
					meanNeut[det]->Draw("SAME");
					meanPhot[det]->Draw("SAME");
					sigNeut[det]->Draw("SAME");
					sigPhot[det]->Draw("SAME");
					cdParam->cd();
					canvasParam->Write();

			}

		}

		cout << "Finished PSD-Energy analysis" << endl;
	}

		/*
		_____ _       _
	 |_   _(_)_ __ (_)_ _  __ _
	   | | | | '  \| | ' \/ _` |
	   |_| |_|_|_|_|_|_||_\__, |
	                      |___/
		*/

	// //slicing tof erg
	// if(TOF_ERG==1)
	// {
	// 	long int MIN_ENTRIES = 200;
	// 	int numGoodSlicestof;
	// 	int energyBinstof;
	// 	double tempTOF, tempErgTOF;
	// 	double *energySliceIndtof, *discLinePointtof;
	// 	TGraph** discLinestof;
	// 	discLinestof = new TGraph* [NUM_DETS];
	// 	double p0Disc, p1Disc, p2Disc;
	//
	// 	TFitResultPtr tofDisc_opt;
	// 	TF1* tofDisc = new TF1("tofDisc", "pol1", MINERG_FIT, MAXERG_FIT);
	// 	tofDisc->SetLineStyle(kDashed);
	// 	tofDisc->SetLineColor(kOrange);
	// 	tofDisc->SetLineWidth(4);
	//
	//
	// 	cout << "Performing energy dependent ToF analysis" << endl;
	// 	//tof slice discrimination
	// 	for(int det = 0; det < NUM_DETS; det++)
	// 	{
	// 		cdPsd->cd();
	// 		// cout << "For detector " << det << ":" << endl;
	//
	// 		energyBinstof = tofErgHists[det]->GetNbinsX(); // get the number of slices
	// 		energySliceIndtof = new double [energyBinstof]; //energy slice
	// 		discLinePointtof = new double [energyBinstof]; //discrimination
	// 		numGoodSlicestof = -1; // reset the number of good slices
	//
	// 		// find the profile to get the center
	// 		TProfile* histErgProfile = tofErgHists[det]->ProfileX("profileEnergy");
	//
	// 		cdToF->cd();
	// 		//cdTofSlices->cd();
	// 		// loop over the slices
	// 		for(int energySlice = 0; energySlice < energyBinstof; energySlice += STEP_SIZE)
	// 		{
	// 			TF1* gausSlicetof = new TF1("gausSlicetof", "[0]*e^(-(x - [1])^2/(2*[2]^2))");
	// 			TF1* neutSlicetof = new TF1("neutSlicetof", "[0]/(1 + ((x - [1])/([2]))^2)");
	// 			TF1* intersectionSlicetof = new TF1("TOFintersect", "abs(neutSlicetof - gausSlicetof)");
	//
	// 			// define the slice
	// 			TH1D* tofErgSlice = tofErgHists[det]->ProjectionY("tofErgSlice", energySlice, energySlice+STEP_SIZE);
	// 			int numEntriesInSlice = tofErgSlice->GetEntries();
	// 			tofErgSlice->SetLineColor(kBlack);
	//
	// 			// close out of the loop if there is not enough info in the slice
	// 			if(numEntriesInSlice < MIN_ENTRIES)
	// 			{
	// 			 	continue;
	// 			}
	//
	// 			// find the initial parameters for the fiting of the photon peak
	// 			tofErgSlice->GetXaxis()->SetRangeUser(tracktof[det]-50, tracktof[det]);
	// 			Double_t tofpMax = tofErgSlice->GetMaximum();
	// 			Double_t tofbinpMax = tofErgSlice->GetMaximumBin();
	// 			Double_t tofxpMax = tofErgSlice->GetXaxis()->GetBinCenter(tofbinpMax);
	// 			Double_t tofpMean = tofErgSlice->GetMean();
	// 			Double_t tofpRMS = tofErgSlice->GetRMS();
	//
	// 			// find the initial parameters for the fitting of the neutron peak
	// 			tofErgSlice->GetXaxis()->SetRangeUser(tracktof[det], tracktof[det]+100);
	// 			Double_t tofnMax = tofErgSlice->GetMaximum();
	// 			Double_t tofbinnMax = tofErgSlice->GetMaximumBin();
	// 			Double_t tofxnMax = tofErgSlice->GetXaxis()->GetBinCenter(tofbinnMax);
	// 			Double_t tofnMean = tofErgSlice->GetMean();
	// 			Double_t tofnRMS = tofErgSlice->GetRMS();
	//
	// 			//reset
	// 			tofErgSlice->GetXaxis()->SetRangeUser(-200, 200);
	//
	// 			// set the initial guesses of gaus fit
	// 			gausSlicetof->SetParameter(0, tofpMax);
	// 			gausSlicetof->SetParameter(1, tofxpMax);
	// 			gausSlicetof->SetParameter(2, tofpRMS);
	// 			TFitResultPtr gausSliceOptimize = tofErgSlice->Fit(gausSlicetof, "SQ");
	// 			gausSlicetof->SetParameter(0, gausSliceOptimize->Parameter(0));
	// 			gausSlicetof->SetParameter(1, gausSliceOptimize->Parameter(1));
	// 			gausSlicetof->SetParameter(2, gausSliceOptimize->Parameter(2));
	//
	// 			//set initial guess of neut fit
	// 			neutSlicetof->SetParameter(0, tofnMax);
	// 			neutSlicetof->SetParameter(1, tofxnMax);
	// 			neutSlicetof->SetParameter(2, tofnRMS);
	// 			TFitResultPtr neutSliceOptimize = tofErgSlice->Fit(neutSlicetof, "+SQ");
	// 			neutSlicetof->SetParameter(0, neutSliceOptimize->Parameter(0));
	// 			neutSlicetof->SetParameter(1, neutSliceOptimize->Parameter(1));
	// 			neutSlicetof->SetParameter(2, neutSliceOptimize->Parameter(2));
	//
	// 			//set initial guesses of intersection fit (no combined fit this time)
	// 			intersectionSlicetof->SetParameter(0, gausSliceOptimize->Parameter(0));
	// 			intersectionSlicetof->SetParameter(1, gausSliceOptimize->Parameter(1));
	// 			intersectionSlicetof->SetParameter(2, gausSliceOptimize->Parameter(2));
	// 			intersectionSlicetof->SetParameter(3, neutSliceOptimize->Parameter(0));
	// 			intersectionSlicetof->SetParameter(4, neutSliceOptimize->Parameter(1));
	// 			intersectionSlicetof->SetParameter(5, neutSliceOptimize->Parameter(2));
	//
	// 			// find the optimal psd line
	// 			tempTOF = intersectionSlicetof->GetMinimumX(gausSliceOptimize->Parameter(1), neutSliceOptimize->Parameter(1));
	// 			tempErgTOF = (histErgProfile->GetBinCenter(energySlice) + histErgProfile->GetBinCenter(energySlice + STEP_SIZE))/2.0;
	// 			// cout << gausSliceOptimize->Parameter(1) << "    " << tempTOF << "," << tempErgTOF << "    " << neutSliceOptimize->Parameter(1);
	//
	// 			if(tempTOF < gausSliceOptimize->Parameter(1) || tempTOF > neutSliceOptimize->Parameter(1) || tempTOF >= 200) {
	// 				// cout << "     throw" << endl;
	// 				continue;
	// 			}
	// 			else {
	// 				numGoodSlicestof++;
	// 				cout << endl;
	// 			}
	// 			// find the points for the graph
	// 			energySliceIndtof[numGoodSlicestof] = tempErgTOF;
	// 			discLinePointtof[numGoodSlicestof] = tempTOF;
	//
	// 			TString cantofSliceName = "tof" + to_string(det) +  "Proj" + to_string(numGoodSlicestof);
	//
	// 			// canvas with the psd
	// 			TCanvas* canvastofSlice = new TCanvas(cantofSliceName, cantofSliceName, 800, 500);
	//
	// 			tofErgSlice->GetYaxis()->SetRangeUser(0, 1.5*gausSliceOptimize->Parameter(0));
	//
	// 			// canvastofSlice->cd();
	// 			//
	// 			// tofErgSlice->Draw();
	// 			// intersectionSlicetof->Draw("SAME");
	// 			// gausSlicetof->SetLineColor(kYellow);
	// 			// neutSlicetof->SetLineColor(kOrange);
	//
	// 			// discrimination line
	// 			TLine* tofline = new TLine(tempTOF, 0, tempTOF, 1.5*gausSliceOptimize->Parameter(0));
	// 			// tofline->SetLineColor(kBlack);
	// 			// tofline->Draw("SAME");
	// 			//
	// 			// cdToF->cd();
	// 			// cdTofSlices->cd();
	// 			// canvastofSlice->Write();
	// 		}
	// 		cdPsd->cd();
	// 		// cout << "Finished looping through TOF slices" << endl;
	//
	// 		if(numGoodSlicestof)
	// 		{
	// 				// create a canvas with the psd discrimination
	// 				TString canDiscErgNametof = "tofErg" + to_string(det);
	// 				TCanvas* canvasDiscErgtof = new TCanvas(canDiscErgNametof, canDiscErgNametof, 800, 500);
	//
	// 				// create a new psd discrimination line
	// 			  discLinestof[det] = new TGraph(numGoodSlicestof+1, energySliceIndtof, discLinePointtof);
	// 				discLinestof[det]->SetLineColor(kRed);
	// 				discLinestof[det]->SetLineWidth(3);
	//
	// 				// fit the psd discriminations
	// 				tofDisc_opt = discLinestof[det]->Fit(tofDisc, "SQ", 0, 0.4); // change the bounds here
	//
	// 				// extract fit information
	// 				p0Disc = tofDisc_opt->Parameter(0);
	// 				p1Disc = tofDisc_opt->Parameter(1);
	//
	// 				// set discrimination line
	// 				tofDisc->SetParameters(p0Disc, p1Disc, p2Disc);
	// 				tofDisc->SetParameters(p0Disc, p1Disc);	//whats the purpose of this?
	//
	//
	// 				// draw the line on top of the histogram
	// 				// canvasDiscErgtof->cd();
	// 				// tofErgHists[det]->Draw();
	// 				// discLinestof[det]->Draw("SAME");
	// 				// tofDisc->Draw("SAME");
	// 				// cdTofErg->cd();
	// 				//canvasDiscErgtof->Write();
	//
	// 		}
	//
	// 	}
	//
	// 	cout << "Finished ToF-Energy analysis" << endl;
	// }



 //
	// _____    ___                         _   _
 // |_   _|__| __|  __ ___ _ _ _ _ ___ __| |_(_)___ _ _
 //   | |/ _ \ _|  / _/ _ \ '_| '_/ -_) _|  _| / _ \ ' \
 //   |_|\___/_|   \__\___/_| |_| \___\__|\__|_\___/_||_|
 //

 // cout << "Correcting times for internal delays" << endl;
 //
	// // time of flight loop
	// double corrTime;
	// for (Long64_t jentry=0; jentry<nentries;jentry++)
	// {
	//  // load tree
	//  Long64_t ientry = LoadTree(jentry);
	//  if (ientry < 0) break;
	//  nb = tree->GetEntry(jentry);   nbytes += nb;
 //
	//  if(tMult >= NUM_DETS)
	//  {
	// 	 continue;
	//  }
 //
	//  // store the channel of the fission trigger
	//  channelTrig = isTrigger(tChan);
 //
	//  for(int part = 0; part < tMult; part++)
	//  {
	// 		// store the channel number
	// 	 	channelDet = isDetector(totChan[part]);
	// 	 	corrTime = totToF[part] - detectors[channelDet].timeDelay[channelTrig];
	// 	 	//totpsd fill histogram w corrtime instead of time
 //
	// 	 	tofDelPhistsCorr[channelDet]->Fill(corrTime);
	// 	 	tofPsdHistsCorr[channelDet]->Fill(totPSP[part], corrTime);
	// 	 	tofErgHistsCorr[channelDet]->Fill(totDep[part]/detectors[channelDet].calibration, corrTime);
 //
	// 	 	// discriminate particles here (make it better) -changed discPSDPoint to discPSD in 2 ifstatements
	// 	 	if(totPSP[part] < detectors[channelDet].discPSD->Eval(totDep[part]/detectors[channelDet].calibration))
	// 	 	{
	// 		 	tofPhists[channelDet]->Fill(corrTime);
	// 		 	kinematicP[channelDet]->Fill(corrTime, totDep[part]/detectors[channelDet].calibration);
	// 	 	}
 //
	// 	 	if(totPSP[part] > detectors[channelDet].discPSD->Eval(totDep[part]/detectors[channelDet].calibration) )
	// 	 	{
	// 		 	tofNhists[channelDet]->Fill(corrTime);
	// 		 	kinematicN[channelDet]->Fill(corrTime, totDep[part]/detectors[channelDet].calibration);
	// 	 	}
	// 	}
	// }

	cout << "Writing histograms" << endl;

	// // write the output histograms
	// for(int i = 0; i < NUM_DETS; i++)
	// {
	// 		cdTOFCorr->cd();
	// 		tofDelPhistsCorr[i]->Write();
	// 		tofPsdHistsCorr[i]->Write();
	// 		tofErgHistsCorr[i]->Write();
	//
	// 		cdKin->cd();
	// 		kinematicN[i]->Write();
	// 		kinematicP[i]->Write();
	// }

	// // save beam parameters
	// if(NUM_BEAMS > 0)
	// {
	//
	// 	for(int i = 0; i < NUM_TRIGGERS; i++)
	// 	{
	// 		beamTimeHist[i]->Write();
	// 	}
	// }

	cout << "Histograms have been written to file" << endl;

	return 0;
}
