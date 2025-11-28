VERSION 5.00
Begin VB.Form frmQueueMonitoring 
   Caption         =   "Form1"
   ClientHeight    =   3135
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   8460
   LinkTopic       =   "Form1"
   ScaleHeight     =   3135
   ScaleWidth      =   8460
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdCheckForMessages 
      Caption         =   "&CheckForMessages"
      Height          =   255
      Left            =   120
      TabIndex        =   17
      Top             =   2880
      Width           =   1695
   End
   Begin VB.ListBox lstAccountingQueue 
      Height          =   840
      Left            =   6000
      TabIndex        =   16
      Top             =   1920
      Width           =   2415
   End
   Begin VB.ListBox lstClaimsQueue 
      Height          =   840
      Left            =   6000
      TabIndex        =   15
      Top             =   600
      Width           =   2415
   End
   Begin VB.Frame Frame1 
      Height          =   975
      Left            =   0
      TabIndex        =   7
      Top             =   480
      Width           =   5895
      Begin VB.TextBox txtClaimsQueueLocation 
         BackColor       =   &H8000000F&
         Height          =   285
         Left            =   3240
         TabIndex        =   9
         Top             =   600
         Width           =   2535
      End
      Begin VB.TextBox txtClaimsQueueName 
         BackColor       =   &H8000000F&
         Height          =   285
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   8
         Top             =   240
         Width           =   2535
      End
      Begin VB.Label Label4 
         Caption         =   "(Claim Verification)"
         Height          =   255
         Left            =   360
         TabIndex        =   13
         Top             =   480
         Width           =   1455
      End
      Begin VB.Label Label2 
         Caption         =   "Queue"
         Height          =   255
         Left            =   360
         TabIndex        =   12
         Top             =   240
         Width           =   1215
      End
      Begin VB.Shape Shape1 
         Height          =   495
         Left            =   120
         Top             =   240
         Width           =   1815
      End
      Begin VB.Label lblQueueLocation 
         Caption         =   "Queue Location:"
         Height          =   255
         Left            =   2040
         TabIndex        =   11
         Top             =   600
         Width           =   1215
      End
      Begin VB.Label Label5 
         Caption         =   "Queue Name:"
         Height          =   255
         Left            =   2040
         TabIndex        =   10
         Top             =   240
         Width           =   1095
      End
   End
   Begin VB.Frame Frame2 
      Height          =   975
      Left            =   0
      TabIndex        =   0
      Top             =   1800
      Width           =   5895
      Begin VB.TextBox txtAccountingQueueName 
         BackColor       =   &H8000000F&
         Height          =   285
         Left            =   3240
         Locked          =   -1  'True
         TabIndex        =   2
         Top             =   240
         Width           =   2535
      End
      Begin VB.TextBox txtAccountingQueueLocation 
         BackColor       =   &H8000000F&
         Height          =   285
         Left            =   3240
         TabIndex        =   1
         Top             =   600
         Width           =   2535
      End
      Begin VB.Label Label8 
         Caption         =   "Queue"
         Height          =   255
         Left            =   360
         TabIndex        =   6
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Queue Name:"
         Height          =   255
         Left            =   2040
         TabIndex        =   5
         Top             =   240
         Width           =   1095
      End
      Begin VB.Label Label6 
         Caption         =   "Queue Location:"
         Height          =   255
         Left            =   2040
         TabIndex        =   4
         Top             =   600
         Width           =   1215
      End
      Begin VB.Label Label7 
         Caption         =   "(Accounting)"
         Height          =   255
         Left            =   360
         TabIndex        =   3
         Top             =   480
         Width           =   1455
      End
      Begin VB.Shape Shape2 
         Height          =   495
         Left            =   120
         Top             =   240
         Width           =   1815
      End
   End
   Begin VB.Label Label3 
      Caption         =   "Claim Entry"
      Height          =   255
      Left            =   2400
      TabIndex        =   14
      Top             =   0
      Width           =   975
   End
   Begin VB.Line Line4 
      X1              =   2520
      X2              =   2760
      Y1              =   360
      Y2              =   480
   End
   Begin VB.Line Line5 
      X1              =   2760
      X2              =   3000
      Y1              =   480
      Y2              =   360
   End
   Begin VB.Line Line6 
      X1              =   2760
      X2              =   2760
      Y1              =   480
      Y2              =   240
   End
   Begin VB.Line Line10 
      X1              =   2760
      X2              =   2760
      Y1              =   1800
      Y2              =   1560
   End
   Begin VB.Line Line11 
      X1              =   2760
      X2              =   3000
      Y1              =   1800
      Y2              =   1680
   End
   Begin VB.Line Line12 
      X1              =   2520
      X2              =   2760
      Y1              =   1680
      Y2              =   1800
   End
End
Attribute VB_Name = "frmQueueMonitoring"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private WithEvents MessageQueueEvents_Claims As MSMQEvent
Attribute MessageQueueEvents_Claims.VB_VarHelpID = -1
Private WithEvents MessageQueueEvents_Accounting As MSMQEvent
Attribute MessageQueueEvents_Accounting.VB_VarHelpID = -1

Private m_mqClaims As MSMQQueue 'Queue for claims department
Attribute m_mqClaims.VB_VarHelpID = -1
Private m_mqAccounting As MSMQQueue 'Queue for accounting department


'---------------------------------------------
'Gets us ready to read in messages from our queues
' [11/13/97, IvoSa]
' [12/6/97, IvoSa] Added error message when queue not found...
'---------------------------------------------
Sub InitializeQueueAccess()
Dim lngError As Long
Dim strError As String
Dim objQI As MSMQQueueInfo
    
    'Claims Queue
    txtClaimsQueueName = INSURANCE_CLAIMS_QUEUE_NAME
    On Error Resume Next
        Set m_mqClaims = openMessageQueue(INSURANCE_CLAIMS_QUEUE_NAME, MQ_PEEK_ACCESS, True, objQI)
        lngError = Err
        strError = Err.Description
    On Error GoTo 0
    
    'See if an error occured...
    If lngError Then
        MsgBox ERROR_TEXT_RUNADMINTOOL & "(specific error text reads: " & strError & ")"
        
        Stop 'Allow the app to be debugged here...
        End 'We should not continue running at this point
    End If

    txtClaimsQueueLocation = objQI.PathName

    'Accounting queue
    txtAccountingQueueName = INSURANCE_ACCOUNTING_QUEUE_NAME
    On Error Resume Next
        Set m_mqAccounting = openMessageQueue(INSURANCE_ACCOUNTING_QUEUE_NAME, MQ_PEEK_ACCESS, True, objQI)
        lngError = Err
        strError = Err.Description
    On Error GoTo 0
    
    'See if an error occured...
    If lngError Then
        MsgBox ERROR_TEXT_RUNADMINTOOL & "(specific error text reads: " & strError & ")"
        
        Stop 'Allow the app to be debugged here...
        End 'We should not continue running at this point
    End If
    
    txtAccountingQueueLocation = objQI.PathName

    'Fill up the listboxes with the labels of messages in the queues
'    resyncClaimsQueueList
'    resyncAccountingQueueList
    
    '-----------------------------------------
    'Set up the events to get notification when messages arrive (these will fire immediately if there are messages in the queue)
    '-----------------------------------------
    Set MessageQueueEvents_Claims = New MSMQEvent
    m_mqClaims.EnableNotification MessageQueueEvents_Claims
    
    Set MessageQueueEvents_Accounting = New MSMQEvent
    m_mqAccounting.EnableNotification MessageQueueEvents_Accounting
    
End Sub

'-------------------------------------------------
'ReSync all of the message lists
' [11/13/97, Ivosa] Created
' [3/19/98, Ivosa]  Fixed queue event handler bug
'-------------------------------------------------
Private Sub cmdCheckForMessages_Click()
    
Dim colMsgLabels As Collection
    'Get the labels of the insurance claims in the 'claims' queue
    Set colMsgLabels = getLabelsOfMessagesInQueue(m_mqClaims)
    'Resync our list
    resyncClaimsQueueList colMsgLabels
    
    'Get the labels of the insurance claims in the 'accounting' queue
    Set colMsgLabels = getLabelsOfMessagesInQueue(m_mqAccounting)
    'Resync out list
    resyncAccountingQueueList colMsgLabels
End Sub

Private Sub Form_Load()
    InitializeQueueAccess
    DoEvents 'Fill the lists with the message events (this will allow the events to fire)
End Sub

'---------------------------------
'This event is triggered when the queue goes from an empty to a non-empty state
'---------------------------------
Private Sub MessageQueueEvents_Accounting_Arrived(ByVal Queue As Object, ByVal Cursor As Long)
Dim colLabels As Collection
    'Get list of message lablels & set the event to fire when the next claim arrives
    Set MessageQueueEvents_Accounting = getEventFireOnNextMessageArrival(m_mqAccounting, colLabels)
    
    resyncAccountingQueueList colLabels
End Sub

Private Sub MessageQueueEvents_Accounting_ArrivedError(ByVal Queue As Object, ByVal ErrorCode As Long, ByVal Cursor As Long)
Stop 'UNDONE
End Sub

'---------------------------------
'This event is triggered when the queue goes from an empty to a non-empty state
'---------------------------------
Private Sub MessageQueueEvents_Claims_Arrived(ByVal Queue As Object, ByVal Cursor As Long)
Dim colLabels As Collection
    
    'Get list of message lablels & set the event to fire when the next claim arrives
    Set MessageQueueEvents_Claims = getEventFireOnNextMessageArrival(m_mqClaims, colLabels)
    
    resyncClaimsQueueList colLabels
End Sub

'---------------------------------------------------
' Get the list of messages out there & fill the list with their labels
' [in] colLables : Collection of labels
' [11/13/97, IvoSa] Created
'---------------------------------------------------
Sub resyncAccountingQueueList(ByVal colLabels As Collection)
Dim vntLabel As Variant
    
    Dim lst As ListBox
    Set lst = lstAccountingQueue
    'Clear the old list
    lst.Clear
    For Each vntLabel In colLabels
        lst.AddItem CStr(vntLabel)
    Next
End Sub

'---------------------------------------------------
' Get the list of messages out there & fill the list with their labels
' [in] colLables : Collection of labels
' [11/13/97, IvoSa] Created
'---------------------------------------------------
Sub resyncClaimsQueueList(ByVal colLabels As Collection)
Dim vntLabel As Variant
    Dim lst As ListBox
    Set lst = lstClaimsQueue
    'Clear the old list
    lst.Clear
    For Each vntLabel In colLabels
        lst.AddItem CStr(vntLabel)
    Next
End Sub

Private Sub MessageQueueEvents_Claims_ArrivedError(ByVal Queue As Object, ByVal ErrorCode As Long, ByVal Cursor As Long)
Stop 'UNDONE
End Sub
