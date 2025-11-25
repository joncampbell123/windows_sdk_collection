/*
	TransSkel version 2.01 - Transportable application skeleton
	
	TransSkel is public domain and was originally written by:

				Paul DuBois
				Wisconsin Regional Primate Research Center
				1220 Capital Court
				Madison WI  53715-1299  USA

	UUCP:		{allegra,uunet}!uwvax!rhesus!dubois
	Internet:	dubois@primate.wisc.edu
			
	Additional changes were made by
		
				Owen Hartnett
				OHM Software Company
				163 Richard Drive
				Tiverton, RI 02878  USA
			
	UUCP:		{allegra,uunet}!brunix!omh	
	Internet:	omh@cs.brown.edu
	CSNET:		omh@cs.brown.edu.CSNET 		

	Owen is also responsible for the port to Lightspeed Pascal.
	
	This version of TransSkel written for LightspeedC.  LightspeedC is a
	trademark of:
			THINK Technologies, Inc
			420 Bedford Street  Suite 350
			Lexington, MA  02173  USA

	Change history is in TSHistory.c
*/


/*
	The following symbol controls support for dialogs.
	"#define	supportDialogs" enables support.
	"#undef		supportDialogs" disables support.
*/

#include <macos\msvcmac.h>	/* Must be included before any macos files */

/* to work with new headers */
#define dangerousPattern

# define	supportDialogs


# ifdef		supportDialogs
#	include	<macos\dialogs.h>
# else
#	include	<macos\windows.h>
# endif

# include	<macos\Events.h>
# include	<macos\Menus.h>
# include	<macos\ToolUtil.h>
# include	<macos\Resource.h>
# include	<macos\Memory.h>
# include	<macos\Desk.h>
# include	<macos\DiskInit.h>
# include	<macos\OsEvents.h>
# include	<macos\QuickDra.h>
# include	<macos\Fonts.h>

# include	"TranSkel.h"

#define rtDRVR  0x44525652

/*
	Integer and Longint should be typedef'd to the 2-byte and 4-byte
	integer types for the compiler being used.  These are int and long
	for LightspeedC.
*/

typedef short	Integer;
typedef long	Longint;


# define	mBarHeight	20	/* menu bar height.  All window sizing
							   code takes this into account */


/*
	New(TypeName) returns handle to new object, for any TypeName.
	If there is insufficient memory, the result is nil.
*/

# define	New(x)	(x **) NewHandle ((Size) sizeof (x))



/*
	Window and Menu handler types, constants, variables.

	whList and mhList are the lists of window and menu handlers.
	whClobOnRmve and mhClobOnRmve are true if the handler disposal proc
	is to be called when a handler is removed.  They are temporarily set
	false when handlers are installed for windows or menus that already
	have handlers - the old handler is removed WITHOUT calling the
	disposal proc.

	Default lower limits on window sizing of 80 pixels both directions is
	sufficient to allow text windows room to draw a grow box and scroll
	bars without having the thumb and arrows overlap.  These values may
	be changed if such a constraint is undesirable with SkelGrowBounds.
	Default upper limits are for the Macintosh, not the Lisa, but are set
	per machine in SkelInit.
*/

typedef struct WHandler	WHandler;

struct WHandler
{
	WindowPtr	whWind;			/* window/dialog to be handled  */
	ProcPtr		whClobber;		/* data structure disposal proc */
	ProcPtr		whMouse;		/* mouse-click handler proc     */
	ProcPtr		whKey;			/* key-click handler proc       */
	ProcPtr		whUpdate;		/* update handler proc          */
	ProcPtr		whActivate;		/* activate event handler proc  */
	ProcPtr		whClose;		/* close "event" handler proc   */
	ProcPtr		whIdle;			/* main loop proc               */
# ifdef	supportDialogs
	ProcPtr		whEvent;		/* event proc                   */
# endif
	Rect		whGrow;			/* limits on window sizing      */
	Boolean		whSized;		/* true = window was resized    */
	Boolean		whFrontOnly;	/* true = idle only when active */
	WHandler	**whNext;		/* next window handler          */
};

static WHandler	**whList = nil;
static Boolean	whClobOnRmve = true;
static Rect		growRect = { 80, 80, 512, 342 - mBarHeight };

typedef struct MHandler	MHandler;

struct MHandler
{
	Integer		mhID;			/* menu id                     */
	ProcPtr		mhSelect;		/* item selection handler proc */
	ProcPtr		mhClobber;		/* menu disposal handler proc  */
	MHandler	**mhNext;		/* next menu handler           */
};


static MHandler	**mhList = nil;			/* list of menu handlers */
static Boolean	mhClobOnRmve = true;


/*
	Variables for default Apple menu handler.  appleID is set to 1 if
	SkelApple is called and is the id of the Apple menu, appleAboutProc
	is the procedure to execute if there is an About... item and it's
	chosen from the Apple menu.  If doAbout is true, then the menu
	contains the About... item, otherwise it's just desk accessories.
*/

static MenuHandle	appleMenu;
static Integer		appleID = 0;
static ProcPtr		appleAboutProc = nil;
static Boolean		doAbout = false;


/*
	Miscellaneous

	screenPort points to the window manager port.

	doneFlag determines when SkelMain returns.  It is set by calling
	SkelWhoa(), which is how the host requests a halt.

	pBkgnd points to a background procedure, to be run during event
	processing.  Set it with SkelBackground.  If nil, there's no
	procedure.

	pEvent points to an event-inspecting hook, to be run whenever an
	event occurs.  Set it with SkelEventHook.  If nil, there's no
	procedure.

	eventMask controls the event types requested in the GetNextEvent
	call in SkelMain.

	diskInitPt is the location at which the disk initialization dialog
	appears, if an uninitialized disk is inserted.
*/

static GrafPtr	screenPort;
static Integer	doneFlag = false;
static ProcPtr	pBkgnd = nil;
static Boolean	(*pEvent)() = nil;
static Integer	eventMask = (Integer)everyEvent;
static Point	diskInitPt = { /* v = */ 120, /* h = */ 100 };

static WindowPtr	oldWindow = nil; 	
static WHandler		**oldWDHandler = nil;
	

# ifdef	supportDialogs

/*
	dlogEventMask specifies events that are passed to dialogs.
	Others are ignored.  Standard mask passes, mousedown, keydown,
	autokey, update, activate and null events.  Null events are
	controlled by bit 0 (always forced on).
*/

static Integer	dlogEventMask = 0x16b;

# endif

/* -------------------------------------------------------------------- */
/*						Internal (private) Routines						*/
/* -------------------------------------------------------------------- */

// local prototypes to keep the compiler happy
static DoActivate (WHandler **h, Boolean active);
static DoClobber (WHandler **h);
static DoClose (WHandler **h);
static DoDialog (register EventRecord *theEvent);
static void DoEvent (register EventRecord *theEvent);
static DoGrow (WHandler **h, GrafPtr growPort, Point startPt);
static DoKey (WHandler **h, char ch, Integer mods);
static DoMenuCommand (Longint command);
static DoMouse (WHandler **h, EventRecord *theEvent);
static DoUpdate (WHandler **h);
static DoZoom (register WHandler **h, GrafPtr zoomPort, short partCode);
static WHandler **GetDHandler (DialogPtr theDialog);
static WHandler **GetWDHandler (WindowPtr theWind);
static WHandler **GetWHandler (WindowPtr theWind);
static TriggerUpdate (WHandler **h, GrafPtr grownPort);

/*
	Get handler associated with user or dialog window.
	Return nil if window doesn't belong to any known handler.
	This routine is absolutely fundamental to TransSkel.
*/


static WHandler **GetWDHandler (theWind)
WindowPtr	theWind;
{
register WHandler	**h;

	if (theWind == oldWindow) 
		return(oldWDHandler);		/* return handler of cached window */

	for (h = whList; h != nil; h = (**h).whNext)
	{
		if ((**h).whWind == theWind)
		{
			oldWindow = theWind;	/* set cached window and handler */
			oldWDHandler = h;
			return (h);
		}
	}
	return (nil);
}


/*
	Get handler associated with user window.
	Return nil if window doesn't belong to any known handler.
	The order of the two tests is critical:  theWind might be nil.
*/

static WHandler **GetWHandler (theWind)
WindowPtr	theWind;
{
register WHandler	**h;

	if ((h = GetWDHandler (theWind)) != nil
		&& ((WindowPeek) theWind)->windowKind != dialogKind)
	{
			return (h);
	}
	return (nil);
}


# ifdef	supportDialogs

/*
	Get handler associated with dialog window.
	Return nil if window doesn't belong to any known handler.
	The order of the two tests is critical:  theDialog might be nil.
*/

static WHandler **GetDHandler (theDialog)
DialogPtr	theDialog;
{
register WHandler	**h;

	if ((h = GetWDHandler (theDialog)) != nil
		&& ((WindowPeek) theDialog)->windowKind == dialogKind)
	{
			return (h);
	}
	return (nil);
}

# endif


/*
	General menu-handler.  Just passes selection to the handler's
	select routine.  If the select routine is nil, selecting items from
	the menu is a nop.
*/

static DoMenuCommand (command)
Longint		command;
{
register Integer	menu;
register Integer	item;
register MHandler	**mh;
register ProcPtr	p;

	menu = HiWord (command);
	item = LoWord (command);
	for (mh = mhList; mh != nil; mh = (**mh).mhNext)
	{
		if ((menu == (**mh).mhID) && ((p = (**mh).mhSelect) != nil))
		{
			(*p) (item);
			break;
		}
	}
	HiliteMenu (0);		/* command done, turn off menu hiliting */
	
	return 0;
}


/*
	Apple menu handler
	
	DoAppleItem:  If the first item was chosen, and there's an "About..."
	item, call the procedure associated with it (if not nil).  If there
	is no "About..." item or the item was not the first one, then open
	the associated desk accessory.  The port is saved and restored
	because OpenDeskAcc does not always preserve it correctly.
	
	DoAppleClobber disposes of the Apple menu.
*/


static DoAppleItem (item)
Integer	item;
{
GrafPtr	curPort;
Str255	str;
Handle	h;

	if (doAbout && item == 1)
	{
		if (appleAboutProc != nil)
			(*appleAboutProc) ();
	}
	else
	{
		GetPort (&curPort);
		GetItem (appleMenu, item, str);		/* get DA name */
		SetResLoad (false);
		h = GetNamedResource (rtDRVR, str);
		SetResLoad (true);
		if (h != nil)
		{
			ResrvMem (SizeResource (h) + 0x1000);
			(void) OpenDeskAcc (str);			/* open it */
		}
		SetPort (curPort);
	}
	
	return 0;
}

static DoAppleClobber () 
{ 
	DisposeMenu (appleMenu);
	return 0;
}


/* -------------------------------------------------------------------- */
/*						Window-handler routing routines					*/
/*																		*/
/*	See manual for discussion of port-setting behavior.  In general,	*/
/*	the current port is made to associate with the active window.		*/
/*	This is done in DoActivate for non-dialog windows, in DoDialog		*/
/*	for dialog windows.													*/
/* -------------------------------------------------------------------- */


/*
	Pass local mouse coordinates, click time, and the modifiers flag
	word to the handler.  Should not be necessary to set the port, as
	the click is passed to the active window's handler.
*/

static DoMouse (h, theEvent)
WHandler	**h;
EventRecord	*theEvent;
{
register ProcPtr	p;
Point				thePt;

	if (h != nil)
	{
		if ((p = (**h).whMouse) != nil)
		{
			thePt = theEvent->where;	/* make local copy */
			GlobalToLocal (&thePt);
			(*p) (thePt, theEvent->when, theEvent->modifiers);
		}
	}
	
	return 0;
}


/*
	Pass the character and the modifiers flag word to the handler.
	Should not be necessary to set the port, as the click is passed
	to the active window's handler.
*/

static DoKey (h, ch, mods)
WHandler	**h;
char		ch;
Integer		mods;
{
register ProcPtr	p;

	if (h != nil)
	{
		if ((p = (**h).whKey) != nil)
			(*p) (ch, mods);
	}
	
	return 0;
}


/*
	Call the window updating procedure, passing to it an indicator whether
	the window has been resized or not.  Then clear the flag, assuming
	the update proc took whatever action was necessary to respond to
	resizing.

	If the handler doesn't have any update proc, the Begin/EndUpdate
	stuff is still done, to clear the update region.  Otherwise the
	Window Manager will keep generating update events for the window,
	stalling updates of other windows.

	Make sure to save and restore the port, as it's not always the
	active window that is updated.
*/

static DoUpdate (h)
WHandler	**h;
{
register WHandler	**rh;
register ProcPtr	p;
register GrafPtr	updPort;
GrafPtr				tmpPort;

	if ((rh = h) != nil)
	{
		GetPort (&tmpPort);
		SetPort (updPort = (**rh).whWind);
		BeginUpdate (updPort);
		if ((p = (**rh).whUpdate) != nil)
		{
			(*p) ((**rh).whSized);
			(**rh).whSized = false;
		}
		EndUpdate (updPort);
		SetPort (tmpPort);
	}
	
	return 0;
}


/*
	Pass activate/deactivate notification to handler.  On activate,
	set the port to the window coming active.  Normally this is done by
	the user clicking in a window.

	*** BUT ***
	Under certain conditions, a deactivate may be generated for a window
	that has not had the port set to it by a preceding activate.  If an
	application puts up window A, then window B in front of A, then
	starts processing events, the first events will be a deactivate for A
	and an activate for B.  Since it therefore can't be assumed the port
	was set to A by an activate, the port needs to be set for deactivates
	as well.

	This matters not a whit for the more usual cases that occur.  If a
	deactivate for one window is followed by an activate for another, the
	port will still be switched properly to the newly active window.  If
	no activate follows the deactivate, the deactivated window is the last
	one, and it doesn't matter what the port ends up set to, anyway.
*/

static DoActivate (h, active)
WHandler	**h;
Boolean		active;
{
register ProcPtr	p;

	if (h != nil)
	{
		SetPort ((**h).whWind);
		if ((p = (**h).whActivate) != nil)
			(*p) (active);
	}
	
	return 0;
}


/*
	Execute a window handler's close box proc.  The close proc for
	handlers for temp windows that want to remove themselves when the
	window is closed can call SkelRmveWind to dispose of the window
	and remove the handler from the window handler list.  Thus, windows
	may be dynamically created and destroyed without filling up the
	handler list with a bunch of invalid handlers.
	
	If the handler doesn't have a close proc, just hide the window.
	The host should provide some way of reopening the window (perhaps
	a menu selection).  Otherwise the window will be lost from user
	control if it is hidden, since it won't receive user-initiated
	events.

	This is called both for regular and dialog windows.

	Since the close box of only the active window may be clicked, it
	is not necessary to set the port.
*/

static DoClose (h)
WHandler	**h;
{
register WHandler	**rh;
register ProcPtr	p;

	if ((rh = h) != nil)
	{
		if ((p = (**rh).whClose) != nil)
			(*p) ();
		else
			HideWindow ((**rh).whWind);
	}
	
	return 0;
}


/*
	Execute a window handler's clobber proc.  This is called both
	for regular and dialog windows.

	Must save, set and restore port, since any window (not just active
	one) may be clobbered at any time.

	Don't need to check whether handler is nil, as in other handler
	procedures, since this is only called by SkelRmveWind with a
	known-valid handler.
*/

static DoClobber (h)
WHandler	**h;
{
register ProcPtr	p;
GrafPtr				tmpPort;

	GetPort (&tmpPort);
	SetPort ((**h).whWind);
	if ((p = (**h).whClobber) != nil)
		(*p) ();
	SetPort (tmpPort);
	return 0;
}


# ifdef	supportDialogs

/* -------------------------------------------------------------------- */
/*							Dialog-handling routines					*/
/* -------------------------------------------------------------------- */


/*
	Handle event if it's for a (modeless) dialog.  The event must be one
	of those that is passed to dialogs according to dlogEventMask.
	This mask can be set so that disk-inserts, for instance, don't
	get eaten up.

	Examine event and set port if dialog window is coming active (for
	normal windows, DoActivate sets the port; there's no such thing
	for dialogs, so it's done here.)  When this is done, the trio
	of GetPort/SetPort/SetPort calls commented out below doesn't appear
	to be necessary any longer.  If you want to be cautious, it doesn't
	hurt to uncomment them...
*/

static DoDialog (theEvent)
register EventRecord	*theEvent;
{
register WHandler	**dh;
DialogPtr			theDialog;
register Integer	what;
Integer				item;
/*GrafPtr			tmpPort;*/
WindowPeek			w;

/*
	handle command keys before they get to IsDialogEvent
*/

	what = theEvent->what;
	if((what == keyDown || what == autoKey) && (theEvent->modifiers & cmdKey))
	{
	   	DoMenuCommand (MenuKey ((char)(theEvent->message & charCodeMask)));
	   	return (true);
	}
	
	if (((1 << what) & dlogEventMask) && IsDialogEvent (theEvent))
	{
		/* ugly programming award semi-finalist follows */
		if (theEvent->what == activateEvt			/* if activate */
			&& (theEvent->modifiers & activeFlag)	/* and coming active */
			&& (w=(WindowPeek) theEvent->message)->windowKind
					== dialogKind)
		{
			SetPort ((GrafPtr) w);
		}
		if (DialogSelect (theEvent, &theDialog, &item)
		   && (dh = GetDHandler (theDialog)) != nil
		   && (**dh).whEvent != nil)
		{
			/*GetPort (&tmpPort);*/
			/*SetPort (theDialog);*/
			(*(**dh).whEvent) (item, theEvent);
			/*SetPort (tmpPort);*/
		}
		return (true);
	}
	return (false);
}

# endif


/* -------------------------------------------------------------------- */
/*							Event-handling routines						*/
/* -------------------------------------------------------------------- */


/*
	Have either zoomed a window or sized it manually.  Invalidate
	it to force an update and set the 'resized' flag in the window
	handler true.  The port is assumed to be set to the port that changed
	size.
*/

static TriggerUpdate (h, grownPort)
WHandler	**h;
GrafPtr		grownPort;
{
	InvalRect (&grownPort->portRect);
	if (h != nil)
		(**h).whSized = true;
	
	return 0;
}


/*
	Size a window.  If the window has a handler, use the grow limits
	in the handler record, otherwise use the defaults.

	The portRect is invalidated to force an update event.  The handler's
	update procedure should check the parameter passed to it to check
	whether the window has changed size, if it needs to adjust itself to
	the new size.  THIS IS A CONVENTION.  Update procs must notice grow
	"events", there is no procedure specifically for such events.
	
	The clipping rectangle is not reset.  If the host application
	keeps the clipping set equal to the portRect or something similar,
	then it will have to arrange to treat window growing with more
	care.

	Since the grow region of only the active window may be clicked,
	it should not be necessary to set the port.
*/

static DoGrow (h, growPort, startPt)
WHandler	**h;
GrafPtr		growPort;
Point		startPt;
{
Rect				r;
register Longint	growRes;

	if (h != nil)
		r = (**h).whGrow;
	else
		r = growRect;	/* use default */

	/* grow result non-zero if size change	*/

	if (growRes = GrowWindow (growPort, startPt, &r))
	{
		SizeWindow (growPort, LoWord (growRes), HiWord (growRes), false);
		TriggerUpdate (h, growPort);
	}
	
	return 0;
}


/*
	Zoom the current window.  Very similar to DoGrow

	Since the zoombox of only the active window may be clicked,
	it should not be necessary to set the port.
*/

static DoZoom (h, zoomPort, partCode)
register WHandler	**h;
GrafPtr				zoomPort;
short				partCode;
{
	ZoomWindow (zoomPort, partCode, 0);
	TriggerUpdate (h, zoomPort);
	return 0;
}


/*
	General event handler
*/

static void DoEvent (theEvent)
register EventRecord	*theEvent;

{
Point				evtPt;
GrafPtr				evtPort;
register Integer	evtPart;
register char		evtChar;
register Integer	evtMods;
register Longint	evtMsge;
register WHandler	**h;
Rect				r;

# ifdef	supportDialogs

	if(DoDialog (theEvent))
		return;

# endif

	evtPt = theEvent->where;
	evtMods = theEvent->modifiers;
	evtMsge = theEvent->message;

	switch (theEvent->what)
	{

		/*case nullEvent:
			break;*/
/*
	Mouse click.  Get the window that the click occurred in, and the
	part of the window.  GetWDHandler is called here, not GetWHandler, since
	we need the handler for a window which might turn out to be a dialog
	window, e.g., if the click is in a close box.
*/
		case mouseDown:
		{
			evtPart = FindWindow (evtPt, &evtPort);
			h = GetWDHandler (evtPort);

			switch (evtPart)
			{
/*
	Click in a desk accessory window.  Pass back to the system.
*/
				case inSysWindow:
				{
					SystemClick (theEvent, evtPort);
					break;
				}
/*
	Click in menu bar.  Track the mouse and execute selected command,
	if any.
*/
				case inMenuBar:
				{
					DoMenuCommand (MenuSelect (evtPt));
					break;
				}
/*
	Click in grow box.  Resize window.
*/
				case inGrow:
				{
					DoGrow (h, evtPort, evtPt);
					break;
				}
/*
	Click in title bar.  Drag the window around.  Leave at least
	4 pixels visible in both directions.
	Bug fix:  The window is selected first to make sure it's at least
	activated (unless the command key is down-see Inside Macintosh).
	DragWindow seems to call StillDown first, so that clicks in drag
	regions while machine is busy don't otherwise bring window to front if
	the mouse is already up by the time DragWindow is called.
*/
				case inDrag:
				{
					if (evtPort != FrontWindow () && (evtMods & cmdKey) == 0)
						SelectWindow (evtPort);
					r = screenPort->portRect;
					r.top += mBarHeight;			/* skip down past menu bar */
					InsetRect (&r, 4, 4);
					DragWindow (evtPort, evtPt, &r);
					break;
				}
/*
	Click in close box.  Call the close proc if the window has one.
*/
				case inGoAway:
				{
					if (TrackGoAway (evtPort, evtPt))
						DoClose (h);
					break;
				}

/*
	Click in zoom box.  Track the click and then zoom the window if
	necessary
*/
				case inZoomIn:
				case inZoomOut:
				{
					if (TrackBox (evtPort, evtPt, evtPart))
						DoZoom (h, evtPort, evtPart);
					break;
				}
/*
	Click in content region.  If the window wasn't frontmost (active),
	just select it, otherwise pass the click to the window's mouse
	click handler.
*/
				case inContent:
				{
					if (evtPort != FrontWindow ())
						SelectWindow (evtPort);
					else
						DoMouse (h, theEvent);
					break;
				}

			}
			break;	/* mouseDown */
		}
/*
	Key event.  If the command key was down, process as menu item
	selection, otherwise pass the character and the modifiers flags
	to the active window's key handler.

	If dialogs are supported, there's no check for command-key
	equivalents, since that would have been checked in DoDialog.
*/
		case keyDown:
		case autoKey:
		{
			evtChar = evtMsge & charCodeMask;

# ifndef	supportDialogs

			if (evtMods & cmdKey)		/* try menu equivalent */
			{
				DoMenuCommand (MenuKey (evtChar));
				break;
			}

# endif

			DoKey (GetWHandler (FrontWindow ()), evtChar, evtMods);
			break;
		}
/*
	Update a window.
*/
		case updateEvt:
		{
			DoUpdate (GetWHandler ((WindowPtr) evtMsge));
			break;
		}
/*
	Activate or deactivate a window.
*/
		case activateEvt:
		{
			DoActivate (GetWHandler ((WindowPtr) evtMsge),
						(Boolean)((evtMods & activeFlag) != 0));
			break;
		}
/*
	handle inserts of uninitialized disks
*/
		case diskEvt:
		{
			if (HiWord (evtMsge) != noErr)
			{
				DILoad ();
				(void) DIBadMount (diskInitPt, evtMsge);
				DIUnload ();
			}
			break;
		}
	}
}


/* -------------------------------------------------------------------- */
/*						Interface (public) Routines						*/
/* -------------------------------------------------------------------- */


/*
	Initialize the various Macintosh Managers.
	Set default upper limits on window sizing.
	FlushEvents does NOT toss disk insert events, so that disks
	inserted while the application is starting up don't result
	in dead drives.

	noMasters is the number of times to call MoreMasters.  gzProc is
	the address of a grow zone procedure to call if memory allocation
	problems occur.  Pass nil if none to be used.
*/

SkelInit (noMasters, gzProc)
Integer		noMasters;
GrowZoneProcPtr	gzProc;
{
	while (noMasters-- > 0)
		MoreMasters ();

	if (gzProc != nil)
		SetGrowZone (NewGrowZoneProc(gzProc));

	MaxApplZone ();
        FlushEvents ((MacOSEventMask)(everyEvent - diskMask), 0 );
	InitGraf (&qd.thePort);
	InitFonts ();
	InitWindows ();
	InitMenus ();
	TEInit ();
	InitDialogs (nil);		/* no restart proc */
	InitCursor ();
/*
	Set upper limits of window sizing to machine screen size.  Allow
	for the menu bar.
*/
	GetWMgrPort (&screenPort);
	growRect.right = screenPort->portRect.right;
	growRect.bottom = screenPort->portRect.bottom - mBarHeight;
	return 0;
}


/*
	Main loop.

	Task care of DA's with SystemTask.
	Run background task if there is one.
	If there is an event, check for an event hook.  If there isn't
	one defined, or if there is but it returns false, call the
	general event handler.  (Hook returns true if TransSkel should
	ignore the event.)
	If no event, call the "no-event" handler for the front window and for
	any other windows with idle procedures that are always supposed
	to run.  This is done in such a way that it is safe for idle procs
	to remove the handler for their own window if they want (unlikely,
	but...)  This loop doesn't check whether the window is really
	a dialog window or not, but it doesn't have to, because such
	things always have a nil idle proc.
	
	doneFlag is reset upon exit.  This allows SkelMain to be called
	repeatedly, or recursively.

	Null events are examined (in SkelMain) and passed to the event
	handler.  This is necessary to make sure, if dialogs are supported,
	that DialogSelect gets called repeatedly, or the caret won't blink
	if a dialog has any editText items.

	Null events are not passed to any event-inspecting hook that may
	be installed.
*/

SkelMain ()
{
EventRecord			theEvent;
register WHandler	**wh, **wh2;
register WindowPtr	w;
Boolean				haveEvent;
GrafPtr				tmpPort;
register ProcPtr	p;

	while (!doneFlag)
	{	
		SystemTask ();
		if (pBkgnd != nil)
			(*pBkgnd) ();

/*
	Now watch carefully.  GetNextEvent calls SystemEvent to handle some
	DA events, and returns false if the event was handled.  However, in
	such cases the event record will still have the event that occurred,
	*not* a null event, as you might reasonably expect.  So it's not
	enough to look at haveEvent.

	Previous versions figured (wrongly) that haveEvent==false meant a null
	event had occurred, and passed it through to DoEvent and DoDialog, so
	that caret-blinking in dialog TextEdit items would occur.  But cmd-key
	equivalents while DA windows were in front, in particular, allowed
	already-processed DA events to get into DoEvent (because haveEvent
	was false), and they got handled twice because when the event record
	was examined, lo and behold, it had a cmd-key event!  So now this
	logic is used:

	If have a real event, and there's no event hook or there is but it
	doesn't handle the event, OR if the "non-event" is a true nullEvent,
	then process it.
*/
			
		haveEvent = GetNextEvent (eventMask, &theEvent);

		if ((haveEvent && (pEvent == nil || (*pEvent)(&theEvent) == false))
				|| theEvent.what == nullEvent)
			DoEvent(&theEvent);

/*
	Run applicable idle procs.  Make sure to save and restore the port,
	since idle procs for the non-active window may be run.
*/

		if (!haveEvent)
		{
			GetPort (&tmpPort);
			for (wh = whList; wh != nil; wh = wh2)
			{
				wh2 = (**wh).whNext;
				w = (**wh).whWind;
				if ( (w == FrontWindow () || !(**wh).whFrontOnly ) )
				{
					SystemTask ();
					if ((p = (**wh).whIdle) != nil)
					{
						SetPort (w);
						(*p) ();
					}
				}
			}
			SetPort (tmpPort);
		}
	}
	doneFlag = false;
	return 0;
}


/*
	Tell SkelMain to stop
*/

SkelWhoa () 
{ 
	doneFlag = true;
	return 0;
}


/*
	Clobber all the menu, window and dialog handlers
*/

SkelClobber ()
{
	while (whList != nil)
		SkelRmveWind ((**whList).whWind);

	while (mhList != nil)
		SkelRmveMenu (GetMHandle((**mhList).mhID));
	
	return 0;
}


/* -------------------------------------------------------------------- */
/*						Menu-handler interface routines					*/
/* -------------------------------------------------------------------- */


/*
	Install handler for a menu.  Remove any previous handler for it.
	Pass the following parameters:

	theMenu	Handle to the menu to be handled.  Must be created by host.
	pSelect	Proc that handles selection of items from menu.  If this is
			nil, the menu is installed, but nothing happens when items
			are selected from it.
	pClobber Proc for disposal of handler's data structures.  Usually
			nil for menus that remain in menu bar until program
			termination.
	
	The menu is installed, and also drawn in the menu bar if drawBar true.
	
	Return 0 if no handler could be allocated, non-zero if successful.
*/

SkelMenu (theMenu, pSelect, pClobber, drawBar)
MenuHandle	theMenu;
ProcPtr		pSelect;
ProcPtr		pClobber;
Boolean		drawBar;
{
register MHandler	**mh;

	mhClobOnRmve = false;
	SkelRmveMenu (theMenu);
	mhClobOnRmve = true;

	if ((mh = New (MHandler)) != nil)
	{
		(**mh).mhNext = mhList;
		mhList = mh;
		(**mh).mhID = (**theMenu).menuID;	/* get menu id number */
		(**mh).mhSelect = pSelect;			/* install selection handler */
		(**mh).mhClobber = pClobber;		/* install disposal handler */
		InsertMenu (theMenu, 0);			/* put menu at end of menu bar */
	}
	if (drawBar)
		DrawMenuBar ();
	return (mh != nil);
}


/*
	Remove a menu handler.  This calls the handler's disposal routine
	and then takes the handler out of the handler list and disposes
	of it.

	Note that the menu MUST be deleted from the menu bar before calling
	the clobber proc, because the menu bar will end up filled with
	garbage if the menu was allocated with NewMenu (see discussion of
	DisposeMenu in Menu Manager section of Inside Macintosh).
*/

SkelRmveMenu (theMenu)
MenuHandle	theMenu;
{
register Integer	mID;
register MHandler	**h, **h2;
register ProcPtr	p;

	mID = (**theMenu).menuID;
	if (mhList != nil)				/* if list empty, ignore */
	{
		if ((**mhList).mhID == mID)	/* is it the first element? */
		{
			h2 = mhList;
			mhList = (**mhList).mhNext;
		}
		else
		{
			for (h = mhList; h != nil; h = h2)
			{
				h2 = (**h).mhNext;
				if (h2 == nil)
					return(0);						/* menu not in list! */
				if ((**h2).mhID == mID)			/* found it */
				{
					(**h).mhNext = (**h2).mhNext;
					break;
				}
			}
		}
		DeleteMenu (mID);
		DrawMenuBar ();
		if (mhClobOnRmve && (p = (**h2).mhClobber) != nil)
			(*p) (theMenu);				/* call disposal routine */
		DisposHandle((Handle)h2);				/* get rid of handler record */
	}
	return(0);
}


/*
	Install a handler for the Apple menu.
	
	SkelApple is called if TransSkel is supposed to handle the apple
	menu itself.  aboutTitle is the title of the first item.  If nil,
	then only desk accessories are put into the menu.  If not nil, then
	the title is entered as the first item, followed by a gray line,
	then the desk accessories.

	SkelApple does not cause the menubar to be drawn, so if the Apple
	menu is the only menu, DrawMenuBar must be called afterward.

	No value is returned, unlike SkelMenu.  It is assumed that SkelApple
	will be called so early in the application that the call the SkelMenu
	is virtually certain to succeed.  If it doesn't, there's probably
	little hope for the application anyway.
*/

SkelApple (aboutTitle, aboutProc)
StringPtr	aboutTitle;
ProcPtr		aboutProc;
{
	appleID = 1;
	appleMenu = NewMenu (appleID, "\001\024");	/* 024 = apple character */
	if (aboutTitle != nil)
	{
		doAbout = true;
		AppendMenu (appleMenu, aboutTitle);	/* add About... item title */
		AppendMenu (appleMenu, "\002(-");		/* add gray line */
		appleAboutProc = aboutProc;
	}
	AddResMenu (appleMenu, rtDRVR);		/* add desk accessories */
	(void) SkelMenu (appleMenu, DoAppleItem, DoAppleClobber, false);
	
	return 0;
}


/* -------------------------------------------------------------------- */
/*					Window-handler interface routines					*/
/* -------------------------------------------------------------------- */


/*
	Install handler for a window and set current port to it.  Remove
	any previous handler for it.  Pass the following parameters:

	theWind	Pointer to the window to be handled.  Must be created by host.
	pMouse	Proc to handle mouse clicks in window.  The proc will be
			passed the point (in local coordinates), the time of the
			click, and the modifier flags word.
	pKey	Proc to handle key clicks in window.  The proc will be passed
			the character and the modifier flags word.
	pUpdate	Proc for updating window.  TransSkel brackets calls to update
			procs with calls to BeginUpdate and EndUpdate, so the visRgn
			is set up correctly.  A flag is passed indicating whether the
			window was resized or not.  BY CONVENTION, the entire portRect
			is invalidated when the window is resized.  That way, the
			handler's update proc can redraw the entire content region
			without interference from BeginUpdate/EndUpdate.  The flag
			is set to false after the update proc is called; the
			assumption is made that it will notice the resizing and
			respond appropriately.
	pActivate Proc to execute when window is activated or deactivated.
			A boolean is passed to it which is true if the window is
			coming active, false if it's going inactive.
	pClose	Proc to execute when mouse clicked in close box.  Useful
			mainly to temp window handlers that want to know when to
			self-destruct (with SkelRmveWind).
	pClobber Proc for disposal of handler's data structures
	pIdle	Proc to execute when no events are pending.
	frontOnly True if pIdle should execute on no events only when
			theWind is frontmost, false if executes all the time.  Note
			that if it always goes, everything else may be slowed down!

	If a particular procedure is not needed (e.g., key events are
	not processed by a handler), pass nil in place of the appropriate
	procedure address.
	
	Return zero if no handler could be allocated, non-zero if successful.
	If zero is returned, the port will not have changed.
*/
 
SkelWindow (theWind, pMouse, pKey, pUpdate, pActivate, pClose,
				pClobber, pIdle, frontOnly)

WindowPtr	theWind;
ProcPtr		pMouse, pKey, pUpdate, pActivate, pClose, pClobber, pIdle;
Boolean		frontOnly;
{
register WHandler	**hHand, *hPtr;

	whClobOnRmve = false;
	SkelRmveWind (theWind);
	whClobOnRmve = true;

/*
	Get new handler, attach to list of handlers.  It is attached to the
	beginning of the list, which is simpler; the order is presumably
	irrelevant to the host, anyway.
*/

	if ((hHand = New (WHandler)) != nil)
	{
		(**hHand).whNext = whList;
		whList = hHand;

/*
	Fill in handler fields
*/

		hPtr = *hHand;
		hPtr->whWind = theWind;
		hPtr->whMouse = pMouse;
		hPtr->whKey = pKey;
		hPtr->whUpdate = pUpdate;
		hPtr->whActivate = pActivate;
		hPtr->whClose = pClose;
		hPtr->whClobber = pClobber;
		hPtr->whIdle = pIdle;
		hPtr->whFrontOnly = frontOnly;
		hPtr->whSized = false;
		hPtr->whGrow = growRect;
		SetPort (theWind);
	}
	return (hHand != nil);
}


/*
	Remove a window handler.  This calls the handler's disposal routine
	and then takes the handler out of the handler list and disposes
	of it.

	SkelRmveWind is also called by SkelRmveDlog.

	Note that if the window cache variable is set to the window whose
	handler is being clobbered, the variable must be zeroed.
*/

SkelRmveWind (theWind)
WindowPtr	theWind;
{
register WHandler	**h, **h2;

	if (theWind == oldWindow)
		oldWindow = nil;

	if (whList != nil)		/* if list empty, ignore */
	{
		if ((**whList).whWind == theWind)	/* is it the first element? */
		{
			h2 = whList;
			whList = (**whList).whNext;
		}
		else
		{
			for (h = whList; h != nil; h = h2)
			{
				h2 = (**h).whNext;
				if (h2 == nil)
					return(0);						/* theWind not in list! */
				if ((**h2).whWind == theWind)	/* found it */
				{
					(**h).whNext = (**h2).whNext;
					break;
				}
			}
		}
		if (whClobOnRmve)
			DoClobber (h2);		/* call disposal routine */
		DisposHandle((Handle)h2);		/* get rid of handler record */
	}
	return(0);
}


# ifdef	supportDialogs

/* -------------------------------------------------------------------- */
/*					Dialog-handler interface routines					*/
/* -------------------------------------------------------------------- */


/*
	Install a handler for a modeless dialog window and set the port
	to it.  Remove any previous handler for it. SkelDialog calls
	SkelWindow as a subsidiary to install a window handler, then sets
	the event procedure on return.

	Pass the following parameters:

	theDialog	Pointer to the dialog to be handled.  Must be created
			by host.
	pEvent	Event-handling proc for dialog events.
	pClose	Proc to execute when mouse clicked in close box.  Useful
			mainly to dialog handlers that want to know when to
			self-destruct (with SkelRmveDlog).
	pClobber Proc for disposal of handler's data structures

	If a particular procedure is not needed, pass nil in place of
	the appropriate procedure address.
	
	Return zero if no handler could be allocated, non-zero if successful.
	If zero is returned, the port will not have changed.
*/

SkelDialog (theDialog, pEvent, pClose, pClobber)
DialogPtr	theDialog;
ProcPtr		pEvent;
ProcPtr		pClose;
ProcPtr		pClobber;
{
	if (SkelWindow (theDialog, nil, nil, nil, nil, pClose, pClobber, nil, false))
	{
		(**GetWDHandler (theDialog)).whEvent = pEvent;
		return (1);
	}
	return (0);
}


/*
	Remove a dialog and its handler
*/

SkelRmveDlog (theDialog)
DialogPtr	theDialog;
{
	SkelRmveWind (theDialog);
	return 0;
}

# endif


/* -------------------------------------------------------------------- */
/*					Miscellaneous interface routines					*/
/* -------------------------------------------------------------------- */


/*
	Override the default sizing limits for a window, or, if theWind
	is nil, reset the default limits used by SkelWindow.
*/

SkelGrowBounds (theWind, hLo, vLo, hHi, vHi)
WindowPtr	theWind;
Integer			hLo, vLo, hHi, vHi;
{
register WHandler	**h;
Rect				r;

	if (theWind == nil)
		SetRect (&growRect, hLo, vLo, hHi, vHi);
	else if ((h = GetWHandler (theWind)) != nil)
	{
		SetRect (&r, hLo, vLo, hHi, vHi);
		(**h).whGrow = r;
	}
	
	return 0;
}


/*
	Set the event mask.
*/

SkelEventMask (mask)
Integer		mask;
{
	eventMask = mask;
	return 0;
}


/*
	Return the event mask.
*/

SkelGetEventMask (mask)
Integer		*mask;
{
	*mask = eventMask;
	return 0;
}


/*
	Install a background task.  If p is nil, the current task is
	disabled.
*/

SkelBackground (p)
ProcPtr	p;
{
	pBkgnd = p;
	return 0;
}


/*
	Return the current background task.  Return nil if none.
*/

SkelGetBackground (p)
ProcPtr	*p;
{
	*p = pBkgnd;
	return 0;
}


/*
	Install an event-inspecting hook.  If p is nil, the hook is
	disabled.
*/

SkelEventHook (p)
Boolean	(*p)();
{
	pEvent = p;
	return 0;
}


/*
	Return the current event-inspecting hook.  Return nil if none.
*/

SkelGetEventHook (p)
Boolean	(**p)();
{
	*p = pEvent;
	return 0;
}


# ifdef	supportDialogs

/*
	Set the mask for event types that will be passed to dialogs.
	Bit 1 is always set, so that null events will be examined.
	(If this is not done, the caret does not blink in editText items.)
*/

SkelDlogMask (mask)
Integer		mask;
{
	dlogEventMask = mask | 1;
	return 0;
}


/*
	Return the current dialog event mask.
*/

SkelGetDlogMask (mask)
Integer		*mask;
{
	*mask = dlogEventMask;
	return 0;
}

# endif
