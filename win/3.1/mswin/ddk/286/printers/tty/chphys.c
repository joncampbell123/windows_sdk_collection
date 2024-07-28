/*/  CHPHYS.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//
//  09 apr 91   peterwo     replace draftStrBlt by ExtTextOut.
//	08 jan 90	peterbe		Restore initial Q size to 100.
//	04 jan 90	peterbe		Allocate QSTRUCT's (text buffers) on
//					local instead of global heap.
//	27 dec 89	peterbe 	Use lpdv->epYPQ instead of
//					static hQueue. Add lpdv param to
//					InitQueue(), DeleteQueue().
//	08 dec 89	peterbe		Handle left margin clipping when
//					there's no clip rect.  Refine handling
//					of underlining.  Force resending of
//					double-width escapes at start of a
//					new line.
//	07 dec 89	peterbe		Font selection calls added:
//					SelectWidth() and SetWidth().
//	06 dec 89	peterbe		Priority queue routines working:
//					new DraftStrBlt(), InitQueue(),
//					DeleteQueue(), TextDump().
//	05 dec 89	peterbe		Adding priority queue handling of
//					DraftStrBlt().
//	14 nov 89	peterbe		Revised/translated some comments.
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"

#ifdef DEBUG
#include "debug.h"
#define DBGphys(msg) DBMSG(msg)
#else
#define DBGphys(msg) /* zip */
#endif

#define NFONTS 6

/* b es el word alto */
extern	 GRAPHRESET reset;

// priority queue size for DraftStrBlt() and TextDump()
// (we don't use Y_INIT_ENT from TTY.H)

// DraftStrBlt puts a string in records like this and then enqueues them
// according to their position.

typedef struct
{
	short x;			// x-origin of string
	short y;			// y-origin of string
	#ifdef DEBUG
	short key;			// save here for debug display
	#endif
	short passes;			// no. of passes for overstrike
	BOOL underline;			// underline attribute
	short charwidth;		// width of a char (in 1 of 6 fonts)
	short count;			// no. of bytes
	BYTE s[1];			// actual string (0-terminated)
} QSTRUCT;

typedef QSTRUCT * PQSTRUCT;

#ifdef DEBUG
int	iQsize;
#endif

// Ep_Output_String(): This does the actual printing of a string.

void NEAR PASCAL Ep_Output_String(
    LPDV lpdv,
    LPSTR lpString,
    short count,
    BYTE defa_c )
{
    register short i, j;

    j = 0;
    for(i = 0; i < count; i++)
	if(lpString[i] >= TRANS_MIN){
	    myWrite(lpdv, &lpString[j], i - j);
	    char_out(lpdv, lpString[i], defa_c);
	    j = i + 1;
	}
    myWrite(lpdv, &lpString[j], i - j);
}

void NEAR PASCAL char_out(lpdv, c, defa_c)
LPDV lpdv;
unsigned char c, defa_c;
{
    unsigned i;
    static unsigned char expanded[9];

    if(c >= TRANS_MIN){
	i = Translate(c, expanded, 8, defa_c);
	if(EP_type(i)==STRING_TRANS)
	    myWrite(lpdv, (LPSTR) expanded, EP_len(i));
	else{
	    i = EP_car(i);
	    myWrite(lpdv, (LPSTR) &i, 1);
	}
    }else
	myWrite(lpdv, (LPSTR) &c, 1);
}



// TextDump() prints the text in the queue.
// if fPrint is FALSE, it just flushes out the queue and frees
// all the string buffers.

void FAR PASCAL TextDump(
    LPDV lpdv,
    BOOL fPrint)			// if FALSE, we just free buffers
{
    int iQ;
    HANDLE hQstruct;
    PQSTRUCT pQstruct;
    int passes;
    int x, y;
    LPSTR lpstr;
    int count;
    int i;
    int j;
    int lasty;
    BOOL fNewLine;

    lasty = -1;

    DBGphys(("TextDump():\n"));

    if (NULL != lpdv->epYPQ)
	{
	while ((iQ = ExtractPQ(lpdv->epYPQ)) != -1)
	    {
	    hQstruct = (HANDLE) iQ;

	    if (!fPrint)
		{
		LocalFree(hQstruct);
		continue;
		}

	    if (NULL != (pQstruct = (PQSTRUCT)LocalLock(hQstruct)) )
		{
		// display on debug terminal.
		DBGphys(("key = %d, ", pQstruct->key));

		DBGphys((
		  "X=%d, Y=%d, passes=%d, %lsCharWidth=%d, Count=%d <%s>\n",
			pQstruct->x,
			pQstruct->y,
			pQstruct->passes,
			(pQstruct->underline) ?
				(LPSTR)"Underline, "  : (LPSTR)"",
			pQstruct->charwidth,
			pQstruct->count,
			(PSTR) pQstruct->s
			));

		// put some stuff in local variables..

		passes = pQstruct->passes;
		x = pQstruct->x;
		y = pQstruct->y;
		count = pQstruct->count;
		lpstr = pQstruct->s;

		// set Y position.  If we're past the end of the page,
		// just free the data buffer.

		fNewLine = lasty != y;

		YMoveTo(lpdv, y);

		lasty = y;

		// Set the width.  Send appropriate escape if there's
		// a change.

		// Print the string (1 to 3 times -- we overstrike to do
		// boldface).

		if(x >= 0)
		  while(passes--)
		    for (j = (pQstruct->underline) ? 1 : 0; j >= 0; j--)
			{  // position the cursor and print the text.
			XMoveTo(lpdv, x, FALSE);

			SetWidth(lpdv, fNewLine, pQstruct->charwidth);
			switch(j)
			    {
			    case 0:	// print the text
				Ep_Output_String(lpdv, lpstr, count, '_');
				break;

			    case 1:	// underline.
				// We overstrike with '_' in order to
				// underline.
				for(i = 0; i < count; i++)
//				    myWrite(lpdv, (*(lpstr+i) != ' ')?
//						    "_":" ", 1);
				    myWrite(lpdv, "_", 1);
			    }

			// advance the current X position according
			// to the font size.

			lpdv->sCurx +=
				(count * lpdv->epXcurwidth);

			// compressed font actually has width of 7.2
			// we are only accounting for 7 so use epCount
			// to keep track.  One unit in epCount is 0.2
			// reported x position.

			if (lpdv->epXcurwidth == 7)  // compressed
			    // 1 pixel == 5 count
			    lpdv->epCount += count;

			// 8 cpi font is 14.4 pixels.  We report 15.
			if (lpdv->epXcurwidth == 15) // compr. exp.
			    lpdv->epCount -= 3 * count;

			lpdv->sCurx += lpdv->epCount / 5;
			lpdv->epCount %= 5;
			}

		// Unlock the data record and free it.
		LocalUnlock(hQstruct);
		LocalFree(hQstruct);
		}
	    }	// while..
	}	// lpdv->epYPQ != NULL

#ifdef DEBUG
iQsize = 0;
#endif

}	// end TextDump()
#if 0

short FAR PASCAL SelectWidth(short width)
{
    int outwidth;

    if(width < ELITE_WIDTH)
	    return COMP_MODE_WIDTH;
    else if(width < PICA_WIDTH)
	    return ELITE_WIDTH;
    else if(width < (int)COMP_MODE_WIDTH2)
	    return PICA_WIDTH;
    else if(width < ELITE_WIDTH2)
	    return COMP_MODE_WIDTH2;
    else if(width < PICA_WIDTH2)
	    return ELITE_WIDTH2;
    else
	    return PICA_WIDTH2;

}	// SelectWidth()
#endif


// This function normalizes a requested width, stores it in lpdv,
// and sends the appropriate escape.

void FAR PASCAL SetWidth(lpdv, newline, width)
LPDV lpdv;
BOOL newline;
short width;
{
    // if this is a new line, and we were in an expanded mode,
    // we want to force sending new escapes even if the width
    // isn't changing, because most printers cancel expanded mode
    // at the end of a line.

    // need expand on
    if (width > PICA_WIDTH)
	{
	if (newline || !lpdv->bExpandOn)
	    {
	    myWrite(lpdv, ESCEXP(lpdv->escapecode.expand_on));
	    lpdv->bExpandOn = TRUE;
	    }
	width >>= 1;
	}
    else
	// need to turn expand off
	if (lpdv->bExpandOn)
	    {
	    myWrite(lpdv, ESCEXP(lpdv->escapecode.expand_off));
	    lpdv->bExpandOn = FALSE;
	    }

    if (lpdv->epXcurwidth == width)
	return;

    // normalize the requested width.  If the requested width is
    // different from the current width, send appropriate escapes
    // and save the new width.

    switch(width)
    {
    case COMP_MODE_WIDTH:
	    myWrite(lpdv, ESCEXP(lpdv->escapecode.compress_on));
	    lpdv->epXcurwidth = COMP_MODE_WIDTH;
	    break;

    case ELITE_WIDTH:
	    myWrite(lpdv, ESCEXP(lpdv->escapecode.elite_on));
	    lpdv->epXcurwidth = ELITE_WIDTH;
	    break;

    case PICA_WIDTH:
	    myWrite(lpdv, ESCEXP(lpdv->escapecode.pica_on));
	    lpdv->epXcurwidth = PICA_WIDTH;
	    break;
    }
}	// SetWidth()


WORD  Min(a, b)
WORD  a, b;
{
    if(a < b)
        return(a);
    else
        return(b);
}



WORD  Max(a, b)
WORD  a, b;
{
    if(a < b)
        return(b);
    else
        return(a);
}


long FAR PASCAL      devExtTextOut(
    LPDV	lpdv,
    short           x,
    short           y,
    LPRECT          lpClipRect,
    LPSTR           lpString,
    short           Count,
    LPFONTINFO      lpFontInfo,
    LPDRAWMODE      lpDrawMode,    /* includes background mode and bkColor */
    LPTEXTXFORM     lpXform,
    short far       * lpCharWidth,    // not in StrBlt()
    LPRECT          lpOpaqueRect,    // not in StrBlt()
    WORD            Options        // not in StrBlt()
)
{
    WORD  ExtraSpacesPerChar,     
                            //  how many extraSpaces should be
                            //  inserted per character?
            newCount,       //  how many characters will the padded
                            //  output string contain?
            ExcessSpace,    //  Total number of pixels resulting from
                            //  CharacterExtra padding not converted to
                            //  spaces.
            BreakCount,     //  normalized count of break chars
            TBreakExtra,    //  number of pixels to fill
            ExtraSpacesPerBreak,
                            //  how many extraSpaces should be
                            //  inserted per space?
            ExtraSpaces,    //  how many extra spaces will be inserted
                            //  at this particular point?
            BreakRem,       //  number of pixels that must be
                            //  distributed unequally among the breaks.
            Xpos,           //  current X position of this character.
            newXpos,        //  where to move the cursor before printing.
            j,              //  indexes  pBuf->s[]  array. (target)
            k, i,           //  temp  counter, src index
            key,            //  used to sort records in the Priority Queue.
            passes,         //  number of passes - used to embolden text.
            nBytes,         //  how many bytes will this character occupy
                            //  including padding - or how many padding
                            //  bytes accompany this character. (two uses)
            width;          //  width of TextExtent (ignores ClipRect)
    short   threshold,      //  PosError must be more negative than this
                            //  before an additional space char is inserted.
            CharacterExtra, //  number of extra pixels to be added
                            //  to the width of every character.
            PosError;       //  positionalError of this break char
                            //  where it is printed on the page vs
                            //  where it should be printed. 
                            //  since this is usually a fraction < 1, it is
                            //  multiplied by TrueBreakCount to
                            //  be usable.
    HANDLE  hQBuf;          //  Priority Queue Record
    PQSTRUCT    pQBuf;      //  pointer to Queue Record.
    BOOL    bExtents;       //  this flag gets set if only extents info
                            //  is requested.
    RECT    rcPageSize;     //  initialized to physical page size
                            //  but later holds intersection of
                            //  lpClipRect with page size.


    if (Count == 0)
        return(0L);  // can't draw opaque rectangle!

    if (Count < 0)
    {
        Count *= -1;   //  make positive
        bExtents = TRUE;
    }
    else
        bExtents = FALSE;

    //  compute width of string to be printed.
    //  Consider justification and proportional spacing.

    if(lpCharWidth)
    {
        for(width = i = 0 ; i < Count ; i++)
        {
            width += lpCharWidth[i];
        }
        width /= lpFontInfo->dfPixWidth ;
        width *= lpFontInfo->dfPixWidth ;
        width = Max(width, lpFontInfo->dfPixWidth * Count);
    }
    else
    {
        CharacterExtra = lpDrawMode->CharExtra;
        ExtraSpacesPerChar = CharacterExtra / lpFontInfo->dfPixWidth ;
        newCount = Count * (ExtraSpacesPerChar + 1);
        if(lpDrawMode->TBreakExtra)  // pad the Breaks.
        {
            ExcessSpace = CharacterExtra % lpFontInfo->dfPixWidth * Count;
            //  if the CharacterExtra space is not a multiple of a space,
            //  we must account for the unused space and distribute it
            //  in the breaks if Break padding is used.
            TBreakExtra = lpDrawMode->TBreakExtra + ExcessSpace;
            newCount += TBreakExtra / lpFontInfo->dfPixWidth ;
            BreakCount = lpDrawMode->BreakCount * lpFontInfo->dfPixWidth ;
            ExtraSpacesPerBreak = TBreakExtra / BreakCount;
            BreakRem = TBreakExtra % BreakCount;
            PosError = lpDrawMode->BreakErr;
            threshold = lpDrawMode->BreakCount / 2 + 1 - BreakCount;
        }
        width = newCount * lpFontInfo->dfPixWidth ;
        if (bExtents && lpDrawMode->TBreakExtra)
            lpDrawMode->BreakErr -= 
                        lpDrawMode->BreakCount * BreakRem % BreakCount;
    }


    if (bExtents)
    {
        return(MAKELONG(width, lpFontInfo->dfPixHeight));
        //  place width in loword, height in hiword.
    }

    // Do not print white text
    if ((lpDrawMode->TextColor & 0xffffff) == 0xffffff)
	return 0;

//  What if Transparent mode?
//  if(lpDrawMode->bkColor == lpDrawMode->TextColor)
//	return(0L);	//  invisible.

    if (lpdv->epYPQ == NULL)
        return(0L);     //  some type of error - hell if I know what.

    //  intersect clipping rectangle with page dimensions
    //  rcPageSize  becomes the new clipping rectange.
    rcPageSize.left = 0;
    rcPageSize.right = lpdv->epPageWidth;
    rcPageSize.top = 0;
    rcPageSize.bottom = lpdv->epPageHeight;

    if(!IntersectRect((LPRECT)&rcPageSize, (LPRECT)&rcPageSize, lpClipRect))
        return(0L);    //  no intersection 


    if(y < rcPageSize.top  ||  y + lpFontInfo->dfPixHeight > rcPageSize.bottom)
        return(0L);  //  font won't fit in Clipping rectangle

    //  otherwise create one record to be added to the priority queue.
    //  how do we apply clipping rectangle?  when padding is in effect
    //  we don't know which characters will fall between x1 and x2.
    //  We can figure out the number of characters which occupy
    //  that Extent.

    hQBuf = NULL;   // initialize
    j = 0;          //  j indexes the target string.

    if(lpCharWidth == 0)
    //  distribute spaces as needed to pad the line of text
    {
        WORD   breaksEncntered = 0;  //  will only treat the first
            //  breakCount  breaks  as breaks.  There after all
            //  other breaks have no special meaning.
            //  this is to prevent overwritting the allocated
            //  buffer space in case the breakCount specified is
            //  less than the actual number of breaks in lpString.

        //  is there any intersection between lpClipRect and output text?
        //  if not exit now.  Otherwise allocate the memory.

        if(x >= rcPageSize.right  ||
            x + newCount * lpFontInfo->dfPixWidth < rcPageSize.left)
            return(0L);  // Null intersection

        //  we can use all methods documented in DDK sec. 2.7.5 to
        //  perform padding to the new case where cursor position
        //  can only be changed in units of dfPixWidth  pixels,
        //  by defining a local variable : 'BreakCount'
        //  Using this redefinition, PosError still has its original
        //  meaning: ie: PosError / lpDrawMode->BreakCount = characters
        //  positional error in pixels (fractions of a pixel).

        newCount = Min(newCount, (rcPageSize.right - rcPageSize.left) /
                                lpFontInfo->dfPixWidth);
            //  the actual number of chars printed may be even smaller
            //  if the intersection of the two is smaller than either
            //  the original string length or the clipping rectangle.

        hQBuf = LocalAlloc(GMEM_MOVEABLE, 
                        sizeof(QSTRUCT) + newCount + 1);
        if(!hQBuf)
            return(MAKELONG(0, 0x8000));  // error - failed to allocate

        pQBuf = (PQSTRUCT)LocalLock(hQBuf);

        for(newXpos = Xpos = x, i = 0 ; i < Count ; i++)
        {
            BYTE  letter;
            WORD  deltaX;

            

            letter = lpString[i];

            if(letter == VK_SPACE  &&  lpDrawMode->TBreakExtra  &&
                breaksEncntered < lpDrawMode->BreakCount)
            {
                breaksEncntered++;
                ExtraSpaces = ExtraSpacesPerChar + ExtraSpacesPerBreak;   
                    // initial value.
                PosError -= BreakRem;
                if(PosError <= threshold) //  err so that print will 
                        // never extend beyond specified length.
                {
                    ExtraSpaces++;
                    PosError += BreakCount;
                }
            }
            else
                ExtraSpaces = ExtraSpacesPerChar;

            deltaX = (ExtraSpaces + 1) * lpFontInfo->dfPixWidth;

            if(Xpos < rcPageSize.left)  //  if even a portion of the 
            {               //  character is clipped,   don't print
                Xpos += deltaX;
                newXpos = Xpos;    //  start printing line here!
            }
            else if(Xpos + deltaX > rcPageSize.right)
                break;  
            else
            {
                pQBuf->s[j++] = letter;
                for(k = 0 ; k < ExtraSpaces ; k++)
                    pQBuf->s[j++] = VK_SPACE;
                Xpos += deltaX;
            }
        }
    }
    else    // lpCharWidth exists.
    {
        WORD    
            StartRead,      //  what character in lpString should
                            //  be output first.
            nRead;          //  how many characters in lpString
                            //  should be read.
        short   error;      //  difference between reqPos and actPos.


        error = StartRead = nRead = newCount = 0;

        for(newXpos = Xpos = x, i = 0 ; i < Count ; i++)
        {
            //  this pass is used just to determine
            //  newCount, StartRead, nRead   and  newXpos.

            WORD  deltaX;
            
            if(lpCharWidth[i] <= lpFontInfo->dfPixWidth)
            {
                error -= lpFontInfo->dfPixWidth - lpCharWidth[i];
                nBytes = 1;
            }
            else
            {
                nBytes = lpCharWidth[i] / lpFontInfo->dfPixWidth;
                error += lpCharWidth[i] % lpFontInfo->dfPixWidth;
            }
            if(lpString[i] == VK_SPACE  &&  error >= lpFontInfo->dfPixWidth)
            {
                nBytes += error / lpFontInfo->dfPixWidth;
                error %= lpFontInfo->dfPixWidth;
            }

            deltaX = nBytes * lpFontInfo->dfPixWidth;

            if(Xpos < rcPageSize.left)  //  if even a portion of the 
            {               //  character is clipped,   don't print
                StartRead = i + 1;
                Xpos += deltaX;
                newXpos = Xpos;    //  start printing line here!
            }
            else if(Xpos + deltaX > rcPageSize.right)
                break;  
            else
            {
                nRead++;
                newCount += nBytes;
                Xpos += deltaX;
            }
        }


        if(nRead)       //  allocate only if characters to transfer.
        {
            hQBuf = LocalAlloc(GMEM_MOVEABLE, 
                            sizeof(QSTRUCT) + newCount + 1);
            if(!hQBuf)
                return(MAKELONG(0, 0x8000));  // error - failed to allocate

            pQBuf = (PQSTRUCT)LocalLock(hQBuf);

            lpCharWidth += StartRead;   //  reset starting point 
            lpString += StartRead;
        }

        for(error = i = 0 ; i < nRead ; i++)
        {
            //  this pass writes appropriately padded subset
            //  of lpString into pQBuf->s

            if(lpCharWidth[i] <= lpFontInfo->dfPixWidth)
            {
                error -= lpFontInfo->dfPixWidth - lpCharWidth[i];
                nBytes = 1;
            }
            else
            {
                nBytes = lpCharWidth[i] / lpFontInfo->dfPixWidth;
                error += lpCharWidth[i] % lpFontInfo->dfPixWidth;
            }
            if(lpString[i] == VK_SPACE  &&  error >= lpFontInfo->dfPixWidth)
            {
                nBytes += error / lpFontInfo->dfPixWidth;
                error %= lpFontInfo->dfPixWidth;
            }
            
            nBytes--;   //  represents number of padding characters to add.
            
            pQBuf->s[j++] = lpString[i];

            for(k = 0 ; k < nBytes ; k++)
                pQBuf->s[j++] = VK_SPACE;
        }
    }

    if(!j)  //  if no characters are being printed.
    {
        if(hQBuf)
        {
            LocalUnlock(hQBuf);
            LocalFree(hQBuf);
        }
        return(0L);
    }

    // determine if we have bold face or even medium text.
    if( lpFontInfo->dfWeight < FW_MEDIUM)
        passes = 1;
    else if(lpFontInfo->dfWeight < FW_BOLD)
        passes = 2;
    else
        passes = 3;

    //  initialize the PQ record:
    pQBuf->count = j;
    pQBuf->s[j] = '\0' ; // null terminate string.
    pQBuf->x = newXpos;
    pQBuf->y = y;
    pQBuf->passes = passes;
    pQBuf->charwidth = lpFontInfo->dfPixWidth;
    pQBuf->underline = lpFontInfo->dfUnderline;
//    pQBuf->StrikeOut = lpFontInfo->dfStrikeOut;


    LocalUnlock(hQBuf);

    // calculate a priority based on the coordinates of the
    // string.	This is the line position times 80 column/line,
    // plus the column position.

    key = y * lpdv->epPageWidthInch + (x / HDPI);

    // now insert the key and the record handle in the queue.

    if (InsertPQ(lpdv->epYPQ, hQBuf, key) == -1)
    {            // we failed, so try to increase the size of the queue
	SizePQ(lpdv->epYPQ, INCRQSIZE);
        // try again .. if we fail, free the record.

	if (InsertPQ(lpdv->epYPQ, hQBuf, key) == -1)
            LocalFree(hQBuf);
    }

    return(0L);
}


long FAR PASCAL	  StrBlt(
    LPDV lpdv,
    short   x,
    short   y,
    LPRECT  lpClipRect,
    LPSTR   lpString,
    short   count,
    LPFONTINFO	lpFont,
    LPDRAWMODE	lpDrawMode,	/* includes background mode and bkColor */
    LPTEXTXFORM lpXform)
{
    // here's what we really do:
	return devExtTextOut(lpdv, x, y, lpClipRect, lpString, count, lpFont,
		lpDrawMode, lpXform, (short far *)NULL, (LPRECT)NULL, NULL);

}
