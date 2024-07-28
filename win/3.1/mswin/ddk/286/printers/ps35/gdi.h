/**[f******************************************************************
 * gdi.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/***************************************************************************
 * 12Jun87	sjp		Added CC_ROUNDRECT.
 ***************************************************************************/



/* new stuff CRUSER format DIBs */

typedef struct tagRGBTRIPLE {
	BYTE	rgbtBlue;
	BYTE	rgbtGreen;
	BYTE	rgbtRed;
} RGBTRIPLE, FAR *LPRGBTRIPLE;

typedef struct tagRGBQUAD {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
} RGBQUAD,  FAR *LPRGBQUAD;

/* structures for defining DIBs */
typedef struct tagBITMAPCOREHEADER {
	DWORD	bcSize;			/* used to get to color table */
	WORD	bcWidth;
	WORD	bcHeight;
	WORD	bcPlanes;
	WORD	bcBitCount;
} BITMAPCOREHEADER;
typedef BITMAPCOREHEADER FAR *LPBITMAPCOREHEADER;
typedef BITMAPCOREHEADER *PBITMAPCOREHEADER;


typedef struct tagBITMAPINFOHEADER{
  	DWORD	   biSize;
  	DWORD	   biWidth;
  	DWORD	   biHeight;
  	WORD	   biPlanes;
  	WORD	   biBitCount;

	DWORD	   biStyle;
	DWORD	   biSizeImage;
	DWORD	   biXPelsPerMeter;
	DWORD	   biYPelsPerMeter;
	DWORD	   biClrUsed;
	DWORD	   biClrImportant;
} BITMAPINFOHEADER;

typedef BITMAPINFOHEADER FAR *LPBITMAPINFOHEADER;
typedef BITMAPINFOHEADER *PBITMAPINFOHEADER;

typedef struct tagBITMAPINFO { 
    BITMAPINFOHEADER	bmiHeader;
    RGBQUAD		bmiColors[1];
} BITMAPINFO;
typedef BITMAPINFO FAR *LPBITMAPINFO;
typedef BITMAPINFO *PBITMAPINFO;

typedef struct tagBITMAPCOREINFO { 
    BITMAPCOREHEADER	bmciHeader;
    RGBTRIPLE		bmciColors[1];
} BITMAPCOREINFO;
typedef BITMAPCOREINFO FAR *LPBITMAPCOREINFO;
typedef BITMAPCOREINFO *PBITMAPCOREINFO;

typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfXHotSpot;
	WORD	bfYHotSpot;
	DWORD	bfOffBits;
} BITMAPFILEHEADER;
typedef BITMAPFILEHEADER FAR *LPBITMAPFILEHEADER;
typedef BITMAPFILEHEADER *PBITMAPFILEHEADER;



#define xcoord x
#define ycoord y
#define PTTYPE POINT

typedef DWORD RGB;		/* GDI rgb type */

/* The escape (control functions) */
#define NEWFRAME	    1
#define ABORTDOC	    2
#define NEXTBAND	    3
#define SETCOLORTABLE	    4
#define GETCOLORTABLE	    5
#define FLUSHOUTPUT	    6
#define DRAFTMODE	    7
#define QUERYESCSUPPORT     8
#define SETABORTPROC	    9
#define STARTDOC	    10
#define ENDDOC		    11
#define GETPHYSPAGESIZE     12
#define GETPRINTINGOFFSET   13
#define GETSCALINGFACTOR    14

/* The output styles */
#define     OS_ARC		3
#define     OS_SCANLINES	4
#define     OS_RECTANGLE	6
#define     OS_ELLIPSE		7
#define     OS_MARKER		8
#define     OS_POLYLINE 	18
#define     OS_ALTPOLYGON	22
#define     OS_WINDPOLYGON	20
#define     OS_PIE		23
#define     OS_POLYMARKER	24
#define     OS_CHORD		39
#define     OS_CIRCLE		55
#define     OS_ROUNDRECT	72


/* The size to allocate for the lfFaceName field in the logical font. */
#define     LF_FACESIZE     32

/* Various constants for defining a logical font. */
#define     OUT_DEFAULT_PRECIS	    0
#define     OUT_STRING_PRECIS	    1
#define     OUT_CHARACTER_PRECIS    2
#define     OUT_STROKE_PRECIS	    3

#define     CLIP_DEFAULT_PRECIS     0
#define     CLIP_CHARACTER_PRECIS   1
#define     CLIP_STROKE_PRECIS	    2

#define     DEFAULT_QUALITY	    0
#define     DRAFT_QUALITY	    1
#define     PROOF_QUALITY	    2

#define     DEFAULT_PITCH	    0
#define     FIXED_PITCH 	    1
#define     VARIABLE_PITCH	    2

#define     ANSI_CHARSET	    0
#define     OEM_CHARSET 	    255


/*	GDI font families.						*/
#define FF_DONTCARE	(0<<4)	/* Don't care or don't know.		*/
#define FF_ROMAN	(1<<4)	/* Variable stroke width, serifed.	*/
				/* Times Roman, Century Schoolbook, etc.*/
#define FF_SWISS	(2<<4)	/* Variable stroke width, sans-serifed. */
				/* Helvetica, Swiss, etc.		*/
#define FF_MODERN	(3<<4)	/* Constant stroke width, serifed or sans-serifed. */
				/* Pica, Elite, Courier, etc.		*/
#define FF_SCRIPT	(4<<4)	/* Cursive, etc.			*/
#define FF_DECORATIVE	(5<<4)	/* Old English, etc.			*/


/*	Font weights lightest to darkest.				*/
#define FW_DONTCARE		0
#define FW_THIN 		100
#define FW_EXTRALIGHT		200
#define FW_LIGHT		300
#define FW_NORMAL		400
#define FW_MEDIUM		500
#define FW_SEMIBOLD		600
#define FW_BOLD 		700
#define FW_EXTRABOLD		800
#define FW_HEAVY		900

#define FW_ULTRALIGHT		FW_EXTRALIGHT
#define FW_REGULAR		FW_NORMAL
#define FW_DEMIBOLD		FW_SEMIBOLD
#define FW_ULTRABOLD		FW_EXTRABOLD
#define FW_BLACK		FW_HEAVY

/* Enumeration font types. */
#define     RASTER_FONTTYPE	    1
#define     DEVICE_FONTTYPE	    2





/* This bit in the dfType field signals that the dfBitsOffset field is an
   absolute memory address and should not be altered. */
#define PF_BITS_IS_ADDRESS  4

/* This bit in the dfType field signals that the font is device realized. */
#define PF_DEVICE_REALIZED  0x80

/* These bits in the dfType give the fonttype -
       raster, vector, other1, other2. */
#define PF_RASTER_TYPE	    0
#define PF_VECTOR_TYPE	    1
#define PF_OTHER1_TYPE	    2
#define PF_OTHER2_TYPE	    3

#define     LF_FACESIZE     32
#define     LS_SOLID		0
#define     LS_DASHED		1
#define     LS_DOTTED		2
#define     LS_DOTDASHED	3
#define     LS_DASHDOTDOT	4
#define     LS_NOLINE		5
#define     MaxLineStyle	LS_NOLINE

#define     BS_SOLID		0
#define     BS_HOLLOW		1
#define     BS_HATCHED		2
#define     BS_PATTERN		3
#define     MaxBrushStyle	3

#define     HS_HORIZONTAL	0
#define     HS_VERTICAL 	1
#define     HS_FDIAGONAL	2
#define     HS_BDIAGONAL	3
#define     HS_CROSS		4
#define     HS_DIAGCROSS	5
#define     MaxHatchStyle	5

#define     InquireInfo     0x01	/* Inquire Device GDI Info	   */
#define     EnableDevice    0x00	/* Enable Device		   */
#define     InfoContext     0x8000	/* Inquire/Enable for info context */


#define OBJ_PEN     1
#define OBJ_BRUSH   2
#define OBJ_FONT    3


/*		Device Technologies				   */

#define     DT_PLOTTER		0	/* Vector plotter	   */
#define     DT_RASDISPLAY	1	/* Raster display	   */
#define     DT_RASPRINTER	2	/* Raster printer	   */
#define     DT_RASCAMERA	3	/* Raster camera	   */
#define     DT_CHARSTREAM	4	/* Character-stream, PLP   */
#define     DT_METAFILE 	5	/* Metafile, VDM	   */
#define     DT_DISPFILE 	6	/* Display-file 	   */

/*		Curve Capabilities				   */

#define     CC_NONE	    00000000	/* Curves not supported */
#define     CC_CIRCLES	    00000001	/* Can do circles	*/
#define     CC_PIE	    00000002	/* Can do pie wedges	*/
#define     CC_CHORD	    00000004	/* Can do chord arcs	*/
#define     CC_ELLIPSES     00000010	/* Can do ellipese	*/
#define     CC_WIDE	    00000020	/* Can do wide lines	*/
#define     CC_STYLED	    00000040	/* Can do styled lines	*/
#define     CC_WIDESTYLED   00000100	/* Can do wide styled lines*/
#define     CC_INTERIORS    00000200	/* Can do interiors	*/
#define     CC_ROUNDRECT    00000400	/* Can do round rects	*/

/*		 Line Capabilities				   */

#define     LC_NONE	    00000000	/* Lines not supported	   */
#define     LC_POLYLINE     00000002	/* Can do polylines	   */
#define     LC_MARKER	    00000004	/* Can do markers	   */
#define     LC_POLYMARKER   00000010	/* Can do polymarkers	   */
#define     LC_WIDE	    00000020	/* Can do wide lines	   */
#define     LC_STYLED	    00000040	/* Can do styled lines	   */
#define     LC_WIDESTYLED   00000100	/* Can do wide styled lines*/
#define     LC_INTERIORS    00000200	/* Can do interiors	   */

/*		 Polygonal Capabilities 			   */

#define     PC_NONE	    00000000	/* Polygonals not supported*/
#define     PC_POLYGON	    00000001	/* Can do polygons	   */
#define     PC_RECTANGLE    00000002	/* Can do rectangles	   */
#define     PC_TRAPEZOID    00000004	/* Can do trapezoids	   */
#define     PC_SCANLINE     00000010	/* Can do scanlines	   */
#define     PC_WIDE	    00000020	/* Can do wide borders	   */
#define     PC_STYLED	    00000040	/* Can do styled borders   */
#define     PC_WIDESTYLED   00000100	/* Can do wide styled borders*/
#define     PC_INTERIORS    00000200	/* Can do interiors	   */

/*		 Polygonal Capabilities 			   */

#define     CP_NONE	    00000000	/* no clipping of Output   */
#define     CP_RECTANGLE    00000001	/* Output clipped to Rects */



/*-------------------------- Text Capabilities ----------------------- */

#define TC_OP_CHARACTER 0000001     /* OutputPrecision	CHARACTER      */
#define TC_OP_STROKE	0000002     /* OutputPrecision	STROKE	       */
#define TC_CP_STROKE	0000004     /* ClipPrecision	STROKE	       */
#define TC_CR_90	0000010     /* CharRotAbility	90	       */
#define TC_CR_ANY	0000020     /* CharRotAbility	ANY	       */
#define TC_SF_X_YINDEP	0000040     /* ScaleFreedom	X_YINDEPENDENT */
#define TC_SA_DOUBLE	0000100     /* ScaleAbility	DOUBLE	       */
#define TC_SA_INTEGER	0000200     /* ScaleAbility	INTEGER        */
#define TC_SA_CONTIN	0000400     /* ScaleAbility	CONTINUOUS     */
#define TC_EA_DOUBLE	0001000     /* EmboldenAbility	DOUBLE	       */
#define TC_IA_ABLE	0002000     /* ItalisizeAbility	ABLE	       */
#define TC_UA_ABLE	0004000     /* UnderlineAbility	ABLE	       */
#define TC_SO_ABLE	0010000     /* StrikeOutAbility	ABLE	       */
#define TC_RA_ABLE	0020000     /* RasterFontAble	ABLE	       */
#define TC_VA_ABLE	0040000     /* VectorFontAble	ABLE	       */
#define TC_RESERVED	0100000     /* Reserved. Must be returned zero */

/*------------------------ Raster Capabilities ----------------------*/

#define RC_NONE 	00000000    /* No Raster Capabilities	   */
#define RC_BITBLT	00000001    /* Can do bitblt		   */
#define RC_BANDING	00000002    /* Requires banding support    */
#define RC_SCALING	00000004    /* Requires scaling support    */
#define RC_GDI15_OUTPUT 00000020    /* has 2.0 output calls        */
#define RC_STRETCHBLT	0x0800		/* supports StretchBlt */

#define RC_DI_BITMAP	0x00080		/* dib to memory */
#define RC_DIBTODEV	0x00200		/* device can do DIBs */
#define RC_STRETCHDIB	0x02000		/* device can do DIBs */
#define RC_BITMAP64	0x00008		/* device can do DIBs */

#define OVERHANG 0

#define C1_TRANSPARENT 	0x0001		/* supports transparency	*/
#define TC_TT_ABLE	0x0002		/* can do TT through DDI or brute */
#define C1_TT_CR_ANY	0x0004		/* can do rotated TT fonts	*/

/* Background Mode definitions */

#define     TRANSPARENT         1
#define     OPAQUE              2


typedef struct {		 
	short int bmType;		 
	unsigned short int bmWidth;	 
	unsigned short int bmHeight;	 
	unsigned short int bmWidthBytes; 
	BYTE		   bmPlanes;	 
	BYTE		   bmBitsPixel;  
	BYTE FAR	  *bmBits;	 
	unsigned long int  bmWidthPlanes;
	BYTE FAR	  *bmlpPDevice;
	unsigned short int bmSegmentIndex;
	unsigned short int bmScanSegment;
	unsigned short int bmFillBytes;
	unsigned short int futureUse4;
	unsigned short int futureUse5;
} BITMAP;			  

#if 0	// old

typedef struct
    {
    short int bmType;
    unsigned short int bmWidth;
    unsigned short int bmHeight;
    unsigned short int bmWidthBytes;
    BYTE	       bmPlanes;
    BYTE	       bmBitsPixel;
    BYTE FAR	      *bmBits;
    unsigned long int  bmWidthPlanes;
    BYTE FAR	      *bmlpPDevice;
    unsigned short int futureUse1;
    RGB bkColor;			/* big hack */
    RGB fgColor;
#if 0
    unsigned short int futureUse2;
    unsigned short int futureUse3;
    unsigned short int futureUse4;
    unsigned short int futureUse5;
#endif
} BITMAP;
#endif

typedef BITMAP FAR *LPBITMAP;





typedef struct
    {
    short int lfHeight;
    short int lfWidth;
    short int lfEscapement;
    short int lfOrientation;
    short int lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    BYTE lfFaceName[LF_FACESIZE];
    } LOGFONT;
typedef LOGFONT FAR *LPLOGFONT;


typedef struct
    {
    short int dfType;
    short int dfPoints;
    short int dfVertRes;
    short int dfHorizRes;
    short int dfAscent;
    short int dfInternalLeading;
    short int dfExternalLeading;
    BYTE dfItalic;
    BYTE dfUnderline;
    BYTE dfStrikeOut;
    short int	dfWeight;
    BYTE dfCharSet;
    short int dfPixWidth;
    short int dfPixHeight;
    BYTE dfPitchAndFamily;
    short int dfAvgWidth;
    short int dfMaxWidth;
    BYTE dfFirstChar;
    BYTE dfLastChar;
    BYTE dfDefaultChar;
    BYTE dfBreakChar;
    short int	dfWidthBytes;
    unsigned long int	dfDevice;
    unsigned long int	dfFace;
    unsigned long int	dfBitsPointer;
    unsigned long int	dfBitsOffset;
    WORD dfSizeFields;
    DWORD dfExtMetricsOffset;
    DWORD dfExtentTable;
    DWORD dfOriginTable;
    DWORD dfPairKernTable;
    DWORD dfTrackKernTable;
    DWORD dfDriverInfo;
    DWORD dfReserved;


    } FONTINFO;
typedef FONTINFO FAR *LPFONTINFO;


/* SCALABLEFONTINFO    struc						    ;*/ typedef     struct  {	       /*
  erType	    dw	    0	; Type field for the font.		    ;*/ short int   erType;	       /*
  erPoints	    dw	    0	; Point size of font.			    ;*/ short int   erPoints;	       /*
  erVertRes	    dw	    0	; Vertical digitization.		    ;*/ short int   erVertRes;	       /*
  erHorizRes	    dw	    0	; Horizontal digitization.		    ;*/ short int   erHorizRes;        /*
  erAscent	    dw	    0	; Baseline offset from char cell top.	    ;*/ short int   erAscent;	       /*
  erInternalLeading dw	    0	; Internal leading included in font	    ;*/ short int   erInternalLeading; /*
  erExternalLeading dw	    0	; Prefered extra space between lines	    ;*/ short int   erExternalLeading; /*
  erItalic	    db	    0	; Flag specifying if italic.		    ;*/ BYTE	    erItalic;	       /*
  erUnderline	    db	    0	; Flag specifying if underlined.	    ;*/ BYTE	    erUnderline;       /*
  erStrikeOut	    db	    0	; Flag specifying if struck out.	    ;*/ BYTE	    erStrikeOut;       /*
  erWeight	    dw	    0	; Weight of font.			    ;*/ short int   erWeight;	       /*
  erCharSet	    db	    0	; Character set of font.		    ;*/ BYTE	    erCharSet;	       /*
  erPixWidth	    dw	    0	; Width field for the font.		    ;*/ short int   erPixWidth;        /*
  erPixHeight	    dw	    0	; Height field for the font.		    ;*/ short int   erPixHeight;       /*
  erPitchAndFamily  db	    0	; Flag specifying pitch and family.	    ;*/ BYTE	    erPitchAndFamily;  /*
  erAvgWidth	    dw	    0	; Average character width.		    ;*/ short int   erAvgWidth;        /*
  erMaxWidth	    dw	    0	; Maximum character width.		    ;*/ short int   erMaxWidth;        /*
  erFirstChar	    db	    0	; First character in the font.		    ;*/ BYTE	    erFirstChar;       /*
  erLastChar	    db	    0	; Last character in the font.		    ;*/ BYTE	    erLastChar;        /*
  erDefaultChar     db	    0	; Default character for out of range.	    ;*/ BYTE	    erDefaultChar;     /*
  erBreakChar	    db	    0	; Character to define wordbreaks.	    ;*/ BYTE	    erBreakChar;       /*
  erWidthBytes	    dw	    0	; Number of bytes in each row.		    ;*/ short int   erWidthBytes;      /*
  erDevice	    dd	    0	; Offset to device name.		    ;*/ unsigned long int   erDevice;  /*
  erFace	    dd	    0	; Offset to face name.			    ;*/ unsigned long int   erFace;    /*
  erBitsPointer     dd	    0	; Bits pointer. 			    ;*/ unsigned long int   erBitsPointer;/*
  erBitsOffset	    dd	    0	; Offset to the begining of the bitmap.     ;*/ unsigned long int   erBitsOffset;/*
  erFlags	    db	    0	; flags, and word align the stuff to come   ;*/ BYTE erReservedByte;		/*
  erUnderlinePos    dw	    0	; underline position relative to cell origin ;*/ short int   erUnderlinePos;  /*
  erUnderlineThick  dw	    0	; underline thickness			     ;*/ short int   erUnderlineThick;/*
  erStrikeoutPos    dw	    0	; Strikeout position relative to cell origin ;*/ short int   erStrikeoutPos;  /*
  erStrikeoutThick  dw	    0	; strikeout thickness			     ;*/ short int   erStrikeoutThick;/*
  erEFontIndex      dw      0   ; Index into FE data base to entry for thefnt;*/   short      erEFontIndex;     /*
  erEM              dw      0   ; Notional size of EM.                       ;*/   short      erEM;             /*
  erSpotSizeX       dd      0   ; 16.16 number defining spot size of device  ;*/   long       erSpotSizeX;      /*
  erSpotSizeY       dd      0   ;  WRT unity pel                             ;*/   long       erSpotSizeY;      /*
  erEscapement	    dw	    0	; Escapement of font.			     ;*/   short      erEscapement;	/*
  erScaleX	    dw	    0	; Scale factor along escapement vector	     ;*/   short      erScaleX; 	/*
  erHeightEM        dw      0   ; Height of the EM in device units.          ;*/   short      erHeightEM;       /*
  erCharBias        dw      0   ; Amount to add to any char when rasterizing.;*/   short      erCharBias;       /*
  erhFontFile       dd      0   ; Handle to engine font file.                ;*/   long       erhFontFile;      /*
  erhPFont          dw      0   ; Handle to actual physical font.            ;*/   short      erhPFont;         /*
  erTempOffset      dw      0   ; Offset to scratch area where bmp is loaded.;*/   short      erTempOffset;     /*
  erTempBufferSize  dw      0   ; Size of that buffer.                       ;*/   short      erTempBufferSize; /*
  erWidthTable      dw      0   ; Table of character increments (widths).    ;*/   short      erWidthTable;     /*
  erACTable         dw      0   ; Offset to table of A and C spaces of glyphs;*/   short      erACTable;        /*
  erSizePFont       dd      0   ; Size of physical font object.              ;*/   long       erSizePFont;      /*
  erBlockSizeInc    dd      0   ; Amount by which to increment glyph cache me;*/   long       erBlockSizeInc;   /*
  erCurrOffset      dd      0   ; Offset to next character's bitmap.         ;*/   long       erCurrOffset;     /*
  erFreeSize        dd      0   ; Size of available space for glyphs in cache;*/   long       erFreeSize;       /*
SCALABLEFONTINFO    ends						    ;*/ } SCALABLEFONTINFO;	       




typedef     struct
    {
    short int ftHeight;
    short int ftWidth;
    short int ftEscapement;
    short int ftOrientation;
    short int ftWeight;
    BYTE ftItalic;
    BYTE ftUnderline;
    BYTE ftStrikeOut;
    BYTE ftOutPrecision;
    BYTE ftClipPrecision;
    unsigned short int ftAccelerator;
    short int ftOverhang;
    } TEXTXFORM;
typedef TEXTXFORM FAR *LPTEXTXFORM;

 typedef struct
    {
    short int tmHeight;
    short int tmAscent;
    short int tmDescent;
    short int tmInternalLeading;
    short int tmExternalLeading;
    short int tmAveCharWidth;
    short int tmMaxCharWidth;
    short int tmWeight;
    BYTE  tmItalic;
    BYTE  tmUnderlined;
    BYTE  tmStruckOut;
    BYTE  tmFirstChar;
    BYTE  tmLastChar;
    BYTE  tmDefaultChar;
    BYTE  tmBreakChar;
    BYTE  tmPitchAndFamily;
    BYTE  tmCharSet;
    short int tmOverhang;
    short int tmDigitizedAspectX;
    short int tmDigitizedAspectY;
    } TEXTMETRIC;
typedef TEXTMETRIC FAR *LPTEXTMETRIC;



typedef struct tagNEWTEXTMETRIC
  {
    int 	tmHeight;
    int 	tmAscent;
    int 	tmDescent;
    int 	tmInternalLeading;
    int 	tmExternalLeading;
    int 	tmAveCharWidth;
    int 	tmMaxCharWidth;
    int 	tmWeight;
    BYTE	tmItalic;
    BYTE	tmUnderlined;
    BYTE	tmStruckOut;
    BYTE	tmFirstChar;
    BYTE	tmLastChar;
    BYTE	tmDefaultChar;
    BYTE	tmBreakChar;
    BYTE	tmPitchAndFamily;
    BYTE	tmCharSet;
    int 	tmOverhang;
    int 	tmDigitizedAspectX;
    int 	tmDigitizedAspectY;
    DWORD	ntmFlags;		/* various flags (fsSelection) */
    WORD	ntmSizeEM;		/* size of EM */
    WORD	ntmCellHeight;		/* height of font in notional units */
    WORD	ntmAvgWidth;		/* average with in notional units */
  } NEWTEXTMETRIC;
typedef NEWTEXTMETRIC *PNEWTEXTMETRIC;
typedef NEWTEXTMETRIC NEAR *NPNEWTEXTMETRIC;
typedef NEWTEXTMETRIC FAR *LPNEWTEXTMETRIC;



 typedef     struct
    {
    short int	      Rop2;
    short int	      bkMode;
    unsigned long int bkColor;
    unsigned long int TextColor;
    short int	      TBreakExtra;
    short int	      BreakExtra;
    short int	      BreakErr;
    short int	      BreakRem;
    short int	      BreakCount;
    short int	      CharExtra;
    unsigned long int LbkColor;
    unsigned long int LTextColor;
    } DRAWMODE;

typedef DRAWMODE FAR *LPDRAWMODE;


 typedef struct
    {
    short int dpVersion;
    short int dpTechnology;
    short int dpHorzSize;
    short int dpVertSize;
    short int dpHorzRes;
    short int dpVertRes;
    short int dpBitsPixel;
    short int dpPlanes;
    short int dpNumBrushes;
    short int dpNumPens;
    short int futureuse;
    short int dpNumFonts;
    short int dpNumColors;
    short int dpDEVICEsize;
    unsigned short int	dpCurves;
    unsigned short int	dpLines;
    unsigned short int	dpPolygonals;
    unsigned short int	dpText;
    unsigned short int	dpClip;
    unsigned short int	dpRaster;
    short int dpAspectX;
    short int dpAspectY;
    short int dpAspectXY;
    short int dpStyleLen;
    POINT    dpMLoWin;
    POINT    dpMLoVpt;
    POINT    dpMHiWin;
    POINT    dpMHiVpt;
    POINT    dpELoWin;
    POINT    dpELoVpt;
    POINT    dpEHiWin;
    POINT    dpEHiVpt;
    POINT    dpTwpWin;
    POINT    dpTwpVpt;
    short int dpLogPixelsX;
    short int dpLogPixelsY;
    short int dpDCManage;
    short int dpCaps1;
    short int futureuse4;
    short int futureuse5;
    short int futureuse6;
    short int futureuse7;
	short	new1, new2, new3;	/* for version 0x300 devices */
    } GDIINFO;

typedef struct
    {
    unsigned short int lopnStyle;
    POINT  lopnWidth;
    unsigned long int  lopnColor;
    } LOGPEN;
typedef LOGPEN FAR *LPLOGPEN;

 typedef struct
    {
    unsigned short int lbStyle;
    unsigned long int  lbColor;
    unsigned short int lbHatch;
    unsigned long int lbBkColor;
    } LOGBRUSH;
typedef LOGBRUSH FAR *LPLOGBRUSH;


typedef struct {
	short	x;
	short	y;
	int	count;
	RECT	ClipRect;
	LPSTR	lpStr;
	short	far *lpWidths;
} APPEXTTEXTDATA;
typedef APPEXTTEXTDATA FAR *LPAPPEXTTEXTDATA;


typedef struct {
	short			nSize;
	LPAPPEXTTEXTDATA	lpInData;
	LPFONTINFO		lpFont;
	LPTEXTXFORM 		lpXForm;
	LPDRAWMODE		lpDrawMode;
} EXTTEXTDATA;
typedef EXTTEXTDATA FAR *LPEXTTEXTDATA;


/* spooler error code */
#define SP_NOTREPORTED	    0x4000  /* set if GDI did not report error */
#define SP_ERROR	    (-1)    /* general errors who know what went wrong */
#define SP_APPABORT	    (-2)    /* app aborted the job - callback function returned false */
#define SP_USERABORT	    (-3)    /* user aborted the job through spooler's front end */
#define SP_OUTOFDISK	    (-4)    /* not enough disk space to spool */
#define SP_OUTOFMEMORY	    (-5)
