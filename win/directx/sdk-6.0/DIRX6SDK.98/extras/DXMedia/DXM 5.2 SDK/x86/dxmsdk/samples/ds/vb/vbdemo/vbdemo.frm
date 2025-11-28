VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "COMCTL32.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Begin VB.Form frmMain 
   Caption         =   "ActiveMovie VB Sample"
   ClientHeight    =   6465
   ClientLeft      =   1830
   ClientTop       =   2880
   ClientWidth     =   5850
   ClipControls    =   0   'False
   LinkTopic       =   "frmMain"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   6465
   ScaleWidth      =   5850
   Begin VB.Frame frameBalance 
      Caption         =   "Balance"
      Height          =   1215
      Left            =   2880
      TabIndex        =   13
      Top             =   1560
      Width           =   2295
      Begin ComctlLib.Slider slBalance 
         Height          =   504
         Left            =   120
         TabIndex        =   14
         Top             =   240
         Width           =   2052
         _ExtentX        =   3625
         _ExtentY        =   900
         _Version        =   327680
         Min             =   -10000
         Max             =   10000
         TickStyle       =   2
         TickFrequency   =   2000
      End
      Begin VB.Label lblRight 
         Caption         =   "Right"
         Height          =   255
         Left            =   1680
         TabIndex        =   16
         Top             =   840
         Width           =   495
      End
      Begin VB.Label lblLeft 
         Caption         =   "Left"
         Height          =   255
         Left            =   120
         TabIndex        =   15
         Top             =   840
         Width           =   495
      End
   End
   Begin VB.Frame frameVideoControls 
      Caption         =   "Video"
      Height          =   1335
      Left            =   240
      TabIndex        =   10
      Top             =   0
      Width           =   2295
      Begin ComctlLib.Toolbar Toolbar1 
         Height          =   600
         Left            =   240
         TabIndex        =   12
         Top             =   360
         Width           =   1935
         _ExtentX        =   3413
         _ExtentY        =   1058
         ButtonWidth     =   979
         ButtonHeight    =   953
         ImageList       =   "ImageList1"
         _Version        =   327680
         BeginProperty Buttons {0713E452-850A-101B-AFC0-4210102A8DA7} 
            NumButtons      =   5
            BeginProperty Button1 {0713F354-850A-101B-AFC0-4210102A8DA7} 
               Caption         =   "Play"
               Object.Tag             =   ""
               ImageIndex      =   1
            EndProperty
            BeginProperty Button2 {0713F354-850A-101B-AFC0-4210102A8DA7} 
               Object.Tag             =   ""
               Style           =   3
            EndProperty
            BeginProperty Button3 {0713F354-850A-101B-AFC0-4210102A8DA7} 
               Caption         =   "Pause"
               Object.Tag             =   ""
               ImageIndex      =   2
            EndProperty
            BeginProperty Button4 {0713F354-850A-101B-AFC0-4210102A8DA7} 
               Object.Tag             =   ""
               Style           =   3
            EndProperty
            BeginProperty Button5 {0713F354-850A-101B-AFC0-4210102A8DA7} 
               Caption         =   "Stop"
               Object.Tag             =   ""
               ImageIndex      =   3
            EndProperty
         EndProperty
      End
   End
   Begin VB.TextBox txtRate 
      BackColor       =   &H00000000&
      BeginProperty Font 
         Name            =   "Terminal"
         Size            =   9
         Charset         =   255
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   270
      Left            =   4440
      TabIndex        =   7
      Top             =   1200
      Width           =   855
   End
   Begin VB.TextBox txtStart 
      BackColor       =   &H00000000&
      BeginProperty Font 
         Name            =   "Terminal"
         Size            =   9
         Charset         =   255
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   270
      Left            =   4440
      TabIndex        =   6
      Top             =   840
      Width           =   855
   End
   Begin VB.TextBox txtElapsed 
      Alignment       =   2  'Center
      BackColor       =   &H00000000&
      BeginProperty Font 
         Name            =   "Terminal"
         Size            =   9
         Charset         =   255
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   270
      Left            =   4440
      TabIndex        =   4
      Top             =   480
      Width           =   855
   End
   Begin VB.Timer Timer1 
      Left            =   5160
      Top             =   4680
   End
   Begin VB.TextBox txtDuration 
      Alignment       =   2  'Center
      BackColor       =   &H00000000&
      BeginProperty Font 
         Name            =   "Terminal"
         Size            =   9
         Charset         =   255
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   270
      Left            =   4440
      TabIndex        =   3
      Top             =   120
      Width           =   855
   End
   Begin VB.Frame frameVolume 
      Caption         =   "Volume"
      Height          =   1215
      Left            =   360
      TabIndex        =   0
      Top             =   1560
      Width           =   2295
      Begin ComctlLib.Slider slVolume 
         Height          =   504
         Left            =   120
         TabIndex        =   11
         Top             =   240
         Width           =   2052
         _ExtentX        =   3625
         _ExtentY        =   900
         _Version        =   327680
         Min             =   -10000
         Max             =   0
         TickStyle       =   2
         TickFrequency   =   1000
      End
      Begin VB.Label lblMax 
         Caption         =   "Max"
         Height          =   255
         Left            =   1800
         TabIndex        =   2
         Top             =   840
         Width           =   375
      End
      Begin VB.Label lblMin 
         Caption         =   "Min"
         Height          =   255
         Left            =   120
         TabIndex        =   1
         Top             =   840
         Width           =   495
      End
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   4920
      Top             =   3120
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
   End
   Begin VB.Label lblDuration 
      Caption         =   "Length (Sec):"
      Height          =   255
      Left            =   2760
      TabIndex        =   17
      Top             =   120
      Width           =   1455
   End
   Begin ComctlLib.ImageList ImageList1 
      Left            =   5040
      Top             =   3840
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   327680
      BeginProperty Images {0713E8C2-850A-101B-AFC0-4210102A8DA7} 
         NumListImages   =   3
         BeginProperty ListImage1 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbdemo.frx":0000
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbdemo.frx":0112
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "vbdemo.frx":0224
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Label lblRate 
      Caption         =   "Playback speed:"
      Height          =   255
      Left            =   2760
      TabIndex        =   9
      Top             =   1200
      Width           =   1335
   End
   Begin VB.Label lblStart 
      Caption         =   "Start position (Sec):"
      Height          =   255
      Left            =   2760
      TabIndex        =   8
      Top             =   840
      Width           =   1455
   End
   Begin VB.Label lblElapsed 
      Caption         =   "Elapsed Time (Sec):"
      Height          =   252
      Left            =   2760
      TabIndex        =   5
      Top             =   480
      Width           =   1572
   End
   Begin VB.Shape Shape1 
      BackStyle       =   1  'Opaque
      BorderColor     =   &H00808080&
      BorderWidth     =   4
      Height          =   3135
      Left            =   240
      Top             =   3000
      Width           =   4095
   End
   Begin VB.Menu mnu_File 
      Caption         =   "&File"
      Begin VB.Menu mnu_FileOpen 
         Caption         =   "&Open"
      End
      Begin VB.Menu mnu_FileExit 
         Caption         =   "E&xit"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ActiveMovie SDK - VB4 interface sample
' Copyright 1996 - 1997 Microsoft Corporation
' This sample demonstrates Visual Basic 4.0 use of the
' ActiveMovie interfaces that are in FilgraphManager.

' This sample defines the following variable prefixes:
'  g_dbl - global double;
'  g_f   - global flag (Boolean);
'  g_obj - object;
'  g_str - string

Dim g_objVideoWindow As IVideoWindow       'VideoWindow Object
Dim g_objMediaControl As IMediaControl     'MediaControl Object
Dim g_objMediaPosition As IMediaPosition   'MediaPosition Object
Dim g_objBasicAudio  As IBasicAudio        'Basic Audio Object
Dim g_objBasicVideo As IBasicVideo         'Basic Video Object

Dim g_strFileName As String      'Name retrieved with FileOpenDlg
Dim g_dblRunLength As Double     'Global duration in seconds
Dim g_dblStartPosition As Double 'Global start position in seconds
Dim g_dblRate As Double          'Global rate
Dim g_fVideoRun As Boolean       'Flag used to trigger clock

Private Sub Form_Load()
    'Alter the coordinate system so that we work
    'in pixels (instead of the default twips)
    
    frmMain.ScaleMode = 3   ' pixels
    
    'Set the granularity for the timer control
    'so that we can display the duration for
    'given video sequence.
    
    Timer1.Interval = 1000 '1 second intervals

End Sub

Private Sub Form_Unload(Cancel As Integer)
    mnu_FileExit_Click
End Sub

Private Sub mnu_FileExit_Click()
    cleanup
    End
End Sub

Private Sub mnu_FileOpen_Click()
' Use the common file dialog to select an ActiveMovie file
' (has the extension .AVI or .MPG.)
' Initialize global variables based on the
' contents of the file:
'   g_strFileName - name of file name selected by the user
'   g_dblRunLength = length of the file; duration
'   g_dblStartPosition - point at which to start playing clip
'   g_objMediaControl, g_objMediaEvent, g_objMediaPosition,
'   g_objBasicAudio, g_objVideoWindow - programmable objects

    'clean up memory (in case a file is already open)
    cleanup
    
    'Retrieve the name of an .avi or an .mpg
    'file that the user wishes to view.
    CommonDialog1.Filter = "ActiveMovie files (*.mpg;*.avi;*.mov)|*.mpg;*.avi;*.mov"
    CommonDialog1.ShowOpen
    g_strFileName = CommonDialog1.filename
    
    'Verify that correct filename was selected
    
    If (InStr(1, g_strFileName, ".avi", 1)) Or (InStr(1, g_strFileName, ".mpg", 1) Or (InStr(1, g_strFileName, ".mov", 1))) Then
    Else
        MsgBox "Invalid Filename Selected!", , "ActiveMovie sample"
        Exit Sub
    End If
    
    'Instantiate a filter graph for the requested
    'file format.
    
    Set g_objMediaControl = New FilgraphManager
    If g_objMediaControl Is Nothing Then
        MsgBox "Cannot create the IMediaControl object"
        Exit Sub
    End If
        
    g_objMediaControl.RenderFile (g_strFileName)
    
    'Setup the IBasicAudio object (this
    'is equivalent to calling QueryInterface()
    'on IFilterGraphManager). Initialize the volume
    'to the maximum value.
    
    ' Some filter graphs don't render audio
    ' In this sample, skip setting volume property
    Set g_objBasicAudio = g_objMediaControl
    On Error Resume Next
    g_objBasicAudio.Volume = slVolume.Value
  
    'Setup the IVideoWindow object. Remove the
    'caption, border, dialog frame, and scrollbars
    'from the default window. Position the window.
    'Set the parent to the app's form.
    
    Set g_objVideoWindow = g_objMediaControl
    g_objVideoWindow.WindowStyle = CLng(&H6000000)
    g_objVideoWindow.Left = CLng(Shape1.Left)
    g_objVideoWindow.Top = CLng(Shape1.Top)
    Shape1.Width = g_objVideoWindow.Width
    Shape1.Height = g_objVideoWindow.Height
    g_objVideoWindow.Owner = frmMain.hWnd
         
    'Setup the IMediaEvent object for the
    'sample toolbar (run, pause, play).
    
    Set g_objMediaEvent = g_objMediaControl
    
    'Setup the IMediaPosition object so that we
    'can display the duration of the selected
    'video as well as the elapsed time.
    
    Set g_objMediaPosition = g_objMediaControl
    g_dblRunLength = g_objMediaPosition.Duration
    txtDuration.Text = CStr(g_dblRunLength)
    
    ' reset start position to 0
    g_dblStartPosition = 0#
    txtStart.Text = CDbl(g_dblStartPosition)
    ' use user-established playback rate
    g_dblRate = g_objMediaPosition.Rate
    txtRate.Text = CStr(g_dblRate)

End Sub

Private Sub slVolume_Change()
    
    'Set the volume on the slider
    If Not g_objMediaControl Is Nothing Then
        g_objBasicAudio.Volume = slVolume.Value
    End If
    
End Sub

Private Sub slBalance_Change()
  
    'Set the balance using the slider
    If Not g_objMediaControl Is Nothing Then
        g_objBasicAudio.Balance = (slBalance.Value * -1)
    End If
    
End Sub

Private Sub txtStart_KeyDown(KeyCode As Integer, Shift As Integer)

    ' handle user input to change the start position
    If KeyCode = vbKeyReturn Then
        If g_objMediaPosition Is Nothing Then
            Exit Sub
        ElseIf CDbl(txtStart.Text) > g_dblRunLength Then
            MsgBox "Specified position invalid: re-enter new position."
        ElseIf CDbl(txtStart.Text) < 0 Then
            MsgBox "Specified position invalid: re-enter new position."
        ElseIf CStr(txtStart.Text) <> "" Then
            g_dblStartPosition = CDbl(txtStart.Text)
            g_objMediaPosition.CurrentPosition = g_dblStartPosition
        End If
    End If

End Sub

Private Sub txtRate_KeyDown(KeyCode As Integer, Shift As Integer)
    
    ' ActiveMovie VB sample
    ' handle user updates to the Rate value
    If KeyCode = vbKeyReturn Then
        If g_objMediaPosition Is Nothing Then
            Exit Sub
        ElseIf CDbl(txtRate.Text) < 0# Then
            MsgBox "Negative values invalid: re-enter value between 0 and 2.0"
        ElseIf CStr(txtRate.Text) <> "" Then
            g_dblRate = CDbl(txtRate.Text)
            g_objMediaPosition.Rate = g_dblRate
        End If
    End If
    
End Sub

Private Sub Timer1_Timer()
    'Retrieve the Elapsed Time and
    'display it in the corresponding
    'textbox.
    Dim Dbl As Double

    If g_fVideoRun = True Then
        Dbl = g_objMediaPosition.CurrentPosition
        If Dbl < g_dblRunLength Then
            txtElapsed.Text = CStr(Dbl)
        Else
            txtElapsed.Text = CStr(g_dblRunLength)
        End If
    End If

End Sub

Private Sub Toolbar1_ButtonClick(ByVal Button As Button)
' handle buttons on the toolbar
' buttons 1, 3 and 5 are defined; 2 and 4 are separators
' all ActiveMovie objects are defined only if the user
' has already selected a filename and initialized the objects

    ' if the objects aren't defined, avoid errors
    If g_objMediaControl Is Nothing Then
        Exit Sub
    End If
    
    If Button.Index = 1 Then 'PLAY
        'Invoke the MediaControl Run() method
        'and play the video through the predefined
        'filter graph.
        
        ' Use specified starting position
        If g_objMediaPosition.CurrentPosition < g_dblStartPosition Then
            g_objMediaPosition.CurrentPosition = g_dblStartPosition
        End If
    
        g_objMediaControl.Run
        g_fVideoRun = True
            
    ElseIf Button.Index = 3 Then  'PAUSE
        'Invoke the MediaControl Pause() method
        'and pause the video that is being
        'displayed through the predefined
        'filter graph.
    
        g_objMediaControl.Pause
        g_fVideoRun = False
        
    ElseIf Button.Index = 5 Then  'STOP
    
        'Invoke the MediaControl Stop() method
        'and stop the video that is being
        'displayed through the predefined
        'filter graph.
    
        g_objMediaControl.Stop
        g_fVideoRun = False
        ' reset to the beginning of the video
        g_objMediaPosition.CurrentPosition = 0
        txtElapsed.Text = "0.0"
        
    End If
End Sub

Private Sub cleanup()
    
    ' clean up memory
    If Not g_objMediaControl Is Nothing Then
        g_objMediaControl.Stop
        Debug.Print "Setting owner to NULL" + Chr(13)
        g_objVideoWindow.Owner = 0          'sets the Owner to NULL
    End If
    Set g_objVideoWindow = Nothing
    Set g_objMediaControl = Nothing
    Set g_objMediaPosition = Nothing
    Set g_objBasicAudio = Nothing
    Set g_objBasicVideo = Nothing

End Sub
