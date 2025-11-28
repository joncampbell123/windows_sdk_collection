VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Connector2"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Option Explicit
' > For an overview of this sample application, search
'   online Help for Coffee.
' > AboutCof.Txt, in the Related Documents folder of
'   CoffWat2.vbp, also contains information about the sample.

' Connector2 class allows multiple clients to share a
' ----------------  single instance of CoffeeMonitor2.  The
'   Connector2 class has its Instancing property set to
'   MultiUse, so each client can create its own Connector2.
'   All the Connector objects return a reference to the
'   single shared CoffeeMonitor2 object, so all the clients
'   share the same CoffeeMonitor2.  (See the CoffeeMonitor2
'   property, below.)

' CoffeeMonitor2 property always returns the single global
' --------------   reference to the shared instance of
'   CoffeeMonitor2.
'
Public Property Get CoffeeMonitor2() As CoffeeMonitor2
    ' If the shared CoffeeMonitor object hasn't been
    '   created, create it and store a global reference
    '   to it.
    If gCoffeeMonitor2 Is Nothing Then
        Set gCoffeeMonitor2 = New CoffeeMonitor2
    End If
    
    Set CoffeeMonitor2 = gCoffeeMonitor2
End Property
    

