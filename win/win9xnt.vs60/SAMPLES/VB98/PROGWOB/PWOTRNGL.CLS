VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Triangle"
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

' The Triangle class implements the IShape
'   interface and the Polygon interface.
'   An inner Polygon object is maintained,
'   and many activities are delegated
'   (drawing, storage of points, and so
'   on).  The Triangle class's own
'   interface has only one method.

Implements IShape
Implements Polygon

' The inner Polygon object actually holds
'   the data and does most of the work.
'   The Triangle keeps references to both
'   the Polygon interface and the IShape
'   interface of the inner Polygon.
Private mpyg As Polygon
Private mish As IShape

' -------------------------------------
' This marks the beginning of the
'   implementation of the IShape interface.

' IShape.DrawToPictureBox is called to
' ------ ----------------   draw a shape,
'   so each class of shape must supply
'   its own implementation.
'
Private Sub IShape_DrawToPictureBox(ByVal pb As PictureBox)
    ' Delegate to the IShape interface of
    '   the inner Polygon.
    Call mish.DrawToPictureBox(pb)
End Sub

' IShape.TimeTest method is used to show
' ====== --------       the reduced call
'   overhead of a method called on an
'   interface that several classes
'   implement -- as opposed to calling
'   a similar method on the classes'
'   default interfaces.
'
Private Sub IShape_TimeTest()
End Sub

' -------------------------------------
' This marks the beginning of the
'   implementation of the Polygon
'   interface.

' Polygon.Color delegates to the inner
' ======= -----     Polygon object.
'
Private Property Get Polygon_Color() As Long
    Polygon_Color = mpyg.Color
End Property
'
Private Property Let Polygon_Color(ByVal RHS As Long)
    mpyg.Color = RHS
End Property

' Polygon.TimeTest - Triangle has three
' ======= --------  TimeTest methods, one
'   on the IShape interface (used to show
'   polymorphism and early binding), one on
'   its own interface (used to show late
'   binding), and this one.  This one is
'   a side effect of the fact that Rectangle
'   implements the Polygon interface; it's
'   not used for anything.
Private Sub Polygon_TimeTest()
End Sub

' Polygon.GetPoint
' ======= --------
'
Private Sub Polygon_GetPoint(ByVal intPoint As Integer, X As Single, Y As Single)
    ' Delegate to inner Polygon.
    Call mpyg.GetPoint(intPoint, X, Y)
End Sub

' Polygon.GetPointCount
' ======= -------------
'
Private Property Get Polygon_GetPointCount() As Integer
    ' There's no point in delegating to
    '   the inner Polygon -- a triangle
    '   always has three points.
    Polygon_GetPointCount = 3
End Property

' Polygon.SetPoints - When implementing the
' ======= ---------     SetPoints method
'   of the Polygon interface, the Triangle
'   executes its own code to verify that
'   the input array contains only three
'   points (six Singles), and then
'   delegates to the inner Polygon
'   object.
Private Sub Polygon_SetPoints(asngPoints() As Single)
    Dim blnBadArray As Boolean
    On Error Resume Next
    If UBound(asngPoints) <> 5 Then blnBadArray = True
    If Err.Number <> 0 Then blnBadArray = True
    If blnBadArray Then
        Err.Raise vbObjectError + 2082, , _
            "A triangle is specified by a zero-based array with six values, even values (beginning with 0) being the x-values and odd being the y-values."
        Exit Sub
    End If
    ' Delegate to the inner Polygon, which
    '   completes validation of the array
    '   and stores it.
    Call mpyg.SetPoints(asngPoints)
End Sub

' --------------------------------------
' This is the beginning of the Triangle's
'   own (default) interface.

' TimeTest method takes no arguments,
' --------      does nothing, and
'   immediately returns.  It's used to
'   illustrate late binding, as opposed
'   to the early binding provided by
'   calling TimeTest on the IShape
'   interface.
Public Sub TimeTest()
End Sub

' --------------------------------------
' This is the beginning of the Triangle's
'   private procedures (helper functions
'   and event procedures).

Private Sub Class_Initialize()
    Dim asngPoints(0 To 5) As Single
    ' Debug code.
    mlngDebugID = DebugInit(Me)
    '
    ' Create the inner Polygon object, and
    '   get a reference to its IShape
    '   interface.
    Set mpyg = New Polygon
    Set mish = mpyg
    ' Initialize the inner Polygon.
    Call mpyg.SetPoints(asngPoints)
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


