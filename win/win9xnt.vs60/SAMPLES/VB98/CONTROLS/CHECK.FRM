VERSION 5.00
Begin VB.Form frmCheck 
   Caption         =   "Check Box Example"
   ClientHeight    =   3075
   ClientLeft      =   2145
   ClientTop       =   1980
   ClientWidth     =   4530
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Height          =   3480
   Left            =   2085
   LinkTopic       =   "Form4"
   ScaleHeight     =   3075
   ScaleWidth      =   4530
   Top             =   1635
   Width           =   4650
   Begin VB.CommandButton cmdClose 
      Caption         =   "&Close"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   3000
      TabIndex        =   3
      Top             =   2160
      Width           =   1095
   End
   Begin VB.CheckBox chkItalic 
      Caption         =   "&Italic"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   480
      TabIndex        =   2
      Top             =   1440
      Width           =   1215
   End
   Begin VB.CheckBox chkBold 
      Caption         =   "&Bold"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   480
      TabIndex        =   1
      Top             =   960
      Width           =   1215
   End
   Begin VB.TextBox txtDisplay 
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
      Text            =   "Some sample text"
      Top             =   360
      Width           =   3615
   End
   Begin VB.Label lblEnter 
      Caption         =   "Select the Bold or Italic check boxes to see their effect on the above text."
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   615
      Left            =   1800
      TabIndex        =   4
      Top             =   1080
      Width           =   2295
   End
End
Attribute VB_Name = "frmCheck"
Attribute VB_Base = "0{1D93679D-C9EF-11CF-84BA-00AA00C007F0}"
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Attribute VB_Customizable = False

Private Sub chkBold_Click()
' The Click event occurs when the check box changes state.
' Value property indicates the new state of the check box.
    If chkBold.Value = 1 Then     ' If checked.
        txtDisplay.FontBold = True
    Else                          ' If not checked.
        txtDisplay.FontBold = False
    End If
End Sub

Private Sub chkItalic_Click()
' The Click event occurs when the check box changes state.
' Value property indicates the new state of the check box.
    If chkItalic.Value = 1 Then     ' If checked.
        txtDisplay.FontItalic = True
    Else                          ' If not checked.
        txtDisplay.FontItalic = False
    End If
End Sub

Private Sub cmdClose_Click()
   Unload Me    ' Unload this form.
End Sub

