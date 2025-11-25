/*
	TransSkel multiple-window demonstration: main module

	This module performs setup and termination operations, installs
	the window and menu handlers, and processes menu item selections.
	
	There are four window handlers in this demonstration.  The code
	for each handler is in its own module.
	
	Help Window             Scrollable non-editable text window
	Edit Window             Non-scrollable editable text window
	Zoom Window             Non-manipulable graphics display window
	Region Window   Manipulable graphics display window

	The project should include MacTraps, TransSkel.c (or a project built
	from TransSkel.c), MSkelHelp.c, MSkelEdit.c, MSkelZoom.c and
	MSkelRgn.c.  You'll also need the MultiSkel.h header file.

	21 April 1988           Paul DuBois
*/

# include       <Desk.h>
# include       <Dialogs.h>
# include       <Memory.h>
# include       <Menus.h>

# include       <Swap.h>

# include       "transkel.h"
# include       "MultSkel.h"
# include       "GrowZone.h"


/* file menu item numbers */

typedef enum {
	open = 1,
	close,
	/* --- */
	quit = 4
} fileItems;

/*  Prototypes for local functions */

SetUpMenus();		// Defined below
MyShowWindow();		// Defined below
RgnWindInit();		// Defined in msklrgn.c
ZoomWindInit();		// Defined in msklzoom.c
EditWindInit();		// Defined in mskledit.c
EditWindEditMenu();		// Defined in mskledit.c
HelpWindInit();		// Defined in msklhelp.c

/*
	Menu handles.  There isn't any apple menu here, since TransSkel will
	be told to handle it itself.
*/

MenuHandle                      fileMenu;
MenuHandle                      editMenu;

RgnHandle                       oldClip;


main ()
{
	SetMaxSwapSize(0);      /* Force App. to thrash. */

	SkelInit (6, MyGZ);     /* install MyGZ as the GrowZone Proc. */

	SetUpMenus ();                  /* install menu handlers */
	RgnWindInit ();                 /* install window handlers  */
	ZoomWindInit ();
	EditWindInit ();
	HelpWindInit ();
	SkelMain ();
	SkelClobber ();         /* throw away windows and menus */
	return 0;
}


/*
	Initialize menus.  Tell Skel to process the Apple menu automatically,
	and associate the proper procedures with the File and Edit menus.
*/

SetUpMenus ()
{
	void DoAbout(void), DoFile(Integer), DoEdit(Integer);

	SkelApple ("\022About Swap Skel...", (ProcPtr)DoAbout);
	fileMenu = GetMenu (fileMenuRes);
	editMenu = GetMenu (editMenuRes);
	(void) SkelMenu (fileMenu, (ProcPtr)DoFile, nil, false);
	(void) SkelMenu (editMenu, (ProcPtr)DoEdit, nil, true);
	return 0;
}


/*
	Handle selection of About MultiSkelÉ item from Apple menu
*/

void DoAbout (void)
{
	(void) Alert (aboutAlrt, nil);
}


/*
	Process selection from File menu.
	
	Open    Make all four windows visible
	Close   Hide the frontmost window.  If it belongs to a desk accessory,
			close the accessory.
	Quit    Request a halt by calling SkelHalt().  This makes SkelMain
			return.
*/

void DoFile (item)
Integer item;
{
WindowPeek      wPeek;

	switch (item)
	{
		case open:
		{
			MyShowWindow ((WindowPeek) rgnWind);
			MyShowWindow ((WindowPeek) zoomWind);
			MyShowWindow ((WindowPeek) editWind);
			MyShowWindow ((WindowPeek) helpWind);
			break;
		}
/*
	Close the front window.  Take into account whether it belongs
	to a desk accessory or not.
*/
		case close:
		{
			if ((wPeek = (WindowPeek) FrontWindow ()) != nil)
			{
				if (wPeek->windowKind < 0)
					CloseDeskAcc (wPeek->windowKind);
				else
					HideWindow (FrontWindow ());
			}
			break;
		}
		case quit:
		{
			SkelWhoa ();            /* request halt */
			break;
		}
	}
}


/*
	Show a window if it's not visible.  Select the window FIRST, then
	show it, so that it comes up in front.  Otherwise it will be drawn
	in back then brought to the front, which is ugly.

	The test for visibility must be done carefully:  the window manager
	stores 255 and 0 for true and false, not real boolean values.
*/

MyShowWindow (wind)
WindowPeek      wind;
{

	if (wind->visible == 0)
	{
		SelectWindow ((WindowPtr)wind);
		ShowWindow ((WindowPtr)wind);
	}

	return 0;
}


/*
	Process item selected from Edit menu.  First check whether it should
	get routed to a desk accessory or not.  If not, then for route the
	selection to the text editing window, as that is the only one for
	this application to which edit commands are valid.
	(The test of FrontWindow is not strictly necessary, as the Edit
	menu is disabled when any of the other windows is frontmost, and so
	this Proc couldn't be called.)
*/

void DoEdit (item)
Integer item;
{
	if (!SystemEdit ((short)(item - 1)))            /* check DA edit choice */
	{
		if (FrontWindow () == editWind)
			EditWindEditMenu (item);
	}
}


/*
	Miscellaneous routines
	These take care of drawing the grow box and the line along
	the right edge of the window, and of setting and resetting the clip
	region to disallow drawing in that right edge by the other drawing
	routines.
*/


DrawGrowBox (wind)
WindowPtr       wind;
{
Rect            r;
RgnHandle       oldClip;

	r = wind->portRect;
	r.left = r.right - 15;          /* draw only along right edge */
	oldClip = NewRgn ();
	GetClip (oldClip);
	ClipRect (&r);
	DrawGrowIcon (wind);
	SetClip (oldClip);
	DisposeRgn (oldClip);
	return 0;
}


SetWindClip (wind)
WindowPtr       wind;
{
Rect            r;

	r = wind->portRect;
	r.right -= 15;          /* don't draw along right edge */
	oldClip = NewRgn ();
	GetClip (oldClip);
	ClipRect (&r);
	return 0;
}


ResetWindClip ()
{
	SetClip (oldClip);
	DisposeRgn (oldClip);
	return 0;
}
