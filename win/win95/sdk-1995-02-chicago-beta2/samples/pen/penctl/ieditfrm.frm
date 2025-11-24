VERSION 2.00
Begin Form IEditFrm 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "IEdit Control"
   ClientHeight    =   5865
   ClientLeft      =   990
   ClientTop       =   630
   ClientWidth     =   7740
   ControlBox      =   0   'False
   Height          =   6270
   Left            =   930
   LinkTopic       =   "Form1"
   ScaleHeight     =   391
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   516
   Top             =   285
   Width           =   7860
   Begin VIedit IEdit1 
      AutoSize        =   3  'Tile Picture
      EraserColor     =   &H00FFFFFF&
      EraserWidth     =   5
      Height          =   2535
      Left            =   240
      TabIndex        =   13
      Top             =   1440
      Width           =   3015
   End
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   855
      Index           =   1
      Left            =   240
      ScaleHeight     =   825
      ScaleWidth      =   7185
      TabIndex        =   10
      Top             =   4200
      Width           =   7215
      Begin Label Label1 
         Alignment       =   2  'Center
         BackColor       =   &H00C0C0C0&
         Caption         =   "Change the AutoSize property to change how the background bitmap is displayed.  Load a new bitmap or icon in the Picture property to create graph paper or lined paper."
         ForeColor       =   &H00000000&
         Height          =   615
         Index           =   1
         Left            =   720
         TabIndex        =   11
         Top             =   120
         Width           =   5655
         WordWrap        =   -1  'True
      End
   End
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   1095
      Index           =   0
      Left            =   240
      ScaleHeight     =   1065
      ScaleWidth      =   7185
      TabIndex        =   8
      Top             =   120
      Width           =   7215
      Begin Label Label1 
         Alignment       =   2  'Center
         BackColor       =   &H00C0C0C0&
         Caption         =   "The Pen IEdit control is like a Picture Box control that allows you to draw and erase on a bitmap background.  To erase ink, press the barrel button on your pen to invoke the context menu."
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   795
         Index           =   0
         Left            =   300
         TabIndex        =   9
         Top             =   120
         Width           =   6615
         WordWrap        =   -1  'True
      End
   End
   Begin CommandButton Command2 
      Caption         =   "Return to Main Menu"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   495
      Left            =   180
      TabIndex        =   7
      Top             =   5220
      Width           =   7335
   End
   Begin CommandButton EraseButton 
      Caption         =   "Clear Ink"
      Height          =   435
      Left            =   3480
      TabIndex        =   6
      Top             =   3600
      Width           =   1875
   End
   Begin SSFrame Frame3D1 
      Caption         =   "AutoSize"
      Font3D          =   0  'None
      ForeColor       =   &H00000000&
      Height          =   2115
      Left            =   3480
      TabIndex        =   1
      Top             =   1380
      Width           =   3975
      Begin SSOption Option3D5 
         Caption         =   "Center Picture"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   240
         Left            =   480
         TabIndex        =   12
         TabStop         =   0   'False
         Top             =   1680
         Width           =   2895
      End
      Begin SSOption Option3D4 
         Caption         =   "Tile Picture"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   240
         Left            =   480
         TabIndex        =   5
         Top             =   1320
         Value           =   -1  'True
         Width           =   2895
      End
      Begin SSOption Option3D3 
         Caption         =   "Size Window to fit Picture"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   240
         Left            =   480
         TabIndex        =   4
         TabStop         =   0   'False
         Top             =   960
         Width           =   2895
      End
      Begin SSOption Option3D2 
         Caption         =   "Stretch Picture to fit Window"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   240
         Left            =   480
         TabIndex        =   3
         TabStop         =   0   'False
         Top             =   600
         Width           =   2895
      End
      Begin SSOption Option3D1 
         Caption         =   "None"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   240
         Left            =   480
         TabIndex        =   2
         TabStop         =   0   'False
         Top             =   240
         Width           =   2895
      End
   End
   Begin CommandButton ChangePicture 
      Caption         =   "Change Picture..."
      Height          =   435
      Left            =   5580
      TabIndex        =   0
      Top             =   3600
      Width           =   1875
   End
   Begin CommonDialog CMDialog1 
      Filename        =   "*.bmp"
      Filter          =   "Bitmaps (*.bmp)|*.bmp|Icons (*.ico)|*.ico"
      Left            =   -240
      Top             =   -240
   End
End
Dim HeightSave As Single
Dim WidthSave As Single

Sub ChangePicture_Click ()
    On Error GoTo ErrTrap
    CMDialog1.Action = 1
    IEdit1.Picture = LoadPicture(CMDialog1.Filename)
    Exit Sub

ErrTrap:
    MsgBox "Err =" + Str$(Err) + ": " + Error$, , "Pen Sample"
    Resume Next
End Sub

Sub Command2_Click ()
    MainFrm.Show
    IEditFrm.Hide
End Sub

Sub EraseButton_Click ()
    IEdit1.EraseInk = True
End Sub

Sub Form_Load ()
    HeightSave = IEdit1.Height
    WidthSave = IEdit1.Width
End Sub

Sub Option3D1_Click (Value As Integer)
    IEdit1.AutoSize = PEN_AUTOSIZE_NONE
    IEdit1.Height = HeightSave
    IEdit1.Width = WidthSave
End Sub

Sub Option3D2_Click (Value As Integer)
    IEdit1.AutoSize = PEN_AUTOSIZE_STRETCHBITMAP
    IEdit1.Height = HeightSave
    IEdit1.Width = WidthSave
End Sub

Sub Option3D3_Click (Value As Integer)
    IEdit1.AutoSize = PEN_AUTOSIZE_SIZEWINDOW
End Sub

Sub Option3D4_Click (Value As Integer)
    IEdit1.AutoSize = PEN_AUTOSIZE_TILE
    IEdit1.Height = HeightSave
    IEdit1.Width = WidthSave
End Sub

Sub Option3D5_Click (Value As Integer)
    IEdit1.AutoSize = PEN_AUTOSIZE_CENTER
    IEdit1.Height = HeightSave
    IEdit1.Width = WidthSave
End Sub

