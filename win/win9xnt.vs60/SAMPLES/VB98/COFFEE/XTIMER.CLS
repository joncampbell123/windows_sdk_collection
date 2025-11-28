VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "XTimer"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit
'===============================================
' WARNING!  DO NOT press the End button while
'   debugging this project!  See explanation
'   at the top of the XTimerSupport module
'   (XTimerS.bas).
'===============================================

' Private storage for XTimer properties:
Private mlngTimerID As Long
Private mlngInterval As Long
Private mblnEnabled As Boolean

' The XTimer's only event is Tick.  XTimer's Tick event
'   doesn't have any arguments (eliminating arguments speeds
'   up the event slightly), but there's no reason why you
'   couldn't supply arguments if you wanted to.
Event Tick()

' TimerID property is required by the EndTimer procedure,
' ----------------   in order to quickly locate the timer
'   in the support module's array of active timers.
'
' There's no reason for the client to use this property,
'   so it's declared Friend instead of Public.
'
Friend Property Get TimerID() As Long
    TimerID = mlngTimerID
End Property

' Enabled property turns the timer on and off.  This is
' ----------------      done by killing the system timer,
'   because there's no way to suspend a system timer.
'   If they exist, they're running.
'
Public Property Get Enabled() As Boolean
    Enabled = mblnEnabled
End Property
'
Public Property Let Enabled(ByVal NewValue As Boolean)
    ' If there's no change to the state of
    '   the property, then exit.  This
    '   prevents starting a second system
    '   timer when one is already running,
    '   etcetera.
    If NewValue = mblnEnabled Then Exit Property
    '
    ' Save the new property setting.
    mblnEnabled = NewValue
    '
    ' If the Interval is zero, the timer
    '   is already stopped.  Don't start it.
    If mlngInterval = 0 Then Exit Property
    '
    ' Turn timer on or off.
    If mblnEnabled Then
        Debug.Assert mlngTimerID = 0
        mlngTimerID = BeginTimer(Me, mlngInterval)
    Else
        ' The following is necessary, because
        '   an XTimer can shut off its system
        '   timer two ways:  Enabled = False,
        '   or Interval = 0.
        If mlngTimerID <> 0 Then
            Call EndTimer(Me)
            mlngTimerID = 0
        End If
    End If
End Property

' Interval property must do more than just set the
' -----------------   timer interval.  If the XTimer
'   is enabled, and the Interval is changed from zero
'   to a non-zero value, then a system timer must be
'   started.  Likewise, if the Interval is changed
'   to zero, the system timer must be stopped.
'
' The Property Let procedure also ends one system timer
'   and starts another whenever the interval changes.
'   This is because there's no way to change the
'   interval of a system timer.
'
Public Property Get Interval() As Long
    Interval = mlngInterval
End Property
'
Public Property Let Interval(ByVal NewInterval As Long)
    ' If the new value for Interval is the same as the old,
    '   there's no reason to do anything.
    If NewInterval = mlngInterval Then Exit Property
    '
    ' Save the new value.
    mlngInterval = NewInterval
    '
    ' If the XTimer is active, mlngTimerID is non-zero.
    '   in this case, the old system timer must be
    '   ended before a new one is started.
    If mlngTimerID <> 0 Then
        Call EndTimer(Me)
        mlngTimerID = 0
    End If
    '
    ' If the new interval is zero, then the XTimer
    '   becomes inactive, regardless of the current
    '   value of Enabled.  If the new interval is
    '   not zero, AND the Enabled property is True,
    '   then a new system timer is started, and its
    '   ID is stored in mlngTimerID.
    If (NewInterval <> 0) And mblnEnabled Then
        mlngTimerID = BeginTimer(Me, NewInterval)
    End If
End Property

' RaiseTick method is called by the support module when
' ----------------   the system timer event occurs for
'   this XTimer object's system timer.
'
' Implementation detail:  You might expect to declare
'   this method Friend instead of Public, as there's
'   no need for the client to call RaiseTick.  However,
'   it's critical that RaiseTick be declared Public,
'   because the XTimer might be released while the
'   Tick event is still being handled.  An object will
'   not terminate while one of its Public methods is
'   on the stack, but it CAN terminate while one of its
'   Friend methods is on the stack.  If the object
'   terminates before the Friend method returns (which
'   could happen if the client executes a lot of code
'   in the XTimer's Tick event), a GPF will result.
'   (Note that this is a highly unusual scenario that
'   depends on an external event; it does not occur in
'   ordinary use of Friend functions.)
'
Public Sub RaiseTick()
    RaiseEvent Tick
End Sub

Private Sub Class_Terminate()
    ' When the client releases its last reference to
    '   an XTimer object, it goes away -- but only
    '   if the XTimer's Enabled property is False,
    '   or its Interval property is True!
    '
    ' This is because while the XTimer's system
    '   timer is running, the XTimerSupport module
    '   has to have a reference to the XTimer in
    '   order to raise its Tick event.  Thus,
    '   failure of the client to disable XTimer
    '   objects before releasing them will LEAK
    '   system timers!
    '
    ' These leaked system timers will not be
    '   recovered until the XTimers component shuts
    '   down -- that is, when the client using
    '   the DLL shuts down.  The DLL will NOT
    '   unload when all XTimer objects are released,
    '   because references to public objects (in
    '   this case, those held by XTimerSupport)
    '   will prevent a DLL from unloading.
    '
    ' So why bother to clean up the system timer
    '   in the Terminate event?  Because when the
    '   DLL is getting shut down, all references
    '   to the XTimer object will be cleaned up
    '   -- and the XTimer will get its Terminate
    '   event.  The system timer should be
    '   destroyed at this point.
    On Error Resume Next
    If mlngTimerID <> 0 Then KillTimer 0, mlngTimerID
    '
    ' The following is what XTimer should do if
    '   it could somehow be released prior to
    '   DLL shutdown.
    'If mlngTimerID <> 0 Then Call EndTimer(Me)
End Sub

