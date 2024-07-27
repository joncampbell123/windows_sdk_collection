/****************************************************************************

    PROGRAM: CLIENT

    PURPOSE: Illustrates client side of DDE conversation

    MODULES:

        CLIENT.C   Window and dialog procedures.
	CLIDATA.C  Maintains conversation data.
        CLIDDE.C   Processes incoming and outgoing DDE messages.

****************************************************************************/

#include "windows.h"

#include "dde.h"
#include "clires.h"
#include "client.h"
#include <string.h>
#include <stdlib.h>

static int    nInstCount;
static HWND   hwndRequestDlg;

static BOOL   bInInitiate = FALSE;
static BOOL   bInRequestDlg = FALSE;
static BOOL   bTerminating = FALSE;

static char  szSelectedApplication[APP_MAX_SIZE+1];
static char  szSelectedTopic[TOPIC_MAX_SIZE+1];
static char  szSelectedItem[ITEM_MAX_SIZE+1];
static char  szSelectedValue[VALUE_MAX_SIZE+1];
static HWND  hwndSelectedClientDDE;
static int   cfSelectedFormat;

long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL            InitApplication(HANDLE);
void            InitAddedInstance(HANDLE, HANDLE);
BOOL            InitInstance(HANDLE, int);
int  NEAR	DoDialog(char *, FARPROC);
BOOL FAR PASCAL AboutDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL AdviseDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL ClearDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL ExecuteDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL InitiateDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL PokeDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL RequestDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL TerminateDlgProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL UnadviseDlgProc(HWND, unsigned, WORD, LONG);
void		AddConversationsToBox(HWND, unsigned);
void NEAR	GetSelectedConversation(HWND, unsigned, unsigned);
    

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: Calls initialization functions and processes message loop

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{
    MSG msg;

    if (!hPrevInstance)
    {
        if (!InitApplication(hInstance))
	       return (FALSE);
    }
    else
    {
        InitAddedInstance(hInstance, hPrevInstance);
    }

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, NULL, NULL)) 
    {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }
    return (msg.wParam);
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window classes

****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    nInstCount = 1;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  "ClientMenu";
    wc.lpszClassName = "ClientWClass";

    if (!RegisterClass(&wc))
	return (FALSE);

    wc.style = NULL;
    wc.lpfnWndProc = DDEWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "ClientDDEWndClass";

    return (RegisterClass(&wc));
}


/****************************************************************************

    FUNCTION: InitAddedInstance

    PURPOSE:  Increment instance counter.

****************************************************************************/
void InitAddedInstance(hInstance, hPrevInstance)
    HANDLE  hInstance;
    HANDLE  hPrevInstance;
{
    GetInstanceData(hPrevInstance, (NPSTR)&nInstCount, sizeof(int));
    nInstCount++;
    return;
}



/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;
    int             nCmdShow;
{
    HDC         hDC;
    TEXTMETRIC  tm;

    hInst = hInstance;

    hwndMain = CreateWindow(
        "ClientWClass",
        "Client",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwndMain)
        return (FALSE);

    InitDataTextMetrics();
    InitAckTimeOut();	    /* in module CLIDDE */

    MoveWindow(hwndMain,
        xDelta*(5+nInstCount), 
        ((nInstCount-1)&1)*nVertRes/2 + yDelta*nInstCount,
        xDelta*30, 
        yDelta*12,
        FALSE);

    if (!(cfLink = RegisterClipboardFormat("Link")))
	return (FALSE);

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for client application.
	      DDE messages are handled by DDEWndProc in CLIDDE.C.

****************************************************************************/

long FAR PASCAL MainWndProc(hwnd, message, wParam, lParam)
HWND hwnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    char  szApplication[APP_MAX_SIZE+1];
    char  szTopic[TOPIC_MAX_SIZE+1];

    switch (message) 
    {
        case WM_PAINT:
	    PaintConvData(hwnd);
            return (0L);

        case WM_INITMENU:
            if (wParam == GetMenu(hwnd))
            {
		if (IsClipboardFormatAvailable(cfLink))
		    EnableMenuItem(wParam, IDM_PASTELINK, MF_ENABLED);
		else
		    EnableMenuItem(wParam, IDM_PASTELINK, MF_GRAYED);
            }
            return (0L);

        case WM_COMMAND:
            switch (wParam) 
            {
		case IDM_INITIATE:
                    /* If we are in the process of terminating, no new
                       conversations are allowed */
                    if (!bTerminating) 
                    {
			DoDialog("Initiate", InitiateDlgProc);
                    }
                    return 0L;

                case IDM_TERMINATE:
		    DoDialog("Terminate", TerminateDlgProc);
                    return 0L;

                case IDM_ADVISE:
		    DoDialog("Advise", AdviseDlgProc);
                    return 0L;

                case IDM_UNADVISE:
		    DoDialog("Unadvise", UnadviseDlgProc);
                    return 0L;

                case IDM_REQUEST:
		    DoDialog("Request", RequestDlgProc);
                    return 0L;
 
                case IDM_POKE:
		    DoDialog("Poke", PokeDlgProc);
                    return 0L;

                case IDM_PASTELINK:
		    DoPasteLink();
                    return 0L;

                case IDM_CLEAR:
		    DoDialog("Clear", ClearDlgProc);
                    return 0L;

		case IDM_EXECUTE:
		    DoDialog("Execute", ExecuteDlgProc);
		    return 0L;

		case IDM_ABOUT:
		    DoDialog("About", AboutDlgProc);
		    break;

                default:
                    return (DefWindowProc(hwnd, message, wParam, lParam));
            }
            break;

        case WM_DESTROY:
            /* Terminate all DDE conversations before destroying 
               client window */
	    bTerminating = TRUE;
	    TerminateConversations();
            PostQuitMessage(0);
            break;
        
	default:
	    return (DefWindowProc(hwnd, message, wParam, lParam));
    }
    return (0L);
}


/****************************************************************************

    FUNCTION: DoDialog

    PURPOSE:  Creates dialog given dialog i.d.

****************************************************************************/
int NEAR DoDialog(szDialog, lpfnDlgProc)
    char   * szDialog;
    FARPROC  lpfnDlgProc;
{
    int      nReturn;

    lpfnDlgProc = MakeProcInstance(lpfnDlgProc, hInst);
    nReturn = DialogBox(hInst, 
	szDialog,
	hwndMain,
        lpfnDlgProc);
    FreeProcInstance(lpfnDlgProc);
    return  (nReturn);
}



/****************************************************************************

    FUNCTION: AboutDlgProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

****************************************************************************/

BOOL FAR PASCAL AboutDlgProc(hDlg, message, wParam, lParam)
    HWND hDlg;
    unsigned message;
    WORD wParam;
    LONG lParam;
{
    switch (message) {
	case WM_INITDIALOG:
	    return (TRUE);

	case WM_COMMAND:
	    if (wParam == IDOK || wParam == IDCANCEL) {
		EndDialog(hDlg, TRUE);
		return (TRUE);
	    }
	    break;
    }
    return (FALSE);
}



/****************************************************************************

    FUNCTION: AdviseDlgProc

    PURPOSE:  Processes messages for the Advise dialog.

****************************************************************************/
BOOL FAR PASCAL AdviseDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND  hwndServerDDE;

    switch (message) 
    {
        case WM_INITDIALOG:
	    AddConversationsToBox(hdlg, CB_ADDSTRING);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
		    GetSelectedConversation(hdlg, CB_GETCURSEL, CB_GETLBTEXT);
                    GetDlgItemText(hdlg, 
                        IDC_ITEM,
                        (LPSTR)szSelectedItem,
                        ITEM_MAX_SIZE);
		    if (DoesAdviseAlreadyExist(hwndSelectedClientDDE,
                                               szSelectedItem))
                        MessageBox(hdlg, "Advisory already established",
                            "Client", MB_ICONEXCLAMATION | MB_OK);
                    else
			hwndServerDDE
			    = GetHwndServerDDE(hwndSelectedClientDDE);
			SendAdvise(hwndSelectedClientDDE,
			    hwndServerDDE,
			    szSelectedItem);
		    return (TRUE);

                case IDC_CANCEL:
                    EndDialog(hdlg, FALSE);
		    return (FALSE);
            }

        default:
            return (FALSE);
    }
    return (FALSE);
}



/****************************************************************************

    FUNCTION: ClearDlgProc

    PURPOSE:  Processes messages for the Clear dialog.

****************************************************************************/
BOOL FAR PASCAL ClearDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND  hwndServerDDE;
    HWND  hctlItemBox;
    int   nIndex;

    switch (message) 
    {
        case WM_INITDIALOG:
	    AddConversationsToBox(hdlg, CB_ADDSTRING);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
		    GetSelectedConversation(hdlg, CB_GETCURSEL, CB_GETLBTEXT);
                    GetDlgItemText(hdlg, 
                        IDC_ITEM,
                        (LPSTR)szSelectedItem,
                        ITEM_MAX_SIZE);
		    hwndServerDDE = GetHwndServerDDE(hwndSelectedClientDDE);
		    SendUnadvise(hwndSelectedClientDDE,
			hwndServerDDE,
			szSelectedItem);
                    EndDialog(hdlg, TRUE);
		    return (TRUE);

                case IDC_CANCEL:
                    EndDialog(hdlg, FALSE);
                    return (TRUE);
            }
        default:
            return (FALSE);
    }
    return (FALSE);
}



/****************************************************************************

    FUNCTION: ExecuteDlgProc

    PURPOSE:  Processes messages for the Execute dialog.

****************************************************************************/
BOOL FAR PASCAL ExecuteDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND  hwndServerDDE;
    char  szExecuteString[EXECUTE_STRING_MAX_SIZE+1];

    switch (message) 
    {
        case WM_INITDIALOG:
	    AddConversationsToBox(hdlg, CB_ADDSTRING);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
		    GetSelectedConversation(hdlg, CB_GETCURSEL, CB_GETLBTEXT);
                    GetDlgItemText(hdlg, 
			IDC_EXECUTE_STRING,
			(LPSTR)szExecuteString,
			EXECUTE_STRING_MAX_SIZE);
		    hwndServerDDE = GetHwndServerDDE(hwndSelectedClientDDE);
		    SendExecute(hwndSelectedClientDDE,
			hwndServerDDE,
			szExecuteString);
		    return (TRUE);

                case IDC_CANCEL:
                    EndDialog(hdlg, FALSE);
		    return (TRUE);
            }

        default:
            return (FALSE);
    }
    return (FALSE);
}




/****************************************************************************

    FUNCTION: InitiateDlgProc

    PURPOSE:  Processes messages for the Initiate dialog.

****************************************************************************/
BOOL FAR PASCAL InitiateDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND      hwndClientDDE;

    switch (message) 
    {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
                    GetDlgItemText(hdlg, 
                        IDC_APPLICATION,
                        (LPSTR)szSelectedApplication,
                        APP_MAX_SIZE);
                    GetDlgItemText(hdlg, 
                        IDC_TOPIC,
                        (LPSTR)szSelectedTopic,
                        TOPIC_MAX_SIZE);
                    EndDialog(hdlg, TRUE);
		    if (hwndClientDDE
			= SendInitiate(szSelectedApplication, szSelectedTopic))
		    {
			LetUserPickConversation(hwndClientDDE);
			InvalidateRect(hwndMain, NULL, TRUE);
		    }
		    return (TRUE);
                        
                case IDC_CANCEL:
                    EndDialog(hdlg, FALSE);
                    return (TRUE);
            }
        default:
            return (FALSE);
    }
    return (FALSE);
}




/****************************************************************************

    FUNCTION: PokeDlgProc

    PURPOSE:  Processes messages for the Poke dialog.

****************************************************************************/
BOOL FAR PASCAL PokeDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND  hwndServerDDE;

    switch (message) 
    {
        case WM_INITDIALOG:
	    AddConversationsToBox(hdlg, CB_ADDSTRING);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
		    GetSelectedConversation(hdlg, CB_GETCURSEL, CB_GETLBTEXT);
                    GetDlgItemText(hdlg, 
                        IDC_ITEM,
                        (LPSTR)szSelectedItem,
                        ITEM_MAX_SIZE);
                    GetDlgItemText(hdlg,
                        IDC_VALUE,
                        (LPSTR)szSelectedValue,
                        VALUE_MAX_SIZE);
		    hwndServerDDE = GetHwndServerDDE(hwndSelectedClientDDE);
		    SendPoke(hwndSelectedClientDDE,
			hwndServerDDE,
                        szSelectedItem,
                        szSelectedValue);
		    return (TRUE);

                case IDC_CANCEL:
                    EndDialog(hdlg, FALSE);
		    return (TRUE);
            }

        default:
            return (FALSE);
    }
    return (FALSE);
}


/****************************************************************************

    FUNCTION: RequestDlgProc

    PURPOSE:  Processes messages for the Request dialog.

****************************************************************************/
BOOL FAR PASCAL RequestDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND  hwndServerDDE;

    switch (message) 
    {
        case WM_INITDIALOG:
            hwndRequestDlg = hdlg;
            bInRequestDlg = TRUE;
	    AddConversationsToBox(hdlg, CB_ADDSTRING);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
		    GetSelectedConversation(hdlg, CB_GETCURSEL, CB_GETLBTEXT);
                    GetDlgItemText(hdlg, 
                        IDC_ITEM,
                        (LPSTR)szSelectedItem,
                        ITEM_MAX_SIZE);
		    hwndServerDDE = GetHwndServerDDE(hwndSelectedClientDDE);
		    SendRequest(hwndSelectedClientDDE,
			hwndServerDDE,
			szSelectedItem);
		    return (TRUE);

                case IDC_CANCEL:
                    bInRequestDlg = FALSE;
                    EndDialog(hdlg, FALSE);
                    return (TRUE);
            }
            break;

        case WM_DESTROY:
            bInRequestDlg = FALSE;
            return (FALSE);

        default:
            return (FALSE);
    }
    return (FALSE);
}



/****************************************************************************

    FUNCTION: TerminateDlgProc

    PURPOSE:  Processes messages for the Terminate dialog.

****************************************************************************/
BOOL FAR PASCAL TerminateDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND hwndServerDDE;

    switch (message) 
    {
        case WM_INITDIALOG:
	    AddConversationsToBox(hdlg, LB_ADDSTRING);
            return (TRUE);
        
        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
		    GetSelectedConversation(hdlg, LB_GETCURSEL, LB_GETTEXT);
                    EndDialog(hdlg, TRUE);
		    hwndServerDDE = GetHwndServerDDE(hwndSelectedClientDDE);
		    SendTerminate(hwndSelectedClientDDE, hwndServerDDE);
		    return (TRUE);

                case IDC_CANCEL:
                    EndDialog(hdlg, FALSE);
		    return (TRUE);
            }
        default:
            return (FALSE);
    }
    return (FALSE);
}




/****************************************************************************

    FUNCTION: UnadviseDlgProc

    PURPOSE:  Processes messages for the Unadvise dialog.

****************************************************************************/
BOOL FAR PASCAL UnadviseDlgProc(hdlg, message, wParam, lParam)
    HWND      hdlg;
    unsigned  message;
    WORD      wParam;
    LONG      lParam;
{
    HWND  hctlItemBox;
    int   nIndex;
    HWND  hwndServerDDE;

    switch (message) 
    {
        case WM_INITDIALOG:
	    AddConversationsToBox(hdlg, CB_ADDSTRING);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam) 
            {
                case IDC_OK:
		    GetSelectedConversation(hdlg, CB_GETCURSEL, CB_GETLBTEXT);
                    GetDlgItemText(hdlg, 
                        IDC_ITEM,
                        (LPSTR)szSelectedItem,
                        ITEM_MAX_SIZE);
		    hwndServerDDE = GetHwndServerDDE(hwndSelectedClientDDE);
		    SendUnadvise(hwndSelectedClientDDE,
			hwndServerDDE,
                        szSelectedItem);
		    return (TRUE);

                case IDC_CANCEL:
                    EndDialog(hdlg, FALSE);
                    return (TRUE);
            }
        default:
            return (FALSE);
    }
    return (FALSE);
}



/****************************************************************

    FUNCTION: AddConversationsToBox

    PURPOSE:  Add server, app, topic info to client list box
   

****************************************************************/
void AddConversationsToBox(hdlg, nAddMessage)
    HWND      hdlg;
    unsigned  nAddMessage;  /* LB_ADDSTRING or CB_ADDSTRING */
{
    HWND  hwndClientDDE;
    HWND  hctlServerList;
    char  szConvInfo[CONVINFO_MAX_SIZE+1];
    char  szApp[APP_MAX_SIZE+1];
    char  szTopic[TOPIC_MAX_SIZE+1];

    hctlServerList = GetDlgItem(hdlg, IDC_CONVBOX);
    SendMessage(hctlServerList,
        nAddMessage == LB_ADDSTRING ? LB_RESETCONTENT : CB_RESETCONTENT,
        0,
        0L);
    hwndClientDDE = NULL;
    while (1)
    {
	if (!(hwndClientDDE = GetNextConv(hwndClientDDE)))
	    break;
	itoa((int)hwndClientDDE, szConvInfo, 16);
	strupr(szConvInfo);
	strcat(szConvInfo,"->");
	itoa((int)GetHwndServerDDE(hwndClientDDE),
	    szConvInfo+strlen(szConvInfo),
	    16);
	strcat(szConvInfo," ");
	GetAppAndTopic(hwndClientDDE, szApp, szTopic);
	strcat(szConvInfo, szApp);
	strcat(szConvInfo, " | ");
	strcat(szConvInfo, szTopic);

        SendMessage(hctlServerList,
            nAddMessage,
            0,
	    (LONG)(LPSTR)szConvInfo);
    }
    if (nAddMessage == CB_ADDSTRING)
    {
	SendMessage(hctlServerList, CB_SETCURSEL, 0, 0L);
    }
    return;
}


/*********************************************************************

    FUNCTION: IsInRequestDlg

    PURPOSE:  Returns whether the user is in the Request dialog or not.

*********************************************************************/
BOOL IsInRequestDlg()
{
    return (bInRequestDlg);
}



/*********************************************************************

    FUNCTION: GetSelectedConversation

    PURPOSE:  Gets the user's selection from the conversation list
	      box, and returns result in hwndSelectedClientDDE.

*********************************************************************/
void NEAR GetSelectedConversation(hdlg, nCurSelMessage, nGetTextMessage)
    HWND      hdlg;
    unsigned  nCurSelMessage;
    unsigned  nGetTextMessage;
{
    HWND hctlConvBox;
    int  nIndex;
    char szConvInfo[CONVINFO_MAX_SIZE+1];
    char * pcBlank;
    

    hctlConvBox = GetDlgItem(hdlg, IDC_CONVBOX);
    if ((nIndex = SendMessage(hctlConvBox, nCurSelMessage, 0, 0L))
        != LB_ERR)
    {
	szConvInfo[0] = 0;
	SendMessage(hctlConvBox,
            nGetTextMessage, 
            nIndex, 
	    (LONG)(LPSTR)szConvInfo);
	/* find '-' in 'hwnd->hwnd' list box entry */
	if (!(pcBlank = strchr(szConvInfo, '-')))
            return;
        *pcBlank = 0;  /* terminate hwnd numeric value */
	hwndSelectedClientDDE = HexToInt(szConvInfo);
     }
     return;
}



/****************************************************************

    FUNCTION: InitDataTextMetrics

    PURPOSE:  Get font information

****************************************************************/
void InitDataTextMetrics()
{
    HDC hDC;
    TEXTMETRIC tm;
   
    hDC = GetDC(hwndMain);
    GetTextMetrics(hDC, (LPTEXTMETRIC)&tm);
    nHorzRes = GetDeviceCaps(hDC, HORZRES);
    nVertRes = GetDeviceCaps(hDC, VERTRES);
    ReleaseDC(hwndMain, hDC);
    yDelta = tm.tmHeight + tm.tmExternalLeading;
    xDelta = tm.tmAveCharWidth;
    return;
}


/*********************************************************************

    FUNCTION: RequestSatisfied

    PURPOSE:  Updates Request dialog box with value requested from
	      server.

*********************************************************************/
void RequestSatisfied(lpszValue)
    LPSTR lpszValue;
{
    HWND hctlValue;

    hctlValue = GetDlgItem(hwndRequestDlg, IDC_VALUE);
    SetWindowText(hctlValue, lpszValue);
    return;
}


/*********************************************************************

    FUNCTION: SetSelectedValue

    PURPOSE:  Set specified value.

*********************************************************************/
void SetSelectedValue(lpszSelectedValue)
    LPSTR lpszSelectedValue;
{
    lstrcpy((LPSTR)szSelectedValue, lpszSelectedValue);
    return;
}
