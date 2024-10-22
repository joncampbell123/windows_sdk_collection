/*
skeleton.h

This is the header file for the skelton.lib file.  This file is used as a
skeleton for creating Windows 3.0 SCR files.

Date        Modification
04 12 90    Original
04 16 90    Changed to allow control panel

/* Define the number for the icon... */
/* This should be the first icon used and must have this resource number.
This is needed as the first icon in the file will be grabbed as well as
the icon number being 100???... */
#define ID_APP      100

/* This function is called by skeleton to use as a Window Procedure.  It is
up to the programmer to handle any of the messages that wish to be 
interpretted.  Any unused messages are then passed back to DefScreenSaverProc
if desired which will take default action on any unprocessed message... */
LONG FAR PASCAL         ScreenSaverProc ( HWND, unsigned, WORD, LONG );

/* This function handles any default messages.  The following interpretations
    are:
WM_CREATE:
WM_DESTROY:
    No default actions are taken.
WM_SETCURSOR:
    By default, this will set the cursor to a null cursor, thereby removing it
    from the screen.
WM_PAINT:
    By default, this will paint the background of the screen.
WM_LBUTTONDOWN:
WM_MBUTTONDOWN:
WM_RBUTTONDOWN:
WM_KEYDOWN:
WM_MOUSEMOVE:
    By default, these will cause the program to terminate.
WM_ACTIVATEAPP:
WM_ACTIVATE:
    By default, if the wParam parameter is FALSE ( signifying that transfer is
    being taken away from the application ), then the program will terminate.

Termination is accomplished by generating a WM_CLOSE message.  This way, if
    the user sets something up in the WM_CREATE, a WM_DESTROY will be 
    generated and it can be destroyed properly.
*/
LONG                DefScreenSaverProc          ( HWND, unsigned, WORD, LONG );

/* A function is also needed for configuring the screen saver.  The function
    should be exactly like it is below and must be exported such that the
    program can use MAKEPROCINSTANCE on it and call up a dialog box. Further-
    more, the template used for the dialog must be called ScreenSaverConfigure
    to allow the main function to access it...
*/
BOOL FAR PASCAL     ScreenSaverConfigureDialog  ( HWND, unsigned, WORD, LONG );

/* To allow the programmer the ability to register child control windows, this
    function is called prior to the creation of the dialog box.  Any 
    registering that is required should be done here, or return TRUE if none
    is needed... */
BOOL                RegisterDialogClasses       ( HANDLE );

/*
There are only three other points that should be of notice:
1) The screen saver must have a string declared as 'szAppName' contaning the
    name of the screen saver, and it must be declared as a global.
2) The screen saver EXE file should be renamed to a file with a SCR
    extension so that the screen saver dialog form the control panel can
    find it when is searches for screen savers.
3) If the executables window or instance are needed, they can be obtained
    by the following definitions in the files that nedd them:

    extern HANDLE   hMainInstance;
    extern HWND     hMainWindow;
*/

#define     WS_GT   ( WS_GROUP | WS_TABSTOP )
