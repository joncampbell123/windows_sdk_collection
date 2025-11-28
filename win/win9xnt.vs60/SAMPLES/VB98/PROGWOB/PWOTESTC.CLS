VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "TestClass"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit
' >> Best viewed in Full Module view. <<
'
' Storage for debug ID number.
Private mlngDebugID As Long
Implements IDebug

' Private storage for the user-defined
'   type (UDT) that gets passed between
'   objects.  The UDT must be declared
'   Public in a standard module (see
'   modFriends).  (In this example, mDemo
'   serves as the storage for both the
'   Demo property and the GetDemo/SetDemo
'   Function/Sub pair.)
Private mDemo As udtDEMO

' Demo property:  The Property Get and Let
' -------------     for the Demo property
'   show how property procedures declared
'   with the Friend keyword can accept and
'   return a UDT.
Friend Property Get Demo() As udtDEMO
    Demo = mDemo
End Property
' Note that the argument of the Property
'   Let must be ByRef for a UDT.
Friend Property Let Demo(NewDemo As udtDEMO)
    mDemo = NewDemo
End Property

' GetDemo and SetDemo show how Friend
' -------------------   functions can
'   return UDTs, and how UDTs can be
'   arguments of Friend procedures.
Friend Function GetDemo() As udtDEMO
    GetDemo = mDemo
End Function
' Note that a UDT argument for a procedure
'   must be declared ByRef.
Friend Sub SetDemo(NewDemo As udtDEMO)
    mDemo = NewDemo
End Sub

' SetDemoParts is a helper method that
' ------------      fills in the elements
'   of the UDT, so there's something to
'   pass.  In a real program, the elements
'   of the UDT might be filled in by code
'   in the TestClass object.
Public Sub SetDemoParts(ByVal A As Integer, _
        ByVal B As Long, ByVal C As String)
    mDemo.intA = A
    mDemo.lngB = B
    mDemo.strC = C
End Sub

' ShowDemo displays the elements of the
' --------      UDT, so they can be viewed
'   after the UDT is passed to the second
'   object.
Public Sub ShowDemo(ByVal Title As String, _
        ByVal Direction As String)
        
    MsgBox Direction & "  " _
        & GetDebugString(Me) & vbCrLf & vbCrLf _
        & mDemo.intA & vbCrLf _
        & mDemo.lngB & vbCrLf _
        & mDemo.strC, , Title
End Sub

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

