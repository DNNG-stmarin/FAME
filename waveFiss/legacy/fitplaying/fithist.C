{
   TFile *ifile = new TFile("temphist.root");
   TH1F *hist = ifile->Get("wf");
   // Try to fit the dying exponential
   TF1 *expf = new TF1("expf","[0]*exp(-(x-3)/[1])+[2]",800,1270);
   expf->FixParameter(0,-594.);
   expf->SetParameter(1,1000.);
   expf->FixParameter(2,2981.);
   expf->FixParameter(3,800.);
   hist->Fit("expf","RM");
   //TCanvas *canvas = new TCanvas("c","c",1200,800);
   //hist->Draw();
}
