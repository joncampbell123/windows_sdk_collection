/* MEMSCR.C -- screen/keyboard I/O for MEMSET */
/* (nearly the same as SUSCR.C for Windows SETUP */

/* Microsoft C include files */
#include <conio.h>		/* for getch() */
#include <dos.h>		/* for int86() */
#include <stdio.h>

/* SETUP include files */
#include "setup.h"

/* constants for video output and keyboard input */
#define	VIDEO	0x10		/* ROM BIOS video interrupt call */
				/* PC 16-bit keyboard input characters: */
#define	BS  8			/* backspace */
#define DEL  0x5300             /* PC DEL key */
#define INS  0x5200             /* PC INS key */
#define CR 13                   /* carriage return */
#define LF 10                   /* line feed */
#define	SPACE 32
#define ESC 0x1b
#define CTRLX 0x18              /* ^X */
#define	UP 0x4800		/* PC up arrow */
#define	DOWN 0x5000		/* PC down arrow */
#define	LEFT 0x4b00		/* PC left arrow */
#define	RIGHT 0x4d00		/* PC right arrow */
#define	HOME 0x4700		/* PC HOME key */
#define	END 0x4f00		/* PC END key */
#define	PGUP 0x4900		/* PC page up key */
#define	PGDN 0x5100		/* PC page down key */

#define	LCOL 2	/* indentation for display of list */

/* static data */

static	char attr = 0x7;	/* global screen attribute */
static	char mflag = 1;		/* flags mode setting */

union REGS scrinr;
union REGS scroutr;

/* message and choices for 'Continue'/'Quit' choice.  The first row of
** these displays is set to be 2 rows after the last line displayed
** by the program.  (ContinueOrQuit() function).
*/

extern textline ContQuitMsg[] ;


/* insertion strings for display text -- inserted when a character is
** between 1 and 31 inclusive (001 and 037 octal). In a C string, the
** number should be inserted as a backslash followed by a 3-digit
** octal number.
** The pointers in entries 001 to 012 of this array are changed when a
** single-application setup is done.
*/


extern int nInserts;
extern char * InsertText[31];

/* global information used by ScrDisplay() and input routines */

static char lastrow = 1;
static char lastcol = 1;
static char rowsused[25];
static char colGlobal;	    /* used by setpos() and displaystring() */

/* forward declaration */
unsigned int getchw();
unsigned int uch(unsigned int);
ExitPgm();

/* text and row for "more.." message */
char smMore[] =
"             (To see more of the list, press the DOWN(\031) key.)";
int iMoreLine = 19;

extern textline ConfirmPathMsg[] ;
extern textline ConfirmSelectionMsg[] ;

/* ******************* top-level functions **************************** */

/*  ScrDisplay() -- display lines of text on the screen.
**
**
**  Input:  'screen' is pointer to list of 'screen' structures,
**          terminated by entry NULL string or 0-length string ("");
**  Output: 'rowsused[]' entries set for each nonblank text row.
**          'lastrow' and 'lastcol' set after end of last non-NULL
**          string OR 0-length string.
*/

ScrDisplay(screen)
textline * screen;
    {
    textline * s;

    s =	screen;

    while ( (s->line !=	NULL) )
	{
	lastrow	= s->row;
	lastcol	= s->col;
	rowsused[lastrow-1] = 1;

	textpos(lastrow, lastcol);

	if ((s->line)[0] == 0)
	    break;

	lastcol	+= displaystring(s->line);

	s++;
	}
    }

/* ScrInputPath() -- input a pathname string
**                   Provides a simple edit box.
**                   Edit functions are BS, LEFT, RIGHT, HOME, END.
**                   Typing a character always inserts it.
**
**          The left end of the edit box begins at (lastrow, lastcol)
**
** Inputs:
**          ins -- initial value of string in edit box. The programmer
**                   is expected to provide a legal pathname.
**          outs -- output string buffer.  Size must be at least
**                   max + 1.
**          max -- maximum size of output string.  Highlighted edit box
**                 have width max + 2.
** Output:
**          Return value is pointer to outs.
**
** The routine checks for illegal characters in the filename and requires
** that the pathname begin with a drive letter followed by a colon, and that
** parts of the pathname between backslashes are from 1 to 8 characters in
** length.
*/

unsigned char * ScrInputPath(ins, outs, max)
unsigned char *ins;
unsigned char *outs;
int max;
  {
  int editcol;		/* first column of input */
  int len;		/* length of outs */
  int curs;		/* relative cursor position */
  unsigned int ich;	/* input char */
  unsigned int tch;	/* test char */
  int plen;		/* count of part of pathname */
  int i;
  int legal;            /* flag for error */
  int lastr = lastrow;
  int lastc = lastcol;

  ScrDisplay(ConfirmPathMsg);

  /* display edit box and do input */

  do {

    /* Position and display highlighted edit box */
    textpos(lastr, lastc);
    setreverse();
    charout(' ', max+2);

    /* copy ins to outs, calculate length, and change to upper case.
    ** the output (and displayed) string is truncated to 'max' length.
    */
    len = strlen(ins);
    if (len > max)
	len = max;
    for	(i = 0;	i < len; i++)
	outs[i]	= ins[i];
    outs[len] =	0;
    strupr(outs);

    /* position and display initial value of outs[] */
    editcol = lastc + 1;
    textpos(lastr, editcol);
    len	= displaystring(outs);
    curs = len;
    textpos(lastr, editcol+curs);


    /* input characters and edit 'outs' string */

    while ((ich	= uch(getchw())) != CR)
	{
	switch(ich)
		{
		case LEFT:  /* move cursor left */
			if (curs > 0)
			    curs--;
			else
			    beep();
			break;
		case RIGHT: /* move cursor right */
			if (curs < len)
			    curs++;
			else
			    beep();
			break;
		case DEL:   /* delete right */
			if (curs >= len)
			    break;
			curs++;
			/* now use BS code -- must follow immediately */
		case BS:    /* delete left */
			if (curs > 0)
			    {
			    for	(i=curs; i < len+1; i++)
				outs[i-1]=outs[i];
			    curs--;
			    len--;
			    textpos(lastr, editcol+curs);
			    displaystring(outs+curs);
			    textpos(lastr, editcol+len);
			    ttyout(' ');
			    }
			else
			    beep();
			break;
		case CTRLX:    /* quit */
			ExitPgm();
		case HOME:  /* move cursor to beginning of line */
			curs = 0;
			break;
		case END:   /* move cursor to end of text */
			curs = len;
			break;

		/* these characters are illegal in path name */
		case '\"':
		case '[':
		case ']':
		/* case ':': legal in second position */
		case '<':
		case '>':
		case '+':
		case '=':
		case ';':
		case ',':

		case '*':
		case '?':
		case '|':
			beep();
			break;
		default:
		    /* characters GREATER THAN space: */
		    if ((ich >=	0x21) && (ich <= 0xff))
			{
			/* insert character at cursor position */
			if (len	< max)
			    {
			    for	(i = 1 + len; i	> curs;	i--)
				outs[i]	= outs[i-1];
			    outs[curs] = ich;
			    textpos(lastr, editcol+curs);
			    displaystring(outs+curs);
			    curs++;
			    len++;
			    }
			else
			    beep();

			/* 0-terminate */
			outs[len] = 0;
			}
		    else
			beep();	    /* unknown control character */
		    break;

		}   /* switch() */
	    textpos(lastr, editcol+curs);

	};  /* .. while() */

	/* check syntax of pathname string: first require drive designator */
	if ( (outs[1] != ':') || (outs[0] < 'A') || (outs[0] > 'Z') )
		ich = 0;    /* force re-execution of character! */

	/* if OK so far, count characters between backslashes */
	else for (i = 2, plen =	0; ((tch = outs[i]) != 0) && (ich != 0)	; i++)
	    {
	    if (tch == '\\' || tch == '/')
		{
		if (outs[i-1] == '\\' || outs[i-1] == '/')
					/* error if double backslash */
		    ich	= 0;
		plen = 0;
		}
	    else
		plen++;
		if (plen > 8)		/* error if dir. name > 8 chars */
		    ich	= 0;
	    }

	/* max-length string must end in backslash */
	if ((len == max) && (outs[len-1] != '\\') && (outs[len-1] != '/'))
	    ich	= 0;

	/* beep if the error checking (just above) failed */
	if (ich	== 0)
	    beep();

    } while (ich != CR);

    /* force backslash on end of string, but don't display it because
    ** the screen will normally be cleared quickly anyway.  The '\'
    ** is NOT appended if the path is only a drive spec.
    */
    if ((len > 2) && (outs[len-1] != '\\') && (outs[len-1] != '/'))
	{
	outs[len] = '\\';
	outs[len+1] = 0;
	}

  /* done, set normal video. */
  setnormal();

  /* return pointer */
  return (outs);
  }

/* ScrSelectList() -- Make a selection from a list
**
** Inputs:  'selections[]' is a null-terminated list of strings to be
**          displayed in a window on the screen.  The window starts
**          two rows below the last text displayed by  ScrDisplay()
**          (on 'lastrow'), and ends 2 lines before the next nonblank
**          line on the screen (as indicated in the used[] array).
** Output:  the returned value is the index of the selected list in
**          'selections[]'.
**
** The user uses cursor keypad keys, space, and backspace to select and
** scroll the text.  A selection is confirmed with the ENTER key.
*/


int ScrSelectList(selections)
unsigned char ** selections;
    {
    int	wintop;	    	/* first row of window (1-based) */
    int	winbottom;  	/* bottom row of window (1-based) */
    int	height;	    	/* number of rows in windows */
    unsigned int ich;   /* input character */
    int	selcount;   	/* number of items in 'selections[]'*/
    int	selno;	    	/* index of current selection in 'selections' */
    int	selline;    	/* relative line number of selection */
    int	oldselline; 	/* relative line number of previous selection */
    int	winstart;   	/* first selection in window */
    int	i;
    int	row;
    unsigned char invalid;   /* flags 'repaint' of selections */

    /* determine screen space available for scrolling window */
    wintop = lastrow + 2;

    ScrDisplay(ConfirmSelectionMsg);

    for	(winbottom = wintop;
	    (winbottom < MAXWINBOT) &&
	    (! rowsused[winbottom]);	/* stop if next row filled */
	    winbottom++) ;
    if (winbottom < MAXWINBOT)	    /* if text below, leave a blank line */
	winbottom--;
    height = winbottom - wintop	+ 1;

    /* determine total number of selections in list */
    for	(selcount = 0; selections[selcount] != NULL; selcount++)
	{
	if (selections[selcount][0] == 0)
	    break;
	}
    if (selcount == 0)      /* exit if list is empty */
	return(0);
    if (selcount <= height)  /* adjust for short list */
	{
	height = selcount;
	winbottom = wintop + height - 1;
	}
    else
	{
	textpos(iMoreLine, 1);         /* display "more.." line */
	displaystring(smMore);
	if (winbottom + 1 == iMoreLine)
	    {
	    height--;
	    winbottom--;
	    }
	}

    /* initialize window */
    invalid = 1;
    winstart = 0;
    selline = 0;
    oldselline = 0;
    selno = 0;
    setnormal();

    /* display text and make selections */
    do {
	if (invalid)
	    {
	    /* clear or scroll window */
	    switch (invalid)
		{
		case 1:				/* redraw window */
		    scrinr.h.ah = 6;            /* scroll up function */
		    scrinr.h.al = 0;            /* blank entire window */
		    break;

		case 2:				/* scroll down */
		    scrinr.h.ah = 7;            /* scroll down function */
		    scrinr.h.al = 1;            /* move down 1 line */
		    break;

		case 3:				/* scroll up */
		    scrinr.h.ah = 6;            /* scroll up function */
		    scrinr.h.al = 1;            /* move down 1 line */
		    break;
		}	/* end switch */

	    scrinr.h.ch = wintop - 1;       /* top row */
	    scrinr.h.dh = winbottom - 1;    /* bottom row */
	    scrinr.h.cl = 0;                /* left side */
	    scrinr.h.dl = 79;               /* right side */
	    scrinr.h.bh = attr;             /* current screen attribute */
	    int86(VIDEO, &scrinr, &scroutr);

	    /* fill blank line(s) with text from 'selections' */
	    switch(invalid)
		   {
		   case 1:	/* redraw whole window */
			for (i = winstart, row = wintop;
				row <=	winbottom; i++,	row++)
			   {
			   textpos(row, LCOL);
			   if (row == selline + wintop)
			   	setreverse();
			   displaystring(selections[i]);
			   setnormal();
			   }
			break;

		   case 2:				/* scroll down */
			textpos(wintop, LCOL);
			setreverse();
			displaystring(selections[winstart]);
			setnormal();
			if (height > 1)
			    {
			    textpos(wintop+1, LCOL);
			    displaystring(selections[winstart+1]);
			    }
			break;

		   case 3:				/* scroll up */
			textpos(winbottom, LCOL);
			setreverse();
			displaystring(selections[winstart + height - 1]);
			setnormal();
			if (height > 1)
			    {
			    textpos(winbottom-1, LCOL);
			    displaystring(selections[winstart + height - 2]);
			    }
			break;

		   } /* end switch */

	    /* position cursor */
	    textpos(wintop+selline, LCOL);

	    /* mark window contents valid */
	    invalid = 0;
	    }
	else /* (invalid == 0) cursor just moves, maybe */
	    {
	    if (selline != oldselline)
		{
		/* move highlighting by rewriting old & new selections */
		textpos(wintop+oldselline, LCOL);
		displaystring(selections[winstart+oldselline]);
		textpos(wintop+selline, LCOL);
		setreverse();
		displaystring(selections[winstart+selline]);
		setnormal();
		textpos(wintop+selline, LCOL);
		}
	    }

	oldselline = selline;

	switch (ich =  getchw())
	    {
	    /* selno is calculated AFTER winstart or selline is adjusted */

	    case LEFT:	    /* move to previous selection */
	    case UP:
	    case BS:
		    if (selline	> 0)
			{
			selline--;
			}
		    else if (selno > 0)
			{
			winstart--;
			invalid	= 2;	/* flag scroll down */
			}
		    else
			beep();
		    break;

	    case PGUP:
		    if (winstart != 0)
			{   /* scroll up */
			winstart -= height - 1;
			if (winstart < 0)
			    winstart = 0;
			selline = height - 1;
			invalid	= 1;
			}
		    break;

	    case RIGHT:	    /* move to next selection */
	    case DOWN:
	    case SPACE:
		    if (selno >= selcount - 1)
			beep();
		    else if (selline < height -	1)
			selline++;
		    else
			{
			winstart++;
			invalid	= 3;	/* flag scroll up */
			}
		    break;

	    case HOME:
		    invalid = 1;
		    winstart = 0;
		    selline = 0;
		    break;

	    case PGDN:
		    if (winstart < selcount - height)
			{   /* scroll down */
			winstart += height - 1;
			if (winstart > selcount	- height)
			    winstart = selcount	- height;
			selline = 0;
			invalid	= 1;
			}
		    break;

	    case END:
		    invalid = 1;
		    winstart = selcount	- height;
		    selline = height - 1;
		    break;

	    case CR:
		    break;

	    case CTRLX:
		    ExitPgm();
/*          case INS:
	    case DEL:
		    winstart = 0;
		    selline = -1;
		    break;
*/
	    default:
		beep();
	    }

	/* after each input, recalculate 'selno' and position cursor */
	selno =	winstart + selline;
	textpos(wintop+selline,	LCOL);

	} while ((ich != CR)/* && (ich != INS) && (ich != DEL) */);

    return(selno);
    }

/* ScrSelectChar() -- Select a character
**
** Input:   'screen' is an array of 'textline' structures.
**          each entry of which contains a string which is
**          to be used as a choice by the user. The first character
**          of the choice is used to select the entry.
**          'defaultchoice' is the index of the default choice.
**
** Output:  the index of the chosen string (in 'screen') is the function
**          value.
**
**          The maximum number of choices is 10.
*/

int ScrSelectChar(screen, defaultchoice)
textline * screen;
int defaultchoice;
    {
    unsigned char answers[10];
    unsigned char rows[10];
    unsigned char cols[10];
    textline * s;
    int	thisrow;
    int	thiscol;
    int	maxchoice;
    int	choice;
    int	i;
    unsigned int ich;

    s =	screen;
    i =	0;

    while ( (s->line !=	NULL) && (i < 10) )
	{
	thisrow	= s->row;
	thiscol	= s->col;

	textpos(thisrow, thiscol);

	if ((s->line)[0] == 0)
	    break;

	displaystring(s->line);

	rows[i]	= thisrow;
	cols[i]	= thiscol;
	answers[i] = uch((s->line)[0]);	/* use first character of choice */

	s++;
	i++;
	}

    maxchoice =	i - 1;

    choice = defaultchoice;
    if (choice > maxchoice)
	choice = 0;
    textpos(rows[choice], cols[choice] - 1);
    setreverse();
    charout(' ', 1);
    charout(answers[choice], 1);
    charout(' ', 1);
    textpos(rows[choice], cols[choice]);

    while ((ich	= getchw()) != CR)
	{
	textpos(rows[choice], cols[choice] - 1);
	setnormal();
	charout(' ', 1);
	charout(answers[choice], 1);
	charout(' ', 1);
	textpos(rows[choice], cols[choice]);
	switch(ich) {
	    case UP:	/* previous choice */
	    case LEFT:
	    case BS:
			if (choice > 0)
			    choice--;
			else
			    beep();
			break;
	    case DOWN:	/* move to next choice */
	    case RIGHT:
	    case SPACE:
			if (choice < maxchoice)
			    choice++;
			else
			    beep();
			break;
	    case HOME:	/* move to first (top) choice */
			choice = 0;
			break;
	    case END:	/* move to last (bottom) choice */
			choice = maxchoice;
			break;
	    case CTRLX:   /* quit the program */
			ExitPgm();
	    default:	/* search 'answers' for a match */
			for (i = 0; i <= maxchoice; i++)
			    {
			    if (uch(ich) == answers[i])
				{
/* ret */                       return(i);
				}
			    }
			/* beep if no match */
			if (i >	maxchoice)
			    beep();
	    }
	textpos(rows[choice], cols[choice] - 1);
	setreverse();
	charout(' ', 1);
	charout(answers[choice], 1);
	charout(' ', 1);
	textpos(rows[choice], cols[choice]);
	}

    return(choice);
    }

/* ScrClear() -- Clear (and initialize) the screen
**
**  Output: The rowsused[] array is cleared.
**
**  Note: The status line is cleared as well as the rest of the screen.
*/

ScrClear()
    {
    int	i;

    if (mflag)	/* set text mode on the first call */
	{
	settextmode();
	mflag =	0;
	}
    setnormal();
    cls();

    lastcol = 1;
    lastrow = 1;
    for	(i = 0;	i < STATLINE; i++)
	rowsused[i] = 0;

    textpos(1,1);
    }

/* ** routines for handling status line display, and fatal errors ** */

/* DisplayStatusLine -- display text in status (bottom) line */
/* ClearStatusLine() -- clear status line */

DisplayStatusLine(s)
unsigned char * s;
{
	setnormal();
	scrinr.h.ch = STATLINE-1;       /* top = bottom row (0-based) */
	scrinr.h.dh = STATLINE-1;       /* bottom = bottom row*/
	textpos(STATLINE,1);
	clearrows();
	displaystring(s);
}

ClearStatusLine()
{
	DisplayStatusLine("");
}

/* FatalError -- display text in fatal-error (next-to-bottom) line
** and exit from SETUP with error code and cursor ABOVE error line. */

FatalError(s, errorcode)
unsigned char * s;
int errorcode;
{
	setreverse();               /* prepare for reverse video display */
	scrinr.h.ch = ERRLINE-1;            /* top row = next-to-bottom */
	scrinr.h.dh = ERRLINE-1;            /* bottom row = next-to-bottom */
	clearrows();
	textpos(ERRLINE,1);
	displaystring(s);
	textpos(ERRLINE-2,1);
	exit(errorcode);
}

PutExt(n)
int n;
{
static char ExtMemsize[10];

	if (n > 0)	{
		itoa(n, ExtMemsize, 10);
		InsertText[0] = ExtMemsize;
	}
	else
		InsertText[0] = "0";		
}

PutExp(n)
int n;
{
static char ExpMemsize[10];

	if (n > 0)	{
		itoa(n, ExpMemsize, 10);
		InsertText[001] = ExpMemsize;
	}
	else
		InsertText[001] = "0";		
}

PutFreeExt(n)
int n;
{
static char FreeExtMemsize[10];

	if (n > 0)	{
		itoa(n, FreeExtMemsize, 10);
		InsertText[2] = FreeExtMemsize;
	}
	else
		InsertText[2] = "0";		
}
		
/* PutFileName() -- set file name string pointer in InsertText[] array
*/
PutFileName(f)
char * f;
{
	InsertText[003] = f;
}


/* PutDestName() -- set destination name string pointer in InsertText[] array
*/
PutDestName(s)
unsigned char * s;
{
    InsertText[004] = s;
}


/* ********** lower-level functions ********** */

/* uch() -- convert (16-bit) character to upper-case
**
** Characters with nonzero high byte and 0 low byte are unchanged.
** now uses C strupr() function.
*/

unsigned int uch(ich)
unsigned int ich;
    {
    unsigned char buf[2];

    if ((ich & 0xff00) && (0 == (ich & 0xff)))
	/* character is 16-bit keyboard input character */
	return(ich);
    else
	{
	/* character is 8-bit character, use strupr() */
	buf[0] = ich;
	buf[1] = 0;
	strupr(buf);
	return(buf[0]);
	}
    }


/* beep() -- beep the console
*/

beep()
    {
    ttyout(7);
    }

/* display string using the current attribute.
**
** Characters in range [1..nInserts] are used to insert strings from the
** insertstring[] array.
**
** Output:  returns length of string.
*/

int displaystring(s)
unsigned char * s;
    {
    unsigned int ch;
    int l;

    l = 0;
    while (((ch = 0xff & *s++) != 0) && (colGlobal <= 80))
	if (ch <= nInserts)
	    l += displaystring(InsertText[ch-1]);
	else
	    {
	    charout(ch, 1);
	    l++;
	    colGlobal++;
	    }
    return (l);
    }

/* getchw -- input 16-bit PC input character via MS-DOS
**
** Returns PC 16-bit character with 00 in low byte for function keys,
** cursor keys, etc. -- otherwise normal ASCII in lo byte, 00 in
** high byte.
*/

unsigned int getchw()
    {
    unsigned int ch;
    if ((ch = getch()) != 0)
	return(0xff & ch);
    else
	return(getch() << 8);	/* return second byte, hi byte */
    }

/* textpos() -- position text (1-based coordinates) */

textpos(row, column)
int column, row;
    {
    colGlobal =	column;		/* set for displaystring() */
    scrinr.h.ah = 2;            /* set cursor position */
    scrinr.h.dh = row - 1;
    scrinr.h.dl = column - 1;
    scrinr.h.bh = 0;            /* page 0 */
    int86(VIDEO, &scrinr, &scroutr);
    }

/* charout() -- display 'count' copies of a character, with current
**              attribute (in 'attr'), and advance cursor.
*/

charout(ch, count)
int ch;
int count;
    {
    union REGS posregs;

    scrinr.h.ah = 3;            /* read cursor position */
    scrinr.h.bh = 0;            /* page */

    int86(VIDEO, &scrinr, &posregs);    /* (dh,dl) = (row, col) */

    scrinr.h.ah = 9;
    scrinr.h.bh = 0;
    scrinr.x.cx = count;
    scrinr.h.al = ch;
    scrinr.h.bl = attr;
    int86(VIDEO, &scrinr, &scroutr);

    posregs.h.ah = 2;		/* advance cursor position */
    posregs.h.dl += count;	/* use (dh,dl) from read-cursor call */
    posregs.h.bh = 0;		/* page 0 */
    int86(VIDEO, &posregs, &scroutr);
    }

/* ttyout() -- output character to screen using int 10H, function 14
**             (this is dumb TTY emulation function)
**             Color attribute of a character in any screen position is
**             left unchanged by this function.
*/

ttyout(ch)
int ch;
    {
    scrinr.h.ah = 14;       /* write teletype to active page */
    scrinr.h.al = ch;
    scrinr.h.bl = 7;        /* who knows? */
    int86(VIDEO, &scrinr, &scroutr);
    }

/* settextmode() -- set normal 80 x 25 text mode, block cursor */

settextmode()
    {
    scrinr.h.ah = 0;            /* set mode function */
    scrinr.h.al = 3;            /* 80 x 25 color */
    int86(VIDEO, &scrinr, &scroutr);

    scrinr.h.ah = 5;            /* select active page = */
    scrinr.h.al = 0;            /* zero */
    int86(VIDEO, &scrinr, &scroutr);

    }

/* cls() -- clear screen */

cls()
    {
    scrinr.h.ch = 0;            /* top row */
    scrinr.h.dh = 24;           /* bottom row */
    clearrows();
    }


/* clearrows -- clear rows in scrinr.h.ch .. scrinr.h.dh
**/
clearrows()
    {
    scrinr.h.ah = 6;            /* scroll up function */
    scrinr.h.al = 0;            /* blank entire window */
    scrinr.h.cl = 0;            /* left side */
    scrinr.h.dl = 79;           /* right side */
    scrinr.h.bh = attr;         /* current screen attribute */

    int86(VIDEO, &scrinr, &scroutr);
    }

/* setnormal() -- set normal video color attributes */

setnormal()
    {
    attr = 0x7;
    }


/* setreverse() -- set reverse video */

setreverse()
    {
    attr = 0x70;
    }


/* ContinueOrQuit() -- handle continue or Quit messages
*
** The prompts appear 2 lines below 'lastrow'.
*/

ContinueOrQuit()
{
    int i;
    int offset = lastrow + 2 - ContQuitMsg[0].row;

    /* Adjust the row position of the text */
    for (i = 0; ContQuitMsg[i].line != NULL; i++)
	{
	ContQuitMsg[i].row += offset;
	}

    /* Display the prompt */
    ScrDisplay(ContQuitMsg);

    /* Display the choice characters and make the selection */
    if (getchw() == CTRLX)
	ExitPgm();
} /* ContinueOrQuit() */

/* ExitPgm() -- Abort program routine
**              Is invoked by ^X, displays warning message and exits
*/

ExitPgm()
{
    textpos(24, 1);
    exit(1);
}

