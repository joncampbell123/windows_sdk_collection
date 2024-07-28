/****************************************************************************
 Draw.c

 The Draw module handles the GDI drawing.

****************************************************************************/

#include "windows.h"
#include <string.h>

#include "MENU.h"

#include "util.h"
#include "main.h"
#include "dc.h"
#include "view.h"
#include "dib.h"
#include "dialogs.h"

#include "draw.h"


/****************************************************************************
   Globals
****************************************************************************/

/* The global drawing values */
DrawValues  dv;


/****************************************************************************
   Functions
****************************************************************************/

int DrawCmdDialog( int item )
/* Run the dialog for menu item 'item' from the Draw menu and return
   TRUE iff the dialog was Ok'ed. */
{
   int   ok = FALSE;

   /* Run the appropriate dialog */
   switch (item) {
      case IDM_SETPIXEL:      ok = Dlg( SetPixelDlg );      break;
      case IDM_LINETO:        ok = Dlg( LineToDlg );        break;
      case IDM_LINE:          ok = Dlg( LineDlg );          break;
      case IDM_RECTANGLE:     ok = Dlg( RectangleDlg );     break;
      case IDM_ELLIPSE:       ok = Dlg( EllipseDlg );       break;
      case IDM_ROUNDRECT:     ok = Dlg( RoundRectDlg );     break;
      case IDM_ARC:           ok = Dlg( ArcDlg );           break;
      case IDM_PIE:           ok = Dlg( PieDlg );           break;
      case IDM_CHORD:         ok = Dlg( ChordDlg );         break;
      case IDM_POLYLINE:      ok = Dlg( PolylineDlg );      break;
      case IDM_POLYGON:       ok = Dlg( PolygonDlg );       break;
      case IDM_POLYPOLYGON:   ok = Dlg( PolyPolygonDlg );   break;
      case IDM_PATBLT:        ok = Dlg( PatBltDlg );        break;
      case IDM_BITBLT:        ok = Dlg( BitBltDlg );        break;
      case IDM_STRETCHBLT:    ok = Dlg( StretchBltDlg );    break;
      case IDM_DIBTODEVICE:   ok = Dlg( DIBToDeviceDlg );   break;
      case IDM_STRETCHDIB:    ok = Dlg( StretchDIBDlg );    break;
      case IDM_TEXTOUT:       ok = Dlg( TextOutDlg );       break;
      case IDM_EXTTEXTOUT:    ok = Dlg( ExtTextOutDlg );    break;
      case IDM_DRAWTEXT:      ok = Dlg( DrawTextDlg );      break;
      case IDM_FLOODFILL:     ok = Dlg( FloodFillDlg );     break;
      case IDM_EXTFLOODFILL:  ok = Dlg( ExtFloodFillDlg );  break;
   }

   return ok;
}


void DrawCmd( int item )
/* Run the command for menu item 'item' from the Draw menu. */
{
   /* Run the appropriate dialog and return if it is cancelled. */
   if( !DrawCmdDialog( item ) ) {
      return;
   }

   /* Setup the DC for drawing with the current attributes */
   SaveDC( drawDC );
   SetupDC( drawDC );

   /* Draw the appropriate API */
   StartWait();
   switch (item) {
      case IDM_SETPIXEL:
         SetPixel( drawDC, dv.pt.x, dv.pt.y, dv.rgb );
         break;
   
      case IDM_LINETO:
         LineTo( drawDC, dv.pt.x, dv.pt.y );
         break;
   
      case IDM_LINE:
         MoveTo( drawDC, dv.ptFrom.x, dv.ptFrom.y );
         LineTo( drawDC, dv.pt.x, dv.pt.y );
         break;
   
      case IDM_RECTANGLE:
         Rectangle( drawDC, dv.r.left, dv.r.top, dv.r.right, dv.r.bottom );
         break;
   
      case IDM_ELLIPSE:
         Ellipse( drawDC, dv.r.left, dv.r.top, dv.r.right, dv.r.bottom );
         break;
   
      case IDM_ROUNDRECT:
         RoundRect( drawDC, dv.r.left, dv.r.top, dv.r.right, dv.r.bottom,
                    dv.corner.x, dv.corner.y );
         break;
   
      case IDM_ARC:
         Arc( drawDC, dv.r.left, dv.r.top, dv.r.right, dv.r.bottom,
              dv.startPt.x, dv.startPt.y, dv.endPt.x, dv.endPt.y );
         break;
   
      case IDM_PIE:
         Pie( drawDC, dv.r.left, dv.r.top, dv.r.right, dv.r.bottom,
              dv.startPt.x, dv.startPt.y, dv.endPt.x, dv.endPt.y );
         break;
   
      case IDM_CHORD:
         Chord( drawDC, dv.r.left, dv.r.top, dv.r.right, dv.r.bottom,
                dv.startPt.x, dv.startPt.y, dv.endPt.x, dv.endPt.y );
         break;
   
      case IDM_POLYLINE:
         Polyline( drawDC, (LPPOINT) dv.pts, dv.nPts );
         break;
   
      case IDM_POLYGON:
         Polygon( drawDC, (LPPOINT) dv.pts, dv.nPts );
         break;
   
      case IDM_POLYPOLYGON:
         PolyPolygon( drawDC, (LPPOINT) dv.pts, (LPINT) dv.counts, dv.nPolys );
         break;
   
      case IDM_PATBLT:
         PatBlt( drawDC, dv.r.left, dv.r.top, Width( dv.r ), 
                 Height( dv.r ), dv.rop );
         break;
   
      case IDM_BITBLT:
         BitBlt( drawDC, dv.r.left, dv.r.top, Width( dv.r ),
                 Height( dv.r ), drawDC, 
                 dv.rFrom.left, dv.rFrom.top, dv.rop );
         break;
   
      case IDM_STRETCHBLT:
         StretchBlt( drawDC, dv.r.left, dv.r.top, Width( dv.r ),
                     Height( dv.r ), drawDC, 
                     dv.rFrom.left, dv.rFrom.top, 
                     Width( dv.rFrom ), Height( dv.rFrom ), dv.rop );
         break;
   
      case IDM_DIBTODEVICE:
         DibBlt( drawDC, dv.r.left, dv.r.top, curDIB,
                 dv.rFrom.left, dv.rFrom.top, 
                 Width( dv.rFrom ), Height( dv.rFrom ),
                 dv.startScan, dv.numScans );
         break;
   
      case IDM_STRETCHDIB:
         StretchDibBlt( drawDC, dv.r.left, dv.r.top, Width( dv.r ),
                        Height( dv.r ), curDIB, 
                        dv.rFrom.left, dv.rFrom.top, 
                        Width( dv.rFrom ), Height( dv.rFrom ), dv.rop );
         break;
   
      case IDM_TEXTOUT:
         TextOut( drawDC, dv.pt.x, dv.pt.y, (LPSTR) dv.text, strlen( dv.text ) );
         break;
   
      case IDM_EXTTEXTOUT:
         ExtTextOut( drawDC, dv.pt.x, dv.pt.y, dv.options, (LPRECT) &dv.r,
                     (LPSTR) dv.text, strlen( dv.text ), 
                     (dv.useDx ? (LPINT) dv.dxArray : (LPINT) NULL) );
         break;
   
      case IDM_DRAWTEXT:
         DrawText( drawDC, (LPSTR) dv.text, -1, (LPRECT) &dv.r, dv.format );
         /* If DT_CALCRECT was used, then no output occurred so we draw the
            resulting rectangle instead. */
         if (!(dv.format & DT_TABSTOP) && (dv.format & DT_CALCRECT)) {
            Rectangle( drawDC, dv.r.left, dv.r.top, dv.r.right, dv.r.bottom );
         }
         break;
   
      case IDM_FLOODFILL:
         FloodFill( drawDC, dv.pt.x, dv.pt.y, dv.rgb );
         break;
   
      case IDM_EXTFLOODFILL:
         ExtFloodFill( drawDC, dv.pt.x, dv.pt.y, dv.rgb, dv.floodType );
         break;
   }
   EndWait();

   /* Restore the drawing DC to it's virgin state */   
   CleanupDC( drawDC );
   RestoreDC( drawDC, -1 );

   /* Redraw the view */   
   isBlank = FALSE;
   RefreshView( FALSE );
}

