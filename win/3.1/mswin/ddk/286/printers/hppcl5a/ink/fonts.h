/**[f******************************************************************
* fonts.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
#define abs(a)  ((a) > 0? (a): (-(a)))
  
#define DEFAULT_HEIGHT  50
#define DEFAULT_WIDTH   30
  
#define HP_LOGFONT     1
#define HP_TEXTMETRIC  2
#define HP_FONTINFO    3
#define HP_DONTCARE    4
#define HP_TEXTXFORM   5
  
#define CHARSET_WEIGHT      14
#define FACENAME_WEIGHT     13
#define PITCH_WEIGHT        11
#define FAMILY_WEIGHT       10
#define LARGE_HEIGHT_WEIGHT  4
#define HEIGHT_WEIGHT        3
#define WIDTH_WEIGHT         0
#define ITALIC_WEIGHT        4
#define WEIGHT_WEIGHT        5      /* this is a right shift */
  
#define NFACES 4
#define COURIER     0
#define LINEPR      1
#define TMSRMN      2
#define HELV        3
  
  
