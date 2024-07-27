#include "nodefs.h"
#include "windows.h"
#include "winexp.h"
#include "spool.h"
#include "spooler.h"

long dnsTimeout;     /* 22-Apr-1987. holds timeout values from win.ini */
long txTimeout;

HANDLE screen[MAXSPOOL * MAXPORT];
short	maxline;
short maxport = 0;
short diskneeded;
short beepcnt = 0;
short ActiveJobs = 0;
char OutOfMemString[IDS_LENGTH];
char SpoolerName[NAME_LEN];
SERVER *servers[MAXPORT] = {0,0,0,0,0};
FARPROC lpprocAbout;

short charwidth, charheight;
short tos, curjob;	/* top of screen */

short high = FALSE;
short SpState = 0;
short flashid = 0;
short CommError;
unsigned wMerge = 0;

FARPROC TimerProc;
HWND  hInst, hWnd = 0;

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND		hDlg;
unsigned	message;
WORD		wParam;
LONG		lParam;
{
    if (message == WM_COMMAND) {
	EndDialog( hDlg, TRUE );
	return TRUE;
	}
    if (message == WM_INITDIALOG)
	return	TRUE;
    else return FALSE;
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
	register SERVER *server;
	MSG msg;
	PSTR buf;

	hWnd = MainInit(hInstance);

	/* don't want multiple instances of the spooler */
	hInst = hInstance;

	if (hPrevInstance)
		{
		SpoolerError(IDS_NAME, IDS_MULTSPOOLER, MB_SYSTEMMODAL | MB_ICONEXCLAMATION, (LPSTR)0);
		_exit(0);
		}

	/* check win.ini for spooler = no line */
	if (checkWinIni() == TRUE)
		_exit(0);

	if (checkconnection(TRUE) == EOF)
		_exit(0);

	lpprocAbout = MakeProcInstance( (FARPROC)About, hInstance);
	TimerProc = MakeProcInstance((FARPROC)SpoolerWndProc, hInst);

        ShowWindow(hWnd, cmdShow);
	SpoolerCreate(hWnd);
	UpdateWindow(hWnd);

	while (TRUE)
		{
		register short j;

		for (j = 0; j < MAXPORT; j++)
			{
			if (!(server = servers[j]))
				continue;

			if (!server->hJCB || (server->pause && server->type != EOF))
				continue;

			if (Output(server) == FALSE)
				{
				FJCB jcb;

				jcb = (FJCB) GlobalLock(server->hJCB);
				if (server->type == EOF || refill(server, jcb) == EOF)
					{
					/* this unlock must be done before calling
					   FreeAll() */
					GlobalUnlock(server->hJCB);
					FreeAll(server->hJCB);
					server->pause &= SP_CLEARSYSTEM;
					AddSpoolJob(server, server->hJCB, SP_DELETEJOB);
					UpdateScreen();
					SpoolerPaint(hWnd);
					}
				else
					GlobalUnlock(server->hJCB);
				}
			}

		if (ActiveJobs)
			{
			if (!PeekMessage((LPMSG)&msg, (HWND)NULL, 0, 0, TRUE))
				continue;
			}
		else
			GetMessage((LPMSG)&msg, (HWND)NULL, 0, 0);
		if (msg.message == WM_QUIT)
			_exit(0);
		TranslateMessage((LPMSG)&msg);
		DispatchMessage((LPMSG)&msg);
		}
}


/* Procedures which make up the window class. */
long FAR PASCAL SpoolerWndProc(hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
{
    PAINTSTRUCT  ps;
    short i, tmp;

    switch (message)
    {
	case WM_LBUTTONDOWN:
	    SpoolerMouse(hWnd, message, wParam, MAKEPOINT(lParam));
	    break;

	case SP_QUERYDISKAVAIL:
	    return CalcDiskAvail();

	case SP_DELETEJOB:
	    if (!servers[(short)lParam])
		    break;

	    if (GlobalSize(wParam) < sizeof(JCB))
		    {
		    GlobalFree(wParam);
		    break;
		    }

	    if (flashid)
		    {
		    KillTimer(hWnd, hWnd);
		    flashid = 0;
		    FlashWindow(hWnd, FALSE);
		    }

	    if (servers[lParam]->hJCB == wParam)
		    {
/* COMM driver */
		    ResetPort(servers[lParam]->portfn);
		    servers[lParam]->type = EOF;
		    break;
		    }

	    FreeAll(wParam);
	    --curjob;
	    /* fall through */

	case SP_NEWJOB:
	    if (!servers[lParam])
		    break;
	    if (AddSpoolJob(servers[lParam], wParam, message) == EOF)
		    return EOF;

	    UpdateScreen();

	    /* change the current job to this one, if this is
	       the currently printing job */

	    for (i = 1; i < maxline; i++)
		    /* new job is located just after a port */
		    if (screen[i] == wParam && screen[i-1] < MAXPORT)
			    {
			    curjob = i;
			    break;
			    }

	    SpoolerPaint(hWnd);
	    break;

	case WM_PAINT:
	    BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
	    SpoolerPaint(hWnd);
	    EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
	    break;

	case SP_DISKNEEDED:
	    diskneeded = wParam;
	    if (lParam)
		    SpoolerError(IDS_NAME, IDS_OUTOFDISK, MB_SYSTEMMODAL | MB_ICONHAND, (LPSTR)lParam);
	    break;

	case WM_TIMER:
	    FlashWindow(hWnd, TRUE);
	    if (!--beepcnt)
		    {
		    MessageBeep(1);
		    beepcnt = BEEPFREQ;
		    }
	    break;

	case WM_KEYDOWN:
	    SpoolerKey(hWnd, wParam);
	    break;

	case WM_VSCROLL:
	    if (SpoolerVertScroll(hWnd, wParam, LOWORD(lParam)))
		    SpoolerPaint(hWnd);
	    break;

#ifdef DISABLE
	case WM_SYSCOMMAND:
	    switch (wParam) {
		case SP_ABOUT:	/* About ... */
			DialogBox(hInst, (LPSTR)"about", hWnd,
				lpprocAbout);
		    break;
		default:
		    return(DefWindowProc(hWnd, message, wParam, lParam));
		    break;
		}
	    break;
#endif

	case WM_ACTIVATE:
	    if (wParam && ! HIWORD(lParam))
		    SetFocus(hWnd);
	    for (i = 0; i < MAXPORT; i++)
		    if (servers[i])
			    servers[i]->pause &= SP_CLEARATTENTION;
	    if (flashid)
		    {
		    KillTimer(hWnd, hWnd);
		    flashid = 0;
		    FlashWindow(hWnd, FALSE);
		    }
	    break;

	case WM_SIZE:
	    if (wParam != SIZEICONIC)
		SpoolerSize(hWnd, LOWORD(lParam), HIWORD(lParam));
	    break;

#if 0
	case WM_SHOWWINDOW:
	    if (LWORD(lParam) == SW_OTHERZOOM)
		    SpState |= SP_OTHERZOOMED;
	    else
		    SpState &= ~SP_OTHERZOOMED;
	    break;
#endif

	case WM_COMMAND:
	    SpoolerCommand(hWnd, wParam, LOWORD(lParam));
	    break;

	case WM_QUERYENDSESSION:
	    return OKToClose(MB_SYSTEMMODAL);

	case WM_CLOSE:
	    if (!OKToClose(FALSE))
		    return TRUE;
	    GetSpoolJob(SP_REGISTER,(long)0);
	    DestroyWindow(hWnd);
	    break;

	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

	case WM_QUIT:
	    _exit(wParam);

	case WM_WININICHANGE:
            /* 22-Apr-1987. davidhab */
            if (!lstrcmp((LPSTR)lParam, "windows")) {
                GetWinIniTimeouts();
            }

	    if (!(tmp = lstrcmp((LPSTR)lParam, "devices")) || !lstrcmp((LPSTR)lParam, "ports"))
		    for (i = 0; i < MAXPORT; i++)
			    if (servers[i])
				    {
				    ClosePort(servers[i]->portfn);
				    servers[i]->portfn = SP_RETRY;
				    }

	    /* devices section also changed */
	    if (!tmp)
		    {
		    GetSpoolJob(SP_INIT, (long)0);
		    i = checkconnection(FALSE);
		    UpdateScreen();
		    SpoolerPaint(hWnd);
		    }
#if 0
	    /* spooler will close itself it there are no connection */
	    if (i == EOF)
		    if (OKToClose(FALSE))
			    {
			    GetSpoolJob(SP_REGISTER,(long)0);
			    PostQuitMessage(0);
			    }
#endif
	    break;

	default:

	    return(DefWindowProc(hWnd, message, wParam, lParam));
    }

    return(0L);
}

SpoolerSize(hWnd, x, y)
HWND hWnd;
short x, y;
{
	SetScrollRange(hWnd, SB_VERT, 0, (y < maxline * charheight || tos? 100: 0), FALSE);
}

SpoolerCommand(hWnd, id, hJCB)
HWND hWnd;
int id;
HANDLE hJCB;
{
	register short i;
	JCB far * jcb;
	char buf[MAX_PROFILE];
	char buf2[MAX_PROFILE];
	char buf3[MAX_PROFILE];

	switch(id)
	{
        case ABOUT:
            DialogBox(hInst, (LPSTR)"about", hWnd, lpprocAbout);
            break;
        case EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0L);
            break;

	case TERMINATE:
		if (curjob >= maxline || (hJCB = screen[curjob]) < MAXPORT)
		    return;

		pause(curjob, SP_SYSTEMPAUSE);
		/* will change to use dialog box */
		jcb = (JCB FAR *) GlobalLock(hJCB);
		LoadString(hInst, IDS_TERMINATE, buf, MAX_PROFILE-lstrlen(jcb->jobName));
		LoadString(hInst, IDS_NAME, buf2, MAX_PROFILE);
		MergeStrings((LPSTR)buf, jcb->jobName, (LPSTR)buf3);
		GlobalUnlock(hJCB);
		if (MessageBox(hWnd, (LPSTR) buf3,
			(LPSTR) buf2,
			MB_ICONQUESTION | MB_OKCANCEL) != MB_OKCANCEL)
			{
			pause(curjob, SP_CLEARSYSTEM);
			return;
			}

		for (i = 0; i < MAXPORT; i++)
		    if (servers[i] && servers[i]->hJCB == hJCB)
			{
/* COMM driver */
			ResetPort(servers[i]->portfn);
			servers[i]->type = EOF;
			return;
			}

		pause(curjob, SP_CLEARSYSTEM);
		--curjob;
		for (i = 0; screen[curjob - i] > MAXPORT; i++);
		FreeAll(hJCB);
		AddSpoolJob(servers[screen[curjob - i]], hJCB, SP_DELETEJOB);
		UpdateScreen();
		SpoolerPaint(hWnd);
		break;

	case PAUSE:
	case RESUME:
		pause(curjob, (id == PAUSE ? SP_USERPAUSE: SP_CLEARUSER));
		SpoolerPaint(hWnd);
		break;

	case LOW:
		if (high)
			{
			CheckMenuItem(GetMenu(hWnd), LOW, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), HIGH, MF_UNCHECKED);
			high = FALSE;
			}
		break;

	case HIGH:
		if (!high)
			{
			CheckMenuItem(GetMenu(hWnd), HIGH, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), LOW, MF_UNCHECKED);
			high = TRUE;
			}
		break;
	}
}


SpoolerPaint(hWnd)
HWND hWnd;
{
	short i, y = -charheight, maxscreen;
	RECT rect;
	HANDLE hJCB, hDC, *screenptr;
	register JCB far * jcb;
	char buf1[NAME_LEN], buf2[NAME_LEN];
	LPSTR bufptr;
	HBRUSH hbr;

        /* don't paint if iconic. 22-Apr-1987. davidhab. */
        if (IsIconic(hWnd))
            return;

	GetClientRect(hWnd, (LPRECT) &rect);
	SetScrollRange(hWnd, SB_VERT, 0, (rect.bottom < maxline * charheight || tos? 100: 0), FALSE);

	if (IsRectEmpty(&rect))
		return;

	hDC =	GetDC(hWnd);
	hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	SelectObject(hDC, hbr);
	PatBlt(hDC, 0, 0, rect.right, rect.bottom, PATCOPY);

	SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
	SetBkMode(hDC, TRANSPARENT);

	LoadString(hInst, IDS_PAUSED, buf1, NAME_LEN);
	LoadString(hInst, IDS_ACTIVE, buf2, NAME_LEN);

	maxscreen = rect.bottom / charheight;
	if (rect.bottom % charheight)
		maxscreen++;

	if (maxline - tos < maxscreen)
		maxscreen = maxline - tos;

	for (i = 0, screenptr = &screen[tos]; i < maxscreen; i++, screenptr++)
		{
		if ((hJCB = *screenptr) < MAXPORT)
			{
			register short x;
			register SERVER * server;

			server = servers[hJCB];
			x = charwidth << PORTINDENT;
			TextOut(hDC, x,  y += charheight,
			    (LPSTR) server->portname, server->portlength);
			x += (server->portlength + 1) * charwidth;
			bufptr = servers[hJCB]->pause ? (LPSTR) buf1: (LPSTR) buf2;
			TextOut(hDC, x, y, bufptr, lstrlen(bufptr));
			x += (lstrlen(bufptr) + 1) * charwidth;
			TextOut(hDC, x,  y, (LPSTR) server->printername,
				    server->printerlength);
			}
		else
			{
			jcb = (JCB far *) GlobalLock(hJCB);
			TextOut(hDC,charwidth<< JOBINDENT, y += charheight,
				 jcb->jobName, lstrlen(jcb->jobName));
			GlobalUnlock(hJCB);
			}
		}
	if (maxline && curjob >= maxline)
		curjob = maxline - 1;
	EnableMenuItem(GetMenu(hWnd), TERMINATE,
			    screen[curjob] > MAXPORT? MF_ENABLED: MF_GRAYED);
	/* no printers installed, blank screen */
	if (y < 0)
		{
		LoadString(hInst, IDS_NOPRINTER, buf1, NAME_LEN);
		TextOut(hDC, charwidth << JOBINDENT, 0, buf1, lstrlen(buf1));
		}
	else
		InvertLine(hDC, curjob, rect.right);

	ReleaseDC(hWnd, hDC);
	DeleteObject(hbr);
}


SpoolerMouse(hWnd, message, wParam, lParam)
HWND	   hWnd;
unsigned   message;
WORD	   wParam;
POINT	   lParam;
{
    short index;
    HANDLE hDC;
    RECT rect;

    if (message == WM_LBUTTONDOWN)
    {
	index = lParam.y / charheight + tos;
	if (index >= maxline || index == curjob)
	    return;
	hDC = GetDC(hWnd);
	GetClientRect(hWnd, (LPRECT) &rect);
	InvertLine(hDC, curjob, rect.right);
	InvertLine(hDC, index, rect.right);
	EnableMenuItem(GetMenu(hWnd), TERMINATE, screen[index] > MAXPORT? MF_ENABLED: MF_GRAYED);
	curjob = index;
	ReleaseDC(hWnd, hDC);
    }
}

FAR InvertLine(hDC, n, width)
HANDLE hDC;
short n, width;
{
	n -= tos;
	PatBlt(hDC, 0, n * charheight, width, charheight, DSTINVERT);
}


/* returns false if all chars are used up */

short Output(server)
SERVER *server;
{
	MSG msg;
	short fn, i, charcnt, charsent;
	register PSTR buf;


	if (!(i = server->bufend - server->bufstart))
		return FALSE;

	if (server->type == EOF)
		return FALSE;

	if (server->type == DIALOG)
		{
		if (/* (SpState & SP_OTHERZOOMED) || */ GetActiveWindow() != hWnd)
			{
			server->pause |= SP_ATTENTIONPAUSE;
			MessageBeep(1);
			MessageBeep(1);
			if (!flashid)
				flashid = SetTimer(hWnd, hWnd, 500, TimerProc);
			return TRUE;
			}
		server->buffer[server->bufend] = '\0';

		server->valid |= SP_JCBLOCKED;
		if (MessageBox(hWnd, (LPSTR) server->buffer,
			(LPSTR) server->portname,
			MB_OKCANCEL) == IDCANCEL)
		    server->type = EOF;
		server->valid &= ~SP_JCBLOCKED;
		return FALSE;
		}

	buf = &server->buffer[server->bufstart];

	if (server->portfn == SP_RETRY)
		{
		if ((fn = OpenPort(server->portname)) == SP_RETRY)
			return i;
		if (fn == SP_CANCEL)
			{
			server->type = EOF;
			return 0;
			}
		server->portfn = fn;
		}
	else
		fn = server->portfn;

	if (diskneeded || high || (((PORT *)fn)->type & SP_FILE_PORT))
		charcnt = i;
	else
		charcnt = (i > CHAR_TRANS? CHAR_TRANS: i);


	if ((charsent = WritePort(fn, buf, charcnt)) & COMM_ERR_BIT)
		{
		register errcode;

		if (SpoolerError(IDS_NAME, IDS_COMMERROR, MB_SYSTEMMODAL | MB_RETRYCANCEL | MB_ICONHAND, (LPSTR)server->portname)
			== IDCANCEL)
			{
			server->type = EOF;
			if (CommError & CE_DNS)
				ResetPort(fn);
			ClosePort(fn);
			server->portfn = SP_RETRY;
			return FALSE;
			}
		charsent &= ~COMM_ERR_BIT;
		}

	server->bufstart += charsent;

	return i - charsent;
}


HandleOutOfMemory()
{
	MessageBox((HWND)hWnd, (LPSTR)OutOfMemString, (LPSTR)SpoolerName, MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
}


/* ** Scan sz1 for merge spec.	If found, insert string sz2 at that point.
      Then append rest of sz1 NOTE! Merge spec guaranteed to be two chars.
      returns TRUE if it does a merge, false otherwise. */
BOOL  MergeStrings(lpszSrc, lpszMerge, lpszDst)
LPSTR lpszSrc;
LPSTR lpszMerge;
LPSTR lpszDst;
{
    LPSTR lpchSrc;
    LPSTR lpchDst;

    lpchSrc = lpszSrc;
    lpchDst = lpszDst;

    /* Find merge spec if there is one. */
    while (*(unsigned far *)lpchSrc != wMerge) {
	*lpchDst++ = *lpchSrc;

	/* If we reach end of string before merge spec, just return. */
	if (!*lpchSrc++)
	    return FALSE;

    }
    /* If merge spec found, insert sz2 there. (check for null merge string */
    if (lpszMerge) {
	while (*lpszMerge)
	    *lpchDst++ = *lpszMerge++;

    }

    /* Jump over merge spec */
    lpchSrc++,lpchSrc++;


    /* Now append rest of Src String */
    while (*lpchDst++ = *lpchSrc++);
    return TRUE;

}


TryAutoShutdown()
/* spooler commits hari-kari here when the last job has printed - 
   Bob Matthews, 7/28/87 */

    {
    if (IsIconic(hWnd) && hWnd != GetActiveWindow())
	PostMessage(hWnd, WM_CLOSE, 0, 0L);
    }
