/****************************************************************************
 DC.c

 The DC module handles setting and reading of the DC attributes.

****************************************************************************/

#include "windows.h"

#include "MENU.h"

#include "util.h"
#include "main.h"
#include "view.h"
#include "dib.h"
#include "dialogs.h"

#include "dc.h"


/****************************************************************************
   Globals
****************************************************************************/

/* The global DC settings */
DCValues dcv;


/****************************************************************************
   Private Data
****************************************************************************/

/* Saved objects used by SetupDC() and CleanupDC() */
static HFONT    prevFont;
static HBRUSH   prevBrush;
static HPEN     prevPen;


/****************************************************************************
   Functions
****************************************************************************/
                         
void DCCmd( int item )
/* Run the command for menu item 'item' from the DC menu group. */
{
   HDC   hdcMain;

   switch( item )
   {
      case IDM_DEFAULTS:
         hdcMain = GetDC( hwndMain );     /* will get default values */
         ReadDC( hdcMain );
         ReleaseDC( hwndMain, hdcMain );
         RefreshView( TRUE );             /* to redraw grid lines */
         break;
         
      case IDM_COORDS:        
         if (Dlg( CoordsDlg ))
            RefreshView( TRUE );          /* to redraw grid lines */
         break;
         
      case IDM_CLIP:          Dlg( ClipDlg );                     break;
      case IDM_BKCOLOR:       Dlg( BkColorDlg );                  break;
      case IDM_BKMODE:        Dlg( BkModeDlg );                   break;
      case IDM_BRUSHORG:      Dlg( BrushOrgDlg );                 break;
      case IDM_PENPOS:        Dlg( PenPosDlg );                   break;
      case IDM_FONT:          Dlg( FontDlg );                     break;
      case IDM_TEXTALIGN:     Dlg( TextAlignDlg );                break;
      case IDM_TEXTJUST:      Dlg( TextJustDlg );                 break;
      case IDM_TEXTEXTRA:     Dlg( TextExtraDlg );                break;
      case IDM_TEXTCOLOR:     Dlg( TextColorDlg );                break;
      case IDM_ROP2:          Dlg( ROP2Dlg );                     break;
      case IDM_POLYMODE:      Dlg( PolyModeDlg );                 break;
      case IDM_BLTMODE:       Dlg( BltModeDlg );                  break;

      case IDM_NULLBRUSH:     dcv.brush.lbStyle = BS_NULL;        break;
      case IDM_SOLIDBRUSH:    Dlg( SolidBrushDlg );               break;
      case IDM_HATCHBRUSH:    Dlg( HatchBrushDlg );               break;
      case IDM_PATBRUSH:      Dlg( PatBrushDlg );                 break;
      case IDM_DIBPATBRUSH:   Dlg( DIBPatBrushDlg );              break;
                              
      case IDM_NULLPEN:       dcv.pen.lopnStyle = PS_NULL;        break;
      case IDM_SOLIDPEN:      dcv.pen.lopnStyle = PS_SOLID;       break;
      case IDM_INSIDEFRAMEPEN:dcv.pen.lopnStyle = PS_INSIDEFRAME; break;
      case IDM_DASHPEN:       dcv.pen.lopnStyle = PS_DASH;        break;
      case IDM_DOTPEN:        dcv.pen.lopnStyle = PS_DOT;         break;
      case IDM_DASHDOTPEN:    dcv.pen.lopnStyle = PS_DASHDOT;     break;
      case IDM_DASHDOTDOTPEN: dcv.pen.lopnStyle = PS_DASHDOTDOT;  break;
      case IDM_PENWIDTH:      Dlg( PenWidthDlg );                 break;
      case IDM_PENCOLOR:      Dlg( PenColorDlg );                 break;
   } /* switch */
}


void ReadDC( HDC hdc )
/* Read the settings from 'hdc' into the global DC settings 'dcv'. */
{
   HFONT    stockFont;
   HPEN     stockPen;
   HBRUSH   stockBrush;

   dcv.mapMode = GetMapMode( hdc );
   *((DWORD *) &dcv.windowOrg) = GetWindowOrg( hdc );
   *((DWORD *) &dcv.windowExt) = GetWindowExt( hdc );
   *((DWORD *) &dcv.viewportOrg) = GetViewportOrg( hdc );
   *((DWORD *) &dcv.viewportExt) = GetViewportExt( hdc );

   *((DWORD *) &dcv.penPos) = GetCurrentPosition( hdc );
   *((DWORD *) &dcv.brushOrg) = GetBrushOrg( hdc );

   dcv.exclude = dcv.intersect = FALSE;

   dcv.bkColor = GetBkColor( hdc );
   dcv.bkMode = GetBkMode( hdc );
   
   stockFont = GetStockObject( SYSTEM_FONT );
   dcv.hFont = SelectObject( hdc, stockFont );
   GetObject( dcv.hFont, sizeof( LOGFONT ), (LPSTR) &dcv.font );
   SelectObject( hdc, dcv.hFont );

   dcv.textAlign = GetTextAlign( hdc );
   dcv.breakExtra = dcv.breakCount = 0;
   dcv.textExtra = GetTextCharacterExtra( hdc );
   dcv.textColor = GetTextColor( hdc );

   dcv.rop2 = GetROP2( hdc );
   dcv.polyFillMode = GetPolyFillMode( hdc );
   dcv.bltMode = GetStretchBltMode( hdc );

   stockBrush = GetStockObject( NULL_BRUSH );
   dcv.hBrush = SelectObject( hdc, stockBrush );
   GetObject( dcv.hBrush, sizeof( LOGBRUSH ), (LPSTR) &dcv.brush );
   SelectObject( hdc, dcv.hBrush );

   stockPen = GetStockObject( NULL_PEN );
   dcv.hPen = SelectObject( hdc, stockPen );
   GetObject( dcv.hPen, sizeof( LOGPEN ), (LPSTR) &dcv.pen );
   SelectObject( hdc, dcv.hPen );
}


HBITMAP MakePatternBitmap( char checks[64] )
/* Create a pattern bitmap by treating 'checks' as an array of booleans
   where a check indicates a 0 bit. */
{
   unsigned    i, mask;
   short       shorts[8];

   for (i = 0; i < 64; i++) {
      mask = 1 << (7 - (i % 8));
      if (checks[i]) {
         shorts[i / 8] &= ~mask;
      }else{
         shorts[i / 8] |= mask;
      }
   }
   return CreateBitmap( 8, 8, 1, 1, (LPSTR) shorts );
}


void SetupDC( HDC hdc )
/* Setup 'hdc' based on the global DC settings 'dcv'. */
{
   COLORREF savedBrushColor;

   SetMapMode( hdc, dcv.mapMode );
   SetWindowOrg( hdc, dcv.windowOrg.x, dcv.windowOrg.y );
   SetWindowExt( hdc, dcv.windowExt.x, dcv.windowExt.y );
   SetViewportOrg( hdc, dcv.viewportOrg.x, dcv.viewportOrg.y );
   SetViewportExt( hdc, dcv.viewportExt.x, dcv.viewportExt.y );

   MoveTo( hdc, dcv.penPos.x, dcv.penPos.y );
   SetBrushOrg( hdc, dcv.brushOrg.x, dcv.brushOrg.y );

   if( dcv.intersect ) {
      IntersectClipRect( hdc, dcv.rIntersect.left, dcv.rIntersect.top, 
                         dcv.rIntersect.right, dcv.rIntersect.bottom );
   }
   if( dcv.exclude ) {
      ExcludeClipRect( hdc, dcv.rExclude.left, dcv.rExclude.top, 
                       dcv.rExclude.right, dcv.rExclude.bottom );
   }

   SetBkColor( hdc, dcv.bkColor );
   SetBkMode( hdc, dcv.bkMode );
   
   dcv.hFont = CreateFontIndirect( &dcv.font );
   if( dcv.hFont == NULL ) {
      SetupError("CreateFontIndirect() failed (invalid LOGFONT)");
      prevFont = NULL;
   }else{
      prevFont = SelectObject( hdc, dcv.hFont );
   }

   SetTextAlign( hdc, dcv.textAlign );
   SetTextJustification( hdc, dcv.breakExtra, dcv.breakCount );
   SetTextCharacterExtra( hdc, dcv.textExtra );
   SetTextColor( hdc, dcv.textColor );

   SetROP2( hdc, dcv.rop2 );
   SetPolyFillMode( hdc, dcv.polyFillMode );
   SetStretchBltMode( hdc, dcv.bltMode );

   savedBrushColor = dcv.brush.lbColor;
   if (dcv.brush.lbStyle == BS_PATTERN) {
      dcv.brush.lbHatch = (short) MakePatternBitmap( dcv.patChecks );
   }else if (dcv.brush.lbStyle == BS_DIBPATTERN) {
      dcv.brush.lbHatch = (short) curDIB;
      dcv.brush.lbColor = (COLORREF) DIB_RGB_COLORS;
   }
   dcv.hBrush = CreateBrushIndirect( &dcv.brush );
   if( dcv.hBrush == NULL ) {
      SetupError("CreateBrushIndirect() failed (invalid LOGBRUSH)");
      prevBrush = NULL;
   }else{
      prevBrush = SelectObject( hdc, dcv.hBrush );
   }
   dcv.brush.lbColor = savedBrushColor;

   dcv.hPen = CreatePenIndirect( &dcv.pen );
   if( dcv.hPen == NULL ) {
      SetupError("CreatePenIndirect() failed (invalid LOGPEN)");
      prevPen = NULL;
   }else{
      prevPen = SelectObject( hdc, dcv.hPen );
   }
}


void SetupError( char *string )
/* Alert and take action for an error during DC setup. */
{
   MessageBox( hwndMain, (LPSTR) string, (LPSTR) "DC Setup Error", MB_OK );
}


void CleanupDC( HDC hdc )
/* Remove and delete any objects selected into 'hdc' by SetupDC(), and
   perform any other post-drawing work for the global DC setting 'dcv'. */
{
   /* Update DC setting fields which drawing can change */
   *((DWORD *) &dcv.penPos) = GetCurrentPosition( hdc );

   /* Clear the text justification since it's not a retained attribute */
   dcv.breakExtra = dcv.breakCount = 0;

   /* Deselect and delete any objects created */
   if (dcv.brush.lbStyle == BS_PATTERN) {
      DeleteObject( (HBITMAP) dcv.brush.lbHatch );
   }
   if (prevFont != NULL) {
      DeleteObject( SelectObject( hdc, prevFont ) );
   }
   if (prevBrush != NULL) {
      DeleteObject( SelectObject( hdc, prevBrush ) );
   }
   if (prevPen != NULL) {
      DeleteObject( SelectObject( hdc, prevPen ) );
   }
}


void CheckDCMenuItems( HMENU hMenu )
/* Update enabling and checks on the menus in the DC menu group. */
{
   CheckMenuCmd( hMenu, IDM_NULLPEN, (dcv.pen.lopnStyle == PS_NULL) );
   CheckMenuCmd( hMenu, IDM_SOLIDPEN, (dcv.pen.lopnStyle == PS_SOLID) );
   CheckMenuCmd( hMenu, IDM_INSIDEFRAMEPEN, (dcv.pen.lopnStyle == PS_INSIDEFRAME) );
   CheckMenuCmd( hMenu, IDM_DASHPEN, (dcv.pen.lopnStyle == PS_DASH) );
   CheckMenuCmd( hMenu, IDM_DOTPEN, (dcv.pen.lopnStyle == PS_DOT) );
   CheckMenuCmd( hMenu, IDM_DASHDOTPEN, (dcv.pen.lopnStyle == PS_DASHDOT) );
   CheckMenuCmd( hMenu, IDM_DASHDOTDOTPEN, (dcv.pen.lopnStyle == PS_DASHDOTDOT) );

   CheckMenuCmd( hMenu, IDM_NULLBRUSH, (dcv.brush.lbStyle == BS_NULL) );
   CheckMenuCmd( hMenu, IDM_SOLIDBRUSH, (dcv.brush.lbStyle == BS_SOLID) );
   CheckMenuCmd( hMenu, IDM_HATCHBRUSH, (dcv.brush.lbStyle == BS_HATCHED) );
   CheckMenuCmd( hMenu, IDM_PATBRUSH, (dcv.brush.lbStyle == BS_PATTERN) );
   CheckMenuCmd( hMenu, IDM_DIBPATBRUSH, (dcv.brush.lbStyle == BS_DIBPATTERN) );
}
