VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Rectangle"
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

' The Rectangle implements two interfaces;
'   it also has one property (Filled) and
'   one method (TimeTest) on its own
'   interface.

Implements IShape
Implements Polygon

' The inner Polygon object actually holds
'   the data and does some of the work.
'   The Rectangle keeps references to both
'   the Polygon interface and the IShape
'   interface of the inner Polygon.
Private mpyg As Polygon
Private mish As IShape

' Storage for Color property (Polygon
'   interface implementation).
Private mrgbColor As Long

' Storage for the Filled property (on the
'   Rectangle object's default interface).
Private mblnFilled As Boolean

' -------------------------------------
' This is the beginning of Rectangle's
'   implementation of the IShape
'   interface.

' IShape.DrawToPictureBox is called to
' ------ ----------------   draw a shape,
'   so each class of shape must supply
'   its own implementation.
'
Private Sub IShape_DrawToPictureBox(ByVal pb As PictureBox)
    ' Instead of delegating to the IShape
    '   interface of the inner Polygon, the
    '   Rectangle takes advantage of the
    '   fact that there's a graphics command
    '   to draw a box in one operation
    '   instead of four (graphics being
    '   presumably the most time-consuming
    '   kind of operation).
    Dim sngX1 As Single, sngY1 As Single
    Dim sngX2 As Single, sngY2 As Single
    Call mpyg.GetPoint(0, sngX1, sngY1)
    Call mpyg.GetPoint(1, sngX2, sngY2)
    If mblnFilled Then
        pb.Line (sngX1, sngY1)-(sngX2, sngY2), mrgbColor, BF
    Else
        pb.Line (sngX1, sngY1)-(sngX2, sngY2), mrgbColor, B
    End If
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
' This is the beginning of the implementation
'   of the Polygon interface.

' Polygon.Color - Because the inner
' ======= -----     Polygon isn't used
'   for drawing the Rectangle, the Color
'   property is completely overridden.
'   This is not required; the Rectangle
'   could delegate storage of the Color
'   property to the inner Polygon (as
'   the Triangle class does), saving
'   the implementation code and storage
'   space.  The color could then be
'   retrieved from the inner Polygon
'   when drawing is done.
Private Property Get Polygon_Color() As Long
    Polygon_Color = mrgbColor
End Property
'
Private Property Let Polygon_Color(ByVal RHS As Long)
    If 0 <> (RHS And &HFF000000) Then
        Err.Raise vbObjectError + 2080, , _
            "Invalid color value for Polygon."
        Exit Property
    End If
    mrgbColor = RHS
End Property

' Polygon.TimeTest - Rectangle has three
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
    ' Delegate to the inner Polygon.
    Call mpyg.GetPoint(intPoint, X, Y)
End Sub

' Polygon.GetPointCount
' ======= -------------
'
Private Property Get Polygon_GetPointCount() As Integer
    ' There's no point in delegating to
    '   the inner Polygon, because the
    '   Rectangle is always specified by
    '   just two points.
    Polygon_GetPointCount = 2
End Property

' Polygon.SetPoints - When implementing the
' ======= ---------     SetPoints method
'   of the Polygon interface, the Rectangle
'   executes its own code to verify that
'   the input array contains only two
'   points (four Singles), and then
'   delegates to the inner Polygon
'   object.
Private Sub Polygon_SetPoints(asngPoints() As Single)
    Dim blnBadArray As Boolean
    On Error Resume Next
    ' Ensure that the input array contains
    '   no more than four points.  (The
    '   Polygon's SetPoint method will
    '   verify that the array is zero-
    '   based.)
    If UBound(asngPoints) <> 3 Then blnBadArray = True
    ' If an error occurred calling UBound,
    '   reject the array.
    If Err.Number <> 0 Then blnBadArray = True
    If blnBadArray Then
        Err.Raise vbObjectError + 2083, , _
            "A rectangle is specified by an array of four numbers (left, top, right, bottom) in a zero-based array."
        Exit Sub
    End If
    ' Delegate to the inner Polygon, which
    '   completes validation of the array
    '   and stores it.
    Call mpyg.SetPoints(asngPoints)
End Sub

' --------------------------------------
' This is the beginning of the Rectangle
'   object's own (default) interface.

' TimeTest method takes no
' --------   arguments, and
'   immediately returns.  It's used to
'   illustrate late binding, as opposed
'   to the early binding provided by
'   calling TimeTest on the IShape
'   interface.
Public Sub TimeTest()
End Sub

' Filled property determines whether a
' ------    rectangle is filled when
'   drawn.
Public Property Get Filled() As Boolean
    Filled = mblnFilled
End Property
'
Public Property Let Filled(ByVal NewValue As Boolean)
    mblnFilled = NewValue
End Property

' --------------------------------------
' This is the beginning of the class's
'   private procedures (helper procedures
'   and event procedures).

Private Sub Class_Initialize()
    Dim asngPoints(0 To 3) As Single
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

