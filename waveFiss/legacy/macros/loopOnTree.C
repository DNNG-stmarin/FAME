#define ANODE_WF_SIZE 983
#define GRID_WF_SIZE 983
//class WaveProc;
//
void getOneEntry(TTree *t,Int_t nentry)
{
  gSystem->Load("/Users/smosby/u8analysis/trunk/smosby_work/libraries/waveform_processing_C.so");
  float a1[ANODE_WF_SIZE];
  float a1_d[ANODE_WF_SIZE];
  float a1_d2[ANODE_WF_SIZE];
  char buf[128];
  sprintf(buf,"a1[%i]",ANODE_WF_SIZE);
  t->SetBranchAddress(buf,a1);
  t->GetEntry(nentry);
  // set up filter parameters
  Int_t sample_delta = 4; // how far away to sample for each point of the derivative
  Int_t derive_point_delta = 10; // how far away to move to calculate the derivative points
  Int_t npts = ANODE_WF_SIZE;

  // make our class to do the waveform processing
  WaveProc wp;

  // calculate 1st derivative
  wp.derivative_filter(npts,a1,a1_d,sample_delta,derive_point_delta);
  // calculate 2nd derivative
  wp.double_derivative_filter(npts,a1,a1_d2,sample_delta,derive_point_delta);
  // find largest deviation for 2d derivative
  float bdelta = wp.locate_peak(npts,a1_d2);
  // now find the zero crossing in light of that
  float zcross = wp.locate_zerocrossing(npts, a1_d2, bdelta/2.);
  printf("Large delta %f zero cross %f\n",bdelta,zcross);
  // now calculate baseline
  Int_t blpts = 100;
  Int_t bloffset = 10;
  float bl = wp.calculate_baseline(blpts,bloffset, a1);
  // and get the peak height
  Int_t cipts = 200;
  Int_t cioffset = 50;
  float a1_ci = wp.calculate_peakheight_ci(cipts,zcross+cioffset,bl,a1);
  printf("baseline %f charge integral %f\n",bl,a1_ci);
}

void processFile(const char *ifname,const char *ofname)
{
  TFile *ifile = new TFile(ifname);
  TTree *itree = (TTree*)ifile->Get("wt");
  // set up the output tree
  TFile *pfile = new TFile(ofname,"recreate");
  TTree *ptree = new TTree("pt","Pulse tree from waveform analysis");
  bool doprint = kFALSE;
  float a1[ANODE_WF_SIZE];
  float a1_d[ANODE_WF_SIZE];
  float a1_d2[ANODE_WF_SIZE];
  float a2[ANODE_WF_SIZE];
  float a2_d[ANODE_WF_SIZE];
  float a2_d2[ANODE_WF_SIZE];
  float g1[GRID_WF_SIZE];
  float g1_d[GRID_WF_SIZE];
  float g1_d2[GRID_WF_SIZE];
  float g2[GRID_WF_SIZE];
  float g2_d[GRID_WF_SIZE];
  float g2_d2[GRID_WF_SIZE];
  char buf[128];
  sprintf(buf,"a1[%i]",ANODE_WF_SIZE);
  itree->SetBranchAddress(buf,a1);
  sprintf(buf,"a2[%i]",ANODE_WF_SIZE);
  itree->SetBranchAddress(buf,a2);
  sprintf(buf,"g1[%i]",GRID_WF_SIZE);
  itree->SetBranchAddress(buf,g1);
  sprintf(buf,"g2[%i]",GRID_WF_SIZE);
  itree->SetBranchAddress(buf,g2);
  // set up filter parameters
  Int_t sample_delta = 4; // how far away to sample for each point of the derivative
  Int_t derive_point_delta = 10; // how far away to move to calculate the derivative points
  Int_t npts = ANODE_WF_SIZE;
  // make our class to do the waveform processing
  WaveProc *wp = new WaveProc();

  PulseInfo *pinfo = new PulseInfo();
  ptree->Branch("pinfo","PulseInfo",&pinfo);


  Int_t nevents = itree->GetEntries();
  for (Int_t i=0;i<nevents;++i)
    {
      // start by resetting the pulse info
      pinfo->reset();

      // get the stuffs
      itree->GetEntry(i);

      // ANODE 1 BLOCK
      // calculate 1st derivative
      wp->derivative_filter(npts,a1,a1_d,sample_delta,derive_point_delta);
      // calculate 2nd derivative
      wp->double_derivative_filter(npts,a1,a1_d2,sample_delta,derive_point_delta);
      // find largest deviation for 2d derivative
      float bdelta = wp->locate_peak_neg(npts,a1_d2);
      // now find the zero crossing in light of that
      float zcross = wp->locate_zerocrossing_neg(npts, a1_d2, bdelta/2.);
      //printf("Large delta %f zero cross %f\n",bdelta,zcross);
      // now calculate baseline
      Int_t blpts = 100;
      Int_t bloffset = 10;
      float bl = wp->calculate_baseline(blpts,bloffset, a1);
      // and get the peak height
      Int_t exppts = 200;
      Int_t expoffset = 175;
      float a1_exp = wp->calculate_peakheight_exp(exppts,zcross+expoffset,bl,a1);
      if (doprint)
        {
          printf("baseline %f exp peakheight %f\n",bl,a1_exp);
        }
      // put anode 1 info in tree
      pinfo->aph[0]=a1_exp;


      // ANODE 2 BLOCK
      // calculate 1st derivative
      wp->derivative_filter(npts,a2,a2_d,sample_delta,derive_point_delta);
      // calculate 2nd derivative
      wp->double_derivative_filter(npts,a2,a2_d2,sample_delta,derive_point_delta);
      // find largest deviation for 2d derivative
      bdelta = wp->locate_peak_neg(npts,a2_d2);
      // now find the zero crossing in light of that
      zcross = wp->locate_zerocrossing_neg(npts, a2_d2, bdelta/2.);
      //printf("Large delta %f zero cross %f\n",bdelta,zcross);
      // now calculate baseline
      blpts = 100;
      bloffset = 10;
      bl = wp->calculate_baseline(blpts,bloffset, a2);
      // and get the peak height
      exppts = 200;
      expoffset = 175;
      float a2_exp = wp->calculate_peakheight_exp(exppts,zcross+expoffset,bl,a2);
      if (doprint)
        {
          printf("baseline %f exp peakheight %f\n",bl,a2_exp);
        }
      // put anode 2 info in tree
      pinfo->aph[1]=a2_exp;

      // GRID 1 BLOCK
      // calculate 1st derivative
      wp->derivative_filter(npts,g1,g1_d,sample_delta,derive_point_delta);
      // calculate 2nd derivative
      wp->double_derivative_filter(npts,g1,g1_d2,sample_delta,derive_point_delta);
      // find largest deviation for 2d derivative
      bdelta = wp->locate_peak_pos(npts,g1_d2);
      // now find the zero crossing in light of that
      zcross = wp->locate_zerocrossing_pos(npts, g1_d2, bdelta/2.);
      if (doprint)
        {
          printf("zero crossing of %d\n",zcross);
        }
      // calculate a baseline
      blpts = 100;
      bloffset = 10;
      bl = wp->calculate_baseline(blpts,bloffset, g1);
      // now calculate the peak height based on that
      Int_t cipts = 300;
      Int_t cioffset = 30;
      float g1_ci = wp->calculate_peakheight_ci(cipts,zcross+cioffset,bl,g1);
      if (doprint)
        {
          printf("g1's baseline %f ci peakheight %f\n",bl,g1_ci);
        }
      printf("g1's baseline %f ci peakheight %f event %i\n",bl,g1_ci,i);
      // put grid 1 info in tree
      pinfo->gph[0]=g1_ci/double(blpts);

      // GRID 2 BLOCK
      // calculate 1st derivative
      wp->derivative_filter(npts,g2,g2_d,sample_delta,derive_point_delta);
      // calculate 2nd derivative
      wp->double_derivative_filter(npts,g2,g2_d2,sample_delta,derive_point_delta);
      // find largest deviation for 2d derivative
      bdelta = wp->locate_peak_pos(npts,g2_d2);
      // now find the zero crossing in light of that
      zcross = wp->locate_zerocrossing_pos(npts, g2_d2, bdelta/2.);
      if (doprint)
        {
          printf("zero crossing of %d\n",zcross);
        }
      // calculate a baseline
      blpts = 100;
      bloffset = 10;
      bl = wp->calculate_baseline(blpts,bloffset, g2);
      // now calculate the peak height based on that
      Int_t cipts = 300;
      Int_t cioffset = 30;
      float g2_ci = wp->calculate_peakheight_ci(cipts,zcross+cioffset,bl,g2);
      if (doprint)
        {
          printf("g2's baseline %f ci peakheight %f\n",bl,g2_ci);
        }
      // put grid 2 info in tree
      pinfo->gph[1]=g2_ci/double(blpts);

      if (kFALSE)
        {
          // do some primitive drawing
          TH1F *temphist = new TH1F("temphist","temphist",GRID_WF_SIZE,0,GRID_WF_SIZE);
          TH1F *temphist1 = new TH1F("temphist1","temphist",GRID_WF_SIZE,0,GRID_WF_SIZE);
          TH1F *temphist2 = new TH1F("temphist2","temphist",GRID_WF_SIZE,0,GRID_WF_SIZE);
          for (Int_t i = 0;i<GRID_WF_SIZE;++i)
            {
              temphisitree->SetBinContent(i,g1[i]);
              temphist1->SetBinContent(i,g1_d[i]);
              temphist2->SetBinContent(i,g1_d2[i]);
            }
          temphisitree->Draw();
          temphist1->SetLineColor(kBlue);
          temphist1->Scale(3);
          temphist1->Draw("same");
          temphist2->SetLineColor(kRed);
          temphist2->Scale(50);
          temphist2->Draw("same");
          break;
        }
      // Fill the tree
      ptree->Fill();
    }
  ptree->Write();
  pfile->Close();
  ifile->Close();

  delete wp;
  delete pinfo;
  //delete itree;
  delete ifile;

}

bool fileGood(int fnumber)
{
  std::vector<int> badstuff;

  return kTRUE;
}

void processAllWaveforms(int lo,int hi)
{
  char buf[1000];
  char buf2[1000];
  for (Int_t i=lo;i<hi;++i)
    {
      sprintf(buf,"./data/data5_%i.root",i);
      sprintf(buf2,"./data/data5_%i_pt.root",i);
      /*
        ifile = new TFile(buf);
        itree = (TTree*)ifile->Get("wt");
        printf("Processing file %s\n",buf);
        processTree(itree,buf2);
        itree->Delete();
        ifile->Close();
        ifile->Delete();
      */
      printf("Processing file %s\n",buf);
      processFile(buf,buf2);
    }
}

TChain* makePulseChain()
{
  TChain *pt = new TChain("pt");
  char buf[1000];
  for (Int_t i=1000;i<1080;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./data/data5_%i_pt.root",i);
          pt->Add(buf);
        }
    }
  return pt;
}

TChain* makeBigEventChain()
{
  TChain *pt = new TChain("et");
  char buf[1000];
  for (Int_t i=666;i<666;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
        }
    }
  for (Int_t i=735;i<746;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
        }
    }
  /*
    for (Int_t i=747;i<1105;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
  */

  return pt;
}

TChain* makeThEventChain()
{
  TChain *pt = new TChain("et");
  char buf[1000];
  for (Int_t i=1519;i<1680;++i)
    {
      if (1)
        {
          sprintf(buf,"./tke_rootfiles/run%.4i_evt.root",i);
          pt->Add(buf);
        }
    }

  return pt;
}

TChain* makeNpEventChain()
{
  TChain *pt = new TChain("et");
  char buf[1000];
  for (Int_t i=6720;i<7408;++i)
    {
      if (1)
        {
          sprintf(buf,"./rootfiles/run%.4i_evt.root",i);
          pt->Add(buf);
        }
    }

  return pt;
}

TChain* makeNpEventChainAll()
{
  TChain *pt = new TChain("et");
  char buf[1000];
  for (Int_t i=6625;i<6809;++i)
    {
      if (1)
        {
          sprintf(buf,"./rootfiles/run%.4i_evt.root",i);
          pt->Add(buf);
        }
    }

  return pt;
}

TChain* makeBigEventChainFriends()
{
  TChain *pt = new TChain("et");
  TChain *pta = new TChain("at");
  char buf[1000];
  for (Int_t i=666;i<666;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
          sprintf(buf,"./tke_rootfiles/run%.3i_theta.root",i);
          pta->Add(buf);
        }
    }
  for (Int_t i=735;i<746;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
          sprintf(buf,"./tke_rootfiles/run%.3i_theta.root",i);
          pta->Add(buf);
        }
    }
  for (Int_t i=747;i<1105;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
          sprintf(buf,"./tke_rootfiles/run%.3i_theta.root",i);
          pta->Add(buf);
        }
    }
  pt->AddFriend("at");

  return pt;
}

TChain* makeBigEventChainLujanGammas()
{
  TChain *pt = new TChain("et");
  char buf[1000];
  /*
    for (Int_t i=1504;i<1507;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1508;i<1510;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1511;i<1514;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1515;i<1518;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1519;i<1520;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1520;i<1570;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
  */
  for (Int_t i=1904;i<2764;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evtg.root",i);
          pt->Add(buf);
        }
    }

  return pt;
}

TChain* makeBigEventChainLujan()
{
  TChain *pt = new TChain("et");
  char buf[1000];
  /*
  // a good set of lujan
  for (Int_t i=1634;i<1855;++i)
  {
  if (fileGood(i))
  {
  sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
  pt->Add(buf);
  }
  }
  */

  for (Int_t i=1755;i<1837;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
        }
    }
  /*
    for (Int_t i=2008;i<2034;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=2049;i<2074;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
  */

  return pt;
}

TChain* makeBigEventChainLujanFriends()
{
  TChain *pt = new TChain("et");
  TChain *pta = new TChain("at");
  TChain *pti = new TChain("ipt");
  TChain *ptir = new TChain("irt");
  char buf[1000];
  /*
    for (Int_t i=1504;i<1507;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1508;i<1510;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1511;i<1514;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1515;i<1518;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1519;i<1520;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
    for (Int_t i=1520;i<1570;++i)
    {
    if (fileGood(i))
    {
    sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
    pt->Add(buf);
    }
    }
  */
  //for (Int_t i=1634;i<1855;++i)
  for (Int_t i=1634;i<1638;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
          sprintf(buf,"./tke_rootfiles/run%.3i_theta.root",i);
          pta->Add(buf);
          sprintf(buf,"./tke_rootfiles/run%.3i_eloss.root",i);
          pti->Add(buf);
          sprintf(buf,"./tke_rootfiles/run%.3i_iter_nua.root",i);
          ptir->Add(buf);
        }
    }
  pt->AddFriend("at");
  pt->AddFriend("ipt");
  pt->AddFriend("irt");

  return pt;
}

TChain* makePu239PulseChain()
{
  TChain *pt = new TChain("pt0_2");
  char buf[1000];
  for (Int_t i=3060;i<3065;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%i_sp.root",i);
          pt->Add(buf);
        }
    }
  return pt;
}
TChain* makeEventChainWNRPu9()
{
  TChain *pt = new TChain("et");
  char buf[1000];
  for (Int_t i=3060;i<3093;++i)
    {
      if (fileGood(i))
        {
          sprintf(buf,"./tke_rootfiles/run%.3i_evt.root",i);
          pt->Add(buf);
        }
    }

  return pt;
}
