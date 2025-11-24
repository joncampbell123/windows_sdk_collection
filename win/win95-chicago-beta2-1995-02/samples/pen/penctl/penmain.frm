VERSION 2.00
Begin Form MainFrm 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Sample Pen Program"
   ClientHeight    =   5400
   ClientLeft      =   1665
   ClientTop       =   750
   ClientWidth     =   6255
   Height          =   5805
   Icon            =   PENMAIN.FRX:0000
   Left            =   1605
   LinkMode        =   1  'Source
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   5400
   ScaleWidth      =   6255
   Top             =   405
   Width           =   6375
   Begin SSPanel Panel3D2 
      Alignment       =   6  'Center - TOP
      BackColor       =   &H00C0C0C0&
      BevelInner      =   1  'Inset
      BevelWidth      =   2
      BorderWidth     =   2
      Caption         =   "Press Button to Activate Demonstration"
      Font3D          =   1  'Raised w/light shading
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   12
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      ForeColor       =   &H00000000&
      Height          =   4335
      Left            =   0
      RoundedCorners  =   0   'False
      TabIndex        =   7
      Top             =   1080
      Width           =   6255
      Begin SSCommand SelectDemo 
         BevelWidth      =   4
         Caption         =   "BEdit Word List Sample"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   4
         Left            =   600
         RoundedCorners  =   0   'False
         TabIndex        =   8
         Top             =   3000
         Width           =   5055
      End
      Begin SSCommand SelectDemo 
         BevelWidth      =   4
         Caption         =   "Transfer Ink Data"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   3
         Left            =   600
         RoundedCorners  =   0   'False
         TabIndex        =   3
         Top             =   2400
         Width           =   5055
      End
      Begin SSCommand SelectDemo 
         BevelWidth      =   4
         Caption         =   "Ink Edit Control"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   2
         Left            =   600
         RoundedCorners  =   0   'False
         TabIndex        =   2
         Top             =   1800
         Width           =   5055
      End
      Begin SSCommand SelectDemo 
         BevelWidth      =   4
         Caption         =   "Exit Pen Sample Programs"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   5
         Left            =   360
         RoundedCorners  =   0   'False
         TabIndex        =   4
         Top             =   3600
         Width           =   5535
      End
      Begin SSCommand SelectDemo 
         BevelWidth      =   4
         Caption         =   "Save and Restore Ink"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   1
         Left            =   600
         RoundedCorners  =   0   'False
         TabIndex        =   1
         Top             =   1200
         Width           =   5055
      End
      Begin SSCommand SelectDemo 
         BevelWidth      =   4
         Caption         =   "DelayRecog and OnTap Properties"
         Font3D          =   0  'None
         ForeColor       =   &H00000000&
         Height          =   495
         Index           =   0
         Left            =   600
         RoundedCorners  =   0   'False
         TabIndex        =   0
         Top             =   600
         Width           =   5055
      End
   End
   Begin SSPanel Panel3D1 
      Alignment       =   6  'Center - TOP
      BackColor       =   &H00C0C0C0&
      BevelInner      =   1  'Inset
      BevelWidth      =   2
      BorderWidth     =   2
      Caption         =   "Sample Pen Program"
      Font3D          =   1  'Raised w/light shading
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   18
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      ForeColor       =   &H00000000&
      Height          =   1095
      Left            =   0
      RoundedCorners  =   0   'False
      TabIndex        =   5
      Top             =   0
      Width           =   6255
      Begin Label Label1 
         Alignment       =   2  'Center
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Copyright (c) 1992-95, Microsoft Corp.  All rights reserved."
         ForeColor       =   &H00000000&
         Height          =   225
         Index           =   1
         Left            =   480
         TabIndex        =   6
         Top             =   600
         Width           =   5175
      End
   End
End

Sub SelectDemo_Click (Index As Integer)
    Screen.MousePointer = 11
    Select Case Index
        Case 0
            DelayFrm.Show
        Case 1
            InkFrm.Show
        Case 2
            IEditFrm.Show
        Case 3
            TransFrm.Show
        Case 4
            WordListFrm.Show
        Case 5
            End
    End Select

    MainFrm.Hide
    Screen.MousePointer = 0
End Sub
