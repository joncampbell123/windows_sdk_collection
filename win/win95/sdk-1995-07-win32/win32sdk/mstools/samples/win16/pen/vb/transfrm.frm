VERSION 2.00
Begin Form TransFrm 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Transfer Ink Data"
   ClientHeight    =   6270
   ClientLeft      =   1035
   ClientTop       =   345
   ClientWidth     =   7665
   ControlBox      =   0   'False
   FontBold        =   0   'False
   FontItalic      =   0   'False
   FontName        =   "MS Sans Serif"
   FontSize        =   8.25
   FontStrikethru  =   0   'False
   FontUnderline   =   0   'False
   Height          =   6675
   Left            =   975
   LinkTopic       =   "Form1"
   ScaleHeight     =   6270
   ScaleWidth      =   7665
   Top             =   0
   Width           =   7785
   Begin VIedit IEdit1 
      EraserColor     =   &H00FFFFFF&
      EraserWidth     =   5
      Height          =   1695
      Left            =   120
      TabIndex        =   18
      Top             =   960
      Width           =   3615
   End
   Begin PictureBox Picture2 
      BackColor       =   &H00C0C0C0&
      Height          =   915
      Left            =   120
      ScaleHeight     =   885
      ScaleWidth      =   7365
      TabIndex        =   16
      Top             =   4740
      Width           =   7395
      Begin Label Label2 
         Alignment       =   2  'Center
         BackStyle       =   0  'Transparent
         Caption         =   "When the InkDataString is assigned, the InkDataMode property is checked to see whether the new ink data replaces or is appended to existing ink.  When ink is transferred to an HEdit, the ink is recognized as if it had been written there."
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   735
         Left            =   120
         TabIndex        =   17
         Top             =   120
         Width           =   7095
      End
   End
   Begin PictureBox Picture1 
      BackColor       =   &H00C0C0C0&
      Height          =   735
      Left            =   120
      ScaleHeight     =   705
      ScaleWidth      =   7365
      TabIndex        =   14
      Top             =   120
      Width           =   7395
      Begin Label Label1 
         Alignment       =   2  'Center
         BackStyle       =   0  'Transparent
         Caption         =   "This demonstration shows how Ink can be easily copied from one control to another by assigning the InkDataString property."
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   555
         Left            =   120
         TabIndex        =   15
         Top             =   120
         Width           =   7155
      End
   End
   Begin CommandButton EndTransferDemo 
      Caption         =   "Return to Main Menu"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   435
      Left            =   120
      TabIndex        =   13
      Top             =   5760
      Width           =   7395
   End
   Begin CommandButton EraseInkButton 
      Caption         =   "Clear Ink"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   555
      Left            =   120
      TabIndex        =   12
      Top             =   3420
      Width           =   3615
   End
   Begin Frame Frame3 
      BackColor       =   &H00FFFFFF&
      Caption         =   "HEdit DelayRecog"
      ForeColor       =   &H00000000&
      Height          =   615
      Left            =   3900
      TabIndex        =   5
      Top             =   3360
      Width           =   3615
      Begin OptionButton OptDelayFalse 
         BackColor       =   &H00FFFFFF&
         Caption         =   "False"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   1860
         TabIndex        =   11
         Top             =   255
         Value           =   -1  'True
         Width           =   1275
      End
      Begin OptionButton OptDelayTrue 
         BackColor       =   &H00FFFFFF&
         Caption         =   "True"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   420
         TabIndex        =   10
         Top             =   240
         Width           =   1335
      End
   End
   Begin Frame Frame2 
      BackColor       =   &H00FFFFFF&
      Caption         =   "HEdit InkDataMode"
      ForeColor       =   &H00000000&
      Height          =   615
      Left            =   3900
      TabIndex        =   4
      Top             =   4020
      Width           =   3615
      Begin OptionButton OptHEditAppend 
         BackColor       =   &H00FFFFFF&
         Caption         =   "Append"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   1860
         TabIndex        =   9
         Top             =   240
         Width           =   1275
      End
      Begin OptionButton OptHEditReplace 
         BackColor       =   &H00FFFFFF&
         Caption         =   "Replace"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   360
         TabIndex        =   8
         Top             =   240
         Value           =   -1  'True
         Width           =   1335
      End
   End
   Begin Frame Frame1 
      BackColor       =   &H00FFFFFF&
      Caption         =   "IEdit InkDataMode"
      ForeColor       =   &H00000000&
      Height          =   615
      Left            =   120
      TabIndex        =   3
      Top             =   4020
      Width           =   3615
      Begin OptionButton OptIEditAppend 
         BackColor       =   &H00FFFFFF&
         Caption         =   "Append"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   1860
         TabIndex        =   7
         Top             =   240
         Width           =   1275
      End
      Begin OptionButton OptIEditReplace 
         BackColor       =   &H00FFFFFF&
         Caption         =   "Replace"
         FontBold        =   -1  'True
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   9.75
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         ForeColor       =   &H00000000&
         Height          =   315
         Left            =   360
         TabIndex        =   6
         Top             =   240
         Value           =   -1  'True
         Width           =   1335
      End
   End
   Begin CommandButton CopyInktoIEdit 
      Caption         =   "Copy InkData to IEdit"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   555
      Left            =   3900
      TabIndex        =   2
      Top             =   2760
      Width           =   3615
   End
   Begin CommandButton CopyInktoHEdit 
      Caption         =   "Copy InkData to HEdit"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   555
      Left            =   120
      TabIndex        =   1
      Top             =   2760
      Width           =   3615
   End
   Begin VHedit HEdit1 
      BackColor       =   &H00FFFFFF&
      CharSet         =   4096
      ForeColor       =   &H00000000&
      Height          =   1695
      InflateBottom   =   270
      InflateLeft     =   270
      InflateRight    =   270
      InflateTop      =   270
      Left            =   3900
      TabIndex        =   0
      Text            =   "This is an HEdit control.  To its left is an IEdit control.  Ink on the controls, and press the 'Copy ...' buttons below to transfer the ink between them."
      Top             =   960
      Width           =   3615
   End
End

Sub CopyInktoHEdit_Click ()
    HEdit1.InkDataString = IEdit1.InkDataString
End Sub

Sub CopyInktoIEdit_Click ()
    IEdit1.InkDataString = HEdit1.InkDataString
End Sub

Sub EndTransferDemo_Click ()
    MainFrm.Show
    TransFrm.Hide
End Sub

Sub EraseInkButton_Click ()
    IEdit1.EraseInk = True
    HEdit1.EraseInk = True
End Sub

Sub OptDelayFalse_Click ()
    HEdit1.DelayRecog = False
End Sub

Sub OptDelayTrue_Click ()
    HEdit1.DelayRecog = True
End Sub

Sub OptHEditAppend_Click ()
    HEdit1.InkDataMode = PEN_INKDATAMODE_APPEND
End Sub

Sub OptHEditReplace_Click ()
    HEdit1.InkDataMode = PEN_INKDATAMODE_REPLACE
End Sub

Sub OptIEditAppend_Click ()
    IEdit1.InkDataMode = PEN_INKDATAMODE_APPEND
End Sub

Sub OptIEditReplace_Click ()
    IEdit1.InkDataMode = PEN_INKDATAMODE_REPLACE
End Sub

