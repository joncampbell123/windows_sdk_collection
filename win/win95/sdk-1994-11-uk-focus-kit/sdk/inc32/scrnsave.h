//----------------------------------------------------------------------------
//
//  SCRNSAVE.H  Windows screen-saver defines and definitions, Version 4.0
//
//  Copyright (c)1991-1995, Microsoft Corp. All rights reserved.
//
//----------------------------------------------------------------------------

// WINDOWS.H must be included first

#ifndef _INC_SCRNSAVE
#define _INC_SCRNSAVE

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


//----------------------------------------------------------------------------
//------------------ stuff provided by SCRNSAVE.LIB --------------------------
//----------------------------------------------------------------------------

extern HINSTANCE _cdecl hMainInstance;
// This contains the instance handle for the running screen saver.

extern HWND _cdecl hMainWindow;
// This contains the window handle of the screen saver window (if present).

extern BOOL _cdecl fChildPreview;
// Check this flag if you need to know whether you're running as a child
// window on another process' display or as a stand alone screen blanker.
// It contains TRUE if you're previewing as a child window.

LRESULT WINAPI DefScreenSaverProc( HWND, UINT, WPARAM, LPARAM );
// Call this from your ScreenSaverProc instead of DefWindowProc.
// It provides common behavior for all screen savers.

// *** OPTIONAL ***
#define SCRM_VERIFYPW   WM_APP
// This message is sent to the main screen saver window when password
// protection is enabled and the user is trying to close the screen saver.  You
// can process this message and provide your own validation technology.  If you
// process this message, you should also support the ScreenSaverChangePassword
// function, described below.  Return zero from this message if the password
// check failed.  Return nonzero for success.  If you run out of memory or
// encounter a similar class of error, return non-zero so the user isn't left
// out in the cold.  The default action is to call the Windows Master
// Password Router to validate the user's password.


//----------------------------------------------------------------------------
//----------------------- stuff provided by YOU ------------------------------
//----------------------------------------------------------------------------

// You must supply a window proceedure for your screen saver.  In your code,
// define a ScreenSaverProc to match the declaration below.  This function
// should rely on DefScreenSaverProc for most of it's processing.
LRESULT CALLBACK ScreenSaverProc( HWND, UINT, WPARAM, LPARAM );

// You must supply an icon with a resource id of ID_APP.  This must be the
// first icon in the executable (so that ExtractIcon will fetch the right
// one by default).
#define ID_APP                   100

// If your screen saver has configurable options, you must supply a dialog
// box for setting them.  This dialog box must have a resource id of
// DLG_SCRNSAVECONFIGURE.  If you have no configurable options, leave this
// resource out, and an appropriate message will be displayed instead.
#define DLG_SCRNSAVECONFIGURE    2003

// You must supply a dialog-box proceedure for your configure.  In your code,
// define a ScreenSaverConfigureDialog to match the declaration below.  If
// you have no configurable options, you should still supply a definition for
// this function. (Just have it return FALSE...)
BOOL CALLBACK ScreenSaverConfigureDialog( HWND, UINT, WPARAM, LPARAM );

// You must supply a function which registers custom control classes for your
// dialog box.  In your code, define a RegisterDialogClasses to match the
// declaration below.  If you have no custom controls to register, you should
// still supply a definition for this function. (Just have it return TRUE...)
// Return FALSE if you are unable to register your control classes.
BOOL _cdecl RegisterDialogClasses( HINSTANCE );

// *** OPTIONAL ***
// You supply this if you provide your own authentication.  Windows will call
// it when the user wants to change the password.  An implementation of this
// function should present password change UI to the user.
// You should only supply this function if you also hook the SCRM_VERIFYPW
// message to validate passwords.
// The default action is to call the Windows Master Password Router.
void CALLBACK ScreenSaverChangePassword( HWND hParent );

//----------------------------------------------------------------------------
// Win3.1 compatbility stuff:
#define WS_GT   (WS_GROUP | WS_TABSTOP)
#define MAXFILELEN  13
#define TITLEBARNAMELEN 40
#define APPNAMEBUFFERLEN 32
#define BUFFLEN    255
#define idsIniFile              1001
#define idsScreenSaver          1002
#define idsAppName              1007
#define idsNoHelpMemory         1008
#define idsHelpFile             1009
#define idsDefKeyword           1010
extern TCHAR _cdecl szAppName[ APPNAMEBUFFERLEN ];
extern TCHAR _cdecl szName[ TITLEBARNAMELEN ];
extern TCHAR _cdecl szIniFile[ MAXFILELEN ];
extern TCHAR _cdecl szScreenSaver[ 22 ];
extern TCHAR _cdecl szHelpFile[ MAXFILELEN ];
extern TCHAR _cdecl szNoHelpMemory[ BUFFLEN ];


//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#ifndef RC_INVOKED
#pragma pack()
#endif  /* !RC_INVOKED */

#endif  /* !_INC_SCRNSAVE */
