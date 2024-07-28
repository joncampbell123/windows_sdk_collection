'
'   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
'   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
'   PURPOSE.
'
'   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
'
VERSION 2.00
Begin MDIForm mdiSample 
   Caption         =   "MCIWnd VBX Sample Application"
   Height          =   4065
   Left            =   750
   LinkTopic       =   "MDIForm1"
   Top             =   510
   Width           =   8205
   Begin PictureBox picSpace 
      Align           =   2  'Align Bottom
      AutoSize        =   -1  'True
      Height          =   2685
      Left            =   0
      ScaleHeight     =   2655
      ScaleWidth      =   8055
      TabIndex        =   1
      Top             =   690
      Width           =   8085
      Begin Frame fraDetailProps 
         Caption         =   "Detailed Properties"
         Height          =   1095
         Left            =   120
         TabIndex        =   22
         Top             =   2640
         Width           =   7815
         Begin CheckBox chkWantPosEvent 
            Caption         =   "WantPosEvent"
            Height          =   255
            Left            =   6120
            TabIndex        =   38
            Top             =   720
            Width           =   1575
         End
         Begin CheckBox chkRepeat 
            Caption         =   "Repeat"
            Height          =   255
            Left            =   6720
            TabIndex        =   37
            Top             =   360
            Width           =   975
         End
         Begin CheckBox chkAutoSizeWindow 
            Caption         =   "AutoSizWin"
            Height          =   255
            Left            =   4680
            TabIndex        =   36
            Top             =   720
            Width           =   1335
         End
         Begin CheckBox chkAutoSizeMovie 
            Caption         =   "AutoSizMovie"
            Height          =   255
            Left            =   3120
            TabIndex        =   35
            Top             =   720
            Width           =   1455
         End
         Begin CheckBox chkEnabled 
            Caption         =   "Enabled"
            Height          =   255
            Left            =   5400
            TabIndex        =   34
            Top             =   360
            Width           =   975
         End
         Begin CheckBox chkVisible 
            Caption         =   "Visible"
            Height          =   255
            Left            =   4200
            TabIndex        =   33
            Top             =   360
            Width           =   855
         End
         Begin CheckBox chkErrorDlg 
            Caption         =   "ErrDlg"
            Height          =   255
            Left            =   3120
            TabIndex        =   32
            Top             =   360
            Width           =   855
         End
         Begin TextBox txtTimerFreq 
            Height          =   285
            Left            =   2280
            MaxLength       =   4
            TabIndex        =   31
            Top             =   720
            Width           =   615
         End
         Begin TextBox txtZoom 
            Height          =   285
            Left            =   720
            MaxLength       =   4
            TabIndex        =   29
            Top             =   720
            Width           =   615
         End
         Begin TextBox txtVolume 
            Height          =   285
            Left            =   2280
            MaxLength       =   4
            TabIndex        =   27
            Top             =   360
            Width           =   615
         End
         Begin TextBox txtSpeed 
            Height          =   285
            Left            =   720
            MaxLength       =   4
            TabIndex        =   25
            Top             =   360
            Width           =   615
         End
         Begin Label lblTimerFreq 
            Caption         =   "TimrFreq:"
            Height          =   255
            Left            =   1440
            TabIndex        =   30
            Top             =   720
            Width           =   855
         End
         Begin Label lblZoom 
            Caption         =   "Zoom:"
            Height          =   255
            Left            =   120
            TabIndex        =   28
            Top             =   720
            Width           =   615
         End
         Begin Label lblVolume 
            Caption         =   "Volume:"
            Height          =   255
            Left            =   1560
            TabIndex        =   26
            Top             =   360
            Width           =   735
         End
         Begin Label lblSpeed 
            Caption         =   "Speed:"
            Height          =   255
            Left            =   120
            TabIndex        =   24
            Top             =   360
            Width           =   615
         End
      End
      Begin CommandButton cmdMoreOrLess 
         Caption         =   "More"
         Height          =   375
         Left            =   6360
         TabIndex        =   0
         TabStop         =   0   'False
         Top             =   240
         Width           =   1575
      End
      Begin Frame fraWriteProps 
         Caption         =   "Write Properties"
         Height          =   1815
         Left            =   4320
         TabIndex        =   8
         Top             =   720
         Width           =   3615
         Begin TextBox txtCommand 
            Height          =   285
            Left            =   1020
            TabIndex        =   23
            Top             =   720
            Width           =   2475
         End
         Begin TextBox txtNewDevice 
            Height          =   285
            Left            =   1200
            TabIndex        =   3
            Top             =   1080
            Width           =   2295
         End
         Begin CheckBox chkRecord 
            Caption         =   "Record"
            Height          =   255
            Left            =   2520
            TabIndex        =   6
            Top             =   1440
            Width           =   975
         End
         Begin CheckBox chkMenu 
            Caption         =   "Menu"
            Height          =   255
            Left            =   1440
            TabIndex        =   5
            Top             =   1440
            Width           =   735
         End
         Begin CheckBox chkPlaybar 
            Caption         =   "Playbar"
            Height          =   255
            Left            =   120
            TabIndex        =   4
            Top             =   1440
            Width           =   975
         End
         Begin TextBox txtFileName 
            Height          =   285
            Left            =   1020
            MaxLength       =   128
            TabIndex        =   2
            Top             =   360
            Width           =   2475
         End
         Begin Label lblNewDevice 
            Caption         =   "NewDevice:"
            Height          =   255
            Left            =   120
            TabIndex        =   20
            Top             =   1080
            Width           =   1095
         End
         Begin Label lblCommand 
            Caption         =   "Command:"
            Height          =   255
            Left            =   120
            TabIndex        =   14
            Top             =   720
            Width           =   855
         End
         Begin Label lblFileName 
            Caption         =   "FileName:"
            Height          =   255
            Left            =   120
            TabIndex        =   9
            Top             =   360
            Width           =   855
         End
      End
      Begin Frame fraReadProps 
         Caption         =   "Read Properties"
         Height          =   2535
         Left            =   120
         TabIndex        =   7
         Top             =   0
         Width           =   4095
         Begin TextBox txtError 
            BorderStyle     =   0  'None
            ForeColor       =   &H000000C0&
            Height          =   285
            Left            =   600
            TabIndex        =   16
            TabStop         =   0   'False
            Top             =   1080
            Width           =   3375
         End
         Begin Label txtNotification 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   1200
            TabIndex        =   48
            Top             =   2160
            Width           =   1695
         End
         Begin Label txtTimeFormat 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   2915
            TabIndex        =   47
            Top             =   1800
            Width           =   1060
         End
         Begin Label txtPosition 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   875
            TabIndex        =   46
            Top             =   1800
            Width           =   1300
         End
         Begin Label txtCommandReturn 
            ForeColor       =   &H00C00000&
            Height          =   255
            Left            =   1600
            TabIndex        =   45
            Top             =   1440
            Width           =   2375
         End
         Begin Label txtLength 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   3175
            TabIndex        =   44
            Top             =   720
            Width           =   800
         End
         Begin Label txtEnd 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   1855
            TabIndex        =   43
            Top             =   720
            Width           =   800
         End
         Begin Label txtStart 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   600
            TabIndex        =   42
            Top             =   720
            Width           =   735
         End
         Begin Label txtMode 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   3100
            TabIndex        =   41
            Top             =   360
            Width           =   875
         End
         Begin Label txtDevice 
            ForeColor       =   &H00800000&
            Height          =   255
            Left            =   840
            TabIndex        =   40
            Top             =   360
            Width           =   1575
         End
         Begin Label lblNotification 
            Caption         =   "Notification:"
            Height          =   255
            Left            =   120
            TabIndex        =   39
            Top             =   2160
            Width           =   1095
         End
         Begin Label lblTimeFormat 
            Caption         =   "Format:"
            Height          =   255
            Left            =   2280
            TabIndex        =   21
            Top             =   1800
            Width           =   615
         End
         Begin Label lblCommandReturn 
            Caption         =   "CommandReturn:"
            Height          =   255
            Left            =   120
            TabIndex        =   19
            Top             =   1440
            Width           =   1455
         End
         Begin Label lblMode 
            Caption         =   "Mode:"
            Height          =   252
            Left            =   2520
            TabIndex        =   18
            Top             =   360
            Width           =   492
         End
         Begin Label lblPosition 
            Caption         =   "Position:"
            Height          =   255
            Left            =   120
            TabIndex        =   17
            Top             =   1800
            Width           =   735
         End
         Begin Label lblError 
            Caption         =   "Error:"
            Height          =   255
            Left            =   120
            TabIndex        =   15
            Top             =   1080
            Width           =   495
         End
         Begin Label lblLength 
            Caption         =   "Len:"
            Height          =   255
            Left            =   2760
            TabIndex        =   13
            Top             =   720
            Width           =   375
         End
         Begin Label lblEnd 
            Caption         =   "End:"
            Height          =   255
            Left            =   1440
            TabIndex        =   12
            Top             =   720
            Width           =   375
         End
         Begin Label lblStart 
            Caption         =   "Start:"
            Height          =   255
            Left            =   120
            TabIndex        =   11
            Top             =   720
            Width           =   495
         End
         Begin Label lblDevice 
            Caption         =   "Device:"
            Height          =   255
            Left            =   120
            TabIndex        =   10
            Top             =   360
            Width           =   735
         End
      End
   End
   Begin Menu mnuFile 
      Caption         =   "&File"
      Begin Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin Menu mnuHelp 
      Caption         =   "&Help"
      Begin Menu mnuHelpAbout 
         Caption         =   "A&bout"
      End
   End
End
Option Explicit

Const KEY_RETURN = &HD
Const MB_OK = 0

Sub chkAutoSizeMovie_Click ()
    ' set AutoSizeMovie prop value acc to checkbox state
    frmChild.MCIWnd.AutosizeMovie = (chkAutoSizeMovie.Value = 1)
End Sub

Sub chkAutoSizeWindow_Click ()
    ' set AutoSizeWindow prop value acc to checkbox state
    frmChild.MCIWnd.AutosizeWindow = (chkAutoSizeWindow.Value = 1)
End Sub

Sub chkEnabled_Click ()
    ' set Enabled prop value acc to checkbox state
    frmChild.MCIWnd.Enabled = (chkEnabled.Value = 1)
End Sub

Sub chkErrorDlg_Click ()
    ' set ErrorDlg prop value acc to checkbox state
    frmChild.MCIWnd.ErrorDlg = (chkErrorDlg.Value = 1)
End Sub

Sub chkMenu_Click ()
    ' set Menu prop value acc to checkbox state
    frmChild.MCIWnd.Menu = (chkMenu.Value = 1)
End Sub

Sub chkPlaybar_Click ()
    ' set Playbar prop value acc to checkbox state
    frmChild.MCIWnd.Playbar = (chkPlaybar.Value = 1)
End Sub

Sub chkRecord_Click ()
    ' set Record prop value acc to checkbox state
    frmChild.MCIWnd.Record = CanRecord And (chkRecord.Value = 1)

    ' check if the device allows recording
    If (frmChild.MCIWnd.Record) Then
        chkRecord.Value = 1
    Else
        chkRecord.Value = 0
    End If
End Sub

Sub chkRepeat_Click ()
    ' set Repeat prop value acc to checkbox state
    frmChild.MCIWnd.Repeat = (chkRepeat.Value = 1)
End Sub

Sub chkVisible_Click ()
    ' set Visible prop value acc to checkbox state
    frmChild.MCIWnd.Visible = (chkVisible.Value = 1)
End Sub

Sub chkWantPosEvent_Click ()
    ' set WantPosEvent prop value acc to checkbox state
    frmChild.MCIWnd.WantPosEvent = (chkWantPosEvent.Value = 1)
End Sub

Sub cmdMoreOrLess_Click ()
    MoreFlag = Not MoreFlag
    fraDetailProps.Visible = MoreFlag
    ' update button text and app window height
    If MoreFlag Then
        cmdMoreOrLess.Caption = "Less"
        picSpace.Height = picSpace.Height + MoreSpace
    Else
        cmdMoreOrLess.Caption = "More"
        picSpace.Height = picSpace.Height - MoreSpace
    End If

    Call ResizeMDIHeight    ' update app window height
End Sub

Sub MDIForm_Load ()
    ' Start with less detail view of properties
    MoreFlag = False
    MoreSpace = fraDetailProps.Height + 90   ' 90 for space in between
    MDIWidth = mdiSample.Width               ' initial MDI form width

    ' set child position in the beginning
    frmChild.Left = 120
    frmChild.Top = 120

    ' Initialize notify string data...
    NotifyStr(1) = "Successful"
    NotifyStr(2) = "Superseded"
    NotifyStr(3) = "Aborted"
    NotifyStr(4) = "Failure"

    ' Initialize CanRecord to false
    CanRecord = False
End Sub

Sub MDIForm_Resize ()
    If (mdiSample.WindowState = 0) Then   ' normal state
        Call ResizeMDIHeight    ' update app window height
        Call ResizeMDIWidth     '  ...   ...  ...   width
    End If
End Sub

Sub mnuFileExit_Click ()
    ' first make sure no device is on and active...
    frmChild.MCIWnd.Command = "stop"
    frmChild.MCIWnd.Command = "close"

    ' now get out of here...
    End
End Sub

Sub mnuHelpAbout_Click ()
    ' show "About" info as a modal dialog box
    Dim strMsg As String

    strMsg = "VB Sample Application for MCIWnd VBX"
    strMsg = strMsg & Chr$(13) & Chr$(13)
    strMsg = strMsg & "  Copyright Microsoft Corporation, 1992-1993."
    strMsg = strMsg & Chr$(13)

    MsgBox strMsg, MB_OK, "About"
End Sub

Sub txtCommand_KeyPress (keyascii As Integer)
    If (keyascii = 13) Then
        'Clear prev Notification and CommandReturn info first
        txtNotification = ""
        txtCommandReturn = ""

        'Now pass on the command...
        frmChild.MCIWnd.Command = Trim$(txtCommand)
        keyascii = 0

        ' if there is any return from the above command, show it...
        txtCommandReturn = frmChild.MCIWnd.CommandReturn
        If (frmChild.MCIWnd.Error = 0) Then
            txtError = ""
        End If
    End If
End Sub

Sub txtError_KeyPress (keyascii As Integer)
    ' Nullify any key press
    keyascii = 0
End Sub

Sub txtFileName_KeyPress (keyascii As Integer)
    ' assign textbox content to FileName property at "Enter"
    If (keyascii = 13) Then
        frmChild.MCIWnd.Filename = Trim$(txtFileName)
        keyascii = 0
    End If
End Sub

Sub txtNewDevice_KeyPress (keyascii As Integer)
    ' assign textbox content to NewDevice property at "Enter"
    If (keyascii = 13) Then
        frmChild.MCIWnd.NewDevice = Trim$(txtNewDevice)
        keyascii = 0
    End If
End Sub

Sub txtSpeed_GotFocus ()
    'display latest value of the property on focus
    txtSpeed = Trim$(Str$(frmChild.MCIWnd.Speed))
End Sub

Sub txtSpeed_KeyPress (keyascii As Integer)
    Dim Speed As Integer

    ' assign textbox content to Speed property at "Enter"
    If (keyascii = KEY_RETURN) Then
        Speed = Val(Trim$(txtSpeed))
        If (Speed > 0) Then
            frmChild.MCIWnd.Speed = Speed
        End If
        txtSpeed = Trim$(Str$(frmChild.MCIWnd.Speed))
        keyascii = 0
    End If
End Sub

Sub txtSpeed_LostFocus ()
    Dim Speed As Integer

    ' assign textbox content to Speed property at "Enter"
    Speed = Val(Trim$(txtSpeed))
    If (Speed > 0) Then
        frmChild.MCIWnd.Speed = Speed
    End If
    txtSpeed = Trim$(Str$(frmChild.MCIWnd.Speed))
End Sub

Sub txtTimerFreq_GotFocus ()
    'display latest value of the property on focus
    txtTimerFreq = Trim$(Str$(frmChild.MCIWnd.TimerFreq))
End Sub

Sub txtTimerFreq_KeyPress (keyascii As Integer)
    Dim Frequency As Integer

    ' assign textbox content to Speed property at "Enter"
    If (keyascii = KEY_RETURN) Then
        Frequency = Val(Trim$(txtTimerFreq))
        If (Frequency > 0) Then
            frmChild.MCIWnd.TimerFreq = Frequency
        End If
        txtTimerFreq = Trim$(Str$(frmChild.MCIWnd.TimerFreq))
        keyascii = 0
    End If
End Sub

Sub txtTimerFreq_LostFocus ()
    Dim Frequency As Integer

    ' assign textbox content to Speed property at "Enter"
    Frequency = Val(Trim$(txtTimerFreq))
    If (Frequency > 0) Then
        frmChild.MCIWnd.TimerFreq = Frequency
    End If
    txtTimerFreq = Trim$(Str$(frmChild.MCIWnd.TimerFreq))
End Sub

Sub txtVolume_GotFocus ()
    'display latest value of the property on focus
    txtVolume = Trim$(Str$(frmChild.MCIWnd.Volume))
End Sub

Sub txtVolume_KeyPress (keyascii As Integer)
    Dim Volume As Integer

    ' assign textbox content to Speed property at "Enter"
    If (keyascii = KEY_RETURN) Then
        Volume = Val(Trim$(txtVolume))
        If (Volume > 0) Then
            frmChild.MCIWnd.Volume = Volume
        End If
        txtVolume = Trim$(Str$(frmChild.MCIWnd.Volume))
        keyascii = 0
    End If
End Sub

Sub txtVolume_LostFocus ()
    Dim Volume As Integer

    ' assign textbox content to Speed property at "Enter"
    Volume = Val(Trim$(txtVolume))
    If (Volume > 0) Then
        frmChild.MCIWnd.Volume = Volume
    End If
    txtVolume = Trim$(Str$(frmChild.MCIWnd.Volume))
End Sub

Sub txtZoom_GotFocus ()
    'display latest value of the property on focus
    txtZoom = Trim$(Str$(frmChild.MCIWnd.Zoom))
End Sub

Sub txtZoom_KeyPress (keyascii As Integer)
    Dim Zoom As Integer

    ' assign textbox content to Speed property at "Enter"
    If (keyascii = KEY_RETURN) Then
        Zoom = Val(Trim$(txtZoom))
        If (Zoom > 0) Then
            frmChild.MCIWnd.Zoom = Zoom
        End If
        txtZoom = Trim$(Str$(frmChild.MCIWnd.Zoom))
        keyascii = 0
    End If
End Sub

Sub txtZoom_LostFocus ()
    Dim Zoom As Integer

    ' assign textbox content to Speed property at "Enter"
    Zoom = Val(Trim$(txtZoom))
    If (Zoom > 0) Then
        frmChild.MCIWnd.Zoom = Zoom
    End If
    txtZoom = Trim$(Str$(frmChild.MCIWnd.Zoom))
End Sub

