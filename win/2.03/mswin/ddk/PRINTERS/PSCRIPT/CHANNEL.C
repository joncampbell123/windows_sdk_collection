#include "pscript.h"


LPSTR PASCAL QuoteToChannel(DV FAR *, LPSTR);
LPSTR PASCAL StrToChannel(DV FAR *, LPSTR);
LPSTR PASCAL IntToChannel(DV FAR *, BOOL, LPSTR);
LPSTR HexToChannel(DV FAR *, LPSTR, int);
LPSTR PASCAL GetDecimal(LPSTR, int far *);

long FAR PASCAL ldiv(long, long);
long FAR PASCAL lmod(long, long);


/****************************************************
* Name: OpenChannel()
*
* Action: Open an output I/O channel.  The output may
*	  either go to the spooler or directly to the
*	  output port depending on the state of the
*	  spooling flag.  The value of the spooling
*	  flag was retrieved from the device's environment
*	  settings.
*
****************************************************
*/
int FAR PASCAL OpenChannel(lpdv, lszDocument)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
LPSTR lszDocument;  /* Far ptr to the document name */
    {

    static char szNull[] = "NULL";
    LPSTR lszFile;  /* Far ptr to the output file name */


    ASSERT(lpdv!=NULL);


    if (lpdv->dh.fContext)
	lpdv->dh.fh = -1;	/* Don't do I/O for InfoContext */
    else
	{
	/* Use the NULL output device if the file name string is empty */
	if (*(lszFile = lpdv->szFile) == 0)
	    lszFile = (LPSTR) szNull;


	if ((lpdv->dh.fh = OpenJob(lszFile, lszDocument, lpdv->dh.hdc))>=0)
	    {
	    StartSpoolPage(lpdv->dh.fh);
	    }

	}
    lpdv->dh.cbSpool = 0;
    lpdv->dh.fDirty = FALSE;
    return(lpdv->dh.fh);
    }


/************************************************************
* Name: WriteChannel()
*
* Action: Send output data to the spooler or directly to
*	  the output port depending on the state of the
*	  spool flag.
*
**************************************************************
*/
void FAR PASCAL WriteChannel(lpdv, lpbSrc, cbSrc)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
LPSTR lpbSrc;	    /* Far ptr to the output buffer to send */
int cbSrc;	    /* Size of the output buffer */
    {


    ASSERT(lpdv!=NULL);
    ASSERT(cbSrc>=0);

    if (lpdv->dh.fh>=0)     /* Check to ensure an open channel */
	{
	lpdv->dh.fDirty = TRUE;
	while (--cbSrc>=0)
	    {
	    /* If the output buffer is full, then flush it */
	    if (lpdv->dh.cbSpool>=sizeof(lpdv->rgbSpool))
		{
		FlushChannel(lpdv);
		if (lpdv->dh.fh<0)
		    break;
		}

	    /* Append a byte to the output buffer */
	    *(lpdv->rgbSpool + lpdv->dh.cbSpool) = *lpbSrc++;
	    ++lpdv->dh.cbSpool;
	    }
	}
    }


/**************************************************
* Name: CloseChannel()
*
* Action: This routine closes the printer's output channel.
*	  Note that the output channel could be connected
*	  to the spooler, or directly to the output port.
*
**************************************************
*/
void FAR PASCAL CloseChannel(lpdv)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
    {
    ASSERT(lpdv!=NULL);


    if (lpdv->dh.fh>=0) 	       /* Check for valid file handle */
	{
	FlushChannel(lpdv);
	EndSpoolPage(lpdv->dh.fh);
	CloseJob(lpdv->dh.fh);
	lpdv->dh.fh = -1;	    /* Mark the output channel as closed */
	lpdv->dh.cbSpool = 0;
	}
    }


/****************************************************
* Name: FlushChannel()
*
* Action: Flush the buffered output data through the output
*	  channel.  If any errors occur, the output channel
*	  is closed.
*
*****************************************************
*/
void FAR PASCAL FlushChannel(lpdv)
DV FAR *lpdv;		/* Far ptr to the device descriptor */
    {
    ASSERT(lpdv!=NULL);


    if (lpdv->dh.fh>=0 && lpdv->dh.cbSpool>0)
	{

	if (WriteSpool(lpdv->dh.fh, (LPSTR) lpdv->rgbSpool, lpdv->dh.cbSpool)<=0)
	    {
	    lpdv->dh.cbSpool = 0;
	    CloseChannel(lpdv);
	    }
	lpdv->dh.cbSpool = 0;
	}
    }


/*********************************************************************
* Name: PrintChannel()
*
* Action: This routine does formated printing to the output
*	  channel in a manner similar to printf.  The following
*	  format conversions are supplied.
*
*	      %c  =   character
*	      %d  =   decimal
*	      %x  =   hexadecimal
*	      %q  =   PostScript string (long ptr to bytes plus bytecount )
*	      %ls =   Long ptr to string.
*	      %lx =   Long hexadecimal.
*	      %ld =   Long decimal
*
*	  Digit counts are also allowed before the decimal and hexadecimal
*	  format specifications.
*
**************************************************************************
*/
void FAR PrintChannel(lpdv, lsz)
DV FAR *lpdv;
LPSTR lsz;
    {
    char bCh;
    LPSTR lpbParams;
    BOOL fIsLong;
    int cDigits;

    ASSERT(lpdv!=NULL);
    ASSERT(lsz!=NULL);


    lpbParams = ((LPSTR)&lsz) + sizeof(LPSTR);
    while (*lsz)
	{
	bCh = *lsz++;
	if (bCh=='%')
	    {
	    lsz = GetDecimal(lsz, (int far *) &cDigits);
	    fIsLong = FALSE;
	    switch(*lsz++)
		{
		case 'l':	/* Long word */
		    fIsLong = TRUE;
		    break;
		case 'd':	/* Decimal word */
		    lpbParams = IntToChannel(lpdv, fIsLong, lpbParams);
		    break;
		case 'x':	/* Hex word */
		    lpbParams = HexToChannel(lpdv, lpbParams, cDigits);
		    break;
		case 's':	/* String */
		    lpbParams = StrToChannel(lpdv, lpbParams);
		    break;
		case 'q':	/* Post-Script string and count */
		    lpbParams = QuoteToChannel(lpdv, lpbParams);
		    break;
		case 'c':
		    WriteChannel(lpdv, lpbParams, 1);
		    ++((int far *)lpbParams);
		    break;
		case '%':
		    WriteChannel(lpdv, (LPSTR)&bCh, 1);
		    break;
		default:
		    goto exit;
		}
	    }
	else
	    {
	    if (bCh==0x0a)
		{
		bCh = 0x0d;
		WriteChannel(lpdv, (LPSTR) &bCh, 1);
		bCh = 0x0a;
		}
	    WriteChannel(lpdv, (LPSTR) &bCh, 1);
	    }
	}
exit:
    ;
    }


/*******************************************************************
* Name: IntToChannel()
*
* Action: This routine is called by the PrintChannel when
*	  it encounters a "%ld" or a "%d" in a format string.
*	  It prints the value on the parameter stack as a
*	  signed decimal value, bumps the parameter stack
*	  ptr past the value and returns this ptr.
*
*********************************************************************
*/
LPSTR PASCAL IntToChannel(lpdv, fIsLong, lpbParams)
	DV FAR *lpdv;
	BOOL fIsLong;
	LPSTR lpbParams;
{
    long lValue;
    BOOL fIsNeg;
    char rgb[13];
    LPSTR lpbDst;

    ASSERT(lpdv!=NULL);
    ASSERT(lpbParams!=NULL);

    lpbDst = ((LPSTR) rgb) + sizeof(rgb);
    *--lpbDst = 0;
    if (fIsLong)
		lValue = *((long far *) lpbParams)++;
    else
		lValue = *((int far *)lpbParams)++;
    if (fIsNeg = (lValue<0))
		lValue = - lValue;
    while (lValue!=0){
		*--lpbDst = (char)lmod(lValue,10L) + '0';
		lValue = ldiv(lValue,10L);
	}
    if (*lpbDst==0)
		*--lpbDst = '0';
    if (fIsNeg)
		*--lpbDst = '-';
    while (*lpbDst)
		WriteChannel(lpdv, lpbDst++, 1);
    return(lpbParams);
}


/*****************************************************************
* Name: HexToChannel()
*
* Action: Output a hexadecimal number to the channel.
*	  The output value will have leading zeros if
*	  the digit count exceeds the number of digits
*	  required to print the enitire value.
*
******************************************************************
*/
LPSTR HexToChannel(lpdv, lpbParams, cDigits)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
LPSTR lpbParams;    /* Far ptr into the parameter stack */
int cDigits;	    /* The number of digits to print */
    {
    unsigned int iValue;
    BOOL fZeros;    /* TRUE if leading zeros should be printed */
    char rgb[5];
    LPSTR lpbDst;
    char bCh;

    ASSERT(lpdv!=NULL);
    ASSERT(lpbParams!=NULL);


    if (cDigits>=sizeof(rgb))
	cDigits = sizeof(rgb) - 1;
    fZeros = cDigits;

    iValue = *((int far *)lpbParams)++;

    /* Fill the buffer with the digits in reverse order */
    lpbDst = ((LPSTR)rgb) + sizeof(rgb);
    *--lpbDst = 0;
    while(iValue!=0 && --cDigits>=0)
	{
	bCh = (char)(iValue & 0x0f);
	*--lpbDst = bCh > 9 ? bCh + 'a' - 10: bCh + '0';
	iValue = (iValue >> 4) & 0x0fff;
	}


    /* Ensure that there is at lease one digit */
    if (*lpbDst==0)
	{
	*--lpbDst = '0';
	--cDigits;
	}

    /* Print any leading zeros */
    if (fZeros)
	while(--cDigits>=0)
	    *--lpbDst = '0';

    /* Write the digits (in the correct order) to the output channel */
    while (*lpbDst)
	WriteChannel(lpdv, lpbDst++, 1);
    return(lpbParams);
    }


/**********************************************************************
* Name: GetDecimal()
*
* Action: This routine converts an ascii decimal number (from the
*	  format string) to a binary integer. The format string ptr
*	  is updated past the number.
*
*	  If there is no number at the current position in the format
*	  string, then the value defaults to zero.
*
************************************************************************
*/

LPSTR PASCAL GetDecimal(lsz, lpiDigits)
LPSTR lsz;
int far *lpiDigits;
    {
    char bCh;

    ASSERT(lsz!=NULL);
    ASSERT(lpiDigits!=NULL);

    *lpiDigits = 0;
    if (lsz)
	{
	while (*lsz)
	    {
	    if (*lsz>='0' && *lsz<='9')
		*lpiDigits = (*lpiDigits * 10) + (*lsz++ - '0');
	    else
		break;
	    }
	}
    return lsz;
    }


/*******************************************************************
* Name: StrToChannel
*
* Action: This routine is called by the PrintChannel function
*	  when it encounters a "%s" in the format string.
*	  The pointer to the string is extracted from the
*	  parameter stack, the string is printed, and the
*	  updated parameter ptr is returned.
*
********************************************************************
*/
LPSTR PASCAL StrToChannel(lpdv, lpbParams)
DV FAR *lpdv;
LPSTR lpbParams;
    {

    LPSTR lsz;

    ASSERT(lpdv!=NULL);
    ASSERT(lpbParams!=NULL);


    if (lsz = *((LPSTR far *) lpbParams)++)
	{
	while (*lsz)
	    WriteChannel(lpdv, lsz++, 1);
	}
    return ((LPSTR) lpbParams);
    }


/*******************************************************************
* Name: QuoteToChannel()
*
* Action: This routine is called by the PrintChannel function
*	  when it encounters a "%q" in the format string.  The
*	  "%q" indicates the string on the parameter stack is
*	  to be printed as a Post-Script quoted string.  Both
*	  the string ptr and byte-count are given as parameters.
*
* Example of usage:  PrintChannel(lpdv, "%q", lpStr, cb)
*
**********************************************************************
*/
LPSTR PASCAL QuoteToChannel(lpdv, lpbParams)
DV FAR *lpdv;
LPSTR lpbParams;
    {
    LPSTR lpb;		     /* Far ptr to the source string */
    int cb;		     /* The source string length */
    int i;		     /* A simple counter */
    char bCh;		     /* A character from the source string */
    int iCh;

    ASSERT(lpdv!=NULL);
    ASSERT(lpbParams!=NULL);


    /* Get a pointer to the string and its bytecount */
    lpb = *((LPSTR far *) lpbParams)++;
    cb = *((short int far *) lpbParams)++;

    ASSERT(lpb!=NULL);
    ASSERT(cb>=0);


    /* Print the quoted string by surrounding it with parenthesis */
    WriteChannel(lpdv, (LPSTR) "(", 1);
    while (--cb>=0)
	{
	iCh = *lpb++ & 0x0ff;
	if (iCh<' ')
	    continue;
	bCh = (char)iCh;

	/* Check for a special character */
	switch(bCh)
	    {
	    case '(':
	    case ')':
	    case '\\':
		WriteChannel(lpdv, (LPSTR) "\\", 1);
		WriteChannel(lpdv, (LPSTR) &bCh, 1);
		break;
	    default:
		if (iCh<127)
		    WriteChannel(lpdv, (LPSTR) &bCh, 1);
		else
		    {
		    /* Output anything greater than 127 as octal */
		    WriteChannel(lpdv, (LPSTR) "\\", 1);
		    for (i=0; i<3; ++i)
			{
			bCh = (char)((iCh >> 6) & 0x07) + '0';
			iCh <<= 3;
			WriteChannel(lpdv, (LPSTR) &bCh, 1);
			}
		    }
		break;
	    }
	}

    WriteChannel(lpdv, (LPSTR) ")", 1);
    return(lpbParams);
    }

