Attribute VB_Name = "Module1"
Option Explicit
' > For an overview of this sample application, search
'   online Help for Coffee.
' > AboutCof.Txt, in the Related Documents folder of
'   CoffWat2.vbp, also contains information about the sample.

' gCoffeeMonitor holds a reference to the shared
' --------------  CoffeeMonitor object.  When a client
'   accesses the CoffeeMonitor property of a Connector
'   object, the Property Get returns this reference.
'   (See the Connector class module.)
Public gCoffeeMonitor As CoffeeMonitor

' gCoffeeMonitor2 holds a reference to the shared
' ---------------  CoffeeMonitor2 object.  When a client
'   accesses the CoffeeMonitor2 property of a Connector2
'   object, the Property Get returns this reference.
'   (See the Connector2 class module.)
Public gCoffeeMonitor2 As CoffeeMonitor2

