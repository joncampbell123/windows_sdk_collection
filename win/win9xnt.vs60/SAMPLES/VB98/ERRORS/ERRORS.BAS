Attribute VB_Name = "Errors"
Option Explicit
Const mnErrDeviceUnavailable = 68
Const mnErrDiskNotReady = 71
Const mnErrDeviceIO = 57
Const mnErrDiskFull = 61
Const mnErrBadFileName = 64
Const mnErrBadFileNameOrNumber = 52
Const mnErrPathDoesNotExist = 76
Const mnErrBadFileMode = 54
Const mnErrFileAlreadyOpen = 55
Const mnErrInputPastEndOfFile = 62
Function FileErrors() As Integer
    Dim intMsgType As Integer
    Dim strMsg As String
    Dim intResponse As Integer
    ' Return Value      Meaning
    ' 0                 Resume
    ' 1                 Resume Next
    ' 2                 Unrecoverable error
    ' 3                 Unrecognized error
    intMsgType = vbExclamation
    Select Case Err.Number
        Case mnErrDeviceUnavailable             ' Error 68
            strMsg = "That device appears unavailable."
            intMsgType = vbExclamation + vbOKCancel
        Case mnErrDiskNotReady                  ' Error 71
            strMsg = "Insert a disk in the drive and close the door."
            intMsgType = vbExclamation + vbOKCancel
        Case mnErrDeviceIO                      ' Error 57
            strMsg = "Internal disk error."
            intMsgType = vbExclamation + vbOKOnly
        Case mnErrDiskFull                      ' Error 61
            strMsg = "Disk is full. Continue?"
            intMsgType = vbExclamation + vbAbortRetryIgnore
        Case mnErrBadFileName, mnErrBadFileNameOrNumber ' Error 64 & 52
            strMsg = "That filename is illegal."
            intMsgType = vbExclamation + vbOKCancel
        Case mnErrPathDoesNotExist                ' Error 76
            strMsg = "That path doesn't exist."
            intMsgType = vbExclamation + vbOKCancel
        Case mnErrBadFileMode                     ' Error 54
            strMsg = "Can't open your file for that type of access."
        Case mnErrFileAlreadyOpen             ' Error 55
            strMsg = "This file is already open."
            intMsgType = vbExclamation + vbOKOnly
        Case mnErrInputPastEndOfFile              ' Error 62
            strMsg = "This file has a nonstandard end-of-file marker, "
            strMsg = strMsg & "or an attempt was made to read beyond "
            strMsg = strMsg & "the end-of-file marker."
            intMsgType = vbExclamation + vbAbortRetryIgnore
        Case Else
            FileErrors = 3
            Exit Function
    End Select
    intResponse = MsgBox(strMsg, intMsgType, "Disk Error")
    Select Case intResponse
        Case 1, 4       ' OK, Retry buttons.
            FileErrors = 0
        Case 2, 5       ' Cancel, Ignore buttons.
            FileErrors = 1
        Case 3          ' Abort button.
            FileErrors = 2
        Case Else
            FileErrors = 3
    End Select
End Function

