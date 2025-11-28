 /*==========================================================================
 *
 *  Copyright (C) 1995-1998 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       comm.c
 *  Content:    DirectPlay related code
 *
 *
 ***************************************************************************/
#include "duel.h"
#include "comm.h"
#include "lobby.h"

/*
 * Externals
 */
extern LPGUID			glpGuid;			// duel's guid
extern LPDPLCONNECTION	glpdplConnection;	// connection settings
extern BOOL				gbUseProtocol;		// DirectPlay Protocol messaging
extern BOOL				gbAsyncSupported;	// asynchronous sends supported

/*
 * Globals
 */
LPDPSESSIONDESC2		glpdpSD;			// current session description
LPDIRECTPLAY4			glpDP	= NULL;		// directplay object pointer

/*
 * CheckCaps
 *
 * Helper function to check for certain Capabilities
 */
void CheckCaps(void)
{
	HRESULT hr;
	DPCAPS caps;

	if (!glpDP)
		return;

	ZeroMemory(&caps, sizeof(DPCAPS));
	caps.dwSize = sizeof(DPCAPS);
	// The caps we are checking do not differ for guaranteed msg
	hr = IDirectPlayX_GetCaps(glpDP, &caps, 0);
	if (FAILED(hr))
		return;

	// Determine if Aync messages are supported.
	gbAsyncSupported = (caps.dwFlags & DPCAPS_ASYNCSUPPORTED) != 0;

	// Diagnostic traces of caps supported
	if (gbAsyncSupported)
	{
		TRACE(_T("Capabilities supported: Async %s %s %s\n"),
				 (caps.dwFlags & DPCAPS_SENDPRIORITYSUPPORTED ? _T("SendPriority") : _T("")),
				 (caps.dwFlags & DPCAPS_SENDTIMEOUTSUPPORTED ? _T("SendTimeout") : _T("")),
				 (caps.dwFlags & DPCAPS_ASYNCCANCELSUPPORTED
					? _T("AsyncCancel") 
					: (caps.dwFlags & DPCAPS_ASYNCCANCELALLSUPPORTED
						? _T("AsyncCancelAll") : _T("")))
				);
	}
	else
		TRACE(_T("CheckCaps - Async not supported\n"));
}

/*
 * DPlayClose
 *
 * Wrapper for DirectPlay Close API
 */
HRESULT DPlayClose(void)
{
	HRESULT hr=E_FAIL;

	if (glpDP) 
		hr = IDirectPlayX_Close(glpDP);
	
	return hr;
}

/*
 * DPlayCreate
 *
 * Wrapper for DirectPlay Create API.
 * Retrieves a DirectPlay4/DirectPlay4A interface based on the UNICODE flag
 * 
 */
HRESULT DPlayCreate(LPVOID lpCon)
{
	HRESULT hr=E_FAIL;

	// create a DirectPlay4(A) interface
	hr = CoCreateInstance(&CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
#ifdef UNICODE
						  &IID_IDirectPlay4, (LPVOID *) &glpDP);
#else
						  &IID_IDirectPlay4A, (LPVOID *) &glpDP);
#endif
	if (FAILED(hr))
		return (hr);

	// initialize w/address
	if (lpCon)
	{
		hr = IDirectPlayX_InitializeConnection(glpDP, lpCon, 0);
		if (FAILED(hr))
			goto FAILURE;
	}

	return hr;

FAILURE:
	IDirectPlayX_Release(glpDP);
	glpDP = NULL;

	return hr;
}

/*
 * DPlayCreatePlayer
 *
 * Wrapper for DirectPlay CreatePlayer API. 
 */

HRESULT DPlayCreatePlayer(LPDPID lppidID, LPTSTR lptszPlayerName, HANDLE hEvent, 
						  LPVOID lpData, DWORD dwDataSize)
{
	HRESULT hr=E_FAIL;
	DPNAME name;
	
	ZeroMemory(&name,sizeof(name));
	name.dwSize = sizeof(DPNAME);

#ifdef UNICODE
	name.lpszShortName = lptszPlayerName;
#else
	name.lpszShortNameA = lptszPlayerName;
#endif

	if (glpDP)
		hr = IDirectPlayX_CreatePlayer(glpDP, lppidID, &name, hEvent, lpData, 
									  dwDataSize, 0);
									
	return hr;
}

/*
 * DPlayCreateSession
 *
 * Wrapper for DirectPlay CreateSession API.Uses the global application guid (glpGuid).
 */
HRESULT DPlayCreateSession(LPTSTR lptszSessionName)
{
	HRESULT hr = E_FAIL;
	DPSESSIONDESC2 dpDesc;

	if (!glpDP)
		return DPERR_NOINTERFACE;

	ZeroMemory(&dpDesc, sizeof(dpDesc));
	dpDesc.dwSize = sizeof(dpDesc);
	dpDesc.dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
	if (gbUseProtocol)
		dpDesc.dwFlags |= DPSESSION_DIRECTPLAYPROTOCOL;

#ifdef UNICODE
	dpDesc.lpszSessionName = lptszSessionName;
#else
	dpDesc.lpszSessionNameA = lptszSessionName;
#endif

	// set the application guid
	if (glpGuid)
		dpDesc.guidApplication = *glpGuid;

	hr = IDirectPlayX_Open(glpDP, &dpDesc, DPOPEN_CREATE);

	// Check for Async message support
	if (SUCCEEDED(hr))
		CheckCaps();

	return hr;
}

/*
 * DPlayDestroyPlayer
 * 
 * Wrapper for DirectPlay DestroyPlayer API. 
 */
HRESULT DPlayDestroyPlayer(DPID pid)
{
	HRESULT hr=E_FAIL;
	
	if (glpDP)
		hr = IDirectPlayX_DestroyPlayer(glpDP, pid);

	return hr;
}

/*
 * DPlayEnumPlayers
 *
 * Wrapper for DirectPlay API EnumPlayers
 */
HRESULT DPlayEnumPlayers(LPGUID lpSessionGuid, LPDPENUMPLAYERSCALLBACK2 lpEnumCallback, 
						 LPVOID lpContext, DWORD dwFlags)
{
	HRESULT hr=E_FAIL;

	if (glpDP)
		hr = IDirectPlayX_EnumPlayers(glpDP, lpSessionGuid, lpEnumCallback, lpContext, dwFlags);

	return hr;
}

/*
 * DPlayEnumSessions
 *
 * Wrapper for DirectPlay EnumSessions API.
 */
HRESULT DPlayEnumSessions(DWORD dwTimeout, LPDPENUMSESSIONSCALLBACK2 lpEnumCallback, 
						  LPVOID lpContext, DWORD dwFlags)
{
	HRESULT hr = E_FAIL;
	DPSESSIONDESC2 dpDesc;

	ZeroMemory(&dpDesc, sizeof(dpDesc));
	dpDesc.dwSize = sizeof(dpDesc);
	if (glpGuid)
		dpDesc.guidApplication = *glpGuid;

	if (glpDP)
		hr = IDirectPlayX_EnumSessions(glpDP, &dpDesc, dwTimeout, lpEnumCallback,
										lpContext, dwFlags);


	return hr;
}

/*
 * DPlayGetPlayerData
 * 
 * Wrapper for DirectPlay GetPlayerData API.
 */
HRESULT DPlayGetPlayerData(DPID pid, LPVOID lpData, LPDWORD lpdwDataSize, DWORD dwFlags)
{
	HRESULT hr=E_FAIL;

	if (glpDP) 
		hr = IDirectPlayX_GetPlayerData(glpDP, pid, lpData, lpdwDataSize, dwFlags);

	return hr;
}

/*
 * DPlayGetSessionDesc
 *
 * Wrapper for DirectPlay GetSessionDesc API. 
 */
HRESULT DPlayGetSessionDesc(void)
{
	HRESULT hr=E_FAIL;
	DWORD dwSize;

	// free old session desc, if any
	if (glpdpSD)
	{
		free(glpdpSD);
		glpdpSD = NULL;
	}

	if (glpDP)
	{
		// first get the size for the session desc
		if ((hr = IDirectPlayX_GetSessionDesc(glpDP, NULL, &dwSize)) == DPERR_BUFFERTOOSMALL)
		{
			// allocate memory for it
			glpdpSD = (LPDPSESSIONDESC2) malloc(dwSize);
			if (glpdpSD)
			{
				// now get the session desc
				hr = IDirectPlayX_GetSessionDesc(glpDP, glpdpSD, &dwSize);
			}
			else
			{
				hr = E_OUTOFMEMORY;
			}
		}
	}

	return hr;
}

/*
 * IsDPlay
 *
 * Returns TRUE if a DirectPlay interface exists, otherwise FALSE.
 */
BOOL IsDPlay(void)
{
	return (glpDP ? TRUE:FALSE);
}

/*
 * DPlayOpenSession
 *
 * Wrapper for DirectPlay OpenSession API. 
 */
HRESULT DPlayOpenSession(LPGUID lpSessionGuid)
{
	HRESULT hr = E_FAIL;
	DPSESSIONDESC2 dpDesc;

	if (!glpDP)
		return DPERR_NOINTERFACE;

	ZeroMemory(&dpDesc, sizeof(dpDesc));
	dpDesc.dwSize = sizeof(dpDesc);
	if (gbUseProtocol)
		dpDesc.dwFlags = DPSESSION_DIRECTPLAYPROTOCOL;

	// set the session guid
	if (lpSessionGuid)
		dpDesc.guidInstance = *lpSessionGuid;
	// set the application guid
	if (glpGuid)
		dpDesc.guidApplication = *glpGuid;

	// open it
	hr = IDirectPlayX_Open(glpDP, &dpDesc, DPOPEN_JOIN);

	// Check for Async message support
	if (SUCCEEDED(hr))
		CheckCaps();

	return hr;
}


/*
 * DPlayReceive
 *
 * Wrapper for DirectPlay Receive API
 */
HRESULT DPlayReceive(LPDPID lpidFrom, LPDPID lpidTo, DWORD dwFlags, LPVOID lpData, LPDWORD lpdwDataSize)
{
	HRESULT hr = E_FAIL;

	if (glpDP)
		hr = IDirectPlayX_Receive(glpDP, lpidFrom, lpidTo, dwFlags, lpData, lpdwDataSize);
	
	return hr;
}

/*
 * DPlayRelease
 *
 * Wrapper for DirectPlay Release API.
 */
HRESULT DPlayRelease(void)
{
	HRESULT hr = E_FAIL;

	if (glpDP)
	{
		// free session desc, if any
		if (glpdpSD) 
		{
			free(glpdpSD);
			glpdpSD = NULL;
		}

		// free connection settings structure, if any (lobby stuff)
		if (glpdplConnection)
		{
			free(glpdplConnection);
			glpdplConnection = NULL;
		}
		// release dplay
		hr = IDirectPlayX_Release(glpDP);
		glpDP = NULL;
	}

	return hr;
}

/*
 * DPlaySend
 * 
 * Wrapper for DirectPlay Send[Ex] API.
 */
HRESULT DPlaySend(DPID idFrom, DPID idTo, DWORD dwFlags, LPVOID lpData, DWORD dwDataSize)
{
	HRESULT hr = DPERR_NOINTERFACE;

	if (glpDP)
	{
		if (dwFlags & DPSEND_ASYNC)
			// We don't specify a priority or timeout.  Would have to check
			// GetCaps() first to see if they were supported
			hr = IDirectPlayX_SendEx(glpDP, idFrom, idTo, dwFlags, lpData,
									 dwDataSize, 0, 0, NULL, NULL);
		else
			hr = IDirectPlayX_Send(glpDP, idFrom, idTo, dwFlags, lpData, dwDataSize);
	}

	return hr;
}

/*
 * DPlaySetPlayerData
 *
 * Wrapper for DirectPlay SetPlayerData API
 */
HRESULT DPlaySetPlayerData(DPID pid, LPVOID lpData, DWORD dwSize, DWORD dwFlags)
{
	HRESULT hr=E_FAIL;

	if (glpDP)
		hr = IDirectPlayX_SetPlayerData(glpDP, pid, lpData, dwSize, dwFlags);
	
	return hr;
}


