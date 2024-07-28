/****************************************************************************
 View.c

 The View module handles the display of the drawing view.

****************************************************************************/

#include "windows.h"
#include <stdlib.h>
#include <string.h>

#include "MENU.h"

#include "util.h"
#include "main.h"
#include "dialogs.h"
#include "dc.h"

#include "view.h"


/****************************************************************************
   Private constants
****************************************************************************/

#define MIN_LABEL_MARGIN   2     /* minimum margin between labels (pixels) */
#define TICK_HEIGHT        4     /* height of tick for grid label (pixels) */
#define TICK_WIDTH         4     /* width of tick for grid label (pixels) */


/****************************************************************************
   Globals
****************************************************************************/

POINT    drawSize = { 256, 256 };   /* size of off-screen bitmap in pixels */
HDC      drawDC = NULL;             /* the off-screen DC */
BOOL     isBlank = TRUE;            /* TRUE iff drawing is blank */
BOOL     pixGrid = TRUE;            /* TRUE iff pixel grid showing */
int      viewScale = DEFAULT_SCALE; /* scale factor for drawing -> view */


/****************************************************************************
   Private Data
****************************************************************************/

static HBITMAP  stockBmp = NULL;           /* stock off-screen bitmap */
static HFONT    labelFont = NULL;          /* font for grid labels */
static int      labelFontAscent;           /* ascent of labelFont */
static DWORD    rgbMajorGrid = RGB(0, 0, 255);      /* color of major grid */
static DWORD    rgbMinorGrid = RGB(128, 128, 128);  /* color of minor grid */


/****************************************************************************
   Functions
****************************************************************************/

int NewView( void )
/* Create a new off-screen drawing (destroy existing one if any). */
{
   HDC         hdcMain = NULL;
   HBITMAP     drawBmp = NULL;
   LOGFONT     lf;
   TEXTMETRIC  tm;
   HFONT       prevFont;

   /* Destroy existing DC and bitmap, if any */
   if (drawDC != NULL) {
      drawBmp = SelectObject( drawDC, stockBmp );
      if (drawBmp != NULL) {
         DeleteObject( drawBmp );
      }
      DeleteDC( drawDC );
   }

   /* Create a compatible DC and off-screen bitmap */    
   hdcMain = GetDC( hwndMain );
   drawDC = CreateCompatibleDC( hdcMain );
   if (drawDC == NULL)
      goto errorExit;
   drawBmp = CreateCompatibleBitmap( hdcMain, drawSize.x, drawSize.y );
   if (drawBmp == NULL)
      goto errorExit;
   stockBmp = SelectObject( drawDC, drawBmp );

   /* Create label font if not already created -- use a small Helv font. */
   if (labelFont == NULL) {
      lf.lfHeight = 10;
      lf.lfWidth = 0;
      lf.lfEscapement = lf.lfOrientation = 0;
      lf.lfWeight = 400;
      lf.lfItalic = lf.lfUnderline = lf.lfStrikeOut = 0;
      lf.lfCharSet = ANSI_CHARSET;
      lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
      lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
      lf.lfQuality = PROOF_QUALITY;    /* no stretching */
      lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
      strcpy( lf.lfFaceName, "Helv");
      labelFont = CreateFontIndirect( &lf );
      if (labelFont == NULL) 
         goto errorExit;
      prevFont = SelectObject( hdcMain, labelFont );
      GetTextMetrics( hdcMain, &tm );
      labelFontAscent = tm.tmAscent - tm.tmInternalLeading;
      SelectObject( hdcMain, prevFont );
   }

   ReleaseDC( hwndMain, hdcMain );
   ClearView();
   return TRUE;

errorExit:
   if (drawBmp != NULL) {
      DeleteObject( drawBmp );
      drawBmp = NULL;
   }
   if (drawDC != NULL) {
      DeleteDC( drawDC );
      drawDC = NULL;
   }
   ReleaseDC( hwndMain, hdcMain );
   return FALSE;
}


void ClearView( void )
/* Clear the drawing. */
{
   RECT  r;
   
   SetRect( &r, -BIGINT, -BIGINT, BIGINT, BIGINT );
   FillRect( drawDC, &r, (HBRUSH) GetStockObject( WHITE_BRUSH ) );
   isBlank = TRUE;
}


void PaintView( HDC hdc, RECT r )
/* Repaint the view of the drawing in portion 'r' of 'hdc'. */
{
   RECT     rDraw, rView, rBlt, rGray;
   HPEN     penMajor, penMinor, prevPen;
   HFONT    prevFont;
   int      x, y, xEnd, yEnd;
   int      labelHeight, xLabelWidth, yLabelWidth;
   int      labelStep;
   int      labelValue;
   char     label[8];
   int      shift;

   StartWait();

   /* Compute drawing view rectangle and needed drawing metrics */
   GetClientRect( hwndMain, &rView );
   if (pixGrid ) {
      /* Create pens for major and minor gridlines and select label font */
      penMajor = CreatePen( PS_SOLID, 0, rgbMajorGrid );
      penMinor = CreatePen( PS_SOLID, 0, rgbMinorGrid );
      prevPen = SelectObject( hdc, penMinor );
      SetTextColor( hdc, rgbMajorGrid );
      prevFont = SelectObject( hdc, labelFont );

      /* Compute height for labels */
      labelHeight = HIWORD( GetTextExtent( hdc, "0", 1 ) ) + MIN_LABEL_MARGIN;

      /* Compute width for labels on the x scale as approx max of leftmost
         and rightmost label widths plus minimum pixel margin. */
      itoa( -dcv.viewportOrg.x, label, 10 );
      xLabelWidth = LOWORD( GetTextExtent( hdc, label, strlen( label ) ) )
            + MIN_LABEL_MARGIN;
      itoa( -dcv.viewportOrg.x + Width( rView ) / viewScale + 1, label, 10 );
      xLabelWidth = Max( xLabelWidth, LOWORD( GetTextExtent( hdc, label, 
            strlen( label ) ) ) + MIN_LABEL_MARGIN );

      /* Compute width for labels on the y scale as approx max of topmost
         and bottommost label widths plus minimum pixel margin. */
      itoa( -dcv.viewportOrg.y, label, 10 );
      yLabelWidth = LOWORD( GetTextExtent( hdc, label, strlen( label ) ) )
            + MIN_LABEL_MARGIN;
      itoa( -dcv.viewportOrg.y + Height( rView ) / viewScale + 1, label, 10 );
      yLabelWidth = Max( yLabelWidth, LOWORD( GetTextExtent( hdc, label, 
            strlen( label ) ) ) + MIN_LABEL_MARGIN );

      /* Compute step for label values so that step is a power of 2 and 
         allows at least enough room for the x label width. */
      labelStep = xLabelWidth / viewScale + 1;
      for (shift = 0; labelStep > 1; shift++) {
         labelStep >>= 1;
      }
      labelStep <<= (shift + 1);

      /* Allow room for scale area to top and left of viewing rectangle */
      rView.left += yLabelWidth + TICK_WIDTH + MIN_LABEL_MARGIN;
      rView.top += labelHeight + TICK_HEIGHT + MIN_LABEL_MARGIN;
   }
   xEnd = Min( rView.right, 
               ToIntegerPin( rView.left + (long) drawSize.x * viewScale ) );
   yEnd = Min( rView.bottom, 
               ToIntegerPin( rView.top + (long) drawSize.y * viewScale ) );

   /* Gray the off-drawing portion of the view rectangle, if any */
   if (xEnd < rView.right ) {
      SetRect( &rGray, xEnd + 1, rView.top, rView.right + 1, rView.bottom + 1 );
      FillRect( hdc, &rGray, GetStockObject( GRAY_BRUSH ) );
   }
   if (yEnd < rView.bottom ) {
      SetRect( &rGray, rView.left, yEnd + 1, rView.right + 1, rView.bottom + 1 );
      FillRect( hdc, &rGray, GetStockObject( GRAY_BRUSH ) );
   }
   
   /* Copy update portion of drawing from off-screen bitmap */
   IntersectRect( &rBlt, &r, &rView );
   rDraw.left = Max( 0, Min( drawSize.x, 
         (rBlt.left - rView.left) / viewScale - 1) );
   rDraw.top = Max( 0, Min( drawSize.y, 
         (rBlt.top - rView.top) / viewScale - 1) );
   rDraw.right = Max( 0, Min( drawSize.x, 
         (rBlt.right - rView.left) / viewScale + 1) );
   rDraw.bottom = Max( 0, Min( drawSize.y, 
         (rBlt.bottom - rView.top) / viewScale + 1) );
   rBlt.left   = rDraw.left * viewScale + rView.left;
   rBlt.top    = rDraw.top * viewScale + rView.top;
   rBlt.right  = rDraw.right * viewScale + rView.left;
   rBlt.bottom = rDraw.bottom * viewScale + rView.top;
   if (isBlank) {
      rBlt.right += 1;
      rBlt.bottom += 1;
      FillRect( hdc, &rBlt, GetStockObject( WHITE_BRUSH ) );
   }else{
      StretchBlt( hdc, rBlt.left, rBlt.top, Width( rBlt ), Height( rBlt ),
                  drawDC, rDraw.left, rDraw.top, Width( rDraw ), 
                  Height( rDraw ), SRCCOPY );
   }

   /* Draw pixel grid and scale if showing */
   if (pixGrid) {
      /* Draw major and minor x grid lines, and ticks with labels at each 
         major grid line (each label step) */
      SetTextAlign( hdc, TA_CENTER | TA_BOTTOM );
      for (x = rView.left; x <= xEnd; x += viewScale ) {
         labelValue = (x - rView.left) / viewScale - dcv.viewportOrg.x;
         MoveTo( hdc, x, yEnd );
         if (labelValue % labelStep == 0) {
            /* Major gridline with tick and label */
            SelectObject( hdc, penMajor );
            LineTo( hdc, x, rView.top - TICK_HEIGHT );
            itoa( labelValue, label, 10 );
            TextOut( hdc, x, rView.top - TICK_HEIGHT, label, strlen( label ) );
         }else if (viewScale > 2) {
            /* Minor gridline */
            SelectObject( hdc, penMinor );
            LineTo( hdc, x, rView.top );
         }
      }

      /* Draw major and minor y grid lines, and ticks with labels at each 
         major grid line (each label step) */
      SetTextAlign( hdc, TA_RIGHT | TA_BASELINE );
      for (y = rView.top; y <= yEnd; y += viewScale ) {
         labelValue = (y - rView.top) / viewScale - dcv.viewportOrg.y;
         MoveTo( hdc, xEnd, y );
         if (labelValue % labelStep == 0) {
            /* Major gridline with tick and label */
            SelectObject( hdc, penMajor );
            LineTo( hdc, rView.left - TICK_WIDTH, y );
            itoa( labelValue, label, 10 );
            TextOut( hdc, rView.left - TICK_WIDTH - MIN_LABEL_MARGIN, 
                     y + labelFontAscent / 2, label, strlen( label ) );
         }else if (viewScale > 2) {
            /* Minor gridline */
            SelectObject( hdc, penMinor );
            LineTo( hdc, rView.left, y );
         }
      }

      /* Clean up */
      SelectObject( hdc, prevPen );
      DeleteObject( penMajor );
      DeleteObject( penMinor );
      SelectObject( hdc, prevFont );
   }

   EndWait();
}


void RefreshView( BOOL erase )
/* Invalidate the screen view for redraw, erasing first iff 'erase'. */
{
   InvalidateRect( hwndMain, NULL, erase );
}


void ViewCmd( int item )
/* Run the command for menu item 'item' from the View menu. */
{
   switch (item) {
      case IDM_CLEAR:
         ClearView();
         RefreshView( FALSE );
         break;

      case IDM_DRAWINGSIZE:
         if (Dlg( DrawingSizeDlg )) {
            NewView();
            RefreshView( TRUE );
         }
         break;

      case IDM_NORMALSIZE:
         viewScale = 1;
         RefreshView( TRUE );
         break;

      case IDM_ZOOMIN:
         viewScale *= 2;
         if (viewScale > MAX_SCALE ) {
            viewScale = MAX_SCALE;
         }
         RefreshView( TRUE );
         break;

      case IDM_ZOOMOUT:
         viewScale /= 2;
         if (viewScale < MIN_SCALE ) {
            viewScale = MIN_SCALE;
         }
         RefreshView( TRUE );
         break;

      case IDM_SETSCALE:
         if (Dlg( SetScaleDlg )) {
            RefreshView( TRUE );
         }
         break;

      case IDM_PIXGRID:
         pixGrid ^= TRUE;
         RefreshView( TRUE );
         break;
   }
}


void CheckViewMenuItems( HMENU hMenu )
/* Update enabling and checks on the menus in the View menu. */
{
   EnableMenuCmd( hMenu, IDM_ZOOMIN, (viewScale < MAX_SCALE) );
   EnableMenuCmd( hMenu, IDM_ZOOMOUT, (viewScale > MIN_SCALE) );
   CheckMenuCmd( hMenu, IDM_PIXGRID, pixGrid );
}
