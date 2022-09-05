///////////////////////////////////
// DANCE frontend checking tool
// ...base code shamelessly ripped off of FARE by
// Shea Mosby 03/2012
///////////////////////////////////

// NOTE: as of the beginning of this program
// I've stripped virtually all the fancy options
// from FARE even though this program is based on
// it... I more or less followed KISS for it, and
// only included things I cared about. If you want
// more FARE-like options, by all means add them

bool printoutput = false;
bool printoutputthresh = 0;
const int 	MaxHitsPerT0= 		10000;		// ...................	If number of hits per T0 exceeds MaxHitsPerT0 the code
bool 		MidasEventPrint=	false;		// ...................	Print Midas Events info to screen
int MidasEventPrintThresh = 15895;
int 		STOPatEVENT=		1e9;		// ...................	Stop th replay at Event No ... Keep 1e9 for the replay of the whole file
double 		FE_INTERFERE=		0.02;		// ...................	if FE are misaligned we will check preceeding and proceeding event 
bool		ONLY_BM=		false;
char 		ROOTOUTPUT_DIR[200]=	"merging/";					// ................... Path to the root ouput files
char		RootString[30]=		"Target";	// ....................	Characteristic string that will be inserted to the output files

// extra includes for this work
#include <utility>
#include <vector>
#include <deque>
#include <algorithm>
// end extra includes for this work
#include "IncludeFiles/Headers.cp"
#include "IncludeFiles/Structures.cp"
#include "TFile.h"
#include "TTree.h"
#include "stdlib.h"
#define GZIP
//#define DTREE

// structure definitions relevant for this work
// evtinfo collects all the information for a given frontend
typedef struct {
    int evtindex;
    int bankindex;
    double time;                    // the time stamp for the event
    short which_fe;                 // which frontend we're looking at
    vector<Bank32_t> bankstructs;   // individual bank headers for event
    TMStruct timestruct;            // the one time structure for this event
    vector<EVStruct> eventstructs;  // the physics event structures for the event
} evtinfo;
// comparison function for this struct
bool paircomp(pair<double,evtinfo> i,pair<double,evtinfo> j)
{
    return (i.first<j.first);
}

// the storage mechanism for all event info
const int nfe = 15;
int last_fe; // the last FE looked at
deque<pair<double, evtinfo> > *fe_events[nfe];   // this thing will be huge - likely over 1 GB
                                  // (which is why we're using some pointers...)
evtinfo tempevt; // temporary event info pointer used to construct the fe_events
vector <EventHeader_t> evt_headers; // event header vector
EventHeader_t endrun; // end run buffer
vector <BankHeader_t> bank_headers; // bank header vector


// definitions for tree
Double_t frontend_time[15];
UShort_t fe_fired;
TFile *f;
TTree *diag_tree;

int RunLOOP;

bool RECONSIDER;

double FE_time;
double FE_min,FE_max;
double FE_time_avg;
double FE_TM[15];
int FE_hits;

double FE_min_PE,FE_max_PE;


int main(int argc,char *argv[])
{
    for (int i=0;i<15;++i) frontend_time[i] = -1;
// initialize the deque
for (int i=0;i<nfe;++i)
{
    fe_events[i] = new deque<pair<double, evtinfo> >();
}

EventHeader_t head;					// Midas EventHeader
BankHeader_t bhead;					// Midas bank header
Bank32_t bank32;					// Midas 32bit bank
TMStruct tm;
EVStruct ev;
Sclr_t sce;

bool CORRUPTFILE = false; // whether or not a corrupt file has been detected.

struct timeval tv;  					// real time  
double bbegin,eend,time_elapsed;			// start,stop, elapsed time

char name[340];						// name of the file to be replayed
char rootfilename[300];					// name of the root ouput file
char rmcmd[340]; // possible command needed

bool run=true;
int blankers[15];
for (int i=0;i<15;++i) blankers[i] = 0;

#ifdef GZIP
char path_gzip[300];
char opath_gzip[300];
#endif
#ifndef GZIP
char path_mid[300];
char opath_mid[300];
#endif

int 	RunFirst[30];
int 	RunLast[30];

printf("\n");
printf(" _______________________________________________________\n");
printf("|		     					|\n");
printf("|			   FeFix	   		|\n");
printf("|-------------------------------------------------------|\n");
printf("|		repair misaligned frontends		|\n");
printf("|		     					|\n");
printf("|	       (send bugs to smosby@lanl.gov)	 	|\n");
printf("|_______________________________________________________|\n");
printf("\n");

gettimeofday(&tv,NULL); 
bbegin=tv.tv_sec+(tv.tv_usec/1000000.0);

#ifdef GZIP
	gzFile gz_in;
    gzFile gz_out_good;
    gzFile gz_out_aux;
#else
	FILE *in;
    FILE *out_good;
    FILE *out_aux;
#endif
FILE *out_log; // log file output

int TotalDataSize; //=head.fDataSize;
char St_RunFirst[10];
char St_RunLast[10];
// LOOP Through files
char inputfile[300];
//bool HAVE_InputFile=false;

/*
if(argc>=2){
	string switchstr;
	
	switchstr=argv[1];
	if(switchstr=="-i") HAVE_InputFile=true;
	cout << switchstr << endl;

}
*/

//if(!HAVE_InputFile && argc == 5){
if(argc == 5){

//gzip=argv[1];
	#ifdef GZIP
		sprintf(path_gzip,"%s",argv[1]);
		sprintf(opath_gzip,"%s",argv[2]);
		//printf("%s\n",path_gzip);
	#endif
	
	#ifndef GZIP
		sprintf(path_mid,"%s",argv[1]);
		sprintf(opath_mid,"%s",argv[2]);
		//printf("%s\n",path_mid);
	#endif
	
	sprintf(St_RunFirst,"%s",argv[3]);
	RunFirst[0]=atoi(St_RunFirst);
			
	sprintf(St_RunLast,"%s",argv[4]);
	RunLast[0]=atoi(St_RunLast);
	

}

else {
	printf("--> EXIT:\nPlease, start FeFix as follows: \n");
	printf("--> EXIT: FeFix string=PATH_TO_FILE string=PATH_TO_OUTPUT int=Run#1 int=Run#2\n");
	return -1;
}

int T0=0;
int T0_cumul=0;

for(RunLOOP=RunFirst[0];RunLOOP<=RunLast[0];RunLOOP++){
T0=0;
FE_hits=0;

int oldeventId=0;
int neweventId=0;;

int SIZE=0;

#ifdef GZIP
	sprintf(name,"%s/run%05d.mid.gz",path_gzip,RunLOOP);
	gz_in=gzopen(name,"rb");
	if(!gz_in) {
		printf("--> EXIT:File %s not found exiting\n",name);
		return -1;
	}
    // "good, repaired" output
	sprintf(name,"%s/run%05d_repaired.mid.gz",opath_gzip,RunLOOP);
	gz_out_good=gzopen(name,"wb");
    // aux output
	sprintf(name,"%s/run%05d_aux.mid.gz",opath_gzip,RunLOOP);
	gz_out_aux=gzopen(name,"wb");
    if (gz_out_good == NULL || gz_out_aux == NULL)
    {
        cout << "output files failed to open .. exit. check the path you gave me\n";
        return 1;
    }
#else
	sprintf(name,"%s/run%05d.mid",path_mid,RunLOOP);
	in=fopen(name,"rb");
	if(!in) {
		printf("--> EXIT:File %s not found exiting\n",name);	
		return -1;
	}
    // "good, repaired" output
	sprintf(name,"%s/run%05d_repaired.mid",opath_mid,RunLOOP);
	out_good=fopen(name,"wb");
    // aux output
	sprintf(name,"%s/run%05d_aux.mid",opath_mid,RunLOOP);
	out_aux=fopen(name,"wb");
    if (out_good == NULL || out_aux == NULL)
    {
        cout << "output files failed to open .. exit. check the path you gave me\n";
        return 1;
    }
#endif
    #ifdef DTREE
        // ttree/tfile crap
        sprintf(rootfilename,"%s/%s_Histos%d.root",ROOTOUTPUT_DIR,RootString,RunLOOP);
        printf("--> Root histograms for each run will be stored in file: %s \n", rootfilename);
        cout << "MAKING NEW TFILE\n";
        f = new TFile(rootfilename,"RECREATE");
        f->cd();
        diag_tree = new TTree("t","diagnostic tree");
        diag_tree->Branch("time[15]",frontend_time);              // frontend times
        diag_tree->Branch("fef",&fe_fired);              // frontend times
    #endif
cout << "Processing file: " << name << endl;

// Zero out the event deque before each run
for (int i=0;i<nfe;++i)
{
    fe_events[i]->clear();
}
// also zero out the event and buffer headers
evt_headers.clear();
bank_headers.clear();

//----------------------------------------------------------------
//----------------------------------------------------------------
//
// 		STart reading the midas file here
//
//----------------------------------------------------------------
//----------------------------------------------------------------

endrun.fEventId = 0;
do{
	
//-----------------------------------------------------------------------------------------
// 				Read event header
//-----------------------------------------------------------------------------------------
		
	#ifdef GZIP	
		gzread(gz_in, &head,sizeof(EventHeader_t));
		//if(gzeof(gz_in)==1) run=false;
	#else
		fread(&head,sizeof(EventHeader_t),1,in);
		//if(feof(in)) run=false;
	#endif
    evt_headers.push_back(head); // event header added
	
	if(!run) break;
	
	TotalDataSize=head.fDataSize;
	if(MidasEventPrint && neweventId > MidasEventPrintThresh){
		cout << "Event_HEADER " << endl;
		cout << hex << head.fEventId << endl;
		cout << dec << head.fTriggerMask << endl;
		cout << dec << head.fSerialNumber << endl;
		cout << dec << head.fTimeStamp << endl;
		cout << dec << head.fDataSize << endl;
	}	
	
	
//-----------------------------------------------------------------------------------------
// 			CASE when fEvent ID=0x8000 || 0x8000 || 0x8000
//-----------------------------------------------------------------------------------------
	

	if(head.fEventId==0x8000 || head.fEventId==0x8001 || head.fEventId==0x8002 ){
		if(head.fEventId==0x8001)
        {
            endrun = head;
            break;
        }
		char *fData;
		fData=(char*)malloc(head.fDataSize);
	
		#ifdef GZIP
			gzread(gz_in, fData,head.fDataSize);

            // write things back out
            gzwrite(gz_out_good,&head,sizeof(EventHeader_t));
			gzwrite(gz_out_good, fData,head.fDataSize);

            gzwrite(gz_out_aux,&head,sizeof(EventHeader_t));
			gzwrite(gz_out_aux, fData,head.fDataSize);
		#endif
		
		#ifndef GZIP 
			fread(fData,head.fDataSize,1,in);

            fwrite(out_good,&head,sizeof(EventHeader_t));
			fwrite(out_good, fData,head.fDataSize);

            fwrite(out_aux,&head,sizeof(EventHeader_t));
			fwrite(out_aux, fData,head.fDataSize);
		#endif	
	
		if(MidasEventPrint && neweventId > MidasEventPrintThresh){
			for(int i=0;i<head.fDataSize;i++){
				cout << fData[i];			
			}
		}	
		free (fData);	
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------//
// 				CASE when fEvent ID=1
//-----------------------------------------------------------------------------------------------------------------------------------------------//

	else if(head.fEventId==1){
		
		//-------------------------------------------------------------
		// ---- First Check if we have the new event ---- ///	
		//-------------------------------------------------------------
		int Nevent=head.fSerialNumber;	
		if(Nevent>STOPatEVENT) run=false;
		/*if(Nevent==37900) {
			EventPrint=true;
			PrintPass=true;
		}
		*/
		
		neweventId=head.fSerialNumber;

		//-------------------------------------------------------------------------------------------------------------------------------//
		//					E V E R Y T H I N G   H A P P E N S   H E R E 						 //	
		//-------------------------------------------------------------------------------------------------------------------------------//
		//
		// Here we have recognized a new event ID 
		// based on a new serial number.
		// Before we start reading let's do the analysis
		//
		// We need to analyze the event:
		//	1) Wait if the analysis thread is still running and join it
		//	2) RECONSIDER the previous event in the memory (FE deviations)
		//		2.1) If yes, start the previous event (PE) processing - (may be done in parallel ?, but now it is
		//			waiting -sequential)
		//	3) Analyze the current event (CE) in a separate thread (the code starts file reading in parallel not to
		//		waste time)
		 
		if(neweventId!=oldeventId) {  		
			
			T0++;
			T0_cumul++;
						
			oldeventId=neweventId;
			//cout << dec << head.fSerialNumber << "	" << SIZE << endl;

            // commented by SMM for performance testing
			printf("Midas Event #: %d	Detectors Fired: %d\r", head.fSerialNumber,SIZE);
			
			// Reconsider the event before if Frontend deviations are acceptable
			
			if(RECONSIDER){
			
				// We got here because FE_max-FE_min > FE_interfere 
				// Now if it is an isolated event let's put it back
				// We still have it in RawEvent structure
				
				// This is a sequential call and will slow down the analysis
				// It may be that it can be done in parallel
				
				// if((FE_min-FE_max_PE)<FE_INTERFERE) ProcessEvents(&RawEvent);

				// For now just reprocess the data from previous event
				// to test for the speed
				// If we need to move some banks from current event to the previous one
				// we do it right here
				
				//ProcessEvents(&RawEvent);
			
			};
			
			// Prepare variables for a new Midas Event
				
			SIZE=0;	
            fe_fired = FE_hits;
			FE_hits=0;
			FE_time_avg=0.;
		
			FE_min_PE=FE_min;	// save minimum and maximum times from the FE
			FE_max_PE=FE_max; 
		
			FE_min=1e35;
			FE_max=0.;
			
			
		
		}

		//--------------------------------------------------------------------------------------------------------------------------------
		// 						NOW - Read in the bank header
		//--------------------------------------------------------------------------------------------------------------------------------
	
		#ifdef GZIP
			gzread(gz_in,&bhead,sizeof(BankHeader_t));
		#else
	 		fread(&bhead,sizeof(BankHeader_t),1,in);	
		#endif
        // add the bank header to our running list of crap
        bank_headers.push_back(bhead);
		
	
		int TotalBanksSize=bhead.fDataSize;
		TotalDataSize-=sizeof(BankHeader_t);

		if(MidasEventPrint && neweventId > MidasEventPrintThresh){
			cout << "Bank_HEADER " << endl;
			cout << dec <<"TotalBankSize (bytes): " << bhead.fDataSize << endl;
			cout << dec << bhead.fFlags << endl;
		}
		
		//--------------------------------------------------------------------------------------------------------------------------------
		// 						Read the individual banks
		//--------------------------------------------------------------------------------------------------------------------------------
	
        // before the individual banks start we initialize the event info
        tempevt = evtinfo();
        tempevt.time = -1;
        tempevt.which_fe = -1;
        tempevt.bankstructs.clear();
        tempevt.eventstructs.clear();
        tempevt.evtindex = evt_headers.size()-1;
        tempevt.bankindex = bank_headers.size()-1;
        last_fe = 0;
		while(TotalDataSize>0){
            if (MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "event loop start\n";
            //cout << "event loop start, size is " << TotalDataSize << endl;
	
			#ifdef GZIP
				gzread(gz_in,&bank32,sizeof(Bank32_t));
			#else
				fread(&bank32,sizeof(Bank32_t),1,in);
			#endif
            // for each event we've got a header
            //tempevt.bankstructs.push_back(bank32);
	
			TotalDataSize-=sizeof(Bank32_t);
			TotalBanksSize-=sizeof(Bank32_t);
	
			if(MidasEventPrint && neweventId > MidasEventPrintThresh){
				cout << "BANK " << endl;
				cout << bank32.fName[0] << bank32.fName[1] << bank32.fName[2]<< bank32.fName[3] << endl;
				cout << dec << bank32.fType << endl;
				cout << dec << bank32.fDataSize << endl;
			}
		
			if(bank32.fName[0]=='T'){
                if (tempevt.bankstructs.size() == 0)
                {
                    // this takes care of the 1st event read
                    tempevt.bankstructs.push_back(bank32);
                }
                //cout << "\n\n\nnew event\n";
                if (tempevt.time!=-1) // we check on time because it's the one thing we MUST have
                {
                    if (tempevt.which_fe == -1)
                    {
                        // caught event where there was a time but no event data
                        //cout << "woot woot here's a stupid event with no actual data in FE " << last_fe+1 << endl;;
                        tempevt.which_fe = last_fe + 1;
                        last_fe = last_fe + 1;
                        // TODO: think about whether this screws things up in different ways
                    }
                    // add the event information to the deque
                    //cout << "add event info\n";
                    //cout << tempevt.which_fe << endl;
                    /*
                    if (tempevt.bankstructs.size() == 3)
                    {
                        cout << "size 3\n";
                        tempevt.bankstructs.pop_back();
                    }
                    */
                    fe_events[tempevt.which_fe-1]->push_back(make_pair(tempevt.time,tempevt));
                    //cout << "added event info\n";

                    //cout << tempevt.which_fe << " " << tempevt.bankstructs.size() << " " << tempevt.eventstructs.size() << endl;
                    // new section of event banks ... make new temporary event info
                    tempevt = evtinfo();
                    tempevt.time = -1;
                    tempevt.which_fe = -1;
                    tempevt.bankstructs.clear();
                    tempevt.eventstructs.clear();
                    tempevt.evtindex = evt_headers.size()-1;
                    tempevt.bankindex = bank_headers.size()-1;
                    tempevt.bankstructs.push_back(bank32);
                    //cout << "evtindex " << tempevt.evtindex << " bankindex " << tempevt.bankindex << endl;
                }
                //cout << "new temp event into\n";
	
				if(MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "Reading TM_Bank" << endl;
				
				#ifdef GZIP
		 			gzread(gz_in,&tm,sizeof(TMStruct));
				#else 
					fread(&tm,sizeof(TMStruct),1,in);
				#endif
		
				if(MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "tm.Seconds: 	" << tm.Seconds << endl;
				if(MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "tm.MicroSeconds: 	" << tm.MicroSeconds << endl;			
		
				TotalDataSize-=sizeof(tm);
				TotalBanksSize-=sizeof(tm);
				
				//-------------------------------------------------------------
				// DO appropriate stuff with frontend times
				//-------------------------------------------------------------
				
				FE_time=double(1.*tm.Seconds+1.*tm.MicroSeconds/1e6);
				
				FE_time_avg+=FE_time;
				FE_TM[FE_hits]=FE_time;
				FE_hits++;
                //cout << "FE time " << tm.MicroSeconds << endl;
		
				if(FE_time<FE_min) FE_min=FE_time;
				if(FE_time>FE_max) FE_max=FE_time;

                // put info in the tree
                frontend_time[FE_hits-1] = FE_time;
                
                // fill our temporary event with the relevant times
                tempevt.timestruct = tm;
                tempevt.time = FE_time;
                //cout << "completed time stuff\n";

                //cout << "end of time processing, size is " << TotalDataSize << endl << endl;
			}
			
			if(bank32.fName[0]=='E'){
                //cout << "in event buffer\n";
                tempevt.bankstructs.push_back(bank32);
				
				if(MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "Reading EV_Bank" << endl;
                if(MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "size of event " << bank32.fDataSize << " " << sizeof(EVStruct) << " " << (bank32.fDataSize)/sizeof(EVStruct) << endl;
		
                //cout << "event size " << (bank32.fDataSize)/sizeof(EVStruct) << endl;
				for(int i=0;i<(bank32.fDataSize)/sizeof(EVStruct);i++){			
                    if(MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "size of event " << bank32.fDataSize << " " << sizeof(EVStruct) << " " << (bank32.fDataSize)/sizeof(EVStruct) << endl;
                    //cout << "in event loop\n";
					
					ev.OCREgFirstDataTime=0;
					
					#ifdef GZIP
						gzread(gz_in,&ev,sizeof(ev));
					#else 
						fread(&ev,sizeof(ev),1,in);
					#endif
					if(MidasEventPrint && neweventId > MidasEventPrintThresh) {				
						cout << "OCREgFirstDataTime: " << ev.OCREgFirstDataTime << endl;				
						cout << "OCREgCrystalID: " << ev.OCREgCrystalID<< endl;				
						cout << "OCREgIntegalWidth: " << ev.OCREgIntegalWidth<< endl;
						cout << "OCREgDataIntegrals1: " << ev.OCREgDataIntegrals1<< endl;	
						cout << "OCREgDataIntegrals2: " << ev.OCREgDataIntegrals2<< endl	;
						cout << "OCREgDataIntegrals3: " << ev.OCREgDataIntegrals3<< endl;
						cout << "OCREgDataIntegrals4: " << ev.OCREgDataIntegrals4<< endl;
						cout << "OCREgDataIntegrals5: " << ev.OCREgDataIntegrals5<< endl;
						cout << "OCREgBkgdWidth: " << ev.OCREgBkgdWidth<< endl;
		    				cout << "OCREgBkgdIntegral: " << ev.OCREgBkgdIntegral<< endl;
						cout << "OCREgDataRaw1: " << ev.OCREgDataRaw1<< endl;
						cout << "OCREgDataRaw2: " << ev.OCREgDataRaw2<< endl;			
						cout << "OCREgDataRaw3: " << ev.OCREgDataRaw3<< endl;
						cout << "OCREgDataRaw4: " << ev.OCREgDataRaw4<< endl;
						cout << "OCREgDataRaw5: " << ev.OCREgDataRaw5<< endl;
						cout << "OCREgDataRaw6: " << ev.OCREgDataRaw6<< endl;
						cout << "OCREgDataRaw7: " << ev.OCREgDataRaw7<< endl;
						cout << "OCREgDataRaw8: " << ev.OCREgDataRaw8<< endl;
						
					}    								
					
					TotalDataSize-=sizeof(ev);
					TotalBanksSize-=sizeof(ev);			
					
					short frontend = static_cast< short >( ev.OCREgCrystalID >> 16 );
    					short unit = static_cast< short >( ( ev.OCREgCrystalID >> 8 ) & 0x000000FF );
		    			short chnl = static_cast< short >( ev.OCREgCrystalID & 0x000000FF );
                    //cout << frontend << endl;
					
					
					if(MidasEventPrint && neweventId > MidasEventPrintThresh) {				
						cout << "Frontend: 	" << frontend << endl;
						cout << "Unit: 		" << unit << endl;
						cout << "Channel: 	" << chnl << endl;
					}

                    // fill the event information bits
                    tempevt.eventstructs.push_back(ev);
                    tempevt.which_fe = frontend;
                    last_fe = frontend;
				}
                if (MidasEventPrint && neweventId > MidasEventPrintThresh)
                {
                    cout << "end event bank loop\n";
                }
                //cout << "end of event  processing FE " << tempevt.which_fe <<  ", size remaining " << TotalDataSize << endl;
                //cout << "event length in units is " << tempevt.eventstructs.size() << " byte size " << sizeof(EVStruct) << " " << sizeof(EVStruct)*tempevt.eventstructs.size()+sizeof(Bank32_t) << endl;
			}	
            if (MidasEventPrint && neweventId > MidasEventPrintThresh)
            {
                cout << "end event bank\n";
            }
		}
        // make sure we cleared out the last event
        if (tempevt.time!=-1)
        {
            if (MidasEventPrint && neweventId > MidasEventPrintThresh)
            {
                cout << "processing last event\n";
            }
            if (tempevt.which_fe == -1)
            {
                // caught event where there was a time but no event data
                //cout << "woot woot here's a stupid event with no actual data in FE " << last_fe+1 << endl;;
                tempevt.which_fe = last_fe + 1;
                last_fe = last_fe + 1;
                // TODO: think about whether this screws things up in different ways
            }
            fe_events[tempevt.which_fe-1]->push_back(make_pair(tempevt.time,tempevt));
            //cout << tempevt.which_fe << " " << tempevt.bankstructs.size() << " " << tempevt.eventstructs.size() << endl;
            /*
            // I'm afraid this will lead to memory leaks
            tempevt = new evtinfo();
            tempevt.time = -1;
            tempevt.which_fe = -1;
            tempevt.bankstructs.clear();
            tempevt.eventstructs.clear();
            */
            if (MidasEventPrint && neweventId > MidasEventPrintThresh)
            {
                cout << "processed last event\n";
            }
        }
        //cout << "\n\n\n\n\n";
        /*
        for (int i=0;i<nfe;++i)
        {
            cout << "length of " << i << "th vector " << fe_events[i]->size() << endl;
        }
        */
        // fill the tree
        #ifdef DTREE
            diag_tree->Fill();
        #endif
        for (int i=0;i<15;++i) 
        {
            /*
            if (frontend_time[i] == -1)
            {
                blankers[i] += 1;
            }
            */
            if (fabs(frontend_time[i]-frontend_time[0]) > 0.02)
            {
                //cout << "frontend " << i << " off by " << (frontend_time[i]-frontend_time[0]) << endl;
                blankers[i] += 1;
                CORRUPTFILE = true;
            }
            else if (i == 0) blankers[0] += 1;

        }
        for (int i=0;i<15;++i) frontend_time[i] = -1;
	}
	
	// endof eventID=1

	else if(head.fEventId==3){
	
		// LOOKS like HV event 
		#ifdef GZIP
			char *gData;
			gData=(char*)malloc(head.fDataSize);			
			gzread(gz_in,gData,head.fDataSize);			
            // write out head and data
            gzwrite(gz_out_good,&head,sizeof(EventHeader_t));
			gzwrite(gz_out_good,gData,head.fDataSize);			

            gzwrite(gz_out_aux,&head,sizeof(EventHeader_t));
			gzwrite(gz_out_aux,gData,head.fDataSize);			

			free(gData);			
		#else	
			Id3event *fData;
			fData=(Id3event*)malloc(head.fDataSize);
			fread(fData,sizeof(Id3event),head.fDataSize/sizeof(Id3event),in);
		#endif	
	}
	
	else if(head.fEventId==5){
		
		// need to learn what is this event about
		// it has size of 124		
		
		char *fData;
		fData=(char*)malloc(head.fDataSize);
	
	 	#ifdef GZIP
			gzread(gz_in,fData,head.fDataSize);
            // write out the head and data
            gzwrite(gz_out_good,&head,sizeof(EventHeader_t));
			gzwrite(gz_out_good,fData,head.fDataSize);

            gzwrite(gz_out_aux,&head,sizeof(EventHeader_t));
			gzwrite(gz_out_aux,fData,head.fDataSize);
		#else
			fread(fData,head.fDataSize,1,in);
            // write out the head and data
            fwrite(out_good,&head,sizeof(EventHeader_t));
			fwrite(out_good,fData,head.fDataSize);

            fwrite(out_aux,&head,sizeof(EventHeader_t));
			fwrite(out_aux,fData,head.fDataSize);
		#endif
	
		free (fData);	

	}
	
	else{
	
		#ifdef GZIP 
			gzread(gz_in,&bhead,sizeof(BankHeader_t));
		#else 
			fread(&bhead,sizeof(BankHeader_t),1,in);
		#endif
		
	
		TotalDataSize-=sizeof(BankHeader_t);
		if(MidasEventPrint && neweventId > MidasEventPrintThresh) {
			cout << endl;
			cout << "Bank_HEADER " << endl;
			cout << dec << bhead.fDataSize << endl;
			cout << dec << bhead.fFlags << endl;
		}
		
		int TotalBanksSize=bhead.fDataSize;


		#ifdef GZIP 
			gzread(gz_in,&bank32,sizeof(Bank32_t));
		#else 
			fread(&bank32,sizeof(Bank32_t),1,in);
		#endif
		
		TotalDataSize-=sizeof(Bank32_t);
	
		if(MidasEventPrint && neweventId > MidasEventPrintThresh) {
			cout << "BANK " << endl;
			cout << bank32.fName << endl;
			cout << dec << bank32.fType << endl;
			cout << dec << bank32.fDataSize << endl;
		}
	
		if(bank32.fType==6){
            // write the event and buffer heads straight back out
            #ifdef GZIP
                gzwrite(gz_out_good,&head,sizeof(EventHeader_t));
                gzwrite(gz_out_good,&bhead,sizeof(BankHeader_t));
                gzwrite(gz_out_good,&bank32,sizeof(Bank32_t));

                gzwrite(gz_out_aux,&head,sizeof(EventHeader_t));
                gzwrite(gz_out_aux,&bhead,sizeof(BankHeader_t));
                gzwrite(gz_out_aux,&bank32,sizeof(Bank32_t));
            #else
                fwrite(out_good,&head,sizeof(EventHeader_t));
                fwrite(out_good,&bhead,sizeof(BankHeader_t));
                fwrite(out_good,&bank32,sizeof(Bank32_t));

                fwrite(out_aux,&head,sizeof(EventHeader_t));
                fwrite(out_aux,&bhead,sizeof(BankHeader_t));
                fwrite(out_aux,&bank32,sizeof(Bank32_t));
            #endif
		
			DWORD *fData;
		
			fData=(DWORD*) malloc(bank32.fDataSize);
		
			#ifdef GZIP 
				for(int i=0;i<bank32.fDataSize/sizeof(DWORD);i++) {
					gzread(gz_in,fData,sizeof(DWORD));
					gzwrite(gz_out_good,fData,sizeof(DWORD));
					gzwrite(gz_out_aux,fData,sizeof(DWORD));
					(*fData)+=sizeof(DWORD);
				}
			#else 
				fread(fData,sizeof(DWORD),bank32.fDataSize/sizeof(DWORD),in);
                fwrite(out_good,fData,bank32.fDataSize);
                fwrite(out_aux,fData,bank32.fDataSize);
			#endif
		
			TotalDataSize-=(bank32.fDataSize);
			
			if(TotalDataSize==112){
				fData=(DWORD*) malloc(sizeof(Sclr_t));
			
				#ifdef GZIP 
					gzread(gz_in,&sce,sizeof(Sclr_t));
					gzwrite(gz_out_good,&sce,sizeof(Sclr_t));
					gzwrite(gz_out_aux,&sce,sizeof(Sclr_t));
				#else 
					fread(&sce,sizeof(Sclr_t),1,in);
					fwrite(out_good,&sce,sizeof(Sclr_t));
					fwrite(out_aux,&sce,sizeof(Sclr_t));
				#endif
				
				TotalDataSize-=(sizeof(Sclr_t));
					
			}
						
			free (fData);
			
			
			
			
		
			if(MidasEventPrint && neweventId > MidasEventPrintThresh)cout << "TotalDataSize after this bank is: " << TotalDataSize << endl;
		
			if(TotalDataSize>0){
				if(MidasEventPrint && neweventId > MidasEventPrintThresh) cout << "Read remaining " << TotalDataSize << " bytes ! " << endl;
				
				
				char *fData;
				fData=(char*)malloc(TotalDataSize);			
				//#ifdef GZip 
				#ifdef GZIP 
					gzread(gz_in,fData,TotalDataSize);
                    gzwrite(gz_out_good,fData,TotalDataSize);
                    gzwrite(gz_out_aux,fData,TotalDataSize);
				#endif
				
				#ifndef GZIP
					fread(fData,TotalDataSize,1,in);
                    fwrite(out_good,fData,TotalDataSize);
                    fwrite(out_aux,fData,TotalDataSize);
				#endif
			
				free (fData);
				TotalDataSize=0;
				
			}		
		}
		else {
			cout << "UInknown stuff, fType = " << bank32.fType << endl;	
		}

	}	


} while (run);
#ifdef DTREE
    diag_tree->Write();
    f->Close();
#endif
for (int i=0;i<15;++i)
{
    cout << "blank " << i << " " << blankers[i] << endl;
}
// open up the log file
ofstream outstats;
char log_name[300];
#ifdef GZIP
    sprintf(log_name,"%srun%05d_log.txt",opath_gzip,RunLOOP);
#else
    sprintf(log_name,"%srun%05d_log.txt",oath_mid,RunLOOP);
#endif
outstats.open(log_name);
if (!outstats.is_open())
{
    cout << "Failed to open logfile ... exit\n";
    return 1;
}
// open the output file for bad event stats
// Check for the existence of corrupt file
if (CORRUPTFILE)
{
    cout << "I found a corrupt file - will attempt to fix\n";
    outstats << "CORRUPT\n";
    outstats << "# number of misaligned events (1st column is total number)\n";
    outstats << "# (also, FE1 is used the reference, so 2nd column is FE2\n";
    for (int i=0;i<14;++i)
    {
        outstats << blankers[i] << " ";
    }
    outstats << blankers[14] << endl;
    outstats << "# what frontends actually matched for a given corrupt event\n";
} else
{
    cout << "This file seems fine - moving on\n";
    outstats << "NOTCORRUPT\n";
    outstats.close();
    // put the "end run" buffer 
    if (endrun.fEventId ==0)
    {
        cout << "weird, i didn't end on an end run buffer..?\n";
        endrun.fEventId = 0x8001;
    }
    #ifdef GZIP
        gzwrite(gz_out_aux,&endrun,sizeof(EventHeader_t));
        gzclose(gz_in);
        gzclose(gz_out_good);
        gzclose(gz_out_aux);
    #endif
    #ifndef GZIP
        fwrite(out_aux,&endrun,sizeof(EventHeader_t));
        fclose(in);
        fclose(out_good);
        fclose(out_aux);
    #endif
    // Right here we should put the removal of files I have to 
    // start building ahead of time because I don't know what's up.
    
    #ifdef GZIP
        sprintf(rmcmd,"rm -f %s/run%05d_repaired.mid.gz",opath_gzip,RunLOOP);
    #else
        sprintf(rmcmd,"rm -f %s/run%05d_repaired.mid",opath_gzip,RunLOOP);
    #endif
    system(rmcmd);
    continue; // should move on to the next runloop file
}
/*
ofstream out;
out.open("screwups.txt",ios::app);

out << RunLOOP << " ";
for (int i=0;i<15;++i) out << blankers[i] << " ";
out << "\n";
out.close();
*/

/////////////////////////////////////
// REPAIR THE FRONTEND TIMING HERE //
/////////////////////////////////////

// 1st, time sort everything
for (int i=0;i<nfe;++i)
{
    sort(fe_events[i]->begin(),fe_events[i]->end(),paircomp);
}
// now begin the reconstruction process
bool reconstruct = false;
// Check that reconstruct should be true
for (int i=0;i<nfe;++i)
{
    if (fe_events[i]->size()>0)
    {
        reconstruct = true; // as long as at least one of the event vectors is not empty we're happy
        break;
    }
}
int counter = 0;
while (reconstruct)
{
    // 1st, identify the frontend with the earliest time
    // NOTE: in the event of multiple with the same time,
    // the 1st one I run into I'll take as good
    int ref_index = -1;
    //double ref_time = fe_events[0]->at(0).first;
    double ref_time = 0;
    for (int i=0;i<nfe;++i)
    {
        // first we establish the 1st reference
        // we must be able to handle some of the vectors being empty
        if (fe_events[i]->size()>0)
        {
            ref_index = i;
            ref_time = fe_events[i]->at(0).first;
        }
    }
    //cout << "ref time " << ref_time << " index " << ref_index << endl;
    for (int i=0;i<nfe;++i)
    {
        if (fe_events[i]->size()>0 && fe_events[i]->at(0).first < ref_time)
        {
            ref_time = fe_events[i]->at(0).first;
            ref_index = i;
        }
    }
    // Now loop through the elements other than the reference
    // and determine where (if at all) they line up
    vector<pair<int,int> > locations; // FE number, which deque entry pair
    locations.push_back(make_pair(ref_index,0));
    // loop over all frontends
    for (int i=0;i<nfe;++i)
    {
        // do NOT loop over the reference .. already done
        if (i==ref_index)
        {
            continue;
        } else
        {
            // loop until hitting end or finding a match
            for (int j=0;j<fe_events[i]->size();++j)
            {
                if (fabs(fe_events[i]->at(j).first - ref_time) < 0.02)
                {
                    locations.push_back(make_pair(i,j));
                    break;
                }
            }
        }
    }
    // Now construct your output event and decide whether it's good or not
    // 2 cases - found nfe aligned FEs or didn't
    if (locations.size()==nfe) // here's where you want to put the stuff for when the event is GOOD
    {
        #include "IncludeFiles/GoodEventWrite.cp"
    } else // here's where you want to put the stuff for when the event is BAD
    {
        #include "IncludeFiles/BadEventWrite.cp"
    }

    // now check the lengths of all vector lengths - as soon as one goes to 0 we stop
    // TODO: make it so all the leftover events are still written - just to the secondary aux file
    int nempty = 0;
    for (int i=0;i<nfe;++i)
    {
        //cout << "FE " << fe_events[i]->size() << " size\n";
        if (fe_events[i]->size() == 0)
        {
            cout << "FE " << i+1 << " empty\n";
            //reconstruct = false;
            nempty = nempty + 1;
        }
    }
    if (nempty == nfe) reconstruct = false; // end when we run all the vectors empty
}
outstats.close();
// put the "end run" buffer 
if (endrun.fEventId ==0)
{
    cout << "weird, i didn't end on an end run buffer..?\n";
    endrun.fEventId = 0x8001;
}
#ifdef GZIP
    gzwrite(gz_out_good,&endrun,sizeof(EventHeader_t));
    gzwrite(gz_out_aux,&endrun,sizeof(EventHeader_t));
#else
    fwrite(out_good,&endrun,sizeof(EventHeader_t));
    fwrite(out_aux,&endrun,sizeof(EventHeader_t));
#endif
cout << "I processed " << counter << " events\n";
	
/////////////////////////////////////////
// END REPAIR THE FRONTEND TIMING HERE //
/////////////////////////////////////////

gettimeofday(&tv,NULL);  
eend=tv.tv_sec+(tv.tv_usec/1000000.0);

time_elapsed = (double)(eend-bbegin); ;
printf("\nTime elapsed:		%7.2lf\n", time_elapsed);

#ifdef GZIP
gzclose(gz_in);
gzclose(gz_out_good);
gzclose(gz_out_aux);
#endif
#ifndef GZIP
fclose(in);
fclose(out_good);
fclose(out_aux);
#endif

}
return 0;
}
