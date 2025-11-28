VERSION 5.00
Begin VB.Form frmOpen 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Automated Teller Machine (A.T.M.)"
   ClientHeight    =   3768
   ClientLeft      =   2268
   ClientTop       =   1980
   ClientWidth     =   5076
   Icon            =   "openbank.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3768
   ScaleWidth      =   5076
   Begin VB.Frame Frame 
      Caption         =   "Please select a language:"
      Height          =   2175
      Left            =   240
      TabIndex        =   10
      Top             =   1320
      Width           =   4575
      Begin VB.CommandButton Command1 
         Caption         =   "English"
         Height          =   375
         Left            =   840
         TabIndex        =   0
         Top             =   480
         Width           =   1095
      End
      Begin VB.CommandButton Command2 
         Caption         =   "Francais"
         Height          =   375
         Left            =   840
         TabIndex        =   1
         Top             =   960
         Width           =   1095
      End
      Begin VB.CommandButton Command3 
         Caption         =   "Deutsch"
         Height          =   375
         Left            =   840
         TabIndex        =   2
         Top             =   1440
         Width           =   1095
      End
      Begin VB.CommandButton Command4 
         Caption         =   "Italiano"
         Height          =   375
         Left            =   3000
         TabIndex        =   3
         Top             =   480
         Width           =   1095
      End
      Begin VB.CommandButton Command5 
         Caption         =   "Espanol"
         Height          =   375
         Left            =   3000
         TabIndex        =   4
         Top             =   960
         Width           =   1095
      End
      Begin VB.CommandButton Command6 
         Caption         =   "Japan"
         Height          =   375
         Left            =   3000
         TabIndex        =   5
         Top             =   1440
         Visible         =   0   'False
         Width           =   1095
      End
      Begin VB.Image Image 
         Height          =   375
         Index           =   0
         Left            =   360
         Picture         =   "openbank.frx":000C
         Stretch         =   -1  'True
         Top             =   480
         Width           =   375
      End
      Begin VB.Image Image 
         Height          =   375
         Index           =   1
         Left            =   360
         Picture         =   "openbank.frx":0316
         Stretch         =   -1  'True
         Top             =   960
         Width           =   375
      End
      Begin VB.Image Image 
         Height          =   375
         Index           =   2
         Left            =   360
         Picture         =   "openbank.frx":0620
         Stretch         =   -1  'True
         Top             =   1440
         Width           =   375
      End
      Begin VB.Image Image 
         Height          =   375
         Index           =   3
         Left            =   2520
         Picture         =   "openbank.frx":092A
         Stretch         =   -1  'True
         Top             =   480
         Width           =   375
      End
      Begin VB.Image Image 
         Height          =   375
         Index           =   4
         Left            =   2520
         Picture         =   "openbank.frx":0C34
         Stretch         =   -1  'True
         Top             =   960
         Width           =   375
      End
      Begin VB.Image Image 
         Height          =   375
         Index           =   5
         Left            =   2520
         Picture         =   "openbank.frx":0F3E
         Stretch         =   -1  'True
         Top             =   1440
         Visible         =   0   'False
         Width           =   375
      End
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   """Custom service around the world"""
      Height          =   240
      Index           =   3
      Left            =   135
      TabIndex        =   9
      Top             =   840
      Width           =   4700
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "Automated Teller Machine"
      Height          =   240
      Index           =   2
      Left            =   135
      TabIndex        =   8
      Top             =   600
      Width           =   4700
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "Welcome to the "
      Height          =   240
      Index           =   0
      Left            =   135
      TabIndex        =   7
      Top             =   120
      Width           =   4700
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "WORLD TRAVELER BANK"
      Height          =   240
      Index           =   1
      Left            =   135
      TabIndex        =   6
      Top             =   360
      Width           =   4700
   End
End
Attribute VB_Name = "frmOpen"
Attribute VB_Base = "0{46B50776-F609-11CF-8A54-0020AF939EAB}"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' Each command button represents a language. If the user chooses English, the application
' will load arguments with ID numbers from 16 to 47.
Option Explicit

Sub command1_click()
    I = 16              ' User chose English.
    frmInput.Show vbModal
End Sub

Sub Command2_Click()
    I = 48              ' User chose French.
    frmInput.Show vbModal
End Sub

Sub Command3_Click()
    I = 80              ' User chose German.
    frmInput.Show vbModal
End Sub

Sub Command4_Click()
    I = 112              ' User chose Italian.
    frmInput.Show vbModal
End Sub

Sub Command5_Click()
    I = 144              ' User chose Spanish.
    frmInput.Show vbModal
End Sub

Sub Command6_Click()     ' Japanese is only available on DBCS systems
    I = 176              ' User chose Japanese.
    frmInput.Show vbModal
End Sub

Private Sub Form_Load()
    Dim iIndex As Byte
    
    ' Initialize the currency conversion table.
    ConversionTable_Initialize
    
    ' Initialize the mouse icon cursor for the button.
    Cursor_Initialize
    SetCursor Command1
    SetCursor Command2
    SetCursor Command3
    SetCursor Command4
    SetCursor Command5
    SetCursor Command6
    
    ' For DBCS, the font name must be set by VB when the
    ' project is loaded. To have this sample working everywhere
    ' we must set the font properties by program.
    For iIndex = 0 To 3
        Label(iIndex).FontSize = 10
        Label(iIndex).FontBold = True
    Next iIndex
    
    ' For DBCS, HighAnsi characters can not be read by Far East
    ' platforms. Because some of these characters are used on the
    ' languages buttons, we need to change then in this case.
    If IMEStatus = vbIMENoOp Then
        Command2.Caption = "Français"
        Command5.Caption = "Español"
    End If
End Sub
