/**[f******************************************************************
 * enable.c (formerly reset.c)
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * ENABLE.C
 *
 * 3Jun87   sjp	 In Enable() copied lszDevice to lpdv->szDevice.
 * 4Jun87   sjp	 Added definition of paper bin string array--PAPERBINS.
 * 5Jun87   sjp	 Added definition of gBinToFeedMap -- BINTOFEEDMAP.
 * 12Jun87  sjp	 Started round rect support.
 * 17Jun87  sjp	 Fixed round rect enable bit problem.
 * 15Sept88 dgh	 Added routines to service ENUMPAPERMETRICS and 
 *		 GETSETPAPERMETRICS escapes.
 * 08Jan89  chrisg	moved lots of junk to other segments
 * 12Mar91  msd  initialize duplex and truetype download variables
 *
 *
 *
 * enable interesting facts:
 *
 *	user calls DeviceMode(hwnd, inst, devtype, port)
 *				      |      |	     |
 *				      v	     v       v
 *	GDI calls  DeviceMode(hwnd, inst, devtype, port)
 *
 *
 *	user calls CreateDC(driver, devtype, port, devmode)
 *				       |       |      | <- GDI may mess here
 *				       v       v      v
 *	GDI calls  Enable(pdevice,  devtype, port, devmode)
 *
 *	for us:
 *		devtype = "PostScript Printer"
 *		port    = "com1:", "lpt1:", etc.
 *
 *********************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "driver.h"
#include "utils.h"
#include "debug.h"
#include "resource.h"
#include "defaults.h"
#include "profile.h"
#include "getdata.h"
#include "psdata.h"
#include "etm.h"
#include "fonts.h"
#include "control2.h"
#include "dmrc.h"
#include "psver.h"
#include "mgesc.h"
#include "truetype.h"

/*------------------------------- flag -------------------------------*/
#define TRUETYPE_DEVICE_SUPPORTED

/*---------------------------- global data ---------------------------*/
char	szModule[] = "PSCRIPT";
HANDLE	ghInst;				 
BOOL    bTTEnabled;                     /* TRUE if truetype enabled */
BOOL    bInAppSetup;                    /* TRUE if app requested printer
                                         * setup (as opposed to the shell)
                                         */


/*--------------------------- local functions -------------------------*/

void	PASCAL GetGdiInfo(GDIINFO FAR *, LPPAPER, int, int, int);


/* these match with grGrays and grColors in graph.c */

#define NUM_PURE_COLORS 8	/* QMS 100 is basically a 3 plane device */
#define NUM_PURE_GRAYS  2	/* black and white for everyone else */


/* Some auxilary defines used to describe the device capabilities */

#define TC1	(TC_CR_90 + TC_CR_ANY + TC_OP_CHARACTER)
#define TC2	(TC_SF_X_YINDEP + TC_SA_DOUBLE + TC_SA_INTEGER + TC_SA_CONTIN)
#define TC3	(TC_UA_ABLE + TC_SO_ABLE)
    // Deleted TC_IA_ABLE  , see bug #10064
    // Deleted TC_EA_DOUBLE, just to be safe. (TT Substitution bug)

#define TC4	(TC_CP_STROKE + TC_OP_STROKE)	/* stroke precision */

#define CC1	(CC_PIE + CC_CHORD + CC_ELLIPSES + CC_INTERIORS + CC_STYLED)
#define CC2	(CC_WIDE + CC_WIDESTYLED)
#define CC3	(CC_ROUNDRECT)

#define PC1	(PC_POLYGON + PC_RECTANGLE + PC_SCANLINE + PC_INTERIORS + PC_STYLED)
#define PC2	(PC_WIDE + PC_WIDESTYLED + PC_TRAPEZOID)

#define LC1	(LC_POLYLINE + LC_INTERIORS + LC_STYLED + LC_WIDE + LC_WIDESTYLED)


/* The device's capabilities */
#define CAP_TXT     (TC1 + TC2 + TC3 + TC4)		/* Text capabilities */


#define CAP_RAS     (RC_STRETCHDIB + RC_BITMAP64 + RC_STRETCHBLT + RC_BITBLT + RC_GDI15_OUTPUT + RC_DIBTODEV)	/* Raster capabilities */
#define CAP_CLIP    (CP_RECTANGLE)	   		/* Clipping capabilities*/

#ifdef ROUNDRECTSTUFF
#define CAP_CURVE	(CC1+CC2+CC3)	/* Curve capabilities */
#else
#define CAP_CURVE	(CC1+CC2)	/* Curve capabilities */
#endif

#define CAP_POLY	(PC1+PC2)	/* Polygon capabilities */

#ifdef ROUNDRECTSTUFF
#define CAP_LINE	(LC1) 		/* Line capabilities */
#else
#define CAP_LINE	(LC1) 		/* Line capabilities */
#endif


#define cxyAspect	424	    /* Hypotenuse of the aspect ratio triangle */
#define cxAspect	300	    /* Horizontal leg of the aspect ratio triangle*/
#define cyAspect	300	    /* Vertical leg of the aspect ratio triangle */
#define MAXSTYLEERR	(cxyAspect * 2)


#define DT_VECTOR	0	/* Vector device type */
#define DT_RASTER	2	/* Raster device type */


/* The default GDI info to describe the printer to windows */
GDIINFO dpDefault = {
    GDI_VERSION,        /* dpVersion */
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
    0, /* 2 */		/* dpNumColors = number of colors in the color table */
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
    0, 0, 0, 0, 0,	/* futureuse3 to futureuse 7 */
    0, 0, 0		/* and 3 more new things */
};


/**********************************************************************
 * void PASCAL GetGdiInfo(lpdp, lpPaper, iRes, iScale, iNumColors)
 *
 * fills in the GDIINFO structure with the proper values.
 * most values are constant but some change according
 * to DEVICEMODE changes. these are:
 *
 *	Scale		allows the output device to be scaled for
 *			reduction/enlargment of output.
 *	Resolution	programmable resolution reduces the amount
 *			of data xferd for bitblts.
 *	Device Size	changes with paper size and scaling
 *	NumColors	changes with various color settings.
 *
 *
 **********************************************************************/

void PASCAL GetGdiInfo(lpdp, lpPaper, iRes, iScale, iNumColors)
GDIINFO FAR *lpdp;
LPPAPER lpPaper;
int	iRes;
int	iScale;		/* % reduction scale value scale by 100/iScale */
int	iNumColors;
{
	ASSERT(lpdp);
	ASSERT(lpPaper);
	ASSERT(iRes);

	DBMSG(("GetGdiInfo()\n"));

	/* Get the default GDIINFO structure */
	*lpdp = dpDefault;

	lpdp->dpNumColors = iNumColors;

	/* Give the paper size in millimeters (25.4 mm/inch) */
	lpdp->dpHorzSize = Scale(lpPaper->cxPage, 254, iRes * 10);
	lpdp->dpVertSize = Scale(lpPaper->cyPage, 254, iRes * 10);

	lpdp->dpHorzSize = Scale(lpdp->dpHorzSize, 100, iScale);
	lpdp->dpVertSize = Scale(lpdp->dpVertSize, 100, iScale);

	/* Give the paper size in number of dots */
	lpdp->dpHorzRes = lpPaper->cxPage;
	lpdp->dpVertRes = lpPaper->cyPage;

	/* scale the dpi to match the scaling value */

	/** are these values correct? **/

	lpdp->dpMLoVpt.xcoord =  Scale(iRes, iScale, 100);
	lpdp->dpMLoVpt.ycoord = -Scale(iRes, iScale, 100);
	lpdp->dpMHiVpt.xcoord =  Scale(iRes, iScale, 100);
	lpdp->dpMHiVpt.ycoord = -Scale(iRes, iScale, 100);
	lpdp->dpELoVpt.xcoord =  Scale(iRes, iScale, 100);
	lpdp->dpELoVpt.ycoord = -Scale(iRes, iScale, 100);
	lpdp->dpEHiVpt.xcoord =  Scale(iRes, iScale, 100);
	lpdp->dpEHiVpt.ycoord = -Scale(iRes, iScale, 100);
	lpdp->dpTwpVpt.xcoord =  Scale(iRes, iScale, 100);
	lpdp->dpTwpVpt.ycoord = -Scale(iRes, iScale, 100);
	lpdp->dpLogPixelsX    =  Scale(iRes, iScale, 100);
	lpdp->dpLogPixelsY    =  Scale(iRes, iScale, 100);

	lpdp->dpCaps1 = C1_TT_CR_ANY;
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
* for all of these cases we must first get the paper metrics for the given
* configuration (set throught DeviceMode).  ie. we must set up for the
* mode the user has selected (portrait/landscape, resolution, and margins)
*
* the current settings for the printer come from the current enviornment.
* if one does not exist it is created (read from win.ini).  all the data
* in the env is then transfered to the PDEVICE (DV) struct.
*
* this implies that the user sets these params through devicemode or
* escapes BEFORE he creates his printer DC (and we get called here).
*
*
**********************************************************************/

int FAR PASCAL Enable(lpdv, wStyle, lpDevType, lszFile, lpdm)
LPDV      lpdv;		/* ptr to 0.
PDEVICE or GDIINFO */
WORD      wStyle;	/* function request code */
LPSTR     lpDevType;   	/* ptr to the device type string "PSCRIPT" */
LPSTR     lszFile;     	/* ptr to the output filename string "COM1:" */
LPPSDEVMODE lpdm;		/* device mode record from the dialog */
{
	int	iRes;	   	/* The printer resolution */
	int	rc = 0;		/* return code */
	PPSDEVMODE pdm = NULL;		/* Device mode */
	PPAPER  pPaper;		/* ptr to the paper array */
	PPAPER  pAllPaper;
	PAPER	paper;
	PPRINTER pPrinter;
	int i, rightMargin, bottomMargin;

	/* Note: The device mode info is set by dialog.c */

	DBMSG(("\n>Enable(%lp %d %ls %ls %lp)\n",
	    lpdv, wStyle, lpDevType, (LPSTR)lszFile, lpdm));

	if (!lpdm ||
	    lpdm->dm.dmSize != sizeof(DEVMODE) ||
	    lpdm->dm.dmDriverVersion != DRIVER_VERSION ||
	    lpdm->dm.dmSpecVersion != GDI_VERSION ||
	    lpdm->dm.dmDriverExtra != sizeof(PSDEVMODE)-sizeof(DEVMODE) ||
	    lstrcmpi(lpDevType, lpdm->dm.dmDeviceName)) {

         pdm = AllocDevMode();
         if (!pdm) goto END_NOFREE_PRINTER;

	    	lpdm = pdm;

		if (MakeEnvironment(lpDevType, lszFile, lpdm, NULL)) {

			DBMSG((" Env created\n"));

			SetEnvironment(lszFile, (LPSTR)lpdm, sizeof(PSDEVMODE));
		} else {

			DBMSG((" Env alread existed\n"));
		}

	} 
    else 
    {
		DBMSG((" Env passed in accepted\n"));
	}

	DBMSG(("Enable() iPrinter %d\n", lpdm->iPrinter));

	/* get the current printer */

	pPrinter = GetPrinter(lpdm->iPrinter);

	if (!pPrinter)
		goto END_NOFREE_PRINTER;

	/* get the paper metrics */

	if (!(pAllPaper = GetPaperMetrics(pPrinter, lpdm)))
		goto END;

	if (!pAllPaper)
		goto END_NOFREE_PAPER;

	pPaper = pAllPaper;	/* use this as default */

	/* loop until we hit the null terminated table entry */

	for (i = 0; pAllPaper[i].iPaper; i++) {
		if (lpdm->dm.dmPaperSize == pAllPaper[i].iPaper) {
			pPaper = &pAllPaper[i];
			break;
		}
	}

	if (lpdm->marginState == ZERO_MARGINS) {
		pPaper->cxMargin = 0;
		pPaper->cyMargin = 0;
		pPaper->cxPage = pPaper->cxPaper;
		pPaper->cyPage = pPaper->cyPaper;
	}

	DBMSG((" dmPaperSize:%d dmDefaultSource:%d dmScale:%d\n", 
	    lpdm->dm.dmPaperSize, lpdm->dm.dmDefaultSource, lpdm->dm.dmScale));

	DBMSG((" Enable(): Paper:x=%d,y=%d, Page:x=%d,y=%d, Mar:x=%d,y=%d\n",
	    pPaper->cxPaper, pPaper->cyPaper,
	    pPaper->cxPage, pPaper->cyPage,
	    pPaper->cxMargin, pPaper->cyMargin));

	/* Rotate the paper metrics by 90 degrees for landscape orientation */

	// note: this no longer assumes symetric x or y margins (peterwo)

    rightMargin = pPaper->cxPaper - pPaper->cxPage - pPaper->cxMargin;
    bottomMargin = pPaper->cyPaper - pPaper->cyPage - pPaper->cyMargin;

	if (lpdm->dm.dmOrientation == DMORIENT_LANDSCAPE) 
    {
		paper.cxPaper  = pPaper->cyPaper;	/* swap x and y params */
		paper.cyPaper  = pPaper->cxPaper;	/* ... */
		paper.cxPage   = pPaper->cyPage;
		paper.cyPage   = pPaper->cxPage;
		paper.iPaper   = pPaper->iPaper;

        //  the above parameters are invariant wrt
        //  mirroring and rotation direction (90 or 270)

        if(lpdm->LandscapeOrient == 270)
        {
	    	paper.cxMargin = pPaper->cyMargin;
    		paper.cyMargin = rightMargin;
        }
        else
        {
	    	paper.cxMargin = bottomMargin;
    		paper.cyMargin = pPaper->cxMargin;
        }
        if(lpdm->bMirror)
        {
    		paper.cyMargin = pPaper->cxPaper - pPaper->cxPage 
                                - paper.cyMargin;
        }
	}
    else 
    {
		paper = *pPaper;	/* copy the paper struct straight over */

        if(lpdm->bMirror)
        {
    		paper.cxMargin = rightMargin;
        }
	}

	/* Scale the metrics for the resolution given in the device mode dialog */
#if 0
	iRes = lpdm->iRes;
#else
	// 10/11/91 ZhanW

	// calculate the max resolution possible to represent this paper
	// size without overflowing a signed 16-bit integer.
	iRes = Scale(32000, DEFAULTRES, max (paper.cxPaper, paper.cyPaper)) + 1;
    //  give us a little headroom for doing arithmetic computations
    //  like averaging two lengths. (use 10000 not 32k)  - Overruled.

	if (iRes >= lpdm->iRes)
	    iRes = lpdm->iRes;	// the true resolution is good enough.
	// otherwise, use 'iRes' as the fake resolution and scale everything
	// into this unit.

#endif
	paper.cxPaper  = Scale(paper.cxPaper,  iRes, DEFAULTRES);
	paper.cyPaper  = Scale(paper.cyPaper,  iRes, DEFAULTRES);
	paper.cxPage   = Scale(paper.cxPage,   iRes, DEFAULTRES);
	paper.cyPage   = Scale(paper.cyPage,   iRes, DEFAULTRES);
	paper.cxMargin = Scale(paper.cxMargin, iRes, DEFAULTRES);
	paper.cyMargin = Scale(paper.cyMargin, iRes, DEFAULTRES);


	/* If this is just an information request, return the metrics in GDIINFO */

	if (wStyle & InquireInfo) {

		DBMSG((" Enable(): InfoContext\n"));

		GetGdiInfo((GDIINFO FAR * )lpdv, &paper, iRes, lpdm->dm.dmScale,
			(lpdm->dm.dmColor == DMCOLOR_COLOR) && pPrinter->fColor ? 
			NUM_PURE_COLORS : NUM_PURE_GRAYS);

		((GDIINFO FAR *)lpdv)->dpDEVICEsize = sizeof(PDEVICE); /* + lstrlen(lszFile); */

		DBMSG((" Enable(): dpC=%ld,dpL=%ld\n",
		    (long)((GDIINFO FAR * )lpdv)->dpCurves,
		    (long)((GDIINFO FAR * )lpdv)->dpLines));
		DBMSG((" Enable(): sizeof(GDIINFO)=%d\n", sizeof(GDIINFO)));

		rc = sizeof(GDIINFO);
		goto END;		/* bail, we are done */
	}

	/* turn on color if the printer supports it (Printer.fColor)
	 * and the user has color selected (lpdv->fColor) */

	lpdv->fColor = (lpdm->dm.dmColor == DMCOLOR_COLOR) && pPrinter->fColor;

	lpdv->fIsClipped = FALSE;
    lpdv->nextSlot = 0;     //  just so DoResetDC won't croak
    lpdv->cbSpool = 0;     //  just so DoResetDC won't croak

	/* Mark all drawing objects as unrealized */
	lpdv->lid = -1L;
	lpdv->GraphData = GD_NONE;	/* no graphics state has been sent */
	lpdv->DLState = DL_NOTHING;
	lpdv->fPenSelected = FALSE;
	lpdv->lidFont = -1L;
	lpdv->ScaleMode = 0;		/* map DIBs to device units */
	lpdv->FillMode = 0;
	lpdv->FillColor = -1L;
	lpdv->PolyMode = PM_POLYLINE;

   /* save a copy of the DeviceMode */
   lpdv->DevMode = *lpdm;

   /* copy over needed information from DEVMODE */
	lpdv->iRealRes = lpdm->iRes;    //  Zhanw's bug
	lpdv->angle = lpdm->ScreenAngle;
	lpdv->freq = lpdm->ScreenFrequency;	// currently not used
        lpdv->bDSC = lpdm->bDSC;
        lpdv->bNegImage = lpdm->bNegImage;
        lpdv->bPerPage = lpdm->bPerPage;
        lpdv->iCustomWidth = lpdm->iCustomWidth;
        lpdv->iCustomHeight = lpdm->iCustomHeight;

        if(lpdm->dm.dmFields & DM_TTOPTION)
        {
            if(lpdm->dm.dmTTOption == DMTT_DOWNLOAD)
                lpdv->DevMode.bSubFonts = FALSE;
            else if(lpdm->dm.dmTTOption == DMTT_SUBDEV)
                lpdv->DevMode.bSubFonts = TRUE;
        }

        lpdv->bMirror = lpdm->bMirror;
        lpdv->bColorToBlack = lpdm->bColorToBlack;
        lpdv->bCompress = lpdm->bCompress;
        lpdv->bErrHandler = lpdm->bErrHandler;

#ifdef TRUETYPE_DEVICE_SUPPORTED
        if (!ACCEPTS_TRUETYPE(pPrinter) && 
              lpdv->DevMode.iDLFontFmt == DLFMT_TRUETYPE)
            lpdv->DevMode.iDLFontFmt = DLFMT_TYPE1;
#else
        if (lpdv->DevMode.iDLFontFmt == DLFMT_TRUETYPE)
            lpdv->DevMode.iDLFontFmt = DLFMT_TYPE1;
#endif

	if (IS_DUPLEX(pPrinter)) {
		if ((lpdm->dm.dmFields & DM_DUPLEX) &&
				lpdm->dm.dmDuplex >= DMDUP_SIMPLEX &&
				lpdm->dm.dmDuplex <= DMDUP_HORIZONTAL) {
			lpdv->iDuplex = lpdm->dm.dmDuplex - DMDUP_SIMPLEX;
		} else
			lpdv->iDuplex = 0;
	} else
		lpdv->iDuplex = -1;

        lpdv->dwMaxVM = lmul((DWORD)lpdm->iMaxVM, 1024);   /* convert to bytes */
        lpdv->dwCurVM = 0L;

	lpdv->fEps	   = FALSE;	/* use full header */

	/* Save the paper metrics and orientation */
	lpdv->iPrinter     = lpdm->iPrinter;
	lpdv->iPaperSource = lpdm->dm.dmDefaultSource;
	lpdv->fLandscape   = lpdm->dm.dmOrientation == DMORIENT_LANDSCAPE;
#if 0
	// binary not supported anymore

	lpdv->fBinary      = lpdm->fBinary;
#else
	lpdv->fBinary      = 0;
#endif

	lpdv->fDoEps	   = lpdm->fDoEps;
	lpdv->PathLevel = 0;	/* start out not in path */

#ifdef PS_IGNORE
	lpdv->fSupressOutput = FALSE;
#endif
	lpdv->iPageNumber = 0;

	lpdv->absPaper = *pPaper;	/* copy the paper struct straight over */
                                //  note, these dimensions are in units
                                //  of 1/100 of an inch.



	/* init the EPS bounding rect here.  hopefully this may be set by the
	 * app through the SET_BOUNDS escape (to give a tigher bounds) */


	lpdv->EpsRect.top    = lpdv->absPaper.cyPaper - lpdv->absPaper.cyMargin;
	lpdv->EpsRect.bottom = lpdv->EpsRect.top - lpdv->absPaper.cyPage;
	lpdv->EpsRect.left   = lpdv->absPaper.cxMargin;
	lpdv->EpsRect.right  = lpdv->absPaper.cxMargin + lpdv->absPaper.cxPage;

    //  scale to default unit size of 1/72 "
    lpdv->EpsRect.top    = Scale(lpdv->EpsRect.top,    72, 100);
    lpdv->EpsRect.bottom = Scale(lpdv->EpsRect.bottom, 72, 100);
    lpdv->EpsRect.left   = Scale(lpdv->EpsRect.left,   72, 100);
    lpdv->EpsRect.right  = Scale(lpdv->EpsRect.right,  72, 100);

	/* if the user has choosen and EPS file we set the output file
	 * name to that file.  If the file is null at the time of the
	 * file being opened, we will prompt for a file (in channel.c) */

	if (lpdm->fDoEps)
		lstrcpy(lpdv->szFile, lpdm->EpsFile);
	else
		lstrcpy(lpdv->szFile, lszFile);

	lpdv->paper        = paper;  // after rotation and scaling to iRes units
	lpdv->iRes         = iRes;
	lpdv->iCopies      = lpdm->dm.dmCopies;
	lpdv->iJobTimeout  = lpdm->iJobTimeout;
	lpdv->fHeader      = lpdm->fHeader;
	lpdv->marginState  = lpdm->marginState;
	lpdv->iBand = 0;	/* haven't done a band yet */

	lpdv->fContext = (wStyle & InfoContext) != 0;

	lpdv->fIntWidths = TRUE;
	lpdv->fh = -1;
	lpdv->iType = 0x5750;
	lpdv->iCurLineCap = -1;
	lpdv->iCurLineJoin = -1;
	lpdv->iCurMiterLimit = -1;
	lpdv->iNewLineCap = -1;
	lpdv->iNewLineJoin = -1;
	lpdv->iNewMiterLimit = -1;
        lpdv->hdc = NULL;
	lpdv->iTrack = 0;

	lpdv->fPairKern = FALSE;

	/* this entry added by Aldus Corporation--19 January 1987--sjp */

	lpdv->epJust = fromdrawmode;
	DBMSG((" Enable(): epJust=%d\n", lpdv->epJust));

	/* get the device name */
	lstrcpy(lpdv->szDevType, lpDevType);
	rc = LoadFontDir(lpdm->iPrinter, lszFile);

   /* save the port name */
   lstrcpy(gPort, lszFile);


   /* initialize the TrueType font list */
   lpdv->sTTFontList = 0;
   lpdv->lpTTFontList = 0;  // must be initialized so FreeTTFontTable
   lpdv->cTTFontList = 0;   // doesn't croak.

   if (!(lpdv->slTTFonts = CreateStringList()))
        rc = 0;

END:

	LocalFree((HANDLE)pAllPaper);

END_NOFREE_PAPER:

	FreePrinter(pPrinter);

END_NOFREE_PRINTER:

	DBMSG(("<Enable() return %d\n", rc));

   if (pdm) FreeDevMode(pdm);

	return rc;
}


/*
 * GetPaperMetrics()
 *
 * in:
 *	pPrinter	printer to get paper structures for
 *
 * returns:
 *	pPaper		paper sizes for all sizes supported
 *
 */

PPAPER FAR PASCAL GetPaperMetrics(PPRINTER pPrinter, LPPSDEVMODE lpdm)
{
	PPAPERSIZE pPaperSizes, pSize;
        RECT r;
	HANDLE hData;
	int size;
	PPAPER pPaper, pP;

	hData = FindResource(ghInst, MAKEINTRESOURCE(PAPERSIZES), MAKEINTRESOURCE(MY_DATA));

	if (!hData)
		return NULL;

	size = SizeofResource(ghInst, hData);

	DBMSG(("sizeof resource PAPERSIZE %d\n", size));

	/* temp paper size buffer */

	if (!(pPaperSizes = (PPAPERSIZE)LocalAlloc(LPTR, size)))
		return NULL;

        /* BEGIN BLOCK: Load in the resource */
        {
            HANDLE h;
            LPSTR lpStr;
        
            h = LoadResource(ghInst, hData);
            lpStr = LockResource(h);
            if (lpStr) 
                lmemcpy(pPaperSizes, lpStr, size);
            UnlockResource(h);
            if (!lpStr) {
                LocalFree((HANDLE)pPaperSizes);
                return NULL;
            }
        }

	DBMSG(("sizeof PAPER %d\n", size * sizeof(PAPER) / sizeof(PAPER_REC)));

	if (!(pPaper = (PPAPER)LocalAlloc(LPTR, size * sizeof(PAPER) / sizeof(PAPER_REC)))) {
		LocalFree((HANDLE)pPaperSizes);
		return NULL;
	}

	/* for each paper type (i.e. letter, legal, etc.) get the
	 * corresponding information */

	pP = pPaper;

	for (pSize = pPaperSizes; pSize->iPaperType; pSize++) {

		/* Get the physical margin values...i.e. non-printable offsets
		 * these could be dependent on the printer type and the type of
		 * paper. */

		if (PaperSupported(pPrinter, lpdm, pSize->iPaperType)) {

			/* generate correct paper values */
			pP->iPaper = pSize->iPaperType;

         /* !!! What are margins for custom paper sizes */
			/* get the physical paper dimensions */
         if (pSize->iPaperType == DMPAPER_USER) {
            pP->cxPaper = lpdm->iCustomWidth;
            pP->cyPaper = lpdm->iCustomHeight;
         } else {
			   pP->cxPaper = pSize->xSize;
			   pP->cyPaper = pSize->ySize;
         }

			GetImageRect(pPrinter, lpdm, pSize->iPaperType, &r);

			DBMSG(("image area[%d]: %d %d %d %d \n", pSize->iPaperType, 
				r.left, 
				r.top,
				r.right, 
				r.bottom));


			pP->cxMargin = r.left;
			pP->cyMargin = r.top;
			pP->cxPage = r.right  - r.left;
			pP->cyPage = r.bottom - r.top;

			pP++;
		}
	}

	pP->iPaper = 0;		/* terminate the list */

	LocalFree((HANDLE)pPaperSizes);

	return pPaper;
}


int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg, WORD cbHeapSize,
                       LPSTR lpszCmdLine)
{
    ghInst = hInstance;
    bTTEnabled = IsTrueTypeEnabled();
    bInAppSetup = FALSE;    

    /* get the help filename */
    if (!LoadString(hInstance, IDS_HELPFILE, gszHelpFile, sizeof(gszHelpFile)))
      *gszHelpFile = '\0';

    return 1;
}
