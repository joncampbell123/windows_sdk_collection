'******************************* MYOWN.INC    *********************************
'Required Files: MFUNCTS.MST, MSTEST.INC
'
'******************************************************************************


'******************************************************************************
' DEFINES
'******************************************************************************

'$DEFINE TESTSCRN
'$DEFINE TESTCTRL
'$DEFINE TESTEVNT



'$INCLUDE 'MSTEST.INC'

'Prototypes from IMAGEF.MST
Declare FUNCTION GetSystemMetrics LIB "USER32" (SM_INDEX) AS INTEGER
Declare FUNCTION FindWindow LIB "USER32" ALIAS "FindWindowA"  (szClass$, szWindow$) AS INTEGER

Declare FUNCTION AttachThreadInput LIB "USER32" ALIAS "AttachThreadInput" (idAttach&, idAttachTo&, fAttach&) AS LONG
Declare FUNCTION GetCurrentThreadId LIB "KERNEL32" ALIAS "GetCurrentThreadId" () AS LONG

Declare Function WinExec Lib "KERNEL32" ALIAS "WinExec" (lpCmdLine$, nCmdShow%) As Integer


Declare SUB PlaceViewWindow(BoxSize$)
Declare SUB PlaceToolBox(tx%,ty%)
Declare SUB PlacePalette(px%,py%)
Declare SUB ToolBoxShow(State$)
Declare SUB PaletteShow(State$)
Declare SUB ViewWindowShow(State$)
Declare SUB GridShow(State$)
Declare SUB SetBrushSize(Size$)
Declare SUB ProcessResults( FileName$, SubName$, DumpIndex% )
Declare SUB CreateGolden( FileName$, SubName$, DumpIndex% )
Declare SUB Verify(FileName$, SubName$, DumpIndex%)
Declare SUB ToolTest(Width$)
Declare SUB IToolTest(Width$)
Declare SUB Bitmap32x32x16init
DECLARE SUB WaitWhileWnd (Title$)
DECLARE SUB WaitForWnd (Title$)

'$INCLUDE 'IMAGEINC.MST'


