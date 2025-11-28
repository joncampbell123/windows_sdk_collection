VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Connector"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Option Explicit
' > For an overview of this sample application, search
'   online Help for Coffee.
' > AboutCof.Txt, in the Related Documents folder of
'   CoffWat2.vbp, also contains information about the sample.

' Connector class
' ---------------
' The Connector allows multiple clients to share a single
'   CoffeeMonitor object.  The Connector class has its
'   Instancing property set to MultiUse, so each client
'   can create its own Connector.  Each of the Connector
'   objects returns a reference to the single shared
'   CoffeeMonitor object, so all the clients share the
'   same CoffeeMonitor.  (See the CoffeeMonitor property,
'   below.)
'
' If you read "Creating an ActiveX Exe Component," in
'   Books Online, you'll notice a difference:  This
'   version of Connector doesn't have a 'use count' that
'   determines when gCoffeeMonitor should be set to
'   Nothing.  There's no need for Initialize event code
'   to increment the use count, or Terminate event code to
'   decrement and test the use count.
'
' This solves a bug described in the topic "Using the
'   Shared CoffeeMonitor" in the chapter mentioned
'   above.  For an explanation of why the use count
'   isn't needed, and how this fixes the bug, see the
'   CoffeeMonitor class module.


' CoffeeMonitor property always returns the single global
' -------------   reference to the shared instance of
'   CoffeeMonitor (which is created the first time a client
'   requests a CoffeeMonitor).
'
Public Property Get CoffeeMonitor() As CoffeeMonitor
    ' If the shared CoffeeMonitor object hasn't been
    '   created, create it and store a global reference
    '   to it.
    If gCoffeeMonitor Is Nothing Then
        Set gCoffeeMonitor = New CoffeeMonitor
    End If
    
    Set CoffeeMonitor = gCoffeeMonitor
End Property
    
