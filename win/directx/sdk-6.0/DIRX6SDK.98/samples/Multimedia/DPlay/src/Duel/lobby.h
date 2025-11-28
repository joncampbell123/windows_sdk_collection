/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:		lobby.h
 *  Content:	lobby related routines include file
 *
 *
 ***************************************************************************/
#define IDIRECTPLAY2_OR_GREATER
#include <dplobby.h>

/*
 * LobbyMessageReceive Modes
 */
#define LMR_PROPERTIES			0
#define LMR_CONNECTIONSETTINGS	1

/*
 * Prototypes
 */

HRESULT DPLobbyCreate(void);
HRESULT DPLobbyConnect(void);
HRESULT DPLobbyGetConnectionSettings(void);
HRESULT DPLobbyRelease(void);
HRESULT DPLobbySetConnectionSettings(void);
HRESULT DPLobbyWaitForConnectionSettings(BOOL bWait);
BOOL	DPLobbyWait(void);

BOOL	DoingLobbyMessages(void);
HRESULT LobbyMessageInit(void);
HRESULT LobbyMessageReceive(DWORD dwMode);
HRESULT LobbyMessageSetProperty(const GUID * lpguidPropTag, LPVOID lpData,
								DWORD dwDataSize);

