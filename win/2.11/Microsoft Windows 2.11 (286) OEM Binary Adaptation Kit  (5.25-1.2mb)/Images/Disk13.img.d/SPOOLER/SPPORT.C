#include "nodefs.h"
#include "windows.h"
#include "winexp.h"
#include "spool.h"
#include "spoolids.h"
#define FILE_PORT  1

/* Routines:
	int WritePort(port, string, cch)
	porthandle OpenPort(string)
 */

extern HWND hWnd;
extern HANDLE hInst;
extern unsigned short CommError;

extern long dnsTimeout; /* timeout values from win.ini */
extern long txTimeout;

/*	WritePort -  Send charcters to given port.  Returns the number	    */
/*	of bytes written.  If the job should be canceled (the dialog	    */
/*	box posted because of an error, the most significant bit of	    */
/*	return value will be set, the lower order bits will contain	    */
/*	the count of characters written.				    */
/*									    */
/*	If an error occurs during the write, the action taken will be	    */
/*	based on the following: 					    */
/*									    */
/*	    Error	Action						    */
/*									    */
/*	    CE_RXOVER	Flush input queue, no effect on retry count	    */
/*	    CE_OVERRUN	Flush input queue, no effect on retry count	    */
/*	    CE_RXPARITY Flush input queue, no effect on retry count	    */
/*	    CE_FRAME	Flush input queue, no effect on retry count	    */
/*									    */
/*	    CE_BREAK	Fatal, return -1 immediately			    */
/*									    */
/*	    CE_CTSTO	ignore, should not occur			    */
/*	    CE_DSRTO	ignore, should not occur			    */
/*	    CE_RLSDTO	ignore, should not occur			    */
/*									    */
/*	    CE_PTO	Fatal, return -1 immediately			    */
/*	    CE_IOE	Fatal, return -1 immediately			    */
/*	    CE_OOP	Fatal, return -1 immediately			    */
/*									    */
/*	    CE_DNS	Non-fatal, retry on minimum count		    */
/*									    */
/*	    CE_TXFULL	If no other errors, no effect on retry count	    */
/*			If fCTSHold | fDSRHold | fXOffHold, non-fatal,	    */
/*			retry on maximum count. 			    */


int far PASCAL WritePort(port, lpsz, cch)
PORT *port;
LPSTR lpsz;
unsigned cch;
{
    int 	retval = 0;
    COMSTAT	comstat;
    long	CurTime;
    /*	abs(retval) is the number of bytes written  */

#if FILE_PORT
    if (port->type & SP_FILE_PORT){
	if ((retval = _lwrite(port->fn, lpsz, cch)) != cch){
	    CurTime=GetCurrentTime();
	    if (port->retry){
                if ((CurTime - port->retry) >= txTimeout){
		    retval |= COMM_ERR_BIT;
		    port->retry = 0;
		}
	    }else
		port->retry = CurTime;

	}else {
	    port->retry = 0;
	}
	return retval;
    }
#endif

    if ((retval = WriteComm(port->fn, lpsz, cch)) <= 0)
    {
	/* reset timer when any character is sent */
	if (retval = - retval)
	    port->retry = 0;

	CommError = GetCommError(port->fn, (COMSTAT far *)&comstat);

	if (CommError & (CE_RXOVER | CE_OVERRUN | CE_RXPARITY | CE_FRAME))
	    FlushComm(port->fn, 1);

	if (CommError & (CE_BREAK | CE_PTO | CE_IOE | CE_OOP))
	    retval |= COMM_ERR_BIT;

	if (CommError & CE_DNS)
	{
	    CurTime=GetCurrentTime();
	    if (port->retry)
	    {
                if ((CurTime - port->retry) >= dnsTimeout)
		{
		    retval |= COMM_ERR_BIT;
		    port->retry = 0;
		}
	    }
	    else
		port->retry = CurTime;
	}

#ifdef DISABLE
        /* removed checking serial port flags in comstat, since this must
         * work for parallel ports also!. 23-Apr-1987. davidhab.
         */
	if ((CommError & CE_TXFULL) && (comstat.fCtsHold | comstat.fDsrHold | comstat.fXoffHold))
#else
        if (CommError & CE_TXFULL)
#endif
	{
	    CurTime=GetCurrentTime();
	    if (port->retry)
	    {
                if ((CurTime - port->retry) >= txTimeout)
		{
		    retval |= COMM_ERR_BIT;
		    port->retry = 0;
		}
	    }
	    else
		port->retry = CurTime;
	}
    }
    else
	port->retry = 0;

    return (retval);
}


/* gets passed a port name [string], returns a number for use with int 17 */

int far PASCAL OpenPort(lpsz)
LPSTR lpsz;
{
	register int retval, errid;
	DCB dcb;
	PORT *port;

	while ((char)*lpsz == ' ') lpsz++;

	retval = OpenComm(lpsz, COMM_INQUE, COMM_OUTQUE);
	switch(retval)
	{
	case IE_OPEN:  /* device already open */
		errid = COMM_OPEN;
		break;

	case IE_HARDWARE:
		errid = COMM_HARDWARE;
		break;

	case IE_BADID:
#if FILE_PORT
		if ((retval = _lcreat(lpsz, 0)) >= 0)
			{
			if (!(port = (PORT *)LocalAlloc(LPTR, sizeof(PORT))))
				{
				HandleOutOfMemory();
				return SP_CANCEL;
				}
			SetPortRaw(retval);
			port->type = SP_FILE_PORT;
			port->fn = retval;
			port->retry = 0;
			return (short) port;
			}
#endif
		errid = COMM_INVALIDPORT;
		break;

	case IE_MEMORY:
		HandleOutOfMemory();
		return SP_CANCEL;

	default:
		if (retval < 0)
			{
			errid = IDS_COMMERROR;
			break;
			}

		GetCommState(retval, (DCB far *)&dcb);
		if (SetDCB(lpsz, (DCB far *)&dcb))
			if (SetCommState((DCB far *)&dcb))
				{
				/* comm port setting not successful */
				errid = COMM_SETTING;
				goto openfail;
				}
		if (!(port = (PORT *)LocalAlloc(LPTR, sizeof(PORT))))
			{
			HandleOutOfMemory();
			return SP_CANCEL;
			}
		port->type = SP_COMM_PORT;
		port->fn = retval;
		port->retry = 0;
		return (short) port;
	}

openfail:
        SpoolerError(IDS_NAME, errid, MB_SYSTEMMODAL | MB_ICONEXCLAMATION, lpsz);
	return SP_CANCEL;
}

far PASCAL ClosePort(port)
PORT * port;
{
	if ((HANDLE)port == SP_CANCEL || (HANDLE)port == SP_RETRY || !port)
		return;
#if FILE_PORT
	if (port->type & SP_FILE_PORT)
		_lclose(port->fn);
	else
#endif
		CloseComm(port->fn);
	LocalFree((HANDLE)port);
}


far ResetPort(port)
PORT * port;
{
#if FILE_PORT
	if (!(port->type & SP_FILE_PORT))
#endif
		{
		FlushComm(port->fn,0);			/* wm091385 */
		EscapeCommFunction(port->fn, 7);
		}
}



SetDCB(portname,lpDCB)
LPSTR portname;
DCB far *lpDCB;
{
	char buf[IDS_LENGTH];
	char buf2[10];
	LPSTR bufptr;
	short field = 0, i = 0;

	lstrcpy((LPSTR)buf2, (LPSTR)portname);
	while (buf2[i++]);
	buf2[i] = '\0';
	buf2[--i] = ':';
	if (!GetProfileString( (LPSTR)"ports",	       /* "app" name */
				    (LPSTR)buf2,
				    (LPSTR)"",	  /* default empty string */
				    (LPSTR)buf,   /* result string */
				    IDS_LENGTH	  /* max chars to plcae in buffer */
				    ))
		return 0;


	for (bufptr = buf, field = 1; *bufptr; bufptr++)
		if (*bufptr == ',')
			{
			*bufptr = '\0';
			field++;
			}

	lpDCB->XonChar = 0x11;	    /* control Q */
	lpDCB->XoffChar = 0x13;     /* control S */
	lpDCB->fOutX = 1;
	bufptr = buf;
	for (i = 0; i < field; i++)
		{
		register short tmp;

		if (*bufptr)
			switch (i)
			{
			/* get baud rate */
			case BAUDRATE:
				if (tmp = atoi(bufptr))
					lpDCB->BaudRate = tmp;
				break;
			case PARITY:
				lpDCB->Parity = (*bufptr == 'n'||*bufptr == 'N')? NOPARITY:
						(*bufptr == 'o'||*bufptr == 'O')? ODDPARITY:
						EVENPARITY;
				break;
			case BYTESIZE:
				if (tmp = atoi(bufptr))
					lpDCB->ByteSize = tmp;
				break;
			case STOPBITS:
				lpDCB->StopBits = (*bufptr == '1')? ONESTOPBIT: TWOSTOPBITS;
				break;
			case REPEAT:
				lpDCB->CtsTimeout = IGNORE;/*actually these are default*/
				lpDCB->DsrTimeout = IGNORE;/*from the Opencomm call*/
				lpDCB->RlsTimeout = IGNORE;
				lpDCB->fOutxCtsFlow = 1;   /*pay attention to cts line */
				lpDCB->fOutxDsrFlow = 1;   /*pay attention to dsr line */
				break;
			}
		while (*bufptr++);
		}
	return field;
}

atoi(lpnum)
LPSTR lpnum;
{
	short n = 0;

	while (*lpnum)
	    {
	    n *= 10;
	    n += *lpnum - '0';
	    lpnum++;
	    }
	return n;
}


SpoolerError(caption_id, msg_id, flag, name)
short caption_id, msg_id;
short flag;
LPSTR name;
{
        HWND    hwndParent;


	char	buf1[IDS_LENGTH],
		buf2[MAX_PROFILE],
		buf3[MAX_PROFILE];

        hwndParent = hWnd;
        if (IsIconic(hWnd) && hWnd != GetActiveWindow())
            hwndParent = NULL;
	LoadString(hInst, caption_id, (LPSTR)buf1, IDS_LENGTH);
	LoadString(hInst, msg_id, (LPSTR)buf2, IDS_LENGTH << 1);

	MergeStrings((LPSTR)buf2, name, (LPSTR)buf3);

        return MessageBox((HWND)hwndParent, (LPSTR)buf3, (LPSTR)buf1, flag);
}
