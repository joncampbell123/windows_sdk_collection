/*
	TransSkel multiple-window demonstration: Help module

	This module handles a help window, in which text may be scrolled but
	not edited.  A TextEdit record is used to hold the text, though.

	21 April 1988		Paul DuBois
*/

# include	"MultSkel.h"
# include	<macos\Controls.h>
# include	<macos\Memory.h>
# include	<macos\Resource.h>
# include	<macos\TextEdit.h>

#define rtTEXT  0x54455854L



WindowPtr				helpWind;
static TEHandle			teHelp;		/* handle to help window TextEdit record */
static ControlHandle	helpScroll;	/* help window scroll bar */
static Integer			helpLine;	/* line currently at top of window */
static Integer			halfPage;	/* number of lines in half a window */

/*  Prototypes for local functions*/

SkelWindow (WindowPtr theWind, ProcPtr pMouse, ProcPtr pKey, ProcPtr pUpdate, 
			ProcPtr pActivate, ProcPtr pClose, ProcPtr pClobber, ProcPtr pIdle,
			Boolean	frontOnly );	// Defined in transkel.c
DrawGrowBox (WindowPtr wind);		// Defined in multskel.c		


static Halt ()
{
	TEDispose (teHelp);
	DisposeControl (helpScroll);
	CloseWindow (helpWind);
	return 0;
}


/*
	Scroll to the correct position.  lDelta is the
	amount to CHANGE the current scroll setting by.
*/

DoScroll (lDelta)
Integer	lDelta;
{
Integer	newLine;

	newLine = helpLine + lDelta;
	if (newLine < 0)
		newLine = 0;
	if (newLine > GetCtlMax (helpScroll))
		newLine = GetCtlMax (helpScroll);
	SetCtlValue (helpScroll, newLine);
	lDelta = (helpLine - newLine ) * (**teHelp).lineHeight;
	TEScroll (0, lDelta, teHelp);
	helpLine = newLine;
	return 0;
}


/*
	Filter proc for tracking mousedown in scroll bar.  The part code
	of the part originally hit is stored as the control's reference
	value.

	The "void" had better be there!  Otherwise Lightspeed will treat
	it as an integer function, not a procedure.
*/

void _pascal TrackScroll (theScroll, partCode)
ControlHandle	theScroll;
Integer			partCode;
{
Integer			lDelta;

	if (partCode == GetCRefCon (theScroll))	/* still in same part? */
	{
		switch (partCode)
		{
			case inUpButton: lDelta = -1; break;
			case inDownButton: lDelta = 1; break;
			case inPageUp: lDelta = -halfPage; break;
			case inPageDown: lDelta = halfPage; break;
		}
		DoScroll (lDelta);
	}
}


/*
	Handle hits in scroll bar
*/

static Mouse (thePt, t, mods)
Point	thePt;
Longint	t;
Integer	mods;
{
Integer	thePart;

		if ((thePart = TestControl (helpScroll, thePt)) == inThumb)
		{
			(void) TrackControl (helpScroll, thePt, nil);
			DoScroll (GetCtlValue (helpScroll) - helpLine);
		}
		else if (thePart != 0)
		{
			SetCRefCon (helpScroll, (Longint) thePart);
			(void) TrackControl (helpScroll, thePt, (ControlActionUPP)TrackScroll);
}
return 0;
}


/*
	Update help window.  The update event might be in response to a
	window resizing.  If so, resize the rects and recalc the linestarts
	of the text.  To resize the rects, only the right edge of the
	destRect need be changed (the bottom is not used, and the left and
	top should not be changed). The viewRect should be sized to the
	screen.  Pull text down if necessary to fill window.
*/

static Update (resized)
Boolean	resized;
{
Rect	r;
Integer	visLines;
Integer	lHeight;
Integer	topLines;
Integer	nLines;
Integer	scrollLines;

	r = helpWind->portRect;
	EraseRect (&r);
	if (resized)
	{
		r.left += 4;
		r.bottom -= 2;
		r.top += 2;
		r.right -= 19;
		(**teHelp).destRect.right = r.right;
		(**teHelp).viewRect = r;
		TECalText (teHelp);
		lHeight = (**teHelp).lineHeight;
		nLines = (**teHelp).nLines;
		visLines = (r.bottom - r.top) / lHeight;
		halfPage = visLines / 2;
		topLines = (r.top - (**teHelp).destRect.top) / lHeight;
		scrollLines = visLines - (nLines - topLines);
		if (scrollLines > 0 && topLines > 0)
		{
			if (scrollLines > topLines)
				scrollLines = topLines;
			TEScroll (0, (short)(scrollLines * lHeight), teHelp);
		}
		scrollLines = nLines - visLines;
		helpLine = (r.top - (**teHelp).destRect.top) / lHeight;
/*
	move and resize the scroll bar as well.  The ValidRect call is done
	because the HideControl adds the control bounds box to the update
	region - which would generate another update event!  Since everything
	gets redrawn below, the ValidRect is used to cancel the update.
*/

		HideControl (helpScroll);
		r = helpWind->portRect;
		r.left = r.right - 15;
		r.bottom -= 14;
		--r.top;
		++r.right;
		SizeControl (helpScroll, (short)(r.right-r.left), (short)(r.bottom-r.top));
		MoveControl (helpScroll, r.left, r.top);
		SetCtlMax (helpScroll, (short)(nLines - visLines < 0 ? 0 : nLines-visLines));
		SetCtlValue (helpScroll, helpLine);
		/*if (scrollLines <= 0)
			HiliteControl (helpScroll, (scrollLines > 0 ? 0 : 255));*/
		ShowControl (helpScroll);
		/*if (GetCtlValue (helpScroll) > scrollLines)
			DoScroll (GetCtlValue (helpScroll) - scrollLines);*/
	}
	DrawGrowBox (helpWind);
	DrawControls (helpWind);	/* redraw scroll bar */
	r = (**teHelp).viewRect;
	TEUpdate (&r, teHelp);		/* redraw text display */
	ValidRect (&helpWind->portRect);
	return 0;
}


/*
	When the window comes active, disable the Edit menu and highlight
	the scroll bar if there are any lines not visible in the content
	region.  When the window is deactivated, enable the Edit menu and
	un-highlight the scroll bar.
*/

static Activate (active)
Boolean	active;
{
	DrawGrowBox (helpWind);
	if (active)
	{
		DisableItem (editMenu, 0);
		HiliteControl (helpScroll, (short)(GetCtlMax (helpScroll) > 0 ? 0 : 255));
	}
	else
	{
		EnableItem (editMenu, 0);
		HiliteControl (helpScroll, 255);
	}
	DrawMenuBar ();
	return 0;
}


HelpWindInit ()
{
Rect	r;
Handle	textHandle;
Integer	visLines;
Integer	scrollLines;

	helpWind = GetNewWindow (helpWindRes, nil, (WindowPtr)-1L);
	(void) SkelWindow (helpWind, Mouse, nil, Update,
				Activate, nil, Halt, nil, true);

	TextFont (0);
	TextSize (0);

	r = helpWind->portRect;
	r.left += 4;
	r.bottom -= 2;
	r.top += 2;
	r.right -= 19;
	teHelp = TENew (&r, &r);
	textHandle = (Handle)GetResource (rtTEXT, helpTextRes);	/* read help text */
	HLock (textHandle);		/* lock it and insert into TERec */
	TEInsert (*textHandle, GetHandleSize (textHandle), teHelp);
	HUnlock (textHandle);
	ReleaseResource (textHandle);	/* done with it, so goodbye */
/*
	Now figure out how many lines will fit in the window and how many
	will not.  Determine the number of lines in half a window for use
	in tracking clicks in the page up and page down regions of the
	scroll bar.  Then create the scroll bar .  Make sure the borders
	overlap the window frame and the frame of the grow box.
*/
	visLines = (r.bottom - r.top) / (**teHelp).lineHeight;
	scrollLines = (**teHelp).nLines - visLines;
	halfPage = visLines / 2;
	helpLine = 0;
	r = helpWind->portRect;
	r.left = r.right - 15;
	r.bottom -= 14;
	--r.top;
	++r.right;
/*
	Build the scroll bar.  Don't need to bother testing whether to
	highlight it or not, since that will be done in response to the
	activate event.
*/
	helpScroll = NewControl (helpWind, &r, "\000", true,
					helpLine, 0, scrollLines, scrollBarProc, 0L);

/*
	GetNewWindow generates an update event for entire portRect.
	Cancel it, since the everything has been drawn already,
	except for the grow box (which will be drawn in response
	to the activate event).
*/
	ValidRect (&helpWind->portRect);

	return 0;
}
