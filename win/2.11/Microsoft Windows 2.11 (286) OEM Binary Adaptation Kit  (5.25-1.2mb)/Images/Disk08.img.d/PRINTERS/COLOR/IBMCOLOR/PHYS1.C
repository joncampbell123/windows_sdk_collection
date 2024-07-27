#include <generic.h>


#ifdef CITOH 
static char flip_table[256] = {
0x0, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 
0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0, 
0x8, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 
0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8, 
0x4, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 
0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4, 
0xc, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 
0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc, 
0x2, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 
0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2, 
0xa, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 
0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa, 
0x6, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 
0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6, 
0xe, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 
0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe, 
0x1, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 
0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1, 
0x9, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 
0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9, 
0x5, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 
0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5, 
0xd, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 
0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd, 
0x3, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 
0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3, 
0xb, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 
0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb, 
0x7, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 
0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7, 
0xf, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 
0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff	     };
#endif  /* CITOH (bitflip table) */

extern GRAPHRESET reset;
extern DELY dely;
/* writes buflen worth of graphics output to the spool buffer.
   output this graphics buffer the y coordinate passed in.
   it also calls ch_line_out to output all the hardware text
   that should be dropped between y and y + NPINS */

#if NO_LINE_OUT
#elif COLOR
/* color printer driver must define its own line_out().
 */
void NEAR PASCAL line_out(lpDevice, buf, buflen, y, colorindex)
LPDEVICE lpDevice;
LPSTR buf;
short buflen;
short y;
short colorindex;
{
        register WORD far *bufptr;
        register short tmp;

	if (buflen % 2)
		FatalExit(0x9999);

	/* treat it as word array */
	buflen /= 2;
	/* scan from right */
        for (bufptr = (WORD far *)buf + buflen; buflen; --buflen)
                if (*--bufptr != 0)
                    break;

	#if defined(CITOH)	
		/* doesn't have ordinary high speed code setting */
	#else
		/* SPEED bit set ==> fast */		
        reset.code = (lpDevice->epMode & HIGHSPEED ? FASTXMS: XMS);
	#endif

        if (buflen)
        {
                YMoveTo(lpDevice, y);
		/* scan from left */
                for (bufptr = (WORD far *)buf; !*bufptr;)
                        bufptr++;

                tmp = (LPSTR)bufptr - buf;
                tmp -= XMoveTo(lpDevice, tmp, FALSE);

		/* now treat it as byte array */
		buflen *= 2;
                buflen -= tmp;
                buf = (LPSTR)buf + tmp;

                lpDevice->epCurx += buflen;

                reset.cnt = buflen;
		myWrite(lpDevice, (LPSTR)CMYTable[colorindex].code,
			CMYTable[colorindex].length);
                myWrite(lpDevice, (LPSTR)&reset, 4);
                myWrite(lpDevice, (LPSTR)buf, buflen);
		/* bring print head back to the left margin */
                XMoveTo(lpDevice, tmp, TRUE);
        }
}
#else
void NEAR PASCAL line_out(lpDevice, buf, buflen, y)
LPDEVICE lpDevice;
LPSTR buf;
short buflen;
short y;
{
        register WORD far *bufptr;
        register short tmp;

	if (buflen % 2)
		FatalExit(0x9999);

	/* treat it as word array */
	buflen /= 2;
        for (bufptr = (WORD far *)buf + buflen; buflen; --buflen)
                if (*--bufptr != 0xffff)
                    break;



	#if defined(CITOH)	
		/* doesn't have ordinary high speed code setting */
	#else
		/* SPEED bit set ==> fast */
        reset.code = (lpDevice->epMode & HIGHSPEED ? FASTXMS: XMS);
	#endif


        if (buflen)
        {
                epsstrip((WORD far *)buf, buflen);
                YMoveTo(lpDevice, y);
                for (bufptr = (WORD far *)buf; !*bufptr;)
                        bufptr++;

                tmp = (LPSTR)bufptr - buf;
                tmp -= XMoveTo(lpDevice, tmp, FALSE);

		/* now treat it as byte array */
		buflen *= 2;
                buflen -= tmp;
                buf = (LPSTR)buf + tmp;

                lpDevice->epCurx += buflen;

#ifdef CITOH
		itoa4(buflen,(LPSTR) reset.count_string);
                myWrite(lpDevice, (LPSTR)&reset, 8);			
#else
                reset.cnt = buflen;
                myWrite(lpDevice, (LPSTR)&reset, 4);
#endif

#ifdef CITOH    /* wires are reverse for citoh landscape*/
		bitflip(buf, buflen);	/* flip the bits */
#endif    

                myWrite(lpDevice, (LPSTR)buf, buflen);
        }
        /* output all character string before this line of graphics */
        if ((lpDevice->epMode & TEXTFLAG) && !ch_line_out(lpDevice, y + NPINS))
                lpDevice->epMode &= ~TEXTFLAG;
}
#endif


#if NO_COLOR_LINE_OUT
#elif COLOR
/* for color printers only */
void NEAR PASCAL color_line_out(lpDevice, buf, buflen, y)
LPDEVICE lpDevice;
LPSTR buf;
short buflen;	/* assume to be of length equal to lpDevice->epPageWidth */
short y;
{
        WORD far *bufptr;
        WORD far *bwbuf;	/* temporary buffer for black/white colors */
        register short tmp;
	short z;
  
	if (lpDevice->epPageWidth % 2 || buflen != lpDevice->epPageWidth)/* must be multiple of words */
		FatalExit(0x9999);

        bwbuf = (WORD far *)((LPSTR)buf + lpDevice->epPageWidth * NPLANES);
        bufptr = (WORD far *)buf;

	/* black and white only */
	if (lpDevice->epBmpHdr.bmPlanes == 1) {
        	for (tmp = lpDevice->epPageWidth / 2 ; tmp; tmp--) {
			*bufptr++ = ~*bufptr;
		}
		line_out(lpDevice, (LPSTR)buf, lpDevice->epPageWidth , y, BLACK);
		goto exit_color_line_out;
	}

	/* process black color first 
	 *                         _     _     _   _____________
	 * BLACK = C AND M AND Y = R AND G AND B = (R OR G OR B)
	 */

        for (tmp = 0; tmp < lpDevice->epPageWidth / 2; tmp++) {
        	bwbuf[tmp] = ~(bufptr[lpDevice->epPageWidth * 2 / 2] | 
			       bufptr[lpDevice->epPageWidth * 1 / 2] |
			       bufptr[lpDevice->epPageWidth * 0 / 2]);
		bufptr++;
	}
	line_out(lpDevice, (LPSTR)bwbuf, lpDevice->epPageWidth, y, BLACK);

	/* Transform RGB planes to CMY planes
	 *     _      _       _
	 * C = R; M = G ; Y = B
	 *
	 * We perform an inversion on each plane and also
	 * clear the corresponding black bits of the CMY planes 
	 * (to prevent black color printed).
	 */

        bufptr = (WORD far *)buf;
	/* clear CMY if CMY = WHITE */
	for (z = NPLANES; z; z--) {
        	for (tmp = 0; tmp < lpDevice->epPageWidth / 2; tmp++)
        		*bufptr++ = ~*bufptr ^ bwbuf[tmp];
	}

	/* process CMY colors
	 */

	buf = (WORD far *)buf + (NPLANES - 1) * lpDevice->epPageWidth / 2;
	/* print light color first */
	for (z = YELLOW; z; z >>= 1) { 
	        line_out(lpDevice, (LPSTR)buf, lpDevice->epPageWidth, y, z);
	        buf = (WORD far *)buf - lpDevice->epPageWidth / 2;
	}

exit_color_line_out:
        /* output all character string before this line of graphics */
        if ((lpDevice->epMode & TEXTFLAG) && !ch_line_out(lpDevice, y + NPINS))
                lpDevice->epMode &= ~TEXTFLAG;
}
#endif


#if NO_DUMP
#elif COLOR
/* for color printers */
/* dumps the current band of graphics output */

void NEAR PASCAL dump(lpDevice)
LPDEVICE lpDevice;
{
    register LPSTR bmp, bmp0, temp;
    short x, y, z, offset;
    LPSTR buffer;

    buffer = lpDevice->epBuf;
    if (lpDevice->epType == DEV_LAND)
        {
        offset = lpDevice->epXOffset - BAND_HEIGHT;

        for (x = 0; x < BAND_LINE;x++)
            {
            temp = buffer + lpDevice->epPageWidth * lpDevice->epBmpHdr.bmPlanes;
	    bmp = (LPSTR) lpDevice->epBmp + x;
	    for (bmp0 = bmp, z = lpDevice->epBmpHdr.bmPlanes; z; z--) {
	    	bmp = bmp0 + (z - 1) * (BAND_SIZE / NPLANES);
            	for (y = lpDevice->epPageWidth; y; --y, bmp += BAND_LINE)
                    *--temp = *bmp;
            }
            color_line_out(lpDevice, buffer, lpDevice->epPageWidth, x * NPINS + offset);
            }
        }
    else    /* portrait */
    {
        bmp = lpDevice->epBmp;
        offset = lpDevice->epYOffset - BAND_HEIGHT;
        for (x = 0; x < BAND_LINE; x++)
        {
	    for (z = lpDevice->epBmpHdr.bmPlanes; z; z--) {
            	dmTranspose(bmp + (z - 1) * (BAND_SIZE / NPLANES), 
			    buffer + (z - 1) * lpDevice->epPageWidth,
			    lpDevice->epPageWidth >> 3);
            }
            color_line_out(lpDevice, buffer, lpDevice->epPageWidth, x * NPINS + offset);
            bmp += lpDevice->epPageWidth;
        }
    }
    /* when we finish dumping a band there should not be any character
       strings left in the queue */
    if (MinPQ(lpDevice->epYPQ) != ERROR)
            FatalExit(MinPQ(lpDevice->epYPQ));
}
#else
/* dumps the current band of graphics output */

void NEAR PASCAL dump(lpDevice)
LPDEVICE lpDevice;
{
    register LPSTR bmp, temp;
    short x, y, offset;
    LPSTR buffer;

    buffer = lpDevice->epBuf;
    if (lpDevice->epType == DEV_LAND)
        {
        offset = lpDevice->epXOffset - BAND_HEIGHT;

        for (x = 0; x < BAND_LINE;x++)
            {
            temp = buffer + lpDevice->epPageWidth;
            bmp = (LPSTR) lpDevice->epBmp + x;
            for (y = lpDevice->epPageWidth; y; --y, bmp += BAND_LINE)
                *--temp = *bmp;

            line_out(lpDevice, buffer, lpDevice->epPageWidth, x * NPINS + offset);
            }
        }
    else    /* portrait */
    {
        bmp = lpDevice->epBmp;
        offset = lpDevice->epYOffset - BAND_HEIGHT;
        for (x = 0; x < BAND_LINE; x++)
        {
            dmTranspose(bmp, buffer, lpDevice->epPageWidth >> 3);
            line_out(lpDevice, buffer, lpDevice->epPageWidth, x * NPINS + offset);
            bmp += lpDevice->epPageWidth;
        }
    }
    /* when we finish dumping a band there should not be any character
       strings left in the queue */
    if (MinPQ(lpDevice->epYPQ) != ERROR)
            FatalExit(MinPQ(lpDevice->epYPQ));
}
#endif

#if NO_EPSSTRIP
#else
void NEAR PASCAL epsstrip(buf, n)
register WORD far *buf;
short n;
{
        while (n--)
        {
            *buf = ~*buf;
            buf++;
        }
}
#endif

#if NO_MYWRITE
#else
/* writes n characters to the spool buffer */

void FAR PASCAL myWrite(lpDevice, str, n)
LPDEVICE lpDevice;
LPSTR str;
short n;
{
        register LPSTR p;
        register short i;

        if (lpDevice->epDoc != TRUE || n <= 0)
                return;

        p = &lpDevice->epSpool[i = lpDevice->epPtr];
        if ((i += n) >= SPOOL_SIZE(lpDevice->epPageWidth))
        {
            if (myWriteSpool(lpDevice) < 0)
                    return ERROR;
            p = lpDevice->epSpool;
            i = n;
        }

        lpDevice->epPtr = i;

        Copy(p, str, n);
}
#endif

#if NO_CH_LINE_OUT
#else
/* output all the character strings before y, return zero if there is
   nothing left in the queue of character strings */

BOOL NEAR PASCAL ch_line_out(lpDevice, y)
LPDEVICE lpDevice;
short y;
{
        short  tag;
        ELEMENT far * ele;
        LPSTR base;
        HANDLE yPQ;

        yPQ = lpDevice->epYPQ;
        base = GlobalLock(lpDevice->epHeap);

        for (;;)
        {
                if ((tag = MinPQ(yPQ)) == ERROR)
                        break;

                ele = (ELEMENT far *) (base + tag);

                if (ele->y >= y)
                        break;
                if (tag != ExtractPQ(yPQ))
                        {
                        /* FatalExit(0x05) */
                        break;
                        }
                YMoveTo(lpDevice, ele->y);
                XMoveTo(lpDevice, ele->x, TRUE);
                myWrite(lpDevice, ele->pstr, ele->count);
                lpDevice->epCurx += ele->size;
        }
        GlobalUnlock(lpDevice->epHeap);

        return (tag == ERROR? FALSE: TRUE);
}
#endif

#if NO_YMOVETO
#else
/* moves the cursor the position y, and update the lpDevice->epCury
   to reflect the changes */

FAR PASCAL YMoveTo(lpDevice, y)
LPDEVICE lpDevice;
short y;
{
        short diff;

        if ((diff = y - lpDevice->epCury) <= 0)
                return TRUE;

        /* do not do super or subscripts in draftmode */

	/*
         * if (lpDevice->epMode & DRAFTFLAG)
         *         if (diff < VDPI / 8)
         *                return TRUE;
	 */

        /* move the printer head down by diff scan lines */

#ifdef CITOH		/* peculiarity of CITOH printer needs <CR> ???? */
        myWrite(lpDevice, ESCEXP(escapecode.cr));	
	lpDevice->epCurx = 0;	/* hopefully don't do xmoveto then ymoveto */
#endif	
	
        for (; diff > 0; diff -= MAXDELY)
                {
                register short y;

                y = diff >= MAXDELY? MAXDELY: diff;

#ifdef CITOH
		{
		    int double_y;
		    
		    double_y = y << 1;		    
		    dely.tens_digit = (double_y / 10) + '0';
		    dely.ones_digit = (double_y % 10) + '0';
	        }
#else
                dely.cnt = y * dely.mult;
#endif

                myWrite(lpDevice, (LPSTR) &dely, dely.length);
                }

        lpDevice->epCury = y;
        /* for some printers feed causes an immediate carriage return
           this will take care of those printers */
#if defined(EPSON) || defined(LQ1500)
        /* force a carriage return when in draftmode, since we
           don't backup in x direction in draftmode */
        if (lpDevice->epMode & DRAFTFLAG)
#endif
                {
                myWrite(lpDevice, ESCEXP(escapecode.cr));
                lpDevice->epCurx = 0;
#if SPECIALDEVICECNT
                lpDevice->epXRemain = 0;
#endif
                }
        return TRUE;
}
#endif

#if NO_XMOVETO
#else
/* moves the cursor postion to x, with or without microspace justification
   as specified by ms.
   returs the difference between actual cursor position and the x
   position requested */

short FAR PASCAL XMoveTo(lpDevice, x, ms)
LPDEVICE lpDevice;
short x;
short ms;           /* whether to use ms justification or not */
{
        register short curwidth;
        register short charsp;

        if (x < 0 || x > lpDevice->epPageWidth)
                {   /* should never happen */
                return -1;
                }
        if (x < lpDevice->epCurx)
                {
		/*
                 * if (lpDevice->epMode & DRAFTFLAG)
                 *         return 0;
		 */
                myWrite(lpDevice, ESCEXP(escapecode.cr));
                lpDevice->epCurx = 0;
#ifdef SPECIALDEVICECNT
                lpDevice->epXRemain = 0;
#endif
                charsp = x;
                }
        else
                charsp = x - lpDevice->epCurx;

        if (! charsp)
               return 0;

        curwidth = lpDevice->epXcurwidth;

        for (; charsp >= curwidth; charsp -= curwidth)
                myWrite(lpDevice, (LPSTR) EP_BLANK);

        if (ms && charsp)
                {
                /* go into graphics mode to move charsp pixels */

#if SPECIALDEVICECNT
                /* some ibmcomaptibles always move by 2 pixels */
                if ((lpDevice->epMode & WRONG_LINESP) && charsp % 2)
                        {
                        charsp += lpDevice->epXRemain;
                        lpDevice->epXRemain = charsp % 2;
                        /* make charsp even */
                        charsp = (charsp >> 1) << 1;
                        }
                if (charsp)
#endif
                        {
#if defined(NECP2)
                	graph.cnt = charsp;
                	myWrite(lpDevice, (LPSTR)&graph, 4);
                	myWrite(lpDevice, (LPSTR)"\0", 1);
                	charsp = 0;
#else
     #if defined(CITOH)
		        itoa4(charsp,(LPSTR) reset.count_string);
			myWrite(lpDevice, (LPSTR)&reset,8);
     #else
                        reset.cnt = charsp;
                        reset.code = XMS;
                        myWrite(lpDevice, (LPSTR)&reset, 4);
     #endif
			for (; charsp; --charsp)
                                myWrite(lpDevice, (LPSTR)"\0", 1);
#endif
                        }
                }
        lpDevice->epCurx = x - charsp;
        return charsp;
}
#endif

#if defined(CITOH)		/* function needed by citoh */
int
FAR itoa4(n,buff)
			/* special variation of c-standard "itoa()" */
			/* returns atoi() of new string */
int n;
LPSTR buff;

{
	int i, orig;
	char c;
	
	orig = n;
	
	if (n <= 0)	/* can't be negative */
	{
		buff[0] = '0';
		buff[1] = '0';
		buff[2] = '0';
		buff[3] = '0';
		return(0);
	}
	else if (n >= 9999)	/* can't be greater than 9,999 */
	{
		buff[0] = '9';
		buff[1] = '9';
		buff[2] = '9';
		buff[3] = '9';
		return(9999);		
	}
	
/* do integer to ascii conversion */
	
	i = 0;
	do {
		buff[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

/* append more zeros if necessary */	
	for (; i < 4; ++i)
		buff[i] = '0';
	
/* reverse the 4 characters */
	
	c = buff[0];
	buff[0] = buff[3];
	buff[3] = c;

	c = buff[1];
	buff[1] = buff[2];
	buff[2] = c;

	return(orig);	/* return the original value */
}
#endif

#ifdef CITOH	/* special print head wire reversal function for C-Itoh */
near PASCAL bitflip(buf, buflen)
LPSTR buf;
short buflen;
{
#if 0
    register LPSTR bufptr;
    register int i;
    int result;
#endif
    int n,index;

#if 0		/* HERE IS OLD CODE FOR GENERATING BITFLIPS: */
|  /* IT WOULD BE USED THOUSANDS OF TIMES FOR COMPLICATED RASTER PRINTOUTS */
|  /* SO I HAVE MADE A LOOKUP TABLE INSTEAD.   -- MitchL  9/16/87  */
|
|    for (bufptr = buf + buflen; buflen; --buflen)
|        if (~(*--bufptr))	/* inefficient scanning backwards if
|				   using table lookup - unnecessary */
|            break;
|    for (n = 0; n < buflen; n++) {
|        result = 0;
|        for (i = 0; i<8; i++) {
|            if (*(buf+n) & (1<<i))
|                result |= (1 << (7-i));
|         }
|        *(buf+n) = result;	
|    }
#endif
    for (n = 0; n < buflen; n++) 
    {
	    index = *buf;
	    index &= 0xFF;	/* make sure it's unsigned */
	    *buf++ = flip_table[index];
    }
}
#endif
