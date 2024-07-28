/**[f******************************************************************
 * gdi.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/***************************************************************************
 * 12Jun87	sjp		Added CC_ROUNDRECT.
 *
 * note: this file is derived from GDIDEFS.INC and windows.h.  definitions
 * of GDI logical object are found here and suppressed from being defined
 * by defining NOGDI in pscript.h
 *
 ***************************************************************************/

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
#define     OS_EOPOLYGON	22
#define     OS_PIE		23
#define     OS_POLYMARKER	24
#define     OS_CHORD		39
#define     OS_CIRCLE		55
#define     OS_ROUNDRECT	72


/* bit in the dfType field signals that the dfBitsOffset field is an
   absolute memory address and should not be altered. */
#define PF_BITS_IS_ADDRESS  4

/* bit in the dfType field signals that the font is device realized. */
#define PF_DEVICE_REALIZED  0x80

/* bits in the dfType give the fonttype: raster, vector, other1, other2 */

#define PF_RASTER_TYPE	0
#define PF_VECTOR_TYPE	1
#define PF_OTHER1_TYPE	2
#define PF_OTHER2_TYPE	3

#define LS_SOLID	0
#define LS_DASHED	1
#define LS_DOTTED	2
#define LS_DOTDASHED	3
#define LS_DASHDOTDOT	4
#define LS_NOLINE	5
#define MaxLineStyle	LS_NOLINE


#define InquireInfo     0x01	/* Inquire Device GDI Info	   */
#define EnableDevice    0x00	/* Enable Device		   */
#define InfoContext     0x8000	/* Inquire/Enable for info context */


/* Curve Capabilities */

#define	CC_ROUNDRECT    00000400	/* Can do round rects	*/

/* #define RC_GDI15_OUTPUT 00000020 */	/* has 2.0 output calls */

#define OVERHANG 0


typedef DWORD RGB;		/* GDI rgb type */

typedef FONTINFO FAR *LPFONTINFO;

typedef TEXTXFORM FAR *LPTEXTXFORM;
typedef DRAWMODE  FAR *LPDRAWMODE;
typedef LOGPEN    FAR *LPLOGPEN;
typedef LOGBRUSH  FAR *LPLOGBRUSH;
typedef LOGFONT   FAR *LPLOGFONT;

typedef struct {
	short	x;
	short	y;
	int	count;
	RECT	ClipRect;
	LPSTR	lpStr;
	short	far *lpWidths;
} APPEXTTEXTDATA;
typedef APPEXTTEXTDATA FAR *LPAPPEXTTEXTDATA;


typedef struct {
	short			nSize;
	LPAPPEXTTEXTDATA	lpInData;
	LPFONTINFO		lpFont;
	LPTEXTXFORM 		lpXForm;
	LPDRAWMODE		lpDrawMode;
} EXTTEXTDATA;
typedef EXTTEXTDATA FAR *LPEXTTEXTDATA;


