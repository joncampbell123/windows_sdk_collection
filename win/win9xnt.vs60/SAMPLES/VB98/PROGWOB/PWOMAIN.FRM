VERSION 5.00
Begin VB.Form frmProgWObMain 
   Caption         =   "Programming with Objects"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form2"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdImplements 
      Caption         =   "Polymorphism and the &Implements keyword"
      Height          =   375
      Left            =   240
      TabIndex        =   3
      Top             =   1680
      Width           =   4095
   End
   Begin VB.CommandButton cmdEvents 
      Caption         =   "Raising and Handling &Events"
      Height          =   375
      Left            =   240
      TabIndex        =   2
      Top             =   1200
      Width           =   4095
   End
   Begin VB.CommandButton cmdFriends 
      Caption         =   "Passing UDTs between objects with &Friend Functions"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   4095
   End
   Begin VB.CommandButton cmdCYOCC 
      Caption         =   "&Creating Your Own Collection Classes"
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   4095
   End
End
Attribute VB_Name = "frmProgWObMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
' frmProgWObMain shows other forms using
'   the hidden global variables Visual
'   Basic creates for each form class
'   (frmFriends, frmCYOCC, ______, and _____,
'   in this case).  The hidden global
'   variable is described in "Life Cycle
'   of Visual Basic Forms" in Books Online.
'
' The forms are shown modeless, and
'   frmProgWObMain specifies itself (Me)
'   as the owner of each form.  As a
'   result, the owned forms always appear
'   on top of frmProgWObMain.

Private Sub cmdEvents_Click()
    frmEvents.Show vbModeless, Me
End Sub

' Friend Members Passing UDTs
Private Sub cmdFriends_Click()
    frmFriends.Show vbModeless, Me
End Sub

' Creating Your Own Collection Classes
Private Sub cmdCYOCC_Click()
    frmCYOCC.Show vbModeless, Me
End Sub

Private Sub cmdImplements_Click()
    frmImplements.Show vbModeless, Me
End Sub

Private Sub Form_Unload(Cancel As Integer)
    If frmFriends Is Nothing Then
        Unload frmFriends
        Set frmFriends = Nothing
    End If
    If frmCYOCC Is Nothing Then
        Unload frmCYOCC
        Set frmCYOCC = Nothing
    End If
    If frmEvents Is Nothing Then
        Unload frmEvents
        Set frmEvents = Nothing
    End If
    If frmImplements Is Nothing Then
        Unload frmImplements
        Set frmImplements = Nothing
    End If
End Sub
