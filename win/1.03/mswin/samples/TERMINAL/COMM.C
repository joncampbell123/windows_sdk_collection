#define CODEFIX

#define    NOGDICAPMASKS
#define    NOVIRTUALKEYCODES
#define    NOWINMESSAGES
#define    NOWINSTYLES
#define    NOICON
#define    NOKEYSTATE
#define    NOSYSCOMMANDS
#define    NORASTEROPS
#define    NOSHOWWINDOW
#define    NOATOM
#define    NOBRUSH
#define    NOCLIPBOARD
#define    NOCOLOR
#define    NOCREATESTRUCT
#define    NODRAWTEXT
#define    NOMEMMGR
#define    NOMETAFILE
#define    NOMINMAX
#define    NOPEN
#define    NOREGION
#define    NOSOUND
#define    NOWH
#define    NOWINOFFSETS
#define    NOWNDCLASS
#define    NODRAWTEXT
#define    NOMSG
#define    NOKANJI


#include "windows.h"
#include "winexp.h"
#include "extern.h"
#include "terminal.h"
#include "termrc.h"

/* Comm port I/O commands */
#define C_OPEN	    0
#define C_WRITE     1
#define C_READ	    2
#define C_CLOSE     3

#define TXTIMEOUT    15        /* The transmit timeout delay in seconds */

extern DWORD FAR PASCAL GetTimerResolution();
extern DWORD FAR PASCAL GetTickCount();


void CommPortError(hwnd, iCode, iError, iPort)
HWND hwnd;
int iCode;
int iError;
int iPort;
    {
    szCommPort[3] = iPort + '1';
    if (iCode==C_OPEN)
	switch(iError)
	    {
	    case IE_BAUDRATE:
		TermAlert(hwnd, IDS_MB14, term.fModem ? IDS_MODEM : IDS_COMPUTER);
		break;
	    case IE_MEMORY:
		TermAlert(hwnd, IDS_MB1, (int) szConnect);
		break;
	    case IE_OPEN:
		TermAlert(hwnd, IDS_MB19, (int) szCommPort);
		break;
	    default:
		TermAlert(hwnd, IDS_MB10, (int) szCommPort);
		break;
	    }
    }


CID OpenPort(pterm)
TERM *pterm;
    {
    CID cid;

    szCommPort[3] = pterm->iPort + '1';
    if ((cid = OpenComm((LPSTR)szCommPort, cbInBuf, cbOutBuf))>=0)
	SetPortState(cid, pterm);

    return(cid);
    }

void ClosePort(cid)
CID cid;
    {

    if (cid >= 0)
	CloseComm(cid);

    }

/* Name: SetPortState()
/*
/* Action: Set the communications parameters as specified
/*	   in the "term" state record.
/*
/* Input:  1. cid = Communications channel handle
/*	   2. term = Teminal state record.
/*
/* Output: None
/*
/**/
void PASCAL SetPortState(cid, pterm)
CID  cid;
TERM *pterm;
    {
    DCB   dcb;


    if (cid>=0)
	{
	if (GetCommState(cid, (DCB FAR *) &dcb)>=0)
	    {
	    dcb.BaudRate = pterm->iBaud;
	    dcb.ByteSize = pterm->iByteSize;
	    dcb.StopBits = pterm->iStopBits;
	    dcb.Parity = pterm->iParity;
	    dcb.XonLim = 30;
	    dcb.XoffLim = (cbInBuf/2) + 1;
	    dcb.fNull = TRUE;

	    switch(pterm->iHandshake)
		{
		case HSSOFTWARE:
		    dcb.XonChar = 0x11;
		    dcb.XoffChar = 0x13;
		    dcb.fOutX = TRUE;
		    dcb.fInX = TRUE;
		    dcb.fRtsflow = FALSE;
		    dcb.fDtrflow = FALSE;
		    break;
		case HSHARDWARE:
		    dcb.fRtsflow = TRUE;
		    dcb.fDtrflow = TRUE;
		    dcb.fOutX = FALSE;
		    dcb.fInX = FALSE;
		    break;
		case HSNONE:
		    dcb.fRtsflow = FALSE;
		    dcb.fDtrflow = FALSE;
		    dcb.fOutX = FALSE;
		    dcb.fInX = FALSE;
		    break;
		}
	    SetCommState((DCB far *)&dcb);
	    }
	}

    }


int FAR PASCAL DrawTermInput(hwnd, lpbSrc, cb)
HWND hwnd;
LPSTR lpbSrc;
int cb;
    {
    return (DoInput(hwnd, lpbSrc, cb));
    }


/**********************************************************
/* Name: PutComm()
/*
/* Action: Write N bytes to the comm port
/*
/* Returns:  The number of bytes written.  If a severe error
/*	     occurs, -1 will be returned indicating that
/*	     output to the comm port should be terminated.
/*
/************************************************************
/**/
int FAR PASCAL PutComm(hwnd, lpbSrc, cb)
HWND hwnd;		    /* The window handle */
LPSTR lpbSrc;		    /* A long ptr to the source bytes */
int   cb;		    /* The number of bytes to write */
    {
    int iErr;
    int cbWrite;
    COMSTAT cstat;


    cbWrite = 0;
    if (fConnect && (cid>=0))
	{
	iErr = 0;
	cbWrite = WriteComm(cid, lpbSrc, cb);
	if (cbWrite!=cb)
	    {
	    if (cbWrite<0)
		cbWrite = - cbWrite;
	    iErr = GetCommError(cid, (COMSTAT far *) &cstat);
	    if (iErr==CE_TXFULL)
		{
		TermInput(hwnd, cid);
		iErr = 0;
		}
	    else
		{
		szCommPort[3] = term.iPort + '1';
		TermAlert(hwnd, IDS_MB12, (int) szCommPort);
		}
	    }
	if (term.fLocalEcho && cbWrite>0)
	    {
	    TranslateInput(lpbSrc, cbWrite);

	    DrawTermInput(hwnd, lpbSrc, cbWrite);
	    }
	if (iErr)
	    cbWrite = -1;
	}
    return cbWrite;
    }


/***************************************************************
/* Name:  WaitTxReady()
/*
/* Wait until the transmit buffer is not full.
/*
/* Returns:  The number of bytes free in the transmit buffer.
/*	     -1 if there was a fatal error.
/*
/****************************************************************
/**/
int   FAR   PASCAL WaitTxReady()
    {
    DWORD  lTimerRes;
    DWORD  lTimerTicks;
    DWORD  lWait;
    int    cbFree;
    COMSTAT cstat;


    cbFree = 0;
    if (fConnect && cid>=0)
	{
	lTimerRes = GetTimerResolution();
	lTimerTicks = GetTickCount();
	lWait = myldiv(mylmul((unsigned long)TXTIMEOUT, 1000000L), lTimerRes);
	while (lWait > (GetTickCount()-lTimerTicks))
	    {
	    TermInput(hWndTM, cid);
	    GetCommError(cid, (COMSTAT far *) &cstat);
	    cbFree = cbOutBuf - cstat.cbOutQue;
	    if (cbFree>0)
		goto DONE;
	    else
		cbFree = 0;
	    }
	szCommPort[3] = term.iPort + '1';
	TermAlert(hWndTM, IDS_MB12, (int) szCommPort);
	}
DONE:
    return cbFree;
    }


void PASCAL DoCmdBreak()
    {
    if (fConnect && cid>=0)
	{
	SetCommBreak(cid);
	Pause(750);
	ClearCommBreak(cid);
	}
    }


BOOL PASCAL DoCmdConnect(hwnd, fConnect)
HWND hwnd;
BOOL  fConnect; 	    /* True if a connect is desired */
    {
    BOOL fCanceled;
    int  iStatus;
    HDC  hdc;
    RECT rtClient;
    HMENU hmenu;


    fCanceled = FALSE;

    hmenu = GetMenu(hwnd);
    if (fConnect)
	{

	/* Put up a dialog for modem use without a phone number */
	if (term.fModem && (term.szPhNum[0]=='\0'))
	    fCanceled = !((BOOL) DoDialog(idlgPhNum, ifnPhNum));

	if (!fCanceled)
	    if ((cid = OpenPort(&term))<0)
		{
		CommPortError(hwnd, C_OPEN, cid, term.iPort);
		fCanceled = TRUE;
		}
	    else
		if (term.fModem && (term.szPhNum[0]!='\0'))
		    {
		    while ((iStatus = DoDialog(idlgAnswer, ifnAnswer))==MS_NOREPLY)
			if (TermAlert(hwnd, IDS_MB7, 0) == IDCANCEL)  /* Redial? */
			    break;

		    switch(iStatus)
			{
			case MS_OK:
			case MS_CONNECT:
			    break;
			default:
			    TermAlert(hwnd, IDS_MB8, 0);  /* Can't find modem */
			    /* Fall through */
			case MS_NOREPLY:
			case MS_CANCELED:
			    ClosePort(cid);
			    fCanceled = TRUE;
			    break;
			}
		    }
	}
    else
	{
	if ((BOOL)DoDialog(idlgDisCon, ifnDisCon))
	    {
	    if (term.fModem && (term.szPhNum[0]!='\0'))
		DisConModem(&term);	     /* Disconnect the modem */
	    ClosePort(cid);
	    }
	else
	    fCanceled = TRUE;
	}

    if (!fCanceled)
	{
	EnableMenuItem(hmenu, miNew, fConnect);
	EnableMenuItem(hmenu, miOpen, fConnect);
	EnableEdit(hWndTM, !fConnect || fPause);

	if (fConnect)
	    {
	    GetClientRect(hWndTM, (LPRECT) &rtClient);
	    hdc = GetDC(hWndTM);
	    PaintScreen(hWndTM);
	    ReleaseDC(hWndTM, hdc);
	    }
	CheckMenuItem(hmenu, miConnect, fConnect ? MF_CHECKED : MF_UNCHECKED);
	}
    return(!fCanceled);
    }


BOOL PASCAL WaitReply(szSuccess, iWait)
char *szSuccess;
int   iWait;
    {
    char *pDst;
    int  cb;
    int  i;
    char bCh;
    char rgbReply[80];


    pDst = rgbReply;
    cb = 0;
    while (TRUE)
	{
	i = ReadComm(cid, (LPSTR) &bCh, 1);
	if (i<0)
	    return FALSE;
	else if (i==1 && bCh==0x0a)
	    break;
	}
    while(cb<sizeof(rgbReply))
	{
	i = ReadComm(cid, (LPSTR) pDst, 1);
	if (i<0)			    /* Exit on error */
	    return FALSE;
	if (i>0)
	    if (*pDst==0x0a)		    /* Check for end of line */
		{
		*pDst = 0;
		break;
		}
	    else if (*pDst!=0x0d)
		++pDst;
	}
    return(!lstrcmp((LPSTR)rgbReply, (LPSTR)szSuccess));
    }

void FAR PASCAL TermInput(hwnd, cid)
HWND hwnd;
CID cid;
    {
    int cb;
    COMSTAT cstat;

    if (cbInputBuf<=0)
	{
	iInputBuf = 0;
	if ((cbInputBuf = ReadComm(cid, (LPSTR)rgbInputBuf, sizeof(rgbInputBuf)-1))<=0)
	    {
	    if (GetCommError(cid, (COMSTAT FAR *) NULL)!=0)
		cbInputBuf = - cbInputBuf;
	    }
	}


    if (cbInputBuf>0)
	{
	cb = DrawTermInput(hwnd, (LPSTR) rgbInputBuf + iInputBuf, cbInputBuf);
	iInputBuf += cb;
	cbInputBuf -= cb;
	}
    }
