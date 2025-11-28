/*==========================================================================
 *
 *  Copyright (C) 1996-1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dplaunch.c
 *  Content:    Implementation of a DirectPlay launching utility
 *
 ***************************************************************************/

#define INITGUID

#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <wtypes.h>
#include <cguid.h>

#include "dplay.h"
#include "dplobby.h"

#include "resource.h"

#if defined(UNICODE) || defined(_UNICODE)
#error This app does not support UNICODE
#endif

// constants
#define NAMEMAX 			200 		// maximum size of a string name
#define ADDRESSTYPEMAX		10			// maximum no. address types
#define MAXSTRLEN			200			// maximum size of temp strings

// GUID for sessions this application creates
// {D559FC00-DC12-11cf-9C4E-00A0C905425E}
DEFINE_GUID(MY_SESSION_GUID, 
0xd559fc00, 0xdc12, 0x11cf, 0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

// structures

// list of address types
typedef struct {
	DWORD	dwCount;
	GUID	guidAddressTypes[ADDRESSTYPEMAX];
} ADDRESSTYPELIST, *LPADDRESSTYPELIST;

// prototypes
BOOL CALLBACK	LauncherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT 		InitializeLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBY3A *lplpDPlayLobby);
HRESULT 		UpdateAddressInfo(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby);
void			DestroyLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby);
void			LaunchDirectPlayApplication(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby);
HRESULT 		GetComboBoxGuid(HWND hWnd, LONG iDialogItem, LPGUID lpguidServiceProvider);
HRESULT 		FillModemComboBox(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby, LPGUID lpguidServiceProvider);
void			ErrorBox(LPSTR lpszErrorStr, HRESULT hr);
char *			GetDirectPlayErrStr(HRESULT hr);


// ---------------------------------------------------------------------------
// WinMain
// ---------------------------------------------------------------------------
// Description:             Main windows entry point.
// Arguments:
//  HINSTANCE               [in] Standard windows stuff
//  HINSTANCE               [in]
//  LPSTR                   [in]
//  int                     [in]
// Returns:
//  int
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow )
{
	int		iResult = 0;
	HRESULT	hr;

	// initialize the COM library
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorBox("CoInitialize failed. Error %s", hr);
		return 0;
	}

	iResult = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_LAUNCHERDIALOG),
							 NULL, LauncherWndProc, (LPARAM) hInstance);

	CoUninitialize();
	return iResult;
}


// ---------------------------------------------------------------------------
// LauncherWndProc
// ---------------------------------------------------------------------------
// Description: Message callback function for Launcher dialog.
// Arguments:
//  HWND        [in] Dialog window handle.
//  UINT        [in] Window message identifier.
//  WPARAM      [in] Depends on message.
//  LPARAM      [in] Depends on message.
// Returns:
//  BOOL        TRUE if message was processed internally.
BOOL CALLBACK LauncherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE				hInst;
	static LPDIRECTPLAYLOBBY3A		lpDPlayLobby;
	HRESULT 						hr;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		// Save the instance handle
		hInst = (HINSTANCE)lParam;
			
		// Initialize dialog with launcher information
		lpDPlayLobby = NULL;
		hr = InitializeLauncherWindow(hWnd, &lpDPlayLobby);
		if (FAILED(hr))
		{
			ErrorBox("Could not initialize. Error %s", hr);
			EndDialog(hWnd, FALSE);
		}
		return TRUE;

	case WM_DESTROY:
		// Destroy launcher information in dialog
		DestroyLauncherWindow(hWnd, lpDPlayLobby);
		break;	// continue with default handling

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_SPCOMBO:
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				// update the address info display
				UpdateAddressInfo(hWnd, lpDPlayLobby);
				return TRUE;
			}
			break;

		case IDC_RUNAPPBUTTON:
			// get settings and launch application
			LaunchDirectPlayApplication(hWnd, lpDPlayLobby);

			return TRUE;

		case IDCANCEL:
			// Return failure
			EndDialog(hWnd, TRUE);

			return TRUE;
		}

		break;
	}

	// Allow for default processing
	return FALSE;
}

// ---------------------------------------------------------------------------
// EnumApp
// ---------------------------------------------------------------------------
// Description:     Enumeration callback called by DirectPlay.
//                  Enumerates the applications registered with DirectPlay.
// Arguments:
//  LPDPLAPPINFO    [in] information about the application
//  LPVOID          [in] user-defined context
//  DWORD           [in] flags
// Returns:
//  BOOL            TRUE to continue enumerating
BOOL FAR PASCAL EnumApp(LPCDPLAPPINFO lpAppInfo, LPVOID lpContext, DWORD dwFlags)
{
	HWND		hWnd = lpContext;
	LRESULT 	iIndex;
	LPGUID		lpGuid;

	// store application name in combo box
	iIndex = SendDlgItemMessage(hWnd, IDC_APPCOMBO, CB_ADDSTRING, 0, (LPARAM) lpAppInfo->lpszAppNameA);
	if (iIndex == LB_ERR)
		goto Failure;

	// make space for application GUID
	lpGuid = (LPGUID) GlobalAllocPtr(GHND, sizeof(GUID));
	if (lpGuid == NULL)
		goto Failure;

	// store pointer to GUID in combo box
	*lpGuid = lpAppInfo->guidApplication;
	SendDlgItemMessage(hWnd, IDC_APPCOMBO, CB_SETITEMDATA, (WPARAM) iIndex, (LPARAM) lpGuid);

Failure:
	return (TRUE);
}

// ---------------------------------------------------------------------------
// EnumSP
// ---------------------------------------------------------------------------
// Description: Enumeration callback called by DirectPlay.
//              Enumerates service providers registered with DirectPlay.
// Arguments:
//  LPCGUID     [in] GUID of service provider
//  LPVOID      [in] connection data (a DirectPlay Address) (NOT USED)
//  DWORD       [in] size of connection data (NOT USED)
//  LPCDPNAME   [in] name of service provider
//  DWORD       [in] connection flags (NOT USED)
//  LPVOID      [in] user-defined context
// Returns:
//  BOOL        TRUE to continue enumerating
BOOL FAR PASCAL EnumSP(LPCGUID lpguidSP,
					   LPVOID lpConnection, DWORD dwConnectionSize,
					   LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
	HWND		hWnd = (HWND) lpContext;
	LRESULT 	iIndex;
	LPGUID		lpGuid;

	// store service provider name in combo box
	iIndex = SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_ADDSTRING, 0, 
									(LPARAM) lpName->lpszShortNameA);
	if (iIndex == LB_ERR)
		goto Failure;

	// make space for service provider GUID
	lpGuid = (LPGUID) GlobalAllocPtr(GHND, sizeof(GUID));
	if (lpGuid == NULL)
		goto Failure;

	// store pointer to GUID in combo box
	*lpGuid = *lpguidSP;
	SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_SETITEMDATA, (WPARAM) iIndex, (LPARAM) lpGuid);

Failure:
	return (TRUE);
}

// ---------------------------------------------------------------------------
// InitializeLauncherWindow
// ---------------------------------------------------------------------------
// Description:             Initializes the window for the Launcher.
// Arguments:
//  HWND                    [in] Window handle.
//  LPDIRECTPLAYLOBBY3A     [out] IDirectPlayLobby interface.
// Returns:
//  HRESULT                 any errors initializing the window
HRESULT InitializeLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBY3A *lplpDPlayLobby)
{
	LPDIRECTPLAY4A			lpDPlay = NULL;
	LPDIRECTPLAYLOBBY3A		lpDPlayLobby3A = NULL;
	HRESULT					hr;
		
	// create a temporary ANSI DirectPlay4 interface
	hr = CoCreateInstance(&CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
						  &IID_IDirectPlay4A, (LPVOID*)&lpDPlay);
	if FAILED(hr)
		return (hr);

	// get ANSI DirectPlayLobby interface
	hr = CoCreateInstance(&CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER,
						  &IID_IDirectPlayLobby3A, (LPVOID *) &lpDPlayLobby3A);
	if FAILED(hr)
		goto Failure;

	// put all the DirectPlay applications in a combo box
	lpDPlayLobby3A->lpVtbl->EnumLocalApplications(lpDPlayLobby3A, EnumApp, hWnd, 0);

	// put all the service providers in a combo box
	lpDPlay->lpVtbl->EnumConnections(lpDPlay, NULL, EnumSP, hWnd, 0);

	// initialize the controls
	SendDlgItemMessage(hWnd, IDC_APPCOMBO, CB_SETCURSEL, (WPARAM) 0, 0);
	SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_SETCURSEL, (WPARAM) 0, 0);
	SendDlgItemMessage(hWnd, IDC_HOSTRADIO, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);

	// update the address info display
	hr = UpdateAddressInfo(hWnd, lpDPlayLobby3A);

	// return the ANSI lobby interface
	*lplpDPlayLobby = lpDPlayLobby3A;
	lpDPlayLobby3A = NULL;		// we no longer own it so don't free it below

	hr = DP_OK;

Failure:
	if (lpDPlay)
		lpDPlay->lpVtbl->Release(lpDPlay);
	if (lpDPlayLobby3A)
		lpDPlayLobby3A->lpVtbl->Release(lpDPlayLobby3A);

	return (hr);
}

// ---------------------------------------------------------------------------
// DestroyLauncherWindow
// ---------------------------------------------------------------------------
// Description:             Destroys the launcher window.
// Arguments:
//  HWND                    [in] Window handle.
//  LPDIRECTPLAYLOBBY3A     [in] DirectPlay Lobby interface to destroy
// Returns:
//  Nothing
void DestroyLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby)
{
	WPARAM  index;
	LRESULT lpData;

	// destroy the GUID's stored with each app name
	index = 0;
	while (TRUE)
	{
		lpData = SendDlgItemMessage(hWnd, IDC_APPCOMBO, CB_GETITEMDATA, (WPARAM) index, 0);
		if ((lpData == CB_ERR) || (lpData == 0))
			break;

		GlobalFreePtr((LPVOID) lpData);
		index += 1;
	}

	// destroy the GUID's stored with each service provider name
	index = 0;
	while (TRUE)
	{
		lpData = SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_GETITEMDATA, (WPARAM) index, 0);
		if ((lpData == CB_ERR) || (lpData == 0))
			break;

		GlobalFreePtr((LPVOID) lpData);
		index += 1;
	}

	// release the lobby interface
	if (lpDPlayLobby)
		lpDPlayLobby->lpVtbl->Release(lpDPlayLobby);
}

// ---------------------------------------------------------------------------
// EnumAddressTypes
// ---------------------------------------------------------------------------
// Description: Enumeration callback called by DirectPlayLobby.
//              Enumerates the address types supported by the
//              given Service Provider and returns them in a list.
// Arguments:
//  REFGUID     [in] GUID of the address type
//  LPVOID      [in] user-defined context
//  DWORD       [in] flags
// Returns:
//  BOOL        FALSE to stop enumerating after the first callback
BOOL FAR PASCAL EnumAddressTypes(REFGUID guidAddressType, LPVOID lpContext,
								DWORD dwFlags)
{
	LPADDRESSTYPELIST	lpAddressTypes = (LPADDRESSTYPELIST) lpContext;

	// make sure there is room
	if (lpAddressTypes->dwCount < ADDRESSTYPEMAX)
	{
		// save the address type guid in the list
		lpAddressTypes->guidAddressTypes[lpAddressTypes->dwCount] = *guidAddressType;
		lpAddressTypes->dwCount++;
	}

	return (TRUE);
}

// ---------------------------------------------------------------------------
// UpdateAddressInfo
// ---------------------------------------------------------------------------
// Description:             Updates address information elements in dialog.
//                          Calls EnumAddressTypes() to determine what address
//                          information should be displayed and arranges dialog
//                          to display and collect the needed information.
// Arguments:
//  HWND                    [in] window handle
//  LPDIRECTPLAYLOBBY3A     [in] DirectPlay Lobby interface to use
// Returns:
//  HRESULT                 DP_OK if it succeedes, otherwise the error
HRESULT UpdateAddressInfo(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby)
{
	GUID				guidServiceProvider, guidAddressType;
	ADDRESSTYPELIST		addressTypeList;
	DWORD				i;
	HRESULT 			hr;

	// get guid of currently selected service provider
	hr = GetComboBoxGuid(hWnd, IDC_SPCOMBO, &guidServiceProvider);
	if FAILED(hr)
		goto Failure;

	// get the list of address types for this service provider
	ZeroMemory(&addressTypeList, sizeof(ADDRESSTYPELIST));
	hr = lpDPlayLobby->lpVtbl->EnumAddressTypes(lpDPlayLobby,
			EnumAddressTypes, &guidServiceProvider, &addressTypeList, 0L);
	if FAILED(hr)
		goto Failure;

	// clear and hide address dialog items
	SendDlgItemMessage(hWnd, IDC_ADDRESSCOMBO, CB_RESETCONTENT,
								(WPARAM) 0, (LPARAM) 0);
	ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSCOMBO), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSCOMBOLABEL), SW_HIDE);

	SetDlgItemText(hWnd, IDC_ADDRESSEDIT, "");
	ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSEDIT), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSEDITLABEL), SW_HIDE);

	SetDlgItemText(hWnd, IDC_PORTEDIT, "");
	ShowWindow(GetDlgItem(hWnd, IDC_PORTEDIT), SW_HIDE);


	// loop over the address types
	for (i = 0; i < addressTypeList.dwCount; i++)
	{
		guidAddressType = addressTypeList.guidAddressTypes[i];

		// phone number
		if (IsEqualGUID(&guidAddressType, &DPAID_Phone))
		{
			SetDlgItemText(hWnd, IDC_ADDRESSEDITLABEL, "Phone number");
			ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSEDIT), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSEDITLABEL), SW_SHOW);
		}

		// modem
		else if (IsEqualGUID(&guidAddressType, &DPAID_Modem))
		{
			SetDlgItemText(hWnd, IDC_ADDRESSCOMBOLABEL, "Modem");
			ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSCOMBO), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSCOMBOLABEL), SW_SHOW);
			FillModemComboBox(hWnd, lpDPlayLobby, &guidServiceProvider);
		}

		// internet address
		else if (IsEqualGUID(&guidAddressType, &DPAID_INet))
		{
			SetDlgItemText(hWnd, IDC_ADDRESSEDITLABEL, "IP address");
			ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSEDIT), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSEDITLABEL), SW_SHOW);
		}

		// internet address port
		else if (IsEqualGUID(&guidAddressType, &DPAID_INetPort))
		{
			SetDlgItemText(hWnd, IDC_ADDRESSCOMBOLABEL, "Port");
			ShowWindow(GetDlgItem(hWnd, IDC_PORTEDIT), SW_SHOW);
			ShowWindow(GetDlgItem(hWnd, IDC_ADDRESSCOMBOLABEL), SW_SHOW);
		}
	}

Failure:
	return (hr);
}

// ---------------------------------------------------------------------------
// CreateAddress
// ---------------------------------------------------------------------------
// Description:         Creates a DPADDRESS using the address information
//                      from the dialog.
// Arguments:
//  HWND                [in] window handle
//  LPDIRECTPLAYLOBBY3A [in] DirectPlay Lobby interface to use
//  LPGUID              [in] GUID of servicer provider to create address for
//  LPVOID*             [out] pointer to return address in
//  LPDWORD             [out] pointer to return address size in
// Returns:
//  HRESULT             any error creating the address
HRESULT CreateAddress(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby,
					  LPGUID lpguidServiceProvider,
					  LPVOID *lplpAddress, LPDWORD lpdwAddressSize)
{
	GUID						guidAddressType;
	ADDRESSTYPELIST 			addressTypeList;
	DPCOMPOUNDADDRESSELEMENT	addressElements[1 + ADDRESSTYPEMAX];
	CHAR						szPhoneNumberString[NAMEMAX];
	CHAR						szModemString[NAMEMAX];
	CHAR						szIPAddressString[NAMEMAX];
	CHAR						szPort[NAMEMAX];
	LPVOID						lpAddress = NULL;
	DWORD						dwAddressSize = 0;
	DWORD						i, dwElementCount;
	WORD						wPort=0;
	HRESULT 					hr;

	// get the list of address types for this service provider
	ZeroMemory(&addressTypeList, sizeof(ADDRESSTYPELIST));
	hr = lpDPlayLobby->lpVtbl->EnumAddressTypes(lpDPlayLobby,
			EnumAddressTypes, lpguidServiceProvider, &addressTypeList, 0L);
	if FAILED(hr)
		goto Failure;

	dwElementCount = 0;

	// all DPADDRESS's must have a service provider chunk
	addressElements[dwElementCount].guidDataType = DPAID_ServiceProvider;
	addressElements[dwElementCount].dwDataSize = sizeof(GUID);
	addressElements[dwElementCount].lpData = lpguidServiceProvider;
	dwElementCount++;

	// loop over the address types
	for (i = 0; i < addressTypeList.dwCount; i++)
	{
		guidAddressType = addressTypeList.guidAddressTypes[i];

		// phone number
		if (IsEqualGUID(&guidAddressType, &DPAID_Phone))
		{
			// add a phone number chunk
			GetDlgItemText(hWnd, IDC_ADDRESSEDIT, szPhoneNumberString, NAMEMAX);
			addressElements[dwElementCount].guidDataType = DPAID_Phone;
			addressElements[dwElementCount].dwDataSize = lstrlen(szPhoneNumberString) + 1;
			addressElements[dwElementCount].lpData = szPhoneNumberString;
			dwElementCount++;
		}

		// modem
		else if (IsEqualGUID(&guidAddressType, &DPAID_Modem))
		{
			// add a modem chunk
			GetDlgItemText(hWnd, IDC_ADDRESSCOMBO, szModemString, NAMEMAX);
			addressElements[dwElementCount].guidDataType = DPAID_Modem;
			addressElements[dwElementCount].dwDataSize = lstrlen(szModemString) + 1;
			addressElements[dwElementCount].lpData = szModemString;
			dwElementCount++;
		}

		// internet address
		else if (IsEqualGUID(&guidAddressType, &DPAID_INet))
		{
			// add an IP address chunk
			GetDlgItemText(hWnd, IDC_ADDRESSEDIT, szIPAddressString, NAMEMAX);
			addressElements[dwElementCount].guidDataType = DPAID_INet;
			addressElements[dwElementCount].dwDataSize = lstrlen(szIPAddressString) + 1;
			addressElements[dwElementCount].lpData = szIPAddressString;
			dwElementCount++;
		}
		else if (IsEqualGUID(&guidAddressType, &DPAID_INetPort))
		{
			memset(szPort, 0, sizeof(szPort));
			// add a Port chunk
			GetDlgItemText(hWnd, IDC_PORTEDIT, szPort, NAMEMAX);
			wPort = atoi(szPort);
			addressElements[dwElementCount].guidDataType = DPAID_INetPort;
			addressElements[dwElementCount].dwDataSize = sizeof(WORD);
			addressElements[dwElementCount].lpData = (LPVOID)&wPort;
			dwElementCount++;
		}
	}

	// bail if no address data is available
	if (dwElementCount == 1)
		return (DPERR_GENERIC);
	
	// see how much room is needed to store this address
	hr = lpDPlayLobby->lpVtbl->CreateCompoundAddress(lpDPlayLobby,
						addressElements, dwElementCount,
						NULL, &dwAddressSize);
	if (hr != DPERR_BUFFERTOOSMALL)
		goto Failure;

	// allocate space
	lpAddress = GlobalAllocPtr(GHND, dwAddressSize);
	if (lpAddress == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto Failure;
	}

	// create the address
	hr = lpDPlayLobby->lpVtbl->CreateCompoundAddress(lpDPlayLobby,
						addressElements, dwElementCount,
						lpAddress, &dwAddressSize);
	if FAILED(hr)
		goto Failure;

	// return the address info
	*lplpAddress = lpAddress;
	*lpdwAddressSize = dwAddressSize;

	return (DP_OK);

Failure:
	if (lpAddress)
		GlobalFreePtr(lpAddress);

	return (hr);
}

// ---------------------------------------------------------------------------
// RunApplication
// ---------------------------------------------------------------------------
// Description: Wrapper for the IDirectPlayLobby::RunApplication() method.
// Arguments:
//  LPDIRECTPLAYLOBBY3A [in] DirectPlay Lobby interface to use
//  LPGUID              [in] GUID of application to launch
//  LPGUID              [in] GUID of session to host with
//  LPSTR               [in] GUID of service provider to connect with
//  LPVOID              [in] service-provider address to connect to
//  DWORD               [in] length of address
//  LPSTR               [in] name of session to host
//  LPSTR               [in] name of our player
//  BOOL                [in] TRUE to host session, FALSE to join
// Returns:
//  HRESULT             any error running the application
HRESULT RunApplication(LPDIRECTPLAYLOBBY3A lpDPlayLobby,
					   LPGUID lpguidApplication,
					   LPGUID lpguidInstance,
					   LPGUID lpguidServiceProvider,
					   LPVOID lpAddress,
					   DWORD  dwAddressSize,
					   LPSTR  lpszSessionName,
					   LPSTR  lpszPlayerName,
					   BOOL   bHostSession)
{
	DWORD				appID;
	DPSESSIONDESC2		sessionInfo;
	DPNAME				playerName;
	DPLCONNECTION		connectInfo;
	HRESULT 			hr;

	if (lpDPlayLobby == NULL)
		return (DPERR_NOINTERFACE);

	// fill out session description
	ZeroMemory(&sessionInfo, sizeof(DPSESSIONDESC2));
	sessionInfo.dwSize = sizeof(DPSESSIONDESC2);	// Size of structure
	sessionInfo.dwFlags = 0;						// DPSESSION_xxx flags
	sessionInfo.guidInstance = *lpguidInstance; 	// ID for the session instance
	sessionInfo.guidApplication = *lpguidApplication;// GUID of the DirectPlay application.
	sessionInfo.dwMaxPlayers = 0;					// Maximum # players allowed in session
	sessionInfo.dwCurrentPlayers = 0;				// Current # players in session (read only)
	sessionInfo.lpszSessionNameA = lpszSessionName; // ANSI name of the session
	sessionInfo.lpszPasswordA = NULL;				// ANSI password of the session (optional)
	sessionInfo.dwReserved1 = 0;					// Reserved for future MS use.
	sessionInfo.dwReserved2 = 0;
	sessionInfo.dwUser1 = 0;						// For use by the application
	sessionInfo.dwUser2 = 0;
	sessionInfo.dwUser3 = 0;
	sessionInfo.dwUser4 = 0;

	// fill out player name
	ZeroMemory(&playerName, sizeof(DPNAME));
	playerName.dwSize = sizeof(DPNAME); 			// Size of structure
	playerName.dwFlags = 0; 						// Not used. Must be zero.
	playerName.lpszShortNameA = lpszPlayerName; 	// ANSI short or friendly name
	playerName.lpszLongNameA = lpszPlayerName;		// ANSI long or formal name
	
	// fill out connection description
	ZeroMemory(&connectInfo, sizeof(DPLCONNECTION));
	connectInfo.dwSize = sizeof(DPLCONNECTION); 	// Size of this structure
	if (bHostSession)
		connectInfo.dwFlags = DPLCONNECTION_CREATESESSION; // Create a new session
	else
		connectInfo.dwFlags = DPLCONNECTION_JOINSESSION; // Join existing session
	connectInfo.lpSessionDesc = &sessionInfo;		// Pointer to session desc to use on connect
	connectInfo.lpPlayerName = &playerName; 		// Pointer to Player name structure
	connectInfo.guidSP = *lpguidServiceProvider;	// GUID of the DPlay SP to use
	connectInfo.lpAddress = lpAddress;				// Address for service provider
	connectInfo.dwAddressSize = dwAddressSize;		// Size of address data

	// launch and connect the game
	hr = lpDPlayLobby->lpVtbl->RunApplication(lpDPlayLobby,
										0,				// Flags
										&appID, 		// App ID
										&connectInfo,	// Connection data
										NULL);			// Connect event
	return (hr);
}

// ---------------------------------------------------------------------------
// LaunchDirectPlayApplication
// ---------------------------------------------------------------------------
// Description: Gathers information from the dialog and runs the application.
// Arguments:
//  HWND                [in] Window handle.
//  LPDIRECTPLAYLOBBY3A [in] DirectPlay Lobby interface to use
// Returns:
//  Nothing
void LaunchDirectPlayApplication(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby)
{
	GUID		guidApplication, guidSession, guidServiceProvider;
	LPSTR		lpPlayerName, lpSessionName;
	LPVOID		lpAddress = NULL;
	DWORD		dwAddressSize = 0;
	CHAR		szPlayerName[NAMEMAX], szSessionName[NAMEMAX];
	LRESULT 	iHost;
	HRESULT 	hr;

	SetDlgItemText(hWnd, IDC_STATUSEDIT, "Launching...");

	// get guid of application to launch
	hr = GetComboBoxGuid(hWnd, IDC_APPCOMBO, &guidApplication);
	if FAILED(hr)
		goto Failure;

	// get guid of service provider to use
	hr = GetComboBoxGuid(hWnd, IDC_SPCOMBO, &guidServiceProvider);
	if FAILED(hr)
		goto Failure;

	// get address to use with this service provider
	hr = CreateAddress(hWnd, lpDPlayLobby, &guidServiceProvider,
					   &lpAddress, &dwAddressSize);
	// ignore the error because lpAddress will just be null

	// get guid of session to create.
	guidSession = MY_SESSION_GUID;
	
	// get name of our player
	GetDlgItemText(hWnd, IDC_PLAYEREDIT, szPlayerName, NAMEMAX);
	lpPlayerName = szPlayerName;

	// get host vs. join flag
	iHost = SendDlgItemMessage(hWnd, IDC_HOSTRADIO, BM_GETCHECK, (WPARAM) 0, 0);
	if (iHost == BST_CHECKED)
	{
		iHost = TRUE;			// we are hosting a session

		// get name of session
		GetDlgItemText(hWnd, IDC_SESSIONEDIT, szSessionName, NAMEMAX);
		lpSessionName = szSessionName;
	}
	else
	{
		iHost = FALSE;			// we are joining an existing session
		lpSessionName = NULL;	// don't need a session name if we are joining
	}

	// launch the application
	hr = RunApplication(lpDPlayLobby,
						&guidApplication,
						&guidSession,
						&guidServiceProvider,
						lpAddress, dwAddressSize,
						lpSessionName, lpPlayerName,
						iHost);
	if FAILED(hr)
		goto Failure;

	SetDlgItemText(hWnd, IDC_STATUSEDIT, "Launch successful");

	if (lpAddress)
		GlobalFreePtr(lpAddress);
	return;

Failure:
	if (lpAddress)
		GlobalFreePtr(lpAddress);

	SetDlgItemText(hWnd, IDC_STATUSEDIT, "Launch failed");

	return;
}

// ---------------------------------------------------------------------------
// GetComboBoxGuid
// ---------------------------------------------------------------------------
// Description: Returns GUID stored with a combo box item
// Arguments:
//  HWND        [in]  Window handle.
//  LONG        [in]  Dialog box item ID of combo box
//  LPGUID      [out] GUID stored with item in combo box
// Returns:
//  HRESULT     any error getting the GUID
HRESULT GetComboBoxGuid(HWND hWnd, LONG iDialogItem, LPGUID lpguidReturn)
{
	LONG    iIndex;

	// get index of selected item
	iIndex = SendDlgItemMessage(hWnd, iDialogItem, CB_GETCURSEL,
								(WPARAM) 0, (LPARAM) 0);
	if (iIndex == CB_ERR)
		return (DPERR_GENERIC);

	// get data associated with this item
	iIndex = SendDlgItemMessage(hWnd, iDialogItem, CB_GETITEMDATA,
								(WPARAM) iIndex, (LPARAM) 0);
	if ((iIndex == CB_ERR) || (iIndex == 0))
		return (DPERR_GENERIC);

	// data is a pointer to a guid
	*lpguidReturn = *((LPGUID) iIndex);

	return (DP_OK);
}

// ---------------------------------------------------------------------------
// EnumModemAddress
// ---------------------------------------------------------------------------
// Description: Enumeration callback called by DirectPlayLobby.
//              Enumerates the DirectPlay address chunks. If the
//              chunk contains modem strings, add them to the control.
// Arguments:
//  REFGUID     [in] GUID of the address type
//  DWORD       [in] size of chunk
//  LPVOID      [in] pointer to chunk
//  LPVOID      [in] user-defined context
// Returns:
//  BOOL        FALSE to stop enumerating after the first callback
BOOL FAR PASCAL EnumModemAddress(REFGUID lpguidDataType, DWORD dwDataSize,
								 LPCVOID lpData, LPVOID lpContext)
{
	HWND	hWnd = (HWND) lpContext;
	LPSTR	lpszStr = (LPSTR) lpData;

	// modem
	if (IsEqualGUID(lpguidDataType, &DPAID_Modem))
	{
		// loop over all strings in list
		while (lstrlen(lpszStr))
		{
			// store modem name in combo box
			SendDlgItemMessage(hWnd, IDC_ADDRESSCOMBO, CB_ADDSTRING, 0, (LPARAM) lpszStr);

			// skip to next string
			lpszStr += lstrlen(lpszStr) + 1;
		}
	}

	return (TRUE);
}

// ---------------------------------------------------------------------------
// FillModemComboBox
// ---------------------------------------------------------------------------
// Description:             Fills combo box with modem names
// Arguments:
//  HWND                    [in]  Window handle.
//  LPDIRECTPLAYLOBBY3A     [in]  DirectPlay Lobby interface to use
//  LPGUID                  [in] GUID of service provider to use
// Returns:
//  HRESULT                 any error
HRESULT FillModemComboBox(HWND hWnd, LPDIRECTPLAYLOBBY3A lpDPlayLobby, LPGUID lpguidServiceProvider)
{
	LPDIRECTPLAY		lpDPlay1 = NULL;
	LPDIRECTPLAY4A		lpDPlay4A = NULL;
	LPVOID				lpAddress = NULL;
	DWORD				dwAddressSize = 0;
	HRESULT 			hr;

	// Use the obsolete DirectPlayCreate() as quick way to load a specific
	// service provider.  Trade off using DirectPlayCreate() and
	// QueryInterface() vs using CoCreateInitialize() and building a
	// DirectPlay Address for InitializeConnection().
	hr = DirectPlayCreate(lpguidServiceProvider, &lpDPlay1, NULL);
	if FAILED(hr)
		goto Failure;

	// query for an ANSI DirectPlay4 interface
	hr = lpDPlay1->lpVtbl->QueryInterface(lpDPlay1, &IID_IDirectPlay4A, (LPVOID *) &lpDPlay4A);
	if FAILED(hr)
		goto Failure;

	// get size of player address for player zero
	hr = lpDPlay4A->lpVtbl->GetPlayerAddress(lpDPlay4A, DPID_ALLPLAYERS, NULL, &dwAddressSize);
	if (hr != DPERR_BUFFERTOOSMALL)
		goto Failure;

	// make room for it
	lpAddress = GlobalAllocPtr(GHND, dwAddressSize);
	if (lpAddress == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto Failure;
	}

	// get the address
	hr = lpDPlay4A->lpVtbl->GetPlayerAddress(lpDPlay4A, DPID_ALLPLAYERS, lpAddress, &dwAddressSize);
	if FAILED(hr)
		goto Failure;
	
	// get modem strings from address and put them in the combo box
	hr = lpDPlayLobby->lpVtbl->EnumAddress(lpDPlayLobby, EnumModemAddress, 
							 lpAddress, dwAddressSize, hWnd);
	if FAILED(hr)
		goto Failure;

	// select first item in list
	SendDlgItemMessage(hWnd, IDC_ADDRESSCOMBO, CB_SETCURSEL, (WPARAM) 0, 0);

Failure:
	if (lpDPlay1)
		lpDPlay1->lpVtbl->Release(lpDPlay1);
	if (lpDPlay4A)
		lpDPlay4A->lpVtbl->Release(lpDPlay4A);
	if (lpAddress)
		GlobalFreePtr(lpAddress);

	return (hr);
}


void ErrorBox(LPSTR lpszErrorStr, HRESULT hr)
{
	char	szStr[MAXSTRLEN];

	wsprintf(szStr, lpszErrorStr, GetDirectPlayErrStr(hr));

	MessageBox(NULL, szStr, "DPLaunch Error", MB_OK);
}

char * GetDirectPlayErrStr(HRESULT hr)
{
	static char		szTempStr[12];

	switch (hr)
	{
	case DP_OK: return ("DP_OK");
	case DPERR_ALREADYINITIALIZED: return ("DPERR_ALREADYINITIALIZED");
	case DPERR_ACCESSDENIED: return ("DPERR_ACCESSDENIED");
	case DPERR_ACTIVEPLAYERS: return ("DPERR_ACTIVEPLAYERS");
	case DPERR_BUFFERTOOSMALL: return ("DPERR_BUFFERTOOSMALL");
	case DPERR_CANTADDPLAYER: return ("DPERR_CANTADDPLAYER");
	case DPERR_CANTCREATEGROUP: return ("DPERR_CANTCREATEGROUP");
	case DPERR_CANTCREATEPLAYER: return ("DPERR_CANTCREATEPLAYER");
	case DPERR_CANTCREATESESSION: return ("DPERR_CANTCREATESESSION");
	case DPERR_CAPSNOTAVAILABLEYET: return ("DPERR_CAPSNOTAVAILABLEYET");
	case DPERR_EXCEPTION: return ("DPERR_EXCEPTION");
	case DPERR_GENERIC: return ("DPERR_GENERIC");
	case DPERR_INVALIDFLAGS: return ("DPERR_INVALIDFLAGS");
	case DPERR_INVALIDOBJECT: return ("DPERR_INVALIDOBJECT");
//	case DPERR_INVALIDPARAM: return ("DPERR_INVALIDPARAM");	 dup value
	case DPERR_INVALIDPARAMS: return ("DPERR_INVALIDPARAMS");
	case DPERR_INVALIDPLAYER: return ("DPERR_INVALIDPLAYER");
	case DPERR_INVALIDGROUP: return ("DPERR_INVALIDGROUP");
	case DPERR_NOCAPS: return ("DPERR_NOCAPS");
	case DPERR_NOCONNECTION: return ("DPERR_NOCONNECTION");
//	case DPERR_NOMEMORY: return ("DPERR_NOMEMORY");		dup value
	case DPERR_OUTOFMEMORY: return ("DPERR_OUTOFMEMORY");
	case DPERR_NOMESSAGES: return ("DPERR_NOMESSAGES");
	case DPERR_NONAMESERVERFOUND: return ("DPERR_NONAMESERVERFOUND");
	case DPERR_NOPLAYERS: return ("DPERR_NOPLAYERS");
	case DPERR_NOSESSIONS: return ("DPERR_NOSESSIONS");
	case DPERR_PENDING: return ("DPERR_PENDING");
	case DPERR_SENDTOOBIG: return ("DPERR_SENDTOOBIG");
	case DPERR_TIMEOUT: return ("DPERR_TIMEOUT");
	case DPERR_UNAVAILABLE: return ("DPERR_UNAVAILABLE");
	case DPERR_UNSUPPORTED: return ("DPERR_UNSUPPORTED");
	case DPERR_BUSY: return ("DPERR_BUSY");
	case DPERR_USERCANCEL: return ("DPERR_USERCANCEL");
	case DPERR_NOINTERFACE: return ("DPERR_NOINTERFACE");
	case DPERR_CANNOTCREATESERVER: return ("DPERR_CANNOTCREATESERVER");
	case DPERR_PLAYERLOST: return ("DPERR_PLAYERLOST");
	case DPERR_SESSIONLOST: return ("DPERR_SESSIONLOST");
	case DPERR_UNINITIALIZED: return ("DPERR_UNINITIALIZED");
	case DPERR_NONEWPLAYERS: return ("DPERR_NONEWPLAYERS");
	case DPERR_INVALIDPASSWORD: return ("DPERR_INVALIDPASSWORD");
	case DPERR_CONNECTING: return ("DPERR_CONNECTING");
	case DPERR_CONNECTIONLOST: return ("DPERR_CONNECTIONLOST");
	case DPERR_UNKNOWNMESSAGE: return ("DPERR_UNKNOWNMESSAGE");
	case DPERR_CANCELFAILED: return ("DPERR_CANCELFAILED");
	case DPERR_INVALIDPRIORITY: return ("DPERR_INVALIDPRIORITY");
	case DPERR_NOTHANDLED: return ("DPERR_NOTHANDLED");
	case DPERR_CANCELLED: return ("DPERR_CANCELLED");
	case DPERR_ABORTED: return ("DPERR_ABORTED");
	case DPERR_BUFFERTOOLARGE: return ("DPERR_BUFFERTOOLARGE");
	case DPERR_CANTCREATEPROCESS: return ("DPERR_CANTCREATEPROCESS");
	case DPERR_APPNOTSTARTED: return ("DPERR_APPNOTSTARTED");
	case DPERR_INVALIDINTERFACE: return ("DPERR_INVALIDINTERFACE");
	case DPERR_NOSERVICEPROVIDER: return ("DPERR_NOSERVICEPROVIDER");
	case DPERR_UNKNOWNAPPLICATION: return ("DPERR_UNKNOWNAPPLICATION");
	case DPERR_NOTLOBBIED: return ("DPERR_NOTLOBBIED");
	case DPERR_SERVICEPROVIDERLOADED: return ("DPERR_SERVICEPROVIDERLOADED");
	case DPERR_ALREADYREGISTERED: return ("DPERR_ALREADYREGISTERED");
	case DPERR_NOTREGISTERED: return ("DPERR_NOTREGISTERED");
	case DPERR_AUTHENTICATIONFAILED: return ("DPERR_AUTHENTICATIONFAILED");
	case DPERR_CANTLOADSSPI: return ("DPERR_CANTLOADSSPI");
	case DPERR_ENCRYPTIONFAILED: return ("DPERR_ENCRYPTIONFAILED");
	case DPERR_SIGNFAILED: return ("DPERR_SIGNFAILED");
	case DPERR_CANTLOADSECURITYPACKAGE: return ("DPERR_CANTLOADSECURITYPACKAGE");
	case DPERR_ENCRYPTIONNOTSUPPORTED: return ("DPERR_ENCRYPTIONNOTSUPPORTED");
	case DPERR_CANTLOADCAPI: return ("DPERR_CANTLOADCAPI");
	case DPERR_NOTLOGGEDIN: return ("DPERR_NOTLOGGEDIN");
	case DPERR_LOGONDENIED: return ("DPERR_LOGONDENIED");
	}

	// For errors not in the list, return HRESULT string
	wsprintf(szTempStr, "0x%08X", hr);
	return (szTempStr);
}

