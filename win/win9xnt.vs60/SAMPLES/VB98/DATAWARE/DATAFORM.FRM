VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Data-aware Classes"
   ClientHeight    =   1860
   ClientLeft      =   1665
   ClientTop       =   1545
   ClientWidth     =   2910
   LinkTopic       =   "Form1"
   ScaleHeight     =   1860
   ScaleWidth      =   2910
   Begin VB.CommandButton cmdCycle 
      Caption         =   "Cycle"
      Height          =   375
      Left            =   840
      TabIndex        =   1
      Top             =   1200
      Width           =   1095
   End
   Begin VB.TextBox txtConsumer 
      Height          =   375
      Left            =   480
      TabIndex        =   0
      Top             =   360
      Width           =   1815
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private objSource As MySource
Private objBindingCollection As BindingCollection
Private objConsumer As MyConsumer

Private Sub cmdCycle_Click()
    ' Call the Cycle method of the data source.
    objSource.Cycle
End Sub


Private Sub Form_Load()
    Set objSource = New MySource
    Set objBindingCollection = New BindingCollection
    Set objConsumer = New MyConsumer
    
    ' Assign the source class to the Binding
    ' Collection's DataSource property.
    Set objBindingCollection.DataSource = objSource
    ' Add a binding.
    objBindingCollection.Add txtConsumer, "Text", "Directory"
    objBindingCollection.Add objConsumer, "DirName", "Directory"
End Sub
