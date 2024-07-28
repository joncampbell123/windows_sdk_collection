/****************************************************************************
 * define the driver specific form of the DEVMODE structure
 *
 ***************************************************************************/

#ifndef INC_PSDEVMODE

#define INC_PSDEVMODE

#include "printers.h"

#define CHEPSFILE   40
/*
 * DEVMODE
 *
 * this structuer is used to save device data for a given port between
 * creations and deletions of PDEVICE data.  it is also used to communicate
 * the data in the DEVICEMODE dialog to the driver.  also note that some
 * escapes change this data.
 *
 */

typedef struct {
	DEVMODE dm;
	short iPrinter;		/* The printer type */
	short iRes; 		/* The device resolution */
	short iJobTimeout;	/* The job timeout in seconds */
	short marginState;	/* margin state: default, zero, tile */
	BOOL  fHeader;		/* TRUE=download header */
	BOOL  fDoEps;		/* output an EPS header with bbos */
	BOOL  fBinary;		/* is binary port */
	int   iScale;		/* scale paper size 100/iScale, iRes by iScale/100 */
	short rgiPaper[DMBIN_FIRST + NUMFEEDS]; /* 1 paper type per paper source */
	char  EpsFile[CHEPSFILE];
        int   ScreenFrequency;  /* halftone screen frequency */
        int   ScreenAngle;      /* halftone screen angle */
        int   LandscapeOrient;  // 90 or 270 degrees rotation from portrait
                                // default will be 90.
        BOOL  bPerPage;         /* TRUE if per page font downloads requested */
        BOOL  bNegImage;        /* TRUE if negative images requested */
        BOOL  bDSC;             /* TRUE if output should conform to DSC */
        BOOL  bSubFonts;        /* TRUE if TrueType fonts should be substituted */
        short iDLFontFmt;       /* font DL format (Type 3, Type 1, TrueType) */
        BOOL  bMirror;          /* TRUE if output should be mirrored */
        BOOL  bColorToBlack;    /* TRUE if all colors should map to black */
        BOOL  bCompress;        /* TRUE if we should ALWAYS compress bitmaps */
        BOOL  bErrHandler;      /* TRUE if we should use error handler */
	BOOL  bNoDownLoad;	/* TRUE if use Device Fonts for TT */
	BOOL  bFavorTT;	    /* TRUE if resolve font ambiguities in favor of TT */
        int   iMaxVM;           /* amount of printer VM in K */
        int   nSubFonts;        /* number of font substitution entries */
        HANDLE  hSubTable;      /* handle to substitution table */
   int iCustomWidth;            /* width of custom paper size */
   int iCustomHeight;           /* height of custom paper size */
   int dmSizeUnit;              /* unit used to specify custom size */
   int iMinOutlinePPEM;         /* crossover point for Type 1 outlines */
} PSDEVMODE;   
 
typedef PSDEVMODE *PPSDEVMODE;
typedef PSDEVMODE FAR *LPPSDEVMODE;

#define AllocDevMode()  ((PPSDEVMODE)LocalAlloc(LPTR, sizeof(PSDEVMODE)))
#define FreeDevMode(h)  LocalFree(LocalHandle((WORD)h))

#endif
