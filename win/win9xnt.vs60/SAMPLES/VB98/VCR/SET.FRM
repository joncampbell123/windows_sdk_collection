VERSION 5.00
Begin VB.Form frmSetTime 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Form1"
   ClientHeight    =   2760
   ClientLeft      =   780
   ClientTop       =   1590
   ClientWidth     =   5175
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2760
   ScaleWidth      =   5175
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   495
      Left            =   3360
      MaskColor       =   &H00000000&
      TabIndex        =   7
      Top             =   1200
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   495
      Left            =   3360
      MaskColor       =   &H00000000&
      TabIndex        =   6
      Top             =   480
      Width           =   1215
   End
   Begin VB.TextBox txtChannel 
      Height          =   375
      Left            =   1200
      TabIndex        =   5
      Text            =   "3"
      ToolTipText     =   "Enter the channel to record"
      Top             =   1920
      Width           =   615
   End
   Begin VB.TextBox txtEnd 
      Height          =   375
      Left            =   1200
      TabIndex        =   4
      Text            =   "12:30 AM"
      ToolTipText     =   "Enter the time to stop recording"
      Top             =   1200
      Width           =   1215
   End
   Begin VB.TextBox txtStart 
      Height          =   375
      Left            =   1200
      TabIndex        =   3
      Text            =   "12:00 AM"
      ToolTipText     =   "Enter the time to start  recording"
      Top             =   480
      Width           =   1215
   End
   Begin VB.Label lblCaption 
      Alignment       =   1  'Right Justify
      Caption         =   "Channel"
      Height          =   255
      Index           =   2
      Left            =   0
      TabIndex        =   2
      Top             =   2040
      Width           =   1095
   End
   Begin VB.Label lblCaption 
      Alignment       =   1  'Right Justify
      Caption         =   "End Time"
      Height          =   255
      Index           =   1
      Left            =   0
      TabIndex        =   1
      Top             =   1320
      Width           =   1095
   End
   Begin VB.Label lblCaption 
      Alignment       =   1  'Right Justify
      Caption         =   "Start Time"
      Height          =   255
      Index           =   0
      Left            =   0
      TabIndex        =   0
      Top             =   600
      Width           =   1095
   End
End
Attribute VB_Name = "frmSetTime"
Attribute VB_Base = "0{FF906415-E9E1-11CF-84BA-00AA00C007F0}"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'**********************************************
' Purpose:  Form for getting user input to set
'           the time and channel for recording.
'           Part of the VCR sample application
'**********************************************
Option Explicit

Private Sub cmdCancel_Click()
    ' unload the form, release the reference
    Unload Me
    Set frmSetTime = Nothing
End Sub

Private Sub cmdOK_Click()
    Dim intOK As Integer        'Return value
    Dim intChannel As Integer   'Channel number
    Dim strMsg As String        'Message text
    
    ' Call the function to validate the entry
    intOK = ValidateTime(txtStart.Text, txtStart)
    ' If it wasn't valid, bail out here
    If intOK = False Then Exit Sub
        
    ' Call the function to validate the entry
    intOK = ValidateTime(txtEnd.Text, txtEnd)
    ' If it wasn't valid, bail out here
    If intOK = False Then Exit Sub
    
    ' Read the value of the channel display
    intChannel = Val(txtChannel)
    ' If out of range, warn the user
    If intChannel < 2 Or intChannel > 13 Then
        strMsg = "Channel must be between 2 and 13!"
        MsgBox strMsg, vbOKOnly, "Invalid Channel"
        txtChannel.SetFocus
        Exit Sub
    End If
    
    ' Set the properties of the Recorder class
    Recorder.StartRecording = CVar(txtStart.Text)
    Recorder.StopRecording = CVar(txtEnd.Text)
    Recorder.Channel = intChannel
        
    ' unload the form, release the reference
    Unload Me
    Set frmSetTime = Nothing
End Sub

Private Sub Form_Load()
    ' If the property of the class contains a
    ' value, assign it to the TextBox
    If Recorder.StartRecording <> Empty Then
        txtStart.Text = Recorder.StartRecording
    End If
    If Recorder.StopRecording <> Empty Then
        txtEnd.Text = Recorder.StopRecording
    End If
    txtChannel = Recorder.Channel
End Sub
'**********************************************
' Purpose:  Validates the time string entered by
'           the user. If correct, it formats the
'           string, if not, it displays a message.
' Inputs:   Time:   The string passed from a
'                   text box.
'           Field:  The name of the text box control.
' Returns:  True for a valid time, False if the
'           string isn't a valid time.
'**********************************************
Function ValidateTime(Time As String, Field As Control) As Boolean
    Dim strMsg As String    ' Message text
    
    ' The IsDate function verifies a valid time
    If IsDate(Time) Then
        ' Format the string and return True
        Field.Text = Format$(Time, "h:mm AM/PM")
        ValidateTime = True
    Else
        ' Notify the user of the error
        strMsg = "Please enter a valid time! "
        strMsg = strMsg & "(Hour:Minute AM/PM)"
        MsgBox strMsg, vbOKOnly, "Invalid Time"
        ' Return focus to the textbox
        Field.SetFocus
        ' Return a value of False
        ValidateTime = False
    End If
    
End Function

Private Sub txtChannel_GotFocus()
    ' Select the existing text
    txtChannel.SelStart = 0
    txtChannel.SelLength = Len(txtChannel.Text)
End Sub

Private Sub txtChannel_KeyPress(KeyAscii As Integer)
    ' Use the IsNumeric function to make sure
    ' only numbers can be entered
    If Not IsNumeric(Chr(KeyAscii)) Then
        ' Not a number, so set it to null
        KeyAscii = 0
    End If
End Sub

Private Sub txtEnd_GotFocus()
    ' Select the existing text
    txtEnd.SelStart = 0
    txtEnd.SelLength = Len(txtEnd.Text)
End Sub

Private Sub txtEnd_LostFocus()
    Dim intOK As Integer    ' Return value
    
    ' Call the function to validate the entry
    intOK = ValidateTime(txtEnd.Text, txtEnd)
End Sub

Private Sub txtStart_GotFocus()
    ' Select the existing text
    txtStart.SelStart = 0
    txtStart.SelLength = Len(txtStart.Text)
End Sub

Private Sub txtStart_LostFocus()
    Dim intOK As Integer    'Return value
    
    ' Call the function to validate the entry
    intOK = ValidateTime(txtStart.Text, txtStart)
End Sub
