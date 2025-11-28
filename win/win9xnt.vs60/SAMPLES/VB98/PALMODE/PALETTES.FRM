VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "PaletteMode Demo"
   ClientHeight    =   5085
   ClientLeft      =   1860
   ClientTop       =   1530
   ClientWidth     =   6465
   LinkTopic       =   "Form1"
   ScaleHeight     =   5085
   ScaleWidth      =   6465
   Begin VB.OptionButton Option1 
      Caption         =   "Custom (Pastel DIB)"
      Height          =   255
      Index           =   3
      Left            =   4440
      TabIndex        =   6
      Top             =   240
      Width           =   1935
   End
   Begin VB.CommandButton cmdQuit 
      Cancel          =   -1  'True
      Caption         =   "Quit"
      Height          =   495
      Left            =   5040
      TabIndex        =   4
      Top             =   4080
      Width           =   1215
   End
   Begin VB.PictureBox Picture1 
      AutoSize        =   -1  'True
      Height          =   3855
      Left            =   240
      ScaleHeight     =   3795
      ScaleWidth      =   4035
      TabIndex        =   3
      Top             =   720
      Width           =   4095
   End
   Begin VB.OptionButton Option1 
      Caption         =   "Custom (Grayscale)"
      Height          =   255
      Index           =   2
      Left            =   2520
      TabIndex        =   2
      Top             =   240
      Width           =   1695
   End
   Begin VB.OptionButton Option1 
      Caption         =   "ZOrder"
      Height          =   255
      Index           =   1
      Left            =   1440
      TabIndex        =   1
      Top             =   240
      Width           =   855
   End
   Begin VB.OptionButton Option1 
      Caption         =   "Halftone"
      Height          =   255
      Index           =   0
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Value           =   -1  'True
      Width           =   1095
   End
   Begin VB.Timer Timer1 
      Interval        =   800
      Left            =   5160
      Top             =   2880
   End
   Begin VB.Label Label1 
      Caption         =   "Note: For full effect, this demo should be run in 256 color mode."
      Height          =   255
      Left            =   240
      TabIndex        =   5
      Top             =   4800
      Width           =   6135
   End
   Begin VB.Image Image1 
      BorderStyle     =   1  'Fixed Single
      Height          =   1290
      Left            =   4680
      Picture         =   "Palettes.frx":0000
      Stretch         =   -1  'True
      Top             =   720
      Width           =   1620
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'// Sample application to demonstrate the PaletteMode ///
'// and Palette properties for forms and controls.    ///
'// Version 1.0 9/16/1996                             ///
'//                                                   ///
'// Note: this sample should be run in 256 color mode ///
'// Palette & PaletteMode have no effect in other     ///
'// color modes.

Option Explicit
Dim objPic As Picture    ' Picture object for option 4.

Private Sub cmdQuit_Click()
    Unload Me
End Sub

Private Sub Form_Load()
    ' Load a 256 color DIB into the picture object
    Set objPic = LoadPicture(App.Path & "\PASTEL.DIB")
End Sub

Private Sub Option1_Click(Index As Integer)
    Timer1.Enabled = False
    Select Case Index
        Case 0
            ' Use the Halftone mode (default).
            Form1.PaletteMode = vbPaletteModeHalftone
        Case 1
            ' Use the palette of the loaded picture -
            ' color "flashing" will result (ZOrder mode).
            Form1.PaletteMode = vbPaletteModeUseZOrder
        Case 2
            'Set Form1.Palette = Nothing
            ' Assign the palette from Image1 to the form.
            Form1.Palette = Image1.Picture
            ' Use the Custom mode.
            Form1.PaletteMode = vbPaletteModeCustom
        Case 3
            'Set Form1.Palette = Nothing
            ' Assign picture object's palette to the form.
            Form1.Palette = objPic
            ' Use the Custom mode.
            Form1.PaletteMode = vbPaletteModeCustom
    End Select
    Picture1.Refresh
    Timer1.Enabled = True
End Sub

Private Sub Timer1_Timer()
    Static intC As Integer
    
    ' Switch between three different images.
    If intC < 1 Then
        Picture1 = LoadPicture(App.Path & "\BANNER.GIF")
        intC = 1
    ElseIf intC = 1 Then
        Picture1 = LoadPicture(App.Path & "\CLOUDS.BMP")
        intC = 2
    Else
        Picture1 = LoadPicture(App.Path & "\FOREST.JPG")
        intC = 0
    End If
End Sub

