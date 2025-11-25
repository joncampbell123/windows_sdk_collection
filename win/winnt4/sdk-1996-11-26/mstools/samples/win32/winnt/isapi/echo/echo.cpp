/*

  Echo.CPP
 
  Copyright (c)1996 Microsoft Corporation, All Right Reserved

  The Echo extension DLL uses Windows Sockets to pass
  HTML FORM parameter requests to a remote server. The
  server process is implemented as an NT service that accepts
  incoming requests and echoes them back to the DLL.

  The Echo can establish and maintain socket connections 
  on behalf of multiple clients. This "gateway-like" functionality
  is implemented using simple connection context manager. 
  The connection context manager generates an HTTP COOKIE, which is 
  used by the browser to uniquley "identify" itself to the DLL.
  Using COOKIEs clients create "virtual" connections with a remote
  echo server via IIS and extension DLL. Even though, COOKIE mechanism
  does not affect browser to IIS connection, with some extra coding
  Echo can be extended to use HTTP keep-alives.

  
  The COOKIE is created when a connection with a remote server
  is established for the first time and reused for all subsequent 
  client requests. This technique allows a browser to go through
  a series of HTML FORMS by maintaining single connection to a
  remote server. The COOKIE is expired and session with a remote 
  host is terminated if: 
	1.Browser explicitly submits disconnect request.
	2.Connection has been inactive for a specified period of time.

  In addition, the DLL includes the sample code that shows how
  to preprocess FORM GET and POST parameters into a C strcuture
  with a name and value pairs.
  
  
  Exports:

    BOOL WINAPI GetExtensionVersion( HSE_VERSION_INFO *pVer )
    BOOL WINAPI HttpExtensionProc(   EXTENSION_CONTROL_BLOCK *pECB )
    DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID pvReserved)

*/

#include <windows.h>
#include <memory.h>
#include <stdio.h>
#include "Echo.h"

// Adding and removing clients to and from the list
// has to be a threadsafe operation
CRITICAL_SECTION g_csClientList;


void DisconnectEchoServer(LPCONNCONTEXT lpConnContext)
/*++

Routine Description:

    This routine disconnects a socket and cleans up 
    client structures

Arguments:

	lpConnContext	- pointer to ConnContext of a currently connected client

Return Value:

    void

--*/

{
CHAR	szBuffer[BUFLEN];
DWORD	dwLen;

	closesocket(lpConnContext->Socket);

	// To delete a cookie return a cookie with the same name
	// and expires time which is in the past
	// I am pretty sure that June 7, 1964 is in the past, 
	// since it is my birthday.
	sprintf(szBuffer,"Content-Type: text/html\r\n"
		"Set-Cookie: %s=%d; expires=Sun, 07-06-64 01:00:00 GMT;"
			"\r\n\r\n"
            "<HEAD><TITLE>ISAPI Echo Demo</TITLE></HEAD>"
			"<BODY><H1><P ALIGN=CENTER>Disconnected from server</H1><P>"
			"<P>"
			"<HR>",
			COOKIE_NAME, (int)(lpConnContext));

    dwLen=lstrlen(szBuffer);
	lpConnContext->pEcb->ServerSupportFunction(lpConnContext->pEcb->ConnID,
                                 HSE_REQ_SEND_RESPONSE_HEADER,
                                 "200 OK",
                                 &dwLen,
                                 (LPDWORD) szBuffer );


	RemoveConnectionContext(lpConnContext);

	return;
}

BOOL CallEchoServer(LPCONNCONTEXT lpConnContext, LPPARAMSBLOCK lpParamsStart)
/*++

Routine Description:

	This routine echoes a FORM parameter block in Name=Value pairs
	to remote server and than sends it back to a client along with 
	HTTP cookie

Arguments:

	lpConnContext	- pointer to ConnContext of a currently connected client
	lpParamsStart	- pointer to FORM parameter block

Return Value:

    void

--*/

{
DWORD	dwLen;
CHAR	szBuffer[BUFLEN];

	sprintf(szBuffer,"Content-Type: text/html\r\n"
			"Set-Cookie: %s=%d"
			"\r\n\r\n"
            "<HEAD><TITLE>ISAPI Echo Demo</TITLE></HEAD>"
			"<BODY><H1><P ALIGN=CENTER>Echoed Form Variables</H1><P>"
			"<P>"
			"<HR>",
			COOKIE_NAME, (int)(lpConnContext));

    dwLen=lstrlen(szBuffer);
	lpConnContext->pEcb->ServerSupportFunction(lpConnContext->pEcb->ConnID,
                                 HSE_REQ_SEND_RESPONSE_HEADER,
                                 "200 OK",
                                 &dwLen,
                                 (LPDWORD) szBuffer );
	while(lpParamsStart) {
		wsprintf(szBuffer, "%s=%s<br>", lpParamsStart->Name, (lpParamsStart->Value) ? lpParamsStart->Value: "");
		// Send Name=Value pair to the server
		if (FALSE == WriteFile((HANDLE)lpConnContext->Socket, szBuffer, strlen(szBuffer), &dwLen, NULL))
			return FALSE;
		// Read data echoed by the server
		if (FALSE == ReadFile((HANDLE)lpConnContext->Socket, szBuffer, sizeof(szBuffer), &dwLen, NULL))
			return FALSE;
		// Finally send it to the browser...
		lpConnContext->pEcb->WriteClient(lpConnContext->pEcb->ConnID,
									  szBuffer,
									  &dwLen,
									  0);
		lpParamsStart = lpParamsStart->Next;
	}


	return TRUE;
}

void ErrorResponse(EXTENSION_CONTROL_BLOCK *pECB, LPSTR lpError)
/*++

Routine Description:

	This routine returns errors to browser
Arguments:

	lpConnContext	- pointer to ConnContext of a currently connected client
	lpError			- pointer to error message
Return Value:

    void

--*/

{
    CHAR pszBuf[BUFLEN];

    sprintf(pszBuf, "Content-Type: text/html\r\n\r\n"
	  "<BODY><H1>"
      "Echo %s"
	  "</H1></BODY>",
      lpError);
 
    pECB->ServerSupportFunction(pECB->ConnID,
                                HSE_REQ_SEND_RESPONSE_HEADER,
								(LPDWORD) "200 Bad Request",
								NULL,
								(LPDWORD)pszBuf);
}

BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
    pVer->dwExtensionVersion = MAKELONG( HSE_VERSION_MINOR, HSE_VERSION_MAJOR );

    strncpy(pVer->lpszExtensionDesc,
            "Echo Server Gateway",
            HSE_MAX_EXT_DLL_NAME_LEN);
    return TRUE;
}

DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
{
    LPSTR			lpHostName		= NULL;
    LPSTR			lpDisconnect	= NULL;
	LPCONNCONTEXT		lpConnContext;
	SOCKET			s;
	LPPARAMSBLOCK	lpParamsBlock	= NULL;
	DWORD			dwResult			= HSE_STATUS_SUCCESS;	


    // only GET and POST supported
    if (stricmp(pECB->lpszMethod, "GET") && stricmp(pECB->lpszMethod, "POST"))
    {
      dwResult = HSE_STATUS_ERROR;
	  goto End;
    }

	// Get FORM parameter names and values sent 
	// from the client 
	lpParamsBlock = CreateParametersBlock(pECB);
	if (lpParamsBlock) {
		// Check if disconnect was requested
		lpDisconnect = GetParameterValue(lpParamsBlock, "Disconnect");
		lpHostName   = GetParameterValue(lpParamsBlock, "HostName");
	}

	// Get client cookie
	lpConnContext = GetClientCookie(pECB);

	if (NULL == lpConnContext && NULL == lpDisconnect) {
		// If there is no cookie it is a new client
		// and we need to establish a new connection.
		// But first, we need to make sure that this   
		// not a disconnect request.
		if (TRUE == ConnectClient(lpHostName, &s)) {
			// Add a connection to a list of open connections
			lpConnContext = AddConnectionContext(pECB, s);
			if (NULL == lpConnContext) {
				closesocket(s);
				ErrorResponse(pECB, "Error: Echo.DLL is out of memory ");
				dwResult = HSE_STATUS_ERROR; 
				goto End;
			}

		}
		else {
			ErrorResponse(pECB, "Error: Could not open socket connection to a remote host");
			dwResult = HSE_STATUS_ERROR; 
			goto End;
		}

	}
	else if (NULL != lpConnContext && NULL != lpDisconnect) {
		// Disconnect a client and dealocate its context
		DisconnectEchoServer(lpConnContext);
		goto End;
	}
	else if (NULL == lpConnContext && NULL != lpDisconnect) {
		// Client has not been connected to a server yet
		ErrorResponse(pECB, "Error: Server is not connected");
		dwResult = HSE_STATUS_ERROR;
		goto End;
	}


	if (FALSE == CallEchoServer(lpConnContext, lpParamsBlock)) {
		ErrorResponse(pECB, "Error: Server is not responding");
		dwResult = HSE_STATUS_ERROR;
		goto End;
	}

End:
	ReleaseParametersBlock(lpParamsBlock);

    return dwResult;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD fReason, LPVOID pvRes)
{
HANDLE hTimerThreadHandle;
DWORD  dwThreadId;
/*
int		err;
WSADATA	WsaData;
*/
  switch (fReason) {
  case DLL_PROCESS_ATTACH:

    InitializeCriticalSection(&g_csClientList);
	// WSAStartup need not be called here
	// since IIS has already initialized WSA.
	// But if for some reason, i.e. debugging
	// independently of IIS, it needs to be 
	// called just uncomment Startup and Cleanup
	// code.
	// ZorG 1-17-96
	/*
    err = WSAStartup( 0x0101, &WsaData );
    if ( err == SOCKET_ERROR )
        return FALSE;
  	*/
	// Create Timer Thread
    hTimerThreadHandle = CreateThread(
							NULL,
							0,
							EchoTimerThread,
							(LPVOID)NULL,
							0,
							&dwThreadId
							);
    if ( hTimerThreadHandle == NULL )
            return FALSE;

    //
    // Close each thread handle as we open them.  We do not need
    // the thread handles.  Note that each thread will continue
    // executing.
    //

    CloseHandle( hTimerThreadHandle );

    break;

  case DLL_THREAD_ATTACH:
    break;

  case DLL_PROCESS_DETACH:
	/*
	WSACleanup();
	*/
    DeleteCriticalSection(&g_csClientList);
    break;

  case DLL_THREAD_DETACH:
    break;
  }
  return TRUE;
}

/*
	GetClientCookie			checks if HTTP_COOKIE is set
	Arguments				a pointer to extnesion control block
	Returns					a pointer to a client context if cookie
							is found or NULL otherwise
*/

LPCONNCONTEXT	GetClientCookie(EXTENSION_CONTROL_BLOCK *pEcb)
/*++

Routine Description:

    This routine checks if HTTP cookie is received from the client.
	It then verifies that it is a valid cookie name and return
	pointer to the connected client context. So Echo cookie is 
	just a pointer to client connection context.

Arguments:

    pEcb	- Extension control block  

	Return Value:

    BOOL - Cookie value, i.e pointer to ConnectionContext or 
		   NULL if there is no cookie defined for the session.

--*/

{
CHAR		szCookieNameValue[COOKIE_LENGTH];
DWORD		dwCookieLen = COOKIE_LENGTH;
LPSTR		lpCookieValue;
DWORD		dwCookie;
LPCONNCONTEXT	lpConnContext;

	if ( TRUE == pEcb->GetServerVariable(pEcb->ConnID,
							"HTTP_COOKIE",
							szCookieNameValue,
							&dwCookieLen)) {
			// Make sure that it is our cookie
			if (!strncmp(szCookieNameValue, COOKIE_NAME, strlen(COOKIE_NAME))) {
				// Get cookie value
				lpCookieValue = strchr(szCookieNameValue, '=');			
				if (lpCookieValue) {
					// Increment lpCookieValue, since cookie value follows the = sign
					lpCookieValue++;
					dwCookie = (DWORD)atoi(lpCookieValue);
					lpConnContext = (LPCONNCONTEXT)dwCookie;
					if (FindConnectionContext(lpConnContext, pEcb) == TRUE)
						return lpConnContext;
				}
			}

	}

	return NULL;
}


BOOL ConnectClient ( LPCTSTR lpHostName, SOCKET *s)

/*++

Routine Description:

    This routine sets up a connecting socket on the SERVER_PORT port, then
    starts a connect request.

Arguments:

    lpHostName  - the the host name for the EchoServer.  

	s			- pointer to the socket to be connected to a server
Return Value:

    BOOL - FALSE if there was an error in connecting the socket.

--*/

{

    SOCKADDR_IN		sin;
    int				err;
    int				zero;
    PHOSTENT		phe;
	CHAR			szHostName[256];
	int				optionValue = SO_SYNCHRONOUS_NONALERT;


	// Set socket handle to non overlapped. This call
	// needs to be executed on a per thread basis.
   	setsockopt( 
        INVALID_SOCKET, 
        SOL_SOCKET, 
        SO_OPENTYPE, 
        (char *)&optionValue, 
        sizeof(optionValue));

    *s = socket( AF_INET, SOCK_STREAM, 0 );
    if ( *s == INVALID_SOCKET ) {
        return FALSE;
    }


    sin.sin_family = AF_INET;
    sin.sin_port = htons( SERVER_PORT );

	if (lpHostName[0])
		strcpy(szHostName, lpHostName);
	else 
		gethostname(szHostName, sizeof(szHostName));

	phe = gethostbyname(szHostName);
	if (phe == NULL) {
        closesocket( *s );
		return FALSE;
	}
    memcpy((char FAR *)&(sin.sin_addr), phe->h_addr,
            phe->h_length);


    zero = 0;
    err = setsockopt( *s, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero) );
    if ( err == SOCKET_ERROR ) {
        closesocket( *s );
        return FALSE;
    }

    //
    // Connect the socket.
	//

	err = connect(*s, (LPSOCKADDR)&sin, sizeof(sin));
    if ( err == SOCKET_ERROR ) {
        closesocket( *s );
        return FALSE;
    }
	return TRUE;
}

DWORD WINAPI EchoTimerThread(LPVOID lpVoid)
{

GotoSleep:

	Sleep(CLEANUP_TIMEOUT);

	RemoveInactiveConnections();

	goto	GotoSleep;
	return 0;
}