#define WIN20

/***************************************************************************
 * ??Jan87	sjp		Fixed box,ellipse and arc drawing.
 * 12Jun87	sjp		Started round rect support.
 * 18Jun87	sjp		Finished round rect support.
 ***************************************************************************/
#include "pscript.h"

#define MaxColors 2
#define RGB_BLACK 0xffffffffL
#define RGB_WHITE 0L
#define CODEFAULT 0L		/* The default HLS color */


void PASCAL PointsToChannel(DV FAR *, LPPOINT, int);
extern void FAR PASCAL SelectBrush(DV FAR *, BR FAR *);
void PASCAL SelectPen(DV FAR *, PEN FAR *);
extern void FAR PASCAL ClipBox(DV FAR *, RECT FAR *);
extern int FAR PASCAL Lightness(RGB);
void FAR PASCAL lmemcpy(LPSTR, LPSTR, int);
BOOL FAR PASCAL lmemIsEqual(LPSTR, LPSTR, int);
WORD FAR PASCAL GetVersion(void);


extern void PASCAL SetColor(DV FAR *, CO);
extern void PASCAL SetROP(DV FAR *, short int);


/* added by Aldus Corporation--22 Jan 87--sjp */
typedef int FAR *LPINT;

#define ODD(X)	((X) & 1)		/* Returns TRUE if X is odd */
#define EVEN(X) (!((X) & 1))	/* Returns TRUE if X is even */

/****************************************************************************/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

/*#define DEBUG_ROUNDRECT*/
#ifdef DEBUG_ROUNDRECT
#define DBMSG1(msg) printf msg
#else
#define DBMSG1(msg)
#endif

/*#define DEBUG_VERSION*/
#ifdef DEBUG_VERSION
#define DBMSG2(msg) printf msg
#else
#define DBMSG2(msg)
#endif
/****************************************************************************/




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
****************************************************************
*/
BOOL FAR PASCAL EnumObj(lpdv, iStyle, lpfn, lpb)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
int iStyle;	    /* The style (brush or pen) */
FARPROC lpfn;	    /* Far ptr to the callback function */
LPSTR lpb;	    /* Far ptr to the client data (passed to callback) */
{
    static RGB rgrgb[MaxColors] =  {RGB_BLACK, RGB_WHITE};

    BOOL fContinue;	/* TRUE if enumeration should continue */
    LOGPEN lopn;	/* The logical pen */
    LOGBRUSH lb;	/* The logical brush */
    int i;

    ASSERT(lpdv!=(LPSTR) (long) NULL);
    ASSERT(lpfn!=(LPSTR) (long) NULL);
    ASSERT(iStyle==OBJ_PEN || iStyle==OBJ_BRUSH);

    fContinue = FALSE;
    switch(iStyle)
	{
	case OBJ_PEN:
	    lopn.lopnWidth.x = 0;
	    lopn.lopnWidth.y = 0;

	    /* Enumerate each pen style for all possible pen colors */
	    for (lopn.lopnStyle=0; lopn.lopnStyle<MaxLineStyle; ++lopn.lopnStyle)
		{
		for (i=0; i<MaxColors; ++i)
		    {
		    lopn.lopnColor = rgrgb[i];
		    if (!(*lpfn)((LOGPEN FAR *) &lopn, lpb))
			goto DONE;
		    }
		}
    break;

	case OBJ_BRUSH:

	    /* Enumerate the hollow brush */
	    lb.lbStyle = BS_HOLLOW;
	    lb.lbColor = RGB_WHITE;
	    lb.lbBkColor = RGB_WHITE;
	    lb.lbHatch = 0;
	    if ((!(*lpfn)((LOGBRUSH FAR *) &lb, lpb)))
		goto DONE;

	    /* Enumerate all possible hatch brushes for black on white */
	    lb.lbStyle = BS_HATCHED;
	    lb.lbColor = RGB_BLACK;
	    lb.lbBkColor = RGB_WHITE;
	    for (lb.lbHatch=0; lb.lbHatch<MaxHatchStyle; ++lb.lbHatch)
		if (!(*lpfn)((LOGBRUSH FAR *) &lb, lpb))
		    goto DONE;

	    /* Enumerate all the possible colors for a solid brush */
	    lb.lbStyle = BS_SOLID;
	    for (i=0; i<MaxColors; ++i)
		{
		lb.lbColor = rgrgb[i];
		if (!(*lpfn)((LOGBRUSH FAR *) &lb, lpb))
		    goto DONE;
		}
    break;
	}

	/* Control comes here if all callbacks returned TRUE */
	fContinue = TRUE;
DONE:
	return(fContinue);
}

/**********************************************************
* Name: PenWidth()
*
* Action: Calculate the PostScript pen width from the pen
*	  X and Y dimensions that GDI specifies in the
*	  logical pen.
*
***********************************************************
*/
short PASCAL PenWidth(lppen)
PEN FAR *lppen;
{
    int iWidth;

	DBMSG(((LPSTR)">PenWidth(): x=%d,y=%d\r",
		lppen->lopn.lopnWidth.x,lppen->lopn.lopnWidth.y));
    /* Make the pen width be its maximum dimension */
    iWidth = lppen->lopn.lopnWidth.x > lppen->lopn.lopnWidth.y ?
		lppen->lopn.lopnWidth.x : lppen->lopn.lopnWidth.y;
	DBMSG(((LPSTR)" PenWidth(): width=%d\r",iWidth));

    /* Make sure the pen width is non-zero */
    if (iWidth == 0) iWidth = 1;
	DBMSG(((LPSTR)"<PenWidth(): width=%d\r",iWidth));
    return(iWidth);
}





/***********************************************************
* Name: PointsToChannel()
*
* Action: Pass a number of points to PostScript by printing
*	  them on the output channel.
*
************************************************************
*/
void PASCAL PointsToChannel(lpdv, lppt, cpt)
DV FAR *lpdv;
LPPOINT lppt;
int cpt;
    {
    int i;


    ASSERT(lpdv!=(LPSTR)(long) NULL);
    ASSERT(lppt!=(LPSTR)(long) NULL);
    ASSERT(cpt>0);

    if (lppt)
	{
	while (cpt>0)
	    {
	    /* Output the points in groups of eight */
	    i = cpt<8 ? cpt : 8;
	    cpt -= i;
	    while (--i>=0)
		{
		PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x, lppt->y);
		++lppt;
		}
	    if (cpt>0)
		PrintChannel(lpdv, (LPSTR) "\n");
	    }
	}
    }





/************************************************************
* Name: Polygon()
*
* Action: Draw a polygon on the display surface.  The perimiter
*	  of the polygon is stroked with the current pen and
*	  the interior is filled with the current brush.
*
* Note: If the polygon is very complex, then its perimeter should
*	specified in segments of 50 points or less to avoid
*	a stack overflow in PostScript.
*
***************************************************************
*/
void PASCAL Polygon(iStyle, lpdv, lppt, cpt)
int iStyle;	    /* The output style */
DV FAR *lpdv;	    /* A far ptr to the device descriptor */
POINT FAR *lppt;    /* A far ptr to an array of points on the perimeter */
int cpt;	    /* The number of points */
{
    int i;
    int cptT;
    POINT FAR *lpptT;


    ASSERT(lpdv!=(LPSTR) (long) NULL);
    ASSERT(iStyle==OS_EOPOLYGON || iStyle==OS_POLYGON);


    if (lppt && cpt>0){
		PrintChannel(lpdv, (LPSTR) "%d %d StartFill\n", lppt->x, lppt->y);
		++lppt;
		--cpt;

		/* Loop once for each segment of 50 points */
		while (cpt>0){
		    cptT = (cpt>50) ? 50 : cpt;
		    lpptT = lppt + cptT;

		    /* Push the points onto the PostScript stack in reverse order */
		    for (i=0; i<cptT; ++i){
				--lpptT;
				PrintChannel(lpdv, (LPSTR) "%d %d ", lpptT->x, lpptT->y);
			}

		    PrintChannel(lpdv, (LPSTR) "%d PolyPoints\n", cptT);
		    cpt -= cptT;
		    lppt += cptT;
		}
		PrintChannel(lpdv, (LPSTR) "%d EndFill\n", iStyle==OS_EOPOLYGON);
	}
}



/*****************************************************************
* Name: PolyLine()
*
* Action: Draw a segmented line on the display surface using the
*	  current pen.
*
* Note: The line is drawn in segments of at most 50 points to
*	avoid overflowing the PostScript stack.
*
******************************************************************
*/

#ifdef WIN20
void PASCAL Polyline(lpdv, lppt, cpt)
DV FAR *lpdv;
POINT FAR *lppt;
int cpt;
    {
    int i;
    int ixLast, iyLast;     /* The last point to be drawn */
    int ixPred, iyPred;     /* The second to the last point drawn */
    POINT FAR *lpptLast;    /* A far ptr to the last point */


    ASSERT(lpdv!=(LPSTR) (long) NULL);
    ASSERT(lppt!=(LPSTR) (long) NULL);
    ASSERT(cpt>0);


    /* If there is nothing to draw, then return */
    if (lppt==NULL || cpt<2)
        return;

    lpptLast = NULL;

    /* The next few lines of code adjust for not drawing the last dot */
    ixLast = lppt[cpt-1].x;       iyLast = lppt[cpt-1].y;
    ixPred = lppt[cpt-2].x;       iyPred = lppt[cpt-2].y;

    if (ixLast==ixPred && iyLast==iyPred)   /* Can a point be eliminated? */
        {
        if (--cpt<2)
            return;             /* Return if there is nothing to draw */
        }
    else
        {
        lpptLast = &lppt[cpt-1];        /* Save a mark to the last point */
        if (ixLast > ixPred)
            --lppt[cpt-1].x;            /* Bump last point left one pixel */
        else if (ixLast < ixPred)
            ++lppt[cpt-1].x;            /* Bump last point right one pixel */

        if (iyLast > iyPred)
            --lppt[cpt-1].y;            /* Bump last point up one pixel */
        else if (iyLast < iyPred)
            ++lppt[cpt-1].y;            /* Bump last point down one pixel */
        }


    /* Draw the line */
    while (cpt>0)
        {
        /* Split the line up into segments of 50 points maximum */
        i = (cpt>50) ? 50 : cpt;

        PointsToChannel(lpdv, lppt, i);
        PrintChannel(lpdv, (LPSTR) "%d GDIPolyLine\n", i);

        /* If there is another segment, start it where this one ended */
        if (cpt > i)
            --i;
        cpt -= i;
        lppt += i;
	}


    /* Restore the value of the last point if we modified it */
    if (lpptLast)
        {
        lpptLast->x = ixLast;
        lpptLast->y = iyLast;
        }

    }



#else



void PASCAL Polyline(lpdv, lppt, cpt)
DV FAR *lpdv;
POINT FAR *lppt;
int cpt;
    {
    int i;


    ASSERT(lpdv!=(LPSTR) (long) NULL);
    ASSERT(lppt!=(LPSTR) (long) NULL);
    ASSERT(cpt>0);

    if (lppt && cpt>0)
	{
	while (cpt>0)
	    {
	    /* Split the line up into segments of 50 points maximum */
	    i = (cpt>50) ? 50 : cpt;
	    PointsToChannel(lpdv, lppt, i);
	    PrintChannel(lpdv, (LPSTR) "%d GDIPolyLine\n", i);

	    /* If there is another segment, start it where this one ended */
	    if (cpt > i)
		--i;
	    cpt -= i;
	    lppt += i;
	    }
	}

    }

#endif



/***************************************************************/

void BoxAdjust(xWeight,yWeight,dl,dt,dr,db)
	int xWeight,yWeight;
	LPINT dl,dt,dr,db;
{
	DBMSG2(((LPSTR)">BoxAdjust()\r"));

	*dl=*dr=*dt=*db=0;

	/* if the current version of Windows is pre-2.0 then do this
	 * adjustment because there was a bug in the early version
	 * of GDI
	 */
	DBMSG2(((LPSTR)" BoxAdjust() version=%d.%d\r",
		GetVersion()&0x00ff,(GetVersion()&0xff00)>>8));
	if((GetVersion() & 0x00ff) < 2){
		if(xWeight<=0) return;
		else *dl=*dr=xWeight/2;

		if(yWeight<=0) return;
		else *dt=*db=yWeight/2;

		if(EVEN(xWeight)) (*dr)--;
		if(EVEN(yWeight)) (*db)--;
	}
	DBMSG2(((LPSTR)"<BoxAdjust():  xw=%d,yw=%d,",xWeight,yWeight));
	DBMSG2(((LPSTR)"dl=%d,dt=%d,dr=%d,db=%d\r",*dl,*dt,*dr,*db));
}

/***************************************************************/

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
******************************************************************
*/
int FAR PASCAL Output(lpdv, iStyle, cpt, lppt, lppen, lpbr, lpdm, lprcClip)
	DV FAR *lpdv;	    /* Far ptr to the device descriptor */
	int iStyle;	    /* The output style (rectangle, elipse, etc) */
	int cpt;	    /* The count of points in the point list */
	POINT FAR *lppt;    /* Far ptr to a list of points */
	PEN FAR *lppen;     /* Far ptr to the pen to use */
	BR FAR *lpbr;	    /* Far ptr to the brush to use */
	DRAWMODE FAR *lpdm; /* Far ptr to the draw mode (bacground color, etc.) */
	LPRECT lprcClip;    /* Far ptr to the clipping rectangle */
{
    int cxPen;	    /* The pen width */
    int cyPen;	    /* The pen height */
    int i;
    int iy;	    /* The vertical position */
    int iDelta1;
    int iDelta2;

	DBMSG(( (LPSTR)">Output(): iStyle=%d cpt=%d\r", iStyle, cpt));
	DBMSG1(( (LPSTR)">Output(): iStyle=%d cpt=%d\r", iStyle, cpt));

	if (!lpdv->dh.iType){
		return (dmOutput(lpdv,iStyle,cpt,lppt,lppen,lpbr,lpdm,lprcClip));
	}

	if (lpdv->dh.iType!=-1)
		return (0);				/* fail: this is not our PDEVICE */

    ASSERT(lpdv!=(LPSTR) (long) NULL);

    /* Select the brush if it exists
	 * Note: The brush must be selected before the pen.
	 *	    this avoids loosing the pen through the save/restore
	 * Note: Bush and pen selection must be done before clipping
	 *	    to avoid loosing them though save/restore
	 */

    switch(iStyle){
		case OS_PIE:
		case OS_ELLIPSE:
		case OS_RECTANGLE:
			DBMSG1(((LPSTR)" Output(): OS_PIE,ELLIPSE,RECTANGLE\r"));
		    SelectBrush(lpdv, lpbr);
		break;

#ifdef ROUNDRECTSTUFF
		/* for now include this as a separate case
		 * so that it will be easier to debug and
		 * exclude from compilation.
		 */
		case OS_ROUNDRECT:
			DBMSG1(((LPSTR)" Output(): OS_ROUNDRECT\r"));
		    SelectBrush(lpdv, lpbr);
		break;
#endif

		case OS_EOPOLYGON:
		case OS_POLYGON:
		case OS_SCANLINES:
			DBMSG1(((LPSTR)" Output(): OS_EOPOLYGON,POLYGON,SCANLINES\r"));
		    if (lpbr->lb.lbStyle!=BS_HOLLOW)
			SelectBrush(lpdv, lpbr);
	    break;
	}


    /* Select the pen if it exists */
    if (lppen){
		/* Potential problem area?  The round pen flag is set only
		 * if NOT rectangle.  With round rects of very small radius
		 * corners this could be a problem.  However, a point of
		 * interest is with a square pen drawing a round corner the
		 * cross section of the radius could be up to sqrt(2) times
		 * the cross section of the vertical or horizontal pieces.
		 */
		lppen->fRound = iStyle!=OS_RECTANGLE;
   	    cxPen = lppen->lopn.lopnWidth.x;
        cyPen = lppen->lopn.lopnWidth.y;
		if (iStyle==OS_SCANLINES){
		    /* Scan-lines must use a 1 X 1 pixel pen */
			lppen->lopn.lopnWidth.x = 1;
			lppen->lopn.lopnWidth.y = 1;
			SelectPen(lpdv, lppen);
			lppen->lopn.lopnWidth.x = cxPen;
			lppen->lopn.lopnWidth.y = cyPen;
		}
		else{
			SelectPen(lpdv, lppen);
		}
	}else{
        cxPen = 0;
        cyPen = 0;
	}


    switch(iStyle){

		case OS_ARC:
			DBMSG1(((LPSTR)" Output(): OS_ARC\r"));
		    ASSERT(lppt!=(LPSTR) (long) NULL);

		    ClipBox(lpdv, lprcClip);
		    if (lppen!=(LPSTR) (long) NULL){
				if (lppen->lopn.lopnWidth.x >= lppen->lopn.lopnWidth.y){
				    iDelta1 = lppen->lopn.lopnWidth.x;
				}else{
				    iDelta1 = lppen->lopn.lopnWidth.y;
				}
				if (iDelta1<=0) iDelta1 = 1;
			}
		    else iDelta1 = 1;

		    iDelta2 = iDelta1/2;
		    iDelta1 = iDelta1 - iDelta2;

		    PrintChannel(lpdv,(LPSTR)"%d %d ",
				lppt->x+iDelta1, lppt->y+iDelta1);
		    ++lppt;
		    PrintChannel(lpdv,(LPSTR)"%d %d ",
				lppt->x-iDelta2, lppt->y-iDelta2);
		    ++lppt;
#if TRUE
		    PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x, lppt->y);
		    ++lppt;
		    PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x, lppt->y);
#else
			{
				int i,dl,dt,dr,db;

	            i = cxPen > cyPen ? cxPen : cyPen;
				BoxAdjust(i,i,(LPINT)&dl,(LPINT)&dt,(LPINT)&dr,(LPINT)&db);
				DBMSG(((LPSTR)"GDIArc: x=%d,y=%d, ",lppt->x,lppt->y));
		    	PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x+dl, lppt->y+dt);
		    	++lppt;
				DBMSG(((LPSTR)"x=%d,y=%d\r",lppt->x,lppt->y));
		    	PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x-dr, lppt->y-db);
			}
#endif

		    PrintChannel(lpdv, (LPSTR) "GDIArc\n");
		    ClipBox(lpdv, (RECT FAR *) (long) NULL);
	    break;

		case OS_PIE:
			DBMSG1(((LPSTR)" Output(): OS_PIE\r"));
		    ASSERT(lppt!=(LPSTR) (long) NULL);

		    ClipBox(lpdv, lprcClip);
		    PointsToChannel(lpdv, lppt, 4);
		    PrintChannel(lpdv, (LPSTR) "GDIPie\n");
		    ClipBox(lpdv, (RECT FAR *) (long) NULL);
	    break;

		case OS_ELLIPSE:
			DBMSG1(((LPSTR)" Output(): OS_ELLIPSE\r"));
		    ASSERT(lppt!=(LPSTR) (long) NULL);

		    ClipBox(lpdv, lprcClip);
#ifdef OLDCODE
		    PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x, lppt->y);
			++lppt;
			PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x - 1, lppt->y - 1);
#else
			{
				int i,dl,dt,dr,db;

	            i = cxPen > cyPen ? cxPen : cyPen;
				BoxAdjust(i,i,(LPINT)&dl,(LPINT)&dt,(LPINT)&dr,(LPINT)&db);

				DBMSG(((LPSTR)"GDIEllipse: x=%d,y=%d, ",lppt->x,lppt->y));
			    PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x+dl, lppt->y+dt);
			    ++lppt;
				DBMSG(((LPSTR)"x=%d,y=%d\r",lppt->x,lppt->y));
			    PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x-dr, lppt->y-db);
			}
#endif
		    PrintChannel(lpdv, (LPSTR) "GDIEllipse\n");
			ClipBox(lpdv, (RECT FAR *) (long) NULL);
	    break;

		case OS_SCANLINES:

			DBMSG1(((LPSTR)" Output(): OS_SCANLINES\r"));
		    ClipBox(lpdv, lprcClip);
		    if (--cpt>0){
				ASSERT(lppt!=(LPSTR) (long) NULL);
				iy = (lppt++)->y;

				/* Loop once for each line segment */
				for (i=0; i<cpt; ++i){
				    PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x, lppt->y);
				    ++lppt;
		    	}
				PrintChannel(lpdv, (LPSTR) "%d %d %d _S\n",
					 iy, cpt, lpbr!=(LPSTR) (long) NULL);
			}
		    ClipBox(lpdv, (RECT FAR *) (long) NULL);
	    break;

		case OS_RECTANGLE:
			DBMSG1(((LPSTR)" Output(): OS_RECTANGLE\r"));
            i = cxPen > cyPen ? cxPen : cyPen;
            i = i / 2;
		    ClipBox(lpdv, lprcClip);


#ifdef OLDCODE
	        PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x  + i, lppt->y + i);
		    ++lppt;
			PrintChannel(lpdv, (LPSTR) "%d %d GDIRect\n",
	        	(lppt->x - 1) - i, (lppt->y - 1) - i);
#else
			{
				int i,dl,dt,dr,db;

	            i = cxPen > cyPen ? cxPen : cyPen;
				BoxAdjust(i,i,(LPINT)&dl,(LPINT)&dt,(LPINT)&dr,(LPINT)&db);

				DBMSG(((LPSTR)"GDIRect: x=%d,y=%d, ",lppt->x,lppt->y));
				PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x+dl, lppt->y+dt);
			    ++lppt;
				DBMSG(((LPSTR)"x=%d,y=%d\r",lppt->x,lppt->y));
			    PrintChannel(lpdv,(LPSTR)"%d %d GDIRect\n",
					lppt->x-dr,lppt->y-db);
			}
#endif
			ClipBox(lpdv, (RECT FAR *) (long) NULL);
	    break;

#ifdef ROUNDRECTSTUFF
		/* for now include this as a separate case
		 * so that it will be easier to debug and
		 * exclude from compilation.
		 */
		case OS_ROUNDRECT:
			DBMSG1(((LPSTR)" Output(): OS_ROUNDRECT\r"));
		    ClipBox(lpdv, lprcClip);
			{
				int i,dl,dt,dr,db;

	            i = cxPen > cyPen ? cxPen : cyPen;
				BoxAdjust(i,i,(LPINT)&dl,(LPINT)&dt,(LPINT)&dr,(LPINT)&db);

				/* get upper left corner */
				DBMSG1(((LPSTR)" Output(): x0=%d,y0=%d, ",lppt->x,lppt->y));
				PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x+dl, lppt->y+dt);

				/* get lower right corner */
			    ++lppt;
				DBMSG1(((LPSTR)"x1=%d,y1=%d, ",lppt->x,lppt->y));
				PrintChannel(lpdv, (LPSTR) "%d %d ", lppt->x-dr, lppt->y-db);

				/* get x and y dimension radii */
			    ++lppt;
				DBMSG1(((LPSTR)"x2=%d,y2=%d\r",lppt->x/2,lppt->y/2));
			    PrintChannel(lpdv,(LPSTR)"%d %d GDIRoundRect\n",
					lppt->x/2,lppt->y/2);
			}
			ClipBox(lpdv, (RECT FAR *) (long) NULL);
		break;
#endif
		case OS_POLYLINE:

			DBMSG1(((LPSTR)" Output(): OS_POLYLINE\r"));
		    ClipBox(lpdv, lprcClip);
		    Polyline(lpdv, lppt, cpt);
		    ClipBox(lpdv, (RECT FAR *) (long) NULL);
	    break;

		case OS_EOPOLYGON:
		case OS_POLYGON:
			DBMSG1(((LPSTR)" Output(): OS_POLYGON: cpt=%d\r",cpt));
		    ClipBox(lpdv, lprcClip);
		    if (lpbr->lb.lbStyle==BS_HOLLOW)
				Polyline(lpdv, lppt, cpt);
		    else
				Polygon(iStyle, lpdv, lppt, cpt);
			ClipBox(lpdv, (RECT FAR *) (long) NULL);
	    break;

		default:
			DBMSG1(((LPSTR)" Output():  default\r"));
		break;
	}

	DBMSG1(((LPSTR)"<Output(): SUCCESS\r"));
    return(1);	    /* Success */
}



/************************************************************
* Name: Pixel()
*
* Action: This routine sets or retrieves the color of a given
*	  pixel.
*
*************************************************************
*/
CO FAR PASCAL Pixel(lpdv, ix, iy, co, lpdm)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
int ix; 	    /* The horizontal device coordinate */
int iy; 	    /* The vertical device coordinate */
CO  co; 	    /* The physical color */
DRAWMODE FAR *lpdm; /* Far ptr to DRAWMODE (includes raster op, etc.) */
    {

#ifdef WIN20

    register int iGray;



    ASSERT(lpdv!=(LPSTR) (long) NULL);

    if (lpdm)
	{

        /* A color change invalidates the current brush and pen */
        lpdv->dh.br.lid = -1;
        lpdv->dh.pen.lid = -1;


        /* Map colors to either black or white */
        iGray = 0;
        if ((co & 0x0ffffffL) == 0x0ffffffL)
            iGray = 1;

        PrintChannel(lpdv, (LPSTR) "%d setgray %d %d moveto %d %d lineto stroke\n",
            iGray, ix, iy, ix+1, iy);


	}
    else
	co = CODEFAULT;

#else

    POINT rgpt[2];


    ASSERT(lpdv!=(LPSTR) (long) NULL);

    if (lpdm)
	{
	SetColor(lpdv, co);
	SetROP(lpdv, lpdm->Rop2);

	rgpt[0].x = ix;
	rgpt[0].y = iy;
	rgpt[1].x = ix + 1;
	rgpt[1].y = iy;
	Polyline(lpdv, (POINT FAR *) rgpt, 2);
	}
    else
	co = CODEFAULT;
#endif



    return(co);
    }





/*******************************************************************
* Name: ScanLR()
*
* Action: This routine is used to scan a single horizontal scan line
*	  on the device's display surface for a pixel having ( or not
*	  having a given color.  The direction of scan can be left or
*	  right from the specified starting position.
*
* Note: We can't support this call for the LaserWriter since the
*	bitmap is down in the printer and there is no way to access it.
*	So the best thing that we can do is return the starting position
*	as the location of the boundary. This will cause flood-fill to
*	generally do the wrong thig, but this is one of those ugly facts
*	of life.
*
*********************************************************************
*/
int FAR PASCAL ScanLR(lpdv, ix, iy, co, iStyle)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
int ix; 	    /* The starting horizontal device coordinate */
int iy; 	    /* The starting vertical device coordinate */
CO  co; 	    /* The physical color to search for */
int iStyle;	    /* The direction of scan, etc. */
    {

    /* For vector devices, assume that we are already at a boundary */
    return (ix);
    }





/****************************************************************
* Name: ColorInfo()
*
* Action: This routine is used to convert between Window's
*	  RGB color representation and the device's physical
*	  color representation.
*
* Note: For a PostScript output device, no color translation
*	is necessary, so we just return the same color value
*	that was input.
*
*******************************************************************
*/
RGB FAR PASCAL ColorInfo(lpdv, rgb, lpco)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
RGB rgb;	    /* The RGB color (or physical color) */
CO FAR *lpco;	    /* Far ptr to the physical color */
    {

    /* Convert any non-white values to black */
    if ((rgb & 0x0ffffffL) != 0x0ffffffL)
        rgb = 0L;

    if (lpco)
	{
	*lpco = (CO) rgb;
	}
    return(rgb);
    }


/******************************************************************
* Name: SetPattern()
*
* Action: This routine is called from SelectBrush() to establish
*	  a new pattern.  Since it is very expensive to change
*	  patterns in PostScript, this routine first checks to make
*	  sure that the pattern has changed before issuing a
*	  patter setup command to the printer.
*
* Output: Returns TRUE if the pattern was changed.
*
*******************************************************************
*/
BOOL PASCAL SetPattern(lpdv, lpbr)
DV FAR *lpdv;		/* Far ptr to the device descriptor */
BR FAR *lpbr;		/* Far ptr to the new brush */
{
    int i;
    LPSTR lpb;


    ASSERT(lpdv!=(LPSTR) (long) NULL);
    ASSERT(lpb!=(LPSTR) (long) NULL);
    ASSERT(lpbr->lb.lbStyle==BS_PATTERN || lpbr->lb.lbStyle==BS_HATCHED);


    /* Check to see if a current pattern exists */
    i = lpdv->dh.br.lb.lbStyle;
    if (lpdv->dh.br.lid!=-1L && (i==BS_PATTERN || i==BS_HATCHED)){
		/* If the pattern is still valid, do nothing */
		if(lmemIsEqual(lpdv->dh.br.rgbPat,lpbr->rgbPat,sizeof(lpbr->rgbPat)))
	    	return(FALSE);
	}

    /* Mark the current font and pen as invalid */
    lpdv->dh.pen.lid = -1L;
    lpdv->dh.lidFont = -1L;


    /* Send the new pattern to the printer */

    lpb = (LPSTR) lpbr->rgbPat;
    PrintChannel(lpdv, (LPSTR) "<");

    for (i=0; i<sizeof(lpbr->rgbPat); ++i)
        PrintChannel(lpdv, (LPSTR) "%02x", ((int) *lpb++) & 0x0ff);

    PrintChannel(lpdv, (LPSTR) "> SetPattern\n");

    return(TRUE);
}





/*********************************************************************
* Name: SelectBrush()
*
* Action: This routine is called to establish a new brush pattern by
*	  all output routines that use brushes.
*
**********************************************************************
*/
void FAR PASCAL SelectBrush(lpdv, lpbr)
DV FAR *lpdv;
BR FAR *lpbr;
{

    ASSERT(lpdv!=(LPSTR) (long) NULL);


    /* If there is no brush then do nothing */
    if (!lpbr)
		return;

    /* If the brush is already selected then do nothing */
    if (lpbr->lid==lpdv->dh.br.lid && lpbr->lb.lbStyle!=BS_PATTERN)
		return;


    switch(lpbr->lb.lbStyle)
	{
	case BS_SOLID:
	    PrintChannel(lpdv, (LPSTR) "%d %d SetBrush\n",
			Lightness(lpbr->lb.lbColor), BS_SOLID);
    break;

	case BS_HOLLOW:
	    PrintChannel(lpdv, (LPSTR) "%d SetBrush\n", BS_HOLLOW);
    break;

	case BS_HATCHED:
	case BS_PATTERN:
	    if (SetPattern(lpdv, lpbr))
			PrintChannel(lpdv, (LPSTR) "%d SetBrush\n", BS_PATTERN);
    break;
	}

    /* Update the current brush */
    lmemcpy((LPSTR)&lpdv->dh.br, (LPSTR)lpbr, sizeof(BR));
}





/*******************************************************************
* Name: SelectPen()
*
* Action: This routine is called to establish a new pen by
*	  all routines that use a pen for output.
*
********************************************************************
*/
void PASCAL SelectPen(lpdv, lppen)
DV FAR *lpdv;	    /* Far ptr to the device */
PEN FAR *lppen;     /* Far ptr to the pen */
{
    int iLineCap;
    int iLineJoin;

    ASSERT(lpdv!=(LPSTR) (long) NULL);

    if (!lppen)
		return;


    iLineCap = lpdv->dh.iNewLineCap;
    iLineJoin = lpdv->dh.iNewLineJoin;


    /* Fix up the line caps if we are defaulting to GDI's style */
    if (iLineCap==-1)
        if (lppen->fRound)
            iLineCap = 1;
        else
            iLineCap = 0;


    /* Fix up the line join if we are defaulting to GDI's style */
    if (iLineJoin==-1)
        if (lppen->fRound)
            iLineJoin = 1;
        else
            iLineJoin = 0;


    /* Set the line cap if it has changed */
    if (lpdv->dh.iCurLineCap != iLineCap){
        PrintChannel(lpdv, (LPSTR) "%d setlinecap\n", iLineCap);
        lpdv->dh.iCurLineCap = iLineCap;
	}

    /* Set the line join if it has changed */
    if (lpdv->dh.iCurLineJoin != iLineJoin){
        PrintChannel(lpdv, (LPSTR) "%d setlinejoin\n", iLineJoin);
        lpdv->dh.iCurLineJoin = iLineJoin;
	}

    /* Update the miter limit */
    if (lpdv->dh.iCurMiterLimit != lpdv->dh.iNewMiterLimit){
        PrintChannel(lpdv, (LPSTR) "%d setmiterlimit\n",
			lpdv->dh.iNewMiterLimit);
        lpdv->dh.iCurMiterLimit = lpdv->dh.iNewMiterLimit;
    }

    /* Change pens if the new one is different */
    if (lpdv->dh.pen.lid==-1L ||
	    PenWidth(lppen) != PenWidth((PEN FAR *)&lpdv->dh.pen) ||
        Lightness(lpdv->dh.pen.lopn.lopnColor)!=Lightness(lppen->lopn.lopnColor) ||
	    lppen->lopn.lopnStyle != lpdv->dh.pen.lopn.lopnStyle
	){
        PrintChannel(lpdv, (LPSTR) "%d %d %d SetPen\n",
			lppen->lopn.lopnStyle, PenWidth(lppen),
			Lightness(lppen->lopn.lopnColor), iLineCap, iLineJoin);

		/* Save the current pen parameters */
		lmemcpy((LPSTR) &lpdv->dh.pen, (LPSTR) lppen, sizeof(PEN));
	}

}



#ifndef WIN20

/* These routines are never referenced */
void PASCAL SetColor(lpdv, co)
DV FAR *lpdv;
CO co;
    {



    }




void PASCAL SetROP(lpdv, rop)
DV FAR *lpdv;
short int rop;
    {
    }
#endif


