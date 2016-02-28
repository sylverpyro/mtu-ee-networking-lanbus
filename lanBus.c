/***********************************************************************
**  Michael Benson
**  lanBus.c
**  November 5, 2008
**
**  Purpose: This program will simulate a LAN network utilizing a Bus 
**	topology withouth carrier sensing.
**
**  Inputs: Can be passed {#ofHosts #ofBusses #timeSlots} on the command 
**	line (in that order).  If omitted, will default to 30 4 1000 
**	(respectivly).  Program will alwayse step though probability
**	on each host in .01 increments, resulting in 100 trials from 1% to 100%.
**
**  Outputs: Will write out results of simulation to OutputData.txt
**	If this file exists at runtime, the program will simply overwrite
**	the previous data file
**
***********************************************************************/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace std;

int host (int prob, int busses);
void summaryWriter(int ** pRunData, int prob, int busses, int slots, ofstream& outStream, const char OUTPUT_FILE[]);
//void summaryWriter(int ** runData, int prob, int busses, int slots);

int main (int noArgs, char *args[])
{

// Set the default values for hosts, busses, and slots
    int hosts = 30;
    int busses = 4;
    int slots = 50000;

// Initialize probability
    int prob = 1;

// Set up out output file and its related stream
    const char OUTPUT_FILE[] = "OutputData.txt";
    ofstream outStream;

    // Clear the output file from previous runs
    outStream.open(OUTPUT_FILE);
    outStream.close();

// Check if we have the required number of arguements
    // NOTE: the command name is 1 arguement, so 3+1 = 4
    if ( noArgs == 4 )
    {
	hosts = atoi(args[1]);
	busses = atoi(args[2]);
	slots = atoi(args[3]);
    }

// If not, skip overwiring defaults
    else    {	cout << "Using default values" << endl;	}
    cout << "# of Hosts  : " << hosts << endl;
    cout << "# of Busses : " << busses << endl;
    cout << "# of Slots  : " << slots << endl;

// Initialize raw data array [SLOTS][BUSSES]
    int **pRunData;
    pRunData = new int*[slots]; // 0=UNUSED 1=USED 1< = COLLISION
    for ( int i = 0; i < slots; i++)
    {	pRunData[i] = new int[busses]; }
    // Clear out all data in memory space
    for ( int i = 0; i < slots; i++ )
    {
	for ( int j = 0; j < busses; j++ )
	{  pRunData[i][j] = 0; 	}
    }

// Seed the random number generator so we dont have to do it again	
    srand(time(0));

    for ( ; prob <= 100; prob++)
    {
    // Run the simulation increemtning though each time slot then each host
	for ( int i = 0; i < slots; i++ )
	{
	    for ( int j = 0; j < hosts; j++)
	    {

	    // See if the host genrates a packet
		int temp=host(prob, busses);
		// If it does, increment the [SLOT][BUSS] 
		if ( temp != -1 )
		{	pRunData[i][temp]++; }
	    }
	}
	// After each probability increment, write out run details
	summaryWriter(pRunData, prob, busses, slots, outStream, OUTPUT_FILE);
    }

// Pass off the data from the run to summaryWriter

    cout << endl << "FINISHED" << endl;

    return 0;
}

/**********************************************************************
**
**  Class: host (int PROB, int BUSSES)
**
**  Takes: Probability (int 0-100) Number of Busses (int)
**	NOTE: Probability is read as PROB/100, so 1 = .01
**
**  Expects: Probability seed must already be set prior to execcution
**
**  Returns: Int representing which bus a packet was sent on
**	     -1 represents no packet sent
***********************************************************************/

int host (int prob, int busses)
{
    // If a packet is not generated, buss will be -1
    int buss = -1;

    // See if a packet is generated
    if ( rand() % 10000 <  prob ) 
    // If a packet is generated, determine the buss number
        { buss = rand() % busses; }

    return buss;
}

/**********************************************************************
**
**  Class: summaryWriter(int** RUNDATA, int PROB, int BUSSES, int SLOTS)
**
**  Takes:  runData - 2d array passed as a double int pointer containing
**	    the data to write out to the output file
**
**	    Probability - int from 0%-100%
**	    
**	    Busses - int, positive number only
**	    
**	    Slots - int, positive number only
**
**  Expects: Write access to the folder the file is located in
**
**  Returns: Nothing
**
**********************************************************************/

void summaryWriter(int ** pRunData, int prob, int busses, int slots, ofstream& outStream, const char OUTPUT_FILE[])
{

// Find the Collisions, empty slots, good sends, bus % used
    enum { COLLIDE, GOOD, BUSS };
    int summaryData[busses+3];
    // Initialize summaryData Array
    for (int i = 0; i < busses+3; i++)
    {	summaryData[i] = 0;     }

// Increment through the time slots
    for ( int i = 0; i < slots; i++ )
    {
	// Track number of busses used and colissions on this slot
	int bussesUsed = 0;

	// Icrement through each buss looking for packets
	for ( int j = 0; j < busses; j++ )
	{
	    // Grab the bus,slot from summary data
	    int testPoint = pRunData[i][j];
	    
	    // If more than 1 packet is on the buss
	    if ( testPoint == 1 )
	    {
		summaryData[GOOD]++;
		bussesUsed++;
	    }

	    // If not empty or a collision, it was a good send, increment
	    else if ( testPoint > 1 )
	    {
		// Increemnt collisions
		summaryData[COLLIDE]++;
	    }

	}
	// Increment the appropriate buss%used
	    summaryData[BUSS+bussesUsed]++;
    }

    outStream.open(OUTPUT_FILE, ios::app);
    // If the output file does not open, break
    if (outStream.fail() )
        { exit(-1); }
   
    if ( prob == 1 )
    {    
    // Write out the headers
        outStream.setf(ios::right);
        outStream.width(7);
	outStream << "PROB" << ";";
	outStream.width(7);
	outStream << "COLI" << ";";
	outStream.width(7);
	outStream << "GOOD" << ";";

	// Write a header for 1of4, 2of4, 3of4 ... ect
	for ( int i = 0; i < busses+1; i++ )
	    { 
		outStream.width(5);
		outStream << i << "/" << busses << ";"; 
	    }
        
	outStream << endl;
    }// End of header Writing

    outStream.setf(ios::right);
    outStream.width(7);
// Write out Probability
    outStream << prob << ";"; 

// Write out summary data
    for ( int i = 0; i < busses+3; i++)
    {	
	outStream.width(7);
	outStream << summaryData[i] << ";";	
    }
    
    outStream << endl;

    outStream.close();
}
