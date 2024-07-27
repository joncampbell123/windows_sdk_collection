#include "motion.h"

extern HANDLE hAccelTable;
extern HBRUSH hbrWhite;
extern char szAppName[];
extern char szLocalExpandFail[];
extern char szAbout[];
extern char szNRSeg1Msg[];
extern char szNRSeg2Msg[];


int PASCAL MotionInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pClass;

    hbrWhite = GetStockObject( WHITE_BRUSH );

    LoadString( hInstance, IDSAPPNAME, (LPSTR)szAppName, 10 );
    LoadString( hInstance, IDSLEXPAND, (LPSTR)szLocalExpandFail, 30 );
    LoadString( hInstance, IDSABOUT,   (LPSTR)szAbout, 10 );
    LoadString( hInstance, IDSSEG1MSG, (LPSTR)szNRSeg1Msg, 30 );
    LoadString( hInstance, IDSSEG2MSG, (LPSTR)szNRSeg2Msg, 30 );

    pClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pClass->hCursor       = LoadCursor( NULL, IDC_ARROW );
    pClass->hIcon         = LoadIcon( hInstance, (LPSTR)szAppName );
    pClass->lpszMenuName  = (LPSTR)szAppName;
    pClass->lpszClassName = (LPSTR)szAppName;
    pClass->hbrBackground = hbrWhite;
    pClass->hInstance     = hInstance;
    pClass->style         = CS_VREDRAW | CS_HREDRAW;
    pClass->lpfnWndProc   = MotionWndProc;

    if (!RegisterClass( (LPWNDCLASS)pClass ) )
        return FALSE;

    LocalFree( (HANDLE)pClass );
    hAccelTable = LoadAccelerators( hInstance, (LPSTR)szAppName );

    return TRUE;
}
