#include "generic.h"
#include "fonts.h"

#define VERTICAL_CLIPPING 0 /* Don't change - see note below */

/* Mitchl [10/22/87] -- Can't do vertical clipping because GDI passes
intersection of true clipping rectangle with band region to the
StrBlt stub.  Since it is not possible to partially clip a hardware
font, there is no way for the driver to know if a textout would have
fit in the desired clipping rectangle until it has gathered a
reciprocal text out in the next band. */

BYTE NEAR PASCAL Translate(c, pos)
BYTE c;
short pos;
{
        if (c >= TRANS_MIN)
                {
                c = (c - TRANS_MIN) * 2;
                return Trans[c + pos];
                }
        return NULL;
}

short NEAR PASCAL StartStyle(lpDevice, lpXform)
LPDEVICE lpDevice;
register LPTEXTXFORM lpXform;
{
#if defined(IBMGRX)
        /* cannot use hardware double strike on some ibmgrx compatibles */
        if (lpXform->ftWeight >= FW_SEMIBOLD)
                if ((lpDevice->epMode & DRAFTFLAG) || !(lpDevice->epMode & WRONG_LINESP))
                        myWrite(lpDevice, ESCEXP(escapecode.bold_on));
#else
        if (lpXform->ftWeight >= FW_SEMIBOLD)
                myWrite(lpDevice, ESCEXP(escapecode.bold_on));
#endif

        /* simulate underline and strike out in graphics */

        if (lpDevice->epMode & DRAFTFLAG)   /* only do it in draftmode */
                {
                if (lpXform->ftUnderline)
                        myWrite(lpDevice, ESCEXP(escapecode.underl_on));
                }

        if (lpXform->ftItalic)
                myWrite(lpDevice, ESCEXP(escapecode.italic_on));
}

short NEAR PASCAL EndStyle(lpDevice, lpXform)
LPDEVICE lpDevice;
register LPTEXTXFORM lpXform;
{
#if defined(IBMGRX)
        /* cannot use hardware double strike on some ibmgrx compatibles */
        if (lpXform->ftWeight >= FW_SEMIBOLD)
                if ((lpDevice->epMode & DRAFTFLAG) || !(lpDevice->epMode & WRONG_LINESP))
                        myWrite(lpDevice, ESCEXP(escapecode.bold_off));
#else
        if (lpXform->ftWeight > FW_SEMIBOLD)
                myWrite(lpDevice, ESCEXP(escapecode.bold_off));
#endif

        if (lpDevice->epMode & DRAFTFLAG)
                {
                if (lpXform->ftUnderline)
                        myWrite(lpDevice, ESCEXP(escapecode.underl_off));
                }

        /* no longer substitute italic with underline for devices which */
        /* do not have italic 10/18/85 */

        if (lpXform->ftItalic)
                myWrite(lpDevice, ESCEXP(escapecode.italic_off));
}

short FAR PASCAL chRealizeObject(lpDevice, lpInObj, lpOutObj, lpTextXForm)
LPDEVICE   lpDevice;
LPLOGFONT lpInObj;
LPFONTINFO lpOutObj;
LPTEXTXFORM lpTextXForm;
{
        short i;
        HANDLE hMd, hResInfo, hResData;
        FONTTAB target;
        register FONTTAB *font;
        unsigned bestvalue, value, bestfont;
        short tmp, nfonts;

        /* translate what they want into our FONTTAB format */
        InfoToCode(HP_LOGFONT, lpInObj, (FONTTAB far *)&target);

        /* go throught the FontTable and pick out the one with the
           lowest penality value */

        nfonts = NFONTS + PSFONTS;

        bestvalue = 0xffff;
        for (i = 0, font = FontTable; i < nfonts; i++, font++)
                {
                /* orientation - nop */
                value = 0;

                /* pitch */
                if (target.pitch != font->pitch)
                        value += 1 << PITCH_WEIGHT;

                /* family */
                if (target.family != font->family)
                        value += 1 << FAMILY_WEIGHT;

                /* facename */
                if (target.facename != font->facename)
                        value += 1 << FACENAME_WEIGHT;

                /* height */
                if (target.height)
                        if (tmp = target.height - font->height)
                                if (tmp > 0)
                                        value += tmp << HEIGHT_WEIGHT;
                                else
                                        value += (-tmp) << LARGE_HEIGHT_WEIGHT;

                /* width */
                if (target.width)
                        if (tmp = target.width - font->width)
                                value += abs(tmp) << WIDTH_WEIGHT;

                /* italic */
                value += (target.italic ^ font->italic) << ITALIC_WEIGHT;

#if 0
                /* all our hardware fonts have the same weight */
                /* weight */
                if (tmp = target.weight - font->weight)
                        value += abs(tmp) << WEIGHT_WEIGHT;
#endif

                if (value < bestvalue)
                        {
                        bestvalue = value;
                        bestfont  = i;
                        }
                };

        hMd = GetModuleHandle(MODULENAME);

        /* the resource compiler is 1 based */
        hResInfo = FindResource(hMd, (LPSTR)(long)bestfont + 1, (LPSTR)(long)MYFONT);

        i = SizeofResource(hMd, hResInfo);

        if (!lpOutObj)
        	return i;

        /* something went wrong */
        if (!(hResData = LoadResource(hMd, hResInfo)))
                return 0;

        Copy((LPSTR)lpOutObj, (LPSTR)LockResource(hResData), i);

        lpOutObj->dfType = PF_BITS_IS_ADDRESS | PF_DEVICE_REALIZED;

        CodeToInfo(&FontTable[bestfont], HP_TEXTXFORM, (LPSTR)lpTextXForm);
        if (lpTextXForm->ftUnderline = lpInObj->lfUnderline)
                lpTextXForm->ftAccelerator |= TC_UA_ABLE;
        if (lpTextXForm->ftStrikeOut = lpInObj->lfStrikeOut)
                lpTextXForm->ftAccelerator |= TC_SO_ABLE;

#if defined(TOSHP351)
        if (escapecode.italic_on.length && lpOutObj->dfPixWidth)
		/* Toshiba P351 cannot do italic in PS fonts */
#else
        if (escapecode.italic_on.length)
                /* device is italic able */
#endif
                lpTextXForm->ftItalic = lpInObj->lfItalic;

        lpTextXForm->ftWeight = target.weight * 100;

        GlobalUnlock(hResData);
        FreeResource(hResData);

        return 1;
}


FAR PASCAL EnumDFonts(lpDevice, lpFaceName, lpCallbackFunc, lpClientData)
LPDEVICE lpDevice;
LPSTR   lpFaceName;
FARPROC lpCallbackFunc;
long    lpClientData;
{
        LOGFONT LogFont;
        TEXTMETRIC TextMetric;
        short status, i, face;
        FONTTAB *font;
        short nfonts;

        if (lpDevice->epType == DEV_LAND)
                return 1;

        /* if facename is non-zero only enumerate the font with
           the matching facename */

        if (lpFaceName && *lpFaceName)
                {
                /* cannot find the face name */
                if ((face = GetFaceName(lpFaceName)) == (-1))
                        return 1;
                font = &FontTable[face];
                goto callback;
                }
        font = FontTable;

        nfonts = NFONTS + PSFONTS;

        for (i = 0; i < nfonts; i++, font++)
                {
        callback:
                CodeToInfo(font, HP_LOGFONT, (LPSTR) &LogFont);
                CodeToInfo(font, HP_TEXTMETRIC, (LPSTR) &TextMetric);
                status = (*lpCallbackFunc)((LPSTR)&LogFont, (LPSTR)&TextMetric,DEVICE_FONTTYPE|RASTER_FONTTYPE,lpClientData);
                if (!status || lpFaceName)
                        break;
                }
        return status;
}



void NEAR PASCAL CodeToInfo(font, style, info)
FONTTAB *font;
short style;
LPSTR info;
{
        short  pnf, weight, lpnf;

        /* logical pitch and family */
        pnf  =  font->pitch | (font->family << 4);
        lpnf =  (font->pitch + 1) | (font->family << 4);
        weight = (font->weight) * 100;

        switch(style)
        {
        case HP_LOGFONT:
                ((LPLOGFONT)info)->lfHeight = font->height;
                ((LPLOGFONT)info)->lfWidth =  font->width;
                ((LPLOGFONT)info)->lfEscapement = 0;
                ((LPLOGFONT)info)->lfOrientation = 0;
                ((LPLOGFONT)info)->lfWeight = weight;
                ((LPLOGFONT)info)->lfItalic = font->italic;
                ((LPLOGFONT)info)->lfUnderline = 0;
                ((LPLOGFONT)info)->lfStrikeOut = 0;
                ((LPLOGFONT)info)->lfCharSet = ANSI_CHARSET;
                ((LPLOGFONT)info)->lfOutPrecision = OUT_CHARACTER_PRECIS;
                ((LPLOGFONT)info)->lfClipPrecision = CLIP_CHARACTER_PRECIS;
                ((LPLOGFONT)info)->lfQuality = DEFAULT_QUALITY;
                ((LPLOGFONT)info)->lfPitchAndFamily = lpnf;
                Copy((LPSTR)((LPLOGFONT)info)->lfFaceName, (LPSTR)facenames[font->facename], LF_FACESIZE);
                break;

        case HP_TEXTXFORM:

                ((LPTEXTXFORM)info)->ftHeight = font->height;
                ((LPTEXTXFORM)info)->ftWidth = font->width;
                ((LPTEXTXFORM)info)->ftEscapement = 0;
                ((LPTEXTXFORM)info)->ftOrientation = 0;
                ((LPTEXTXFORM)info)->ftWeight = weight;
                ((LPTEXTXFORM)info)->ftItalic = font->italic;
                ((LPTEXTXFORM)info)->ftOutPrecision = OUT_CHARACTER_PRECIS;
                ((LPTEXTXFORM)info)->ftClipPrecision = CLIP_CHARACTER_PRECIS;
                ((LPTEXTXFORM)info)->ftAccelerator = TC_OP_CHARACTER;
                ((LPTEXTXFORM)info)->ftOverhang = 0;
                break;

        case HP_TEXTMETRIC:
#if 0
                Copy((LPSTR)info, (LPSTR)&TMModel, sizeof(TMModel));
#endif
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
                ((LPTEXTMETRIC)info)->tmPitchAndFamily = pnf;
                ((LPTEXTMETRIC)info)->tmCharSet = ANSI_CHARSET;
                ((LPTEXTMETRIC)info)->tmOverhang = 0;
                ((LPTEXTMETRIC)info)->tmDigitizedAspectX = VDPI;
                ((LPTEXTMETRIC)info)->tmDigitizedAspectY = HDPI;
        }
}

void NEAR PASCAL InfoToCode(style, info, fonttab)
short style;
LPLOGFONT info;
FONTTAB far *fonttab;
{
        fonttab->pitch = (info->lfPitchAndFamily & 0xf) == VARIABLE_PITCH? 1: 0;

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
        if ((fonttab->facename = GetFaceName((LPSTR)info->lfFaceName)) == (-1))
                    fonttab->facename = DEFAULT_FACENAME;
}

short FAR PASCAL GetFaceName(name)
LPSTR name;
{
        short i;

        for (i = 0; i < NFACES; i++)
                if (!lstrcmp(name, facenames[i]))
                        return i;
        return -1;
}

long FAR PASCAL chStrBlt(lpDevice, x, y, lpString, count, lpFont, lpDrawMode, lpXform, lpClipRect)
LPDEVICE lpDevice;
short   x          ;
short   y          ;
LPSTR   lpString   ;
short   count      ;
LPFONTINFO lpFont     ;
LPDRAWMODE lpDrawMode ;     /* includes background mode and bkColor */
LPTEXTXFORM lpXform   ;
LPRECT  lpClipRect ;
{
        long size;
	short left_edge, right_edge;
#if VERTICAL_CLIPPING	
	short top_edge, bottom_edge;
#endif
        short oldDDA, newDDA, i, fFree = 0;
        LPSTR lpString2;

        /* use the epSpool area to store justified output */

        i = count > 0? count: -count;

        if ((lpString2 = CheckString(lpString, i, lpFont)) == (LPSTR) OEM_FAILED)
                return OEM_FAILED;

        if (lpString2 != lpString)
                fFree = TRUE;
	
        if (count < 0)
                {
                size = str_out(lpDevice, lpString2, count, lpFont, lpDrawMode, lpXform);
                if (fFree)
                        GlobalFree(HWORD((long)lpString2));
                return size;
                }

        oldDDA = lpDrawMode->BreakErr;

	
	if (lpClipRect)		/* user gave us a clipping rectangle */
	{
/* can assume clipping rectangle is correctly situated in device units */
		
		left_edge = lpClipRect->left;	
		left_edge = MAX(0,left_edge);  
		
		right_edge = lpClipRect->right;
		right_edge = MIN(lpDevice->epPageWidth,right_edge);  

#if VERTICAL_CLIPPING			  
/* original ClipRect vertical coordinates are relative to the start of
   the band; top_edge, bottom_edge, & y are in full page coordinates */
		
		top_edge = lpClipRect->top + lpDevice->epYOffset;
		bottom_edge = lpClipRect->bottom + lpDevice->epYOffset;

/* clip region is inclusive of top/left, but exclusive of bottom/right */
#endif  /* took out vertical clipping */		
	}
        else 		/* default to page as clipping region */
	{
#if VERTICAL_CLIPPING		
		top_edge = 0;
		bottom_edge = lpDevice->epPF->YPhys;
#endif		
		left_edge = 0;
		right_edge = lpDevice->epPageWidth;
	}	

#if VERTICAL_CLIPPING	
	/* first see if it is within the vertical coordinates */
	if (((y + lpFont->dfInternalLeading) < top_edge) ||
	    ((y + lpFont->dfInternalLeading+lpFont->dfAscent) > bottom_edge))
	{
/* set up an impossible horizontal clip region */		
		left_edge = lpDevice->epPageWidth + 1; 
	}
#endif	
        /* clip to the left (page or clip) boundary */
        while ((x < left_edge) && (count > 0))
                {
                size = str_out(lpDevice, lpString2, -1, lpFont, lpDrawMode, lpXform);
                lpString2++;
                lpString++;
                --count;
                x += LWORD(size);
                }

        newDDA = lpDrawMode->BreakErr;

        /* clip to the right page boundary */
        while (count > 0)
                {
		lpDevice->epPtr = 0;
#if COLOR
		/* color printers need a few passes */
                size = str_out(lpDevice, lpString2, -count, lpFont, lpDrawMode, lpXform);
                lpDrawMode->BreakErr = newDDA;
#else
                size = str_out(lpDevice, lpString2, count, lpFont, lpDrawMode, lpXform);
#endif
                if (x + LWORD(size) <= right_edge)
                        break;
                --count;
                }
no_print:
        if (count == 0) {
		if (fFree) 
                        GlobalFree(HWORD((long)lpString2));
		lpDevice->epPtr = 0;
        	lpDrawMode->BreakErr = oldDDA;
                return 0;
        }

        lpDevice->epMode |= TEXTFLAG;

	/* insert string into priority queue and flush buffer */
#if defined(TOSHP351)
	/* Toshiba P351 cannot do microspacing in Proportional fonts, 
	 * so insert them one word at a time or even one character at
	 * a time if microspacing is needed in PS fonts
	 */
        if ((lpFont->dfPitchAndFamily & 0x1) &&
            ((lpDrawMode->TBreakExtra && lpDrawMode->BreakCount) || lpDrawMode->CharExtra)) {
		/* PS font and extra pixels */
		BYTE breakChar;
		short strlen, wordlen, wordsize;

         	breakChar = lpFont->dfBreakChar + lpFont->dfFirstChar;
                lpDrawMode->BreakErr = newDDA;

		for (size = strlen = wordlen = 0; strlen < count; strlen++) {
        	    if (lpString2[strlen] != breakChar && !lpDrawMode->CharExtra			&& strlen != count - 1)
		        wordlen++;
		    else {
			lpDevice->epPtr = 0;
        	        str_out(lpDevice, &lpString2[strlen-wordlen], 
				wordlen+1, lpFont, lpDrawMode, lpXform);
        	        wordsize = LWORD(str_out(lpDevice, 
						&lpString2[strlen-wordlen],
				     		-(wordlen+1), 
						lpFont, lpDrawMode, lpXform));
        	        size += InsertString(lpDevice, x+size, y, (short) wordsize);
		        wordlen = 0;
		    }
		}
	} else 
#endif
#if COLOR
		/* do str_out and InsertString for each color */
                size = color_str_out(lpDevice, lpString2, count, lpFont, lpDrawMode, lpXform, x, y);
#else
        	size = InsertString(lpDevice, x, y, (short) size);
#endif


#ifdef SPECIALDEVICECNT
        /* need to simulate double strike */
        if (lpXform->ftWeight >= FW_SEMIBOLD && (lpDevice->epMode & WRONG_LINESP))
                InsertString(lpDevice, x, y, (short) size);
#endif
        lpDevice->epPtr = 0;	/* clear the buffer */

        if (fFree)      /* translation took place */
                {
                register short i;
                short size2, skip;
                BYTE c;

                for (i = 0; i < count; i++)
                        {
                        /* this line is needed for epson since overstriking
                           is done differently for different fonts */
                        if (lpString2[i] == lpString[i] ||
			    (c = Translate(lpString[i], 1)) <= SPACE)
                                continue;

                        lpDrawMode->BreakErr = newDDA;
                        skip = LWORD(str_out(lpDevice, lpString2, - i, lpFont, lpDrawMode, lpXform));
#if COLOR
			/* do str_out and InsertString for each color */
                	color_str_out(lpDevice, &c, 1, lpFont, lpDrawMode, lpXform, x + skip, y);
#else
                        size2 = (short)str_out(lpDevice, &c, 1, lpFont, lpDrawMode, lpXform);
                        size2 = InsertString(lpDevice, x + skip, y, size2);
#endif
#ifdef SPECIALDEVICECNT
                        /* need to simulate double strike */
                        if (lpXform->ftWeight >= FW_SEMIBOLD && (lpDevice->epMode & WRONG_LINESP))
                                size2 = InsertString(lpDevice, x + skip, y, size2);
#endif
        		lpDevice->epPtr = 0;	/* clear the buffer */
                        }
                GlobalFree(HWORD((long)lpString2));
                }
        lpDrawMode->BreakErr = oldDDA;
        return size;
}


short NEAR PASCAL InsertString(lpDevice, x, y,size)
LPDEVICE lpDevice;
short x, y, size;
{
        short tag, count;
        ELEMENT far * ele;

        count = lpDevice->epPtr;

        if ((tag = myAlloc(lpDevice, count)) == -1)
                return 0;

        ele = (ELEMENT far *) (GlobalLock(lpDevice->epHeap) + tag);
        ele->y = y;
        ele->x = x;
        ele->count = count;
        ele->size = size;
        Copy((LPSTR) ele->pstr, lpDevice->epSpool, count);

        if (InsertPQ(lpDevice->epYPQ, tag, y) == ERROR)
                if (SizePQ(lpDevice->epYPQ, Y_INIT_ENT) == ERROR)
                        {
                        GlobalUnlock(lpDevice->epHeap);
                        lpDevice->epHPptr = tag;
                        return FALSE;
                        }
                else
                        InsertPQ(lpDevice->epYPQ, tag, y);

        GlobalUnlock(lpDevice->epHeap);
        return size;
}

LPSTR NEAR PASCAL CheckString(src, cnt, lpFont)
LPSTR       src;
short       cnt;
LPFONTINFO  lpFont;
{
        register LPSTR  p;
        short           i;
        LPSTR           lpString;
        HANDLE          hString;
        BYTE            DefaultChar;

        if (!lpFont->dfDevice)
                {
                /* raster font in draft mode */
                DefaultChar = TMDefaultChar;
                }
        else
                {
                /* device's hardware font */
                DefaultChar = lpFont->dfDefaultChar + lpFont->dfFirstChar;
                }

        for (p = src, i = 0; i < cnt; i++, p++)
                if (*p < lpFont->dfFirstChar || *p >= (BYTE) 0x7f)
                                break;

        /* the string does not contain none-printing characters */
        if (i == cnt)
                return src;

        /* cannot modify the string itself directly */

        if (!(hString = GlobalAlloc(GMEM_FIXED, (long)cnt)))
                return (LPSTR) OEM_FAILED;

        lpString = (LPSTR) GlobalLock(hString);

        for (p = src, i = 0; i < cnt;i++, p++)
                {
                if ((lpString[i] = *p) < lpFont->dfFirstChar
                        || (*p >= (BYTE) 0x7f && *p < TRANS_MIN))
                        lpString[i] = DefaultChar;
                if (*p >= TRANS_MIN)
#if defined(EPSON_OVERSTRIKE)
/* will not translate if we use special ROM chars */
                        if (!Translate(*p, 1) || Translate(*p, 1) >= SPACE)
#endif
                                lpString[i] = Translate(*p, 0);
                }
        GlobalUnlock(hString);

        return lpString;
}

/* 10/19/85 moved up from chphys.c */

short NEAR PASCAL myAlloc(lpDevice, n)
LPDEVICE lpDevice;
short n;
{
        register short ptr = lpDevice->epHPptr;

        if ((lpDevice->epHPptr += n + sizeof(ELEHDR)) >= lpDevice->epHPsize)
        {
            lpDevice->epHPsize = (lpDevice->epHPptr + MARG_BUF) / MARG_BUF * MARG_BUF;
            if (!GlobalReAlloc(lpDevice->epHeap, (LONG)lpDevice->epHPsize, GMEM_MOVEABLE))
                    /* unsuccessful realloc restore the heap size and return failure */
                    {
                    lpDevice->epHPptr = ptr;
                    lpDevice->epHPsize = (lpDevice->epHPptr + MARG_BUF) / MARG_BUF * MARG_BUF;
                    return -1;
                    }
        }
        return  ptr;
}

/* 10/19/85 moved up from chphys */
short FAR PASCAL heapinit(lpDevice)
LPDEVICE lpDevice;
{
        if (!(lpDevice->epHeap = GlobalAlloc(GMEM_MOVEABLE, (long) INIT_BUF)))
                return FALSE;
        lpDevice->epHPsize = INIT_BUF;
        lpDevice->epHPptr = 0;
        return TRUE;
}
