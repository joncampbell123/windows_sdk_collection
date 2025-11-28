VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "ComDlg32.OCX"
Begin VB.Form frmBookSales 
   Caption         =   "Book Sales Server"
   ClientHeight    =   2235
   ClientLeft      =   840
   ClientTop       =   7170
   ClientWidth     =   5055
   ClipControls    =   0   'False
   Icon            =   "Book_svr.frx":0000
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2235
   ScaleWidth      =   5055
   WhatsThisHelp   =   -1  'True
   Begin MSComDlg.CommonDialog dlgFindDB 
      Left            =   4200
      Top             =   75
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
      FontSize        =   2.54052e-29
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Instance Count:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   195
      Left            =   225
      TabIndex        =   5
      Top             =   165
      Width           =   1365
   End
   Begin VB.Label lblInstanceCount 
      BorderStyle     =   1  'Fixed Single
      Height          =   285
      Left            =   1665
      TabIndex        =   4
      Top             =   120
      Width           =   750
   End
   Begin VB.Label lblStatus 
      BorderStyle     =   1  'Fixed Single
      Height          =   765
      Index           =   1
      Left            =   1665
      TabIndex        =   3
      Top             =   1380
      Width           =   3255
      WordWrap        =   -1  'True
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Local Response:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   195
      Index           =   1
      Left            =   165
      TabIndex        =   2
      Top             =   1425
      Width           =   1440
   End
   Begin VB.Label lblStatus 
      BorderStyle     =   1  'Fixed Single
      Height          =   765
      Index           =   0
      Left            =   1665
      TabIndex        =   1
      Top             =   510
      Width           =   3255
      WordWrap        =   -1  'True
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Remote Request:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   195
      Index           =   0
      Left            =   120
      TabIndex        =   0
      Top             =   525
      Width           =   1485
   End
End
Attribute VB_Name = "frmBookSales"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

