' ------------------------------------------------------------------------
'
'     WATTools.inc -- Windows 3.0 API Declarations for WATTDrvr
'
'         This file only contains the general purpose defines.  To get
'            the API declarations, include either WinGDI.Bi, WinKern.Bi, or
'            WinUser.Bi  individually or in pairs or all together.
'
'              Copyright (C) 1991 Microsoft Corporation
'
'  You have a royalty-free right to use, modify, reproduce and distribute
'  this file (and/or any modified version) in any way you find useful,
'  provided that you agree that Microsoft has no warranty, obligation or
'  liability for its contents.  Refer to the Microsoft Windows Programmer's
'  Reference for further information.
'
' This file is not garanteed by Microsoft to be error free.  Every effort has been
'  made to ensure proper data-types and declarations etc., but no testing has been performed
'  using this include file.  Additionally, some API's, though listed, may not be compatible
'  with the WATTDrvr language.
' ------------------------------------------------------------------------
'//----------------------------------------------------------------------------
'// Copyright Microsoft Corporation 1990-1991
'//----------------------------------------------------------------------------


'//----------------------------------------------------------------------------
'// Generic Global Variables
'//----------------------------------------------------------------------------
Global szCR$ 
szCR$ = Chr$(10) + Chr$(13)

Const True% = -1
Const False% = 0
Const NULL = 0
Global NULL$
NULL$ = Chr$(0)

Const MouseMove% = 777
Const MouseLClick% = 768
Const MouseLClkDrg% = 774
Const Append% = 0
Const Replace% = 1
Const Insert% = 2

Const MB_SysModal% = 4096
Const MB_TaskModal% = 8192      'Windows TASKMODAL Message box type
Const MB_IconStop% = 16         'Stop Sign for Message box type
Const MB_IconQuestion% = 32     '? Sign for Message box type
Const MB_IconExclamation% = 48  '! Sign for Message box type
Const MB_IconInfo% = 64         '* Sign for Message box type
Const GW_HWNDFIRST% = 0
Const GW_HWNDLAST% = 1
Const GW_HWNDNEXT% = 2
Const GW_HWNDPREV% = 3
Const GW_HWNDOWNER% = 4
Const SW_HIDE% = 0
Const SW_NORMAL% = 1
Const SW_MINIMIZED% = 2
Const SW_MAXIMIZE% = 3
Const SW_NOACTIVATE% = 4
Const SW_SHOW% = 5
Const SW_MINIMIZE% = 6
Const SW_MINNOACTIVE% = 7
Const SW_NA% = 8
Const SW_RESTORE% = 9

'//----------------------------------------------------------------------------
'//
'//  Error Codes and Function Declarations for WATTEvnt.DLL
'//
'//----------------------------------------------------------------------------

'// Timer Structure
TYPE TimerEventStr
       delay as integer
       Event as integer
       FileName as string * 13
       x1 as integer
       y1 as integer
       x2 as integer
       y2 as integer
       scrnumber as integer
       action as integer
       hide as integer
       EvhWnd as integer
       CompareType as integer
       sendesc as integer
END TYPE


'// WATTEvnt.DLL Functions

DECLARE FUNCTION PlayKeysSpeed    LIB "WATTEvnt.DLL"  (Speed as Long) as Integer
DECLARE FUNCTION PlayKeys         LIB "WATTEvnt.DLL"  (PlayString as STRING,  LENSTRING as integer,  Flag as integer) As Integer
DECLARE FUNCTION MouseEvent       LIB "WATTEvnt.DLL"  ( MouseE as integer, x as integer, y as integer, flag as integer) As Integer
DECLARE FUNCTION SleepDelay       LIB "WATTEvnt.DLL"  ( NumSeconds as integer) As Integer
' TimerEvent: Use the PTR function to get the address of a TimerEventStr.
DECLARE FUNCTION TimerEvent       LIB "WATTEvnt.DLL"  (lpTimeStruct as Long) As Integer
DECLARE FUNCTION GetEventError    LIB "WATTEvnt.DLL"  () As Integer
DECLARE FUNCTION PlayKeyshWnd     LIB "WATTEvnt.DLL"  (PlayString as STRING,  LENSTRING as integer,  hWnd as integer) As Integer
DECLARE FUNCTION GetWattTimerData LIB "WATTEvnt.DLL"  (lpTimeStruct as Long) As Integer
DECLARE SUB SetGlobalEventhWnd     LIB "WATTEvnt.DLL"  ( hWnd as INTEGER)
DECLARE SUB SetWaitFlag            LIB "WATTEvnt.DLL"  ( hWnd as INTEGER)


'// Error Messages for WATTEvnt

CONST errStrTooLong   = 1
CONST errKeyParse     = 2
CONST errOutofMem     = 3
CONST errNotValidMouse= 4
CONST errInvalidX     = 5
CONST errInvalidY     = 6
CONST errAlreadyInUse = 7
CONST errNoTimer      = 8
CONST errActiveApp    = 9
CONST errKillTimerApp =10
CONST errSetSleep     =11
CONST errTimerAllSet = 12
CONST errCouldntfindMouse = 13

'// Timer Events
CONST EV_NOTHING       =  0
CONST EV_SCREENDUMP    =  1
CONST EV_SCREENCOMP    =  2
CONST EV_WINDOWDUMP    =  3
CONST EV_WINDOWCOMP    =  4
CONST NOEVENTSCHEDULED = 998
CONST EVENTPENDING     = 999

'// Mouse Event Codes
CONST WM_UMOUSEFIRST            = &h300
CONST WM_LCLICK                 = &h300
CONST WM_RCLICK                 = &h301
CONST WM_MCLICK                 = &h302
CONST WM_LDBLCLICK              = &h303
CONST WM_RDBLCLICK              = &h304
CONST WM_MDBLCLICK              = &h305
CONST WM_LCLICKDRAG             = &h306
CONST WM_RCLICKDRAG             = &h307
CONST WM_MCLICKDRAG             = &h308
CONST WM_UMOUSEMOVE             = &h309
CONST WM_ALT_LCLICK             = &h30A
CONST WM_ALT_RCLICK             = &h30B
CONST WM_ALT_MCLICK             = &h30C
CONST WM_CTRL_LCLICK            = &h30D
CONST WM_CTRL_RCLICK            = &h30E
CONST WM_CTRL_MCLICK            = &h30F
CONST WM_SHIFT_LCLICK           = &h310
CONST WM_SHIFT_RCLICK           = &h311
CONST WM_SHIFT_MCLICK           = &h312
CONST WM_CTRL_SHIFT_LCLICK      = &h313
CONST WM_CTRL_SHIFT_RCLICK      = &h314
CONST WM_CTRL_SHIFT_MCLICK      = &h315
CONST WM_ALT_LCLICKDRAG         = &h316
CONST WM_ALT_RCLICKDRAG         = &h317
CONST WM_ALT_MCLICKDRAG         = &h318
CONST WM_CTRL_LCLICKDRAG        = &h319
CONST WM_CTRL_RCLICKDRAG        = &h31A
CONST WM_CTRL_MCLICKDRAG        = &h31B
CONST WM_SHIFT_LCLICKDRAG       = &h31C
CONST WM_SHIFT_RCLICKDRAG       = &h31D
CONST WM_SHIFT_MCLICKDRAG       = &h31E
CONST WM_SHIFT_MOVE             = &h31F
CONST WM_CTRL_MOVE              = &h320
CONST WM_ALT_MOVE               = &h321
CONST WM_CTRL_SHIFT_MOVE        = &h322
CONST WM_LRCLICK                = &h323
CONST WM_LMCLICK                = &h324
CONST WM_RMCLICK                = &h325
CONST WM_CTRL_LRCLICK           = &h326
CONST WM_CTRL_LMCLICK           = &h327
CONST WM_CTRL_RMCLICK           = &h328
CONST WM_SHIFT_LRCLICK          = &h329
CONST WM_SHIFT_LMCLICK          = &h32A
CONST WM_SHIFT_RMCLICK          = &h32B
CONST WM_ALT_LRCLICK            = &h32C
CONST WM_ALT_LMCLICK            = &h32D
CONST WM_ALT_RMCLICK            = &h32E
CONST WM_CTRL_SHIFT_LRCLICK     = &h32F
CONST WM_CTRL_SHIFT_LMCLICK     = &h330
CONST WM_CTRL_SHIFT_RMCLICK     = &h331
CONST WM_SHIFT_LDBLCLICK        = &h332
CONST WM_SHIFT_RDBLCLICK        = &h333
CONST WM_SHIFT_MDBLCLICK        = &h334
CONST WM_SHIFT_LRDBLCLICK       = &h335
CONST WM_SHIFT_LMDBLCLICK       = &h336
CONST WM_SHIFT_RMDBLCLICK       = &h337
CONST WM_CTRL_SHIFT_LCLICKDRAG  = &h338
CONST WM_CTRL_SHIFT_RCLICKDRAG  = &h339
CONST WM_CTRL_SHIFT_MCLICKDRAG  = &h33A
CONST WM_CTRL_SHIFT_LRCLICKDRAG = &h33B
CONST WM_CTRL_SHIFT_LMCLICKDRAG = &h33C
CONST WM_CTRL_SHIFT_RMCLICKDRAG = &h33D
CONST WM_ALT_SHIFT_LCLICKDRAG   = &h33E
CONST WM_ALT_SHIFT_RCLICKDRAG   = &h33F
CONST WM_ALT_SHIFT_MCLICKDRAG   = &h340
CONST WM_ALT_SHIFT_LRCLICKDRAG  = &h341
CONST WM_ALT_SHIFT_LMCLICKDRAG  = &h342
CONST WM_ALT_SHIFT_RMCLICKDRAG  = &h343
CONST WM_LBUP                   = &h344
CONST WM_RBUP                   = &h345
CONST WM_MBUP                   = &h346
CONST WM_LRBUP                  = &h347
CONST WM_LMBUP                  = &h348
CONST WM_RMBUP                  = &h349
CONST WM_LBDOWN                 = &h34A
CONST WM_RBDOWN                 = &h34B
CONST WM_MBDOWN                 = &h34C
CONST WM_LRBDOWN                = &h34D
CONST WM_LMBDOWN                = &h34E
CONST WM_RMBDOWN                = &h34F
CONST WM_LRCLICKDRAG            = &h350
CONST WM_LMCLICKDRAG            = &h351
CONST WM_RMCLICKDRAG            = &h352
CONST WM_SHIFT_LRCLICKDRAG      = &h353
CONST WM_SHIFT_LMCLICKDRAG      = &h354
CONST WM_SHIFT_RMCLICKDRAG      = &h355
CONST WM_CTRL_LRCLICKDRAG       = &h356
CONST WM_CTRL_LMCLICKDRAG       = &h357
CONST WM_CTRL_RMCLICKDRAG       = &h358
CONST WM_ALT_LRCLICKDRAG        = &h359
CONST WM_ALT_LMCLICKDRAG        = &h35A
CONST WM_ALT_RMCLICKDRAG        = &h35B
CONST WM_CTRL_ALT_LCLICKDRAG    = &h35C
CONST WM_CTRL_ALT_RCLICKDRAG    = &h35D
CONST WM_CTRL_ALT_MCLICKDRAG    = &h35E
CONST WM_CTRL_ALT_LRCLICKDRAG   = &h35F
CONST WM_CTRL_ALT_LMCLICKDRAG        = &h360
CONST WM_CTRL_ALT_RMCLICKDRAG        = &h361
CONST WM_SHIFT_CTRL_ALT_LCLICKDRAG   = &h362
CONST WM_SHIFT_CTRL_ALT_RCLICKDRAG   = &h363
CONST WM_SHIFT_CTRL_ALT_MCLICKDRAG   = &h364
CONST WM_SHIFT_CTRL_ALT_LRCLICKDRAG  = &h365
CONST WM_SHIFT_CTRL_ALT_LMCLICKDRAG  = &h366
CONST WM_SHIFT_CTRL_ALT_RMCLICKDRAG  = &h367
CONST WM_CTRL_ALT_MOVE               = &h368
CONST WM_SHIFT_CTRL_ALT_MOVE         = &h369
CONST WM_SHIFT_ALT_LCLICK            = &h36A
CONST WM_CTRL_ALT_LCLICK             = &h36B
CONST WM_SHIFT_CTRL_ALT_LCLICK       = &h36C
CONST WM_SHIFT_ALT_RCLICK            = &h36D
CONST WM_CTRL_ALT_RCLICK             = &h36E
CONST WM_SHIFT_CTRL_ALT_RCLICK       = &h36F
CONST WM_SHIFT_ALT_MCLICK            = &h370
CONST WM_CTRL_ALT_MCLICK             = &h371
CONST WM_SHIFT_CTRL_ALT_MCLICK       = &h372
CONST WM_SHIFT_ALT_LRCLICK           = &h373
CONST WM_CTRL_ALT_LRCLICK            = &h374
CONST WM_SHIFT_CTRL_ALT_LRCLICK      = &h375
CONST WM_SHIFT_ALT_LMCLICK           = &h376
CONST WM_CTRL_ALT_LMCLICK            = &h377
CONST WM_SHIFT_CTRL_ALT_LMCLICK      = &h378
CONST WM_SHIFT_ALT_RMCLICK           = &h379
CONST WM_CTRL_ALT_RMCLICK            = &h37A
CONST WM_SHIFT_CTRL_ALT_RMCLICK      = &h37B
CONST WM_SHIFT_ALT_MOVE              = &h37C
CONST WM_UMOUSELAST                  = &h37C
