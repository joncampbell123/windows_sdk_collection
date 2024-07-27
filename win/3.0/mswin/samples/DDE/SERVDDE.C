/****************************************************************************

    MODULE: SERVDDE.C

    PURPOSE: Processes incoming and outgoing DDE messages


****************************************************************************/

#include "windows.h"

#include "dde.h"
#include "server.h"
#include <string.h>

#define DEFAULT_ACK_TIME_OUT_MILLISEC 10000
static int nAckTimeOut;

static BOOL   bTerminating = FALSE;

int  NEAR GetDocIndexGivenName(char*);
int  NEAR GetItemNumber(char*);




/****************************************************************************

    FUNCTION: DDEWndProc

    PURPOSE:  Handles all DDE messages received by the server application.

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
            ServerAcknowledge(hwnd, (HWND)wParam, lParam);
	    return (0L);

	case WM_TIMER:	 /* time out on waiting for ACK in response */
			 /* to WM_DDE_DATA sent by this server	    */

	    ServerAcknowledge(hwnd, (HWND)wParam, 0L); /* simulates NACK */
	    return (0L);

        case WM_DDE_ADVISE:
            ServerAdvise(hwnd, (HWND)wParam, lParam);
	    return (0L);

        case WM_DDE_POKE:
            ServerPoke(hwnd, (HWND)wParam, lParam);
	    return (0L);

        case WM_DDE_TERMINATE:
            ServerTerminate(hwnd, (HWND)wParam);
	    return (0L);

        case WM_DDE_UNADVISE:
            ServerUnadvise(hwnd, (HWND)wParam, lParam);
	    return (0L);

        case WM_DDE_REQUEST:
            ServerRequest(hwnd, (HWND)wParam, lParam);
	    return (0L);

	case WM_DDE_EXECUTE:
	    ServerExecute(hwnd, (HWND)wParam, (HANDLE)HIWORD(lParam));
	    return (0L);

	default:
	      return (DefWindowProc(hwnd, message, wParam, lParam));
    }
}



/****************************************************************************

    FUNCTION: GetItemNumber

    PURPOSE:  Get server control i.d. (1, 2, or 3) given item name.

****************************************************************************/
int NEAR GetItemNumber(szItem)
    char * szItem;
{
    int nItem;

    if (!strcmpi(szItem, "ITEM1"))
        nItem = 1;
    else if (!strcmpi(szItem, "ITEM2"))
        nItem = 2;
    else if (!strcmpi(szItem, "ITEM3"))
        nItem = 3;
    else
        nItem = 0;
    return (nItem);
}



/****************************************************************************

    FUNCTION: InitAckTimeOut

    PURPOSE:  Get DDE timeout value from win.ini.  Value is in milliseconds.

****************************************************************************/
void InitAckTimeOut(void)
{

   /* Finds value in win.ini section corresponding to application name */

   nAckTimeOut = GetPrivateProfileInt("Server",
			       "DdeTimeOut",
			       DEFAULT_ACK_TIME_OUT_MILLISEC,
                               "server.ini");
   return;
}



/****************************************************************************

    FUNCTION: SendData

    PURPOSE:  Send data to client.

****************************************************************************/
void SendData(hwndServerDDE, hwndClientDDE, szItemName, szItemValue,
                   bDeferUpdate, bAckRequest) 
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
    char * szItemName;
    char * szItemValue;
    BOOL   bDeferUpdate;
    BOOL   bAckRequest;
{
    ATOM          atomItem;
    HANDLE        hData;
    DDEDATA FAR * lpData;
    int           nItem;

    if (bDeferUpdate)
    {
        atomItem = GlobalAddAtom((LPSTR)szItemName);
	/* notify client with null data since advise was set up for */
	/* deferred update					    */
	if (!PostMessage(hwndClientDDE,
                WM_DDE_DATA,
		hwndServerDDE,
		MAKELONG(0, atomItem)))
        {
            GlobalDeleteAtom(atomItem);
        }
        return;
    }

    /* Allocate size of DDE data header, plus the data:  a string,  */
    /* <CR> <LR> <NULL>.  The byte for the string null terminator */
    /* is counted by DDEDATA.Value[1].				   */

    if (!(hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 
			      (LONG)sizeof(DDEDATA) +lstrlen(szItemValue) + 2)))
        return;
    if (!(lpData = (DDEDATA FAR*)GlobalLock(hData)))
    {
	GlobalFree(hData);
        return;
    }

    lpData->fAckReq = bAckRequest;
    lpData->cfFormat = CF_TEXT;
    lstrcpy((LPSTR)lpData->Value, (LPSTR)szItemValue);
    /* each line of CF_TEXT data is terminated by CR/LF */
    lstrcat((LPSTR)lpData->Value, (LPSTR)"\r\n");
    GlobalUnlock(hData);
    atomItem = GlobalAddAtom((LPSTR)szItemName);
    if (!PostMessage(hwndClientDDE,
            WM_DDE_DATA,
	    hwndServerDDE,
            MAKELONG(hData, atomItem)))
    {
        GlobalFree(hData);
        GlobalDeleteAtom(atomItem);
	return;
    }
    if (bAckRequest)
    {
	SetTimer(hwndServerDDE, hwndClientDDE, nAckTimeOut, NULL);
	nItem = GetItemNumber(szItemName);
	/* hData is to be deleted if not read by client for some reason */
	CheckOutSentData(hwndServerDDE, nItem, atomItem, hData);
    }
    return;
}



/****************************************************************************

    FUNCTION: SendTerminate

    PURPOSE:  Post terminate message and indicate that conversation is
	      in process ot being terminated.

****************************************************************************/
void SendTerminate(hwndServerDDE, hwndClientDDE)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
{
    SetConvInTerminateState(hwndServerDDE);
    PostMessage(hwndClientDDE, WM_DDE_TERMINATE, hwndServerDDE, 0L);
    return;
}



/****************************************************************************

    FUNCTION: ServerAcknowledge

    PURPOSE:  Called when server application receives ACK or NACK, or
	      when server receives time out waiting for response to
	      WM_DDE_DATA.

****************************************************************************/
void ServerAcknowledge(hwndServerDDE, hwndClientDDE, lParam)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
    LONG  lParam;
{
    char szItemName[ITEM_NAME_MAX_SIZE+1];
    int  nItem;

    KillTimer(hwndServerDDE, hwndClientDDE);

    if (!(LOWORD(lParam) & 0x8000))
    {
        GlobalGetAtomName(HIWORD(lParam), szItemName, ITEM_NAME_MAX_SIZE);
        nItem = GetItemNumber(szItemName);
	GlobalFreeSentData(hwndServerDDE, nItem);
	MessageBox(hwndMain,
            "DDE send data failed",
            "Server",
            MB_ICONEXCLAMATION | MB_OK);
    }
    if (HIWORD(lParam))    /* 0 if time-out, so don't try to delete */
	GlobalDeleteAtom(HIWORD(lParam));
    return;
}



/****************************************************************************

    FUNCTION: ServerAdvise

    PURPOSE:  Called when server application receives WM_DDE_ADVISE message.

****************************************************************************/
void ServerAdvise(hwndServerDDE, hwndClientDDE, lParam)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
    LONG  lParam;
{
    HANDLE          hDDEAdviseOptions;
    DDEADVISE FAR * lpDDEAdviseOptions;
    ATOM            atomItem;
    char            szItem[ITEM_NAME_MAX_SIZE+1];
    int             nItem;

    hDDEAdviseOptions = LOWORD(lParam);
    atomItem = HIWORD(lParam);
 
    GlobalGetAtomName(atomItem, szItem, ITEM_NAME_MAX_SIZE);

    if (!(nItem = GetItemNumber(szItem))
	|| !AddAdvise(hwndServerDDE, hDDEAdviseOptions, atomItem, nItem))
    {
	PostMessage(hwndClientDDE,
            WM_DDE_ACK,
	    hwndServerDDE,
            MAKELONG(0, atomItem)); /* negative acknowledgement */
        return;
    }
    PostMessage(hwndClientDDE,
        WM_DDE_ACK,
	hwndServerDDE,
        MAKELONG(0x8000, atomItem)); /* positive acknowledgement */
    return;
}



/****************************************************************************

    FUNCTION: ServerExecute

    PURPOSE:  Called when server application receives WM_DDE_EXECUTE message.

****************************************************************************/
void ServerExecute(hwndServerDDE, hwndClientDDE, hCommand)
    HWND    hwndServerDDE;
    HWND    hwndClientDDE;
    HANDLE  hCommand;
{
    LPSTR   lpstrCommand;
    char    szExecuteString[EXECUTE_STRING_MAX_SIZE+1];

    if (!(lpstrCommand = GlobalLock(hCommand)))
    {
	PostMessage(hwndClientDDE,
            WM_DDE_ACK,
	    hwndServerDDE,
	    MAKELONG(0, hCommand)); /* negative acknowledgement */
        return;
    }
    if (lstrlen(lpstrCommand) > EXECUTE_STRING_MAX_SIZE)
	lpstrCommand[EXECUTE_STRING_MAX_SIZE] = 0;
    lstrcpy(szExecuteString, lpstrCommand);
    GlobalUnlock(hCommand);
    PostMessage(hwndClientDDE,
	WM_DDE_ACK,
	hwndServerDDE,
	MAKELONG(0x8000, hCommand)); /* positive acknowledgement */

    MessageBox(hwndMain,
	szExecuteString,
	"Server Received Execute Command",
	MB_OK);
    return;
}




/****************************************************************************

    FUNCTION: ServerInitiate

    PURPOSE:  Called when server application receives WM_DDE_INITIATE message.

****************************************************************************/
void ServerInitiate(hwndClientDDE, lParam)
    HWND  hwndClientDDE;
    LONG  lParam;
{
    HWND  hwndServerDDE;
    ATOM  atomApplicationRcvd;
    ATOM  atomTopicRcvd;
    ATOM  atomApplicationReturn;
    ATOM  atomTopicReturn;
    char  szApplication[APP_MAX_SIZE+1];
    char  szTopic[TOPIC_MAX_SIZE+1];

    if (!(hwndServerDDE = CreateWindow(
	    "ServerDDEWndClass",
	    "ServerDDE",
	    WS_CHILD,	/* not visible */
	    0, 0, 0, 0, /* no position or dimensions */
	    hwndMain,	/* parent */
	    NULL,	/* no menu */
	    hInst,
	    NULL)))
    {
	return;
    }

    if (atomApplicationRcvd = LOWORD(lParam))
        GlobalGetAtomName(atomApplicationRcvd, szApplication, APP_MAX_SIZE);
    if (atomApplicationRcvd && strcmpi(szApplication,"SERVER"))
    { /* if application was specified but it wasn't "server" */
        return; 
    }
    if (atomTopicRcvd = HIWORD(lParam))
    {
        GlobalGetAtomName(atomTopicRcvd, szTopic, TOPIC_MAX_SIZE);
        if (strcmpi(szTopic, szDocName))
            return;
    }
    if (AddConv(hwndServerDDE, hwndClientDDE))
    {
        atomApplicationReturn = GlobalAddAtom("SERVER");
        atomTopicReturn = GlobalAddAtom(szDocName);
	if (!SendMessage(hwndClientDDE,
                WM_DDE_ACK, 
		hwndServerDDE,
                MAKELONG(atomApplicationReturn, atomTopicReturn)))
        {
            GlobalDeleteAtom(atomApplicationReturn);
            GlobalDeleteAtom(atomTopicReturn);
        }
    }
    return;
}



/****************************************************************************

    FUNCTION: ServerPoke

    PURPOSE:  Called when server application receives WM_DDE_POKE message.

****************************************************************************/
void ServerPoke(hwndServerDDE, hwndClientDDE, lParam)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
    LONG  lParam;
{
    HANDLE        hPokeData;
    DDEPOKE FAR * lpPokeData;
    ATOM          atomItem;
    int           nItem;
    char          szItemName[ITEM_NAME_MAX_SIZE+1];
    char          szItemValue[ITEM_VALUE_MAX_SIZE+1];
    BOOL          bRelease;
    char        * pcCarriageReturn;


    hPokeData = LOWORD(lParam);
    atomItem = HIWORD(lParam);
    
    GlobalGetAtomName(atomItem, szItemName, ITEM_NAME_MAX_SIZE);
    if (!(lpPokeData = (DDEPOKE FAR *)GlobalLock(hPokeData))
        || lpPokeData->cfFormat != CF_TEXT
        || !(nItem = GetItemNumber(szItemName)))
    {
	PostMessage(hwndClientDDE,
           WM_DDE_ACK, 
	   hwndServerDDE,
           MAKELONG(0, atomItem)); /* negative acknowledgement */
	return;
    }

    lstrcpy(szItemValue, lpPokeData->Value);
    if (pcCarriageReturn = strchr(szItemValue, '\r')) 
        *pcCarriageReturn = 0;  /* remove CR/LF */
    SetDlgItemText(hwndMain, nItem, szItemValue);
    MaybeAdviseData(nItem);

    /* Save value of fRelease, since pointer may be invalidated by */
    /*	GlobalUnlock()						  */
    bRelease = lpPokeData->fRelease;
    GlobalUnlock(hPokeData);

    if (bRelease)
    {
        GlobalFree(hPokeData);
    }

    /* Since we are re-using the item atom, we should not delete it */
    /* if PostMessage fails:  the client should delete the atom     */
    /* when it gets a time-out on the expected ACK.		    */
    PostMessage(hwndClientDDE,
	WM_DDE_ACK,
	hwndServerDDE,
	MAKELONG(0x8000, atomItem));  /* positive acknowledgement */
    return;
}



/****************************************************************************

    FUNCTION: ServerRequest

    PURPOSE:  Called when server application receives WM_DDE_REQUEST message.

****************************************************************************/
void ServerRequest(hwndServerDDE, hwndClientDDE, lParam)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
    LONG  lParam;
{
    char szItem[ITEM_NAME_MAX_SIZE+1];
    char szItemValue[ITEM_VALUE_MAX_SIZE+1];
    int  nItem;

    GlobalGetAtomName(HIWORD(lParam), szItem, ITEM_NAME_MAX_SIZE);
    if (!(nItem = GetItemNumber(szItem))
	|| (LOWORD(lParam) != CF_TEXT)) /* this app supports only CF_TEXT */
    {
	PostMessage(hwndClientDDE,
            WM_DDE_ACK,
	    hwndServerDDE,
            MAKELONG(0, HIWORD(lParam))); /* NACK */
        return;
    }
    if (!GetDlgItemText(hwndMain, nItem, szItemValue, ITEM_VALUE_MAX_SIZE))
    {
	strcpy(szItemValue," ");
    }
    /* send now, don't defer, and don't ask for ACK */
    SendData(hwndServerDDE, hwndClientDDE, szItem, szItemValue, FALSE, FALSE);
    GlobalDeleteAtom(HIWORD(lParam));
    return;
}


/****************************************************************************

    FUNCTION: ServerTerminate

    PURPOSE:  Called when server application receives WM_DDE_TERMINATE message.

****************************************************************************/
void ServerTerminate(hwndServerDDE, hwndClientDDE)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
{

    if (!IsConvInTerminateState(hwndClientDDE))
    { /* Client has requested terminate: respond with terminate */
	PostMessage(hwndClientDDE, WM_DDE_TERMINATE, hwndServerDDE, 0L);
    }

    RemoveConv(hwndServerDDE);
    DestroyWindow(hwndServerDDE);
    return;
}



/****************************************************************************

    FUNCTION: ServerUnadvise

    PURPOSE:  Called when server application receives WM_DDE_UNADIVSE message.

****************************************************************************/
void ServerUnadvise(hwndServerDDE, hwndClientDDE, lParam)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
    LONG  lParam;
{
    char szItem[ITEM_NAME_MAX_SIZE+1];
    int  nItem;
    BOOL bSuccess;
   
    if (HIWORD(lParam))
    {
        GlobalGetAtomName(HIWORD(lParam), szItem, ITEM_NAME_MAX_SIZE);
        nItem = GetItemNumber(szItem);
	bSuccess = RemoveAdvise(hwndServerDDE, nItem);
    }
    else
    {   /* HIWORD(lParam)==0 means remove all advises */   
	bSuccess = RemoveAdvise(hwndServerDDE, 0);
    }
    if (bSuccess)
    {
	PostMessage(hwndClientDDE,
            WM_DDE_ACK, 
	    hwndServerDDE,
            MAKELONG(0x8000, HIWORD(lParam))); /* positive ack */
    }
    else
    {
	PostMessage(hwndClientDDE,
            WM_DDE_ACK, 
	    hwndServerDDE,
            MAKELONG(0, HIWORD(lParam))); /* negative ack */
    }
    return;
}

/****************************************************************************

    FUNCTION: TerminateConversations

    PURPOSE:  Processes WM_DESTROY message, terminates all conversations.

****************************************************************************/
void TerminateConversations()
{
   HWND  hwndServerDDE;
   LONG  lTimeOut;
   MSG   msg;


   /* Terminate each active conversation */
   hwndServerDDE = NULL;
   while (hwndServerDDE = GetNextConv(hwndServerDDE))
   {
	SendTerminate(hwndServerDDE, GetHwndClientDDE(hwndServerDDE));
   }

   /* Wait for all conversations to terminate OR for time out */
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
