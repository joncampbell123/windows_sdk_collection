/* SUREADME.C  --  provides routines for displaying readme files
*/
/*  History
**
**  25 sep 87   mp      writing module
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <sys\types.h>
#include <errno.h>
#include <sys\stat.h>
#include "setup.h"

#define BUFLENGTH 23*82         /* buffer for screen: 23 lines with 80 char */
#define MAXPAGES 30             /* number of pages in one readme file */
#define SPACE 32
#define CR '\x0D'
#define LF '\x0A'
#define ESC 0x1b
#define CTRLX 0x18              /* ^X */
#define UP 0x4800               /* PC up arrow */
#define DOWN 0x5000             /* PC down arrow */
#define PGUP 0x4900             /* PC page up key */
#define PGDN 0x5100             /* PC page down key */


/* externals from SUDATA.C
*/
extern CHAR smDisplay[];
extern CHAR ReadmeNames[];
extern CHAR Readme[];
extern textline ReadmeMsg[];
extern textline ReadmeChoices[];


/* forward declarations
*/
void DisplayReadmes();
void DisplayFile(CHAR *);


/* statics
*/
CHAR LineDelimiter[] = "\x0A\x1A";        /* LF or EOF */


/*  DisplayReadmes() -- display all readme files in destination dir
*/
void DisplayReadmes()
{
    FINDTYPE findbuf;
    int status;
					/* try to find readmes */
    if (status = ffirst(FileName(WindowsPath, Readme), 0, &findbuf))
	return;                         /* no readmes */

    ScrClear();
    ScrDisplay(ReadmeMsg);              /* display choice */

	getchw();

	return;

/* 2.1 - never display the readmes */



    if (ScrSelectChar(ReadmeChoices, 0) == 1)
		return;

    PutFileName(Readme); /* display main readme */
    DisplayFile(FileName(WindowsPath, Readme));
					/* search for other readmes*/
    status = ffirst(FileName(WindowsPath, ReadmeNames), 0, &findbuf);
    while (!status) {                   /* display all files */
	if (strcmp(Readme, findbuf.name)) {
	    PutFileName(findbuf.name);
	    DisplayFile(FileName(WindowsPath, findbuf.name));
	}
	status = fnext(&findbuf);
    }
}

/* DisplayFile() -- display a readme file and allow PgUp/PgDn
*/
void DisplayFile(fname)
CHAR *fname;                            /* file name */
{
    int fh;                             /* file handle */
    int page = 0;
    CHAR *line;
    int lineno;
    int ich;
    int length;
    CHAR buffer[BUFLENGTH];             /* buffer for current page */
    long offset[MAXPAGES];              /* file offset for each page */

    if ((fh = open(fname, O_RDONLY|O_BINARY)) == -1)
	return;

    offset[0] = 0L;

    while (page < MAXPAGES - 1) {
	if (page < 0)
	    page = 0;
	else if (lseek(fh, offset[page], SEEK_SET) >= 0L &&
	    (length = read(fh, buffer, BUFLENGTH)) > 0) {

	    ScrClear();
	    line = strtok(buffer, LineDelimiter);
	    for (lineno = 1;
		 line && lineno < STATLINE - 1 && line - buffer < length;
		 lineno++) {
		textpos(lineno, 1);
		cputs(line);

		line = strtok(NULL, LineDelimiter);
	    }
	    offset[page + 1] = offset[page] +
		(long)(line - buffer);  /* calculate next file offset */

	    DisplayStatusLine(smDisplay);
	} else
	    page--;

	for (;;) {
	    switch (ich = uch(getchw())) {
		case PGDN:
		case DOWN:
		case CR:
		case SPACE:
		    page++;
		    break;
		case PGUP:
		case UP:
		    page--;
		    break;
		case ESC:
		case CTRLX:
		    close(fh);
		    return;
		default:
		    continue;
	    }
	    break;
	}

    }
    close(fh);
    return;
}

