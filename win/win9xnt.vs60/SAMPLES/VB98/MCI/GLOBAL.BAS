Attribute VB_Name = "Module1"
' Public Constants
Public Const conMCIAppTitle = "MCI Control Application"

' These constants are defined in mmsystem.h.
Public Const conMCIErrInvalidDeviceID = 30257
Public Const conMCIErrDeviceOpen = 30263
Public Const conMCIErrCannotLoadDriver = 30266
Public Const conMCIErrUnsupportedFunction = 30274
Public Const conMCIErrInvalidFile = 30304

#If Win32 Then
    Declare Function GetFocus Lib "User32" () As Long
#Else
    Declare Function GetFocus Lib "User" () As Integer
#End If

' Public variables
Public DialogCaption As String

Public Sub DisplayWarning()
    Dim WMsg As String
    Dim WTitle As String
   
    WMsg = "When you insert an audio CD in Windows 95 or Windows NT, a default audio CD player may open and begin playing the audio CD automatically. If this occurs, select the default audio CD player from the Windows Taskbar and close it before proceeding."
    WTitle = "Warning"
    
    MsgBox WMsg, vbOKOnly, WTitle
End Sub

