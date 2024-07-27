/*********************************************************************
 * PRINTERS.H
 *
 * 14Apr87	sjp		Creation date.
 * 17Apr87	sjp		Added all printer specific constants, etc. so that
 *					all necessary info (except what is in PSCRIPT.RC)
 *					that is necessary to add, del, modify printers is
 *					in this file.  To modify logic, consult CONTROL.C.
 * 4Jun87	sjp		Added gPaperBins[][] array.
 *********************************************************************
 */


/* this is set so that it is 20 inches in picas
 * and large enough to accomodate the Linotype at 1270 dpi
 */
#define IRESMAX     1440
#define DEFAULTRESOLUTION   300

#define DEFAULTORIENTATION FALSE

/* length of special descriptor string that can replace the 
 * "Upper tray" default entry in the print dialog
 */
#define SPECIALSTRINGSIZE 32


/* if this is defined then do not include the confidential printers
 * in the driver.
 */
/* #define CONFIDENTIAL */

/* printer index list */

#define APPLE			0
#define APPLEPLUS		1
#define DPLZR2665		2
#define LN03R			3
#define PRINTSERVER40	4
#define IBM1            5
#define LINOTYPE		6
#define QMSPS800		7
#define QMSPS80P		8
#define OMNILASER2108	9
#define OMNILASER2115   10
#define WANGLCS15       11
#define WANGLCS15FP     12
#define NULL_PR			13

/* index of the start of the confidential printer list */
#ifdef CONFIDENTIAL
#define CPRINTERS       11
#else
#define CPRINTERS       13
#endif


/* device mode constants */

#define PORTRAIT    10
#define LANDSCAPE   11
#define CONTFEED    12
#define CUTSHEET    13

#define IPAPERMIN   	LETTER
#define LETTER	    15
#define LEGAL	    16
#define TABLOID	    17
#define STATEMENT	18
#define DINA3	    19
#define DINA4	    20
#define DINA5	    21
#define DINB4		22
#define DINB5		23
/* #define NOTE	    24 */
#define IPAPERMAX   	DINB5

#define CPAPERS     (IPAPERMAX - IPAPERMIN + 1)


#define EDITFIELD   25
#define LISTFIELD   26
#define COPIES	    27


#define IFEEDMIN		MANUALFEED
#define MANUALFEED  28
#define UPPERTRAY   29
#define LOWERTRAY   30
#define IFEEDMAX		LOWERTRAY


#define RESMIN			RES75
#define RES75		34
#define RES100		35
#define RES150		36
#define RES300		37
#define RESCUSTOM	38
#define RESMAX			RESCUSTOM


/* the start of IDS constants */
#define IDS_FIRST   40


#define IDS_PRMIN		40
#define IDS_PR1		40	/* Apple LaserWriter */
#define IDS_PR2		41	/* Apple LaserWriter Plus */
#define IDS_PR3		42	/* Dataproducts LZR 2665 */
#define IDS_PR4		43	/* Digital LN03R ScriptPrinter */
#define IDS_PR5		44	/* Digital LPS PrintServer 40 */
#define IDS_PR6		45	/* IBM Personal Pageprinter */
#define IDS_PR7		46	/* Linotronic 100/300/500 */
#define IDS_PR8		47	/* QMS-PS 800 */
#define IDS_PR9		48	/* QMS-PS 800+ */
#define IDS_PR10	49	/* TI OmniLaser 2108 */
#define IDS_PR11	50	/* TI OmniLaser 2115 */
#define IDS_PR12	51	/* Wang LCS15 */
#define IDS_PR13	52	/* Wang LCS15 FontPlus */
#ifdef CONFIDENTIAL
#define IDS_PRMAX		50
#else
#define IDS_PRMAX		52
#endif

#define IDS_PAPERMIN	60
#define IDS_PAPER0  60	    /* US Letter */
#define IDS_PAPER1  61	    /* Legal */
#define IDS_PAPER2	62		/* Tabloid (11x17) */
#define IDS_PAPER3	63		/* Statement */
#define IDS_PAPER4  64	    /* DIN A3 */
#define IDS_PAPER5  65	    /* DIN A4 */
#define IDS_PAPER6  66	    /* DIN A5 */
#define IDS_PAPER7	67		/* DIN B4 */
#define IDS_PAPER8	68		/* DIN B5 */
#define IDS_PAPERMAX	68

#define IDS_DLOADED	80		/* for header-downloaded flag */
#define IDS_YES		81		/* "yes" */
#define IDS_NO		82		/* "no" */
#define IDS_TM		83		/* for "tile mode" flag */
#define IDS_LIE		84		/* for "margins" flag */

/* special tray entries */
#define SPECIALMIN		90
#define IDS_AUTO	90		/* for auto tray select */
#define IDS_UPPER	91		/* for clearing special tray select */
#define IDS_CASSETTE 92		/* for Lino output cassette select */
#define SPECIALMAX		92


#define NAME_LEN   32



/* NEW STUFF...14 April 87--sjp */

#define NUMPRINTERS	(IDS_PRMAX-IDS_PRMIN+1)
#define NUMPAPERS	(IDS_PAPERMAX-IDS_PAPERMIN+1)
#define NUMFEEDS	(IFEEDMAX-IFEEDMIN+1)
#define NUMBINS		(NUMFEEDS+SPECIALMAX-SPECIALMIN+1)
/* end of NEW STUFF */


#define PAPERSIZES	1
#define ZEROMARGINS	2
#define NEGMARGINS	3
#define PAPERBINS	4
#define BIN2FEEDMAP	5


#if DEFAULTRES==100
/* paper metrics given that resolution=100dpi */

#define LET_W 850
#define LET_H 1100
#define LET_DX 30
#define LET_DY 35

#define LEG_W 850
#define LEG_H 1400
#define LEG_DX 30
#define LEG_DY 35
#define LEG_LM_DX 90
#define LEG_LM_DY 52

#define TAB_W 1100
#define TAB_H 1700
#define TAB_DX 30
#define TAB_DY 35
#define TAB_LM_DX 90
#define TAB_LM_DY 50

#define STM_W 550
#define STM_H 850
#define STM_DX 30
#define STM_DY 35

#define DA3_W 1169
#define DA3_H 1654
#define DA3_DX 35
#define DA3_DY 35

#define DA4_W 827
#define DA4_H 1169
#define DA4_DX 35
#define DA4_DY 35

#define DA5_W 583
#define DA5_H 827
#define DA5_DX 35
#define DA5_DY 35

#define DB4_W 984
#define DB4_H 1390
#define DB4_DX 30
#define DB4_DY 35

#define DB5_W 693
#define DB5_H 984
#define DB5_DX 30
#define DB5_DY 35

#endif


/*********************************************************************
 * NOSTRUCTURES is defined, then ignore all printer structure related
 * data in this file.  This is necessary so that PSCRIPT.RC does not
 * go crazy when it sees this stuff.
 *********************************************************************
 */
#ifndef NOSTRUCTURES

/* A description of the device mode dialog's parameter's */
typedef struct
    {
    short iPrinter;	/* The printer type */
    short fLandscape;	/* orientation - portrait or landscape */
    short iPaper;	/* The current paper type */
    short iRes; 	/* The device resolution */
    short iCopies;	/* The number of copies to print */
    short iFeed;        /* The paper source */
    short rgiPaper[IFEEDMAX-IFEEDMIN+1];  /* 1 paper type per paper source */
    char  szDevice[CBNAME+1];
    }DEVMODE;
typedef DEVMODE FAR *LPDEVMODE;

/* printer capability structure--used to consolidate the individual
 * printer's capabilities and requirements.
 */
typedef struct{
	BOOL paper[NUMPAPERS];
/*	 	 LETTER
 * 		 LEGAL
 *		 TABLOID
 *		 STATEMENT
 *		 DINA3
 *		 DINA4
 *		 DINA5
 *		 DINB4
 *		 DINB5
 */
	BOOL feed[NUMFEEDS];
/*		MANUALFEED
 *		UPPERTRAY
 *		LOWERTRAY
 */
	WORD special;
/* IDS_UPPER | IDS_AUTO | IDS_CASSETTE 
 */
	POINT imageDelta[NUMPAPERS];
/* list of paper metrics for each of the paper sizes
 */
} PRINTER;
typedef PRINTER FAR *LPPRINTER;




#ifdef GLOBALSTUFF

#ifdef GPAPERSIZES
POINT gPaperSizes[NUMPAPERS]={
	{LET_W, LET_H},
	{LEG_W, LEG_H},
	{TAB_W, TAB_H},
	{STM_W, STM_H},
	{DA3_W, DA3_H},
	{DA4_W, DA4_H},
	{DA5_W, DA5_H},
	{DB4_W, DB4_H},
	{DB5_W, DB5_H}
};
#else
extern POINT gPaperSizes[];
#endif

#define DEFAULT_IMAGE_DELTAS \
	{LET_DX, LET_DY},\
	{LEG_DX, LEG_DY},\
	{TAB_DX, TAB_DY},\
	{STM_DX, STM_DY},\
	{DA3_DX, DA3_DY},\
	{DA4_DX, DA4_DY},\
	{DA5_DX, DA5_DY},\
	{DB4_DX, DB4_DY},\
	{DB5_DX, DB5_DY}

#define LOWMEMORY_IMAGE_DELTAS \
	{LET_DX, LET_DY},\
	{LEG_LM_DX, LEG_LM_DY},\
	{TAB_LM_DX, TAB_LM_DY},\
	{STM_DX, STM_DY},\
	{DA3_DX, DA3_DY},\
	{DA4_DX, DA4_DY},\
	{DA5_DX, DA5_DY},\
	{DB4_DX, DB4_DY},\
	{DB5_DX, DB5_DY}

#define ZERO_IMAGE_DELTAS \
	{0, 0},	/* letter */\
	{0, 0},	/* legal */\
	{0, 0},	/* tabloid--11x17 */\
	{0, 0},	/* statement--half paper */\
	{0, 0},	/* a3 */\
	{0, 0},	/* a4 */\
	{0, 0},	/* a5 */\
	{0, 0},	/* b4 */\
	{0, 0}	/* b5 */

/***********************************************************************
 * LIES LIES LIES LIES LIES LIES LIES LIES LIES LIES LIES LIES LIES LIES
 ***********************************************************************
 */

#ifdef PRINTERLIES
/* little lie approach:  the idea is not to report the real printable area,
 * because conversion to scanlines happens with alarming frequency, and
 * is deadly.  Besides, it's too hard to keep up with printable areas for
 * each printer. Uses win.ini flag--"margins=no".
 */
PAPER rgPaper[NUMPAPERS] = {
	{LETTER,  LET_W, LET_H, LET_W, LET_H, 0, 0},
	{LEGAL ,  LEG_W, LEG_H, LEG_W, LEG_H, 0, 0},
	{TABLOID, TAB_W, TAB_H, TAB_W, TAB_H, 0, 0},
	{STATEMENT, STM_W, STM_H, STM_W, STM_H, 0, 0},
	{DINA3,   DA3_W, DA3_H, DA3_W, DA3_H, 0, 0},
	{DINA4,   DA4_W, DA4_H, DA4_W, DA4_H, 0, 0},
	{DINA5,   DA5_W, DA5_H, DA5_W, DA5_H, 0, 0},
	{DINB4,   DB4_W, DB4_H, DB4_W, DB4_H, 0, 0},
	{DINB5,   DB5_W, DB5_H, DB5_W, DB5_H, 0, 0},
};


/* big lie approach:  for doing tiling, you want to image an area much
 * larger than the physical page.  So, we tell gdi that this is the case.
 * the majority of apps do strange things, so we make it depend on a
 * win.ini flag--"Tile Mode=yes" invalidates "margins=no".
 */
#define PA 4550 /* about 4550 is probably the limit, due to twips conversions */
#define LET_X (-(PA/2))
#define LET_Y (-(PA/2))
#define LEG_X (-(PA/2))
#define LEG_Y (-(PA/2))
#define TAB_X (-(PA/2))
#define TAB_Y (-(PA/2))
#define STM_X (-(PA/2))
#define STM_Y (-(PA/2))
#define DA3_X (-(PA/2))
#define DA3_Y (-(PA/2))
#define DA4_X (-(PA/2))
#define DA4_Y (-(PA/2))
#define DA5_X (-(PA/2))
#define DA5_Y (-(PA/2))
#define DB4_X (-(PA/2))
#define DB4_Y (-(PA/2))
#define DB5_X (-(PA/2))
#define DB5_Y (-(PA/2))

PAPER pmPaper[NUMPAPERS] = {
	{LETTER,  LET_W, LET_H, PA, PA, LET_X, LET_Y},  /* the big lie... */
	{LEGAL ,  LEG_W, LEG_H, PA, PA, LEG_X, LEG_Y},
	{TABLOID, TAB_W, TAB_H, PA, PA, TAB_X, TAB_Y},
	{STATEMENT,STM_W, STM_H, PA, PA, STM_X, STM_Y},
	{DINA3,   DA3_W, DA3_H, PA, PA, DA3_X, DA3_Y},
	{DINA4,   DA4_W, DA4_H, PA, PA, DA4_X, DA4_Y},
	{DINA5,   DA5_W, DA5_H, PA, PA, DA5_X, DA5_Y},
	{DINB4,   DB4_W, DB4_H, PA, PA, DB4_X, DB4_Y},
	{DINB5,   DB5_W, DB5_H, PA, PA, DB5_X, DB5_Y},
};
#endif
/**********************************************************************
 * end of LIES
 **********************************************************************
 */
#ifdef NULLPRINTERCAPS
/* Null Printer */
PRINTER gNullPrCaps={
	{FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE},
	{FALSE,FALSE,FALSE},
	IDS_UPPER,
	{
		DEFAULT_IMAGE_DELTAS
	}
};
#else
extern PRINTER gNullPrCaps;
#endif



#ifdef PRINTERCAPS
PRINTER gPrCaps[NUMPRINTERS]={
	{/* Apple LaserWriter */
		{TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,TRUE},
		{TRUE,TRUE,FALSE},
		IDS_UPPER,
		{
			LOWMEMORY_IMAGE_DELTAS
		}
	},
	{/* Apple LaserWriter Plus */
		{TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,TRUE},
		{TRUE,TRUE,FALSE},
		IDS_UPPER,
		{
			LOWMEMORY_IMAGE_DELTAS
		}
	},
	{/* Dataproducts LZR 2665 */
		{TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
		{TRUE,TRUE,FALSE},
		IDS_AUTO,	/* if auto then upper=true lower=false */
		{
			DEFAULT_IMAGE_DELTAS
		}
	},
	{/* Digital LN03R ScriptPrinter */
		{TRUE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE},
		{FALSE,TRUE,FALSE},
		IDS_UPPER,
		{
			DEFAULT_IMAGE_DELTAS
		}
	},
	{/* Digital LPS PrintServer 40 */
		{TRUE,TRUE,TRUE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE},
		{FALSE,TRUE,TRUE},
		IDS_UPPER,
		{
			DEFAULT_IMAGE_DELTAS
		}
	},
	{/* IBM Personal Pageprinter */
		{TRUE,TRUE,FALSE,TRUE,FALSE,TRUE,TRUE,FALSE,TRUE},
		{FALSE,TRUE,FALSE},
		IDS_UPPER,
		{
			{22, 27},	/* letter */
			{22, 27},	/* legal */
			{0, 0},	/* tabloid--11x17 */
			{24, 27},	/* statement--half paper */
			{0, 0},	/* a3 */
			{24, 27},	/* a4 */
			{25, 27},	/* a5 */
			{0, 0},	/* b4 */
			{24, 27}	/* b5 */
		}
	},
	{/* Linotronic 100/300/500 */
		{TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
		{FALSE,TRUE,FALSE},
		IDS_CASSETTE,	/* if cassette then upper=true lower=false */
		{
			ZERO_IMAGE_DELTAS
		}
	},
	{/* QMS-PS 800 */
		{TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE},
		{TRUE,TRUE,FALSE},
		IDS_UPPER,
		{
			LOWMEMORY_IMAGE_DELTAS
		}
	},
	{/* QMS-PS 800+ */
		{TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE},
		{TRUE,TRUE,FALSE},
		IDS_UPPER,
		{
			LOWMEMORY_IMAGE_DELTAS
		}
	},
	{/* TI OmniLaser 2108 */
		{TRUE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE},
		{FALSE,TRUE,FALSE},
		IDS_UPPER,
		{
			LOWMEMORY_IMAGE_DELTAS
		}
	},
	{/* TI OmniLaser 2115 */
		{TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE},
		{FALSE,TRUE,TRUE},
		IDS_UPPER,
		{
			LOWMEMORY_IMAGE_DELTAS
		}

#ifndef CONFIDENTIAL
	},
	{/* Wang LCS15 */
		{TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE},
		{FALSE,TRUE,FALSE},
		IDS_AUTO,
		{
			LOWMEMORY_IMAGE_DELTAS
		}
	},
	{/* Wang LCS15 FontPlus */
		{TRUE,TRUE,FALSE,TRUE,FALSE,TRUE,FALSE,FALSE,FALSE},
		{FALSE,TRUE,FALSE},
		IDS_AUTO,
		{
			LOWMEMORY_IMAGE_DELTAS
		}
#endif
	}
};
#else
extern PRINTER gPrCaps[];
#endif
#endif


#define BINSTRLEN 24

#ifdef GLOBALSTUFF
#ifdef GPAPERBINS
char gPaperBins[NUMBINS][BINSTRLEN]={
	"Manual feed",
	"Upper tray",
	"Lower tray",
	"Auto tray select",
	"Cassette"				/* for Lino */
};
#else
extern char gPaperBins[][BINSTRLEN];
#endif


#ifdef BINTOFEEDMAP
/* maps the bin numbers to to appropriate feed numbers...
 * NUMBINS bins to NUMFEEDS feeds
 * i.e. 5 to 3 mapping -- the reverse mapping can be found
 * in the MakeBinList() proc. in CONTROL.C
 */
char gBinToFeedMap[NUMBINS]={ 0, 1, 2, 1, 1 };
#else
extern char gBinToFeedMap[];
#endif
#endif

#endif
/*********************************************************************
 * end of NOSTRUCTURES
 *********************************************************************
 */

BOOL FAR PASCAL GetPrinterCaps(HANDLE,short,LPPRINTER);

