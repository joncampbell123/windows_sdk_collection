'$IFNDEF MFDLL
'$DEFINE MFDLL

'******************************************************************************
'
' This is the include file for MFDLL.DLL. It contains definitions for the
' subroutines and functions in MFDLL.DLL and a few commonly used WATTDRVR
' routines.
'
' Change History:
'    10/12/91 - mikegi added File Manager functions
'    09/24/91 - mikeli moved non-MFDLL stuff to WTDUTIL.INC
'    09/16/91 - mikegi oops, changed string(128," ") to string$(128," ")
'    09/10/91 - mikegi changed s$ = " ... " to s$ = string(128," ")
'
'******************************************************************************

'------------------------  Constant Declarations  -----------------------------

CONST MF_GETITEM   =  1
CONST MF_CLICK     =  2
CONST MF_DBLCLICK  =  3


' Constants for IsCursor

CONST MFC_ARROW    = 32512
CONST MFC_IBEAM    = 32513
CONST MFC_WAIT     = 32514
CONST MFC_CROSS    = 32515
CONST MFC_UPARROW  = 32516
CONST MFC_SIZE     = 32640
CONST MFC_ICON     = 32641
CONST MFC_SIZENWSE = 32642
CONST MFC_SIZENESW = 32643
CONST MFC_SIZEWE   = 32644
CONST MFC_SIZENS   = 32645


'-----------------------  Subroutine Declarations  ----------------------------

DECLARE SUB      SelectWindow         LIB "MFDLL.DLL" ( n$ )
DECLARE FUNCTION WindowExists         LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE SUB      MaximizeWindow       LIB "MFDLL.DLL" ()
DECLARE SUB      MinimizeWindow       LIB "MFDLL.DLL" ()
DECLARE SUB      RestoreWindow        LIB "MFDLL.DLL" ()
DECLARE SUB      PositionWindow       LIB "MFDLL.DLL" ( x%, y% )
DECLARE SUB      SizeWindow           LIB "MFDLL.DLL" ( dx%, dy% )


DECLARE SUB      SelectMenuItem       LIB "MFDLL.DLL" ( n$ )
DECLARE FUNCTION MenuItemExists       LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE FUNCTION GetMenuCount         LIB "MFDLL.DLL" ()     AS INTEGER
DECLARE SUB      GetMenuItemText      LIB "MFDLL.DLL" ( i%, t$, c% )
DECLARE FUNCTION IsMenuItemGrayed     LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE FUNCTION IsMenuItemChecked    LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE FUNCTION IsMenuItemEnabled    LIB "MFDLL.DLL" ( n$ ) AS INTEGER


DECLARE FUNCTION ButtonExists         LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE FUNCTION ButtonEnabled        LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE FUNCTION ButtonHasFocus       LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE SUB      ClickButton          LIB "MFDLL.DLL" ( n$ )
DECLARE FUNCTION GetButtonState       LIB "MFDLL.DLL" ( n$ ) AS INTEGER
DECLARE SUB      SetButtonState       LIB "MFDLL.DLL" ( n$, state% )


DECLARE SUB      SelectListBox        LIB "MFDLL.DLL" ( n$ )
DECLARE FUNCTION ListBoxItemExists    LIB "MFDLL.DLL" ( item$ ) AS INTEGER
DECLARE FUNCTION ListBoxItemSelected  LIB "MFDLL.DLL" ( i% ) AS INTEGER
DECLARE SUB      SelectListBoxItem    LIB "MFDLL.DLL" ( i%, s% )
DECLARE FUNCTION GetListBoxCount      LIB "MFDLL.DLL" ()        AS INTEGER
DECLARE SUB      GetListBoxText       LIB "MFDLL.DLL" ( i%, t$, c% )
DECLARE SUB      GetListBoxSelection  LIB "MFDLL.DLL" ( t$, c% )
DECLARE SUB      ClickListBoxItem     LIB "MFDLL.DLL" ( item$ )
DECLARE SUB      DblClickListBoxItem  LIB "MFDLL.DLL" ( item$ )
DECLARE FUNCTION AdvListBox           LIB "MFDLL.DLL" ( index%, action%) AS LONG


DECLARE SUB      SelectComboBox       LIB "MFDLL.DLL" ( n$ )
DECLARE FUNCTION ComboBoxItemExists   LIB "MFDLL.DLL" ( item$ ) AS INTEGER
DECLARE FUNCTION GetComboBoxCount     LIB "MFDLL.DLL" ()        AS INTEGER
DECLARE FUNCTION GetComboBoxTextLength LIB "MFDLL.DLL" () AS INTEGER
DECLARE SUB      GetComboBoxText      LIB "MFDLL.DLL" ( t$, c% )
DECLARE SUB      SetComboBoxText      LIB "MFDLL.DLL" ( t$ )
DECLARE SUB      GetComboBoxItemText  LIB "MFDLL.DLL" ( i%, t$, c% )
DECLARE SUB      ClickComboBoxItem    LIB "MFDLL.DLL" ( item$ )
DECLARE SUB      DblClickComboBoxItem LIB "MFDLL.DLL" ( item$ )
DECLARE FUNCTION AdvComboBox          LIB "MFDLL.DLL" ( index%, action%) AS LONG


DECLARE SUB      SelectEditControl    LIB "MFDLL.DLL" ( n$ )
DECLARE FUNCTION GetEditTextLength    LIB "MFDLL.DLL" ()        AS INTEGER
DECLARE SUB      GetEditText          LIB "MFDLL.DLL" ( t$, c% )
DECLARE SUB      SetEditText          LIB "MFDLL.DLL" ( t$ )


DECLARE SUB      FMSelectTree         LIB "MFDLL.DLL" ( t$ )

DECLARE FUNCTION FMGetDirCount        LIB "MFDLL.DLL" () AS INTEGER
DECLARE SUB      FMGetCurrentDir      LIB "MFDLL.DLL" ( n$, l% )
DECLARE SUB      FMGetDirName         LIB "MFDLL.DLL" ( i%, n$, l% )
DECLARE SUB      FMClickDir           LIB "MFDLL.DLL" ( n$ )
DECLARE SUB      FMDblClickDir        LIB "MFDLL.DLL" ( n$ )

DECLARE FUNCTION FMGetFileCount       LIB "MFDLL.DLL" () AS INTEGER
DECLARE SUB      FMGetCurrentFile     LIB "MFDLL.DLL" ( n$, l% )
DECLARE SUB      FMGetFileName        LIB "MFDLL.DLL" ( i%, n$, l% )
DECLARE SUB      FMClickFile          LIB "MFDLL.DLL" ( n$ )
DECLARE SUB      FMDblClickFile       LIB "MFDLL.DLL" ( n$ )

DECLARE SUB      GetStaticText        LIB "MFDLL.DLL" ( i%, s$, l% )
DECLARE FUNCTION IsStaticVisible      LIB "MFDLL.DLL" ( i% ) AS INTEGER
DECLARE FUNCTION StaticExists         LIB "MFDLL.DLL" ( n$ ) AS INTEGER


DECLARE SUB      QuestionBox          LIB "MFDLL.DLL" ( p$, i$, b$, l% )
DECLARE SUB      SelectBox            LIB "MFDLL.DLL" ( p$, c$, b$, l% )
DECLARE SUB      PrintScreen          LIB "MFDLL.DLL" ( p$, l$ )
DECLARE FUNCTION IsCursor             LIB "MFDLL.DLL" ( id% ) AS INTEGER


'----------------------------  Error Handling  --------------------------------

DECLARE SUB GetErrorText LIB "MFDLL.DLL" ( t$, l% )


TRAP AbortScript FROM "MFDLL.DLL"
  s$ = string$(128," ")
  GetErrorText s$, len(s$)
  VIEWPORT ON                    ' make sure viewport is on to view msg...
  Print "ERROR: "; s$
  Pause "ERROR: " + s$
  Print "Script Aborted"
  END
END TRAP

'$ENDIF
