VERSION 5.00
Begin VB.Form frmText 
   Caption         =   "Text Box Properties"
   ClientHeight    =   4455
   ClientLeft      =   930
   ClientTop       =   1290
   ClientWidth     =   6915
   Height          =   4860
   Left            =   870
   LinkTopic       =   "Form1"
   ScaleHeight     =   4455
   ScaleWidth      =   6915
   Top             =   945
   Width           =   7035
   Begin VB.CommandButton cmdReset 
      Caption         =   "&Reset"
      Height          =   495
      Left            =   2400
      TabIndex        =   2
      Top             =   3720
      Width           =   1215
   End
   Begin VB.CommandButton cmdClose 
      Cancel          =   -1  'True
      Caption         =   "&Close"
      Height          =   495
      Left            =   5280
      TabIndex        =   3
      Top             =   3720
      Width           =   1215
   End
   Begin VB.Frame fraInsert 
      Caption         =   "Set the Insertion Point"
      Height          =   2295
      Left            =   360
      TabIndex        =   5
      Top             =   1200
      Width           =   3255
      Begin VB.OptionButton optText 
         Caption         =   "Insert &Text"
         Height          =   255
         Left            =   360
         TabIndex        =   9
         Top             =   1800
         Width           =   2055
      End
      Begin VB.OptionButton optSelect 
         Caption         =   "&Select All Text"
         Height          =   255
         Left            =   360
         TabIndex        =   8
         Top             =   1440
         Width           =   2055
      End
      Begin VB.OptionButton optInsert 
         Caption         =   "&Insertion Point After 5th Character"
         Height          =   255
         Left            =   360
         TabIndex        =   7
         Top             =   1080
         Width           =   2775
      End
      Begin VB.OptionButton optEnd 
         Caption         =   "Insertion Point at &End"
         Height          =   255
         Left            =   360
         TabIndex        =   6
         Top             =   720
         Width           =   1815
      End
      Begin VB.OptionButton optDefault 
         Caption         =   "&Default Settings"
         Height          =   255
         Left            =   360
         TabIndex        =   1
         Top             =   360
         Value           =   -1  'True
         Width           =   1815
      End
   End
   Begin VB.TextBox txtDisplay 
      Height          =   375
      HideSelection   =   0   'False
      Left            =   360
      TabIndex        =   0
      Text            =   "The MultiLine property is set to False in this example"
      Top             =   480
      Width           =   3255
   End
   Begin VB.TextBox txtMulti 
      Height          =   615
      Left            =   4080
      MultiLine       =   -1  'True
      TabIndex        =   4
      TabStop         =   0   'False
      Text            =   "Text.frx":0000
      Top             =   480
      Width           =   2415
   End
   Begin VB.Label lblHelp 
      Caption         =   "Select an option to see its effect on the first text box."
      Height          =   735
      Left            =   4200
      TabIndex        =   10
      Top             =   2160
      Width           =   2295
   End
End
Attribute VB_Name = "frmText"
Attribute VB_Base = "0{1D9367A3-C9EF-11CF-84BA-00AA00C007F0}"
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Attribute VB_Customizable = False




Private Sub cmdClose_Click()
    Unload Me   ' Unload this form.
End Sub

Private Sub cmdReset_Click()
    ' restore the original text
    txtDisplay.Text = "The MultiLine property is set to False in this example"
    ' reset the option buttons to the default
    optDefault.Value = True
End Sub

Private Sub optDefault_Click()
    ' place the insertion point at the beginning
    txtDisplay.SelStart = 0
    
    ' set the focus to the text box so we can see
    ' the result of our settings
    txtDisplay.SetFocus
End Sub

Private Sub optEnd_Click()
    ' find the length of the string and place
    ' the insertion point at the end
    txtDisplay.SelStart = Len(txtDisplay.Text)
    
    ' set the focus to the text box so we can see
    ' the result of our settings
    txtDisplay.SetFocus
End Sub


Private Sub optInsert_Click()
    ' place the insertion point after 5th char
    txtDisplay.SelStart = 5
        
    ' set the focus to the text box so we can see
    ' the result of our settings
    txtDisplay.SetFocus
End Sub


Private Sub optSelect_Click()
    ' place the insertion point at the beginning
    txtDisplay.SelStart = 0
    ' find the length of the string and
    ' select that number of characters
    txtDisplay.SelLength = Len(txtDisplay.Text)
    
    ' set the focus to the text box so we can see
    ' the result of our settings
    txtDisplay.SetFocus
End Sub


Private Sub optText_Click()
    ' insert "NEW STRING" at the insertion point -
    ' if text is selected, it will be replaced
    txtDisplay.SelText = "NEW TEXT"
    
    ' set the focus to the text box so we can see
    ' the result of our settings
    txtDisplay.SetFocus
End Sub

