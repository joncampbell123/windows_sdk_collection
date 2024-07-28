/****************************************************************************
 View.h

 The View module handles the display of the drawing view.

****************************************************************************/


/****************************************************************************
   Constants
****************************************************************************/

/* Drawing scales */
#define MIN_SCALE          1
#define MAX_SCALE          256
#define DEFAULT_SCALE      8


/****************************************************************************
   Globals
****************************************************************************/

extern POINT   drawSize;      /* drawing size in pixels */
extern HDC     drawDC;        /* drawing DC */
extern BOOL    isBlank;       /* TRUE iff drawing is blank */
extern BOOL    pixGrid;       /* TRUE iff drawing has pixel grid and scale */
extern int     viewScale;     /* viewing scale multiple */


/****************************************************************************
   Functions
****************************************************************************/

void ViewCmd( int item );
int NewView( void );
void ClearView( void );
void PaintView( HDC hdc, RECT r );
void RefreshView( BOOL erase );
void CheckViewMenuItems( HMENU hMenu );

