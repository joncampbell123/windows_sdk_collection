VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "CoffeeTracker"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

' CoffeeTracker is a small private object
' =============     for keeping track of
'   a task a Coffee object is executing.
'   The reason for using an object is that
'   WithEvents variables cannot be in
'   arrays.  In order to have a varying
'   number of them, you have to have a
'   class of objects to keep the WithEvents
'   variables.
'
' Set the ThreadID and Size before you start
'   the long task; set the Coffee property
'   JUST before you call StartLongTask.  ID
'   is assigned by the NewTracker procedure
'   in frmThread; it's the object's index
'   in the CoffeeTrackers Collection object.

Public ThreadID As Long
Public Size As Long
Public ID As String

' Storage for the Coffee object being tracked.
Private WithEvents mwCoffee As Coffee
Attribute mwCoffee.VB_VarHelpID = -1
'
' Start time (from timeGetTimer API).
Private mlngStart As Long

Public Property Get Coffee() As Coffee
    Set Coffee = mwCoffee
End Property
Public Property Set Coffee(ByVal NewValue As Coffee)
    ' Save the start time.
    mlngStart = timeGetTime
    Set mwCoffee = NewValue
End Property

' The Coffee object raises a Complete
'   event when the task being tracked
'   is complete.  CoffeeTracker puts
'   information about the task (thread
'   ID, size, and seconds per iteration)
'   into a list box on frmThread.
'
Private Sub mwCoffee_Complete(ByVal Canceled As Boolean)
    Dim lngEnd As Long
    Dim dblElapsed As Double
    
    lngEnd = timeGetTime
    '
    ' Free the Coffee object.
    Set mwCoffee = Nothing
    '
    ' Add a report line to the list box.
    If Canceled Then
        frmThread.lstResults.AddItem ThreadID _
            & " (" & Size & ") canceled", 0
    Else
        frmThread.lstResults.AddItem ThreadID _
            & " (" & Size & ") " _
            & (CDbl(lngEnd) - mlngStart) / Size / 1000# _
            & " sec/iteration", 0
    End If
    '
    ' CoffeeTracker removes its reference
    '   from the collection, leaving itself
    '   without references -- so that it
    '   can terminate.
    frmThread.CoffeeTrackers.Remove ID
End Sub

' For long tasks, Coffee events raise
'   a Progress event for every 10% of the
'   task it completes.  CoffeeTracker adds
'   an entry to the list box on frmThread.
'
Private Sub mwCoffee_Progress(ByVal PercentDone As Single, Cancel As Boolean)
    frmThread.lstResults.AddItem ThreadID _
        & " (" & Size & ") " _
        & Format$(PercentDone * 100, "#0.0") & "%", 0
    '
    ' As each CoffeeTracker notices the
    '   global Cancel flag, it turns off
    '   the long task it's been watching.
    '   The Coffee object will then raise
    '   a Complete event (see above).
    If frmThread.CancelAll Then Cancel = True
End Sub

