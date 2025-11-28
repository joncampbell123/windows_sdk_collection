/*==========================================================================
 *
 *  Copyright (C) 1996-1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       lobby.cpp
 *  Content:	Uses information from the lobby to establish a connection.
 *
 ***************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <dplobby.h>

#include "dpslots.h"

HRESULT ConnectUsingLobby(LPDPLAYINFO lpDPInfo)
{
	LPDIRECTPLAY4A		lpDirectPlay4A = NULL;
	LPDIRECTPLAYLOBBY3A	lpDPlayLobby3A = NULL;
	LPDPLCONNECTION		lpConnectionSettings = NULL;
	DPID				dpidPlayer;
	DWORD				dwSize;
	BOOL				bIsHost;
	DWORD				dwPlayerFlags;
	HRESULT				hr;

	// get an ANSI DirectPlay lobby interface
	hr = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, 
						  IID_IDirectPlayLobby3A, (LPVOID*)&lpDPlayLobby3A);
	if FAILED(hr)
		goto FAILURE;

    // get connection settings from the lobby
	// if this routine returns DPERR_NOTLOBBIED, then a lobby did not
	// launch this application and the user needs to configure the connection.

	// pass in a NULL pointer to just get the size of the connection setttings
	hr = lpDPlayLobby3A->GetConnectionSettings(0, NULL, &dwSize);
	if (DPERR_BUFFERTOOSMALL != hr)
		goto FAILURE;

	// allocate memory for the connection setttings
	lpConnectionSettings = (LPDPLCONNECTION) GlobalAllocPtr(GHND, dwSize);
	if (NULL == lpConnectionSettings)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// get the connection settings
	hr = lpDPlayLobby3A->GetConnectionSettings(0, lpConnectionSettings, &dwSize);
	if FAILED(hr)
		goto FAILURE;

	// see if we are the host or not
	if (lpConnectionSettings->dwFlags & DPLCONNECTION_CREATESESSION)
	{
		lpConnectionSettings->lpSessionDesc->dwFlags = NONSECURESESSIONFLAGS;
		bIsHost = TRUE;
	}
	else
	{
		bIsHost = FALSE;
	}

    // store the updated connection settings
    hr = lpDPlayLobby3A->SetConnectionSettings(0, 0, lpConnectionSettings);
	if FAILED(hr)
		goto FAILURE;

	// connect to the session - getting an ANSI IDirectPlay4A interface
	hr = lpDPlayLobby3A->ConnectEx(0, IID_IDirectPlay4A,
								   (LPVOID *)&lpDirectPlay4A, NULL);
	if FAILED(hr)
		goto FAILURE;

	// if we are hosting then we need to create a server player
	if (bIsHost)
		dwPlayerFlags = SERVERPLAYERFLAGS;
	else
		dwPlayerFlags = CLIENTPLAYERFLAGS;

	// create a player with the name returned in the connection settings
	hr = lpDirectPlay4A->CreatePlayer(&dpidPlayer,
							lpConnectionSettings->lpPlayerName, 
							lpDPInfo->hPlayerEvent, NULL, 0, dwPlayerFlags);
	if FAILED(hr)
		goto FAILURE;

	// return connection info
	lpDPInfo->lpDirectPlay4A = lpDirectPlay4A;
	lpDPInfo->dpidPlayer = dpidPlayer;
	lpDPInfo->bIsHost = bIsHost;
	lpDPInfo->bIsSecure = FALSE;

	lpDirectPlay4A = NULL;	// set to NULL here so it won't release below

FAILURE:
	if (lpDirectPlay4A)
		lpDirectPlay4A->Release();

	if (lpDPlayLobby3A)
		lpDPlayLobby3A->Release();

	if (lpConnectionSettings)
		GlobalFreePtr(lpConnectionSettings);

	return (hr);
}

