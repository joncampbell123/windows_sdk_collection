Attribute VB_Name = "AMSample"
' ActiveMovie OCX Sample Code
' Copyright (c) 1996 - 1997 Microsoft Corporation
' All Rights Reserved
    
Option Explicit

' Counters for Timer, PositionChange, and StateChange events.
Global g_cTimer As Long
Global g_cPositionChange As Long
Global g_cStateChange As Long

' Tracks file name extension and whether or not a file is currently open.
Global g_FileExtension As String
Global g_FileOpened As Boolean
    

Sub Main()

    ' Main entry point to ActiveMovie OCX Sample application.
    
    ' Load main form, position and show.
    Load frmMain
    With frmMain
        .Top = Screen.Height * 0.05
        .Left = Screen.Width * 0.05
        .Visible = True
    End With
    
    ' Load viewer form but don't show yet.
    Load frmViewer
    With frmViewer
        .Visible = False
        .Left = frmMain.Left + frmMain.Width
        .Top = frmMain.Top
    End With
    
    ' Initialize global variables.
    g_FileOpened = False
    g_FileExtension = ""
    
End Sub


Sub ResizeViewer()

    ' Resize form to dimensions of ActiveMovie control + nonclient region.
    With frmViewer
        .Visible = False
        .Height = .ActiveMovie1.Height + (.Height - .ScaleHeight)
        .Width = .ActiveMovie1.Width + (.Width - .ScaleWidth)
        .Visible = True
    End With
    
End Sub



Sub UpdateStatusBar()

    ' Update the main form status bar to show the current number of events.

    With frmMain.StatusBar1
         .Panels(1).Text = "Timer Events: " & g_cTimer
         .Panels(2).Text = "State Changes: " & g_cStateChange
         .Panels(3).Text = "Position Changes: " & g_cPositionChange
    End With
    
End Sub

  

 
