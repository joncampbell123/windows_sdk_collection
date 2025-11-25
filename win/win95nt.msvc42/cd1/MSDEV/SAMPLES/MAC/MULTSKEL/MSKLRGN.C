/*
	TransSkel multiple-window demonstration: Region module

	This module handles a window in which the mouse may be clicked and
	dragged to draw rectangles.  The rects so drawn are combined into
	a single region, the outline of which is drawn.  Rects drawn while
	the shift key is held down are subtracted from the region.
	Double-clicking the mouse clears the display.  If the window is
	resized, the region that is drawn is resized as well.

	21 April 1988		Paul DuBois

	Changes:
	07/08/86 Changed outline so that it's drawn as a marquee.
*/

# include	"MultSkel.h"
# include	<macos\Events.h>

WindowPtr	rgnWind;
Rect		rgnPortRect;	/* portRect size - for detecting wind grows */
RgnHandle	selectRgn;		/* current region to be drawn */
Longint		selectWhen;		/* time of last click */
Point		selectWhere;	/* location of last click */

Pattern		marqueePat = { 0x0f, 0x87, 0xc3, 0xe1, 0xf0, 0x78, 0x3c, 0x1e };

/*  Prototypes for local functions*/

SetWindClip (WindowPtr wind);  					// Defined in multskel.c
ResetWindClip ();						  		// Defined in multskel.c
DoSelectRect (Point startPoint, Rect* dstRect);	// Defined below
MarqueeRgn (RgnHandle r);						// Defined below
SkelWindow (WindowPtr theWind, ProcPtr pMouse, ProcPtr pKey, ProcPtr pUpdate, 
			ProcPtr pActivate, ProcPtr pClose, ProcPtr pClobber, ProcPtr pIdle,
			Boolean	frontOnly );	// Defined in transkel.c
DrawGrowBox (WindowPtr wind);		// Defined in multskel.c		


static Clobber ()
{
	DisposeRgn (selectRgn);
	CloseWindow (rgnWind);
	return 0;
}


/*
	On double-click, clear window.  On single click, draw gray selection
	rectangle as long as mouse is held down.  If user draws non-empty rect,
	then add it to the selection region and redraw the region's outline.
	If the shift-key was down, then subtract the selection region instead
	and redraw.
*/

static void Mouse (thePt, t, mods)
Point	thePt;
Longint	t;
Integer	mods;

{
Rect		r;
RgnHandle	rgn;

	r = rgnWind->portRect;
	if (thePt.h >= r.right - 15)		/* must not click in right edge */
		return;
	if ((unsigned)(t - selectWhen) <= (unsigned)(GetDblTime()))	/* it's a double-click */
	{
		selectWhen = 0L;		/* don't take next click as dbl-click */
		SetWindClip (rgnWind);
		EraseRgn (selectRgn);
		ResetWindClip ();
		SetEmptyRgn (selectRgn);	/* clear region */
	}
	else
	{
		selectWhen = t;				/* update click variables */
		selectWhere = thePt;
		DoSelectRect (thePt, &r);	/* draw selection rectangle */
		if (!EmptyRect (&r))
		{
			EraseRgn (selectRgn);
			selectWhen = 0L;
			rgn = NewRgn ();
			RectRgn (rgn, &r);
			if ((mods & shiftKey) != 0)		/* test shift key */
				DiffRgn (selectRgn, rgn, selectRgn);
			else
				UnionRgn (selectRgn, rgn, selectRgn);
			DisposeRgn (rgn);
		}
	}
}


static Idle ()
{

	SetWindClip (rgnWind);
	MarqueeRgn (selectRgn);	/* draw selection region outline */
	ResetWindClip ();		/* restore previous clipping */
	return 0;
}


/*
	Redraw the current region.  If the window was resized, resize
	the region to fit.
*/

static Update (resized)
Boolean	resized;
{
Rect	r;

	EraseRect (&rgnWind->portRect);
	if (resized)
	{
		r = rgnWind->portRect;
		rgnPortRect.right -= 15;	/* don't use right edge of window */
		r.right -= 15;
		MapRgn (selectRgn, &rgnPortRect, &r);
		rgnPortRect = rgnWind->portRect;
	}
	DrawGrowBox (rgnWind);
	Idle ();
	return 0;
}


static Activate (active)
Boolean	active;
{
	DrawGrowBox (rgnWind);
	if (active)
		DisableItem (editMenu, 0);
	else
		EnableItem (editMenu, 0);
	DrawMenuBar ();
	return 0;
}


MarqueeRgn (r)
RgnHandle	r;
{
PenState	p;
Byte		b;
Integer		i;

	GetPenState (&p);
	PenPat ((ConstPatternParam)&marqueePat);
	PenMode (patCopy);
	FrameRgn (r);
	SetPenState (&p);
	b = marqueePat.pat[0];		/* shift pattern for next call */
	for (i = 0; i < 7; ++i)
		marqueePat.pat[i] = marqueePat.pat[i+1];
	marqueePat.pat[7] = b;
	return 0;
}


/*
	While mouse is down, draw gray selection rectangle in the current
	port.  Return the resultant rect in dstRect.  The rect is always
	clipped to the current portRect.
*/

DoSelectRect (startPoint, dstRect)
Point	startPoint;
Rect	*dstRect;
{
Point		pt, dragPt;
Rect		rClip;
GrafPtr		thePort;
Boolean		result;
PenState	ps;
Integer		i;

	GetPort (&thePort);
	rClip = thePort->portRect;
	rClip.right -= 15;
	GetPenState (&ps);
	PenPat ((ConstPatternParam)&qd.gray);
	PenMode (patXor);
	dragPt = startPoint;
	Pt2Rect (dragPt, dragPt, dstRect);
	FrameRect (dstRect);
	for (;;)
	{
		GetMouse (&pt);
		if (!EqualPt (pt, dragPt))	/* mouse has moved, change region */
		{
			FrameRect (dstRect);
			dragPt = pt;
			Pt2Rect (dragPt, startPoint, dstRect);
			result = SectRect (dstRect, &rClip, dstRect);
			FrameRect (dstRect);
			for (i = 0; i < 1000; ++i) { /* empty */ }
		}
		if (!StillDown ()) break;
	}
	FrameRect (dstRect);	/* erase last rect */
	SetPenState (&ps);
	return 0;
}



RgnWindInit ()
{
	rgnWind = GetNewWindow (rgnWindRes, nil, (WindowPtr)-1L);
	(void) SkelWindow (rgnWind,
		 (void*)Mouse,		/* draw rectangles */
				nil,		/* ignore keyclicks */
				Update,
				Activate,
				nil,		/* no close proc */
				Clobber,	/* disposal proc */
				Idle,		/* idle proc */
				true);

	rgnPortRect = rgnWind->portRect;
	selectRgn = NewRgn ();	/* selected region empty initially */
	selectWhen = 0L;		/* first click can't be taken as dbl-click */
	return 0;
}
