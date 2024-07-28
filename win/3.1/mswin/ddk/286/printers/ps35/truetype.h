/*
 *  truetype.h
 *
 *  prototypes, defines and typedefs for TrueType support
 */

// high byte of dfType defines

#define TYPE_TRUETYPE	    0x0100	// font is truetype
#define TYPE_TTRESIDENT     0x0200  // font is resident TrueType font
#define TYPE_HAVEWIDTHS     0x0400	// width table has been generated
#define TYPE_SETCONTEXT     0x0800	// remember to set font context
#define TYPE_OUTLINE        0x1000      // Type 1 outline
#define TYPE_SUBSTITUTED        0x2000      // TrueType substituted

// download flags
#define TTFLAGS_HEADERDOWN     0x0001	// header has been downloaded

// Engine Glyph flags

#define FD_QUERY_CHARIMAGE  1
#define FD_QUERY_OUTLINE    2

#define NCHARS	256

#define CWGLYPHFLAGS (NCHARS/(8*sizeof(WORD)))

#define MAKEFIXED(n) ((LONG)(n)<<16)
#define FROMFIXED(n) HIWORD(n)

#define ROT_PORT MAKEFIXED(0)		// rotation for portrait
#define ROT_LAND MAKEFIXED(270) 	// rotation for landscape

#define LVHWORD(l) (((WORD FAR *)&l)[1])
#define LVLWORD(l) (((WORD FAR *)&l)[0])

typedef LONG FIXED;
typedef unsigned long ULONG;

typedef WORD FAR * LPINT;

typedef BYTE huge * LPDIBITS;

typedef unsigned short USHORT;

typedef struct _pointfx
  {
    FIXED x;
    FIXED y;
  } POINTFX;

typedef struct _sizel
  {
    LONG cx;
    LONG cy;
  } SIZEL;

typedef struct _bitmapmetrics
  {
    SIZEL sizlExtent;
    POINTFX pfxOrigin;
    POINTFX pfxCharInc;
  } BITMAPMETRICS, FAR * LPBITMAPMETRICS;

typedef struct _ttfontinfo
  {
    WORD *prgfGlyphDown;
    WORD rgwWidths[256];
    WORD  TTRefEM;  //  the EM square at which the TT font was
                        // rasterized at
    int  iPageNumber;
    int sx, sy;
    long lid;
    short lfPoints;
    BYTE TTFaceName[12]; /* MSTT31xxxx style font names */
    LOGFONT lfCopy;
    TEXTXFORM ftCopy;
  } TTFONTINFO, FAR * LPTTFONTINFO;

typedef struct _POLYGONHEADER { /* */
    ULONG cb;
    ULONG iType;	/*  Must be FD_POLYGON_TYPE */
    POINTFX pteStart;   
} POLYGONHEADER;

typedef POLYGONHEADER FAR *LPPOLYGONHEADER;

typedef struct _FDPOLYCURVE { /* fdpc */
  USHORT iType;	/* Must be either FD_PRIM_LINE, FD_PRIM_SPLINE,	*/
		/* or FD_PRIM_FLT				*/
  USHORT cptfx; /* Number of points */
  POINTFX pte[1];
} FDPOLYCURVE;

typedef FDPOLYCURVE FAR *LPFDPOLYCURVE;

#define FD_POLYGON_TYPE 24
#define FD_PRIM_LINE 1
#define FD_PRIM_QSPLINE 2 
#define FD_PRIM_SPLINE 3
#define FD_PRIM_FLT 7

#define ISCHARDOWN(lpttfi,ch) (((lpttfi)->prgfGlyphDown[(ch)/16]) & (1 << ((ch)%16)))
#define SETCHARDOWN(lpttfi,ch) (((lpttfi)->prgfGlyphDown[(ch)/16]) |= (1 << ((ch)%16)))
#define CLRCHARDOWN(lpttfi,ch) (((lpttfi)->prgfGlyphDown[(ch)/16]) &= ~(1 << ((ch)%16)))

/*
 *  GDI Engine exports
 */

WORD FAR PASCAL EngineRealizeFont(LPLOGFONT, LPTEXTXFORM, LPFONTINFO);
WORD FAR PASCAL EngineDeleteFont(LPFONTINFO);
WORD FAR PASCAL EngineEnumerateFont(LPSTR, FARPROC, DWORD);
WORD FAR PASCAL EngineGetCharWidth(LPFONTINFO,BYTE,BYTE,LPINT);
WORD FAR PASCAL EngineSetFontContext(LPFONTINFO,WORD);
int  FAR PASCAL EngineGetGlyphBmp(WORD,LPFONTINFO,WORD,WORD,LPSTR,DWORD,LPBITMAPMETRICS);

int FAR PASCAL TTDownLoadFont(LPDV lpdv, LPDF lpdf, BOOL bFullFont);
int FAR PASCAL TTUpdateFont(LPDV lpdv, LPDF lpdf, LPSTR lpStr, int cb); 
int FAR PASCAL TTLockFont(LPDV lpdv, LPDF lpdf);
void FAR PASCAL TTUnlockFont(LPDV lpdv, LPDF lpdf);
void FAR PASCAL TTFlushFonts(LPDV lpdv);
BOOL FAR PASCAL IsTrueTypeEnabled(void);
LPDF FAR PASCAL TTGetBaseFont(LPDV lpdv, LPDF lpdf);
void FAR PASCAL TTSetBaseFont(LPDV lpdv, LPDF lpdf, LPDF lpdfBase);
