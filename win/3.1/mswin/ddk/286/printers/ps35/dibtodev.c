/*********************************************************************
 * DIBtoDev.C
 *
 * 17Feb89	chrisg	created
 *
 * things to know:
 *
 *	dib scan lines are DWORD alligned
 *
 *	on non color devices all DIB formats get converted to 8 bit
 *	image operations.
 *
 *	this stuff produces lots of data (bitmap size x 2 or 4!)
 *
 *	we don't support RLE or any biStyle != 0.
 *
 *	DIBs are compresed and then uncompressed using a tiff like
 *	RLE scheme.  (this is different from the DIB RLE encoding).
 *	This can be disabled by undefing the symbol ENCODE. (but
 *	be sure to test things in this state).
 *
 *
 * 17Feb90 
 *	Support for StretchDIB() added. We now translate old DibToDevice()
 *	calls with SETDIBSCALING escape parameters to StretchDIB() calls.
 *	Note that we only implement a small portion of this.
 *	In the future we will add more functionallity including ROPs 
 *	brush support etc.  This is what the unused parameters are for.
 *
 *********************************************************************/

#include "pscript.h"
#include "driver.h"
#include "utils.h"
#include "channel.h"
#include "debug.h"
#include "getdata.h"
#include "resource.h"
#include "psdata.h"

char image_proc[] = "{currentfile bytestr readhexstring pop} bind\n";
char str_def[] = "/bytestr %d string def\n";
char matrix[] = "[%d 0 0 -%d 0 %d]\n";
char restore[] = "restore\n";
char three_vals[] = "%d %d %d\n";

typedef BYTE *PBYTE;

void FAR PASCAL RunEncode(LPDV lpdv, PBYTE lpbytes, int len);

void PASCAL TransScaleStretchDIB(
	LPDV lpdv,
	WORD	DestX,			/* Destination X (on screen) */
	WORD	DestY,			/* Destination Y (on screen) */
	WORD	DestXE,
	WORD	DestYE,
	LPRECT	lpClip
);

int PASCAL GrayToDevice(
	LPDV	lpdv,			/* physical device */
	WORD	DestX,  WORD DestY,
	WORD	DestXE,	WORD DestYE,
	WORD	SrcX, 	WORD SrcY,
	WORD	SrcXE,	WORD SrcYE,
	LPSTR	lpBits,			/* pointer to DIBitmap Bits */
	LPBITMAPINFO lpBitmapInfo,	/* pointer to DIBitmap info Block */
	LPRECT	lpClip
);

int PASCAL ColorToDevice(
	LPDV	lpdv,			/* physical device */
	WORD	DestX,  WORD DestY,
	WORD	DestXE,	WORD DestYE,
	WORD	SrcX, 	WORD SrcY,
	WORD	SrcXE,	WORD SrcYE,
	LPSTR	lpBits,			/* pointer to DIBitmap Bits */
	LPBITMAPINFO lpBitmapInfo,	/* pointer to DIBitmap info Block */
	LPRECT	lpClip
);

int FAR PASCAL StretchDIB(
	LPDV	lpdv,			/* physical device or mem bitmap */
	WORD	wSetGetMode,
	WORD	DestX,  WORD DestY,
	WORD	DestXE,	WORD DestYE,
	WORD	SrcX, 	WORD SrcY,
	WORD	SrcXE,	WORD SrcYE,
	LPSTR	lpBits,			/* pointer to DIBitmap Bits */
	LPBITMAPINFO lpBitmapInfo,	/* pointer to DIBitmap info Block */
	LPSTR	lpConversionInfo,	/* not used */
	DWORD	dwRop,
	LPBR    lpbr,
	LPDRAWMODE lpdm,
	LPRECT	lpClip
);


#define START		0	/* states of the compression state machine */
#define IN_RUN		1	/* building a run of repeating chars */
#define IN_NON_RUN	2	/* building a run of non repeating chars */
#define DONE		3	/* all chars have been processed */
#define TERM		4	/* end of a run or non-run */
#define RUN_LEN		129	/* 129 */
#define NON_RUN_LEN	128	/* 128 */


/***************************************************************************
 * void RunEncode(LPDV lpdv, LPSTR lpbytes, int len)
 *
 * run length encode a string of bytes.  this scheme counts both runs of
 * repeating chars and non repeating chars to reduce overhead in non
 * repeating strings.  so the worst case for this method will add a byte
 * to a string of non repeating chars of NON_RUN_LEN length (128 for bytes
 * producing an expansion of the data by 1/128).  Best case we compress
 * runs of repeating chars of RUN_LEN length (129 for bytes to take
 * 129 chars to 2 (size of output == str_len/128 * 2).
 *
 * in:
 *	lpdv	output PDEVICE
 *	lpbytes	string to encode
 *	len	length of string referenced by lpbytes
 *
 * out:
 *	data compressed string goes to output device referenced by lpdv
 *
 * returns:
 *	nothing
 *
 * future mods:
 *	add compile time switch to make this return the effective
 *	compression for use in benchmarks.
 *
 **************************************************************************/


void FAR PASCAL RunEncode(LPDV lpdv, PBYTE lpbytes, int len)
{
	PBYTE lpfirst;
	PBYTE lpchar;
	/* BOOL in_run; */
	register int count;
	register int state;

	lpfirst = lpbytes;
	state = START;

	for (;;) {		/* do this until we are DONE */

	switch (state) {

	case IN_RUN:
		if ((lpchar + 1) >= (lpbytes + len)) {	/* off the end? */
			state = DONE;
			break;
		}

		if (*lpchar != *(lpchar + 1)) {
			state = TERM;
			break;
		}

		lpchar++;

		count++;

		if (count >= RUN_LEN)
			state = TERM;
		break;

	case IN_NON_RUN:
		if ((lpchar + 1) >= (lpbytes + len)) {	/* off the end? */
			state = DONE;
			break;
		}

		if (*lpchar == *(lpchar + 1)) {
			lpchar--;	/* back up to give */
			count--;	/* upcomming run more */
			state = TERM;
			break;
		} else

		lpchar++;
		count++;

		if (count >= NON_RUN_LEN)
			state = TERM;
		break;


	case START:

		/* special case, one char (last in string) */

		if ((lpfirst + 1) >= (lpbytes + len)) {
			count = 1;		/* minimal case */
			state = DONE;
		} else {

			lpchar = lpfirst + 1;	/* not over end */

			/* in_run = FALSE; */
			count = 2;		/* start with strings of 2 */

			if (*lpchar == *lpfirst)
				state = IN_RUN;
			else
				state = IN_NON_RUN;
		}
		break;

	case TERM:
	case DONE:

		/* is it the minimal case of a single char or
		 * a non run */

/*		if ((count < 3) || (*lpchar != *(lpchar - 1))) */

		if ((count < 2) || (*lpchar != *(lpchar - 1))) {

			/* this is a non run (non repeating chars) */
			/* output value in rage of 0 - 127 -> 1 - 128 */

			DBMSG1(("non run count:%d %02x\n", count, (BYTE)(count - 1)));
			PrintChannel(lpdv, hex, (BYTE)(count - 1));	/* the count */

			ASSERT(count);

			while (count--) {				/* the values */
				PrintChannel(lpdv, hex, (BYTE)*lpfirst++);
			}

		} else {

			/* we just came from a run */
			/* output value in range 128 - 255 -> 3 - 130 */

			DBMSG1(("run count:%d %02x char:%02x\n", count, 
				(BYTE)(~count + 3), (BYTE)*lpchar));

/*			PrintChannel(lpdv, hex, (BYTE)(~count + 3));	 the count */
			PrintChannel(lpdv, hex, (BYTE)(~count + 2));	/* the count */
			PrintChannel(lpdv, hex, (BYTE)*lpchar);	/* the value */
		}

		if (state == DONE) {
			return;
		} else {
			lpfirst = lpchar + 1;

			if (lpfirst >= (lpbytes + len))
				return;

			state = START;
		}
		break;
	}
	}
}

int FAR PASCAL StretchDIB(
	LPDV	lpdv,			/* physical device */
	WORD	wSetGetMode,
	WORD	DestX,  WORD DestY,
	WORD	DestXE,	WORD DestYE,
	WORD	SrcX, 	WORD SrcY,
	WORD	SrcXE,	WORD SrcYE,
	LPSTR	lpBits,			/* pointer to DIBitmap Bits */
	LPBITMAPINFO lpBitmapInfo,	/* pointer to DIBitmap info Block */
	LPSTR	lpConversionInfo,	/* not used */
	DWORD	dwRop,
	LPBR    lpbr,
	LPDRAWMODE lpdm,
	LPRECT	lpClip
)
{
	int num;

	DBMSG(("StretchDIB() DestX %d DestY %d Style %d\n", 
		DestX, DestY, lpBitmapInfo->bmiHeader.biStyle));

	if (!lpdv->iType)	// dest memory, GDI simulate
		return -1;

#ifdef PS_IGNORE
	// must do this after we are sure this is not going to memory
	// or else we may fault looking into something that is not our
	// pdevice
	if (lpdv->fSupressOutput)
		return 1;
#endif

	// test for all cases that we want GDI to simulate

	if (wSetGetMode != 0 ||			    	// GET operation
	    lpBitmapInfo->bmiHeader.biStyle != 0 ||	// RLE format
//	    lpBitmapInfo->bmiHeader.biBitCount == 1 ||	// 1 bit case
	    dwRop != SRCCOPY)				// no ROPs
		return -1;


	if (lpdv->fColor)
		num = ColorToDevice(lpdv, 
			DestX, DestY, 
			DestXE, DestYE,
			SrcX, SrcY,
			SrcXE, SrcYE,
			lpBits, lpBitmapInfo, 
			lpClip);
	else
		num = GrayToDevice(lpdv, 
			DestX, DestY, 
			DestXE, DestYE,
			SrcX, SrcY,
			SrcXE, SrcYE,
			lpBits, lpBitmapInfo, 
			lpClip);

	PrintChannel(lpdv, restore);	// match "save" in TransScaleDIB

	ClipBox(lpdv, NULL);		// match in TransScaleDIB

	return num;
}





/*
 * do either Color or Gray DIB
 */

int FAR PASCAL DIBToDevice(
	LPDV	lpdv,			/* physical device */
	WORD	DestX,			/* Destination X (on screen) */
	WORD	DestY,			/* Destination Y (on screen) */
	WORD	nStart,
	WORD	nNumScans,		/* # of scan lines to do */
	LPRECT	lpClip,
	LPDRAWMODE lpdm,
	LPSTR	lpBits,			/* pointer to DIBitmap Bits */
	LPBITMAPINFO lpBitmapInfo,	/* pointer to DIBitmap info Block */
	LPSTR	lpConversionInfo	/* not used */
)
{
	DWORD realHeight;
	RECT rc;
	int res;
	WORD xScale, yScale;

	if (lpdv->ScaleMode == 2) {
		// scale to lpdv->dx by lpdv->dy
		// these values are the size of the final image
		xScale = lpdv->dx;
		yScale = lpdv->dy;
	} else {
		// ScaleMode 0 or 1
		xScale = (int)lpBitmapInfo->bmiHeader.biWidth;
		yScale = (int)lpBitmapInfo->bmiHeader.biHeight;
	}
		

	realHeight = lpBitmapInfo->bmiHeader.biHeight;
	lpBitmapInfo->bmiHeader.biHeight = nNumScans;	// patch up the height

	rc.left = DestX;
	rc.top = DestY + Scale(((WORD)realHeight - nStart - nNumScans), yScale, (WORD)realHeight);
	rc.right = rc.left + xScale;
	rc.bottom = rc.top + Scale(nNumScans, yScale, (WORD)realHeight);


	res = StretchDIB(lpdv, 0,
		rc.left, rc.top,
		xScale, Scale(nNumScans, yScale, (WORD)realHeight),
		0, 0, (WORD)lpBitmapInfo->bmiHeader.biWidth, nNumScans,
		lpBits,	lpBitmapInfo, lpConversionInfo, SRCCOPY, NULL, lpdm, &rc);

	lpBitmapInfo->bmiHeader.biHeight = realHeight;	// restore the patched height

	return res;
}



/****************************************************************************
 * GrayToDevice()
 *
 * this routine generates postscript that will print DIB in gray scale
 * using the postscript image operator.  to ease the bit manipulation we
 * convert everything into 8 bit image operations.
 * since we compress that data this is not too big of a hit.
 *
 * supports:
 *	4, 8, 24 bit formats.  NOTE: no RLE formats or 1 bit dibs
 *
 * mods:
 *	postscript RLE complression added.
 *
 ****************************************************************************/

int PASCAL GrayToDevice(
	LPDV	lpdv,			/* physical device */
	WORD	DestX,  WORD DestY,
	WORD	DestXE,	WORD DestYE,
	WORD	SrcX, 	WORD SrcY,
	WORD	SrcXE,	WORD SrcYE,
	LPSTR	lpBits,			/* pointer to DIBitmap Bits */
	LPBITMAPINFO lpBitmapInfo,	/* pointer to DIBitmap info Block */
	LPRECT	lpClip
)
{
	BYTE huge *lpbits;		/* let the compiler do seg stuff */
	BYTE huge *lplinestart;
	BYTE byte;
	WORD i, j, k;
	int bit_count;
	int scan_width;
	WORD table_size;
	BYTE gray_table[256];
	LPRGBQUAD lppal;
	HANDLE	hBuf;
	BYTE	*pBuf;
        BOOL    bEncode;

	DBMSG(("GrayToDevice() dest:%d %d %d %d src:%d %d %d %d\n",
		DestX, DestY, DestXE, DestYE, SrcX, SrcY, SrcXE, SrcYE));

	bit_count = lpBitmapInfo->bmiHeader.biBitCount;

        /* determine if bitmap should be encoded */
        bEncode = lpdv->bCompress;

	DBMSG(("GrayToDevice() bitcount %d\n", bit_count));

	// for 4 and 8 we build a translate table from the dib color table.

	if (bit_count != 24) 
    {
        if(!(table_size = lpBitmapInfo->bmiHeader.biClrUsed))
    		table_size = (1 << bit_count);

		lppal = (LPRGBQUAD)((LPSTR)lpBitmapInfo + lpBitmapInfo->bmiHeader.biSize);

		DBMSG(("GrayToDevice(): table_size:%d\n", table_size));

		/* here we build a gray scale palette from the RGB palette.
	 	 * the values used here are from Foley Van Dam p. 613 */

		for (i = 0; i < table_size; i++) {
			gray_table[i] = INTENSITY(lppal[i].rgbRed,
					          lppal[i].rgbGreen,
					          lppal[i].rgbBlue);

			DBMSG1(("%02x %02x %02x -> %02x\n",
				lppal[i].rgbRed,
				lppal[i].rgbGreen,
				lppal[i].rgbBlue,
				gray_table[i]));
		}
	}

	// SrcXE is the size of the string buffer (decode buffer)
	// (we convert 4 -> 8 and 24 -> 8 so this is always SrcXE

	PrintChannel(lpdv, str_def, SrcXE);

	if (!(hBuf = LocalAlloc(LPTR, SrcXE + 2)))	// pad slightly
		goto ERR_EXIT;

	if (!(pBuf = LocalLock(hBuf))) {
		LocalFree(hBuf);
		goto ERR_EXIT;
	}

        if (bEncode)
	    DumpResourceString(lpdv, PS_DATA, PS_UNPACK);

	TransScaleStretchDIB(lpdv, DestX, DestY, DestXE, DestYE, lpClip);

	PrintChannel(lpdv, three_vals, SrcXE, SrcYE, 8);

	/* this matrix maps user space (after xlate and scale) into
	 * the image space and does the image flipping */

	PrintChannel(lpdv, matrix, SrcXE, SrcYE, SrcYE);

        if (bEncode)
	    PrintChannel(lpdv, "{unpack} bind\n");
        else
	    PrintChannel(lpdv, image_proc);
	PrintChannel(lpdv, "image\n");

	// scan_width is the width of one scan line.
	// DIB scanlines are on DWORD boundaries so this needs to be 
	// rounded up

	scan_width = (((WORD)lpBitmapInfo->bmiHeader.biWidth * bit_count + 7) / 8 + 3) & 0xFFFC;

	// offset to start scan

	lplinestart = lpBits + scan_width * SrcY;	

	// and offset to starting pixel

	switch (bit_count) {
        case 1:
                lplinestart += SrcX / 8;
                break;

	case 4:
		lplinestart += SrcX / 2;
		break;

	case 8:
		lplinestart += SrcX;
		break;

	case 24:
		lplinestart += (WORD)lmul((LONG)SrcX, 3L);
		break;
	}


    for (i = 0; i < SrcYE; i++) 
    {

        lpbits = lplinestart;

        DBMSG1(("Line %d\n", i));

        if (bit_count == 1) 
        {
            WORD  pixels = 0;

            for (j = 0; j < (SrcXE + 7) / 8; j++) 
            {
                byte = *lpbits++;
                for (k = 0; k < 8; ++k) 
                {
                    if(pixels++ >= SrcXE)
                        break;

                    if (bEncode)
                        pBuf[j*8+k] = gray_table[(byte >> (7-k)) & 1];
                    else
                        PrintChannel(lpdv, hex,
                                gray_table[(byte >> (7-k)) & 1]);
                }
            }
        } 
        else if (bit_count == 4) 
        {
            WORD  pixels = 1;

            for (j = 0; j < (SrcXE + 1) / 2; j++, pixels += 2) 
            {
                byte = *lpbits++;
                if (bEncode) 
                {
                    pBuf[j*2] = gray_table[byte >> 4];

                    if(pixels >= SrcXE)
                        break;
                
                    pBuf[j*2+1] = gray_table[byte & 0x0F];
                }
                else 
                {
                    PrintChannel(lpdv, "%02x", gray_table[byte >> 4]);

                    if(pixels >= SrcXE)
                        break;
                
                    PrintChannel(lpdv, "%02x", gray_table[byte & 0x0F]);
                }
            }

        }
        else if (bit_count == 8) 
        {
            for (j = 0; j < SrcXE; j++) 
            {
                byte = *lpbits++;
                if (bEncode)
                    pBuf[j] = gray_table[byte];
                else
                    PrintChannel(lpdv, hex, gray_table[byte]);
            }
        } 
        else if (bit_count == 24) 
        {
            for (j = 0; j < SrcXE; j++) 
            {
                byte = INTENSITY(((LPRGBTRIPLE)lpbits)->rgbtRed,
                             ((LPRGBTRIPLE)lpbits)->rgbtGreen,
                             ((LPRGBTRIPLE)lpbits)->rgbtBlue);
                if (bEncode) 
                    pBuf[j] = byte;
                else
                    PrintChannel(lpdv, hex, byte);
                lpbits += sizeof (RGBTRIPLE);
            }
        }

        if (bEncode)
            RunEncode(lpdv, pBuf, SrcXE);

        if (lpdv->fh < 0)
            goto DIB_DONE;    // user abort

        PrintChannel(lpdv, newline);
        lplinestart += scan_width;
    }


DIB_DONE:
    LocalUnlock(hBuf);
    LocalFree(hBuf);

    return SrcYE;
ERR_EXIT:
    DBMSG(("local alloc failed\n"));
    return 0;
}



/****************************************************************************
 * ColorToDevice()
 *
 * NOTE: this only handles 4, 8 or 24 bit dibs.  if it is 1 bit
 * use GrayToDevice.
 *
 * method:
 *    we download the color table in the form of an array and then
 *    send pixel values down (compressed).  The values are used to
 *    look up into the table to produce a 24 bit RGB.  these
 *    RGBs are used with a single procedure colorimage operator.
 *    
 *
 * supports:
 *    4, 8, 24 bit formats.  NOTE, no RLE formats
 *
 * 4 bit dibs are now expanded to 8 to avoid the compelity of having
 * to decompress non byte alligned pixels.  (also, the read4 routine
 * was complex and slow).
 *
 ***************************************************************************/

int PASCAL ColorToDevice(
    LPDV    lpdv,            /* physical device */
    WORD    DestX,  WORD DestY,
    WORD    DestXE,    WORD DestYE,
    WORD    SrcX,     WORD SrcY,
    WORD    SrcXE,    WORD SrcYE,
    LPSTR    lpBits,            /* pointer to DIBitmap Bits */
    LPBITMAPINFO lpBitmapInfo,    /* pointer to DIBitmap info Block */
    LPRECT    lpClip
)
{
    BYTE huge *lpbits;
    BYTE huge *lplinestart;
    LPRGBQUAD lppal;
    BYTE byte;
    int i, j, k;
    int pal_size;
    int scan_width;
    int bit_count = lpBitmapInfo->bmiHeader.biBitCount;
    HANDLE    hBuf;
    BYTE    *pBuf;
        BOOL    bEncode;
        WORD    w;

    DBMSG(("ColorToDevice()\n"));

        /* Determine if bitmap should be encoded */
        bEncode = lpdv->bCompress;


    PrintChannel(lpdv, "/rgbstr %d string def\n", SrcXE * 3);

    if (bit_count != 24) {

        /* if biNumColors is zero use 2**biBitCount as size */

        if ((pal_size = (int)lpBitmapInfo->bmiHeader.biClrUsed) == 0)
            pal_size = (1 << bit_count);

        lppal = (LPRGBQUAD)((LPSTR)lpBitmapInfo + lpBitmapInfo->bmiHeader.biSize);


                /* Bracket palette with %%BeginData/%%EndData comments */
                PrintChannel(lpdv, "%%%%BeginData: %d ASCII Lines\n",
                             2 + pal_size / 20);

        /* keck up the palette */

        PrintChannel(lpdv, "/pal <\n");
        for (i = 0; i < pal_size; i++ ) {

            PrintChannel(lpdv, "%02x%02x%02x",
                lppal[i].rgbRed,
                lppal[i].rgbGreen,
                lppal[i].rgbBlue);

            if ((i % 21) == 20)
                PrintChannel(lpdv, newline);
        }
        PrintChannel(lpdv, "> def\n");

                PrintChannel(lpdv, "%%%%EndData\n");

        // for 4 or 8 bits we need a temp string that holds the pixel
         // values.  these values are extracted and used to look up in
         // the color table.

        PrintChannel(lpdv, str_def, SrcXE);

        // local buffer for non 24 bit dibs

        if (!(hBuf = LocalAlloc(LPTR, SrcXE+2)))
            goto ERR_EXIT;

        if (!(pBuf = LocalLock(hBuf))) {
            LocalFree(hBuf);
            goto ERR_EXIT;
        }
    }

        if (bEncode)
        DumpResourceString(lpdv, PS_DATA, PS_UNPACK);

    DumpResourceString(lpdv, PS_DATA, PS_CIMAGE);

    TransScaleStretchDIB(lpdv, DestX, DestY, DestXE, DestYE, lpClip);

    PrintChannel(lpdv, three_vals, SrcXE, SrcYE, 8);
    PrintChannel(lpdv, matrix, SrcXE, SrcYE, SrcYE);

        /* Bracket image with %%BeginData / %%EndData pair */
        PrintChannel(lpdv, "%%%%BeginData: %d ASCII Lines\n", 1 + SrcYE);

    if (bit_count == 24) 
        PrintChannel(lpdv, "{read24} false 3 colorimage\n");
    else
        PrintChannel(lpdv, "{read8} false 3 colorimage\n");

    /* DIB scanlines are on DWORD boundaries */

    scan_width = (((WORD)lpBitmapInfo->bmiHeader.biWidth * bit_count + 7) / 8 + 3) & 0xFFFC;

    lplinestart = lpBits;
    
    lplinestart += scan_width * SrcY;

    switch (bit_count) {
        case 1:
                lplinestart += SrcX / 8;
                break;

    case 4:
        // we need to adjust this to get those non byte alligned pixels
        lplinestart += SrcX / 2;
        break;

    case 8:
        lplinestart += SrcX;
        break;

    case 24:
        lplinestart += (WORD)lmul((LONG)SrcX, 3L);
        break;
    }

    for (i = 0; i < (int)SrcYE; i++) {
        lpbits = lplinestart;

            if (bEncode) {
        if (bit_count == 24) {

            for (j = 0; j < (int)SrcXE ; j++) {
                PrintChannel(lpdv, hex, ((LPRGBTRIPLE)lpbits)->rgbtRed);
                PrintChannel(lpdv, hex, ((LPRGBTRIPLE)lpbits)->rgbtGreen);
                PrintChannel(lpdv, hex, ((LPRGBTRIPLE)lpbits)->rgbtBlue);
                lpbits += 3;  // next BGR triple!
            }

        } else if (bit_count == 8) {

            for (j = 0; j < (int)SrcXE; j++)
                pBuf[j] = *lpbits++;

            RunEncode(lpdv, pBuf, SrcXE);

        } else if (bit_count == 4) {

            for (j = 0; j < ((int)SrcXE + 1) / 2; j++) {
                byte = *lpbits++;

                pBuf[j*2] = (BYTE)(byte >> 4);
                pBuf[j*2+1] = (BYTE)(byte & 0x0F);
            }

            RunEncode(lpdv, pBuf, SrcXE);
        } else if (bit_count == 1) {
            for (j = 0; j < ((int)SrcXE + 1) / 8; j++) {
                byte = *lpbits++;
                                for (k = 0; k < 8; ++k) {
                        pBuf[j*8+k] = (BYTE)((byte >> (7-k)) & 1);
                                }
            }
            RunEncode(lpdv, pBuf, SrcXE);
                }
            } else {    /* Not bEncodeD */
        // only works for 8?

        for (w = 0; w < SrcXE; w++) {
            PrintChannel(lpdv, hex, *lpbits++);
        }
            }

        if (lpdv->fh < 0)
            goto DIB_DONE;    // user abort

        PrintChannel(lpdv, newline);
        lplinestart += scan_width;
    }

DIB_DONE:
        PrintChannel(lpdv, "%%%%EndData\n");

        if (bEncode) {
        if (bit_count != 24) {
            LocalUnlock(hBuf);
            LocalFree(hBuf);
        }
        }

    return SrcYE;
ERR_EXIT:
    DBMSG(("local alloc failed\n"));
    return 0;
}



/**************************************************************************
 * void PASCAL TransScaleStretchDIB()
 *
 * clip, translate and scale.  sets up coordinate space to for dib
 * stretching.
 *
 **************************************************************************/

void PASCAL TransScaleStretchDIB(
    LPDV lpdv,
    WORD    DestX,            /* Destination X (on screen) */
    WORD    DestY,            /* Destination Y (on screen) */
    WORD    DestXE,
    WORD    DestYE,
    LPRECT    lpClip
)
{
    ClipBox(lpdv, lpClip);

    PrintChannel(lpdv, "save %d %d translate %d %d scale\n", DestX, DestY, DestXE, DestYE);
}



