/*
	TransSkel multiple-window demonstration: ZoomRect module

	This module handles a window in which successive randomly generated
	rectangles are smoothly interpolated into one another.  The display
	is white on black, which results in some interesting problems (see
	ZDrawGrowBox, for instance).  The display adjusts itself to the size
	of the window, so that the zoom series always lie entirely within
	the window.  Clicking the mouse in the window pauses the display until
	the button is released.

	21 April 1988		Paul DuBois
*/

# include	"MultSkel.h"


# define	zoomSteps	15		/* # rects in interpolative series */

WindowPtr		zoomWind;
static Rect		zRect[zoomSteps];	/* set of interpolated rectangles */
static Rect		zSrcRect;
static Integer	sizeX;				/* size of window in pixels */
static Integer	sizeY;

/*  Prototypes for local functions*/

SetWindClip (WindowPtr wind);  					// Defined in multskel.c
ResetWindClip ();						  		// Defined in multskel.c
SkelWindow (WindowPtr theWind, ProcPtr pMouse, ProcPtr pKey, ProcPtr pUpdate, 
			ProcPtr pActivate, ProcPtr pClose, ProcPtr pClobber, ProcPtr pIdle,
			Boolean	frontOnly );	// Defined in transkel.c
DrawGrowBox (WindowPtr wind);		// Defined in multskel.c		

Rand (Integer max);
ZoomRect (Rect r1, Rect r2);

SetZoomSize ()
{
Rect	r;

	r = zoomWind->portRect;
	r.right -= 15;				/* don't use right edge */
	sizeX = r.right;
	sizeY = r.bottom;
	return 0;
}


/*
	return integer between zero and max (inclusive).  assumes max is
	non-negative.
*/

Rand (max)
Integer	max;
{
register Integer	t;

	t = Random ();
	if (t < 0) t = -t;
	return (t % (short)(max + 1));
};


/*
	Interpolate one rectangle smoothly into another.  Erase the previous
	series as the new one is drawn.
*/

ZoomRect (r1, r2)
Rect	r1, r2;

{
register Integer	r1left, r1top;
register Integer	l, t;
register Integer	j;
Integer				hDiff, vDiff, widDiff, htDiff;
Integer				r, b;
Integer				rWid, rHt;


	r1left = r1.left;
	r1top = r1.top;
	hDiff = r2.left - r1left;	/* positive if moving to right */
	vDiff = r2.top - r1top;		/* positive if moving down */
	rWid = r1.right - r1left;
	rHt = r1.bottom - r1top;
	widDiff = (r2.right - r2.left) - rWid;
	htDiff = (r2.bottom - r2.top) - rHt;
/*
	order of evaluation is important in the rect coordinate calculations.
	since all arithmetic is integer, you can't save time by calculating
	j/zoomSteps and using that - it'll usually be zero.
*/
	for (j = 1; j <= zoomSteps; j++)
	{
		FrameRect (&zRect[j-1]);				/* erase a rectangle */
		l = r1left + (hDiff * j) / zoomSteps;
		t = r1top + (vDiff * j) / zoomSteps;
		r = l + rWid + (widDiff * j) / zoomSteps;
		b = t + rHt + (htDiff * j) / zoomSteps;
		SetRect (&zRect[j-1], l, t, r, b);
		FrameRect (&zRect[j-1]);
	}

	return 0;
}


static Idle ()
{
Point	pt1, pt2;
Rect	dstRect;

	SetPt (&pt1, (short)Rand(sizeX), (short)Rand(sizeY));	/* generate new rect */
	SetPt (&pt2, (short)Rand(sizeX), (short)Rand(sizeY));	/* and zoom to it */
	Pt2Rect (pt1, pt2, &dstRect);
	SetWindClip (zoomWind);			/* don't draw in right edge */
	ZoomRect (zSrcRect, dstRect);
	ResetWindClip ();
	zSrcRect = dstRect;
	return 0;
}


/*
	just pause zoom display while mouse down
*/

static Mouse (thePt, t, mods)
Point	thePt;
Longint	t;
Integer	mods;

{
	while (StillDown ()) {  /* wait until mouse button released */ }
	return 0;
}


/*
	Draw the grow box in white on black.  This is tricky:  if the window
	is inactive, the grow box will be drawn black, as it should be.  But
	if the window is active, the box will STILL be drawn black on white!
	So have to check whether the window is active or not.  The test for
	active has to be done carefully:  the window manager stores 255 and 0
	for true and false, not real boolean values.
*/

ZDrawGrowBox ()
{
Rect	r;

	PenMode (notPatCopy);
	DrawGrowBox (zoomWind);
	PenMode (patXor);
	if ( ((WindowPeek) zoomWind)->hilited)	/* grow box draw in white */
	{										/* no matter what if active */
		r = zoomWind->portRect;				/* - invert to fix */
		r.left = r.right - 14;
		r.top = r.bottom - 14;
		InvertRect (&r);
	}

	return 0;
}


static Update (resized)
Boolean	resized;
{
Integer	i;

	EraseRect (&zoomWind->portRect);
	ZDrawGrowBox ();
	SetWindClip (zoomWind);
	for (i = 0; i < zoomSteps; ++i)
		FrameRect (&zRect[i]);
	ResetWindClip ();
	if (resized)
		SetZoomSize ();		/* adjust to new window size */
	return 0;
}


static Activate (active)
Boolean	active;
{

	ZDrawGrowBox ();
	if (active)
		DisableItem (editMenu, 0);
	else
		EnableItem (editMenu, 0);
	DrawMenuBar ();
	return 0;
}


static Halt ()
{
	CloseWindow (zoomWind);
	return 0;
}


ZoomWindInit ()
{
Integer	i;
int foo;
foo = patXor;

	zoomWind = GetNewWindow (zoomWindRes, nil, (WindowPtr)-1L);
	(void) SkelWindow (zoomWind,
				Mouse,		/* pause while button down */
				nil,		/* ignore key clicks */
				Update,
				Activate,
				nil,		/* no close proc */
				Halt,		/* when done with window */
				Idle,		/* draw a new series */
				true);		/* run only when frontmost */
   foo = patXor;

	SetZoomSize ();
foo = patXor;
	BackPat ((ConstPatternParam)&qd.black);
foo = patXor;
	PenMode (patXor);
	SetRect (&zSrcRect, 0, 0, 0, 0);
	for (i = 0; i < zoomSteps; ++i)		/* initialize rect array */
		zRect[i] = zSrcRect;
	return 0;
}
