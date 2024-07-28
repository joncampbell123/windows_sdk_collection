VERSION 2.00
Begin Form frmChild 
   BorderStyle     =   0  'None
   ControlBox      =   0   'False
   Height          =   750
   Left            =   2145
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   345
   ScaleWidth      =   3975
   Top             =   1005
   Width           =   4095
   Begin MCIWND MCIWnd 
      AutosizeMovie   =   0   'False
      AutosizeWindow  =   -1  'True
      ErrorDlg        =   0   'False
      Filename        =   ""
      Height          =   375
      Left            =   0
      Menu            =   -1  'True
      Playbar         =   -1  'True
      Record          =   0   'False
      Repeat          =   0   'False
      Speed           =   1000
      TimeFormat      =   ""
      TimerFreq       =   500
      Top             =   0
      Volume          =   1000
      WantPosEvent    =   -1  'True
      Width           =   3975
      Zoom            =   100
   End
End
Option Explicit

Sub MCIWnd_Error (ErrorCode As String)
    ' update to the latest error message
    mdiSample.txtError = ErrorCode
End Sub

Sub MCIWnd_MediaChange (Media As String)
    'first check if it can record
    MCIWnd.Command = "capability can record"
    CanRecord = (LCase$(MCIWnd.CommandReturn) = "true")

    ' media / device changed -- update all the details
    Call ShowMCIWndProperties(Media)
End Sub

Sub MCIWnd_ModeChange (Mode As String)
    ' update to the latest mode
    mdiSample.txtMode = Mode
End Sub

Sub MCIWnd_Notify (NotifyCode As Long)
    ' display the notification info
    mdiSample.txtNotification = NotifyStr(NotifyCode)
End Sub

Sub MCIWnd_PositionChange (Position As Long)
    ' display the latest position
    mdiSample.txtPosition = MCIWnd.PositionString
    ' if format changed, update Start, End, Length and TimeFormat
    If (StrComp(mdiSample.txtTimeFormat.Caption, MCIWnd.TimeFormat) <> 0) Then
        mdiSample.txtTimeFormat.Caption = MCIWnd.TimeFormat
        mdiSample.txtStart = Trim$(Str$(MCIWnd.Start))
        mdiSample.txtEnd = Trim$(Str$(MCIWnd.End))
        mdiSample.txtLength = Trim$(Str$(MCIWnd.Length))
    End If
End Sub

Sub MCIWnd_SizeChange ()
    ' if control size changed, resize the app main window
    Call ResizeMDIHeight    ' update app window height
    Call ResizeMDIWidth     '  ...   ...  ...   width
End Sub

Sub ShowMCIWndProperties (Media As String)
    ' first take care of the size
    Call ResizeMDIHeight    ' update app window height
    Call ResizeMDIWidth     '  ...   ...  ...   width

    ' then display all the details
    mdiSample.txtFileName = MCIWnd.Filename
    mdiSample.txtDevice = MCIWnd.Device
    mdiSample.txtStart = Trim$(Str$(MCIWnd.Start))
    mdiSample.txtEnd = Trim$(Str$(MCIWnd.End))
    mdiSample.txtLength = Trim$(Str$(MCIWnd.Length))
    mdiSample.txtError = MCIWnd.ErrorString
    mdiSample.txtTimeFormat = MCIWnd.TimeFormat
    mdiSample.txtNotification = ""   ' clear prev Notification info

    ' next set the checkboxes to show the options
    If (MCIWnd.Playbar) Then
        mdiSample.chkPlaybar.Value = 1
    Else
        mdiSample.chkPlaybar.Value = 0
    End If
    If (MCIWnd.Menu) Then
        mdiSample.chkMenu.Value = 1
    Else
        mdiSample.chkMenu.Value = 0
    End If
    If (CanRecord And MCIWnd.Record) Then
        mdiSample.chkRecord.Value = 1
    Else
        mdiSample.chkRecord.Value = 0
    End If

    ' display the detailed property values
    mdiSample.txtSpeed = Trim$(Str$(MCIWnd.Speed))
    mdiSample.txtVolume = Trim$(Str$(MCIWnd.Volume))
    mdiSample.txtZoom = Trim$(Str$(MCIWnd.Zoom))
    mdiSample.txtTimerFreq = Trim$(Str$(MCIWnd.TimerFreq))

    ' and set the detailed property checkboxes too...
    If (MCIWnd.ErrorDlg) Then
        mdiSample.chkErrorDlg.Value = 1
    Else
        mdiSample.chkErrorDlg.Value = 0
    End If
    If (MCIWnd.Visible) Then
        mdiSample.chkVisible.Value = 1
    Else
        mdiSample.chkVisible.Value = 0
    End If
    If (MCIWnd.Enabled) Then
        mdiSample.chkEnabled.Value = 1
    Else
        mdiSample.chkEnabled.Value = 0
    End If
    If (MCIWnd.Repeat) Then
        mdiSample.chkRepeat.Value = 1
    Else
        mdiSample.chkRepeat.Value = 0
    End If
    If (MCIWnd.AutosizeMovie) Then
        mdiSample.chkAutoSizeMovie.Value = 1
    Else
        mdiSample.chkAutoSizeMovie.Value = 0
    End If
    If (MCIWnd.AutosizeWindow) Then
        mdiSample.chkAutoSizeWindow.Value = 1
    Else
        mdiSample.chkAutoSizeWindow.Value = 0
    End If
    If (MCIWnd.WantPosEvent) Then
        mdiSample.chkWantPosEvent.Value = 1
    Else
        mdiSample.chkWantPosEvent.Value = 0
    End If
End Sub

