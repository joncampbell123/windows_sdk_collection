VERSION 5.00
Begin VB.Form frmClient 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Pass Through Client"
   ClientHeight    =   2520
   ClientLeft      =   3945
   ClientTop       =   3210
   ClientWidth     =   3825
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2520
   ScaleWidth      =   3825
   Begin VB.Frame Frame2 
      Caption         =   "Hello Server"
      Height          =   750
      Left            =   135
      TabIndex        =   7
      Top             =   105
      Width           =   3540
      Begin VB.CommandButton cmdSayHello 
         Caption         =   "Say Hello"
         Height          =   300
         Left            =   2400
         TabIndex        =   8
         Top             =   270
         Width           =   1050
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Interface Server"
      Height          =   990
      Left            =   135
      TabIndex        =   1
      Top             =   960
      Width           =   3540
      Begin VB.CommandButton cmdUpdate 
         Caption         =   "Update"
         Height          =   300
         Left            =   2400
         TabIndex        =   6
         Top             =   270
         Width           =   1050
      End
      Begin VB.Label lab1 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Date:"
         Height          =   270
         Index           =   0
         Left            =   195
         TabIndex        =   5
         Top             =   315
         Width           =   465
      End
      Begin VB.Label lab1 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Time:"
         Height          =   270
         Index           =   1
         Left            =   195
         TabIndex        =   4
         Top             =   660
         Width           =   450
      End
      Begin VB.Label labDate 
         BackColor       =   &H00C0C0C0&
         Caption         =   "labDate"
         Height          =   210
         Left            =   690
         TabIndex        =   3
         Top             =   315
         Width           =   840
      End
      Begin VB.Label labTime 
         BackColor       =   &H00C0C0C0&
         Caption         =   "labTime"
         Height          =   210
         Left            =   690
         TabIndex        =   2
         Top             =   660
         Width           =   840
      End
   End
   Begin VB.CommandButton cmdClose 
      Caption         =   "Close"
      Default         =   -1  'True
      Height          =   300
      Left            =   150
      TabIndex        =   0
      Top             =   2085
      Width           =   1050
   End
End
Attribute VB_Name = "frmClient"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim mobjPassThruSvr As Object
Dim mbPassThruSvrCreated As Integer

Private Sub cmdClose_Click()
  Unload frmClient
End Sub

Private Sub cmdSayHello_Click()
'This routine uses the PassThru server to receive references to an OLE server that it wants to talk to.
'On a single machine, this is not very interesting... but if the PassThru server is registered for
'remote execution on another machine through Remote Automation, then this method provides a
'simple way for clients to access InProcess servers that also reside on the remote machine.
  
  Dim bSuccess  As Integer
  Dim objServer As Object
  On Error GoTo shError
  
  Set objServer = mobjPassThruSvr.RunServer("HelloProj.HelloClass", bSuccess)
  If bSuccess Then MsgBox objServer.SayHello
  Set objServer = Nothing
  GoTo shExit

shError:
  MsgBox Error$
  Resume shExit

shExit:
End Sub

Private Sub cmdUpdate_Click()
'This routine uses the PassThru server to receive references to an OLE server that it wants to talk to.
'On a single machine, this is not very interesting... but if the PassThru server is registered for
'remote execution on another machine through Remote Automation, then this method provides a
'simple way for clients to access InProcess servers that also reside on the remote machine.

  Dim bSuccess  As Integer
  Dim objInterface  As Object
  Dim bInterfaceOpen As Integer
  Dim objServer As Object
  Dim bServerOpen As Integer
  On Error GoTo cuError

  Set objInterface = mobjPassThruSvr.RunServer("InterfaceProj.ServerInterface", bSuccess)
  If Not bSuccess Then GoTo cuError2
  bInterfaceOpen = True
  
  Set objServer = objInterface.objGetClassInstance("InterfaceDateClass")
  bServerOpen = True
  labDate.Caption = objServer.GetDate
  Set objServer = Nothing
  bServerOpen = False
  
  Set objServer = objInterface.objGetClassInstance("InterfaceTimeClass")
  bServerOpen = True
  labTime.Caption = objServer.GetTime
  GoTo cuExit

cuError:
  MsgBox Error$
  Resume cuExit

cuError2:
  MsgBox "Unable to connect to Server"

cuExit:
  If bServerOpen Then Set objServer = Nothing
  If bInterfaceOpen Then Set objInterface = Nothing
End Sub

Private Sub Form_Load()
  Set mobjPassThruSvr = CreateObject("PassThruProj.PassThruClass")
  mbPassThruSvrCreated = True
  cmdUpdate_Click
End Sub

Private Sub Form_Unload(Cancel As Integer)
  If mbPassThruSvrCreated Then Set mobjPassThruSvr = Nothing
End Sub

