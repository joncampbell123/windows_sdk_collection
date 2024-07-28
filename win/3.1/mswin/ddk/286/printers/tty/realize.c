/*/  REALIZE.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	02 feb 89	peterbe		Added GetCharWidth(), but it's in #ifdef
//					since our dev.caps say RC_NONE!
//					Also, #ifdef out CuantosImprime().
//					Added some tracing
//					(DBG) calls.
//	20 nov 89	peterbe		infoToCode() changed to match only with
//					facenames for which escapes are defined
//	13 nov 89	peterbe		In GetFaceName(), return the existance
//					of a face name only if bHasFont[] is
//					set for this face (this is set up
//					when the TTY.DAT file is read or
//					updated).  In EnumDFonts(), only
//					callback if bHasFont[i] is true.
//					In chRealizeObject(), only realize
//					fonts with TRUE in bHasFont[].
//	30 oct 89	peterbe		Update from Fralc. see
//					.. = pnf & 0x07; around line 285
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"
#include "fonts.h"

#ifdef DEBUG
#include "debug.h"
#define DBGmsg(msg) DBMSG(msg)
#else
#define DBGmsg(msg) /* zip */
#endif

#define NFONTS 6


unsigned NEAR PASCAL Translate(
    BYTE    c,
    BYTE    expanded[],
    short   maxlen,
    BYTE    defa_c )
{
    int		len,
		i,
		pos;
    static BYTE tString[ESCAPELEN+1];

    // just return if it's a normal ASCII character
    if (c < 128)
	return c;

    // if ANSISTART = 128 we'll never see this. Otherwise,
    // we return the default character value
    if(c < ANSISTART)
	return defa_c & 0xFF;

    // index to piPrinterTable[][0]:
    pos = c - ANSISTART;

    // count nonzero characters in escape:
    for(len = 0; Printer.piPrinterTable[pos][len] ; len++)
	{
	if(len >= ESCAPELEN)
	    break;

	if(Printer.piPrinterTable[pos][len] == (unsigned char)'\xFF')
	    tString[len] = '\0';
	else
	    tString[len] = Printer.piPrinterTable[pos][len];
	}

    if(!len)
	return defa_c & 0xFF;

    if(len == 1)
	return tString[0] & (unsigned char)0xFF;

    len = len > maxlen ? maxlen : len;

    for(i=0;i<len; i++)
	expanded[i] = tString[i];

    return len | ((STRING_TRANS)<<8);
}


short FAR PASCAL chRealizeObject(lpdv, lpInObj, lpOutObj, lpTextXForm)
LPDV   lpdv;
LPLOGFONT lpInObj;
LPFONTINFO lpOutObj;
LPTEXTXFORM lpTextXForm;
{
    short	i;
    HANDLE	hResInfo, hResData;
    register	FONTTAB *font;
    unsigned	bestvalue, value, bestfont;
    short	tmp;
    FONTTAB	target;

    if (!lpOutObj)
	return sizeof(FONTINFO) + 16;

    // translate what they want according to our format FONTTAB
    /* transladar lo que ellos quieren dentro de nuestro formato FONTTAB */

    InfoToCode(HP_LOGFONT, lpInObj, (FONTTAB far *)&target);

    // go through the FontTable and chose the one which has the lowest
    // penalty value.

    bestvalue = 0xffff;
    bestfont = 2;		// in case we fail somehow

    for (i = 0, font = FontTable; i < NFONTS; i++, font++)
	{	  // search for best fit
	if (!bHasFont[i])
	    continue;

	/*width */
	if (target.width)
	   tmp = target.width - font->width;
	else
	   tmp = PICA_WIDTH - font->width;

	value = abs(tmp);

	// facename is everything
	if (target.facename == font->facename)
	    {
	    bestfont = i;
	    break;
	    }

	if (value < bestvalue)
	    {
	    bestvalue = value;
	    bestfont  = i;
	    }
	}

    Copy((LPSTR)lpOutObj, (LPSTR)&gFontInfo, sizeof(FONTINFO));

    // fix up the width;
    lpOutObj->dfAvgWidth =
    lpOutObj->dfMaxWidth =
    lpOutObj->dfPixWidth = FontTable[bestfont].width;
    lpOutObj->dfWeight = target.weight * 100;
    lpOutObj->dfUnderline = lpInObj->lfUnderline;

    lpOutObj->dfDevice = 0;
    lpOutObj->dfFace = sizeof(FONTINFO);
    lstrcpy((LPSTR)lpOutObj + sizeof(FONTINFO),
		facenames[FontTable[bestfont].facename]);

    CodeToInfo(&FontTable[bestfont], HP_TEXTXFORM, (LPSTR)lpTextXForm);

    if (lpTextXForm->ftUnderline = lpInObj->lfUnderline)
	lpTextXForm->ftAccelerator |= TC_UA_ABLE;

    if (lpTextXForm->ftStrikeOut = lpInObj->lfStrikeOut)
	lpTextXForm->ftAccelerator |= TC_SO_ABLE;

    lpTextXForm->ftWeight = target.weight * 100;

    // return success
    return TRUE;
}	// chRealizeObject()

// Enumerate device fonts

FAR PASCAL EnumDFonts(
    LPDV lpdv,
    LPSTR   lpFaceName,
    short   (FAR PASCAL *lpCallbackFunc)(LPLOGFONT, LPTEXTMETRIC, short, LPSTR),
    LPSTR   lpClientData)
{
    LOGFONT LogFont;
    TEXTMETRIC TextMetric;
    short status, i, face;
    FONTTAB *font;

    if (lpdv->iType == DEV_MEMORY)
	    return 1;

    // if "facename" is non-zero just enumerate the fonts with this
    // facename.
    /* si el "facename" es non-zero solo enumerar  el font con
       el "facename" encontrado */

    if (lpFaceName && *lpFaceName){
	/* no se pudo encontrar el "face name" */
	if ((face = GetFaceName(lpFaceName)) == (-1)){
		return 1;
	}
	font = &FontTable[face];
	goto callback;
    }
    font = FontTable;

    for (i = 0; i < NFONTS; i++, font++){
	if (!bHasFont[i])	// this font may not be supported on this
	    continue;		// printer.
    callback:
	CodeToInfo(font, HP_LOGFONT, (LPSTR) &LogFont);
	CodeToInfo(font, HP_TEXTMETRIC, (LPSTR) &TextMetric);

	status = (*lpCallbackFunc)(&LogFont,
			    &TextMetric,
			    DEVICE_FONTTYPE|RASTER_FONTTYPE,
			    lpClientData);

	if (!status || lpFaceName)
		break;
    }
    return status;

}	// enumDFonts()

// If the dev. caps. specified RC_GDI20_OUTPUT, we'd need this, but
// we don't.  Include in #ifdef for future reference!


FAR PASCAL devGetCharWidth(
    LPDV    lpdv,		//
    short far *	lpBuffer,		// fill this buffer with widths
    WORD	firstChar,		// range of chars to provide widths
    WORD	lastChar,		//   for.
    LPFONTINFO	lpFont,			// ==> font structure
    LPDRAWMODE	lpDrawMode,		// ignore in this driver
    LPTEXTXFORM	lpXform)		// ignore in this driver
	
{
    int charwidth;

    // we must have an output buffer to write widths into!
    if (!lpBuffer)
	{
	DBGmsg(("TTY's GetCharWidth() = FALSE.\n"));
	return FALSE;
	}

    // as in DraftStrBlt() [ ExtTextOut() ] .. we get the width ..
    // (SelectWidth is in CHPHYS.C)
    // SelectWidth forces the character width to be the width of the
    // widest TTY font less than or equal to lpFont->dfPixWidth.
    // SelectWidth pondra el ancho del caracter a alguno que
    // sea menor o igual a lpFont->dfPixWidth.

//    charwidth = SelectWidth(lpFont->dfPixWidth);
    charwidth = lpFont->dfPixWidth;

    // fill the buffer with the fixed width.
    for ( ; firstChar <= lastChar; ++firstChar, ++lpBuffer)
	*lpBuffer = charwidth;

    DBGmsg(("TTY's GetCharWidth() = TRUE.\n"));

    return TRUE;

}	// GetCharWidth()


// =============== Local functions =============================

void NEAR PASCAL CodeToInfo(
    FONTTAB *font,
    short style,
    LPSTR info)
{
    short  pnf, weight;

    pnf  =  1 | (font->family << 4);

    weight = (font->weight) * 100;

    switch(style){
	case HP_LOGFONT:
	    ((LPLOGFONT)info)->lfHeight = font->height;
	    ((LPLOGFONT)info)->lfWidth =  font->width;
	    ((LPLOGFONT)info)->lfEscapement = 0;
	    ((LPLOGFONT)info)->lfOrientation = 0;
	    ((LPLOGFONT)info)->lfWeight = weight;
	    ((LPLOGFONT)info)->lfItalic = font->italic;
	    ((LPLOGFONT)info)->lfUnderline = 0;
	    ((LPLOGFONT)info)->lfStrikeOut = 0;
	    ((LPLOGFONT)info)->lfCharSet = 0;
	    ((LPLOGFONT)info)->lfOutPrecision = OUT_CHARACTER_PRECIS;
	    ((LPLOGFONT)info)->lfClipPrecision = CLIP_CHARACTER_PRECIS;
	    ((LPLOGFONT)info)->lfQuality = DEFAULT_QUALITY;
	    ((LPLOGFONT)info)->lfPitchAndFamily = pnf;
	    Copy((LPSTR)((LPLOGFONT)info)->lfFaceName,
		(LPSTR)facenames[font->facename], LF_FACESIZE);
	    break;

	case HP_TEXTXFORM:
	    ((LPTEXTXFORM)info)->ftHeight = font->height;
	    ((LPTEXTXFORM)info)->ftWidth = font->width;
	    ((LPTEXTXFORM)info)->ftEscapement = 0;
	    ((LPTEXTXFORM)info)->ftOrientation = 0;
	    ((LPTEXTXFORM)info)->ftWeight = weight;
	    ((LPTEXTXFORM)info)->ftItalic = 0;
	    ((LPTEXTXFORM)info)->ftOutPrecision = OUT_CHARACTER_PRECIS;
	    ((LPTEXTXFORM)info)->ftClipPrecision = CLIP_CHARACTER_PRECIS;
	    ((LPTEXTXFORM)info)->ftAccelerator = TC_OP_CHARACTER;
	    ((LPTEXTXFORM)info)->ftOverhang = 0;
	    break;

	case HP_TEXTMETRIC:
	    ((LPTEXTMETRIC)info)->tmHeight = font->height;
#ifdef TMAscent
	    ((LPTEXTMETRIC)info)->tmAscent = TMAscent;
	    ((LPTEXTMETRIC)info)->tmDescent = TMDescent;
#else
	    ((LPTEXTMETRIC)info)->tmAscent = font->ascent;
	    ((LPTEXTMETRIC)info)->tmDescent = font->height - font->ascent;
#endif
	    ((LPTEXTMETRIC)info)->tmExternalLeading = TMExternalLeading;
	    ((LPTEXTMETRIC)info)->tmInternalLeading = TMInternalLeading;
	    ((LPTEXTMETRIC)info)->tmAveCharWidth = font->width;
	    ((LPTEXTMETRIC)info)->tmMaxCharWidth = font->width;
	    ((LPTEXTMETRIC)info)->tmWeight = weight;
	    ((LPTEXTMETRIC)info)->tmItalic = font->italic;
	    ((LPTEXTMETRIC)info)->tmUnderlined = 0;
	    ((LPTEXTMETRIC)info)->tmStruckOut = 0;
	    ((LPTEXTMETRIC)info)->tmFirstChar = TMFirstChar;
	    ((LPTEXTMETRIC)info)->tmLastChar = TMLastChar;
	    ((LPTEXTMETRIC)info)->tmDefaultChar = TMDefaultChar;
	    ((LPTEXTMETRIC)info)->tmBreakChar = TMBreakChar;
	    ((LPTEXTMETRIC)info)->tmPitchAndFamily = pnf & 0x07; // 30 oct 89
	    ((LPTEXTMETRIC)info)->tmCharSet = 0;
	    ((LPTEXTMETRIC)info)->tmOverhang = 0;
	    ((LPTEXTMETRIC)info)->tmDigitizedAspectX = VDPI;
	    ((LPTEXTMETRIC)info)->tmDigitizedAspectY = HDPI;
    }
}

void NEAR PASCAL InfoToCode(
      short style,
      LPLOGFONT info,
      FONTTAB far *fonttab)
{
    if (!(fonttab->family = (info->lfPitchAndFamily & 0xf0) >> 4))
	fonttab->family = DEFAULT_FAMILY;

    if (info->lfHeight > 0)
	fonttab->height = info->lfHeight + DIFF_CELL_CHAR;
    else
	fonttab->height = -info->lfHeight;

    fonttab->width = info->lfWidth;

    if (!(fonttab->weight = info->lfWeight / 100))
	fonttab->weight = DEFAULT_WEIGHT;

    fonttab->italic = info->lfItalic;
    fonttab->facename = GetFaceName((LPSTR)info->lfFaceName);
}

short FAR PASCAL GetFaceName(LPSTR name)
{
    short i;

    for (i = 0; i < NFACES; i++)
	{
	if (!lstrcmp(name, facenames[i]))
	    {
	    if (bHasFont[i])
		return i;
	    }
	}
    return -1;
}

#if 0
// chStrBlt() is now called from StrBlt() (instead of ExtTextOut())
// to determine the width of strings.

long FAR PASCAL chStrBlt(
		       LPDV lpdv,
		       short   x	  ,
		       short   y	  ,
		       LPSTR   lpString	  ,
		       short   count	  ,
		       LPFONTINFO lpFont     ,
		       LPDRAWMODE lpDrawMode , // incluye modo "background" y
						// bkColor
		       LPTEXTXFORM lpXform   ,
		       LPRECT  lpClipRect )
{
    int	    width = lpFont->dfPixWidth;
    int	    charcnt;

    if (count < 0){
	count = -count;

	if(lpClipRect){
	    if(x < lpClipRect->left){ /* a la izquierda del rectangulo */
		charcnt = ( (x+count * width)-lpClipRect->left ) / width;
		if(charcnt <= 0 )
		    return 0;
		x += (count - charcnt ) * width;
		lpString += count-charcnt;
		count = charcnt;
	    }

	    if( (x + count * width) > lpClipRect->right)
	    { /* a la der. del rectangulo */
		count = ( lpClipRect->right-x) / width;
		if( count <= 0 ) /* si esta todo el string a la derecha */
		    return 0;
	    }
	}

	if (width < ELITE_WIDTH){
	    charcnt = count * COMP_MODE_WIDTH + 2 * count / 10;
	} else if (width < PICA_WIDTH)
	    charcnt = count * ELITE_WIDTH;
	else if (width < (int)COMP_MODE_WIDTH2)
	    charcnt = count * PICA_WIDTH;
	else if (width < ELITE_WIDTH2){
	    charcnt = count * COMP_MODE_WIDTH2 + 2 * count / 5;
	} else if (width < PICA_WIDTH2)
	    charcnt = count * ELITE_WIDTH2;
	else
	    charcnt = count * PICA_WIDTH2;
	return charcnt;
    }

    /*
       nunca permito que entre aqui con valores de count positivos
    */

    return OEM_FAILED;

}	// chStrBlt()

#endif


LPSTR NEAR PASCAL CheckString(src, cnt, lpFont)
LPSTR	    src;
short	    cnt;
LPFONTINFO  lpFont;
{
	register LPSTR	p;
	short		i;
	static LPSTR	lpString;
	static HANDLE	hString;
	BYTE   LastChar,DefaultChar;

	if (!lpFont->dfDevice)
		{
		/* raster font en modo draft */
		LastChar = TMLastChar;
		DefaultChar = TMDefaultChar;
		}
	else
		{
		/* hardware font del device */
		LastChar = lpFont->dfLastChar;
		DefaultChar = lpFont->dfDefaultChar + lpFont->dfFirstChar;
		}

	for (p = src, i = 0; i < cnt; i++, p++)
		if (*p < lpFont->dfFirstChar || *p >= (BYTE) 0x7f)
				break;

	/* el string solo contiene caracteres imprimibles */
	if (i == cnt)
		return src;

	/* no se puede modificar el string mismo directamente */

    if (!(hString = GlobalAlloc(GMEM_FIXED, (long)cnt)))
		return (LPSTR) OEM_FAILED;

    lpString = (LPSTR) GlobalLock(hString);

    for (p = src, i = 0; i < cnt;i++, p++)
	if ((lpString[i] = *p) <
		lpFont->dfFirstChar|| (*p >= (BYTE) 0x7f && *p < TRANS_MIN))
	lpString[i] = DefaultChar;

    GlobalUnlock(hString);

    return lpString;
}


// returns length (byte count) of string less trailing blanks

#ifdef NoOneCallsThis
CuantosImprime(LPDV lpdv,LPSTR	  lpString)
{
	int len;

	len = lstrlen(lpString);
	while(*(lpString+len-1)==' ')
	    len--;
	return len;
}
#endif
