VERSION 5.00
Begin VB.Form frmConfirm 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Form1"
   ClientHeight    =   2595
   ClientLeft      =   3540
   ClientTop       =   5310
   ClientWidth     =   5325
   ControlBox      =   0   'False
   BeginProperty Font 
      Name            =   "Tahoma"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "Confirm.frx":0000
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2595
   ScaleWidth      =   5325
   StartUpPosition =   2  'CenterScreen
   Tag             =   "10000"
   Begin VB.CheckBox chkDontShowAgain 
      Caption         =   "chkDontShowAgain"
      Height          =   270
      Left            =   420
      MaskColor       =   &H00000000&
      TabIndex        =   1
      Tag             =   "10002"
      Top             =   1380
      Width           =   4590
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "cmdOK"
      Default         =   -1  'True
      Height          =   495
      Left            =   1680
      MaskColor       =   &H00000000&
      TabIndex        =   0
      Tag             =   "10003"
      Top             =   1935
      Width           =   1875
   End
   Begin VB.Image Image1 
      Height          =   930
      Left            =   195
      Picture         =   "Confirm.frx":000C
      Stretch         =   -1  'True
      Top             =   210
      Width           =   975
   End
   Begin VB.Label lblConfirm 
      Caption         =   "lblConfirm"
      Height          =   990
      Left            =   1560
      TabIndex        =   2
      Tag             =   "10001"
      Top             =   165
      Width           =   3480
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "frmConfirm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub chkDontShowAgain_Click()
    On Error Resume Next
  
    If chkDontShowAgain.Value = vbChecked Then
        SaveSetting APP_CATEGORY, WIZARD_NAME, CONFIRM_KEY, DONTSHOW_CONFIRM
    Else
        SaveSetting APP_CATEGORY, WIZARD_NAME, CONFIRM_KEY, vbNullString
    End If

End Sub

Private Sub cmdOK_Click()
    Unload Me
End Sub

Private Sub Form_Load()
    LoadResStrings Me
End Sub

