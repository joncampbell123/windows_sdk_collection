Attribute VB_Name = "Module1"
Option Explicit

Declare Function timeGetTime Lib "winmm.dll" () As Long

' CoffeeWatch must be an ActiveX Exe, because
'   it exposes the public NotifyMe object
'   used for the call-back method demo.  It
'   will never be started as a server, only
'   as a standalone application.
Sub Main()
    If App.StartMode = vbSModeStandalone Then
        Form1.Show
    End If
End Sub
