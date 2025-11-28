VERSION 5.00
Begin VB.Form fromAdminTitleScreen 
   Caption         =   "Insurance Workflow Administrator"
   ClientHeight    =   1845
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4155
   LinkTopic       =   "Form1"
   ScaleHeight     =   1845
   ScaleWidth      =   4155
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdViewExistingQueues 
      Caption         =   "&View existing queues"
      Height          =   495
      Left            =   960
      TabIndex        =   1
      Top             =   840
      Width           =   2055
   End
   Begin VB.CommandButton cmdCreateNewQueues 
      Caption         =   "&Create new queues"
      Height          =   495
      Left            =   960
      TabIndex        =   0
      Top             =   240
      Width           =   2055
   End
End
Attribute VB_Name = "fromAdminTitleScreen"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private Sub cmdCreateNewQueues_Click()
    frmCreateQueues.Show
End Sub

Private Sub cmdViewExistingQueues_Click()
    frmQueueMonitoring.Show
End Sub

