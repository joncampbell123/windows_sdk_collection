/*
	TransSkel multiple-window demonstration: TextEdit module

	This module handles a simple TextEdit window, in which text may be
	typed and standard Cut/Copy/Paste/Clear operations may be performed.
	Undo is not supported, nor is text scrolling.

	21 April 1988		Paul DuBois
*/

# include	"MultSkel.h"
# include	<macos\TextEdit.h>
# include	<macos\Scrap.h>



/* edit menu item numbers */

typedef enum {
	undo = 1,
	/* --- */
	cut = 3,
	copy,
	paste,
	clear
} editItems;



/*
	Text Window - simple text editing window
*/

WindowPtr			editWind;
static TEHandle		teEdit;		/* handle to text window TextEdit record */

/*  Prototypes for local functions */

SkelWindow (WindowPtr theWind, ProcPtr pMouse, ProcPtr pKey, ProcPtr pUpdate, 
			ProcPtr pActivate, ProcPtr pClose, ProcPtr pClobber, ProcPtr pIdle,
			Boolean	frontOnly );	// Defined in transkel.c
DrawGrowBox (WindowPtr wind);		// Defined in multskel.c		



static Halt ()
{
	TEDispose (teEdit);
	CloseWindow (editWind);
	return 0;
}


static Idle ()
{
	TEIdle (teEdit);	/* blink that cursor! */
	return 0;
}


static Key (ch, mods)
char	ch;
Integer	mods;
{
	TEKey (ch, teEdit);
	return 0;
}


static Mouse (thePt, t, mods)
Point	thePt;
Longint	t;
Integer	mods;
{
	TEClick (thePt, (Boolean)((mods & shiftKey) != 0), teEdit);
	return 0;
}


/*
	Update text window.  The update event might be in response to a
	window resizing.  If so, resize the rects and recalc the linestarts
	of the text.  To resize the rects, only the right edge of the
	destRect need be changed (the bottom is not used, and the left and
	top should not be changed). The viewRect should be sized to the
	screen.
*/

static Update (resized)
Boolean	resized;
{
Rect	r;

	r = editWind->portRect;
	EraseRect (&r);
	r.left += 4;
	r.bottom -= 2;
	r.top += 2;
	r.right -= 19;
	if (resized)
	{
		(**teEdit).destRect.right = r.right;
		(**teEdit).viewRect = r;
		TECalText (teEdit);
	}
	DrawGrowBox (editWind);
	TEUpdate (&r, teEdit);
	return 0;
}


static Activate (active)
Boolean	active;
{
	DrawGrowBox (editWind);
	if (active)
	{
		TEActivate (teEdit);
		DisableItem (editMenu, undo);
	}
	else
	{
		TEDeactivate (teEdit);
		EnableItem (editMenu, undo);
	}

	return 0;
}


/*
	Handle Edit menu items for text window
*/

EditWindEditMenu (item)
Integer	item;
{
	switch (item)
	{
/*
	cut selection, put in TE Scrap, clear clipboard and put
	TE scrap in it
*/
		case cut:
		{
			TECut (teEdit);
			(void) ZeroScrap ();
			(void) TEToScrap ();
			break;
		}
/*
	copy selection to TE Scrap, clear clipboard and put
	TE scrap in it
*/
		case copy:
		{
			TECopy (teEdit);
			(void) ZeroScrap ();
			(void) TEToScrap ();
			break;
		}
/*
	get clipboard into TE scrap, put TE scrap into edit record
*/
		case paste:
		{
			(void) TEFromScrap ();
			TEPaste (teEdit);
			break;
		}
/*
	delete selection without putting into TE scrap or clipboard
*/
		case clear:
		{
			TEDelete (teEdit);
			break;
		}
	}

	return 0;
}


EditWindInit ()
{
Rect	r;
StringPtr	str;

	editWind = GetNewWindow (editWindRes, nil, (WindowPtr)-1L);
	(void) SkelWindow (editWind, Mouse, Key, Update,
				Activate, nil, Halt, Idle, true);

	TextFont (0);
	TextSize (0);

	r = editWind->portRect;
	r.left += 4;
	r.bottom -= 2;
	r.top += 2;
	r.right -= 19;
	teEdit = TENew (&r, &r);
	str = (StringPtr) "\040This is the text editing window.";
	TEInsert (&str[1], (Longint) str[0], teEdit);
	return 0;
}
