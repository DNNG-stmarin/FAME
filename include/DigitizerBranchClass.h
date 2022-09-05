/*
Author: Stefano Marin, Isabel Hernandez
Purpose: Digitizer branch classes, used to read the branches of the digitizer, given the initial form of the data
Date: May 6th, 2020
*/

#ifndef COMPASS_DIG_h
#define COMPASS_DIG_h


// standard compass digitizer class
class COMPASS_DIG
{
public:
	// Declaration of leaf types
	UShort_t        Board;
	UShort_t        Channel;
	ULong64_t       Timestamp;
	UShort_t        Energy;
	UShort_t        EnergyShort;
	UInt_t          Flags;

	// List of branches
	TBranch        *b_Board;   //!
	TBranch        *b_Channel;   //!
	TBranch        *b_Timestamp;   //!
	TBranch        *b_Energy;   //!
	TBranch        *b_EnergyShort;   //!
	TBranch        *b_Flags;   //!

	// output functions
	int getDetector()
	{
		int detNum = (int)(Board*16 + Channel);
		return detNum;
	}

	// output functions
	long double getTime()
	{
		double detTime = ((double)Timestamp)/1000.0;
		return detTime;
	}

	// output functions
	double getEnergy()
	{
		double detErg = ((double)Energy);
		return detErg;
	}

	// output functions
	double getTail()
	{
		double detTail = ((double)Energy - (double)EnergyShort);
		return detTail;
	}
};

#endif

// midas converted digitizer class
#ifndef MIDAS_DIG_h
#define MIDAS_DIG_h

class MIDAS_DIG
{
public:
   // Declaration of leaf types
   Float_t         interpolation;
   ULong64_t       time;
   Int_t           detector;
   UShort_t        baseline;
   UShort_t        total;
   UShort_t        head;
   UShort_t        tail;

   // List of branches
   TBranch        *b_interpolation;   //!
   TBranch        *b_time;   //!
   TBranch        *b_detector;   //!
   TBranch        *b_baseline;   //!
   TBranch        *b_total;   //!
   TBranch        *b_head;   //!
   TBranch        *b_tail;   //!


   	// output functions
	int getDetector()
	{
		int detNum = (int)detector;
		return detNum;
	}

	// output functions
	long double getTime()
	{
		double detTime = (double)time*2.0 - (double)interpolation*2.0;
		return detTime;
	}

	// output functions
	double getEnergy()
	{
		double detErg = ((double)tail);
		return detErg;
	}

	// output functions
	double getTail()
	{
		double detTail = ((double)head);
		return detTail;
	}

};
#endif
