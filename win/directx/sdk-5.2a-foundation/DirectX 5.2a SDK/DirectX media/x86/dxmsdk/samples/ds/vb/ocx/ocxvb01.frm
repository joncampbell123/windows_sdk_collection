VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "COMCTL32.OCX"
Begin VB.Form frmMain 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "ActiveMovie OCX Sample"
   ClientHeight    =   2910
   ClientLeft      =   3435
   ClientTop       =   2295
   ClientWidth     =   5370
   Icon            =   "ocxvb01.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2910
   ScaleWidth      =   5370
   ShowInTaskbar   =   0   'False
   Begin VB.Frame frmImageSize 
      Caption         =   "Change Image Size"
      Enabled         =   0   'False
      Height          =   2292
      Left            =   2880
      TabIndex        =   9
      Top             =   120
      Width           =   2292
      Begin VB.OptionButton obOneHalfScreen 
         Caption         =   "One Half Screen"
         Height          =   255
         Left            =   240
         TabIndex        =   14
         Top             =   1800
         Width           =   1935
      End
      Begin VB.OptionButton obOneFourthScreen 
         Caption         =   "One Fourth Screen"
         Height          =   255
         Left            =   240
         TabIndex        =   13
         Top             =   1440
         Width           =   1935
      End
      Begin VB.OptionButton obOneSixteenthScreen 
         Caption         =   "One Sixteenth Screen"
         Height          =   255
         Left            =   240
         TabIndex        =   12
         Top             =   1080
         Width           =   1935
      End
      Begin VB.OptionButton obDoubleOriginal 
         Caption         =   "Double Original Size"
         Height          =   255
         Left            =   240
         TabIndex        =   11
         Top             =   720
         Width           =   1935
      End
      Begin VB.OptionButton obOriginal 
         Caption         =   "Original Size"
         Height          =   255
         Left            =   240
         TabIndex        =   10
         Top             =   360
         Width           =   1815
      End
   End
   Begin VB.Frame frmPlaybackRate 
      Caption         =   "Change Playback Rate"
      Height          =   1212
      Left            =   240
      TabIndex        =   4
      Top             =   1200
      Width           =   2412
      Begin ComctlLib.Slider Slider1 
         Height          =   492
         Left            =   240
         TabIndex        =   5
         Top             =   240
         Width           =   1932
         _ExtentX        =   3413
         _ExtentY        =   873
         _Version        =   327680
         Enabled         =   -1  'True
         LargeChange     =   1
         TickStyle       =   2
      End
      Begin VB.Label Label3 
         Alignment       =   2  'Center
         Caption         =   "1.5"
         Height          =   252
         Left            =   1860
         TabIndex        =   8
         Top             =   840
         Width           =   252
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "1.0"
         Height          =   252
         Left            =   1020
         TabIndex        =   7
         Top             =   840
         Width           =   372
      End
      Begin VB.Label Label1 
         Alignment       =   2  'Center
         Caption         =   "0.5"
         Height          =   252
         Left            =   240
         TabIndex        =   6
         Top             =   840
         Width           =   372
      End
   End
   Begin VB.Frame frmChangeFrames 
      Caption         =   "Change Current Position"
      Height          =   972
      Left            =   240
      TabIndex        =   1
      Top             =   120
      Width           =   2412
      Begin VB.CommandButton cmdNextFrame 
         Caption         =   "&>>"
         Enabled         =   0   'False
         Height          =   372
         Left            =   1320
         TabIndex        =   3
         Top             =   360
         Width           =   852
      End
      Begin VB.CommandButton cmdPreviousFrame 
         Caption         =   "&<<"
         Enabled         =   0   'False
         Height          =   372
         Left            =   240
         TabIndex        =   2
         Top             =   360
         Width           =   852
      End
   End
   Begin ComctlLib.StatusBar StatusBar1 
      Align           =   2  'Align Bottom
      Height          =   408
      Left            =   0
      TabIndex        =   0
      Top             =   2508
      Width           =   5376
      _ExtentX        =   9472
      _ExtentY        =   714
      SimpleText      =   ""
      _Version        =   327680
      BeginProperty Panels {0713E89E-850A-101B-AFC0-4210102A8DA7} 
         NumPanels       =   3
         BeginProperty Panel1 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   1
            Object.Width           =   3122
            TextSave        =   ""
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel2 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   1
            Object.Width           =   3122
            TextSave        =   ""
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel3 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   1
            Object.Width           =   3122
            TextSave        =   ""
            Object.Tag             =   ""
         EndProperty
      EndProperty
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   240
      Top             =   5880
      _ExtentX        =   688
      _ExtentY        =   688
      _Version        =   327680
   End
   Begin VB.Menu mnu_File 
      Caption         =   "&File"
      Begin VB.Menu mnu_File_Open 
         Caption         =   "&Open"
      End
      Begin VB.Menu sep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnu_File_Run 
         Caption         =   "&Run"
      End
      Begin VB.Menu mnu_File_Pause 
         Caption         =   "&Pause"
      End
      Begin VB.Menu mnu_File_Stop 
         Caption         =   "&Stop"
      End
      Begin VB.Menu sep2 
         Caption         =   "-"
      End
      Begin VB.Menu mnu_File_Exit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnu_View 
      Caption         =   "&View"
      Begin VB.Menu mnu_View_DisplayPanel 
         Caption         =   "&Display Panel"
         Checked         =   -1  'True
      End
      Begin VB.Menu sep4 
         Caption         =   "-"
      End
      Begin VB.Menu mnu_View_ControlPanel 
         Caption         =   "&Control Panel"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnu_View_PositionControls 
         Caption         =   "&Position Controls"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnu_View_SelectionControls 
         Caption         =   "&Selection Controls"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnu_View_Tracker 
         Caption         =   "&Tracker"
         Checked         =   -1  'True
      End
   End
   Begin VB.Menu mnu_Enable 
      Caption         =   "E&nable"
      Begin VB.Menu mnu_Enable_PositionControls 
         Caption         =   "&Position Controls"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnu_Enable_SelectionControls 
         Caption         =   "&Selection Controls"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnu_Enable_Tracker 
         Caption         =   "&Tracker"
         Checked         =   -1  'True
      End
   End
   Begin VB.Menu mnu_Options 
      Caption         =   "&Options"
      Begin VB.Menu mnu_Options_AutoRewind 
         Caption         =   "Auto &Rewind"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnu_Options_DisplayTime 
         Caption         =   "Display &Time"
         Checked         =   -1  'True
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ActiveMovie Control Sample
' Copyright (c) 1996 - 1997 Microsoft Corporation
' All Rights Reserved.
'
    



Private Sub cmdNextFrame_Click()
    If ((frmViewer.ActiveMovie1.CurrentPosition + 0.1) <= frmViewer.ActiveMovie1.Duration) Then
        frmViewer.ActiveMovie1.CurrentPosition = frmViewer.ActiveMovie1.CurrentPosition + 0.1
    End If
End Sub

Private Sub cmdPreviousFrame_Click()
    If ((frmViewer.ActiveMovie1.CurrentPosition - 0.1) >= 0) Then
        frmViewer.ActiveMovie1.CurrentPosition = frmViewer.ActiveMovie1.CurrentPosition - 0.1
    End If
    
End Sub



Private Sub Form_Load()

    ' ActiveMovie Control Sample Code
    ' Copyright (c) 1996 - 1997 Microsoft Corporation
    ' All Rights Reserved
    
    ' This sample demonstrates the ActiveMovie Control.
    ' It contains the following controls:
    '
    '   The ActiveMovie control (in its own form).
    '   A status bar control.
    '   The common dialogs control.
    '   Two buttons that allow you to step through the stream.
    '   Option buttons that let you select the stream size.
    '   A slider that lets you change the playback rate.
    
    ' The main form, frmMain, contains several menus that
    ' control various ActiveMovie controls.
    ' When events are enabled, a global counter is used to
    ' keep a running account of each ActiveMovie control event,
    ' and the value is displayed in the status control.


    ' Initialize control properties and set menu items to match.
    
    Slider1.Value = 5   ' Right in the middle.
    ' Default value for ActiveMovie1.Rate = 1.0
    ' Use default value.
    obOriginal.Value = True
    With frmViewer.ActiveMovie1

'***
' The DisplayMode property does not function with the most recent build
' (IE3.0) of ActiveMovie
        ' Display current position in time, rather than frames.
        '.DisplayMode = amvTime
        'mnu_Options_DisplayTime.Checked = True
'***

        ' Show the display panel.
        .ShowDisplay = True
        mnu_View_DisplayPanel.Checked = True
        
        ' Show the control panel and all other controls.
        .ShowControls = True
        mnu_View_ControlPanel.Checked = True
        .ShowPositionControls = False
        mnu_View_PositionControls.Checked = False
        .ShowSelectionControls = False
        mnu_View_SelectionControls.Checked = False
        .ShowTracker = True
        mnu_View_Tracker.Checked = True
        
        ' Enable the sub-controls on the control panel.
        .EnablePositionControls = False
        mnu_Enable_PositionControls.Checked = False
        .EnableSelectionControls = False
        mnu_Enable_SelectionControls.Checked = False
        .EnableTracker = False
        mnu_Enable_Tracker.Checked = False
        
        ' Set AutoRewind.
        .AutoRewind = True
        mnu_Options_AutoRewind = True
    End With
    
End Sub

Private Sub Form_Unload(Cancel As Integer)

    End ' end program

End Sub






Private Sub mnu_Enable_PositionControls_Click()

    ' Let the user change the prop value from the menu.
    
    If (mnu_Enable_PositionControls.Checked) Then
        mnu_Enable_PositionControls.Checked = False
        frmViewer.ActiveMovie1.EnablePositionControls = False
    Else
        mnu_Enable_PositionControls.Checked = True
        frmViewer.ActiveMovie1.EnablePositionControls = True
    End If
    
End Sub


Private Sub mnu_Enable_SelectionControls_Click()

    ' Let the user change the prop value from the menu.
    
    If (mnu_Enable_SelectionControls.Checked) Then
        mnu_Enable_SelectionControls.Checked = False
        frmViewer.ActiveMovie1.EnableSelectionControls = False
    Else
        mnu_Enable_SelectionControls.Checked = True
        frmViewer.ActiveMovie1.EnableSelectionControls = True
    End If
    
End Sub

Private Sub mnu_Enable_Tracker_Click()
    
    ' Let the user change the prop value from the menu.
    
    If (mnu_Enable_Tracker.Checked) Then
        mnu_Enable_Tracker.Checked = False
        frmViewer.ActiveMovie1.EnableTracker = False
    Else
        mnu_Enable_Tracker.Checked = True
        frmViewer.ActiveMovie1.EnableTracker = True
    End If
    
End Sub
















Private Sub mnu_File_Exit_Click()
    
    End ' End program.

End Sub

Private Sub mnu_File_Open_Click()

    ' Opens an ActiveMovie file.
    ' Displays Viewer form if successful.

    On Error GoTo err_FileOpen
    
    CommonDialog1.Filter = "All files (*.*)|*.*|ActiveMovie files (*.mpg;*.mpa;*.mpv;*.mov;*.mpeg;*.enc;*.m1v;*.mp2)|*.mpg;*.mpa;*.mpv;*.mov;*.mpeg;*.enc;*.m1v;*.mp2|Audio files (.wav)|*.wav|Video for Windows files (.avi)|*.avi"
    CommonDialog1.Flags = 4 'Hide read-only check box
    CommonDialog1.ShowOpen
    
    ' New file resets playback speed to 1.0
    Slider1.Value = 5   ' right in the middle
    ' Default value for ActiveMovie1.Rate = 1.0
    ' If did not escape...
    If CommonDialog1.filename <> "" Then
        frmViewer.ActiveMovie1.filename = CommonDialog1.filename
        g_FileOpened = True
        g_FileExtension = Right$(CommonDialog1.filename, Len(CommonDialog1.filename) - InStr(CommonDialog1.filename, "."))
    Else
        GoTo err_FileOpen
    End If
    
    ' Reset controls and menu settings for new file.
    With frmViewer.ActiveMovie1
        .ShowPositionControls = False
        mnu_View_PositionControls.Checked = False
        .ShowSelectionControls = False
        mnu_View_SelectionControls.Checked = False
        .ShowTracker = True
        mnu_View_Tracker.Checked = True
        
        ' Also, enable the sub-controls on the control panel
        .EnablePositionControls = False
        mnu_Enable_PositionControls.Checked = False
        .EnableSelectionControls = False
        mnu_Enable_SelectionControls.Checked = False
        .EnableTracker = False
        mnu_Enable_Tracker.Checked = False
        
        ' Enable change position buttons.
        ' Can't use buttons for .asf files.
        If g_FileExtension = "asf" Then
            cmdPreviousFrame.Enabled = False
            cmdNextFrame.Enabled = False
        Else
            cmdPreviousFrame.Enabled = True
            cmdNextFrame.Enabled = True
            Slider1.Enabled = True
        End If
        
        ' Enable frame image size panel.
        frmImageSize.Enabled = True
        
    End With
 
    Exit Sub
    
err_FileOpen:

    MsgBox "File open error.", 0, "ActiveMovie OCX Sample"
    
    Exit Sub

End Sub

Private Sub mnu_File_Pause_Click()

    ' Pauses the current file.

    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.Pause
    End If

End Sub

Private Sub mnu_File_Run_Click()

    ' Starts playing the current file.

    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.Run
        frmViewer.ZOrder 0
    End If

End Sub

Private Sub mnu_File_Stop_Click()

    ' Stops file play.

    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.Stop
    End If

End Sub


Private Sub mnu_Options_AutoRewind_Click()

    ' Toggles the AutoRewind property.

    If (mnu_Options_AutoRewind.Checked) Then
        frmViewer.ActiveMovie1.AutoRewind = False
        mnu_Options_AutoRewind.Checked = False
    Else
        frmViewer.ActiveMovie1.AutoRewind = True
        mnu_Options_AutoRewind.Checked = True
    End If
    
End Sub


Private Sub mnu_Options_DisplayTime_Click()
'***
' The DisplayMode property does not function with the most recent build
' (IE3.0) of ActiveMovie.
    
    ' Toggles the DisplayMode between time and frames.
    
'    With frmViewer.ActiveMovie1
'        If (.DisplayMode = amvTime) Then
'            .DisplayMode = amvFrames
'            mnu_Options_DisplayTime.Checked = False
'        Else
'            .DisplayMode = amvTime
'            mnu_Options_DisplayTime.Checked = True
'        End If
'    End With
'***
    
End Sub







Private Sub mnu_View_ControlPanel_Click()

    ' Toggles the ShowControls property.
    
    With frmViewer.ActiveMovie1
        If (.AllowHideControls) Then
            If (.ShowControls) Then
                .ShowControls = False
            Else
                .ShowControls = True
            End If
            ' Toggle menu check mark.
            mnu_View_ControlPanel.Checked = Not mnu_View_ControlPanel.Checked
        End If
    End With
    
    ResizeViewer

End Sub


Private Sub mnu_View_DisplayPanel_Click()
    
    ' Toggles ShowDisplay property.
    
    With frmViewer.ActiveMovie1
        If (.AllowHideDisplay) Then
            If (.ShowDisplay) Then
                .ShowDisplay = False
            Else
                .ShowDisplay = True
            End If
            ' Toggle menu check mark.
            mnu_View_DisplayPanel.Checked = Not mnu_View_DisplayPanel.Checked
        End If
    End With
    
    ResizeViewer

End Sub


Private Sub mnu_View_PositionControls_Click()
    
    ' Toggles ShowPositionControls property.
    
    frmViewer.ActiveMovie1.ShowPositionControls = Not frmViewer.ActiveMovie1.ShowPositionControls
    mnu_View_PositionControls.Checked = Not mnu_View_PositionControls.Checked
    
    ResizeViewer
    
End Sub

Private Sub mnu_View_SelectionControls_Click()
    
    ' Toggles ShowSelectionControls property.
    
    frmViewer.ActiveMovie1.ShowSelectionControls = Not frmViewer.ActiveMovie1.ShowSelectionControls
    mnu_View_SelectionControls.Checked = Not mnu_View_SelectionControls.Checked
    
    ResizeViewer

End Sub

Private Sub mnu_View_Tracker_Click()
    
    ' Toggles ShowTracker property.
    
    frmViewer.ActiveMovie1.ShowTracker = Not frmViewer.ActiveMovie1.ShowTracker
    mnu_View_Tracker.Checked = Not mnu_View_Tracker.Checked
    
    ResizeViewer

End Sub





Private Sub obDoubleOriginal_Click()
  ' Set image size to double original size.
    
    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.MovieWindowSize = amvDoubleOriginalSize
        Call ResizeViewer
    End If
End Sub









Private Sub obOneFourthScreen_Click()
  ' Set image size to one fourth screen.
    
    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.MovieWindowSize = amvOneFourthScreen
        Call ResizeViewer
    End If
End Sub

Private Sub obOneHalfScreen_Click()
  ' Set image size to one half screen.
    
    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.MovieWindowSize = amvOneHalfScreen
        Call ResizeViewer
    End If
End Sub


Private Sub obOneSixteenthScreen_Click()
  ' Set image size to one sixteenth screen.
    
    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.MovieWindowSize = amvOneSixteenthScreen
        Call ResizeViewer
    End If
End Sub

Private Sub obOriginal_Click()
    ' Set image size to original.

    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.MovieWindowSize = amvOriginalSize
        Call ResizeViewer
    End If
    
End Sub

Private Sub Slider1_Change()

    ' Set playback rate.
    ' 10 settings, each setting = 0.1
    
    If g_FileOpened = True Then
        frmViewer.ActiveMovie1.Rate = 0.5 + (0.1 * Slider1.Value)
    End If
    
End Sub
