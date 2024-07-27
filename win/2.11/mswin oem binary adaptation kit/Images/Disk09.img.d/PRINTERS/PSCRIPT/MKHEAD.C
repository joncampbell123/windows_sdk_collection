
/*********************************************************************
* This program is used to generate the postscript header file
* that is included in the printer driver as a resource.
*
* This program filters out white space, PostScript comments and
* also puts a bytecount at the beginning of the file.  The bytecount
* is necessary because the resource manager only maintains resource
* sizes to the nearest paragraph boundary.  We must not download
* any extra garbage to PostScript or it will get upset and abort the
* print job.
*
***********************************************************************
*/


#include <fcntl.h>
#include <types.h>
#include <stat.h>
#include <io.h>


#define TRUE 1
#define FALSE 0

/* modes:
 */
#define PARTA	1	/* permanent-download head */
#define PARTB	2	/* %( common section %) */
#define PARTC	3	/* permanent-download tail */
int dl_mode = PARTA;


char bUnget;
int fUnget = FALSE;



/*****************************************************************
* Name: UngetChar()
*
* Action: Put a character back in the input bufffer.
*
******************************************************************
*/
UngetChar(iCh)
int iCh;
    {
    bUnget = iCh;
    fUnget = TRUE;
    }




/*************************************************************
* Name: GetChar()
*
* Action: Get a character from the input buffer.
*
**************************************************************
*/
int GetChar(fh)
int fh;
    {
    static int cbBuf = 0;
    static int fEof = FALSE;
    static char rgbBuf[512];
    static char *pbBuf;


    if (fUnget)
	{
	fUnget = FALSE;
	return(((int)bUnget) & 0x0ff);
	}

    if (fEof)
	return(-1);

    if (cbBuf<=0)
	{
	cbBuf = read(fh, rgbBuf, sizeof(rgbBuf));

	pbBuf = rgbBuf;
	if (cbBuf<=0)
	    {
	    fEof = TRUE;
	    return(-1);
	    }
	}
    --cbBuf;
    return(((int) *pbBuf++) & 0x0ff);
    }





/**************************************************************
* Name: EatWhite()
*
* Action: This function advances the input buffer pointer to
*	  the first non-white character.
*
***************************************************************
*/
EatWhite(fh)
int fh;
    {
    int iCh;

    iCh = GetChar(fh);
    while(iCh==' ' || iCh=='\t')
	iCh = GetChar(fh);

    if (iCh>0)
	UngetChar(iCh);
    }




/*************************************************************
* Name: EatLine()
*
* Action: This function advances the input buffer pointer
*	  past the end of line, usually in order to discard a PostScript
*	  comment.
*
***************************************************************
*/
EatLine(fh)
int fh;
    {
    int iCh;

    iCh = GetChar(fh);
    while(iCh>0 && iCh!='\n')
	iCh = GetChar(fh);
    if (iCh>0)
	UngetChar(iCh);
    }





/***************************************************************
* Name: ReadLine()
*
* Action: This function scans the input file for the next line
*	  of PostScript code that is to be written out.  In the
*	  process it will remove superfalous white space and
*	  comments.
*
****************************************************************
*/
int ReadLine(fh, pbBuf, cbBuf)
int fh;
char *pbBuf;
int cbBuf;
    {
    static int fEof = FALSE;
    char iCh;
    int cb;

    if (fEof)
	return(-1);

    cb = 0;
    while (cbBuf>1)
	{

	if ((iCh = GetChar(fh))<0)
	    {
	    fEof = TRUE;
	    if (cb<=0)
		return(-1);
	    break;
	    }
	if (iCh==0x0d || iCh==0x0a)
	    break;
	else if (iCh=='%')
	    {
		iCh = GetChar(fh);
		UngetChar (iCh);
		if (iCh == '(')
			dl_mode = PARTB;
		else if (iCh == ')')
			dl_mode = PARTC;
	    /* Discard a comment */
	    EatLine(fh);
	    continue;
	    }
	else if (dl_mode == PARTA || dl_mode == PARTC)
		{
		EatLine(fh);
		continue;
		}
	else if (iCh==' ' || iCh=='\t')
	    {
	    /* Convert a long white space to a single space */
	    iCh = ' ';
	    EatWhite(fh);
	    }
	*pbBuf++ = iCh;
	--cbBuf;
	++cb;
	}
    if (cb>0)
	{
	*pbBuf++ = '\r';
	++cb;
	*pbBuf++ = '\n';
	++cb;
	}


    return(cb);
    }






main(argc, argv)
int argc;
char **argv;
    {
    short cb;
    int fhIn;
    int fhOut;
    char rgb[512];


    if (argc!=2)
	{
	printf("mkhead: wrong number of parameters\n");
	exit(1);
	}
    ++argv;
    if ((fhIn = open(*argv, O_RDONLY | O_BINARY))>=0)
	{
	if ((fhOut = creat("header.bin", S_IWRITE | S_IREAD))>=0)
	    {
	    close(fhOut);
	    fhOut = open("header.bin", O_RDWR | O_BINARY);

	    cb = 0;
	    write(fhOut, (char *)&cb, sizeof(cb));

	    while ((cb = ReadLine(fhIn, rgb, sizeof(rgb)))>=0)
		{
		if (cb>0)
		    write(fhOut, rgb, cb);
		}

	    cb =  ((short)lseek(fhOut, 0L, 2)) - sizeof(cb);
	    lseek(fhOut, 0L, 0);
	    write(fhOut, (char *)&cb, sizeof(cb));

	    close(fhOut);
	    }
	else
	    printf("mkhead: Can't create header.bin\n");
	close(fhIn);
	}
    else
	printf("mkhead: Can't open %s\n", *argv);
    }
