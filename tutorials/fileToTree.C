
// stefanos psudeocode 

{

  TString nameFile = "../sampleData/FS3_openV5_dist_270.d";
  cout << "reading from " << nameFile << endl;
	
  // variable used for defining lengths of histories
  const int MAX_LINES = 1000; // put a large number representing the maximum number of interactions a single history can generate (i.e. the number of lines all having the same history)
 
  //attributes of each interaction
  int f_history; // stores the current history number
  int f_numLines; // this is a dynamic number (changes for every history) that stores the number of lines in the present history. If the history is not in the file, this number should be 0
  
  // variables of the tree as such they are arrays since a single history might contain more than one interaction
  // V
  int f_type[MAX_LINES] = {0};
  double f_energyDep[MAX_LINES] = {0}; // deposited 
  double f_energyInc[MAX_LINES] = {0}; // incident 
  int f_interaction[MAX_LINES] = {0};
  int f_zaid[MAX_LINES] = {0};
  int f_cell[MAX_LINES] = {0};
  double f_time[MAX_LINES] = {0};
  double f_weight[MAX_LINES] = {0};
  double f_posX[MAX_LINES] = {0};
  double f_posY[MAX_LINES] = {0};
  double f_posZ[MAX_LINES] = {0};
  int f_particle[MAX_LINES] = {0};
  // V
  int f_scatters[MAX_LINES] = {0};
  int f_code[MAX_LINES] = {0}; // fission or another interaction 
  int f_generation[MAX_LINES] = {0}; 

  
  
  //initalize collisionFile.root tree
  TFile *outfile = TFile::Open("CollisionFile.root", "RECREATE");
  TTree *collisionTree = new TTree("CollisionFile", "CollisionFile");
  collisionTree->SetFileNumber(0);
  collisionTree->SetMaxTreeSize(1000000000LL);

  
  collisionTree->Branch("History", &f_history, "History/I");
  collisionTree->Branch("NumLines", &f_numLines, "NumLines/I");
  // V check the varibles, some have to change format (doubles change to /D ?)
  collisionTree->Branch("Type", f_type, "Type[NumLines]/I");
  collisionTree->Branch("EnergyDeposited", f_energyDep, "EnergyDeposited[NumLines]/D");
  collisionTree->Branch("EnergyIncident", f_energyInc, "EnergyIncident[NumLines]/D");
  collisionTree->Branch("Interaction", f_interaction, "Interaction[NumLines]/I");
  collisionTree->Branch("Zaid", f_zaid, "Zaid[NumLines]/I");
  collisionTree->Branch("Cell", f_cell, "Cell[NumLines]/I");
  collisionTree->Branch("Time", f_time, "Time[NumLines]/D");
  collisionTree->Branch("Weight", f_weight, "Weight[NumLines]/D");
  collisionTree->Branch("PositionX", f_posX, "PositionX[NumLines]/D");
  collisionTree->Branch("PositionY", f_posY, "PositionY[NumLines]/D");
  collisionTree->Branch("PositionZ", f_posZ, "PositionZ[NumLines]/D");
  collisionTree->Branch("Particle", f_particle, "Particle[NumLines]/I");
  collisionTree->Branch("Scatters", f_scatters, "Scatters[NumLines]/I");
  collisionTree->Branch("Code", f_code, "Code[NumLines]/I");
  collisionTree->Branch("Generation", f_generation, "Generation[NumLines]/I");
  

  // open a connection with the collision file
  ifstream file(nameFile);
  if(!file.is_open())
  {
	cout << "error, file not found \n";
  }

  // initialize the strings that you need
  string line;
  double val; // this simply stores a value while you decide what to assign it to

  // loop through the entire .d file and populate the tree
  // the first history will be 1
  f_history = 1; 
  f_numLines = 0; 
  int historyNew;
  double row[16] = {0}; // V hardcoding 
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
        cout << f_history << " " << f_numLines << endl;
        collisionTree->Fill(); 

        // reset all the arrays and variables 
        f_numLines = 0; 

        /*f_type[f_numLines] = 0;
        f_energyDep[MAX_LINES] = {0}; // deposited 
        f_energyInc[MAX_LINES] = {0}; // incident 
        f_interaction[MAX_LINES] = {0};
        f_zaid[MAX_LINES] = {0};
        f_cell[MAX_LINES] = {0};
        f_time[MAX_LINES] = {0};
        f_weight[MAX_LINES] = {0};
        f_posX[MAX_LINES] = {0};
        f_posY[MAX_LINES] = {0};
        f_posZ[MAX_LINES] = {0};
        f_particle[MAX_LINES] = {0};
        f_scatters[MAX_LINES] = {0};
        f_code[MAX_LINES] = {0};
        f_generation[MAX_LINES] = {0};*/

        // loops through the skipped histories and fills them with 0 
        f_history++;
        for(f_history; f_history < historyNew; f_history++)
        {
            cout << f_history << " " << f_numLines << endl;
            collisionTree->Fill();
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

  // close out of everything
  outfile = collisionTree->GetCurrentFile();
  collisionTree->Write();
  outfile->Close();
  delete outfile;

}