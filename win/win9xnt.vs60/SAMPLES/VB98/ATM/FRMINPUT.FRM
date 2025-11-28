VERSION 5.00
Begin VB.Form frmInput 
   BorderStyle     =   3  'Fixed Dialog
   ClientHeight    =   3636
   ClientLeft      =   3612
   ClientTop       =   2628
   ClientWidth     =   5220
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3636
   ScaleWidth      =   5220
   Begin VB.Frame fraAccount 
      Caption         =   "fraAccount"
      Height          =   1215
      Left            =   1080
      TabIndex        =   8
      Top             =   960
      Width           =   3975
      Begin VB.OptionButton optChecking 
         Caption         =   "optChecking"
         Height          =   255
         Left            =   240
         TabIndex        =   1
         Top             =   360
         Value           =   -1  'True
         Width           =   3495
      End
      Begin VB.OptionButton optSavings 
         Caption         =   "optSavings"
         Height          =   255
         Left            =   240
         TabIndex        =   2
         Top             =   720
         Width           =   3495
      End
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "cmdOK"
      Default         =   -1  'True
      Height          =   375
      Left            =   3960
      TabIndex        =   4
      Top             =   3120
      Width           =   1095
   End
   Begin VB.TextBox txtUSDollarsAmt 
      Height          =   285
      Left            =   1080
      TabIndex        =   3
      Top             =   2640
      Width           =   2295
   End
   Begin VB.TextBox Text1 
      Height          =   285
      HideSelection   =   0   'False
      Left            =   1080
      PasswordChar    =   "*"
      TabIndex        =   0
      Top             =   480
      Width           =   3975
   End
   Begin VB.Label lblUSDollars 
      Caption         =   "US Dollars"
      Height          =   195
      Left            =   3480
      TabIndex        =   7
      Top             =   2685
      Width           =   1575
   End
   Begin VB.Image imgFlag 
      BorderStyle     =   1  'Fixed Single
      Height          =   735
      Left            =   120
      Stretch         =   -1  'True
      Top             =   120
      Width           =   735
   End
   Begin VB.Label lblAmount 
      Caption         =   "lblAmount"
      Height          =   195
      Left            =   1080
      TabIndex        =   6
      Top             =   2400
      Width           =   3975
   End
   Begin VB.Label lblPINCode 
      Caption         =   "lblPINCode"
      Height          =   195
      Left            =   1080
      TabIndex        =   5
      Top             =   240
      Width           =   3975
   End
End
Attribute VB_Name = "frmInput"
Attribute VB_Base = "0{DC791295-F602-11CF-8A54-0020AF939EAB}"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' This sample application demonstrates how to use resource files to localize
' a Visual Basic application. It does not use error handling or data validation.

' This application uses the LoadResPicture function to load a picture, LoadResString
' to load a string, and LoadResData to read a .WAV file from the resource file,
' depending on which command button the user chose (in frmOpen).
Option Explicit

Sub Form_Load()
    imgFlag = LoadResPicture(I, vbResBitmap)
    Caption = LoadResString(I)
    lblPINCode = LoadResString(1 + I)
    fraAccount = LoadResString(2 + I)
    optChecking.Caption = LoadResString(3 + I)
    optSavings.Caption = LoadResString(4 + I)
    lblAmount = LoadResString(5 + I)
    cmdOK.Caption = LoadResString(6 + I)
    SetCursor cmdOK
End Sub

Sub cmdOK_click()
    MsgBox LoadResString(7 + I)   ' Display a process message.
    frmAmountWithdrawn.Show vbModal
    Unload Me
End Sub

Private Sub Form_Unload(Cancel As Integer)
    EndPlaySound
End Sub
