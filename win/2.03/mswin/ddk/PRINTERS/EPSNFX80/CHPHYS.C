#include <generic.h>

/* b is the high order word */
PSTR pcsb = (PSTR) pcsbtab;
extern   GRAPHRESET reset;
extern DEVMODE devmode;

#if NO_COLOR_STR_OUT
#elif COLOR
long NEAR PASCAL color_str_out(lpDevice, lpString, count, lpFont, lpDrawMode, lpXform, x, y)
LPDEVICE lpDevice;
LPSTR lpString;
short count;
LPFONTINFO lpFont;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpXform;
short x;
short y;
{
        long size;
        BYTE pColor;
        short i;

	pColor = (~((LPPHYSCOLOR)&(lpDrawMode->TextColor))->Mono) & 0x07;

	/* WHITE = 0x00; BLACK = 0x07 */
	if (pColor == WHITE) {
	    return 0;
	} else if (pColor == BLACK || lpDevice->epBmpHdr.bmPlanes == 1) {
	    i = BLACK;
	    goto string_out;
	} else {
	    /* CYAN = 0x01; MAGENTA = 0x02; YELLOW = 0x04 */
	    for (i = YELLOW; i; i >>= 1) { 
		if (pColor & i) {
string_out:
        	    lpDevice->epPtr = 0;	/* clear the buffer */
		    myWrite(lpDevice, (LPSTR)CMYTable[i].code,
			    CMYTable[i].length);
		    size = str_out(lpDevice, lpString, count, lpFont,
				   lpDrawMode, lpXform);
		    InsertString(lpDevice, x, y, (short)size);
		    if (i == BLACK)
			break;
		}
	    }
	}
	return size;
}
#endif

#if NO_STR_OUT
#else
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
            myWrite(lpDevice, (LPSTR) pcsb + ((LPPRDFONTINFO)lpFont)->bfont.offset, ((LPPRDFONTINFO)lpFont)->bfont.length);
            StartStyle(lpDevice, lpXform);
        } else
                count = - count;

        breakChar = lpFont->dfBreakChar + lpFont->dfFirstChar;

#if FIXED_PITCH_FONTS_ONLY
#else
        if (lpFont->dfPitchAndFamily & 0x1)
                {
                size = 0;
                widthptr = (short far *) ((LPSTR)lpFont + ((LPPRDFONTINFO)lpFont)->widthtable) - lpFont->dfFirstChar;
                breaksize = widthptr[breakChar];
                for (width = 0; width < count; width++)
#if defined(EPSON_OVERSTRIKE)
                        if (lpString[width] >= TRANS_MIN)
                                size += GetSpecialWidth(lpString[width], widthptr, lpXform->ftItalic);
                        else
#endif
                        size += widthptr[lpString[width]];
                }
        else
#endif
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
#if defined(TOSHP351)
	    /* no need for extra width in PS Font in Toshiba P351
	     */
        		if (lpFont->dfPitchAndFamily & 0x1)
                            char_out(lpDevice, *lpString, breaksize, 0);
			else
#endif
                            char_out(lpDevice, *lpString, breaksize, width);
                    }

                if (!real)
                        lpDrawMode->BreakErr = breakErr;

        } else
                if (real) {
#if defined(EPSON_OVERSTRIKE)
                        Ep_Output_String(lpDevice, lpString, count);
#else
                        myWrite(lpDevice, lpString, count);
#endif
		}

#if SPECIALDEVICECNT
        if (lpDevice->epMode & WRONG_LINESP)
                {
                size -= lpDevice->epXRemain;
                lpDevice->epXRemain = 0;
                }
#endif
        if (real) {
                EndStyle(lpDevice, lpXform);
                myWrite(lpDevice, (LPSTR) pcsb + ((LPPRDFONTINFO)lpFont)->efont.offset, ((LPPRDFONTINFO)lpFont)->efont.length);
                return size;
        } else
                return MAKELONG(lpFont->dfPixHeight, size);
}
#endif


#if defined(EPSON_OVERSTRIKE)
#if NO_GETSPECIALWIDTH
#else
short NEAR PASCAL GetSpecialWidth(c, widthptr, fItalic)
BYTE c;
short far *widthptr;
short fItalic;
{
        if (Translate(c, 1) && Translate(c, 1) < SPACE)
                {
/* width table is for proportional fonts, multiply by two for proportional
   expanded fonts */
                register INT_WIDTH *p, *q;
                short width;

                p = (INT_WIDTH *) Trans_width;
                q = (INT_WIDTH *) Trans_width + TRANS_WIDTH_SIZE; /* number of entries in the table */
                while (p < q)
                        {
                        if (p->charvalue == c)
                                break;
                        p++;
                        }
                /* use the width of the space to tell if it is expanded */
                width = fItalic? p->italic: p->upright;
                return (widthptr[SPACE] > PICA_WIDTH) ? width << 1: width;
                }
        else
                return widthptr[Translate(c, 0)];
}
#endif

#if NO_EP_OUTPUT_STRING
#else
void NEAR PASCAL Ep_Output_String(lpDevice, lpString, count)
LPDEVICE lpDevice;
LPSTR lpString;
short count;
{
        register short i, j;

        j = 0;
        for (i = 0; i < count; i++)
                if (lpString[i] >= TRANS_MIN)
                        {
                        myWrite(lpDevice, &lpString[j], i - j);
                        char_out(lpDevice, lpString[i], 1, 0);
                        j = i + 1;
                        }
        myWrite(lpDevice, &lpString[j], i - j);
}
#endif
#endif

/* width is the extra pixel to insert after the output character */

#if NO_CHAR_OUT
#else
void NEAR PASCAL char_out(lpDevice, c, xcurwidth, width)
LPDEVICE lpDevice;
unsigned char c;
short xcurwidth;
short width;
{
        short i;

        /* this is only temporary need to check if c is a printing
           character  -- put in the character translation table here */
#if defined(EPSON_OVERSTRIKE)
        if (c >= TRANS_MIN)
                if ((i = Translate(c, 1)) < 32)
                        {
                        countryesc.country = i;
                        countryesc.actualchar = Translate(c, 0);
                        myWrite(lpDevice, (LPSTR) &countryesc, sizeof(countryesc));
                        }
                else
                        {
                        myWrite(lpDevice, (LPSTR) &i, 1);
                        myWrite(lpDevice, (LPSTR) "\010", 1);
                        i = Translate(c, 0);
                        myWrite(lpDevice, (LPSTR) &i, 1);
                        }
        else
#endif
                myWrite(lpDevice, (LPSTR) &c, 1);


        for (; width >= xcurwidth; width -= xcurwidth) 
                myWrite(lpDevice, (LPSTR) EP_BLANK);

        /* go into graphics mode to do microspace justification */

        if (width > 0)
                {
#if SPECIALDEVICECNT
                /* some ibmcomaptibles always move by 2 pixels */
                if ((lpDevice->epMode & WRONG_LINESP) && width % 2)
                        {
                        width += lpDevice->epXRemain;
                        lpDevice->epXRemain = width % 2;
                        /* make width even */
                        width = (width >> 1) << 1;
                        }
                if (width)
#endif
                        {
#if defined(NECP2)
                        graph.cnt = width;
                	myWrite(lpDevice, (LPSTR)&graph, 4);
                	myWrite(lpDevice, (LPSTR)"\0", 1);
#elif defined(CITOH)

			itoa4(width,(LPSTR) reset.count_string);
			myWrite(lpDevice,(LPSTR)&reset,8); 
                        for (; width; --width)
                                myWrite(lpDevice, (LPSTR)"\0", 1);
#else
                        reset.cnt = width;
                        reset.code = XMS;
                        myWrite(lpDevice, (LPSTR)&reset, 4);
                        for (; width; --width)
                                myWrite(lpDevice, (LPSTR)"\0", 1);
#endif
                        }
                }
}
#endif


#if NO_DRAFTSTRBLT
#else
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
        short charmode;
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
                /* clip to the page */
                if (x >= lpDevice->epPageWidth ||
		    (lpDevice->epCurx += (charcnt * lpDevice->epXcurwidth)) >
			    lpDevice->epPageWidth)
                        break;

                if (x >= 0)
#if defined(EPSON_OVERSTRIKE)
                        Ep_Output_String(lpDevice, lpString2, charcnt);
#else
                        myWrite(lpDevice, lpString2, charcnt);
#endif

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
#endif


#if NO_FINDWORD
#else
/* scans lpString to find the beginning of the next word -
    look for a space, then a non-space
   return the offset from lpString */

short NEAR PASCAL findword(lpString, cnt)
register LPSTR lpString;
short cnt;
{
        register short pos = 0;

        while (pos < cnt && *lpString != SPACE)
                {
                pos++;
                lpString++;
                }
        while (pos < cnt && *lpString++ == SPACE)
                pos++;
        return pos;
}
#endif

#if NO_SETMODE
#else
/*      C 4.0 complains about redefinitions
SetMode(lpDevice, lpFont)
*/
void NEAR PASCAL SetMode(lpDevice, lpFont)
LPDEVICE lpDevice;
LPFONTINFO lpFont;
{
        /* variable pitch fonts have dfPixWidth == 0, then we will
           use a smaller font than what it's avewidth says */
        short width;

        if (!(width = lpFont->dfPixWidth))
                width = lpFont->dfAvgWidth - VAR_PITCH_KLUDGE;
#if DRAFTMODE_HAS_ELITE_PRINT
        if (width < ELITE_WIDTH)
#else
        if (width < PICA_WIDTH)
#endif
                {
                if (lpDevice->epXcurwidth != COMP_MODE_WIDTH)
                        {
                        myWrite(lpDevice, ESCEXP(escapecode.compress_on));
                        lpDevice->epXcurwidth = COMP_MODE_WIDTH;
                        }
                }
#if DRAFTMODE_HAS_ELITE_PRINT
        else if (width < PICA_WIDTH)
                {
                if (lpDevice->epXcurwidth != ELITE_WIDTH)
                        {
                        myWrite(lpDevice, ESCEXP(escapecode.elite_on));
                        lpDevice->epXcurwidth = ELITE_WIDTH;
                        }
                }
#endif
        else
                {
                if (lpDevice->epXcurwidth != PICA_WIDTH)
                        {
                        myWrite(lpDevice, ESCEXP(escapecode.pica_on));
                        lpDevice->epXcurwidth = PICA_WIDTH;
                        }
                }
}
#endif
