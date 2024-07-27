/*
;***************************************************************************
;                                                                          * 
; Copyright (C) 1983,1984 by Microsoft Inc.  				   *
; 									   *
;***************************************************************************
*/ 
#include <generic.h> 

void FAR PASCAL FillBuffer(LPSTR, WORD, WORD);

FAR PASCAL Control(lpDevice, function, lpInData, lpOutData)
LPDEVICE lpDevice;
short   function;
LPSTR   lpInData;
LPPOINT lpOutData;
{
        short y;
        unsigned i;
        register short n;

        switch (function)
        {
        case NEXTBAND:
                if (lpDevice->epDoc != TRUE)
                        goto SpoolFail;

                if (!(n = lpDevice->epNband++))
                        {
                        if ((lpDevice->epDoc = StartSpoolPage(lpDevice->epJob)) != TRUE)
                                goto SpoolFail;

#ifdef EP_ENABLE
                        myWrite(lpDevice, (LPSTR) EP_ENABLE);
#endif
#if defined(EP_CHAR2)
                        myWrite(lpDevice, (LPSTR) EP_CHAR2);
#endif

#ifdef CITOH
                        if (lpDevice->epMode & HIGHSPEED)
                            myWrite(lpDevice, (LPSTR) EP_BIDIRECTIONAL);
#endif		

/* center the printable area to the length of the paper */
                        lpDevice->epCury = 0;
                        YMoveTo(lpDevice, lpDevice->epPF->VOffset);

                        lpDevice->epYcurwidth =
        		    lpDevice->epXCursPos =
                                lpDevice->epCurx = lpDevice->epCury = 0;

                        /* set logical page origin */
                        myWrite(lpDevice, ESCEXP(escapecode.pica_on));
                        lpDevice->epXcurwidth = PICA_WIDTH;
                        myWriteSpool(lpDevice);
                        }

                if (lpDevice->epMode & DRAFTFLAG)
                    {
                    if (!n)
                            {
                            SetRect((LPRECT) lpOutData, 0, 0, lpDevice->epPageWidth, PG_DOWN);
                            return TRUE;
                            }
                    else
                            {
                            if (lpDevice->epXcurwidth != PICA_WIDTH)
                                    {
                                    /* reset the printer back to pica */
                                    myWrite(lpDevice, ESCEXP(escapecode.pica_on));
                                    lpDevice->epXcurwidth = PICA_WIDTH;
                                    }
                            n = MAXBAND;
                            }
                    }
                else
                        lpDevice->epPtr = 0;

                if (lpDevice->epType == DEV_LAND)
                    {
#if defined(OKI92)
                    y = lpDevice->epXOffset = (MAXBAND - n - 1) * BAND_HEIGHT;
#else
                    y = lpDevice->epXOffset = n * BAND_HEIGHT;
#endif
                    SetRect((LPRECT) lpOutData, y, 0, y + BAND_HEIGHT, lpDevice->epPageWidth);
                    }
                else
                    {
                    y = lpDevice->epYOffset = n * BAND_HEIGHT;
                    SetRect((LPRECT) lpOutData, 0, y, lpDevice->epPageWidth, y + BAND_HEIGHT);
                    }

                switch (n)
                {
                case MAXBAND:
                        SetRectEmpty((LPRECT) lpOutData);
                        if (!(lpDevice->epMode & DRAFTFLAG))
                                if (lpDevice->epMode & GRXFLAG) {
                                	dump(lpDevice);
				} else if (lpDevice->epMode & TEXTFLAG) {
                                        ch_line_out(lpDevice, y);
                                }
                        lpDevice->epNband = 0;

#ifdef CITOH	/* loses form feed */
                        YMoveTo(lpDevice, lpDevice->epPF->FormLength - lpDevice->epPF->VOffset);
#else		/* !CITOH */

    #if defined(SPECIALDEVICECNT)
                        if (lpDevice->epMode & LOSES_FF || lpDevice->epPF->code != LETTER)
                                YMoveTo(lpDevice, lpDevice->epPF->FormLength - lpDevice->epPF->VOffset);
                        else
                                myWrite(lpDevice, (LPSTR)EP_FF);
    #else
                        if (lpDevice->epPF->code != LETTER)
                                YMoveTo(lpDevice, lpDevice->epPF->FormLength - lpDevice->epPF->VOffset);
                        else
                                myWrite(lpDevice, (LPSTR)EP_FF);
    #endif
#endif   /* !CITOH */

#ifdef EP_NORMAL
                        myWrite(lpDevice, (LPSTR)EP_NORMAL);
#endif
                        myWriteSpool(lpDevice);
                        if (lpDevice->epDoc == TRUE)
                                lpDevice->epDoc = EndSpoolPage(lpDevice->epJob);
                        break;

                default:
                        if (lpDevice->epMode & GRXFLAG)
                                {
                                dump(lpDevice);
                                /* fall through */

                case 0:
				/* clear the buffer to 0xffff's */
				FillBuffer((LPSTR)lpDevice->epBmp, (WORD)0xFFFF,
#if COLOR
					   (WORD)BAND_SIZE(lpDevice->epPageWidth)
				/ 2 / NPLANES * lpDevice->epBmpHdr.bmPlanes
#else
					   (WORD)BAND_SIZE(lpDevice->epPageWidth)
						   / 2
#endif
					   );
                                }
                        else
                                if (lpDevice->epMode & TEXTFLAG)
                                        {
                                        ch_line_out(lpDevice, y);
                                        }
                                else
                                        break;
                        lpDevice->epMode &= ~(GRXFLAG | TEXTFLAG);
                        myWriteSpool(lpDevice);
                }       /* switch */
                lpDevice->epHPptr = 0;

                if (lpDevice->epDoc != TRUE)
SpoolFail:
                        {
                        SetRectEmpty((LPRECT) lpOutData);
                        if (lpDevice->epJob)
                                {
                                DeleteJob(lpDevice->epJob, 0);
                                lpDevice->epJob = 0;
                                }
                        }
                /* fall through and return the status */

        case NEWFRAME:
                return lpDevice->epDoc;

        case SETABORTPROC:
                lpDevice->ephDC = *(HANDLE far *)lpInData;
                break;

        case DRAFTMODE:
                /* draft mode is not supported in landscape */
                if (lpDevice->epType == DEV_LAND)
                        return !(*(short far *)lpInData);

  		if (*(short far *)lpInData)
                  	lpDevice->epMode |= DRAFTFLAG;
                  else
  			lpDevice->epMode &= ~DRAFTFLAG;
                break;

        case STARTDOC:
                /* error in lpDevice->epDoc means this device does not have
                   an open channel */
		
                if (lpDevice->epDoc != TRUE)
                     if ((lpDevice->epJob = OpenJob((LPSTR)lpDevice->epPort, 
			                    lpInData, lpDevice->ephDC)) > 0)
                         lpDevice->epDoc = TRUE;
		if (lpDevice->epJob)
			return(TRUE);	/* job is started */
		else
			return(-1);
		break;
		

        case ABORTDOC:
                if (lpDevice->epDoc == TRUE)
		{
                        DeleteJob(lpDevice->epJob, 0);			
		}
                lpDevice->epDoc = 0;
                break;
		
        case ENDDOC:
/* close the job */		
		if (lpDevice->epDoc == TRUE)
		{
                    CloseJob(lpDevice->epJob);
		}

                lpDevice->epDoc = 0;
                return TRUE;

        case QUERYESCSUPPORT:
                i = * (short far *) lpInData;
                switch(i)
                {
                    case DRAFTMODE:
                            return (lpDevice->epType == DEV_PORT);
                    case NEWFRAME:
        	    case SETABORTPROC:
                    case ABORTDOC:
                    case NEXTBAND:
                    case QUERYESCSUPPORT:
                    case STARTDOC:
                    case ENDDOC:
                    case GETPHYSPAGESIZE:
                    case GETPRINTINGOFFSET:
#if 0
                    case GETEXTENTTABLE:
                    case EXTTEXTOUT:
#endif
                            return TRUE;
                    default:
                            return FALSE;
                }

        case GETPHYSPAGESIZE:
                lpOutData->xcoord = lpDevice->epPF->XPhys;
                lpOutData->ycoord = lpDevice->epPF->YPhys;
                break;

        case GETPRINTINGOFFSET:
                lpOutData->xcoord = lpDevice->epPF->XPrintingOffset;
                lpOutData->ycoord = lpDevice->epPF->YPrintingOffset;
                break;


#if 0
        case GETEXTENTTABLE:
            {
                LPEXTWIDTHDATA  lpExtData;
                LPSTR           lpt;
                unsigned        chFirst;
                unsigned        chLast;

                lpExtData = (LPEXTWIDTHDATA)lpInData;

                if (lpExtData->lpFont->dfDevice) {
                	lpt   = lpExtData->lpInData;
                	chFirst = *lpt++;
                	chLast  = *lpt;
                  	return ExtWidths(lpDevice, chFirst, chLast,
                       	    (short far *)lpOutData, lpExtData->lpFont, lpExtData->lpXForm);
		}

                if (lpDevice->epType)
                    lpDevice = (LPDEVICE) &lpDevice->epBmpHdr;

                /*
                return dmControl(lpDevice, function, lpInData, lpOutData);
                */
            }
            break;


        case EXTTEXTOUT:
            {
                LPEXTTEXTDATA       lpExtData;
                LPAPPEXTTEXTDATA    lpAppExtData;
                int                 xoffset;
                int                 yoffset;
                int                 status;


                lpExtData = (LPEXTTEXTDATA)lpInData;
                lpAppExtData =  lpExtData->lpInData;

                if (lpExtData->lpFont->dfDevice ||
                    lpDevice->epType && (lpDevice->epMode & DRAFTFLAG))
                        return ExtStrOut(lpDevice, lpAppExtData->x,
			    lpAppExtData->y, (LPRECT) &(lpAppExtData->ClipRect),
                            lpAppExtData->lpStr, lpAppExtData->count, 
			    lpExtData->lpFont, lpExtData->lpXForm, 
			    lpExtData->lpDrawMode,lpAppExtData->lpWidths);


                if (lpDevice->epType) {

                    lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;
                    lpAppExtData->x -= (xoffset = lpDevice->epYOffset);
                    lpAppExtData->y -= (yoffset = lpDevice->epXOffset);
                    lpDevice->epMode |= GRXFLAG;
                    lpDevice = (LPDEVICE) &lpDevice->epBmpHdr;

                }
                /*
                status = dmControl(lpDevice, function, lpInData, lpOutData);
                */
                if (lpDevice->epType) {
                    lpAppExtData->x += xoffset;
                    lpAppExtData->y += yoffset;
                }

		return status;
            }
            break;
#endif

        default:
                return FALSE;
        }
        return TRUE;
}



short NEAR PASCAL myWriteSpool(lpDevice)
register LPDEVICE lpDevice;
{
        short n;

        if (!lpDevice->epPtr || lpDevice->epDoc != TRUE)
                return 0;

        if ((n = WriteSpool(lpDevice->epJob, (LPSTR) lpDevice->epSpool, lpDevice->epPtr))!= lpDevice->epPtr)
                lpDevice->epDoc = n;
        lpDevice->epPtr = 0;
        return  n;
}
