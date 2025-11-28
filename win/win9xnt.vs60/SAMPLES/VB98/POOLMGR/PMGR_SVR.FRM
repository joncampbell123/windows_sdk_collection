VERSION 5.00
Begin VB.Form frmPoolMngr 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Pool Manager Server"
   ClientHeight    =   2820
   ClientLeft      =   3450
   ClientTop       =   4215
   ClientWidth     =   5340
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2820
   ScaleWidth      =   5340
   Begin VB.CommandButton cmdDelete 
      Caption         =   "&Delete"
      Height          =   315
      Left            =   4335
      TabIndex        =   13
      Top             =   1305
      Width           =   900
   End
   Begin VB.CommandButton cmdUpdate 
      Caption         =   "&Update"
      Height          =   315
      Left            =   4335
      TabIndex        =   12
      Top             =   900
      Width           =   900
   End
   Begin VB.CommandButton cmdAdd 
      Caption         =   "&Add"
      Height          =   315
      Left            =   4335
      TabIndex        =   11
      Top             =   495
      Width           =   900
   End
   Begin VB.Frame fraSettings 
      Caption         =   "Server Pool Settings"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   1845
      Left            =   120
      TabIndex        =   3
      Top             =   870
      Width           =   3870
      Begin VB.TextBox txtCloseDelay 
         Alignment       =   1  'Right Justify
         Height          =   285
         Left            =   3180
         TabIndex        =   10
         Text            =   "0"
         Top             =   960
         Width           =   495
      End
      Begin VB.TextBox txtMaxPool 
         Alignment       =   1  'Right Justify
         Height          =   285
         Left            =   3180
         TabIndex        =   9
         Text            =   "0"
         Top             =   630
         Width           =   495
      End
      Begin VB.TextBox txtMinPool 
         Alignment       =   1  'Right Justify
         Height          =   285
         Left            =   3180
         TabIndex        =   8
         Text            =   "0"
         Top             =   300
         Width           =   495
      End
      Begin VB.CheckBox chkCreateAhead 
         Caption         =   "Use C&reate Ahead"
         Height          =   255
         Left            =   180
         TabIndex        =   4
         Top             =   1440
         Width           =   2055
      End
      Begin VB.Label Label1 
         Caption         =   "&Close Delay (seconds)"
         Height          =   240
         Index           =   3
         Left            =   180
         TabIndex        =   7
         Top             =   1035
         Width           =   2130
      End
      Begin VB.Label Label1 
         Caption         =   "Mi&nimum Pool Size"
         Height          =   240
         Index           =   1
         Left            =   180
         TabIndex        =   6
         Top             =   375
         Width           =   1785
      End
      Begin VB.Label Label1 
         Caption         =   "Ma&ximum Pool Size"
         Height          =   240
         Index           =   2
         Left            =   180
         TabIndex        =   5
         Top             =   705
         Width           =   1965
      End
   End
   Begin VB.CommandButton cmdClose 
      Caption         =   "Close"
      Height          =   315
      Left            =   4335
      TabIndex        =   2
      Top             =   90
      Width           =   900
   End
   Begin VB.ComboBox cboServers 
      Height          =   315
      Left            =   120
      TabIndex        =   0
      Text            =   "cboServers"
      Top             =   390
      Width           =   3900
   End
   Begin VB.Label Label1 
      Caption         =   "Managed &Servers"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Index           =   0
      Left            =   120
      TabIndex        =   1
      Top             =   135
      Width           =   2100
   End
End
Attribute VB_Name = "frmPoolMngr"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Private Sub cboServers_Click()
  Dim nPrjNode As Integer
      
  If cboServers.ListIndex < 0 Then Exit Sub
  nPrjNode = cboServers.ItemData(cboServers.ListIndex)
  If nPrjNode < 0 Then Exit Sub
  
  txtMinPool.Text = Format$(gaPrjInfo(nPrjNode).intMinUseCount)
  txtMaxPool.Text = Format$(gaPrjInfo(nPrjNode).intMaxUseCount)
  txtCloseDelay.Text = Format$(gaPrjInfo(nPrjNode).intCloseDelay)
  If gaPrjInfo(nPrjNode).bLookAheadCreate Then
    chkCreateAhead.Value = vbChecked
  Else
    chkCreateAhead.Value = vbUnchecked
  End If
End Sub

Private Sub cmdClose_Click()
    Unload frmPoolMngr
End Sub

Private Sub Form_Load()
  Dim nPrjNode As Integer
  
  InitProjectArray

  nPrjNode = gnFirstPrjNode
  While nPrjNode <> gnNIL
    cboServers.AddItem gaPrjInfo(nPrjNode).strProgID
    cboServers.ItemData(cboServers.NewIndex) = nPrjNode
    nPrjNode = gaPrjInfo(nPrjNode).Next
  Wend
  If cboServers.ListCount > 0 Then cboServers.ListIndex = 0

#If gbDEBUG Then       'Debug code... use for testing without external client
  Dim bSuccess  As Integer
  Dim nProjID  As Integer
  Dim objPoolMngr As Object
  Dim objInterface  As Object
  Dim objServer As Object
  Dim objSimple  As Object

  Set objPoolMngr = CreateObject("PoolMngrProj.PoolMngrClass")
  Exit Sub
  Set objInterface = objPoolMngr.objGetProjInstance("InterfaceProj.ServerInterface", nProjID)
  Set objServer = objInterface.objGetClassInstance("InterfaceDateClass")
  Debug.Print objServer.GetDate
  Set objServer = Nothing
  Set objServer = objInterface.objGetClassInstance("InterfaceTimeClass")
  Debug.Print objServer.GetTime
  Set objServer = Nothing
  bSuccess = objPoolMngr.ReturnProjInstance("InterfaceProj.ServerInterface", nProjID)

  Set objSimple = objPoolMngr.objGetProjInstance("SimpleProj.SimpleTimeClass", nProjID)
  Debug.Print objSimple.GetTime
  bSuccess = objPoolMngr.ReturnProjInstance("SimpleProj.SimpleTimeClass", nProjID)

  Set objPoolMngr = Nothing
#End If
End Sub

Private Sub Form_Unload(Cancel As Integer)
  If MsgBox("Warning: Closing the Pool Manager will terminate all open servers it is managing", vbOKCancel + vbCritical) = vbOK Then
    CloseOpenServers
  Else
    Cancel = True
  End If
End Sub

