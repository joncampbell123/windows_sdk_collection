/****************************************************************************
 Util.h

 The Util module handles misc. low-level utilities.

****************************************************************************/


/****************************************************************************
  Constants
****************************************************************************/

#define MAXINT    32767
#define BIGINT    8192


/****************************************************************************
   Globals
****************************************************************************/

extern HCURSOR hcurSave;   /* saved cursor before hourglass is shown */


/****************************************************************************
   Lexical macros
****************************************************************************/

#define WNDPROC   long FAR PASCAL _export 
#define DLGPROC   int FAR PASCAL _export
#define CLASSPROC long (FAR PASCAL *)()


/****************************************************************************
   Non-functional macros
****************************************************************************/

/* Macro to declare a dialog prog */
#define DlgProcDecl( name )   \
   DLGPROC name##Proc( HWND, unsigned, WORD, DWORD )

/* Macro to run a dialog box, with main window as parent, with generated 
   template name and dialog proc */
#define Dlg( name )  \
   RunDlg( #name, MakeProcInstance( (FARPROC) name##Proc, hInst ), hwndMain )

/* Macro to run a dialog box, with a given window as parent, with generated 
   template name and dialog proc */
#define NestedDlg( parent, name )  \
   RunDlg( #name, MakeProcInstance( (FARPROC) name##Proc, hInst ), parent )


/****************************************************************************
   Functional macros
****************************************************************************/

#define Width( r )   ((r).right - (r).left)
#define Height( r )  ((r).bottom - (r).top)

/* Macro to enable or disable a menu item */
#define EnableMenuCmd( hMenu, item, on )   \
   EnableMenuItem( hMenu, item, (on) ? MF_ENABLED : (MF_DISABLED | MF_GRAYED) );

/* Macro to check a menu item on or off */
#define CheckMenuCmd( hMenu, item, on )   \
   CheckMenuItem( hMenu, item, (on) ? MF_CHECKED : MF_UNCHECKED );

/* Macros to display/remove hourglass cursor for lengthy operations */
#define StartWait() hcurSave = SetCursor( LoadCursor( NULL, IDC_WAIT ) )
#define EndWait()   SetCursor( hcurSave )


/****************************************************************************
   Functions
****************************************************************************/

int ToIntegerPin( long l );
int Min( int x, int y );
int Max( int x, int y );
unsigned long HexToLong( char *s);
