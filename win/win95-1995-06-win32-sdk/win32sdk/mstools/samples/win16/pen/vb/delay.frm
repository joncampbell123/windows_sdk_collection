VERSION 2.00
Begin Form DelayFrm 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Delayed Recognition and OnTap Demonstration"
   ClientHeight    =   6255
   ClientLeft      =   300
   ClientTop       =   390
   ClientWidth     =   9015
   ControlBox      =   0   'False
   FontBold        =   -1  'True
   FontItalic      =   0   'False
   FontName        =   "System"
   FontSize        =   9.75
   FontStrikethru  =   0   'False
   FontUnderline   =   0   'False
   Height          =   6660
   Left            =   240
   LinkMode        =   1  'Source
   LinkTopic       =   "Form1"
   ScaleHeight     =   6255
   ScaleWidth      =   9015
   Top             =   45
   Width           =   9135
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   975
      Index           =   0
      Left            =   120
      ScaleHeight     =   945
      ScaleWidth      =   8745
      TabIndex        =   0
      TabStop         =   0   'False
      Top             =   120
      Width           =   8775
      Begin Label Label1 
         Alignment       =   2  'Center
         BackColor       =   &H00C0C0C0&
         Caption         =   "The BEdit and HEdit controls both support inking and recognition.  Both controls allow for delayed recognition and recognition when the user taps the control."
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   555
         Index           =   0
         Left            =   180
         TabIndex        =   9
         Top             =   60
         Width           =   8355
      End
      Begin Label Label1 
         Alignment       =   2  'Center
         BackColor       =   &H00C0C0C0&
         Caption         =   "With DelayRecog set to FALSE, any handwiting within the control will be recognized."
         ForeColor       =   &H00000000&
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   16
         Top             =   600
         Width           =   8295
      End
   End
   Begin VHedit HEdit1 
      BackColor       =   &H00FFFFFF&
      CharSet         =   4096
      ForeColor       =   &H00000000&
      Height          =   975
      InflateBottom   =   270
      InflateLeft     =   270
      InflateRight    =   270
      InflateTop      =   270
      Left            =   120
      TabIndex        =   1
      Top             =   1200
      Width           =   6855
   End
   Begin CommandButton ClearText 
      Caption         =   "Clear Text"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "System"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   975
      Left            =   7080
      TabIndex        =   2
      Top             =   1200
      Width           =   1815
   End
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   375
      Index           =   2
      Left            =   120
      ScaleHeight     =   345
      ScaleWidth      =   8745
      TabIndex        =   20
      TabStop         =   0   'False
      Top             =   2280
      Width           =   8775
      Begin Label Label1 
         Alignment       =   2  'Center
         BackColor       =   &H00C0C0C0&
         Caption         =   "Set the DelayRecog property to TRUE.  Writing will remain as Ink until DelayRecog is set to FALSE."
         ForeColor       =   &H00000000&
         Height          =   315
         Index           =   8
         Left            =   120
         TabIndex        =   21
         Top             =   60
         Width           =   8535
      End
   End
   Begin PictureBox Picture3 
      BackColor       =   &H00FFFFFF&
      Height          =   615
      Left            =   120
      ScaleHeight     =   585
      ScaleWidth      =   8745
      TabIndex        =   17
      TabStop         =   0   'False
      Top             =   2760
      Width           =   8775
      Begin Frame Frame3 
         BackColor       =   &H00FFFFFF&
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "System"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   555
         Left            =   2280
         TabIndex        =   18
         Top             =   -60
         Width           =   2535
         Begin OptionButton DelayOn 
            BackColor       =   &H00FFFFFF&
            Caption         =   "TRUE"
            FontBold        =   -1  'True
            FontItalic      =   0   'False
            FontName        =   "System"
            FontSize        =   9.75
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            ForeColor       =   &H00000000&
            Height          =   285
            Left            =   240
            TabIndex        =   3
            Top             =   180
            Width           =   1005
         End
         Begin OptionButton DelayOff 
            BackColor       =   &H00FFFFFF&
            Caption         =   "FALSE"
            FontBold        =   -1  'True
            FontItalic      =   0   'False
            FontName        =   "System"
            FontSize        =   9.75
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            ForeColor       =   &H00000000&
            Height          =   285
            Left            =   1320
            TabIndex        =   4
            Top             =   180
            Value           =   -1  'True
            Width           =   975
         End
      End
      Begin CommandButton EraseInk 
         Caption         =   "Erase Ink"
         Enabled         =   0   'False
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "System"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   375
         Left            =   5400
         TabIndex        =   5
         Top             =   120
         Width           =   2895
      End
      Begin Label Label2 
         BackColor       =   &H00FFFFFF&
         Caption         =   "DelayRecog"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Index           =   2
         Left            =   840
         TabIndex        =   19
         Top             =   120
         Width           =   1335
      End
   End
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   1335
      Index           =   1
      Left            =   120
      ScaleHeight     =   1305
      ScaleWidth      =   8745
      TabIndex        =   12
      TabStop         =   0   'False
      Top             =   3480
      Width           =   8775
      Begin Label Label1 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Recognition occurs when the DelayRecog property is changed from FALSE to TRUE."
         ForeColor       =   &H00000000&
         Height          =   255
         Index           =   5
         Left            =   120
         TabIndex        =   13
         Top             =   120
         Width           =   8295
      End
      Begin Label Label1 
         BackColor       =   &H00C0C0C0&
         Caption         =   "When this occurs, the OnTap property is evaluated.  If it is TRUE, the recognition will not occur until the user taps on the edit field.  Try setting OnTap and DelayRecog to TRUE."
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   6
         Left            =   120
         TabIndex        =   14
         Top             =   360
         Width           =   8535
      End
      Begin Label Label1 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Write some text in the control and then set DelayRecog to FALSE.  Recognition will not occur until the control is tapped."
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   14
         Left            =   120
         TabIndex        =   22
         Top             =   840
         Width           =   8415
      End
   End
   Begin PictureBox Picture2 
      BackColor       =   &H00FFFFFF&
      Height          =   615
      Left            =   2280
      ScaleHeight     =   585
      ScaleWidth      =   4305
      TabIndex        =   10
      TabStop         =   0   'False
      Top             =   4920
      Width           =   4335
      Begin Frame Frame4 
         BackColor       =   &H00FFFFFF&
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "System"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   555
         Left            =   1500
         TabIndex        =   11
         Top             =   -60
         Width           =   2610
         Begin OptionButton TapOn 
            BackColor       =   &H00FFFFFF&
            Caption         =   "TRUE"
            FontBold        =   -1  'True
            FontItalic      =   0   'False
            FontName        =   "System"
            FontSize        =   9.75
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            ForeColor       =   &H00000000&
            Height          =   255
            Left            =   360
            TabIndex        =   6
            Top             =   180
            Width           =   885
         End
         Begin OptionButton TapOff 
            BackColor       =   &H00FFFFFF&
            Caption         =   "FALSE"
            FontBold        =   -1  'True
            FontItalic      =   0   'False
            FontName        =   "System"
            FontSize        =   9.75
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            ForeColor       =   &H00000000&
            Height          =   255
            Left            =   1440
            TabIndex        =   7
            Top             =   180
            Value           =   -1  'True
            Width           =   1005
         End
      End
      Begin Label Label2 
         BackColor       =   &H00FFFFFF&
         Caption         =   "OnTap"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Index           =   1
         Left            =   540
         TabIndex        =   15
         Top             =   120
         Width           =   735
      End
   End
   Begin CommandButton Command1 
      Caption         =   "Return to Main Menu"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   495
      Left            =   120
      TabIndex        =   8
      Top             =   5640
      Width           =   8775
   End
End

Sub ClearText_Click ()
    HEdit1.Text = ""
End Sub

Sub Command1_Click ()
    MainFrm.Show
    DelayFrm.Hide
End Sub

Sub DelayOff_Click ()
    HEdit1.TabStop = True
    HEdit1.DelayRecog = False
    EraseInk.Enabled = False
End Sub

Sub DelayOn_Click ()
    HEdit1.DelayRecog = True
    EraseInk.Enabled = True
    HEdit1.TabStop = False
End Sub

Sub EraseInk_Click ()
    HEdit1.EraseInk = True
End Sub

Sub Form_Unload (Cancel As Integer)
    MainFrm.Show
End Sub

Sub TapOff_Click ()
    HEdit1.OnTap = False
End Sub

Sub TapOn_Click ()
    HEdit1.OnTap = True
End Sub
