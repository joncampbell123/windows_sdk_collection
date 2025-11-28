VERSION 5.00
Begin VB.Form frmCYOCC 
   Caption         =   "Creating Your Own Collection Classes"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form2"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdBricks 
      Caption         =   "House of Bricks"
      Height          =   375
      Left            =   240
      TabIndex        =   2
      Top             =   1200
      Width           =   3255
   End
   Begin VB.CommandButton cmdSticks 
      Caption         =   "House of Sticks"
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   3255
   End
   Begin VB.CommandButton cmdStraw 
      Caption         =   "House of Straw"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   3255
   End
End
Attribute VB_Name = "frmCYOCC"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
' frmCYOCC shows its subsidiary forms using
'   the hidden global variables Visual
'   Basic creates for each form class
'   (frmStraw, frmSticks, and frmBricks,
'   in this case).  The hidden global
'   variable is described in "Life Cycle
'   of Visual Basic Forms" in Books Online.
'
' The forms are shown modeless, and
'   frmCYOCC specifies itself (Me) as the
'   owner of each form.  As a result, the
'   owned forms always appear on top of
'   frmCYOCC.

' House of Straw -- the public Collection object.
Private Sub cmdStraw_Click()
    frmStraw.Show vbModeless, Me
End Sub

' House of Sticks -- the private Collection object.
Private Sub cmdSticks_Click()
    frmSticks.Show vbModeless, Me
End Sub

' House of Bricks -- the Collection Class.
Private Sub cmdBricks_Click()
    frmBricks.Show vbModeless, Me
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' If any of the modeless forms are
    '   still loaded, close them.
    If Not frmBricks Is Nothing Then
        Unload frmBricks
        Set frmBricks = Nothing
    End If
    If Not frmSticks Is Nothing Then
        Unload frmBricks
        Set frmBricks = Nothing
    End If
    If Not frmStraw Is Nothing Then
        Unload frmBricks
        Set frmBricks = Nothing
    End If
    
    ' Set the hidden global for frmCYOCC
    '   to Nothing, freeing its resources.
    Set frmCYOCC = Nothing
End Sub
