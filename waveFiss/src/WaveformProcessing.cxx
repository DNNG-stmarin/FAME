////////////////////////////////////////////////////////////////////////////////
///  \file   WaveformProcessing.cxx
///  \author Devin S. Connolly
///  \email  dconnolly@lanl.gov
///  \author Shea M. Mosby
///  \email  smosby@lanl.gov
///  \brief  implements waveformProcessing.h
////////////////////////////////////////////////////////////////////////////////

#include "WaveformProcessing.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TSpline.h"

////////////////////////////////////////////////////////////////////////////////
bool IsValid(Double_t par)
{
  if (par == -1) {
    return false;
  } else {
    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////
void TKESettings::reset()
{
  for (int eye = 0; eye < MAXNBOARDS; eye++) {
    for (int jay = 0; jay < MAXNCHANNELS; ++jay) {
      sampleDelta[eye][jay]      = 4;
      derivePointDelta[eye][jay] = 10;
      Ns[eye][jay]               = 4096;
      bLineOS[eye][jay]          = 1;
      bLineNpts[eye][jay]        = 10;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void ProcessedRootEvent::reset()
{
  bnum     = 255;
  chnum    = 255;
  ts       = 0;
  tTrig    = -1;
  tPeak    = -1;
  peak     = -1;
  baseline = -1;
#ifdef KEEP_WAVEFORMS
  for (int eye = 0; eye < WF_SIZE; eye++) {
    wf[eye]   = 0;
    Eflt[eye] = 0;
    Tflt[eye] = 0;
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////
void GammaSingles::reset()
{
  bnum     = 255;
  chnum    = 255;
  t0t      = -1;
  dt       = -1;
  ge       = -1;
  baseline = -1;
}

////////////////////////////////////////////////////////////////////////////////
void EventInfo::reset()
{
  t0t = -1; // t0
  ct  = -1; // cathode time
  dt  = -1; // uncalibrated time of flight
  cph = -1; // cathode pulse height ccoinc = false;
#ifdef SAVE_WAVEFORMS
  for (int eye = 0; eye < WF_SIZE; eye++) {
    a0wf[eye]   = 0;
    a1wf[eye]   = 0;
    cwf[eye]    = 0;
    g0wf[eye]   = 0;
    g1wf[eye]   = 0;
    sum0wf[eye] = 0;
    sum1wf[eye] = 0;
#ifdef WRITE_FILTERS
    sum0Eflt[eye] = 0;
    sum1Eflt[eye] = 0;
    sum0Tflt[eye] = 0;
    sum1Tflt[eye] = 0;
#endif
  }
#endif
  for (int eye = 0; eye < 8; eye++) { tTrig[eye] = -1; }

  for (int eye = 0; eye < 2; eye++) {
    tTrig[eye] = -1;
    aph[eye]   = -1; // anode pulse height
    gph[eye]   = -1; // grid pulse height
#ifdef SAVE_WAVEFORMS
    sumph[eye];
    sumtTrig[eye];
    sumtPeak[eye];
    sumBl[eye];
#endif
#ifndef GAMMAS_OFF
    gt[eye] = -1;
    ge[eye] = -1;
#endif
  }
#ifndef GAMMAS_OFF
  gmult = 0;
  for (int eye = 0; eye < 10; eye++) {
    gtl[eye]  = -1;
    gel[eye]  = -1;
    gchl[eye] = -1;
  }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// CAEN_DGTZ_Event Class /////////////////////////////////
void CAEN_DGTZ_Event::reset()
{
  gD = new TGraph();
  gW = new TGraph();
  gE = new TGraph();
  gWF = new TGraph();
  gW->SetName("gW");
  gD->SetName("gD");
  gE->SetName("gE");
  gWF->SetName("gWF");

  // cout << WF_SIZE << endl;
  bnum  = 255;
  chnum = 255;
  ts    = 0;

  for (int eye = 0; eye < WF_SIZE; eye++) {
    wf[eye]   = 0;
    Eflt[eye] = 0;
    Tflt[eye] = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// calculate peak height with a simple charge integration
Double_t CAEN_DGTZ_Event::calcPhChargeInt(Int_t Ns, Int_t offset, Short_t* wf)
{
  Double_t integral = 0;
  for (int eye = offset; eye < offset + Ns; eye++) {
    integral += (double)wf[eye];
    ;
    // cout << "amp " << wf[eye] << " baseline " << baseline << " integral " << integral <<
    // endl;
  }
  // cout << "final integral " << integral << endl;
  return integral / ((double)Ns);
}

////////////////////////////////////////////////////////////////////////////////
/// calculate peak height with a simple charge integration

Double_t CAEN_DGTZ_Event::calcPhGrid(Int_t Ns, Int_t offset, Int_t Npts, Short_t* wf)
{
  Int_t max = -10000;
  int maxInd = -1;
  for (int eye = 0; eye < Ns; eye++)
  {
    if (wf[eye] > max)
    {
      max = wf[eye];
      maxInd = eye;
    }
  }

  Double_t integral = 0;

  for (int eye = maxInd + offset; eye < maxInd + offset + Npts; eye++)
  {
    integral += (double)wf[eye];
  }
  // cout << "maxInd " << maxInd << " final integral " << integral << endl;
  return -1 * integral / ((double)Npts);
}



/*
WE ARE NOT JUST DOING SIMPLE CHARGE INTEGRATION. WE CAN DO BETTER.

Double_t CAEN_DGTZ_Event::calcPhGrid(Int_t Ns, Int_t Npts, Short_t* wf)
{
  Double_t integral = 0;
  for (int eye = Ns - Npts; eye < Ns; eye++) {
    integral += (double)wf[eye];
    ;
    // cout << "amp " << wf[eye] << " baseline " << baseline << " integral " << integral <<
    // endl;
  }
  // cout << "final integral " << integral << endl;
  return integral / ((double)Npts);
}
*/

////////////////////////////////////////////////////////////////////////////////
/// calculate peak height by fitting with an exponential function
Double_t CAEN_DGTZ_Event::calcPhExp(Int_t Ns, Int_t Npts, Int_t offset, Double_t baseline,
                                    Short_t* wf, Double_t phGuess, Double_t tau)
{
  TH1F* h0 = new TH1F("h0", "h0", Ns, 0, Ns);
  for (int eye = 0; eye < Ns; eye++) {
    h0->SetBinContent(eye + 1, wf[eye]);
    // cout << "wf 4 fit " << eye << " " << wf[eye] << endl;
  }

  TF1* expf = new TF1("expf", "[0]*exp(-(x-[3])/[1])+[2]", offset, offset + Npts);
  expf->SetParameter(0, phGuess);
  expf->FixParameter(1, tau);
  expf->FixParameter(2, baseline);
  expf->FixParameter(3, offset);
  // cout << "ph guess " << phGuess << " decay " << tau << " baseline " << baseline << "
  // offset " << offset << " end " << offset+Ns << endl;
  h0->Fit("expf", "MNQR0");
  // h0->Fit("expf");
  Double_t ph = expf->GetParameter(0);
  h0->Delete();
  expf->Delete();
  // cout << "ph " << ph << endl;
  return ph;
}

////////////////////////////////////////////////////////////////////////////////
Double_t CAEN_DGTZ_Event::calcBaseline(Int_t Ns, Int_t offset, Short_t* wf)
{
  Double_t num   = 0;
  Double_t denom = 0;
  // cout << "input Ns " << Ns << "offset " << offset << endl;
  for (int eye = offset; eye < offset + Ns; eye++) {
    // cout << wf[eye] << endl;
    num += wf[eye];
    denom += 1.;
  }
  // cout << "num " << num << "denom " << denom << endl;
  return num / denom;
}

////////////////////////////////////////////////////////////////////////////////
void CAEN_DGTZ_Event::CFD(Int_t Ns, Short_t* wf, Double_t* CFD, Int_t delay, Double_t frac)
{
  for (int eye = delay; eye < Ns; eye++) { CFD[eye] = frac * wf[eye] - wf[eye - delay]; }
};

////////////////////////////////////////////////////////////////////////////////
void CAEN_DGTZ_Event::derivativeFilter(Int_t Ns, Short_t* wf, Double_t* outarray, Int_t sampleDelta,
                                       Int_t derivePointDelta)
{
  // global loop
  for (int eye = sampleDelta + derivePointDelta; eye < Ns - sampleDelta - derivePointDelta; eye++) {
    Double_t loavg = 0.;
    // calc low average
    int lolim = (eye - sampleDelta - derivePointDelta);
    int uplim = eye + sampleDelta - derivePointDelta;
    for (int jay = lolim; jay <= uplim; ++jay) {
      // cout << "filtering in array " << wf[jay] << endl;
      loavg += wf[jay];
    }
    loavg /= 2. * sampleDelta + 1.;
    Double_t hiavg = 0.;
    // calc hi average
    lolim = eye - sampleDelta + derivePointDelta;
    uplim = eye + sampleDelta + derivePointDelta;
    for (int jay = lolim; jay <= uplim; ++jay) { hiavg += wf[jay]; }
    hiavg /= 2. * sampleDelta + 1.;
    Double_t derivative = (hiavg - loavg) / (2. * derivePointDelta + 1.);
    outarray[eye]       = derivative;
  }
};

////////////////////////////////////////////////////////////////////////////////
void CAEN_DGTZ_Event::doubleDerivativeFilter(Int_t Ns, Short_t* wf, Double_t* outarray,
                                             Int_t sampleDelta, Int_t derivePointDelta,
                                             TGraph* gW, TGraph* gD)
{
  // global loop
  int gllim = (2. * sampleDelta + 1) + derivePointDelta;
  int gulim = Ns - ((2. * sampleDelta + 1) + derivePointDelta);
  for (int eye = gllim; eye < gulim; eye++) {
    Double_t loavg = 0.;
    // calc low average
    int lolim = eye - 1 - 2. * sampleDelta - derivePointDelta;
    int uplim = eye - 1 - derivePointDelta;
    for (int jay = lolim; jay <= uplim; ++jay) { loavg += wf[jay]; }
    loavg /= 2. * sampleDelta + 1.;
    Double_t hiavg = 0.;
    // calc hi average
    lolim = eye - 1 - 2 * sampleDelta;
    uplim = eye - 1;
    for (int jay = lolim; jay <= uplim; ++jay) { hiavg += wf[jay]; }
    hiavg /= 2. * sampleDelta + 1.;
    Double_t derivative = (hiavg - loavg) / (2. * derivePointDelta + 1.);
    // other derivative
    loavg = 0.;
    // calc low average
    lolim = eye + 1;
    uplim = eye + 1 + 2 * sampleDelta;
    for (int jay = lolim; jay <= uplim; ++jay) { loavg += wf[jay]; }
    loavg /= 2. * sampleDelta + 1.;
    hiavg = 0.;
    // calc hi average
    lolim = eye + 1 + derivePointDelta;
    uplim = eye + 1 + 2. * sampleDelta + derivePointDelta;
    for (int jay = lolim; jay <= uplim; ++jay) { hiavg += wf[jay]; }
    hiavg /= 2. * sampleDelta + 1.;
    Double_t derivative2       = (hiavg - loavg) / (2. * derivePointDelta + 1.);
    Double_t second_derivative = (derivative2 - derivative) / derivePointDelta;
    outarray[eye]              = second_derivative;
    // cout << "i " << eye << " original waveform " << wf[eye] << " flt " << outarray[eye] <<
    // " deriv 1 " << derivative << " deriv 2 " << derivative2 << endl;

    gW->SetPoint(gW->GetN(), eye, wf[eye]);
    gD->SetPoint(gD->GetN(), eye, second_derivative);

  }
};

////////////////////////////////////////////////////////////////////////////////
void CAEN_DGTZ_Event::fastFilter(Int_t Ns, Short_t* wf, Double_t* ff, Int_t T_r, Int_t T_clk,
                                 Double_t tau)
{
  Double_t d_kl = 0;
  Double_t p_n  = 0;
  Double_t r_n  = 0;
  Double_t s_n  = 0;
  Double_t M    = 1.0 / (exp(((double)T_clk) / tau) - 1);
  for (Int_t en = 0; en < Ns; en++) {
    if (en < 2 * T_r) {
      ff[en] = 0;
      continue;
    } else {
      d_kl = wf[en];
      d_kl -= 2 * wf[en - T_r];
      d_kl += wf[en - 2 * T_r];
    }
    p_n += d_kl;
    r_n = p_n + M * d_kl;
    s_n += r_n;
    ff[en] = s_n;
    ff[en] /= (T_r * tau);
  }
}

////////////////////////////////////////////////////////////////////////////////
Int_t CAEN_DGTZ_Event::ffExtrapTs(Int_t Ns, Double_t* ff, Int_t offset, Int_t tRise,
                                  Double_t thresh)
{
  bool  armed  = false;
  Int_t t_half = 0;
  for (int eye = offset; eye < Ns; eye++) {
    if (!armed) {
      if (wf[eye] > thresh / 2.0) {
        t_half = eye;
        armed  = true;
        continue;
      }
    } else {
      if (wf[eye] > thresh) { return 2 * t_half - eye; }
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
void CAEN_DGTZ_Event::ffCFD(Int_t Ns, Short_t* wf, Double_t* CFD, Int_t T_r, Int_t T_clk,
                            Double_t tau, Int_t delay, Double_t frac)
{
  Double_t ff[Ns];
  Double_t d_kl = 0;
  Double_t p_n  = 0;
  Double_t r_n  = 0;
  Double_t s_n  = 0;
  Double_t M    = 1.0 / (exp(((double)T_clk) / tau) - 1);
  for (Int_t en = 0; en < Ns; en++) {
    if (en < 2 * T_r) {
      ff[en] = 0;
      continue;
    } else {
      d_kl = wf[en];
      d_kl -= 2 * wf[en - T_r];
      d_kl += wf[en - 2 * T_r];
    }
    p_n += d_kl;
    r_n = p_n + M * d_kl;
    s_n += r_n;
    ff[en] = s_n;
    ff[en] /= (T_r * tau);
  }
  for (int eye = delay; eye < Ns; eye++) { CFD[eye] = frac * ff[eye] - ff[eye - delay]; }
}

////////////////////////////////////////////////////////////////////////////////
/// calc peak height height using trapezoidal filter
Double_t CAEN_DGTZ_Event::getPhTrapFilter(Int_t Ns, Double_t* tf, Int_t offset, Int_t tRise,
                                          Int_t tPeak, Int_t nPeak)
{
  Int_t    t0  = offset + tRise + tPeak - nPeak + 1;
  Int_t    t_f = offset + tRise + tPeak;
  // cout << "Been trappin " << t0 << " " << t_f << endl;
  Double_t ph  = 0;
  for (Int_t eye = t0; eye <= t_f; eye++) { ph += tf[eye]; }
  return ph / ((double)nPeak);
}

////////////////////////////////////////////////////////////////////////////////
Double_t CAEN_DGTZ_Event::locatePeak(Int_t Ns, Double_t* wf)
{
  Double_t mag   = 0.;
  int      index = 0;
  for (int eye = 0; eye < Ns; eye++) {
    if (wf[eye] > mag) {
      index = eye;
      mag   = wf[eye];
    }
  }
  return mag;
}

////////////////////////////////////////////////////////////////////////////////
/// simple location of zero crossing - takes sample after zero crossing as ts
Double_t CAEN_DGTZ_Event::locate0X(Int_t Ns, Double_t* wf, Double_t armthresh, Int_t offset)
{
  bool armed = false;
  for (int eye = offset; eye < Ns; eye++) {
    if (!armed) {
      if (wf[eye] > armthresh) {
        armed = true;
        continue;
      }
    } else {
      if (wf[eye] < 0.) { return eye; }
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
Double_t CAEN_DGTZ_Event::linearInterp0X(Int_t Ns, Double_t* wf, Double_t armthresh, Int_t offset,
                                         Int_t Npts)
{
  TGraph* g0 = new TGraph();
  for (int eye = 0; eye < Ns; eye++) { g0->SetPoint(eye, eye, wf[eye]); }
  bool armed = false;
  for (int eye = offset; eye < Ns; eye++) {
    if (!armed) {
      if (wf[eye] > armthresh) {
        armed = true;
        continue;
      }
    } else {
      if (wf[eye] < 0.) {
        Int_t x0 = eye - Npts / 2;
        Int_t xf = eye + Npts / 2 - 1;

        TF1* fit = new TF1("fit", "pol1", x0, xf);
        fit->SetParameters(100, -10.0);
        g0->Fit("fit", "MNQR0");
        Double_t zeroInterp = fit->GetX(0, x0, xf);
        g0->Delete();
        fit->Delete();
        // cout << "zeroInterp = " << zeroInterp << "\n";
        return zeroInterp;
      }
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
Double_t CAEN_DGTZ_Event::cubicInterp0X(Int_t Ns, Double_t* wf, Double_t armthresh, Int_t offset,
                                        Int_t Npts)
{
  TGraph* g0 = new TGraph();
  for (int eye = 0; eye < Ns; eye++) { g0->SetPoint(eye, eye, wf[eye]); }
  bool armed = false;
  for (int eye = offset; eye < Ns; eye++) {
    if (!armed) {
      if (wf[eye] > armthresh) {
        armed = true;
        // cout << "armed" << endl;
        continue;
      }
    } else {
      if (wf[eye] < 0.) {
        // cout << "Found cross at " << eye << endl;
        Int_t x0  = eye - Npts / 2;
        Int_t xf  = eye + Npts / 2 - 1;
        TF1*  fit = new TF1("fit", "pol3", x0, xf);
        fit->SetNpx(100);
        fit->SetParameters(-1.0e3, 10.0, -0.02, 1.0e-5);
        g0->Fit("fit", "MNQR0");
        Double_t zeroInterp = fit->GetX(0, x0, xf);
        g0->Delete();
        fit->Delete();
        // cout << "zeroInterp = " << zeroInterp << "\n";
        return zeroInterp;
      }
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
Double_t CAEN_DGTZ_Event::cSplineInterp0X(Int_t Ns, Double_t* wf, Double_t armthresh, Int_t offset,
                                          Int_t Npts, Double_t tolerance)
{
  bool      armed      = false;
  Double_t  zeroInterp = 0.0;
  TSpline3* s3;
  Double_t  xx[Npts];
  Double_t  yy[Npts];

  for (int eye = 0; eye < Ns; eye++) {
    if (!armed) {
      if (wf[eye] > armthresh) {
        armed = true;
        continue;
      }
    } else {
      if (wf[eye] < 0.) {
        for (Int_t jay = 0; jay <= Npts; jay++) {
          xx[jay] = (eye - Npts / 2 + jay);
          yy[jay] = (wf[eye - Npts / 2 + jay]);
        }
        TSpline3* s3        = new TSpline3("", xx, yy, Npts);
        bool      converged = false;
        Double_t  step      = 0.5;
        Double_t  xeval     = eye - step;
        Double_t  eval;
        while (!converged) {
          eval = s3->Eval(xeval);
          if (fabs(eval) < tolerance) {
            converged  = true;
            zeroInterp = xeval;
            break;
          } else {
            step *= 0.5;
            if (eval > 0) { xeval += step; }
            if (eval < 0) { xeval -= step; }
          }
        }
        break;
      }
    }
  }
  return zeroInterp;
}

////////////////////////////////////////////////////////////////////////////////
void CAEN_DGTZ_Event::trapFilter(Int_t Ns, Short_t* wf, Double_t* tf, Int_t zcross, Int_t T_r,
                                 Int_t T_g, Int_t T_clk, Double_t tau)
{
  Double_t d_kl = 0;
  Double_t p_n  = 0;
  Double_t r_n  = 0;
  Double_t s_n  = 0;
  Double_t M    = 1.0 / (exp(((double)T_clk) / tau) - 1);
  Int_t    ell  = T_g + T_r;
  for (Int_t en = zcross; en < Ns; en++) {
    d_kl = wf[en];
    if (en - T_r >= zcross) { d_kl -= wf[en - T_r]; } // In the gap
    if (en - ell >= zcross) { d_kl -= wf[en - ell]; } // In the fall time
    if (en - T_r - ell >= zcross) { d_kl += wf[en - T_r - ell]; } // Past fall
    p_n += d_kl;
    r_n = p_n + M * d_kl;
    s_n += r_n;
    tf[en] = s_n;
    tf[en] /= (T_r * tau);
    // save energy filter
    gE->SetPoint(gE->GetN(), en, tf[en]);
  }

}

////////////////////////////////////////////////////////////////////////////////
/// Single delay line filter
void CAEN_DGTZ_Event::sdlFilter(Int_t Ns, Short_t* wf, Double_t* sdl, Int_t delay, Double_t tau)
{
  for (Int_t eye = 0; eye < Ns; eye++) {
    if (eye >= delay)
      sdl[eye] = wf[eye] - wf[eye - delay] * exp(-delay / tau);
    else
      sdl[eye] = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Single delay line filter + CFD
void CAEN_DGTZ_Event::sdlCFD(Int_t Ns, Short_t* wf, Double_t* CFD, Int_t ff_rise, Double_t tau,
                             Int_t delay, Double_t frac)
{
  Double_t sdl[Ns];
  for (Int_t eye = 0; eye < Ns; eye++) {
    if (eye >= ff_rise)
      sdl[eye] = wf[eye] - wf[eye - ff_rise] * exp(-ff_rise / tau);
    else
      sdl[eye] = 0;
  }
  for (int eye = delay; eye < Ns; eye++) { CFD[eye] = frac * sdl[eye] - sdl[eye - delay]; }
}

////////////////////////////////////////////////////////////////////////////////
/// this is a critical function - it defines  how all the wavefoms get processed
Int_t CAEN_DGTZ_Event::processWf(TKESettings tke, double* tTrig, int* tPeak, double* peak,
                                 double* baseline, TGraph* gWF)
{

  for (Int_t eye = 0; eye < tke.Ns[bnum][chnum]; eye++) // Store current wf in graph
  {
    gWF->SetPoint(gWF->GetN(), eye, wf[eye]);
  }

  // invert negative waveforms
  if (tke.polarity[bnum][chnum].compare("negative") == 0) {
    for (Int_t eye = 0; eye < tke.Ns[bnum][chnum]; eye++) { wf[eye] = pow(2, NBITSADC) - wf[eye]; }
  }
#ifdef SMOOTHING
  if (this->chnum == 2 || this->chnum == 3) {
    TH1D*     hsmooth;
    Double_t temp[tke.Ns[bnum][chnum]];
    for (Int_t eye = 0; eye < tke.Ns[bnum][chnum]; eye++) { temp[eye] = (Double_t)wf[eye]; }
    hsmooth->SmoothArray(tke.Ns[bnum][chnum], temp, 1);
    for (Int_t eye = 0; eye < tke.Ns[bnum][chnum]; eye++) { wf[eye] = (Short_t)temp[eye]; }
  }
#endif
  // find the baseline
  Double_t bl = calcBaseline(tke.bLineNpts[bnum][chnum], tke.bLineOS[bnum][chnum], this->wf);
  // cout << "baseline " << bl << endl;
  *baseline = bl;

  if (!(tke.eMethod[bnum][chnum].compare("expFit") == 0)) {
    for (Int_t eye = 0; eye < tke.Ns[bnum][chnum]; eye++) { wf[eye] -= bl; }
  }

  // filter the waveform (timing)
  if (tke.tMethod[bnum][chnum].compare("cfd") == 0 ||
      tke.tMethod[bnum][chnum].compare("CFD") == 0) {
    CFD(tke.Ns[bnum][chnum], this->wf, this->Tflt, tke.delay[bnum][chnum], tke.frac[bnum][chnum]);
  } else if (tke.tMethod[bnum][chnum].compare("derivative") == 0) {
    // derivativeFilter(tke.Ns[bnum][chnum], this->wf, this->Eflt, tke.sampleDelta[bnum][chnum],
    //                  tke.derivePointDelta[bnum][chnum]);
    doubleDerivativeFilter(tke.Ns[bnum][chnum], this->wf, this->Tflt, tke.sampleDelta[bnum][chnum],
                           tke.derivePointDelta[bnum][chnum], gW, gD);
  } else if (tke.tMethod[bnum][chnum].compare("ffCFD") == 0 ||
             tke.tMethod[bnum][chnum].compare("ffcfd") == 0) {
    ffCFD(tke.Ns[bnum][chnum], this->wf, this->Tflt, tke.ffRise[bnum][chnum], 1,
          tke.tau[bnum][chnum], tke.delay[bnum][chnum], tke.frac[bnum][chnum]);
  } else if (tke.tMethod[bnum][chnum].compare("sdlCFD") == 0 ||
             tke.tMethod[bnum][chnum].compare("sdlcfd") == 0) {
    sdlCFD(tke.Ns[bnum][chnum], this->wf, this->Tflt, tke.ffRise[bnum][chnum],
           tke.tau[bnum][chnum], tke.delay[bnum][chnum], tke.frac[bnum][chnum]);
  } else if (tke.tMethod[bnum][chnum].compare("sdl") == 0 ||
             tke.tMethod[bnum][chnum].compare("sdlFilter") == 0 ||
             tke.tMethod[bnum][chnum].compare("sdlFlt") == 0) {
    sdlFilter(tke.Ns[bnum][chnum], this->wf, this->Tflt, tke.ffRise[bnum][chnum],
              tke.tau[bnum][chnum]);
  } else if (tke.tMethod[bnum][chnum].compare("ff") == 0 ||
             tke.tMethod[bnum][chnum].compare("fFlt") == 0 ||
             tke.tMethod[bnum][chnum].compare("fFilter") == 0 ||
             tke.tMethod[bnum][chnum].compare("fastFilter") == 0) {
    fastFilter(tke.Ns[bnum][chnum], this->wf, this->Tflt, tke.ffRise[bnum][chnum], 1,
               tke.tau[bnum][chnum]);
  } else {
    fastFilter(tke.Ns[bnum][chnum], this->wf, this->Tflt, tke.ffRise[bnum][chnum], 1,
               tke.tau[bnum][chnum]);
  }

  Double_t zcross = 0;
  Double_t bdelta = 0;
  if (tke.thresh[bnum][chnum] >= 0) {
    bdelta = 2 * tke.thresh[bnum][chnum];
  } else {
    bdelta = locatePeak(tke.Ns[bnum][chnum], this->Tflt);
    // cout << (int)chnum  << " " << bdelta << endl;
  }
  // get time stamp
  if (tke.tMethod[bnum][chnum].compare("ff") == 0 ||
      tke.tMethod[bnum][chnum].compare("fFlt") == 0 ||
      tke.tMethod[bnum][chnum].compare("fFilter") == 0 ||
      tke.tMethod[bnum][chnum].compare("fastFilter") == 0 ||
      tke.tMethod[bnum][chnum].compare("sdlFilter") == 0 ||
      tke.tMethod[bnum][chnum].compare("sdlFlt") == 0 ||
      tke.tMethod[bnum][chnum].compare("sdl") == 0) {
        cout << "Using dumb method" << endl;
    // timestamp is time pointed to by linear extrap of time to thresh & time to half thresh
    zcross = (double)ffExtrapTs(tke.Ns[bnum][chnum], this->Tflt, tke.tOffset[bnum][chnum],
                                tke.ffRise[bnum][chnum], bdelta / 2.);
  } else {
    // cout << "Start zero crossing " << tke.interp[bnum][chnum] << " " << (int)chnum << endl;
    // locate zero crossing of timing filter
    if (tke.interp[bnum][chnum].compare("spline") == 0) {
      zcross = cSplineInterp0X(tke.Ns[bnum][chnum], this->Tflt, bdelta / 2.,
                               tke.tOffset[bnum][chnum], tke.nZero[bnum][chnum]);
    } else if (tke.interp[bnum][chnum].compare("cubic") == 0) {
      zcross = cubicInterp0X(tke.Ns[bnum][chnum], this->Tflt, bdelta / 2., tke.tOffset[bnum][chnum],
                             tke.nZero[bnum][chnum]);
    } else if (tke.interp[bnum][chnum].compare("linear") == 0) {
      zcross = linearInterp0X(tke.Ns[bnum][chnum], this->Tflt, bdelta / 2.,
                              tke.tOffset[bnum][chnum], tke.nZero[bnum][chnum]);
    } else {
      zcross = locate0X(tke.Ns[bnum][chnum], this->Tflt, bdelta / 2., tke.tOffset[bnum][chnum]);
    }
    // cout << "End zero crossing" << endl;

  }
  // set time of triggers to cross
  *tTrig = zcross;

  // cout << "time in the output is " << *tTrig << endl;
  // calc the peak height
  Double_t ph = 0;
  if (tke.eMethod[bnum][chnum].compare("chargeInt") == 0) {
    ph =
      calcPhChargeInt(tke.nPeak[bnum][chnum], ((int)zcross) + tke.eOffset[bnum][chnum],
      this->wf);
    *peak = ph;
  } else if (tke.eMethod[bnum][chnum].compare("chargeIntGrid") == 0) {
      ph    = calcPhGrid(tke.Ns[bnum][chnum], tke.gOffset[bnum][chnum], tke.nPeak[bnum][chnum], this->wf);
      *peak = ph;
  } else if (tke.eMethod[bnum][chnum].compare("expFit") == 0) {
    // cout << "number of points in waveform " << tke.Ns[bnum][chnum] << endl;
    ph    = calcPhExp(tke.Ns[bnum][chnum], tke.nPeak[bnum][chnum],
                   (int)zcross + tke.eOffset[bnum][chnum], bl, this->wf, tke.phGuess[bnum][chnum],
                   tke.tau[bnum][chnum]);
    *peak = ph;
    // cout << "baseline " << bl << "peak height " << *peak << endl;
  } else if (tke.eMethod[bnum][chnum].compare("trapFilter") == 0)
  {
    if (((int)zcross) > 0) {
      trapFilter(tke.Ns[bnum][chnum], this->wf, this->Eflt, ((int)zcross), tke.tRise[bnum][chnum],
                 tke.tGap[bnum][chnum], 1, tke.tau[bnum][chnum]);
      ph = getPhTrapFilter(tke.Ns[bnum][chnum], this->Eflt, ((int)zcross), tke.tRise[bnum][chnum],
                           tke.tPeak[bnum][chnum], tke.nPeak[bnum][chnum]);
      *peak  = ph;
      *tPeak = zcross + tke.tRise[bnum][chnum] + tke.tPeak[bnum][chnum];
    } else {
      *tPeak = -1;
      *peak  = -1;
    }
    // cout << "baseline " << bl << "peak height " << *peak << endl;
  }
  return 0;
}
