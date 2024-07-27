#define WIN20


/***************************************************************************
 * 9Mar87	sjp		added IBM stuff
 * 7Apr87	sjp		added DataProducts LZR 2665 (1st cut)
 * 8Apr87	sjp		added DEC LN03R ScriptPrinter
 * 14Apr87	sjp		consolidated printer specific stuff from pscript.h
 *					and added a capabilities structure
 * 3Jun87	sjp		Added szDevice[] entry to device descriptor (DV).
 * 12Jun87	sjp		Added OS_ROUNDRECT.
 ***************************************************************************/


#include    "windows.h"
#include    "gdi.h"

/* temporary...to turn round rects on/off */
#define ROUNDRECTSTUFF


/* #define ACHECK	   /* Define this for an assertion check */

#ifdef ACHECK
extern far error();
#define ASSERT(p) if(!(p))error()
#else
#define ASSERT(p)
#endif


#define     CBNAME	 32
#define     CBFNMAX	 80	/* The maximum filename length */

#define TRUE 1
#define FALSE 0
#define NULL 0

#define     HRES    1440    /* The horizontal positioning resolution in DPI */
#define     VRES    1440    /* The vertical positioning resolution in DPI */
#define     BHRES    300    /* The horizontal bitmap resolution in DPI */
#define     BVRES    300    /* The vertical bitmap resolution in DPI */


#define     CBBANDMAX   2048
#define     CBSPOOLMAX	256	/* The spool buffer size */
#define     CBBUFMAX	512	/* The temporary working buffer size */
#define     CFONTS	13	/* The number of fonts */

#define     DEFAULTRES	100	/* The default printer resolution */




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
#define     OS_POLYGON		20
#define     OS_EOPOLYGON	22
#define     OS_PIE		23
#define     OS_POLYMARKER	24
#define     OS_CHORD		39
#define     OS_CIRCLE		55
#define		OS_ROUNDRECT	72


/*********** formerly resource.h ******************/

/* Conventional dialog box and message box command IDs */
#define IDOK	     1
#define IDCANCEL     2
#define IDABORT      3
#define IDRETRY      4
#define IDIGNORE     5
#define IDYES	     6
#define IDNO	     7

/* Window styles */
#define WS_TILED	0x00000000L
#define WS_POPUP	0x80000000L
#define WS_CHILD	0x40000000L
#define WS_ICONIC	0x20000000L
#define WS_VISIBLE	0x10000000L
#define WS_DISABLED	0x08000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_CLIPCHILDREN 0x02000000L

#define WS_BORDER	0x00800000L
#define WS_CAPTION	0x00c00000L
#define WS_DLGFRAME	0x00400000L
#define WS_VSCROLL	0x00200000L
#define WS_HSCROLL	0x00100000L
#define WS_SYSMENU	0x00080000L
#define WS_SIZEBOX	0x00040000L
#define WS_GROUP	0x00020000L
#define WS_TABSTOP	0x00010000L


#define MYFONTDIR  257
#define MYFONT	   258
#define PSHEADER   259
#define MYETM	   260
#define MY_DATA	   261
#define PR_CAPS	   262



typedef long RGB;	/* A window's RGB color */
typedef long CO;	/* A physical color */
typedef int (FAR PASCAL *FARPROC)();


/* The pair kerning structure */
/* Note: the kern amount is given in hundreths of a point per character */
typedef struct
    {
    int cPairs; 	    /* The number of kerning pairs */
    struct
	{
	int iKey;	    /* The kerning pair concatenated into a key */
	int iKernAmount;
	}rgPairs[1];
     }KP;
typedef KP FAR *LPKP;


/* The info for a single kern track */
typedef struct
    {
    short iDegree;    /* The degree of kerning */
    short iPtMin;     /* The minimum point size */
    short iKernMin;   /* The minimum kern amount */
    short iPtMax;     /* The maximum point size */
    short iKernMax;   /* The maximum kern amount */
    }TRACK;

/* The track kerning table for a font */
typedef struct
    {
    short cTracks;	  /* The number of kern tracks */
    TRACK rgTracks[1];	/* The kern track information */
    }KT;
typedef KT FAR *LPKT;



/* A physical pen (the device driver's complete pen description) */
typedef struct
    {
    LOGPEN lopn;	/* The logical pen */
    long lid;		/* The physical pen id */
    BOOL fRound;	/* TRUE if the pen has round endcaps */
    }PEN;



/* A physical brush (the device driver's complete brush description) */
typedef struct
    {
    LOGBRUSH lb;	/* A copy of the logical brush */
    long   lid; 	/* The brush id */
    char   rgbPat[8];	/* The brush pattern bitmap */
    }BR;


/* bitmap structure */
typedef struct
    {
    short  iType;	/* The bitmap type */
    short   cx; 	/* The bitmap width in pixels */
    short   cy; 	/* The bitmap height in pixels */
    short   cbx;	/* The bitmap width in bytes */
    short   cb; 	/* The bitmap size in bytes */
    LPSTR   lpbBits;	/* Ptr to the bitmap bits */
    }BM;
typedef BM far *LPBM;


/* The paper metrics */
typedef struct
    {
    short iPaper;	/* The paper type */
    short cxPaper;	/* The paper width in dots */
    short cyPaper;	/* The paper height in dots */
    short cxPage;	/* The printable area width in dots */
    short cyPage;	/* The printable area height in dots */
    short cxMargin;	/* The left margin width to printable area in dots */
    short cyMargin;	/* The top margin depth to printable area in dots */
    }PAPER;
typedef PAPER FAR *LPPAPER;


typedef struct
    {
    short etmSize;
    short etmPointSize;
    short etmOrientation;
    short etmMasterHeight;
    short etmMinScale;
    short etmMaxScale;
    short etmMasterUnits;
    short etmCapHeight;
    short etmXHeight;
    short etmLowerCaseAscent;
    short etmLowerCaseDescent;
    short etmSlant;
    short etmSuperScript;
    short etmSubScript;
    short etmSuperScriptSize;
    short etmSubScriptSize;
    short etmUnderlineOffset;
    short etmUnderlineWidth;
    short etmDoubleUpperUnderlineOffset;
    short etmDoubleLowerUnderlineOffset;
    short etmDoubleUpperUnderlineWidth;
    short etmDoubleLowerUnderlineWidth;
    short etmStrikeOutOffset;
    short etmStrikeOutWidth;
    WORD etmNKernPairs;
    WORD etmNKernTracks;
    }ETM;
typedef ETM FAR *LPETM;



/* The format of the printer font metrics file */
typedef struct
    {
    WORD dfVersion;
    DWORD dfSize;
    char dfCopyright[60];
    FONTINFO df;
    }PFM;
typedef PFM FAR *LPPFM;


/* The extended FONTINFO structure */
typedef struct
    {
    /* The first group of fields is set each time the font is used */
    LPFONTINFO lpdf;	/* A back ptr to the fontinfo structure */
    LPSTR lszFont;	/* Far ptr to the font name */
    LPSTR lszFace;	/* Far ptr to the face name */
    LPSTR lszDevice;	/* Far ptr to the device name */
    LPKT  lpkt; 	/* Far ptr to the track kerning table */
    LPKP  lpkp; 	/* Far ptr to the pair kerning table */
    short far *rgWidths;/* Far ptr to the width table */

    /* The second group of fields are set when the font is realized */
    int iFont;		/* The font number */
    int sx;		/* The horizontal scale factor */
    int sy;		/* The vertical scale factor */
    int orientation;	/* The character rotation angle */
    int escapement;	/* The escapement vector angle */
#ifdef WIN20
    int fxAvgWidth;     /* The unscaled average width */
#endif
    long lid;		/* The font instance id */
    DWORD dfFont;
    }FX;
typedef FX FAR *LPFX;


typedef FONTINFO DF;
typedef FONTINFO FAR *LPDF;
typedef DRAWMODE DM;
typedef DRAWMODE FAR *LPDM;
typedef TEXTXFORM FT;
typedef TEXTXFORM FAR *LPFT;

typedef EXTTEXTDATA ETD;
typedef EXTTEXTDATA FAR *LPETD;
typedef APPEXTTEXTDATA ATD;
typedef APPEXTTEXTDATA FAR *LPATD;
typedef FARPROC LPFN;



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


/* The device header:
 * A new copy of this structure is allocated for each instance of the
 * printer driver.  Note: since a single data segment is shared
 * amoung all instances, unshared variables must be stored in the
 * device header.
*/
typedef struct
    {
    short	iType;	    /* The device type (non-zero) */
    BITMAP	bm;	    /* A dummy memory bitmap for BitBlt */
    BOOL	fContext;   /* TRUE for InfoContext (DH only) */
    BOOL	fIsClipped; /* TRUE if a ClipBox is active */
    BOOL	fLandscape; /* Page orientation (TRUE=LANDSCAPE) */
    BOOL	fDirty;     /* TRUE if the current page is dirty */
    int 	iPaperSource;	/* The current paper source */
    BOOL	fPairKern;  /* TRUE if pairwise kerning is turned on */
    BOOL	fIntWidths; /* TRUE for integer character widths */
    HDC 	hdc;	    /* The device's display context */
    HANDLE	hmod;	    /* The module handle, also called hinst */
    HANDLE	hFontDir;   /* The font directory handle */
    short	fh;	    /* The output file handle or job number */
    unsigned	cbSpool;    /* The current byte offset into the spool buffer */
    unsigned	cbSpoolMax; /* The size of the spool buffer */
    short	iStatus;    /* The current status of the document */
    short	iPrinter;   /* The printer type */
    int 	iRes;	    /* The printer resolution in dots per inch */
    int 	iCopies;    /* The number of copies to print */
    int 	iTrack;     /* The track-kerning track number. */
    PAPER	paper;	    /* The paper metrics */
    short	iBand;	    /* The current band number */
    BR		br;	    /* The current brush */
    PEN 	pen;	    /* The current pen */

    int         iCurLineJoin;       /* The current line join style */
    int         iCurLineCap;        /* The current line cap style */
    int         iCurMiterLimit;     /* The current miter limit */
    int         iNewLineJoin;       /* The new line join style */
    int         iNewLineCap;        /* The new line cap style */
    int         iNewMiterLimit;

    unsigned long int TextColor;
    long	lid;	    /* A long interger used to tag objects (id) */
    long	lidFont;    /* The current font id */

	/* added by Aldus Corporation 19 January 1987
	 * This stuff is to enhance the justification schema used in the driver
	 * to allow for negative justification and word break and letter
	 * justification.
	 */
	JUSTBREAKTYPE epJust;	/*kind of justification*/
	JUSTBREAKREC epJustWB; 	/*justification rec for word breaks*/
	JUSTBREAKREC epJustLTR;	/*justification rec for letters*/

    }DH;
typedef DH far *LPDH;



/* The device descriptor structure */
typedef struct
    {
    DH	    dh;						/* The device header */
    char    rgbSpool[CBSPOOLMAX];   /* The output spool buffer */
    char    rgbBand[CBBANDMAX];     /* The band bitmap */
    char	szDevice[CBNAME+1];		/* device name */
    char    szFile[40];				/* The start of the output file name */
    }DV;
typedef DV far *LPDV;


/* The aspect ratio structure */
typedef struct
    {
    char    bError;	    /* The initial style error status */
    char    bHypotenuse;    /* The hypotenuse */
    char    bx; 	    /* The x major distance */
    char    by; 	    /* The y major distance */
    }ASPECT;


HANDLE FAR  PASCAL OpenJob(LPSTR, LPSTR, HANDLE);
short FAR  PASCAL StartSpoolPage(HANDLE);
short FAR  PASCAL EndSpoolPage(HANDLE);
short FAR  PASCAL WriteSpool(HANDLE, LPSTR, short);
short FAR  PASCAL CloseJob(HANDLE);
short FAR  PASCAL DeleteJob(HANDLE, short);
short FAR  PASCAL WriteDialog(HANDLE, LPSTR, short);
long  FAR  PASCAL QueryJob(HANDLE, short);
short FAR  PASCAL QueryAbort(HANDLE, short);


/* _SPOOL constants for queryjob */
#define SP_QUERYVALIDJOB    30
#define SP_QUERYDISKAVAIL   0x1004


int FAR PASCAL RealizeFont(DV FAR *, LPLOGFONT, LPFONTINFO, TEXTXFORM FAR *);
extern int PASCAL RealizePen(DV FAR *, LPLOGPEN, PEN FAR *);
extern int PASCAL RealizeBrush(DV FAR *, LPLOGBRUSH, BR FAR *);
extern void far Print();

extern void PASCAL StrToDev(DV FAR *, LPSTR, int);


extern int FAR PASCAL OpenChannel(DV FAR *, LPSTR);
extern void FAR PASCAL CloseChannel(DV FAR *);
extern void FAR PASCAL FlushChannel(DV FAR *);
extern void FAR PASCAL WriteChannel(DV FAR *, LPSTR, int);

extern void FAR PrintChannel();     /* Variable parameter count! */
extern int FAR PASCAL Scale(int, int, int);


void PASCAL NewPath(DV FAR *, int);
void PASCAL LineTo(DV FAR *, int, int);
extern void PASCAL MoveTo(DV FAR *, int, int);

extern void FAR PASCAL Justify(DV FAR *, DRAWMODE FAR *);

#define PP_NULL     0	/* Null type postscript path */
#define PP_STROKE   1	/* Stroke type Post Script path */
#define PP_FILL     2	/* Fill type Post Script path */



far PASCAL  dmBitblt(LPDV, int, int, BITMAP FAR *, int, int, int, int, long, long, LPDM);
far PASCAL  dmOutput(LPDV, int, int, LPPOINT, PEN FAR *, BR FAR *, DRAWMODE FAR *, LPRECT);
far PASCAL  dmRealizeObject(LPDV, int, LPSTR, LPSTR, LPTEXTXFORM);
LONG far PASCAL  dmStrBlt(LPDV, int, int, LPRECT, LPSTR, int, LPDF, DRAWMODE FAR *, LPTEXTXFORM);


/****************************** UTIL ********************************/

extern BOOL FAR PASCAL lstrcmp(LPSTR, LPSTR);
extern int 	FAR PASCAL lstrlen(LPSTR);
extern void FAR PASCAL lstrcpy(LPSTR, LPSTR);
extern void FAR PASCAL lstrncat(LPSTR, LPSTR, int);
extern void FAR PASCAL lmemcpy(LPSTR, LPSTR, int);
extern BOOL FAR PASCAL lmemIsEqual(LPSTR, LPSTR, int);

/****************************** DISABLE ******************************/
extern void FAR PASCAL DeleteFontDir(int);
extern int FAR PASCAL Disable(LPDV);


/****************************** RESET  *******************************/
extern void FAR PASCAL SetKey(LPSTR);



LPSTR FAR PASCAL LockFontDir(int);
void FAR PASCAL UnlockFontDir(int);

#ifndef LOCAL_USEAGE
extern int FAR printf();
#endif

LPFX FAR PASCAL LockFont(LPDF);

LPSTR FAR PASCAL GetResourceData(HANDLE,LPHANDLE,LPSTR,LPSTR);
BOOL  FAR PASCAL UnGetResourceData(HANDLE);

