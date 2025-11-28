VERSION 5.00
Begin VB.Form frmOptions 
   Caption         =   "Options"
   ClientHeight    =   3825
   ClientLeft      =   3255
   ClientTop       =   2400
   ClientWidth     =   5040
   Height          =   4230
   Left            =   3195
   LinkTopic       =   "Form1"
   ScaleHeight     =   3825
   ScaleWidth      =   5040
   Top             =   2055
   Width           =   5160
   Begin VB.OptionButton opt686 
      Caption         =   "P&entium Pro"
      Height          =   255
      Left            =   360
      TabIndex        =   2
      Top             =   2040
      Width           =   1335
   End
   Begin VB.OptionButton opt586 
      Caption         =   "&Pentium"
      Height          =   255
      Left            =   360
      TabIndex        =   1
      Top             =   1560
      Width           =   1335
   End
   Begin VB.CommandButton cmdClose 
      Caption         =   "&Close"
      Height          =   495
      Left            =   3480
      TabIndex        =   5
      Top             =   2880
      Width           =   1095
   End
   Begin VB.OptionButton opt486 
      Caption         =   "&486"
      Height          =   255
      Left            =   360
      TabIndex        =   0
      Top             =   1080
      Value           =   -1  'True
      Width           =   1575
   End
   Begin VB.Frame fraSystem 
      Caption         =   "Operating System"
      Height          =   1455
      Left            =   2400
      TabIndex        =   7
      Top             =   960
      Width           =   2175
      Begin VB.OptionButton optWin95 
         Caption         =   "&Windows 95"
         Height          =   255
         Left            =   360
         TabIndex        =   3
         Top             =   360
         Value           =   -1  'True
         Width           =   1335
      End
      Begin VB.OptionButton optWinNT 
         Caption         =   "Windows &NT"
         Height          =   255
         Left            =   360
         TabIndex        =   4
         Top             =   840
         Width           =   1335
      End
   End
   Begin VB.Label lblHelp 
      Caption         =   "Select a processor and operating system"
      Height          =   615
      Left            =   360
      TabIndex        =   8
      Top             =   2880
      Width           =   2295
   End
   Begin VB.Label lblDisplay 
      BackColor       =   &H00C0C0C0&
      BorderStyle     =   1  'Fixed Single
      Height          =   255
      Left            =   360
      TabIndex        =   6
      Top             =   360
      Width           =   4215
   End
End
Attribute VB_Name = "frmOptions"
Attribute VB_Base = "0{1D9367AF-C9EF-11CF-84BA-00AA00C007F0}"
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Attribute VB_Customizable = False
' set up two string variables to hold the captions
Dim strComputer As String
Dim strSystem As String

Sub DisplayCaption()
    ' concatenate the caption with the two string
    ' variables.
    lblDisplay.Caption = "You selected a " & _
     strComputer & " running " & strSystem
End Sub


Private Sub cmdClose_Click()
    Unload Me   'unload the form
End Sub

Private Sub Form_Load()
    ' invoke a Click event in the default options
    ' to update the label caption
    opt486_Click
    optWin95_Click
End Sub

Private Sub opt486_Click()
    ' assign a value to the first string variable
    strComputer = "486"
    ' call the subroutine
    Call DisplayCaption
End Sub


Private Sub opt586_Click()
    ' assign a value to the first string variable
    strComputer = "Pentium"
    ' call the subroutine
    Call DisplayCaption
End Sub


Private Sub opt686_Click()
    ' assign a value to the first string variable
    strComputer = "Pentium Pro"
    ' call the subroutine
    Call DisplayCaption
End Sub


Private Sub optWin95_Click()
    ' assign a value to the second string variable
    strSystem = "Windows 95"
    ' call the subroutine
    Call DisplayCaption
End Sub


Private Sub optWinNT_Click()
    ' assign a value to the second string variable
    strSystem = "Windows NT"
    ' call the subroutine
    Call DisplayCaption
End Sub


