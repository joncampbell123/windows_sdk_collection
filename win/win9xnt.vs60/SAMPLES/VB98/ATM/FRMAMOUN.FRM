VERSION 5.00
Begin VB.Form frmAmountWithdrawn 
   BorderStyle     =   3  'Fixed Dialog
   ClientHeight    =   3048
   ClientLeft      =   5256
   ClientTop       =   3588
   ClientWidth     =   4164
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3048
   ScaleWidth      =   4164
   Begin VB.Frame fraMoney 
      Caption         =   "fraMoney"
      Height          =   1215
      Left            =   120
      TabIndex        =   2
      Top             =   1080
      Width           =   3855
      Begin VB.TextBox txtConvertedAmt 
         Height          =   285
         Left            =   2040
         TabIndex        =   4
         Top             =   600
         Width           =   1575
      End
      Begin VB.TextBox txtUSDollarsAmt 
         Height          =   285
         Left            =   240
         TabIndex        =   3
         TabStop         =   0   'False
         Top             =   600
         Width           =   1575
      End
      Begin VB.Label lblCurrency 
         Caption         =   "lblCurrency"
         Height          =   195
         Left            =   2040
         TabIndex        =   6
         Top             =   360
         Width           =   1575
      End
      Begin VB.Label lblUSDollars 
         Caption         =   "US Dollars"
         Height          =   195
         Left            =   240
         TabIndex        =   5
         Top             =   360
         Width           =   1575
      End
   End
   Begin VB.CommandButton cmdOKEnd 
      Caption         =   "cmdOKEnd"
      Default         =   -1  'True
      Height          =   375
      Left            =   2880
      TabIndex        =   1
      Top             =   2520
      Width           =   1095
   End
   Begin VB.Image imgFlag 
      BorderStyle     =   1  'Fixed Single
      Height          =   735
      Left            =   120
      Stretch         =   -1  'True
      Top             =   120
      Width           =   735
   End
   Begin VB.Label lblAmountWith 
      Caption         =   "lblAmountWith"
      Height          =   735
      Left            =   1080
      TabIndex        =   0
      Top             =   120
      Width           =   2895
   End
End
Attribute VB_Name = "frmAmountWithdrawn"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdOKEnd_Click()
    ' To play the sound, a soundcard with the Microsoft ADPCM Audio CODEC driver is
    ' necessary. This driver is included in Windows 95 and Windows NT 3.51 or later.
    BeginPlaySound I
    MsgBox LoadResString(8 + I)   ' Display a thank you message.
    Unload Me
End Sub

Private Sub Form_Load()
    Dim ConversionValue As Currency
    ConversionValue = ConversionTable((I - 16) \ 32)
    imgFlag = LoadResPicture(I, vbResBitmap)
    lblAmountWith = LoadResString(9 + I) & " " & _
        IIf(frmInput.optChecking, LoadResString(3 + I), LoadResString(4 + I))
    lblCurrency = LoadResString(10 + I)
    cmdOKEnd.Caption = LoadResString(6 + I)
    fraMoney = "1 USD ($) = " & CStr(ConversionValue) & " " & lblCurrency.Caption
    txtUSDollarsAmt = Val(frmInput.txtUSDollarsAmt.Text)
    txtConvertedAmt = ConversionValue * CCur(txtUSDollarsAmt.Text)
    SetCursor cmdOKEnd
    
    ' For DBCS, the font name must be set by VB when the
    ' project is loaded. To have this sample working everywhere
    ' we must set the font properties by program.
    lblAmountWith.FontBold = True
End Sub
