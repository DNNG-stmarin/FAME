{
   string fname = "Waveform_0_0_2.txt";
   ifstream fcount;
   fcount.open(fname.c_str());
   int nlines = 0;
   char buf[1000];
   while (fcount.getline(buf,1000))
   {
     nlines +=1;
   }
   cout << "Number of lines " << nlines << endl;
   ifstream ifile;
   ifile.open(fname.c_str());
   // here we make the array of points for the tgraph
   const unsigned int nelements = nlines-1;
   TH1F *hist = new TH1F("waveform","waveform",nelements,0,nelements);
   float thingy;
   for (int i=0;i<nlines;++i)
   {
      ifile >> thingy;
      hist->SetBinContent(i,thingy);
      
   }
   // Try to fit the dying exponential
   TF1 *expf = new TF1("expf","[0]*exp(-x/[1])+[2]",180,24000);
   expf->SetParameter(0,4500.);
   expf->SetParameter(1,5300.);
   expf->SetParameter(1,7400.);
   hist->Fit("expf","RM");
   TCanvas *canvas = new TCanvas("c","c",1200,600);
   canvas->Divide(1,2);
   canvas->cd(1);
   hist->Draw();
   //expf->SetRange(200,15000);
   //expf->Draw("same");
 
   string fname = "Waveform_0_0_1.txt";
   ifstream fcount;
   fcount.open(fname.c_str());
   int nline = 0;
   char buf[1000];
   while (fcount.getline(buf,1000))
   {
     nlines +=1;
   }
   cout << "Number of lines " << nlines << endl;
   ifstream ifile;
   ifile.open(fname.c_str());
   // here we make the array of points for the tgraph
   const unsigned int nelements = nlines-1;
   TH1F *hist2 = new TH1F("fwaveform","fwaveform",nelements,0,nelements);
   float thingy;
   for (int i=0;i<nlines;++i)
   {
      ifile >> thingy;
      hist2->SetBinContent(i,thingy);
      
   }
   canvas->cd(2);
   hist2->Draw();


}
