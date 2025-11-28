VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ICoffeeNotify"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

' The ICoffeeNotify module defines an interface
'   containing the call-back method, CoffeeReady,
'   and a property used to store the client's
'   ID (assigned by CoffeeMonitor2).
'

' CoffeeReady implemention contains all the code
' -----------      the client wants to execute
'   when a call-back occurs.
'
Public Sub CoffeeReady()
End Sub

' NotifyID implementation must provide a Long to
' --------    store the ID number.  CoffeeMonitor2
'   uses this to look up the call-back object when
'   a request to disconnect is made.
'
Public Property Get NotifyID() As Long
End Property
Public Property Let NotifyID(ByVal NewValue As Long)
End Property
