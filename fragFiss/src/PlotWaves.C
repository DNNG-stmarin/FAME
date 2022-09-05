#include "fragFiss.h"
#include "constants.h"
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>

void fragFiss::PlotWaves()
{
  double timeAxis[RECORD_LENGTH] = {0};

   double** cathodeSignal;
   double** anode1Signal;
   double** anode2Signal;
   double** grid1Signal;
   double** grid2Signal;

   cathodeSignal = new double* [NUM_WAVES];
   anode1Signal = new double* [NUM_WAVES];
   anode2Signal = new double* [NUM_WAVES];
   grid1Signal = new double* [NUM_WAVES];
   grid2Signal = new double* [NUM_WAVES];


   for(int num = 0; num < NUM_WAVES; num++)
   {
	   cathodeSignal[num] = new double [RECORD_LENGTH];
	   anode1Signal[num] = new double [RECORD_LENGTH];
	   anode2Signal[num] = new double [RECORD_LENGTH];
	   grid1Signal[num] = new double [RECORD_LENGTH];
	   grid2Signal[num] = new double [RECORD_LENGTH];
   }

   for(int tS = 0; tS < RECORD_LENGTH; tS++)
	  {
	  	timeAxis[tS] = tS*TIME_SAMPLE_CONVERSION;
	  }

   for (Long64_t jentry=0; jentry < NUM_WAVES; jentry++)
   {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;

     eventChain->GetEntry(jentry);

     for(int tS = 0; tS < RECORD_LENGTH; tS++)
	   {
	  	 cathodeSignal[jentry][tS] = cwf[tS];
       anode1Signal[jentry][tS] = a0wf[tS];
       anode2Signal[jentry][tS] = a1wf[tS];
       grid1Signal[jentry][tS] = g0wf[tS];
       grid2Signal[jentry][tS] = g1wf[tS];
	   }
   }

   TCanvas* c_waves = new TCanvas("c_waves", "Signals IC");
   c_waves->Divide(3,2);


   c_waves->cd(2);
   // cout << numCathode << "cathode channels received" << endl;
   for(int nB = 0; nB < NUM_WAVES; nB++)
   {
		TGraph* g_cathode = new TGraph(RECORD_LENGTH, timeAxis, cathodeSignal[nB]);
		g_cathode->SetName("g_cathode");
		g_cathode->SetTitle("Cathode");
		g_cathode->GetYaxis()->SetRangeUser(-DIGITIZER_BITS/2, DIGITIZER_BITS/2);
		if(nB == 0) g_cathode->Draw();
		else g_cathode->Draw("SAME");

    }

   c_waves->cd(3);
   for(int nB = 0; nB < NUM_WAVES; nB++)
   {
	   TGraph* g_an1 = new TGraph(RECORD_LENGTH, timeAxis, anode1Signal[nB]);
	   g_an1->SetName("g_an1");
	   g_an1->SetTitle("Anode1");
	   g_an1->GetYaxis()->SetRangeUser(-DIGITIZER_BITS/2, DIGITIZER_BITS/2);
	   if(nB == 0) g_an1->Draw();
	   else g_an1->Draw("SAME");
   }

   c_waves->cd(4);
   for(int nB = 0; nB < NUM_WAVES; nB++)
   {
	   TGraph* g_an2 = new TGraph(RECORD_LENGTH, timeAxis, anode2Signal[nB]);
	   g_an2->SetName("g_an2");
	   g_an2->SetTitle("Anode2");
	   g_an2->GetYaxis()->SetRangeUser(-DIGITIZER_BITS/2, DIGITIZER_BITS/2);
	   if(nB == 0) g_an2->Draw();
	   else g_an2->Draw("SAME");
	}

   c_waves->cd(5);
   for(int nB = 0; nB < NUM_WAVES; nB++)
   {
	   TGraph* g_grid1 = new TGraph(RECORD_LENGTH, timeAxis, grid1Signal[nB]);
	   g_grid1->SetName("g_grid1");
	   g_grid1->SetTitle("Grid1");
	   g_grid1->GetYaxis()->SetRangeUser(-DIGITIZER_BITS/2, DIGITIZER_BITS/2);
	   if(nB == 0) g_grid1->Draw();
	   else g_grid1->Draw("SAME");
	}

   c_waves->cd(6);
   for(int nB = 0; nB < NUM_WAVES; nB++)
   {
	   TGraph* g_grid2 = new TGraph(RECORD_LENGTH, timeAxis, grid2Signal[nB]);
	   g_grid2->SetName("g_grid2");
	   g_grid2->SetTitle("Grid2");
	   g_grid2->GetYaxis()->SetRangeUser(-DIGITIZER_BITS/2, DIGITIZER_BITS/2);
	   if(nB == 0) g_grid2->Draw();
	   else g_grid2->Draw("SAME");
	}

  fragDiagnostics->cd();
  c_waves->Write();


}
