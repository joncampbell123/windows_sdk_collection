/****************************************************************************
 Draw.h

 The Draw module handles the GDI drawing.

****************************************************************************/


/****************************************************************************
   Types
****************************************************************************/

/* Constants for the DrawValues type */
#define MAX_PTS            8
#define MAX_POLYPOLY_PTS   12
#define MAX_COUNTS         5
#define MAX_TEXT_LEN       1024
#define MAX_DX             24

/* Values needed for drawing commands */
typedef struct {
   POINT    pt;         /* draw point for SetPixel, LineTo, FloodFill */
   POINT    ptFrom;     /* for MoveTo/LineTo */
   RECT     r;          /* bounding rect for shapes */
   POINT    corner;     /* for RoundRect */
   POINT    startPt;    /* for arc/pie/chord */
   POINT    endPt;      /* for arc/pie/chord */
   int      nPts;       /* for polys */
   POINT    pts[MAX_POLYPOLY_PTS];  /* for polys */
   int      nPolys;     /* for PolyPolygon */
   int      counts[MAX_COUNTS];  /* for PolyPolygon */
   RECT     rFrom;      /* for blts */
   char     text[MAX_TEXT_LEN + 1];  /* for text (0 terminated) */
   WORD     options;    /* for ExtTextOut */
   WORD     format;     /* for DrawText */
   WORD     tabStop;    /* for DrawText */
   BOOL     useDx;      /* for ExtTextOut */
   int      dxArray[MAX_DX];  /* for ExtTextOut */
   DWORD    rgb;        /* for SetPixel and FloodFill */
   DWORD    rop;        /* for blts */
   BOOL     floodType;  /* for ExtFloodFill */
   int      startScan;  /* for SetDIBitsToDevice */
   int      numScans;   /* for SetDIBitsToDevice */
} DrawValues;


/****************************************************************************
   Globals
****************************************************************************/

/* The global drawing values */
extern DrawValues  dv;


/****************************************************************************
   Functions
****************************************************************************/

int DrawCmdDialog( int item );
void DrawCmd( int item );

