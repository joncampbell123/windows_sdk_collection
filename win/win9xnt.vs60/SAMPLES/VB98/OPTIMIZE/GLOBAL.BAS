Attribute VB_Name = "Global"
Option Explicit
Public Const VBVer = "5.0"
Public Const LastAppUpdate = "9/4/96"

Public Const WM_SYSCOMMAND = &H112
Public Const SC_RESTORE = &HF120
Public Const SWP_NOMOVE = 2
Public Const SWP_NOSIZE = 1
Public Const FLAGS = SWP_NOMOVE Or SWP_NOSIZE
Public Const HWND_TOPMOST = -1
Public Const HWND_NOTOPMOST = -2

Public Const WM_SETREDRAW = &HB

' Windows API calls for creating Topmost window
Declare Function SetWindowPos Lib "User32" (ByVal h&, ByVal hb&, ByVal x&, ByVal y&, ByVal cx&, ByVal cy&, ByVal f&) As Long
Declare Function FindWindow Lib "User32" (ByVal lpClassName As Any, ByVal lpWindowName As Any) As Long
Declare Function SendMessage Lib "User32" (ByVal hWnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any) As Long

Declare Function SetParent Lib "User32" (ByVal hWndChild As Long, ByVal hWndNewParent As Long) As Long

Type BookAuthor
    ID As Integer
    Name As String * 25
End Type

Dim arDemos(20) As String

Public Function SetRedraw(hWnd As Long, lState As Long) As Long
  
  Dim lRetVal As Long
  
  On Error GoTo SetRedraw_Error
  ' Set iState = 0 to Disable automatic Updating
  ' Set iState = 1 to Enable automatic updating
  lRetVal = SendMessage(hWnd, WM_SETREDRAW, lState, 0)
  If lRetVal <> False Then GoTo SetRedraw_Error
  
  SetRedraw = True
  Exit Function
  
SetRedraw_Error:
  SetRedraw = lRetVal
  Exit Function
  
End Function

Public Function SetTopWindow(hWnd As Long, bState As Boolean) As Boolean
      
  If bState = True Then 'Put the window on top
    SetTopWindow = SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, FLAGS)
  ElseIf bState = False Then ' Turn off the TopMost flag
    SetTopWindow = SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, FLAGS)
  Else
    Debug.Print "bState Unknown."
    SetTopWindow = False
  End If
  
End Function

' Centers the Form object passed to it.
Sub CenterMe(frmForm As Form)
  frmForm.Left = (Screen.Width - frmForm.Width) / 2
  frmForm.Top = (Screen.Height - frmForm.Height) / 2
End Sub

Sub Main()
  Dim iCount As Integer
  Dim iLimit As Integer
  Dim tvwSample As TreeView
  Dim nodRoot As Node
  Dim nodX As Node
  Dim sLastParent As String
  Dim ilImages As ListImages
  Dim iNextParentImage As Integer
  
  Set tvwSample = frmExplore.tvExample
  Set ilImages = frmExplore.ilExplore.ListImages
  
  frmExplore.Show
  
  ' Display Speed Categories
  arDemos(0) = "Algorithms"
  arDemos(1) = "Display Speed"
  arDemos(2) = "Paint Picture"
  arDemos(3) = "Display Speed"
  arDemos(4) = "Graphics"
  arDemos(5) = "Display Speed"
  
  ' Real Speed Categories
  arDemos(6) = "String Manipulation"
  arDemos(7) = "Real Speed"
  arDemos(8) = "Code Optimizations"
  arDemos(9) = "Real Speed"
  arDemos(10) = "Numeric Types"
  arDemos(11) = "Real Speed"
  
  ' Resource Usage Categories
  arDemos(12) = "Picture vs. Image"
  arDemos(13) = "Resource Usage"
  arDemos(14) = "Destroy Form Objects"
  arDemos(15) = "Resource Usage"
  
  ' Apparent Speed Categories
  arDemos(16) = "Splash Screen"
  arDemos(17) = "Apparent Speed"
    
  ' Collection Categories
  arDemos(18) = "Collections"
  arDemos(19) = "Collections"
  
  Set nodRoot = tvwSample.Nodes.Add(, , "Root", "Optimizing Samples")
  nodRoot.Image = 1
  iNextParentImage = 2
  'Set up the first Demo Type
  'Create a new Demo Type off the Root and Make this node a child of it.
  iCount = 0
  sLastParent = arDemos(iCount + 1)
  Set nodX = tvwSample.Nodes.Add("Root", tvwChild, sLastParent, arDemos(iCount + 1))
  nodX.Image = iNextParentImage  'Display Speed
  iNextParentImage = iNextParentImage + 1
  'Make this node a child of the last Demo Type
  Set nodX = tvwSample.Nodes.Add(sLastParent, tvwChild, , arDemos(iCount))
  nodX.Image = 7
  iLimit = UBound(arDemos)
  For iCount = 2 To iLimit - 1 Step 2
    If arDemos(iCount + 1) <> arDemos(iCount - 1) Then
      nodX.EnsureVisible  ' Make sure all children are visible in the last group
      'Create a new Demo Type off the Root and Make this node a child of it.
      sLastParent = arDemos(iCount + 1)
      Set nodX = tvwSample.Nodes.Add("Root", tvwChild, sLastParent, arDemos(iCount + 1))
      nodX.Image = iNextParentImage
      iNextParentImage = iNextParentImage + 1
    End If
    'Make this node a child of the last Demo Type
    Set nodX = tvwSample.Nodes.Add(sLastParent, tvwChild, , arDemos(iCount))
    nodX.Image = 7
  Next iCount
    
  nodX.EnsureVisible
  
  ' Destroy all of the objects we have created.
  Set nodX = Nothing
  Set nodRoot = Nothing
  Set ilImages = Nothing
  Set tvwSample = Nothing

End Sub

Sub PosForm(frmForm As Form)
  frmForm.Left = frmExplore.Left + frmExplore.Width
  frmForm.Top = frmExplore.Top
End Sub


