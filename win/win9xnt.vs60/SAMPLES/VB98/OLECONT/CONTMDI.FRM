VERSION 5.00
Begin VB.MDIForm ParentForm 
   BackColor       =   &H8000000C&
   Caption         =   "OLE Container Control Demo"
   ClientHeight    =   5205
   ClientLeft      =   765
   ClientTop       =   1770
   ClientWidth     =   8025
   LinkTopic       =   "MDIForm1"
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      NegotiatePosition=   1  'Left
      Begin VB.Menu mnuFileNew 
         Caption         =   "&New..."
      End
      Begin VB.Menu mnuOpen 
         Caption         =   "&Open..."
      End
      Begin VB.Menu sep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
      Begin VB.Menu sep2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuAbout 
         Caption         =   "A&bout..."
      End
   End
End
Attribute VB_Name = "ParentForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub MDIForm_Load()
    ChDir App.Path
End Sub

Private Sub mnuAbout_Click()
    DisplayInstructions
End Sub

Private Sub mnuExit_Click()
    End
End Sub

Private Sub mnuFileNew_Click()
    NewObject
End Sub

Private Sub mnuOpen_Click()
    OpenObject
End Sub

