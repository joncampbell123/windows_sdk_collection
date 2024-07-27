#define NO_STR_OUT 	1
#define NO_CHAR_OUT 	1
#define NO_DRAFTSTRBLT 	1

#include "chp1.c"

long NEAR PASCAL str_out(lpDevice, lpString, count, lpFont, lpDrawMode, lpXform)
LPDEVICE lpDevice;
LPSTR lpString;
short count;
LPFONTINFO lpFont;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpXform;
{
        short basicwidth, width, size, real, breaksize;
        char breakChar;
        short tbreakExtra, breakExtra, breakRem, breakCount, breakErr;
        short far *widthptr;

        if (real = (count > 0)) {
		/* epXRemain is the error term in x direction.  Since
		   the printer cannot move in 1/140"s, the offset is 
		   in 1/840"s */
		lpDevice->epXRemain = 0;	/* no offset yet */
                myWrite(lpDevice, (LPSTR) pcsb + ((LPPRDFONTINFO)lpFont)->bfont.offset, ((LPPRDFONTINFO)lpFont)->bfont.length);
                StartStyle(lpDevice, lpXform);
        } else
                count = - count;

        breakChar = lpFont->dfBreakChar + lpFont->dfFirstChar;

        size = (breaksize = lpFont->dfPixWidth) * count;

        breakCount = lpDrawMode->BreakCount;
        tbreakExtra = lpDrawMode->TBreakExtra;

        if ((tbreakExtra && breakCount) || lpDrawMode->CharExtra) {
                breakRem = lpDrawMode->BreakRem;
                breakErr = lpDrawMode->BreakErr;
                breakExtra = lpDrawMode->BreakExtra;

                for (; count; --count, lpString++)
                    {
                    width = lpDrawMode->CharExtra;

                    if (tbreakExtra && *lpString == breakChar)
                        {
                        width += breakExtra;
                        if ((breakErr -= breakRem) <= 0)
                            {
                            width++;
                            breakErr += breakCount;
                            }
                        }

                    size += width;
                    if (real)
                            char_out(lpDevice, *lpString, breaksize, width);
                    }

                if (!real)
                        lpDrawMode->BreakErr = breakErr;

        } else
                if (real)
			for ( ; count; count--, lpString++)
                            char_out(lpDevice, *lpString, breaksize, 0);

        if (real) {
                EndStyle(lpDevice, lpXform);
                myWrite(lpDevice, (LPSTR) pcsb + ((LPPRDFONTINFO)lpFont)->efont.offset, ((LPPRDFONTINFO)lpFont)->efont.length);
                return size;
        } else
                return MAKELONG(lpFont->dfPixHeight, size);
}

/* width is the extra pixel to insert after the output character */

void NEAR PASCAL char_out(lpDevice, c, xcurwidth, width)
LPDEVICE lpDevice;
unsigned char c;
short xcurwidth;
short width;
{
        short i;

        /* this is only temporary need to check if c is a printing
           character  -- put in the character translation table here */
	if (c < SPACE)
        	myWrite(lpDevice, (LPSTR) SPECIAL_CHAR);
       	myWrite(lpDevice, (LPSTR) &c, 1);

	if (!width)
		return;
        delx.cnt = width * 6 / 7;
	lpDevice->epXRemain += width * 6 % 7;
	if (lpDevice->epXRemain >= 7) {
		lpDevice->epXRemain -= 7;
		delx.cnt++;
	}
        if (delx.cnt > 0)
        	myWrite(lpDevice, (LPSTR) &delx, delx.length);
}

FAR PASCAL DraftStrblt(lpDevice, x, y, lpString, count, lpFont, lpDrawMode, lpXForm, lpRect)
LPDEVICE lpDevice;
short   x          ;
short   y          ;
LPSTR   lpString   ;
short   count      ;
LPFONTINFO lpFont     ;
LPDRAWMODE lpDrawMode ;     /* includes background mode and bkColor */
LPTEXTXFORM lpXForm;
LPRECT  lpRect ;
{
        short charcnt;
        short charmode, i;
        short oldx, breakErr, fFree = 0;
        LPSTR lpString2;

        /* set mode will set the current character width to one that
        is less than or equal to lpFont->dfAveWidth */

        SetMode(lpDevice, lpFont);

        /* check for a word that was broken up into 2 textout calls */
        oldx = x;

        if ((y - lpDevice->epCury >= VDPI / 8) ||
            (y > lpDevice->epCury && x < lpDevice->epXCursPos))
		/* move print head down only when greater than 1/8"
		   or new line */
        	YMoveTo(lpDevice, y);
        else if (x < lpDevice->epXCursPos)
		/* cannot back up in draftmode */
		return FALSE;
        else if ((lpDevice->epMode & BREAKFLAG) && x == lpDevice->epXCursPos)
                /* word broken up into two textout calls */
                x = lpDevice->epCurx;

        if ((lpString2 = CheckString(lpString, count, lpFont)) == (LPSTR) OEM_FAILED)
                return OEM_FAILED;

        if (lpString2 != lpString)
                fFree = TRUE;

        XMoveTo(lpDevice, x, FALSE);
        StartStyle(lpDevice, lpXForm);
        do {
                XMoveTo(lpDevice, x, FALSE);
                /* find how many chars in the next word to be printed */
                charcnt = findword(lpString2, count);
                lpDevice->epCurx += charcnt * lpDevice->epXcurwidth;
		if (lpDevice->epXcurwidth == COMP_MODE_WIDTH)
			/* compressed print has width 8 1/6 device units */
                   	lpDevice->epCurx += (charcnt + 5) / 6;
                /* clip to the page */
                if (x >= PG_ACROSS ||
                    lpDevice->epCurx > PG_ACROSS)
                        break;

                if (x >= 0)
			for ( i = 0; i < charcnt; i++)
                            char_out(lpDevice, *(lpString2 + i), 0, 0);

                if (x < oldx)
                        x = oldx;
                breakErr = lpDrawMode->BreakErr;
                x += (short) StrBlt(lpDevice, 0, 0, (LPRECT)0, lpString2, -charcnt, lpFont, lpDrawMode, lpXForm);
                lpDrawMode->BreakErr = breakErr;
                lpString2 += charcnt;
        } while (count -= charcnt);
        EndStyle(lpDevice, lpXForm);

        if (*(--lpString2) == SPACE)
                lpDevice->epMode &= ~BREAKFLAG;
        else
                lpDevice->epMode |= BREAKFLAG;
        lpDevice->epXCursPos = x;

        if (fFree)
                GlobalFree(HWORD((long)lpString2));

        return TRUE;
}
