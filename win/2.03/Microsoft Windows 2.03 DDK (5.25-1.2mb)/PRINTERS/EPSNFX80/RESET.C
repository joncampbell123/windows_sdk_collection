#include "generic.h"
#include "resource.h"

void NEAR PASCAL SetDeviceMode(LPDEVICE, DEVMODE far *);
void FAR PASCAL GetDeviceMode(LPSTR, DEVMODE far *);
void FAR PASCAL SaveDeviceMode(LPSTR, DEVMODE far *);
short NEAR PASCAL atoi(LPSTR);
BOOL FAR PASCAL isUSA();
short NEAR BuffInit(LPDEVICE);
short NEAR BuffFree(LPDEVICE);

extern  DEVICEHDR port_device;
extern  DEVICEHDR land_device;

#if DEVMODE_WIDEPAPER			     
extern  DEVICEHDR wide_port_device;
extern  DEVICEHDR wide_land_device;
#endif

extern  char land_infobase[];
extern  char port_infobase[];

#if DEVMODE_WIDEPAPER			     
extern  char wide_land_infobase[];
extern  char wide_port_infobase[];
#endif		

short NEAR BuffInit(lpDevice)
LPDEVICE lpDevice;
{    
/* Assume we know band widths. Allocate buffers for spooler and bitmaps: */
	
	if (lpDevice->epHBmp = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
		(long) BAND_SIZE(lpDevice->epPageWidth)))
	{
	    if (lpDevice->epHBuf = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
	       (long) BUF_SIZE(lpDevice->epPageWidth)))
            { 
	        if (lpDevice->epHSpool = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
		   (long) SPOOL_SIZE(lpDevice->epPageWidth)))
          	{
			/* all allocations worked */
		        goto no_deAllocations;
		}
		else
			goto dealloc1; /* get rid of one allocation */
	    }
	    else
		goto dealloc2;  /* get rid of two allocations */
        }		
dealloc2:
	GlobalFree(lpDevice->epHBuf);
dealloc1:			
	GlobalFree(lpDevice->epHBmp);

/* nullify handles and pointers; 
   return error to indicate insufficient memory */	
	
	lpDevice->epHSpool = lpDevice->epHBuf = lpDevice->epHBmp = 0; 
	lpDevice->epBmp = lpDevice->epBuf = lpDevice->epSpool =
		(LPSTR) NULL;
		
	lpDevice->epBuffSet = FALSE;	
	
	return(FALSE);		/* couldn't allocate the memory */
/*      ------------------------------------------------------- */
	
no_deAllocations:

/* lock down the memory for the bands --> returns long pointers to memory */
/* No way this could fail; Kernel returns [<Handle>:0000] as pointer and
   sets lock count */

	lpDevice->epBmp = (LPSTR) GlobalLock(lpDevice->epHBmp);
	lpDevice->epBuf = (LPSTR) GlobalLock(lpDevice->epHBuf);
	lpDevice->epSpool = (LPSTR) GlobalLock(lpDevice->epHSpool);
	
/* buffer memory is all set up. */
        lpDevice->epBuffSet = TRUE;
	return(TRUE);
}


short NEAR BuffFree(lpDevice)
LPDEVICE lpDevice;
{
/* 
 *  Now unlock & free bitmap & buffer memory.
 */

    if (lpDevice->epBuffSet == TRUE)
    {
	    GlobalUnlock(lpDevice->epHBmp);
	    GlobalUnlock(lpDevice->epHBuf);		
   	    GlobalUnlock(lpDevice->epHSpool);		
		
	    GlobalFree(lpDevice->epHBmp);
	    GlobalFree(lpDevice->epHBuf);	
 	    GlobalFree(lpDevice->epHSpool);	
	    lpDevice->epHSpool = lpDevice->epHBuf = lpDevice->epHBmp = 0; 
	    lpDevice->epBmp = lpDevice->epBuf = lpDevice->epSpool =
		    (LPSTR) NULL;
	    
	    lpDevice->epBuffSet = FALSE;
    }
}

short FAR PASCAL Enable(lpDevice, style, lpDeviceType, lpOutputFile, lpStuff)
LPDEVICE    lpDevice;
short       style       ;
LPSTR       lpDeviceType;
LPSTR       lpOutputFile;
DEVMODE far *lpStuff     ;
{
	DEVMODE DevMode;

/* make sure we have the right devmode data */
	
        if (!lpStuff
            || lstrcmp((LPSTR) lpDeviceType, (LPSTR)lpStuff->DeviceName))
                /* wrong lpStuff */
		GetDeviceMode((LPSTR)lpDeviceType, (DEVMODE FAR *)&DevMode);
	else
                Copy((LPSTR)&DevMode, (LPSTR)lpStuff, sizeof(DEVMODE));
	
/* Now DevMode contains the devmode structure; do not use lpStuff */
	
/* "if style is 1 fill GDIINFO data structure with information about the 
   support module and its peripheral graphics device." */
	
	if (style & InquireInfo)
        {		
		
/* select paperformat data: (wide landscape, wide portrait, plain landscape,
			     plain portrait) */
#if DEVMODE_WIDEPAPER			     
            if (DevMode.use_wide_carriage)
                Copy((LPSTR)lpDevice,
                     (DevMode.orient == LANDSCAPE ? (LPSTR)wide_land_infobase:
		     (LPSTR)wide_port_infobase), sizeof(GDIINFO));	    
	    else
#endif		
                Copy((LPSTR)lpDevice,
                     (DevMode.orient == LANDSCAPE ? (LPSTR)land_infobase: 
		     (LPSTR)port_infobase), sizeof(GDIINFO));

/* Tell GDI how big the lpDevice needs to be reallocated as */
	     
#if 0
                ((GDIINFO far *)lpDevice)->dpDEVICEsize =
                     style & InfoContext? sizeof(DEVICEHDR):
		     		sizeof(DEVICE);  
#else			
/* - ***** ****** new code 10/21/87 dynamically allocates the printing buffers; they
     are not all contiguous.  (For simplicity I allocate the slightly larger 
     size (DEVICE instead of DEVICEHDR, to use page width field etc... )
 */
                ((GDIINFO far *)lpDevice)->dpDEVICEsize = sizeof(DEVICE);
#endif	
                return sizeof(GDIINFO);
        }

	/* implicit initializations necessary for IC or DC */
	
/* We can assume lpDevice points to an uninitialized DEVICE structure now
   (I hope) [if this assumption is wrong then 3 extra blocks may be 
   left in memory] */
	
	lpDevice->epHSpool = lpDevice->epHBuf = lpDevice->epHBmp = 0; 
	lpDevice->epBmp = lpDevice->epBuf = lpDevice->epSpool =
		    (LPSTR) NULL;
	    
	lpDevice->epBuffSet = FALSE;  /* have not set up buffers */
	
#if DEVMODE_WIDEPAPER		     
	if (DevMode.use_wide_carriage)
		lpDevice->epPageWidth = WIDE_PG_ACROSS;
	else
#endif	
            lpDevice->epPageWidth = PG_ACROSS;
	
	
#if DEVMODE_WIDEPAPER			     
        if (DevMode.use_wide_carriage)
            Copy((LPSTR)lpDevice, (DevMode.orient == LANDSCAPE ? 
		    (LPSTR)&wide_land_device : (LPSTR)&wide_port_device),
		    sizeof(DEVICEHDR));		
        else
#endif
            Copy((LPSTR)lpDevice, (DevMode.orient == LANDSCAPE ? 
		    (LPSTR)&land_device : (LPSTR)&port_device), 
		    sizeof(DEVICEHDR));

        SetDeviceMode(lpDevice, (DEVMODE FAR *)&DevMode);
        lpDevice->epJob = ERROR;
        lpDevice->epXcurwidth = CHARWIDTH;
	
#if defined(SPECIALDEVICECNT)
        FindDeviceMode(lpDevice, lpDeviceType);
#endif

/* "if style is 0, initialize support module and graphics peripheral for
   use by GDI routines" (do necessary heap allocations) */
			
        if (!(style & InfoContext))   /* Initialization for a DC */
        {
                lstrcpy(lpDevice->epPort, lpOutputFile);
		
		if (BuffInit(lpDevice) == FALSE)
			return(FALSE); /* couldn't allocate buffer memory */
		
                if (lpDevice->epType == DEV_PORT)
                {
                        if (heapinit(lpDevice))
			{
                                if (lpDevice->epYPQ = CreatePQ(Y_INIT_ENT))
/* everything is cool, this DC will be able to realize any hardware fonts */
                                        return TRUE;
                                else
                                        GlobalFree(lpDevice->epHeap);
			}
/* it has failed either because heapinit failed or can't set priority queue */
			
			BuffFree(lpDevice);  /* get rid of buffers */	
                        return FALSE;
                }
        }
        else
                lpDevice->epMode |= INFO;
        return TRUE;
}

/***** Windows 1.03 and before may NOT pass a valid lpDevice here *****/
FAR PASCAL Disable(lpDevice)
LPDEVICE lpDevice;
{
	if (lpDevice->epBuffSet == TRUE)
	    BuffFree(lpDevice);	 /* discard print buffers (if they exist) */
    
        if (lpDevice->epType == DEV_PORT)
        {
            DeletePQ(lpDevice->epYPQ);
            GlobalFree(lpDevice->epHeap);
        }
        return TRUE;
}

void NEAR PASCAL SetDeviceMode(lpDevice, lpStuff)
LPDEVICE lpDevice;
DEVMODE far *lpStuff;
{
        /* if you had any device modes set it up here --
            store the information in lpDevice */
        /* high quality means low speed and low quality will get you
           high speed */
        if (lpStuff->res == LOW)
                lpDevice->epMode |= HIGHSPEED;

        lpDevice->epPF = &PaperFormat[lpStuff->paper - STANDARDPF +
#if DEVMODE_WIDEPAPER		     
                (lpStuff->use_wide_carriage ? NSMALLPAPERFORMATS: 0) +
#endif
                (lpStuff->orient == LANDSCAPE ? MAXPAPERFORMAT: 0)];


#if COLOR
        if (lpStuff->color == BLACK_COLOR)
        	lpDevice->epBmpHdr.bmPlanes = 1;
#endif
}

#if defined(SPECIALDEVICECNT)
void NEAR PASCAL FindDeviceMode(lpDevice, lpDeviceType)
LPDEVICE lpDevice;
LPSTR lpDeviceType;
{
        register i;

        for (i = 0;i < SPECIALDEVICECNT; i++)
                if (!lstrcmp((LPSTR)lpDeviceType, DeviceSpec[i].DeviceName))
                        {
                        lpDevice->epMode |= DeviceSpec[i].flag;
                        break;
                        }
}
#endif

void FAR PASCAL GetDeviceMode(lpDeviceType, lpDevMode)
LPSTR        lpDeviceType;
DEVMODE far *lpDevMode;
{
        char lpOrientation[16], lpPrintQuality[16], lpPaperFormat[16],
	     lpColorSelection[16], lpLargeWidth[16];
    
     /* try to get device mode from WIN.INI */
     
/* Person who wrote this must have thought GetProfileString would return 0
  if the key word string is not found.  Actually, it copies the string pointed
  to in the default to the return buffer.  Since this code works ok as it is,
  I am not going to change it (the strings are almost always found, unless
  user has messed with win.ini).  However, since the profile string 
  "Extra Wide Paper" is searched even on epson fx-80 and oki 92/192, I put
  a meaningful default string in the call.  ***** ****** - 12/15/87 */
  
	if (GetProfileString((LPSTR)lpDeviceType, 
			     (LPSTR) "Orientation",
			     (LPSTR) NULL,
			     (LPSTR) lpOrientation,
			     16)
#if DEVMODE_WIDEPAPER			     
	 && GetProfileString((LPSTR)lpDeviceType, 
			     (LPSTR) "Extra Wide Paper",
			     (LPSTR) "No",
			     (LPSTR) lpLargeWidth,
			     16)
#endif

#if DEVMODE_NO_PRINT_QUALITY
#else
	 && GetProfileString((LPSTR)lpDeviceType, 
			     (LPSTR) "Print Quality",
			     (LPSTR) NULL,
			     (LPSTR) lpPrintQuality,
			     16)
#endif
#if COLOR
	 && GetProfileString((LPSTR)lpDeviceType, 
			     (LPSTR) "Color Selection",
			     (LPSTR) NULL,
			     (LPSTR) lpColorSelection,
			     16)
#endif
	 && GetProfileString((LPSTR)lpDeviceType, 
			     (LPSTR) "Paper Format",
			     (LPSTR) NULL,
			     (LPSTR) lpPaperFormat,
			     16)) {

#if DEVMODE_WIDEPAPER			     
		lpDevMode->use_wide_carriage = (*lpLargeWidth == 'Y' ||
				          *lpLargeWidth == 'y') ?
				    1   : 0;
#endif
		lpDevMode->orient = (*lpOrientation == 'L' ||
				     *lpOrientation == 'l') ?
				    LANDSCAPE : PORTRAIT;
			    
#if DEVMODE_NO_PRINT_QUALITY
#else
		lpDevMode->res = (*lpPrintQuality == 'L' ||
			          *lpPrintQuality == 'l') ?
			         LOW : HIGH;
#endif
#if COLOR
		lpDevMode->color = (*lpColorSelection == 'B' ||
			            *lpColorSelection == 'b') ?
			           BLACK_COLOR : ALL_COLOR;
#endif
		lpDevMode->paper = (*lpPaperFormat == 'E' ||
				    *lpPaperFormat == 'e') ?
				   FANFOLD :
				   (*lpPaperFormat == 'D' ||
				    *lpPaperFormat == 'd') ?
				   DINA4 : LETTER;
	} else {
		/* set up default parameters */
#if DEVMODE_WIDEPAPER			     
		lpDevMode->use_wide_carriage = 0;	/* bugfix: 12/15/87 */
#endif	
		lpDevMode->orient = PORTRAIT;		
#if DEVMODE_NO_PRINT_QUALITY
#else
		lpDevMode->res = HIGH;
#endif
#if COLOR
		lpDevMode->color = ALL_COLOR;
#endif
		lpDevMode->paper = isUSA() ? LETTER: DINA4;
	}
	lstrcpy((LPSTR)(lpDevMode->DeviceName), (LPSTR)lpDeviceType);
}

void FAR PASCAL SaveDeviceMode(lpDeviceType, lpDevMode)
LPSTR        lpDeviceType;
DEVMODE far *lpDevMode;
{
	/* New Device Mode */
	WriteProfileString((LPSTR)lpDeviceType, 
			   (LPSTR) "Orientation",
			   (lpDevMode->orient == LANDSCAPE) ?
				 (LPSTR)"Landscape" : (LPSTR)"Portrait");
			     
#if DEVMODE_WIDEPAPER
  #if defined(EPSON) || defined(IBMGRX)
  if ((lstrcmp((LPSTR)"Epson FX-80", (LPSTR)lpDevMode->DeviceName) == 0) ||
  (lstrcmp((LPSTR)"Okidata 92/192 (IBM)",(LPSTR)lpDevMode->DeviceName) == 0))
{}
else
  #endif /* one of the drivers with dual modes */
	WriteProfileString((LPSTR)lpDeviceType, 
			     (LPSTR) "Extra Wide Paper",
			   (lpDevMode->use_wide_carriage != 0) ?
				 (LPSTR)"Yes" : (LPSTR)"No");
#endif

#if DEVMODE_NO_PRINT_QUALITY
#else
	WriteProfileString((LPSTR)lpDeviceType, 
			   (LPSTR) "Print Quality",
			   (lpDevMode->res == LOW ) ?
				 (LPSTR)"Low" : (LPSTR)"High");
#endif
#if COLOR
	WriteProfileString((LPSTR)lpDeviceType, 
			   (LPSTR) "Color Selection",
			   (lpDevMode->color == BLACK_COLOR ) ?
				 (LPSTR)"Black/White" : (LPSTR)"Color");
#endif
	WriteProfileString((LPSTR)lpDeviceType, 
			   (LPSTR) "Paper Format",
			   (lpDevMode->paper == LETTER) ?
				 (LPSTR)"US Letter" :
				 (lpDevMode->paper == DINA4) ?
				     (LPSTR)"DIN A4" : (LPSTR)"Euro Fanfold");
}

BOOL FAR PASCAL isUSA()
{
        BYTE buf[LINE_LEN];

        if (GetProfileString((LPSTR)"intl", (LPSTR)"icountry", (LPSTR)"",(LPSTR) buf, LINE_LEN))
                /* 1 if the USA country code */
                if (atoi((LPSTR) buf) != USA_COUNTRYCODE)
                        return FALSE;
        return TRUE;
}

short NEAR PASCAL atoi(s)
LPSTR s;
{
    short n, i;

    for (i = n = 0;; n *= 10)
        {
        n += s[i] - '0';
        if (!s[++i])
            break;
        }
    return n;
}
