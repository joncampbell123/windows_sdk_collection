/****************************************************************************
*
*
*    PROGRAM: WebRunner.c
*
*    PURPOSE: Example of Internet Server Application
*
*  Exports:
*
*  BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
*
*   As per the Web Server API Spec, this just returns the
*   version of the spec that this server was built with.  This
*   function is prototyped in httpext.h
*
*   BOOL WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
*
*   This function does all of the work.  Once called it retrieves all of
*   the environment data from the server via the GetServerVariable
*   callback function, and reads all of the client data if it's not already
*   available. Then it processes the request.
****************************************************************************/

// Standart includes

#include <windows.h>
#include <httpext.h>
#include "runner.h"

// Variables 
						
HANDLE hHeapHandle = 0;

/****************************************************************************
*
*    FUNCTION: GetExtensionVersion
*
*    PURPOSE: Standart procedure which needs to be exported from the DLL.
*
*    COMMENTS:   Will be called by the server.
*
****************************************************************************/

BOOL WINAPI GetExtensionVersion (HSE_VERSION_INFO *pVer)
{
    pVer->dwExtensionVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);

    lstrcpyn(pVer->lpszExtensionDesc,
                  "Sample Internet Web Server WebRunner Application",
                  HSE_MAX_EXT_DLL_NAME_LEN );

    return TRUE;
} // GetExtensionVersion()



/****************************************************************************
*
*    FUNCTION:   HttpExtensionProc
*
*    PURPOSE: Standart procedure which needs to be exported from the DLL.
*
*    COMMENTS:   Will be called by the server.
*
****************************************************************************/

DWORD WINAPI HttpExtensionProc (EXTENSION_CONTROL_BLOCK *pECB)
{
    CHAR        szBuff[4096];
	CHAR        *lpszQuery;
	CHAR        *lpszTemp = NULL;
    CHAR        *lpszTemp2 = NULL;
	CHAR         *szTempBuff2 = NULL;
	CHAR        *lpszListBox = NULL;
	CHAR        *szTemp1 = NULL;
	CHAR        *szTemp2 = NULL; 
  	LPSTR       lpszTempBuff1;
  	BOOL        bAdmin; 
    DWORD    dwLen;
 	DWORD    cbQuery;
 	int              cReturn=0,     cTab = 0;

	wsprintf ( szBuff,
                 "<html>\n"
			     "Content-Type: text/html\r\n"
                 "\r\n"
                "<head><title>WebRunner</title></head>\n"
                "<body><h1>Welcome to WebRunner.</h1>\n"
                "<hr>\n");

    dwLen = lstrlen ( szBuff );
    pECB -> ServerSupportFunction ( pECB -> ConnID,
    		HSE_REQ_SEND_RESPONSE_HEADER, "200 OK", &dwLen, ( LPDWORD ) 
    		szBuff );

    // We need to read registry to retreive mode (admin or user) and 
    // available commands
	if (    !(  lpszListBox = ReadRegistry ( &bAdmin ) ) )
    {
        // We could not retreive registry settings.
  	    wsprintf ( szBuff, "Registry settings are corrupted!<br>\r\n" );
	    dwLen = lstrlen ( szBuff );
		pECB -> WriteClient ( pECB -> ConnID, szBuff, &dwLen, dwLen );
		return HSE_STATUS_ERROR;
	}
	else
        lstrcpy (szBuff, "<h4>Mode:	" );


	// Note: we can use PUT on any Web page constructed below. 
    // Refer to HTML documentation in differences between GET and PUT.

    if ( bAdmin ) 
    {
        // We are in the administrator mode. User can enter any command.
        // We need to draw  Web Page with the edit box on it.
		// We don't specify any hardcoded path for runner.dll. It will allow 
        // us to keep this dll us to keep this dll in any directory in the run
        //  time.

		lstrcat ( szBuff, "administrator</h4><br><h2>To run your command " );
		lstrcat ( szBuff, "please enter it here:</h2><p><form  " );
     	lstrcat ( szBuff, "action=\"runner.dll\" method=get><INPUT NAME= " );
       	lstrcat ( szBuff, "\"COMMAND\" VALUE=\"\" ><BR><input " );
		lstrcat ( szBuff, "	type=\"submit\" value=\"Submit Entry\"><input " );
        lstrcat ( szBuff, "type=\"reset\" value=\"Reset Form\">"  );
         lstrcat ( szBuff, "</form><p><h2>Output:</h2><p><hr><pre> " );
	    dwLen = lstrlen ( szBuff );
		pECB -> WriteClient ( pECB -> ConnID, szBuff, &dwLen, dwLen );
    }
	else
	{ 
        // We are in the user mode. User can only enter command from the list 
        // box. We need to draw  Web Page with the list box on it.
		  
	 	lstrcat ( szBuff, "user</h4><br><h2> Please choose command to run:");
		lstrcat ( szBuff, "</h2><p><form action=\"runner.dll\" method=get> ");
        lstrcat ( szBuff, "	<SELECT NAME=\"COMMAND\" SIZE=3>");
 	    dwLen = lstrlen(szBuff);
		pECB -> WriteClient ( pECB -> ConnID, szBuff, &dwLen, dwLen);

        // Now we will output items of the list box.   lpszListBox  has been 
        // retrieved from the registry before.
        dwLen=lstrlen(lpszListBox);
		pECB->WriteClient(pECB->ConnID, lpszListBox, &dwLen, dwLen);
		
		lstrcpy ( szBuff,"<p></SELECT><input type=\"submit\" value=\"Submit ");
        lstrcat ( szBuff,"Entry\"><input type=\"reset\" value=\"Reset Form\">");
        lstrcat ( szBuff,"</form><p><h2>Output:</h2><p><hr><pre>");
		dwLen = lstrlen ( szBuff );
		pECB -> WriteClient ( pECB -> ConnID, szBuff, &dwLen, dwLen);
	  		
	 }

	if ( !stricmp ( pECB -> lpszMethod, "get") )
        // GET
	    lpszQuery = pECB->lpszQueryString;		  
    else 
    {												  
        // POST
        if ( pECB -> cbTotalBytes == 0)     			  
		{
            // No Query at all
  	  	    wsprintf ( szBuff, "You command was empty<br>\r\n" );
	        dwLen = lstrlen ( szBuff );
		    pECB -> WriteClient ( pECB -> ConnID, szBuff, &dwLen, dwLen );
		    return HSE_STATUS_ERROR;
		}
        else
        {
            // Start processing of input information here.
            if( ! ( lpszTemp = ( CHAR * ) LocalAlloc ( LPTR, 
                                          pECB -> cbTotalBytes ) ) )	
                return HSE_STATUS_ERROR;
            strcpy ( lpszTemp,  ( CHAR * ) pECB -> lpbData );
            if ( ( cbQuery = pECB -> cbTotalBytes - pECB -> cbAvailable) > 0 )
                pECB -> ReadClient ( pECB -> ConnID,  ( LPVOID ) 
                              ( lpszTemp + pECB -> cbAvailable ), &cbQuery );
            //
            // For POST requests, two terminating characters are added to the 
            // end of the data. Ignore them by placing a null in the string.
            //
            * ( lpszTemp + pECB -> cbTotalBytes - 2 ) = '\0';
            lpszQuery = lpszTemp;
        }
    }

	// At  this point   lpszQuery  has a full parametr string supplied to the 
    // server.  We will need to parse this string to get a value assosiated 
    // with the  "COMMAND". We will then execute this command. 
    // Acctual command will be stored in lpszTemp2

	 lpszTemp2 = GetParamValue ( lpszQuery, "COMMAND" );
	 LocalFree ( lpszTemp ); 
	 

	
	if(lpszTemp2)
    {
        // Server was supplied with the acctual command.
	    // Do all processing here.
	    // We need to spoofing verification for user mode only. 
        // Since user can fake a list box on the page and supply not 
        // authorized command to execute, we need to check if this 
        // command in the registry.

		if ((! strstr ( lpszListBox, lpszTemp2 ) ) && !bAdmin )
		{
		    wsprintf (szBuff, 
		              "<h2>Command: <i>%s</i> is not in the list box. %s%s%s",
                      lpszTemp2, "<br>You are not authorized to run it.",
                      "<br>\r\n", ENDPAGE);   
		    dwLen = lstrlen ( szBuff );
			pECB -> WriteClient ( pECB -> ConnID, szBuff, &dwLen, dwLen );
			pECB -> ServerSupportFunction ( pECB -> ConnID, 
                          HSE_REQ_DONE_WITH_SESSION, "200 OK", 
                          NULL, NULL );
			return HSE_STATUS_SUCCESS;
		}
        // We need to deallocate registry buffer when we done with it.
       	// Spoofing verification was the last time when we used it.

    	LocalFree ( ( HLOCAL ) lpszListBox ); 

        // Now it is time to execute the command.
        // lpszTempBuff1 is a pointer to command output buffer. 
	    dwLen = RunApp ( lpszTemp2, ( LPSTR * ) &lpszTempBuff1 );

	    if ( !dwLen )
	    {
 		    lstrcpy ( szBuff, "Create failed<br>\r\n" );	
	        dwLen = lstrlen ( szBuff );
		    pECB -> WriteClient ( pECB -> ConnID, szBuff, 
                          &dwLen, dwLen);
		    return HSE_STATUS_ERROR;
	    }
	    {
            // We need to strip all enter character to avoid empty 
            // lines in the command output. We will use <pre></pre>
            // tags to simulate exact output text on the Web page.
		    DWORD i;
		    for ( i = 0; i < dwLen; i++ ) 
                if ( lpszTempBuff1 [i] == '\r' ) 
                    lpszTempBuff1[i] = ' ';
	    }
	    pECB -> WriteClient ( pECB -> ConnID, lpszTempBuff1, &dwLen, dwLen );
	    dwLen = lstrlen ( ENDPAGE );
	    pECB -> WriteClient ( pECB -> ConnID, ENDPAGE, &dwLen, dwLen );
	    pECB -> ServerSupportFunction( pECB->ConnID,
                      HSE_REQ_DONE_WITH_SESSION, "200 OK", NULL, NULL );
    }
    else
    {
	    // If we are here then one of two situation a had place
        // 1. HTML file did not have a COMMAND value.
        // This is only the case it HTML was manualy edited incorrectly.
        // 2. Emty command was emtpy.
		wsprintf ( szBuff, " There is no COMMAND field in HTML source"
                                     " or empty command was entered<br>\r\n" );
	    dwLen = lstrlen ( szBuff );
		pECB -> WriteClient ( pECB -> ConnID, szBuff, &dwLen, dwLen );
		return HSE_STATUS_ERROR;
    }
	FreeOutBuffer(lpszTempBuff1);
	return HSE_STATUS_SUCCESS;
}   // End of Program.



/****************************************************************************
*
*    FUNCTION: GetParamValue
*
*    PURPOSE: This function return value assosiated with lpszParam in the
*  			  lpszQuery string.
*
*    COMMENTS: 
*			  If  lpszQuery   does not have  lpszParam  in it or no value
*   		  is assisiated with with  lpszParam  NULL will be return.
*   		  Example: 
*		       lpszQuery [ ] = "COMMAND1=Test1&COMMAND2=Test2"
*      		   lpszParam [ ] = "COMMAND2"
*	       	   Return string: Test2
*
*
****************************************************************************/

CHAR * GetParamValue(CHAR *lpszQuery, CHAR *lpszParam)
{

    CHAR *pValueStart = NULL;
    CHAR *pValueEnd = NULL;
    CHAR *lpszValue = NULL;
    CHAR *szTemp1 = NULL;
     ULONG cbValue;

    //
    // Find the value passed in by the client for some particular 
    // parameter within the query string. 
    //
    // Don't forget to free the returned new string!
    //

    //
    // First we find the occurance of the parameter, add the length of the
    // parameter name, and then add one for the "=" character put between
    // the parameter and the value; this locates the value.
    //

    pValueStart = strstr ( lpszQuery, lpszParam );
    if ( ! pValueStart )			// parameter doesn't exist
	    return NULL;

    pValueStart += strlen ( lpszParam ) + 1;
  
    //
    // Now determine the length of the value string.
    //

    pValueEnd = strchr(pValueStart, '&');
    if (pValueEnd)		
        // if this wasn't the last param in the list
        cbValue = pValueEnd - pValueStart;
    else	
        // this was the last param in the list
        cbValue = strlen(pValueStart);

    // Return NULL if we  have zero lenght string.
    if ( !cbValue ) return NULL;
    if ( ! ( lpszValue = (CHAR *) LocalAlloc ( LPTR, cbValue + 1 ) ) )  
        return NULL;
    strncat ( lpszValue, pValueStart, cbValue ); 

    // Finally lpszValue has needed value.


    //
    // Now replace "+" characters with " " characters adn
    // "%xx" (hexadecemal) to the ASCII representation.
    // 

    szTemp1 = lpszValue;
    while ( * szTemp1 )
    {
        if ( *szTemp1 ==  '+' )  *szTemp1 = ' ';
        szTemp1++;
    }
	EscapeToAscii(lpszValue);
    return lpszValue;
} // GetParamValue



/****************************************************************************
*
*    FUNCTION: EscapeToAscii
*
*    PURPOSE: This function calls HexToAscii for each occurance of %xx in the 
*			  parametr string.
*
*    COMMENTS: 
*
****************************************************************************/

void EscapeToAscii (CHAR *lpEscape) 
{
    int i, j;
    for (i = 0, j = 0; lpEscape [j] ; ++i, ++j ) 
    {
        if ( ( lpEscape [i] = lpEscape [j] ) == '%' ) 
        {
            lpEscape [ i ] = HexToAscii ( &lpEscape [j+1] );
            j+=2;
        }
    }
    lpEscape[i] = '\0';
}


/****************************************************************************
*
*    FUNCTION: HexToAscii
*
*    PURPOSE: This function will replace %xx (hexadecemal) symbols to the 
*			  ASCII representation by calling HexToAscii .
*
*    COMMENTS: 
*
****************************************************************************/

CHAR HexToAscii ( CHAR *lpString) 
{
  CHAR CH;
  CH = ( lpString [0] >= 'A' ? ( ( lpString [0] & 0xDF ) - 'A' ) + 10 :  
       ( lpString [0] - '0' ) );
  CH *= 16;
  CH += ( lpString [1] >= 'A' ? ( ( lpString [1] & 0xDF ) - 'A' ) + 10 :  
       ( lpString [1] - '0' ) );
  return(CH);
}

/****************************************************************************
*
*    FUNCTION: RunApp
*
*    PURPOSE: Starts a process to run the command line specified
*
*    COMMENTS:
*
*
****************************************************************************/
DWORD RunApp(LPSTR input, LPSTR * output)
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 
		                      NULL,   // NULL security descriptor
							  TRUE};  // Inherit handles (necessary!)
	HANDLE hReadHandle, hWriteHandle, hErrorHandle;
	LPSTR outputbuffer, lpOutput;
	DWORD AvailableOutput;
	BOOL TimeoutNotReached = TRUE;
	DWORD BytesRead;
	OVERLAPPED PipeOverlapInfo = {0,0,0,0,0};

	// Create the heap if it doesn't already exist
	if (hHeapHandle == 0)
	{
		if ((hHeapHandle = HeapCreate(0,
			                     8192,
								 0)) == NULL) return 0;
	}
	
	// Create buffer to receive stdout output from our process
	if ((outputbuffer = HeapAlloc(hHeapHandle,
		                     HEAP_ZERO_MEMORY,
							 4096)) == NULL) return 0;
	*output = outputbuffer;

	// Check input parameter
	if (input == NULL)
	{
		lstrcpy(outputbuffer, "RUNAPP ERROR: No command line specified");
		return lstrlen(outputbuffer);
	}

	// Zero init process startup struct
	FillMemory(&StartupInfo, sizeof(StartupInfo), 0);

	StartupInfo.cb = sizeof(StartupInfo);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;  // Use the our stdio handles
	
	// Create pipe that will xfer process' stdout to our buffer
	if (!CreatePipe(&hReadHandle,
		           &hWriteHandle,
				   &sa,
				   0)) 
	{
		lstrcpy(outputbuffer, "RUNAPP ERROR: Could not redirect STDIO");
		return lstrlen(outputbuffer);
	}
	// Set process' stdout to our pipe
	StartupInfo.hStdOutput = hWriteHandle;
	
	// We are going to duplicate our pipe's write handle
	// and pass it as stderr to create process.  The idea
	// is that some processes have been known to close
	// stderr which would also close stdout if we passed
	// the same handle.  Therefore we make a copy of stdout's
	// pipe handle.
	if (!DuplicateHandle(GetCurrentProcess(),
		            hWriteHandle,
					GetCurrentProcess(),
					&hErrorHandle,
					0,
					TRUE,
					DUPLICATE_SAME_ACCESS))
	{
		lstrcpy(outputbuffer,"RUNAPP ERROR: Could not duplicate STDIO handle");
		return lstrlen(outputbuffer);
	}
	StartupInfo.hStdError = hErrorHandle;

	// Initialize our OVERLAPPED structure for our I/O pipe reads
	PipeOverlapInfo.hEvent = CreateEvent(NULL,
		                                 TRUE,
										 FALSE,
										 NULL);
	if (PipeOverlapInfo.hEvent == NULL)
	{
		lstrcpy(outputbuffer, "RUNAPP ERROR: Could not create I/O Event");
		return lstrlen(outputbuffer);
	}

	// Create the Process!
	if (!CreateProcess(NULL,				 // name included in command line
		              input,				 // Command Line
					  NULL,					 // Default Process Sec. Attribs
					  NULL,					 // Default Thread Sec. Attribs
					  TRUE,					 // Inherit stdio handles
					  NORMAL_PRIORITY_CLASS, // Creation Flags
					  NULL,					 // Use this process' environment
					  NULL,					 // Use the current directory
					  &StartupInfo,
					  &ProcessInfo))
	{
		lstrcpy(outputbuffer, "RUNAPP ERROR: Could not create process");
		return lstrlen(outputbuffer);
	}

	// lpOutput is moving output pointer
	lpOutput = outputbuffer;
	AvailableOutput = HeapSize(hHeapHandle,
		                       0,
							   outputbuffer);
	// Close the write end of our pipe (both copies)
	// so it will die when the child process terminates
	CloseHandle(hWriteHandle);
	CloseHandle(hErrorHandle);

	while (TimeoutNotReached)
	{
		// Keep a read posted on the output pipe
		if (ReadFile(hReadHandle,
				lpOutput,
				AvailableOutput,
				&BytesRead,
				&PipeOverlapInfo) == TRUE)
		{
			// Already received data...adjust buffer pointers
			AvailableOutput-=BytesRead;
			lpOutput += BytesRead;
			if (AvailableOutput == 0)
			{
				// We used all our buffer,  allocate more
				LPSTR TempBufPtr = HeapReAlloc(hHeapHandle,
						                 HEAP_ZERO_MEMORY,
										 outputbuffer,
										 HeapSize(hHeapHandle,
										          0,
												  outputbuffer) + 4096);

				if (TempBufPtr == NULL)
				{
					// Copy error message to end of buffer
					lstrcpy(outputbuffer 
						    + HeapSize(hHeapHandle,0, outputbuffer) 
							- lstrlen(MEMERROR) - 1, 
							MEMERROR);
					return lstrlen(outputbuffer);
				}
				// Fix pointers in case ouir buffer moved
				outputbuffer = TempBufPtr;
				lpOutput = outputbuffer + BytesRead;
				AvailableOutput = HeapSize(hHeapHandle, 0, outputbuffer) - BytesRead;
				*output = outputbuffer;
			}
		}
		else
		{
			// Switch on ReadFile result
			switch (GetLastError())
			{
			case ERROR_IO_PENDING:
				// No data yet, set event so we will be triggered
				// when there is data
				ResetEvent(PipeOverlapInfo.hEvent);
				break;
			case ERROR_MORE_DATA:
				{
					// Our buffer is too small...grow it
					DWORD CurrentBufferOffset = lpOutput 
						                        - outputbuffer 
												+ BytesRead;

					LPSTR TempBufPtr = HeapReAlloc(hHeapHandle,
						                     HEAP_ZERO_MEMORY,
											 outputbuffer,
											 4096);

					if (TempBufPtr == NULL)
					{
						// Copy error message to end of buffer
						lstrcpy(outputbuffer + HeapSize
						       (hHeapHandle,0, outputbuffer) - 
						       lstrlen(MEMERROR) - 1, MEMERROR);
						return lstrlen(outputbuffer);
					}
					// Set parameters to post a new ReadFile
					lpOutput = outputbuffer + CurrentBufferOffset;
					AvailableOutput = HeapSize(hHeapHandle, 0, outputbuffer) 
						              - CurrentBufferOffset;
					*output = outputbuffer;
				}
				break;
			case ERROR_BROKEN_PIPE:
				// We are done..

				//Make sure we are null terminated
				*lpOutput = 0;
				return (lpOutput - outputbuffer);
				break;
			case ERROR_INVALID_USER_BUFFER:
			case ERROR_NOT_ENOUGH_MEMORY:
				// Too many I/O requests pending...wait a little while
				Sleep(2000);
				break;
			default:
				// Wierd error...return
				lstrcpy(outputbuffer, "RUNAPP ERROR: Error reading STDIO");
				return lstrlen(outputbuffer);
			}
		}

		// Wait for data to read
		if (WaitForSingleObject(PipeOverlapInfo.hEvent, 
			                    INACTIVETIMEOUT) == WAIT_TIMEOUT) 
			TimeoutNotReached = FALSE;
	}
}


/****************************************************************************
*
*    FUNCTION: FreeOutBuffer
*
*    PURPOSE: Frees memory allocated by the RunApp function
*
*    COMMENTS:
*
*
****************************************************************************/
BOOL FreeOutBuffer(LPSTR buffer)
{
	return HeapFree(hHeapHandle, 0, buffer);
}


																					
CHAR * ReadRegistry ( BOOL * bFlag)
{
    // ReadRegistry  will read the registry and return string with
    // commands to run. This string is used when list box is drawn 
    // on the Web page.
    // bFlag will be to  TRUE for admin (1 in a registry) otherwise FALSE.
	HKEY pPntr, pPntr1;
	DWORD dType, dType1,   dSize = 1, dSize1 = sizeof (DWORD);
	CHAR * lpszValue = NULL;
	DWORD  pTemp;

	*bFlag = TRUE;

    // Open key to check mode: admin or user.
	if ( RegOpenKeyEx (HKEY_LOCAL_MACHINE, PARAMETRS, 0, 
       KEY_READ, &pPntr1) != ERROR_SUCCESS)
	   return NULL;

	if ( RegQueryValueEx ( pPntr1, MODE, NULL, &dType1, (BYTE *) &pTemp,
       &dSize1) != ERROR_SUCCESS)
	    return NULL;
	else
	{
		switch (pTemp)
		{
		case 1:
			*bFlag = TRUE;
		break;
		case 0:
			*bFlag = FALSE;
		break;
		default:
			return NULL;
		}
    }


	if ( RegOpenKeyEx (HKEY_LOCAL_MACHINE, LOCATION, 0, KEY_READ,
        &pPntr) != ERROR_SUCCESS)
		return NULL;

	
    // Determine size of the string in the registry.
	if ( RegQueryValueEx ( pPntr, POSITION, NULL, &dType, NULL, &dSize) 
        != ERROR_SUCCESS)
	    return NULL;

	if (! ( lpszValue = (CHAR *) LocalAlloc (LPTR, dSize) ) )
		return NULL;


	if ( RegQueryValueEx ( pPntr, POSITION, NULL, &dType, (BYTE *) lpszValue, 
        &dSize) != ERROR_SUCCESS)
		return NULL;
	else
		return lpszValue;
}
