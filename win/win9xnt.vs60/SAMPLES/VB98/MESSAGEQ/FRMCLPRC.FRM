VERSION 5.00
Begin VB.Form frmClaimProcessing 
   Caption         =   "Claim Processing"
   ClientHeight    =   3585
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   2520
   LinkTopic       =   "Form1"
   ScaleHeight     =   3585
   ScaleWidth      =   2520
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox lstPendingMessages 
      Height          =   3375
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   2295
   End
End
Attribute VB_Name = "frmClaimProcessing"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'--------------------------------------------------------------
'This form is acutally used in 2 different programs,
'the 'claim processor' & the 'accounting dept' processor.
'The logic in both cases is identical, only the queue names change
'--------------------------------------------------------------
#If CLAIM_PROCESSING Then
    'QUEUE NAMES
    Const RECIEVE_QUEUE = INSURANCE_CLAIMS_QUEUE_NAME
    Const SEND_QUEUE = INSURANCE_ACCOUNTING_QUEUE_NAME

    'UI STUFF
    Const FORM_CAPTION = "Claim Processing Department"

#ElseIf ACCOUNTING_PROCESSING Then
    'QUEUE NAMES
    Const RECIEVE_QUEUE = INSURANCE_ACCOUNTING_QUEUE_NAME
    Const SEND_QUEUE = "" 'No queue to send to, this is the last stage in the workflow

    'UI STUFF
    Const FORM_CAPTION = "Accounting Department"
#End If

Private m_mqRecieveQueue As MSMQQueue 'Queue for claims coming in
Private m_mqSendQueue As MSMQQueue 'Queue for claims going out
Private WithEvents MessageQueueEvents_RecieveQueue As MSMQEvent
Attribute MessageQueueEvents_RecieveQueue.VB_VarHelpID = -1


Private Sub Form_Load()
    Me.Caption = FORM_CAPTION
    InitializeQueueAccess
    DoEvents 'Allow the mesage arrival events to fire before we bring up the UI
End Sub

'---------------------------------------------
'Sets our access up to messages queues that recieve/send messages & hooks up the event handling for the recieve queue
'
' [11/13/97, IvoSa] Created
'---------------------------------------------
Sub InitializeQueueAccess()
    'Open the queue
    OpenMessageQueueForRecieve
    OpenMessageQueueForSend

'    'Fill list with message headers
'    FillPendingMessagesList
    
    '-----------------------------------------
    'Set up the event to get notification when messages arrive (this will fire immediately if there are messages in the queue)
    '-----------------------------------------
    Set MessageQueueEvents_RecieveQueue = New MSMQEvent
    m_mqRecieveQueue.EnableNotification MessageQueueEvents_RecieveQueue
End Sub

'---------------------------------------------
'Fill our entire list with the labels of pending messages
' [in] colLabels : Collection of labels
'---------------------------------------------
Sub resyncPendingMessageList(ByVal colLabels As Collection)
Dim vntLabel As Variant
    lstPendingMessages.Clear
    
    For Each vntLabel In colLabels
        lstPendingMessages.AddItem CStr(vntLabel)
    Next
End Sub

Private Sub lstPendingMessages_DblClick()
    ShowIndividualClaim lstPendingMessages.List(lstPendingMessages.ListIndex)
End Sub

'---------------------------------------------
'Shows the claim for an individual person
'---------------------------------------------
Sub ShowIndividualClaim(strLabel As String)
Dim objMsg As MSMQMessage
    Set objMsg = getMessageGivenLabelAndQueue(m_mqRecieveQueue, strLabel)
    
    If (objMsg Is Nothing) Then
        MsgBox "The messge " & strLabel & "no longer exists"
        Exit Sub
    End If
    
    'Depersist the message
    Dim objStrBag As StringBag
    Set objStrBag = New StringBag
    objStrBag.DeSerialize objMsg.Body
    
    'Show a form with this data
    Dim frm As frmProcessClaim
    Set frm = New frmProcessClaim
    frm.Initailzie objStrBag, m_mqSendQueue
    frm.Show
    DoEvents 'Make it paint
    
    'Fill list with message headers again...
    resyncPendingMessageList getLabelsOfMessagesInQueue(m_mqRecieveQueue)
End Sub


'---------------------------------------------------------
'Opens a the 'Claims' message queue for us to submit messages to
' [11/13/97, IvoSa] Created
' [12/6/97, IvoSa] Added error checking
'---------------------------------------------------------
Sub OpenMessageQueueForRecieve()
Dim lngError As Long
Dim strError As String
    
    If RECIEVE_QUEUE <> "" Then
        On Error Resume Next
            Set m_mqRecieveQueue = openMessageQueue(RECIEVE_QUEUE, MQ_RECEIVE_ACCESS, True)
            lngError = Err
            strError = Err.Description
        On Error GoTo 0
    
        If lngError Then
            MsgBox ERROR_TEXT_RUNADMINTOOL & "(specific error text reads: " & strError & ")"
            
            Stop 'Allow the app to be debugged here...
            End 'We should not continue running at this point
        End If
    
    End If
End Sub

'---------------------------------------------------------
'Opens a the 'Claims' message queue for us to submit messages to
' [11/13/97, IvoSa] Created
' [12/6/97, IvoSa] Added error checking
'---------------------------------------------------------
Sub OpenMessageQueueForSend()
Dim lngError As Long
Dim strError As String
    
    If SEND_QUEUE <> "" Then
        On Error Resume Next
            Set m_mqSendQueue = openMessageQueue(SEND_QUEUE, MQ_SEND_ACCESS, True)
            lngError = Err
            strError = Err.Description
        On Error GoTo 0
        
        If lngError Then
            MsgBox ERROR_TEXT_RUNADMINTOOL & "(specific error text reads: " & strError & ")"
            
            Stop 'Allow the app to be debugged here...
            End 'We should not continue running at this point
        End If
    End If
End Sub


'---------------------------------------------------------
'Called when new messages arrive
'---------------------------------------------------------
Private Sub MessageQueueEvents_RecieveQueue_Arrived(ByVal Queue As Object, ByVal Cursor As Long)
Dim colLabels As Collection
    'Get list of message lablels & set the event to fire when the next claim arrives
    Set MessageQueueEvents_RecieveQueue = getEventFireOnNextMessageArrival(m_mqRecieveQueue, colLabels)
    
    resyncPendingMessageList colLabels
End Sub

