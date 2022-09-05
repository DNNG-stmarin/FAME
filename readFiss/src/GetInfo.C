#include "readFiss.h"

#include <sstream>

using namespace std;


// readFiss main constructor
readFiss::readFiss()
{
  runNum = 0;
  expFile = nullptr;
  simFile = nullptr;
  beamFile = nullptr;
  TRIGGERS = nullptr;
  writeFile = nullptr;

  //ANN
  if (ANN_mode) crossTalkANN = new myANN();
}

void readFiss::SetInfo(MainWindow* main_in)
{
  if(main_in == nullptr)
  {
    cout << "main_in is nullptr" << endl;
    return;
  }
  w = main_in;

  cout << "Getting user input..." << endl;

  // modes
  mode = w->mode();

  CovEM_in = w->CovEM_in();

  // paths
  nameExp = w->nameExp();
  if(mode == SIM_MODE)
      nameSim = w->nameSim();
  else if(mode == BEAM_MODE)
      nameBeam = w->nameBeam();
  nameCoords = w->nameCoords();
  nameWrite = w->nameWrite();

  numExpFiles = w->numExpFiles();
  if(mode == SIM_MODE)
      numSimFiles = w->numSimFiles();

  // detectors
  NUM_DETECTORS = w->NUM_DETECTORS();
  cout << "Set detnum to " << NUM_DETECTORS << endl;
  THRESHOLD = w->THRESHOLD();
  CLIPPING = w->CLIPPING();
  MAX_TIME_N = w->MAX_TIME_N();
  MIN_TIME_N = w->MIN_TIME_N();
  MAX_TIME_P = w->MAX_TIME_P();
  MIN_TIME_P = w->MIN_TIME_P();

  // triggers
  NUM_TRIGGERS = w->NUM_TRIGGERS();
  if(TRIGGERS)
  {
    delete[] TRIGGERS;
  }
  TRIGGERS = w->TRIGGERS();
  THRESHOLD_DEP = w->THRESHOLD_DEP();
  CLIPPING_DEP = w->CLIPPING_DEP();

  // other settings for all modes
  BACKGROUND_DELAY = w->BACKGROUND_DELAY();
  FISS_PILEUP_TIME = w->FISS_PILEUP_TIME();

  // beam settings
  if(mode == BEAM_MODE)
  {
      BEAM_ERG_MIN = w->BEAM_ERG_MIN();
      BEAM_ERG_MAX = w->BEAM_ERG_MAX();
      BEAM_ERG_BINNUM = w->BEAM_ERG_BINNUM();
      sizeBerg = (BEAM_ERG_MAX - BEAM_ERG_MIN)/BEAM_ERG_BINNUM;
  }

  if(mode == FRAG_MODE)
  {
    MIN_ANGLE = w->MIN_ANGLE();
    MAX_ANGLE = w->MAX_ANGLE();
    MIN_MASS = w->MIN_MASS();
    MAX_MASS = w->MAX_MASS();
    MASS_BINNUM = w->MASS_BINNUM();
  }

  // CovEM settings
  if(CovEM_in)
  {
      MIN_N_ERG = w->MIN_N_ERG();
      MAX_N_ERG = w->MAX_N_ERG();
      MIN_P_ERG = w->MIN_P_ERG();
      MAX_P_ERG = w->MAX_P_ERG();
      BN = w->BN();
      BP = w->BP();
      BA = w->BA();

      sizeNerg = (MAX_N_ERG-MIN_N_ERG)/BN;
      sizePerg = (MAX_P_ERG-MIN_P_ERG)/BP;
      sizeNgAng = (MAX_THETA-MIN_THETA)/BA;
  }

  if(mode == SIM_MODE)
  {
    cout << "Simulation mode activated" << endl;
  }
  else if(mode == BEAM_MODE)
  {
    cout << "Beam mode activated" << endl;
  }
  else if(mode == FRAG_MODE)
  {
    cout << "Fragment mode activated" << endl;
  }
}

void readFiss::Run()
{
  cout << "\n\nPreparing for analysis..." << endl;
  // get writeFile ready

  writeFile = new TFile((TString)nameWrite + (TString)".root", "RECREATE");
  cout << "Writing output to " << ((TString)nameWrite + (TString)".root") << endl;
  if(!writeFile->IsOpen())
  {
    w->noWriteFile();
  }

  cd_basics = writeFile->mkdir("Basic");
  cd_individual = writeFile->mkdir("Individual");
  cd_FAME = writeFile->mkdir("FAME");
  cd_correlated = writeFile->mkdir("Correlated");
  cd_trigger = writeFile->mkdir("Trigger");

  // initialize experiment tree
  // TTree* tree;
  TChain *expChain = new TChain("Fiss");
  cout << "Initializing experiment tree from " << nameExp << endl;
  TString nameExpAdd = (TString)nameExp + ".root";
  expChain->Add(nameExpAdd);
  for(int expInd = 1; expInd < numExpFiles; expInd++)
  {
    nameExpAdd = (TString)nameExp + "_" + (TString)to_string(expInd) + ".root";
    expChain->Add(nameExpAdd);
  }
  InitExp(expChain);

  // if in simulation mode, initialize simulation tree
  if(mode == SIM_MODE)
  {
    cd_simComparison = writeFile->mkdir("SimComparison");

    TChain *simChain = new TChain("fissionTree");
    // cout << "Initializing simulation tree from " << nameSim << endl;
    // simFile = (TFile*)gROOT->GetListOfFiles()->FindObject(nameSim);
    // if (!simFile || !simFile->IsOpen())
    // {
    //     simFile = new TFile(nameSim);
    // }
    // simFile->GetObject("fissionTree", tree);
    TString nameSimAdd = (TString)nameSim + ".root";
    simChain->Add(nameSimAdd);
    for(int simInd = 1; simInd < numSimFiles; simInd++)
    {
      nameSimAdd = (TString)nameSim + "_" + (TString)to_string(simInd) + ".root";
      simChain->Add(nameSimAdd);
    }
    InitSim(simChain);
  }

  // if in beam mode, prep directories
  else if(mode == BEAM_MODE)
  {
    cd_beam = writeFile->mkdir("Beam");
    cd_beamTime = cd_beam->mkdir("BeamTime");
    cd_alphaSub = cd_beam->mkdir("AlphaSubtraction");
    cd_multRatio = cd_beam->mkdir("MultiplicityRatio");
    cd_multErg = cd_beam->mkdir("MultipicityErg");
    cd_stack = cd_beam->mkdir("FissionSpectrumDueToBeamErg");
    cd_LO = cd_beam->mkdir("LightOutput");
    cd_gammaSpecBeam = cd_beam->mkdir("gammaSpecBeam");

    cout << "Reading beam information from " << nameBeam << endl;
  }

  if(mode == FRAG_MODE)
  {
    cd_fragment = writeFile->mkdir("Fragment");
  }

  // change visuals for plots
  legend_x1 = 0.48;
  legend_x2 = 0.8;
  legend_y1 = 0.7;
  legend_y2 = 0.9;

  x_labelSize = 0.04;
  y_labelSize = 0.04;

  x_tickSize = 0.05;
  y_tickSize = 0.03;

  // perform analysis
  cout << "Performing analysis..." << endl;

  GenerateAngles();
  InitializeHistograms();
  InitializeFunctions();
  if(CovEM_in) InitializeCovHistograms();

  // loop through
  LoopExp();
  if(mode == SIM_MODE)
  {
    LoopSim();
  }
  if(mode == BEAM_MODE)
  {
    ReadBeamInfo();
    BeamDepAnalysis();
    BeamErgAnalysis();
    FitMult();
  }

  // correlated analysis
  // CorrAnalysis();

  // run CovEM if user wanted to
  if(CovEM_in){
    // covEM plot
    // CovEM();
    ExtractCov();
    WriteCovEM();
  }

  // plotting sections
  PlotAll();
  PlotCorr();
  if(mode == SIM_MODE)
  {
    CompareAll();
  }

  if(mode == BEAM_MODE)
  {
    PlotBeamTime();
    PlotDepSubtraction();
    PlotRatioMult();
    PlotMultErg();
    PlotStack();
    PlotLO();
    PlotBeamLO();
  }

  if(mode == FRAG_MODE)
  {
    PlotFragmentEmission();
    // PlotNeutronSawtooth();
  }

  cout << "finished" << endl;
  writeFile->Close();

  // cout << writeFile << endl;

  // ++runNum;
}

void readFiss::Save(MainWindow* main_in)
{
  if(main_in == nullptr)
  {
    cout << "main_in is nullptr" << endl;
    return;
  }
  w = main_in;

  string nameSave = w->GetNameInput();
  cout << "Saving " << nameSave << endl;
  ofstream fout(nameSave);
  if(!fout.is_open())
  {
      cout << "Couldn't open " << nameSave << " for saving" << endl;
      return;
  }

  Print(fout);
  fout.close();
}

void readFiss::Load(MainWindow* main_in)
{
  if(main_in == nullptr)
  {
    cout << "main_in is nullptr" << endl;
    return;
  }
  w = main_in;

  string nameLoad = w->GetNameInput();
  cout << "Loading " << nameLoad << endl;
  ifstream fin(nameLoad);
  if(!fin.is_open())
  {
      cout << "Couldn't open " << nameLoad << " for loading" << endl;
      return;
  }
  LoadInput(fin);
  fin.close();
}

void readFiss::Print(ostream &out)
{
    // cout <<  w->GetNameInput() << endl;
    out << w->THRESHOLD(-1) << endl;
    out << w->CLIPPING(-1) << endl;
    out << w->MIN_TIME_P(-1) << " "  << w->MAX_TIME_P(-1) << endl;
    out << w->MIN_TIME_N(-1) << " "  << w->MAX_TIME_N(-1) << endl;
    out << w->THRESHOLD_DEP(-1) << endl;
    out << w->CLIPPING_DEP(-1) << endl;
    out << w->BACKGROUND_DELAY(-1) << endl;
    out << w->FISS_PILEUP_TIME(-1)<< endl;

    out << w->mode() << endl;
    out << w->CovEM_in() << endl;

    out << w->nameWrite() << endl;
    out << w->nameExp() << endl;
    out << w->numExpFiles() << endl;
    if(mode == SIM_MODE)
    {
        out << w->nameSim() << endl;
        out << w->numSimFiles() << endl;
    }
    else if(mode == BEAM_MODE)
    {
        out << w->nameBeam() << endl;
    }
    out << w->nameCoords() << endl;

    out << w->NUM_DETECTORS() << endl;
    // out << w->THRESHOLD() << endl;
    // out << w->CLIPPING() <<  endl;
    // out << w->MAX_TIME_N() << endl;

    out << w->NUM_TRIGGERS() << endl;
    int* saveTRIGGERS = w->TRIGGERS();
    for(int i = 0; i < w->NUM_TRIGGERS(); ++i)
    {
        out << saveTRIGGERS[i] << " ";
    }
    delete[] saveTRIGGERS;
    out << endl;

    if(w->CovEM_in())
    {
        out << w->MIN_N_ERG() << " " << w->MAX_N_ERG() << " ";
        out << w->MIN_P_ERG() << " " << w->MAX_P_ERG() << endl;
        out << w->BN() << " " << w->BP() << " " << w->BA() << endl;
    }

    if(w->mode() == BEAM_MODE)
    {
        out << w->BEAM_ERG_MIN() << " " << w->BEAM_ERG_MAX() << " ";
        out << w->BEAM_ERG_BINNUM() << endl;
    }

    if(w->mode() == FRAG_MODE)
    {
        out << w->MIN_ANGLE() << " " << w->MAX_ANGLE() << endl;
        out << w->MIN_MASS() << " " << w->MAX_MASS()  << " " << w->MASS_BINNUM() << endl;
    }
}

void readFiss::LoadInput(istream &in)
{
    // load in settings
    string word;
    int integer;
    double dbl;
    cout << "Loading Input" << endl;

    // cout << w << endl;

    // range check boxes
    in >> dbl;
    w->THRESHOLD(dbl);
    in >> dbl;
    w->CLIPPING(dbl);

    in >> dbl;
    w->MIN_TIME_P(dbl);
    in >> dbl;
    w->MAX_TIME_P(dbl);
    in >> dbl;
    w->MIN_TIME_N(dbl);
    in >> dbl;
    w->MAX_TIME_N(dbl);

    in >> dbl;
    w->THRESHOLD_DEP(dbl);
    in >> dbl;
    w->CLIPPING_DEP(dbl);
    in >> dbl;
    w->BACKGROUND_DELAY(dbl);
    in >> dbl;
    w->FISS_PILEUP_TIME(dbl);

    // modes
    in >> integer;
    w->mode(integer);

    in >> integer;
    w->CovEM_in(integer);

    // paths
    in >> word;
    w->nameWrite(word);
    in >> word;
    w->nameExp(word);
    in >> integer;
    w->numExpFiles(integer);

    if(w->mode() == SIM_MODE)
    {
        in >> word;
        w->nameSim(word);
        in >> integer;
        w->numSimFiles(integer);
    }

    else if(w->mode() == BEAM_MODE)
    {
        cout << "Beam mode on" << endl;
        in >> word;
        w->nameBeam(word);
    }

    in >> word;
    w->nameCoords(word);

    // detectors
    in >> integer;
    w->NUM_DETECTORS(integer);
    // triggers
    in >> integer;
    w->NUM_TRIGGERS(integer);
    int loadTRIGGERS[w->NUM_TRIGGERS()];
    for(int i = 0; i < w->NUM_TRIGGERS(); ++i)
    {
      in >> loadTRIGGERS[i];
    }
    w->TRIGGERS(loadTRIGGERS);

    // CovEM settings
    if(w->CovEM_in() == 1)
    {
        in >> dbl;
        w->MIN_N_ERG(dbl);
        in >> dbl;
        w->MAX_N_ERG(dbl);
        in >> dbl;
        w->MIN_P_ERG(dbl);
        in >> dbl;
        w->MAX_P_ERG(dbl);
        in >> integer;
        w->BN(integer);
        in >> integer;
        w->BP(integer);
        in >> integer;
        w->BA(integer);

        //cout << w->BN() << " " << w->BP() << " " << w->BA() << endl;
    }

    // beam settings
    if(w->mode() == BEAM_MODE)
    {
        in >> dbl;
        w->BEAM_ERG_MIN(dbl);
        in >> dbl;
        w->BEAM_ERG_MAX(dbl);
        in >> integer;
        w->BEAM_ERG_BINNUM(integer);
    }

    // frag settings
    if(w->mode() == FRAG_MODE)
    {
        in >> dbl;
        w->MIN_ANGLE(dbl);
        in >> dbl;
        w->MAX_ANGLE(dbl);
        in >> dbl;
        w->MIN_MASS(dbl);
        in >> dbl;
        w->MAX_MASS(dbl);
        in >> integer;
        w->MASS_BINNUM(integer);

    }
}
