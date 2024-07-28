/* (c) Copyright 1989 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1303 Arapho Road, Richardson, Tx.  75081.

******************************************************************************

                                escapes.h

******************************************************************************

*/

#define ALTERNATE		     1
#define WINDING 		     2

typedef struct
{
    short RenderMode;
    BYTE FillMode;
    BYTE BkMode;
    LOGPEN Pen;
    LOGBRUSH Brush;
    DWORD BkColor;
} PATHINFO;

typedef PATHINFO FAR *LPPATHINFO;

#define RM_NO_DISPLAY       0
#define RM_OPEN             1
#define RM_CLOSED           2


#define CLIP_SAVE           0
#define CLIP_RESTORE        1
#define CLIP_INCLUSIVE      2
#define CLIP_EXCLUSIVE      3

#define COUNTERCLOCKWISE    0
#define CLOCKWISE           1

#define PM_POLYLINE         1
#define PM_BEZIER           2
#define POLYLINESEGMENT     3

#define R2_CAPS             1
#define PATTERN_CAPS        2
#define PATH_CAPS           3
#define POLYGON_CAPS        4
#define PATTERN_COLOR_CAPS  5
#define R2_TEXT_CAPS        6

#define PATH_ALTERNATE      1
#define PATH_WINDING        2
#define PATH_INCLUSIVE      4
#define PATH_EXCLUSIVE      8

#define DM_UPDATE           1
#define DM_COPY             2
#define DM_PROMPT           4

#define RASTER_TEXT         1
#define DEVICE_TEXT         2
#define VECTOR_TEXT         4
