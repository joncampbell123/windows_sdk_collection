/**[f******************************************************************
 * pscript.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/*
 * changed ifdef NOV18 to #if 1
 * #define NOV18
 *
 */

/***************************************************************************
 * 9Mar87	sjp	added IBM stuff
 * 7Apr87	sjp	added DataProducts LZR 2665 (1st cut)
 * 8Apr87	sjp	added DEC LN03R ScriptPrinter
 * 14Apr87	sjp	consolidated printer specific stuff from pscript.h
 *			and added a capabilities structure
 * 3Jun87	sjp	Added szDevice[] entry to device descriptor (DV).
 * 12Jun87	sjp	Added OS_ROUNDRECT.
 * 88Jan14	chrisg	renamed szDevice to szDevType (now it makes sense)
 * 91Mar12	msd	added variable iDuplex & replaced bType1Supported with
 *                iDLFontFmt.
 ***************************************************************************/

#include "win.h"
#include "gdi.h"
#include "paper.h"
#include "strlist.h"
#include "drivinit.h"
#include "devmode.h"

/* EM describes the basic character cell dimension (in Adobe units) */
#define EM 1000

// smallest ppem size to download outline instead of bitmap
//     ppem <  MINOUTLINEPPEM -> bitmap
//     ppem >= MINOUTLINEPPEM -> outline (if not downloaded already)

#define MINOUTLINEPPEM    101

#define EOF	4	/* Control-D = end of file */

/* temporary...to turn round rects on/off */
#define ROUNDRECTSTUFF


#define CBFNMAX	 80	/* The maximum filename length */

#define HRES    1440    /* The horizontal positioning resolution in DPI */
#define VRES    1440    /* The vertical positioning resolution in DPI */
#define BHRES    300    /* The horizontal bitmap resolution in DPI */
#define BVRES    300    /* The vertical bitmap resolution in DPI */


#define CBBANDMAX	2048
#define CBSPOOLMAX	256	/* The spool buffer size */
#define CBBUFMAX	512	/* The temporary working buffer size */
#define CFONTS		13	/* The number of fonts */
#define	MAXSOFTFONTS	99	/* Max soft fonts */

/* this charset value implies that no translation will
 * will occur on the character set in question.
 */
#define NO_TRANSLATE_CHARSET 200 /* djm 12/20/87 */

#define PP_NULL     0	/* Null type postscript path */
#define PP_STROKE   1	/* Stroke type Post Script path */
#define PP_FILL     2	/* Fill type Post Script path */

/* _SPOOL constants for queryjob */
#define SP_QUERYVALIDJOB    30
#define SP_QUERYDISKAVAIL   0x1004


typedef DWORD CO;	/* A physical color */
typedef CO FAR *LPCO;


/* A physical pen (the device driver's complete pen description) */

typedef struct {
	LOGPEN lopn;		/* The logical pen */
	BOOL fRound;		/* TRUE if the pen has round endcaps */
} PEN;
typedef PEN FAR *LPPEN;



/* A physical brush (the device driver's complete brush description) */

typedef struct {
	LOGBRUSH	lb;		/* A copy of the logical brush */
	BYTE		rgbPat[8];	/* The brush pattern bitmap */
} BR;
typedef BR FAR *LPBR;


typedef enum {
	fromdrawmode,
	justifywordbreaks,
	justifyletters
} JUSTBREAKTYPE;

typedef struct {
	short extra;
	short rem;
	short err;
	WORD count;
	WORD ccount;
} JUSTBREAKREC;

typedef JUSTBREAKTYPE FAR *LPJUSTBREAKTYPE;
typedef JUSTBREAKREC FAR *LPJUSTBREAKREC;


#define CBNAME	 32

#define DL_NOTHING	0x0000
#define DL_UNPACK	0x0001
#define DL_FONTS	0x0002
#define DL_CIMAGE	0x0004
#define DL_T3HEADER     0x0008

#define DLFMT_TYPE3     0
#define DLFMT_TYPE1     1
#define DLFMT_TRUETYPE  2
#define DLFMT_NODOWNLOAD     3

/*
 * DV (device).  also known as PDEVICE
 *
 * A new copy of this structure is allocated for each instance of the
 * printer driver.  Note: since a single data segment is shared
 * amoung all instances, unshared variables must be stored here.
 *
 * the data in this structure comes and goes when printer DC's are
 * created and deleted.  thus, data that should stay around needs
 * to be saved by GDI in an enviornment (associated with each port).
 *
 */

typedef struct {
    short	iType;	    	/* The device type (non-zero) */
    BOOL	fContext;   	/* TRUE for InfoContext (DH only) */
    BOOL	fIsClipped; 	/* TRUE if a ClipBox is active */
    BOOL	fLandscape; 	/* Page orientation (TRUE=LANDSCAPE) */
    BOOL	fDirty;     	/* TRUE if the current page is dirty */
    int 	iPaperSource;	/* The current paper source */
    BOOL	fPairKern;  	/* TRUE if pairwise kerning is turned on */
    BOOL	fIntWidths; 	/* TRUE for integer character widths */
    HDC 	hdc;	    	/* The device's display context */
    HANDLE	hFontDir;   	/* The font directory handle */
    short	fh;	    	/* The output file handle or job number */
    unsigned	cbSpool;    	/* The current byte offset into the spool buffer */
    unsigned	cbSpoolMax; 	/* The size of the spool buffer */
    short	iStatus;    	/* The current status of the document */
    short	iPrinter;   	/* The printer type */
    int 	iRes;		/* The printer resolution in dots per inch */
    int 	iRealRes;	/* true graphics resolution. 'iRes' is fake.*/
				// This is needed to represent large paper
				// sizes (e.x. 17") in high resolutions
				// (e.x. 2540dpi) without overflow.
    int 	iCopies;    	/* The number of copies to print */
    short	iJobTimeout;	/* the individual job timeout in seconds */
    short	fHeader;    	/* FALSE=don't download the header each time */
    short	marginState;	/* default, zero, or tilemode */
    int 	iTrack;     	/* The track-kerning track number. */
    PAPER	paper;	    	/* The paper metrics */
    PAPER	absPaper;	    /* Unrotated paper metrics */
    short	iBand;	    	/* The current band number */
    BR		br;	    	/* The current brush */
    PEN 	pen;	    	/* The current pen */
    int         iCurLineJoin;   /* The current line join style */
    int         iCurLineCap;    /* The current line cap style */
    int         iCurMiterLimit; /* The current miter limit */
    int         iNewLineJoin;   /* The new line join style */
    int         iNewLineCap;    /* The new line cap style */
    int         iNewMiterLimit;
    BOOL	fColor;		/* print in color or not */
    BOOL	fBinary;	/* port is binary capable */
    BOOL	fEps;		/* use minimal header (for EPS files) */
    BOOL	fDoEps;		/* print an eps file */
    RECT	EpsRect;	/* bounding rect set by SET_BOUNDS escape */
    DWORD	TextColor;
    long	lid;		/* A long interger used to tag objects (id) */
    long	lidFont;	/* The current font id */
    BOOL	fPenSelected;		/* the pen exists */
    WORD	GraphData;	/* graphics state data */
    WORD	DLState;
    DWORD	HatchColor;	/* hatch and pattern colors */
    DWORD	FillColor;	/* solid and opaque filling color */
    WORD	FillMode;	/* mode of filling (pattern, solid, hatch) */
    WORD	PolyMode;	/* for SET_POLY_MODE escape */
    WORD	PathLevel;	/* for BEGIN_PATH END_PATH */
    HANDLE      slSupplied;     /* list of supplied resources */
    HANDLE      slNeeded;       /* list of needed resources */
    HANDLE      slPageResources;/* list of page resources */
    HANDLE      slTTFonts;      /* list of truetype fonts /w download data */

    /* added by Aldus Corporation 19 January 1987
     * This stuff is to enhance the justification schema used in the driver
     * to allow for negative justification and word break and letter
     * justification. */

    JUSTBREAKTYPE	epJust;		/*kind of justification*/
    JUSTBREAKREC	epJustWB; 	/*justification rec for word breaks*/
    JUSTBREAKREC	epJustLTR;	/*justification rec for letters*/
    
    /* Soft font status information for downloading control
     * 	nextSlot=-1 if empty
     * 	slotsArray contains all currently loaded fonts on any given page */

    int nextSlot;			/* points to next open slot */
    int slotsArray[MAXSOFTFONTS];	/* store font instance ID's */

    char    rgbSpool[CBSPOOLMAX];	/* The output spool buffer */
    char    szDevType[CBNAME+1]; 	/* device name */
    char    szFile[40];		 	/* The start of the output file name */
    short	dx, dy;	/* dib scale values */
    char ScaleMode;	/* DIB ScaleMode */
    int angle;	/* in 10ths of an degree for SETSCREEN escape */
    int freq;	/* in 10ths of an inch */
    BOOL bDSC;	    /* TRUE if conforming output should be generated */
    BOOL bNegImage; /* TRUE if image should be inverted */
    BOOL bPerPage; /* TRUE if fonts should be downloaded each page */
//    this already contained in DevMode
//    short iDLFontFmt;   /* font DL format (Type 3, Type 1, TrueType) */
//    BOOL bSubFonts;   /* TRUE if font substitution should be performed */
    BOOL bMirror;     /* TRUE if output should be mirrored */
    BOOL bColorToBlack; /* TRUE if all colors should map to black */
    BOOL bCompress;   /* TRUE if we should ALWAYS compress bitmaps */
    BOOL bErrHandler; /* TRUE if we should use our error handler */
    int iDuplex; /* the requested duplex style, -1 means not supported */
    DWORD dwMaxVM; /* amount of printer VM in bytes */
    DWORD dwCurVM; /* estimate of VM used */        

    int iCustomWidth;      /* width of custom paper */
    int iCustomHeight;     /* height of custom paper */
    BOOL bUsingCustomPaper;   /* TRUE if user requested custom paper */

    WORD FAR *lpTTFontList;/* handle to array of atoms for fonts used */
    int    sTTFontList;    /* number of entries in list */
    int    cTTFontList;    /* count of fonts in list */

#ifdef PS_IGNORE
    BOOL fSupressOutput;		/* used by POSTSCRIPT_DATA and
					 * POSTSCRIPT_IGNORE */
#endif
    int iPageNumber;

    PSDEVMODE DevMode;    /* devmode used to setup lpdv */
} DV;

typedef DV FAR *LPDV;

typedef DV PDEVICE;			/* these are the conventional names */
typedef PDEVICE FAR *LPPDEVICE;


#define GD_NONE	0x0000	/* fill mode is defined */
#define GD_FM	0x0001	/* fill mode is defined */
#define GD_PAT	0x0002	/* fill mode is defined */
#define GD_FC	0x0004	/* fill mode is defined */
#define GD_HC	0x0008	/* fill mode is defined */

/* The aspect ratio structure */
typedef struct {
    char    bError;	    /* The initial style error status */
    char    bHypotenuse;    /* The hypotenuse */
    char    bx; 	    /* The x major distance */
    char    by; 	    /* The y major distance */
} ASPECT;

typedef HANDLE		    HMENU;
HWND  FAR PASCAL CreateWindow(LPSTR, LPSTR, DWORD, int, int, int, int, HWND, HMENU, HANDLE, LPSTR);
