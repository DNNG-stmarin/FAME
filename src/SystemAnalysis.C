// /*
// Author: Stefano Marin, Isabel Hernandez
// Purpose: This code loops through the processed tree and using the applied cuts and corrections computes the histograms of interest
// */
//
// #define SystemAnalysis_cxx
//
// #include "DetectorSystemClass.h"
//
// #include <TH2.h>
// #include <TStyle.h>
// #include <TCanvas.h>
// #include <TString.h>
// #include <TLegend.h>
// #include <TCut.h>
// #include <TFitResult.h>
// #include <THStack.h>
//
// #include <stdlib.h>
// #include <stdio.h>
//
// #include <iostream>
// #include <fstream>
// #include <sstream>
//
// using namespace std;
//
// void DetectorSystemClass::SystemAnalysis()
// {
//
//    detFile->cd();
//    /*
//   __  __      _        _
//  |  \/  |__ _(_)_ _   | |   ___  ___ _ __
//  | |\/| / _` | | ' \  | |__/ _ \/ _ \ '_ \
//  |_|  |_\__,_|_|_||_| |____\___/\___/ .__/
//                                     |_|
//    */
//
//    int nMult, pMult;
//
//    if (tree == 0) return;
//
//    Long64_t nentries = tree->GetEntriesFast();
//    Long64_t nbytes = 0, nb = 0;
//
//    for (Long64_t jentry=0; jentry<nentries;jentry++)
//    {
//     // load tree
//     Long64_t ientry = LoadTree(jentry);
//     if (ientry < 0) break;
//     nb = tree->GetEntry(jentry);   nbytes += nb;
//
//
//     double psdDisc = 0.17; // change this to the point specific
//
//     // fill coincidence histograms
//     for(int j = 0; j < tMult; j++)
//     {
//          hSingles->Fill(isDetector(totChan[j]));
//
//          for(int k = 0; k < tMult; k++)
//          {
//
//             // find the reflections
//             reflections[isDetector(totChan[j])][isDetector(totChan[k])]->Fill(totToF[k] - totToF[j], totDep[k]);
//
//             // find the doubles
//             if(j != k)
//             {
//                hDoubles->Fill(isDetector(totChan[j]), isDetector(totChan[k]));
//             }
//
//             // compute the coincidences
//             if((totDep[j] > DETECTOR_THRESHOLD) & (totDep[k] > DETECTOR_THRESHOLD))
//             {
//
//                // fill the bi-correlation
//                hBicorr[isDetector(totChan[j])][isDetector(totChan[k])]->Fill(totToF[j], totToF[k]);
//
//                // fill the cross-correlations
//                if((totPSP[j] > psdDisc) & (totPSP[k] > psdDisc)) // neutron - neutron
//                {
//                   nnMult[isDetector(totChan[j])][isDetector(totChan[k])]->Fill(totToF[j] - totToF[k]);
//                }
//
//                if((totPSP[j] > psdDisc) & (totPSP[k] < psdDisc)) // neutron - photon
//                {
//                   ngMult[isDetector(totChan[j])][isDetector(totChan[k])]->Fill(totToF[j] - totToF[k]);
//                }
//
//                if((totPSP[j] < psdDisc) & (totPSP[k] > psdDisc)) // photon - neutron
//                {
//                   gnMult[isDetector(totChan[j])][isDetector(totChan[k])]->Fill(totToF[j] - totToF[k]);
//                }
//
//                if((totPSP[j] < psdDisc) & (totPSP[k] < psdDisc)) // photon - photon
//                {
//                   ggMult[isDetector(totChan[j])][isDetector(totChan[k])]->Fill(totToF[j] - totToF[k]);
//                }
//             }
//
//          }
//       }
//    }
//
//
//    // don't plot on the display
//    gROOT->SetBatch(kTRUE);
//
//
//    // multiplicity
//    cdMult->cd();
//    TCanvas* canvMult = new TCanvas("multC", "multC", 800, 500);
//    canvMult->cd();
//    hMult->Draw("COLZ");
//    canvMult->Write();
//    hMult->Write();
//
//
//    // coincidences saving
//    cdCoinc->cd();
//    TCanvas* canvCoinc = new TCanvas("coincC", "coincC", 800, 500);
//    canvCoinc->Divide(NUM_DETS, NUM_DETS);
//
//    for(int det1 = 0; det1 < NUM_DETS; det1++)
//    {
//       for(int det2 = 0; det2 < NUM_DETS; det2++)
//       {
//          // select position on canvas
//          canvCoinc->cd(det1 * NUM_DETS + det2 + 1);
//
//          // draw histograms
//          allCoinc[det1][det2]->Draw("nostack");
//
//
//          nnMult[det1][det2]->Write();
//          ngMult[det1][det2]->Write();
//          gnMult[det1][det2]->Write();
//          ggMult[det1][det2]->Write();
//
//          allCoinc[det1][det2]->Write();
//
//       }
//    }
//
//    canvCoinc->Write();
//
//    // Now save the figures related to coincidences
//    cdFigCoinc->cd();
//    TCanvas* canvEach[NUM_DETS][NUM_DETS];
//    TString titlePlot, namePlot;
//    TString nameCoincT = "coincidence_";
//
//    for(int det1 = 0; det1 < NUM_DETS; det1++)
//    {
//       for(int det2 = 0; det2 < NUM_DETS; det2++)
//       {
//          namePlot = nameCoincT + to_string(det1) + "_" + to_string(det2);
//          titlePlot = namePlot;
//
//          canvEach[det1][det2] = new TCanvas(namePlot, titlePlot, 800, 500);
//
//          canvEach[det1][det2]->cd();
//          allCoinc[det1][det2]->Draw("nostack");
//
//          // legend
//          TLegend* legendTT = new TLegend(0.1,0.7,0.48,0.9);
//          legendTT->AddEntry(nnMult[det1][det2], "NN", "l");
//          legendTT->AddEntry(ngMult[det1][det2], "NG", "l");
//          legendTT->AddEntry(gnMult[det1][det2], "GN", "l");
//          legendTT->AddEntry(ggMult[det1][det2], "GG", "l");
//          legendTT->Draw();
//
//          // save the figures
//          canvEach[det1][det2]->Write();
//       }
//    }
//
//
//    cdBicorr->cd();
//    TCanvas* canvBicorr[NUM_DETS][NUM_DETS];
//    TString nameBicorrT = "bicorr_";
//
//    for(int det1 = 0; det1 < NUM_DETS; det1++)
//    {
//       for(int det2 = 0; det2 < NUM_DETS; det2++)
//       {
//          namePlot = nameBicorrT + to_string(det1) + "_" + to_string(det2);
//          titlePlot = namePlot;
//
//          canvBicorr[det1][det2] = new TCanvas(namePlot, titlePlot, 800, 500);
//
//          canvBicorr[det1][det2]->cd();
//          hBicorr[det1][det2]->Draw();
//
//          // save the figures
//          canvBicorr[det1][det2]->Write();
//
//          hBicorr[det1][det2]->Write();
//       }
//    }
//
//    hSingles->Write();
//    hDoubles->Write();
//
//    // saving reflection plots
//    cdRef->cd();
//
//    for(int det1 = 0; det1 < NUM_DETS; det1++)
//    {
//       for(int det2 = 0; det2 < NUM_DETS; det2++)
//       {
//          reflections[det1][det2]->Write();
//       }
//    }
//
//    detFile->Close();
//
//    // and reopen the display
//    gROOT->SetBatch(kTRUE);
// }
