/**[f******************************************************************
* paperfmt.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
/*  The PAPERFORMATs are stored in a binary file that the driver
*  accesses from the resources (HPPCL.RC).  It consists of a header
*  that points to an array of lists and an array of paper formats.
*  A list contains indexes to the paper types supported by one
*  printer.  Each paper format contains the dimensions of the page
*  and the selection string.
*
*  The printer strings in the resource file contain the index into
*  the array of lists for each printer.
*/
#define PAPERID_LETTER  0x0001
#define PAPERID_LEGAL   0x0002
#define PAPERID_EXEC    0x0004
#define PAPERID_A4      0x0008
#define PAPERID_MONARCH 0x0010
#define PAPERID_COM10   0x0020
#define PAPERID_DL      0x0040
#define PAPERID_C5      0x0080
  
#define MAXPAPERFORMAT  8
  
/*  This struct is duplicated in device.i */
typedef struct {
    short xPhys;            /* physical paper width */
    short yPhys;            /* physical paper height */
    short xImage;           /* image area width */
    short yImage;           /* image area height */
    short xPrintingOffset;  /* printing offset in x direction */
    short yPrintingOffset;  /* printing offset in y direction */
    BYTE select[16];        /* paper select string */
} PAPERFORMAT;
  
typedef struct {
    short id;               /* paper type (letter, legal, etc) */
    short indPortPaperFormat;
    short indLandPaperFormat;
} PAPERLISTITEM;
  
typedef struct {
    short len;              /* number of entries in the list */
    PAPERLISTITEM p[MAXPAPERFORMAT];
} PAPERLIST;
  
typedef struct {
    short numLists;         /* number of paper list structs */
    short numFormats;       /* number of paper format structs */
    DWORD offsLists;        /* offset to array of lists */
    DWORD offsFormats;      /* offset to array of paper formats */
} PAPERHEAD;
  
  
/*  ONLY MAKERES.C should define this symbol.
*/
#ifdef PAPER_DATA
  
/*  PAPERLIST
*
*  Each entry in this list contains the paper ids and offsets into
*  the array of PAPERFORMATs for the corresponding portrait and
*  landscape paper formats.
*
*  NOTE:  If you add an entry to the paper list, you must change the
*  constant MAX_PAPERLIST in resource.h to reflect the number of
*  entries.
*/
  
/* ELI:  Increased PaperList from 1 element to 2 and added list for ELI */
PAPERLIST PaperList[2] = {
  
    /*  0 LaserJet Series III, LaserJet IIID
    */
    { 8, PAPERID_LETTER,  0, 8,
         PAPERID_LEGAL,   1, 9,
         PAPERID_EXEC,    2, 10,
         PAPERID_A4,      3, 11,
         PAPERID_MONARCH, 4, 12,
         PAPERID_COM10,   5, 13,
         PAPERID_DL,      6, 14,
         PAPERID_C5,      7, 15
    },
    /* 1 LaserJet IIISi
    */
    { 7, PAPERID_LETTER,  0, 8,
         PAPERID_LEGAL,   1, 9,
         PAPERID_EXEC,    2, 10,
         PAPERID_A4,      3, 11,
         PAPERID_MONARCH, 4, 12,
         PAPERID_COM10,   5, 13,
         PAPERID_DL,      6, 14
    }
};
  
  
/*  PAPER SIZES
*
*  xPhys, yPhys, xImage, yImage, xPrintingOffset,
*  yPrintingOffset, select (string)
*/
/*         Note: yPrintingOffset = is set to 75 for portrait orintation
                                   because lines per inch is set to 4 and
                                   top margin set 1 which puts y=0 at 75 
                                   dots from the top of the page.
                 
                 yImage = yPhys - yPrintingOffset - bottom margin
                 
                 bottom margin = distance from bottom edge of page to the
                                 printable area which is 50 dots in
                                 portrait orientation.
                 
           example:  yImage for letter size paper in Portait
                    
                     yImage = 3300 - 75 - 50 = 3175           jsmart
*/

PAPERFORMAT PaperFormat[16] = {
  
    /*  LaserJet Series III and LaserJet IIID.
    */
    /*  0 P LETTER */ { 2550, 3300, 2400, 3175, 75, 75, "\033&l0o2a4d1e42F"  },
    /*  1 P LEGAL  */ { 2550, 4200, 2400, 4075, 75, 75, "\033&l0o3a4d1e54F"  },
    /*  2 P EXEC   */ { 2175, 3150, 2025, 3025, 75, 75, "\033&l0o1a4d1e40F"  },
    /*  3 P A4     */ { 2480, 3507, 2338, 3382, 71, 75, "\033&l0o26a4d1e45F" },
    /*  4 P MONARC */ { 1162, 2250, 1012, 2125, 75, 75, "\033&l0o80a4d1e28F" },
    /*  5 P COM10  */ { 1237, 2850, 1087, 2725, 75, 75, "\033&l0o81a4d1e36F" },
    /*  6 P DL     */ { 1299, 2598, 1157, 2473, 71, 75, "\033&l0o90a4d1e33F" },
    /*  7 P C5     */ { 1913, 2704, 1771, 2579, 71, 75, "\033&l0o91a4d1e34F" },
                      /*  note: yImage must be a multiple of 8 for
                                bit maps to work in landscape.   jcs  */
    /*  8 L LETTER */ { 3300, 2550, 3180, 2448, 60, 50, "\033&l1o2a1e48F"  },
    /*  9 L LEGAL  */ { 4200, 2550, 4080, 2448, 60, 50, "\033&l1o3a1e48F"  },
    /* 10 L EXEC   */ { 3150, 2175, 3030, 2072, 60, 50, "\033&l1o1a1e40F"  },
    /* 11 L A4     */ { 3507, 2480, 3389, 2376, 59, 50, "\033&l1o26a1e46F" },
    /* 12 L MONARC */ { 2250, 1162, 2130, 1056, 60, 50, "\033&l1o80a1e18F" },
    /* 13 L COM10  */ { 2850, 1237, 2730, 1136, 60, 50, "\033&l1o81a1e18F" },
    /* 14 L DL     */ { 2598, 1299, 2480, 1192, 59, 50, "\033&l1o90a1e18F" },
    /* 15 L C5     */ { 2704, 1913, 2586, 1808, 59, 50, "\033&l1o91a1e36F" }
  
};
#endif
  
