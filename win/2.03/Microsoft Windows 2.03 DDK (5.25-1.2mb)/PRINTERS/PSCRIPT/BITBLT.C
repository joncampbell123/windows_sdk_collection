#define WIN20


#include "pscript.h"
#define NEWCODE
#define SRCCOPY     (DWORD)0x00CC0020  /* dest=source                       */
void PASCAL PrintRun(LPDV, int, int, LPSTR);
void PASCAL EncodeBuf(LPDV, LPSTR, int, int, int);
int PASCAL PatLength(LPSTR, int, int);
int PASCAL RunLength(LPSTR, int, int);
void FAR PASCAL SelectBrush(DV FAR *, BR FAR *);
#define WHITE 0x0ff	/* The value of a white byte in a bitmap */

/* The StretchBlt parameter structure
 */
typedef struct
	{
	WORD ixDst;					/* The destination's horizontal origion */
	WORD iyDst;					/* The destination's vertical origion */
	WORD cxDst;					/* The destination rectangle width */
	WORD cyDst;					/* The destination rectangle height */
	BITMAP FAR *lpbm;			/* The source bitmap */
	WORD ixSrc;					/* The source offset */
	WORD iySrc;					/* The destination offset */
	WORD cxSrc;					/* The source rectangle width */
	WORD cySrc;					/* The source rectangle height */
	DWORD rop;					/* The raster operation code */
	DWORD lpdm;
	BR FAR *lpbr;
	} SBLT;
typedef SBLT FAR *LPSBLT;
void FAR PASCAL StretchBlt(DV FAR *, LPSBLT);

#ifdef UNDEFINED

/* ========================= BEGINNING OF PURGED CODE ========
 */

/*****************************************************
 *	  Name: PatLength()
 *
 *	  Action: Compute the length of the smallest substring
 *		  that can be replicated to form the longest
 *		  run starting at the current position in the
 *		  buffer.
 *
 *******************************************************
 */
int PASCAL PatLength(lpbBuf, cbBuf, cbPat)
	LPSTR lpbBuf;
	int cbBuf;
	int cbPat;
	{
	int cbLim;
	int cbFactor;
	int i;
	ASSERT(lpbBuf != NULL);

	/* Make the pattern window no larger than half the buffer size
	 */
	cbBuf /= 2;

	while (cbPat > cbBuf)
		--cbPat;

	/* Find the longest pattern that is replicated in the pattern window
	 */
	while (cbPat > 0 && !lrgbIsEqual(lpbBuf, lpbBuf + cbPat, cbPat))
		--cbPat;

	/* Factor the pattern into its least common divisor
	 */
	for (cbFactor = 1; cbFactor < cbPat; ++cbFactor)
		{
		cbLim = cbPat - cbFactor;

		for (i = 0; i <= cbLim; i += cbFactor)
			if (!lrgbIsEqual(lpbBuf, lpbBuf + i, cbFactor))
				break;

		if (i == cbPat)
			break;
		}

	return (cbFactor);
	}

/***************************************************************
 *	  Name: RunLength()
 *
 *	  Action: Compute the number of contiguous repetitions of a pattern
 *		  contained in the buffer.  The pattern is in the first N bytes
 *		  of the buffer.
 *
 *****************************************************************
 */
int PASCAL RunLength(lpbBuf, cbBuf, cbPat)
	LPSTR lpbBuf;				/* Pointer to the buffer */
	int cbBuf;					/* The buffer length */
	int cbPat;					/* The pattern length */
	{
	LPSTR lpbPat;				/* Pointer to the pattern */
	int cRepeat;			/* The number of times the pattern is repeated*/
	ASSERT(lpbBuf != NULL);
	lpbPat = lpbBuf;
	cRepeat = 0;

	while (cbBuf >= cbPat)
		{
		/* Stop when the pattern fails to match
		 */
		if (!lrgbIsEqual(lpbBuf, lpbPat, cbPat))
			break;

		/* Move over in the buffer by the pattern length
		 */
		cbBuf -= cbPat;
		lpbBuf += cbPat;
		++cRepeat;
		}

	return (cRepeat);
	}

/***********************************************************
 *	  Name: PrintRun()
 *
 *	  Action: Print a specified number of repetitions of a given
 *		  pattern.
 *
 *	  Note: The output is given as hexadecimal digits and consists
 *		of a two byte "repetition count" followed by a one byte
 *		"pattern length" followed by the pattern.
 *		When there is only one occurance of the pattern, the
 *		repetition count field is used for the length because
 *		of its extended range.	This special case is marked by
 *		outputting the pattern length field as zero.
 *
 ****************************************************************
 */
void PASCAL PrintRun(lpdv, cRun, cbPat, lpbPat)
	DV FAR *lpdv;				/* Far ptr to the device descriptor */
	int cRun;					/* The number of run repetitions */
	int cbPat;					/* The length of the pattern to repeat */
	LPSTR lpbPat;				/* A far ptr to the pattern */
	{
	int i;
	int i1;
	int i2;
	ASSERT(lpdv != NULL);
	ASSERT(lpbPat != NULL);

	if (cRun > 0)	/* For a repeated pattern, use 16 bits for run count */
		{
		i1 = cRun;
		i2 = cbPat;
		}
	else
		{
		/* For a non-repeated pattern, use 16 bits for bytecount
		 */
		i1 = cbPat;
		i2 = 0;
		}

	PrintChannel(lpdv, (LPSTR)"%02x%02x%02x", i1 >> 8, i1, i2);

	/* Print the pattern as 32 hex digits per line
	 */
	while (cbPat > 0)
		{
		for (i = 0; i < 32 && cbPat > 0; ++i)
			{
			PrintChannel(lpdv, (LPSTR)"%02x", *lpbPat++);
			--cbPat;
			}

		PrintChannel(lpdv, (LPSTR)"\n");
		}
	}

/********************************************************************
 *	  Name: ScanToChannel()
 *
 *	  Action: Run length encode the buffer and output the encoded
 *		  buffer as ASCII hexadecimal bytes so that they can
 *		  be read in using the "readhexstring" function in
 *		  PostScript.  The PostScript function "ExpandImage"
 *		  is executed by the "Image" operator to reconstruct
 *		  the bits.
 *
 *
 *	  Note: The algorithm used here is smart enough to recognize
 *		runs of extended patterns instead of the usual single
 *		byte run length encoding scheme. This is so that pattern
 *		bitmaps that have been stretched from 72 dpi to 300 dpi
 *		will be recognized, allowing filled areas to be compressed.
 *
 *		The pattern matching algorithm has an exponential execution
 *		time so the pattern window should be kept relatively small
 *		(1 to 16 bytes).
 *
 **********************************************************************
 */
void PASCAL ScanToChannel(lpdv, lpbSrc, cbSrc, cbPatMax, cbRunMin)
	DV FAR *lpdv;				/* A far ptr to the device descriptor */
	LPSTR lpbSrc;				/* Pointer to the source buffer */
	int cbSrc;					/* The source buffer size */
	int cbPatMax;				/* The size of the pattern window */
	int cbRunMin;				/* The minimum length of a run */
	{
	int cbPat;					/* The pattern length */
	int cRun;			/* The number of runs (repetitions) of the pattern*/
	int cbRun;					/* The run length in bytes */
	int cbPrefix;		/* The number of non-repeating bytes before pattern*/
	LPSTR lpbPrefix;			/* Ptr to the beginning of the prefix */
	ASSERT(lpdv != NULL);
	ASSERT(lpbSrc != NULL);
	lpbPrefix = lpbSrc;
	cbPrefix = 0;

	while (cbSrc > 0)
		{
		/* Compute the run and pattern lengths
		 */
		cbPat = PatLength(lpbSrc, cbSrc, cbPatMax);
		cRun = RunLength(lpbSrc, cbSrc, cbPat);

		if (cRun == 1)
			cbRun = cbPat;
		else
			cbRun = cRun * cbPat;

		/* Encode only "runs" of a reasonable length
		 */
		if (cbRun < cbRunMin)
			cbPrefix += cbRun;
		else
			{
			if (cbPrefix)
				{
				PrintRun(lpdv, 1, cbPrefix, lpbPrefix);
				cbPrefix = 0;
				}

			PrintRun(lpdv, cRun, cbPat, lpbSrc);
			lpbPrefix = lpbSrc + cbRun;
			}

		lpbSrc += cbRun;
		cbSrc -= cbRun;
		}

	if (cbPrefix)
		PrintRun(lpdv, 1, cbPrefix, lpbPrefix);
	}

/*********************************************************
 *	  Name: PrintScan()
 *
 *	  Action: Send a horizontal scan line (1 pixel high) from
 *		  a bitmap to the printer.
 *
 *
 *	  Note: It is important to keep the output file size to a
 *		minimum because it takes approximately 20 minutes per
 *		megabyte to transmit data at 9600 baud.  To accomplish
 *		this, the white space is trimmed from both ends of the
 *		scan line, then the remainder is compressed by
 *		using a smart run length encoding algorithm that detects
 *		repeated runs of patterns up to 8 bytes long.
 *		Thus, even pattern filled polygons can be compressed.
 *
 ************************************************************
 */
void PASCAL PrintScan(lpdv, ix, iy, cx, lpb)
	LPDV lpdv;					/* Far ptr to the device descriptor */
	int ix;						/* The X coordinate */
	int iy;						/* The Y coordinate */
	int cx;						/* The width of the scan in bits */
	LPSTR lpb;					/* Far ptr to the scan buffer */
	{
	static char rgbMask[8] = {
		0, 0x7f, 0x3f, 0x1f, 0x0f, 7, 3, 1 };
	char bSave;				/* A variable to save the last byte in the scan */
	LPSTR lpbSave;				/* A pointer to the saved byte's location */
	int cb;
	LPSTR lpbFirst;
	LPSTR lpbLast;
	ASSERT(lpdv != NULL);
	lpbFirst = lpb;
	lpbLast = lpb + ((((cx + 7) >> 3) & 0x1fff) - 1);

	/* Make the bits just past the right edge white
	 */
	lpbSave = lpbLast;
	bSave = *lpbLast;
	*lpbLast |= rgbMask[cx & 0x07];

	/* Find the first non-white byte in the scan
	 */
	while ((lpbFirst <= lpbLast) && ((*lpbFirst & 0x0ff) == WHITE))
		++lpbFirst;

	/* Find the last non-white byte in the scan
	 */
	while ((lpbLast >= lpbFirst) && ((*lpbLast & 0x0ff) == WHITE))
		--lpbLast;

	/* If the scan is not empty, then compress it and send it to the printer
	 */
	if (lpbFirst <= lpbLast)
		{
		ix += (lpbFirst - lpb) * 8;
		cx = ((lpbLast - lpbFirst) + 1) * 8;
		PrintChannel(lpdv, (LPSTR)"%d %d %d Scan\n", ix, iy, cx);
		ScanToChannel(lpdv, lpbFirst, (cx + 7) / 8, 8, 8);
		}

	*lpbSave = bSave;
	}

/*================ END OF PURGED CODE  ================
 */
#endif

/*	c1to2 - convert 1-byte "binary" data into 2-byte "hex" data
 *
 *	the current implementation assumes ASCII.
 */
static void PASCAL c1to2(n, s, d)
	register int n;				/* number of bytes in the source array */
	register LPSTR s;			/* source array */
	register LPSTR d;			/* destination array */
	{
	register char t;
	register char u;

	while (--n >= 0)
		{
		u = ((t = *s++) >> 4) & 0xf;

		if (u < 10)
			*d++ = u + '0';
		else
			*d++ = u - 10 + 'A';

		u = t & 0xf;

		if (u < 10)
			*d++ = u + '0';
		else
			*d++ = u - 10 + 'A';
		}
	}

/* do a band bitblt
 *
 * Note -- seems like we only want to trim if white is see-through.  I wonder
 * if SRCCOPY works at all any more...(87-1-20 sec)
 */
void PASCAL BandOut(lpdv, lpbm, ix, iy, cx, cy, lpHex)
	LPDV lpdv;
	BITMAP FAR *lpbm;
	int ix;
	int iy;
	int cx;
	int cy;
	LPSTR	lpHex;
	{
	int cbx;
	unsigned char far *lpb;
	unsigned char far *lpbBand;
	int ixLeft;
	int ixRight;
	int i, j;
	int outbytes;

	/* shorthands:
	 */
	cbx = lpbm->bmWidthBytes;
	lpbBand = lpbm->bmBits;

	/* Trim white space from the top of the bitmap
	 * cy,iy are adjusted.
	 */
	while (cy > 0)
		{
		lpb = lpbBand;

		for (i = 0; i < cbx; ++i)
			if (*lpb++ != WHITE)
				goto TRIMBOTTOM;

		lpbBand += cbx;
		--cy;
		++iy;
		}

	/* Trim white space from the bottom of the bitmap
	 * cy is adjusted.
	 */
TRIMBOTTOM:
	while (cy > 0)
		{
		lpb = lpbBand + (cy - 1) * cbx;

		for (i = 0; i < cbx; ++i)
			if (*lpb++ != WHITE)
				goto TRIMLEFT;

		--cy;
		}

	/* trim on the left, to the byte level.  ixLeft is computed.
	 * note that we are marching down columns rather than across
	 * rows...
	 */
TRIMLEFT:
	for (ixLeft = 0; ixLeft < cbx; ++ixLeft)
		{
		lpb = lpbm->bmBits + ixLeft;

		for (j = 0; j < cy; ++j)
			{
			if (*lpb != WHITE)
				goto TRIMRIGHT;

			lpb += cbx;
			}
		}

	/* trim on the right, to the byte level.  ixRight is computed such
	 * that ixRight - ixLeft is the byte count.
	 */
TRIMRIGHT:
	for (ixRight = cbx; ixRight > ixLeft; --ixRight)
		{
		lpb = lpbm->bmBits + (ixRight - 1);

		for (j = 0; j < cy; ++j)
			{
			if (*lpb != WHITE)
				goto TRIMDONE;

			lpb += cbx;
			}
		}

/* adjust things...I believe that ix is assumed to be a multiple of eight,
 * which is true from PageMaker, I believe, but may not be universally
 * true.  This whole trimming thing needs to be re-done, I think.
 */
TRIMDONE:
	if (ixLeft > 0)
		{
		cx -= ixLeft << 3;
		ix += ixLeft << 3;
		}

	if (ixRight < cbx)
		{
#if 0
		cx -= cx & 7;
		cx -= (cbx - ixRight) << 3;
#endif
		cx = (ixRight - ixLeft) << 3;
		}

	if (cx <= 0)
		return;

	lpb = lpbBand + ixLeft;
	outbytes = ixRight - ixLeft;
	PrintChannel(lpdv, (LPSTR)"%d %d %d %d BitBlt\n", ix, iy, cx, cy);

	while (--cy >= 0)
		{
#if 0
		for (i = 0; i < outbytes; ++i)
			{
			PrintChannel(lpdv, (LPSTR)"%02x", *(lpb + i) & 0x0ff);

			if (i % 32 == 0 && i > 0)
				PrintChannel(lpdv, (LPSTR)"\n ");
			}
#endif
		c1to2(outbytes, lpb, lpHex);
		WriteChannel(lpdv, lpHex, outbytes * 2);
		PrintChannel(lpdv, (LPSTR)"\n");
		lpb += cbx;
		}
	}

void PASCAL ShapeBitmap(lpbm, cx, cy)
	BITMAP FAR *lpbm;
	int cx;
	int cy;
	{
	lpbm->bmWidthBytes = (cx + 7) / 8;

	if (lpbm->bmWidthBytes > CBBANDMAX)
		lpbm->bmWidthBytes = CBBANDMAX;

	lpbm->bmWidth = lpbm->bmWidthBytes * 8;
	lpbm->bmHeight = CBBANDMAX / lpbm->bmWidthBytes;

	if (lpbm->bmHeight > cy)
		lpbm->bmHeight = cy;

	lpbm->bmWidthPlanes = lpbm->bmWidthBytes * lpbm->bmHeight;
	}

/***************************************************************
 *	  Name: BitBlt()
 *
 *	  Action: This routine transfers a rectangular region of a
 *		  source bitmap to the display surface.  The bits
 *		  in the source, destination, and brush pattern are
 *		  combines as specified by the raster operation code
 *		  in the drawing mode structure.
 *
 *	  Returns: TRUE = Success
 *		   FALSE = Failure
 *
 ****************************************************************
 */
short FAR PASCAL BitBlt(lpdvDst, ixDst, iyDst, lpbmSrc, ixSrc, iySrc, cx, cy,
		rop, lpbr, lpdm)
	DV FAR *lpdvDst;		/* Far ptr to the destination device descriptor */
	short ixDst;				/* The destination's horizontal origion */
	short iyDst;				/* The destination's vertical origion */
	BITMAP FAR *lpbmSrc;		/* Far ptr to the source device descriptor */
	short ixSrc;				/* The source's horizontal origion */
	short iySrc;				/* The source's vertical origion */
	short cx;					/* The horizontal extent (count of x units) */
	short cy;					/* The vertical extent (count of y units) */
	long rop;					/* The raster operation code */
	BR FAR *lpbr;				/* Far ptr to the brush (pattern) */
	LPDM lpdm;					/* Far ptr to the drawing mode */
	{
	LPSTR lpb;					/* A ptr to the band */
	int cbxBand;				/* The byte width of the band */
	int cyBand;					/* The number of scan lines in the band */
	int i;
	int cbx;
	LPSTR	lpHex;
	int		MaxHexBytes;
	HANDLE	hHex;

	ASSERT(lpdvDst != NULL);

	/* If there is no source device, then this is a pattern fill command
	 */
	if (!lpbmSrc && lpbr && cx > 0 && cy > 0)
		{
		SelectBrush(lpdvDst, lpbr);

/* LF 9/30/87 - Fixed patblt by not subtracting 1 from box corners */
                PrintChannel(lpdvDst, (LPSTR)"%d %d %d %d Box\n", ixDst, iyDst, cx, cy);
		return (TRUE);
		}

	/* Only do a BitBlt if the source device is a memory bitmap
	 */
	if (lpbmSrc->bmType)
		{
		return (FALSE);
		}

	/* Handle an image copy between two memory bitmaps
	 */
	if (!lpdvDst->dh.iType)
		{
		return (dmBitblt(lpdvDst, ixDst, iyDst, lpbmSrc, ixSrc, iySrc, cx, cy,
			rop, (long)lpbr, lpdm));
		}

	/* Handle the case where the destination is our physical device
	 */
	MaxHexBytes = 4 * ((cx + 15) / 16) + 1;
	hHex = GlobalAlloc(GMEM_FIXED, (long)MaxHexBytes);
	lpHex = GlobalLock(hHex);

	while (cy > 0)
		{
		ShapeBitmap((BITMAP FAR *) & lpdvDst->dh.bm, cx, cy);
		cyBand = lpdvDst->dh.bm.bmHeight;
		lpdvDst->dh.bm.bmBits = (LPSTR)lpdvDst->rgbBand;

		if (!dmBitblt((LPDV) & lpdvDst->dh.bm, 0, 0, lpbmSrc, ixSrc, iySrc,
				cx, cyBand, SRCCOPY, (long)lpbr, lpdm))
			{
#ifdef WIN20
                        GlobalUnlock(hHex);
                        GlobalFree(hHex);
#endif
			return (FALSE);
			}

		cy -= cyBand;
		iySrc += cyBand;

		/* Print the band bitmap one horizontal scan line at a time
		 */
		cbxBand = lpdvDst->dh.bm.bmWidthBytes;
		lpb = (LPSTR)lpdvDst->rgbBand;
#ifdef NEWCODE
		BandOut(lpdvDst, (LPBM) & lpdvDst->dh.bm, ixDst, iyDst, cx, cyBand,
		 lpHex);
		iyDst += cyBand;
#else

		while (--cyBand >= 0)
			{
			PrintScan(lpdvDst, ixDst, iyDst, cx, lpb);
			lpb += cbxBand;
			++iyDst;
			}

#endif
		}
	GlobalUnlock(hHex);
	GlobalFree(hHex);

	return (TRUE);
	}

/*******************************************************
 *	  Name: StretchBlt()
 *
 *	  Action: Copy a bitmap to the printer stretching it if
 *		  necessary.
 *
 *	  Method: The source bitmap is first converted to a monochrome
 *		  bitmap by allowing the brute to copy it.  The monochrome
 *		  bitmap is then downloaded to the printer where it is
 *		  stretched internally by the "image" operator.
 *
 *		  Note that if the source bitmap is large that we
 *		  may have to download it in several chunks.
 *
 *********************************************************
 */
void FAR PASCAL StretchBlt(lpdv, lpsblt)
	DV FAR *lpdv;
	LPSBLT lpsblt;
	{
	int ixSrc;
	int iySrc;
	int cxSrc;					/* The horizontal extent in object space */
	int cySrc;					/* The vertical extent in object space */
	LPSTR lpb;
	int i, j;
	BITMAP FAR *lpbmSrc;		/* A far ptr to the bitmap */
	BITMAP bmDst;
	ASSERT(lpdv != NULL);
	ASSERT(lpsblt != NULL);

	if (lpsblt == NULL)
		return;

	lpbmSrc = lpsblt->lpbm;

	if (lpbmSrc == NULL)
		return;

	cxSrc = lpsblt->cxSrc;
	cySrc = lpsblt->cySrc;
	ixSrc = lpsblt->ixSrc;
	iySrc = lpsblt->iySrc;
	PrintChannel(lpdv, (LPSTR)"gsave\n");
	PrintChannel(lpdv, (LPSTR)"%d %d translate\n", lpsblt->ixDst,
		lpsblt->iyDst);
	PrintChannel(lpdv, (LPSTR)"%d %d scale\n", Scale(lpsblt->cxDst, 72,
		lpdv->dh.iRes), Scale(lpsblt->cyDst, 72, lpdv->dh.iRes));
	PrintChannel(lpdv, (LPSTR)"%d %d true\n", cxSrc, cySrc);
	PrintChannel(lpdv, (LPSTR)"[%d 0 0 %d 0 %d]\n", cxSrc, -cySrc, cySrc);
	PrintChannel(lpdv, (LPSTR)"{<\n");

	/* Initialize the band bitmap
	 */
	lpb = (LPSTR) & bmDst;

	for (i = 0; i < sizeof (bmDst); ++i)
		*lpb++ = 0;

	bmDst.bmType = 0;
	bmDst.bmWidth = cxSrc;
	bmDst.bmWidthBytes = (cxSrc + 7) / 8;
	bmDst.bmHeight = sizeof (lpdv->rgbBand) / bmDst.bmWidthBytes;

	if (bmDst.bmHeight > cySrc)
		bmDst.bmHeight = cySrc;

	bmDst.bmPlanes = 1;
	bmDst.bmBitsPixel = 1;
	bmDst.bmBits = (LPSTR)lpdv->rgbBand;
	bmDst.bmWidthPlanes = bmDst.bmWidthBytes * bmDst.bmHeight;

	/* Handle the case where the destination is our physical device
	 */
	while (cySrc > 0)
		{
		/* Transfer from the source to the band bitmap
		 */
		if (!dmBitblt((LPDV) & bmDst, 0, 0, lpbmSrc, ixSrc, iySrc, cxSrc,
				bmDst.bmHeight, SRCCOPY, (long)lpsblt->lpbr,
				(LPDM)lpsblt->lpdm))
			{
			return;
			}

		cySrc -= bmDst.bmHeight;
		iySrc += bmDst.bmHeight;
		lpb = (LPSTR)lpdv->rgbBand;

		for (j = 0; j < bmDst.bmHeight; ++j)
			{
			for (i = 0; i < bmDst.bmWidthBytes; ++i)
				PrintChannel(lpdv, (LPSTR)"%02x", ((int) * lpb++) & 0x0ff);

			PrintChannel(lpdv, (LPSTR)"\n");
			}
		}

	PrintChannel(lpdv, (LPSTR)">} imagemask\n");
	PrintChannel(lpdv, (LPSTR)"grestore\n");
	}

