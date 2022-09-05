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

void readFiss::PlotTrigger()
{
  writeFile->cd();
  cd_trigger->cd();

  h_fissRej->Write();
  h_fissTriggers->Write();

  for(int ind = 0; ind < NUM_TRIGGERS; ind++)
  {
    h_timeDiffTrig[ind]->Write();
  }

}
