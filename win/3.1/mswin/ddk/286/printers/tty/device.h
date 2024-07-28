// DEVICE.H for TTY
//
// Copyright (c) 1989-1990, Microsoft Corporation.
//
//----------------------------------------------------------------------------
//	Microsoft history
//	14 nov 89	peterbe		Correct spelling error.
//	22 oct 89	peterbe		TRANS_MIN = 80h now.
//	20 oct 89	peterbe		checked in.
//----------------------------------------------------------------------------

#define MYCHANGE    1


#define BLOCK_SIZE	512
#define NAME_LEN	32
#define LINE_LEN	80	/* length of a typical line */

#define	 HEADERSIZE	 66 /* FONTINFO header size */

#define INITQSIZE	(100)		/* 100 initial size */
#define INCRQSIZE	(50)		/* 50 incr. when above is too small */


/*      Font weights lightest to darkest.                               */
#define FW_DONTCARE		0
#define FW_THIN			100
#define FW_EXTRALIGHT		200
#define FW_LIGHT		300
#define FW_NORMAL		400
#define FW_MEDIUM		500
#define FW_SEMIBOLD		600
#define FW_BOLD			700
#define FW_EXTRABOLD		800
#define FW_HEAVY		900

#define FW_ULTRALIGHT		FW_EXTRALIGHT
#define FW_REGULAR		FW_NORMAL
#define FW_DEMIBOLD		FW_SEMIBOLD
#define FW_ULTRABOLD		FW_EXTRABOLD
#define FW_BLACK		FW_HEAVY

#define TRANS_MIN   ((BYTE) 0x80)

typedef struct
{
    short y,
	  x,
	  wheel,
	  size,	    /* width of this string in XM */
	  count;    /* number of chars in the output string */
}   ELEHDR;

typedef struct
{
    short y,
	  x,
	  wheel,
	  size,
	  count;
    char  pstr[2];     /* offset to base of string object */
}   ELEMENT;

typedef struct
{
    char first;
    char last;
    short sub;
}   CTT;


/* heap structure :

    base                                                    base + start
    |                                                            |
   \|/                                                          \|/
    string chars (ELEMENT)--->                              index ---->
*/

typedef struct
{
    short code;		/* code for what kind of paper */
    short XPhys;	/* logical paper size in x direction (dots) */
    short YPhys;	/* logical paper size in y direction (dots)  */
    short XPhysMM;	/* physical page size in tenths of milimeters in x */
    short YPhysMM;	/* physical page size in tenths of milimeters in y */
    short FormLength;
    short VOffset;	/* physical offset -- move the printer down by so much
                           before start to print the first scan line */
} PAPERFORMAT;

/* block of printer escape sequences */
typedef struct{
	BYTE *code;
	BYTE length;
}ESCAPEPAIR, FAR * LPESCAPEPAIR;

typedef struct{
	ESCAPEPAIR compress_on;
	ESCAPEPAIR pica_on;
	ESCAPEPAIR elite_on;
	ESCAPEPAIR expand_on;
	ESCAPEPAIR expand_off;
	ESCAPEPAIR  reset;
}ESCAPECODE, FAR * LPESCAPECODE;

typedef struct
{
    short	iType  ;	/* DEV_LAND means landscape device
                                   DEV_PORT means portrait device */
    ESCAPECODE	escapecode;	// put escape code on pdevice
    BITMAP	epBmpHdr;	/* bitmap structure */
    PAPERFORMAT *pf;		/* paper format */
    short	hDC;		/* apps's callback abort proc */
    short	bInfo;		/* info context */
    short	nBand;		/* nth band */
    short	epXOffset;
    short	epYOffset;
    short	hJob;		/* job number */
    short	status; 	/* job status */
    HANDLE	epYPQ;		/* y priority queue */
    short	epXRemain;	/* keep round off errors for MS */
    short	epXcurwidth;	/* current x width */
    short	epYcurwidth;
    short	sCurx;	       /* current x position points */
    short	sCury;
    HANDLE	epHeap;
    short	epHPsize;
    short	epHPptr;
    short	epXCursPos;	/* actual cursor position in draftmode */
    char	epPort[ NAME_LEN ];
    short	epBuffSet;

    LPSTR      epBuf;		/* pointer to buffer */
    LPSTR      epBmp;		/* pointer to band's bitmap */

    short      epPageWidth; 	/* storage of current PG_ACROSS value */
    short      epPageHeight;
    short      epPageWidthInch;
    short      bExpandOn;
    short      bPageBreak;
    short      bCutSheet;
    short      bFirstCharacter;
    short      epCount;
    unsigned   oSpool;		/* spool buffer pointer */
    char       chSpool[CCHSPOOL];/* pointer to spool buffer */
} DEVICE, far *LPDV;

typedef struct
{
    char InitStyleError;
    char Hypoteneuse;
    char XMajorDistance;
    char YMajorDistance;
} ASPECT;

typedef struct {
	char esc;
	char code;
	short cnt;
	} GRAPHRESET;

typedef struct	{
	char esc;
	char code;
	unsigned char cnt;
	char lf;
	char length;
	char mult; // ratio of escape control resolution to logical resolution
}	DELY;

/* the following defines are used only by the epson driver to select
   in different country character sets, and to define the special
   width table */
typedef struct {
	BYTE esc;
	BYTE code;
	BYTE country;
	BYTE actualchar;
	BYTE esc1;
	BYTE code2;
	BYTE country2;
} COUNTRYESCAPE;

typedef struct {
	BYTE charvalue;
	BYTE upright;
	BYTE italic;
} INT_WIDTH;


typedef struct{
	BYTE Cyan;
	BYTE Magenta;
	BYTE Yellow;
	BYTE Mono;  /* (bit 0): C; (bit 1): M; (bit 2): Y;
               (bit 6): Monochrome */
} PHYSCOLOR, far *LPPHYSCOLOR;

typedef struct {
    short	x;
    short	y;
    int		count;
    RECT	ClipRect;
    LPSTR	lpStr;
    short	far *lpWidths;
} APPEXTTEXTDATA;

typedef APPEXTTEXTDATA	*PAPPEXTTEXTDATA;

typedef APPEXTTEXTDATA FAR *LPAPPEXTTEXTDATA;


typedef struct {
    short	    nSize;
    LPAPPEXTTEXTDATA	lpInData;
    LPFONTINFO		lpFont;
    LPTEXTXFORM		lpXForm;
    LPDRAWMODE		lpDrawMode;
} EXTTEXTDATA;

typedef EXTTEXTDATA FAR *LPEXTTEXTDATA;

typedef struct {
    short	    nSize;
    LPSTR		lpInData;
    LPFONTINFO		lpFont;
    LPTEXTXFORM		lpXForm;
    LPDRAWMODE		lpDrawMode;
} EXTWIDTHDATA;

typedef EXTWIDTHDATA FAR *LPEXTWIDTHDATA;

/* driver's own stuff */

/* devmode dependiente */
typedef struct {
    DEVMODE	    dm;			  /* standard device mode */
    short	    feed;		  /* cut sheet or continuous */
    short	    paper;		  /* paper tpe */
    short	    use_wide_carriage;	  /* Boolean - wide or standard width */
    short	    page_break;		  /* ignorar form feeds */
} EXDEVMODE, FAR *LPDM;


#if 0
far PASCAL  dmBitblt(LPDV, short, short, BITMAP far *, short,
	short, short, short, long, long, long);
far PASCAL  dmEnumObj(LPDV, short, long, long);
far PASCAL  dmOutput(LPDV, short, short, LPPOINT, long, long, long, long );
far PASCAL  dmPixel(LPDV, short, short, long,long);
far PASCAL  dmRealizeObject(LPDV, short, LPSTR, LPSTR, LPSTR);
LONG far PASCAL  dmStrBlt(LPDV, short, short, LPRECT, LPSTR, short,
	LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
far PASCAL  dmScanLR(LPDV, short, short, long, short);
far PASCAL  dmTranspose(LPSTR, LPSTR, short);
#endif

far PASCAL  dmColorInfo(LPDV, long, long);

void FAR PASCAL Copy(LPSTR, LPSTR, short);
void FAR PASCAL FillBuffer(LPSTR, WORD, WORD);

long FAR PASCAL StrBlt(LPDV, short, short, LPRECT,  LPSTR,
	short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);

LPSTR NEAR PASCAL CheckString(LPSTR, short, LPFONTINFO);


#define ESCEXP(esc)	(LPSTR)(esc).code, (esc).length
