/****************************************************************************
 DC.h

 The DC module handles setting and reading of the DC attributes.

****************************************************************************/


/****************************************************************************
   Types
****************************************************************************/

/* Values needed to setup a DC */
typedef struct {
   short    mapMode;
   POINT    windowOrg;
   POINT    windowExt;
   POINT    viewportOrg;
   POINT    viewportExt;
   
   POINT    penPos;
   POINT    brushOrg;

   BOOL     intersect;
   RECT     rIntersect;
   BOOL     exclude;
   RECT     rExclude;

   DWORD    bkColor;
   short    bkMode;

   LOGFONT  font;
   WORD     textAlign;
   short    breakExtra;
   short    breakCount;
   short    textExtra;
   DWORD    textColor;

   short    rop2;
   short    polyFillMode;
   short    bltMode;

   LOGBRUSH brush;
   BYTE     patChecks[64];   /* treated as array of boolean, TRUE => 0 bit */
   LOGPEN   pen;

   /* synthesized fields */
   HFONT    hFont;
   HPEN     hPen;
   HBRUSH   hBrush;
} DCValues;


/****************************************************************************
   Globals
****************************************************************************/

/* The global DC settings */
extern DCValues dcv;


/****************************************************************************
   Functions
****************************************************************************/

void DCCmd( int item );
void ReadDC( HDC hdc );
void SetupDC( HDC hdc );
void SetupError( char *string );
void CleanupDC( HDC hdc );
void CheckDCMenuItems( HMENU hMenu );

