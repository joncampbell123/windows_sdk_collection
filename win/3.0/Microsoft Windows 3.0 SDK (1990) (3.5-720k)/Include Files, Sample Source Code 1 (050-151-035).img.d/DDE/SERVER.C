/***************************************************************************

    PROGRAM: SERVER

    PURPOSE: Illustrates server side of DDE conversation

    MODULES:

        SERVER.C    Window and dialog procedures.
	SERVDATA.C  Maintains conversation information.
        SERVDDE.C   Processes incoming and outgoing DDE messages.

****************************************************************************/

#include "windows.h"
#include "dde.h" 
#include "server.h"
#include "servres.h"
#include <string.h>
#include <stdlib.h>



static int     xDelta;
static int     yDelta;

long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL InitApplication(HANDLE);
void InitAddedInstance(HANDLE, HANDLE);
BOOL InitInstance(HANDLE, int);
void MaybeAdviseData(int);
BOOL FAR PASCAL AboutDlgProc(HWND, unsigned, WORD, LONG);
    

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE:  Calls initialization function, processes message loop

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{
    MSG msg;
    HANDLE hAccel;

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

    hAccel = LoadAccelerators(hInstance, "ServerAcc");

    while (GetMessage(&msg, NULL, NULL, NULL)) 
    {
	if (!TranslateAccelerator(hwndMain, hAccel, &msg))
	{
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}

    }
    return (msg.wParam);
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    nDoc = 1;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  "ServerMenu";
    wc.lpszClassName = "ServerWClass";

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
    wc.lpszClassName = "ServerDDEWndClass";

    return (RegisterClass(&wc));
}


/****************************************************************************

    FUNCTION: InitAddedInstance

    PURPOSE:  Increment document number

****************************************************************************/
void InitAddedInstance(hInstance, hPrevInstance)
    HANDLE  hInstance;
    HANDLE  hPrevInstance;
{
    GetInstanceData(hPrevInstance, (NPSTR)&nDoc, sizeof(int));
    nDoc++;
    return;
}



/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle, creates main window, and creates
	      3 child edit controls with id's 1, 2, and 3.

****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;
    int             nCmdShow;
{
    char        szNumber[4];
    char        szCaption[20];
    HDC         hDC;
    PAINTSTRUCT ps;
    TEXTMETRIC  tm;
    int         nItem;
    int         nHorzRes, nVertRes;

    InitAckTimeOut(); /* in module SERVDDE */

    hInst = hInstance;

    strcpy(szDocName, "FILE");
    itoa(nDoc, szNumber, 10);
    strcat(szDocName, szNumber);
    strcpy(szCaption, "Server -- ");
    strcat(szCaption, szDocName);

    hwndMain = CreateWindow(
        "ServerWClass",
        szCaption,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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

    hDC = GetDC(hwndMain);
    GetTextMetrics(hDC, (LPTEXTMETRIC)&tm);
    xDelta = tm.tmAveCharWidth;
    yDelta = tm.tmHeight + tm.tmExternalLeading;
    nHorzRes = GetDeviceCaps(hDC, HORZRES);
    nVertRes = GetDeviceCaps(hDC, VERTRES);
    ReleaseDC(hwndMain, hDC);

    MoveWindow(hwndMain,
	nHorzRes/2 + xDelta*(nDoc+5),
        ((nDoc-1)&1)*nVertRes/2 + yDelta*nDoc,
        xDelta*30, 
	yDelta*12,
        FALSE);

    for (nItem = 1; nItem < 4; nItem++)
    {
        CreateWindow("edit", NULL,
	    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | WS_TABSTOP,
            9*xDelta, (2*nItem-1)*yDelta, 12*xDelta, (yDelta*3)/2,
	    hwndMain, nItem, hInst, NULL);
    }

    if (!(cfLink = RegisterClipboardFormat("Link")))
	return (FALSE);

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);


    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for server

****************************************************************************/

long FAR PASCAL MainWndProc(hwnd, message, wParam, lParam)
HWND hwnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    HDC  hDC;
    PAINTSTRUCT ps;
    HWND hctlItem;
    int  nItem;
    char szItemName[8];
    FARPROC lpAboutDlgProc;


    switch (message)
    {
        case WM_SETFOCUS:
            SetFocus(GetDlgItem(hwnd,1));
            break;

        case WM_PAINT:
	    hDC = BeginPaint(hwnd, &ps);
            strcpy(szItemName, "Item1:");
            for (nItem = 1; nItem < 4; nItem++)
            {
		/* display labels for the edit controls */
                TextOut(hDC, xDelta, (2*nItem-1)*yDelta, szItemName, 6);
                szItemName[4]++;
            }
	    EndPaint(hwnd, &ps);
            break;

        case WM_COMMAND:
            switch (wParam) 
            {
               case IDM_COPY:
		   hctlItem = GetFocus();
		   for (nItem = 1; nItem <= 3; nItem++)
		   {
		       if (hctlItem == GetDlgItem(hwnd, nItem))
		       {
			   DoEditCopy(nItem);
			   break;
		       }
		   }
		   break;

	       case ID_TAB:
	       case ID_SHIFT_TAB:
		   if (IsChild(hwndMain, GetFocus()))
		   {
		       nItem = GetWindowWord(GetFocus(), GWW_ID);
		       if (wParam == ID_TAB)
		       {
			   if (nItem++ == 3)
			       nItem = 1;
		       }
		       else
		       {
			   if (nItem-- == 1)
			       nItem = 3;
		       }
		   }
		   else
		   {
		       nItem = 1;
		   }
		   SetFocus(GetDlgItem(hwndMain, nItem));
		   break;

	       case 1:
               case 2:
               case 3:
                  if (HIWORD(lParam)==EN_KILLFOCUS)
                  {
                     hctlItem = GetDlgItem(hwnd, wParam);
                     if (SendMessage(hctlItem, EM_GETMODIFY, 0, 0L))
                     {
			MaybeAdviseData(wParam);
                        SendMessage(hctlItem, EM_SETMODIFY, 0, 0L);
                     }
                  }
                  break;

		case IDM_ABOUT:
		    lpAboutDlgProc = MakeProcInstance(AboutDlgProc, hInst);
		    DialogBox(hInst,
			"About",
			hwndMain,
			lpAboutDlgProc);
		    FreeProcInstance(lpAboutDlgProc);
		    break;
	    }
            break;

        case WM_DDE_INITIATE:
            ServerInitiate((HWND)wParam, lParam);
            break;

        case WM_DESTROY:

            /* Terminate all DDE conversations before destroying 
               client window */
	    TerminateConversations();
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hwnd, message, wParam, lParam));
    }
    return (0L);
}



/****************************************************************************

    FUNCTION: MaybeAdviseData

    PURPOSE:  Send data to all clients for which a hot or warm link
	      has been established for the specified item.

****************************************************************************/
void MaybeAdviseData(nItem)
    int   nItem;
{
    HWND hwndServerDDE;
    char szItemName[ITEM_NAME_MAX_SIZE+1];
    char szItemValue[ITEM_VALUE_MAX_SIZE+1];
    BOOL bDeferUpdate;
    BOOL bAckRequest;

    hwndServerDDE = NULL;
    
    while (1)
    {
	if (hwndServerDDE = GetNextAdvise(hwndServerDDE, nItem))
        {
	    GetAdviseData(hwndServerDDE,
                nItem,
                szItemName,
                szItemValue,
                &bDeferUpdate,
                &bAckRequest); 

	    SendData(hwndServerDDE,
		GetHwndClientDDE(hwndServerDDE),
                szItemName,
                szItemValue,
                bDeferUpdate,
                bAckRequest);
        }
        else return;
    }
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
