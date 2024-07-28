'
'   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
'   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
'   PURPOSE.
'
'   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
'
VERSION 2.00
Begin Form Form1 
   BackColor       =   &H00C0C0C0&
   Caption         =   "Basic Capture"
   ClientHeight    =   4230
   ClientLeft      =   1095
   ClientTop       =   1815
   ClientWidth     =   4785
   FontBold        =   -1  'True
   FontItalic      =   0   'False
   FontName        =   "System"
   FontSize        =   9.75
   FontStrikethru  =   0   'False
   FontUnderline   =   0   'False
   Height          =   4920
   Icon            =   0
   Left            =   1035
   LinkTopic       =   "Form1"
   ScaleHeight     =   282
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   319
   Top             =   1185
   Width           =   4905
   Begin CAPWND CapWnd1 
      AudioBits       =   8
      AudioChannels   =   1
      AudioRate       =   11025
      AutoSize        =   -1  'True
      CaptureAudio    =   -1  'True
      CaptureDOSMem   =   -1  'True
      CaptureFile     =   "\CAPTURE.AVI"
      CaptureHitOK    =   0   'False
      CaptureLimit    =   0
      CaptureUseLimit =   0   'False
      Connect         =   -1  'True
      DriverNum       =   0
      Height          =   3630
      Left            =   0
      MCIDeviceEnable =   0   'False
      MCIDeviceName   =   ""
      MCIStartMS      =   0
      MCIStepCapture  =   0   'False
      MCIStopMS       =   0
      Overlay         =   0   'False
      PalNumColors    =   256
      PalNumFrames    =   20
      Preview         =   0   'False
      PreviewRate     =   15
      Top             =   0
      VideoRate       =   15
      Width           =   4830
      Yield           =   0   'False
   End
   Begin MCIWND MCIWnd1 
      AutosizeMovie   =   -1  'True
      AutosizeWindow  =   -1  'True
      ErrorDlg        =   0   'False
      Filename        =   ""
      Height          =   3615
      Left            =   0
      Menu            =   -1  'True
      Playbar         =   -1  'True
      Record          =   0   'False
      Repeat          =   0   'False
      Speed           =   1000
      TimeFormat      =   ""
      TimerFreq       =   500
      Top             =   0
      Visible         =   0   'False
      Volume          =   1000
      WantPosEvent    =   -1  'True
      Width           =   4815
      Zoom            =   100
   End
   Begin Label Status 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "Status:"
      ForeColor       =   &H0000FF00&
      Height          =   615
      Left            =   0
      TabIndex        =   0
      Top             =   3600
      Width           =   4815
      WordWrap        =   -1  'True
   End
   Begin Menu File 
      Caption         =   "&File"
      Begin Menu SetCaptureFile 
         Caption         =   "Set Capture &File..."
      End
      Begin Menu SaveVideoAs 
         Caption         =   "&Save Video As..."
      End
      Begin Menu Dummy1 
         Caption         =   "-"
      End
      Begin Menu Exit 
         Caption         =   "E&xit"
      End
   End
   Begin Menu Edit 
      Caption         =   "&Edit"
      Begin Menu EditCopy 
         Caption         =   "&Copy"
         Shortcut        =   ^C
      End
   End
   Begin Menu Options 
      Caption         =   "&Options"
      Begin Menu OptionsPreview 
         Caption         =   "&Preview"
      End
      Begin Menu OptionsOverlay 
         Caption         =   "&Overlay"
      End
      Begin Menu Dummy2 
         Caption         =   "-"
      End
      Begin Menu OptionsAudio 
         Caption         =   "&Audio"
      End
      Begin Menu Dummy9 
         Caption         =   "-"
      End
      Begin Menu VideoFormat 
         Caption         =   "Video &Format..."
      End
      Begin Menu VideoSource 
         Caption         =   "Video &Source..."
      End
      Begin Menu VideoDisplay 
         Caption         =   "Video &Display..."
      End
      Begin Menu Dummy3 
         Caption         =   "-"
      End
      Begin Menu MakePalette 
         Caption         =   "&Make Palette"
      End
   End
   Begin Menu Capture 
      Caption         =   "&Capture"
      Begin Menu Rate 
         Caption         =   "&Rate"
         Begin Menu Rate1 
            Caption         =   "&1 fps"
         End
         Begin Menu Rate7 
            Caption         =   "&7 fps"
         End
         Begin Menu Rate12 
            Caption         =   "1&2 fps"
         End
         Begin Menu Rate15 
            Caption         =   "1&5 fps"
         End
         Begin Menu Rate24 
            Caption         =   "&24 fps"
         End
         Begin Menu Rate30 
            Caption         =   "&30 fps"
         End
      End
      Begin Menu CaptureVideo 
         Caption         =   "Capture &Video"
      End
   End
   Begin Menu Play 
      Caption         =   "&Play!"
   End
End



Sub Capture_Click ()
    ShowCaptureWindow
End Sub

Sub CaptureVideo_Click ()
    Form1.Status = "Capturing.  Right mouse to stop"
    Form1.Status.Refresh
    CapWnd1.Capture = True
End Sub

Sub CapWnd1_Error ()
    If Len(CapWnd1.Error) > 1 Then
	MsgBox CapWnd1.Error, 0, "Capture Error"
    End If
End Sub

Sub CapWnd1_Status ()
    Rem only gets executed when yielding
    If Len(CapWnd1.Status) > 1 Then
	Form1.Status = CapWnd1.Status
	Form1.Status.Refresh
    End If
End Sub

Sub ClearRateMenu ()
    Form1.Rate1.Checked = False
    Form1.Rate7.Checked = False
    Form1.Rate12.Checked = False
    Form1.Rate15.Checked = False
    Form1.Rate24.Checked = False
    Form1.Rate30.Checked = False
End Sub

Sub Edit_Click ()
    ShowCaptureWindow
End Sub

Sub EditCopy_Click ()
    CapWnd1.EditCopy = True
End Sub

Sub Exit_Click ()
    End
End Sub

Sub File_Click ()
    ShowCaptureWindow
End Sub

Sub Form_Load ()
    Rem Select which driver to use
    CapWnd1.DriverNum = 0

    Rem By default, connect to the driver
    CapWnd1.Connect = True
    If CapWnd1.Connect <> True Then
	MsgBox "Unable to connect to capture driver!", 16, "Error"
    End If
 
    Form1.Status = CapWnd1.DriverName + "   " + CapWnd1.DriverVersion
    CapWnd1.PreviewRate = 15
    Form1.Rate15.Checked = True

    Rem Set the buttons to initial conditions
    If CapWnd1.CanOverlay Then
	CapWnd1.Overlay = True
	Form1.OptionsOverlay.Enabled = True
	Form1.OptionsOverlay.Checked = True
    Else
	Form1.OptionsOverlay.Enabled = False
	CapWnd1.Preview = True
    End If

    If CapWnd1.CaptureAudio Then
	Form1.OptionsAudio.Checked = True
    Else
	Form1.OptionsAudio.Enabled = False
    End If
    SizeWindow
End Sub

Sub MakePalette_Click ()
    CapWnd1.PalCreate = True
End Sub

Sub Options_Click ()
    ShowCaptureWindow
End Sub

Sub OptionsAudio_Click ()
    OptionsAudio.Checked = Not OptionsAudio.Checked
    CapWnd1.CaptureAudio = OptionsAudio.Checked
End Sub

Sub OptionsOverlay_Click ()
    OptionsOverlay.Checked = Not OptionsOverlay.Checked
    CapWnd1.Overlay = OptionsOverlay.Checked
    OptionsPreview.Checked = False
    Status = "Overlay"
End Sub

Sub OptionsPreview_Click ()
    OptionsPreview.Checked = Not OptionsPreview.Checked
    CapWnd1.Preview = OptionsPreview.Checked
    OptionsOverlay.Checked = False
    Status = "Preview"
End Sub

Sub Play_Click ()
    ShowPlayWindow
    MCIWnd1.Command = "Play"
End Sub

Sub Rate1_Click ()
    CapWnd1.VideoRate = 1
    ClearRateMenu
    Rate1.Checked = True
End Sub

Sub Rate12_Click ()
    CapWnd1.VideoRate = 12
    ClearRateMenu
    Rate12.Checked = True
End Sub

Sub Rate15_Click ()
    CapWnd1.VideoRate = 15
    ClearRateMenu
    Rate15.Checked = True

End Sub

Sub Rate24_Click ()
    CapWnd1.VideoRate = 24
    ClearRateMenu
    Rate24.Checked = True

End Sub

Sub Rate30_Click ()
    CapWnd1.VideoRate = 30
    ClearRateMenu
    Rate30.Checked = True

End Sub

Sub Rate7_Click ()
    CapWnd1.VideoRate = 7
    ClearRateMenu
    Rate7.Checked = True

End Sub

Sub SaveVideoAs_Click ()
    Dim f$

    f = InputBox$("Copy capture file to:", "Copy Capture File", "c:\Test.avi")
    If Len(f) > 2 Then
	CapWnd1.CaptureSave = f
    End If

End Sub

Sub SetCaptureFile_Click ()
    Dim f$

    f = InputBox$("Capture file and path", "Set Capture File", CapWnd1.CaptureFile)
    If Len(f) > 2 Then
	CapWnd1.CaptureFile = f
    End If
End Sub

Sub ShowCaptureWindow ()
    If CapWnd1.Connect = True Then
	Exit Sub
    End If
    Status.Caption = "Opening capture window..."
    MCIWnd1.Visible = False
    MCIWnd1.Command = "Close"
    
    CapWnd1.Connect = True
    CapWnd1.Overlay = OptionsOverlay.Checked
    CapWnd1.Preview = OptionsPreview.Checked
    CapWnd1.Visible = True
End Sub

Sub ShowPlayWindow ()
    Dim TempFilename$

    Status.Caption = "Closing capture window..."
    CapWnd1.Visible = False
    TempFilename$ = CapWnd1.CaptureFile
    MCIWnd1.Visible = True
    CapWnd1.Connect = False
    MCIWnd1.Filename = TempFilename$
    Status.Caption = ""
End Sub

Sub SizeWindow ()
    Dim h, w

    ScaleMode = 3

    w = (CapWnd1.Width * Screen.TwipsPerPixelX) + (Width - (ScaleWidth * Screen.TwipsPerPixelX))
    h = (CapWnd1.Height * Screen.TwipsPerPixelY) + (Height - (ScaleHeight * Screen.TwipsPerPixelY))
    Form1.Move Form1.Left, Form1.Top, w, h

    Rem do it twice in case the caption wrapped
    w = (CapWnd1.Width * Screen.TwipsPerPixelX) + (Width - (ScaleWidth * Screen.TwipsPerPixelX))
    h = (CapWnd1.Height * Screen.TwipsPerPixelY) + (Height - (ScaleHeight * Screen.TwipsPerPixelY)) + 500
    Form1.Move Form1.Left, Form1.Top, w, h

    Form1.Status.Move 0, ScaleHeight - (500 / Screen.TwipsPerPixelY), ScaleWidth
End Sub

Sub UpdateStatus ()
    Refresh
End Sub

Sub VideoDisplay_Click ()
    CapWnd1.VideoDisplayDlg = True
End Sub

Sub VideoFormat_Click ()
    Dim h, w

    CapWnd1.VideoFormatDlg = True
    SizeWindow
End Sub

Sub VideoSource_Click ()
	CapWnd1.VideoSourceDlg = True
End Sub

