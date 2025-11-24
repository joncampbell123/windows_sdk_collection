// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
// PURPOSE:
//    Contains declarations for all globally scoped names in the program.
//

//-------------------------------------------------------------------------
// Product identifier string defines

#define APPNAME       APP32
#define ICONFILE      APP32.ICO
#define SZAPPNAME     "APP32"
#define SZDESCRIPTION "DLL and Application Skeleton"
#define SZABOUT       "About APP32"
#define SZVERSION     "Version 4.0"


//-------------------------------------------------------------------------
// Functions for handling main window messages.  The message-dispatching
// mechanism expects all message-handling functions to have the following
// prototype:
//
//     LRESULT FunctionName(HWND, UINT, WPARAM, LPARAM);

LRESULT MsgCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDestroy(HWND, UINT, WPARAM, LPARAM);


//-------------------------------------------------------------------------
// Functions for handling main window commands--ie. functions for
// processing WM_COMMAND messages based on the wParam value.
// The message-dispatching mechanism expects all command-handling
// functions to have the following prototype:
//
//     LRESULT FunctionName(HWND, WORD, WORD, HWND);

LRESULT CmdExit(HWND, WORD, WORD, HWND);
LRESULT CmdThunkDlg(HWND, WORD, WORD, HWND);
LRESULT CmdAbout(HWND, WORD, WORD, HWND);


//-------------------------------------------------------------------------
// Global function prototypes.

BOOL InitApplication(HINSTANCE, int);
BOOL CenterWindow(HWND, HWND);

    // Callback functions.  These are called by Windows.
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//-------------------------------------------------------------------------
// Command ID definitions.  These definitions are used to associate menu
// items with commands.

// File menu
#define IDM_THUNK    1000
#define IDM_EXIT     1002

// Help menu
#define IDM_ABOUT   1100

//-------------------------------------------------------------------------
// String Table ID definitions.

#define IDS_APPNAME     1
#define IDS_DESCRIPTION 2

//-------------------------------------------------------------------------
//  About dialog defines.

#define IDD_VERFIRST    100
#define IDD_VERLAST     104

#define IDD_HWND        200
#define IDD_CHAR        201
#define IDD_INT         202
#define IDD_SHORT       203
#define IDD_LONG        204
#define IDD_ULONG       205
#define IDD_LPSTR       206
#define IDD_LPVOID      207

#define IDD_HWND32      210
#define IDD_CHAR32      211
#define IDD_INT32       212
#define IDD_USHORT32    213
#define IDD_LONG32      214
#define IDD_ULONG32     215
#define IDD_LPSTR32     216
#define IDD_LPVOID32    217

#define IDD_HWND16      220
#define IDD_CHAR16      221
#define IDD_INT16       222
#define IDD_USHORT16    223
#define IDD_LONG16      224
#define IDD_ULONG16     225
#define IDD_LPSTR16     226
#define IDD_LPVOID16    227

//-------------------------------------------------------------------------
// Global variable declarations.

extern HINSTANCE hInst;          // The current instance handle

#define hwndMDIClient NULL        /* Stub for NON-MDI applications. */


//-------------------------------------------------------------------------
// Message and command dispatch infrastructure.  The following type
// definitions and functions are used by the message and command dispatching
// mechanism and do not need to be changed.

    // Function pointer prototype for message handling functions.
typedef LRESULT (*PFNMSG)(HWND,UINT,WPARAM,LPARAM);

    // Function pointer prototype for command handling functions.
typedef LRESULT (*PFNCMD)(HWND,WORD,WORD,HWND);

    // Enumerated type used to determine which default window procedure
    // should be called by the message- and command-dispatching mechanism
    // if a message or command is not handled explicitly.
typedef enum
{
   edwpNone,            // Do not call any default procedure.
   edwpWindow,          // Call DefWindowProc.
   edwpDialog,          // Call DefDlgProc (This should be used only for
                        // custom dialogs - standard dialog use edwpNone).
   edwpMDIChild,        // Call DefMDIChildProc.
   edwpMDIFrame         // Call DefFrameProc.
} EDWP;                // Enumeration for Default Window Procedures

    // This structure maps messages to message handling functions.
typedef struct _MSD
{
    UINT   uMessage;
    PFNMSG pfnmsg;
} MSD;                 // MeSsage Dispatch structure

    // This structure contains all of the information that a window
    // procedure passes to DispMessage in order to define the message
    // dispatching behavior for the window.
typedef struct _MSDI
{
    int  cmsd;          // Number of message dispatch structs in rgmsd
    MSD *rgmsd;         // Table of message dispatch structures
    EDWP edwp;          // Type of default window handler needed.
} MSDI, FAR *LPMSDI;   // MeSsage Dipatch Information

    // This structure maps command IDs to command handling functions.
typedef struct _CMD
{
    WORD   wCommand;
    PFNCMD pfncmd;
} CMD;                 // CoMmand Dispatch structure

    // This structure contains all of the information that a command
    // message procedure passes to DispCommand in order to define the
    // command dispatching behavior for the window.
typedef struct _CMDI
{
    int  ccmd;          // Number of command dispatch structs in rgcmd
    CMD *rgcmd;         // Table of command dispatch structures
    EDWP edwp;          // Type of default window handler needed.
} CMDI, FAR *LPCMDI;   // CoMmand Dispatch Information

    // Message and command dispatching functions.  They look up messages
    // and commands in the dispatch tables and call the appropriate handler
    // function.
LRESULT DispMessage(LPMSDI, HWND, UINT, WPARAM, LPARAM);
LRESULT DispCommand(LPCMDI, HWND, WPARAM, LPARAM);

    // Message dispatch information for the main window
extern MSDI msdiMain;
    // Command dispatch information for the main window
extern CMDI cmdiMain;



//-------------------------------------------------------------------------
// Version string definitions--Leave these alone.

#define SZRCOMPANYNAME "CompanyName"
#define SZRDESCRIPTION "FileDescription"
#define SZRVERSION     "FileVersion"
#define SZRAPPNAME     "InternalName"
#define SZRCOPYRIGHT   "LegalCopyright"
#define SZRTRADEMARK   "LegalTrademarks"
#define SZRPRODNAME    "ProductName"
#define SZRPRODVER     "ProuctVersion"



