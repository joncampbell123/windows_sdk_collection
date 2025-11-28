VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Coffee"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit
Private Declare Function timeGetTime Lib "winmm.dll" () As Long

' The Coffee object represents a different style
'   of asynchronous notifications from those
'   performed by the CoffeeMonitors.  Instead
'   of periodic notifications Coffee provides
'   progress reports on a long task, and a
'   completion event.
'
' The mechanism used for these notifications
'   is to raise events.  You could also use
'   call-back methods, and in fact there are
'   advantages to doing so.  Call-backs would
'   allow a component to deal intelligently
'   with errors in the client, whereas events
'   don't return client errors.  This is
'   discussed in "When to Use Events or
'   Call-Backs for Notifications," in Books
'   Online.

' Number of iterations to perform in the dummy
'   task.
Private mlngIterations As Long

' XTimer is used to kick the long task off
'   asynchronously.
Private WithEvents mwXTimer As XTimer
Attribute mwXTimer.VB_VarHelpID = -1

Event Progress(ByVal PercentDone As Single, _
        ByRef Cancel As Boolean)
Event Complete(ByVal Canceled As Boolean)
        
' ThreadID returns the system thread ID of
' --------      the thread the object was
'   created on.
'
Public Property Get ThreadID() As Long
    ThreadID = App.ThreadID
End Property

' NumberOnThread returns the number of Coffee
' --------------    objects running on this
'   thread.  This is just the value of the
'   global data variable glngGlobalData, which
'   Coffee objects increment in their Initialize
'   events and decrement in their Terminate
'   events.
'
' If MTCoffee was compiled with Thread Per
'   Object, the only way for multiple objects
'   to share a thread (and the instance of
'   global data associated with it) is if
'   another Coffee has been created on this
'   thread by calling GetCoffeeOnSameThread.
'
' If MTCoffee was compiled with a Thread Pool
'   and the count of active objects exceeded
'   the number of threads in the pool, then
'   Coffee objects will be sharing threads.
'
Public Property Get NumberOnThread() As Long
    NumberOnThread = glngGlobalData
End Property

' StartLongTask sets things up for the long
' -------------     dummy task.  The task is
'   actually started by a code-only XTimer
'   that StartLongTask sets running.
'
Public Sub StartLongTask(ByVal Iterations As Long)
    ' This is a short circuit for testing call
    '   overhead.  See CallAnotherCoffee.
    If Iterations = 0 Then Exit Sub
    '
    ' Store the size of the dummy task.
    mlngIterations = Iterations
    '
    ' Give the timer a short
    '   interval, and set it running just
    '   before returning.
    mwXTimer.Interval = 55
    mwXTimer.Enabled = True
End Sub

' GetCoffeeOnSameThread creates a new Coffee
' ---------------------     object on the same
'   thread, simulating the effects of thread
'   pooling.  This can only be done internally,
'   as explained in "How Object Creation Works
'   in Visual Basic" in Books Online.
'
Public Function GetCoffeeOnSameThread() As Coffee
    ' All objects created using New will be on
    '   the creator's thread, even a new
    '   Coffee object.
    Set GetCoffeeOnSameThread = New Coffee
End Function

' GetCoffeeOnNewThread creates a new Coffee
' --------------------     object on a new
'   thread, by calling CreateObject to create
'   the new Coffee object.  The difference
'   between this and the internal creation
'   done by GetCoffeeOnSameThread is explained
'   in "How Object Creation Works in Visual
'   Basic" in Books Online.
'
' Note that this technique could be used to
'   create objects on different threads that
'   could communicate with each other, without
'   the client having to pass one object a
'   reference to the other (as CoffeeWatch
'   does).  If you experiment with this,
'   remember that the overhead of marshaling
'   calls between threads is almost as great
'   as the overhead of marshaling calls
'   across processes.
'
Public Function GetCoffeeOnNewThread() As Coffee
    ' Create as if by external client.
    Set GetCoffeeOnNewThread = CreateObject("MTCoffee.Coffee")
End Function

' CallAnotherCoffee gives a rough measure of
' -----------------     cross-thread call
'   overhead.  Pass it a Coffee object on
'   another thread, or on the same thread,
'   and compare the results; the method
'   makes dummy calls to StartLongTask, so
'   that it's essentially measuring only
'   the call overhead.
'
Public Function CallAnotherCoffee(ByVal cfe As Coffee) As Double
    Const TRIES = 10000
    Dim timeStart As Long
    Dim timeEnd As Long
    Dim lngTries As Long
    
    timeStart = timeGetTime
    For lngTries = 1 To TRIES
        cfe.StartLongTask 0
    Next
    timeEnd = timeGetTime
    '
    ' Return seconds (ss.mmm) per call.  (This
    '   will give an incorrect result if you
    '   happen to run CallAnotherCoffee just
    '   as the system timer is rolling over
    '   to zero.)
    CallAnotherCoffee = ((CDbl(timeEnd) - timeStart) / 1000#) / TRIES
End Function

Private Sub Class_Initialize()
    ' Increment the global count (that is,
    '   for this thread) of Coffee objects.
    glngGlobalData = glngGlobalData + 1
    '
    ' Create a timer object.
    Set mwXTimer = New XTimer
End Sub

Private Sub Class_Terminate()
    ' Decrement the global count (that is,
    '   for this thread) of Coffee objects.
    glngGlobalData = glngGlobalData - 1
    '
    ' Free the timer object.
    Set mwXTimer = Nothing
End Sub

Private Sub mwXTimer_Tick()
    ' First thing, turn off the timer.
    mwXTimer.Enabled = False
    Call LongTask
End Sub

' The dummy task.
'
Private Sub LongTask()
    Dim dblDummy As Double
    Dim lngCt As Long
    Dim sngNextMark As Single
    Dim blnCancel As Boolean
    
    ' For small transactions, don't bother to
    '   call back while running.
    If mlngIterations < 100000 Then
        sngNextMark = 1!
    Else
        sngNextMark = 0.1!
    End If
        
    ' This is just a time-waster.
    For lngCt = 1 To mlngIterations
        ' If this were a real application, a
        '   unit of work would be done here.
        '   You may find it interesting to
        '   replace this processor-intensive
        '   activity with one that waits on
        '   the system a lot, such as calls
        '   to a database on another machine,
        '   or reading a very large file.
        '   Throughput on a single-processor
        '   workstation is far greater when
        '   most threads are blocked,
        '   waiting for file input or the
        '   result of a database call.
        '
        dblDummy = 3033.14159 * 2081.14159 * 1138.14159
        '
        If CDbl(lngCt) / mlngIterations > sngNextMark Then
            RaiseEvent Progress(sngNextMark, blnCancel)
            If blnCancel Then
                ' If the client is tired of waiting
                '   and wants the task canceled,
                '   raise the Complete event with
                '   True (canceled).
                RaiseEvent Complete(True)
                Exit Sub
            End If
            sngNextMark = sngNextMark + 0.1!
        End If
    Next
    ' On successful completion, raise the
    '   Complete event with False (not
    '   canceled).
    RaiseEvent Complete(False)
End Sub
