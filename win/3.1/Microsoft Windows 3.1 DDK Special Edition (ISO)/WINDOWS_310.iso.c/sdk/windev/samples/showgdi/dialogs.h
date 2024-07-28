/****************************************************************************
 Dialogs.h

 The Dialogs module contains all of the dialog handler procs.

****************************************************************************/


/****************************************************************************
  Functions
****************************************************************************/

/* Dialog utilities */
int RunDlg( LPSTR name, FARPROC procInst, HWND parent );
void CenterDialog( HWND hDlg );
LPSTR FileNameFromPath( LPSTR path );

/* Main program dialogs */
DlgProcDecl( AboutDlg );

/* View dialogs */
DlgProcDecl( SetScaleDlg );
DlgProcDecl( DrawingSizeDlg );

/* DC dialogs */
DlgProcDecl( CoordsDlg );
DlgProcDecl( ClipDlg );
DlgProcDecl( BkColorDlg );
DlgProcDecl( BkModeDlg );
DlgProcDecl( ROP2Dlg );
DlgProcDecl( PolyModeDlg );
DlgProcDecl( BltModeDlg );
DlgProcDecl( PenWidthDlg );
DlgProcDecl( PenColorDlg );
DlgProcDecl( PenPosDlg );
DlgProcDecl( FontDlg );
DlgProcDecl( TextAlignDlg );
DlgProcDecl( TextJustDlg );
DlgProcDecl( TextExtraDlg );
DlgProcDecl( TextColorDlg );
DlgProcDecl( SolidBrushDlg );
DlgProcDecl( HatchBrushDlg );
DlgProcDecl( PatBrushDlg );
DlgProcDecl( DIBPatBrushDlg );
DlgProcDecl( BrushOrgDlg );

/* Drawing dialogs */
DlgProcDecl( SetPixelDlg );
DlgProcDecl( LineToDlg );
DlgProcDecl( LineDlg );
DlgProcDecl( RectangleDlg );
DlgProcDecl( EllipseDlg );
DlgProcDecl( RoundRectDlg );
DlgProcDecl( ArcDlg );
DlgProcDecl( PieDlg );
DlgProcDecl( ChordDlg );
DlgProcDecl( PolylineDlg );
DlgProcDecl( PolygonDlg );
DlgProcDecl( PolyPolygonDlg );
DlgProcDecl( PatBltDlg );
DlgProcDecl( BitBltDlg );
DlgProcDecl( StretchBltDlg );
DlgProcDecl( DIBToDeviceDlg );
DlgProcDecl( StretchDIBDlg );
DlgProcDecl( TextOutDlg );
DlgProcDecl( ExtTextOutDlg );
DlgProcDecl( DrawTextDlg );
DlgProcDecl( FloodFillDlg );
DlgProcDecl( ExtFloodFillDlg );

