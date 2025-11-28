VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "CoffeeMonitor2"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit
' > For an overview of this sample application, search
'   online Help for Coffee.
' > AboutCof.Txt, in the Related Documents folder of
'   CoffWat2.vbp, also contains information about the sample.

' CoffeeMonitor2 class
' --------------------
' Like the CoffeeMonitor object, CoffeeMonitor2
'   monitors an imaginary serial interface to a high-
'   tech coffee pot, using a timer to determine
'   how often to check the coffee status.
'
' Instead of raising an event when the coffee's ready,
'   the CoffeeMonitor2 object invokes a call-back method
'   that must be implemented by one of the client's
'   classes.  The call-back method is declared in the
'   ICallBack class.
'
' (Since the high-tech coffee pot has not yet been
'   invented, this sample application simply invokes the
'   call-back method every ten seconds.)
'
' IMPORTANT: To simplify a rather complex example, the
'   ICallBack class has been included in this project.
'   This will NOT work in real-life systems, which usually
'   go through many versions.  If a second version of
'   Coffee2 is made, the interface ID version number will
'   be incremented, and earlier clients will NOT work with
'   the new version of Coffee2.  Standard interfaces like
'   ICallBack should be created by themselves in small DLLs
'   which can be referenced by both client and component.
'   Once an interface is in use by finished applications,
'   it must never be changed.  For more information, search
'   for "polymorphism" in Books Online.
'
' Note that the CoffeeMonitor2 class's Instancing property
'   is set to PublicNotCreatable.  This means that clients
'   cannot create a CoffeeMonitor2; they can only get a
'   reference to the shared CoffeeMonitor2 by creating a
'   Connector2 object and accessing its CoffeeMonitor2
'   property.
'
' Like the CoffeeMonitor class, the CoffeeMonitor2 class
'   fixes the bug described in the topic "Using the Shared
'   CoffeeMonitor," in "Creating an ActiveX Exe Component,"
'   in Books Online, whereby multiple CoffeeMonitor objects
'   could sometimes be created.

' =======================================================
'  WARNING!  Code-only timers are inherently dangerous
'       in the Visual Basic development environment,
'       becaue the system blindly calls back into your
'       code until the timer is turned off with an API
'       call.  It's safer to use Timer controls during
'       most of the development process, and only switch
'       to call-back timers at the very end.
' =======================================================

Const ICN_ARRAYINCREMENT = 10

' maicnClients stores references to all the clients that have
' ------------   requested call-backs.  (Note that this is
'   different from the use of events in CoffeeMonitor;
'   one event can be received by any number of clients, while
'   call-backs must be made one by one.)  An array is used,
'   rather than a Collection, because Collection objects keep
'   objects in Variants, resulting in late binding.
Private maicnClients() As ICoffeeNotify
Private mlngMaxClients As Long

' mXTimer holds a reference to a code-only timer that
' -------   tells CoffeeMonitor2 when to check the pot.
'   Because the variable is declared WithEvents, the
'   CoffeeMonitor2 object receives the XTimer object's Tick
'   events (see Sub mwXTimer_Tick, below).  Code for the
'   XTimer object can be found in XTimers.vbp.
Private WithEvents mwXTimer As XTimer
Attribute mwXTimer.VB_VarHelpID = -1

Private Sub Class_Initialize()
    ' Allocate some space in the array of client objects.
    mlngMaxClients = ICN_ARRAYINCREMENT
    ReDim maicnClients(1 To mlngMaxClients)
    '
    ' Create the XTimer object.  When this assignment is
    '   made, Visual Basic connects the XTimer's Tick event
    '   to the mwXTimer_Tick event procedure (see below).
    Set mwXTimer = New XTimer
    '
    ' The timer is set to tick every ten seconds (10,000
    '   milliseconds).
    mwXTimer.Interval = 10000
    mwXTimer.Enabled = True
End Sub

Private Sub Class_Terminate()
    Dim intCt As Integer
    
    ' It's important to disable the XTimer before releasing
    '   it.  As described in XTimers.vbp, abandoning a
    '   running XTimer essentially leaks a system timer
    '   until XTimers.DLL finally shuts down.
    mwXTimer.Enabled = False
    Set mwXTimer = Nothing
    '
    ' Release all remaining call-back clients, in case they
    '   released CoffeeMonitor2 without first requesting
    '   an end to notifications.
    For intCt = 1 To mlngMaxClients
        Set maicnClients(intCt) = Nothing
    Next
    '
    Debug.Print "CoffeeMonitor2 (call-backs) terminated at " & Now
End Sub

' TellMeReady is called by a client who wants to receive a
' -----------   call-back when the coffee is ready.  The
'   client must implement the ICoffeeNotify interface,
'   defined in the ICoffeeNotify class.
'
Public Sub TellMeReady(ByVal icn As ICoffeeNotify)
    Dim lngCt As Long
    
    ' Find an opening in the array of interfaces.
    For lngCt = 1 To mlngMaxClients
        If maicnClients(lngCt) Is Nothing Then Exit For
    Next
    '
    ' If there were no openings, grow the array.
    If lngCt > mlngMaxClients Then
        mlngMaxClients = mlngMaxClients + ICN_ARRAYINCREMENT
        ReDim Preserve maicnClients(1 To mlngMaxClients)
    End If
    '
    Set maicnClients(lngCt) = icn
    '
    ' Give the object the index of its entry, as a key for
    '   quick lookup when disconnection is requested.
    icn.NotifyID = lngCt
End Sub

' CeaseCallBacks removes the client from the list of objects
' --------------   receiving call-back notifications, using
'   the key the object was assigned when it requested
'   notifications.
'
Public Sub CeaseCallBacks(ByVal icn As ICoffeeNotify)
    Set maicnClients(icn.NotifyID) = Nothing
End Sub

' mwXTimer_Tick is the event procedure CoffeeMonitor2 uses
' -------------   to receive the XTimer object's Tick
'   events.  The name of an event procedure that's
'   associated with a WithEvents variable always has the
'   variable name as a prefix.
'
Private Sub mwXTimer_Tick()
    Dim lngCt As Long
    
    ' (Code to test serial port omitted.)
    '
    On Error Resume Next
    '
    ' The call-back method must be called for each object
    '   that has requested a notification.
    For lngCt = 1 To mlngMaxClients
        If Not maicnClients(lngCt) Is Nothing Then
            maicnClients(lngCt).CoffeeReady
            If Err.Number <> 0 Then
                ' Error &H80010005 is ignored, because it
                '   can be caused by the client object being
                '   temporarily unresponsive.
                If Err.Number <> &H80010005 Then
                    ' If a client application has closed without
                    '   ending the notifications, remove it from
                    '   the list.
                    Set maicnClients(lngCt) = Nothing
                End If
                '
                ' When On Error Resume Next is used, the
                '   error number must be cleared after each
                '   error.
                Err.Number = 0
            End If
        End If
    Next
End Sub

