// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
// PURPOSE:
//    Contains declarations for all globally scoped names in the program.
//

//-------------------------------------------------------------------------
// Product identifier string defines

//  **TODO** Change these strings to the name of your application.

    #define APPNAME       GDIMeta
    #define ICONFILE      GDIMeta.ICO
    #define SZAPPNAME     "GDIMeta"
    #define SZDESCRIPTION "GDI MetaFile Example Application"
    #define SZABOUT       "About GDI Meta"
    #define SZVERSION     "Version 4.0"


//-------------------------------------------------------------------------
// Functions for handling main window messages.  The message-dispatching
// mechanism expects all message-handling functions to have the following
// prototype:
//
//     LRESULT FunctionName(HWND, UINT, WPARAM, LPARAM);

// **TODO**  Add message-handling function prototypes here.  Be sure to
//           add the function names to the main window message table in
//           gdimeta.c.

LRESULT MsgCommand         (HWND, UINT, WPARAM, LPARAM); // gdimeta.c
LRESULT MsgDestroy         (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgCreate          (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgSize            (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgPaletteChanged  (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgQueryNewPalette (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgQueryEndSession (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgClose           (HWND, UINT, WPARAM, LPARAM);
LRESULT MsgNotify          (HWND, UINT, WPARAM, LPARAM); // toolbar.c
LRESULT MsgTimer           (HWND, UINT, WPARAM, LPARAM); // statbar.c
LRESULT MsgMenuSelect      (HWND, UINT, WPARAM, LPARAM);


//-------------------------------------------------------------------------
// Functions for handling main window commands--ie. functions for
// processing WM_COMMAND messages based on the wParam value.
// The message-dispatching mechanism expects all command-handling
// functions to have the following prototype:
//
//     LRESULT FunctionName(HWND, WORD, WORD, HWND);

// **TODO**  Add message-handling function prototypes here.  Be sure to
//           add the function names to the main window command table in
//           gdimeta.c.

LRESULT CmdExit       (HWND, WORD, WORD, HWND);     // gdimeta.c
LRESULT CmdRefresh    (HWND, WORD, WORD, HWND);

LRESULT CmdAbout      (HWND, WORD, WORD, HWND);     // about.c

LRESULT CmdInfo       (HWND, WORD, WORD, HWND);     // infodlg.c

LRESULT CmdDrawMode   (HWND, WORD, WORD, HWND);     // client.c
LRESULT CmdFill       (HWND, WORD, WORD, HWND);
LRESULT CmdCreatePen  (HWND, WORD, WORD, HWND);
LRESULT CmdCreateBrush(HWND, WORD, WORD, HWND);

LRESULT CmdFileNew    (HWND, WORD, WORD, HWND);     // file.c
LRESULT CmdFileOpen   (HWND, WORD, WORD, HWND);
LRESULT CmdFileSave   (HWND, WORD, WORD, HWND);
LRESULT CmdFileSaveAs (HWND, WORD, WORD, HWND);


//-------------------------------------------------------------------------
// Global function prototypes.

// **TODO**  Add global function prototypes here.

BOOL InitApplication(HINSTANCE, int);               // Init
BOOL CenterWindow(HWND, HWND);                      // Misc
HWND CreateClientWindow(HWND);                      // Client
VOID ClientNewDrawing(VOID);
HENHMETAFILE GetCurrentEMF(VOID);

VOID Open(LPSTR, BOOL, HWND);                       // File
VOID SaveAs(LPSTR, HWND);
BOOL QuerySaveFile(HWND);

typedef struct tagFILEMAP
{
    HANDLE  hFile;
    HANDLE  hFileMap;
    LPVOID  pFileMap;
} FILEMAP, *PFILEMAP;

BOOL MapFileReadOnly(LPCSTR, LPCSTR, PFILEMAP);
BOOL MapFileReadWrite(LPCSTR, LPCSTR, PFILEMAP, DWORD, DWORD);
BOOL CloseFileMapping(PFILEMAP);

#define MAKEBOOL(i)         ((BOOL) !!(i))
#define RECTWIDTH(lprc)     ((lprc)->right - (lprc)->left)
#define RECTHEIGHT(lprc)    ((lprc)->bottom - (lprc)->top)

#ifndef RC_INVOKED
   #include "gdimeta.inl"
#endif

    // Callback functions.  These are called by Windows.

// **TODO**  Add new callback function prototypes here.

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ClientWndProc(HWND, UINT, WPARAM, LPARAM);


//-------------------------------------------------------------------------
// Global variable declarations.

extern HINSTANCE    hInst;          // The current instance handle
extern char         szAppName[];    // The name of this application
extern char         szTitle[];      // The title bar text
extern HWND         hWndClient;     // The Client window
extern HCURSOR      hcursHourGlass; // IDC_WAIT cursor
extern HENHMETAFILE hEMF;           // Current metafile handle
extern BOOL         bModify;        // TRUE if current doc has been modified

// palette-related globals
extern HPALETTE  hPalette;       // App's logical palette
extern BOOL      bPalDevice;     // palette device flag

// **TODO**  For NON-MDI applications, uncomment line 1 below and comment
//           line 2.  For MDI applications, uncomment line 2 below, comment
//           line 1, and then define hwndMDIClient as a global variable in
//           INIT.C
#define hwndMDIClient NULL        /* (1) Stub for NON-MDI applications. */
// extern HWND hwndMDIClient;     /* (2) For MDI applications.          */


//-------------------------------------------------------------------------
// Message and command dispatch infrastructure.  The following type
// definitions and functions are used by the message and command dispatching
// mechanism and do not need to be changed.

    // Function pointer prototype for message handling functions.
typedef LRESULT (*PFNMSG)(HWND, UINT, WPARAM, LPARAM);

    // Function pointer prototype for command handling functions.
typedef LRESULT (*PFNCMD)(HWND, WORD, WORD, HWND);

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
