/*
 +--- FONTS.H for TTY ------------------------------------------------------+
 |                                                                          |
 |  Copyright (c) 1989-1990, Microsoft Corporation.			    |
 |  Modificado en 1986 por Jaime Garza V. FRALC Consultores (W 1.03)        |
 |  Modificado en 1988 por Jaime Garza V. FRALC Consultores (W 2.03)        |
 |  Modificado en 1989 por Armando Rodri'guez M. FRALC Consultores (W 3.00) |
 |  Modificado en 1989 por Jaime Garza V. FRALC Consultores (W 3.00)        |
 |                                                                          |
 +--------------------------------------------------------------------------+
*/

//	Microsoft history (LAST FIRST)
//	20 nov 89	peterbe		Make DEFAULT_FACENAME = 1 (12 cpi),
//					and  DEFAULT_FACENAME2 = 2 (10 cpi).
//	14 nov 89	peterbe		Make face names same as in EPSON9
//					("Roman 17cpi" etc.)
//	13 nov 89	peterbe		Added bHasFont[] array
//	20 oct 89	peterbe		checked in

#define abs(a)	((a) > 0? (a): (-(a)))

typedef struct
{
    char    facename;		/* index to facenames[], bHasFont[] */
    char    family;		/* see adaptation guide E.2 */
    char    height;		/* in device pixels */
    char    width;		/* in device pixels */
    char    italic;		/* 1 - italic, 0 - normal font */
    char    weight;		/* in increments of 100 */
}   FONTTAB;

#define ORIENTATION_WEIGHT   0
#define PITCH_WEIGHT	    11
#define FACENAME_WEIGHT	     0
#define FAMILY_WEIGHT	    12
#define LARGE_HEIGHT_WEIGHT  7
#define HEIGHT_WEIGHT	     6
#define WIDTH_WEIGHT	    13
#define ITALIC_WEIGHT	     0
#define WEIGHT_WEIGHT	     0

/* default: 12 pitch fixed font, elite */

#define DEFAULT_FACENAME2    2	/* 10-pitch pica */
#define DEFAULT_FACENAME     1	/* 12-pitch elite */
#define DEFAULT_FAMILY	     3
#define DEFAULT_WEIGHT	     4

/* Text Metrics */

#define TMHeight	  1
#define TMAscent	  1
#define TMDescent	  0
#define TMInternalLeading 0
#define TMExternalLeading 0
#define TMFirstChar	  32
#define TMLastChar	 126
#define TMBreakChar	  32
#define TMDefaultChar	  46

#define DIFF_CELL_CHAR	    0	/* number of pixels between your cell height
                                   and character height */

#define HP_LOGFONT     1
#define HP_TEXTMETRIC  2
#define HP_FONTINFO    3
#define HP_DONTCARE    4
#define HP_TEXTXFORM   5


#define NFONTS		6
#define PSFONTS		0
#define NFACES		6


FONTTAB FontTable[] = {
/*
 fnam fam heigh width ital weight */
{0x0, 0x3, 0x1, 0x7,  0x0, 0x4},   /* pica compressed 17 cpi*/
{0x1, 0x3, 0x1, 0x0a, 0x0, 0x4},   /* elite 12 cpi*/
{0x2, 0x3, 0x1, 0xc,  0x0, 0x4},   /* pica 10 cpi*/
{0x3, 0x3, 0x1, 0xf,  0x0, 0x4},   /* pica compressed expanded 8 cpi*/
{0x4, 0x3, 0x1, 0x14, 0x0, 0x4},   /* elite expanded 6 cpi*/
{0x5, 0x3, 0x1, 0x18, 0x0, 0x4},   /* pica expanded 5 cpi*/
};


// same as EPSON9 driver face names now.
char *facenames[] = {
	"Roman 17cpi",
	"Roman 12cpi",
	"Roman 10cpi",
	"Roman 8cpi",
	"Roman 6cpi",
	"Roman 5cpi"
};

// this indicates whether a font is supported by this printer.
// Initialized in FILE.C from the printer file.

BOOL bHasFont[] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

void NEAR PASCAL CodeToInfo(FONTTAB *, short, LPSTR);
void NEAR PASCAL InfoToCode(short, LPLOGFONT, FONTTAB far *);
