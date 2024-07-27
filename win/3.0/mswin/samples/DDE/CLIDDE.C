/****************************************************************************

    MODULE: CLIDDE.C

    PURPOSE: Processes incoming and outgoing DDE messages


****************************************************************************/

#include "windows.h"

#include "dde.h"
#include "clires.h"
#include "client.h"
#include <string.h>

#define DEFAULT_ACK_TIME_OUT_MILLISEC 10000
static int nAckTimeOut;

static BOOL   bInInitiate = FALSE;

BOOL NEAR AwaitingAck(HWND);


/****************************************************************************

    FUNCTION: AwaitingAck

    PURPOSE:  Inform user if acknowledgement is required before further 
              action.

****************************************************************************/
BOOL NEAR AwaitingAck(hwndClientDDE)
    HWND hwndClientDDE;
{
    if (GetConvPendingAck(hwndClientDDE) == NONE)
    {
        return (FALSE);
    }
    MessageBox(hwndMain,
        "Previous DDE operation must be acknowledged first",
        "Client",
        MB_ICONEXCLAMATION | MB_OK);
    return (TRUE);
}



/****************************************************************************

    FUNCTION: ClientAcknowledge

    PURPOSE:  Called when client application receives WM_DDE_ACK message
	      or WM_TIMER message (time out on wait for ACK).

****************************************************************************/
void ClientAcknowledge(hwndClientDDE, hwndServerDDE, lParam, bTimeOut)
    HWND hwndClientDDE;
    HWND hwndServerDDE;
    LONG lParam;    /* lParam of WM_DDE_ACK message */
    BOOL bTimeOut;  /* TRUE if NACK is due to time-out */
{
    enum PENDINGACK ePendingAck;
    char szApplication[APP_MAX_SIZE+1];
    char szTopic[TOPIC_MAX_SIZE+1];
    char szItem[ITEM_MAX_SIZE+1];
    char message[80];

    ePendingAck = GetConvPendingAck(hwndClientDDE);
    SetConvPendingAck(hwndClientDDE, NONE);
    KillTimer(hwndClientDDE, hwndServerDDE);

    if (bInInitiate)
    {
        GlobalGetAtomName(LOWORD(lParam),
            szApplication,
            APP_MAX_SIZE);
        GlobalGetAtomName(HIWORD(lParam),
            szTopic,
            TOPIC_MAX_SIZE);
	if (!AddConv(hwndClientDDE, hwndServerDDE, szApplication, szTopic))
        {
	    MessageBox(hwndMain,
		"Maximum conversation count exceeded",
                "Client",
                MB_ICONEXCLAMATION | MB_OK);
        }
	/*
	GlobalDeleteAtom(LOWORD(lParam));
        GlobalDeleteAtom(HIWORD(lParam));
	*/
        return;
    }
    if ((ePendingAck == ADVISE) && (LOWORD(lParam) & 0x8000))
    {	/* received positive ACK in response to ADVISE */
        GlobalGetAtomName(HIWORD(lParam), szItem, ITEM_MAX_SIZE);
	AddItemToConv(hwndClientDDE, szItem);

	/* Conversation item is established: now get current value */
	/* and update screen					  */
	SendRequest(hwndClientDDE, hwndServerDDE, szItem);
	InvalidateRect(hwndMain, NULL, TRUE);
    }
    if ((ePendingAck == UNADVISE) && (LOWORD(lParam) & 0x8000))
    {	/* received positive ACK in response to UNADVISE */
        GlobalGetAtomName(HIWORD(lParam), szItem, ITEM_MAX_SIZE);
	RemoveItemFromConv(hwndClientDDE, szItem);
	InvalidateRect(hwndMain, NULL, TRUE);
    }
    if (!(LOWORD(lParam) & 0x8000))    /* NACK */
    {
        strcpy(message, "DDE ");
        strcat(message, ePendingAck == ADVISE?    "ADVISE "
                      : ePendingAck == UNADVISE?  "UNADVISE "
                      : ePendingAck == POKE?      "POKE "
                      : ePendingAck == REQUEST?   "REQUEST "
		      : ePendingAck == EXECUTE?   "EXECUTE "
                      : " ");
        strcat(message, bTimeOut? "acknowledge time out"
                                : "operation failed");
	MessageBox(hwndMain,
            message,
            "Client",
            MB_ICONEXCLAMATION | MB_OK); 
    }
    switch (ePendingAck)
    {
	case ADVISE:
	case UNADVISE:
	case POKE:
	case REQUEST:
	    if (HIWORD(lParam))  /* will not be available for time-out */
		GlobalDeleteAtom(HIWORD(lParam));
	    break;
	case EXECUTE:
	    GlobalFree(HIWORD(lParam)); /* hCommand (execute string) */
	    break;
    }
    return;
}



/****************************************************************************

    FUNCTION: ClientReceiveData

    PURPOSE:  Called when client application receives WM_DDE_DATA message.

****************************************************************************/
void ClientReceiveData(hwndClientDDE, hwndServerDDE, lParam)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
    LONG  lParam;
{
    DDEDATA FAR * lpDDEData;
    char          szItem[ITEM_MAX_SIZE+1];
    BOOL          bRelease;
    BOOL          bAck;

    if (IsConvInTerminateState(hwndClientDDE, hwndServerDDE))
    { /* Terminate in progress: do not receive data */
        GlobalFree(LOWORD(lParam));
        GlobalDeleteAtom(HIWORD(lParam));
        return;
    }

    if (GetConvPendingAck(hwndClientDDE) == REQUEST)
    {
	SetConvPendingAck(hwndClientDDE, NONE);
	KillTimer(hwndClientDDE, hwndServerDDE);
    }

    if (!(lpDDEData = (DDEDATA FAR *)GlobalLock(LOWORD(lParam)))
        || (lpDDEData->cfFormat != CF_TEXT))
    {
	PostMessage(hwndServerDDE,
            WM_DDE_ACK,
	    hwndClientDDE,
            MAKELONG(0, HIWORD(lParam)));  /* Negative ACK */
    }
    bAck = FALSE;
    if (IsInRequestDlg())
    {
	/* Update REQUEST dialog box value */
	RequestSatisfied(lpDDEData->Value);
        bAck = TRUE;
    }
    else 
    {
        GlobalGetAtomName(HIWORD(lParam), szItem, ITEM_MAX_SIZE);
	bAck = SetConvItemValue(hwndClientDDE, szItem, lpDDEData->Value);
    }
    if (lpDDEData->fAckReq)
    {
	/* return ACK or NACK */
	PostMessage(hwndServerDDE,
            WM_DDE_ACK,
	    hwndClientDDE,
	    MAKELONG( (bAck? 0x8000:0), HIWORD(lParam)));
    }
    bRelease = lpDDEData->fRelease;
    GlobalUnlock(LOWORD(lParam));
    if (bRelease)
        GlobalFree(LOWORD(lParam));
    return;
}
            

/****************************************************************************

    FUNCTION: ClientTerminate

    PURPOSE:  Called when client application receives WM_DDE_TERMINATE
	      message.

****************************************************************************/
void ClientTerminate(hwndClientDDE, hwndServerDDE)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
{
    if (!IsConvInTerminateState(hwndClientDDE, hwndServerDDE))
    { /* Server has requested terminate: respond with terminate */
	PostMessage(hwndServerDDE, WM_DDE_TERMINATE, hwndClientDDE, 0L);
    }
    RemoveConv(hwndClientDDE, hwndServerDDE);
    if (!IsHwndClientDDEUsed(hwndClientDDE))
	DestroyWindow(hwndClientDDE);
    InvalidateRect(hwndMain, NULL, TRUE);
    return;
}



/****************************************************************************

    FUNCTION: DDEWndProc

    PURPOSE:  Handles all DDE messages received by the client application.

****************************************************************************/
long FAR PASCAL DDEWndProc(hwnd, message, wParam, lParam)
    HWND      hwnd;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    switch (message)
    {
	case WM_DDE_ACK:
	    ClientAcknowledge(hwnd,(HWND)wParam, lParam, FALSE);
	    return (0L);

	case WM_TIMER:
	    /* Negative ACK because of time out */
	    ClientAcknowledge(hwnd,(HWND)wParam, 0L, TRUE);
	    return (0L);

	 case WM_DDE_DATA:
	     ClientReceiveData(hwnd,(HWND)wParam, lParam);
	     return (0L);

	 case WM_DDE_TERMINATE:
	      ClientTerminate(hwnd,(HWND)wParam);
	      return (0L);

	 default:
	      return (DefWindowProc(hwnd, message, wParam, lParam));
    }
}



/****************************************************************************

    FUNCTION: DoPasteLink

    PURPOSE:  Get conversation information (app/topic/item) from clipboard.
	      Initiate conversation, if not already initiated.
	      Then, request server to advise the specified item.

	      Note, the standard clipboard format registered with the
	      name "Link" is as follows:

	      <app> <null> <topic> <null> <item> <null> <null>

****************************************************************************/
void DoPasteLink(void)
{

    HANDLE hData;
    LPSTR  lpData;
    HWND   hwndClientDDE;
    HWND   hwndServerDDE;
    char   szApplication[APP_MAX_SIZE+1];
    char   szTopic[TOPIC_MAX_SIZE+1];
    char   szItem[ITEM_MAX_SIZE+1];
    int    nBufLen;
   
    if (OpenClipboard(hwndMain))
    {
       if (!(hData = GetClipboardData(cfLink)) ||
	  !(lpData = GlobalLock(hData)))
       {
          CloseClipboard();
	  return;
       }


       /* Parse clipboard data */
       if ((nBufLen = lstrlen(lpData)) >= APP_MAX_SIZE)
       {
	    CloseClipboard();
	    GlobalUnlock(hData);
	    return;
       }
       lstrcpy(szApplication, lpData);
       lpData += (nBufLen+1); /* skip over null */
       if ((nBufLen = lstrlen(lpData)) >= TOPIC_MAX_SIZE)
       {
	    CloseClipboard();
	    GlobalUnlock(hData);
	    return;
       }
       lstrcpy(szTopic, lpData);
       lpData += (nBufLen+1); /* skip over null */
       if ((nBufLen = lstrlen(lpData)) >= ITEM_MAX_SIZE)
       {
	    CloseClipboard();
	    GlobalUnlock(hData);
	    return;
       }
       lstrcpy(szItem, lpData);

       GlobalUnlock(hData);
       CloseClipboard();

       if (hwndClientDDE = FindConvGivenAppTopic(szApplication, szTopic))
       {   /* app/topic conversation already started */
	   if (DoesAdviseAlreadyExist(hwndClientDDE, szItem))
	       MessageBox(hwndMain,"Advisory already established",
                   "Client", MB_ICONEXCLAMATION | MB_OK);
           else
	       hwndServerDDE = GetHwndServerDDE(hwndClientDDE);
	       SendAdvise(hwndClientDDE, hwndServerDDE, szItem);
       }
       else
       {   /* must initiate new conversation first */
	   SendInitiate(szApplication, szTopic);
	   if (hwndClientDDE = FindConvGivenAppTopic(szApplication, szTopic))
	   {
		hwndServerDDE = GetHwndServerDDE(hwndClientDDE);
		SendAdvise(hwndClientDDE, hwndServerDDE, szItem);
	   }
       }
    }

    return;
}



/****************************************************************************

    FUNCTION: InitAckTimeOut

    PURPOSE:  Get DDE timeout value from win.ini.  Value is in milliseconds.

****************************************************************************/
void InitAckTimeOut(void)
{

   /* Finds value in win.ini section corresponding to application name */

   nAckTimeOut = GetPrivateProfileInt("Client",
			       "DdeTimeOut",
			       DEFAULT_ACK_TIME_OUT_MILLISEC,
                               "client.ini");
   return;
}

/****************************************************************************

    FUNCTION: SendAdvise

    PURPOSE:  Send advise message to server.


****************************************************************************/
void SendAdvise(hwndClientDDE, hwndServerDDE, szItem)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
    char * szItem;
{
    ATOM            atomItem;
    HANDLE          hOptions;
    DDEADVISE FAR * lpOptions;

    /* don't send another message requiring an ACK until first message */
    /* is acknowledged						       */
    if (AwaitingAck(hwndClientDDE))
        return;

    if (!(hOptions 
          = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (LONG)sizeof(DDEADVISE))))
        return;
    if (!(lpOptions
          = (DDEADVISE FAR *)GlobalLock(hOptions)))
    {
	GlobalFree(hOptions);
        return;
    }
    lpOptions->cfFormat = CF_TEXT;
    lpOptions->fAckReq = TRUE;
    lpOptions->fDeferUpd = FALSE;
    GlobalUnlock(hOptions);
    atomItem = GlobalAddAtom((LPSTR)szItem);
    SetConvPendingAck(hwndClientDDE, ADVISE);
    SetTimer(hwndClientDDE, hwndServerDDE, nAckTimeOut, NULL);
    if (!PostMessage(hwndServerDDE,
            WM_DDE_ADVISE, 
	    hwndClientDDE,
            MAKELONG(hOptions, atomItem)))
    {
        GlobalDeleteAtom(atomItem);
        GlobalFree(hOptions);
    }
    return;
}



/****************************************************************************

    FUNCTION: SendExecute

    PURPOSE:  Send execute string to server.


****************************************************************************/
void SendExecute(hwndClientDDE, hwndServerDDE, szExecuteString)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
    char * szExecuteString;
{
    HANDLE	hExecuteString;
    LPSTR	lpExecuteString;

    /* don't send another message requiring an ACK until first message */
    /* is acknowledged						       */
    if (AwaitingAck(hwndClientDDE))
        return;

    if (!(hExecuteString
          = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 
			(DWORD)lstrlen(szExecuteString) + 1)))
        return;
    if (!(lpExecuteString
	  = GlobalLock(hExecuteString)))
    {
	GlobalFree(hExecuteString);
        return;
    }
    lstrcpy(lpExecuteString, szExecuteString);
    GlobalUnlock(hExecuteString);
    SetConvPendingAck(hwndClientDDE, EXECUTE);
    SetTimer(hwndClientDDE, hwndServerDDE, nAckTimeOut, NULL);
    if (!PostMessage(hwndServerDDE,
	    WM_DDE_EXECUTE,
	    hwndClientDDE,
	    MAKELONG(0, hExecuteString)))
    {
	GlobalFree(hExecuteString);
    }
    return;
}



/****************************************************************************

    FUNCTION: SendInitiate

    PURPOSE:  Sends initiate message to all windows.  By the time this
	      function returns, all servers matching the app/topic will
	      have acknowledged, and this client applicaiton will have
	      temporarily registered the new conversations.   If more
	      than one server responded, then this client application
	      asks the user which conversation to keep; all other
	      conversations will then be terminated.   This function
	      returns the handle of the hidden DDE window used to
	      initiate the conversation with server(s).

****************************************************************************/
HWND SendInitiate(szApplication, szTopic)
    char * szApplication;
    char * szTopic;
{
    HWND  hwndClientDDE;
    ATOM  atomApplication;
    ATOM  atomTopic;

    if (!(hwndClientDDE = CreateWindow(
	    "ClientDDEWndClass",
	    "ClientDDE",
	    WS_CHILD,	/* not visible */
	    0, 0, 0, 0, /* no position or dimensions */
	    hwndMain,	/* parent */
	    NULL,	/* no menu */
	    hInst,
	    NULL)))
    {
	return (NULL);
    }

    atomApplication
        = *szApplication == 0 ? NULL : GlobalAddAtom((LPSTR)szApplication);
    atomTopic
        = *szTopic == 0 ? NULL : GlobalAddAtom((LPSTR)szTopic);

    /* flag bIniInitiate is queried when client processes the server's ACK */
    bInInitiate = TRUE;
    SendMessage(-1, 
        WM_DDE_INITIATE, 
	hwndClientDDE,
        MAKELONG(atomApplication, atomTopic));
    bInInitiate = FALSE;
    if (atomApplication != NULL)
        GlobalDeleteAtom(atomApplication);
    if (atomTopic != NULL)
        GlobalDeleteAtom(atomTopic);
    return (hwndClientDDE);
}

/****************************************************************************

    FUNCTION: SendPoke

    PURPOSE:  Send poke message to server.


****************************************************************************/
void SendPoke(hwndClientDDE, hwndServerDDE, szItem, szValue)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
    char * szItem;
    char * szValue;
{
    ATOM        atomItem;
    HANDLE      hPokeData;
    DDEPOKE FAR * lpPokeData;

    /* don't send another message requiring an ACK until first message */
    /* is acknowledged						       */
    if (AwaitingAck(hwndClientDDE))
        return;

    /* Allocate size of DDE data header, plus the data:  a string   */
    /* terminated by <CR> <LF> <NULL>.  The <NULL> is counted by    */
    /* by DDEPOKE.Value[1].                                         */

    if (!(hPokeData
          = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 
                        (LONG)sizeof(DDEPOKE) + lstrlen(szValue) + 2)))
        return;
    if (!(lpPokeData
          = (DDEPOKE FAR*)GlobalLock(hPokeData)))
    {
	GlobalFree(hPokeData);
        return;
    }
    lpPokeData->fRelease = TRUE;
    lpPokeData->cfFormat = CF_TEXT;
    lstrcpy((LPSTR)lpPokeData->Value, (LPSTR)szValue);
    /* each line of CF_TEXT data is terminated by CR/LF */
    lstrcat((LPSTR)lpPokeData->Value, (LPSTR)"\r\n");
    GlobalUnlock(hPokeData);
    atomItem = GlobalAddAtom((LPSTR)szItem);
    SetConvPendingAck(hwndClientDDE, POKE);
    SetTimer(hwndClientDDE, hwndServerDDE, nAckTimeOut, NULL);
    if (!PostMessage(hwndServerDDE,
            WM_DDE_POKE,
	    hwndClientDDE,
            MAKELONG(hPokeData, atomItem)))
    {
        GlobalDeleteAtom(atomItem);
        GlobalFree(hPokeData);
    }
    return;
}



/****************************************************************************

    FUNCTION: SendRequest

    PURPOSE:  Send request message to server.


****************************************************************************/
void SendRequest(hwndClientDDE, hwndServerDDE, szItem)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
    char * szItem;
{
    ATOM  atomItem;

    /* don't send another message requiring an ACK until first message */
    /* is acknowledged						       */
    if (AwaitingAck(hwndClientDDE))
        return;

    atomItem = GlobalAddAtom((LPSTR)szItem);
    SetConvPendingAck(hwndClientDDE, REQUEST);
    SetTimer(hwndClientDDE, hwndServerDDE, nAckTimeOut, NULL);
    if (!PostMessage(hwndServerDDE,
            WM_DDE_REQUEST,
	    hwndClientDDE,
            MAKELONG(CF_TEXT,atomItem)))
    {
        GlobalDeleteAtom(atomItem);
    }
    return;
}


/****************************************************************************

    FUNCTION: SendTerminate

    PURPOSE:  Send terminate message to server.

****************************************************************************/
void SendTerminate(hwndClientDDE, hwndServerDDE)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
{
    MSG msg;
    LONG lTimeOut;

    SetConvInTerminateState(hwndClientDDE, hwndServerDDE);
    PostMessage(hwndServerDDE, WM_DDE_TERMINATE, hwndClientDDE, 0L);
    return;
}




/****************************************************************************

    FUNCTION: SendUnadvise

    PURPOSE:  Send unadvise message to server.


****************************************************************************/
void SendUnadvise(hwndClientDDE, hwndServerDDE, szItem)
    HWND  hwndClientDDE;
    HWND  hwndServerDDE;
    char * szItem;
{
    ATOM  atomItem;

    /* don't send another message requiring an ACK until first message */
    /* is acknowledged						       */
    if (AwaitingAck(hwndClientDDE))
        return;

    atomItem = GlobalAddAtom((LPSTR)szItem);
    SetConvPendingAck(hwndClientDDE, UNADVISE);
    SetTimer(hwndClientDDE, hwndServerDDE, nAckTimeOut, NULL);
    if (!PostMessage(hwndServerDDE,
            WM_DDE_UNADVISE, 
	    hwndClientDDE,
            MAKELONG(0,atomItem)))
    {
        GlobalDeleteAtom(atomItem);
    }
    return;
}


/****************************************************************************

    FUNCTION: TerminateConversations

    PURPOSE:  Processes WM_DESTROY message, terminates all conversations.

****************************************************************************/
void TerminateConversations(void)
{
   HWND  hwndClientDDE;
   HWND  hwndServerDDE;
   LONG  lTimeOut;
   MSG   msg;


   /* Terminate each active conversation */
   hwndClientDDE = NULL;
   while (hwndClientDDE = GetNextConv(hwndClientDDE))
   {
	hwndServerDDE = GetHwndServerDDE(hwndClientDDE);
	if (IsWindow(hwndServerDDE)) /* if server window still alive */
	    SendTerminate(hwndClientDDE, hwndServerDDE);
   }

   /* Wait for all conversations to terminate or for time out */
   lTimeOut = GetTickCount() + (LONG)nAckTimeOut;
   while (PeekMessage(&msg, NULL, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE))
   {
         DispatchMessage (&msg);
	 if (msg.message == WM_DDE_TERMINATE)
	 {
	     if (!AtLeastOneConvActive())
		 break;
	 }
         if (GetTickCount() > lTimeOut)
             break;
   }

   return;
}
