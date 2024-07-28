/**[f******************************************************************
* hppcl.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

#define TEXTBAND        1 /* collect text calls in the first band */
  
#define NAME_LEN        32
  
#define TENPT_PIXHEIGHT 42
#define STRIKEOUT_THICKNESS 3
  
  
#ifndef NO_PRINTER_STUFF
  
/******************** escape sequences for all laserjets**********************/
  
/* set landscape mode, move cursor to the right by 8"(5760 decipoints),
set resolution, start graphics mode */
  
#define RESET           "\033E",2
  
/* push and pop current cursor position */
  
#define PUSH_POSITION   "\033&f0S", 5
#define POP_POSITION    "\033&f1S", 5
  
/* shift-in and shift-out */
#define SHIFT_OUT       "\016", 1
#define SHIFT_IN        "\017", 1
  
#define FONT_DEFAULT    "\033(8U\033(s3ts10hbP", 15
  
#define MAN_FEED        "\033&l2H", 5
#define UPPER_TRAY  "\033&l1H", 5
#define MAN_ENVFEED "\033&l3H", 5
#define LOWER_TRAY  "\033&l4H", 5
#define PAPER_DECK  "\033&l5H", 5
#define ENV_TRAY        "\033&l6H", 8
  
#define DEL_FONT        "\033*c2F", 5
#define DEL_DL_FONTS    "\033*c1F", 5
#define DEL_ALL_FONTS   "\033*c0F", 5
  
/* set resolution, start graphics mode */
#define HP_SET_75RES      "\033*t75R", 6
#define HP_SET_150RES     "\033*t150R", 7
#define HP_SET_300RES     "\033*t300R", 7
  
/* black rule or pattern */
#define BLACK_PATTERN   "\033*c0P", 5
  
#define HP_FF         "\014",  1
#define HP_UNDERLINE_ON     "\033&dD", 4
#define HP_UNDERLINE_OFF    "\033&d@", 4
  
#define PORT_COM10_RESET    "\033&l81a0o1E", 10
#define LAND_COM10_RESET    "\033&l81a1o1E", 10
  
/*the following are turned into escape sequences by MakeEscape*/
#define HP_RESET '*', 'b', 'W'   /* *b#W  transfer raster graphics*/
#define HP_HCP   '&', 'a', 'H'   /* &a#H  horizontal cursor pos*/
#define HP_VCP   '&', 'a', 'V'   /* &a#V  vertical cursor pos*/
#define DOT_HCP  '*', 'p', 'X'   /*horz cursor pos in dots*/
#define DOT_VCP  '*', 'p', 'Y'   /*vertical cursor pos in dots*/
#define HP_COPIES '&', 'l', 'X'  /*set number of copies*/
#define SET_FONT  '*', 'c', 'D'   /*specify font ID when downloading*/
#define DES_FONT  '(','0', 'X'   /*designate font with ID*/
#define DOT_HRPS '*', 'c', 'A'  /* horz rule/pattern size in dots */
#define DOT_VRPS '*', 'c', 'B'  /* vert rule/pattern size in dots */
#define PAT_ID    '*', 'c', 'G' /* set rule/pattern id */
#define PAT_PRINT '*', 'c', 'P' /* print rule/pattern */
#define HP_DUPLEX '&', 'l', 'S' /* simplex/duplex printing */
  
/*********** escape sequences for all laserjet+ and 500**********************/
  
/******************** constants for all laserjets*****************************/
/*memory limits*/
/*page sizes*/
#define HSIZE        8         /* 7.893 inches across the page*/
#define VSIZE       10         /* 10.24 (9.6) inches down the page */
#define MM_HSIZE    200         /*pagewidth in mm = HSIZE * 25.4 */
#define MM_VSIZE    260         /*pagelength in mm = VSIZE * 25.4 */
  
  
/* we tell GDI to always do stuff at 300 dpi , we scale to printer resolution
by giving GDI a scaling factor*/
  
#define GDI_DPI     300
#define HDPI        GDI_DPI
#define VDPI        GDI_DPI
  
#define XYASPECT    424   /*Distance moving X and Y*/
#define XASPECT     HDPI   /*Distance moving Y only*/
#define YASPECT     VDPI   /*Distance moving X only*/
#define MAXSTYLELEN (XYASPECT * 2)
  
#define MAX_BAND_WIDTH  3510        /* also defined in device.i */
#define MAX_BANDDEPTH   64
  
#define OVERHANG    0
  
/*  300 dots per inch in graphics mode, but 720 dpi precision
*  in text mode:
*
*    720 / 300 == 12 / 5
*/
#define trans(n)    ((n) * 12 / 5)
  
/********************** device-independent info ***********************/
  
#define DEV_MEMORY  0
#define BYTESIZE 8
#define NPINS   1           /* HP LaserJet fires one pin at a time */
  
#define SPOOL_SIZE (2048)       /* 2k buffer */
  
/* draft mode flags and graphics accelerator */
  
#define DRAFTFLAG   0x01
#define BREAKFLAG   0x02
#define GRXFLAG     0x04    /* flag to see if anything is drawn to the buffer */
#define SKIPFLAG    0x08    /* if we skipped a band without graphics */
  
/* device mode */
  
#define PRINTER     0x01
#define ORIENT      0x02
#define FEED        0x04
#define SIZE        0x08
#endif
  
typedef struct {
    char esc;
    char start1;
    char start2;
    char num[10];
} ESCtype, FAR *lpESC;
  
typedef HANDLE far * LPHANDLE;
  
