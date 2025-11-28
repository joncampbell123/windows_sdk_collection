VERSION 5.00
Begin VB.Form frmWordWrap 
   Caption         =   "WordWrap and Autosize"
   ClientHeight    =   4320
   ClientLeft      =   1110
   ClientTop       =   1665
   ClientWidth     =   5760
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Height          =   4725
   Left            =   1050
   LinkTopic       =   "Form1"
   ScaleHeight     =   4320
   ScaleWidth      =   5760
   Top             =   1320
   Width           =   5880
   Begin VB.CommandButton cmdCycle 
      Caption         =   "C&ycle"
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
      Top             =   2520
      Width           =   1215
   End
   Begin VB.CheckBox chkAutoSize 
      Caption         =   "&AutoSize"
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
      Left            =   3840
      TabIndex        =   0
      Top             =   1080
      Width           =   1215
   End
   Begin VB.CheckBox chkWordWrap 
      Caption         =   "&WordWrap"
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
      Left            =   3840
      TabIndex        =   1
      Top             =   1680
      Width           =   1575
   End
   Begin VB.CommandButton cmdClose 
      Cancel          =   -1  'True
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
      Left            =   3840
      TabIndex        =   3
      Top             =   2520
      Width           =   1215
   End
   Begin VB.Label lblHelp 
      BackColor       =   &H00C0C0C0&
      Caption         =   "Check the AutoSize or WordWrap buttons to see their effect on the label."
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
      Left            =   480
      TabIndex        =   5
      Top             =   3360
      Width           =   2775
   End
   Begin VB.Label lblDisplay 
      BorderStyle     =   1  'Fixed Single
      Caption         =   "A demonstration of the AutoSize and WordWrap properties."
      Height          =   255
      Left            =   480
      TabIndex        =   4
      Top             =   480
      Width           =   1815
   End
End
Attribute VB_Name = "frmWordWrap"
Attribute VB_Base = "0{1D936791-C9EF-11CF-84BA-00AA00C007F0}"
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Attribute VB_Customizable = False

Sub Display()
    ' this subroutine changes the display
    ' characteristics of the label control
    
    ' Reset the example.
    lblDisplay.AutoSize = False
    lblDisplay.WordWrap = False
    lblDisplay.Width = 1695
    lblDisplay.Height = 225
    ' Check for WordWrap and Autosize.
    If chkWordWrap.Value = 1 Then
        lblDisplay.WordWrap = True
    End If
    If chkAutoSize.Value = 1 Then
        lblDisplay.AutoSize = True
    End If
End Sub

Private Sub chkAutoSize_Click()
    ' call the Display subroutine
    Call Display
End Sub


Private Sub chkWordWrap_Click()
    ' call the Display subroutine
    Call Display
End Sub


Private Sub cmdClose_Click()
   Unload Me    ' Unload this form.
End Sub

Private Sub cmdCycle_Click()
    ' cycle through the four possible combinations
    
    ' neither check box is selected
    If chkAutoSize.Value = 0 And _
     chkWordWrap.Value = 0 Then
        ' select the AutoSize check box
        chkAutoSize.Value = 1
    ' both check boxes are selected
    ElseIf chkAutoSize.Value = 1 And _
     chkWordWrap.Value = 1 Then
        ' deselect the AutoSize check box and
        ' select the WordWrap check box
        chkAutoSize.Value = 0
        chkWordWrap.Value = 1
    ' only the WordWrap check box is selected
    ElseIf chkAutoSize.Value = 0 And _
     chkWordWrap.Value = 1 Then
        ' deselect both check boxes
        chkAutoSize.Value = 0
        chkWordWrap.Value = 0
    ' only the AutoSize check box is selected
    Else
        ' select the WordWrap check box - the
        ' AutoSize check box is already selected
        chkWordWrap.Value = 1
    End If
End Sub


