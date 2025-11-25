VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "COMCTL32.OCX"
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   1545
   ClientLeft      =   3915
   ClientTop       =   1530
   ClientWidth     =   4515
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1545
   ScaleWidth      =   4515
   Begin VB.Timer Timer1 
      Interval        =   200
      Left            =   3720
      Top             =   1200
   End
   Begin VB.CommandButton StopButton 
      Caption         =   "&Stop"
      Height          =   495
      Left            =   2160
      TabIndex        =   3
      Top             =   120
      Width           =   735
   End
   Begin VB.CommandButton PlayButton 
      Caption         =   "&Play"
      Height          =   495
      Left            =   1200
      TabIndex        =   2
      Top             =   120
      Width           =   735
   End
   Begin VB.CommandButton OpenButton 
      Caption         =   "&Open"
      Height          =   495
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   735
   End
   Begin ComctlLib.Slider Slider1 
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   840
      Width           =   3735
      _ExtentX        =   6588
      _ExtentY        =   661
      _Version        =   327680
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   3240
      Top             =   1200
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim pMC As Object
Dim bOpen As Boolean
Dim bPlay As Boolean
Dim bSeeking As Boolean

Private Sub Form_Load()

Set pMC = Nothing
bOpen = False
bPlay = False
bSeeking = False

End Sub




Private Sub OpenButton_Click()
    If bPlay Then pMC.Stop
    If bOpen Then Set pMC = Nothing
    bOpen = False

    Set pMC = New FilgraphManager

    CommonDialog1.ShowOpen
    pMC.RenderFile CommonDialog1.filename

    Rem make window appear
    Dim pVW As IVideoWindow
    Set pVW = pMC
    pVW.Visible = True
    Set pVW = Nothing

    bOpen = True


End Sub

Private Sub PlayButton_Click()
    If Not bOpen Then Exit Sub
    If bPlay Then Exit Sub

    pMC.Run
    bPlay = True
    OpenButton.Enabled = False
    PlayButton.Enabled = False

End Sub



Private Sub Slider1_Change()

bSeeking = False
If Not bPlay Then
pMC.Stop
End If

End Sub

Private Sub Slider1_Scroll()

If Not bOpen Then Exit Sub

bSeeking = True
pMC.Pause

Dim pMP As IMediaPosition
Set pMP = pMC

Dim v As Double
v = Slider1.Value - Slider1.Min
v = v / Slider1.Max * pMP.Duration
pMP.CurrentPosition = v
Set pMP = Nothing


End Sub

Private Sub StopButton_Click()
    If Not bOpen Then Exit Sub
    pMC.Stop
    bPlay = False
    OpenButton.Enabled = True
    PlayButton.Enabled = True

End Sub

Private Sub Timer1_Timer()

    If Not bOpen Then Exit Sub

    If bSeeking Then Exit Sub

    Dim pMP As IMediaPosition
    Set pMP = pMC

    Dim curpos As Double
    Dim length As Double
    Dim mark As Long

    curpos = pMP.CurrentPosition
    length = pMP.Duration
    mark = (curpos / length) * 10

    If Slider1.Value <> mark Then
        Slider1.Value = mark
    End If

    Set pMP = Nothing

    If Not bPlay Then Exit Sub

    Dim pME As IMediaEvent
    Dim EventCode As Long
    Set pME = pMC

    On Error Resume Next
    pME.WaitForCompletion 0, EventCode

    Set pME = Nothing

    If EventCode = 0 Then Exit Sub

    pMC.Stop
    bPlay = False
    OpenButton.Enabled = True
    PlayButton.Enabled = True

End Sub



