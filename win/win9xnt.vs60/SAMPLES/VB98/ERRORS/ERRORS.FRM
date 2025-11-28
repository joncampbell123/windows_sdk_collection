VERSION 5.00
Begin VB.Form frmErrors 
   Caption         =   "Error Handling Example"
   ClientHeight    =   2670
   ClientLeft      =   1140
   ClientTop       =   1530
   ClientWidth     =   5295
   LinkTopic       =   "Form1"
   ScaleHeight     =   2670
   ScaleWidth      =   5295
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdMakeError 
      Caption         =   "Generate an Error"
      Height          =   375
      Left            =   2760
      TabIndex        =   4
      Top             =   1920
      Width           =   2055
   End
   Begin VB.CommandButton cmdCentral 
      Caption         =   "Centralized Error Handling"
      Height          =   375
      Left            =   480
      TabIndex        =   3
      Top             =   1920
      Width           =   2055
   End
   Begin VB.TextBox txtFileSpec 
      Height          =   375
      Left            =   480
      TabIndex        =   2
      Text            =   "*:\error.xyz"
      Top             =   600
      Width           =   3255
   End
   Begin VB.CommandButton cmdInline 
      Caption         =   "Inline Error Handling"
      Height          =   375
      Left            =   2760
      TabIndex        =   1
      Top             =   1320
      Width           =   2055
   End
   Begin VB.CommandButton cmdNone 
      Caption         =   "No Error Handling"
      Height          =   375
      Left            =   480
      TabIndex        =   0
      Top             =   1320
      Width           =   2055
   End
   Begin VB.Label Label1 
      Caption         =   "Enter a invalid File Spec here:"
      Height          =   255
      Left            =   480
      TabIndex        =   5
      Top             =   360
      Width           =   3375
   End
End
Attribute VB_Name = "frmErrors"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdCentral_Click()
    Dim strErrnum As String
    Dim intErr As Integer
    Dim intReturn As Integer
    
    On Error GoTo CallFileErrors
    strErrnum = InputBox("Enter an error number", "Errors", "68")
    intErr = Val(strErrnum)
    Err.Raise Number:=intErr
    Exit Sub
CallFileErrors:
    intReturn = FileErrors()
    If intReturn = 0 Then
        Resume
    ElseIf intReturn = 1 Then
        Resume Next
    ElseIf intReturn = 2 Then
        MsgBox "Unrecoverable Error"
        End
    Else
        MsgBox "Unknown Error"
        Resume Next
    End If
        
End Sub

Private Sub cmdInline_Click()
    Dim blnResult As Boolean
    
    blnResult = FileExists2(txtFileSpec.Text)
End Sub


Private Sub cmdMakeError_Click()
    Err.Raise Number:=71        ' Simulate "Disk Not Ready" error.
End Sub

Private Sub cmdNone_Click()
    Dim blnResult As Boolean
    
    blnResult = FileExists1(txtFileSpec.Text)
End Sub

Function FileExists1(filename) As Boolean
    ' No error handling
    FileExists1 = (Dir(filename) <> "")
End Function
Function FileExists2(filename) As Boolean
    Dim Msg As String
    ' Turn on error trapping so error handler responds
    ' if any error is detected.
    On Error GoTo CheckError
        FileExists2 = (Dir(filename) <> "")
        ' Avoid executing error handler if no error occurs.
        Exit Function

CheckError:                 ' Branch here if error occurs.
    ' Define constants to represent intrinsic Visual Basic error
    ' codes.
    Const mnErrDiskNotReady = 71, mnErrDeviceUnavailable = 68
    ' vbExclamation, vbOK, vbCancel, vbCritical, and vbOKCancel are
    'constants defined in the VBA type library.
    If (Err.Number = mnErrDiskNotReady) Then
        Msg = "Put a floppy disk in the drive and close the door."
        ' Display message box with an exclamation mark icon and with
        ' OK and Cancel buttons.
        If MsgBox(Msg, vbExclamation & vbOKCancel) = vbOK Then
            Resume
        Else
            Resume Next
        End If
    ElseIf Err.Number = mnErrDeviceUnavailable Then
        Msg = "This drive or path does not exist: " & filename
        MsgBox Msg, vbExclamation
        Resume Next
    Else
        Msg = "Unexpected error #" & Str(Err.Number) & " occurred: " _
        & Err.Description
        ' Display message box with Stop sign icon and OK button.
        MsgBox Msg, vbCritical
        Stop
    End If
    Resume
End Function

