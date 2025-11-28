VERSION 5.00
Begin VB.Form frmAbout 
   Appearance      =   0  'Flat
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "About - Optimizing Visual Basic 6.0"
   ClientHeight    =   2208
   ClientLeft      =   2040
   ClientTop       =   2040
   ClientWidth     =   4836
   ClipControls    =   0   'False
   ControlBox      =   0   'False
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   7.8
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H80000008&
   Icon            =   "ABOUT.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2208
   ScaleWidth      =   4836
   ShowInTaskbar   =   0   'False
   WhatsThisHelp   =   -1  'True
   Begin VB.CommandButton ExitButton 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      Caption         =   "&OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   3700
      TabIndex        =   3
      Top             =   135
      Width           =   1050
   End
   Begin VB.Image Image1 
      Height          =   384
      Left            =   192
      Picture         =   "ABOUT.frx":030A
      Top             =   192
      Width           =   384
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00808080&
      Index           =   1
      X1              =   915
      X2              =   4615
      Y1              =   1260
      Y2              =   1260
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00FFFFFF&
      BorderWidth     =   2
      Index           =   0
      X1              =   900
      X2              =   4600
      Y1              =   1275
      Y2              =   1275
   End
   Begin VB.Label lblDesc 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      BackStyle       =   0  'Transparent
      Caption         =   "Optimizing Visual Basic Code Sample Application."
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   7.8
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00800000&
      Height          =   465
      Left            =   960
      TabIndex        =   0
      Top             =   720
      Width           =   2175
   End
   Begin VB.Label lblCredits 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      BackStyle       =   0  'Transparent
      Caption         =   " © 1998 - Microsoft Corporation"
      ForeColor       =   &H00800000&
      Height          =   435
      Left            =   900
      TabIndex        =   1
      Top             =   1395
      Width           =   3315
   End
   Begin VB.Label lblAppInfo 
      Appearance      =   0  'Flat
      BackColor       =   &H00C0C0C0&
      BackStyle       =   0  'Transparent
      ForeColor       =   &H00800000&
      Height          =   525
      Left            =   900
      TabIndex        =   2
      Top             =   195
      Width           =   2655
   End
End
Attribute VB_Name = "frmAbout"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub ExitButton_Click()
    
    Unload Me

End Sub

Private Sub Form_Load()
    
    CenterMe Me
    DoEvents
    
    lblAppInfo = "Optimizing Visual Basic 6.0" & _
                      vbCrLf & _
                      "Ver. " & App.Major & "." & App.Minor

End Sub

Private Sub Picture1_DblClick()
    
End Sub

