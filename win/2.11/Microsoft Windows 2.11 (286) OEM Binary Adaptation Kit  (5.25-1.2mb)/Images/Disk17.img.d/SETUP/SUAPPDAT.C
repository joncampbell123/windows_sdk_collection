/* SUAPPDAT.C -- handles SETUP.DAT for single-application setup.
**
** Information from SETUP.DAT, for single-application Windows systems,
** is read and saved.
**
************************************************************************ 
**
** History
**
**  5 jun 87	plb	Started
** 14 jun 87	plb	Combined .H files into SETUP2.H
** 16 jun 87	plb	Made GetSuDatLine() unsigned char *.
**
*/

#include <fcntl.h>
#include "setup.h"


/* max space needed for various lines of SETUP.DAT */

#define SVOL	14		/* line 1 -- volume name */
#define SDISK	82		/* line 2 -- disk description */
#define SAPP	14		/* line 3 -- .EXE filename */
#define SPROD	42		/* line 4 -- 1 or 2 word product name */
#define SVER	42		/* line 5 -- version string */
#define SCOPY	82		/* lines 6..11 -- copyright strings */

#define APBUFSIZE SVOL+SDISK+SAPP+SPROD+SVER+(6*SCOPY)

static unsigned char appbuff[APBUFSIZE];
static int datline[12];

static int d2insert[10] =
    { D_DISKNAME, D_PRODNAME, D_VERSION, D_PRODNAME2,
      D_COPYR1,  D_COPYR2,  D_COPYR3,  D_COPYR4,  D_COPYR5,  D_COPYR6 }; 


/* ReadSuDat()
**
** The file SETUP.DAT is read. If it is read successfully, the
** strings from the first 11 lines of the file are stored, and
** a flag is set to indicate that it has been read.  Some insertion
** string pointers in SUDATA are set to point to some of these
** strings, so that SETUP menus contain the application file and
** disk name, etc..
**
** Returns TRUE iff file exists, is read properly, and there are at
** least 11 lines in the buffer.
*/

BOOL ReadSuDat()
{
    int bcount;
	int line;
	int i;
	int j;
	int fhDat;
	unsigned ch;

	if (-1 == (fhDat = open("setup.dat", O_RDONLY)))
		return(FALSE);
    else
	{
	bcount = read(fhDat, appbuff, APBUFSIZE);

	/* find lines in buffer and zero terminate them.
	** Save an index to the beginning of each line.
	*/
	for (i = 0, j = 0, line = 0; (line < 11) && (i < APBUFSIZE); i++)
	    {
	    /* save index of beginning of each line */
	    if (j == 0)
	    	datline[line] = i;
	    ch = (unsigned char) appbuff[i];
	    if (ch >= 32)
		/* advance string char count for each non-control character */
		j++;
	    else
	    	{
		appbuff[i] = 0;		/* replace ctrl char with 0 */
		if (ch == '\n')
		    {
		    line++;
		    j = 0;
		    }
		}
	    }
	}

    close(fhDat);

    if (line == 11)	/* success? */
    	{
	/* last entry in array points to last word of 1 or 2 word string
	** in the D_PRODNAME entry -- see the wierd 'for' statement!
	** If there are no spaces, this entry points to the whole
	** Product Name string -- otherwise it points to the
	** character after the first space. */

	for (i = datline[D_PRODNAME2] = datline[D_PRODNAME];
		(0 != (ch = appbuff[i])) ; i++)
	    if (ch == ' ')
		{
		datline[D_PRODNAME2] = i+1;
		break;
		}

	/* set string pointers in SUDATA to these strings. */
	for (i = 1; i < 10; i++)
	    inserttext[i] = & appbuff[datline[d2insert[i]]];

	return(TRUE);
	}
    else
    	return(FALSE);

}  /*  ReadSuDat() */




/* GetSuDatLine(n)
**
** Once ReadSuDat() has been executed successfully, this function
** will return a pointer to the n-th line of the file.
*/

unsigned char * GetSuDatLine(n)
int n;
{
    return(&appbuff[datline[n]]);

} /* GetSuDatLine(n) */

