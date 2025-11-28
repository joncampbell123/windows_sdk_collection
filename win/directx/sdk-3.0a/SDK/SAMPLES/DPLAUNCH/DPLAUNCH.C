/*==========================================================================
 *
 *  Copyright (C) 1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dplaunch.c
 *  Content:    Implementation of a DirectPlay launching utility
 *
 ***************************************************************************/

#define INITGUID
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <cguid.h>

#include "dplay.h"
#include "dplobby.h"

#include "resource.h"

// maximum size of a string name
#define NAMEMAX         200

// service provider information
typedef struct {
        GUID    guidServiceProvider;            // guid of service provider
        GUID    guidAddressType;                        // address type required by service provider
} SPINFO, *LPSPINFO;

typedef struct {
        HWND                            hWnd;
        LPDIRECTPLAYLOBBYA      lpDPLobbyA;
} ENUMINFO, *LPENUMINFO;

// GUID for sessions this application creates
// {D559FC00-DC12-11cf-9C4E-00A0C905425E}
DEFINE_GUID(MY_SESSION_GUID, 
0xd559fc00, 0xdc12, 0x11cf, 0x9c, 0x4e, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);

// prototypes
BOOL CALLBACK           LauncherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT                         InitializeLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBYA *lplpDPlayLobby);
void                            DestroyLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBYA lpDPlayLobby);
void                            LaunchDirectPlayApplication(HWND hWnd, LPDIRECTPLAYLOBBYA lpDPlayLobby);


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
    return DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_LAUNCHERDIALOG), NULL, LauncherWndProc, (LPARAM) hInstance);
}


// ---------------------------------------------------------------------------
// LauncherWndProc
// ---------------------------------------------------------------------------
// Description:             Message callback function for Launcher dialog.
// Arguments:
//  HWND                    [in] Dialog window handle.
//  UINT                    [in] Window message identifier.
//  WPARAM                  [in] Depends on message.
//  LPARAM                  [in] Depends on message.
// Returns:
//  BOOL                    TRUE if message was processed internally.
BOOL CALLBACK LauncherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE                    hInst;
        static LPDIRECTPLAYLOBBYA       lpDPlayLobby;
        HRESULT                                         hr;

    switch(uMsg)
    {
        case WM_INITDIALOG:
            // Save the instance handle
            hInst = (HINSTANCE)lParam;
                        
                        // Initialize dialog with launcher information
                        lpDPlayLobby = NULL;
                        hr = InitializeLauncherWindow(hWnd, &lpDPlayLobby);
            break;

        case WM_DESTROY:
                        // Destroy launcher information in dialog
                        DestroyLauncherWindow(hWnd, lpDPlayLobby);

            // Return failure
            EndDialog(hWnd, FALSE);

            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_RUNAPPBUTTON:
                                        // get settings and launch application
                    LaunchDirectPlayApplication(hWnd, lpDPlayLobby);

                    break;

                case IDCANCEL:
                    // Return failure
                    EndDialog(hWnd, TRUE);

                    break;
            }

            break;
    }

    // Allow for default processing
    return FALSE;
}

// ---------------------------------------------------------------------------
// EnumApp
// ---------------------------------------------------------------------------
// Description:             Enumeration callback called by DirectPlay.
//                                                      Enumerates the applications registered with DirectPlay.
// Arguments:
//  LPDPLAPPINFO            [in] information about the application
//  LPVOID                                  [in] user-defined context
//  DWORD                                       [in] flags
// Returns:
//  BOOL                                        TRUE to continue enumerating
BOOL FAR PASCAL EnumApp(LPCDPLAPPINFO lpAppInfo, LPVOID lpContext, DWORD dwFlags)
{
    HWND                        hWnd = lpContext;
    LRESULT                     iIndex;
        LPGUID                  lpGuid;

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
// EnumAddressType
// ---------------------------------------------------------------------------
// Description:             Enumeration callback called by DirectPlayLobby.
//                                                      Enumerates the address types supported by the
//                                                      given Service Provider.
// Arguments:
//  REFGUID                 [in] GUID of the address type
//  LPVOID                                  [in] user-defined context
//  DWORD                                       [in] flags
// Returns:
//  BOOL                                        FALSE to stop enumerating after the first callback
BOOL FAR PASCAL EnumAddressType(REFGUID guidAddressType, LPVOID lpContext,
                                        DWORD dwFlags)
{
        LPGUID  lpguidAddr = (LPGUID)lpContext;


        // Save the address type guid in the pointer
        *lpguidAddr = *guidAddressType;

        // Note: It is possible that some Service Providers will contain more
        // than one address type.  We are only using the first address type
        // returned in this sample.  A good application should save all
        // address types.
        return FALSE;
}


// ---------------------------------------------------------------------------
// EnumSP
// ---------------------------------------------------------------------------
// Description:             Enumeration callback called by DirectPlay.
//                                                      Enumerates service providers registered with DirectPlay.
// Arguments:
//  LPGUID                                      [in] GUID of service provider
//  LPTSTR                                      [in] name of service provider
//  DWORD                                       [in] major version of DirectPlay
//  DWORD                                       [in] minor version of DirectPlay
//  LPVOID                                  [in] user-defined context
// Returns:
//  BOOL                                        TRUE to continue enumerating
BOOL FAR PASCAL EnumSP(LPGUID lpGuid, LPTSTR lptszDesc, DWORD dwMajorVersion,
                               DWORD dwMinorVersion, LPVOID lpContext)
{
        LPENUMINFO                      lpEnumInfo = (LPENUMINFO)lpContext;
    LPDIRECTPLAYLOBBYA  lpDPLobbyA = lpEnumInfo->lpDPLobbyA;
        HWND                            hWnd = lpEnumInfo->hWnd;
        HRESULT                         hr;
    LRESULT                             iIndex;
        LPSPINFO                        lpSPInfo;

        // store service provider name in combo box
        iIndex = SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_ADDSTRING, 0, (LPARAM) lptszDesc);
        if (iIndex == LB_ERR)
                goto Failure;

        // make space for service provider info
        lpSPInfo = (LPSPINFO) GlobalAllocPtr(GHND, sizeof(SPINFO));
        if (lpSPInfo == NULL)
                goto Failure;

        // Initialize the guid to GUID_NULL in case it has no address types
        memset(lpSPInfo, 0, sizeof(SPINFO));

        // store service provider GUID and address type in combo box
        lpSPInfo->guidServiceProvider = *lpGuid;

        // Get the address type for the Service Provider
        hr = lpDPLobbyA->lpVtbl->EnumAddressTypes(lpDPLobbyA,
                        (LPDPLENUMADDRESSTYPESCALLBACK)EnumAddressType,
                        lpGuid, &lpSPInfo->guidAddressType, 0L);

        SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_SETITEMDATA, (WPARAM) iIndex, (LPARAM) lpSPInfo);

Failure:
    return (TRUE);
}

// ---------------------------------------------------------------------------
// InitializeLauncherWindow
// ---------------------------------------------------------------------------
// Description:             Initializes the window for the Launcher.
// Arguments:
//  HWND                    [in] Window handle.
//  LPDIRECTPLAYLOBBYA          [out] IDirectPlayLobby interface.
// Returns:
//  HRESULT                                     any errors initializing the window
HRESULT InitializeLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBYA *lplpDPlayLobby)
{
        LPDIRECTPLAYLOBBYA      lpDPlayLobbyA = NULL;
        ENUMINFO                        EnumInfo;
        HRESULT                         hr;
                
        // get a ANSI DirectPlay lobby interface
        hr = DirectPlayLobbyCreate(NULL, &lpDPlayLobbyA, NULL, NULL, 0);
        if FAILED(hr)
                goto Failure;

        // put all the DirectPlay applications in a combo box
        lpDPlayLobbyA->lpVtbl->EnumLocalApplications(lpDPlayLobbyA, EnumApp, hWnd, 0);

        // setup the EnumInfo structure
        EnumInfo.hWnd = hWnd;
        EnumInfo.lpDPLobbyA = lpDPlayLobbyA;
        
        // put all the service providers in a combo box
        DirectPlayEnumerate(EnumSP, &EnumInfo);

        // initialize the controls
        SendDlgItemMessage(hWnd, IDC_APPCOMBO, CB_SETCURSEL, (WPARAM) 0, 0);
        SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_SETCURSEL, (WPARAM) 0, 0);
        SendDlgItemMessage(hWnd, IDC_HOSTRADIO, BM_SETCHECK, (WPARAM) BST_CHECKED, 0);

        // return the ANSI lobby interface
        *lplpDPlayLobby = lpDPlayLobbyA;

        return (DP_OK);

Failure:

        return (hr);
}

// ---------------------------------------------------------------------------
// DestroyLauncherWindow
// ---------------------------------------------------------------------------
// Description:             Destroys the launcher window.
// Arguments:
//  HWND                    [in] Window handle.
//  LPDIRECTPLAYLOBBYA      [in] DirectPlay Lobby interface to destroy
// Returns:
//  Nothing
void DestroyLauncherWindow(HWND hWnd, LPDIRECTPLAYLOBBYA lpDPlayLobby)
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
// CreateAddress
// ---------------------------------------------------------------------------
// Description:             Wrapper for the IDirectPlayLobby::CreateAddress() method.
// Arguments:
//  LPDIRECTPLAYLOBBYA      [in] DirectPlay Lobby interface to use
//  LPGUID                                      [in] GUID of servicer provider to create address for
//  LPGUID                                      [in] GUID of address data type
//  LPSTR                                       [in] string to use as address data
//  LPVOID*                                     [out] pointer to return address in
//  LPDWORD                                     [out] pointer to return address size in
// Returns:
//  HRESULT                                     any error creating the address
HRESULT CreateAddress(LPDIRECTPLAYLOBBYA lpDPlayLobby,
                                          LPGUID lpguidServiceProvider,
                                          LPGUID lpguidAddressType, LPSTR lpszAddressText,
                                          LPVOID *lplpAddress, LPDWORD lpdwAddressSize)
{
        LPVOID          lpAddress = NULL;
        DWORD           dwAddressSize = 0;
        HRESULT         hr;

        // check for invalid address types
        if (IsEqualGUID(lpguidAddressType, &GUID_NULL))
                return (DPERR_INVALIDPARAM);
        
        // see how much room is needed to store this address
        hr = lpDPlayLobby->lpVtbl->CreateAddress(lpDPlayLobby, lpguidServiceProvider,
                                        lpguidAddressType, lpszAddressText, strlen(lpszAddressText) + 1,
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
        hr = lpDPlayLobby->lpVtbl->CreateAddress(lpDPlayLobby, lpguidServiceProvider,
                                        lpguidAddressType, lpszAddressText, strlen(lpszAddressText) + 1,
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
// Description:             Wrapper for the IDirectPlayLobby::RunApplication() method.
// Arguments:
//  LPDIRECTPLAYLOBBYA          [in] DirectPlay Lobby interface to use
//  LPGUID                                      [in] GUID of application to launch
//  LPGUID                                      [in] GUID of session to host with
//  LPSTR                                       [in] GUID of service provider to connect with
//  LPVOID                                      [in] service-provider address to connect to
//  DWORD                                       [in] length of address
//  LPSTR                                       [in] name of session to host
//  LPSTR                                       [in] name of our player
//  BOOL                                        [in] TRUE to host session, FALSE to join
// Returns:
//  HRESULT                                     any error running the application
HRESULT RunApplication(LPDIRECTPLAYLOBBYA lpDPlayLobby,
                                           LPGUID lpguidApplication,
                                           LPGUID lpguidInstance,
                                           LPGUID lpguidServiceProvider,
                                           LPVOID lpAddress,
                                           DWORD  dwAddressSize,
                                           LPSTR  lpszSessionName,
                                           LPSTR  lpszPlayerName,
                                           BOOL   bHostSession)
{
        DWORD                           appID;
        DPSESSIONDESC2          sessionInfo;
        DPNAME                          playerName;
        DPLCONNECTION           connectInfo;
        HRESULT                         hr;

        if (lpDPlayLobby == NULL)
                return (DPERR_NOINTERFACE);

        // fill out session description
        ZeroMemory(&sessionInfo, sizeof(DPSESSIONDESC2));
    sessionInfo.dwSize = sizeof(DPSESSIONDESC2);        // Size of structure
        sessionInfo.dwFlags = 0;                                                // DPSESSION_xxx flags
    sessionInfo.guidInstance = *lpguidInstance;         // ID for the session instance
    sessionInfo.guidApplication = *lpguidApplication;// GUID of the DirectPlay application.
    sessionInfo.dwMaxPlayers = 0;                                       // Maximum # players allowed in session
    sessionInfo.dwCurrentPlayers = 0;                           // Current # players in session (read only)
        sessionInfo.lpszSessionNameA = lpszSessionName; // ANSI name of the session
        sessionInfo.lpszPasswordA = NULL;                               // ANSI password of the session (optional)
        sessionInfo.dwReserved1 = 0;                                    // Reserved for future MS use.
    sessionInfo.dwReserved2 = 0;
    sessionInfo.dwUser1 = 0;                                            // For use by the application
    sessionInfo.dwUser2 = 0;
    sessionInfo.dwUser3 = 0;
    sessionInfo.dwUser4 = 0;

        // fill out player name
        ZeroMemory(&playerName, sizeof(DPNAME));
        playerName.dwSize = sizeof(DPNAME);                             // Size of structure
    playerName.dwFlags = 0;                                                     // Not used. Must be zero.
        playerName.lpszShortNameA = lpszPlayerName;             // ANSI short or friendly name
        playerName.lpszLongNameA = lpszPlayerName;              // ANSI long or formal name
        
        // fill out connection description
        ZeroMemory(&connectInfo, sizeof(DPLCONNECTION));
        connectInfo.dwSize = sizeof(DPLCONNECTION);             // Size of this structure
        if (bHostSession)
                connectInfo.dwFlags = DPLCONNECTION_CREATESESSION; // Create a new session
        else
                connectInfo.dwFlags = DPLCONNECTION_JOINSESSION; // Join existing session
        connectInfo.lpSessionDesc = &sessionInfo;               // Pointer to session desc to use on connect
        connectInfo.lpPlayerName = &playerName;                 // Pointer to Player name structure
        connectInfo.guidSP = *lpguidServiceProvider;    // GUID of the DPlay SP to use
        connectInfo.lpAddress = lpAddress;                              // Address for service provider
        connectInfo.dwAddressSize = dwAddressSize;              // Size of address data

        // launch and connect the game
        hr = lpDPlayLobby->lpVtbl->RunApplication(lpDPlayLobby,
                                                                                0,                      // Flags
                                                                                &appID,         // App ID
                                                                                &connectInfo,   // Connection data
                                                                                NULL);          // Connect event
        return (hr);
}

// ---------------------------------------------------------------------------
// LaunchDirectPlayApplication
// ---------------------------------------------------------------------------
// Description:             Gathers information from the dialog and runs the application.
// Arguments:
//  HWND                    [in] Window handle.
//  LPDIRECTPLAYLOBBYA      [in] DirectPlay Lobby interface to use
// Returns:
//  Nothing
void LaunchDirectPlayApplication(HWND hWnd, LPDIRECTPLAYLOBBYA lpDPlayLobby)
{
        GUID                            guidApplication, guidSession, guidServiceProvider, guidAddressType;
        LPSTR                           lpPlayerName, lpSessionName;
        LPVOID                          lpAddress = NULL;
        DWORD                           dwAddressSize;
        LPSPINFO                        lpSPInfo;
        char                            strPlayerName[NAMEMAX], strSessionName[NAMEMAX], strAddressText[NAMEMAX];
        LRESULT                         iApp, iSP, iHost;
        HRESULT                         hr;

        SetDlgItemText(hWnd, IDC_STATUSEDIT, "Launching...");

        // get guid of application to launch
        iApp = SendDlgItemMessage(hWnd, IDC_APPCOMBO, CB_GETCURSEL, (WPARAM) 0, 0);
        if (iApp == CB_ERR)
                goto Failure;

        iApp = SendDlgItemMessage(hWnd, IDC_APPCOMBO, CB_GETITEMDATA, (WPARAM) iApp, 0);
        if ((iApp == CB_ERR) || (iApp == 0))
                goto Failure;

        guidApplication = *((LPGUID) iApp);

        // get info for service provider to use for the connection
        iSP = SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_GETCURSEL, (WPARAM) 0, 0);
        if (iSP == CB_ERR)
                goto Failure;

        iSP = SendDlgItemMessage(hWnd, IDC_SPCOMBO, CB_GETITEMDATA, (WPARAM) iSP, 0);
        if ((iSP == CB_ERR) || (iSP == 0))
                goto Failure;

        lpSPInfo = (LPSPINFO) iSP;
        guidServiceProvider = lpSPInfo->guidServiceProvider;    // GUID of service provider
        guidAddressType = lpSPInfo->guidAddressType;                    // address type required by service provider

        // get guid of session to create.
        guidSession = MY_SESSION_GUID;
        
        // get name of our player
        GetDlgItemText(hWnd, IDC_PLAYEREDIT, strPlayerName, NAMEMAX);
        lpPlayerName = strPlayerName;

        // get host vs. join flag
        iHost = SendDlgItemMessage(hWnd, IDC_HOSTRADIO, BM_GETCHECK, (WPARAM) 0, 0);
        if (iHost == BST_CHECKED)
        {
                iHost = TRUE;                   // we are hosting a session
                lpAddress = NULL;               // don't need an address to host
                dwAddressSize = 0;

                // get name of session
                GetDlgItemText(hWnd, IDC_SESSIONEDIT, strSessionName, NAMEMAX);
                lpSessionName = strSessionName;
        }
        else
        {
                iHost = FALSE;                  // we are joining an existing session
                lpSessionName = NULL;   // don't need a session name if we are joining

                lpAddress = NULL;               // assume we don't need address data
                dwAddressSize = 0;

                // the service provider supports address data
                if (!IsEqualGUID(&guidAddressType, &GUID_NULL))
                {
                        // get service-provider specific address data as a string
                        GetDlgItemText(hWnd, IDC_ADDRESSEDIT, strAddressText, NAMEMAX);

                        // convert string to a DirectPlay address
                        hr = CreateAddress(lpDPlayLobby, &guidServiceProvider,
                                                           &guidAddressType, strAddressText,
                                                           &lpAddress, &dwAddressSize);
                }
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

