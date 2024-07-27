#define MYCHANGE 	1


#define BLOCK_SIZE      512
#define NAME_LEN        50      /* extended from 32 - ***** ****** 9/26/87 */
#define LINE_LEN        80      /* length of a typical line */

#define DEV_PORT    0x8888
#define DEV_LAND    0x8889

#define DRAFTFLAG       0x01
#define BREAKFLAG       0x02
#define GRXFLAG         0x04    /* flag to see if any graphics output to the buffer */
#define HIGHSPEED       0x08
#define TEXTFLAG        0x10    /* text output to the buffer */
#define INFO            0x20
#define ITALIC_ABLE     0x40    /* MX-80 for ibmgrx.drv */
#define LOSES_FF        0x80    /* loses form feed */
#define WRONG_LINESP    0x100   /* incorrect line spacing when doing double striking */
#define SPECIAL_LINESP  0x200   /* must use control A for line spacing */

typedef TEXTXFORM far * LPTEXTXFORM;
typedef LOGFONT   far * LPLOGFONT;
typedef FONTINFO  far * LPFONTINFO;
typedef DRAWMODE  far * LPDRAWMODE;
typedef TEXTMETRIC far * LPTEXTMETRIC;

#define  HEADERSIZE      66	/* FONTINFO header size */

/*      Font weights lightest to darkest.                               */
#define FW_DONTCARE             0
#define FW_THIN                 100
#define FW_EXTRALIGHT           200
#define FW_LIGHT                300
#define FW_NORMAL               400
#define FW_MEDIUM               500
#define FW_SEMIBOLD             600
#define FW_BOLD                 700
#define FW_EXTRABOLD            800
#define FW_HEAVY                900

#define FW_ULTRALIGHT           FW_EXTRALIGHT
#define FW_REGULAR              FW_NORMAL
#define FW_DEMIBOLD             FW_SEMIBOLD
#define FW_ULTRABOLD            FW_EXTRABOLD
#define FW_BLACK                FW_HEAVY

#define TRANS_MIN   ((BYTE) 0xa0)

BOOL FAR PASCAL TextOut(HANDLE, short, short, LPSTR, short);

int         FAR PASCAL SetRect(LPRECT, int, int, int, int);
int         FAR PASCAL SetRectEmpty(LPRECT);
int         FAR PASCAL CopyRect(LPRECT, LPRECT);
int         FAR PASCAL InflateRect(LPRECT, int, int);
int         FAR PASCAL IntersectRect(LPRECT, LPRECT, LPRECT);
int         FAR PASCAL UnionRect(LPRECT, LPRECT, LPRECT);
int         FAR PASCAL OffsetRect(LPRECT, int, int);
BOOL        FAR PASCAL IsRectEmpty(LPRECT);
BOOL        FAR PASCAL PtInRect(LPRECT, LPPOINT);



typedef struct
{
    short y,
          x,
          wheel,
          size,     /* width of this string in XM */
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

/* short pcd (printer control description) */
typedef struct
{
    short   offset;     /* PCSB releative byte pointer to start of control sequence */
    char    mod;        /* modification byte */
    char    length;     /* length of the control sequence */
}   PCD;

/* long pcd - controls with parameter characters */
typedef struct
{
    short   offset;     /* PCSB releative byte pointer to start of control sequence */
    char    mod;        /* byte */
    char    length;     /* length of the control sequence */
    char    magic;      /* magic number to be added to parameter */
    char    max;        /* max value of parameter */
}   LPCD;

/* pcds are arranged as follow */

typedef struct
{
    PCD   reset_b;      /* reset printer, beginning of document */
    PCD   reset_e;      /* reset printer, end of document */
    LPCD  formlen_f;    /* set form length, first part */
    PCD   formlen_s;    /* set form length, second part */
    LPCD  linesp_f;     /* set line spacing, first part */
    PCD   linesp_s;     /* set line spacing , second part */
    LPCD  charsp_f;     /* set char spacing, first part */
    PCD   charsp_s;     /* set char spacing, second part */
    PCD   bin1_b;       /* bin1 begin - sheet feeder control */
    PCD   bin1_e;       /* bin1 end */
    PCD   bin2_b;       /* bin2 begin - sheet feeder control */
    PCD   bin2_e;       /* bin2 end */
    PCD   line_b;       /* line begin - output at */
    PCD   line_e;       /* beginning of each line sent to the printer */
    PCD   Text_b;       /* text begin */
    PCD   Text_e;       /* text end */
    /*  char  reserved[RES_LEN];    stripped out by convert */
    PCD   bold_b;       /* begin bold */
    PCD   bold_e;       /* end bold */
    PCD   italic_b;     /* begin italic */
    PCD   italic_e;     /* end italic */
    LPCD  underl_b;     /* begin underline */
    PCD   underl_e;
    LPCD  strikeout_b;  /* begin strikeout */
    PCD   strikeout_e;
    LPCD  dunderl_b;    /* begin double underline */
    PCD   dunderl_e;
    LPCD  supers_b;     /* begin superscript */
    PCD   supers_e;     /* end surperscript */
    LPCD  subs_b;       /* begin subscript */
    PCD   subs_e;
}   PCDBLOCK;

typedef struct
{
    char first;
    char last;
    short sub;
}   CTT;

extern short pcdtab[], pcsbtab[];
extern PCDBLOCK *pcdblock;
extern PSTR pcsb;

short       FAR PASCAL AddFontResource( LPSTR);
void        FAR PASCAL RemoveFontResource( LPSTR );

/* heap structure :

    base                                                    base + start
    |                                                            |
   \|/                                                          \|/
    string chars (ELEMENT)--->                              index ---->
*/

typedef struct
{
        char     font[sizeof(FONTINFO) - 1];
        short    wheel;
        PCD      bfont;
        PCD      efont;
        DWORD    widthtable;
} PRDFONTINFO, far * LPPRDFONTINFO;

typedef struct
{
    short code;         /* code for what kind of paper */
    short XPhys;        /* logical paper size in x direction   */
    short YPhys;        /* logical paper size in y direction   */
    short XPrintingOffset;  /* logical printing offset in x direction */
    short YPrintingOffset;  /* logical printing offset in y direction */
    short FormLength;
    short VOffset;      /* physical offset -- move the printer down by so much
                           before start to print the first scan line */
} PAPERFORMAT;


typedef struct
{
        short       epType  ;       /* DEV_LAND means landscape device
                                       DEV_PORT means portrait device */
        BITMAP      epBmpHdr;       /* bitmap structure */
        PAPERFORMAT *epPF;          /* paper format */
        short       ephDC;          /* apps's callback abort proc */
        short       epMode;         /* draft mode */
        short       epNband;        /* nth band */
        short       epXOffset;
        short       epYOffset;
        short       epJob;          /* job number */
        unsigned    epPtr;          /* spool buffer pointer */
        short       epDoc;          /* document flag */
        HANDLE      epYPQ;          /* X priority queue */
        HANDLE      epXPQ;          /* y priority queue */
        short       epXcurwidth;    /* current x width */
        short       epYcurwidth;
        short       epCurx;         /* current x position */
        short       epCury;
        HANDLE      epHeap;
        short       epHPsize;
        short       epHPptr;
        short       epXCursPos;     /* actual cursor position in draftmode */
} DEVICEHDR;

typedef struct
{
        short       epType  ;       /* DEV_LAND means landscape device
                                       DEV_PORT means portrait device */
        BITMAP      epBmpHdr;       /* bitmap structure */
        PAPERFORMAT *epPF;          /* paper format */
        short       ephDC;          /* apps's callback abort proc */
        short       epMode;         /* draft mode */
        short       epNband;        /* nth band */
        short       epXOffset;
        short       epYOffset;
        short       epJob;          /* job number */
        unsigned    epPtr;          /* spool buffer pointer */
        short       epDoc;          /* document flag */
        HANDLE      epYPQ;          /* y priority queue */
        short       epXRemain;      /* keep round off errors for MS */
        short       epXcurwidth;    /* current x width */
        short       epYcurwidth;
        short       epCurx;         /* current x position */
        short       epCury;
        HANDLE      epHeap;
        short       epHPsize;
        short       epHPptr;
        short       epXCursPos;     /* actual cursor position in draftmode */
        char        epPort[ NAME_LEN ];	
	short 	    epBuffSet;		
	
	HANDLE	    epHBuf;	/* handle to buffer */
	HANDLE	    epHBmp;	/* handle to band's bitmap */
	HANDLE	    epHSpool;	/* handle to spool buffer */
	
	LPSTR      epSpool;	/* pointer to spool buffer */
	LPSTR	   epBuf;	/* pointer to buffer */
	LPSTR	   epBmp;	/* pointer to band's bitmap */

        short 	   epPageWidth;	/* storage of current PG_ACROSS value */
} DEVICE, far *LPDEVICE;

typedef struct
{
    char InitStyleError;
    char Hypoteneuse;
    char XMajorDistance;
    char YMajorDistance;
} ASPECT;

#if defined(CITOH)
typedef struct {
	char esc0;	/* escape to put into original graphics mode */
	char code0;
        char esc;
        char code;
        char count_string[5];  /* C-Itoh uses 4 char ascii string for count */
        } GRAPHRESET;
#else
typedef struct {
        char esc;
        char code;
        short cnt;
        } GRAPHRESET;
#endif

#ifdef CITOH
typedef struct  {
        char esc;
        char code;
        char tens_digit;
        char ones_digit;	
        char lf;
        char length;
        char mult;      /* ratio of escape control resolution to logical resolution */	
	char reserved_byte;	/* not presently used */	
}       DELY;
#else
typedef struct  {
        char esc;
        char code;
        unsigned char cnt;
        char lf;
        char length;
        char mult;      /* ratio of escape control resolution to logical resolution */
}       DELY;
#endif

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

/* block of printer escape sequences */
typedef struct{
        BYTE *code;
        BYTE length;
}ESCAPEPAIR;

typedef struct{
        ESCAPEPAIR italic_on;
        ESCAPEPAIR italic_off;
        ESCAPEPAIR bold_on;
        ESCAPEPAIR bold_off;
        ESCAPEPAIR underl_on;
        ESCAPEPAIR underl_off;
        ESCAPEPAIR cr;          /* carriage return */
        ESCAPEPAIR compress_on;
        ESCAPEPAIR pica_on;
        ESCAPEPAIR elite_on;
}ESCAPECODE;

typedef struct{
        BYTE Cyan;
        BYTE Magenta;
        BYTE Yellow;
        BYTE Mono;	/* (bit 0): C; (bit 1): M; (bit 2): Y; 
			   (bit 6): Monochrome */
} PHYSCOLOR, far *LPPHYSCOLOR;

typedef struct {
    short       x;
    short       y;
    int         count;
    RECT        ClipRect;
    LPSTR       lpStr;
    short       far *lpWidths;
} APPEXTTEXTDATA;

typedef APPEXTTEXTDATA	*PAPPEXTTEXTDATA;

typedef APPEXTTEXTDATA FAR *LPAPPEXTTEXTDATA;

typedef struct {
    short    		nSize;
    LPAPPEXTTEXTDATA    lpInData;
    LPFONTINFO          lpFont;
    LPTEXTXFORM         lpXForm;
    LPDRAWMODE          lpDrawMode;
} EXTTEXTDATA;

typedef EXTTEXTDATA FAR *LPEXTTEXTDATA;

typedef struct {
    short    		nSize;
    LPSTR               lpInData;
    LPFONTINFO          lpFont;
    LPTEXTXFORM         lpXForm;
    LPDRAWMODE          lpDrawMode;
} EXTWIDTHDATA;

typedef EXTWIDTHDATA FAR *LPEXTWIDTHDATA;

#define NAME_LEN           50	
/* changed NAMELEN from 32 to 50 (assumed arbitrary) - ***** ****** 9/23/87 */

/* driver's own stuff */

typedef struct
{
    char DeviceName[NAME_LEN];
    short orient;       /* orientation - portrait or landscape */
    short feed;         /* cut sheet or continuous */
    short paper;        /* paper size - letter, dina4, fanfold */
    short res;          /* low or high resolution */

#if COLOR
    short color;        /* black/white or color */
#endif

    short use_wide_carriage;  /* Boolean - wide or standard width */
}   DEVMODE;


far PASCAL  dmBitblt(LPDEVICE, short, short, BITMAP far *, short, short, short, short, long, long, long);
far PASCAL  dmColorInfo(LPDEVICE, long, long);
far PASCAL  dmEnumDFonts(LPDEVICE, long, long, long);
far PASCAL  dmEnumObj(LPDEVICE, short, long, long);
far PASCAL  dmOutput(LPDEVICE, short, short, LPPOINT, long, long, long, long );
far PASCAL  dmPixel(LPDEVICE, short, short, long,long);
far PASCAL  dmRealizeObject(LPDEVICE, short, LPSTR, LPSTR, LPSTR);
LONG far PASCAL  dmStrBlt(LPDEVICE, short, short, LPRECT, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
far PASCAL  dmScanLR(LPDEVICE, short, short, long, short);
far PASCAL  dmTranspose(LPSTR, LPSTR, short);

void FAR PASCAL Copy(LPSTR, LPSTR, short);
void FAR PASCAL FillBuffer(LPSTR, WORD, WORD);

long FAR PASCAL StrBlt(LPDEVICE, short, short, LPRECT,  LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);

LPSTR NEAR PASCAL CheckString(LPSTR, short, LPFONTINFO);


#define ESCEXP(esc)     (LPSTR)(esc).code, (esc).length

