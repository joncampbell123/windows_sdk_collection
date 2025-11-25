VERSION 5.00
Object = "{05589FA0-C356-11CE-BF01-00AA0055595A}#2.0#0"; "amovie.ocx"
Begin VB.Form frmViewer 
   Caption         =   "ActiveMovie OCX Viewer"
   ClientHeight    =   1305
   ClientLeft      =   9840
   ClientTop       =   5475
   ClientWidth     =   4065
   ControlBox      =   0   'False
   Icon            =   "viewer.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1305
   ScaleWidth      =   4065
   Begin AMovieCtl.ActiveMovie ActiveMovie1 
      Height          =   1200
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   3990
      _ExtentX        =   7038
      _ExtentY        =   2117
   End
End
Attribute VB_Name = "frmViewer"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False


Private Sub ActiveMovie1_PositionChange(ByVal oldPosition As Double, ByVal newPosition As Double)
    
    ' Tracks PositionChange events.
    
    g_cPositionChange = g_cPositionChange + 1
    UpdateStatusBar
    
End Sub

Private Sub ActiveMovie1_StateChange(ByVal oldState As Long, ByVal newState As Long)
    
    ' Tracks StateChange events.
    g_cStateChange = g_cStateChange + 1
    UpdateStatusBar

End Sub


Private Sub ActiveMovie1_ReadyStateChange(ReadyState As ReadyStateConstants)
If ReadyState = amvComplete Then
 ResizeViewer
End If
End Sub

Private Sub ActiveMovie1_Timer()
    
    ' Tracks Timer events.
    
    g_cTimer = g_cTimer + 1
    UpdateStatusBar
    
End Sub


Private Sub Form_Load()

    ' ActiveMovie OCX Sample Code
    ' Copyright (c) 1996 - 1997 Microsoft Corporation
    ' All Rights Reserved
    
    ' See the ActiveMovie1 object for events.

End Sub


Private Sub sdmen_Click()
MsgBox Str$(ActiveMovie1.Width) + Str$(ActiveMovie1.Height)
End Sub
