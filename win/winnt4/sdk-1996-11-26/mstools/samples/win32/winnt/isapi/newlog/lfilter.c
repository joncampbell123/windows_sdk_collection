/*++

Copyright (c) 1995-1996  Microsoft Corporation

Module Name:

    logfilter.c

Abstract:

    This module is a Microsoft HTTP server filter interface that logs
	additional information.  It logs the Referer, UserAgent, and the 
	Cookie used for the MSN custom home page.

Author:

    Mark Grossman (markgros)   07-Nov-1995

Revision History:

	Mark Ingalls  (marking)		05-Feb-1996
	Changed behaviour of this filter to only log the Referrer and 
	the User Agent String to the log for use on www.microsoft.com.

    The format of the Log File is changed by this filter.  Any log
    conversion utility (including convlog.exe that comes with IIS)
    will not work on these log files.

    Old log Format:
    ClientIP, UserName, Date, Time, Service, ServerName, ServerIP, ProcessingTime, BytesReceived, BytesSent, ServiceStatus, Win32Status, Operation, TargetURL, Parameters

    New log Format:
    ClientIP, UserName, Date, Time, Service, ServerName, ServerIP, ProcessingTime, BytesReceived, BytesSent, ServiceStatus, Win32Status, Operation, TargetURL, UserAgent, Referer, Parameters

    As far as IIS is concerned, this filter puts the new information into the
    TargetURL field.  This will cause problems if IIS is configured to using ODBC
    logging and the new field is longer than 255 chars.
--*/

#include <windows.h>
#include <httpfilt.h>

#define MAJOR_VERSION 1
#define MINOR_VERSION 0

// ReportWFEvent
//
// report an event to the NT event watcher
// pass 1, 2 or 3 strings
//
// no return value

VOID ReportWFEvent(PTSTR string1,PTSTR string2,PTSTR string3,PTSTR string4,WORD eventType, WORD eventID) {
	HANDLE hEvent;
	PTSTR pszaStrings[4];
	WORD cStrings;

	cStrings = 0;
	if ((pszaStrings[0] = string1) && (string1[0])) cStrings++;
	if ((pszaStrings[1] = string2) && (string2[0])) cStrings++;
	if ((pszaStrings[2] = string3) && (string3[0])) cStrings++;
	if ((pszaStrings[3] = string4) && (string4[0])) cStrings++;
	if (cStrings == 0)
		return;
	
	hEvent = RegisterEventSource(
					NULL,		// server name for source (NULL means this computer)
					"NewLog");	// source name for registered handle  
	if (hEvent) {
		ReportEvent(hEvent,					// handle returned by RegisterEventSource 
				    eventType,				// event type to log 
				    0,						// event category 
				    eventID,				// event identifier 
				    0,						// user security identifier (optional) 
				    cStrings,				// number of strings to merge with message  
				    0,						// size of binary data, in bytes
				    pszaStrings,			// array of strings to merge with message 
				    0);		 				// address of binary data 
		DeregisterEventSource(hEvent);
	}
}

DWORD
OnLog(
    HTTP_FILTER_CONTEXT *  pfc,
    HTTP_FILTER_LOG *      pvData
    );

//
//  Globals

BOOL
WINAPI
GetFilterVersion(
    HTTP_FILTER_VERSION * pVer
    )
{
    //Write event to Event Log to indicate that we loaded ok.
    
    ReportWFEvent(	"[NewLog]",
					"[GetFilterVersion] Starting Extended Logging",
					"",
					"",
					EVENTLOG_INFORMATION_TYPE,
					3 ); 

    pVer->dwFilterVersion = HTTP_FILTER_REVISION;
    // Version 1.0

    //
    //  Specify the types and order of notification
    //  We will only get called as the server is about to write information
    //  to the log file.

    pVer->dwFlags = (SF_NOTIFY_LOG | SF_NOTIFY_ORDER_DEFAULT);

    strcpy( pVer->lpszFilterDesc, "Extended Logging Filter" );


    return TRUE;
}

DWORD
WINAPI
HttpFilterProc(
    HTTP_FILTER_CONTEXT *      pfc,
    DWORD                      NotificationType,
    VOID *                     pvData )
{
    DWORD dwRet;



    //
    //  Indicate this notification to the appropriate routine
    //

    switch ( NotificationType )
    {

    case SF_NOTIFY_LOG:

        dwRet = OnLog( pfc,
                       (PHTTP_FILTER_LOG) pvData );
    break;

    default:

        dwRet = SF_STATUS_REQ_NEXT_NOTIFICATION;
    break;
    }

    return dwRet;
}

DWORD
OnLog(
    HTTP_FILTER_CONTEXT *  pfc,
    HTTP_FILTER_LOG *      pvData
    )
{
#define HEADER_SIZE	4182
#define LOG_FIELD_DELIMITER_SIZE 2

	CHAR  achUserAgent[HEADER_SIZE];
    DWORD cbUserAgent= HEADER_SIZE;
	CHAR  achReferer[HEADER_SIZE];
    DWORD cbReferer= HEADER_SIZE;
	DWORD cbLogInfo = 0;
	PTSTR pszLogInfo;
	DWORD i,j;

	//Get user agent from server
	//If the Web Client does not provide a User Agent String, we substitute a '-'
	//If an error occurs (other than not finding a User Agent String) we log
	//an error to the event log and return.

	if ( !pfc->GetServerVariable( pfc,"HTTP_USER_AGENT",achUserAgent,&cbUserAgent ))
    {
        //ERROR_INVALID_INDEX indicates that the client did not supply
        //the string we are looking for
        
		if (ERROR_INVALID_INDEX != GetLastError())  {
			ReportWFEvent(	"[NewLog]",
							"[OnLog] GetHeader User Agent Error",
							"",
							"",
							EVENTLOG_ERROR_TYPE,
							7);

			return SF_STATUS_REQ_ERROR;
		}

        //if the Web Client did not supply a User Agent String
		//we substitute a dash '-'
		
		else {
			achUserAgent[0] = '-';
			achUserAgent[1] = '\0';
			cbUserAgent = 2;
		}
    }    
    
	//Get referer from server
	//If the Web Client does not provide a Referer String, we substitute a '-'
	//If an error occurs (other than not finding a Referer String) we log
	//an error to the event log and return.

	if ( !pfc->GetServerVariable( pfc,"HTTP_REFERER",achReferer,&cbReferer ))
    {
        //ERROR_INVALID_INDEX indicates that the client did not supply
        //the string we are looking for
        
        if (ERROR_INVALID_INDEX != GetLastError())  {
			ReportWFEvent(	"[NewLog]",
							"[OnLog] GetHeader Referer Error",
							"",
							"",
							EVENTLOG_ERROR_TYPE,
							8);

			return SF_STATUS_REQ_ERROR;
		}

        //if the Web Client did not supply a User Agent String
		//we substitute a dash '-'

		else {
			achReferer[0] = '-';
			achReferer[1] = '\0';
			cbReferer = 2;
		}
    }    
	
	//calculate the length of the new buffer needed

	cbLogInfo =		strlen(pvData->pszTarget) + 
					LOG_FIELD_DELIMITER_SIZE + 
					cbUserAgent + 
					LOG_FIELD_DELIMITER_SIZE + 
					cbReferer + 
					1;   

	//Allocate memory from server using the API provided

	pszLogInfo = pfc->AllocMem(pfc, cbLogInfo,0);

	//copy old target into new log string

	for( i=0; (pvData->pszTarget)[i]; i++)  {
		pszLogInfo[i]=(pvData->pszTarget)[i];
	}

	//add comma for separator

	pszLogInfo[i++]=',';
	pszLogInfo[i++]=' ';
	
	//add user agent to new log string

	for (j=0; achUserAgent[j]; j++,i++)  {
		pszLogInfo[i]=achUserAgent[j];
	}
	
	//add comma for separator

	pszLogInfo[i++]=',';
	pszLogInfo[i++]=' ';
	
	//add referer to new log string trimming off any parameters

	for (j=0; achReferer[j] && ('?'!=achReferer[j]); j++,i++)  {
		pszLogInfo[i]=achReferer[j];
	}
	
	//finish new log string

	pszLogInfo[i]='\0';

    //set the pointer for TargetURL to our new log string
	
	pvData->pszTarget = (CONST CHAR*) pszLogInfo;

    return SF_STATUS_REQ_NEXT_NOTIFICATION;
}
