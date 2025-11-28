VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "clsMessageArrival"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Private WithEvents MessageQueueEvents As MSMQEvent
Attribute MessageQueueEvents.VB_VarHelpID = -1


Sub Initialize(objMQ As MSMQQueue)
    'Create a new event object (this seems rather odd - would expect MSMQ to create this)
    Set MessageQueueEvents = New MSMQEvent
    
    'Set up event notification...
    objMQ.EnableNotification MessageQueueEvents
End Sub

Private Sub MessageQueueEvents_Arrived(ByVal Queue As Object, ByVal Cursor As Long)
'    Stop
    'Restock the list of messages
'    frmClaimProcessing.FillPendingMessagesList
End Sub

Private Sub MessageQueueEvents_ArrivedError(ByVal Queue As Object, ByVal ErrorCode As Long, ByVal Cursor As Long)
 '   Stop
End Sub
