/*
 +--- GENERIC.H for TTY ----------------------------------------------------+
 |                                                                          |
 |  Copyright (c) 1989-1990, Microsoft Corporation.			    |
 |  Modificado en 1986 por Jaime Garza V. FRALC Consultores (W 1.03)        |
 |  Modificado en 1988 por Jaime Garza V. FRALC Consultores (W 2.03)        |
 |  Modificado en 1989 por Armando Rodri'guez M. FRALC Consultores (W 3.00) |
 |  Modificado en 1989 por Jaime Garza V. FRALC Consultores (W 3.00)        |
 |                                                                          |
 +--------------------------------------------------------------------------+
*/

//	Microsoft history
//	8/19/91 	LinS		Include print.h.  Remove all excessive
//					include files
//	27 dec 89	peterbe		Added parameters to InitQueue(),
//					DeleteQueue(), TextDump().
//	08 dec 89	peterbe		Added BOOL to SetWidth().
//	07 dec 89	peterbe		Changed SetMode() to SetWidth().
//					Added SelectWidth().
//	20 oct 89	peterbe		checked in

#define PRINTDRIVER
#include "print.h"
#include "gdidefs.inc"
#include "tty.h"
#include "device.h"
#include "file.h"
#include "defs.h"
#include "ttyres.h"

#define DEVICENAME  "tty.exe"

#define MODULENAME  "tty"

#define DEV_WINVERSION	    0x30a   /* windows 3.00 */
#define DEV_MIVERSION	    0x30a   /* 3.1 driver   */

extern	PAPERFORMAT PaperFormat[];
extern	BYTE	    Trans[];
extern	INT_WIDTH   Trans_width[];
extern	ESCAPECODE  escapecode;
extern	COUNTRYESCAPE countryesc;
extern PrinterInfo Printer;
extern char	   PrinterFileName[];
extern int	   PrinterNumber;
extern unsigned char defaultchars[];
extern GDIINFO	gBaseInfo;
extern PrinterInfo Printer;
extern HANDLE hInst;
extern LPDM lpDevmode;
extern	BOOL bHasFont[];		// TRUE if font [i] is supported.
					// (used here to check for Elite
					// support).
extern FONTINFO gFontInfo;
