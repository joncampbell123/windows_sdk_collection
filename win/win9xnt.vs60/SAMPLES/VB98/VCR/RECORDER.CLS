VERSION 1.0 CLASS 
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "clsRecorder"
Attribute VB_Base = "0{FCFB3D2A-A0FA-1068-A738-08002B3371B5}"
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Customizable = False
'**********************************************
' Class module for the VCR sample application
' Acts as the logic module for the VCR to store
' data and track recording activity
'**********************************************

Option Explicit

Public Channel As Integer   'Channel to record
Public StartRecording As Variant    'Start time
Public StopRecording As Variant     'End time
Public Enabled As Boolean   'Recording mode

Private Sub Class_Initialize()
    ' Initialize the class properties
    Enabled = True
    Channel = 3
    StartRecording = Empty
    StopRecording = Empty
End Sub
