/**[f******************************************************************
 * control2.c
 *
 *
 **f]*****************************************************************/

/*********************************************************************
 * 8Jan89	chrisg	created (moved from enable.c)
 * 31Jul90      kensy   added StartNewPage() for per page initialization
 * 7Feb92   peterwo  calculate EPS bounding rectangle correctly.
 *              DefaultPSToDevice() and DeviceToDefaultPS
 *              transform points between the two coordinate systems.
 *
 *
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
#include "control2.h"
#include "dmrc.h"
#include "channel.h"
#include "etm.h"
#include "truetype.h"

void FAR PASCAL OpaqueBox(LPDV lpdv, BOOL fExtTextOut, int x, int y, int dx, int dy, DWORD color);

void FAR PASCAL DefaultPSToDevice( LPDV lpdv, LPPOINT  lpPoint);

/*--------------------------- local functions -------------------------*/

short PASCAL CompareImageableAreas(LPRECT, LPRECT, LPBOOL);
short PASCAL SearchPaperMetrics(PPAPER, LPRECT, LPSHORT, LPBOOL, short, LPPSDEVMODE);

#define ERROR     (-2)
#define NOTFND    (-1)


/*--------------------------------------------------------------------------
 * This routine is called by the ENUMPAPERMETRICS Escape. It is designed to 
 * return an array of RECTS whose values representing the imageable areas of
 * the supported paper types.
 * Returns: a positive value if successful or -1 otherwise.
 *
 * if mode == 0 then we stuff the # of rects we have into lprcMetrics (short)
 * if mode == 1 then we fill lprcMetrics with all the rects that we support
 *
 * Digby Horner - 9/15/88 Adobe Systems Inc.                                
 *
 *------------------------------------------------------------------------*/

short FAR PASCAL EnumPaperMetrics(lpdv, lprcMetrics, mode, fTypesetter)
LPDV	lpdv;
LPRECT	lprcMetrics;	// return stuff here
short	mode;		// modes: 0=INFORM 1=PERFORM */
short	fTypesetter;	// if TRUE support even negative top left (?)
{
	#define PERFORM 1
	PPRINTER pPrinter;
	PPAPER pPaper;    /* ptr to the array of paper metrics */
	short top, left, result = NOTFND;
	short rect_count;
	int iPaper;

	ASSERT(lpdv);
	ASSERT(lprcMetrics);


	pPrinter = GetPrinter(lpdv->iPrinter);


	if (!pPrinter) {
		result = ERROR;
		goto END_NOFREE;
	}

	pPaper = GetPaperMetrics(pPrinter, &lpdv->DevMode);

	if (!pPaper) {
		result = ERROR;
		goto END;
	}

#if 0
	if (mode == PERFORM)
		rect_count =  0;	// we are going to copy out
	else
		rect_count =  1;	// we are just going to count
#else
	rect_count =  0;	// none found yet
#endif

	for (iPaper = 0; /* FOREVER */ ; iPaper++) {
                /* check to see if we have exhausted all the paper entries */
                if (GetPaperEntry(pPrinter, &lpdv->DevMode, iPaper) < 0)
                    break;


		DBMSG(("iPaper %d\n", iPaper));

		top = Scale(pPaper[iPaper].cyMargin, lpdv->iRes, DEFAULTRES);
		left = Scale(pPaper[iPaper].cxMargin, lpdv->iRes, DEFAULTRES);

		if ((top > 0 || left > 0) || (fTypesetter == 1)) {
			if (mode == PERFORM) {

				/* imageable area with default margins */

				lprcMetrics[rect_count].top = top;
				lprcMetrics[rect_count].left = left;
				lprcMetrics[rect_count].right = 
				    Scale(pPaper[iPaper].cxPage + pPaper[iPaper].cxMargin, lpdv->iRes, DEFAULTRES);
				lprcMetrics[rect_count].bottom = 
				    Scale(pPaper[iPaper].cyPage + pPaper[iPaper].cyMargin, lpdv->iRes, DEFAULTRES);

				DBMSG(("  %d %d %d %d\n", 
					lprcMetrics[rect_count].left, lprcMetrics[rect_count].top,  
					lprcMetrics[rect_count].right, lprcMetrics[rect_count].bottom
				));

				rect_count++;

				/* imageable area with no margins */

				lprcMetrics[rect_count].top = lprcMetrics[rect_count].left = 0;
				lprcMetrics[rect_count].right = Scale(pPaper[iPaper].cxPaper, lpdv->iRes, DEFAULTRES);
				lprcMetrics[rect_count].bottom = Scale(pPaper[iPaper].cyPaper, lpdv->iRes, DEFAULTRES);

				DBMSG(("  %d %d %d %d\n", 
					lprcMetrics[rect_count].left, lprcMetrics[rect_count].top,  
					lprcMetrics[rect_count].right, lprcMetrics[rect_count].bottom
				));

				rect_count++;
			} else
				rect_count += 2;
			result = 1;
		}
	}
	if (mode == PERFORM) {
#if 0
		/* imageable area with tiled margins */
		lprcMetrics[rect_count].top = lprcMetrics[rect_count].left = -6825;
		lprcMetrics[rect_count].right = lprcMetrics[rect_count].bottom = 13650;
#endif
	} else
		*((short far * )lprcMetrics) = rect_count;	// return # of rects we have

	DBMSG(("  rect_count %d\n", rect_count));

	LocalFree((HANDLE)pPaper);
END:
	FreePrinter(pPrinter);
END_NOFREE:
	return result;
}

/*--------------------------------------------------------------------------
   Compares two RECTS representing imageable areas - returning TRUE if a
   match occurs FALSE otherwise. Sets.dm.dmOrientation == DMORIENT_LANDSCAPE according to r2's
   orientation. 

   assumptions: 
     - the rect r1 will always be in portrait mode 
     - r2 could be in either portrait or landscape orientation
 *--------------------------------------------------------------------------*/

short PASCAL CompareImageableAreas(r1, r2, lpfLandscape)
LPRECT r1, r2;
LPBOOL lpfLandscape;
{
	if (r1->top == r2->top && r1->left == r2->left
	     && r1->right == r2->right && r1->bottom == r2->bottom) {
		*lpfLandscape = FALSE;
		return TRUE;
	}
	if (r1->top == r2->left && r1->left == r2->top
	     && r1->right == r2->bottom && r1->bottom == r2->right) {
		*lpfLandscape = TRUE;
		return TRUE;
	}
	return FALSE;
}


/*--------------------------------------------------------------------------*/
short PASCAL SearchPaperMetrics(pPaper, lpRect, margins, lpfLandscape, res, lpdm)
PPAPER  pPaper;
LPRECT	lpRect;
LPSHORT margins;
LPBOOL  lpfLandscape;
short	res;
LPPSDEVMODE lpdm;
{
	short	j;
	RECT	r;

	for (j = 0; pPaper[j].iPaper; j++) {

		r.top = Scale(pPaper[j].cyMargin, res, DEFAULTRES);
		r.left = Scale(pPaper[j].cxMargin, res, DEFAULTRES);
		if (r.top || r.left) {
			r.right = Scale(pPaper[j].cxPage + pPaper[j].cxMargin, res, DEFAULTRES);
			r.bottom = Scale(pPaper[j].cyPage + pPaper[j].cyMargin, res, DEFAULTRES);
			if (CompareImageableAreas(lpRect, &r, lpfLandscape)) {
				*margins = DEFAULT_MARGINS;
				return (j);
			}
			r.top = r.left = 0;
			r.right = Scale(pPaper[j].cxPaper, res, DEFAULTRES);
			r.bottom = Scale(pPaper[j].cyPaper, res, DEFAULTRES);
			if (CompareImageableAreas(lpRect, &r, lpfLandscape)) {
				*margins = ZERO_MARGINS;
				return (j);
			}
		}
	}

#if 0
	/* tiling has been removed from the driver */

	/* check for tiled margin state */

	if (lpRect->left == -6825 && lpRect->top == -6825
	     && lpRect->right == 6825 && lpRect->bottom == 6825) {

		*margins = TILE_MARGINS;
		*lpfLandscape = lpdm->dm.dmOrientation == DMORIENT_LANDSCAPE;
		return lpdm->dm.dmPaperSize;
	}
#endif
	return NOTFND;
}


/*--------------------------------------------------------------------------
 * service routine for the GETSETPAPERMETRICS escape - sets the current paper
 * type and imageable area taking into account margin state and orientation.
 *
 * this updates *lpdm and *lpdv with new paper metrics
 *
 *-------------------------------------------------------------------------*/

short FAR PASCAL SetPaperMetrics(lpdv, lpdm, lpbIn)
LPDV lpdv;
LPPSDEVMODE lpdm;
LPRECT lpbIn;
{
	PPRINTER pPrinter;
	PPAPER	pPaper;	/* ptr to the array of paper metrics */
	short	j, margins, result = ERROR;
	BOOL	fLandscape;

	ASSERT(lpdv);
	ASSERT(lpbIn);


	pPrinter = GetPrinter(lpdm->iPrinter);

	if (!pPrinter) {
		result = ERROR;
		goto END_NOFREE_PRINTER;
	}

	pPaper = GetPaperMetrics(pPrinter, lpdm);

	if (!pPaper) {
		result = ERROR;
		goto END_NOFREE_PAPER;
	}

	j = SearchPaperMetrics(pPaper, lpbIn, &margins, &fLandscape, lpdv->iRes, lpdm);

	if (j == NOTFND) {
		result = NOTFND;
		goto END;
	}

	lpdv->marginState = lpdm->marginState = margins;
	lpdv->paper.iPaper = lpdm->dm.dmPaperSize = pPaper[j].iPaper;
	lpdm->rgiPaper[pPrinter->defFeed] = pPaper[j].iPaper;
	lpdv->fLandscape = lpdm->dm.dmOrientation == DMORIENT_LANDSCAPE;

	lpdv->paper.cxPage = fLandscape ? 
	    Scale(pPaper[j].cyPage, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cxPage, lpdv->iRes, DEFAULTRES);

	lpdv->paper.cyPage = fLandscape ? 
	    Scale(pPaper[j].cxPage, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cyPage, lpdv->iRes, DEFAULTRES);

	switch (lpdv->marginState) {
	case ZERO_MARGINS:
		lpdv->paper.cxMargin = 0;
		lpdv->paper.cyMargin = 0;
		lpdv->paper.cxPaper = fLandscape ?
		    Scale(pPaper[j].cyPaper, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cxPaper, lpdv->iRes,
		     DEFAULTRES);
		lpdv->paper.cyPaper = fLandscape ?
		    Scale(pPaper[j].cxPaper, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cyPaper, lpdv->iRes,
		     DEFAULTRES);
		break;
	case DEFAULT_MARGINS:
		lpdv->paper.cxMargin = fLandscape ?
		    Scale(pPaper[j].cyMargin, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cxMargin, lpdv->iRes,
		     DEFAULTRES);
		lpdv->paper.cyMargin = fLandscape ?
		    Scale(pPaper[j].cxMargin, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cyMargin, lpdv->iRes,
		     DEFAULTRES);
		lpdv->paper.cxPaper = fLandscape ?
		    Scale(pPaper[j].cyPaper, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cxPaper, lpdv->iRes,
		     DEFAULTRES);
		lpdv->paper.cyPaper = fLandscape ?
		    Scale(pPaper[j].cxPaper, lpdv->iRes, DEFAULTRES) : Scale(pPaper[j].cyPaper, lpdv->iRes,
		     DEFAULTRES);
		break;
	}
	result = 1;
END:

	LocalFree((HANDLE)pPaper);
END_NOFREE_PAPER:
	FreePrinter(pPrinter);
END_NOFREE_PRINTER:
	return result;
}


/*
 * StartNewPage()
 *
 * Called when the driver is getting ready to write output to a new page.
 * Any per page initialization should be placed here.
 *
 * Return value is currently undefined.
 *
 */

int FAR PASCAL StartNewPage(LPDV lpdv)
{
    /* output page comment and save state */
    ++lpdv->iPageNumber;

    if (!lpdv->fDoEps)
        PrintChannel(lpdv, "%%%%Page: %d %d\n%%%%PageResources: (atend)\n", lpdv->iPageNumber,
                    lpdv->iPageNumber);

    if (!lpdv->bDSC && lpdv->dwCurVM > lpdv->dwMaxVM) {
        PrintChannel(lpdv, "SVDoc restore /SVDoc save def\n");
        TTFlushFonts(lpdv);
        lpdv->lidFont = -1;
        lpdv->dwCurVM = 0L;
        lpdv->DLState = DL_NOTHING;
    }

#if 0
    PrintChannel(lpdv, "%ld (estVM:%ld  actVM:) print vmstatus pop dup /dbstr 20 string def dbstr cvs print ( diff:) print exch pop sub dbstr cvs print (\n) print flush\n",
                    lpdv->dwCurVM, lpdv->dwCurVM);
#endif

    PrintChannel(lpdv, "SS\n");

    /* set custom page size (if necessary) */
    if (lpdv->paper.iPaper == DMPAPER_USER)
        PrintChannel(lpdv, "{statusdict begin %d %d 1 setpage end} stopped pop\n",
                     lpdv->paper.cxPaper, lpdv->paper.cyPaper);

    /* set the paper metrics */
	
    if(lpdv->fLandscape)
    {
        int orient, yMargin;

        orient = lpdv->DevMode.LandscapeOrient;

        if(orient == 270)
        {
            yMargin = lpdv->absPaper.cyMargin;
            //  size of top unprintable margin.
        }
        else
        {
            orient = 90;
            yMargin = lpdv->absPaper.cyPaper -  
                lpdv->absPaper.cyPage - lpdv->absPaper.cyMargin;
                //  yMargin  now represets the bottom unprintable
                //  margin.

        }
        PrintChannel(lpdv, "%d %d %d %d %d %d %d SM\n",
            lpdv->bMirror,
	        orient,
	        lpdv->absPaper.cxMargin,
            yMargin,
	        lpdv->absPaper.cxPage,
	        lpdv->absPaper.cyPaper,
	        lpdv->iRes);
    }
    else
    {
        PrintChannel(lpdv, "%d %d %d %d %d %d %d SM\n",
            lpdv->bMirror,
	        lpdv->fLandscape,
	        lpdv->absPaper.cxMargin,
	        lpdv->absPaper.cyMargin,
	        lpdv->absPaper.cxPage,
	        lpdv->absPaper.cyPaper,
	        lpdv->iRes);
    }

    /* if we are doing a negative image start with a black background */
    if (!lpdv->bDSC && lpdv->bNegImage) 
    {
       if (lpdv->fDoEps)
       {
            // let EpsRect determine bounding box.
            // But we must first tranform it into the current
            // device cooridinate system.

            POINT  pointA, pointB;
            int    swap;

            // EpsRect will be in default Postscript coords.
	        pointB.y = lpdv->EpsRect.top    ;
	        pointA.y = lpdv->EpsRect.bottom ;
	        pointB.x = lpdv->EpsRect.right  ;
	        pointA.x = lpdv->EpsRect.left   ;

            DefaultPSToDevice(lpdv, (LPPOINT)&pointA);
            DefaultPSToDevice(lpdv, (LPPOINT)&pointB);

            if(pointA.y > pointB.y)  // need to swap
            {
                swap = pointA.y;
                pointA.y = pointB.y;
                pointB.y = swap;
            }
            if(pointA.x > pointB.x)  // need to swap
            {
                swap = pointA.x;
                pointA.x = pointB.x;
                pointB.x = swap;
            }


            OpaqueBox(lpdv, FALSE, pointA.x, pointA.y,
                    pointB.x - pointA.x,
                    pointB.y - pointA.y,
                    RGB(255,255,255) /* need white to produce black */
                   );
       }
       else
          OpaqueBox(lpdv, FALSE, 0, 0,  //  (0,0) is inside the margins
                    lpdv->paper.cxPage, lpdv->paper.cyPage,
                    RGB(255,255,255) /* need white to produce black */
                   );
    }

    /* Empty the page resources string list */
    EmptyStringList(lpdv->slPageResources);

    /* reset DL status */
    if (lpdv->bPerPage)
        lpdv->DLState = DL_NOTHING;

    /* restore all graphic states to ground */
    lpdv->iCurLineCap = -1;
    lpdv->iCurLineJoin = -1;
    lpdv->iCurMiterLimit = -1;

    return 0;
}





/* ------------------------------------------------
   this function transforms one point in the device (apps)
   coordinate system (as defined by bMirror, fLandscape, iRes, margins)
   to the default Postscript coordinate system (origin at lower 
   left hand corner of page, units are 1/72 ").
   ----------------------------------------------------*/


void FAR PASCAL DeviceToDefaultPS(lpdv, lpPoint)
LPDV lpdv;
LPPOINT  lpPoint;
{
    int  iRes, dx, dy, orient;

    orient = lpdv->DevMode.LandscapeOrient;

    iRes = lpdv->iRes;

    // scale input point to 100ths of an inch
    // all intermediate computations will use this unit.
    dx = Scale(lpPoint->x, 100, iRes);
    dy = Scale(lpPoint->y, 100, iRes);

    // all dimensions in absPaper are in 1/100ths of an inch

    if(!lpdv->fLandscape)
    {
        lpPoint->y = lpdv->absPaper.cyPaper - lpdv->absPaper.cyMargin - dy;
        if(lpdv->bMirror)
            lpPoint->x = lpdv->absPaper.cxMargin + lpdv->absPaper.cxPage - dx;
        else
            lpPoint->x = dx + lpdv->absPaper.cxMargin;
    }
    else if(orient == 270)
    {
        lpPoint->y = lpdv->absPaper.cyPaper - lpdv->absPaper.cyMargin - dx;
        if(lpdv->bMirror)
            lpPoint->x = dy + lpdv->absPaper.cxMargin;
        else
            lpPoint->x = lpdv->absPaper.cxMargin + lpdv->absPaper.cxPage - dy;
    }
    else  // (90 deg landscape)
    {
        lpPoint->y = dx + lpdv->absPaper.cyPaper - lpdv->absPaper.cyMargin
            - lpdv->absPaper.cyPage;
        if(lpdv->bMirror)
            lpPoint->x = lpdv->absPaper.cxMargin + lpdv->absPaper.cxPage - dy;
        else
            lpPoint->x = dy + lpdv->absPaper.cxMargin;
    }

    //  scale to default unit size of 1/72 "
    lpPoint->x = Scale(lpPoint->x, 72, 100);
    lpPoint->y = Scale(lpPoint->y, 72, 100);
    return;
}


/* ------------------------------------------------
   this function transforms one point in the
   default Postscript coordinate system to the device (apps)
   coordinate system
   ----------------------------------------------------*/

void FAR PASCAL DefaultPSToDevice(lpdv, lpPoint)
LPDV lpdv;
LPPOINT  lpPoint;
{
    int  iRes, dx, dy, orient;

    orient = lpdv->DevMode.LandscapeOrient;

    iRes = lpdv->iRes;

    // scale input point to 100ths of an inch
    // all intermediate computations will use this unit.
    dx = Scale(lpPoint->x, 100, 72);
    dy = Scale(lpPoint->y, 100, 72);

    // all dimensions in absPaper are in 1/100ths of an inch

    if(!lpdv->fLandscape)
    {
        lpPoint->y = lpdv->absPaper.cyPaper - lpdv->absPaper.cyMargin - dy;
        if(lpdv->bMirror)
            lpPoint->x = lpdv->absPaper.cxMargin + lpdv->absPaper.cxPage - dx;
        else
            lpPoint->x = dx - lpdv->absPaper.cxMargin;
    }
    else if(orient == 270)
    {
        lpPoint->x = lpdv->absPaper.cyPaper - lpdv->absPaper.cyMargin - dy;
        if(lpdv->bMirror)
            lpPoint->y = dx - lpdv->absPaper.cxMargin;
        else
            lpPoint->y = lpdv->absPaper.cxMargin + lpdv->absPaper.cxPage - dx;
    }
    else  // (90 deg landscape)
    {
        lpPoint->x = dy + lpdv->absPaper.cyMargin + lpdv->absPaper.cyPage
            -  lpdv->absPaper.cyPaper;
        if(lpdv->bMirror)
            lpPoint->y = lpdv->absPaper.cxMargin + lpdv->absPaper.cxPage - dx;
        else
            lpPoint->y = dx - lpdv->absPaper.cxMargin;
    }

    //  scale to device unit size of 1/iRes "
    lpPoint->x = Scale(lpPoint->x, iRes, 100);
    lpPoint->y = Scale(lpPoint->y, iRes, 100);
    return;
}


