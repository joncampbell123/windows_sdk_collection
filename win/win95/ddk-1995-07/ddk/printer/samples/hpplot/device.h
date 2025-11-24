/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

				   device

*******************************************************************************

*/

/* #define META_SPOOL not needed anymore  5/19/94 MMQ */

/* Driver shell options */
#define COLORSORTING            /* Implement color sorting */
#define MAXCOLORS MAX_PENS * MAX_CAROUSELS  /* Maximum colors allowed */
#define LINEOPTIMIZE            /* Optimize polyline/scanline output */
#define EXCLUDEWHITE            /* Exclude white output */

/* Buffer size for resource string copy */
#define BUFSIZE 80

#define STEPSPERMM 40
/* there are actually 4 plotter units per mm
   -ssn 10/25/91 Old code: #define STEPSPERMM 40
	This should be defined to be 4, but fonts are really
	small in MS Word if we do that.  For now
	I'll leave it alone. --10/30/91 */

/* Additional information at end of FONTINFO */
//#define DF_MAPSIZE 20

#define RESOLUTION      5
#define NUM_PLOTTERS    15
#define MAX_PENS        8
#define MAX_CAROUSELS   6
#define MAX_COLORS      10
#define MAX_PEN_TYPES   15
#define MAX_PEN_GROUPS  5
#define MAX_MEDIA_SIZES 12
#define NUM_OPTIONS     5
#define NUM_PEN_STYLES  5

#define MAX_FEEDS       2
#define MAX_STRING_LENGTH 256

#define PAPER        0
#define TRANSPARENCY 1
#define ROLLERBALL   2
#define DISPOSABLE   3
#define REFILLABLE   4

#define UP 0
#define DOWN 1

#ifdef _WIN31
typedef struct BOOL *PBOOL;
typedef struct BOOL FAR *LPBOOL;
#endif
typedef struct
{
    long Color;                 /* RGB color */
    BOOL Available;             /* Color available? */
} COLORSAVAILABLE;

typedef struct
{
    short Color,                    /* Pen color */
	  Type,                     /* Pen type */
	  Speed,                    /* Pen speed */
	  Force,                    /* Pen force */
	  Acceleration,             /* Pen acceleration */
	  Usage;                    /* Pen usage */
} PENINFO;
typedef PENINFO FAR *LPPENINFO;

typedef struct
{
    PENINFO    Pen [MAX_PENS];           /* Pen information */
} CARSLINFO;

// don added
#define CCHDEVNAME   64
#define CCHPAPERNAME 64
#define CCHBINNAME   24

/* Pen Styles */
#define PS_SOLID        0
#define PS_DASH         1
#define PS_DOT          2
#define PS_DASHDOT      3
#define PS_DASHDOTDOT   4
#define PS_NULL         5
#define PS_INSIDEFRAME  6

/* device capabilities indices */
#define DC_FIELDS           1
#define DC_PAPERS           2
#define DC_PAPERSIZE        3
#define DC_MINEXTENT        4
#define DC_MAXEXTENT        5
#define DC_BINS             6
#define DC_DUPLEX           7
#define DC_SIZE             8
#define DC_EXTRA            9
#define DC_VERSION          10
#define DC_DRIVER           11
#define DC_BINNAMES         12
#define DC_ENUMRESOLUTIONS  13
#define DC_FILEDEPENDENCIES 14
#define DC_TRUETYPE         15
#define DC_PAPERNAMES       16
#define DC_ORIENTATION      17
#define DC_COPIES           18

/* bit fields of the return value (DWORD) for DC_TRUETYPE */
#define DCTT_BITMAP         0x0000001L
#define DCTT_DOWNLOAD       0x0000002L
#define DCTT_SUBDEV         0x0000004L


typedef struct
{
   DEVMODE    dm;
   char       ModelName[CCHDEVICENAME];
   short      Plotter,                        /* Plotter type */
	      CurrentCarousel;                /* Current carousel */
   CARSLINFO  Carousel [MAX_CAROUSELS];       /* Carousel information */
   BOOL       ActiveCarousel[6],              /* Active carousel information */
	      Draft,                          /* Draft output? */

	      Preloaded;                      /* Paper preloaded? */
   short      Orientation,                    /* Orientation */
	      PaperFeed,                      /* Paper feed */
	      Size,                           /* Media size */
	      Length;                         /* Roll length */
} ENVIRONMENT;
typedef ENVIRONMENT *PENVIRONMENT;
typedef ENVIRONMENT FAR *LPENVIRONMENT;

#ifdef COLORSORTING
typedef struct
{
    DWORD PhysicalColor;        /* Physical color of entry */
    BOOL Used,                  /* Have we output this color? */
	 Current;               /* Are we currently outputing it? */
} COLORENTRY;
#endif

typedef struct
{
    DWORD PhysicalColor;                /* REQUIRED: Pen Physical Color */
    short Style;                        /* Style of pen */
    POINT Width;                        /* Width of pen */
} PPEN;
typedef PPEN *PPPEN;
typedef PPEN FAR *LPPPEN;

typedef struct
{
    short Type;                         /* REQUIRED: Bitmap or Device */
    BOOL Spooling;                      /* REQUIRED: Current spooling state */
#ifdef COLORSORTING
    short NumColors;                    /* Number of colors in color table */
    BOOL ColorTableBuilt,               /* Color table built? */
	 ColorRealized;                 /* Current color realized? */
    COLORENTRY ColorTable [MAXCOLORS];  /* Color table used for sorting */
    RECT CurrentBand;                   /* Current band boundary */
#endif
#ifdef LINEOPTIMIZE
    POINT PenPos;                       /* Current position of pen */
#endif
    short NumPens,                      /* Number of pens for drawing */
	  NumBands,                     /* Number of bands for drawing */
	  NumBandsCompleted,            /* Current band */
	  hJob,                         /* Spooling job number */
	  CurrentHeight,                /* Current selected character height */
	  CurrentWidth,                 /* Current selected character width */
	  PenStatus,                    /* Pen status */
	  StartingCarousel,             /* Starting carousel */
	  CurrentPenStyle,              /* Current style */
	  CurrentSpeed,                 /* Current pen speed */
	  CurrentForce,                 /* Current pen force */
	  CurrentAcceleration,          /* Current pen acceleration */
	  CurrentBrushStyle,            /* Current brush style */
	  CurrentBrushAngle,            /* Current brush angle */
	  CurrentBrushHatch,            /* Current brush hatch pattern */
	  CurrentPenThickness,          /* Current pen thickness */
	  LastBandLength,               /* Length of roll feed band */
	  AltCharSet;                   /* Current alternate character set */
    BOOL TextSlanted,                   /* Text slanted? */
	 LineInConstruction,            /* Line still in construction? */
	 LocatingPen,                   /* Currently locating next pen? */
	 StandardCharSetActive,         /* Standard character set active? */
	 PageStarted;                   /* Page started? */
    unsigned VertRes,                   /* Vertical resolution */
	     HorzRes,                   /* Horizontal resolution */
	     LengthRes;                 /* Resolution of full length band */
    long BandOffset;                    /* Band offset for roll feed */
    DWORD SavePhysColor;                /* Save physical color looking for */
    HDC hDC;                            /* Handle to output display context */
    LPPPEN CurPen;                      /* Current pen for polygon scanlines */
    char OutputFile [20];               /* Output port */
    COLORSAVAILABLE                     /* Colors Available */
      ColorsAvailable [MAX_COLORS];
    ENVIRONMENT Setup;                  /* Environment setup */
    BOOL        SpoolPageStarted;               /* Kludge 25JAN90 for openfile err */
    BOOL bFile;
    BOOL bFirstPage;
} PDEVICE;
typedef PDEVICE *PPDEVICE;
typedef PDEVICE FAR *LPPDEVICE;

typedef struct
{
    DWORD PhysicalColor;                /* REQUIRED: Brush Physical Color */
    short Style;                        /* Style of brush */
    short Hatch;                        /* Hatch pattern */
} PBRUSH;
typedef PBRUSH *PPBRUSH;
typedef PBRUSH FAR *LPPBRUSH;
