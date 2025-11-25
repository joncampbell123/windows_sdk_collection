//*---------------------------------------------------------------------------------
//|  ODBC System Administrator
//|
//|  This code is furnished on an as-is basis as part of the ODBC SDK and is
//|  intended for example purposes only.
//|
//*---------------------------------------------------------------------------------
#include "headers.h"

//*---------------------------------------------------------------------------------
//|	Global variables
//*---------------------------------------------------------------------------------
char 			szErrOut[100];

dCSEG(char)	szErrTitle[]						=	"Error!";
dCSEG(char) szError[]							=	"Error: %s,  File: %s, Line: %d";
dCSEG(char) szOutOfMemory[]					=	"Memory levels are very low.  Please exit other applications and try your request again.";
dCSEG(char) szInvalidParms[]					=	"Invalid parameters";
dCSEG(char) szRegisterClassFailed[]			=	"Register class failed";


//*------------------------------------------------------------------------
//| GetSQLState:
//|     Parameters:
//|			henv				- Pointer to environment
//|			hdbc				- Pointer to connection handle
//|			hstmt				- Pointer to statement
//|			szState			- Return sqlstate
//|			szNative			- Native return code (driver specific)
//|			szMessage		- Return message
//*------------------------------------------------------------------------
LPSTR GetSQLState(HENV henv, HDBC hdbc, HSTMT hstmt, 
				LPSTR szState, SDWORD FAR * pfNative, LPSTR szMessage)
{
	RETCODE 	rc;
	SWORD		cb;

	rc = SQLError(henv, hdbc, hstmt, szState, pfNative,
				szMessage, RTN_MSG_SIZE, &cb);
	if(rc == SQL_NO_DATA_FOUND || rc == SQL_ERROR)
		return NULL;
	else
		return szState;
}



//*------------------------------------------------------------------------
//| DoPostError:
//|	This function will post an error message to standard output, whereever
//|		that should be.
//| Parms:
//|	in			szErr						Error message
//|	in			szFile					File name
//|	in			cbLine					Line number
//| Returns:
//|	Nothing.
//*---------------------------------------------------------------------------------
void DoPostError(LPSTR szErr, LPSTR szFile, int cbLine)
{
	wsprintf(szErrOut, szError, (LPSTR)szErr, szFile, cbLine);
	MessageBox(NULL, szErrOut, szErrTitle, MB_OK);
}



//*------------------------------------------------------------------------
//| PrintErrors:
//|     Print out all relevant errors.
//|			ci				-  Pointer to client information
//*------------------------------------------------------------------------
void PrintErrors(CHILDINFO FAR * ci)
{
	if(!ci->hwndOut)
		DisplayErrors(ci->hwndOut, (LPSTR)szErrTitle, ci->henv, ci->hdbc, ci->hstmt);
	PrintErrorsHwnd(ci->hwndOut, ci->henv, ci->hdbc, ci->hstmt);
}


//*------------------------------------------------------------------------
//| PrintErrorsHwnd:
//|	Does the actual work.  Needed as separate function for those
//|	function which are not woking directly with a ci struct.
//| Parms:
//|	hwnd			Output window
//|	henv			Environment handle
//|	hdbc			Connection handle
//|	hstmt			Statement handle
//*------------------------------------------------------------------------
void PrintErrorsHwnd(HWND hwnd, HENV henv, HDBC hdbc, HSTMT hstmt)
{
	char 		szState[7]="";
	char 		szMessage[RTN_MSG_SIZE];
	SDWORD 	pfNative=0;

	while(GetSQLState(henv, hdbc, hstmt, 
				szState, &pfNative, szMessage) != NULL)
		szWrite(hwnd, 
				GetidsString(idsErrorString, szErrOut, sizeof(szErrOut)), 
				(LPSTR)szState,
				(LPSTR)szMessage);
}


//*------------------------------------------------------------------------
//| DisplayErrors:
//|	This will take all of the errors from the ODBC handles and display
//|	them using message box.  This is usually done when there is no
//|	output window to write them to.
//| Parms:
//|	hwnd			Window handle to own the message box
//|	title			The title for the message box
//|	henv			Environment handle to look on
//|	hdbc			Connection handle to look at
//|	hstmt			Statement handle to look at
//| Returns:
//|	Nothing
//*------------------------------------------------------------------------
void DisplayErrors(HWND hwnd, LPSTR title, HENV henv, HDBC hdbc, HSTMT hstmt)
{
	char 		szState[7]="";
	char 		szMessage[RTN_MSG_SIZE];
	SDWORD 	pfNative=0;

	while(GetSQLState(henv, hdbc, hstmt, 
				szState, &pfNative, szMessage) != NULL)
		szMessageBox((hwnd) ? hwnd : GetActiveWindow(), 
						MB_ICONEXCLAMATION,
						title,
						GetidsString(idsMsgErrorString, szErrOut, sizeof(szErrOut)), 
						(LPSTR)szState,
						(LPSTR)szMessage);
}
