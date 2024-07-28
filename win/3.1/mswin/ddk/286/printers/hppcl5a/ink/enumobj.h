/**[f******************************************************************
* enumobj.h -
*
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved
*
**f]*****************************************************************/
  
/**********************************************************************
*
*  2 Oct 89    clarkc  Wrote it.
*/
  
#define MAXCOLORS 2
#define RGB_BLACK 0xFFFFFFL
#define RGB_WHITE 0L
  
#define MAXHATCHSTYLE       6
/* Brush Styles */
#define BS_SOLID        0
#define BS_NULL         1
#define BS_HATCHED      2
#define BS_PATTERN      3
#define BS_INDEXED      4
#define BS_DIBPATTERN       5
  
/* Pen Styles */
#define PS_SOLID        0
#define PS_DASH         1       /* -------  */
#define PS_DOT          2       /* .......  */
#define PS_DASHDOT      3       /* _._._._  */
#define PS_DASHDOTDOT       4       /* _.._.._  */
#define PS_NULL         5
#define PS_INSIDEFRAME      6
  
typedef int (far pascal *FARPROC)();
  
