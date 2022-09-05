// how big are the waveforms
Int_t dec_factor = 5;
static int nsamples_cathode = 11996;
static int nsamples_anode = 4915;
void double_derivative_filter(Int_t npts,float *inarray,float *outarray,Int_t sample_delta,Int_t derive_point_delta)
{
    // global loop
    int gllim = (2.*sample_delta+1)+derive_point_delta;
    int gulim = npts - ((2.*sample_delta+1)+derive_point_delta);
    for (int i=gllim;i<gulim;++i)
    {
        float loavg = 0.;
        // calculate low average
        int lolim = i-1-2.*sample_delta-derive_point_delta;
        int uplim = i-1-derive_point_delta;
        for (int j=lolim;j<=uplim;++j)
        {
            loavg += inarray[j];
        }
        loavg /= 2.*sample_delta+1.;
        float hiavg = 0.;
        // calculate hi average
        lolim = i-1-2*sample_delta;
        uplim = i-1;
        for (int j=lolim;j<=uplim;++j)
        {
            hiavg += inarray[j];
        }
        hiavg /= 2.*sample_delta+1.;
        float derivative = (hiavg-loavg)/(2.*derive_point_delta+1.);
        // other derivative
        float loavg = 0.;
        // calculate low average
        int lolim = i+1;
        int uplim = i+1+2*sample_delta;
        for (int j=lolim;j<=uplim;++j)
        {
            loavg += inarray[j];
        }
        loavg /= 2.*sample_delta+1.;
        float hiavg = 0.;
        // calculate hi average
        lolim = i+1+derive_point_delta;
        uplim = i+1+2.*sample_delta+derive_point_delta;
        for (int j=lolim;j<=uplim;++j)
        {
            hiavg += inarray[j];
        }
        hiavg /= 2.*sample_delta+1.;
        float derivative2 = (hiavg-loavg)/(2.*derive_point_delta+1.);
        float second_derivative = (derivative2-derivative)/derive_point_delta;
        outarray[i] = second_derivative;
    }
};
void derivative_filter(Int_t npts,float *inarray,float *outarray,Int_t sample_delta,Int_t derive_point_delta)
{
    // global loop
    for (int i=sample_delta+derive_point_delta;i<npts-sample_delta-derive_point_delta;++i)
    {
        float loavg = 0.;
        // calculate low average
        int lolim = (i-sample_delta-derive_point_delta);
        int uplim = i+sample_delta-derive_point_delta;
        for (int j=lolim;j<=uplim;++j)
        {
            loavg += inarray[j];
        }
        loavg /= 2.*sample_delta+1.;
        float hiavg = 0.;
        // calculate hi average
        lolim = i-sample_delta+derive_point_delta;
        uplim = i+sample_delta+derive_point_delta;
        for (int j=lolim;j<=uplim;++j)
        {
            hiavg += inarray[j];
        }
        hiavg /= 2.*sample_delta+1.;
        float derivative = (hiavg-loavg)/(2.*derive_point_delta+1.);
        outarray[i] = derivative;
    }
};

void
TTree::filter_waveform(Int_t evt)
{
    ////////////////////////////
    // BEGIN LOGISTICAL STUFF //
    ////////////////////////////
    /*
    TFile *ifile = new TFile("data/data5_000.root");
    TTree *itree = ifile->Get("wt");
    */
    // assign the arrays for branches
    float t0[11996];
    float c[11996];
    float a1[4915];
    float g1[4915];
    float a2[4915];
    float g2[4915];
    GetLeaf("t0")->SetAddress(t0);
    GetLeaf("c")->SetAddress(c);
    GetLeaf("a1")->SetAddress(a1);
    GetLeaf("g1")->SetAddress(g1);
    GetLeaf("a2")->SetAddress(a2);
    GetLeaf("g2")->SetAddress(g2);
    //////////////////////////
    // END LOGISTICAL STUFF //
    //////////////////////////


    ////////////////////////////
    // BEGIN WAVEFORM PLAYING //
    ////////////////////////////
    GetEntry(evt);
    // we're going to play with a1 signal - 1st decimate by 5
    float a1_dec[983];
    for (int i=0;i<nsamples_anode/dec_factor;++i)
    {
        float temp = 0.;
        for (int j=0;j<dec_factor;++j)
        {
            temp += a1[dec_factor*i+j];
        }
        temp /= (float)dec_factor;
        a1_dec[i] = temp - a1[300];
    }

    // Now implement a derivative filter in a primitive way
    Int_t sample_delta = 4; // how far away to sample for each point of the derivative
    Int_t derive_point_delta = 10; // how far away to move to calculate the derivative points
    float a1_dec_deriv_filter[983];
    float a1_dec_dderiv_filter[983];
    derivative_filter(983,a1_dec,a1_dec_deriv_filter,sample_delta,derive_point_delta);
    Int_t sample_delta = 4; // how far away to sample for each point of the derivative
    Int_t derive_point_delta = 10; // how far away to move to calculate the derivative points
    double_derivative_filter(983,a1_dec,a1_dec_dderiv_filter,sample_delta,derive_point_delta);
    //////////////////////////
    // END WAVEFORM PLAYING //
    //////////////////////////

    char names[3][100] = {"h_wf","h_filter","h_ddfilter"};
    for (int i=0;i<3;++i)
    {
        if (gDirectory->Get(names[i]))
        {
            gDirectory->Get(names[i])->Delete();
        }
    }


    // draw waveforms
    TH1F *h_wf = new TH1F("h_wf","h_wf",983,0,983);
    TH1F *h_filter = new TH1F("h_filter","h_filter",983,0,983);
    TH1F *h_ddfilter = new TH1F("h_ddfilter","h_ddfilter",983,0,983);
    for (int i=0;i<983;++i)
    {
        h_wf->SetBinContent(i,a1_dec[i]);
        h_filter->SetBinContent(i,a1_dec_deriv_filter[i]);
        h_ddfilter->SetBinContent(i,a1_dec_dderiv_filter[i]);
    }
    h_ddfilter->Draw();
    /*
    h_filter->SetLineColor(kRed);
    h_ddfilter->SetLineColor(kBlue);
    h_wf->Draw();
    h_filter->Scale(20);
    h_ddfilter->Scale(100);
    h_filter->Draw("same");
    h_ddfilter->Draw("same");

    TLegend *legend = new TLegend(0.75,0.85,0.99,0.99);
    legend->AddEntry(h_wf,"Raw Waveform","l");
    legend->AddEntry(h_filter,"Derivative Filter","l");
    legend->AddEntry(h_ddfilter,"Double Derivative","l");
    legend->Draw("same");

    TLine line(0,0,900,0);
    line.Draw("same");
    */
}
