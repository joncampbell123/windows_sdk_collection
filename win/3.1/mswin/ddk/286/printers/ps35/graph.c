/**[f******************************************************************
 * graph.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/***************************************************************************
 * ??Jan87	sjp		Fixed box,ellipse and arc drawing.
 * 12Jun87	sjp		Started round rect support.
 * 18Jun87	sjp		Finished round rect support.
 ***************************************************************************/

#include "pscript.h"
#include "channel.h"
#include "utils.h"
#include "debug.h"
#include "graph.h"
#include "mgesc.h"
#include "gdidm.h"
#include "driver.h"
#include "psdata.h"

// postscript is limited to 1500 polygon points.  each x and y of a moveto
// is one (2 per pair).  for a curveto each x,y is 2 (4 per pair).

#define PATH_LIMIT	1200

extern char MoveCmd;
extern int XformLevel;

/* note: these defs are duplicated in the postscript code
 * that does the filling. */

#define FM_NONE		0x0000	/* do no filling */
#define FM_FILL		0x0100	/* fill with fill color */
#define FM_HATCH	0x0200	/* do hatch (possibly over fill) */
#define FM_PATTERN	0x0400	/* do pattern (possibly over fill) */




typedef int	FAR *LPINT;

#define RGB_WHITE 0x00FFFFFFL
#define RGB_BLACK 0x00000000L
#define CODEFAULT RGB_WHITE	/* The default RGB color */


/* these include only pure colors (assuming a 3 plane device like the
 * QMS 100).  Enumerating non pure colors gives things like
 * narrow pens uglyness and some apps depend on the fact that pens
 * only come in pure colors. */

#define MAX_GRAYS 2
DWORD grGrays[MAX_GRAYS] = {
	RGB_BLACK,
	RGB_WHITE
};

/* all pure colors (assuming a 3 plane device like the QMS 100)
 * (less black and white which are in the grays) */

#define MAX_COLORS 6
DWORD grColors[MAX_COLORS] = {
	0x000000FF,		/* pure red */
	0x0000FF00,		/* pure green */
	0x00FF0000,		/* pure blue */
	0x0000FFFF,		/* red green */
	0x00FFFF00,		/* green blue */
	0x00FF00FF		/* blue red */
};


/*-------------------------- local functions -----------------------------*/


short	PASCAL	PenWidth(LPPEN);
BOOL	PASCAL	Polygon(LPDV, int, BOOL, LPPOINT, int);
BOOL	PASCAL	SetPattern(LPDV, LPBR);
DWORD	PASCAL	RGBtoGray(DWORD rgb);
void	PASCAL	CurveToChannel(LPDV, LPPOINT, int);
void	PASCAL	PointsToChannel(LPDV, LPPOINT, int);


void FAR PASCAL FillDraw(LPDV lpdv, BOOL Fill, BOOL Draw, int style)
{

	if (Fill && (lpdv->FillMode != FM_NONE)) {
		PrintChannel(lpdv, "%d F\n", style);
	}

	if (Draw && (lpdv->pen.lopn.lopnStyle != LS_NOLINE)) {
		PrintChannel(lpdv, "S\n");
	}
	if (Fill || Draw)
		PrintChannel(lpdv, "n\n");	/* newpath */
}



/* added by Aldus Corporation--22 Jan 87--sjp */


/****************************************************************
 * Name: EnumObj()
 *
 * Action: This routine is used to enumerate pens and brushes
 *	  available on the device.  For each object belonging
 *	  to the given style (pen or brush), the callback function
 *	  is called with the information for that object.  The
 *	  callback function is called until there are no more
 *	  objects or until the callback function returns zero.
 *
 *****************************************************************/

BOOL FAR PASCAL EnumObj(lpdv, iStyle, lpfn, lpb)
LPDV	lpdv;	    /* Far ptr to the device descriptor */
int	iStyle;	    /* The style (brush or pen) */
FARPROC	lpfn;	    /* Far ptr to the callback function */
LPSTR	lpb;	    /* Far ptr to the client data (passed to callback) */
{

	LOGPEN lopn;	/* The logical pen */
	LOGBRUSH lb;	/* The logical brush */
	int	i;

	ASSERT(lpdv);
	ASSERT(lpfn);
	ASSERT(iStyle == OBJ_PEN || iStyle == OBJ_BRUSH);

	DBMSG((">EnumObj()\n"));

	switch (iStyle) {
	case OBJ_PEN:
		DBMSG((">EnumObj() OBJ_PEN\n"));
		lopn.lopnWidth.x = lopn.lopnWidth.y = 1;

		/* Enumerate each pen style for all possible pen colors */
		for (lopn.lopnStyle = 0; lopn.lopnStyle < MaxLineStyle; ++lopn.lopnStyle) {

			for (i = 0; i < MAX_GRAYS; ++i) {
				lopn.lopnColor = grGrays[i];

				if (!(*lpfn)((LPLOGPEN)&lopn, lpb))
					goto DONE;
			}

			if (lpdv->fColor) {
				for (i = 0; i < MAX_COLORS; ++i) {
					lopn.lopnColor = grColors[i];

					if (!(*lpfn)((LPLOGPEN)&lopn, lpb))
						goto DONE;
				}
			}
		}
		break;

	case OBJ_BRUSH:
		DBMSG((">EnumObj() OBJ_BRUSH:\n"));

		lb.lbBkColor = RGB_WHITE;

		/* Enumerate the hollow brush */
		lb.lbStyle = BS_HOLLOW;
		lb.lbColor = RGB_WHITE;
		/* lb.lbBkColor = RGB_WHITE; not used */
		lb.lbHatch = 0;

		DBMSG((">EnumObj() BS_HOLLOW:\n"));

		if ((!(*lpfn)((LPLOGBRUSH) &lb, lpb)))
			goto DONE;

		/* Enumerate all possible hatch brushes for black on white */
		lb.lbStyle = BS_HATCHED;
		lb.lbColor = RGB_BLACK;
		/* lb.lbBkColor = RGB_WHITE; not used */

		DBMSG((">EnumObj() BS_HATCHED:\n"));

		for (lb.lbHatch = 0; lb.lbHatch <= MaxHatchStyle; ++lb.lbHatch)
			if (!(*lpfn)((LPLOGBRUSH) &lb, lpb))
				goto DONE;

		/* Enumerate all the possible colors for a solid brush */
		lb.lbStyle = BS_SOLID;

		DBMSG((">EnumObj() BS_SOLID (grays):\n"));

		for (i = 0; i < MAX_GRAYS; ++i) {
			lb.lbColor = grGrays[i];
			if (!(*lpfn)((LPLOGBRUSH)&lb, lpb))
				goto DONE;
		}

		if (lpdv->fColor) {

			DBMSG((">EnumObj() BS_SOLID (colors):\n"));

			for (i = 0; i < MAX_COLORS; ++i) {
				lb.lbColor = grColors[i];
				if (!(*lpfn)((LPLOGBRUSH)&lb, lpb))
					goto DONE;
			}
		}

#if 0
		/* only do these if we support special hatch brush styles */

		/* enum patterned brushes */

		lb.lbStyle = BS_PATTERN;

		DBMSG((">EnumObj() BS_PATTERN (grays):\n"));

		for (i = 0; i < MAX_GRAYS; ++i) {
			lb.lbColor = grGrays[i];
			if (!(*lpfn)((LPLOGBRUSH)&lb, lpb))
				goto DONE;
		}
#endif

		if (lpdv->fColor) {

			DBMSG((">EnumObj() BS_PATTERN (colors):\n"));

			for (i = 0; i < MAX_COLORS; ++i) {
				lb.lbColor = grColors[i];
				if (!(*lpfn)((LPLOGBRUSH)&lb, lpb))
					goto DONE;
			}
		}

		break;
	}

	/* Control comes here if all callbacks returned TRUE */
	return TRUE;
DONE:
	return FALSE;
}


/**********************************************************
* Name: PenWidth()
*
* Action: Calculate the PostScript pen width from the pen
*	  X and Y dimensions that GDI specifies in the
*	  logical pen (GDI only passes X down, Y is ignored).
*
* to avoid problem with clipping a zero width pen we make
* the min width 1.  (zero width pen in PS is defined as
* the thinest line possible).  Zero width pens clip randomly
* (behave differen in protrat vs landscape)
*
*
***********************************************************/

short PASCAL PenWidth(LPPEN lppen)
{
	if (lppen->lopn.lopnWidth.x == 0)
		return 1;
	else
		return lppen->lopn.lopnWidth.x;
}





/***********************************************************
 * Name: PointsToChannel()
 *
 * Action: Pass a number of points to PostScript by printing
 *	  them on the output channel.
 *
 ************************************************************/

void PASCAL PointsToChannel(lpdv, lppt, cpt)
LPDV	lpdv;
LPPOINT lppt;
int	cpt;
{
	int	i;
	int	cptT;		/* temp point count */
	LPPOINT lpptT;		/* temp point pointer */
		

	/* M == moveto path start */

	PrintChannel(lpdv, "%d %d %c ", lppt->x, lppt->y, MoveCmd);
	++lppt;
	--cpt;


	while (cpt > 0) {
		cptT = (cpt > 20) ? 20 : cpt;
		lpptT = lppt + cptT;

		/* Push the points onto the PostScript stack in reverse order */
		for (i = 0; i < cptT; ++i) {
			--lpptT;
			PrintChannel(lpdv, "%d %d ",
				lpptT->x - (lpptT-1)->x,
				lpptT->y - (lpptT-1)->y);
		}

		/* PP == PolyPoints */
		PrintChannel(lpdv, "%d PP\n", cptT);
		cpt -= cptT;
		lppt += cptT;
	}
}


/***********************************************************
 * Name: CurveToChannel()
 *
 * this is used by the MG SET_POLY_MODE escape.
 *
 * it seems that designer will not use this escape unless
 * all of the other exotic MG escapes have been
 * implemented.  if thats the way they want to be...
 *
 ************************************************************/


void PASCAL CurveToChannel(lpdv, lppt, cpt)
LPDV	lpdv;
LPPOINT lppt;
int	cpt;
{
	int	i;
	POINT	pt0;		/* x0, y0 for rcurveto stuff */

	ASSERT(cpt);		/* must be some points */
		
	/* M == moveto path start */

	PrintChannel(lpdv, "%d %d %c ", lppt->x, lppt->y, MoveCmd);
	lppt++;
	cpt--;


	while (cpt >= 3) {

		pt0 = *(lppt-1);

		for (i = 0; i < 3; i++) {
			PrintChannel(lpdv, "%d %d ",
				lppt->x - pt0.x,
				lppt->y - pt0.y);
			lppt++;
		}

		PrintChannel(lpdv, "rct\n");
		cpt -= 3;
	}

	if (cpt == 1)
		PrintChannel(lpdv, "%d %d rlt\n",
			lppt->x - (lppt-1)->x,
			lppt->y - (lppt-1)->y);
	else if (cpt == 2) {
		pt0 = *(lppt-1);
		PrintChannel(lpdv, "%d %d %d %d %d %d rct\n",
			lppt->x - pt0.x,
			lppt->y - pt0.y,
			lppt->x - pt0.x,
			lppt->y - pt0.y,
			(lppt+1)->x - pt0.x,
			(lppt+1)->y - pt0.y);
	}

}


/****************************************************************************
 * Polygon() and Polyline()
 *
 * Action: Draw a polygon on the display surface.  The perimiter
 *	  of the polygon is stroked with the current pen and
 *	  the interior is filled with the current brush.
 *
 * new strategy (borrowed from MGXPS).  we will use rlineto instead
 * of lineto because the delta # are smaller than absolute #.
 *
 * returns:
 *	1	success
 *	0	failure
 *	-1	request GDI to simulate. 
 *		NOTE: GDI simulation produces tons of data. this is very bad
 *		so avoid at all costs.
 *
 ****************************************************************************/

BOOL PASCAL Polygon(lpdv, FillStyle, bFill, lppt, cpt)
LPDV	lpdv;		/* A far ptr to the device descriptor */
int	FillStyle;	/* The filling type (if Polygon) */
BOOL	bFill;		/* fill or not (this is a polygon) */
LPPOINT lppt;		/* A far ptr to an array of points on the perimeter */
int	cpt;		/* The number of points */
{
	int point_limit;

	ASSERT(lpdv);
	ASSERT(lppt);

	// we have to punt if we are defining a path larger than the PS limit

	if (lpdv->PolyMode == PM_BEZIER)
		point_limit = PATH_LIMIT / 4;	// curveto takes 4 per point
	else
		point_limit = PATH_LIMIT / 2;	// moveto takes 2

	// for fills with patterns or hatches we reduce the number of points
	// by 2 because we have to create a clipping path as well as
	// a filling path.

	if (bFill && (lpdv->FillMode & (FM_PATTERN | FM_HATCH)))
		point_limit /= 2;

	if (cpt >= point_limit) {

		DBMSG(("Too many points, make GDI simulate!\n"));
		return FALSE;	// make GDI simulate
	}

	DBMSG(("point_limit %d  num points %d\n", point_limit, cpt));

	// ok, we can do this

	if (lpdv->PolyMode == PM_BEZIER)
		CurveToChannel(lpdv, lppt, cpt);
	else
		PointsToChannel(lpdv, lppt, cpt);


	if (lpdv->PathLevel <= 0)
		FillDraw(lpdv, bFill, TRUE, FillStyle == OS_ALTPOLYGON);

	return TRUE;
}



/***************************************************************
* Name: Output()
*
* Action: This routine draws geometric shapes on the display
*	  surface.  The output style determines the type of
*	  shape to be drawn and whether or not it should be
*	  filled, etc.
*
* Returns: 1 = success
*	   0 = failure for any reason
*	  -1 = request GDI to simulate the output primitive
*
* note:
*	we do everything GDI asks or pass things on the the brute
*	routines (dmBitBlt etc).
*
*****************************************************************/

int FAR PASCAL Output(lpdv, iStyle, cpt, lppt, lppen, lpbr, lpdm, lprcClip)
LPDV	lpdv;		/* Far ptr to the device descriptor */
int	iStyle;		/* The output style (rectangle, elipse, etc) */
int	cpt;		/* The count of points in the point list */
LPPOINT lppt;		/* Far ptr to a list of points */
LPPEN	lppen;		/* Far ptr to the pen to use */
LPBR	lpbr;		/* Far ptr to the brush to use */
LPDRAWMODE lpdm;	/* Far ptr to the draw mode (bacground color, etc.) */
LPRECT	lprcClip;	/* Far ptr to the clipping rectangle */
{
	int	cxPen;	    /* The pen width */
	int	cyPen;	    /* The pen height */
	int	iy;	    /* The vertical position */
	int	i, x, y;
	BOOL	Draw;		/* draw with the pen */
	BOOL	Fill;		/* fill with brush */
	int	x_center, y_center;
	int	x0, y0, x1, y1, x2, y2, x3, y3;


	ASSERT(lpdv);
	DBMSG1((">Output(): iStyle=%d cpt=%d\n", iStyle, cpt));

	if (!lpdv->iType) {
		return dmOutput(lpdv, iStyle, cpt, lppt, lppen, lpbr, lpdm, lprcClip);
	}

#ifdef PS_IGNORE
	if (lpdv->fSupressOutput)
		return TRUE;
#endif

	/* if lpdv->PathLevel > 0 then we are inside a BEGIN_PATH escape.
	 * in this mode we do not do output until the END_PATH
	 * escape is seen */

	if (lpdv->PathLevel <= 0) {

		/* not inside a PATH escape. do everything normally */

		/* Select the pen if it exists */
		if (lppen) {
			/* Potential problem area?  The round pen flag is set only
		 	* if NOT rectangle.  With round rects of very small radius
		 	* corners this could be a problem.  However, a point of
		 	* interest is with a square pen drawing a round corner the
		 	* cross section of the radius could be up to sqrt(2) times
		 	* the cross section of the vertical or horizontal pieces.
		 	*/
			lppen->fRound = iStyle != OS_RECTANGLE;
			cxPen = lppen->lopn.lopnWidth.x;
			cyPen = lppen->lopn.lopnWidth.y;

			if (iStyle == OS_SCANLINES) {
				/* Scan-lines must use a 1 X 1 pixel pen */
				lppen->lopn.lopnWidth.x = 1;
				lppen->lopn.lopnWidth.y = 1;
				SelectPen(lpdv, lppen);
				lppen->lopn.lopnWidth.x = cxPen;
				lppen->lopn.lopnWidth.y = cyPen;
			} else {
				SelectPen(lpdv, lppen);
			}
			Draw = TRUE;	/* default: draw all outlines */
		} else {

			/* does lppen == NULL imply NULL pen??
		 	 * the header sets up the default pen as NULL (NOLINE style) */

			cxPen = 0;
			cyPen = 0;
			Draw = FALSE;	/* no pen, don't draw */
		}

		if (iStyle != OS_POLYLINE) {	/* polyline does not use a brush */

			SelectBrush(lpdv, lpbr, lpdm);
		}

		/* note, this does a "gsave" */
		/* be sure to select pens and brushes before we do this */

		if (!XformLevel && lprcClip)
			ClipBox(lpdv, lprcClip);

		Fill = TRUE;	/* default: fill all objects */
	}

	switch (iStyle) {

	case OS_ARC:

		DBMSG1((" Output(): OS_ARC (pie)\n"));
		Fill = FALSE;

                /* PostScript & GDI have differing ideas about what an arc
                 * with starting and ending endpoints looks like.  PostScript
                 * says it is an empty circle; postscript says it's a 
                 * complete circle.  So, if the endpoints match turn output
                 * call into an ellipse call.
                 */
                if (lppt[2].x == lppt[3].x && lppt[2].y == lppt[3].y) {
                        iStyle = OS_ELLIPSE;
                        goto do_ellipse;
                }

		/* fall through... */

	case OS_PIE:
		DBMSG1((" Output(): OS_PIE\n"));

		x0 = lppt->x;
		y0 = lppt->y;

		x1 = (lppt+1)->x;
		y1 = (lppt+1)->y;

		x2 = (lppt+2)->x;
		y2 = (lppt+2)->y;

		x3 = (lppt+3)->x;
		y3 = (lppt+3)->y;

		x_center = (x0 + x1) / 2;
		y_center = (y0 + y1) / 2;

		if (x3 == x_center && y3 == y_center) {
			lppt->x = x_center;
			lppt->y = y_center;
			(lppt+1)->x = x2;
			(lppt+1)->y = y2;
			goto DO_RECT;
		}

		if (x2 == x_center && y2 == y_center) {
			lppt->x = x_center;
			lppt->y = y_center;
			(lppt+1)->x = x3;
			(lppt+1)->y = y3;
			goto DO_RECT;
		}


		// take care of degenerate cases

		if (x0 == x1) {
			if (y2 <= y3) {
				x2 = x_center + 1;	// 0 to 90
				y2 = y_center;
				x3 = x_center;
				y3 = y_center - 1;
			} else {
				x2 = x_center;		// 270 to 360
				y2 = y_center + 1;
				x3 = x_center + 1;
				y3 = y_center;
			}
		}

		if (y0 == y1) {
			if (x2 <= x3) {
				x2 = x_center;		// 90 to 180
				y2 = y_center - 1;
				x3 = x_center - 1;
				y3 = y_center;
			} else {
				x2 = x_center + 1;	// 0 to 90
				y2 = y_center;
				x3 = x_center;
				y3 = y_center - 1;
			}
		}

		PrintChannel(lpdv, "0 0 1 %d %d %d %d %d %d %d %d %c\n",
			y2 - y_center, 
			x2 - x_center, 
			y3 - y_center, 
			x3 - x_center,
			(x1 - x0 + 1) / 2,	// scale
			(y1 - y0 + 1) / 2,	// scale
			x_center, y_center,
			iStyle == OS_ARC ? 'A' : 'P');
		break;

	case OS_ELLIPSE:
do_ellipse:
		DBMSG1((" Output(): OS_ELLIPSE\n"));

#if 1
		// do this like this for now.
        // this bug's for you Aldus.

		x = (lppt->x)/2 + (lppt+1)->x / 2;
		y = (lppt->y)/2 + (lppt+1)->y / 2;

		PrintChannel(lpdv, "%d %d %d %d E\n",
			(lppt+1)->x/2 - lppt->x/2, 
            (lppt+1)->y/2 - lppt->y/2, /* dx dy */
			x, y);	/* x y */
#else
		// there is matching code for this in the header (commented out)
		// that does ellipse with a bit more accuracy, avoiding the
		// rounding in the integer math (above)

		PrintChannel(lpdv, "%d %d %d %d E\n",
			lppt->x, lppt->y,
			(lppt+1)->x, (lppt+1)->y);
#endif
		break;

	case OS_SCANLINES:

		Fill = FALSE;
		Draw = FALSE;	/* done inside SL */

		DBMSG1((" Output(): OS_SCANLINES\n"));

		if (--cpt > 0) {
			iy = (lppt++)->y;

			/* Loop once for each line segment */
			for (i = 0; i < cpt; ++i) {

				if (lpbr) {
					// do scanline with the brush
					// create a box to fill
					PrintChannel(lpdv, "%d .5 %d %d B 1 F n\n", 
						lppt->y - lppt->x, 
						lppt->x, iy);
				} else {
					// do scanline with the pen
					// draw a line
					PrintChannel(lpdv, "%d %d M %d 0 rlt S n\n", 
						lppt->x, iy,
						lppt->y - lppt->x);
				}

				++lppt;
			}
		}
		break;

	case OS_RECTANGLE:

DO_RECT:

		DBMSG1((" Output(): OS_RECTANGLE\n"));

		if (lpbr && (lpbr->lb.lbStyle == BS_PATTERN || 
		             lpbr->lb.lbStyle == BS_HATCHED)) 
        {

			// apply the same trick here as with scan lines
			// for hatched and pattern things we clip to the
			// path we define.  this means it has to have
			// some finite width.  so we add .5 to all the dx dy
			// parts of the box  
            // due to bug 13470 we must ensure min dimensions
            // of box at least 1.0
            
            int  dx, dy;

            dx = (lppt+1)->x - lppt->x - 1;
            dy = (lppt+1)->y - lppt->y - 1;

            if(dx < 1)
                dx = 1;
            if(dy < 1)
                dy = 1;
            
			PrintChannel(lpdv, "%d %d %d %d B\n",
				dx, dy, lppt->x, lppt->y);

		} else {
			PrintChannel(lpdv, "%d %d %d %d B\n",
				(lppt+1)->x - lppt->x - 1, (lppt+1)->y - lppt->y - 1,
				lppt->x, lppt->y);
		}

		break;

	case OS_ROUNDRECT:

		DBMSG1((" Output(): OS_ROUNDRECT\n"));

		PrintChannel(lpdv, "%d %d %d %d %d %d RR\n",
			lppt->x, lppt->y,			/* ul corner */
			(lppt+1)->x, (lppt+1)->y,		/* lr corner */
			(lppt+2)->x / 2, (lppt+2)->y / 2);	/* x y dim radii */
		break;

	case OS_POLYLINE:
	case OS_ALTPOLYGON:	/* eofill */
	case OS_WINDPOLYGON:	/* fill */

		DBMSG1((" Output(): OS_POLYLINE\n"));
		DBMSG1((" Output(): OS_POLYGON: cpt=%d\n", cpt));

		Fill = FALSE;	/* NOTE: fill and stroke are performed */
		Draw = FALSE;	/* inside Polygon() and Polyline() */

		if (!Polygon(lpdv, iStyle, iStyle != OS_POLYLINE, lppt, cpt)) {
			if (lprcClip)
				ClipBox(lpdv, NULL);
			return -1;	// GDI simulate!
		}
		break;

	default:
		Draw = FALSE;
		Fill = FALSE;
	}


	/* if lpdv->PathLevel > 0 then we are inside a BEGIN_PATH escape.
	 * in this mode we do not do output until the END_PATH
	 * escape is seen */

	if (lpdv->PathLevel <= 0) {

		FillDraw(lpdv, Fill, Draw, 1);

		if (!XformLevel && lprcClip)
			ClipBox(lpdv, NULL);
	} else {

		/* when inside a path we redfine the move operator (M) to
		 * connect paths (using lineto) instead of moving to
		 * a new point (and starting a new subpath). */

		if (MoveCmd == 'M') {
			MoveCmd = 'L';
		}
	}

	return TRUE;
}



/************************************************************
* Name: Pixel()
*
* Action: This routine sets or retrieves the color of a given
*	  pixel.
*
**************************************************************/

CO FAR PASCAL Pixel(lpdv, ix, iy, co, lpdm)
LPDV	lpdv;	    /* Far ptr to the device descriptor */
int	ix; 	    /* The horizontal device coordinate */
int	iy; 	    /* The vertical device coordinate */
CO	co; 	    /* The physical color */
LPDRAWMODE lpdm;    /* ptr to DRAWMODE (includes raster op, etc.) */
{
	ASSERT(lpdv);

	if (!lpdv->iType)
		return dmPixel(lpdv, ix, iy, co, lpdm);

#ifdef PS_IGNORE
	if (lpdv->fSupressOutput)
		return 1;
#endif
	if (lpdm) {
		#define R2_NOP 11	// No Op, just move the current position

		if (lpdm->Rop2 == R2_NOP) {
			PrintChannel(lpdv, "%d %d M\n", ix, iy);
		} else {

			PrintChannel(lpdv, "%d %d %d sc %d %d M %d %d L st\n",
		    	GetRComponent(lpdv, co),
		    	GetGComponent(lpdv, co),
		    	GetBComponent(lpdv, co),
		    	ix, iy, ix + 1, iy);
		}
	} else
		co = CODEFAULT;

	return co;
}





/*******************************************************************
* Name: ScanLR()
*
* Action: This routine is used to scan a single horizontal scan line
*	  on the device's display surface for a pixel having ( or not
*	  having a given color.  The direction of scan can be left or
*	  right from the specified starting position.
*
* Note: We can't support this call for Postscript since the
*	bitmap is down in the printer and there is no way to access it.
*	So the best thing that we can do is return the starting position
*	as the location of the boundary. This will cause flood-fill to
*	generally do the wrong thig, but this is one of those ugly facts
*	of life.
*
*********************************************************************/

int	FAR PASCAL ScanLR(lpdv, ix, iy, co, iStyle)
LPDV	lpdv;	    /* Far ptr to the device descriptor */
int	ix; 	    /* The starting horizontal device coordinate */
int	iy; 	    /* The starting vertical device coordinate */
CO	co; 	    /* The physical color to search for */
int	iStyle;	    /* The direction of scan, etc. */
{
	if (!lpdv->iType)
		return dmScanLR(lpdv, ix, iy, co, iStyle);
	return (ix);	/* punt */
}



#if 0
/*****************************************************************
 * DWORD	PASCAL	RGBtoGray(long rgb);
 *
 * convert a logical RGB into a gray level RGB
 * 255 | 255 | 255 == White
 *               0 == Black
 *****************************************************************/

DWORD PASCAL RGBtoGray(DWORD rgb)
{
	WORD	ave;

	ave = INTENSITY(GetRComponent(lpdv, rgb),GetGComponent(lpdv, rgb),GetBComponent(lpdv, rgb));

	return (DWORD)RGB(ave, ave, ave);
}
#endif



/****************************************************************
 * DWORD FAR PASCAL ColorInfo(LPDV lpdv, DWORD rgb, LPCO lpco)
 *
 * convert between logical RGB color representation and the 
 * device's physical color representation and return closest color.
 *
 * in:
 *	rgb	logical RGB color to convert or get info on
 *	lpco	pointer to physical color to fill in if not NULL
 *
 * out:
 *	*lpco gets the nearest RGB (this actually goes into the
 *	DRAWMODE struct for future TextOut or SetPixel calls)
 *
 * return:
 *	the nearest solid color (that can be represented by a pixel)
 *	for black and white we convert to black and white, for
 *	color we keep the hi bit of each color component.
 *
 *******************************************************************/

DWORD FAR PASCAL ColorInfo(LPDV lpdv, DWORD rgb, LPCO lpco)
{
	if (!lpdv->iType)	/* is this our PDEVICE? */
		return dmColorInfo(lpdv, rgb, lpco);

	// are we converting logical to physical?

	if (lpco)
		*lpco = (CO)rgb;	// store pcolor (same as logical)

	
	if (lpdv->fColor) {
		DWORD color = RGB_BLACK;
		if (GetRComponent(lpdv, rgb) > 127)
			color |= 0xFF;
		if (GetGComponent(lpdv, rgb) > 127)
			color |= 0xFF00;
		if (GetBComponent(lpdv, rgb) > 127)
			color |= 0xFF0000;
		return color;
	} else {

		// mono black and white type device

		if (INTENSITY(GetRComponent(lpdv, rgb),GetGComponent(lpdv, rgb),GetBComponent(lpdv, rgb)) > 127)
			return RGB_WHITE;
		else
			return RGB_BLACK;
	}
}


/*********************************************************************
* Name: SelectBrush()
*
* Action: This routine is called to establish a new brush pattern by
*	  all output routines that use brushes (Output and BitBlt).
*
* This code is ugly because it tries to output only the minimal
* amount of information necessary to define the current brush.  that
* is it keeps track of what parts of the brush have already been
* defined and does not repeat downloading that data. 
***********************************************************************/

void FAR PASCAL SelectBrush(lpdv, lpbr, lpdm)
LPDV lpdv;
LPBR lpbr;		/* physical brush */
LPDRAWMODE lpdm;		/* has fg and bg color for patterns */
{
	LPSTR	lpb;
	DWORD	FillColor, HatPatColor;
	WORD	fm;		/* fill mode */
	int	i;

	ASSERT(lpdv);

	if (!lpbr || (lpbr->lb.lbStyle == BS_HOLLOW)) {
		lpdv->FillMode = FM_NONE;
		return;
	}

	switch (lpbr->lb.lbStyle) {
	case BS_SOLID:
		FillColor = lpbr->lb.lbColor;
		fm = FM_FILL;
		break;

	case BS_HATCHED:
		FillColor = lpbr->lb.lbBkColor;
		HatPatColor = lpbr->lb.lbColor;

		fm = FM_HATCH | lpbr->lb.lbHatch;

		if (lpdm->bkMode == OPAQUE)
			fm |= FM_FILL;
		break;


	case BS_PATTERN:

		// we should check the pattern for all 0s or 1s and
		// replace the pattern fill with a solid fill to speed
		// things up (pattern filling can be slow)
        {
            BOOL  SnowWhite = TRUE, PitchBlack = TRUE;
            WORD  i ;

            for(i = 0 ; i < sizeof(lpbr->rgbPat) ; i++)
            {
                if(lpbr->rgbPat[i] != 0xFF)
                    SnowWhite = FALSE;
                if(lpbr->rgbPat[i])
                    PitchBlack = FALSE;
                if(!(SnowWhite || PitchBlack))
                    break;
            }

            if(SnowWhite || PitchBlack)
            {
	            lpbr->lb.lbStyle = BS_SOLID;
        		fm = FM_FILL;

                if(SnowWhite)
            		FillColor = lpdm->bkColor;
                else  //  must then be PitchBlack
            		FillColor = lpdm->TextColor;

        		lpbr->lb.lbColor = FillColor & (DWORD)0x00ffffff;
                break;
            }
        }
		#if 0

		DBMSG(("SelectBrush(): fg %lx bk %lx\n",
			lpbr->lb.lbColor, lpbr->lb.lbBkColor));

		FillColor = lpbr->lb.lbBkColor;
		HatPatColor = lpbr->lb.lbColor;

		#else

		// use the currently active text and bk colors
		// for mono bitmaps (there is no color info in the bitmap)

		FillColor = lpdm->bkColor;
		HatPatColor = lpdm->TextColor;

		#endif

		fm = FM_PATTERN;

		// pattern brushes are always OPAQUE!

		// if (lpdm->bkMode == OPAQUE)
			fm |= FM_FILL;

		break;
	}

	/* this applies to cases that fill (solid, opaque pats and hatches) */

	/* is this the first time we have done a fill or is the fill color
	 * different */

	if ((fm & FM_FILL)  &&
	    (!(lpdv->FillMode & FM_FILL) || lpdv->FillColor != FillColor)) {
		PrintChannel(lpdv, "%d %d %d fC\n",
			GetRComponent(lpdv, FillColor),
			GetGComponent(lpdv, FillColor),
			GetBComponent(lpdv, FillColor));
		lpdv->FillColor = FillColor;      	/* save the color */
	}

	/* this applies to both pattern and hatch */

	if (fm & (FM_HATCH | FM_PATTERN)) {

		/* is this the first time we set hC or are the colors different */

		if (!(lpdv->FillMode & (FM_HATCH | FM_PATTERN)) ||
		    lpdv->HatchColor != HatPatColor) {

			PrintChannel(lpdv, "%d %d %d hC\n",
				GetRComponent(lpdv, HatPatColor),
				GetGComponent(lpdv, HatPatColor),
				GetBComponent(lpdv, HatPatColor));
			lpdv->HatchColor = HatPatColor;	/* save the color */

		}

	}

	/* update the filling mode if it has changed */

	if (lpdv->FillMode != fm)
		PrintChannel(lpdv, "/fm %d def\n", fm);

	/* update the fill pattern if:
	*	we are pattern filling now
	*	AND
	*	the last fillmode did not use a pattern (this gets the
	*	initial case)
	*	OR
	*	the the fill pattern has changed */

	if ((fm & FM_PATTERN) &&
	(!(lpdv->FillMode & FM_PATTERN) ||
	!lmemIsEqual(lpdv->br.rgbPat, lpbr->rgbPat,
	    		sizeof(lpbr->rgbPat)))) {
		WriteChannelChar(lpdv, '<');
		lpb = lpbr->rgbPat;
		for (i = 0; i < sizeof(lpbr->rgbPat); i++)
			PrintChannel(lpdv, hex, *lpb++);
		PrintChannel(lpdv, "> p\n");

		/* save the pattern */
		lmemcpy(lpdv->br.rgbPat, lpbr->rgbPat, sizeof(lpbr->rgbPat));
	}


	lpdv->FillMode = fm;	/* do this last, code above depends on this */
}




/*******************************************************************
 * Name: SelectPen()
 *
 * Action: This routine is called to establish a new pen by
 *	  all routines that use a pen for output.
 *
 *********************************************************************/

void FAR PASCAL SelectPen(lpdv, lppen)
LPDV	lpdv;	/* Far ptr to the device */
LPPEN	lppen;	/* Far ptr to the pen */
{
	int	iLineCap;
	int	iLineJoin;

	if (!lppen || (lppen->lopn.lopnStyle == LS_NOLINE)) {
		lpdv->pen.lopn.lopnStyle = LS_NOLINE;
		return;
	}

	iLineCap  = lpdv->iNewLineCap;
	iLineJoin = lpdv->iNewLineJoin;


	/* Fix up the line caps if we are defaulting to GDI's style */
	if (iLineCap == -1)
		if (lppen->fRound)
			iLineCap = 1;
		else
			iLineCap = 0;


	/* Fix up the line join if we are defaulting to GDI's style */
	if (iLineJoin == -1)
		if (lppen->fRound)
			iLineJoin = 1;
		else
			iLineJoin = 0;


	/* Set the line cap if it has changed */
	if (lpdv->iCurLineCap != iLineCap) {
		PrintChannel(lpdv, "%d lc\n", iLineCap);
		lpdv->iCurLineCap = iLineCap;
	}

	/* Set the line join if it has changed */
	if (lpdv->iCurLineJoin != iLineJoin) {
		PrintChannel(lpdv, "%d lj\n", iLineJoin);
		lpdv->iCurLineJoin = iLineJoin;
	}


	/* Update the miter limit */
	if (lpdv->iCurMiterLimit != lpdv->iNewMiterLimit) {
		PrintChannel(lpdv, "%d ml\n",
		    lpdv->iNewMiterLimit);
		lpdv->iCurMiterLimit = lpdv->iNewMiterLimit;
	}


	/* Change pen color if necessary */

	if (!lpdv->fPenSelected	|| lppen->lopn.lopnColor != lpdv->pen.lopn.lopnColor) {
		PrintChannel(lpdv, "%d %d %d pC\n",
			GetRComponent(lpdv, lppen->lopn.lopnColor),
			GetGComponent(lpdv, lppen->lopn.lopnColor),
			GetBComponent(lpdv, lppen->lopn.lopnColor));

		lpdv->pen.lopn.lopnColor = lppen->lopn.lopnColor;
	}

	/* change width and style (dashed or not) */

	if (!lpdv->fPenSelected || PenWidth(lppen) != PenWidth(&lpdv->pen) ||
	    lppen->lopn.lopnStyle != lpdv->pen.lopn.lopnStyle) {

		/* SP == set pen */
		PrintChannel(lpdv, "%d %d SP\n", lppen->lopn.lopnStyle, PenWidth(lppen));

		/* Save the current pen parameters */
		lpdv->pen = *lppen;
	}

	lpdv->fPenSelected = TRUE;
}


int FAR PASCAL GetRComponent(LPDV lpdv, DWORD dwRGB)
{
    int r;

    r = GetRValue(dwRGB);
    if (lpdv->bColorToBlack 
        && ((dwRGB & RGB(255,255,255)) != RGB(255,255,255)))
        r = 0;

    return r;
}

int FAR PASCAL GetGComponent(LPDV lpdv, DWORD dwRGB)
{
    int g;

    g = GetGValue(dwRGB);
    if (lpdv->bColorToBlack
        && ((dwRGB & RGB(255,255,255)) != RGB(255,255,255)))
        g = 0;

    return g;
}

int FAR PASCAL GetBComponent(LPDV lpdv, DWORD dwRGB)
{
    int b;

    b = GetBValue(dwRGB);
    if (lpdv->bColorToBlack
        && ((dwRGB & RGB(255,255,255)) != RGB(255,255,255)))
        b = 0;

    return b;
}

