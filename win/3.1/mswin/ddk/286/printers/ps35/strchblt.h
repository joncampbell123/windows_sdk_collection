/*
 * strchblt.h	defs for stretchblt escape
 *
 */

typedef struct {
	WORD	X, Y;
	WORD	nWidth, nHeight;
	WORD	XSrc, YSrc;
	WORD	nSrcWidth, nSrcHeight;
	DWORD	dwRop;
	DWORD	TextColor;
	DWORD	bkColor;
	RECT	ClipRect;
	BITMAP	bm;
} SBLT;

typedef SBLT FAR *LPSBLT;

BOOL FAR PASCAL StrechBlt(LPDV, LPSBLT);



