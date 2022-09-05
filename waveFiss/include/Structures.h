////////////////////////////////////////////////////////////////////////////////
///  Shea M. Mosby
///  smosby@lanl.gov
///  Devin S. Connolly
///  dconnolly@lanl.gov
///  structures.h
////////////////////////////////////////////////////////////////////////////////

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>

// DEFINITIONS OF TYPES AND DATA STRUCTURES
typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

typedef struct {
  DWORD timestamp_low;
  DWORD timestamp_high;
  DWORD energy;
  DWORD nreads;
  BYTE  board;
  BYTE  channel;
} DEVT_BANK;

typedef struct {
  uint32_t nreads; // read event number
  uint8_t  bnum;   // board number
  uint8_t  chnum;  // channel number
  uint16_t e;
  uint64_t ts;        // timestamp
  int16_t  wf[1000];  // waveform
  int16_t  fwf[1000]; // waveform
} CAENDGTZ_Bank_t;

// MIDAS event
typedef struct {
  int32_t tof;              // time-of-flight as acquired from midas bank
  DWORD   ID;               // Detector ID
  WORD    ord;              // it makes a beam spill of MAX=65535 hits long
  DWORD   Ifast;            // fast integral
  DWORD   Islow;            // slow integral
  short   IslowIntegral[5]; // slow integral
  double  tof_dt;           // refined time-of-flight as acquired from midas bank
  double  TOF;              // refined time-of-flight as acquired from midas bank
  double  E;                // calib energy
  BYTE    Wave[32];         // fast component of a signal
  WORD    Valid;
  WORD    TimeCard;
  short   BkgdWidth;
  short   BkgdIntegral;
  WORD    PU; // pile-up count in one crystal
  bool    UseLowGain;
  bool    EndWaveForm;
  bool    FollowingPeak;
  WORD    IntWidth;
} Event_A;

// typedef struct {
//   std::string tMethod;   // method to use to evaluate timing
//   std::string interp;    // method to use to interpolate zero corssing
//   std::string eMethod;   // method to use to evaluate peak height
//   std::string polarity;  // polarity of input signal
//   Int_t       Ns;        // number of samples in wf
//   Int_t       bLineOS;   // offset to go into the baseline
//   Int_t       bLineNpts; // number of points to go in the baseline
//   Int_t       phOS;      // initial values for exponential fitting - peak height offset
//   Int_t       ph;        // initial values for exponential fitting - peak height
//   Int_t       ffRise;
//   Int_t       tRise;
//   Int_t       tGap;
//   Int_t       tPeak;
//   Int_t       nPeak;
//   Int_t       nZero;
//   Int_t       delay;
//   Double_t    tau; // CSP decay constant
//   Double_t    frac;
//   Double_t    thresh;
// } analysisParams;

// typedef struct {
//   // double Physics_Coincidence_Window;
//   // double MCP_Coincidence_Window;
//   // double Si_Coincidence_Window;
//   // double LaBr3_Coincidence_Window;
//   // double HPGe_Coincidence_Window;
//   // double Analysis_Window;

//   // double Energy_Threshold; // MeV
//   // Bools
//   // bool Read_Binary;
//   // bool Write_Binary;
//   // bool Read_Simulation;
//   // bool HAVE_Threshold;
//   // bool FitTimeDev;
//   // QGated
//   int RunNumber;
//   // int         Long_Gate;
//   // bool        Use_Firmware_FineTime;
//   // int         Analysis_Stage;
//   // // Unpacker variables
//   // double Buffer_Depth;
// } inputParams;

// TM Bank Structure
typedef struct {
  DWORD Seconds;
  DWORD MicroSeconds;
} TMStruct;

// EV Bank Structure
typedef struct {
  DWORD OCREgFirstDataTime;
  DWORD OCREgCrystalID;
  WORD  OCREgIntegalWidth;
  WORD  OCREgDataIntegrals1;
  WORD  OCREgDataIntegrals2;
  WORD  OCREgDataIntegrals3;
  WORD  OCREgDataIntegrals4;
  WORD  OCREgDataIntegrals5;
  WORD  OCREgBkgdWidth;
  WORD  OCREgBkgdIntegral;
  DWORD OCREgDataRaw1;
  DWORD OCREgDataRaw2;
  DWORD OCREgDataRaw3;
  DWORD OCREgDataRaw4;
  DWORD OCREgDataRaw5;
  DWORD OCREgDataRaw6;
  DWORD OCREgDataRaw7;
  DWORD OCREgDataRaw8;
} EVStruct;

// DATA for splitting the array
// - may be done nicer probably

// typedef struct {
//   DWORD N;
//   // int lower;
//   // int upper;
//   Event_A A[MaxHitsPerT0];
// } test_struct;

typedef struct {
  double SlowInt;
  double FastInt;
  double wave[32];
  double wave_cumul[32];
  int    Satur;
  int    id;
  bool   pileup;
  bool   truncated;
  double constbck;
  double expbck;
  double Aexpbck;
  double width;
  double Aslow;
  double SlowIntWidth;
  double BckIntWidth;
  double dt;
  double tail;
  double maxwave;
  int    imaxwave;
} SP;

typedef struct {
  int    id;
  char   name[30];
  int    NoOfPoints;
  double x[30];
  double y[30];
} BM_struct;

// struct mdlLoc {
// //------------------------------------------------------------------------------
//   unsigned char crate;
//   unsigned char sttn;
//   unsigned short int chns;
//   unsigned short int vsn;
// };

// typedef struct{
// // millitime MLTM
//         DWORD Scalers_MT;
//         static struct mdlLoc sclrMdl[2]={{1, 19, 12},{1, 20, 12}};

//     if ( runinfo.state == STATE_RUNNING ) {
//         bk_create( pevent, "SCLR", TID_DWORD, &pdata );
//     } else {
//         pdataOrig = (DWORD *)malloc( sizeof(DWORD) * N_SCLR );
//         pdata = pdataOrig;
//     }

//     for ( iMdl = 0; iMdl < sizeof(sclrMdl)/sizeof(struct mdlLoc); ++iMdl ) {
//         for ( iSpicket = 0; iSpicket < sclrMdl[iMdl].chns; ++iSpicket ) {
//             DWORD dat = 0;
//             cdreg( &cmd, 0, sclrMdl[iMdl].crate, sclrMdl[iMdl].sttn, iSpicket );
//             cfsa( 2, cmd, &dat, &q );
//            *pdata++ = dat;
//             if ( iMdl == 0 ) {
//                 ss_printf( 2, 25, "        Scaler 0:%6d", dat );
//             }
//         }
//     }

//     if ( runinfo.state == STATE_RUNNING ) {
//         bk_close( pevent, pdata );
//     } else {
//         free( pdataOrig );
//     }

// } Scalers;

#endif
