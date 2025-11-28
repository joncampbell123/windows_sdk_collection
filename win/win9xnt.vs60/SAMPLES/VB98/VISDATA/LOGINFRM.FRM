VERSION 5.00
Begin VB.Form frmLogin 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Login"
   ClientHeight    =   1980
   ClientLeft      =   2550
   ClientTop       =   3330
   ClientWidth     =   2820
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   HelpContextID   =   2016133
   Icon            =   "Loginfrm.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   ScaleHeight     =   1980
   ScaleWidth      =   2820
   ShowInTaskbar   =   0   'False
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   375
      Left            =   1440
      MaskColor       =   &H00000000&
      TabIndex        =   3
      Top             =   1440
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "&OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   120
      MaskColor       =   &H00000000&
      TabIndex        =   2
      Top             =   1440
      Width           =   1215
   End
   Begin VB.TextBox txtPassword 
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   120
      PasswordChar    =   "*"
      TabIndex        =   1
      Top             =   960
      Width           =   2535
   End
   Begin VB.TextBox txtLoginName 
      Height          =   285
      Left            =   120
      TabIndex        =   0
      Top             =   360
      Width           =   2535
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Password: "
      Height          =   195
      Index           =   1
      Left            =   120
      TabIndex        =   5
      Top             =   720
      Width           =   795
   End
   Begin VB.Label lblLabels 
      AutoSize        =   -1  'True
      Caption         =   "Login Name: "
      Height          =   195
      Index           =   0
      Left            =   120
      TabIndex        =   4
      Top             =   120
      Width           =   930
   End
End
Attribute VB_Name = "frmLogin"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const FORMCAPTION = "Login"
Const BUTTON1 = "&OK"
Const BUTTON2 = "&Cancel"
Const Label1 = "Login Name:"
Const Label2 = "Password:"
'>>>>>>>>>>>>>>>>>>>>>>>>


Private Sub cmdCancel_Click()
  On Error Resume Next
  'if the global workspace object is not set, then we must be
  'opening the app so we should end
  If gwsMainWS Is Nothing Then
    End
  Else
    'just unload since the workspace is already set
    Unload Me
  End If
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2016133
  End If
End Sub

Private Sub Form_Load()
  Me.Caption = FORMCAPTION
  cmdOK.Caption = BUTTON1
  cmdCancel.Caption = BUTTON2
  lblLabels(0).Caption = Label1
  lblLabels(1).Caption = Label2
End Sub

Private Sub cmdOK_Click()
  On Error GoTo OKErr
  
  Dim wsp As Workspace
  Dim sTmp As String

  If Not gwsMainWS Is Nothing Then
    If UCase(txtLoginName.Text) = UCase(gwsMainWS.UserName) Then
      'same as current login name
      Unload Me
      Exit Sub
    End If
  End If
  
  'set the new login name
  DBEngine.DefaultUser = txtLoginName.Text
  DBEngine.DefaultPassword = txtPassword.Text
  
  Set wsp = DBEngine.CreateWorkspace("MainWS", txtLoginName.Text, txtPassword.Text)
  'must have been successful so set the gswMainWS
  Set gwsMainWS = wsp
  Unload Me
  
  Exit Sub

OKErr:
  MsgBox Error
  txtLoginName.SetFocus
  Exit Sub        'give them another chance

End Sub

Private Sub txtLoginName_GotFocus()
  txtLoginName.SelStart = 0
  txtLoginName.SelLength = Len(txtLoginName.Text)
End Sub

Private Sub txtPassword_GotFocus()
  txtPassword.SelStart = 0
  txtPassword.SelLength = Len(txtPassword.Text)
End Sub
