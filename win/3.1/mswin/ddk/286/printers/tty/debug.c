/**[f******************************************************************
 * debug.c - 
 *
 * Copyright (C) 1988,1989 Aldus Corporation
 * Copyright (C) 1988-1990 Microsoft Corporation.
 * All rights reserved.  Company confidential.
 *
 **f]*****************************************************************/

/***************************************************************************/
/*******************************   debug.c   *******************************/
/*
 *  Debug module.
 *
 *  rev:
 *
 *  05 dec 89	peterbe	add DBGclose()
 *  11-14-86    msd     compile for Wrning level 2
 */


#include "print.h"

#define DEBUG_PORT 1        /* Do you want to change both of these? */
#define DEBUG_FILE "COM1"   /* Could be a file if DEBUG_PORT is undef'd */

typedef char *NPSTR;        /* A near ptr to a string */

static int fhDebug = -1;    /* The debugging console file handle */

int far PASCAL _lopen(LPSTR, int);

int FAR PASCAL GimmeDBGfile(void);
LPSTR FAR PASCAL DoFormat(int, LPSTR, LPSTR far *);
LPSTR FAR PASCAL PutDec(int, int, BOOL, LPSTR);
LPSTR FAR PASCAL PutHex(int, int, BOOL, LPSTR);
LPSTR FAR PASCAL MyPutChar(int, int far *);
LPSTR FAR PASCAL PutString(int, BOOL, LPSTR);
void FAR PASCAL CharToFile(int, char);
LPSTR FAR PASCAL GetDigits(LPSTR, int far *);


int FAR PASCAL GimmeDBGfile()
    {
    if (fhDebug < 0)
        fhDebug = _lopen((LPSTR)DEBUG_FILE, OF_READWRITE);
    return (fhDebug);
    }

void FAR PASCAL DBGclose()
    {
    if (fhDebug >= 0)
	_lclose(fhDebug);
    }


#if 0
void cdecl DBGprint(lsz)
LPSTR lsz;
    {
    char bCh;
    LPSTR lpbParams;


    if (fhDebug<0)
    {
    fhDebug = _lopen((LPSTR)DEBUG_FILE, OF_READWRITE);
    if (fhDebug<0)
        return;
    }

    lpbParams = ((LPSTR)&lsz) + sizeof(LPSTR);
    while (*lsz)
    {
    bCh = *lsz++;
    if (bCh=='%')
        {
        lsz = DoFormat(fhDebug, lsz, (LPSTR far *) &lpbParams);
        }
    else
        {
        if (bCh=='\\')
        {
        switch(bCh = *lsz++)
            {
            case 'n':
            bCh = 0x0a;
            break;
            case 't':
            bCh = 0x09;
            break;
            case 0:
            goto exit;
            default:
            break;
            }
        }
        CharToFile(fhDebug, bCh);
        }
    }
exit:
    ;

    }
#endif





LPSTR FAR PASCAL DoFormat(fh, lsz, lppbParams)
int fh;
LPSTR lsz;
LPSTR far *lppbParams;
    {
    int cDigits;
    BOOL fIsLong;



    cDigits = 0;
    lsz = GetDigits(lsz, (int far *) &cDigits);
    if (fIsLong = *lsz=='l')
    ++lsz;
    switch(*lsz)
    {
    case 'd':   /* Decimal long or int */
        *lppbParams = (LPSTR) PutDec(fh, cDigits, fIsLong, *lppbParams);
        ++lsz;
        break;
    case 'c':   /* A single character */
        *lppbParams = (LPSTR) MyPutChar(fh, (int far *) *lppbParams);
        ++lsz;
        break;
    case 'x':   /* Hexadecimal long or int */
        *lppbParams = (LPSTR) PutHex(fh, cDigits, fIsLong, *lppbParams);
        ++lsz;
        break;
    case 's':   /* A long or short string */
        *lppbParams = (LPSTR) PutString(fh, fIsLong, *lppbParams);
        ++lsz;
        break;
    case 'p':   /* A long or short pointer */
        if (fIsLong)
        {
        PutHex(fh, 4, FALSE, *lppbParams + 2);
        CharToFile(fh, ':');
        PutHex(fh, 4, FALSE, *lppbParams);
        *lppbParams += 4;
        }
        else
        *lppbParams = (LPSTR) PutHex(fh, 4, FALSE, *lppbParams);
        ++lsz;
        break;
    }


    return(lsz);
    }


LPSTR FAR PASCAL PutDec(fh, cDigits, fIsLong, lpbParams)
int fh;
int cDigits;
BOOL fIsLong;
LPSTR lpbParams;
    {
    long lValue;
    BOOL fIsNeg;
    char rgb[32];
    LPSTR lpbDst;


    if (cDigits>30)
    cDigits = 30;
    lpbDst = ((LPSTR) rgb) + sizeof(rgb);
    *--lpbDst = 0;
    if (fIsLong)
    lValue = *((long far *) lpbParams)++;
    else
    lValue = *((int far *)lpbParams)++;
    if (fIsNeg = (lValue<0))
    lValue = - lValue;

    while (lValue!=0)
    {
    *--lpbDst = (char)(lValue % 10L) + '0';
    lValue = (lValue/10L);
    --cDigits;
    }
    if (*lpbDst==0)
    {
    *--lpbDst = '0';
    --cDigits;
    }
    while(--cDigits>=0)
    CharToFile(fh, '0');
    if (fIsNeg)
    CharToFile(fh, '-');
    while (*lpbDst)
    CharToFile(fh, *lpbDst++);


    return(lpbParams);
    }





LPSTR FAR PASCAL PutHex(fh, cDigits, fIsLong, lpbParams)
int fh;
int cDigits;
BOOL fIsLong;
LPSTR lpbParams;
    {
    unsigned long lValue;
    int i;
    char rgb[32];
    LPSTR lpbDst;
    char bCh;



    if (cDigits>(sizeof(rgb)-1))
    cDigits = sizeof(rgb) - 1;
    lpbDst = ((LPSTR)rgb) + sizeof(rgb);
    *--lpbDst = 0;
    if (fIsLong)
    lValue = *((long far *)lpbParams)++;
    else
    lValue = (*((int far *)lpbParams)++) & 0x0ffffL;

    for (i=0; i<(sizeof(rgb)-2) && lValue!=0 && cDigits>0; ++i)
    {
    bCh = (char)(lValue & 0x0fL);
    *--lpbDst = bCh > 9 ? bCh + 'A' - 10: bCh + '0';
    lValue = (lValue/16L);
    --cDigits;
    }

    if (*lpbDst==0)
    {
    *--lpbDst = '0';
    --cDigits;
    }
    while(--cDigits>=0)
    CharToFile(fh, '0');
    while (*lpbDst)
    CharToFile(fh, *lpbDst++);

    return(lpbParams);
    }



/* Name: MyPutChar()
*
* Action: Send a character to the current debugging console
*     from the parameter stack.
*
*/
LPSTR FAR PASCAL MyPutChar(fh, lpi)
int fh;         /* The file handle to write to */
int far *lpi;       /* Far ptr to the 16 bit char to output */
    {
    CharToFile(fh, (char)*lpi++);
    return ((LPSTR) lpi);
    }





LPSTR FAR PASCAL PutString(fh, fIsLong, lp)
int fh;
BOOL fIsLong;
LPSTR lp;
    {
    static char szNull[] = "{ NULL }";

    LPSTR lpb;


    if (fIsLong)
    lpb = *((LPSTR far *) lp)++;
    else
    lpb = (LPSTR) *((NPSTR far *) lp)++;
    if (!lpb)
    lpb = (LPSTR) szNull;
    while (*lpb)
    CharToFile(fh, *lpb++);
    return ((LPSTR) lp);
    }





LPSTR FAR PASCAL GetDigits(lsz, lpiDigits)
LPSTR lsz;
int far *lpiDigits;
    {

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



#if defined(DEBUG_PORT)

#include <bios.h>

#endif



void FAR PASCAL CharToFile(fh, bCh)
int fh;
char bCh;
    {

#if defined(DEBUG_PORT)
    /* Pause until terminal key hit if ^S received on com port */

    if (_bios_serialcom(_COM_STATUS,DEBUG_PORT-1,0) & 0x0100)
    if ((_bios_serialcom(_COM_RECEIVE,DEBUG_PORT-1,0) & 0x7f) == 'S'-0x40)
        while (_bios_serialcom(_COM_RECEIVE,DEBUG_PORT-1,0) & 0x8000)
        ;
#endif

    if (bCh == '\033')
    _lwrite(fh, (LPSTR) "\\ESC", 4);
    else
    _lwrite(fh, (LPSTR) &bCh, 1);
    if (bCh==0x0a)
    {
    bCh = 0x0d;
    _lwrite(fh, (LPSTR) &bCh, 1);
    }
    }
