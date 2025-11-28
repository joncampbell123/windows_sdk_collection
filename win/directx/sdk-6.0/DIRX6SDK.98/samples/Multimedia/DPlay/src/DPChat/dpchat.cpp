/*==========================================================================
 *
 *  Copyright (C) 1996-1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dpchat.cpp
 *  Content:	Simple chat program using DirectPlay.
 *
 ***************************************************************************/

#include <windows.h>
#include <windowsx.h>

#include "dpchat.h"
#include "resource.h"

#if defined(UNICODE) || defined(_UNICODE)
#error This app does not support UNICODE
#endif

// constants
const DWORD APPMSG_CHATSTRING	= 0;			// message type for chat string
const UINT	WM_USER_ADDSTRING	= WM_USER+257;	// window message to add string to chat string list
const DWORD	MAXSTRLEN			= 200;			// max size of a temporary string

// structures

// message structure used to send a chat string to another player
typedef struct {
	DWORD	dwType;								// message type (APPMSG_CHATSTRING)
	char	szMsg[1];							// message string (variable length)
} MSG_CHATSTRING, *LPMSG_CHATSTRING;

// globals
HANDLE			ghReceiveThread = NULL;			// handle of receive thread
DWORD			gidReceiveThread = 0;			// id of receive thread
HANDLE			ghKillReceiveEvent = NULL;		// event used to kill receive thread
HWND			ghChatWnd = NULL;				// main chat window

// prototypes
BOOL CALLBACK	ChatWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT			SetupConnection(HINSTANCE hInstance, LPDPLAYINFO lpDPInfo);
HRESULT			ShutdownConnection(LPDPLAYINFO lpDPInfo);
DWORD WINAPI	ReceiveThread(LPVOID lpThreadParameter);
HRESULT			ReceiveMessage(LPDPLAYINFO lpDPInfo);
void			HandleApplicationMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
										 DPID idFrom, DPID idTo);
void			HandleSystemMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
									DPID idFrom, DPID idTo);
HRESULT			GetChatPlayerName(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
								  LPDPNAME *lplpName);
HRESULT			SendChatMessage(HWND hWnd, LPDPLAYINFO lpDPInfo);
HRESULT			NewChatString(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
							  LPSTR lpszMsg, LPSTR *lplpszStr);
char *			GetDirectPlayErrStr(HRESULT hr);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow )
{
	DPLAYINFO	DPInfo;
	int			iResult = 0;
	HRESULT		hr;

	// Initialize COM library
	hr = CoInitialize(NULL);
	if ( FAILED(hr) )
		goto FAILURE;


	// setup the connection
	hr = SetupConnection(hInstance, &DPInfo);
	if FAILED(hr)
		goto FAILURE;

	// show the chat window
	iResult = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CHATDIALOG), NULL, (DLGPROC) ChatWndProc, (LPARAM) &DPInfo);

FAILURE:
	// shut down the connection
	hr = ShutdownConnection(&DPInfo);

	// Uninitialize the COM library
	CoUninitialize();

	return (iResult);
}

BOOL CALLBACK ChatWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPDPLAYINFO	lpDPInfo;
	DWORD				dwTextLen;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		// Save the connection info pointer
		lpDPInfo = (LPDPLAYINFO) lParam;

		// store global window
		ghChatWnd = hWnd;
		break;

	case WM_DESTROY:
		ghChatWnd = NULL;
		break;

	// this is a user-defined message used to add strings to the log window
	case WM_USER_ADDSTRING:
		// get length of text in log window
		dwTextLen = SendDlgItemMessage(hWnd, IDC_LOGEDIT, WM_GETTEXTLENGTH,
									   (WPARAM) 0, (LPARAM) 0);

		// put selection at end
		dwTextLen = SendDlgItemMessage(hWnd, IDC_LOGEDIT, EM_SETSEL,
									   (WPARAM) dwTextLen, (LPARAM) dwTextLen);

		// add string in lParam to log window
		SendDlgItemMessage(hWnd, IDC_LOGEDIT, EM_REPLACESEL,
							(WPARAM) FALSE, (LPARAM) lParam);
		GlobalFreePtr((LPVOID) lParam);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_SENDBUTTON:
			SendChatMessage(hWnd, lpDPInfo);
			break;			

		case IDCANCEL:
			EndDialog(hWnd, FALSE);
			break;
		}
		break;
	}

	// Allow for default processing
	return FALSE;
}

HRESULT SetupConnection(HINSTANCE hInstance, LPDPLAYINFO lpDPInfo)
{
	HRESULT		hr;

	ZeroMemory(lpDPInfo, sizeof(DPLAYINFO));

	// create event used by DirectPlay to signal a message has arrived
	lpDPInfo->hPlayerEvent = CreateEvent(NULL,		// no security
										 FALSE,		// auto reset
										 FALSE,		// initial event reset
										 NULL);		// no name
	if (lpDPInfo->hPlayerEvent == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// create event used to signal that the receive thread should exit
	ghKillReceiveEvent = CreateEvent(NULL,		// no security
									 FALSE,		// auto reset
									 FALSE,		// initial event reset
									 NULL);		// no name
	if (ghKillReceiveEvent == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// create thread to receive player messages
	ghReceiveThread = CreateThread(NULL,			// default security
								   0,				// default stack size
								   ReceiveThread,	// pointer to thread routine
								   lpDPInfo,		// argument for thread
								   0,				// start it right away
								   &gidReceiveThread);
	if (ghReceiveThread == NULL)
	{
		hr = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// try to connect using the lobby
	hr = ConnectUsingLobby(lpDPInfo);
	if FAILED(hr)
	{
		// if the error returned is DPERR_NOTLOBBIED, that means we
		// were not launched by a lobby and we should ask the user for
		// connection settings. If any other error is returned it means
		// we were launched by a lobby but there was an error making the
		// connection.

		if (hr != DPERR_NOTLOBBIED)
			ErrorBox("Could not connect using lobby because of error %s", hr);

		// if there is no lobby connection, ask the user for settings
		hr = ConnectUsingDialog(hInstance, lpDPInfo);
		if FAILED(hr)
			goto FAILURE;
	}

	return (DP_OK);	

FAILURE:
	ShutdownConnection(lpDPInfo);

	return (hr);
}

HRESULT ShutdownConnection(LPDPLAYINFO lpDPInfo)
{
	if (ghReceiveThread)
	{
		// wake up receive thread and wait for it to quit
		SetEvent(ghKillReceiveEvent);
		WaitForSingleObject(ghReceiveThread, INFINITE);

		CloseHandle(ghReceiveThread);
		ghReceiveThread = NULL;
	}

	if (ghKillReceiveEvent)
	{
		CloseHandle(ghKillReceiveEvent);
		ghKillReceiveEvent = NULL;
	}

	if (lpDPInfo->lpDirectPlay4A)
	{
		if (lpDPInfo->dpidPlayer)
		{
			lpDPInfo->lpDirectPlay4A->DestroyPlayer(lpDPInfo->dpidPlayer);
			lpDPInfo->dpidPlayer = 0;
		}
		lpDPInfo->lpDirectPlay4A->Close();
		lpDPInfo->lpDirectPlay4A->Release();
		lpDPInfo->lpDirectPlay4A = NULL;
	}

	if (lpDPInfo->hPlayerEvent)
	{
		CloseHandle(lpDPInfo->hPlayerEvent);
		lpDPInfo->hPlayerEvent = NULL;
	}

	return (DP_OK);
}

DWORD WINAPI ReceiveThread(LPVOID lpThreadParameter)
{
	LPDPLAYINFO	lpDPInfo = (LPDPLAYINFO) lpThreadParameter;
	HANDLE		eventHandles[2];

	eventHandles[0] = lpDPInfo->hPlayerEvent;
	eventHandles[1] = ghKillReceiveEvent;

	// loop waiting for player events. If the kill event is signaled
	// the thread will exit
	while (WaitForMultipleObjects(2, eventHandles, FALSE, INFINITE) == WAIT_OBJECT_0)
	{
		// receive any messages in the queue
		ReceiveMessage(lpDPInfo);
	}

	ExitThread(0);

	return (0);
}

HRESULT ReceiveMessage(LPDPLAYINFO lpDPInfo)
{
	DPID				idFrom, idTo;
	LPVOID				lpvMsgBuffer;
	DWORD				dwMsgBufferSize;
	HRESULT				hr;

	lpvMsgBuffer = NULL;
	dwMsgBufferSize = 0;

	// loop to read all messages in queue
	do
	{
		// loop until a single message is successfully read
		do
		{
			// read messages from any player, including system player
			idFrom = 0;
			idTo = 0;

			hr = lpDPInfo->lpDirectPlay4A->Receive(&idFrom, &idTo, DPRECEIVE_ALL,
												   lpvMsgBuffer, &dwMsgBufferSize);

			// not enough room, so resize buffer
			if (hr == DPERR_BUFFERTOOSMALL)
			{
				if (lpvMsgBuffer)
					GlobalFreePtr(lpvMsgBuffer);
				lpvMsgBuffer = GlobalAllocPtr(GHND, dwMsgBufferSize);
				if (lpvMsgBuffer == NULL)
					hr = DPERR_OUTOFMEMORY;
			}
		} while (hr == DPERR_BUFFERTOOSMALL);

		if ((SUCCEEDED(hr)) &&							// successfully read a message
			(dwMsgBufferSize >= sizeof(DPMSG_GENERIC)))	// and it is big enough
		{
			// check for system message
			if (idFrom == DPID_SYSMSG)
			{
				HandleSystemMessage(lpDPInfo, (LPDPMSG_GENERIC) lpvMsgBuffer,
									dwMsgBufferSize, idFrom, idTo);
			}
			else
			{
				HandleApplicationMessage(lpDPInfo, (LPDPMSG_GENERIC) lpvMsgBuffer,
										 dwMsgBufferSize, idFrom, idTo);
			}
		}
	} while (SUCCEEDED(hr));

	// free any memory we created
	if (lpvMsgBuffer)
		GlobalFreePtr(lpvMsgBuffer);

	return (DP_OK);
}

void HandleApplicationMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
							  DPID idFrom, DPID idTo)
{
	LPSTR	lpszStr = NULL;
	HRESULT	hr;

	switch (lpMsg->dwType)
	{
	case APPMSG_CHATSTRING:
		{
			LPMSG_CHATSTRING   lp = (LPMSG_CHATSTRING) lpMsg;

			// create string to display
			hr = NewChatString(lpDPInfo->lpDirectPlay4A, idFrom, lp->szMsg, &lpszStr);
			if FAILED(hr)
				break;
		}
		break;
	}

	// post string to chat window
	if (lpszStr)
	{
		// make sure window is still valid
		if (ghChatWnd)
			PostMessage(ghChatWnd, WM_USER_ADDSTRING, (WPARAM) 0, (LPARAM) lpszStr);
		else
			GlobalFreePtr(lpszStr);
	}
}

void HandleSystemMessage(LPDPLAYINFO lpDPInfo, LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
						 DPID idFrom, DPID idTo)
{
	LPSTR		lpszStr = NULL;

	// The body of each case is there so you can set a breakpoint and examine
	// the contents of the message received.
	switch (lpMsg->dwType)
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP 	lp = (LPDPMSG_CREATEPLAYERORGROUP) lpMsg;
			LPSTR							lpszPlayerName;
			LPSTR							szDisplayFormat = "\"%s\" has joined\r\n";
			
			// get pointer to player name
			if (lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) +
												   lstrlen(lpszPlayerName) + 1);
			if (lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr, szDisplayFormat, lpszPlayerName);
		}
		break;

	case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP	lp = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			LPSTR							lpszPlayerName;
			LPSTR							szDisplayFormat = "\"%s\" has left\r\n";
			
			// get pointer to player name
			if (lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) +
												   lstrlen(lpszPlayerName) + 1);
			if (lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr, szDisplayFormat, lpszPlayerName);
		}
		break;

	case DPSYS_ADDPLAYERTOGROUP:
		{
			LPDPMSG_ADDPLAYERTOGROUP lp = (LPDPMSG_ADDPLAYERTOGROUP)lpMsg;
		}
		break;

	case DPSYS_DELETEPLAYERFROMGROUP:
		{
			LPDPMSG_DELETEPLAYERFROMGROUP lp = (LPDPMSG_DELETEPLAYERFROMGROUP)lpMsg;
		}
		break;

	case DPSYS_SESSIONLOST:
		{
			LPDPMSG_SESSIONLOST lp = (LPDPMSG_SESSIONLOST)lpMsg;
		}
		break;

	case DPSYS_HOST:
		{
			LPDPMSG_HOST	lp = (LPDPMSG_HOST)lpMsg;
			LPSTR			szDisplayFormat = "You have become the host\r\n";

			// allocate space for string
			lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) + 1);
			if (lpszStr == NULL)
				break;

			// build string
			lstrcpy(lpszStr, szDisplayFormat);

			// we are now the host
			lpDPInfo->bIsHost = TRUE;
		}
		break;

	case DPSYS_SETPLAYERORGROUPDATA:
		{
			LPDPMSG_SETPLAYERORGROUPDATA lp = (LPDPMSG_SETPLAYERORGROUPDATA)lpMsg;
		}
		break;

	case DPSYS_SETPLAYERORGROUPNAME:
		{
			LPDPMSG_SETPLAYERORGROUPNAME lp = (LPDPMSG_SETPLAYERORGROUPNAME)lpMsg;
		}
		break;
	}

	// post string to chat window
	if (lpszStr)
	{
		// make sure window is still valid
		if (ghChatWnd)
			PostMessage(ghChatWnd, WM_USER_ADDSTRING, (WPARAM) 0, (LPARAM) lpszStr);
		else
			GlobalFreePtr(lpszStr);
	}
}

HRESULT GetChatPlayerName(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
						  LPDPNAME *lplpName)
{
	LPDPNAME	lpName = NULL;
	DWORD		dwNameSize;
	HRESULT		hr;

	// get size of player name data
	hr = lpDirectPlay4A->GetPlayerName(dpidPlayer, NULL, &dwNameSize);
	if (hr != DPERR_BUFFERTOOSMALL)
		goto FAILURE;

	// make room for it
	lpName = (LPDPNAME) GlobalAllocPtr(GHND, dwNameSize);
	if (lpName == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// get player name data
	hr = lpDirectPlay4A->GetPlayerName(dpidPlayer, lpName, &dwNameSize);
	if FAILED(hr)
		goto FAILURE;

	// return pointer to name structure
	*lplpName = lpName;

	return (DP_OK);

FAILURE:
	if (lpName)
		GlobalFreePtr(lpName);

	return (hr);
}

HRESULT SendChatMessage(HWND hWnd, LPDPLAYINFO lpDPInfo)
{
	LPSTR				lpszChatStr = NULL;
	LPSTR				lpszStr = NULL;
	LPMSG_CHATSTRING	lpChatMessage = NULL;
	DWORD				dwChatMessageSize;
	LONG				lStrLen;
	HRESULT				hr;

	// get length of item text
	lStrLen = SendDlgItemMessage(hWnd, IDC_SENDEDIT, WM_GETTEXTLENGTH,
								(WPARAM) 0, (LPARAM) 0);

	// make room for it
	lpszChatStr = (LPSTR) GlobalAllocPtr(GHND, lStrLen + 1);
	if (lpszChatStr == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// get item text
	lStrLen = GetDlgItemText(hWnd, IDC_SENDEDIT, lpszChatStr, lStrLen + 1);

	// create string to display this text
	hr = NewChatString(lpDPInfo->lpDirectPlay4A, lpDPInfo->dpidPlayer, lpszChatStr, &lpszStr);
	if FAILED(hr)
		goto FAILURE;

	// display this string
	PostMessage(hWnd, WM_USER_ADDSTRING, (WPARAM) 0, (LPARAM) lpszStr);
	lpszStr = NULL;						// set to NULL so we don't delete it below

	// create space for message plus string (string length included in message header)
	dwChatMessageSize = sizeof(MSG_CHATSTRING) + lstrlen(lpszChatStr);
	lpChatMessage = (LPMSG_CHATSTRING) GlobalAllocPtr(GHND, dwChatMessageSize);
	if (lpChatMessage == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpChatMessage->dwType = APPMSG_CHATSTRING;
	lstrcpy(lpChatMessage->szMsg, lpszChatStr);
	
	// send this string to all other players
	hr = lpDPInfo->lpDirectPlay4A->Send(lpDPInfo->dpidPlayer, DPID_ALLPLAYERS,
										DPSEND_GUARANTEED, lpChatMessage, dwChatMessageSize);
	if FAILED(hr)
		goto FAILURE;

FAILURE:
	if (lpszChatStr)
		GlobalFreePtr(lpszChatStr);

	if (lpszStr)
		GlobalFreePtr(lpszStr);

	if (lpChatMessage)
		GlobalFreePtr(lpChatMessage);

	SetDlgItemText(hWnd, IDC_SENDEDIT, "");

	return (hr);
}

HRESULT NewChatString(LPDIRECTPLAY4A lpDirectPlay4A, DPID dpidPlayer,
					  LPSTR lpszMsg, LPSTR *lplpszStr)
{
	LPDPNAME	lpName = NULL;
	LPSTR		lpszStr = NULL;
	LPSTR		lpszPlayerName;
	LPSTR		szDisplayFormat = "%s>\t%s\r\n";
	HRESULT		hr;
	
	// get name of player
	hr = GetChatPlayerName(lpDirectPlay4A, dpidPlayer, &lpName);
	if FAILED(hr)
		goto FAILURE;

	if (lpName->lpszShortNameA)
		lpszPlayerName = lpName->lpszShortNameA;
	else
		lpszPlayerName = "unknown";

	// allocate space for display string
	lpszStr = (LPSTR) GlobalAllocPtr(GHND, lstrlen(szDisplayFormat) +
										   lstrlen(lpszPlayerName) +
										   lstrlen(lpszMsg) + 1);
	if (lpszStr == NULL)
	{
		hr = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build string
	wsprintf(lpszStr, szDisplayFormat, lpszPlayerName, lpszMsg);

	*lplpszStr = lpszStr;
	lpszStr = NULL;

FAILURE:
	if (lpszStr)
		GlobalFreePtr(lpszStr);
	
	if (lpName)
		GlobalFreePtr(lpName);

	return (hr);
}

void ErrorBox(LPSTR lpszErrorStr, HRESULT hr)
{
	char	szStr[MAXSTRLEN];

	wsprintf(szStr, lpszErrorStr, GetDirectPlayErrStr(hr));

	MessageBox(NULL, szStr, "Error", MB_OK);
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

