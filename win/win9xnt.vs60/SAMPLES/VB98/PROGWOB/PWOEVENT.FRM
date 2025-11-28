VERSION 5.00
Begin VB.Form frmEvents 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Raising and Handling Events"
   ClientHeight    =   4245
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4710
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4245
   ScaleWidth      =   4710
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdAddForm 
      Caption         =   "Add &Receiver"
      Height          =   375
      Left            =   2880
      TabIndex        =   3
      Top             =   3360
      Width           =   1695
   End
   Begin VB.TextBox txtMessage 
      Height          =   285
      Left            =   120
      TabIndex        =   2
      Top             =   3000
      Width           =   4455
   End
   Begin VB.CommandButton cmdPercentDone 
      Caption         =   "&Start a long task that uses an event to report progress"
      Height          =   615
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   4215
   End
   Begin VB.Label Label2 
      Caption         =   "Message after all recipients have handled it:"
      Height          =   375
      Left            =   120
      TabIndex        =   6
      Top             =   3360
      Width           =   2295
   End
   Begin VB.Label lblPercentDone 
      Height          =   255
      Left            =   360
      TabIndex        =   5
      Top             =   960
      Width           =   4095
   End
   Begin VB.Label lblEcho 
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   3840
      Width           =   4455
   End
   Begin VB.Label Label1 
      Caption         =   $"PWOEvent.frx":0000
      Height          =   1335
      Left            =   120
      TabIndex        =   1
      Top             =   1560
      Width           =   4455
   End
   Begin VB.Line Line1 
      X1              =   120
      X2              =   4560
      Y1              =   1440
      Y2              =   1440
   End
End
Attribute VB_Name = "frmEvents"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

' ========================================
'     Declarations for Broadcast Demo
'
' The Broadcast event has one argument,
'   the message to be sent.  The argument
'   is ByRef, so recipients can change it.
Event Broadcast(Message As String)

' Collection of receivers.
Private mcolReceivers As New Collection

' ========================================
'       Declarations and Code for
'           Percent Done Demo
'
Private WithEvents mWidget As Widget
Attribute mWidget.VB_VarHelpID = -1
Private mblnCancel As Boolean

Private Sub mWidget_PercentDone(ByVal Percent As Double, Cancel As Boolean)
    lblPercentDone.Caption = CInt(100 * Percent) & " percent complete"
    DoEvents
    If mblnCancel Then Cancel = True
End Sub

Private Sub cmdPercentDone_Click()
    Static blnProcessing As Boolean
    If blnProcessing Then
        mblnCancel = True
    Else
        blnProcessing = True
        cmdPercentDone.Caption = "&Cancel Task"
        mblnCancel = False
        lblPercentDone.Caption = "0 percent complete"
        lblPercentDone.Refresh
        
        ' Create a Widget and start the
        '   long-running task.
        Set mWidget = New Widget
        On Error Resume Next
        Call mWidget.LongTask(14.4, 0.9)
        '
        ' See if the call ended because it
        '   was canceled (can't just test
        '   mblnCancel for this, because
        '   it might have been set just as
        '   LongTask was returning).
        If Err.Number = 0 Then
            lblPercentDone.Caption = "Task Complete"
        ElseIf Err.Number = vbObjectError + wdgERRTaskCanceled Then
            lblPercentDone.Caption = "Task Canceled"
        Else
            ' (Handling for other errors omitted.)
            lblPercentDone.Caption = "Something bad happened"
        End If
        Set mWidget = Nothing
        cmdPercentDone.Caption = "&Start a long task that uses an event to report progress"
        blnProcessing = False
    End If
End Sub

' ========================================
'        Code for Broadcast Demo
'
Private Sub cmdAddForm_Click()
    Dim frm As New frmReceiver
    ' Keep track of the receivers.
    mcolReceivers.Add frm
    frm.Show vbModeless, Me
    Me.SetFocus
    txtMessage.SetFocus
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim frm As frmReceiver
    On Error Resume Next
    Do While mcolReceivers.Count > 0
        Unload mcolReceivers(1)
        mcolReceivers.Remove 1
    Loop
End Sub

Private Sub txtMessage_Change()
    Dim strMessage As String
    strMessage = txtMessage.Text
    '
    ' Raise the Broadcast event.  Note that
    '   there's no way of knowing if there
    '   are any receivers handling the
    '   event.
    RaiseEvent Broadcast(strMessage)
    '
    ' Display the message after all
    '   receivers (if any) have handled
    '   it.  Note that there's no way
    '   to know which receiver altered the
    '   message, or what interim values
    '   the message may have had.
    lblEcho = strMessage
End Sub
