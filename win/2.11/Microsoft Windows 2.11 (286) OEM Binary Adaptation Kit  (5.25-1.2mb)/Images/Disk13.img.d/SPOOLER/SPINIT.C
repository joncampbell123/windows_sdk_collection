#include "nodefs.h"
#include "windows.h"
#include "winexp.h"
#include "spool.h"
#include "spooler.h"

MainInit(hInstance)
HANDLE hInstance;
{
	HMENU hMenu;
	HWND  hWnd;
	char szAbout[IDS_LENGTH];
	char	szMerge[3];
	extern unsigned wMerge;

#ifdef DISABLE
	LoadString(hInstance, IDS_ABOUT, (LPSTR)szAbout, IDS_LENGTH);
#endif
	LoadString(hInstance, IDS_NAME, (LPSTR)SpoolerName, NAME_LEN);
	LoadString(hInstance, IDS_OUTOFMEMORY, (LPSTR)OutOfMemString, IDS_LENGTH);
	LoadString(hInstance, IDS_MERGE, (LPSTR)szMerge, 3);
	wMerge = *(unsigned *)szMerge;

	SpoolerInit(hInstance, (LPSTR) SpoolerName);

	hWnd = CreateWindow((LPSTR) SpoolerName,
		      (LPSTR) SpoolerName,
		      (long) WS_TILEDWINDOW,
                      CW_USEDEFAULT,
                      0,
                      CW_USEDEFAULT,
                      0,
		      (HWND)NULL,      /* no parent */
		      (HMENU)NULL,  /* use class menu */
		      (HANDLE)hInstance, /* handle to window instance */
		      (LPSTR)NULL	 /* no params to pass on */
		      );

#ifdef DISABLE
	hMenu = GetSystemMenu(hWnd, FALSE);
	ChangeMenu(hMenu, 0, NULL, SP_ABOUT, MF_APPEND | MF_SEPARATOR);
	ChangeMenu(hMenu, 0, (LPSTR)szAbout, SP_ABOUT, MF_APPEND | MF_STRING);
#endif
	return hWnd;
}

/* Procedure called when the application is loaded */
short FAR SpoolerInit( hInstance, name)
HANDLE hInstance;
LPSTR  name;
{
    register PWNDCLASS	 pSpoolerClass;

    /* Allocate class structure in local heap */
    pSpoolerClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    /* get necessary resources */
    pSpoolerClass->hCursor	 = LoadCursor( NULL, MAKEINTRESOURCE(IDC_ARROW));
    pSpoolerClass->hIcon	 = LoadIcon( hInstance, (LPSTR)name);
    pSpoolerClass->lpszMenuName  = (LPSTR)name;
    pSpoolerClass->hInstance	 = hInstance;
    pSpoolerClass->lpszClassName = (LPSTR)name;
    pSpoolerClass->hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    pSpoolerClass->lpfnWndProc	 = SpoolerWndProc;
    pSpoolerClass->style         = CS_BYTEALIGNCLIENT;

    /* register this new class with WINDOWS */
    if (!RegisterClass( (LPWNDCLASS)pSpoolerClass ) )
	return FALSE;	/* Initialization failed */

    LocalFree( (HANDLE)pSpoolerClass );

    return TRUE;    /* Initialization succeeded */
}

FAR SpoolerCreate(hWnd)
HANDLE hWnd;
{
	register short i, len;
	HDC hDC;
	TEXTMETRIC tmLocalTM;

	hDC = GetDC(hWnd);
	GetTextMetrics( hDC, (TEXTMETRIC FAR *)&tmLocalTM );
	ReleaseDC(hWnd, hDC);

	charheight = tmLocalTM.tmHeight + tmLocalTM.tmInternalLeading;
	charwidth = tmLocalTM.tmAveCharWidth;
	curjob = 0;
	tos = 0;
	GetSpoolJob(SP_REGISTER, (long) hWnd);
	UpdateScreen();
}

checkWinIni()
{
	char buf[MAX_PROFILE];

	if (GetProfileString( (LPSTR)WININI_SECTION,	     /* "app" name */
				    (LPSTR)WININI_LHS,
				    (LPSTR)"",	  /* default empty string */
				    (LPSTR)buf,   /* result string */
				    MAX_PROFILE    /* max chars to plcae in buffer */
				    ))
		if (!lstrcmp((LPSTR)buf, (LPSTR)WININI_RHS))
			{
			SpoolerError(IDS_NAME, IDS_NOSPOOLER, MB_SYSTEMMODAL, (LPSTR) 0);
			return TRUE;
			}

        GetWinIniTimeouts();

	return FALSE;
}


GetWinIniTimeouts()
{
    extern long dnsTimeout;
    extern long txTimeout;

    /* 02-Mar-1987 davidhab. Try to read timeout values from win.ini.
     * If they don't exist, use defaults.  Values in win.ini are in
     * seconds, and get converted to milliseconds.
     */
    dnsTimeout = 1000L * (LONG)GetProfileInt((LPSTR)"windows",  /* section title */
                            (LPSTR)"DeviceNotSelectedTimeout",  /* keyname       */
                            (int)(DNSTIMEOUT/1000));            /* default value */


    txTimeout = 1000L * (LONG)GetProfileInt((LPSTR)"windows",   /* section title */
                            (LPSTR)"TransmissionRetryTimeout",  /* keyname       */
                            (int)(TXTIMEOUT/1000));             /* default value */
}


checkconnection(msgflag)
short msgflag;
{
	short oldmaxport;

	oldmaxport = maxport;

	/* only show the msg when msgflag == true */
	if (msgflag && GetSpoolJob(SP_LISTEDPORTCNT, (long)0) > MAXPORT)
		SpoolerError(IDS_NAME, IDS_TOOMANYPORTS,MB_SYSTEMMODAL | MB_ICONEXCLAMATION | MB_OK, (LPSTR)0);

	if (((maxport = GetSpoolJob(SP_CONNECTEDPORTCNT, (long)0)) > 0) || oldmaxport)
		SetupPortTable();

	if (maxport > 0)
		return TRUE;

	/* always show the message when msgflag == true */
	if (oldmaxport != maxport || msgflag)
		{
		SpoolerError(IDS_NAME, maxport? IDS_MULTIPRINTER:IDS_NOPRINTER, MB_SYSTEMMODAL | MB_ICONEXCLAMATION | MB_OK, 0);
		curjob = 0;
		}

	return EOF;
}


SetupPortTable()
{
	char buf[BUFFERLENGTH];
	PSTR bufptr, bufptr2;
	short i, len;
	register SERVER *server;

	*(short *)buf = BUFFERLENGTH;

	GetSpoolJob(SP_PRINTERNAME, (long)(LPSTR)buf);
	bufptr = buf;
	for (i = 0; i < MAXPORT; i++)
		{
		bufptr2 = bufptr;
		while (*bufptr++);
		if (*bufptr)
			/* has printer connection */
			{
			if (!servers[i])
				if (!(servers[i] = (SERVER *)LocalAlloc(LPTR, sizeof(SERVER))))
					{
					HandleOutOfMemory();
					continue;
					}
			server = servers[i];
			lstrcpy(server->printername, bufptr);
			server->printerlength = lstrlen((LPSTR) bufptr);

			len = lstrlen((LPSTR) bufptr2);
			server->portlength = len;
			--len;
			lstrcpy(server->portname, bufptr2);
			server->portfn = SP_RETRY;
			server->valid = SP_VALID;
			}
		else
			if (servers[i])
				{
				if (servers[i]->jobcnt)
					servers[i]->valid = FALSE;
				else
					FreePort(i);
				}
		while (*bufptr++);
		}
}

FreePort(i)
{
	    LocalFree((HANDLE)servers[i]);
	    servers[i] = 0;
}


OkToClose(flag)
unsigned flag;
{
	register short i;
	HANDLE hJCB;
	char buf[MAX_PROFILE];
	char buf2[NAME_LEN];

	for (i = 0; i < MAXPORT; i++)
	    if (servers[i] && servers[i]->jobcnt)
		    {
		    LoadString(hInst, IDS_TERMALL, buf, MAX_PROFILE);
		    LoadString(hInst, IDS_NAME, buf2, NAME_LEN);
		    if (MessageBox(hWnd,(LPSTR) buf,(LPSTR) buf2,
			    flag | MB_ICONQUESTION | MB_OKCANCEL) != MB_OKCANCEL)
			    return FALSE;
		    break;
		    }
	for (i = 0; i < MAXPORT; i++)
	    {
	    if (!servers[i])				/* 092085wm */
	       continue;				/* 092085wm */
	    if (servers[i]->portfn > 0)
		    {
		    ResetPort(servers[i]->portfn);
		    ClosePort(servers[i]->portfn);
		    servers[i]->portfn = SP_RETRY;
		    }
	    if (servers[i]->valid & SP_JCBLOCKED)
		    GlobalUnlock(servers[i]->hJCB);
	    while(servers[i] && servers[i]->jobcnt)
		    {
		    hJCB = *servers[i]->queue;
		    AddSpoolJob(servers[i], hJCB, SP_DELETEJOB);
		    FreeAll(hJCB);
		    }
	    }
	UpdateScreen();
	return TRUE;
}
