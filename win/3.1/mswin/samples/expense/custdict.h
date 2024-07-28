/************************************************************

	PROGRAM:	CUSTDICT.H

	PURPOSE:
	
		Header file for CUSTDICT.C


************************************************************/


/* Global Variables */
char	* szDescription = 	"Custom Dictionary (Expense Report)";
char	* rgszCustWords[] = 
	{
	"Taxi",
	"Food",
	"Flight",
	"Hotel",
	"Misc"
	};

/* Constants */
#define CUST_DICT_SIZE (sizeof(rgszCustWords)/sizeof(char *))
#define cbMaxLen	256


/* Prototypes */
BOOL NEAR PASCAL FLookup(LPSTR);
BOOL NEAR PASCAL FBestGuess(LPSTR, LPSYV);
int  NEAR PASCAL GetDllDescription(LPSTR, int);
