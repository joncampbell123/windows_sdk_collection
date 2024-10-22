#include <windows.h>
#include <wincom.h>
#include <mediaman.h>

#include "reverse.h"


BOOL AppInit(HANDLE hInst,HANDLE hPrev,WORD sw,LPSTR szCmdLine)
{
    WNDCLASS cls;

    hInstApp = hInst;

    if (!hPrev) {
        /* Register bitmap display class */
        cls.hCursor        = LoadCursor(NULL, IDC_ARROW);
        cls.hIcon          = LoadIcon(hInst, szAppName);
        cls.lpszMenuName   = szAppName;
        cls.lpszClassName  = szAppName;
        cls.hbrBackground  = GetStockObject(LTGRAY_BRUSH);
        cls.hInstance      = hInst;
        cls.style          = 0;
        cls.lpfnWndProc    = ReverseWndProc;
        cls.cbWndExtra     = 2;	// window instance structure storage
        cls.cbClsExtra     = 0;

        if (!RegisterClass(&cls))
	    return FALSE;
    
    }

    hwndApp = CreateWindow (szAppName,		    // Class name
                            szAppName,              // Caption
                            WS_OVERLAPPEDWINDOW,    // Style bits
                            CW_USEDEFAULT, CW_USEDEFAULT,       // Position
                            WMAIN_DX, WMAIN_DY,			// Size
                            (HWND)NULL,             // Parent window
                            (HMENU)NULL,            // use class menu
                            (HANDLE)hInst,          // instance handle
                            (LPSTR)NULL             // no params to pass on
                           );
		   
    ShowWindow(hwndApp,sw);

    
    hwndPlay = CreateWindow( "BUTTON", "Play", 
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			PLAY_X, PLAY_Y,
			PLAY_DX, PLAY_DY,
			hwndApp, IDB_PLAY, hInstApp, NULL );
    if( !hwndPlay ) {
	return( FALSE );
    }

    hwndQuit = CreateWindow( "BUTTON", "Quit",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			QUIT_X, QUIT_Y,
			QUIT_DX, QUIT_DY,
			hwndApp, IDB_QUIT, hInstApp, NULL );

    if( !hwndQuit ) {
	return( FALSE );
    }
    EnableWindow( hwndPlay, TRUE );
    EnableWindow( hwndQuit, TRUE );

    hwndName = CreateWindow("edit","",
			WS_CHILD|WS_VISIBLE|WS_BORDER,
			NAME_X, NAME_Y,
			NAME_DX, NAME_DY,
			hwndApp, IDE_NAME, hInstApp, NULL);
    if( !hwndName ) {
	return( FALSE );
    }

    medClientInit();

    medLoadHandlerDLL("medwave.mmh");
		
    return TRUE;
}
