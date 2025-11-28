VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Employee"
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

' Properties of the Employee class.
Public Name As String
Public Salary As Double

' Private data for the write-once ID property.
Private mstrID As String

Property Get ID() As String
    ID = mstrID
End Property
'
' The first time the ID property is set, the static
'   Boolean is also set.  Subsequent calls do nothing.
'   (It would be better to raise an error, instead.)
Property Let ID(strNew As String)
    Static blnAlreadySet As Boolean
    If Not blnAlreadySet Then
        blnAlreadySet = True
        mstrID = strNew
    End If
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


