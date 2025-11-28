VERSION 5.00
Begin VB.Form frmZoom 
   Caption         =   "Zoom"
   ClientHeight    =   1800
   ClientLeft      =   3600
   ClientTop       =   3930
   ClientWidth     =   4560
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   HelpContextID   =   2016148
   Icon            =   "ZOOM.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   ScaleHeight     =   1800
   ScaleWidth      =   4560
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.CommandButton cmdClose 
      Caption         =   "&Close"
      Height          =   264
      Left            =   120
      MaskColor       =   &H00000000&
      TabIndex        =   4
      Top             =   36
      Visible         =   0   'False
      Width           =   972
   End
   Begin VB.TextBox txtZoomData 
      Height          =   285
      Left            =   40
      TabIndex        =   0
      Top             =   360
      Width           =   4455
   End
   Begin VB.CommandButton cmdSave 
      Caption         =   "&Save Changes"
      Height          =   264
      Left            =   120
      MaskColor       =   &H00000000&
      TabIndex        =   2
      Top             =   36
      Visible         =   0   'False
      Width           =   1932
   End
   Begin VB.CommandButton cmdCloseNoSave 
      Cancel          =   -1  'True
      Caption         =   "&Close w/o Changes"
      Height          =   264
      Left            =   2160
      MaskColor       =   &H00000000&
      TabIndex        =   3
      Top             =   40
      Visible         =   0   'False
      Width           =   1932
   End
   Begin VB.TextBox txtMemo 
      Height          =   1332
      Left            =   48
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   1
      Top             =   360
      Visible         =   0   'False
      Width           =   4452
   End
End
Attribute VB_Name = "frmZoom"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const BUTTON1 = "&Close"
Const BUTTON2 = "&Save Changes"
Const BUTTON3 = "&Close w/o Changes"
'>>>>>>>>>>>>>>>>>>>>>>>>


Private Sub txtZoomData_KeyPress(KeyAscii As Integer)
  'throw away the key if save not allowed
  If cmdSave.Visible = False Then KeyAscii = 0
End Sub

Private Sub cmdCloseNoSave_Click()
  gsZoomData = "__CANCELLED__"
  Unload Me
End Sub

Private Sub cmdClose_Click()
  Call cmdCloseNoSave_Click
  Unload Me
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2016148
  End If
End Sub

Private Sub Form_KeyPress(KeyAscii As Integer)
  'check for the escape key
  If KeyAscii = vbKeyEscape Then
    Call cmdCloseNoSave_Click
    Exit Sub
  End If
End Sub

Private Sub Form_Load()
  cmdClose.Caption = BUTTON1
  cmdSave.Caption = BUTTON2
  cmdCloseNoSave.Caption = BUTTON3
  
  Me.Width = 4600
  SendKeys "{End}"
End Sub

Private Sub Form_Resize()
  On Error Resume Next

  If txtZoomData.Visible Then
    txtZoomData.Width = Me.Width - 200
  Else
    txtMemo.Width = Me.Width - 200
    txtMemo.Height = Me.Height - 850
  End If
End Sub

Private Sub cmdSave_Click()
  If txtZoomData.Visible Then
    gsZoomData = txtZoomData.Text
  Else
    gsZoomData = txtMemo.Text
  End If
  Unload Me
End Sub
