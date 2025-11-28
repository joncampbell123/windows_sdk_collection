VERSION 1.0 CLASS 
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "clsTape"
Attribute VB_Base = "0{FCFB3D2A-A0FA-1068-A738-08002B3371B5}"
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Customizable = False
'**********************************************
' Class module for the VCR sample application
' Acts as the tape transport mechanism for the
' VCR and controls "playback" of the images
'**********************************************

Option Explicit

Public Left As Integer      'Last location
Public Forward As Boolean   'Tape direction
Public Speed As Integer     'Tape speed
'**********************************************
' Purpose:  Calculates the new coordinates for
'           each animation step.
' Inputs:   Width: the width of the picture box
'           where the animation is displayed
'**********************************************
Public Sub Animate(Width As Integer)
    If Forward = True Then
        ' moving forward - increment the current left
        ' unless approaching the right edge
        If Left < Width - 50 Then
            Left = Left + 50
        Else
            Left = 0
        End If
    Else
        ' moving forward - decrement the current left
        ' unless approaching the left edge
        If Left > 0 Then
            Left = Left - 50
        Else
            Left = Width - 50
        End If
    End If
End Sub

Private Sub Class_Initialize()
    ' Initialize the class properties
    Forward = True
    Left = 0
    Speed = 300
End Sub
