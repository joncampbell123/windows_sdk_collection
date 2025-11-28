Attribute VB_Name = "Module2"
' Force all run-time errors to be handled here.
Sub DisplayErrorMessageBox()
    Dim Msg As String
    Select Case Err
        Case conMCIErrCannotLoadDriver
            Msg = "Error load media device driver."
        Case conMCIErrDeviceOpen
            Msg = "The device is not open or is not known."
        Case conMCIErrInvalidDeviceID
            Msg = "Invalid device id."
        Case conMCIErrInvalidFile
            Msg = "Invalid filename."
        Case conMCIErrUnsupportedFunction
            Msg = "Action not available for this device."
        Case Else
            Msg = "Unknown error (" + Str$(Err) + ")."
    End Select

    MsgBox Msg, 48, conMCIAppTIitle
End Sub

' This procedure allows any Windows event to be processed.
' This may be necessary to solve any synchronization
' problems with Windows events.
' This procedure can also be used to force a delay in
' processing.
Sub WaitForEventsToFinish(NbrTimes As Integer)
    Dim i As Integer

    For i = 1 To NbrTimes
        dummy% = DoEvents()
    Next i
End Sub


