VERSION 5.00
Object = "{C1A8AF28-1257-101B-8FB0-0020AF039CA3}#1.1#0"; "MCI32.OCX"
Begin VB.Form frmWave 
   BorderStyle     =   1  'Fixed Single
   ClientHeight    =   1380
   ClientLeft      =   1350
   ClientTop       =   4230
   ClientWidth     =   5715
   LinkMode        =   1  'Source
   LinkTopic       =   "Form4"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1380
   ScaleWidth      =   5715
   Begin VB.HScrollBar hsbWave 
      Height          =   255
      Left            =   240
      Max             =   100
      TabIndex        =   0
      Top             =   360
      Width           =   5295
   End
   Begin MCI.MMControl mciWave 
      Height          =   495
      Left            =   1080
      TabIndex        =   3
      Top             =   720
      Width           =   3540
      _ExtentX        =   6244
      _ExtentY        =   873
   End
   Begin VB.Label lblWaveSec 
      BeginProperty Font 
         Name            =   "Courier"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   4800
      TabIndex        =   2
      Top             =   0
      Width           =   735
   End
   Begin VB.Label lblWave 
      BeginProperty Font 
         Name            =   "Courier"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   360
      TabIndex        =   1
      Top             =   0
      Width           =   615
   End
   Begin VB.Menu AL_FILE 
      Caption         =   "&File"
      Begin VB.Menu AI_OPEN 
         Caption         =   "&Open..."
      End
      Begin VB.Menu AI_SEPARATOR 
         Caption         =   "-"
      End
      Begin VB.Menu AI_EXIT 
         Caption         =   "&Exit"
      End
   End
End
Attribute VB_Name = "frmWave"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Const conInterval = 50
Const conIntervalPlus = 55

Dim CurrentValue As Double

Private Sub AI_EXIT_Click()
    Unload frmWave
End Sub

Private Sub AI_OPEN_Click()
    Dim msec As Double

    ' Set the number of milliseconds between successive
    ' StatusUpdate events.
    mciWave.UpdateInterval = 0

    ' Display the File Open dialog box.
    frmOpenDlg.dlgOpenFile.FilterIndex = 1
    frmOpenDlg.dlgOpenFile.Flags = vbOFNReadOnly Or vbOFNFileMustExist
    frmOpenDlg.dlgOpenFile.CancelError = True
    frmOpenDlg.dlgOpenFile.filename = ""
    On Error Resume Next
    frmOpenDlg.dlgOpenFile.ShowOpen

    If Err <> 0 Then
        ' No file selected from the Open File dialog box.
        Exit Sub
    End If

    ' If the device is open, close it.
    If Not mciWave.Mode = vbMCIModeNotOpen Then
        mciWave.Command = "Close"
    End If

    ' Open the device with the new filename.
    mciWave.filename = frmOpenDlg.dlgOpenFile.filename
    On Error GoTo MCI_ERROR
    mciWave.Command = "Open"
    On Error GoTo 0
    Caption = DialogCaption + mciWave.filename
    
    ' Set the timing labels on the form.
    mciWave.TimeFormat = vbMCIFormatMilliseconds
    lblWave.Caption = "0.0"
    msec = (CDbl(mciWave.Length) / 1000)
    lblWaveSec.Caption = Format$(msec, "0.00")

    ' Set the scrollbar values.
    hsbWave.value = 0
    CurrentValue = 0#
    Exit Sub

MCI_ERROR:
    DisplayErrorMessageBox
    Resume MCI_EXIT

MCI_EXIT:
    Unload frmWave
    
End Sub

Private Sub Form_Load()
    ' Force the multimedia MCI control to complete before returning
    ' to the application.
    frmWave.mciWave.Wait = True
End Sub

Private Sub Form_Unload(Cancel As Integer)
    frmMCITest.Show
End Sub

Private Sub mciWave_PauseClick(Cancel As Integer)
    ' Set the number of milliseconds between successive
    ' StatusUpdate events.
    mciWave.UpdateInterval = 0
End Sub

Private Sub mciWave_PlayClick(Cancel As Integer)
    ' Set the number of milliseconds between successive
    ' StatusUpdate events.
    mciWave.UpdateInterval = conInterval
End Sub

Private Sub mciWave_PrevClick(Cancel As Integer)
    ' Set the number of milliseconds between successive
    ' StatusUpdate events.
    mciWave.UpdateInterval = 0
    
    ' Reset the scrollbar values.
    hsbWave.value = 0
    CurrentValue = 0#
    
    mciWave.Command = "Prev"
End Sub

Private Sub mciWave_StatusUpdate()
    Dim value As Integer

    ' If the device is not playing, reset to the beginning.
    If Not mciWave.Mode = vbMCIModePlay Then
        hsbWave.value = hsbWave.Max
        mciWave.UpdateInterval = 0
        Exit Sub
    End If
    
    ' Determine how much of the file has played.  Set a
    ' value of the scrollbar between 0 and 100.
    CurrentValue = CurrentValue + conIntervalPlus
    value = CInt((CurrentValue / mciWave.Length) * 100)
    
    If value > hsbWave.Max Then
        value = 100
    End If

    hsbWave.value = value
End Sub

