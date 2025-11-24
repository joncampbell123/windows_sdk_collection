VERSION 2.00
Begin Form WordListFrm 
   BorderStyle     =   3  'Fixed Double
   Caption         =   "BEdit Word List Sample"
   ClientHeight    =   5910
   ClientLeft      =   1755
   ClientTop       =   570
   ClientWidth     =   6135
   Height          =   6315
   Left            =   1695
   LinkTopic       =   "Form1"
   ScaleHeight     =   5910
   ScaleWidth      =   6135
   Top             =   225
   Width           =   6255
   Begin CommandButton Command4 
      Caption         =   "Return to Main Menu"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   495
      Left            =   240
      TabIndex        =   0
      Top             =   5280
      Width           =   5655
   End
   Begin VBedit BEdit2 
      CellHeight      =   480
      CellWidth       =   360
      CombBaseLine    =   390
      CombEndHeight   =   120
      CombHeight      =   60
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "Times New Roman"
      FontSize        =   15.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   1215
      InflateBottom   =   0
      InflateLeft     =   0
      InflateRight    =   0
      InflateTop      =   0
      Left            =   120
      ScrollBars      =   2  'Vertical
      TabIndex        =   1
      Text            =   "Ink here using the word list."
      Top             =   3840
      Width           =   5895
   End
   Begin Frame Frame3 
      Caption         =   "WordListStr"
      Height          =   1215
      Left            =   120
      TabIndex        =   11
      Top             =   2400
      Width           =   3375
      Begin VBedit BEdit1 
         BorderStyle     =   0  'None
         CellHeight      =   480
         CellWidth       =   360
         CombBaseLine    =   330
         CombEndHeight   =   120
         CombHeight      =   300
         CombStyle       =   1  'Box Style
         Height          =   375
         InflateBottom   =   0
         InflateLeft     =   0
         InflateRight    =   0
         InflateTop      =   0
         Left            =   120
         TabIndex        =   2
         Text            =   "BEdit1"
         Top             =   240
         Width           =   3135
      End
      Begin CommandButton Command1 
         Caption         =   "Add Word"
         Height          =   375
         Left            =   600
         TabIndex        =   3
         Top             =   720
         Width           =   2175
      End
   End
   Begin Frame Frame2 
      Caption         =   "WordListCoercion"
      Height          =   1455
      Left            =   4200
      TabIndex        =   10
      Top             =   960
      Width           =   1815
      Begin OptionButton Option3 
         Caption         =   "Force"
         Height          =   255
         Left            =   360
         TabIndex        =   8
         Top             =   960
         Width           =   1215
      End
      Begin OptionButton Option2 
         Caption         =   "Advise"
         Height          =   375
         Left            =   360
         TabIndex        =   16
         Top             =   600
         Value           =   -1  'True
         Width           =   1215
      End
      Begin OptionButton Option1 
         Caption         =   "None"
         Height          =   255
         Left            =   360
         TabIndex        =   15
         Top             =   360
         Width           =   1095
      End
   End
   Begin Frame Frame1 
      Caption         =   "WordListFile"
      Height          =   1455
      Left            =   120
      TabIndex        =   9
      Top             =   960
      Width           =   3975
      Begin VHedit HEdit1 
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   12
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   375
         InflateBottom   =   0
         InflateLeft     =   0
         InflateRight    =   0
         InflateTop      =   0
         Left            =   120
         TabIndex        =   14
         Text            =   "wordlist.lst"
         Top             =   360
         Width           =   1815
      End
      Begin CommandButton Command2 
         Caption         =   "Read Word List"
         Height          =   495
         Left            =   2040
         TabIndex        =   13
         Top             =   240
         Width           =   1815
      End
      Begin CommandButton Command3 
         Caption         =   "Write Word List"
         Height          =   495
         Left            =   2040
         TabIndex        =   12
         Top             =   840
         Width           =   1815
      End
   End
   Begin PictureBox Picture2 
      BackColor       =   &H00C0C0C0&
      Height          =   1095
      Left            =   3600
      ScaleHeight     =   1065
      ScaleWidth      =   2385
      TabIndex        =   5
      Top             =   2520
      Width           =   2415
      Begin Label Label6 
         Alignment       =   2  'Center
         BackStyle       =   0  'Transparent
         Caption         =   "To add words to the word list, either edit the file directly or use the WordListStr property."
         Height          =   975
         Left            =   120
         TabIndex        =   7
         Top             =   120
         Width           =   2175
      End
   End
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   735
      Left            =   120
      ScaleHeight     =   705
      ScaleWidth      =   5865
      TabIndex        =   4
      Top             =   120
      Width           =   5895
      Begin Label Label5 
         Alignment       =   2  'Center
         BackStyle       =   0  'Transparent
         Caption         =   "To associate a word list with a BEdit control, use the WordListFile and WordListCoercion properties."
         Height          =   495
         Left            =   240
         TabIndex        =   6
         Top             =   120
         Width           =   5295
      End
   End
End

Sub Command1_Click ()
    BEdit2.WordListStr = BEdit1.Text
    BEdit2.AddWordList = True
End Sub

Sub Command2_Click ()
    On Error GoTo ReadError
    
    BEdit2.WordListFile = HEdit1.Text
    BEdit2.ReadWordList = True
    Exit Sub

ReadError:
    MsgBox "Cannot read the word list from the specified file."
    Resume Next
End Sub

Sub Command3_Click ()
    On Error GoTo ErrorHandler
    
    BEdit2.WordListFile = HEdit1.Text
    BEdit2.WriteWordList = True
    Exit Sub

ErrorHandler:
    MsgBox "Cannot write the word list to the specified file."
    Resume Next
End Sub

Sub Command4_Click ()
    MainFrm.Show
    WordListFrm.Hide
End Sub

Sub Option1_Click ()
   BEdit2.WordListCoercion = 0
End Sub

Sub Option2_Click ()
    BEdit2.WordListCoercion = 1
End Sub

Sub Option3_Click ()
    BEdit2.WordListCoercion = 2
End Sub

