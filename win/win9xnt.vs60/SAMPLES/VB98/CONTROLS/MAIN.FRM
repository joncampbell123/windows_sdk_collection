VERSION 5.00
Begin VB.Form frmMain 
   Caption         =   "Control Examples"
   ClientHeight    =   2910
   ClientLeft      =   210
   ClientTop       =   1935
   ClientWidth     =   4830
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form2"
   ScaleHeight     =   2910
   ScaleWidth      =   4830
   Begin VB.CommandButton cmdImages 
      Caption         =   "&Images"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2640
      TabIndex        =   5
      Top             =   1440
      Width           =   1575
   End
   Begin VB.CommandButton cmdOption 
      Caption         =   "O&ption Buttons"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2640
      TabIndex        =   4
      Top             =   840
      Width           =   1575
   End
   Begin VB.CommandButton cmdCheck 
      Caption         =   "&Check Box"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2640
      TabIndex        =   3
      Top             =   240
      Width           =   1575
   End
   Begin VB.CommandButton cmdText 
      Caption         =   "Te&xt Box"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   480
      TabIndex        =   2
      Top             =   1440
      Width           =   1575
   End
   Begin VB.CommandButton cmdWordWrap 
      Caption         =   "&Word Wrap"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   480
      TabIndex        =   1
      Top             =   840
      Width           =   1575
   End
   Begin VB.CommandButton cmdButtons 
      Caption         =   "&Test Buttons"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   480
      TabIndex        =   0
      Top             =   240
      Width           =   1575
   End
   Begin VB.CommandButton cmdExit 
      Cancel          =   -1  'True
      Caption         =   "E&xit"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2640
      TabIndex        =   6
      Top             =   2160
      Width           =   1575
   End
   Begin VB.Menu mnuFileMain 
      Caption         =   "&File"
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuOptionsMain 
      Caption         =   "&Options"
      Begin VB.Menu mnuButtons 
         Caption         =   "&Test Buttons"
      End
      Begin VB.Menu mnuWordWrap 
         Caption         =   "&WordWrap"
      End
      Begin VB.Menu mnuText 
         Caption         =   "Te&xt Box Properties"
      End
      Begin VB.Menu mnuCheck 
         Caption         =   "&Check Box"
      End
      Begin VB.Menu mnuOption 
         Caption         =   "O&ption Buttons"
      End
      Begin VB.Menu mnuImages 
         Caption         =   "&Images"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_Base = "0{A2B7C66B-E9E1-11CF-84BA-00AA00C007F0}"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub cmdButtons_Click()
    ' invoke a Click event in the menu
    mnuButtons_Click
End Sub


Private Sub cmdCheck_Click()
    ' invoke a Click event in the menu
    mnuCheck_Click
End Sub

Private Sub cmdExit_Click()
    Unload Me       ' unload the form
    End             ' end the application
End Sub



Private Sub cmdImages_Click()
    ' invoke a Click event in the menu
    mnuImages_Click
End Sub

Private Sub cmdOption_Click()
    ' invoke a Click event in the menu
    mnuOption_Click
End Sub

Private Sub cmdText_Click()
    ' invoke a Click event in the menu
    mnuText_Click
End Sub

Private Sub cmdWordWrap_Click()
    ' invoke a Click event in the menu
    mnuWordWrap_Click
End Sub





Private Sub Form_Load()
    frmMain.Height = 3600
    frmMain.Width = 4965
End Sub

Private Sub mnuButtons_Click()
    ' display the form
    frmButton.Show
End Sub

Private Sub mnuCheck_Click()
    ' display the form
    frmCheck.Show
End Sub







Private Sub mnuFileExit_Click()
    ' invoke a Click event in the command button
    cmdExit_Click
End Sub

Private Sub mnuImages_Click()
    ' display the form
    frmImages.Show
End Sub

Private Sub mnuOption_Click()
    ' display the form
    frmOptions.Show
End Sub



Private Sub mnuText_Click()
    ' display the form
    frmText.Show
End Sub

Private Sub mnuWordWrap_Click()
    ' display the form
    frmWordWrap.Show
End Sub

