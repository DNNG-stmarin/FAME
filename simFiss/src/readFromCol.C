//
//

#include "SFAME.h"
#include "Constants.h"
#include <sstream>

void sfame::readFromCol()
{
  outFile->cd();


  // initialize the strings that you need
  string line;
  double val; // this simply stores a value while you decide what to assign it to
  long int numEvents = 0;

  // loop through the entire .d file and populate the tree
  // the first history will be 1
  for(int nJ = FIRST_FILE; nJ < FIRST_FILE + NUM_FILES; nJ++ )
  {
    cout << "Read from Col File is Running" << endl;
    cout << "Reading from " << nameColFile + to_string(nJ) + ".d" << endl;
    // TFile *outfile = TFile::Open("CollisionFile.root", "RECREATE");

    // open a connection with the collision file
    ifstream file(nameColFile + to_string(nJ) + ".d");
    if(!file.is_open())
    {
      cout << "error, file " << nJ << " not found \n";
      continue;
    }


    f_history = 1;
    f_numLines = 0;
    int historyNew;
    double row[NUM_ROWS] = {0};
    int i;
    while(getline(file, line))
    {
    	// read the next line, and ignore commas
      stringstream ss(line);
      if(ss.peek() == ',') ss.ignore();

      // how to get the history for the new line before going through everything?
      ss >> val;
      historyNew = (int)val; // gets current history value
      // if at a new history value
      if(historyNew != f_history)
      {
          // fill the previous history
          //cout << f_history << " " << f_numLines << endl;
          collisionTree->Fill();
          numEvents++;

          // reset numLines
          f_numLines = 0;

          // loops through the skipped histories and fills them with 0
          f_history++;
          for(f_history; f_history < historyNew; f_history++)
          {
              //cout << f_history << " " << f_numLines << endl;
              collisionTree->Fill();
              numEvents++;
          }
          //f_history = historyNew;

      }
      // loops through and saves each value on current line
      i = 1;
      while(ss >> val)
      {
          row[i] = val;
          ++i;
      }

      // fills all of the branch varribles with that row's data
      f_particle[f_numLines] = (int)row[1];
      f_type[f_numLines] = (int)row[2];
      f_interaction[f_numLines] = (int)row[3];
      f_zaid[f_numLines] = (int)row[4];
      f_cell[f_numLines] = (int)row[5];
      f_energyDep[f_numLines] = (double)row[6];
      f_time[f_numLines] = (double)row[7];
      f_posX[f_numLines] = (double)row[8];
      f_posY[f_numLines] = (double)row[9];
      f_posZ[f_numLines] = (double)row[10];
      f_weight[f_numLines] = (double)row[11];
      f_generation[f_numLines] = (int)row[12];
      f_scatters[f_numLines] = (int)row[13];
      f_code[f_numLines] = (int)row[14];
      f_energyInc[f_numLines] = (double)row[15];

      // increase the number of lines recorded by 1
      f_numLines++;

    }

    cout << "Finished looping through collision file" << endl;

    file.close();

    cout << "Events so far: " << collisionTree->GetEntries() << endl;

  }


  // close out of everything
  outFile = collisionTree->GetCurrentFile();
  outFile->Write();

  outFile->Close();


  // cout << collisionTree << endl;
  // cout << collisionTree->LoadTree(100) << endl;
  // cout << f_history << " " << f_numLines << endl;
}
