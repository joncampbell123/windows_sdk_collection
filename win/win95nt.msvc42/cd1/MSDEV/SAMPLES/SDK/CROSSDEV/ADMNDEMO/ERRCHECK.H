//*---------------------------------------------------------------------------------
//|  ODBC System Administrator
//|
//|  This code is furnished on an as-is basis as part of the ODBC SDK and is
//|  intended for example purposes only.
//|
//|
//|	 Title: ERRCHECK.H
//|
//|		This include file contains the definitions for the ERRCHECK.C module.
//|			This module contains common error checking functions.
//|
//*---------------------------------------------------------------------------------
#ifndef errcheck_DEFS
#define errcheck_DEFS

//------------------------------------------------------------------------
//  Defines
//------------------------------------------------------------------------
#define	RTN_MSG_SIZE		SQL_MAX_MESSAGE_LENGTH - 1

#define PostError(str)															\
		DoPostError(str, __FILE__, __LINE__)


//------------------------------------------------------------------------
//  Declare global variables
//------------------------------------------------------------------------
extern dCSEG(char) szErrTitle[];
extern dCSEG(char) szError[];
extern dCSEG(char) szOutOfMemory[];
extern dCSEG(char) szInvalidParms[];
extern dCSEG(char) szRegisterClassFailed[];


//------------------------------------------------------------------------
//  Declare function prototypes
//------------------------------------------------------------------------
LPSTR GetSQLState(HENV henv, HDBC hdbc, HSTMT hstmt, LPSTR szState, 
		SDWORD FAR * pfNative, LPSTR szMessage);
void DoPostError(LPSTR szErr, LPSTR szFile, int cbLine);
void PrintErrors(CHILDINFO FAR * ci);
void PrintErrorsHwnd(HWND hwnd, HENV henv, HDBC hdbc, HSTMT hstmt);
void DisplayErrors(HWND hwnd, LPSTR title, HENV henv, HDBC hdbc, HSTMT hstmt);

                                                                              
#endif
