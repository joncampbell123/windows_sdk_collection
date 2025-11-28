VERSION 5.00
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#1.1#0"; "COMCTL32.OCX"
Begin VB.Form Pictures 
   AutoRedraw      =   -1  'True
   Caption         =   "Picture vs. Image Controls"
   ClientHeight    =   3495
   ClientLeft      =   2100
   ClientTop       =   2085
   ClientWidth     =   6390
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H80000008&
   LinkTopic       =   "Form4"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   233
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   426
   Tag             =   "Display"
   WhatsThisHelp   =   -1  'True
   Begin VB.PictureBox Picture1 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      AutoSize        =   -1  'True
      BackColor       =   &H80000005&
      ForeColor       =   &H80000008&
      Height          =   2280
      Index           =   0
      Left            =   3240
      Picture         =   "FRMPICS.frx":0000
      ScaleHeight     =   2250
      ScaleWidth      =   3000
      TabIndex        =   0
      Top             =   480
      Width           =   3030
   End
   Begin MSComCtlLib.StatusBar sbStatus 
      Align           =   2  'Align Bottom
      Height          =   285
      Left            =   0
      TabIndex        =   1
      Top             =   3210
      Width           =   6390
      _ExtentX        =   11271
      _ExtentY        =   503
      Style           =   1
      SimpleText      =   ""
      BeginProperty Panels {2C787A51-E01C-11CF-8E74-00A0C90F26F8} 
         NumPanels       =   1
         BeginProperty Panel1 {2C787A53-E01C-11CF-8E74-00A0C90F26F8} 
            AutoSize        =   1
            Object.Width           =   10780
            TextSave        =   ""
            Key             =   ""
            Object.Tag             =   ""
         EndProperty
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      MouseIcon       =   "FRMPICS.frx":7972
   End
   Begin VB.Label Label3 
      Caption         =   "Click on the images to test resource usage"
      Height          =   255
      Left            =   1320
      TabIndex        =   4
      Top             =   2880
      Width           =   3735
   End
   Begin VB.Label Label2 
      Caption         =   "PictureBox control"
      Height          =   375
      Left            =   3840
      TabIndex        =   3
      Top             =   120
      Width           =   1935
   End
   Begin VB.Label Label1 
      Caption         =   "Image control"
      Height          =   255
      Left            =   840
      TabIndex        =   2
      Top             =   120
      Width           =   2055
   End
   Begin VB.Image Image1 
      Appearance      =   0  'Flat
      Height          =   2250
      Index           =   0
      Left            =   120
      Picture         =   "FRMPICS.frx":798E
      Top             =   480
      Width           =   3000
   End
End
Attribute VB_Name = "Pictures"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Form_Load()
  PosForm Me
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Set frmPics = Nothing
End Sub

Private Sub Image1_Click(Index As Integer)
  
  On Error Resume Next
  
  Dim i As Integer, j As Integer
    
  Screen.MousePointer = vbHourglass
    i = 1
    Do
        Load Image1(i)
        Image1(i).Move Image1(i - 1).Left + 4, Image1(i - 1).TOP + 4
        Image1(i).Visible = True
        i = i + 1
        If i Mod 20 = 0 Then
          sbStatus.SimpleText = Str$(i) & " Image Controls"
          DoEvents
        End If
    Loop Until Err Or i = 5000
    For j = i To 1 Step -1
        Unload Image1(i)
        Set Image1(i) = Nothing
    Next j
    frmPictures.Refresh
    frmExplore.Refresh
    MsgBox "Allocated " & Str$(i) & " Image Controls", vbInformation, "Image Control"
    Screen.MousePointer = vbDefault
End Sub

Private Sub Picture1_Click(Index As Integer)

On Error Resume Next

Dim i As Integer, j As Integer
    i = 1
    Screen.MousePointer = vbHourglass
    Do
        Load Picture1(i)
        Picture1(i).Move Picture1(i - 1).Left + 4, Picture1(i - 1).TOP + 4
        Picture1(i).Visible = True
        If i Mod 20 = 0 Then
          sbStatus.SimpleText = Str$(i) & " Picture Controls"
          sbStatus.Refresh
        End If
        i = i + 1
    Loop Until Err
    
    sbStatus.SimpleText = Str$(i) & " Picture Controls"
    sbStatus.Refresh
    
    For j = i To 1 Step -1
        Unload Picture1(i)
        Set Picture1(i) = Nothing
    Next j
    
    frmPictures.Refresh
    frmExplore.Refresh
    Screen.MousePointer = vbDefault
    MsgBox "Allocated " & Str$(i) & " Picture Controls", vbInformation, "Picture Controls"

End Sub

