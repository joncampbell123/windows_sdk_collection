/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    homefilt.c

Abstract:

    This filter shows how to replace a particular url

--*/

#include <windows.h>
#include <httpfilt.h>

#include <stdio.h>
#include <stdarg.h>

//
//  DebugMsg() is used for debugging.  Choose debugger output or log file.
//

#define OUTPUT_DEBUG_STRING		// comment out to use log file

#ifdef OUTPUT_DEBUG_STRING

	#define DEST               buff
	#define DebugMsg( x )      {                                    \
									char buff[256];                 \
									wsprintf x;                     \
									OutputDebugString( buff );      \
							   }

#else

	#define DEST ghFile
	#define DebugMsg(x) WriteToFile x;

	HANDLE ghFile;

	#define LOGFILE "c:\\homefilt.log"

	void WriteToFile (HANDLE hFile, char *szFormat, ...)
		{
		char szBuf[1024];
		DWORD dwWritten;

		va_list list;
		va_start (list, szFormat);

		vsprintf (szBuf, szFormat, list);

		hFile = CreateFile (LOGFILE, GENERIC_WRITE, 0, NULL,
							OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
			{
			SetFilePointer (hFile, 0, NULL, FILE_END);
			WriteFile (hFile, szBuf, lstrlen (szBuf), &dwWritten, NULL);
			CloseHandle (hFile);
			}

		va_end (list);
		}

#endif

//
// The script to run when '/' is queried.  Change this to your own home URL.
// If you change this, and the URL isn't correct, you'll get an error on
// your browser.  It will likely come in the form of a dialog box.  See 
// important note below (in OnPreprocHeaders function) before changing
// HOME_SCRIPT.
//

#define HOME_SCRIPT       "/sdk/home.htm"

//
//  Private prototypes
//

DWORD
OnPreprocHeaders(
    HTTP_FILTER_CONTEXT *         pfc,
    HTTP_FILTER_PREPROC_HEADERS * pvData
    );

//
//  Globals
//

BOOL
WINAPI
GetFilterVersion(
    HTTP_FILTER_VERSION * pVer
    )
{
	
    DebugMsg(( DEST,
            "[GetFilterVersion] Server filter version is %d.%d\n",
            HIWORD( pVer->dwServerFilterVersion ),
            LOWORD( pVer->dwServerFilterVersion ) ));

    pVer->dwFilterVersion = MAKELONG( 0, 1 );   // Version 1.0

    //
    //  Specify the types and order of notification
    //

    pVer->dwFlags = (SF_NOTIFY_SECURE_PORT        |
                     SF_NOTIFY_NONSECURE_PORT     |
                     SF_NOTIFY_PREPROC_HEADERS    |
                     SF_NOTIFY_ORDER_DEFAULT);

    strcpy( pVer->lpszFilterDesc, "Home script redirector, v1.0" );

    return TRUE;
}

DWORD
WINAPI
HttpFilterProc(
    HTTP_FILTER_CONTEXT *      pfc,
    DWORD                      NotificationType,
    VOID *                     pvData
    )
{
    DWORD dwRet;

    //
    //  Indicate this notification to the appropriate routine
    //

    switch ( NotificationType )
    {
    case SF_NOTIFY_PREPROC_HEADERS:

        dwRet = OnPreprocHeaders( pfc,
                                  (PHTTP_FILTER_PREPROC_HEADERS) pvData );
        break;

    default:
        DebugMsg(( DEST,
                "[HttpFilterProc] Unknown notification type, %d\n",
                NotificationType ));

        dwRet = SF_STATUS_REQ_NEXT_NOTIFICATION;
        break;
    }

    return dwRet;
}


DWORD
OnPreprocHeaders(
    HTTP_FILTER_CONTEXT *         pfc,
    HTTP_FILTER_PREPROC_HEADERS * pvData
    )
{
    CHAR  achUrl[2048];
    DWORD cb;

	//
	// A VERY IMPORTANT NOTE
	//
	// This filter will change the URL when it detects a slash-only (http:/)
	// URL.  During the set call, the filter will be re-entered!  This gives
	// the filter another chance to redirect the URL, but it also might
	// introduce an infinite loop.  Be careful when setting the new URL.
	//
	// For example, if we set HOME_SCRIPT to "/", we'll create an infinite
	// loop that will break the server.
	//

	//
	// This function is called when the filter receives a notification to
	// process the headers.  We scan for an empty URL and dynamically change
	// it to another URL.
	//

    //
    //  Get the url and user agent fields via ISAPI API
    //

    cb = sizeof( achUrl );

    if ( !pvData->GetHeader( pfc,
                             "url",
                             achUrl,
                             &cb ))
    {
		DebugMsg(( DEST,
                "[OnPreprocHeaders] GetHeader(url) failed.\r\n"));
        return SF_STATUS_REQ_ERROR;
    }

    //
    //  Redirect as appropriate
    //

    if ( achUrl[0] == '/' && achUrl[1] == '\0' )
    {
		//
		// Change the URL from / to /sdk/home.htm.
		// After all filters are done, IIS will then return 
		// home.htm to the user.
		//

        if ( !pvData->SetHeader( pfc,
                                 "url",
                                 HOME_SCRIPT ))
        {
		DebugMsg(( DEST,
                "[OnPreprocHeaders] SetHeader(url, %s) failed.\r\n", 
				HOME_SCRIPT));
            return SF_STATUS_REQ_ERROR;
        }
    }
	//
	// Pass the possibly changed header to the next filter in the chain
	//

    return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

