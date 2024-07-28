/**[f******************************************************************
 * printers.h -
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * PRINTERS.H
 *
 * 14Apr87 sjp	Creation date.
 * 17Apr87 sjp	Added all printer specific constants, etc. so that
 *	   	all necessary info (except what is in PSCRIPT.RC)
 *	   	that is necessary to add, del, modify printers is
 *	   	in this file.  To modify logic, consult CONTROL.C.
 * 04Jun87 sjp	Added gPaperBins[][] array.
 * 11Sep87 sjp	Added new printers.
 * 03Nov87 sjp	Converted this file to use the stuff from APD compiler.
 * 26Mar91 msd	Added default duplex value.
 *********************************************************************/

#define DEFAULTORIENTATION	DMORIENT_PORTRAIT	/* portrait */
#define DEFAULTDUPLEX		DMDUP_SIMPLEX		/* duplex */
#define DEFAULT_COLOR		DMCOLOR_COLOR		/* yes */

/* max length of the bin (aka: feed, source) names */

#define NUM_INT_PRINTERS	23


#define INT_PRINTER_MIN 1
#define INT_PRINTER_MAX (INT_PRINTER_MIN + NUM_INT_PRINTERS - 1)
#define EXT_PRINTER_MIN (INT_PRINTER_MAX + 1)
#define FIRST_PRINTER INT_PRINTER_MIN

#define BINSTRLEN	24

#define DEFAULT_PRINTER		2

#define DMPAPER_ENVELOPE    DMPAPER_USER

#define DMPAPER_USER_FIRST  50

#define DMPAPER_LETTER_EXTRA	         50    // Letter Extra 9 \275 x 12 in
#define DMPAPER_LEGAL_EXTRA 	         51    // Legal Extra 9 \275 x 15 in
#define DMPAPER_TABLOID_EXTRA	         52    // Tabloid Extra 11.69 x 18 in
#define DMPAPER_A4_EXTRA     	         53    // A4 Extra 9.27 x 12.69 in
#define DMPAPER_LETTER_TRANSVERSE	 54    // Letter Transverse 11 x 8 \275 in
#define DMPAPER_A4_TRANSVERSE		 55    // Transverse 297 x 210 mm
#define DMPAPER_LETTER_EXTRA_TRANSVERSE  56    // Letter Extra Transverse 12 x 9\275 in

#define DMPAPER_USER_LAST   DMPAPER_LETTER_EXTRA_TRANSVERSE

/* bin selections (remove defines from drivinit.h then define our own).

#undef DMBIN_ONLYONE	   
#undef DMBIN_LOWER	    
#undef DMBIN_MIDDLE	    
#undef DMBIN_ENVELOPE	  
#undef DMBIN_ENVMANUAL   
#undef DMBIN_AUTO	    
#undef DMBIN_TRACTOR	   
#undef DMBIN_SMALLFMT	   
#undef DMBIN_LARGEFMT	   
#undef DMBIN_LARGECAPACITY
#undef DMBIN_ANYSMALLFMT  
#undef DMBIN_ANYLARGEFMT  
#undef DMBIN_CASSETTE     
#undef DMBIN_MANUAL	    
#undef DMBIN_LAST	    

#define DMBIN_ONLYONE	    2
#define DMBIN_LOWER	    3
#define DMBIN_MIDDLE	    4
#define DMBIN_ENVELOPE	    5
#define DMBIN_ENVMANUAL     6
#define DMBIN_AUTO	    7
#define DMBIN_TRACTOR	    8
#define DMBIN_SMALLFMT	    9
#define DMBIN_LARGEFMT	    10
#define DMBIN_LARGECAPACITY 11
#define DMBIN_ANYSMALLFMT   12
#define DMBIN_ANYLARGEFMT   13
#define DMBIN_CASSETTE      14
#define DMBIN_MANUAL	    15
#define DMBIN_LAST	    DMBIN_MANUAL    */

#include "printcap.h"
