//*---------------------------------------------------------------------------------
//|  ODBC System Administrator
//|
//|  This code is furnished on an as-is basis as part of the ODBC SDK and is
//|  intended for example purposes only.
//|
//|	Title:	INFO.H
//|		This file contains the prototypes and defines for executing SQL
//|			statements and displaying their results.
//*---------------------------------------------------------------------------------
#ifndef info_DEFS
#define info_DEFS

//*---------------------------------------------------------------------------------
//|	Macros and Defines
//*---------------------------------------------------------------------------------
#define MAX_QUALIFIER			45
#define	MAX_OWNER				40
#define	MAX_NAME	 			40
#define	MAX_TYPE	 			40
#define	MAX_SPEC_VAL			128

#define LIST_ALL				0
#define LIST_TABLES				1
#define LIST_VIEWS				2			
#define USER_SETCHECKSTATE		WM_USER + 1
#define USER_SETBUTTONSTATE 	WM_USER + 2
#define	USER_RESETLIST 			WM_USER + 3
#define	USER_SETSTATES			WM_USER + 4
#define	USER_SETDEFAULTS		WM_USER + 5

#define	LIST_WIDTH				184

//*---------------------------------------------------------------------------------
//|	Global variables
//*---------------------------------------------------------------------------------
extern dCSEG(char) szResultsClass[];
extern dCSEG(char) szODBCFunctions[];

//*---------------------------------------------------------------------------------
//|	Function prototypes
//*---------------------------------------------------------------------------------
void EditPipe(CHILDINFO FAR * ci);
BOOL DisplayGetInfo(CHILDINFO FAR * ci);
BOOL DisplayODBCFunctions(CHILDINFO FAR * ci);
BOOL DisplayODBCDataSources(CHILDINFO FAR * ci);
BOOL DisplayODBCDataTypes(CHILDINFO FAR * ci);
void HandlePipe(lpCHILDINFO lpci, lpRESULTSINFO lpri);

#endif
