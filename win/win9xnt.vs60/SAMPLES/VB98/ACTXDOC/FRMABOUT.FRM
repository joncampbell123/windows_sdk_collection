VERSION 5.00
Begin VB.Form frmAbout 
   BackColor       =   &H00FFFF80&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "About BIBLIO"
   ClientHeight    =   1920
   ClientLeft      =   1995
   ClientTop       =   4455
   ClientWidth     =   5595
   ControlBox      =   0   'False
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1920
   ScaleWidth      =   5595
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton Command1 
      BackColor       =   &H00FFFF80&
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   2160
      TabIndex        =   0
      Top             =   1320
      Width           =   1215
   End
   Begin VB.Label Label3 
      BackColor       =   &H00FFFF80&
      Caption         =   "Copyright (c) 1995 Microsoft Corporation"
      Height          =   255
      Left            =   1320
      TabIndex        =   3
      Top             =   960
      Width           =   2895
   End
   Begin VB.Label Label2 
      BackColor       =   &H00FFFF80&
      Caption         =   "Data Control Sample Application"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   1320
      TabIndex        =   2
      Top             =   480
      Width           =   3015
   End
   Begin VB.Label Label1 
      BackColor       =   &H00FFFF80&
      Caption         =   "BIBLIO -- Bibliography Database Browser"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   120
      Width           =   5175
   End
End
Attribute VB_Name = "frmAbout"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub Command1_Click()
    Unload frmAbout
End Sub


