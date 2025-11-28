VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "CoffeeMonitor"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit
' > For an overview of this sample application, search
'   online Help for Coffee.
' > AboutCof.Txt, in the Related Documents folder of
'   CoffWat2.vbp, also contains information about the sample.

' CoffeeMonitor class monitors an imaginary serial interface
' -------------------   to a high-tech coffee pot, using
'   a call-back timer (XTimer object) to determine how
'   often to check the coffee status.  When the coffee's
'   ready, the CoffeeMonitor object raises an event to
'   notify clients.  (Since the high-tech coffee pot has
'   not yet been invented, this sample application just
'   raises the CoffeeReady event every ten seconds.)
'
' (See also the CoffeeMonitor2 class, which demonstrates
'   notification by a call-back method instead of an event.)
'
' Note that the CoffeeMonitor class's Instancing property
'   is set to PublicNotCreatable.  This means that clients
'   cannot create CoffeeMonitors; they can only get a
'   reference to the shared CoffeeMonitor by creating a
'   Connector object and accessing its CoffeeMonitor
'   property.
'
' By using a code-only timer, this version of CoffeeMonitor
'   fixes the bug described in the topic "Using the Shared
'   CoffeeMonitor," in "Creating an ActiveX Exe Component,"
'   in Books Online, whereby multiple CoffeeMonitor objects
'   could sometimes be created.
'
' That bug was caused by the 'use count' code the Connector
'   object used to release the global reference to the shared
'   CoffeeMonitor.  The use count was required because the
'   step-by-step procedures in Books Online used a hidden
'   form to hold a Timer control; hidden forms keep
'   components from unloading (as described in "Starting and
'   Ending a Component" in Books Online), which keeps global
'   memory from being freed, which keeps gCoffeeMonitor from
'   being set to Nothing.
'
' Unlike the hidden form with its Timer control, the XTimer
'   object won't keep the Coffee2 component from unloading
'   when the last client releases its last reference.  (Note,
'   however, that the global variable gCoffeeMonitor will
'   keep the CoffeeMonitor object alive until all objects
'   provided by Coffee2 are released by their respective
'   clients.)

' =======================================================
'  WARNING!  Code-only timers are inherently dangerous
'       in the Visual Basic development environment,
'       becaue the system blindly calls back into your
'       code until the timer is turned off with an API
'       call.  It's safer to use Timer controls during
'       most of the development process, and only switch
'       to call-back timers at the very end.
' =======================================================


' mXTimer holds a reference to a code-only timer that
' -------   tells CoffeeMonitor when to check the pot.
'   Because the variable is declared WithEvents, the
'   CoffeeMonitor object receives the XTimer object's Tick
'   events (see Sub mwXTimer_Tick, below).  Code for the
'   XTimer object can be found in XTimers.vbp.
Private WithEvents mwXTimer As XTimer
Attribute mwXTimer.VB_VarHelpID = -1

' CoffeeReady is the event CoffeeMonitor raises for its
' -----------   clients when the coffee is done.  The
'   Public keyword is omitted from the event declaration
'   because events are always public.
Event CoffeeReady()
    
Private Sub Class_Initialize()
    ' The first thing a CoffeeMonitor object does is
    '   create the XTimer object.  When this assignment is
    '   made, Visual Basic connects the XTimer's Tick event
    '   to the mwXTimer_Tick event procedure (see below).
    '
    Set mwXTimer = New XTimer
    '
    ' The timer is set to tick every ten seconds (10,000
    '   milliseconds).
    mwXTimer.Interval = 10000
    mwXTimer.Enabled = True
End Sub

Private Sub Class_Terminate()
    ' It's important to disable the XTimer before releasing
    '   it.  As described in XTimers.vbp, abandoning a
    '   running XTimer essentially leaks a system timer
    '   until XTimers.DLL finally shuts down.
    mwXTimer.Enabled = False
    Set mwXTimer = Nothing
    '
    Debug.Print "CoffeeMonitor (events) terminated at " & Now
End Sub

' mwXTimer_Tick is the event procedure CoffeeMonitor uses
' -------------   to receive the XTimer object's Tick
'   events.  The name of an event procedure that's
'   associated with a WithEvents variable always has the
'   variable name as a prefix.
'
Private Sub mwXTimer_Tick()
    ' (Code to test serial port omitted.)
    '
    ' Notify the client.
    RaiseEvent CoffeeReady
End Sub
