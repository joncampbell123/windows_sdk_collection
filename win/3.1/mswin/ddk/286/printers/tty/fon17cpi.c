/*/  FON17CPI.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History
//	14 nov 89	peterbe		Made face name same as in EPSON9.DRV
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------
// Pica compressed = Roman 17cpi
// ------------------------------------------------------------------------

#include "printer.h"
#include "gdidefs.inc"
#include "TTY.h"
#include "drivinit.h"
#include "device.h"

char FntFile[] = "fon17cpi.myf";

char DeviceName[] = "TTY";   /* ver dfDevice */

char FaceName[] = "Roman 17cpi";    /* ver dfFace */

FONTINFO FontInfo = {
    0x80,   /* dfType (0x0080 es el "device font") */
    8,	    /* dfPoints (en 1/72") */
    72,	    /* dfVertRes */
    120,    /* dfHorizRes */
    7,	    /* dfAscent */
    0,	    /* dfInternalLeading */
    2,	    /* dfExternalLeading */
    0,	    /* dfItalic */
    0,	    /* dfUnderline */
    0,	    /* dfStrikeOut */
    400,    /* dfWeight */
    0,	    /* dfCharSet */
    7,	    /* dfPixWidth */
    10,	    /* dfPixHeight */
    0x30,   /* dfPitchAndFamily
             * el bit bajo es la bandera de variable "pitch"
             *    0 => FF_DONTCARE
             *    1 => FF_ROMAN
             *    2 => FF_SWISS
             *    3 => FF_MODERN esta es la buena
             *    4 => FF_SCRIPT
             *    5 => FF_DECORATIVE
             */
    7,	    /* dfAvgWidth */
    7,	    /* dfMaxWidth */
    0x20,   /* dfFirstChar */
    0xff,   /* dfLastChar */
    0x2e,   /* dfDefaultChar + dfFirstChar */
    0x20,   /* dfBreakChar + dfFirstChar */
    0,	    /* dfWidthBytes ( = 0 ) */
    0x41,   /* dfDevice - ver DeviceName */
    0x4d,   /* dfFace - ver FaceName */
    0x0,    /* dfBitsPointer ( = 0 ) */
    0x0,    /* dfBitsOffset ( = 0 ) */
};

PRDFONTINFO PrdFontInfo = { /* Especifica Info del Driver */
    "",	    /* dfFont */
    0,	    /* wheel */
	    /* bfont: */
    0,	    /* offset */
    01,	    /* mod */
    01,	    /* length */
	    /* efont: */
    1,	    /* offset */
    00,	    /* mod */
    01,	    /* length */
    0x0,    /* widthtable - ver WidthTable */
};

/* No Width Table */
int bWidthTable = FALSE;
short WidthTable[] = {
    0,
};
unsigned char WidthFirstChar = 0;
unsigned char WidthLastChar = 0;
