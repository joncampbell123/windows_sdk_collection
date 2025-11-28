/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       lobby.c
 *  Content:    DirectPlayLobby related code
 *
 *
 ***************************************************************************/
#include "duel.h"
#include "lobby.h"

/*
 * Definitions
 */
#define TIMERID			1			// timer ID to use
#define TIMERINTERVAL	250			// timer interval

/*
 * Externals
 */
extern LPDIRECTPLAY4				glpDP;			 		// directplay object pointer
extern BOOL							gbIsHost;				// Flag indicating if we are hosting
extern HWND							ghWndMain;				// Main application window handle
extern HINSTANCE					ghinst;					// Application instance handle		

/*
 * Globals
 */
LPDPLCONNECTION						glpdplConnection;		// connection settings

/*
 * Statics
 */
static LPDIRECTPLAYLOBBY3			glpDPL;					// lobby object pointer
static BOOL							gbLobbyMsgSupported;	// lobby messages are supported
static GUID							gguidPlayer;			// this player in this session


/*
 * Local Prototypes
 */
BOOL CALLBACK WaitDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
 * DPLobbyCreate
 *
 * Create the global lobby interface.
 */
HRESULT DPLobbyCreate(void)
{
	// Let's do a weak assertion that the Lobby Interface should not exist
	if (glpDPL)
		TRACE(_T("DPLobbyCreate - Lobby Interface already exists\n"));

	return CoCreateInstance(&CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER,
#ifdef UNICODE
							&IID_IDirectPlayLobby3, (LPVOID *) &glpDPL);
#else
							&IID_IDirectPlayLobby3A, (LPVOID *) &glpDPL);
#endif
}

/*
 * DoLobbyConnect
 *
 * Do ConnectEx and set up the global interface ptr.
 */
HRESULT DoLobbyConnect(DWORD dwFlags)
{
#ifdef UNICODE
	return IDirectPlayLobby_ConnectEx(glpDPL, dwFlags, &IID_IDirectPlay4,
									  &glpDP, NULL);
#else
	return IDirectPlayLobby_ConnectEx(glpDPL, dwFlags, &IID_IDirectPlay4A,
									  &glpDP, NULL);
#endif
}

/*
 * DlgProc for the status dialog
 */
BOOL CALLBACK StatusDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static UINT				idTimer;
	static HRESULT *		lphr;
	static BOOL				bInsideConnect;
	HRESULT					hr;


    switch(msg)
    {
        case WM_INITDIALOG:
			// Save the hresult pointer
			lphr = (HRESULT *)lParam;
			
			bInsideConnect = FALSE;
			// set a timer to refresh the session list
			idTimer = SetTimer(hwnd, TIMERID, TIMERINTERVAL, NULL);
            break;

        case WM_DESTROY:
			if (idTimer)
			{
				KillTimer(hwnd, idTimer); 
				idTimer = 0;
			}
            break;

		case WM_TIMER:
			// Make sure we don't re-enter Connect
			if (bInsideConnect)
				break;

			// Call Connect
			bInsideConnect = TRUE;
			hr = DoLobbyConnect(DPCONNECT_RETURNSTATUS);
			bInsideConnect = FALSE;

			switch (hr)
			{
				case DPERR_CONNECTING:
					break;

				case DP_OK:
				default:
					*lphr = hr;
					EndDialog(hwnd, TRUE);
			}
			break;
        
		case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_LOBBYCONNECTCANCEL:
                case IDCANCEL:
					// the async Connect will be stopped when the
					// IDirectPlayLobby3 interface is released.
					
					// Set the hresult to user cancel and close the dialog
					*lphr = DPERR_USERCANCEL;
					EndDialog(hwnd, TRUE);
                    break;
            }

            break;
    }

    // Allow for default processing
    return FALSE;
}

/*
 * Displays connection status to the user
 */
void ShowConnectStatus(HRESULT * lphr)
{
    DialogBoxParam(ghinst, MAKEINTRESOURCE(IDD_CONNECT_STATUS), ghWndMain,
			StatusDlgProc, (LPARAM)lphr);
}

/*
 * DPLobbyConnect
 *
 * Wrapper for DirectPlayLobby Connect API.
 */
HRESULT DPLobbyConnect(void)
{
	HRESULT hr;

	// If we're hosting, just call Connect, no async.  Otherwise, display
	// the connecting dialog and do the connect asynchronously.
    if(gbIsHost)
	{
		// Call Connect to see if we've gotten connected
		hr = DoLobbyConnect(0);
	}
	else
	{
		ShowConnectStatus(&hr);
	}

	return hr;
}

/*
 * DPLobbyGetConnectionSettings
 *
 * Wrapper for DirectPlayLobby GetConnectionSettings API
 */
HRESULT DPLobbyGetConnectionSettings(void)
{
	HRESULT hr=E_FAIL;
	DWORD dwSize;

	if (glpDPL)
	{
		// get size for the connection settings structure
		hr = IDirectPlayLobby_GetConnectionSettings(glpDPL, 0, NULL, &dwSize);
		if (DPERR_BUFFERTOOSMALL == hr)
		{ 
			// if we already have one, free it
			if (glpdplConnection)
			{
				free(glpdplConnection);
				glpdplConnection = NULL;
			}

			// allocate memory for the new one
			glpdplConnection = (LPDPLCONNECTION) malloc(dwSize);

			// get the connection settings
			if (glpdplConnection)
				hr = IDirectPlayLobby_GetConnectionSettings(glpDPL, 0, glpdplConnection, &dwSize);
		}
	}

	return hr;
}

/*
 * DPLobbyRelease
 *
 * Wrapper for DirectPlayLobby Release API
 */
HRESULT DPLobbyRelease(void)
{
	// free our connection settings
	if (glpdplConnection)
	{
		free(glpdplConnection);
		glpdplConnection = NULL;
	}

	// release the lobby object
	if (glpDPL)
	{
		IDirectPlayLobby_Release(glpDPL);
		glpDPL = NULL;
	}

	return NOERROR;
}

/*
 * DPLobbySetConnectionSettings
 *
 * Wrapper for DirectPlayLobby SetConnectionSettings API
 */
HRESULT DPLobbySetConnectionSettings(void)
{
	HRESULT hr=E_FAIL;

    hr = IDirectPlayLobby_SetConnectionSettings(glpDPL, 0, 0, glpdplConnection);

	return hr;
}

/*
 * DPLobbyWaitForConnectionSettings
 *
 * Wrapper for DirectPlayLobby WaitForConnectionSettings API
 */
HRESULT DPLobbyWaitForConnectionSettings(BOOL bWait)
{
	HRESULT hr = E_FAIL;
	DWORD dwFlags = bWait ? 0 : DPLWAIT_CANCEL;	// init or cancel wait

	hr = IDirectPlayLobby_WaitForConnectionSettings(glpDPL, dwFlags);

	return hr;
}

/******************************************************************************
 * DPLobbyWait
 *
 * Enter WaitForConnectionSettings mode and monitor with a Cancel dialog
 */
BOOL DPLobbyWait(void)
{
	HRESULT hr;

	// create a lobby object
	hr = DPLobbyCreate();
	if (FAILED(hr))
	{
		ShowError(IDS_DPLOBBY_ERROR_C);
		return FALSE;
	}

	// Enter Wait mode
	DPLobbyWaitForConnectionSettings(TRUE);

	// Launch Cancel Dialog
	DialogBoxParam(ghinst, MAKEINTRESOURCE(IDD_WAIT_FOR_LOBBY), ghWndMain,
				   WaitDlgProc, (LPARAM)&hr);
	if (FAILED(hr))
	{	// User Canceled
		DPLobbyWaitForConnectionSettings(FALSE);
		DPLobbyRelease();
		return FALSE;
	}

	// get connection settings from the lobby (into glpdplConnection)
	hr = DPLobbyGetConnectionSettings();
	if (FAILED(hr))
	{
		ShowError(IDS_DPLOBBY_ERROR_GCS);
		DPLobbyRelease();
		return FALSE;
	}
	
	// Lobby object is left active
	return TRUE;
}

BOOL CALLBACK WaitDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static UINT 		idTimer;
	static HRESULT *	lphr;

	switch(msg)
	{
		case WM_INITDIALOG:
			// Save the hresult pointer
			lphr = (HRESULT *)lParam;
			
			// set a timer to check for lobby messages
			idTimer = SetTimer(hwnd, TIMERID, TIMERINTERVAL, NULL);
			return TRUE;

		case WM_DESTROY:
			if (idTimer)
			{
				KillTimer(hwnd, idTimer); 
				idTimer = 0;
			}
			break;

		case WM_TIMER:
			// Check for ConnectionSettings msg
			if (SUCCEEDED(LobbyMessageReceive(LMR_CONNECTIONSETTINGS)))
			{
				*lphr = DP_OK;
				EndDialog(hwnd, TRUE);
			}
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_LOBBYWAITCANCEL:
				case IDCANCEL:
					*lphr = DPERR_USERCANCEL;
					EndDialog(hwnd, TRUE);
					return TRUE;
			}

			break;
	}

	// Allow for default processing
	return FALSE;
}

/******************************************************************************
 * DoingLobbyMessages
 *
 * Return TRUE if connected to a lobby and messages are supported.
 */
BOOL DoingLobbyMessages(void)
{
	return (glpDPL && gbLobbyMsgSupported);
}

/******************************************************************************
 * LobbyMessageInit
 *
 * Initialize lobby message processing.  Must be done while a lobby connection
 * is open.
 */
HRESULT LobbyMessageInit(void)
{
	HRESULT hr;
	DPLMSG_GETPROPERTY msgGetProp;

	if (!glpDPL)						// not connected to a lobby
		return E_FAIL;

	gbLobbyMsgSupported = FALSE;		// until we find out otherwise.
	
	// Find out if lobby messages are supported by the lobby
	ZeroMemory(&msgGetProp, sizeof(DPLMSG_GETPROPERTY));
	msgGetProp.dwType = DPLSYS_GETPROPERTY;
	msgGetProp.dwRequestID = 1;					// guidPlayer is not used
	msgGetProp.guidPropertyTag = DPLPROPERTY_MessagesSupported;
	hr = IDirectPlayLobby_SendLobbyMessage(glpDPL, DPLMSG_STANDARD, 0,
									&msgGetProp, sizeof(DPLMSG_GETPROPERTY));
	return hr;
}

/******************************************************************************
 * LobbyMessageReceive
 *
 * Check for any lobby messages and handle them
 *
 * There are two modes:
 *		LMR_CONNECTIONSETTINGS - Waiting only for settings from a lobby
 *		LMR_PROPERTIES - Handle property message responses
 */
HRESULT LobbyMessageReceive(DWORD dwMode)
{
	HRESULT		hr = NOERROR;
	LPVOID		lpMsgBuffer = NULL;
	DWORD		dwBufferSize = 0, dwRequiredSize;
	DWORD		dwMsgFlags;

	if (!glpDPL)
		return DPERR_NOINTERFACE;		// no lobby interface

	while (SUCCEEDED(hr))	// get all queued messages
	{
		dwRequiredSize = dwBufferSize;
		hr = IDirectPlayLobby_ReceiveLobbyMessage(glpDPL, 0, 0, &dwMsgFlags,
												  lpMsgBuffer, &dwRequiredSize);
		if (hr == DPERR_BUFFERTOOSMALL)		// alloc msg buffer and try again
		{
			if (!lpMsgBuffer)
				lpMsgBuffer = GlobalAllocPtr(GHND, dwRequiredSize);
			else
				lpMsgBuffer = GlobalReAllocPtr(lpMsgBuffer, dwRequiredSize, 0);
			if (!lpMsgBuffer) return (DPERR_NOMEMORY);
			dwBufferSize = dwRequiredSize;
			hr = NOERROR;
		}
		else if (SUCCEEDED(hr) && dwRequiredSize >= sizeof(DPLMSG_GENERIC))
		{	// decode the message

			// Are we just looking for the CONNECTIONSETTINGS msg?
			if (dwMode == LMR_CONNECTIONSETTINGS)
			{
				if ((dwMsgFlags & DPLMSG_SYSTEM) &&
					((LPDPLMSG_GENERIC)lpMsgBuffer)->dwType == 
												DPLSYS_NEWCONNECTIONSETTINGS)
					break;
				else
					TRACE(_T("Non CONNECTIONSETTINGS lobby message ignored\n"));
			}

			// otherwise we handle only GetProperty responses
			else if ((dwMsgFlags & DPLMSG_STANDARD) && 
				((LPDPLMSG_GENERIC)lpMsgBuffer)->dwType == 
													DPLSYS_GETPROPERTYRESPONSE)
			{
				LPDPLMSG_GETPROPERTYRESPONSE lpMsgGPR =
									(LPDPLMSG_GETPROPERTYRESPONSE)lpMsgBuffer;
				if (IsEqualGUID(&lpMsgGPR->guidPropertyTag,
								&DPLPROPERTY_MessagesSupported))
				{
					if ((BOOL)lpMsgGPR->dwPropertyData[0])	// supported
					{
						// so request our player instance guid
						DPLMSG_GETPROPERTY msgGetProp;
						ZeroMemory(&msgGetProp, sizeof(DPLMSG_GETPROPERTY));
						msgGetProp.dwType = DPLSYS_GETPROPERTY;
						msgGetProp.dwRequestID = 2;
						// guidPlayer is left NULL
						msgGetProp.guidPropertyTag = DPLPROPERTY_PlayerGuid;
						hr = IDirectPlayLobby_SendLobbyMessage(glpDPL,
												DPLMSG_STANDARD, 0, &msgGetProp,
												sizeof(DPLMSG_GETPROPERTY));
						hr = NOERROR;	// keep fetching messages
					}
					else	// not supported so close up shop
					{
						TRACE(_T("Lobby Messages not supported\n"));
						DPLobbyRelease();
						break;
					}
				}
				else if (IsEqualGUID(&lpMsgGPR->guidPropertyTag,
									 &DPLPROPERTY_PlayerGuid))
				{	// Have our player guid, ready to send property msgs
					gguidPlayer = ((LPDPLDATA_PLAYERGUID)
										&lpMsgGPR->dwPropertyData)->guidPlayer;
					gbLobbyMsgSupported = TRUE;
				}
			}
			else
				TRACE(_T("Unrecognized lobby message ignored\n"));
		}
	}	// msg loop

	if (lpMsgBuffer)
		GlobalFreePtr(lpMsgBuffer);
    return hr;
}	// LobbyMessageReceive

/******************************************************************************
 * LobbyMessageSetProperty
 *
 * Send a SetProperty message
 */
HRESULT LobbyMessageSetProperty(const GUID * lpguidPropTag, LPVOID lpData,
								DWORD dwDataSize)
{
	HRESULT hr;
	LPBYTE lpBuffer;
	LPDPLMSG_SETPROPERTY lpMsgSP;
	DWORD dwMsgSize;

	if (!glpDPL)
		return DPERR_NOCONNECTION;		// not connected to a lobby
	if (!gbLobbyMsgSupported)
		return DPERR_UNAVAILABLE;

	// allocate and pack up the message
	// Property data starts at dwPropertyData[0] for the size calculation
	dwMsgSize = sizeof(DPLMSG_SETPROPERTY) - sizeof(DWORD) + dwDataSize;
	lpBuffer = (LPBYTE)GlobalAllocPtr(GHND, dwMsgSize);
	if (!lpBuffer) return (DPERR_NOMEMORY);
	lpMsgSP = (LPDPLMSG_SETPROPERTY)lpBuffer;
	lpMsgSP->dwType = DPLSYS_SETPROPERTY;
	lpMsgSP->dwRequestID = DPL_NOCONFIRMATION;
	lpMsgSP->guidPlayer = gguidPlayer;			// player property assumed
	lpMsgSP->guidPropertyTag = *lpguidPropTag;
	lpMsgSP->dwDataSize = dwDataSize;
	memcpy(lpMsgSP->dwPropertyData, lpData, dwDataSize);
	hr = IDirectPlayLobby_SendLobbyMessage(glpDPL, DPLMSG_STANDARD, 0,
											 lpBuffer, dwMsgSize);
	GlobalFreePtr(lpBuffer);
	return hr;
}


