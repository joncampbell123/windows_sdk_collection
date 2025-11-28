VERSION 5.00
Begin VB.Form frmButton 
   Caption         =   "Test Buttons"
   ClientHeight    =   2790
   ClientLeft      =   1815
   ClientTop       =   2205
   ClientWidth     =   4770
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Height          =   3195
   Left            =   1755
   LinkTopic       =   "Form3"
   ScaleHeight     =   2790
   ScaleWidth      =   4770
   Top             =   1860
   Width           =   4890
   Begin VB.CommandButton cmdClose 
      Caption         =   "&Close"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   3000
      TabIndex        =   1
      Top             =   1320
      Width           =   1215
   End
   Begin VB.CommandButton cmdChange 
      Caption         =   "Change &Signal"
      Default         =   -1  'True
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   600
      TabIndex        =   0
      Top             =   1320
      Width           =   1815
   End
   Begin VB.Label lblHelp 
      Caption         =   "To change the signal, click either the Change Signal button or the traffic signal icon itself."
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   600
      TabIndex        =   2
      Top             =   2040
      Width           =   3615
   End
   Begin VB.Image imgRed 
      Appearance      =   0  'Flat
      Height          =   480
      Left            =   2160
      Picture         =   "BUTTON.frx":0000
      Top             =   480
      Visible         =   0   'False
      Width           =   480
   End
   Begin VB.Image imgYellow 
      Height          =   480
      Left            =   2160
      Picture         =   "BUTTON.frx":030A
      Top             =   480
      Visible         =   0   'False
      Width           =   480
   End
   Begin VB.Image imgGreen 
      Height          =   480
      Left            =   2160
      Picture         =   "BUTTON.frx":0614
      Top             =   480
      Width           =   480
   End
End
Attribute VB_Name = "frmButton"
Attribute VB_Base = "0{1D936797-C9EF-11CF-84BA-00AA00C007F0}"
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Attribute VB_Customizable = False

Private Sub ChangeSignal()
    ' Check to see what color the light is, and then change
    ' it to the next color.  The order is green, yellow,
    ' and then red.
    If imgGreen.Visible = True Then
        imgGreen.Visible = False
        imgYellow.Visible = True
    ElseIf imgYellow.Visible = True Then
        imgYellow.Visible = False
        imgRed.Visible = True
    Else
        imgRed.Visible = False
        imgGreen.Visible = True
    End If
End Sub

Private Sub cmdChange_Click()
    Call ChangeSignal        ' Call the ChangeSignal procedure.
End Sub

Private Sub cmdClose_Click()
   Unload Me            ' Unload this form.
End Sub

Private Sub imgGreen_Click()
    Call ChangeSignal        ' Call the ChangeSignal procedure.
End Sub

Private Sub imgRed_Click()
    Call ChangeSignal        ' Call the ChangeSignal procedure.
End Sub

Private Sub imgYellow_Click()
    Call ChangeSignal        ' Call the ChangeSignal procedure.
End Sub

