'$IFNDEF WTDUTIL
'$DEFINE WTDUTIL
' Include Watt.Inc if it hasn't been included yet
'$IFNDEF WATT_INCLUDED
'$Include: 'Watt.Inc'
'$ENDIF

'=======================================================================
'NOTES:  * Use chr$(0) for ""
'        * Call Subroutines with multiple params without ()
'             EX: NetConnect "r:", "\\pyrex\user", chr$(0)
'        * ALWAYS do something with function's return value!!! 
'
'
' History:
'  09/25/91 - GerardoB: Added ExpandTabs$ and Modified PadLeft and PadRight.
'  09/24/91 - GerardoB: Sorted subs alphabetically. Replaced Delay()
'  09/24/91 - mikeLi: created USER, GDI, KERNEL, WATTOOLS sections
'  09/20/91 - mikegi  started history section
'
'=======================================================================

'-----------------------------------------------------------------------
' USER
'-----------------------------------------------------------------------

   ' return codes for MessageBox () 
CONST IDOK     = 1
CONST IDCANCEL = 2
CONST IDABORT  = 3
CONST IDRETRY  = 4
CONST IDIGNORE = 5
CONST IDYES    = 6
CONST IDNO     = 7
   ' Commands for GetWindow
Const GW_HWNDFIRST = 0
Const GW_HWNDLAST  = 1
Const GW_HWNDNEXT  = 2
Const GW_HWNDPREV  = 3
Const GW_OWNER     = 4
Const GW_CHILD     = 5
   ' Flags for GetMenuState
Const MF_BYCOMMAND = &H0
Const MF_BYPOSITION = &H400
   ' Codes for SendMessage
Const WM_USER       = &H400
Const LB_GETCURSEL  = (WM_USER+9)
Const LB_GETTEXT    = (WM_USER+10)
Const LB_GETTEXTLEN = (WM_USER+11)
' Type For GetCursorPos
Type CursorPos
 X As Integer
 Y As Integer
End Type
' Type For GetWindowRect
Type Rect
 Left   As Integer
 Top    As Integer
 Right  As Integer
 Bottom As Integer
End Type 
'
Declare Function ExitWindows           Lib "User" (dwReserved&,wReturnCode)         As Integer
DECLARE FUNCTION GetActiveWindow       LIB "USER" ()                                AS INTEGER
Declare Function GetClassName          Lib "User" (hWnd%,lpClassName$,nMaxCount%)   As Integer
Declare Sub GetCursorPos               Lib "User" (lpPoint As CursorPos)
Declare Function GetDlgItem            Lib "User" (hDlg%,nIDDlgItem%)               As Integer
Declare Function GetFocus              Lib "User" ()                                As Integer
Declare Function GetMenu               Lib "User" (hWnd%)                           As Integer
Declare Function GetMenuItemCount      Lib "User" (hMenu%)                          As Integer
Declare Function GetMenuState          Lib "User" (hMenu%,wId%,wFlags%)             As Integer
Declare Function GetMenuString         Lib "User" (hMenu%,wIDItem%,lpString$,nMaxCount%,wFlag%) As Integer
Declare Function GetSubMenu            Lib "User" (hMenu%,nPos%)                    As Integer
Declare Function GetTopWindow          Lib "User" (hWnd%) As Integer
Declare Function GetWindow             Lib "User" (hWnd%,wCmd%)                     As Integer
Declare Sub GetWindowRect              Lib "User" (hWnd%,Rect As Rect)
Declare Function GetWindowText         Lib "User" (hWnd%,lpString$,aint%)           As Integer
Declare Function GetWindowTextLength   Lib "User" (hWnd%)                           As Integer
Declare Function IsChild               Lib "User" (hWndParent%,hWnd%)               As Integer
Declare Function IsIconic              Lib "User" (hWnd%)                           As Integer
Declare Function IsWindow              Lib "User" (hWnd%)                           As Integer
DECLARE FUNCTION LoadString            LIB "USER" (h%, i%, s$, l%)                  AS INTEGER
DECLARE FUNCTION MessageBox            Lib "User" (hWnd%,lpText$,lpCaption$,wType%) AS INTEGER
Declare Function SendMessage           Lib "User" (hWnd%,wMsg%,wParam%,lParam As Any) As Long
DECLARE FUNCTION WNetAddConnection     LIB "USER" (n$, p$, l$)                      AS INTEGER
DECLARE FUNCTION WNetCancelConnection  LIB "USER" (l$, f% )                         AS INTEGER

'-----------------------------------------------------------------------
' KERNEL
'-----------------------------------------------------------------------

DECLARE FUNCTION GetModuleHandle          LIB "KERNEL" ( n$ ) AS INTEGER
DECLARE FUNCTION WriteProfileString       LIB "KERNEL" ( app$, kkey$, s$ ) AS INTEGER
DECLARE FUNCTION GetPrivateProfileString  LIB "KERNEL" ( app$, kkey$, def$, buf$, nsize%, fn$ ) AS INTEGER
DECLARE SUB      OutputDebugString        LIB "KERNEL" ( Buffer$ )

'-----------------------------------------------------------------------
' GDI
'-----------------------------------------------------------------------

'-----------------------------------------------------------------------
'///////////////////////////////////////////////////////////////////////
'-----------------------------------------------------------------------


DECLARE SUB      CloseDrWatson
DECLARE FUNCTION CountSetFile%
DECLARE SUB      Delay         (hundredths&)
DECLARE FUNCTION ExpandTabs$   (Text$, Spaces%)
DECLARE FUNCTION GetBuild()    AS STRING
DECLARE FUNCTION GetDate$ 
DECLARE FUNCTION GetRandom     (minInt&, maxInt&) AS INTEGER
DECLARE FUNCTION GetTime$ 
DECLARE FUNCTION GetOrganization() AS STRING
DECLARE FUNCTION GetUser()     AS STRING
DECLARE SUB      Inc           (nCount%)          
DECLARE FUNCTION Min%          (Value1%, Value2%)
DECLARE FUNCTION NetConnect    (localName$, netName$, passWord$) AS INTEGER
DECLARE FUNCTION NetDisconnect (localName$) AS INTEGER
DECLARE FUNCTION PadLeft       (textIn$, desiredLen%) AS STRING
DECLARE FUNCTION PadRight      (textIn$, desiredLen%) AS STRING
DECLARE FUNCTION RemoveNulls(szString$) As String
DECLARE SUB      SendKeys      (keys$)
DECLARE FUNCTION Trim$         (text$)
DECLARE FUNCTION TrimStr$      (value%)
DECLARE FUNCTION YesNoBox      (szWinTitle$, msg$) AS INTEGER

'-----------------------------------------------------------------------
'-----------------------------------------------------------------------

'-------------------------------------------------------------------------
' CloseDrWatson finds and closes DrWatson so that it won't interfere
' with UAE trapping.  This routine requires WATT.INC, which is included
' by default at the top of this include file.
'-------------------------------------------------------------------------
Sub CloseDrWatson
  If WFndWnd("dr. watson", FW_FOCUS) then
    Key K_ALT+K_F4
  End if
End sub

FUNCTION CountSetFile%
   ' Returns current number of files in FILELIST

   DIM nCount AS INTEGER
   DIM szTemp AS STRING

   FOR szTemp IN FILELIST
      Inc nCount
   NEXT szTemp
   CountSetFile% = nCount
END FUNCTION

'-----------------------------------------------------------------------

Sub delay (hundredths&)
'
' Timer is reset every 24 hours. 8640000 is its maximum value.
'
  If Hundredths& > 0 Then
    InitialTime& = Timer
    While  Timer >= InitialTime& AND Timer < InitialTime& + Hundredths&
    Wend
    If Timer < InitialTime& Then
      ElapsedTime& = 8640000 - InitialTime&
      While Timer < Hundredths& - ElapsedTime&    
      Wend
    End If
  End If  
End Sub

'-----------------------------------------------------------------------

Function ExpandTabs$    (Text$, Spaces%)
 
  Tab$ = Chr$(9)
  TextBuffer$ = Text$
  StartAt% = 1
  TabPosition% = 1
  While TabPosition% > 0 And StartAt% <= Len(TextBuffer$)
    TabPosition% = Instr(StartAt%, TextBuffer$, Tab$)
    If TabPosition% > 0 Then
      TextBuffer$ = Mid$(TextBuffer$,1,TabPosition%-1) + String$(Spaces%," ") + Mid$(TextBuffer$, TabPosition%+1)
      StartAt% = TabPosition% + Spaces%
    End If     
  Wend
  ExpandTabs$ = TextBuffer$ 

End Function

'-----------------------------------------------------------------------

FUNCTION GetTime$
   ' returns the stupid time.

   GetTime$ = "[" + Mid$ (DateTime$, 10, 8) + "] "
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION GetDate$
   ' returns the stupid date.

   GetDate$ = Mid$ (DateTime$, 1, 8)
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION GetBuild() AS STRING
'  print "Entering GetBuild()"

  s$ = string$(64," ")
  h%  = GetModuleHandle( "USER" )
  rc% = LoadString( h%, 516, s$, len(s$) )
  GetBuild = s$
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION GetOrganization() AS STRING
  s$ = string$(64," ")
  h%  = GetModuleHandle( "USER" )
  rc% = LoadString( h%, 515, s$, len(s$) )
  GetOrganization = s$
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION GetRandom (minInt&, maxInt&) AS INTEGER
   ' returns random integer between minInt% and maxInt%
   ' NOTE: You should initialize the random number generator before 
   '       using this.  Something like:
   ' Randomize Timer                  ' get seed
   
   temp% = (Rnd * (maxInt& - minInt& + 1)) / 32767 + minInt&  ' (0 .. 1) * maxInt%

   print "   GetRandom(): random # = " + str$ (temp%)
   getRandom = temp%
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION GetUser() AS STRING
  s$ = string$(64," ")
  h%  = GetModuleHandle( "USER" )
  rc% = LoadString( h%, 514, s$, len(s$) )
  GetUser = s$
END FUNCTION

'-----------------------------------------------------------------------

SUB Inc (nCount%)          
   ' increments integer values
   nCount% = nCount% + 1
END SUB

'-----------------------------------------------------------------------

Function Min%(Value1%, Value2%)
  If Value1% < Value2% Then
    Min% = Value1%
   Else
    Min% = Value2%
  End If  
End Function

'-----------------------------------------------------------------------

FUNCTION NetConnect (localName$, netName$, passWord$) AS INTEGER
   ' returns 0 if successful

   print "   ESTABLISHING NET CONNECTION: " + localName$ + " " + netName$ + " " + passWord$
   print "   CALLING WNETCANCELCONNECTION"
   rc% = WNetCancelConnection (localName$, 0)
   print "   RETURN CODE = " + " " + str$ (rc%)
 
   print "   CALLING WNETADDCONNECTION"
   code% = WNetAddConnection (netName$, passWord$, localName$) 
   print "   RETURN CODE = " + " " + str$ (code%)

   IF (code% <> 0) THEN
     print "   ERROR CONNECTING TO NETWORK SHARE: " + " " + netName$
   END IF
   
   NetConnect = code%
END FUNCTION   

'-----------------------------------------------------------------------

FUNCTION NetDisconnect (localName$) AS INTEGER
   ' returns 0 if successful
   print "   DISCONNECTING NET CONNECTION: " + " " + localName$ 
   print "   CALLING WNETCANCELCONNECTION"
   code% = WNetCancelConnection (localName$, 0)
   print "   RETURN CODE = " + " " + str$ (code%)
 
   IF (code% <> 0) THEN
      print "   ERROR DISCONNECTING FROM LOCAL NETWORK SHARE: " + " " + localName$
   END IF
   
   NetDisconnect = code%
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION PadLeft (textIn$, desiredLen%) AS STRING
   ' Returns textIn$ with spaces added before the string to make it 
   '   desiredLen%.  Typically used for aligning columns of numbers.
   ' Returns (textIn$ - 3) + "..." if textIn$ >= desiredLen%


   textIn$ = Trim$ (ExpandTabs$(textIn$,1))
   If Len(TextIn$) <= DesiredLen% Then   
     PadLeft = String$(DesiredLen% - Len(textIn$), " ") + textIn$
    Else 
     Dots% = Min%(3,DesiredLen% /3)
     PadLeft = Mid$(textIn$, 1, desiredLen% - Dots%) + String$(Dots%, ".")
   End If

END FUNCTION

'-----------------------------------------------------------------------

FUNCTION PadRight (textIn$, desiredLen%) AS STRING
   ' Returns textIn$ with spaces added after the string to make it 
   '   desiredLen%.  Typically used for aligning columns of text.
   ' Returns (textIn$ - 3) + "..." if textIn$ >= desiredLen%

   textIn$ = Trim$ (ExpandTabs$(textIn$,1))
   If Len(TextIn$) <= DesiredLen% Then   
     PadRight =  TextIn$ + String$(DesiredLen% - Len(textIn$), " ")
    Else 
     Dots% = Min%(3,DesiredLen% /3)
     PadRight = Mid$(textIn$, 1, desiredLen% - Dots%) + String$(Dots%, ".")
   End If

END FUNCTION

'-----------------------------------------------------------------------
Function RemoveNulls(szString$) As String
        For x% = 1 To Len(szString$)   'FIND THE FIRST NULL CHAR
            If Mid$(szString$, x%, 1) = CHR$(0) Then
                Exit For               'JUMP OUT OF LOOP WHEN FOUND
                End If
            Next
        RemoveNulls = Mid$(szString$, 1, x% - 1)
End FUNCTION

'-----------------------------------------------------------------------

SUB SendKeys( keys$ )              
  KeyStrokes% = PlayKeys(keys$, len(keys$), 0)
END SUB                            

'-----------------------------------------------------------------------

FUNCTION Trim$ (text$)
   Trim$ = LTrim$ (RTrim$ (text$))
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION TrimStr$ (value%)
   ' trims leading spaces returned by str$()
   TrimStr = LTrim$ (Str$ (value%))
END FUNCTION

'-----------------------------------------------------------------------

FUNCTION YesNoBox (szWinTitle$, msg$) AS INTEGER
   ' Returns IDYES if 'YES' button was pressed

'   CONST MB_YESNO = &H4

   YesNoBox = MessageBox (GetActiveWindow (), msg$, szWinTitle$, &H4)
END FUNCTION 


'$ENDIF
