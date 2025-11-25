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

#include <commctrl.h>  // TreeView and ImageList declarations live here.

//-------------------------------------------------------------------------
// Product identifier string defines

//  **TODO** Change these strings to the name of your application.

#define APPNAME       MixTree
#define ICONFILE      MixTree.ico
#define SZAPPNAME     "MixTree"
#define SZDESCRIPTION "MixTree Example Application"
#define SZABOUT       "About MixTree"
#define SZVERSION     "Version 4.0"


//-------------------------------------------------------------------------
// Functions for handling main window messages.  The message-dispatching
// mechanism expects all message-handling functions to have the following
// prototype:
//
//     LRESULT FunctionName(HWND, UINT, WPARAM, LPARAM);

// **TODO**  Add message-handling function prototypes here.  Be sure to
//           add the function names to the main window message table in
//           mixtree.c.

LRESULT MsgCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgSize(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgNotify(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgChangeBranch(HWND, UINT, WPARAM, LPARAM);

//-------------------------------------------------------------------------
// Functions for handling main window commands--ie. functions for
// processing WM_COMMAND messages based on the wParam value.
// The message-dispatching mechanism expects all command-handling
// functions to have the following prototype:
//
//     LRESULT FunctionName(HWND, WORD, WORD, HWND);

// **TODO**  Add message-handling function prototypes here.  Be sure to
//           add the function names to the main window command table in
//           mixtree.c.

LRESULT CmdExit(HWND, WORD, WORD, HWND);
LRESULT CmdAbout(HWND, WORD, WORD, HWND);
LRESULT CmdFill(HWND, WORD, WORD, HWND);
LRESULT CmdDisplayAll(HWND, WORD, WORD, HWND);
LRESULT CmdCollapseAny(HWND, WORD, WORD, HWND);

//-------------------------------------------------------------------------
// Global function prototypes.

// **TODO**  Add global function prototypes here.

BOOL CenterWindow(HWND, HWND);                              // defined in misc.c
BOOL InitApplication(HINSTANCE, int);                       // defined in init.c
BOOL BuildMixerTree(HWND);                                  // defined in mixline.c
void ExpandTreeView(void);                                  // defined in mixline.c
void ExpandBranch(HTREEITEM);                               // defined in mixline.c
BOOL GetItemInfo(HWND, long);                               // defined in mixline.c
void DisplayItemInfo(HWND, HDC);                            // defined in mixline.c
BOOL InitMixer(HWND);                                       // defined in mixinfo.c
MMRESULT GetLineInfo(HWND, LPMIXERLINE, DWORD);             // defined in mixinfo.c
MMRESULT GetLineControls(HWND, LPMIXERLINECONTROLS, DWORD); // defined in mixinfo.c

    // Callback functions.  These are called by Windows.

// **TODO**  Add new callback function prototypes here.

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//-------------------------------------------------------------------------
// Command ID definitions.  These definitions are used to associate menu
// items with commands.

// **TODO**  Add IDs for all menu commands here.  Be sure that the IDs you
//           add are also added to the main window command table in generic.c
//           and the menu statements in the resource file.

// File menu
#define IDM_EXIT        1000
#define IDM_FILL        1001
#define IDM_DISPLAYALL  1002
#define IDM_COLLAPSEANY 1003

// Help menu
#define IDM_ABOUT       1100

//-------------------------------------------------------------------------
// String Table ID definitions.

#define IDS_APPNAME     1
#define IDS_DESCRIPTION 2

//-------------------------------------------------------------------------
//  About dialog defines.

#define IDD_VERFIRST    100
#define IDD_VERLAST     104

//-------------------------------------------------------------------------
// Global variable declarations.

extern HINSTANCE hInst;          // The current instance handle
extern char      szAppName[];    // The name of this application
extern char      szTitle[];      // The title bar text

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
} EDWP;                 // Enumeration for Default Window Procedures

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
} MSDI, FAR *LPMSDI;    // MeSsage Dipatch Information

    // This structure maps command IDs to command handling functions.
typedef struct _CMD
{
    WORD   wCommand;
    PFNCMD pfncmd;
} CMD;                  // CoMmand Dispatch structure

    // This structure contains all of the information that a command
    // message procedure passes to DispCommand in order to define the
    // command dispatching behavior for the window.
typedef struct _CMDI
{
    int  ccmd;          // Number of command dispatch structs in rgcmd
    CMD *rgcmd;         // Table of command dispatch structures
    EDWP edwp;          // Type of default window handler needed.
} CMDI, FAR *LPCMDI;    // CoMmand Dispatch Information

    // Message and command dispatching functions.  They look up messages
    // and commands in the dispatch tables and call the appropriate handler
    // function.
LRESULT DispMessage(LPMSDI, HWND, UINT, WPARAM, LPARAM);
LRESULT DispCommand(LPCMDI, HWND, WPARAM, LPARAM);

    // Message dispatch information for the main window
extern MSDI msdiMain;
    // Command dispatch information for the main window
extern CMDI cmdiMain;

HMENU       hMenu;                  // handle of application menu
HWND        hwndTreeView;           // hwnd of TreeView control
HFONT       hfTVFont;               // handle to font used in TreeView control
HIMAGELIST  hImageList;             // handle to ImageList
long        lCurrentItem;               // currently selected item in mixer list
HTREEITEM   htiCurrentRoot;         // root item of currently selected branch
BOOL        fDisplayAll;            // for View.Display All menu command
BOOL        fCollapseAny;           // for View.Collapse Any menu command
BOOL        fExpandEnabled;         // controls expansion of TreeView items

#define PM_CHANGEBRANCH  WM_USER    // private message used when user selects
				    // a new branch of the TreeView control
				    // when only one may be expanded at once.

// TreeView control state
#define MIXITEM_NOSELECTION  -1     // No item is selected in TreeView control.
				    // This is a state of the control, not an
				    // attribute of an item in the control.

// TreeView control item attributes
#define MIXITEM_UNDEFINED    0      // TreeView item is undefined
#define MIXITEM_MIXERLINE    1      // TreeView item represents a mixer line
#define MIXITEM_MIXERCONTROL 2      // TreeView item represents a mixer control

// structure used to associate a mixer line or control with a TreeView item
typedef struct tagMIXITEMINFO {   
    DWORD dwItemType;               // MIXITEM_MIXERLINE or MIXITEM_MIXERCONTROL
    union
    {
	DWORD dwLineID;             // line ID if MIXERLINE
	DWORD dwControlID;          // control ID if MIXERCONTROL
    };       
}   MIXITEMINFO, FAR * LPMIXITEMINFO;

#define MAXMIXITEMS 100             // max number of items in TreeView control
				    // increase this for larger systems

MIXITEMINFO amixii[MAXMIXITEMS];    // array of structs for associating mixer
				    // lines and controls with TreeView items.

UINT uMixItems;                     // total number of TreeView items used

UINT      uNumMixers;               // number of mixer devices in system
HMIXER    hMixer;                   // handle of mixer device
UINT      uMixerID;                 // identifier for selected mixer
MIXERCAPS MixerCaps;                // device capabilities of selected mixer

// bitmap resource id defines
#define IDB_DESTLINE 101            // mixer destination line
#define IDB_SRCLINE  102            // mixer source line
#define IDB_CUSTOM   103            // mixer custom control
#define IDB_METER    104            // mixer meter control
#define IDB_SWITCH   105            // mixer switch control
#define IDB_NUMBER   106            // mixer number control
#define IDB_SLIDER   107            // mixer slider control
#define IDB_FADER    108            // mixer fader control
#define IDB_TIME     109            // mixer time control
#define IDB_LIST     110            // mixer list control

// image list defines
#define IL_DESTLINE  0              // mixer destination line
#define IL_SRCLINE   1              // mixer source line
#define IL_CUSTOM    2              // mixer custom control
#define IL_METER     3              // mixer meter control
#define IL_SWITCH    4              // mixer switch control
#define IL_NUMBER    5              // mixer number control
#define IL_SLIDER    6              // mixer slider control
#define IL_FADER     7              // mixer fader control
#define IL_TIME      8              // mixer time control
#define IL_LIST      9              // mixer list control

#define IL_MAXIMAGES 10             // max number of images in image list

// structure for associating a bitmap with an index in the image list
typedef struct tagIMAGELISTINFO {   
    UINT uBitmapID;                 // bitmap resource ID
    UINT uImageIndex;               // index of image in the image list
} IMAGELISTINFO, FAR * LPIMAGELISTINFO;

IMAGELISTINFO aImageInfo[IL_MAXIMAGES];  // info for entire image list

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
