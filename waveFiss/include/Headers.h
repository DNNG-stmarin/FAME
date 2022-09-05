////////////////////////////////////////////////////////////////////////////////
///  Shea M. Mosby
///  smosby@lanl.gov
///  Devin S. Connolly
///  dconnolly@lanl.gov
///  headers.h
////////////////////////////////////////////////////////////////////////////////

#ifndef HEADERS_H
#define HEADERS_H

// C++ includes
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <zlib.h>
#include <zconf.h>
#include <math.h>
#include <string.h>

#include "TMidasBanks.h"

// If root histos are wanted
#ifdef HAVE_ROOT
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
//#include "TDirectoryFile.h"  // for root >5.16
#include "TDirectory.h" // for root >5.16
#endif

using namespace std;

#endif
