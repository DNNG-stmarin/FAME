////////////////////////////////////////////////////////////////////////////////
///  Shea M. Mosby
///  smosby@lanl.gov
///  Devin S. Connolly
///  dconnolly@lanl.gov
///  unpacker.cxx
////////////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <zlib.h>

// ROOT includes
#include "TTree.h"
#include "TFile.h"

// File includes
#include "Headers.h"
#include "Structures.h"
#include "definitions.h"

const int MaxHitsPerT0          = 10000;
bool      MidasEventPrint       = false;
int       MidasEventPrintThresh = 0;
uint32_t  STOPatEVENT           = 1e9;
// int STOPatEVENT = 1000;

int main(int argc, char* argv[])
{
  // ROOT stuffs
  TFile*     ofile;                    // output file
  TTree*     otree;                    // output tree
  DEVT_BANK* evinfo = new DEVT_BANK(); // caen event info

  int           neweventId = 0;
  int           nevt       = 0;
  EventHeader_t head;          // Midas EventHeader
  BankHeader_t  bhead;         // Midas bank header
  Bank32_t      bank32;        // Midas 32bit bank
  Bank_t        bank;          // Midas 32bit bank
  EventHeader_t endrun;        // end run buffer
  int           TotalDataSize; //=head.fDataSize;
  int           TotalBankSize; //=head.fDataSize;
  int           EventBankSize; //=head.fDataSize;
  printf("\n");
  printf(" _______________________________________________________\n");
  printf("|                                                       |\n");
  printf("|                      CAEN Unpacker                    |\n");
  printf("|-------------------------------------------------------|\n");
  printf("|         Unpacker DPP-enable CAEN digital ouput        |\n");
  printf("|                                                       |\n");
  printf("|           (send bugs to dconnolly@lanl.gov)           |\n");
  printf("|_______________________________________________________|\n");
  printf("\n");

  gzFile in;
  char   path_mid[300];
  char   opath_mid[300];
  short  waveform[WF_SIZE];
  long   devt_padding = 4; // padding for the devt struct read

  if (argc == 3) {
    sprintf(path_mid, "%s", argv[1]);
    sprintf(opath_mid, "%s", argv[2]);
    // printf("%s\n",path_mid);
  } else {
    printf("--> EXIT:\nPlease, start unpacker as follows: \n");
    printf("--> EXIT: unpacker string=PATH_TO_FILE string=PATH_TO_OUTPUT\n");
    return -1;
  }
  in = gzopen(path_mid, "rb");
  if (! in) {
    fprintf(stderr, "problem opening file %s.\n", path_mid);
    return -1;
  }
  // Open output file, set up root tree for raw waveforms
  ofile = new TFile(opath_mid, "recreate");
  otree = new TTree("t", "CAEN digital output tree");
  // make the actual branches
  otree->Branch("bnum", &evinfo->board);
  otree->Branch("chnum", &evinfo->channel);
  otree->Branch("ts", &evinfo->timestamp_low);
  char buf[16];
  sprintf(buf, "wf[%d]", WF_SIZE);
  otree->Branch(buf, waveform);

  bool run = true;
  do {
    // printf("insid run loop\n");
    gzread(in, &head, sizeof(EventHeader_t));
    TotalDataSize = head.fDataSize;
    // printf("total remaining data size %i  and sizeof EventHeader
    // %i\n",TotalDataSize,sizeof(EventHeader_t));
    if (MidasEventPrint && nevt >= MidasEventPrintThresh) {
      cout << "Event_HEADER " << endl;
      cout << hex << head.fEventId << endl;
      cout << dec << head.fTriggerMask << endl;
      cout << dec << head.fSerialNumber << endl;
      cout << dec << head.fTimeStamp << endl;
      cout << dec << head.fDataSize << endl;
    }
    if (head.fEventId == 0x8000 || head.fEventId == 0x8001 || head.fEventId == 0x8002) {
      if (head.fEventId == 0x8001) {
        endrun = head;
        break;
      }
      char* fData;
      fData = (char*)malloc(head.fDataSize);
      gzread(in, fData, head.fDataSize);
      if (MidasEventPrint && neweventId > MidasEventPrintThresh) {
        for (int i = 0; i < head.fDataSize; i++) { cout << fData[i]; }
      }
      free(fData);
    } else if (head.fEventId == 1) {
      // this is event data
      // printf("This is event data\n");
      gzread(in, &bhead, sizeof(BankHeader_t));
      if (MidasEventPrint && nevt > MidasEventPrintThresh) {
        cout << "Bank_HEADER " << endl;
        cout << dec << "TotalBankSize (bytes): " << bhead.fDataSize << endl;
        cout << dec << bhead.fFlags << endl;
      }
      TotalBankSize = bhead.fDataSize;
      // printf("Bank32 is size %i\n",sizeof(Bank32_t));
      /*
        char *fData;
        fData=(char*)malloc(bhead.fDataSize);
        fread(fData,bhead.fDataSize,1,in);
        if(MidasEventPrint && neweventId > MidasEventPrintThresh){
        for(int i=0;i<bhead.fDataSize;i++){
        cout << fData[i];
        }
        }
        free (fData);
      */
      int tempcount     = 0;
      int insidecounter = 0;
      while (TotalBankSize > 0) {
        insidecounter += 1;

        // if (insidecounter > 2) break;
        // printf("WITHIN EVENT BANKS insidecounter %i\n",insidecounter);
        gzread(in, &bank32, sizeof(Bank32_t));
        TotalBankSize -= sizeof(Bank32_t);
        // printf("total remaining bank size %i and sizeof Bank32 %i and Bank
        // %i\n",TotalBankSize,sizeof(Bank32_t),sizeof(Bank_t));
        if (MidasEventPrint && nevt > MidasEventPrintThresh) {
          cout << "BANK " << endl;
          cout << bank32.fName[0] << bank32.fName[1] << bank32.fName[2] << bank32.fName[3] << endl;
          cout << dec << bank32.fType << endl;
          cout << dec << bank32.fDataSize << endl;
        }
        EventBankSize = bank32.fDataSize;
        if (bank32.fName[0] == 'D' && bank32.fName[1] == 'E') {
          // TODO: this is a COMPLETE disaster but just happens to work. Fix it.
          // fread(evinfo,sizeof(DEVT_BANK)+4,1,in);
          gzread(in, evinfo, sizeof(DEVT_BANK));
          gzseek(in, devt_padding, SEEK_CUR);
          // printf("channel number %i\n",evinfo->channel);
          TotalBankSize -= sizeof(DEVT_BANK) + devt_padding;
          EventBankSize -= sizeof(DEVT_BANK) + devt_padding;
          // printf("total remaining bank size %i and total remaining event %i size and sizeof
          // Bank32 %i and Bank %i and sizeof devt bank
          // %i\n",TotalBankSize,EventBankSize,sizeof(Bank32_t),sizeof(Bank_t),sizeof(DEVT_BANK));

          gzread(in, &bank32, sizeof(Bank32_t));
          TotalBankSize -= sizeof(Bank32_t);
          EventBankSize = bank32.fDataSize;
          if (MidasEventPrint && nevt > MidasEventPrintThresh) {
            cout << "BANK " << endl;
            cout << bank32.fName[0] << bank32.fName[1] << bank32.fName[2] << bank32.fName[3]
                 << endl;
            cout << dec << bank32.fType << endl;
            cout << dec << bank32.fDataSize << endl;
          }
          if (bank32.fName[0] == 'W' && bank32.fName[1] == 'A') {
            for (int eye = 0; eye < WF_SIZE; ++eye) { waveform[eye] = 0; }
            gzread(in, waveform, EventBankSize);
          }
          TotalBankSize -= EventBankSize;
          // printf("total bank left now is %i\n",TotalBankSize);

          /*
            char *fData;
            int readamount = TotalBankSize-2*4096;
            fData=(char*)malloc(readamount);
            fread(fData,readamount,1,in);
            if(MidasEventPrint && neweventId > MidasEventPrintThresh){
            for(int i=0;i<head.fDataSize;i++){
            cout << fData[i];
            }
            }
            free (fData);
            TotalBankSize -= readamount;
            fread(waveform,TotalBankSize,1,in);
            TotalBankSize-=TotalBankSize+0;
            EventBankSize-=TotalBankSize+0;
          */
          otree->Fill();
          // printf("read a wave bank\n");

          // otree->Fill();
        }
      }

      // printf("BROKEN OUT OF EVENT READ LOOP\n");
    } else {
      char* fData;
      fData = (char*)malloc(head.fDataSize);
      gzread(in, fData, head.fDataSize);
      if (MidasEventPrint && neweventId > MidasEventPrintThresh) {
        for (int i = 0; i < head.fDataSize; i++) { cout << fData[i]; }
      }
      free(fData);
    }
    nevt += 1;
    if (nevt % 100 == 0) {
      printf("\rUnpacking MIDAS event %i", nevt);
      fflush(stdout);
    }
    if (nevt > STOPatEVENT) { run = false; }
  } while (run);
  cout << "\ndone!\n";
  cout << "Unpacked " << nevt << " MIDAS events\n\n";

  // Write the tree and close things out
  otree->Write();
  ofile->Close();
  return 0;
}
