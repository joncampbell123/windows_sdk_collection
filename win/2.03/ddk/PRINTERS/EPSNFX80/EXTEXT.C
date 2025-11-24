#include <generic.h>


int  FAR PASCAL ExtWidths(lpDevice, chFirst, chLast, lpOut, lpFont, lpXform)
LPDEVICE    lpDevice;
BYTE	    chFirst;
BYTE	    chLast;
short far * lpOut;
LPFONTINFO  lpFont;
LPTEXTXFORM lpXform;
{
    short far *    lpWidthTable;
    register BYTE  ch;
    register short width;
    short          defaultWidth, i;

    if (lpOut == NULL) {
#if defined(DEBUG)
	FatalExit(0x500);
#endif
        return 0;  /* if output buffer is null skip     */
    }

#if FIXED_PITCH_FONTS_ONLY
#else
    if (lpFont->dfPitchAndFamily & 0x1) {

    	lpWidthTable = (short far *) ((LPSTR)lpFont + ((LPPRDFONTINFO)lpFont)->widthtable) - lpFont->dfFirstChar;
    	defaultWidth = lpWidthTable[lpFont->dfDefaultChar + lpFont->dfFirstChar];

        for (ch = chFirst, i = 0; i < chLast - chFirst + 1; ch++, i++) {

            if (ch < lpFont->dfFirstChar
                    || (ch >= 0x7f && ch < TRANS_MIN))
                width = defaultWidth;
            else if (ch >= TRANS_MIN)
#if defined(EPSON_OVERSTRIKE)
                width = GetSpecialWidth(ch, lpWidthTable, lpXform->ftItalic);
#else
                width = lpWidthTable[Translate(ch,0)];
#endif
            else
                width = lpWidthTable[ch];;

            *lpOut++ = width;
        }

    } else 
#endif
    {

    	width = lpFont->dfPixWidth;
        for ( i = 0; i < chLast - chFirst + 1 ; i++)
            *lpOut++ = width;

    }

    return lpFont->dfPixHeight;
}

DWORD FAR PASCAL ExtStrOut(lpDevice, x, y, lpClipRect, lpStr, count,  lpFont,  lpXform, lpDrawMode, lpWidths)
LPDEVICE    lpDevice;
short       x;
short       y;
LPRECT      lpClipRect;
LPSTR       lpStr;
short       count;
LPFONTINFO  lpFont;
LPTEXTXFORM lpXform;
LPDRAWMODE  lpDrawMode;
short far   *lpWidths;
{

    BYTE        ch;
    short       strlen, width, defaultWidth, xStart;
    short far * lpWidthTable;
    int         vPitch;
    short       TBreakExtra, TCharExtra;
/*
 *  DWORD 	strwidth;
 */

    if (count <= 0)
        return 0L;

/*
 *  strwidth = StrBlt(lpDevice, 0, 0, (LPRECT)0, (LPSTR)&lpStr[count - 1] , -1, lpFont, lpDrawMode, lpXform) + lpWidths[count - 1] - lpWidths[0];
 */

    TBreakExtra =  lpDrawMode->BreakExtra;
    TCharExtra  =  lpDrawMode->CharExtra;

    lpDrawMode->BreakExtra = 0;
    lpDrawMode->CharExtra  = 0;

    if (lpDevice->epMode & DRAFTFLAG) {
	/* handle raster font */

    	while (count) {

    	    strlen = findword(lpStr, count);
            DraftStrBlt(lpDevice, x, y, lpStr, strlen, lpFont, lpDrawMode, lpXform, lpClipRect);
	    count -= strlen;
	    lpStr += strlen;
	    for ( ; strlen > 0; strlen--)
		x += *lpWidths++;
        }
	goto EXTDONE;
    }

#if FIXED_PITCH_FONTS_ONLY
#else
    if (vPitch = (lpFont->dfPitchAndFamily & 0x1)) {
	lpWidthTable = (short far *) ((LPSTR)lpFont + ((LPPRDFONTINFO)lpFont)->widthtable) - lpFont->dfFirstChar;

	defaultWidth = lpWidthTable[lpFont->dfDefaultChar + lpFont->dfFirstChar];
    }
#endif

    width = lpFont->dfPixWidth;

    while (count) {

    	for (xStart = x, strlen = 0; count > 0; ) {

		ch = lpStr[strlen];

#if FIXED_PITCH_FONTS_ONLY
#else
      		if (vPitch) {
            	    	if (ch < lpFont->dfFirstChar
                                || (ch >= 0x7f && ch < TRANS_MIN))
                	    width = defaultWidth;
            		else if (ch >= TRANS_MIN)
#if defined(EPSON_OVERSTRIKE)
                	    width = GetSpecialWidth(ch, lpWidthTable, lpXform->ftItalic);
#else
                	    width = lpWidthTable[Translate(ch,0)];
#endif
            		else
                	    width = lpWidthTable[ch];;
       	 	}
#endif

        	strlen++;
		count--;
		x += *lpWidths;

        	if (*lpWidths++ != width)
			break;

       	}

        StrBlt(lpDevice, xStart, y, lpClipRect, lpStr, strlen, lpFont, lpDrawMode, lpXform);
	lpStr += strlen;
    }

EXTDONE:
    lpDrawMode->BreakExtra = TBreakExtra ;
    lpDrawMode->CharExtra  = TCharExtra  ;

    return TRUE;
/*
 * return strwidth;
 */
}
