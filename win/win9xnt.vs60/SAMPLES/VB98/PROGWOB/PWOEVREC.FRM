VERSION 5.00
Begin VB.Form frmReceiver 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Message received by handling Broadcast event"
   ClientHeight    =   690
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6420
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   690
   ScaleWidth      =   6420
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox chkReceive 
      Caption         =   "&Receive messages"
      Height          =   255
      Left            =   3480
      TabIndex        =   2
      Top             =   360
      Width           =   2895
   End
   Begin VB.CheckBox chkGarble 
      Caption         =   "&Garble messages"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   360
      Width           =   2895
   End
   Begin VB.Label lblMessage 
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   6135
   End
End
Attribute VB_Name = "frmReceiver"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
' The Source variable is declared using
'   the WithEvents keyword, so that the
'   events of the frmEvents object
'   assigned to it can be handled (see the
'   Source_Broadcast event procedure).  When
'   the reference is assigned, Visual Basic
'   connects the object with the event
'   procedure.
Private WithEvents Source As frmEvents
Attribute Source.VB_VarHelpID = -1

Private Sub chkReceive_Click()
    If chkReceive = vbChecked Then
        Set Source = frmEvents
    Else
        ' Setting the WithEvents variable
        '   to Nothing disconnects the
        '   event procedure from the
        '   frmEvents object.
        Set Source = Nothing
    End If
End Sub

Private Sub Form_Load()
    chkReceive = vbChecked
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Set Source = Nothing
End Sub

' Event procedure for the Broadcast event.
'   Because the Message argument is ByRef,
'   it can be altered -- and these changes
'   will be seen by all subsequent
'   handlers of the event.
Private Sub Source_Broadcast(Message As String)
    Dim intCt As Integer
    lblMessage = Message
    '
    ' If Garble is checked, garble the
    '   message.
    If chkGarble = vbChecked Then
        ' Use a random Step that hits every
        '   third character (minimum) to
        '   every ninth (maximum).
        For intCt = 1 To Len(Message) Step (Int(7 * Rnd) + 3)
            ' For characters that are to be garbled,
            '   change the ASCII value by a random
            '   amount from -3 to +3.
            Mid$(Message, intCt, 1) = Chr$(Asc(Mid$(Message, intCt, 1)) + Int(7 * Rnd) - 3)
        Next
    End If
End Sub
