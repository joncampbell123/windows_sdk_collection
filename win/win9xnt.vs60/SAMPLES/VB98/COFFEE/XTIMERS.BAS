Attribute VB_Name = "XTimerSupport"
Option Explicit
'================================================
' WARNING!  DO NOT press the End button while
'   debugging this project!  While in Break
'   mode, do NOT make edits that reset the
'   project!
'
' This module is dangerous because it uses the
'   SetTimer API and the AddressOf operator to
'   set up a code-only timer.  Once such a
'   timer is set up, the system will continue
'   to call the TimerProc function EVEN AFTER
'   YOU RETURN TO DESIGN TIME.
'
' Since TimerProc isn't available at design
'   time, the system will cause a PROGRAM
'   FAULT in Visual Basic.
'
' When debugging this module, you need to make
'   sure that all system timers have been
'   stopped (using KillTimer) before returning
'   to design time.  You can do this by calling
'   SCRUB from the Immediate window.
'
' Call-back timers are inherently dangerous.
'   It's much safer to use Timer controls for
'   most of your development process, and only
'   switch to call-back timers at the very
'   end.
'==================================================

' Amount to increase size of the array maxti when more
'   active timers are needed.  (See 'MoreRoom:' below.)
Const MAXTIMERINCREMEMT = 5

Private Type XTIMERINFO   ' Hungarian xti
    xt As XTimer
    id As Long
    blnReentered As Boolean
End Type

Declare Function SetTimer Lib "user32" (ByVal hWnd As Long, ByVal nIDEvent As Long, ByVal uElapse As Long, ByVal lpTimerProc As Long) As Long
Declare Function KillTimer Lib "user32" (ByVal hWnd As Long, ByVal nIDEvent As Long) As Long

' maxti is an array of active XTimer objects.  The reason
' -----   for using an array of user-defined types
'   instead of a Collection object is to get early
'   binding when we raise the XTimer object's Tick event.
Private maxti() As XTIMERINFO
'
' mintMaxTimers tells us how large the array maxti is at
' -------------   any given time.
Private mintMaxTimers As Integer

' BeginTimer function is called by an XTimer object when
' -------------------   the XTimer's Interval property is
'   set to a new non-zero value.
'
' The function makes the API calls required to set up a
'   timer.  If a timer is successfully created, the
'   function puts a reference to the XTimer object into
'   the array maxti.  This reference will be used to call
'   the method that raises the XTimer's Tick event.
'
Public Function BeginTimer(ByVal xt As XTimer, ByVal Interval As Long)
    Dim lngTimerID As Long
    Dim intTimerNumber As Integer
    
    lngTimerID = SetTimer(0, 0, Interval, AddressOf TimerProc)
    ' Success is a non-zero return from SetTimer.  If we can't
    '   get a timer, raise an error.
    If lngTimerID = 0 Then Err.Raise vbObjectError + 31013, , "No timers available"
    
    ' The following loop locates the first available slot
    '   in the array maxti.  If the upper bound is exceeded,
    '   an error occurs and the array is made larger.  (If
    '   you compile this DLL to Native Code, DO NOT turn off
    '   Bounds Checking!)
    For intTimerNumber = 1 To mintMaxTimers
        If maxti(intTimerNumber).id = 0 Then Exit For
    Next
    '
    ' If no empty space was found, increase the
    '   size of the array.
    If intTimerNumber > mintMaxTimers Then
        mintMaxTimers = mintMaxTimers + MAXTIMERINCREMEMT
        ReDim Preserve maxti(1 To mintMaxTimers)
    End If
    '
    ' Save a reference to use when raising the
    '   XTimer object's Tick event.
    Set maxti(intTimerNumber).xt = xt
    '
    ' Save the timer id returned by the SetTimer API, and
    '   return the value to the XTimer object.
    maxti(intTimerNumber).id = lngTimerID
    maxti(intTimerNumber).blnReentered = False
    BeginTimer = lngTimerID
End Function

' TimerProc is the timer procedure which the system will
' ---------   call whenever one of your timers goes off.
'
' IMPORTANT -- Because this procedure must be in a
'   standard module, all of your timer objects must share
'   it.  This means the procedure must identify which timer
'   has gone off.  This is done by searching the array
'   maxti for the ID of the timer (idEvent).
'
' If this Sub declaration is wrong, PROGRAM FAULTS will
'   occur!  This is one of the dangers of using APIs
'   that require call-back functions.
'
Public Sub TimerProc(ByVal hWnd As Long, ByVal uMsg As Long, ByVal idEvent As Long, ByVal lngSysTime As Long)
    Dim intCt As Integer

    For intCt = 1 To mintMaxTimers
        If maxti(intCt).id = idEvent Then
            ' Don't raise the event if an earlier
            '   instance of this event is still
            '   being processed.
            If maxti(intCt).blnReentered Then Exit Sub
            ' The blnReentered flag blocks further
            '   instances of this event until the
            '   current instance finishes.
            maxti(intCt).blnReentered = True
            On Error Resume Next
            ' Raise the Tick event for the appropriate
            '   XTimer object.
            maxti(intCt).xt.RaiseTick
            If Err.Number <> 0 Then
                ' If an error occurs, the XTimer has
                '   somehow managed to terminate without
                '   first letting go of its timer.  Clean
                '   up the orphaned timer, to prevent GP
                '   faults later.
                KillTimer 0, idEvent
                maxti(intCt).id = 0
                '
                ' Release the reference to the
                '   XTimer object.
                Set maxti(intCt).xt = Nothing
            End If
            '
            ' Allow this event to enter TimerProc
            '   again.
            maxti(intCt).blnReentered = False
            Exit Sub
        End If
    Next
    ' The following line is a fail-safe, in case an
    '   XTimer somehow got freed without the Windows
    '   system timer getting killed.
    '
    ' Execution can also reach this point because of
    '   a known bug with NT 3.51, whereby you may
    '   receive one extra timer event AFTER you have
    '   executed the KillTimer API.
    KillTimer 0, idEvent
End Sub

' EndTimer procedure is called by the XTimer whenever
' ------------------   the Enabled property is set to
'   False, and whenever a new timer interval is required.
'   There is no way to reset a system timer, so the only
'   way to change the interval is to kill the existing
'   timer and then call BeginTimer to start a new one.
'
Public Sub EndTimer(ByVal xt As XTimer)
    Dim lngTimerID As Long
    Dim intCt As Integer
    
    ' Ask the XTimer for its TimerID, so we can search the
    '   array for the correct XTIMERINFO.  (You could
    '   search for the XTimer reference itself, using the
    '   Is operator to compare xt with maxti(intCt).xt, but
    '   that wouldn't be as fast.)
    lngTimerID = xt.TimerID
    '
    ' If the timer ID is zero, EndTimer has been
    '   called in error.
    If lngTimerID = 0 Then Exit Sub
    '
    For intCt = 1 To mintMaxTimers
        If maxti(intCt).id = lngTimerID Then
            ' Kill the system timer.
            KillTimer 0, lngTimerID
            '
            ' Release the reference to the XTimer
            '   object.
            Set maxti(intCt).xt = Nothing
            '
            ' Clean up the ID, to free the slot for
            '   a new active timer.
            maxti(intCt).id = 0
            Exit Sub
        End If
    Next
End Sub

' Scrub procedure is a safety valve for debugging purposes
' ---------------   only:  If you have to End this project
'   while there are XTimer objects active, call Scrub from
'   the Immediate pane.  This will call KillTimer for all
'   of the system timers, so that the development
'   environment can safely return to design mode.
'
Public Sub Scrub()
    Dim intCt As Integer
    ' Kill remaining active timers.
    For intCt = 1 To mintMaxTimers
        If maxti(intCt).id <> 0 Then KillTimer 0, maxti(intCt).id
    Next
End Sub
