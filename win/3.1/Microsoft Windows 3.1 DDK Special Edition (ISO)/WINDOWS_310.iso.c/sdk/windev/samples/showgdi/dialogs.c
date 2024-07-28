/****************************************************************************
 Dialogs.c

 The Dialogs module contains all of the dialog handler procs.

****************************************************************************/

#include "windows.h"
#include <stdlib.h>

#include "DLG.h"

#include "util.h"
#include "main.h"
#include "view.h"
#include "dc.h"
#include "draw.h"
#include "dib.h"

#include "dialogs.h"


/****************************************************************************
   General dialog utilities
****************************************************************************/

#define GetIntField( hDlg, item )   \
   ((int) GetDlgItemInt( hDlg, item, NULL, TRUE))

#define SetIntField( hDlg, item, value  )   \
   SetDlgItemInt( hDlg, item, value, TRUE)

#define GetCheckField( hDlg, item ) \
   (SendDlgItemMessage( hDlg, item, BM_GETCHECK, 0, 0L) != 0)

#define SetCheckField( hDlg, item, value ) \
   SendDlgItemMessage( hDlg, item, BM_SETCHECK, value, 0L)

#define EnableItem( hDlg, item, enable ) \
   EnableWindow( GetDlgItem( hDlg, item ), enable )


int RunDlg( LPSTR name, FARPROC procInst, HWND parent )
/* Run the dialog template 'name' with handler proc given by 'procInst'
   which was calculated via MakeProcInstance() and return the result. */
{
   int   result;

   result = DialogBox( hInst, name, parent, procInst );
   FreeProcInstance( procInst );
   return result;
}


void CenterDialog( HWND hDlg )
/* Center the dialog w.r.t. hwndMain. */
{
   RECT  mainRect, dlgRect;
   int   screenWidth, screenHeight;
   int   x, y;

   /* Center's its dialogs relative to the main window. However,
      if the window is off the screen, the whole dialog is shown. */
   GetWindowRect( hwndMain, &mainRect);
   GetWindowRect( hDlg, &dlgRect );
   screenWidth = GetSystemMetrics( SM_CXSCREEN );
   screenHeight = GetSystemMetrics( SM_CYSCREEN );

   /* Calculate the new position; first center, then ensure the whole dialog
      is on the screen */
   x = (Width( mainRect )-Width( dlgRect )) / 2 + mainRect.left;
   y = (Height( mainRect )-Height( dlgRect )) / 2 + mainRect.top;
   x = Max( 0, Min( screenWidth - Width( dlgRect ), x ));
   y = Max( 0, Min( screenHeight - Height( dlgRect ), y ));
   SetWindowPos( hDlg, NULL, x, y, 0, 0, 
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
}


LPSTR FileNameFromPath( LPSTR path )
/* Return the filename portion of a pathname. */
{
   LPSTR p;

   if (path == NULL)
      return NULL;

   p = path + lstrlen( path ) - 1;
   while( p > path && *p != '\\' && *p != ':' )
      p--;
   return (p > path) ? (p + 1) : path;
}


/****************************************************************************
   About box
****************************************************************************/

DLGPROC AboutDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


/****************************************************************************
   View menu dialogs
****************************************************************************/

DLGPROC SetScaleDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   newScale;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_SCALE, viewScale );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            newScale = GetIntField( hDlg, IF_SCALE );
            if (newScale >= MIN_SCALE && newScale <= MAX_SCALE) {
               viewScale = newScale;
               EndDialog( hDlg, TRUE );
            }else{
               SetIntField( hDlg, IF_SCALE, Max( MIN_SCALE, Min( MAX_SCALE,
                     newScale ) ) );
            }
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC DrawingSizeDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_X, drawSize.x );
         SetIntField( hDlg, IF_Y, drawSize.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            drawSize.x = GetIntField( hDlg, IF_X );
            drawSize.y = GetIntField( hDlg, IF_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


/****************************************************************************
   Dialogs progs for DC settings
****************************************************************************/

DLGPROC CoordsDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetCheckField( hDlg, BF_MM_TEXT, (dcv.mapMode == MM_TEXT) );
         SetCheckField( hDlg, BF_MM_TWIPS, (dcv.mapMode == MM_TWIPS) );
         SetCheckField( hDlg, BF_MM_ISOTROPIC, (dcv.mapMode == MM_ISOTROPIC) );
         SetCheckField( hDlg, BF_MM_ANISOTROPIC, (dcv.mapMode == MM_ANISOTROPIC) );
         SetCheckField( hDlg, BF_MM_LOMETRIC, (dcv.mapMode == MM_LOMETRIC) );
         SetCheckField( hDlg, BF_MM_HIMETRIC, (dcv.mapMode == MM_HIMETRIC) );
         SetCheckField( hDlg, BF_MM_LOENGLISH, (dcv.mapMode == MM_LOENGLISH) );
         SetCheckField( hDlg, BF_MM_HIENGLISH, (dcv.mapMode == MM_HIENGLISH) );
         SetIntField( hDlg, IF_VO_X, dcv.viewportOrg.x );
         SetIntField( hDlg, IF_VO_Y, dcv.viewportOrg.y );
         SetIntField( hDlg, IF_VE_X, dcv.viewportExt.x );
         SetIntField( hDlg, IF_VE_Y, dcv.viewportExt.y );
         SetIntField( hDlg, IF_WO_X, dcv.windowOrg.x );
         SetIntField( hDlg, IF_WO_Y, dcv.windowOrg.y );
         SetIntField( hDlg, IF_WE_X, dcv.windowExt.x );
         SetIntField( hDlg, IF_WE_Y, dcv.windowExt.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            if (GetCheckField( hDlg, BF_MM_TEXT ))
               dcv.mapMode = MM_TEXT;
            else if (GetCheckField( hDlg, BF_MM_TWIPS ))
               dcv.mapMode = MM_TWIPS;
            else if (GetCheckField( hDlg, BF_MM_ISOTROPIC ))
               dcv.mapMode = MM_ISOTROPIC;
            else if (GetCheckField( hDlg, BF_MM_ANISOTROPIC ))
               dcv.mapMode = MM_ANISOTROPIC;
            else if (GetCheckField( hDlg, BF_MM_LOMETRIC ))
               dcv.mapMode = MM_LOMETRIC;
            else if (GetCheckField( hDlg, BF_MM_HIMETRIC ))
               dcv.mapMode = MM_HIMETRIC;
            else if (GetCheckField( hDlg, BF_MM_LOENGLISH ))
               dcv.mapMode = MM_LOENGLISH;
            else if (GetCheckField( hDlg, BF_MM_HIENGLISH ))
               dcv.mapMode = MM_HIENGLISH;
            else
               dcv.mapMode = MM_TEXT;
            dcv.viewportOrg.x = GetIntField( hDlg, IF_VO_X );
            dcv.viewportOrg.y = GetIntField( hDlg, IF_VO_Y );
            dcv.viewportExt.x = GetIntField( hDlg, IF_VE_X );
            dcv.viewportExt.y = GetIntField( hDlg, IF_VE_Y );
            dcv.windowOrg.x = GetIntField( hDlg, IF_WO_X );
            dcv.windowOrg.y = GetIntField( hDlg, IF_WO_Y );
            dcv.windowExt.x = GetIntField( hDlg, IF_WE_X );
            dcv.windowExt.y = GetIntField( hDlg, IF_WE_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC ClipDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetCheckField( hDlg, BF_INTERSECT_CLIP, dcv.intersect );
         SetIntField( hDlg, IF_INTERSECT_LEFT, dcv.rIntersect.left );
         SetIntField( hDlg, IF_INTERSECT_TOP, dcv.rIntersect.top );
         SetIntField( hDlg, IF_INTERSECT_RIGHT, dcv.rIntersect.right );
         SetIntField( hDlg, IF_INTERSECT_BOTTOM, dcv.rIntersect.bottom );
         SetCheckField( hDlg, BF_EXCLUDE_CLIP, dcv.exclude );
         SetIntField( hDlg, IF_EXCLUDE_LEFT, dcv.rExclude.left );
         SetIntField( hDlg, IF_EXCLUDE_TOP, dcv.rExclude.top );
         SetIntField( hDlg, IF_EXCLUDE_RIGHT, dcv.rExclude.right );
         SetIntField( hDlg, IF_EXCLUDE_BOTTOM, dcv.rExclude.bottom );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.intersect = GetCheckField( hDlg, BF_INTERSECT_CLIP );
            dcv.rIntersect.left = GetIntField( hDlg, IF_INTERSECT_LEFT );
            dcv.rIntersect.top = GetIntField( hDlg, IF_INTERSECT_TOP );
            dcv.rIntersect.right = GetIntField( hDlg, IF_INTERSECT_RIGHT );
            dcv.rIntersect.bottom = GetIntField( hDlg, IF_INTERSECT_BOTTOM );
            dcv.exclude = GetCheckField( hDlg, BF_EXCLUDE_CLIP );
            dcv.rExclude.left = GetIntField( hDlg, IF_EXCLUDE_LEFT );
            dcv.rExclude.top = GetIntField( hDlg, IF_EXCLUDE_TOP );
            dcv.rExclude.right = GetIntField( hDlg, IF_EXCLUDE_RIGHT );
            dcv.rExclude.bottom = GetIntField( hDlg, IF_EXCLUDE_BOTTOM );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC BkColorDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_RGB_R, (int) GetRValue( dcv.bkColor ) );
         SetIntField( hDlg, IF_RGB_G, (int) GetGValue( dcv.bkColor ) );
         SetIntField( hDlg, IF_RGB_B, (int) GetBValue( dcv.bkColor ) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.bkColor = RGB( GetIntField( hDlg, IF_RGB_R ),
                               GetIntField( hDlg, IF_RGB_G ),
                               GetIntField( hDlg, IF_RGB_B ) );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC BkModeDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetCheckField( hDlg, BF_OPAQUE, (dcv.bkMode == OPAQUE) );
         SetCheckField( hDlg, BF_TRANSPARENT, (dcv.bkMode == TRANSPARENT) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.bkMode = GetCheckField( hDlg, BF_TRANSPARENT ) ? 
                  TRANSPARENT : OPAQUE;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC BrushOrgDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_X, dcv.brushOrg.x );
         SetIntField( hDlg, IF_Y, dcv.brushOrg.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.brushOrg.x = GetIntField( hDlg, IF_X );
            dcv.brushOrg.y = GetIntField( hDlg, IF_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PenPosDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_TO_X, dcv.penPos.x );
         SetIntField( hDlg, IF_TO_Y, dcv.penPos.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.penPos.x = GetIntField( hDlg, IF_TO_X );
            dcv.penPos.y = GetIntField( hDlg, IF_TO_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


#if FALSE

DLGPROC RelAbsDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetCheckField( hDlg, BF_ABSOLUTE, (dcv.relAbs == ABSOLUTE) );
         SetCheckField( hDlg, BF_RELATIVE, (dcv.relAbs == RELATIVE) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.relAbs = GetCheckField( hDlg, BF_RELATIVE ) ?
                  RELATIVE : ABSOLUTE;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}

#endif


DLGPROC FontDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetDlgItemText( hDlg, SF_FACENAME,  dcv.font.lfFaceName );
         SetIntField( hDlg, IF_HEIGHT,       dcv.font.lfHeight );
         SetIntField( hDlg, IF_WIDTH,        dcv.font.lfWidth );
         SetIntField( hDlg, IF_ESCAPEMENT,   dcv.font.lfEscapement );
         SetIntField( hDlg, IF_ORIENTATION,  dcv.font.lfOrientation );
         SetIntField( hDlg, IF_WEIGHT,       dcv.font.lfWeight );
         SetCheckField( hDlg, BF_ITALIC,     dcv.font.lfItalic );
         SetCheckField( hDlg, BF_UNDERLINE,  dcv.font.lfUnderline );
         SetCheckField( hDlg, BF_STRIKEOUT,  dcv.font.lfStrikeOut );
         SetCheckField( hDlg, BF_CS_ANSI,    (dcv.font.lfCharSet == ANSI_CHARSET) );
         SetCheckField( hDlg, BF_CS_OEM,     (dcv.font.lfCharSet == OEM_CHARSET) );
         SetCheckField( hDlg, BF_CS_SYMBOL,  (dcv.font.lfCharSet == SYMBOL_CHARSET) );
         SetCheckField( hDlg, BF_OP_DEFAULT, (dcv.font.lfOutPrecision == OUT_DEFAULT_PRECIS) );
         SetCheckField( hDlg, BF_OP_STRING,  (dcv.font.lfOutPrecision == OUT_STRING_PRECIS) );
         SetCheckField( hDlg, BF_OP_CHAR,    (dcv.font.lfOutPrecision == OUT_CHARACTER_PRECIS) );
         SetCheckField( hDlg, BF_OP_STROKE,  (dcv.font.lfOutPrecision == OUT_STROKE_PRECIS) );
         SetCheckField( hDlg, BF_CP_DEFAULT, (dcv.font.lfClipPrecision == CLIP_DEFAULT_PRECIS) );
         SetCheckField( hDlg, BF_CP_CHAR,    (dcv.font.lfClipPrecision == CLIP_CHARACTER_PRECIS) );
         SetCheckField( hDlg, BF_CP_STROKE,  (dcv.font.lfClipPrecision == CLIP_STROKE_PRECIS) );
         SetCheckField( hDlg, BF_Q_DEFAULT,  (dcv.font.lfQuality == DEFAULT_QUALITY) );
         SetCheckField( hDlg, BF_Q_DRAFT,    (dcv.font.lfQuality == DRAFT_QUALITY) );
         SetCheckField( hDlg, BF_Q_PROOF,    (dcv.font.lfQuality == PROOF_QUALITY) );
         SetCheckField( hDlg, BF_P_DEFAULT,  ((dcv.font.lfPitchAndFamily & 3) == DEFAULT_PITCH) );
         SetCheckField( hDlg, BF_P_FIXED,    ((dcv.font.lfPitchAndFamily & 3) == FIXED_PITCH) );
         SetCheckField( hDlg, BF_P_VARIABLE, ((dcv.font.lfPitchAndFamily & 3) == VARIABLE_PITCH) );
         SetCheckField( hDlg, BF_F_DONTCARE, ((dcv.font.lfPitchAndFamily & ~3) == FF_DONTCARE) );
         SetCheckField( hDlg, BF_F_ROMAN,    ((dcv.font.lfPitchAndFamily & ~3) == FF_ROMAN) );
         SetCheckField( hDlg, BF_F_SWISS,    ((dcv.font.lfPitchAndFamily & ~3) == FF_SWISS) );
         SetCheckField( hDlg, BF_F_MODERN,   ((dcv.font.lfPitchAndFamily & ~3) == FF_MODERN) );
         SetCheckField( hDlg, BF_F_SCRIPT,   ((dcv.font.lfPitchAndFamily & ~3) == FF_SCRIPT) );
         SetCheckField( hDlg, BF_F_DECORATIVE, ((dcv.font.lfPitchAndFamily & ~3) == FF_DECORATIVE) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            GetDlgItemText( hDlg, SF_FACENAME, dcv.font.lfFaceName, LF_FACESIZE );
            dcv.font.lfHeight = GetIntField( hDlg, IF_HEIGHT );
            dcv.font.lfWidth = GetIntField( hDlg, IF_WIDTH );
            dcv.font.lfEscapement = GetIntField( hDlg, IF_ESCAPEMENT );
            dcv.font.lfOrientation = GetIntField( hDlg, IF_ORIENTATION );
            dcv.font.lfWeight = GetIntField( hDlg, IF_WEIGHT );
            dcv.font.lfItalic = (BYTE) GetCheckField( hDlg, BF_ITALIC );
            dcv.font.lfUnderline = (BYTE) GetCheckField( hDlg, BF_UNDERLINE );
            dcv.font.lfStrikeOut = (BYTE) GetCheckField( hDlg, BF_STRIKEOUT );

            if (GetCheckField( hDlg, BF_CS_ANSI )) {
               dcv.font.lfCharSet = ANSI_CHARSET;
            }else if (GetCheckField( hDlg, BF_CS_OEM )) {
               dcv.font.lfCharSet = OEM_CHARSET;
            }else{
               dcv.font.lfCharSet = SYMBOL_CHARSET;
            }
            if (GetCheckField( hDlg, BF_OP_DEFAULT )) { 
               dcv.font.lfOutPrecision = OUT_DEFAULT_PRECIS;
            }else if (GetCheckField( hDlg, BF_OP_STRING )) {
               dcv.font.lfOutPrecision = OUT_STRING_PRECIS;
            }else if (GetCheckField( hDlg, BF_OP_CHAR )) {    
               dcv.font.lfOutPrecision = OUT_CHARACTER_PRECIS;
            }else{
               dcv.font.lfOutPrecision = OUT_STROKE_PRECIS;
            }
            if (GetCheckField( hDlg, BF_CP_DEFAULT )) {
               dcv.font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            }else if (GetCheckField( hDlg, BF_CP_CHAR )) {
               dcv.font.lfClipPrecision = CLIP_CHARACTER_PRECIS;
            }else{
               dcv.font.lfClipPrecision = CLIP_STROKE_PRECIS;
            }
            if (GetCheckField( hDlg, BF_Q_DEFAULT )) {  
               dcv.font.lfQuality = DEFAULT_QUALITY;
            }else if (GetCheckField( hDlg, BF_Q_DRAFT )) {
               dcv.font.lfQuality = DRAFT_QUALITY;
            }else{
               dcv.font.lfQuality = PROOF_QUALITY;
            }
            dcv.font.lfPitchAndFamily = 0;
            if (GetCheckField( hDlg, BF_P_DEFAULT ))     dcv.font.lfPitchAndFamily |= DEFAULT_PITCH; 
            if (GetCheckField( hDlg, BF_P_FIXED ))       dcv.font.lfPitchAndFamily |= FIXED_PITCH;   
            if (GetCheckField( hDlg, BF_P_VARIABLE ))    dcv.font.lfPitchAndFamily |= VARIABLE_PITCH;
            if (GetCheckField( hDlg, BF_F_DONTCARE ))    dcv.font.lfPitchAndFamily |= FF_DONTCARE;  
            if (GetCheckField( hDlg, BF_F_ROMAN ))       dcv.font.lfPitchAndFamily |= FF_ROMAN;     
            if (GetCheckField( hDlg, BF_F_SWISS ))       dcv.font.lfPitchAndFamily |= FF_SWISS;     
            if (GetCheckField( hDlg, BF_F_MODERN ))      dcv.font.lfPitchAndFamily |= FF_MODERN;    
            if (GetCheckField( hDlg, BF_F_SCRIPT ))      dcv.font.lfPitchAndFamily |= FF_SCRIPT;    
            if (GetCheckField( hDlg, BF_F_DECORATIVE ))  dcv.font.lfPitchAndFamily |= FF_DECORATIVE;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC TextAlignDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   horz, vert, update;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         horz = dcv.textAlign & (TA_LEFT | TA_CENTER | TA_RIGHT);
         vert = dcv.textAlign & (TA_TOP | TA_BASELINE | TA_BOTTOM);
         update = dcv.textAlign & (TA_UPDATECP | TA_NOUPDATECP);
         SetCheckField( hDlg, BF_LEFT, (horz == TA_LEFT) );
         SetCheckField( hDlg, BF_CENTER, (horz == TA_CENTER) );
         SetCheckField( hDlg, BF_RIGHT, (horz == TA_RIGHT) );
         SetCheckField( hDlg, BF_TOP, (vert == TA_TOP) );
         SetCheckField( hDlg, BF_BASELINE, (vert == TA_BASELINE) );
         SetCheckField( hDlg, BF_BOTTOM, (vert == TA_BOTTOM) );
         SetCheckField( hDlg, BF_UPDATECP, (update == TA_UPDATECP) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.textAlign = 0;
            if (GetCheckField( hDlg, BF_LEFT ))
               dcv.textAlign |= TA_LEFT;
            if (GetCheckField( hDlg, BF_CENTER ))
               dcv.textAlign |= TA_CENTER;
            if (GetCheckField( hDlg, BF_RIGHT ))
               dcv.textAlign |= TA_RIGHT;
            if (GetCheckField( hDlg, BF_TOP ))
               dcv.textAlign |= TA_TOP;
            if (GetCheckField( hDlg, BF_BASELINE ))
               dcv.textAlign |= TA_BASELINE;
            if (GetCheckField( hDlg, BF_BOTTOM ))
               dcv.textAlign |= TA_BOTTOM;
            if (GetCheckField( hDlg, BF_UPDATECP )) {
               dcv.textAlign |= TA_UPDATECP;
            }else{
               dcv.textAlign |= TA_NOUPDATECP;
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC TextJustDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_WIDTH, dcv.breakExtra );
         SetIntField( hDlg, IF_N, dcv.breakCount );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.breakExtra = GetIntField( hDlg, IF_WIDTH );
            dcv.breakCount = GetIntField( hDlg, IF_N );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC TextExtraDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_WIDTH, dcv.textExtra );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.textExtra = GetIntField( hDlg, IF_WIDTH );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC TextColorDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_RGB_R, (int) GetRValue( dcv.textColor ) );
         SetIntField( hDlg, IF_RGB_G, (int) GetGValue( dcv.textColor ) );
         SetIntField( hDlg, IF_RGB_B, (int) GetBValue( dcv.textColor ) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.textColor = RGB( GetIntField( hDlg, IF_RGB_R ),
                                 GetIntField( hDlg, IF_RGB_G ),
                                 GetIntField( hDlg, IF_RGB_B ) );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC ROP2DlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   i;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         for (i = BF_BLACK; i <= BF_WHITE; i++) {
            SetCheckField( hDlg, i, FALSE );
         }
         if (dcv.rop2 >= R2_BLACK && dcv.rop2 <= R2_WHITE) {
            SetCheckField( hDlg, dcv.rop2 - R2_BLACK + BF_BLACK, TRUE );
         }else{
            SetCheckField( hDlg, BF_COPYPEN, TRUE );
         }
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            for (i = BF_BLACK; i <= BF_WHITE; i++) {
               if (GetCheckField( hDlg, i ))
                  dcv.rop2 = i - BF_BLACK + R2_BLACK;
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PolyModeDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetCheckField( hDlg, BF_ALTERNATE, (dcv.polyFillMode == ALTERNATE) );
         SetCheckField( hDlg, BF_WINDING, (dcv.polyFillMode == WINDING) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.polyFillMode = GetCheckField( hDlg, BF_WINDING ) ?
               WINDING : ALTERNATE;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC BltModeDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetCheckField( hDlg, BF_BLACKONWHITE, (dcv.bltMode == BLACKONWHITE) );
         SetCheckField( hDlg, BF_WHITEONBLACK, (dcv.bltMode == WHITEONBLACK) );
         SetCheckField( hDlg, BF_COLORONCOLOR, (dcv.bltMode == COLORONCOLOR) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            if (GetCheckField( hDlg, BF_BLACKONWHITE )) {
               dcv.bltMode = BLACKONWHITE;
            }else if (GetCheckField( hDlg, BF_WHITEONBLACK )) {
               dcv.bltMode = WHITEONBLACK;
            }else{
               dcv.bltMode = COLORONCOLOR;
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC SolidBrushDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_RGB_R, (int) GetRValue( dcv.brush.lbColor ) );
         SetIntField( hDlg, IF_RGB_G, (int) GetGValue( dcv.brush.lbColor ) );
         SetIntField( hDlg, IF_RGB_B, (int) GetBValue( dcv.brush.lbColor ) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.brush.lbColor = RGB( GetIntField( hDlg, IF_RGB_R ),
                                     GetIntField( hDlg, IF_RGB_G ),
                                     GetIntField( hDlg, IF_RGB_B ) );
            dcv.brush.lbStyle = BS_SOLID;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC HatchBrushDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   hatch;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         hatch = (dcv.brush.lbStyle == BS_HATCHED) ? 
               dcv.brush.lbHatch : HS_HORIZONTAL;
         SetCheckField( hDlg, BF_VERTICAL, (hatch == HS_VERTICAL) );
         SetCheckField( hDlg, BF_HORIZONTAL, (hatch == HS_HORIZONTAL) );
         SetCheckField( hDlg, BF_CROSS, (hatch == HS_CROSS) );
         SetCheckField( hDlg, BF_FDIAGONAL, (hatch == HS_FDIAGONAL) );
         SetCheckField( hDlg, BF_BDIAGONAL, (hatch == HS_BDIAGONAL) );
         SetCheckField( hDlg, BF_DIAGCROSS, (hatch == HS_DIAGCROSS) );
         SetIntField( hDlg, IF_RGB_R, (int) GetRValue( dcv.brush.lbColor ) );
         SetIntField( hDlg, IF_RGB_G, (int) GetGValue( dcv.brush.lbColor ) );
         SetIntField( hDlg, IF_RGB_B, (int) GetBValue( dcv.brush.lbColor ) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            if (GetCheckField( hDlg, BF_VERTICAL )) {
               dcv.brush.lbHatch = HS_VERTICAL;
            }else if (GetCheckField( hDlg, BF_HORIZONTAL )) {
               dcv.brush.lbHatch = HS_HORIZONTAL;
            }else if (GetCheckField( hDlg, BF_CROSS )) {
               dcv.brush.lbHatch = HS_CROSS;
            }else if (GetCheckField( hDlg, BF_FDIAGONAL )) {
               dcv.brush.lbHatch = HS_FDIAGONAL;
            }else if (GetCheckField( hDlg, BF_BDIAGONAL )) {
               dcv.brush.lbHatch = HS_BDIAGONAL;
            }else{
               dcv.brush.lbHatch = HS_DIAGCROSS;
            }
            dcv.brush.lbColor = RGB( GetIntField( hDlg, IF_RGB_R ),
                                     GetIntField( hDlg, IF_RGB_G ),
                                     GetIntField( hDlg, IF_RGB_B ) );
            dcv.brush.lbStyle = BS_HATCHED;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PatBrushDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   unsigned    i;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         for (i = 0; i < 64; i++) {
            SetCheckField( hDlg, i + BF_PATTERN_FIRST, dcv.patChecks[i] );
         }
         return (TRUE);

      case WM_COMMAND:
         if (wParam == BF_CLEAR) {
            for (i = 0; i < 64; i++) {
               dcv.patChecks[i] = 0;
               SetCheckField( hDlg, i + BF_PATTERN_FIRST, 0 );
            }
         }else if (wParam == IDOK) {
            for (i = 0; i < 64; i++) {
               dcv.patChecks[i] = (BYTE) GetCheckField( hDlg, i + BF_PATTERN_FIRST );
            }
            dcv.brush.lbStyle = BS_PATTERN;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC DIBPatBrushDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         if (curDIB == NULL) {
            SetDlgItemText( hDlg, SF_DIB, "(none)" );
            EnableItem( hDlg, IDOK, FALSE );
         }else{
            SetDlgItemText( hDlg, SF_DIB, FileNameFromPath( curDIBName ) );
         }
         return (TRUE);

      case WM_COMMAND:
         if (wParam == BF_OPEN) {
            if (OpenNewCurDIB( hDlg )) {
               SetDlgItemText( hDlg, SF_DIB, FileNameFromPath( curDIBName ) );
               EnableItem( hDlg, IDOK, TRUE );
            }
         }else if (wParam == IDOK) {
            dcv.brush.lbStyle = BS_DIBPATTERN;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PenWidthDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_WIDTH, dcv.pen.lopnWidth.x );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.pen.lopnWidth.x = dcv.pen.lopnWidth.y 
                  = GetIntField( hDlg, IF_WIDTH );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PenColorDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_RGB_R, (int) GetRValue( dcv.pen.lopnColor ) );
         SetIntField( hDlg, IF_RGB_G, (int) GetGValue( dcv.pen.lopnColor ) );
         SetIntField( hDlg, IF_RGB_B, (int) GetBValue( dcv.pen.lopnColor ) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dcv.pen.lopnColor = RGB( GetIntField( hDlg, IF_RGB_R ),
                                     GetIntField( hDlg, IF_RGB_G ),
                                     GetIntField( hDlg, IF_RGB_B ) );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


/****************************************************************************
   Dialogs progs for drawing commands
****************************************************************************/

DLGPROC SetPixelDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_X, dv.pt.x );
         SetIntField( hDlg, IF_Y, dv.pt.y );
         SetIntField( hDlg, IF_RGB_R, (int) GetRValue( dv.rgb ) );
         SetIntField( hDlg, IF_RGB_G, (int) GetGValue( dv.rgb ) );
         SetIntField( hDlg, IF_RGB_B, (int) GetBValue( dv.rgb ) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.pt.x = GetIntField( hDlg, IF_X );
            dv.pt.y = GetIntField( hDlg, IF_Y );
            dv.rgb = RGB( GetIntField( hDlg, IF_RGB_R ),
                          GetIntField( hDlg, IF_RGB_G ),
                          GetIntField( hDlg, IF_RGB_B ) );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC LineToDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_TO_X, dv.pt.x );
         SetIntField( hDlg, IF_TO_Y, dv.pt.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.pt.x = GetIntField( hDlg, IF_TO_X );
            dv.pt.y = GetIntField( hDlg, IF_TO_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC LineDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_FROM_X, dv.ptFrom.x );
         SetIntField( hDlg, IF_FROM_Y, dv.ptFrom.y );
         SetIntField( hDlg, IF_TO_X, dv.pt.x );
         SetIntField( hDlg, IF_TO_Y, dv.pt.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.ptFrom.x = GetIntField( hDlg, IF_FROM_X );
            dv.ptFrom.y = GetIntField( hDlg, IF_FROM_Y );
            dv.pt.x = GetIntField( hDlg, IF_TO_X );
            dv.pt.y = GetIntField( hDlg, IF_TO_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC RectangleDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_LEFT, dv.r.left );
         SetIntField( hDlg, IF_TOP, dv.r.top );
         SetIntField( hDlg, IF_RIGHT, dv.r.right );
         SetIntField( hDlg, IF_BOTTOM, dv.r.bottom );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.r.left = GetIntField( hDlg, IF_LEFT );
            dv.r.top = GetIntField( hDlg, IF_TOP );
            dv.r.right = GetIntField( hDlg, IF_RIGHT );
            dv.r.bottom = GetIntField( hDlg, IF_BOTTOM );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC EllipseDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   return RectangleDlgProc( hDlg, msg, wParam, lParam );
}


DLGPROC RoundRectDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_LEFT, dv.r.left );
         SetIntField( hDlg, IF_TOP, dv.r.top );
         SetIntField( hDlg, IF_RIGHT, dv.r.right );
         SetIntField( hDlg, IF_BOTTOM, dv.r.bottom );
         SetIntField( hDlg, IF_DIAM_X, dv.corner.x );
         SetIntField( hDlg, IF_DIAM_Y, dv.corner.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.r.left = GetIntField( hDlg, IF_LEFT );
            dv.r.top = GetIntField( hDlg, IF_TOP );
            dv.r.right = GetIntField( hDlg, IF_RIGHT );
            dv.r.bottom = GetIntField( hDlg, IF_BOTTOM );
            dv.corner.x = GetIntField( hDlg, IF_DIAM_X );
            dv.corner.y = GetIntField( hDlg, IF_DIAM_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC ArcDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_LEFT, dv.r.left );
         SetIntField( hDlg, IF_TOP, dv.r.top );
         SetIntField( hDlg, IF_RIGHT, dv.r.right );
         SetIntField( hDlg, IF_BOTTOM, dv.r.bottom );
         SetIntField( hDlg, IF_START_X, dv.startPt.x );
         SetIntField( hDlg, IF_START_Y, dv.startPt.y );
         SetIntField( hDlg, IF_END_X, dv.endPt.x );
         SetIntField( hDlg, IF_END_Y, dv.endPt.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.r.left = GetIntField( hDlg, IF_LEFT );
            dv.r.top = GetIntField( hDlg, IF_TOP );
            dv.r.right = GetIntField( hDlg, IF_RIGHT );
            dv.r.bottom = GetIntField( hDlg, IF_BOTTOM );
            dv.startPt.x = GetIntField( hDlg, IF_START_X );
            dv.startPt.y = GetIntField( hDlg, IF_START_Y );
            dv.endPt.x = GetIntField( hDlg, IF_END_X );
            dv.endPt.y = GetIntField( hDlg, IF_END_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PieDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   return ArcDlgProc( hDlg, msg, wParam, lParam );
}


DLGPROC ChordDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   return ArcDlgProc( hDlg, msg, wParam, lParam );
}


DLGPROC PolylineDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   i, field;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_N, dv.nPts );
         for (i = 0, field = IF_PTS_FIRST; i < MAX_PTS; i++) {
            SetIntField( hDlg, field++, dv.pts[i].x );
            SetIntField( hDlg, field++, dv.pts[i].y );
         }
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.nPts = GetIntField( hDlg, IF_N );
            for (i = 0, field = IF_PTS_FIRST; i < MAX_PTS; i++) {
               dv.pts[i].x = GetIntField( hDlg, field++ );
               dv.pts[i].y = GetIntField( hDlg, field++ );
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PolygonDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   return PolylineDlgProc( hDlg, msg, wParam, lParam );
}


DLGPROC PolyPolygonDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   i, field;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_N, dv.nPolys );
         for (i = 0, field = IF_COUNT0; i < MAX_COUNTS; i++) {
            SetIntField( hDlg, field++, dv.counts[i] );
         }
         for (i = 0, field = IF_PTS_FIRST; i < MAX_POLYPOLY_PTS; i++) {
            SetIntField( hDlg, field++, dv.pts[i].x );
            SetIntField( hDlg, field++, dv.pts[i].y );
         }
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.nPolys = GetIntField( hDlg, IF_N );
            for (i = 0, field = IF_COUNT0; i < MAX_COUNTS; i++) {
               dv.counts[i] = GetIntField( hDlg, field++ );
            }
            for (i = 0, field = IF_PTS_FIRST; i < MAX_POLYPOLY_PTS; i++) {
               dv.pts[i].x = GetIntField( hDlg, field++ );
               dv.pts[i].y = GetIntField( hDlg, field++ );
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC PatBltDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   i;
   int   other;

   static struct {
      int   field;
      DWORD rop;
   } rops[] = {
      { BF_PATCOPY,     PATCOPY },
      { BF_PATINVERT,   PATINVERT },
      { BF_DSTINVERT,   DSTINVERT },
      { BF_BLACKNESS,   BLACKNESS },
      { BF_WHITENESS,   WHITENESS },
      { -1,             0L }
   };

   #define ROP_STRING_LEN  12
   char  ropHexString[ROP_STRING_LEN];   /* hex representation of 'rop' */
   
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_X, dv.r.left );
         SetIntField( hDlg, IF_Y, dv.r.top );
         SetIntField( hDlg, IF_WIDTH, Width( dv.r ) );
         SetIntField( hDlg, IF_HEIGHT, Height( dv.r ) );
         if (dv.rop == 0L ) {
            dv.rop = PATCOPY;
         }
         for (i = 0, other = TRUE; rops[i].field != -1; i++) {
            if (dv.rop == rops[i].rop) {
               SetCheckField( hDlg, rops[i].field, TRUE );
               other = FALSE;
            }else{
               SetCheckField( hDlg, rops[i].field, FALSE );
            }
         }
         SetCheckField( hDlg, BF_OTHER, other );
         ltoa( dv.rop, ropHexString, 16);
         SetDlgItemText( hDlg, SF_ROP, ropHexString );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.r.left = GetIntField( hDlg, IF_X );
            dv.r.top = GetIntField( hDlg, IF_Y );
            dv.r.right = dv.r.left + GetIntField( hDlg, IF_WIDTH );
            dv.r.bottom = dv.r.top + GetIntField( hDlg, IF_HEIGHT );
            for (i = 0, other = TRUE; rops[i].field != -1; i++) {
               if (GetCheckField( hDlg, rops[i].field )) {
                  dv.rop = rops[i].rop;
                  other = FALSE;
               }
            }
            if (other) {
               GetDlgItemText( hDlg, SF_ROP, ropHexString, ROP_STRING_LEN );
               dv.rop = HexToLong( ropHexString );
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC BitBltDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   /* BitBlt is a strict subset of StretchBlt */
   return StretchBltDlgProc( hDlg, msg, wParam, lParam );
}


DLGPROC StretchBltDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   i;
   int   other;

   static struct {
      int   field;
      DWORD rop;
   } rops[] = {
      { BF_SRCPAINT,    SRCPAINT },
      { BF_SRCCOPY,     SRCCOPY },
      { BF_SRCAND,      SRCAND },
      { BF_SRCINVERT,   SRCINVERT },
      { BF_SRCERASE,    SRCERASE },
      { BF_NOTSRCCOPY,  NOTSRCCOPY },
      { BF_NOTSRCERASE, NOTSRCERASE },
      { BF_MERGECOPY,   MERGECOPY },
      { BF_MERGEPAINT,  MERGEPAINT },
      { BF_PATCOPY,     PATCOPY },
      { BF_PATPAINT,    PATPAINT },
      { BF_PATINVERT,   PATINVERT },
      { BF_DSTINVERT,   DSTINVERT },
      { BF_BLACKNESS,   BLACKNESS },
      { BF_WHITENESS,   WHITENESS },
      { -1,             0L }
   };

   #define ROP_STRING_LEN  12
   char  ropHexString[ROP_STRING_LEN];   /* hex representation of 'rop' */
   
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_X, dv.r.left );
         SetIntField( hDlg, IF_Y, dv.r.top );
         SetIntField( hDlg, IF_WIDTH, Width( dv.r ) );
         SetIntField( hDlg, IF_HEIGHT, Height( dv.r ) );
         SetIntField( hDlg, IF_SRC_X, dv.rFrom.left );
         SetIntField( hDlg, IF_SRC_Y, dv.rFrom.top );
         SetIntField( hDlg, IF_SRC_WIDTH, Width( dv.rFrom ) );
         SetIntField( hDlg, IF_SRC_HEIGHT, Height( dv.rFrom ) );
         if (dv.rop == 0L ) {
            dv.rop = SRCCOPY;
         }
         for (i = 0, other = TRUE; rops[i].field != -1; i++) {
            if (dv.rop == rops[i].rop) {
               SetCheckField( hDlg, rops[i].field, TRUE );
               other = FALSE;
            }else{
               SetCheckField( hDlg, rops[i].field, FALSE );
            }
         }
         SetCheckField( hDlg, BF_OTHER, other );
         ltoa( dv.rop, ropHexString, 16);
         SetDlgItemText( hDlg, SF_ROP, ropHexString );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.r.left = GetIntField( hDlg, IF_X );
            dv.r.top = GetIntField( hDlg, IF_Y );
            dv.r.right = dv.r.left + GetIntField( hDlg, IF_WIDTH );
            dv.r.bottom = dv.r.top + GetIntField( hDlg, IF_HEIGHT );
            dv.rFrom.left = GetIntField( hDlg, IF_SRC_X );
            dv.rFrom.top = GetIntField( hDlg, IF_SRC_Y );
            dv.rFrom.right = dv.rFrom.left + GetIntField( hDlg, IF_SRC_WIDTH );
            dv.rFrom.bottom = dv.rFrom.top + GetIntField( hDlg, IF_SRC_HEIGHT );
            for (i = 0, other = TRUE; rops[i].field != -1; i++) {
               if (GetCheckField( hDlg, rops[i].field )) {
                  dv.rop = rops[i].rop;
                  other = FALSE;
               }
            }
            if (other) {
               GetDlgItemText( hDlg, SF_ROP, ropHexString, ROP_STRING_LEN );
               dv.rop = HexToLong( ropHexString );
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC DIBToDeviceDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         if (curDIB == NULL) {
            SetDlgItemText( hDlg, SF_DIB, "(none)" );
            EnableItem( hDlg, IDOK, FALSE );
            EnableItem( hDlg, BF_DIBSIZE, FALSE );
            EnableItem( hDlg, BF_ONEBAND, FALSE );
         }else{
            SetDlgItemText( hDlg, SF_DIB, FileNameFromPath( curDIBName ) );
         }
         SetIntField( hDlg, IF_X, dv.r.left );
         SetIntField( hDlg, IF_Y, dv.r.top );
         SetIntField( hDlg, IF_SRC_X, dv.rFrom.left );
         SetIntField( hDlg, IF_SRC_Y, dv.rFrom.top );
         SetIntField( hDlg, IF_SRC_WIDTH, Width( dv.rFrom ) );
         SetIntField( hDlg, IF_SRC_HEIGHT, Height( dv.rFrom ) );
         SetIntField( hDlg, IF_STARTSCAN, dv.startScan );
         SetIntField( hDlg, IF_NUMSCANS, dv.numScans );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == BF_OPEN) {
            if (OpenNewCurDIB( hDlg )) {
               SetDlgItemText( hDlg, SF_DIB, FileNameFromPath( curDIBName ) );
               EnableItem( hDlg, IDOK, TRUE );
               EnableItem( hDlg, BF_DIBSIZE, TRUE );
               EnableItem( hDlg, BF_ONEBAND, TRUE );
            }
         }else if (wParam == BF_DIBSIZE) {
            SetIntField( hDlg, IF_SRC_X, 0 );
            SetIntField( hDlg, IF_SRC_Y, 0 );
            SetIntField( hDlg, IF_SRC_WIDTH, curDIBSize.x );
            SetIntField( hDlg, IF_SRC_HEIGHT, curDIBSize.y );
         }else if (wParam == BF_ONEBAND) {
            SetIntField( hDlg, IF_STARTSCAN, 0 );
            SetIntField( hDlg, IF_NUMSCANS, curDIBSize.y );
         }else if (wParam == IDOK) {
            dv.r.left = GetIntField( hDlg, IF_X );
            dv.r.top = GetIntField( hDlg, IF_Y );
            dv.rFrom.left = GetIntField( hDlg, IF_SRC_X );
            dv.rFrom.top = GetIntField( hDlg, IF_SRC_Y );
            dv.rFrom.right = dv.rFrom.left + GetIntField( hDlg, IF_SRC_WIDTH );
            dv.rFrom.bottom = dv.rFrom.top + GetIntField( hDlg, IF_SRC_HEIGHT );
            dv.startScan = GetIntField( hDlg, IF_STARTSCAN );
            dv.numScans = GetIntField( hDlg, IF_NUMSCANS );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC StretchDIBDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   i;
   int   other;

   static struct {
      int   field;
      DWORD rop;
   } rops[] = {
      { BF_SRCPAINT,    SRCPAINT },
      { BF_SRCCOPY,     SRCCOPY },
      { BF_SRCAND,      SRCAND },
      { BF_SRCINVERT,   SRCINVERT },
      { BF_SRCERASE,    SRCERASE },
      { BF_NOTSRCCOPY,  NOTSRCCOPY },
      { BF_NOTSRCERASE, NOTSRCERASE },
      { BF_MERGECOPY,   MERGECOPY },
      { BF_MERGEPAINT,  MERGEPAINT },
      { BF_PATCOPY,     PATCOPY },
      { BF_PATPAINT,    PATPAINT },
      { BF_PATINVERT,   PATINVERT },
      { BF_DSTINVERT,   DSTINVERT },
      { BF_BLACKNESS,   BLACKNESS },
      { BF_WHITENESS,   WHITENESS },
      { -1,             0L }
   };

   #define ROP_STRING_LEN  12
   char  ropHexString[ROP_STRING_LEN];   /* hex representation of 'rop' */
   
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         if (curDIB == NULL) {
            SetDlgItemText( hDlg, SF_DIB, "(none)" );
            EnableItem( hDlg, IDOK, FALSE );
            EnableItem( hDlg, BF_DIBSIZE, FALSE );
         }else{
            SetDlgItemText( hDlg, SF_DIB, FileNameFromPath( curDIBName ) );
         }
         SetIntField( hDlg, IF_X, dv.r.left );
         SetIntField( hDlg, IF_Y, dv.r.top );
         SetIntField( hDlg, IF_WIDTH, Width( dv.r ) );
         SetIntField( hDlg, IF_HEIGHT, Height( dv.r ) );
         SetIntField( hDlg, IF_SRC_X, dv.rFrom.left );
         SetIntField( hDlg, IF_SRC_Y, dv.rFrom.top );
         SetIntField( hDlg, IF_SRC_WIDTH, Width( dv.rFrom ) );
         SetIntField( hDlg, IF_SRC_HEIGHT, Height( dv.rFrom ) );
         if (dv.rop == 0L ) {
            dv.rop = SRCCOPY;
         }
         for (i = 0, other = TRUE; rops[i].field != -1; i++) {
            if (dv.rop == rops[i].rop) {
               SetCheckField( hDlg, rops[i].field, TRUE );
               other = FALSE;
            }else{
               SetCheckField( hDlg, rops[i].field, FALSE );
            }
         }
         SetCheckField( hDlg, BF_OTHER, other );
         ltoa( dv.rop, ropHexString, 16);
         SetDlgItemText( hDlg, SF_ROP, ropHexString );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == BF_OPEN) {
            if (OpenNewCurDIB( hDlg )) {
               SetDlgItemText( hDlg, SF_DIB, FileNameFromPath( curDIBName ) );
               EnableItem( hDlg, IDOK, TRUE );
               EnableItem( hDlg, BF_DIBSIZE, TRUE );
            }
         }else if (wParam == BF_DIBSIZE) {
            SetIntField( hDlg, IF_SRC_X, 0 );
            SetIntField( hDlg, IF_SRC_Y, 0 );
            SetIntField( hDlg, IF_SRC_WIDTH, curDIBSize.x );
            SetIntField( hDlg, IF_SRC_HEIGHT, curDIBSize.y );
         }else if (wParam == IDOK) {
            dv.r.left = GetIntField( hDlg, IF_X );
            dv.r.top = GetIntField( hDlg, IF_Y );
            dv.r.right = dv.r.left + GetIntField( hDlg, IF_WIDTH );
            dv.r.bottom = dv.r.top + GetIntField( hDlg, IF_HEIGHT );
            dv.rFrom.left = GetIntField( hDlg, IF_SRC_X );
            dv.rFrom.top = GetIntField( hDlg, IF_SRC_Y );
            dv.rFrom.right = dv.rFrom.left + GetIntField( hDlg, IF_SRC_WIDTH );
            dv.rFrom.bottom = dv.rFrom.top + GetIntField( hDlg, IF_SRC_HEIGHT );
            for (i = 0, other = TRUE; rops[i].field != -1; i++) {
               if (GetCheckField( hDlg, rops[i].field )) {
                  dv.rop = rops[i].rop;
                  other = FALSE;
               }
            }
            if (other) {
               GetDlgItemText( hDlg, SF_ROP, ropHexString, ROP_STRING_LEN );
               dv.rop = HexToLong( ropHexString );
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC TextOutDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetDlgItemText( hDlg, SF_TEXT,  dv.text );
         SetIntField( hDlg, IF_X, dv.pt.x );
         SetIntField( hDlg, IF_Y, dv.pt.y );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            GetDlgItemText( hDlg, SF_TEXT, dv.text, MAX_TEXT_LEN );
            dv.pt.x = GetIntField( hDlg, IF_X );
            dv.pt.y = GetIntField( hDlg, IF_Y );
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC ExtTextOutDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   int   i;

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetDlgItemText( hDlg, SF_TEXT,  dv.text );
         SetIntField( hDlg, IF_X, dv.pt.x );
         SetIntField( hDlg, IF_Y, dv.pt.y );
         SetIntField( hDlg, IF_LEFT, dv.r.left );
         SetIntField( hDlg, IF_TOP, dv.r.top );
         SetIntField( hDlg, IF_RIGHT, dv.r.right );
         SetIntField( hDlg, IF_BOTTOM, dv.r.bottom );
         SetCheckField( hDlg, BF_CLIPPED, ((dv.options & ETO_CLIPPED) != 0) );
         SetCheckField( hDlg, BF_OPAQUE, ((dv.options & ETO_OPAQUE) != 0) );
         SetCheckField( hDlg, BF_USE_DX, dv.useDx );
         for (i = 0; i < MAX_DX; i++ ) {
            SetIntField( hDlg, i + IF_DX_FIRST, dv.dxArray[i] );
         }
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            GetDlgItemText( hDlg, SF_TEXT, dv.text, MAX_TEXT_LEN );
            dv.pt.x = GetIntField( hDlg, IF_X );
            dv.pt.y = GetIntField( hDlg, IF_Y );
            dv.r.left = GetIntField( hDlg, IF_LEFT );
            dv.r.top = GetIntField( hDlg, IF_TOP );
            dv.r.right = GetIntField( hDlg, IF_RIGHT );
            dv.r.bottom = GetIntField( hDlg, IF_BOTTOM );
            dv.options = 0;
            if (GetCheckField( hDlg, BF_CLIPPED ))
               dv.options |= ETO_CLIPPED;
            if (GetCheckField( hDlg, BF_OPAQUE ))
               dv.options |= ETO_OPAQUE;
            dv.useDx = GetCheckField( hDlg, BF_USE_DX );
            for (i = 0; i < MAX_DX; i++ ) {
               dv.dxArray[i] = GetIntField( hDlg, i + IF_DX_FIRST );
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}

DLGPROC DrawTextDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   #define  horzMask    (DT_LEFT | DT_CENTER | DT_RIGHT)
   #define  vertMask    (DT_TOP | DT_VCENTER | DT_BOTTOM)

   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetDlgItemText( hDlg, SF_TEXT,  dv.text );
         SetIntField( hDlg, IF_LEFT, dv.r.left );
         SetIntField( hDlg, IF_TOP, dv.r.top );
         SetIntField( hDlg, IF_RIGHT, dv.r.right );
         SetIntField( hDlg, IF_BOTTOM, dv.r.bottom );
         SetCheckField( hDlg, BF_LEFT, ((dv.format & horzMask) == DT_LEFT) );
         SetCheckField( hDlg, BF_CENTER, ((dv.format & horzMask) == DT_CENTER) );
         SetCheckField( hDlg, BF_RIGHT, ((dv.format & horzMask) == DT_RIGHT) );
         SetCheckField( hDlg, BF_TOP, ((dv.format & vertMask) == DT_TOP) );
         SetCheckField( hDlg, BF_VCENTER, ((dv.format & vertMask) == DT_VCENTER) );
         SetCheckField( hDlg, BF_BOTTOM, ((dv.format & vertMask) == DT_BOTTOM) );
         SetCheckField( hDlg, BF_SINGLELINE, dv.format & DT_SINGLELINE );
         SetCheckField( hDlg, BF_WORDBREAK, dv.format & DT_WORDBREAK );
         SetCheckField( hDlg, BF_EXPANDTABS, dv.format & DT_EXPANDTABS );
         SetCheckField( hDlg, BF_TABSTOP, dv.format & DT_TABSTOP );
         if (dv.format & DT_TABSTOP) {
            SetCheckField( hDlg, BF_NOCLIP, FALSE );
            SetCheckField( hDlg, BF_NOPREFIX, FALSE );
            SetCheckField( hDlg, BF_EXTERNAL, FALSE );
            SetCheckField( hDlg, BF_INTERNAL, FALSE );
            SetCheckField( hDlg, BF_CALCRECT, FALSE );
         }else{
            SetCheckField( hDlg, BF_NOCLIP, dv.format & DT_NOCLIP );
            SetCheckField( hDlg, BF_NOPREFIX, dv.format & DT_NOPREFIX );
            SetCheckField( hDlg, BF_EXTERNAL, dv.format & DT_EXTERNALLEADING );
            SetCheckField( hDlg, BF_INTERNAL, dv.format & DT_INTERNAL );
            SetCheckField( hDlg, BF_CALCRECT, dv.format & DT_CALCRECT );
         }
         if (dv.tabStop <= 0)
            dv.tabStop = 8;
         SetIntField( hDlg, IF_TABSTOP, dv.tabStop );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            GetDlgItemText( hDlg, SF_TEXT, dv.text, MAX_TEXT_LEN );
            dv.r.left = GetIntField( hDlg, IF_LEFT );
            dv.r.top = GetIntField( hDlg, IF_TOP );
            dv.r.right = GetIntField( hDlg, IF_RIGHT );
            dv.r.bottom = GetIntField( hDlg, IF_BOTTOM );
            dv.format = 0;
            if (GetCheckField( hDlg, BF_CENTER ))
               dv.format |= DT_CENTER;
            else if (GetCheckField( hDlg, BF_RIGHT ))
               dv.format |= DT_RIGHT;
            else
               dv.format |= DT_LEFT;
            if (GetCheckField( hDlg, BF_VCENTER ))
               dv.format |= DT_VCENTER;
            else if (GetCheckField( hDlg, BF_BOTTOM ))
               dv.format |= DT_BOTTOM;
            else
               dv.format |= DT_TOP;
            if (GetCheckField( hDlg, BF_SINGLELINE ))
               dv.format |= DT_SINGLELINE;
            if (GetCheckField( hDlg, BF_WORDBREAK ))
               dv.format |= DT_WORDBREAK;
            if (GetCheckField( hDlg, BF_NOCLIP ))
               dv.format |= DT_NOCLIP;
            if (GetCheckField( hDlg, BF_NOPREFIX ))
               dv.format |= DT_NOPREFIX;
            if (GetCheckField( hDlg, BF_EXTERNAL ))
               dv.format |= DT_EXTERNALLEADING;
            if (GetCheckField( hDlg, BF_INTERNAL ))
               dv.format |= DT_INTERNAL;
            if (GetCheckField( hDlg, BF_CALCRECT ))
               dv.format |= DT_CALCRECT;
            if (GetCheckField( hDlg, BF_EXPANDTABS ))
               dv.format |= DT_EXPANDTABS;
            if (GetCheckField( hDlg, BF_TABSTOP )) {
               dv.format |= DT_TABSTOP;
               dv.format &= 0x00FF;
               dv.tabStop = GetIntField( hDlg, IF_TABSTOP );
               dv.format |= (dv.tabStop << 8);
            }
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}


DLGPROC FloodFillDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   return SetPixelDlgProc( hDlg, msg, wParam, lParam );
}


DLGPROC ExtFloodFillDlgProc( HWND hDlg, unsigned msg, WORD wParam, DWORD lParam )
{
   switch (msg) {
      case WM_INITDIALOG:
         CenterDialog( hDlg );
         SetIntField( hDlg, IF_X, dv.pt.x );
         SetIntField( hDlg, IF_Y, dv.pt.y );
         SetIntField( hDlg, IF_RGB_R, (int) GetRValue( dv.rgb ) );
         SetIntField( hDlg, IF_RGB_G, (int) GetGValue( dv.rgb ) );
         SetIntField( hDlg, IF_RGB_B, (int) GetBValue( dv.rgb ) );
         SetCheckField( hDlg, BF_BORDER, (dv.floodType == FLOODFILLBORDER) );
         SetCheckField( hDlg, BF_SURFACE, (dv.floodType == FLOODFILLSURFACE) );
         return (TRUE);

      case WM_COMMAND:
         if (wParam == IDOK) {
            dv.pt.x = GetIntField( hDlg, IF_X );
            dv.pt.y = GetIntField( hDlg, IF_Y );
            dv.rgb = RGB( GetIntField( hDlg, IF_RGB_R ),
                          GetIntField( hDlg, IF_RGB_G ),
                          GetIntField( hDlg, IF_RGB_B ) );
            dv.floodType = (GetCheckField( hDlg, BF_SURFACE )) ?
                           FLOODFILLSURFACE : FLOODFILLBORDER;
            EndDialog( hDlg, TRUE );
         }else if (wParam == IDCANCEL) {
            EndDialog( hDlg, FALSE );
         }
         return (TRUE);
   }
   return (FALSE);
}
