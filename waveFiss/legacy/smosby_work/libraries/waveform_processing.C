//#include "RTypes.h"
#include "TH1.h"
#include "TF1.h"
#include <cmath>
#define ANODE_WF_SIZE 983
#define CATHODE_WF_SIZE 2399
class PulseInfo
{
    public:
        double t0t; // t0
        double ct; // cathode time
        double dt; // uncalibrated time of flight
        double cph; // cathode pulse height
        double aph[2]; // anode pulse height
        double gph[2]; // grid pulse height

    public:
        void reset();
};

void PulseInfo::reset()
{
        t0t=-1; // t0
        ct=-1; // cathode time
        dt=-1; // uncalibrated time of flight
        cph=-1; // cathode pulse height
        for (int i=0;i<2;++i)
        {
            aph[i]=-1; // anode pulse height
            gph[i]=-1; // grid pulse height
        }
}

class WaveProc
{
    public:
        void double_derivative_filter(Int_t npts,float *inarray,float *outarray,Int_t sample_delta,Int_t derive_point_delta);
        void derivative_filter(Int_t npts,float *inarray,float *outarray,Int_t sample_delta,Int_t derive_point_delta);
        float locate_zerocrossing_neg(Int_t npts, float *inarray, float armthresh);
        float locate_zerocrossing_pos(Int_t npts, float *inarray, float armthresh);
        float locate_peak_neg(Int_t npts, float *inarray);
        float locate_peak_pos(Int_t npts, float *inarray);
        float calculate_baseline(Int_t npts,Int_t offset, float *inarray);
        float calculate_peakheight_ci(Int_t npts,Int_t offset,float baseline, float *inarray); // charge integration
        float calculate_peakheight_exp(Int_t npts,Int_t offset,float baseline, float *inarray); // exponential fitting
};
float WaveProc::calculate_peakheight_exp(Int_t npts,Int_t offset,float baseline, float *inarray)
{
    TH1F *hist = new TH1F("thist","thist",ANODE_WF_SIZE,0,ANODE_WF_SIZE);
    for (int i=0;i<ANODE_WF_SIZE;++i)
    {
        hist->SetBinContent(i+1,inarray[i]);
    }

    TF1 *expf = new TF1("expf","[0]*exp(-(x-[3])/[1])+[2]",offset,offset+npts);
    expf->SetParameter(0,-40000.);
    expf->FixParameter(1,6170.);
    expf->FixParameter(2,baseline);
    expf->FixParameter(3,offset);

    hist->Fit("expf","RMQNO");
    float peakheight = expf->GetParameter(0);

    hist->Delete();
    expf->Delete();
    return peakheight;
}

float WaveProc::calculate_peakheight_ci(Int_t npts,Int_t offset,float baseline, float *inarray)
{
    // this function performs a simple charge integration
    float integral = 0;
    for (int i=offset;i<npts+offset;++i)
    {
        integral += (inarray[i]-baseline);
    }
    return integral;
}

float WaveProc::calculate_baseline(Int_t npts,Int_t offset, float *inarray)
{
    float num = 0;
    float denom = 0;
    for (int i=offset;i<offset+npts;++i)
    {
        num += inarray[i];
        denom += 1.;
    }
    return num/denom;
}

float WaveProc::locate_peak_neg(Int_t npts, float *inarray)
{
    
    float mag = 0.;
    int index = 0;
    for (int i=0;i<npts;++i)
    {
        if (inarray[i]<mag)
        {
            index = i;
            mag = inarray[i];
        }
    }
    return mag;
}
float WaveProc::locate_peak_pos(Int_t npts, float *inarray)
{
    
    float mag = 0.;
    int index = 0;
    for (int i=0;i<npts;++i)
    {
        if (inarray[i]>mag)
        {
            index = i;
            mag = inarray[i];
        }
    }
    return mag;
}
float WaveProc::locate_zerocrossing_neg(Int_t npts, float *inarray, float armthresh)
{
    bool armed = false;
    for (int i=0;i<npts;++i)
    {
        if (!armed)
        {
            if (inarray[i]<armthresh)
            {
                armed = true;
                continue;
            }
        } else
        {
            if (inarray[i]>0.)
            {
                float y1 = inarray[i-1];
                float y2 = inarray[i];
                float deltay = y2-y1;
                float frac = abs(y1/deltay);
                float indexinterp = (float)i+frac;
                return indexinterp;
            }
        }
    }
    return 0;
}
float WaveProc::locate_zerocrossing_pos(Int_t npts, float *inarray, float armthresh)
{
    bool armed = false;
    for (int i=0;i<npts;++i)
    {
        if (!armed)
        {
            if (inarray[i]>armthresh)
            {
                armed = true;
                continue;
            }
        } else
        {
            if (inarray[i]<0.)
            {
                float y1 = inarray[i-1];
                float y2 = inarray[i];
                float deltay = y2-y1;
                float frac = abs(y1/deltay);
                float indexinterp = (float)i+frac;
                return indexinterp;
            }
        }
    }
    return 0;
}

void WaveProc::double_derivative_filter(Int_t npts,float *inarray,float *outarray,Int_t sample_delta,Int_t derive_point_delta)
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
        loavg = 0.;
        // calculate low average
        lolim = i+1;
        uplim = i+1+2*sample_delta;
        for (int j=lolim;j<=uplim;++j)
        {
            loavg += inarray[j];
        }
        loavg /= 2.*sample_delta+1.;
        hiavg = 0.;
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
void WaveProc::derivative_filter(Int_t npts,float *inarray,float *outarray,Int_t sample_delta,Int_t derive_point_delta)
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
