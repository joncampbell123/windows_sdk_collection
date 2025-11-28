VERSION 5.00
Begin VB.Form frmThread 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Multithreading Demo"
   ClientHeight    =   4950
   ClientLeft      =   4140
   ClientTop       =   1470
   ClientWidth     =   7095
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4950
   ScaleWidth      =   7095
   Begin VB.Timer tmrShort 
      Enabled         =   0   'False
      Interval        =   1000
      Left            =   2280
      Top             =   3840
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "&Cancel Long Tasks"
      Height          =   375
      Left            =   240
      TabIndex        =   8
      Top             =   4440
      Width           =   2895
   End
   Begin VB.TextBox txtN 
      Height          =   375
      Left            =   2760
      MaxLength       =   1
      TabIndex        =   6
      Text            =   "4"
      Top             =   2160
      Width           =   375
   End
   Begin VB.CommandButton cmdNLong 
      Caption         =   "...with &N LongTasks"
      Height          =   375
      Left            =   240
      TabIndex        =   5
      Top             =   2160
      Width           =   2415
   End
   Begin VB.CommandButton cmdShortLong 
      Caption         =   "...with &Long Task"
      Height          =   375
      Left            =   240
      TabIndex        =   4
      Top             =   1680
      Width           =   2895
   End
   Begin VB.CommandButton cmdShortOnly 
      Caption         =   "&Short Tasks, Serialized..."
      Height          =   375
      Left            =   240
      TabIndex        =   3
      Top             =   1200
      Width           =   2895
   End
   Begin VB.CommandButton cmdIDs 
      Caption         =   "List Thread &IDs"
      Height          =   375
      Left            =   240
      TabIndex        =   2
      Top             =   720
      Width           =   2895
   End
   Begin VB.ListBox lstResults 
      Height          =   4575
      Left            =   3360
      TabIndex        =   1
      Top             =   120
      Width           =   3615
   End
   Begin VB.CommandButton cmdXThread 
      Caption         =   "Cross-Thread &Overhead"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   2895
   End
   Begin VB.Label Label1 
      Caption         =   "Your experiments here..."
      Height          =   255
      Left            =   240
      TabIndex        =   7
      Top             =   2760
      Width           =   2895
   End
End
Attribute VB_Name = "frmThread"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
' The multithreading demo:
'   - Shows thread ids and number of objects
'       on a thread (this will be more
'       interesting if you compile MTCoffee.exe
'       with a thread pool size of 3 or 4).
'   - Compares call overhead for same-thread
'       vs. cross-thread calls.
'   - Time per iteration for serial short
'       tasks (see Coffee object defined in
'       MTCoffee.cls).
'   - Times per iteration for a long task, and
'       for serial short tasks run at the same
'       time.
'   - Times per iteration for a number of long
'       tasks, with serial short tasks running
'       at the same time.
'
' You can add your own tests to this framework.
'   You may find it interesting to compare the
'   behavior of tasks that block (such as
'   database queries on remote computers, or
'   large file transfers).  On a computer with
'   a single processor, such tasks will behave
'   much better than the computation-intensive
'   tasks used in the examples above.  Threads
'   that perform computation-intensive tasks
'   compete with each other for the machine's
'   single processor, and so their performance
'   suffers as the number of active threads
'   increases.
'
' For more information, see "Scalability and
'   Multithreading," in "Building Code
'   Components" in Books Online.
 
' These constants control the relative size of
'   a long task and a short task.  You may need
'   to adjust these for the speed of your
'   processor.
Const SHORTTASKSIZE = 50000
Const LONGTASKSIZE = 2000000

' Array of Coffee objects.
Private macfe(1 To 20) As Coffee

' Collection of CoffeeTracker objects.
Public CoffeeTrackers As New Collection

' Cancel flag.
Public CancelAll As Boolean

' How many short tasks to run.
Private mintHowManyShort As Integer

Private Sub cmdCancel_Click()
    ' When the user clicks Cancel, set a
    '   flag that CoffeeTracker can use
    '   to cancel all long tasks when they
    '   raise their next Progress event.
    CancelAll = True
End Sub

' Run a few short tasks serially, to get a
'   feel for their speed when not competing
'   for the processor.
Private Sub cmdShortOnly_Click()
    lstResults.Clear
    CancelAll = False
    mintHowManyShort = 10
    tmrShort.Interval = 250
    tmrShort.Enabled = True
End Sub

' Run a long task, then run short tasks
'   serially while it's running.
Private Sub cmdShortLong_Click()
    Dim cft As CoffeeTracker
    
    lstResults.Clear
    CancelAll = False
    Set cft = NewTracker(macfe(1).ThreadID, LONGTASKSIZE)
    Set cft.Coffee = macfe(1)
    Call macfe(1).StartLongTask(LONGTASKSIZE)
    '
    ' Line up some short tasks to run (one
    '   every quarter second) while task
    '   runs.
    mintHowManyShort = 10
    tmrShort.Enabled = True
    tmrShort.Interval = 250
End Sub

' Start N long tasks (1 - 9), then run a series
'   of short tasks.
Private Sub cmdNLong_Click()
    Dim intCt As Integer
    Dim cft As CoffeeTracker
    
    lstResults.Clear
    CancelAll = False
    For intCt = 1 To CLng("0" & txtN)
        Set cft = NewTracker(macfe(intCt).ThreadID, LONGTASKSIZE)
        Set cft.Coffee = macfe(intCt)
        Call macfe(intCt).StartLongTask(LONGTASKSIZE)
    Next
    '
    ' Line up some short tasks to run while
    '   the long ones run.
    mintHowManyShort = 10 + CLng("0" & txtN)
    tmrShort.Enabled = True
    tmrShort.Interval = 250
End Sub

' Compare the call overhead for calls to an
'   object on the same thread, vs. calls to
'   an object on another thread.
'
Private Sub cmdXThread_Click()
    Dim cfeSame As Coffee
    Dim cfeNew As Coffee
    
    cmdXThread.Caption = "Working..."
    cmdXThread.Enabled = False
    
    ' Create a Coffee object on another thread.
    Set cfeNew = macfe(10).GetCoffeeOnNewThread
    ' In case of thread pooling; except in
    '   the degenerate case of one thread in
    '   the pool, this should get a different
    '   thread the second time.
    If cfeNew.ThreadID = macfe(10).ThreadID Then
        Set cfeNew = macfe(10).GetCoffeeOnNewThread
        If cfeNew.ThreadID = macfe(10).ThreadID Then
            MsgBox "Unable to run comparison between same-thread and cross-thread calls; can't get an object on another thread."
            Exit Sub
        End If
    End If
    '
    ' Create a Coffee object on the same thread.
    Set cfeSame = macfe(10).GetCoffeeOnSameThread
    
    ' Use the newly created coffee objects to
    '   perform the test.
    MsgBox "Same thread:  " & macfe(10).CallAnotherCoffee(cfeSame) & " sec/call" & vbCrLf _
        & "Cross-thread:  " & macfe(10).CallAnotherCoffee(cfeNew) & " sec/call"
    
    cmdXThread.Caption = "Cross-Thread Overhead"
    cmdXThread.Enabled = True
    '
    ' The Coffee objects created for this test
    '   are terminated when cfeNew and cfeSame
    '   go out of scope at the end of this
    '   procedure.
End Sub

' List the thread IDs of the Coffee objects
'   created when this form loaded.
'
Private Sub cmdIDs_Click()
    Dim intCt As Integer
    lstResults.Clear
    For intCt = 1 To 10
        lstResults.AddItem macfe(intCt).ThreadID _
            & "  (" & macfe(intCt).NumberOnThread & " on thread)"
    Next
End Sub

' Create a series of Coffee objects, each on
'   its own thread.  (If you recompile MTCoffee
'   with a thread pool less than 10, some of
'   these will share thread and global state.)
'
Private Sub Form_Load()
    Dim intCt As Integer
    For intCt = 1 To 10
        Set macfe(intCt) = New Coffee
    Next
    Form1.cmdMT.Enabled = True
    Form1.cmdMT.MousePointer = vbDefault
End Sub

Private Sub Text1_KeyPress(KeyAscii As Integer)
    Select Case KeyAscii
        Case 48 To 57, 8
        Case Else
            Beep
            KeyAscii = 0
    End Select
End Sub

' Provide unique keys for CoffeeTrackers.
'
Private Function NewKey() As String
    Static lngLastKey As Long
    lngLastKey = lngLastKey + 1
    NewKey = "K" & lngLastKey
End Function

' Add a new CoffeeTracker.  Properly speaking,
'   this should be a method of a CoffeeTrackers
'   collection class.
'
Private Function NewTracker(ByVal ThreadID As Long, _
        ByVal Size As Long) As CoffeeTracker
        
    Dim cft As New CoffeeTracker
    '
    ' Cache the thread ID of the Coffee object
    '   the tracker will be keeping track of.
    cft.ThreadID = ThreadID
    '
    ' Set the size of the task assigned to the
    '   Coffee object the tracker will track.
    cft.Size = Size
    '
    ' Give the tracker a unique key for the
    '   collection.
    cft.ID = NewKey
    '
    ' Put the new tracker into a collection.
    CoffeeTrackers.Add cft, cft.ID
    '
    ' Return a reference to the new tracker.
    Set NewTracker = cft
End Function

' Timer is used to start a series of short
'   tasks, at regular intervals, using one
'   Coffee object (that is, a single thread).
'   Before starting a new task, it checks to
'   see whether the preceding task is done.
'
' If you run MTCoffee in the development
'   environment, comment out the code that
'   checks for preceding task completion.
'
Private Sub tmrShort_Timer()
    Static intCt As Integer
    Static strWaitingFor As String
    Dim cft As CoffeeTracker
    
    ' If strWaitingFor contains a key, then
    '   the Coffee object is (or was) performing
    '   a task, and the static string variable
    '   contains the key of the CoffeeTracker
    '   that's watching it.
    If strWaitingFor <> "" Then
        On Error Resume Next
        '
        ' If the CoffeeTracker we're waiting
        '   for has dropped out of the
        '   collection, then an error will
        '   occur -- meaning it's time to
        '   start another one.
        Set cft = CoffeeTrackers(strWaitingFor)
        If Err.Number = 0 Then Exit Sub
    Else
        ' If the static string variable is
        '   empty, then the previous series
        '   of short tasks is complete.  A
        '   new series is beginning, so reset
        '   the static counter (intCt).
        intCt = 0
    End If
    
    If Not CancelAll Then
        intCt = intCt + 1
        '
        ' Create a CoffeeTracker to wait
        '   for the request to finish.
        Set cft = NewTracker(macfe(10).ThreadID, SHORTTASKSIZE)
        '
        ' Give the CoffeeTracker its Coffee
        '   object to watch.
        Set cft.Coffee = macfe(10)
        '
        ' Begin the task.
        Call macfe(10).StartLongTask(SHORTTASKSIZE)
        '
        ' Prepare to wait for the
        '   CoffeeTracker.
        strWaitingFor = cft.ID
    End If
    '
    ' Check to see if we've completed the
    '   series of short tasks, or if we've
    '   been stopped by the Cancel button:
    If (intCt >= mintHowManyShort) Or CancelAll Then
        intCt = 0
        tmrShort.Enabled = False
        strWaitingFor = ""
    End If
End Sub
