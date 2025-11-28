VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Widget"
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

' Defining error numbers in a public Enum
'   makes them visible throughout the
'   project.
Public Enum WidgetErrors
    wdgERRTaskCanceled = 1059
End Enum

' PercentDone event is raised periodically
'   during LongTask, to notify the caller
'   of progress.  The event arguments are
'   the percent complete and a ByRef Cancel
'   argument that the caller can set to
'   True to cancel LongTask.
Event PercentDone(ByVal Percent As Double, _
    Cancel As Boolean)
    
' LongTask method simulates a long-running
' --------      task that raises the
'   PercentDone event, and allows the caller
'   to cancel the operation.
'
' The first argument tells LongTask how
'   long you want the simulated task to
'   last.  The second argument gives the
'   minimum interval for raising events
'   to notify the caller of progress.
'
' Using a time interval to determine
'   when to raise the event gives more
'   consistent results on different
'   computers.  For an alternate
'   approach, see LongTask2.
'
Public Sub LongTask(ByVal Duration As Double, _
        ByVal MinimumInterval As Double)
    Dim dblThreshold As Double
    Dim dblStart As Double
    Dim blnCancel As Boolean
    
    dblStart = Timer
    dblThreshold = MinimumInterval
    
    Do While Timer < (dblStart + Duration)
        ' In a real application, a unit of
        '   work would be done here.  The
        '   work must be divided up so
        '   that units are neither too large
        '   (too long between notifications)
        '   nor too small (the more times you
        '   test, the less efficient LongTask
        '   will be).
        
        ' After each unit of work, test to
        '   see if it's time to notify the
        '   caller of LongTask's progress.
        If Timer > (dblStart + dblThreshold) Then
            ' Raise the event; execution of
            '   LongTask will not continue
            '   until the caller's event
            '   procedure returns!
            RaiseEvent PercentDone( _
                dblThreshold / Duration, _
                blnCancel)
            '
            ' Test to see whether the caller
            '   wants to cancel LongTask.
            If blnCancel Then
                Err.Raise vbObjectError + wdgERRTaskCanceled, , _
                    "Task Cancelled"
' NOTE: If your program breaks here, right-click
'   to bring up the code window context menu.
'   Click Toggle, then click Break on Unhandled
'   Errors.  Press F5 to continue running the
'   program.  (You may have to press Alt+Tab to
'   get the Events form back.)  Here's why you
'   toggle the setting:
' The default setting, Break in Class Module,
'   is useful if you're getting an error on a
'   call to a method of a class, because it allows
'   Visual Basic to break INSIDE the class module,
'   at the point of the error.  If your class
'   raises errors routinely, as here, this is not
'   so convenient!
' You can set the default to Break on Unhandled
'   Errors, using the General tab of the Options
'   dialog box, accessible from the Tools menu.
'   If you do this, just remember that when you
'   break on a method call, and you want to run
'   to the point of the error, you can use
'   the code window context menu to Toggle to
'   Break in Class Module.
' Note that you can also use Alt+F5 to run past
'   a single error when you're using Break in
'   Class Module (or Alt+F8 to step past).  If
'   these keys leave you at the same line of code,
'   then there's no error handler available.
' For more information, see "Debugging Class
'   Modules" in Books Online.
' [End Digression]
            End If
            '
            ' Set the threshold for the next
            '   notification.
            dblThreshold = dblThreshold + MinimumInterval
       End If
    Loop
End Sub
 
' LongTask2 also simulates a long-running
' ---------     task that raises the
'   PercentDone event, and allows the caller
'   to cancel the operation.
'
' The simulated task consists of repeated
'   floating-point calculations.  The first
'   argument tells LongTask2 how many
'   iterations you want the task to have.
'   The second argument gives the change
'   in percentage complete that triggers
'   the notification event.  Note that
'   this method results in a variable
'   length of time between notifications --
'   a variation that may be compounded by
'   differences in machine performance.
'
' By contrast, LongTask uses a time
'   interval to determine how often to
'   raise the event; this gives more
'   consistent results on different
'   computers.
'
Public Sub LongTask2(ByVal Iterations As Long, _
        ByVal PercentChange As Byte)
    Dim lngThreshold As Long
    Dim dblIterationsPerEvent As Double
    Dim lngCt As Long
    Dim dblDummy As Double
    Dim blnCancel As Boolean
    
    dblIterationsPerEvent = Iterations _
        * (CDbl(PercentChange) / 100)
    lngThreshold = dblIterationsPerEvent
    
    For lngCt = 1 To Iterations
        ' In a real application, a unit of
        '   work would be done here.  The
        '   work must be divided up so
        '   that units are neither too large
        '   (too long between notifications)
        '   nor too small (the more times you
        '   test, the less efficient LongTask
        '   will be).
        dblDummy = 3.14159 * 2640 * 2640
        
        ' After each unit of work, test to
        '   see if it's time to notify the
        '   caller of LongTask's progress.
        If lngCt > lngThreshold Then
            ' Raise the event; execution of
            '   LongTask2 will not continue
            '   until the caller's event
            '   procedure returns!
            RaiseEvent PercentDone( _
                lngCt * 100 / Iterations, _
                blnCancel)
            '
            ' Test to see whether the caller
            '   wants to cancel LongTask2.
            If blnCancel Then
                Err.Raise vbObjectError + wdgERRTaskCanceled, , _
                    "Task Cancelled"
            End If
            '
            ' Set the threshold for the next
            '   notification.
            lngThreshold = lngThreshold + dblIterationsPerEvent
        End If
    Next
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


