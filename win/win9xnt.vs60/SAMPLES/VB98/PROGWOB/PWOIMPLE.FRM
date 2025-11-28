VERSION 5.00
Begin VB.Form frmImplements 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Polymorphism and the Implements keyword"
   ClientHeight    =   3510
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5310
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   2  'Custom
   Picture         =   "PWOImple.frx":0000
   ScaleHeight     =   3510
   ScaleWidth      =   5310
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer tmrDisplay 
      Interval        =   220
      Left            =   1800
      Top             =   2760
   End
   Begin VB.PictureBox picShapes 
      BackColor       =   &H00FFFFFF&
      Height          =   1095
      Left            =   120
      Picture         =   "PWOImple.frx":0446
      ScaleHeight     =   1035
      ScaleWidth      =   1155
      TabIndex        =   2
      Top             =   2280
      Width           =   1215
   End
   Begin VB.CommandButton cmdLate 
      Caption         =   "&Late Bound"
      Height          =   375
      Left            =   3600
      TabIndex        =   1
      Top             =   3000
      Width           =   1575
   End
   Begin VB.CommandButton cmdEarly 
      Caption         =   "&Early Bound"
      Height          =   375
      Left            =   3600
      TabIndex        =   0
      Top             =   2520
      Width           =   1575
   End
   Begin VB.Label lblLateResult 
      Height          =   255
      Left            =   1560
      TabIndex        =   6
      Top             =   3120
      Width           =   1935
   End
   Begin VB.Label lblEarlyResult 
      Height          =   255
      Left            =   1560
      TabIndex        =   5
      Top             =   2640
      Width           =   1935
   End
   Begin VB.Label Label2 
      Caption         =   "Method Call Overhead"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1440
      TabIndex        =   4
      Top             =   2160
      Width           =   3855
   End
   Begin VB.Label Label1 
      Caption         =   $"PWOImple.frx":088C
      Height          =   2055
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   5055
   End
End
Attribute VB_Name = "frmImplements"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Const NUMOBJECTS = 100
Const NUMREPEATSEARLY As Long = 10000
Const NUMREPEATSLATE As Long = 500

' For demo purposes, three arrays of object
'   references are kept.  Each object the
'   demo creates has an entry in all three
'   arrays:
' Array of IShape interfaces;
Private maishEarly(1 To NUMOBJECTS) As IShape
' Array of Polygon interfaces;
Private mapyg(1 To NUMOBJECTS) As Polygon
' Array of default interfaces (Polygon,
'   Triangle, or Rectangle, depending on
'   the object).
Private maobjLate(1 To NUMOBJECTS) As Object

' Time test for early binding calls each
'   of NUMOBJECTS objects early bound,
'   using the IShape interface.  (The
'   TimeTest method is all overhead --
'   it takes no arguments, and immediately
'   returns.)  This is repeated
'   NUMREPEATSEARLY times.
'
Private Sub cmdEarly_Click()
    Dim lngCt As Long
    Dim intCt As Integer
    Dim timeMark As Long
    
    ' Disable the display of shapes during
    '   the test.
    tmrDisplay.Enabled = False
    cmdEarly.Caption = "Working..."
    timeMark = timeGetTime
    For lngCt = 1 To NUMREPEATSEARLY
        For intCt = 1 To NUMOBJECTS
            ' Make the calls to TimeTest
            '   through the IShape interface,
            '   which all three classes
            '   (Polygon, Rectangle, and
            '   Triangle) implement.
            maishEarly(intCt).TimeTest
        Next
    Next
    timeMark = timeGetTime - timeMark
    lblEarlyResult = ShowElapsed(timeMark, _
        NUMOBJECTS * NUMREPEATSEARLY, _
        "Early Bound Call Overhead")
    cmdEarly.Caption = "&Early Bound"
    '
    ' Start displaying shapes again.
    tmrDisplay.Enabled = True
End Sub

Private Sub cmdLate_Click()
    Dim lngCt As Long
    Dim intCt As Integer
    Dim timeMark As Long
    
    ' Disable the display of shapes during
    '   the test.
    tmrDisplay.Enabled = False
    cmdLate.Caption = "Working..."
    timeMark = timeGetTime
    For lngCt = 1 To NUMREPEATSLATE
        For intCt = 1 To NUMOBJECTS
            ' Make the calls to TimeTest
            '   late-bound, through the default
            '   interfaces of the objects
            '   (Polygon, Rectangle, and
            '   Triangle).
            maobjLate(intCt).TimeTest
        Next
    Next
    timeMark = timeGetTime - timeMark
    lblLateResult = ShowElapsed(timeMark, _
        NUMOBJECTS * NUMREPEATSLATE, _
        "Late Bound Call Overhead")
    cmdLate.Caption = "&Late Bound"
    '
    ' Start displaying shapes again.
    tmrDisplay.Enabled = True
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' Free resources associated with the
    '   form, by clearing the hidden
    '   global variable.
    Set frmImplements = Nothing
End Sub

' If the shape-display picture box is
'   clicked, display all the shapes at
'   once (using early binding).
Private Sub picShapes_Click()
    Dim intCt As Integer
    For intCt = 1 To NUMOBJECTS
        Call maishEarly(intCt).DrawToPictureBox(picShapes)
    Next
End Sub

Private Sub Form_Load()
    Dim intCt As Integer
    Dim asngPoints() As Single
    Set Picture = New StdPicture
    Call Randomize(Timer)
    For intCt = 1 To NUMOBJECTS
        ' Randomly create 1/3 Polygons,
        '   1/3 Triangles, and 1/3 Rectangles.
        '   Store the reference to each of
        '   these objects in the late-bound
        '   array.
        Select Case Int(Rnd * 3)
            Case 0
                Set maobjLate(intCt) = New Polygon
            Case 1
                Set maobjLate(intCt) = New Triangle
            Case 2
                Set maobjLate(intCt) = New Rectangle
        End Select
        ' Save a reference to the object's
        '   IShape interface, to demonstrate
        '   early binding using polymorphism.
        '   Each of the three classes
        '   implements IShape, so Visual
        '   Basic is able to query for the
        '   IShape interface and make the
        '   assignment.
        Set maishEarly(intCt) = maobjLate(intCt)
        ' Save a reference to the object's
        '   Polygon interface, as well.
        Set mapyg(intCt) = maobjLate(intCt)
        ' If the object was a Polygon (rather
        '   than a Triangle or Rectangle,
        '   which simply implement the
        '   Polygon interface), it will have
        '   only one point.  Give it a
        '   random number of points (from
        '   four to 24).
        If mapyg(intCt).GetPointCount = 1 Then
            ReDim asngPoints(0 To (Int(21 * Rnd) + 4) * 2 - 1)
            Call mapyg(intCt).SetPoints(asngPoints)
        End If
        ' Assign the object a random color.
        mapyg(intCt).Color = Int(Rnd * &HFFFFFF)
    Next
    Debug.Print "If you go back and look at the debug numbers"
    Debug.Print "of the shape objects, you'll notice that "
    Debug.Print "a lot more than " & NUMOBJECTS & " objects were created."
    Debug.Print "This is because each Triangle and each"
    Debug.Print "Rectangle creates an inner Polygon object."
    '
    ' Assign random values to each
    '   point in each object.
    Call NewShapes
End Sub

Private Sub tmrDisplay_Timer()
    ' Iterate repeatedly through the
    '   shape objects, displaying
    '   them in a PictureBox.
    Static intCt As Integer
    picShapes.Cls
    intCt = intCt + 1
    If intCt > NUMOBJECTS Then intCt = 1
    Call maishEarly(intCt).DrawToPictureBox(picShapes)
End Sub

' ShowElapsed helper procedure displays
' -----------       the result of an early
'   or late-bound time test.
'
Private Function ShowElapsed(ByVal Milliseconds As Long, _
        ByVal Iterations As Long, _
        ByVal Caption As String) As String
    Dim strMSec As String
    strMSec = Format$(Milliseconds / Iterations, "0.0000")
    
    MsgBox Format$(Iterations, "#,###,##0") _
        & " iterations in " _
        & Format$(Milliseconds / 1000#, "##,##0.00") _
        & " seconds" & vbCrLf _
        & strMSec _
        & " milliseconds per call", , Caption
        
    ShowElapsed = strMSec & " msec/call"
End Function

' NewShapes changes the shape of each of
' ---------     the objects, calling
'   MakeRandomPoints to generate a set
'   of random points.  It does not change
'   the number of points in a Polygon.
'
Private Sub NewShapes()
    Dim intCt As Integer
    Dim intPt As Integer
    Dim pyg As Polygon
    Dim asngPoints() As Single
    Dim intNumPts As Integer
    
    For intCt = 1 To NUMOBJECTS
        intNumPts = mapyg(intCt).GetPointCount
        Call MakeRandomPoints(intNumPts, asngPoints)
        Call mapyg(intCt).SetPoints(asngPoints)
    Next
End Sub

' MakeRandomPoints creates a set of random
' ----------------      points for a
'   Polygon object, placing them in the
'   zero-based, one-dimensional array the
'   SetPoints method requires.
'
Private Sub MakeRandomPoints( _
        ByVal intNumPts As Integer, _
        asngPoints() As Single)
        
    Dim intPt As Integer
    
    ReDim asngPoints(0 To intNumPts * 2 - 1)
    For intPt = 0 To intNumPts * 2 - 1 Step 2
        asngPoints(intPt) = Rnd * picShapes.ScaleWidth
        asngPoints(intPt + 1) = Rnd * picShapes.ScaleHeight
    Next
End Sub

