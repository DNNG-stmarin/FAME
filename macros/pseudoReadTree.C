// structure of code to read tree the correct way
{


  TString nameFile = "put the name of the .d file here"
  cout << "reading from " << nameFile << endl;

  // variable used for defining lengths of histories
  const int MAX_LINES = 1000; // put a large number representing the maximum number of interactions a single history can generate (i.e. the number of lines all having the same history)


  //attributes of each interaction
  int f_history; // stores the current history number
  int f_numLines; // this is a dynamic number (changes for every history) that stores the number of lines in the present history. If the history is not in the file, this number should be 0

  // variables of the tree as such they are arrays since a single history might contain more than one interaction
  int f_type[MAX_LINES] = {0};
  double f_energy[MAX_LINES] = {0};
  //
  // continue adding the rest of the columns of the collision file here, making sure you use the correct type (int vs double). Weight is double.
  //

   //initalize collisionFile.root tree
  TFile *outfile = TFile::Open(nameColTree, "RECREATE");
  TTree *collisionTree = new TTree("CollisionFile", "CollisionFile");
  fissionTree->SetFileNumber(0);
  fissionTree->SetMaxTreeSize(1000000000LL);

  // initialie the branches (one for each of the variables defined above, see root documentation for the meaning of these, as it will change for some variables)
  fissionTree->Branch("History", &f_historuy, "History/I");
  fissionTree->Branch("NumLines", &f_numLines, "NumLines/I");

  fissionTree->Branch("Type", f_type, "Type[NumLines]/I");
  fissionTree->Branch("Energy", f_energy, "Energy[NumLines]/I");


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
  while(getline(file, line))
  {


  	// read the next line, and ignore commas
    stringstream ss(line);
    if(ss.peek() == ',') ss.ignore();


    // gather the history from the line
    // if new history{
    // fill the previous history and reset all the arrays and variables (also fill any history in between with f_numLines = 0, i.e. if the currenthistory is 20, and the prev history is 12,
    // fill 12 with the content of that history, and 13, 14, 15, 16, 17, 18, 19 with just f_numLines = 0 and f_history equal to those histories.)
    // filling a tree is easy, you can use
    // collisionTree->Fill();

    // while(ss >> val){ }
    // here you are looping through the columns of a single interaction. Let's say that the "val" you have stored is the energy deposited, then:
    // f_energy = (double)val;
    // and fill all the variables defined above

    // increase the number of lines recorded by 1
    // f_numLines++;
  }

  // close out of everything
  outfile = collisinoTree->GetCurrentFile();
  collisinoTree->Write();
  outFile->Close();
  delete outfile;






}
