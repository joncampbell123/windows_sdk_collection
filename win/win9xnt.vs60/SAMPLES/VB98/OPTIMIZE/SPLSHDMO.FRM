VERSION 5.00
Begin VB.Form frmSplashDemo 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Using a Splash Screen"
   ClientHeight    =   1710
   ClientLeft      =   1515
   ClientTop       =   1470
   ClientWidth     =   5565
   ClipControls    =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   114
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   371
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton cmdAppStartup 
      Caption         =   "With&out  Splash"
      Height          =   500
      Index           =   1
      Left            =   3015
      TabIndex        =   1
      Top             =   1080
      Width           =   1395
   End
   Begin VB.CommandButton cmdAppStartup 
      Caption         =   "&With  Splash"
      Height          =   500
      Index           =   0
      Left            =   915
      TabIndex        =   0
      Top             =   1080
      Width           =   1215
   End
   Begin VB.Label Label1 
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Using a splash screen provides the user with feedback while your application is loading."
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00800000&
      Height          =   720
      Left            =   30
      TabIndex        =   2
      Top             =   45
      Width           =   5490
   End
End
Attribute VB_Name = "frmSplashDemo"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdAppStartup_Click(Index As Integer)
  Dim dStartTime As Double
  
  dStartTime = Timer
  
    Select Case Index
      Case 0  'With Splash
        ShowSplash
      Case 1  'No Splash
        NoSplash
    End Select
    
    MsgBox "Startup Time = " & Format$(Timer - dStartTime, "##.##") & " secs.", _
                                            vbInformation, _
                                            "App Startup Time"
  End Sub

Private Sub NoSplash()
            
  ' Now load a bunch of forms.
  Dim foo1 As New frmImages
  foo1.Caption = "Foo1"
  foo1.Left = 0
  foo1.Top = 0
          
  Dim foo2 As New frmImages
  foo2.Caption = "Foo2"
  foo2.Left = 200
  foo2.Top = 100
          
  Dim foo3 As New frmImages
  foo3.Caption = "Foo3"
  foo3.Left = 300
  foo3.Top = 150
          
  Dim foo4 As New frmImages
  foo4.Caption = "Foo4"
  foo4.Left = 400
  foo4.Top = 200

End Sub


Private Sub ShowSplash()
  Dim success%
  Dim iStatusBarWidth As Integer
  
  On Error GoTo SplashLoadErr
  
  iStatusBarWidth = 4575
            
  Screen.MousePointer = vbHourglass
  Load Splash
  Splash.Show
  DoEvents
  ' Set the splash screen to stay on top.
  success% = SetWindowPos(Splash.hWnd, HWND_TOPMOST, 0, 0, 0, 0, FLAGS)

  ' Now load a bunch of forms.
  Dim foo1 As New frmImages
  'Splash.Refresh
  foo1.Caption = "Foo1"
  foo1.Move 0, 0
  DoEvents
  Splash.rctStatusBar.Width = iStatusBarWidth * 0.25
          
  Dim foo2 As New frmImages
  'Splash.Refresh
  foo2.Caption = "Foo2"
  foo2.Move 0, 0
  DoEvents
  Splash.rctStatusBar.Width = iStatusBarWidth * 0.5

  Dim foo3 As New frmImages
  'Splash.Refresh
  foo3.Caption = "Foo3"
  foo3.Move 0, 0
  DoEvents
  Splash.rctStatusBar.Width = iStatusBarWidth * 0.75
          
  Dim foo4 As New frmImages
  'Splash.Refresh
  foo4.Caption = "Foo4"
  foo4.Move 0, 0
  DoEvents
  Splash.rctStatusBar.Width = iStatusBarWidth

  ' Turn off the top most window flag.
  success% = SetWindowPos(Splash.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, FLAGS)
  Unload Splash
  Screen.MousePointer = vbDefault
  Exit Sub

SplashLoadErr:
  success% = SetWindowPos(Splash.hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, FLAGS)
  Unload Splash
  Screen.MousePointer = vbDefault
  MsgBox Error$ & " - " & Str$(Err), vbExclamation, "Application Load Error"
  Exit Sub

End Sub
Private Sub Form_Load()
  Me.Left = frmExplore.Width + 400
  Me.Top = (Screen.Height - Me.Height) * 0.9
End Sub


