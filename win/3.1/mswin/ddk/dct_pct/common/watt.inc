'----------------------------------------------------------------------------
'
'  WATT.inc -- API Declarations for WATT Tools
'
'  To include the various portions of this file, any combination of the
'  following $DEFINE's must be used in your script prior to $INCLUDEing
'  Watt.INC:
'
'     WATT:  includes all of WATT.INC
'
'         WATTCTRL: includes all of WATTCTRL
'             W_WINDOW
'             W_MENU
'             W_ERROR
'             W_CONTROL: includes all of the control declares & routines
'                 W_BUTTON
'                 W_CHECK
'                 W_OPTION
'                 W_EDIT
'                 W_LIST
'                 W_COMBO
'
'         WATTSCR:  includes all of WATTSCR, Declares & Error codes
'             WATTSCR_DECL: includes all SCR Declares
'             WATTSCR_ERRS: includes all SCR Error codes
'
'         WATTDLGS: includes all of WATTDLGS, Declares & Error codes
'             WATTDLGS_DECL: includes all DLGS Declares
'             WATTDLGS_ERRS: includes all DLGS Error codes
'
'         WATTEVNT: includes all of WATTEVNT, Declares, Mouse event & Errors
'             WATTEVNT_DECL:  includes all EVNT Declares
'             WATTEVNT_MOUSE: includes all EVNT Mouse event codes
'             WATTEVNT_ERRS:  includes all EVNT Error codes
'
'
'  NOTE: Including WATT.INC without out using any of the above $DEFINE's will
'        include only WATTDRVR.EXE trappable and untrappable error codes,
'        and declares for CRLF, TRUE (-1), FALSE(0), Key values for
'        PlayKeys()), and a few routines from WATTCtrl.  Along with "Key"
'        and "Wait", two layered routines for PlayKeys() and SleepDelay().
'
'----------------------------------------------------------------------------
'
'              Copyright (C) 1991 Microsoft Corporation
'
'  You have a royalty-free right to use, modify, reproduce and distribute
'  this file (and/or any modified version) in any way you find useful,
'  provided that you agree that Microsoft has no warranty, obligation or
'  liability for its contents.  Refer to the Microsoft Windows Programmer's
'  Reference for further information.
'
'  This file is not garanteed by Microsoft to be error free.  Every effort
'  has been made to ensure proper data-types and declarations etc., but no
'  testing has been performed using this include file.  Additionally, some
'  API's, though listed, may not be compatible with the WATTDrvr language.
'
'----------------------------------------------------------------------------
'
' Copyright Microsoft Corporation 1990-1991
'
'----------------------------------------------------------------------------

'                                                       *********************
'$IFNDEF WATT_INCLUDED
'$DEFINE WATT_INCLUDED
'                                                       *********************

'----------------------------------------------------------------------------
' Generic Global Variables
'----------------------------------------------------------------------------
Global CRLF As String
CRLF = Chr$(13) + Chr$(10)
Const TRUE  = -1
Const FALSE = 0


'----------------------------------------------------------------------------
' Unrecoverable WATTDRVR error codes
'----------------------------------------------------------------------------
Const ERR_STACK_OVERFLOW         = 0
Const ERR_STACK_UNDERFLOW        = 1
Const ERR_OUT_OF_STRING_SPACE    = 2
Const ERR_CANT_LOAD_WATTVIEW_DLL = 3
Const ERR_OUT_OF_MEMORY          = 4


'----------------------------------------------------------------------------
' Recoverable WATTDRVR error codes
'----------------------------------------------------------------------------
Const ERR_GOSUB_STACK_OVERFLOW   = 5
Const ERR_RETURN_WITHOUT_GOSUB   = 6
Const ERR_BAD_FILE_NUMBER        = 7
Const ERR_FILE_IO                = 8
Const ERR_RUN_CMD_TOO_LONG       = 9
Const ERR_SHELL_CMD_TOO_LONG     = 10
Const ERR_SETFILE                = 11
Const ERR_FILE_NUMBER_IN_USE     = 12
Const ERR_CANT_OPEN_FILE         = 13
Const ERR_ILLEGAL_FUNCTION_CALL  = 14
Const ERR_INVALID_PATH           = 15
Const ERR_INVALID_DRIVE          = 16
Const ERR_NO_CURRENT_WORKING_DIR = 17
Const ERR_BAD_RUN_CMD            = 18
Const ERR_DIVISION_BY_ZERO       = 19
Const ERR_CANT_LOAD_DLL          = 20
Const ERR_PROC_NOT_FOUND_IN_DLL  = 21
Const ERR_CANNOT_RESUME          = 22
Const ERR_MEM_ALLOC              = 23
Const ERR_INVALID_POINTER        = 24
Const ERR_INVALID_ALLOC_SIZE     = 25
Const ERR_NULL_PIONTER_REF       = 26
Const ERR_SUBSCRIPT_RANGE        = 27
Const ERR_INPUT_PAST_EOF         = 28

'----------------------------------------------------------------------------
' Key contants used with WKey, PlayKeys, & PlayKeyshWnd
'----------------------------------------------------------------------------
Const K_ENTER  = "{ENTER}"
Const K_ESC    = "{ESC}"
Const K_UP     = "{UP}"
Const K_DOWN   = "{DOWN}"
Const K_LEFT   = "{LEFT}"
Const K_RIGHT  = "{RIGHT}"
Const K_END    = "{END}"
Const K_PGUP   = "{PGUP}"
Const K_PGDN   = "{PGDN}"
Const K_INSERT = "{INSERT}"
Const K_DELETE = "{DELETE}"
Const K_HOME   = "{HOME}"
Const K_TAB    = "{TAB}"
Const K_BS     = "{BS}"
Const K_F1     = "{F1}"
Const K_F2     = "{F2}"
Const K_F3     = "{F3}"
Const K_F4     = "{F4}"
Const K_F5     = "{F5}"
Const K_F6     = "{F6}"
Const K_F7     = "{F7}"
Const K_F8     = "{F8}"
Const K_F9     = "{F9}"
Const K_F10    = "{F10}"
Const K_F11    = "{F11}"
Const K_F12    = "{F12}"
Const K_F13    = "{F13}"
Const K_F14    = "{F14}"
Const K_F15    = "{F15}"
Const K_F16    = "{F16}"
Const K_SHIFT  = "+"
Const K_CTRL   = "^"
Const K_ALT    = "%"


'----------------------------------------------------------------------------
' Routines, types and Constants from WATTCTRL.DLL that are always included.
'----------------------------------------------------------------------------
Const MAX_CAPTION = 128

Type INFO
    hWnd            As Integer
    hWndParent      As Integer
    szClass         As String * MAX_CAPTION
    szCaption       As String * MAX_CAPTION
    szParentClass   As String * MAX_CAPTION
    szParentCaption As String * MAX_CAPTION
    szModuleName    As String * MAX_CAPTION
    lStyle          As Long
    fChild          As Integer
    wID             As Integer
    left	    As Integer
    top 	    As Integer
    right	    As Integer
    bottom	    As Integer
    width	    As Integer
    height	    As Integer
End Type

Declare Function WGetActWnd Lib "WATTCtrl.dll" (hWnd%)     As Integer
Declare Function WFndWnd    Lib "WATTCtrl.dll" (lpszCaption$, wFlags%) As Integer
Declare Function WFndWndC   Lib "WATTCtrl.dll" (lpszText$, lpszClass$, wFlags%) As Integer
Declare Sub      WGetInfo   Lib "WATTCtrl.dll" (hWnd%, lpInfo As INFO)

Const FW_DEFAULT = &h0000
Const FW_ACTIVE  = &h0008
Const FW_ALL     = &h0000
Const FW_CASE    = &h0004
Const FW_NOCASE  = &h0000
Const FW_PART    = &h0002
Const FW_FULL    = &h0000
Const FW_FOCUS   = &h0001
Const FW_NOFOCUS = &h0000


'----------------------------------------------------------------------------
' Routines from WATTEvnt always needed, so the Layered routines
' "Key" and "Wait" can always be available.  The Declares for these
' WATTEvnt routines are commented out below in the WATTEvnt section.
'----------------------------------------------------------------------------
Declare Function SleepDelay   Lib "WATTEvnt.DLL" (NumSeconds%) As Integer
Declare Function PlayKeys     Lib "WATTEvnt.DLL" (lpszPlayString$, cchString%, Flag%) As Integer
Declare Function PlayKeyshWnd Lib "WATTEvnt.DLL" (lpszPlayString$, cchString%, hWnd%) As Integer

'----------------------------------------------------------------------------
' Key: Layered routine for PlayKeys() of WATTEvnt
'----------------------------------------------------------------------------
Declare Sub Key(stKeys$)
Sub Key(stKeys$)
    r = PlayKeys(stKeys, Len(stKeys), FALSE)
End Sub

'----------------------------------------------------------------------------
' KeyHwnd: Layered routine for PlayKeys() of WATTEvnt
'----------------------------------------------------------------------------
Declare Sub KeyHwnd(hWnd%, stKeys$)
Sub KeyHwnd(hWnd%, stKeys$)
    r = PlayKeysHwnd(stKeys, Len(stKeys), hWnd)
End Sub

'----------------------------------------------------------------------------
' Wait: Layered routine for SleepDelay() of WATTEvnt
'----------------------------------------------------------------------------
Declare Sub Wait(sSeconds%)
Sub Wait(sSeconds%)
    r = SleepDelay(sSeconds)
End Sub

'                                                       *********************
'$ENDIF
'                                                       *********************

'----------------------------------------------------------------------------
'  DEFINE's that control the inclusion of the remainder of WATT.INC
'----------------------------------------------------------------------------
'$IFDEF WATT
    '$DEFINE WATTCTRL
    '$DEFINE WATTSCR
    '$DEFINE WATTDLGS
    '$DEFINE WATTEVNT
'$ENDIF

'$IFDEF WATTCTRL
    '$DEFINE W_WINDOW
    '$DEFINE W_MENU
    '$DEFINE W_ERROR
    '$DEFINE W_CONTROL
'$ENDIF

'$IFDEF W_CONTROL
    '$DEFINE W_BUTTON
    '$DEFINE W_CHECK
    '$DEFINE W_OPTION
    '$DEFINE W_EDIT
    '$DEFINE W_LIST
    '$DEFINE W_COMBO
'$ENDIF

'$IFDEF WATTSCR
    '$DEFINE WATTSCR_DECL
    '$DEFINE WATTSCR_ERRS
'$ENDIF

'$IFDEF WATTDLGS
    '$DEFINE WATTDLGS_DECL
    '$DEFINE WATTDLGS_ERRS
'$ENDIF

'$IFDEF WATTEVNT
    '$DEFINE WATTEVNT_DECL
    '$DEFINE WATTEVNT_ERRS
    '$DEFINE WATTEVNT_MOUSE
'$ENDIF

'$IFDEF  W_BUTTON OR W_CHECK OR W_OPTION OR W_EDIT OR W_LIST OR W_COMBO
    '$DEFINE W_A_CONTROL
'$ENDIF

'                                                       *********************
'$IFDEF WATTEVNT_DECL AND NOT WATTEVNT_DECL_INCLUDED
'                                                       *********************

'----------------------------------------------------------------------------
' WATTEvnt.DLL:
'   Type, Const, and Function declarations for use with WATTEvnt
'----------------------------------------------------------------------------
'*** Timer Structure
'
Type TimerEventStr
    delay        As Integer
    Event        As Integer
    lpszFileName As String * 13
    x1           As Integer
    y1           As Integer
    x2           As Integer
    y2           As Integer
    scrnumber    As Integer
    action       As Integer
    hide         As Integer
    EvhWnd       As Integer
    CompareType  As Integer
    sendesc      As Integer
End Type

' WATTEvnt.DLL Functions
'
' NOTE: TimerEvent: Use the PTR Function to get the address of a TimerEventStr.
'
Declare Function AppendKeys         Lib "WATTEvnt.DLL" (lpszPlayString$, cchString%, Flag%) As Integer
Declare Function GetEventError      Lib "WATTEvnt.DLL" () As Integer
Declare Function GetWattTimerData   Lib "WATTEvnt.DLL" (lpTimeStr As TimerEventStr) As Integer
Declare Function PlayKeysSpeed      Lib "WATTEvnt.DLL" (Speed&) As Integer
Declare Sub      SetGlobalEventhWnd Lib "WATTEvnt.DLL" (hWnd%)
Declare Sub      SetWaitFlag        Lib "WATTEvnt.DLL" (hWnd%)
Declare Function TimerEvent	    Lib "WATTEvnt.DLL" (lpTimeStr As TimerEventStr) As Integer
Declare Function MouseEvent	    Lib "WATTEvnt.DLL" (MouseE%, x%, y%, Flag%) As Integer
'**** The following commented functions are always included  ***
'* Declare Function PlayKeys           Lib "WATTEvnt.DLL" (lpszPlayString$, cchString%, Flag%) As Integer
'* Declare Function SleepDelay         Lib "WATTEvnt.DLL" (NumSeconds%) As Integer
'* Declare Function PlayKeyshWnd       Lib "WATTEvnt.DLL" (lpszPlayString$, cchString%, hWnd%) As Integer

'*** Timer Events
'
Const EV_NOTHING       = 0
Const EV_SCREENDUMP    = 1
Const EV_SCREENCOMP    = 2
Const EV_WINDOWDUMP    = 3
Const EV_WINDOWCOMP    = 4
Const NOEVENTSCHEDULED = 998
Const EVENTPENDING     = 999


'                                                       *********************
'$DEFINE WATTEVNT_DECL_INCLUDED
'$ENDIF
'$IFDEF WATTEVNT_MOUSE AND NOT WATTEVNT_MOUSE_INCLUDED
'                                                       *********************


'*** Mouse Event Codes
'
Const WM_UMOUSEFIRST            =&h300
Const WM_LCLICK                 =&h300
Const WM_RCLICK                 =&h301
Const WM_MCLICK                 =&h302
Const WM_LDBLCLICK              =&h303
Const WM_RDBLCLICK              =&h304
Const WM_MDBLCLICK              =&h305
Const WM_LCLICKDRAG             =&h306
Const WM_RCLICKDRAG             =&h307
Const WM_MCLICKDRAG             =&h308
Const WM_UMOUSEMOVE             =&h309
Const WM_ALT_LCLICK             =&h30A
Const WM_ALT_RCLICK             =&h30B
Const WM_ALT_MCLICK             =&h30C
Const WM_CTRL_LCLICK            =&h30D
Const WM_CTRL_RCLICK            =&h30E
Const WM_CTRL_MCLICK            =&h30F
Const WM_SHIFT_LCLICK           =&h310
Const WM_SHIFT_RCLICK           =&h311
Const WM_SHIFT_MCLICK           =&h312
Const WM_CTRL_SHIFT_LCLICK      =&h313
Const WM_CTRL_SHIFT_RCLICK      =&h314
Const WM_CTRL_SHIFT_MCLICK      =&h315
Const WM_ALT_LCLICKDRAG         =&h316
Const WM_ALT_RCLICKDRAG         =&h317
Const WM_ALT_MCLICKDRAG         =&h318
Const WM_CTRL_LCLICKDRAG        =&h319
Const WM_CTRL_RCLICKDRAG        =&h31A
Const WM_CTRL_MCLICKDRAG        =&h31B
Const WM_SHIFT_LCLICKDRAG       =&h31C
Const WM_SHIFT_RCLICKDRAG       =&h31D
Const WM_SHIFT_MCLICKDRAG       =&h31E
Const WM_SHIFT_MOVE             =&h31F
Const WM_CTRL_MOVE              =&h320
Const WM_ALT_MOVE               =&h321
Const WM_CTRL_SHIFT_MOVE        =&h322
Const WM_LRCLICK                =&h323
Const WM_LMCLICK                =&h324
Const WM_RMCLICK                =&h325
Const WM_CTRL_LRCLICK           =&h326
Const WM_CTRL_LMCLICK           =&h327
Const WM_CTRL_RMCLICK           =&h328
Const WM_SHIFT_LRCLICK          =&h329
Const WM_SHIFT_LMCLICK          =&h32A
Const WM_SHIFT_RMCLICK          =&h32B
Const WM_ALT_LRCLICK            =&h32C
Const WM_ALT_LMCLICK            =&h32D
Const WM_ALT_RMCLICK            =&h32E
Const WM_CTRL_SHIFT_LRCLICK     =&h32F
Const WM_CTRL_SHIFT_LMCLICK     =&h330
Const WM_CTRL_SHIFT_RMCLICK     =&h331
Const WM_SHIFT_LDBLCLICK        =&h332
Const WM_SHIFT_RDBLCLICK        =&h333
Const WM_SHIFT_MDBLCLICK        =&h334
Const WM_SHIFT_LRDBLCLICK       =&h335
Const WM_SHIFT_LMDBLCLICK       =&h336
Const WM_SHIFT_RMDBLCLICK       =&h337
Const WM_CTRL_SHIFT_LCLICKDRAG  =&h338
Const WM_CTRL_SHIFT_RCLICKDRAG  =&h339
Const WM_CTRL_SHIFT_MCLICKDRAG  =&h33A
Const WM_CTRL_SHIFT_LRCLICKDRAG =&h33B
Const WM_CTRL_SHIFT_LMCLICKDRAG =&h33C
Const WM_CTRL_SHIFT_RMCLICKDRAG =&h33D
Const WM_ALT_SHIFT_LCLICKDRAG   =&h33E
Const WM_ALT_SHIFT_RCLICKDRAG   =&h33F
Const WM_ALT_SHIFT_MCLICKDRAG   =&h340
Const WM_ALT_SHIFT_LRCLICKDRAG  =&h341
Const WM_ALT_SHIFT_LMCLICKDRAG  =&h342
Const WM_ALT_SHIFT_RMCLICKDRAG  =&h343
Const WM_LBUP                   =&h344
Const WM_RBUP                   =&h345
Const WM_MBUP                   =&h346
Const WM_LRBUP                  =&h347
Const WM_LMBUP                  =&h348
Const WM_RMBUP                  =&h349
Const WM_LBDOWN                 =&h34A
Const WM_RBDOWN                 =&h34B
Const WM_MBDOWN                 =&h34C
Const WM_LRBDOWN                =&h34D
Const WM_LMBDOWN                =&h34E
Const WM_RMBDOWN                =&h34F
Const WM_LRCLICKDRAG            =&h350
Const WM_LMCLICKDRAG            =&h351
Const WM_RMCLICKDRAG            =&h352
Const WM_SHIFT_LRCLICKDRAG      =&h353
Const WM_SHIFT_LMCLICKDRAG      =&h354
Const WM_SHIFT_RMCLICKDRAG      =&h355
Const WM_CTRL_LRCLICKDRAG       =&h356
Const WM_CTRL_LMCLICKDRAG       =&h357
Const WM_CTRL_RMCLICKDRAG       =&h358
Const WM_ALT_LRCLICKDRAG        =&h359
Const WM_ALT_LMCLICKDRAG        =&h35A
Const WM_ALT_RMCLICKDRAG        =&h35B
Const WM_CTRL_ALT_LCLICKDRAG    =&h35C
Const WM_CTRL_ALT_RCLICKDRAG    =&h35D
Const WM_CTRL_ALT_MCLICKDRAG    =&h35E
Const WM_CTRL_ALT_LRCLICKDRAG   =&h35F
Const WM_CTRL_ALT_LMCLICKDRAG        =&h360
Const WM_CTRL_ALT_RMCLICKDRAG        =&h361
Const WM_SHIFT_CTRL_ALT_LCLICKDRAG   =&h362
Const WM_SHIFT_CTRL_ALT_RCLICKDRAG   =&h363
Const WM_SHIFT_CTRL_ALT_MCLICKDRAG   =&h364
Const WM_SHIFT_CTRL_ALT_LRCLICKDRAG  =&h365
Const WM_SHIFT_CTRL_ALT_LMCLICKDRAG  =&h366
Const WM_SHIFT_CTRL_ALT_RMCLICKDRAG  =&h367
Const WM_CTRL_ALT_MOVE               =&h368
Const WM_SHIFT_CTRL_ALT_MOVE         =&h369
Const WM_SHIFT_ALT_LCLICK            =&h36A
Const WM_CTRL_ALT_LCLICK             =&h36B
Const WM_SHIFT_CTRL_ALT_LCLICK       =&h36C
Const WM_SHIFT_ALT_RCLICK            =&h36D
Const WM_CTRL_ALT_RCLICK             =&h36E
Const WM_SHIFT_CTRL_ALT_RCLICK       =&h36F
Const WM_SHIFT_ALT_MCLICK            =&h370
Const WM_CTRL_ALT_MCLICK             =&h371
Const WM_SHIFT_CTRL_ALT_MCLICK       =&h372
Const WM_SHIFT_ALT_LRCLICK           =&h373
Const WM_CTRL_ALT_LRCLICK            =&h374
Const WM_SHIFT_CTRL_ALT_LRCLICK      =&h375
Const WM_SHIFT_ALT_LMCLICK           =&h376
Const WM_CTRL_ALT_LMCLICK            =&h377
Const WM_SHIFT_CTRL_ALT_LMCLICK      =&h378
Const WM_SHIFT_ALT_RMCLICK           =&h379
Const WM_CTRL_ALT_RMCLICK            =&h37A
Const WM_SHIFT_CTRL_ALT_RMCLICK      =&h37B
Const WM_SHIFT_ALT_MOVE              =&h37C
Const WM_UMOUSELAST                  =&h37C


'                                                       *********************
'$DEFINE WATTEVNT_MOUSE_INCLUDED
'$ENDIF
'$IFDEF WATTEVNT_ERRS AND NOT WATTEVNT_ERRS_INCLUDED
'                                                       *********************


'*** Error Messages for WATTEvnt
'
Const ERR_EVNT_STRTOOLONG       = 1
Const ERR_EVNT_KEYPARSE         = 2
Const ERR_EVNT_OUTOFMEM         = 3
Const ERR_EVNT_NOTVALIDMOUSE    = 4
Const ERR_EVNT_INVALIDX         = 5
Const ERR_EVNT_INVALIDY         = 6
Const ERR_EVNT_ALREADYINUSE     = 7
Const ERR_EVNT_NOTIMER          = 8
Const ERR_EVNT_ACTIVEAPP        = 9
Const ERR_EVNT_KILLTIMER_APP    = 10
Const ERR_EVNT_SETSLEEP         = 11
Const ERR_EVNT_TIMERALLSET      = 12
Const ERR_EVNT_COULDNTFINDMOUSE = 13


'                                                       *********************
'$DEFINE WATTEVNT_ERRS_INCLUDED
'$ENDIF
'$IFDEF WATTSCR_DECL AND NOT WATTSCR_DECL_INCLUDED
'                                                       *********************


'----------------------------------------------------------------------------
' WATTScr.DLL:
'       Type, Const, and Function declarations for use with WATTScr
'----------------------------------------------------------------------------
Type wRect
    x1 As Integer
    y1 As Integer
    x2 As Integer
    y2 As Integer
End Type

Const SCRNAPPEND  = 0
Const SCRNREPLACE = 1
Const SCRNINSERT  = 2

'*** WATTSCR.DLL Routines
'
Declare Function fCompFiles      Lib "WATTScr.DLL" (lpszFileName1$, Scr1%, lpszFileName2$, Scr2%, CompareType%) As Integer
Declare Function fCompScreen     Lib "WATTScr.DLL" (lpszFileName$, lpRect As wRect, Scr1%, Hide%, Flag%) As Integer
Declare Function fCompWindow     Lib "WATTScr.DLL" (lpszFileName$, hWnd%, Scr1%, Hide%, Flag%) As Integer
Declare Function fDelScreen      Lib "WATTScr.DLL" (lpszFileName$, Scr%) As Integer
Declare Function fDumpFileToClip Lib "WATTScr.DLL" (lpszFileName$, Scr%) As Integer
Declare Function fDumpScreen     Lib "WATTScr.DLL" (lpszFileName$, lpRect As wRect, Action%, Scr1%, Flag%) As Integer
Declare Function fDumpSrnToClip  Lib "WATTScr.DLL" (lpRect As wRect, Hide%) As Integer
Declare Function fDumpWindow     Lib "WATTScr.DLL" (lpszFileName$, wHnd%, Action%, Scr1%, Flag%) As Integer
Declare Function fDumpWndToClip  Lib "WATTScr.DLL" (hWnd%, Hide%) As Integer
Declare Function fFileInfo       Lib "WATTScr.DLL" (lpszFileName$, Rectangle&, VideoMode%, ScreenNum%) As Integer
Declare Function fGetDLLVersion  Lib "WATTScr.DLL" (lpszFileName$) As Integer
Declare Function fGetMaxScreen   Lib "WATTScr.DLL" (lpszFileName$) As Integer
Declare Function fGetOS          Lib "WATTScr.DLL" (lpszFileName$) As Integer
Declare Function fSaveFileToDIB  Lib "WATTScr.DLL" (lpszFileName1$, Scr%, lpszFileName2$) As Integer
Declare Function fSaveSrnToDIB   Lib "WATTScr.DLL" (lpszFileName$, lpRect As wRect, Hide%) As Integer
Declare Function fSaveWndToDIB   Lib "WATTScr.DLL" (lpszFileName$, hWnd%, Hide%) As Integer
Declare Function fViewScreen     Lib "WATTScr.DLL" (lpszFileName$, hWnd%, Scr1%, Action%) As Integer


'                                                       *********************
'$DEFINE WATTSCR_DECL_INCLUDED
'$ENDIF
'$IFDEF WATTSCR_ERRS AND NOT WATTSCR_ERRS_INCLUDED
'                                                       *********************


'*** WATTScr.DLL Error Codes
'
Const ERR_SCR_NOERROR    = 0
Const ERR_SCR_FILEACCESS = 301
Const ERR_SCR_INVALIDFIL = 302
Const ERR_SCR_INVALSRNID = 303
Const ERR_SCR_INVALSRNMD = 304
Const ERR_SCR_OUTOMEMORY = 305
Const ERR_SCR_READSRNFIL = 306
Const ERR_SCR_RELMEMORY  = 307
Const ERR_SCR_CREATEDDB  = 308
Const ERR_SCR_RWSRNTABLE = 309
Const ERR_SCR_RWCOLTABLE = 310
Const ERR_SCR_WSRNIMAGE  = 311
Const ERR_SCR_WFILEHEAD  = 312
Const ERR_SCR_CREATEDIB  = 313
Const ERR_SCR_SCREENSIZE = 314
Const ERR_SCR_DISPSCREEN = 315
Const ERR_SCR_INVALIDACT = 316
Const ERR_SCR_IMAGEDIFF  = 317
Const ERR_SCR_SRNSIZEDIF = 318
Const ERR_SCR_FILEEXIST  = 319
Const ERR_SCR_CTEMPFILE  = 320
Const ERR_SCR_HIDEWIN    = 321
Const ERR_SCR_INVALWHAND = 322
Const ERR_SCR_OFILEFORM  = 323
Const ERR_SCR_SRNFILEFUL = 324
Const ERR_SCR_INVALSCALE = 325
Const ERR_SCR_OPENCB     = 326
Const ERR_SCR_EMPTYCB    = 327
Const ERR_SCR_COPYTOCB   = 328
Const ERR_SCR_CLOSECB    = 329
Const ERR_SCR_CREATEPAL  = 330


'                                                       *********************
'$DEFINE WATTSCR_ERRS_INCLUDED
'$ENDIF
'$IFDEF WATTDLGS_DECL AND NOT WATTDLGS_DECL_INCLUDED
'                                                       *********************


'----------------------------------------------------------------------------
' WATTDlgs.DLL:
'       Type, Const, and Function declarations for use with WATTDlgs
'----------------------------------------------------------------------------
Const cchCLASSMAC = 32
Const cchTEXTMAC  = 256
Const wVEREB    = 1

Type DCR
    xLeft  As Integer
    yMin   As Integer
    xRight As Integer
    yLast  As Integer
End Type

Type CtlDef
    rgText  As String * cchTEXTMAC
    rgClass As String * cchCLASSMAC
    nState  As Integer
    fFlags  As String * 1
    dcrect  As DCR
End Type

Declare Function AwaitSaveCompletion Lib "WATTDlgs.DLL" () As Integer
Declare Function ComparisonResults   Lib "WATTDlgs.DLL" () As Integer
Declare Function CmpWindow           Lib "WATTDlgs.DLL" (hWnd%, nDialog%, fIncludeParent%) As Integer
Declare Function CmpWindowActivate   Lib "WATTDlgs.DLL" (lpszOpenKeys$, lpszCloseKey$, nDialog%, fIncludeParent%) As Integer
Declare Function CmpWindowCaption    Lib "WATTDlgs.DLL" (lpszCap$, nDialog%, fIncludeParent%) As Integer
Declare Function CmpWindowDelayed    Lib "WATTDlgs.DLL" (nDelay%, nDialog%, fIncludeParent%, lpszCloseKeys$) As Integer
Declare Function DynAdd              Lib "WATTDlgs.DLL" (lpCtl As CtlDef) As Integer
Declare Function DynCreate           Lib "WATTDlgs.DLL" () As Integer
Declare Function DynDelete           Lib "WATTDlgs.DLL" (nCtlNum%) As Integer
Declare Sub      DynDestroy          Lib "WATTDlgs.DLL" ()
Declare Function DynFullDialog       Lib "WATTDlgs.DLL" (fFull%) As Integer
Declare Function DynReplace          Lib "WATTDlgs.DLL" (lpCtl As CtlDef, nCtlNum%) As Integer
Declare Function FindWindowCaption   Lib "WATTDlgs.DLL" (lpszCap$, hWndStart%) As Integer
Declare Function MaxDialogs          Lib "WATTDlgs.DLL" () As Integer
Declare Function SaveMenu            Lib "WATTDlgs.DLL" (hWnd%, nDialog%, lpszDesc$, fReplace%) As Integer
Declare Function SaveMenuActivate    Lib "WATTDlgs.DLL" (lpszOpenKeys$, lpszCloseKeys$, nDialog%, lpszDesc$, fReplace%) As Integer
Declare Function SaveMenuCaption     Lib "WATTDlgs.DLL" (lpszCap$, nDialog%, lpszDesc$, fReplace%) As Integer
Declare Function SaveMenuDelayed     Lib "WATTDlgs.DLL" (nDelay%, nDialog%, lpszDesc$, fReplace%, CloseKeys$) As Integer
Declare Function SaveWindow          Lib "WATTDlgs.DLL" (hWnd%, nDialog%, lpszDesc$, fReplace%, fIncludeParent%) As Integer
Declare Function SaveWindowActivate  Lib "WATTDlgs.DLL" (lpszOpenKeys$, lpszCloseKeys$, nDialog%, lpszDesc$, fReplace%, fIncludeParent%) As Integer
Declare Function SaveWindowCaption   Lib "WATTDlgs.DLL" (lpszCap%, nDialog%, lpszDesc$, fReplace%, fIncludeParent%) As Integer
Declare Function SaveWindowDelayed   Lib "WATTDlgs.DLL" (nDelay%, nDialog%, lpszDesc$, fReplace%, fIncludeParent%, lpszCloseKeys$) As Integer
Declare Function SetDialogFile       Lib "WATTDlgs.DLL" (lpszDialogName$) As Integer
Declare Function SetLogFile          Lib "WATTDlgs.DLL" (lpszLogName$) As Integer
Declare Function WDlgsInit           Lib "WATTDlgs.DLL" () As Integer


'                                                       *********************
'$DEFINE WATTDLGS_DECL_INCLUDED
'$ENDIF
'$IFDEF WATTDLGS_ERRS AND NOT WATTDLGS_ERRS_INCLUDED
'                                                       *********************


'*** Function return codes
'
Const ERR_DLGS_NOERR          =   0
Const ERR_DLGS_FUZZY          =  -1
Const ERR_DLGS_EXCESS         =  -2
Const ERR_DLGS_CTLNOTFOUND    =  -3
Const ERR_DLGS_NODLGFILE      = -10
Const ERR_DLGS_FILENOTFOUND   = -11
Const ERR_DLGS_BADWDLFILE     = -12
Const ERR_DLGS_LIBLOADERR     = -13
Const ERR_DLGS_SAVEERR        = -14
Const ERR_DLGS_DLGFILEERR     = -15
Const ERR_DLGS_TMPFILEERR     = -16
Const ERR_DLGS_VERSIONERR     = -17
Const ERR_DLGS_DLGFILEFULL    = -18
Const ERR_DLGS_OUTOFMEMORY    = -20
Const ERR_DLGS_BUFFERERR      = -21
Const ERR_DLGS_NOTIMER        = -22
Const ERR_DLGS_NODYNDIALOG    = -30
Const ERR_DLGS_INVALIDHWND    = -31
Const ERR_DLGS_BADCAPTION     = -32
Const ERR_DLGS_BADDLGNUM      = -33
Const ERR_DLGS_BADCTLINDEX    = -34
Const ERR_DLGS_BADCTLTYPE     = -35
Const ERR_DLGS_BADSAVEACTION  = -36
Const ERR_DLGS_APPSPECIFIC    = -37


'                                                       *********************
'$DEFINE WATTDLGS_ERRS_INCLUDED
'$ENDIF
'$IFDEF W_WINDOW AND NOT W_WINDOW_INCLUDED
'                                                       *********************

'----------------------------------------------------------------------------
' WATTCtrl.dll routines, types, & constants
'----------------------------------------------------------------------------

'*** Window size and position types
'
Type WNDPOS
    left   As Integer
    top    As Integer
End Type

Type WNDSIZ
    width  As Integer
    height As Integer
End Type

Type WNDPOSSIZ
    left   As Integer
    top    As Integer
    width  As Integer
    height As Integer
End Type

'*** Window Positioning and Sizing routines & contants
'
Declare Sub WMinWnd        Lib "WATTCtrl.dll" (hWnd%)
Declare Sub WMaxWnd        Lib "WATTCtrl.dll" (hWnd%)
Declare Sub WResWnd        Lib "WATTCtrl.dll" (hWnd%)
Declare Sub WSetWndPos	   Lib "WATTCtrl.dll" (hWnd%, left%, top%)
Declare Sub WSetWndSiz	   Lib "WATTCtrl.dll" (hWnd%, width%, height%)
Declare Sub WSetWndPosSiz  Lib "WATTCtrl.dll" (hWnd%, left%, top%, width%, height%)
Declare Sub WAdjWndPos	   Lib "WATTCtrl.dll" (hWnd%, deltaLeft%,  deltaTop%)
Declare Sub WAdjWndSiz	   Lib "WATTCtrl.dll" (hWnd%, deltaWidth%, deltaHeight%)
Declare Sub WAdjWndPosSiz  Lib "WATTCtrl.dll" (hWnd%, deltaLeft%,  deltaTop%, deltaWidth%, deltaHeight%)
Declare Sub WGetWndPos     Lib "WATTCtrl.dll" (hWnd%, lpWndPos    As WNDPOS,    fRelative%)
Declare Sub WGetWndSiz     Lib "WATTCtrl.dll" (hWnd%, lpWndSiz    As WNDSIZ)
Declare Sub WGetWndPosSiz  Lib "WATTCtrl.dll" (hWnd%, lpWndPosSiz As WNDPOSSIZ, fRelative%)

Const RELATIVE = TRUE
Const ABSOLUTE = FALSE


'                                                       *********************
'$DEFINE W_WINDOW_INCLUDED
'$ENDIF
'$IFDEF W_MENU AND NOT W_MENU_INCLUDED
'                                                       *********************


'*** Menu routines & constants
'
Declare Sub	 WMenu	       Lib "WATTCtrl.dll" (lpszName$)
Declare Function WMenuExists   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WMenuGrayed   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WMenuChecked  Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WMenuEnabled  Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WMenuCount    Lib "WATTCtrl.dll" () As Integer
Declare Sub	 WMenuText     Lib "WATTCtrl.dll" (sIndex%, lpszBuffer$)
Declare Function  MenuText			  (sIndex%) As String
Declare Function WMenuLen      Lib "WATTCtrl.dll" (sIndex%) As Integer
Declare Sub	 WMenuFullText Lib "WATTCtrl.dll" (sIndex%, lpszBuffer$)
Declare Function  MenuFullText			  (sIndex%) As String
Declare Function WMenuFullLen  Lib "WATTCtrl.dll" (sIndex%) As Integer

'----------------------------------------------------------------------------
' MenuText: Layered routine for WMenuText()
'----------------------------------------------------------------------------
Function MenuText(sIndex%) As String
Dim lpszBuffer As String
    menuLength = WMenuLen(sIndex)
    If menuLength >= 0 Then
	lpszBuffer = String$(menuLength+1, " ")
	WMenuText sIndex, lpszBuffer
	MenuText = lpszBuffer
	lpszBuffer = ""
    End if
End Function

'----------------------------------------------------------------------------
' MenuFullText: Layered routine for WMenuFullText()
'----------------------------------------------------------------------------
Function MenuFullText(sIndex%) As String
Dim lpszBuffer As String
    menuLength = WMenuFullLen(sIndex)
    If menuLength >= 0 Then
	lpszBuffer = String$(menuLength+1, " ")
	WMenuFullText sIndex, lpszBuffer
	MenuFullText = lpszBuffer
	lpszBuffer = ""
    End If
End Function


'                                                       *********************
'$DEFINE W_MENU_INCLUDED
'$ENDIF
'$IFDEF W_A_CONTROL AND NOT W_A_CONTROL_INCLUDED
'                                                       *********************


'*** Global routine & constants used by all controls
'
Declare Sub WStaticSetClass Lib "WATTCtrl.dll" (lpszClassName$)
Declare Sub WResetClasses   Lib "WATTCtrl.dll" ()


'                                                       *********************
'$DEFINE W_A_CONTROL_INCLUDED
'$ENDIF
'$IFDEF W_BUTTON AND NOT W_BUTTON_INCLUDED
'                                                       *********************


'*** Button routines
'
Declare Sub      WButtonSetClass Lib "WATTCtrl.dll" (lpszClassName$)
Declare Function WButtonExists   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WButtonEnabled  Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WButtonFocus    Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub      WButtonClick    Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WButtonHide     Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WButtonShow     Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WButtonEnable   Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WButtonDisable  Lib "WATTCtrl.dll" (lpszName$)


'                                                       *********************
'$DEFINE W_BUTTON_INCLUDED
'$ENDIF
'$IFDEF W_CHECK AND NOT W_CHECK_INCLUDED
'                                                       *********************


' CheckBox routines
'
Declare Sub      WCheckSetClass Lib "WATTCtrl.dll" (lpszClassName$)
Declare Function WCheckExists   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WCheckEnabled  Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WCheckFocus    Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WCheckState    Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub      WCheckClick    Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WCheckHide     Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WCheckShow     Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WCheckEnable   Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WCheckDisable  Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WCheckCheck    Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WCheckUnCheck  Lib "WATTCtrl.dll" (lpszName$)

Const UNCHECKED = 0
Const CHECKED   = 1
Const GRAYED    = 2


'                                                       *********************
'$DEFINE W_CHECK_INCLUDED
'$ENDIF
'$IFDEF W_OPTION AND NOT W_OPTION_INCLUDED
'                                                       *********************


'*** Option Button routines
'
Declare Sub      WOptionSetClass Lib "WATTCtrl.dll" (lpszClassName$)
Declare Function WOptionExists   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WOptionEnabled  Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WOptionFocus    Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WOptionState    Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub      WOptionClick    Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WOptionHide     Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WOptionShow     Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WOptionEnable   Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WOptionDisable  Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WOptionSelect   Lib "WATTCtrl.dll" (lpszName$)


'                                                       *********************
'$DEFINE W_OPTION_INCLUDED
'$ENDIF
'$IFDEF W_LIST AND NOT W_LIST_INCLUDED
'                                                       *********************


'*** Listbox routines
'
Declare Sub	 WListSetClass	   Lib "WATTCtrl.dll" (lpszClass$)
Declare Function WListExists	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WListCount	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub	 WListText	   Lib "WATTCtrl.dll" (lpszName$, lpszBuffer$)
Declare Function  ListText			      (lpszName$) As String
Declare Function WListLen	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WListIndex	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WListTopIndex	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub	 WListItemText	   Lib "WATTCtrl.dll" (lpszName$, sItem%, lpszBuffer$)
Declare Function  ListItemText			      (lpszName$, sItem%) As String
Declare Function WListItemLen	   Lib "WATTCtrl.dll" (lpszName$, sItem%) As Integer
Declare Function WListItemExists   Lib "WATTCtrl.dll" (lpszName$, lpszItem$) As Integer
Declare Sub	 WListItemClk	   Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WListItemCtrlClk  Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WListItemShftClk  Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WListItemDblClk   Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WListItemClkT	   Lib "WATTCtrl.dll" (lpszName$, lpszItem$)
Declare Sub	 WListItemCtrlClkT Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WListItemShftClkT Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WListItemDblClkT  Lib "WATTCtrl.dll" (lpszName$, lpszItem$)
Declare Sub	 WListClear	   Lib "WATTCtrl.dll" (lpszName$)
Declare Sub	 WListAddItem	   Lib "WATTCtrl.dll" (lpszName$, lpszItem$)
Declare Sub	 WListDelItem	   Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WListDelItemT	   Lib "WATTCtrl.dll" (lpszName$, lpszItem$)

'----------------------------------------------------------------------------
' ListText: Layered routine for WListText()
'----------------------------------------------------------------------------
Function ListText(lpszName$) As String
Dim lpszBuffer As String
    itemLength = WListLen(lpszName)
    If itemLength >= 0 Then
	lpszBuffer = String$(itemLength+1, " ")
	WListText lpszName, lpszBuffer
	ListText = lpszBuffer
	lpszBuffer = ""
    End If
End Function

'----------------------------------------------------------------------------
' ListItemText: Layered routine for WListItemText()
'----------------------------------------------------------------------------
Function ListItemText(lpszName$, sItem%) As String
Dim lpszBuffer As String
    itemLength = WListItemLen(lpszName, sItem)
    If itemLength >= 0 Then
	lpszBuffer = String$(itemLength+1, " ")
	WListItemText lpszName, sItem, lpszBuffer
	ListItemText = lpszBuffer
	lpszBuffer = ""
    End If
End Function


'                                                       *********************
'$DEFINE W_LIST_INCLUDED
'$ENDIF
'$IFDEF W_COMBO AND NOT W_COMBO_INCLUDED
'                                                       *********************


'*** Combobox routines
'
Declare Sub      WComboSetClass    Lib "WATTCtrl.dll" (lpszClass$)
Declare Function WComboExists      Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WComboCount       Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub      WComboSetText     Lib "WATTCtrl.dll" (lpszName$, lpszText$)
Declare Sub      WComboText        Lib "WATTCtrl.dll" (lpszName$, lpszBuffer$)
Declare Function  ComboText			      (lpszName$) As String
Declare Function WComboLen	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub	 WComboSelText	   Lib "WATTCtrl.dll" (lpszName$, lpszBuffer$)
Declare Function  ComboSelText			      (lpszName$) As String
Declare Function WComboSelLen	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Function WComboIndex	   Lib "WATTCtrl.dll" (lpszName$) As Integer
Declare Sub      WComboItemText    Lib "WATTCtrl.dll" (lpszName$, sItem%, lpszBuffer$)
Declare Function  ComboItemText 		      (lpszName$, sItem%) As String
Declare Function WComboItemLen	   Lib "WATTCtrl.dll" (lpszName$, sItem%) As Integer
Declare Function WComboItemExists  Lib "WATTCtrl.dll" (lpszName$, lpszItem$) As Integer
Declare Sub	 WComboItemClk	   Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub	 WComboItemDblClk  Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub      WComboItemClkT    Lib "WATTCtrl.dll" (lpszName$, lpszItem$)
Declare Sub      WComboItemDblClkT Lib "WATTCtrl.dll" (lpszName$, lpszItem$)
Declare Sub      WComboClear       Lib "WATTCtrl.dll" (lpszName$)
Declare Sub      WComboAddItem     Lib "WATTCtrl.dll" (lpszName$, lpszItem$)
Declare Sub	 WComboDelItem	   Lib "WATTCtrl.dll" (lpszName$, sItem%)
Declare Sub      WComboDelItemT    Lib "WATTCtrl.dll" (lpszName$, lpszItem$)

'----------------------------------------------------------------------------
' ComboText: Layered routine for WComboText()
'----------------------------------------------------------------------------
Function ComboText(lpszName$) As String
Dim lpszBuffer As String
    itemLength = WComboLen(lpszName$)
    If itemLength >= 0 Then
	lpszBuffer = String$(itemLength+1, " ")
	WComboText lpszName, lpszBuffer
	ComboText = lpszBuffer
	lpszBuffer = ""
    End If
End Function

'----------------------------------------------------------------------------
' ComboSelText: Layered routine for WComboSelText()
'----------------------------------------------------------------------------
Function ComboSelText(lpszName$) As String
Dim lpszBuffer As String
    itemLength = WComboSelLen(lpszName$)
    If itemLength >= 0 Then
	lpszBuffer = String$(itemLength+1, " ")
	WComboSelText lpszName, lpszBuffer
	ComboSelText = lpszBuffer
	lpszBuffer = ""
    End If
End Function

'----------------------------------------------------------------------------
' ComboItemText: Layered routine for WComboItemText()
'----------------------------------------------------------------------------
Function ComboItemText(lpszName$, sItem%) As String
Dim lpszBuffer As String
    itemLength = WComboItemLen(lpszName, sItem)
    If itemLength >= 0 Then
	lpszBuffer = String$(itemLength+1, " ")
	WComboItemText lpszName, sItem, lpszBuffer
	ComboItemText = lpszBuffer
	lpszBuffer = ""
    End If
End Function


'                                                       *********************
'$DEFINE W_COMBO_INCLUDED
'$ENDIF
'$IFDEF W_EDIT AND NOT W_EDIT_INCLUDED
'                                                       *********************


'*** Edit box routines
'
Declare Sub       WEditSetClass Lib "WATTCtrl.dll"  (lpszClass$)
Declare Function  WEditExists   Lib "WATTCtrl.dll"  (lpszName$) As Integer
Declare Function  WEditLen	Lib "WATTCtrl.dll"  (lpszName$) As Long
Declare Sub       WEditText     Lib "WATTCtrl.dll"  (lpszName$, lpszBuffer$)
Declare Function   EditText                         (lpszName$) As String
Declare Sub       WEditSetText  Lib "WATTCtrl.dll"  (lpszName$, lpszBuffer$)
Declare Sub       WEditSelText  Lib "WATTCtrl.dll"  (lpszName$, lpszBuffer$)
Declare Function   EditSelText                      (lpszName$) As String
Declare Function  WEditSelLen	Lib "WATTCtrl.dll"  (lpszName$) As Long
Declare Sub	  WEditLineText Lib "WATTCtrl.dll"  (lpszName$, Index%, lpszBuffer$)
Declare Function   EditLineText 		    (lpszName$, Index%) As String
Declare Function  WEditLineLen	Lib "WATTCtrl.dll"  (lpszName$, Index%) As Long
Declare Function  WEditPos      Lib "WATTCtrl.dll"  (lpszName$) As Integer
Declare Function  WEditLine     Lib "WATTCtrl.dll"  (lpszName$) As Integer
Declare Function  WEditChar     Lib "WATTCtrl.dll"  (lpszName$) As Long
Declare Function  WEditFirst    Lib "WATTCtrl.dll"  (lpszName$) As Integer
Declare Function  WEditLines    Lib "WATTCtrl.dll"  (lpszName$) As Integer
Declare Sub       WEditClick    Lib "WATTCtrl.dll"  (lpszName$)

'----------------------------------------------------------------------------
' EditText: Layered routine for WEditText()
'----------------------------------------------------------------------------
Function EditText(lpszName$) As String
Dim lpszBuffer As String
    textLength = WEditLen(lpszName)
    If textLength >=0 Then
	lpszBuffer = String$(textLength+1, " ")
	WEditText lpszName, lpszBuffer
	EditText = lpszBuffer
	lpszBuffer = ""
    End If
End Function

'----------------------------------------------------------------------------
' EditSelText: Layered routine for WEditSelText()
'----------------------------------------------------------------------------
Function EditSelText(lpszName$) As String
Dim lpszBuffer As String
    selLength = WEditSelLen(lpszName)
    If selLength >= 0 Then
	lpszBuffer = String$(selLength+1, " ")
	WEditSelText lpszName, lpszBuffer
	EditSelText = lpszBuffer
	lpszBuffer = ""
    End If
End Function

'----------------------------------------------------------------------------
' EditLineText: Layered routine for WEditLineText()
'----------------------------------------------------------------------------
Function EditLineText(lpszName$, sIndex%) As String
Dim lpszBuffer As String
    lineLength = WEditLineLen(lpszName, sIndex)
    If lineLength >= 0 Then
	lpszBuffer = String$(lineLength+1, " ")
	WEditLineText lpszName, sIndex, lpszBuffer
	EditLineText = lpszBuffer
	lpszBuffer = ""
    End if
End Function


'                                                       *********************
'$DEFINE W_EDIT_INCLUDED
'$ENDIF
'$IFDEF W_ERROR AND NOT W_ERROR_INCLUDED
'                                                       *********************


Declare Function WError     Lib "WATTCtrl.dll" () As Integer
Declare Sub      WErrorSet  Lib "WATTCtrl.dll" (sErrorNum%)
Declare Sub      WErrorText Lib "WATTCtrl.dll" (lpszBuffer$)
Declare Function  ErrorText		       () As String
Declare Function WErrorLen  Lib "WATTCtrl.dll" () As Integer

Const NO_ERROR			  = 0
Const ERR_MENU_NOT_FOUND          = 1
Const ERR_MENU_ITEM_NOT_FOUND     = 2
Const ERR_NOT_A_LISTBOX           = 3
Const ERR_LISTBOX_NOT_FOUND       = 4
Const ERR_ITEM_NOT_IN_LISTBOX     = 5
Const ERR_INVALID_LISTBOX_INDEX   = 6
Const ERR_LISTBOX_HAS_NO_STRINGS  = 7
Const ERR_NOT_A_COMBOBOX          = 8
Const ERR_COMBOBOX_NOT_FOUND      = 9
Const ERR_ITEM_NOT_IN_COMBOBOX    = 10
Const ERR_INVALID_COMBOBOX_INDEX  = 11
Const ERR_COMBOBOX_HAS_NO_EDITBOX = 12
Const ERR_COMBOBOX_HAS_NO_STRINGS = 13
Const ERR_NOT_AN_EDITBOX          = 14
Const ERR_EDITBOX_NOT_FOUND       = 15
Const ERR_BUTTON_NOT_FOUND        = 16
Const ERR_OPTION_BUTTON_NOT_FOUND = 17
Const ERR_CHECKBOX_NOT_FOUND      = 18
Const ERR_INVALID_WINDOW_HANDLE   = 19
Const MAX_ERROR                   = 20

'----------------------------------------------------------------------------
' ErrorText: Layered routine for WErrorText() of WATTCtrl.DLL.
'            Simulates a DLL String Function.
'----------------------------------------------------------------------------
Function ErrorText() As String
Dim lpszBuffer As String
    errorLength= WErrorLen
    lpszBuffer = String$(errorLength+1, " ")
    WErrorText lpszBuffer
    ErrorText = lpszBuffer
    lpszBuffer = ""
End Function


'                                                       *********************
'$DEFINE W_ERROR_INCLUDED
'$ENDIF
'                                                       *********************
