VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "SmallBusiness1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit
' >> Best viewed in Full Module view. <<
'
' Storage for debug ID number.
Private mlngDebugID As Long
Implements IDebug

' Public Collection object is easy to
'   declare, but it leads to trouble --
'   as shown in frmStraw.
Public Employees As New Collection

Property Get NewEmployeeID() As String
    Static intEmpNum As Integer
    intEmpNum = intEmpNum + 1
    NewEmployeeID = "E" & Format$(intEmpNum, "00000")
End Property

Private Sub Class_Initialize()
    mlngDebugID = DebugInit(Me)
End Sub

Private Sub Class_Terminate()
    DebugTerm Me
End Sub

' -------- IDebug Implementation --------
'
' IDebug.DebugID gives you a way to tell
' ====== -------    objects apart.  It's
'   required by the DebugInit, DebugTerm,
'   and DebugShow debugging procedures
'   declared in modFriend.
'
Private Property Get IDebug_DebugID() As Long
    IDebug_DebugID = mlngDebugID
End Property


