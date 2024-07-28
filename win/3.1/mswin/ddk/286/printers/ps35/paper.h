/**[f******************************************************************
 * paper.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/* The paper metrics */
typedef struct {
    short iPaper;	/* the DMPAPER_* type */
    short cxPaper;	/* The paper width in dots */
    short cyPaper;	/* The paper height in dots */
    short cxPage;	/* The printable area width in dots */
    short cyPage;	/* The printable area height in dots */
    short cxMargin;	/* The left margin width to printable area in dots */
    short cyMargin;	/* The top margin depth to printable area in dots */
} PAPER, FAR *LPPAPER, *PPAPER;

/* these are the structures defined in PAPERSIZ.RC */
typedef struct {
	short iPaperType;
	short xSize;		/* in 100ths of an inch */
	short ySize;		/* same */
} PAPERSIZE, FAR *LPPAPERSIZE, *PPAPERSIZE;

#define PA 4550

