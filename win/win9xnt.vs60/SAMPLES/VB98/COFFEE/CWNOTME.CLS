VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "NotifyMe"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

' NotifyMe implements ICoffeeNotify so that
'   its call-back method (CoffeeReady) can
'   be called early-bound.
Implements ICoffeeNotify

' Storage for implementation of NotifyID.
Private mlngNotifyID As Long

' ICoffeeNotify.CoffeeReady is called by the
' ============= -----------     CoffeeMonitor2
'   object in Coffee2.  This implementation
'   adds the time of the call to a list box,
'   and deletes the oldest entry if there are
'   more than ten entries.
'
Private Sub ICoffeeNotify_CoffeeReady()
    With Form1.lstCallBacks
        .AddItem Format$(Now, "ddd hh:mm:ss"), 0
        If .ListCount > 10 Then .RemoveItem 10
    End With
End Sub

' ICoffeeNotify.NotifyID property holds the
' ============= --------       notification
'   object's key in CoffeeMonitor2's array
'   of client objects.  Client implementation
'   simply stores and returns the value.
'
Private Property Let ICoffeeNotify_NotifyID(ByVal RHS As Long)
    mlngNotifyID = RHS
End Property
'
Private Property Get ICoffeeNotify_NotifyID() As Long
    ICoffeeNotify_NotifyID = mlngNotifyID
End Property
