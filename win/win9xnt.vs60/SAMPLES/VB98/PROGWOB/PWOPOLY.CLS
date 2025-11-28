VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Polygon"
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

' Polygon is a VERY rudimentary class of
' -------       polygon objects.  It only
'   allows polygons to be created (with
'   the SetPoints method); there's no way
'   to modify the points once they're set.
'   (Of course, you can call SetPoints
'   again, to reset the entire polygon.)

Private Type POLYPOINT  ' prefix pyp
    X As Single
    Y As Single
    Angle As Single
End Type

' Polygon defaults to one point, at (0,0).
Private mapyp() As POLYPOINT

' Storage for read-only Normalized property.
Private mblnNormalized As Boolean

' Storage for Color property.
Private mrgbColor As Long

' IShape is the interface that's used to
' ------    display the polygon.  It also
'   has a TimeTest method that's used to
'   compare early and late binding call
'   overhead.
Implements IShape

' -------------------------------------
' This marks the beginning of the
'   implementation of the IShape
'   interface.

' IShape.DrawToPictureBox is called to
' ====== ----------------   draw a shape,
'   so each class of shape must supply
'   its own implementation.
'
Private Sub IShape_DrawToPictureBox(ByVal pb As PictureBox)
    Dim sngXLast As Single
    Dim sngYLast As Single
    Dim sngX As Single
    Dim sngY As Single
    Dim intMax As Integer
    Dim intCt As Integer
    
    intMax = UBound(mapyp)
    sngX = mapyp(intMax).X
    sngY = mapyp(intMax).Y
    If intMax = 0 Then
        pb.PSet (sngX, sngY), mrgbColor
    Else
        For intCt = 0 To intMax
            sngXLast = sngX
            sngYLast = sngY
            sngX = mapyp(intCt).X
            sngY = mapyp(intCt).Y
            pb.Line (sngXLast, sngYLast)-(sngX, sngY), mrgbColor
        Next
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

' --------------------------------------
' This is the beginning of the Polygon
'   class's default interface (Public
'   properties and methods).  This is
'   the Polygon interface that Triangle
'   and Rectangle implement.

' Color property.
' -----
'
Public Property Get Color() As Long
    Color = mrgbColor
End Property
'
Public Property Let Color(ByVal rgb As Long)
    If 0 <> (rgb And &HFF000000) Then
        Err.Raise vbObjectError + 2080, , _
            "Invalid color value for Polygon."
        Exit Property
    End If
    mrgbColor = rgb
End Property

' TimeTest method takes no arguments,
' --------      does nothing, and
'   immediately returns.  It's used to
'   illustrate the call overhead for
'   late binding, as opposed to the
'   early binding provided by calling
'   TimeTest on the IShape interface.
'
' You might think we would make TimeTest
'   a Friend property, like DebugID, to
'   save Triangle and Rectangle -- which
'   implement Polygon's interface -- from
'   having to implement Polygon_TimeTest.
'   (Friend properties and methods are
'   NOT part of a class's interface.)
'   The reason we can't do this is that
'   TimeTest must be called LATE bound
'   for the demo -- but Friend properties
'   and methods must always be called
'   EARLY bound.
Public Sub TimeTest()
End Sub

' GetPoint sets two ByRef Singles to
' --------      the X and Y values for
'   the requested point.  (If Polygon's
'   interface wasn't being implemented
'   by Triangle and Rectangle, GetPoint
'   could be declared Friend, and could
'   return a POLYPOINT -- which would
'   have to be declared Public in a
'   standard module in that case;
'   however, Friend members are not part
'   of a class's interface, so making
'   GetPoint a Friend would prevent
'   Triangle and Rectangle from
'   implementing an early-bound
'   Polygon_GetPoint.)
'
Public Sub GetPoint(ByVal intPoint As Integer, _
        ByRef X As Single, ByRef Y As Single)
    X = mapyp(intPoint).X
    Y = mapyp(intPoint).Y
End Sub

' GetPointCount returns the number of
' -------------     points in the Polygon.
'
Public Property Get GetPointCount() As Integer
    GetPointCount = UBound(mapyp) + 1
End Property

' SetPoints accepts a zero-based array
' ---------     of Singles, the even-numbered
'   elements (0, 2, etc.) being the X
'   values, and the odd-numbered elements
'   being the Y values of the points.
'
Public Sub SetPoints(asngPoints() As Single)
    Dim blnBadArray As Boolean
    Dim intMax As Integer
    Dim intPoint As Integer
    
    On Error Resume Next
    If LBound(asngPoints) <> 0 Then blnBadArray = True
    intMax = UBound(asngPoints)
    ' The upper bound of a zero-based
    '   array must be an odd number --
    '   validate this.
    If (intMax / 2#) = (intMax \ 2) Then blnBadArray = True
    ' If an error occurs in the UBound
    '   function, declare array invalid.
    If Err.Number <> 0 Then blnBadArray = True
    If blnBadArray Then
        Err.Raise vbObjectError + 2081, , _
            "SetPoints must receive a zero-based, one-dimensional array with an even number of elements, the odd entries being X values and the even entries Y values."
        Exit Sub
    End If
    ' Convert the maximum index of the input
    '   array to the maximum index of the
    '   internal array of the Polygon.
    intMax = intMax \ 2
    ReDim mapyp(0 To intMax)
    ' Read in the point values.
    For intPoint = 0 To intMax
        mapyp(intPoint).X = asngPoints(intPoint * 2)
        mapyp(intPoint).Y = asngPoints(intPoint * 2 + 1)
    Next
End Sub

' --------------------------------------
' This is the beginning of the Polygon's
'   private procedures (helper procedures
'   and event procedures).

Private Sub Class_Initialize()
    ' Debug code.
    mlngDebugID = DebugInit(Me)
    '
    ' Polygon defaults to a point.
    ReDim mapyp(0 To 0)
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

