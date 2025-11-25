/*
	TransSkel demonstration:  Traditional Skel

	This program mimics the original Skel application:  one sizable,
	dragable, non-closable dark gray window, an "About" alert and two
	dialogs.  Desk accessories supported.

	The project should include this file, TransSkel.c (or a project
	built from TransSkel.c), and MacTraps.

	21 Apr 1988	Paul DuBois
	29 Jan 1989 Conversion for TransSkel 2.0.  Integer should be a
				typedef for compiler 2-byte integer type.
*/

# include     <macos\msvcmac.h> /* Needed before including macos files */

/* to work with new headers */
#define dangerousPattern

# include	<macos\Dialogs.h>	/* includes WindowMgr, QuickDraw, etc. */
# include	<macos\Menus.h>
# include	<macos\ToolUtil.h>
# include	<macos\Memory.h>
# include	<macos\QuickDra.h>

# include	"TranSkel.h"


typedef	short	Integer;

/*
	Resource numbers
*/

# define	fileMenuRes	2		/* File menu */
# define	aboutAlrt	1000	/* About box */
# define	theWindRes	260		/* window */
# define	reportDlog	257		/* message dialog box */
# define	aboutStr	1		/* message strings */
# define	rattleStr	2
# define	frightStr	3



/* file menu item numbers */

typedef enum {
	rattle = 1,
	frighten,
	/* --- */
	quit = 4
} fileItems;


WindowPtr	theWind;

/*
	Menu handles.  There isn't any apple menu here, since TransSkel will
	be told to handle it itself.
*/

MenuHandle	fileMenu;

/* function prototypes */
SetParamText (Integer strNum);
Report (Integer strNum);

/* -------------------------------------------------------------------- */
/*						Menu handling procedures						*/
/* -------------------------------------------------------------------- */


/*
	Read a string resource and put into the Alert/Dialog paramtext
	values
*/

SetParamText (strNum)
Integer	strNum;
{
StringHandle	h;

	h = GetString (strNum);
	HLock (h);
	ParamText (*h, "", "", "");
	HUnlock (h);
	return 0;
}


/*
	Handle selection of "About SkelÉ" item from Apple menu
*/

DoAbout ()
{
	SetParamText (aboutStr);
	(void) Alert (aboutAlrt, nil);
	return 0;
}


/*
	Put up a dialog box with a message and an OK button.  The message
	is stored in the 'STR ' resource whose number is passed as strNum.
*/

Report (strNum)
Integer	strNum;
{
DialogPtr	theDialog;
Integer		itemHit;

	SetParamText (strNum);
	theDialog = GetNewDialog (reportDlog, nil, (WindowPtr)(-1L));
	ModalDialog (nil, &itemHit);
	DisposDialog (theDialog);
	return 0;
}


/*
	Process selection from File menu.
	
	Rattle, Frighten	A dialog box with message
	Quit	Request a halt by calling SkelHalt().  This makes SkelMain
			return.
*/

DoFileMenu (item)
Integer	item;
{

	switch (item)
	{
		case rattle:	Report (rattleStr); break;
		case frighten:	Report (frightStr); break;
		case quit:		SkelWhoa (); break;	/* request halt */
	}
	
	return 0;
}


/*
	Initialize menus.  Tell TransSkel to process the Apple menu
	automatically, and associate the proper procedures with the
	File and Edit menus.
*/

SetUpMenus ()
{

	SkelApple ("\015About Skel...", DoAbout);
	fileMenu = GetMenu (fileMenuRes);
	(void) SkelMenu (fileMenu, DoFileMenu, nil, true);
	return 0;
}


/* -------------------------------------------------------------------- */
/*					Window handling procedures							*/
/* -------------------------------------------------------------------- */


WindActivate (active)
Boolean	active;
{
	Boolean activeused = active;

	DrawGrowIcon (theWind);	/* make grow box reflect new window state */
	return 0;
}


/*
	On update event, can ignore the resizing information, since the whole
	window is always redrawn in terms of the current size, anyway.
	Content area is dark gray except scroll bar areas, which are white.
	Draw grow box as well.
*/

WindUpdate (resized)
Boolean	resized;
{
Rect	r;
Boolean resizedused = resized;

	r = theWind->portRect;		/* paint window dark gray */
	r.bottom -= 15;				/* don't bother painting the */
	r.right -= 15;				/* scroll bar areas */
	FillRect (&r, &qd.dkGray);
	r = theWind->portRect;		/* paint scroll bar areas white */
	r.left = r.right - 15;
	FillRect (&r, &qd.white);
	r = theWind->portRect;
	r.top = r.bottom - 15;
	FillRect (&r, &qd.white);
	DrawGrowIcon (theWind);
	return 0;
}


WindHalt () 
{ 
	DisposeWindow (theWind);
	return 0;
}


/*
	Read window from resource file and install handler for it.  Mouse
	and key clicks are ignored.  There is no close proc since the window
	doesn't have a close box.  There is no idle proc since nothing is
	done while the window is in front (all the things that are done are
	handled by TransSkel).
*/

WindInit ()
{

	theWind = GetNewWindow (theWindRes, nil, (WindowPtr)-1L);
	(void) SkelWindow (theWind, nil, nil, WindUpdate, WindActivate, nil,
					WindHalt, nil, false);
	return 0;
}


/* -------------------------------------------------------------------- */
/*									Main								*/
/* -------------------------------------------------------------------- */


main ()
{

	SkelInit (6, nil);			/* initialize */
	SetUpMenus ();				/* install menu handlers */
	WindInit();					/* install window handler */
	SkelMain ();				/* loop 'til Quit selected */
	SkelClobber ();				/* clean up */
	return 0;
}
