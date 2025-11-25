/*/////////////////////////////////////////////////////////////////////////////
// MODULE:
//              CVTDOC.CPP
//
// DESCRIPTION:

  CVTDOC - ISAPI Filter for Dynamic Document Conversion
  -----------------------------------------------------
Web content creators and webmasters often want to "publish" a document
or data file to the Web. However, it can be very inconvenient to constantly
run a conversion program to generate new HTML each time the document
or data file is updated.  Relying on the webmaster to run the conversion
program for data that is often updated is also prone to error. 

CVTDOC is an ISAPI filter that converts documents to HTML dynamically
as needed when the HTML file is accessed.  If the HTML file is missing
it will be generated from a source document found with the same basename.
If the HTML file is older than a source document, it will be regenerated
from the conversion program.

SETUP
-----
1. Copy CVTDOC.DLL to an appropriate subdirectory, such as the CGI-BIN
subdirectory of your Web content directory.  
2. Update the Filter DLLs parameter of IIS. Run REGEDT32.EXE.
3. Add the full path of CVTDOC.DLL to  
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\W3SVC\Parameters\Filter DLLs
(the DLLs are separated by commas)
4. Create a Conversion subkey of the W3SVC\Parameters key. 
5. Add each extension (e.g. .xls, .doc) as a separate value. 
6. Enter the full path of the conversion program to run for each extension. 
7. List each conversion program as taking two arguments of "%s %s".
8. Place the conversion programs called in the directories referenced.
9. The conversion program needs to be able to be executed from the command
line with two arguments: the source file name and the generated HTML file.
You can find conversion programs for almost any data format on the Web, but
you may need to write "wrapper" batch files or programs that allow the 
conversion program to conform to this format.

// USES:
//
// EXPORTS:
//		GetFilterVersion
//		HttpFilterProc
//
*/

#include <ctype.h>  
#include <stdio.h>                
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <iostream.h>
#include <strstrea.h>
#include <fstream.h>
#include <httpext.h>
#include <httpfilt.h>


#define W3SVCKEY "System\\CurrentControlSet\\Services\\W3SVC"

HANDLE hEvtLog;

///////////////////////////////////////
// FUNCTION:
//              DllEntryPoint
// DESCRIPTION:
//              DllEntryPoint allows potential initialization of state variables.
// RETURNS:
//              TRUE - only TRUE right now
BOOL WINAPI DllEntryPoint (HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpv)
{
    return (TRUE);
}

//////////////////////////////////////
// FUNCTION:
//   GetFilterVersion  
// DESCRIPTION:
//   Return the version this server is built for.  See <httpext.h> for
//   a prototype.  This function is required by the spec.
// RETURNS:
//         TRUE 
BOOL WINAPI GetFilterVersion (PHTTP_FILTER_VERSION pFilterVersion)
{
   pFilterVersion->dwFilterVersion = HTTP_FILTER_REVISION;
   strcpy (pFilterVersion->lpszFilterDesc,
	    "CVTDOC - Converts document or data into HTML if HTML not present or older");
	    
	// now register for events we're interested in 
	pFilterVersion->dwFlags= (	SF_NOTIFY_ORDER_HIGH | // high so we can be sure to intercept!
								SF_NOTIFY_SECURE_PORT |
								SF_NOTIFY_NONSECURE_PORT |
 // tell us about all URL requests
								SF_NOTIFY_URL_MAP  
							);

	hEvtLog=RegisterEventSource(NULL,"CVTDOC");	


	return TRUE;
} 


/////////////////////////////////////
//	FUNCTION:
//		Win32Exec
//	DESCRIPTION:
//		alternative to WinExec() function 
//		(which should not be called in Win32 programs)
//		calls Create Process and waits for termination
// INPUTS
//		szCommand - the command line to run: 
//		full path and all command line arguments
// RETURNS
//		0 - if CreateProcess fails
//		dwExitCode - if CreateProcess succeeds
//
int Win32Exec(char *szCommand)
{
	STARTUPINFO si;
	PROCESS_INFORMATION piProcess;
	ZeroMemory(&si,sizeof si);
	si.cb=sizeof si;
	BOOL result; 
	result=CreateProcess(NULL,szCommand,NULL,NULL,FALSE,
					CREATE_DEFAULT_ERROR_MODE|DETACHED_PROCESS,
					NULL,NULL,&si,&piProcess);
	DWORD dwExitCode;
	if (result==TRUE)
	{
		CloseHandle(piProcess.hThread);
	
		if (WaitForSingleObject(piProcess.hProcess,INFINITE)!=WAIT_FAILED)
			GetExitCodeProcess(piProcess.hProcess,&dwExitCode);
		CloseHandle(piProcess.hProcess);
		
	}
	return result==TRUE? dwExitCode: 0;
}


//////////////////////////////////////
// FUNCTION:
//		CvtToHTML
// DESCRIPTION:
//		Converts file name to HTML if:
//			1) the file name extension is registered and associated with conversion program
//			2) if it's a structured storage file, and then launch the conversion program based on the GUID instead of the extension.
// RETURNS:
//		TRUE - if we could convert it
BOOL CvtToHTML(char *pszSrcFile,char *pszHTMLFname)
{
	BOOL bConverted=FALSE;
 	// check to see if we have conversion registered for this file
	char *pszExt=strrchr(pszSrcFile,'.');
	if (pszExt){
		char szKey[1024];
		strcpy(szKey,W3SVCKEY);
		strcat(szKey,"\\Parameters\\Conversions");
		HKEY hkey;
		LONG lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,szKey,0,KEY_READ,&hkey);
		char szValue[16],szData[256];
		DWORD iValue,nLenValue=sizeof szValue,dwType,nLenData=sizeof szData;
		if (lResult==ERROR_SUCCESS){
			// look for a conversion to run 
			for (iValue=0;
				(lResult=RegEnumValue(hkey,iValue,(LPTSTR)szValue,&nLenValue,0,&dwType,(LPBYTE)szData,&nLenData))==ERROR_SUCCESS;
				iValue++)
			{
				if (!_stricmp(szValue,pszExt)) // found conversion!
				{
					char szCmd[_MAX_PATH*2];
					wsprintf(szCmd,szData,pszSrcFile,pszHTMLFname);
					if (Win32Exec(szCmd)){
						bConverted=TRUE;
					}
					break;
				}		
				nLenValue=sizeof szValue;
				nLenData=sizeof szData;
			}
		}

		LPSTR aszMsg[]={
			"Failed to find conversion",
		};
		if (bConverted!=TRUE){
			ReportEvent(hEvtLog,EVENTLOG_WARNING_TYPE,0,
				1,	// event identifier 
				NULL,1,0,
				(const char **)aszMsg,NULL);
		}
	}
	return bConverted;		
}

//////////////////////////////////////
// FUNCTION:
//		FileDateCompare
// DESCRIPTION:
//		compares two file modification dates
// 
// RETURNS:
//              1 - if first file is newer 
//				0 - same date
//				-1 - first file older

int FileDateCompare(char *pszFname1,char *pszFname2)
{
	struct _stat buf1,buf2;
	if (_stat(pszFname1,&buf1))
		return -1;
	if (_stat(pszFname2,&buf2)) // 1 if file 2 not there
		return 1;
	return buf1.st_mtime>buf2.st_mtime?1:(buf1.st_mtime<buf2.st_mtime?-1:0);
}

//////////////////////////////////////
// FUNCTION:
//		HttpExtensionProc
// DESCRIPTION:
//       Reads in form data and sends message via MAPI
//       to To:, Cc:, Bcc: specified in form
//       with Subject and Body also specified by form
// RETURNS:
//              HSE_STATUS_SUCCESS - if everything works OK
DWORD WINAPI HttpFilterProc (PHTTP_FILTER_CONTEXT pFC,DWORD dwNotificationType,LPVOID pvNotification)
{
	// look at SF_NOTIFY_URL_MAPs only!
	if (dwNotificationType==SF_NOTIFY_URL_MAP){
		HTTP_FILTER_URL_MAP *pURLMap=(HTTP_FILTER_URL_MAP *)pvNotification;
		char szSrcFile[_MAX_PATH];
		char *pszExt;
		strcpy(szSrcFile,pURLMap->pszPhysicalPath);
		if (pszExt=strrchr(szSrcFile,'.')){ // check for extension
			if (!strnicmp(pszExt,".htm",3)){ // is it HTML?
				*pszExt='\0';
				if (!access(szSrcFile,0)){//check for presence of source file
					if (FileDateCompare(szSrcFile,pURLMap->pszPhysicalPath)>0) // source file newer
						if (CvtToHTML(szSrcFile,pURLMap->pszPhysicalPath)==TRUE) // convert file if possible
							return SF_STATUS_REQ_HANDLED_NOTIFICATION;
				}
			}
		}
	}
	// can't convert it
	return SF_STATUS_REQ_NEXT_NOTIFICATION; 
}

