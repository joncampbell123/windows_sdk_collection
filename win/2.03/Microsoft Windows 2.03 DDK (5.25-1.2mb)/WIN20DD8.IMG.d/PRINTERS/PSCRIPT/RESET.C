

/*********************************************************************
 * RESET.C
 *
 * 3Jun87	sjp		In Enable() copied lszDevice to lpdv->szDevice.
 * 4Jun87	sjp		Added definition of paper bin string array--PAPERBINS.
 * 5Jun87	sjp		Added definition of gBinToFeedMap -- BINTOFEEDMAP.
 * 12Jun87	sjp		Started round rect support.
 * 17Jun87	sjp		Fixed round rect enable bit problem.
 *********************************************************************
 */

#include "pscript.h"
#include "printers.h"


int PASCAL LoadFontDir(int, LPSTR);
int PASCAL LoadDirEntry(LPSTR, LPSTR);
int PASCAL LoadSoftFonts(LPSTR, int far *, LPSTR);

int rgDirLink[CPRINTERS];
HANDLE rghFontDir[CPRINTERS];

extern char szKey[];    /* The profile key */

extern BOOL gl_tm;		/* "tile mode" (big printable area) */
extern BOOL gl_lie;		/* "little lie" (printable area = paper size) */

void FAR PASCAL SetKey(LPSTR);
void FAR PASCAL ReadProfile(HANDLE, DEVMODE FAR *, LPSTR);
int FAR PASCAL GetPaperType(HANDLE, int);
int FAR PASCAL MapProfile(HANDLE, LPSTR, int, int, int);

void FAR PASCAL lstrcpy(LPSTR, LPSTR);
BOOL FAR PASCAL lstrcmp(LPSTR, LPSTR);
int  FAR PASCAL lstrlen(LPSTR);
void FAR PASCAL lmemcpy(LPSTR, LPSTR, int);

#define EOF	4	/* Control-D = end of file */


/* Some auxilary defines used to describe the device capabilities */
#define TC1    (TC_CR_90 + TC_CR_ANY)
#define TC2    (TC_SF_X_YINDEP + TC_SA_DOUBLE + TC_SA_INTEGER + TC_SA_CONTIN)
#define TC3    (TC_EA_DOUBLE + TC_IA_ABLE + TC_UA_ABLE + TC_SO_ABLE)

#define CC1	(CC_PIE + CC_CHORD + CC_ELLIPSES + CC_INTERIORS + CC_STYLED)
#define CC2	(CC_WIDE + CC_WIDESTYLED)
#ifdef ROUNDRECTSTUFF
#define CC3 CC_ROUNDRECT
#endif

#define PC1	(PC_POLYGON + PC_RECTANGLE + PC_SCANLINE + PC_INTERIORS + PC_STYLED)
#define PC2	(PC_WIDE + PC_WIDESTYLED + PC_TRAPEZOID)

#define LC1	(LC_POLYLINE + LC_INTERIORS + LC_STYLED + LC_WIDE + LC_WIDESTYLED)


/* The device's capabilities */
#define CAP_TXT     (TC1 + TC2 + TC3)	/* Text capabilities */
#define CAP_RAS     RC_BITBLT + RC_GDI15_OUTPUT		/* Raster capabilities */
#define CAP_CLIP    CP_RECTANGLE	/* Clipping capabilities */

#ifdef ROUNDRECTSTUFF
#define CAP_CURVE   CC1+CC2+CC3	/* Curve capabilities */
#else
#define CAP_CURVE   CC1+CC2		/* Curve capabilities */
#endif

#define CAP_POLY    PC1+PC2		/* Polygon capabilities */
#ifdef ROUNDRECTSTUFF
#define CAP_LINE    LC1 		/* Line capabilities */
#else
#define CAP_LINE    LC1 		/* Line capabilities */
#endif


#define cxyAspect 424	    /* Hypotenuse of the aspect ratio triangle */
#define cxAspect 300	    /* Horizontal leg of the aspect ratio triangle*/
#define cyAspect 300	    /* Vertical leg of the aspect ratio triangle */
#define MAXSTYLEERR (cxyAspect * 2)


#define     DT_VECTOR		0	/* Vector device type */
#define     DT_RASTER		2	/* Raster device type */


static char szModule[] = "pscript";

/* The default GDI info to describe the printer to windows */
GDIINFO dpDefault =
    {
    0x101,              /* dpVersion = 1.01 */
    DT_RASTER,		/* dpTechnology = raster/plotter mixture */
    0,			/* dpHorzSize = page width in millimeters */
    0,			/* dpVertSize = page height in millimeters */
    0,			/* dpHorzRes = pixel width of page */
    0,			/* dpVertRes = pixel height of page */
    1,			/* dpBitsPixel = bits per pixel */
    1,			/* dpPlanes = number of bit planes */
    17, 		/* dpNumBrushes = number of brushes */
    8,			/* dpNumPens = number of pens on the device */
    0,			/* futureuse (not documented) */
    4,			/* dpNumFonts = number of fonts the device has */
    2,			/* dpNumColors = number of colors in the color table*/
    0,			/* dpDEVICEsize = size of the device desciptor */
    CAP_CURVE,		/* dpCurves = (all capabilities) */
    CAP_LINE,		/* dpLines = (all capabilities) */
    CAP_POLY,		/* dpPolygonals = (all capabilities) */
    CAP_TXT,		/* dpText = (all capabilities) */
    CAP_CLIP,		/* dpClip = (all capabilities) */
    CAP_RAS,		/* dpRaster = (BitBlt and 2.0 support) */
    cxAspect,		/* dpAspectX = x major distance */
    cyAspect,		/* dpAspectY = y major distance */
    cxyAspect,		/* dpAspectXY = hypotenuse distance */
    MAXSTYLEERR,	/* dpStyleLen = Length of segment for line style */
    {254, 254}, 	/* dpMLoWin: tenths of a millimeter in an inch */
    {0, 0},		/* dpMLoVpt: device resolution in dots per inch */
    {2540, 2540},	/* dpMHiWin: hundreths of a millimeter in an inch */
    {0, 0},		/* dpMHiVpt: device resolution in dots per inch */
    {100, 100}, 	/* dpELoWin: hundreths of an inch in an inch */
    {0, 0},		/* dpELoVpt: device resolution in dots per inch */
    {1000, 1000},	/* dpEHiWin: thousandths of an inch in an inch */
    {0, 0},		/* dpEHiVpt: device resolution in dots per inch */
    {1440, 1440},	/* dpTwpWin: twips in an inch */
    {0, 0},		/* dpTwpVpt: device resolution in dots per inch */
    0,			/* dpLogPixelsX */
    0,			/* dpLogPixelsY */
    1,			/* dpDCManage: Seperate PDevice required per filename*/
    0, 0, 0, 0, 0	/* futureuse3 to futureuse 7 */
    };


/****************************************************************************/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

/*#define DEBUG1_ON*/
#ifdef DEBUG1_ON
#define DBMSG1(msg) printf msg
#else
#define DBMSG1(msg)
#endif

/*#define DEBUG_RC*/
#ifdef DEBUG_RC
#define DBMSG2(msg) printf msg
#else
#define DBMSG2(msg)
#endif
/****************************************************************************/

/****************************************************************************
* Name: StripColon()
*
* Action:   Copy the source string to the destination
*	    buffer, truncating the source string at
*	    the first colon ":" encountered.  Note that
*	    this routine may be called to get the
*	    length of the truncated string with a NULL
*	    destination ptr.
*
* Returns:
*	The "truncated" string length.
*
***************************************************************************
*/
int PASCAL StripColon(lszDst, lszSrc)
LPSTR lszDst;	    /* The destination buffer ptr */
LPSTR lszSrc;	    /* The source string ptr */
    {
    int cb;
    char bCh;

    cb = 0;
    if (lszSrc)
	{
	while ((bCh = *lszSrc++) && (bCh!= ':'))
	    {
	    ++cb;
	    if (lszDst)
		*lszDst++ = bCh;
	    }
	}
    ++cb;
    if (lszDst)
	*lszDst = 0;

    return(cb);
    }


/******************************************************************
* Name: GetGdiInfo()
*
* Action: This function gets the GDI information structure that
*	  describes the capabilities, size, and resolution of the
*	  device to GDI.
*
* Note: Someone (obviously with infinite wisdom) designed the GDIINFO
*	structure so that every driver must give the size of the
*	display surface in many different measurment systems.  Why
*	doesn't GDI  compute these values from a single quantity?
*	Luckily, we don't have to compute the display size in fathoms,
*	cubits, or light-years (maybe we'll see this as an enhancement
*	in some future release)!
*
*********************************************************************
*/
void PASCAL GetGdiInfo(lpdp, lpPaper, iRes)
GDIINFO FAR *lpdp;
PAPER FAR *lpPaper;
int iRes;
{
    ASSERT(lpdp!=NULL);
    ASSERT(lpPaper!=NULL);
    ASSERT(iRes!=0);


    /* Get the default GDIINFO structure */
    lmemcpy((LPSTR) lpdp, (LPSTR) &dpDefault, sizeof(GDIINFO));

    /* Give the paper size in millimeters (25.4 mm/inch) */
    lpdp->dpHorzSize = Scale(lpPaper->cxPage, 254, iRes * 10);
    lpdp->dpVertSize = Scale(lpPaper->cyPage, 254, iRes * 10);

    /* Give the paper size in number of dots */
    lpdp->dpHorzRes = lpPaper->cxPage;
    lpdp->dpVertRes = lpPaper->cyPage;


    /* Scale the display surface size for all measurement systems */
    lpdp->dpMLoVpt.x = iRes;
    lpdp->dpMLoVpt.y = -iRes;
    lpdp->dpMHiVpt.x = iRes;
    lpdp->dpMHiVpt.y = -iRes;
    lpdp->dpELoVpt.x = iRes;
    lpdp->dpELoVpt.y = -iRes;
    lpdp->dpEHiVpt.x = iRes;
    lpdp->dpEHiVpt.y = -iRes;
    lpdp->dpTwpVpt.x = iRes;
    lpdp->dpTwpVpt.y = -iRes;
    lpdp->dpLogPixelsX = iRes;
    lpdp->dpLogPixelsY = iRes;
}


BOOL GetPaperMetrics(hInstance,nPrinter,lpPaper)
	HANDLE hInstance;
	short nPrinter;
	LPPAPER lpPaper;
{
	short i;
	LPPOINT lpPaperSizes;
	HANDLE hData;
	PRINTER thePrinter;
	BOOL fResource=FALSE;
	BOOL rc=TRUE;

	if(lpPaperSizes=(LPPOINT)GetResourceData(hInstance,(LPHANDLE)&hData,
		(LPSTR)(long)PAPERSIZES,(LPSTR)(long)MY_DATA)
	){
		fResource=TRUE;
	}else{
		rc=(int)FALSE;
		goto END;
	}
	/* get the relevant printer capabilities structure
	 */
	if(!GetPrinterCaps(hInstance,nPrinter,(LPPRINTER)&thePrinter)){
		rc=FALSE;
		goto END;
	}

	/* for each paper type (i.e. letter, legal, etc.) get the corresponding
	 * information
	 */
	for(i=0;i<NUMPAPERS;i++){
		/* generate correct paper values */
		lpPaper[i].iPaper=i+IPAPERMIN;

		/* get the physical paper dimensions */
		lpPaper[i].cxPaper=(lpPaperSizes+i)->x;
		lpPaper[i].cyPaper=(lpPaperSizes+i)->y;

		/* Get the physical margin values...i.e. non-printable offsets
		 * these could be dependent on the printer type and the type of
		 * paper.
		 */
		lpPaper[i].cxMargin=thePrinter.imageDelta[i].x;
		lpPaper[i].cyMargin=thePrinter.imageDelta[i].y;

		/* calculate the imageable area on the paper */
		lpPaper[i].cxPage=lpPaper[i].cxPaper - 2*lpPaper[i].cxMargin;
                lpPaper[i].cyPage=lpPaper[i].cyPaper - 2*lpPaper[i].cyMargin;
	}

END:
	if(fResource) UnGetResourceData(hData);
	return(rc);
}


/*********************************************************************
* Name: Enable()
*
* Action: The enable routine performs three different functions
*	  depending on the setting of the bit fields in the
*	  style parameter.
*
*	  1. Request the printer driver to fill in the GDIINFO
*	     structure that describes the capabilities and resolution
*	     of the printer.
*
*	  2. Initialization for an InfoContext.  For this style,
*	     you only have to initialize enough to allow applications
*	     to measure strings, etc. without actually printing.
*
*	  3. A full blown initialization that will allow printing
*	     to occur.
*
**********************************************************************
*/
int FAR PASCAL Enable(lpdv, iStyle, lszDevice, lszFile, lpdm)
	DV FAR *lpdv;	    /* Far ptr to device-specific device descriptor */
	int iStyle;	    	/* The function request code */
	LPSTR lszDevice;    /* Far ptr to the device type string */
	LPSTR lszFile;	    /* Far ptr to the output filename string */
	DEVMODE far *lpdm;  /* The device mode record from the dialog */
{
    extern void FAR PASCAL ReadProfile(HANDLE, DEVMODE FAR *, LPSTR);

    LPSTR lpb;
    int i;
    int cbdv;	    /* The device descriptor size */
    int cxPaper;    /* Horizontal page size in dots */
    int cyPaper;    /* Vertical page size in dots */
    int oxPage;     /* Horizontal offset to the printable area in dots */
    int oyPage;     /* Vertical offset to the printable area in dots */
    int iRes;	    /* The printer resolution */
	int rc;			/* return code */
    HANDLE hres;
	HANDLE hData;	/* handle to the "global" resource data */
    DEVMODE dm;     /* Device mode */
    PAPER far *lpPaper;     /* A far ptr to the paper array */
    PAPER paper,truePaper[NUMPAPERS];

	BOOL fResource=FALSE;	/* flag indicating paper resource read and that 
					 		 * deletion is necessary at end of procedure */

	HANDLE hInstance=GetModuleHandle((LPSTR)szModule);


    /* Note: The device mode info is set by dialog.c */
	DBMSG1(((LPSTR)">Enable(): lpdv=%ld\r",lpdv));

	{
		BOOL differentPorts=FALSE;

	    /* Don't use the device mode info if it is for a different port */
		if (lpdm!=(LPDM) (long) NULL){
			if (lstrcmp(lszDevice, (LPSTR) lpdm->szDevice)){
				differentPorts=TRUE;
			    lpdm = (LPSTR)(long)NULL;
			}
		}

	    /* Def. to the device mode params in win.ini if none were supplied */
		if (!lpdm){
			lpdm = (DEVMODE FAR *) &dm;

			ReadProfile(hInstance, lpdm, lszFile);
			if(!differentPorts){
				SetEnvironment(lszFile, (LPSTR)lpdm, sizeof(DEVMODE));
			}
		}
	}
    cbdv = sizeof(DV) + StripColon((LPSTR) (long) NULL, lszFile);

    /* Search for the paper type in the paper table */

	/* if "Tile Mode=yes", use the big printable area (the big lie)
	 * if "margins=no", use the the little lie (gl_lie)
	 */
	if (gl_tm){
		DBMSG1(((LPSTR)" Enable(): Tile Mode=yes\r"));
		if(lpPaper=(LPPAPER)GetResourceData(hInstance,(LPHANDLE)&hData,
			(LPSTR)(long)NEGMARGINS,(LPSTR)(long)MY_DATA)
		){
			fResource=TRUE;
		}else{
			goto ERROR1;
		}
	}else if (gl_lie){
		DBMSG1(((LPSTR)" Enable(): Margins=no\r"));
		if(lpPaper=(LPPAPER)GetResourceData(hInstance,(LPHANDLE)&hData,
			(LPSTR)(long)ZEROMARGINS,(LPSTR)(long)MY_DATA)
		){
			fResource=TRUE;
		}else{
			goto ERROR1;
		}
	}else{
		DBMSG1(((LPSTR)" Enable(): Normal\r"));
		/* Failure implies that there was some problem reading the
		 * printer information from the resource or that it couldn't
		 * be locked or freed.
		 */
		if(!GetPaperMetrics(hInstance,lpdm->iPrinter,(LPPAPER)truePaper)){
			goto ERROR0;
		}
    	lpPaper = (LPPAPER)truePaper;
	}

	/* Search through list of supported papers to find the paper type
	 * that was requested by the user and set the pointer to look at it.
	 */
    for (i=0; i<(CPAPERS-1); ++i){
		if (lpdm->iPaper == lpPaper->iPaper) break;
		++lpPaper;
	}

	DBMSG1(((LPSTR)" Enable(): iP=%d, Paper:x=%d,y=%d, Page:x=%d,y=%d, Mar:x=%d,y=%d\r",
		lpPaper->iPaper,
		lpPaper->cxPaper,lpPaper->cyPaper,
		lpPaper->cxPage,lpPaper->cyPage,
		lpPaper->cxMargin,lpPaper->cyMargin));

    /* Rotate the paper metrics by 90 degrees for landscape orientation */
    if (lpdm->fLandscape){
		paper.cxPaper = lpPaper->cyPaper;
		paper.cyPaper = lpPaper->cxPaper;
		paper.cxPage = lpPaper->cyPage;
		paper.cyPage = lpPaper->cxPage;
		paper.cxMargin = lpPaper->cyMargin;
		paper.cyMargin = lpPaper->cxMargin;
		paper.iPaper = lpPaper->iPaper;
	}else{
		paper=*lpPaper;
	}

    /* Scale the metrics for the resolution given in the device mode dialog */
    iRes = lpdm->iRes;

    paper.cxPaper = Scale(paper.cxPaper, iRes, DEFAULTRES);
    paper.cyPaper = Scale(paper.cyPaper, iRes, DEFAULTRES);
    paper.cxPage  = Scale(paper.cxPage, iRes, DEFAULTRES);
    paper.cyPage  = Scale(paper.cyPage, iRes, DEFAULTRES);
    paper.cxMargin = Scale(paper.cxMargin, iRes, DEFAULTRES);
    paper.cyMargin = Scale(paper.cyMargin, iRes, DEFAULTRES);

    /* If this is just an information request, return the metrics in GDIINFO
	 */
    if (iStyle & InquireInfo){
		GetGdiInfo((GDIINFO FAR *)lpdv, (PAPER FAR *) &paper, iRes);
		((GDIINFO FAR *) lpdv)->dpDEVICEsize = cbdv;

		DBMSG1(((LPSTR)" Enable(): dpC=%ld,dpL=%ld\r",
			(long)((GDIINFO FAR *)lpdv)->dpCurves,
			(long)((GDIINFO FAR *)lpdv)->dpLines));
		DBMSG1(((LPSTR)" Enable(): sizeof(GDIINFO)=%d\r",sizeof(GDIINFO)));

		rc=sizeof(GDIINFO);
		goto END;
	}

    /* Initialize the device header to all zeros */
    lpb = (LPSTR) lpdv;
    while (--cbdv>=0)
		*lpb++ = 0;

    lpdv->dh.fIsClipped = FALSE;
    lpdv->dh.iPaperSource = 0;

    /* Mark all drawing objects as unrealized */
    lpdv->dh.lid = -1L;
    lpdv->dh.br.lid = -1L;
    lpdv->dh.pen.lid = -1L;
    lpdv->dh.lidFont = -1L;

    /* Save the paper metrics and orientation */
    lpdv->dh.iPrinter = lpdm->iPrinter;
    lpdv->dh.iPaperSource = lpdm->iFeed;
    lpdv->dh.fLandscape = lpdm->fLandscape;
    lmemcpy((LPSTR) &lpdv->dh.paper, (LPSTR) &paper, sizeof(PAPER));
	lpdv->dh.iRes = iRes;
    lpdv->dh.iCopies = lpdm->iCopies;

    /* Set up the band bitmap as a memory bitmap device */
    lpdv->dh.iBand = 0;
    lpdv->dh.bm.bmType = 0;
    lpdv->dh.bm.bmWidthBytes = (paper.cxPage+7)/8;
    if (lpdv->dh.bm.bmWidthBytes>sizeof(lpdv->rgbBand))
	lpdv->dh.bm.bmWidthBytes = sizeof(lpdv->rgbBand);
    lpdv->dh.bm.bmWidth = lpdv->dh.bm.bmWidthBytes * 8;
    lpdv->dh.bm.bmHeight = sizeof(lpdv->rgbBand)/lpdv->dh.bm.bmWidthBytes;
    lpdv->dh.bm.bmPlanes = 1;
    lpdv->dh.bm.bmBitsPixel = 1;
    lpdv->dh.bm.bmBits = (LPSTR) lpdv->rgbBand;
    lpdv->dh.bm.bmWidthPlanes = lpdv->dh.bm.bmWidthBytes * 
		lpdv->dh.bm.bmHeight;
    if (InfoContext)
		lpdv->dh.bm.bmBits = (LPSTR) (long) NULL;
    else
		lpdv->dh.bm.bmBits = (LPSTR) lpdv->rgbBand;

    lpdv->dh.fContext = (iStyle & InfoContext) != 0;

    lpdv->dh.fh = -1;
    lpdv->dh.iType = -1;
    lpdv->dh.fIntWidths = TRUE;

    lpdv->dh.iCurLineCap = -1;
    lpdv->dh.iCurLineJoin = -1;
    lpdv->dh.iCurMiterLimit = -1;
    lpdv->dh.iNewLineCap = -1;
    lpdv->dh.iNewLineJoin = -1;
    lpdv->dh.iNewMiterLimit = -1;

    lpdv->dh.iTrack = 0;

    lpdv->dh.fPairKern = FALSE;

    lpdv->dh.hmod = hInstance;

	/* this entry added by Aldus Corporation--19 January 1987--sjp */
	lpdv->dh.epJust = fromdrawmode;
	DBMSG(((LPSTR)" Enable(): epJust=%d\r",lpdv->dh.epJust));

	/* get the output filename string */
    StripColon((LPSTR) lpdv->szFile, lszFile);

	/* get the device name */
	lstrcpy(lpdv->szDevice, lszDevice);
    rc=LoadFontDir(lpdm->iPrinter, lszFile);

END:
	if(fResource){
		if(!UnGetResourceData(hData)) goto ERROR1;
	}
	DBMSG1(((LPSTR)"<Enable()\r"));
	return(rc);

/* if you're not exactly sure or if a resource WAS accessed... */
ERROR0:
	if(fResource) UnGetResourceData(hData);

/* if you know for sure that a resource was not acessed...*/
ERROR1:
/*  This doesn't quite work...maybe figure out a better way later.
 *	MessageBox(hInstance,(LPSTR)"Can't read resource.",
 *		(LPSTR)"Fatal driver error!",MB_OK | MB_ICONEXCLAMATION);
 */	return((int)FALSE);
}


/*********************************************************************
 * Name: LoadFontDir()
 *
 * Action: Load the specified font directory into memory from the
 *	  resource and append the font metrics for any
 *	  external softfonts if they exist.
 *
 **********************************************************************
 */
int PASCAL LoadFontDir(iPrinter, lszFile)
	int iPrinter;
	LPSTR lszFile;	    /* Far ptr to the output filename string */
{
    HANDLE hres;	    /* The font directory resource */
    HANDLE hmod;	    /* The printer driver's module handle */
    HANDLE h;
    int cSoftFonts;	    /* The number of soft fonts */
    int cFonts; 	    /* The number of fonts */
    int fh;		    /* The resource file handle */
    int cb;		    /* Byte count */
    int i;
    LPSTR lpbDst;
    LPSTR lpbDir;

    /* If the font directory is already loaded, just return */
    if (lpbDir = LockFontDir(iPrinter)){
		UnlockFontDir(iPrinter);
		++rgDirLink[iPrinter];
		return(TRUE);
	}
    hmod = GetModuleHandle((LPSTR)szModule);

    /* Find the font directory */
    if (!(hres = FindResource(hmod, (LPSTR)(long)iPrinter+1,
		 (LPSTR)(long)MYFONTDIR)))
	{
	return(FALSE);
}


    /* There will be two passes through the following code. */
    /* Once to compute the directory size, then again to load it */

AGAIN:

    cb = SizeofResource(hmod, hres);
    lpbDst = lpbDir;

    if (lpbDir){
		/* Read the font directory into memory */
		fh = AccessResource(hmod, hres);
		if (_lread(fh, lpbDir, cb)<0)
		    cb = 0;
		_lclose(fh);

		if (cb<=0)
	    {
		    UnlockFontDir(iPrinter);
		    DeleteFontDir(iPrinter);
		    return(FALSE);
	    }

		/* Compute ptr to first "softfont" slot */
		cFonts = *((short far *)lpbDst)++;
		for (i=0; i<cFonts; ++i)
		    lpbDst += *((short far *)lpbDst);
	}

    /* Extend the font directory by adding any softfonts */
    cb += LoadSoftFonts(lpbDst, (int far *) &cSoftFonts, lszFile);

    /* Update the font count at the beginning of the directory */
    if (lpbDir) *((short far *)lpbDir) = cFonts + cSoftFonts;

    /* At this point we may have only computed the font directory size.
     * If so, then allocate memory for it and go back to load it.
     */
    if (!lpbDir){
                if (h = GlobalAlloc(GMEM_FIXED | GMEM_DDESHARE | GMEM_LOWER, (long) cb)){
		    rghFontDir[iPrinter] = h;
		    rgDirLink[iPrinter] = 1;
		    if (lpbDir = LockFontDir(iPrinter))
				goto AGAIN;
			DeleteFontDir(iPrinter);
		}
		return(FALSE);
	}
    else
		UnlockFontDir(iPrinter);
    return(TRUE);
}


/***********************************************************
* Name: LoadSoftDir()
*
* Action: Calculate the size of the softfont directory and
*	  load it (if storage has been allocated for it).
*
************************************************************
*/
int PASCAL LoadDirEntry(lszFile, lpbDst)
LPSTR lszFile;		/* The PFM file name */
LPSTR lpbDst;	      /* Ptr to place to load the entry */
    {
    int fh;
    int cbEntry;	/* The size of the directory entry */
    int cbdf;		/* The size of the device font header */
    int cbDevice;	/* The size of the device name */
    int cbFace; 	/* The size of the face name */
    int cbFont; 	/* Size of the font name */
    int cbRead;
    LPSTR lpbSrc;
    int i;

    PFM pfm;
    char szDevice[32];
    char szFace[32];

    if ((fh = _lopen(lszFile, READ)) > 0);
	{
	if (_lread(fh, (LPSTR) &pfm, sizeof(PFM))!=sizeof(PFM))
	    goto READERR;
	_llseek(fh, (long) pfm.df.dfFace, 0);
	if (_lread(fh, (LPSTR) szFace, sizeof(szFace))<=0)
	    goto READERR;
	_llseek(fh, (long) pfm.df.dfDevice, 0);
	if (_lread(fh, (LPSTR) szDevice, sizeof(szDevice))<=0)
	    goto READERR;
	_lclose(fh);
	}

    cbdf = ((LPSTR) &pfm.df.dfBitsPointer) - (LPSTR)&pfm;
    cbFace = lstrlen((LPSTR) szFace) + 1;
    cbDevice = lstrlen((LPSTR) szDevice) + 1;
    cbFont = lstrlen((LPSTR) lszFile) + 2;
    cbEntry = cbdf + cbFace + cbDevice + cbFont + 4;

    if (lpbDst)
	{
	*((short far *)lpbDst)++ = cbEntry;
	*((short far *)lpbDst)++ = cbdf + cbFace + cbDevice;


	lpbSrc = (LPSTR) &pfm.df;
	pfm.df.dfFace = cbdf;
	pfm.df.dfDevice = cbdf + cbFace;
	for (i=0; i<cbdf; ++i)
	    *lpbDst++ = *lpbSrc++;

	/* Copy the face name into the font directory */
	lpbSrc = szFace;
	for (i=0; i<cbFace; ++i)
	    *lpbDst++ = *lpbSrc++;

	/* Copy the device name into the font directory */
	lpbSrc = szDevice;
	for (i=0; i<cbDevice; ++i)
	    *lpbDst++ = *lpbSrc++;

	/* Copy the PFM file name into the font directory */
	*lpbDst++ = '$';	 /* Mark this as a softfont */
	lpbSrc = lszFile;
	for (i=0; i<cbFont; ++i)
	    *lpbDst++ = *lpbSrc++;
	}
    return(cbEntry);

READERR:
    _lclose(fh);
    return(0);
    }



/****************************************************************
* Name: LoadSoftFonts()
*
* Action: Either load the soft fonts into memory or get information
*	  about them depending on the value of the destination pointer.
*	  If the destination pointer is NULL, then this routine just
*	  computes the size and number of softfonts specified in
*	  win.ini.  If the destination pointer is not NULL, then
*	  the soft fonts are actually loaded.
*
* Returns: The size of the softfonts.
*
*	   Indirectly returns the number of soft fonts.
*
*********************************************************************
*/
int PASCAL LoadSoftFonts(lpbDir, lpcFonts, lszFile)
	LPSTR lpbDir;
	int far *lpcFonts;
	LPSTR lszFile;	    /* Far ptr to the output filename string */
{
    static char szField[] = "softfontNN";
    static char szDrNm[] = "PostScript,";

    LPSTR lpbDst;
    int cFonts;
    int i, iVal;
    char szFile[40];
    int cbDir;
    int cbEntry;

	/* I'll bet this isn't the right way to do it...the name of the driver
	 * should come from somewhere central...
	 */
	SetKey (lszFile);	/* see dialog.c */
	DBMSG(( (LPSTR)"lszFile=%ls\n",lszFile));
	DBMSG(( (LPSTR)"szKey=%ls\n",(LPSTR)szKey));

    cbDir = 0;
    cFonts = GetProfileInt((LPSTR) szKey, (LPSTR) "softfonts", 0);


    if (cFonts>99)
		cFonts = 99;

    *lpcFonts = cFonts;

    for (i=1; i<=cFonts; ++i)
	{

	lpbDst = szField + sizeof(szField) - 3;
	iVal = i/10;
	if (iVal!=0)
	    *lpbDst++ = (char)iVal + '0';
	*lpbDst++ = (char)(i - iVal * 10) + '0';
	*lpbDst++ = 0;

	GetProfileString((LPSTR) szKey, (LPSTR) szField, (LPSTR) "",
				    (LPSTR) szFile, sizeof(szFile));

	cbEntry = LoadDirEntry((LPSTR) szFile, lpbDir);
	if (lpbDir)
	    lpbDir += cbEntry;
	cbDir += cbEntry;
	}
    return(cbDir);
}
