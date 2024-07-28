/**[f******************************************************************
 * channel.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/


#include "pscript.h"
#include <winexp.h>
#include "atprocs.h"
#include "atstuff.h"
#include "channel.h"
#include "utils.h"
#include "debug.h"
#include "spooler.h"
#include "resource.h"
#include "psdata.h"
#include "control2.h"
#include "getdata.h"




/*------------------------- local functions -------------------------*/

LPSTR PASCAL QuoteToChannel(LPDV , LPSTR);
LPSTR PASCAL StrToChannel(LPDV, LPSTR);
LPSTR PASCAL IntToChannel(LPDV, LPSTR);
LPSTR PASCAL UnsignedToChannel(LPDV, LPSTR);
LPSTR PASCAL LongToChannel(LPDV, LPSTR);
LPSTR PASCAL GetDecimal(LPSTR, int far *);
LPSTR PASCAL HexToChannel(LPDV , LPSTR, int, BOOL);

void FAR PASCAL OutputDebugString(LPSTR);

#define USE_STRLEN    /* tests show this make things slightly faster */


/****************************************************
* Name: OpenChannel()
*
* Action: Open an output I/O channel.  The output may
*      either go to the spooler or directly to the
*      output port depending on the state of the
*      spooling flag.  The value of the spooling
*      flag was retrieved from the device's environment
*      settings.
* History:  7/27/91 Add optional lpDocInfo parameter
*
*****************************************************/

int FAR PASCAL OpenChannel(lpdv, lszDocument, lpDocInfo)
LPDV lpdv;        /* Far ptr to the device descriptor */
LPSTR lszDocument;  /* Far ptr to the document name */
LPDOCINFO   lpDocInfo;
{
    char szFile[80];    /* for big paths? */
    LPSTR    lpFile;
    int err;

    ASSERT(lpdv);

    DBMSG((">OpenChannel(): fIC=%d,File=%ls\n", lpdv->fContext, lpdv->szFile));

    if (lpdv->fContext) 
    {
        lpdv->fh = -1;    /* Don't do I/O for InfoContext */
    }
    else 
    {

        /* if the output file for EPS is null (lpdv->szFile copied
         * from EpsFile at enable time) we redirect the output to
         * a file by doing our OpenJob with the file "FILE:".  */

        if (!lpdv->fDoEps && lpDocInfo && lpDocInfo->lpszOutput)
            lpFile = lpDocInfo->lpszOutput;
        else
        {
            if (*(lpdv->szFile))
               lpFile = lpdv->szFile;
            else
            {
                LoadString (ghInst, IDS_FILE, szFile, sizeof(szFile));
                lpFile = szFile;
            }
        }

#if 0
        if (!*(lpdv->szFile)) {

            LoadString (ghInst, IDS_FILE, szFile, sizeof(szFile));

        } else
            lstrcpy(szFile, lpdv->szFile);
#endif

#ifdef APPLE_TALK
        if (ATState()) {
            lpdv->fh = ATOpen(szFile, lszDocument, lpdv->hdc);
        } else {
#endif
            if ((lpdv->fh = OpenJob(lpFile, lszDocument, lpdv->hdc)) >= 0) {
                if ((err = StartSpoolPage(lpdv->fh)) < 0)
                    lpdv->fh = err;
            }
#ifdef APPLE_TALK
        }
#endif
    }
    lpdv->cbSpool = 0;        /* empty buffer */
    lpdv->fDirty = FALSE;        /* empty page */

    DBMSG(("<OpenChannel(): fh=%d\n", lpdv->fh));

    return lpdv->fh;
}


/************************************************************
* Name: WriteChannel()
*
* Action: Send output data to the spooler or directly to
*      the output port depending on the state of the
*      spool flag.
*
**************************************************************/

void FAR PASCAL WriteChannel(lpdv, lpbSrc, cbSrc)
LPDV    lpdv;        /* ptr to the device descriptor */
LPSTR    lpbSrc;        /* ptr to the output buffer to send */
register unsigned int    cbSrc;    /* Size of the output buffer */
{
    ASSERT(lpdv);

    DBMSG((">WriteChannel(): fh=%d,#bytes=%u,#spooled=%d\n",
        lpdv->fh, cbSrc, lpdv->cbSpool));

    // Invalid spool handle
    if (lpdv->fh < 0)
        {
        DBMSG((" WriteChannel(): BAD fh\n"));
        return;
        }

        if (!lpdv->fDirty) {
        lpdv->fDirty = TRUE;    /* something has been written on this page */
            StartNewPage(lpdv);
        }

    while (cbSrc-- > 0) {

        /* If the output buffer is full, then flush it */

        if (lpdv->cbSpool >= sizeof(lpdv->rgbSpool)) {
            FlushChannel(lpdv);
        }

        /* Append a byte to the output buffer */
        *(lpdv->rgbSpool + lpdv->cbSpool) = *lpbSrc++;
        ++lpdv->cbSpool;
    }

    DBMSG(("<WriteChannel(): fh=%d,#bytes=%d,#spooled=%d\n",
        lpdv->fh, cbSrc, lpdv->cbSpool));
}


/************************************************************
 * void FAR PASCAL WriteChannelChar(lpdv, ch)
 *
 * write a single character out
 *
 **************************************************************/

void FAR PASCAL WriteChannelChar(lpdv, ch)
LPDV    lpdv;        /* ptr to the device descriptor */
BYTE    ch;
{
    ASSERT(lpdv);

    // Invalid spool handle
    if (lpdv->fh < 0)
        {
        DBMSG((" WriteChannelChar(): BAD fh\n"));
        return;
        }

        if (!lpdv->fDirty) {
        lpdv->fDirty = TRUE;    /* something has been written on this page */
            StartNewPage(lpdv);
        }

    if (lpdv->cbSpool >= sizeof(lpdv->rgbSpool)) {
        FlushChannel(lpdv);
    }

    /* Append a byte to the output buffer */
    *(lpdv->rgbSpool + lpdv->cbSpool) = ch;
    ++lpdv->cbSpool;
}


/**********************************************************************
* Name: CloseChannel()
*
* Action: This routine closes the printer's output channel.
*      Note that the output channel could be connected
*      to the spooler, or directly to the output port.
*
***********************************************************************/

void FAR PASCAL CloseChannel(LPDV lpdv)
{
    ASSERT(lpdv);

    DBMSG((">CloseChannel(): fh=%d\n", lpdv->fh));

    if (lpdv->fh < 0)
        return;

    FlushChannel(lpdv);

    if (lpdv->fh < 0)    // FlusChannel() can make fh == -1
        return;

#ifdef APPLE_TALK
    if (ATState()) {
        ATSendEOF();
        ATClose();
    } else {
#endif
        EndSpoolPage(lpdv->fh);
        CloseJob(lpdv->fh);
#ifdef APPLE_TALK
    }
#endif
    lpdv->fh = -1;        /* Mark the output channel as closed */
    lpdv->cbSpool = 0;


    DBMSG(("<CloseChannel(): fh=%d\n", lpdv->fh));
}


/****************************************************
* Name: FlushChannel()
*
* Action: Flush the buffered output data through the output
*      channel.  If any errors occur, the output channel
*      is closed.
*
******************************************************/

void FAR PASCAL FlushChannel(lpdv)
register LPDV lpdv;
{
    int err;

    ASSERT(lpdv);

    DBMSG((">FlushChannel(): fh=%d,#spooled=%d\n", lpdv->fh,
        lpdv->cbSpool));

    if (lpdv->cbSpool > 0) {

#ifdef APPLE_TALK
        if (ATState()) {

            if (ATWrite(lpdv->rgbSpool, lpdv->cbSpool) < 0) {
                lpdv->cbSpool = 0;
                ATClose();
                lpdv->fh = -1;
            }

        } else {
#endif
            err = WriteSpool(lpdv->fh, lpdv->rgbSpool, lpdv->cbSpool);

            if (err <= 0) {

                DBMSG((" FlushChannel(): JOB ABORT\n"));
#ifdef APPLE_TALK
                if (ATState()) {
                    ATSendEOF();
                    ATClose();
                }
#endif
                DeleteJob(lpdv->fh, 0);
                lpdv->cbSpool = 0;
                lpdv->fh = err;
            }
#ifdef APPLE_TALK
        }
#endif
        lpdv->cbSpool = 0;
    }

    DBMSG(("<FlushChannel(): fh=%d,#spooled=%d\n", lpdv->fh,
        lpdv->cbSpool));
}


/*************************************************************************
* Name: PrintChannel()
*
* Action: This routine does formated printing to the output
*      channel in a manner similar to printf.  The following
*      format conversions are supplied.
*
*          %c  =   character
*          %d  =   decimal
*             %u  =   unsigned decimal
*          %x  =   hexadecimal
*          %q  =   PostScript string (long ptr to bytes plus bytecount)
*          %s  =   Long ptr to string.
*          %ld =   Long decimal
*             %F  =   Print long as 16.16 floating point number
*
*      Digit counts are also allowed before the decimal and hexadecimal
*      format specifications.
*
* If the high word of lsz is 0 then the low word contains a resource ID of
* type PS_DATA.  The resource will be loaded, NullTerminateData called,
* and the resulting string will be used as the format string.
*
*
**************************************************************************/

void FAR PrintChannel(LPDV lpdv, LPSTR lsz, ...)
{
    char    bCh;
    LPSTR    lpbParams;
    int    cDigits;
        long far *lpl;
        BOOL    bResource;
        HANDLE  hCtl;

    ASSERT(lpdv);
    ASSERT(lsz);

    DBMSG((">PrintChannel()\n"));

        /* If the control string is a resource ID load the resource first */
        bResource = !HIWORD((DWORD)lsz);
        if (bResource) {
            lsz = GetResourceData(&hCtl, MAKEINTRESOURCE(LOWORD((DWORD)lsz)),
                                  MAKEINTRESOURCE(PS_DATA));
            if (!lsz)
                return;
            NullTerminateData(lsz, TERMCHAR);
        }

    lpbParams = ((LPSTR) & lsz) + sizeof(LPSTR);
    while (*lsz) {
        bCh = *lsz++;
        if (bCh == '%') {
            lsz = GetDecimal(lsz, (int far * ) & cDigits);
            switch (*lsz++) {
            case 'F':    /* fixed point number */
                                lpl = (long far *) lpbParams;
                                if (*lpl < 0) {
                                    WriteChannelChar(lpdv, '-');
                                    *lpl = -*lpl;
                                }

                UnsignedToChannel(lpdv, lpbParams+2);
                                WriteChannelChar(lpdv, '.');
                                WriteChannelChar(lpdv, '0');
//                UnsignedToChannel(lpdv, lpbParams);

                                lpbParams += 4;
                break;

            case 'l':    /* Long word */
                lsz++;    /* skip 'd' */
                lpbParams = LongToChannel(lpdv, lpbParams);
                break;
            case 'd':    /* Decimal word */
                lpbParams = IntToChannel(lpdv, lpbParams);
                break;
            case 'x':    /* Hex word */
                        case 'X':
                lpbParams = HexToChannel(lpdv, lpbParams, cDigits, 
                                                         (bCh == 'X' ? TRUE : FALSE));
                break;
            case 's':    /* String */
                lpbParams = StrToChannel(lpdv, lpbParams);
                break;
            case 'q':    /* Post-Script string and count */
                lpbParams = QuoteToChannel(lpdv, lpbParams);
                break;
            case 'c':
                WriteChannelChar(lpdv, *(LPSTR)lpbParams);
                ++((int far * )lpbParams);
                break;
            case '%':
                WriteChannelChar(lpdv, bCh);
                break;
            default:
                goto exit;
            }
        } else {
            if (bCh == 0x0a) {
                bCh = 0x0d;
                WriteChannelChar(lpdv, bCh);
                bCh = 0x0a;
            }

                        if (bCh != 0x0d)
                WriteChannelChar(lpdv, bCh);
        }
    }
exit:
        if (bResource)
            UnGetResourceData(hCtl);

    DBMSG(("<PrintChannel()\n"));
}


/*******************************************************************
 * Name: IntToChannel()
 *
 * Action: This routine is called by the PrintChannel when
 *      it encounters a "%ld" or a "%d" in a format string.
 *      It prints the value on the parameter stack as a
 *      signed decimal value, bumps the parameter stack
 *      ptr past the value and returns this ptr.
 *
 * this routine has been seperated from the old IntToChannel to increase
 * speed (use short math instead of long).  No one uses "%ld" anyway...
 *
 *******************************************************************/

LPSTR PASCAL IntToChannel(lpdv, lpbParams)
LPDV    lpdv;
LPSTR    lpbParams;
{
    int    iValue;
    BOOL    fIsNeg;
    char    rgb[13];
    LPSTR    lpbDst;

    ASSERT(lpdv);
    ASSERT(lpbParams);

    lpbDst = ((LPSTR) rgb) + sizeof(rgb);
    *--lpbDst = 0;

    iValue = *((int far * )lpbParams)++;

    if (fIsNeg = (iValue < 0))
        iValue = -iValue;

    while (iValue != 0) {
        *--lpbDst = (char)((iValue % 10) + '0');
        iValue /= 10;
    }

    if (*lpbDst == 0)
        *--lpbDst = '0';
    if (fIsNeg)
        *--lpbDst = '-';

#ifdef USE_STRLEN

    WriteChannel(lpdv, lpbDst, lstrlen(lpbDst));

#else

    while (*lpbDst)
        WriteChannelChar(lpdv, *lpbDst++);
#endif
    return lpbParams;
}


/*******************************************************************
 * Name: UnsignedToChannel()
 *
 * Action: This routine is called by the PrintChannel when
 *      it encounters a "%u" in a format string.
 *      It prints the value on the parameter stack as an
 *      unsigned decimal value, bumps the parameter stack
 *      ptr past the value and returns this ptr.
 *
 *******************************************************************/

LPSTR PASCAL UnsignedToChannel(lpdv, lpbParams)
LPDV    lpdv;
LPSTR    lpbParams;
{
    unsigned uValue;
    char    rgb[13];
    LPSTR    lpbDst;

    ASSERT(lpdv);
    ASSERT(lpbParams);

    lpbDst = ((LPSTR) rgb) + sizeof(rgb);
    *--lpbDst = 0;

    uValue = *((int far * )lpbParams)++;

    while (uValue != 0) {
        *--lpbDst = (char)((uValue % 10) + '0');
        uValue /= 10;
    }

    if (*lpbDst == 0)
        *--lpbDst = '0';

#ifdef USE_STRLEN

    WriteChannel(lpdv, lpbDst, lstrlen(lpbDst));

#else

    while (*lpbDst)
        WriteChannelChar(lpdv, *lpbDst++);
#endif
    return lpbParams;
}

/*******************************************************************
* Name: LongToChannel()
*
* Action: This routine is called by the PrintChannel when
*      it encounters a "%ld" in a format string.
*      It prints the value on the parameter stack as a
*      signed decimal value, bumps the parameter stack
*      ptr past the value and returns this ptr.
*
* this routine has been seperated from the old IntToChannel to increase
* speed.  No one uses "%ld" anyway...
*
********************************************************************/

LPSTR PASCAL LongToChannel(lpdv, lpbParams)
LPDV    lpdv;
LPSTR    lpbParams;
{
    long    lValue;
    BOOL    fIsNeg;
    char    rgb[13];
    LPSTR    lpbDst;

    ASSERT(lpdv);
    ASSERT(lpbParams);

     DBMSG1(("LongToChannel() %ld hex:%x%x\n", lValue, 
         HIWORD(lValue), LOWORD(lValue)));

    lpbDst = ((LPSTR) rgb) + sizeof(rgb);
    *--lpbDst = 0;

    lValue = *((long far * ) lpbParams)++;

    if (fIsNeg = (lValue < 0))
        lValue = -lValue;

    while (lValue != 0) {
        *--lpbDst = (char)(lmod(lValue, 10L) + '0');
        lValue = ldiv(lValue, 10L);
    }

    if (*lpbDst == 0)
        *--lpbDst = '0';
    if (fIsNeg)
        *--lpbDst = '-';

#ifdef USE_STRLEN

    WriteChannel(lpdv, lpbDst, lstrlen(lpbDst));

#else

    while (*lpbDst)
        WriteChannelChar(lpdv, *lpbDst++);
#endif

    return lpbParams;
}

/*****************************************************************
* Name: HexToChannel()
*
* Action: Output a hexadecimal number to the channel.
*      The output value will have leading zeros if
*      the digit count exceeds the number of digits
*      required to print the enitire value.
*
******************************************************************/

LPSTR PASCAL HexToChannel(lpdv, lpbParams, cDigits, bUpperCase)
LPDV lpdv;        /* Far ptr to the device descriptor */
LPSTR lpbParams;    /* Far ptr into the parameter stack */
int    cDigits;        /* The number of digits to print */
BOOL bUpperCase;
{
    unsigned int    iValue;
    BOOL    fZeros;    /* TRUE if leading zeros should be printed */
    char    rgb[5];
    LPSTR    lpbDst;
    char    bCh, bBase;

    ASSERT(lpdv != NULL);
    ASSERT(lpbParams != NULL);

        bBase = (char)(bUpperCase ? 'A' : 'a');

    if (cDigits >= sizeof(rgb))
        cDigits = sizeof(rgb) - 1;
    fZeros = cDigits;

    iValue = *((int far * )lpbParams)++;

    /* Fill the buffer with the digits in reverse order */
    lpbDst = ((LPSTR)rgb) + sizeof(rgb);
    *--lpbDst = 0;
    while (iValue != 0 && --cDigits >= 0) {
        bCh = (char)(iValue & 0x0f);
        *--lpbDst = (char)(bCh > 9 ? bCh + bBase - 10 : bCh + '0');
        iValue = (iValue >> 4) & 0x0fff;
    }


    /* Ensure that there is at lease one digit */
    if (*lpbDst == 0) {
        *--lpbDst = '0';
        --cDigits;
    }

    /* Print any leading zeros */
    if (fZeros)
        while (--cDigits >= 0)
            *--lpbDst = '0';

    /* Write the digits (in the correct order) to the output channel */

#ifdef USE_STRLEN

    WriteChannel(lpdv, lpbDst, lstrlen(lpbDst));

#else

    while (*lpbDst)
        WriteChannelChar(lpdv, *lpbDst++);
#endif

    return lpbParams;
}


/**********************************************************************
* Name: GetDecimal()
*
* Action: This routine converts an ascii decimal number (from the
*      format string) to a binary integer. The format string ptr
*      is updated past the number.
*
*      If there is no number at the current position in the format
*      string, then the value defaults to zero.
*
***********************************************************************/

LPSTR PASCAL GetDecimal(lsz, lpiDigits)
LPSTR lsz;
int    far *lpiDigits;
{

    ASSERT(lsz != NULL);
    ASSERT(lpiDigits != NULL);

    *lpiDigits = 0;
    if (lsz) {
        while (*lsz) {
            if (*lsz >= '0' && *lsz <= '9')
                *lpiDigits = (*lpiDigits * 10) + (*lsz++ - '0');
            else
                break;
        }
    }

        // if no digits were specified assume max
        if (!*lpiDigits)
            *lpiDigits = 0x7fff;

    return lsz;
}

LPSTR FAR PASCAL farGetDecimal(LPSTR lsz, int FAR *lpiDigits)
{
   return GetDecimal(lsz, lpiDigits);
}

/*******************************************************************
* Name: StrToChannel
*
* Action: This routine is called by the PrintChannel function
*      when it encounters a "%s" in the format string.
*      The pointer to the string is extracted from the
*      parameter stack, the string is printed, and the
*      updated parameter ptr is returned.
*
********************************************************************/

LPSTR PASCAL StrToChannel(lpdv, lpbParams)
LPDV lpdv;
LPSTR lpbParams;
{

    LPSTR lsz;

    ASSERT(lpdv);
    ASSERT(lpbParams);


    if (lsz = *((LPSTR far * ) lpbParams)++) {
#ifdef USE_STRLEN
        WriteChannel(lpdv, lsz, lstrlen(lsz));
#else
        while (*lsz)
            WriteChannelChar(lpdv, *lsz++);
#endif
    }
    return lpbParams;
}


/*******************************************************************
* Name: QuoteToChannel()
*
* Action: This routine is called by the PrintChannel function
*      when it encounters a "%q" in the format string.  The
*      "%q" indicates the string on the parameter stack is
*      to be printed as a Post-Script quoted string.  Both
*      the string ptr and byte-count are given as parameters.
*
* Example of usage:  PrintChannel(lpdv, "%q", lpStr, cb)
*
********************************************************************/

LPSTR PASCAL QuoteToChannel(lpdv, lpbParams)
LPDV lpdv;
LPSTR lpbParams;
{
    LPSTR    lpb;         /* Far ptr to the source string */
    int    cb;        /* The source string length */
    int    i;         /* A simple counter */
    char    bCh;        /* A character from the source string */
    int    iCh;

    ASSERT(lpdv);
    ASSERT(lpbParams);

    /* Get a pointer to the string and its bytecount */
    lpb = *((LPSTR far * ) lpbParams)++;
    cb = *((short int far * ) lpbParams)++;

    ASSERT(lpb);
    ASSERT(cb >= 0);

    /* Print the quoted string by surrounding it with parenthesis */
    WriteChannelChar(lpdv, '(');

    while (--cb >= 0) {
        iCh = *lpb++ & 0x0ff;

        /* msd - 10/26/88:  Disable this check here because we pre-parse the
         *    string and replace any characters outside the range of dfFirstChar
         *    and dfLastChar with dfDefaultChar (this is done in DumpStr()).
         *
         *    if (iCh<' ')
         *        continue;
         */

        bCh = (char)iCh;

        /* Check for a special character */
        switch (bCh) {
        case '(':
        case ')':
        case '\\':
            WriteChannelChar(lpdv, '\\');
            WriteChannelChar(lpdv, bCh);
            break;

        default:
            if (iCh > 31 && iCh < 127)
                WriteChannelChar(lpdv, bCh);
            else {
                /* Output anything greater than 127 as octal */
                WriteChannelChar(lpdv, '\\');
                for (i = 0; i < 3; ++i) {
                    bCh = (char)(((iCh >> 6) & 0x07) + '0');
                    iCh <<= 3;
                    WriteChannelChar(lpdv, bCh);
                }
            }
            break;
        }
    }

    WriteChannelChar(lpdv, ')');
    return lpbParams;
}
