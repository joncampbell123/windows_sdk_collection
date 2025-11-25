/*===========================================================================*\
|
|  File:        cgremote.cpp
|
|  Description:
|   Routines to send and receive remote actions for controlling objects
|   on remote machines.  Uses DirectPlay for transferring data.
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

//** include files **
#include <stdlib.h>
#ifndef __WATCOMC__
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "cgglobl.h" // For ghInst for dialog box
#include "cgrsrce.h"
#include "dplay.h"
#include "cglevel.h"
#include "cgremote.h"
#include "strrec.h"

#include <stdarg.h>
#include <stdio.h>

static void InitReceivePoll( void );

// [johnhall]
// hack -- get GENERIC_CHAR_INFO from cgkrusty.cpp
typedef signed char SCHAR;

typedef struct 
{
    LONG    posx;
    LONG    posy;
    SCHAR   state;
    SCHAR   velx;
    SCHAR   vely;
    SCHAR   curZ;
} GENERIC_CHAR_INFO;

//** local definitions **
// structure to be used to pass remote actions across the link
typedef struct _GAMEMESSAGE {
    BYTE                    Action;     // action code
    BYTE            NumBytes;   // size of data 
    REMOTE_OBJECT       RemObj;     // unique object id
    char            Data[1];    // action specific data
} GAMEMESSAGE, *PGAMEMESSAGE;


//** external functions **

//** external data **
// KLUDGE:
CGameLevel  *gCurLevel=NULL;

//** public data **
//** private data **
static  IDirectPlay *lpIDC=NULL;    // DirectPlay Object
static  DPID    dcoID=0;        // our DirectPlay ID
static  HANDLE  dphEvent = NULL;

static BOOL fAbort = FALSE; // Abort flag for RemoteConnect()

//** public functions **
//** private functions **
static DWORD RequestThreadProc(void *Dummy);
static BOOL CALLBACK AbortDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


#define DP_BROADCAST_ID  0

// ----------------------------------------------------------
// CreateRemotePeer - create a new character across all peers
// ----------------------------------------------------------
REMOTE_OBJECT *CreateRemotePeers(
    char    *name,      // name of object as per level->Add() 
    DWORD   InstanceID  // unique instance ID of object type
)
{
    // Gotta have a DCO object to do this!
    if (lpIDC == NULL)
    {
        return(NULL);
    }

    // Setup a message and an unique obect id
    char *pBuffer = NULL;
    DWORD lenBuff;
    PGAMEMESSAGE    pGameMsg;
    REMOTE_OBJECT *pObj = new REMOTE_OBJECT;
    memset(pObj, 0, sizeof(REMOTE_OBJECT));

    // Make buffer big enough to hold game message plus
    // leading non-system-char byte
    lenBuff = sizeof(GAMEMESSAGE);
    pBuffer = new char[lenBuff];
    pGameMsg = (PGAMEMESSAGE)&pBuffer[0];

    // Fill in object id info so that this object is unique across
    // all machines
    pObj->OwnerID = (BYTE) dcoID;
    pObj->InstanceID = (BYTE) InstanceID;
    lstrcpyn(pObj->ObjectID, name, MAX_OBJ_NAME);
    memcpy(&pGameMsg->RemObj, pObj, sizeof(REMOTE_OBJECT));

    pGameMsg->Action = (BYTE) CREATE_OBJECT;

    // Broadcast it to everyone in the group.
    lpIDC->Send( dcoID,  // From
                 DP_BROADCAST_ID,
                 0,
                 (LPSTR)pBuffer,
                 lenBuff);  

    // Delete buffer, once sent
    delete []pBuffer;

    return(pObj);
}   





// ----------------------------------------------------------
// SendRemoteAction - broadcast an action to remote peers
// ----------------------------------------------------------
BOOL SendRemoteAction(
    REMOTE_OBJECT   *pObj,      // unique object id
    ACTION      Action,     // action code 
    void        *Data,      // action data
    DWORD       nDataSize   // size of action data
)
{
    PGAMEMESSAGE    gameMsg;
    char *pBuffer;
    DWORD lenBuff;
      //   char chBuffer[128];



    // Gotta have a valid DirectPlay object
    if (lpIDC == NULL)
    {
        return(NULL);
    }

    // Allocate the GAME buffer with room for data,
    // plus room at the beginning for a non-sytem-message char
    lenBuff = sizeof(GAMEMESSAGE) + nDataSize; 
    pBuffer = new char [lenBuff]; 
    // Now point gameMsg at the rest of the buffer
    gameMsg = (PGAMEMESSAGE)&pBuffer[0];

    // Copy the action code and data to message buffer
    gameMsg->Action = Action;
    memcpy(&gameMsg->RemObj, pObj, sizeof(REMOTE_OBJECT));
    gameMsg->NumBytes = (BYTE) nDataSize;
    memcpy(gameMsg->Data, Data, nDataSize);
#if 0
        wsprintf( chBuffer, "%d (10%s) %d %d %d",
            gameMsg->Action,
            gameMsg->RemObj.ObjectID,
            gameMsg->RemObj.InstanceID,
            gameMsg->RemObj.OwnerID,
            gameMsg->NumBytes);
        OutputDebugString(chBuffer);
        if (nDataSize == sizeof(GENERIC_CHAR_INFO))
        {
            GENERIC_CHAR_INFO *pci;
            pci = (GENERIC_CHAR_INFO *) Data;
            wsprintf( chBuffer,"; %d %d %d %d %d %d",
                pci->state,
                pci->posx,
                pci->posy,
                pci->velx,
                pci->vely,
                pci->curZ);
            OutputDebugString(chBuffer);
        }
        OutputDebugString("\r\n");
#endif

    // Broadcast the action to all peers
    lpIDC->Send(dcoID, // from
                DP_BROADCAST_ID, // to
                0,
                pBuffer,
                lenBuff);   
    delete []pBuffer;
    return(TRUE);
}   

// ----------------------------------------------------------
// DestroyRemotePeer - Tell remote peers to kill an object
// ----------------------------------------------------------
BOOL DestroyRemotePeer(
    REMOTE_OBJECT *pObj     // unique object id
)
{
    PGAMEMESSAGE    gameMsg;
    char *pBuffer = NULL;
    DWORD lenBuff;

    if (pObj == NULL)
        return(TRUE);

    // Allocate a buffer for the GAME buffer with roon for data,
    // plus an extra character at the beginning to be the non-system
    // message char.
    lenBuff = sizeof(GAMEMESSAGE)+1; 
    pBuffer = new char [lenBuff];

    // Point the game message pointer at the rest of the buffer
    gameMsg = (PGAMEMESSAGE)&pBuffer[0];

    // Format a destroy message
    memcpy(&gameMsg->RemObj, pObj, sizeof(REMOTE_OBJECT));
    gameMsg->RemObj.OwnerID = (BYTE) dcoID;
    gameMsg->NumBytes = 0;
    gameMsg->Action = (BYTE) DESTROY_OBJECT;

    // Broadcast the destroy message to all peers
    lpIDC->Send( dcoID, 
                 DP_BROADCAST_ID,
                 0,
                 pBuffer,
                 lenBuff);  
    // Delete buffer
    delete []pBuffer;

    // Don't need the object id anymore
    delete pObj;
    return(TRUE);
}   

// -----------------------------------------------------------------
// RemoteConnect - establish an active connection with remotes
// -----------------------------------------------------------------

BOOL FAR PASCAL EnumSession(
        LPDPSESSIONDESC lpDPGameDesc,
        LPVOID pContext,
        LPDWORD pTimeOut,
        DWORD dwFlags )
{
    LONG iIndex;
    HWND hWnd = (HWND) pContext;

    if( dwFlags == DPESC_TIMEDOUT )
    {
    return FALSE;
    }

    iIndex = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM) lpDPGameDesc->szSessionName);
    if (iIndex != LB_ERR)
        SendMessage(hWnd, LB_SETITEMDATA, iIndex, (LPARAM) lpDPGameDesc->dwSession);

    SetFocus(hWnd);
    SendMessage(hWnd, LB_SETCURSEL, 0, 0);
    return(TRUE);

}

BOOL FAR PASCAL EnumSP(LPGUID lpGuid, LPSTR lpDesc,
            DWORD dwMajor, DWORD dwMinor, LPVOID lpv)
{
    LONG iIndex;
    HWND hWnd = (HWND) lpv;

    iIndex = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM) lpDesc);
    if (iIndex != LB_ERR)
        SendMessage(hWnd, LB_SETITEMDATA, iIndex, (LPARAM) lpGuid);

    SetFocus(hWnd);
    SendMessage(hWnd, LB_SETCURSEL, 0, 0);
    return(TRUE);
}



BOOL CALLBACK DlgProcQCreate (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch(wParam)
        {
        case IDC_CREATE:
            EndDialog(hDlg, 1);
            return(TRUE);

        case IDC_CONNECT:
            EndDialog(hDlg, 2);
            return(TRUE);

        case IDCANCEL:
            EndDialog(hDlg, -1);
            return(TRUE);
        }
        break;

    }
    return(FALSE);
}
BOOL CALLBACK DlgProcChooseProvider (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPGUID  lpGuid;
    static  LONG    iIndex;
    static  HWND hWndCtl;

    switch (msg)
    {
    case WM_INITDIALOG:

        hWndCtl = GetDlgItem(hDlg, IDC_LIST1);
        if (hWndCtl == NULL)
        {
            EndDialog(hDlg, TRUE);
            return(TRUE);
        }
        DirectPlayEnumerate(EnumSP, (LPVOID) hWndCtl);
        SetFocus(hWndCtl);
        SendMessage(hWndCtl, LB_SETCURSEL, 0, 0);
        return(FALSE);

    case WM_COMMAND:

        switch( HIWORD(wParam))
        {
        case LBN_SELCHANGE:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            hWndCtl = (HWND) lParam;
            return(FALSE);

        case LBN_DBLCLK:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            if (iIndex != LB_ERR)
            {
                lpGuid = (LPGUID) SendMessage((HWND) lParam, LB_GETITEMDATA, iIndex, 0);
                DirectPlayCreate(lpGuid, &lpIDC, NULL);
                EndDialog(hDlg, TRUE);
                return(TRUE);
            }
            break;

        case 0:
            if (LOWORD(wParam) == IDOK)
            {
                if (iIndex != LB_ERR)
                {
                    lpGuid = (LPGUID) SendMessage(hWndCtl, LB_GETITEMDATA, iIndex, 0);
                    if (lpGuid)
                    {
                        DirectPlayCreate(lpGuid, &lpIDC, NULL);
                        EndDialog(hDlg, TRUE);
                    }
                    else
                        EndDialog(hDlg, FALSE);
                    return(TRUE);
                }
            }
            else if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, FALSE);
                return(TRUE);
            }
            break;

        }
    }
    return (FALSE);
}

LPGUID g_lpGuid;

BOOL CALLBACK DlgProcSelSession (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static  LONG    iIndex;
    static  HWND hWndCtl;
    DPSESSIONDESC dpDesc;
    HRESULT hr = DP_OK + 10;

    switch (msg)
    {
    case WM_INITDIALOG:

        hWndCtl = GetDlgItem(hDlg, IDC_LB_SESSION);
        if (hWndCtl == NULL)
        {
            EndDialog(hDlg, TRUE);
            return(TRUE);
        }
        memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
        dpDesc.dwSize = sizeof(dpDesc);
        dpDesc.guidSession = *g_lpGuid;
        lpIDC->EnumSessions(&dpDesc, 5000, EnumSession, (LPVOID) hWndCtl, 0 );

        SetFocus(hWndCtl);
        return(FALSE);

    case WM_COMMAND:

        switch( HIWORD(wParam))
        {
        case LBN_SELCHANGE:
            iIndex = SendMessage((HWND) lParam, LB_GETCURSEL, 0, 0);
            hWndCtl = (HWND) lParam;
            return(FALSE);

        case 0:
            if (LOWORD(wParam) == IDCANCEL)
            {
                lpIDC->Close();
                lpIDC->Release();
                lpIDC = NULL;
                EndDialog(hDlg, FALSE);
                return(TRUE);
            }
            //
            // Fall Through.
            //
        case LBN_DBLCLK:
            if (HIWORD(wParam) == LBN_DBLCLK)
            {
                hWndCtl = (HWND) lParam;
                iIndex = SendMessage(hWndCtl, LB_GETCURSEL, 0, 0);
            }

            if (iIndex != LB_ERR)
            {
                memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
                dpDesc.dwSize       = sizeof(dpDesc);
                dpDesc.guidSession  = *g_lpGuid;
                dpDesc.dwFlags      = DPOPEN_OPENSESSION;
                dpDesc.dwSession    = SendMessage((HWND) hWndCtl, LB_GETITEMDATA, iIndex, 0);
                hr = lpIDC->Open(&dpDesc);

                if (hr != DP_OK)
                {
                    lpIDC->Close();
                    lpIDC->Release();
                    lpIDC = NULL;
                    EndDialog(hDlg, FALSE);
                }

                EndDialog(hDlg, TRUE);
                return(TRUE);

            }
        }
    }
    return (FALSE);
}

INT GetProvider()
{

  return(DialogBox (NULL, (LPCTSTR) IDD_CHOOSEPROVIDER, NULL, (DLGPROC) DlgProcChooseProvider));
}

INT CreateGame()
{
  return(DialogBox (NULL, (LPCTSTR) IDD_Q_CREATE, NULL, (DLGPROC) DlgProcQCreate));
    
}

INT GetGame()
{

  return(DialogBox (NULL, (LPCTSTR) IDD_SELSESSION, NULL, (DLGPROC) DlgProcSelSession));
}



BOOL RemoteCreate(REFGUID pGuid, LPSTR FullName, LPSTR NickName)
{
    HRESULT hr;
    DPSESSIONDESC dpDesc;

    // Be sure we aren't already initialized.
    if (lpIDC != NULL)
    {
        return( FALSE );
    }

    GetProvider();

    if (lpIDC == NULL)
        return(FALSE);

    switch( CreateGame())
    {
    case 1:             // Create
        memset(&dpDesc, 0x00, sizeof(DPSESSIONDESC));
        dpDesc.dwSize = sizeof(dpDesc);
        dpDesc.dwMaxPlayers = 10;
        dpDesc.dwFlags = DPOPEN_CREATESESSION;
        dpDesc.guidSession = pGuid;
        strcpy( dpDesc.szSessionName, FullName);
        
        if ((hr = lpIDC->Open(&dpDesc)) != DP_OK)
        {
            lpIDC->Release();
            lpIDC = NULL;
            return(FALSE);
        }
        
        break;

    case 2:             // Connect
        g_lpGuid = (LPGUID) &pGuid;

        GetGame();

        if (lpIDC == NULL)
            return(FALSE);

        break;

    default:
        return(FALSE);
    }

    if ((hr = lpIDC->CreatePlayer(&dcoID, NickName,
                                  "IKlowns Player", &dphEvent)) != DP_OK)
    {
        lpIDC->Close();
        lpIDC->Release();
        lpIDC = NULL;
        return(FALSE);
    }
    
    
    InitReceivePoll();
    return(TRUE);

}


// -----------------------------------------------------------------
// SetCurrentLevel - set pointer to current level object
// -----------------------------------------------------------------
void SetCurrentLevel(
    void    *newLevel
)
{
    gCurLevel = (CGameLevel *)newLevel;
}

// ----------------------------------------------------------
// CreateNewCharacter - create a new game object!
// ----------------------------------------------------------
void CreateNewCharacter(
    REMOTE_OBJECT *pObj // unique object id
)
{
    char dataBuf[256];
    char graphicsBuf[256];

    // Get specific data on object from profile
    // NOTE: This means we hit the disk.  For performance,
    // it would be better if objects could be created based
    // on an in memory object so that we don't ever have to
    // hit the disk!


    GetPrivateProfileString(
            gCurLevel->GetLevelName(),
            "Graphics",
            "",
            graphicsBuf,
            sizeof( graphicsBuf ),
            gCurLevel->GetProfileName()
            );

    GetPrivateProfileString(
            graphicsBuf,
            pObj->ObjectID,
            "",
            dataBuf,
            sizeof( dataBuf ),
            gCurLevel->GetProfileName()
            );

    // parse the data string into fields
    CStringRecord fields( dataBuf, "," );

    if (fields.GetNumFields() >= 5)
    {
        // Add the object to the game list!
        gCurLevel->Add(pObj->ObjectID,
            atoi(fields[1]), 
            atoi(fields[3]),
            atoi(fields[4]),
            (void *)pObj);
    }
}

// ----------------------------------------------------------
// ProcessIncomingActions - Parse received messages & queue them
// ----------------------------------------------------------
BOOL ProcessIncomingActions(
    REMOTE_OBJECT *pObj,    // unique object id
    ACTION action,      // action code
    void *Data,     // action data
    DWORD nDataSize     // sizeof action data
)
{
    CLinkedList *ActionList;
    BOOL        fQueuedUp = FALSE;
        // char            chBuffer[128];

        // wsprintf( chBuffer, "IncomingAction %d size %d\r\n", action, nDataSize);
        // OutputDebugString(chBuffer);

    // Guard against activity occurring before we are ready!
    if (gCurLevel == NULL)
        return(FALSE);

    // Don't do anything about our own requests
    if (pObj->OwnerID == dcoID)
        return(FALSE);
        
    // Figure out what to do with this message
    switch (action) {

    // Need to create a brand new object locally
        case CREATE_OBJECT: {

                // OutputDebugString("CreateObject \r\n");
        // Create a game character
        if (!FindRemoteObjectEntry(pObj))
        {
                        // OutputDebugString("CreateNewCharacter \r\n");
            CreateNewCharacter(pObj);
        }
                // OutputDebugString("Leave CreateObject \r\n");
        break;
    }

    // Time to destroy the object!
    case DESTROY_OBJECT:    {
        REMOTE_DATA *pRemoteEntry = FindRemoteObjectEntry(pObj);

                // OutputDebugString("DestroyObject \r\n");
        // Need to tell level object to remove the character!
        if (pRemoteEntry != NULL)
            gCurLevel->Remove((CGameGraphic *)pRemoteEntry->Data);

        break;
    }

    // Must be some character specific action
    default:{

        // Get action list for this object
        ActionList = GetRemoteObjectQueue(pObj);

        // No action list -> a new object, so try to create it.
        if (ActionList == NULL)
        {
            CreateNewCharacter(pObj);
            ActionList = GetRemoteObjectQueue(pObj);
            if (ActionList == NULL)
                return(FALSE);
        }

        // Place this action onto the queue
                // OutputDebugString("QueueRemoteAction \r\n");
        QueueRemoteAction(ActionList, action, Data, nDataSize);
        fQueuedUp = TRUE;
        break;
    }
    }
    return(fQueuedUp);
}

// ----------------------------------------------------------
// ReleaseRemoteData - free remote buffer
// ----------------------------------------------------------
void ReleaseRemoteData(
    LPVOID  pData       // ptr to app data portion
)
{
    PGAMEMESSAGE    pMsg;   // desired pointer
    int     iDiff;  // difference between the two

    // Pretend we got passed in the proper ptr
    pMsg = (PGAMEMESSAGE)pData;

    // Calculate the difference between what we got
    // and what we need.
    iDiff = (LPBYTE)(&pMsg->Data) - (LPBYTE)(pMsg);

    // Adjust accordingly and free the message!
    pMsg = (PGAMEMESSAGE)((LPBYTE)pMsg - iDiff);
    delete pMsg;
}

#define MAX_BUFFER_SIZE (sizeof(GAMEMESSAGE)+512)




static char *pBuffer = NULL;
static DWORD lenBuff = MAX_BUFFER_SIZE; 

static void InitReceivePoll( void )
{
    // We allocate a buffer to receive into first,
    // which may have to cope with system messages,
    // before copying the real message to the buffer to pass
    // to the game
    pBuffer = new char[ lenBuff ];

}

void PollForRemoteReceive( void )
{
    DPID        fromID, dcoReceiveID;
    DWORD       nBytes;
    PGAMEMESSAGE    pMsg;
    int         i;
    const       int MAX_MESSAGES = 16;
    BOOL        fCheckForMore = TRUE;

    // Paranoia check
    if ( lpIDC )
    {
        // We try to receive MAX_MESSAGES at a time so that
        // a backlog doesn't build up.  If we run out of messages,
        // we stop looking for them.

        for( i = 0; i < MAX_MESSAGES && fCheckForMore ; i++ )
        {
            HRESULT status;
            nBytes = lenBuff;
            status = lpIDC->Receive(
                        &fromID,
                        &dcoReceiveID,
                        DPRECEIVE_ALL,
                        pBuffer,
                        &nBytes);
            switch( status )
            {
            case DP_OK:
                if ( fromID == 0 )
                {
                    // We do not in fact utilise the system
                    // messages during the game.
                }
                else
                {
                    // User message - we copy the buffer minus the
                    // user message byte to the game buffer, then 
                    // pass it on.
        
                    // Allocate buffer for game message
                    pMsg = (PGAMEMESSAGE)new char [MAX_BUFFER_SIZE];
                    memcpy( pMsg, &pBuffer[0], nBytes );
        
                if (!ProcessIncomingActions(&pMsg->RemObj, pMsg->Action,
                                pMsg->Data, nBytes))
                        delete pMsg;
                }
            break;

            default:
                // Error condition of some kind - we just stop
                // checking for now
                fCheckForMore = FALSE;
                break;
            }
        }
    }
}


BOOL CALLBACK AbortDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    WORD wID, wNotifyCode;
    BOOL retVal = 0;

    switch( uMsg )
    {
        case WM_INITDIALOG:
            // We didn't set focus
            retVal = 1;
            break;
        case WM_COMMAND:
            wID = LOWORD( wParam );
            wNotifyCode = HIWORD( wParam );
            switch ( wID )
            {
                case IDCANCEL:
                    if ( BN_CLICKED == wNotifyCode ) {
                        fAbort = TRUE;
                        retVal = 1;
                    }
                    break;
                default:
                    break;
            }   
            break;
    }
    return( retVal );
}
