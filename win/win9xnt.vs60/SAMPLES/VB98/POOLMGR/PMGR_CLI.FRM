VERSION 5.00
Begin VB.Form frmClient 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Pool Manager Client"
   ClientHeight    =   2520
   ClientLeft      =   3225
   ClientTop       =   2715
   ClientWidth     =   3825
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2520
   ScaleWidth      =   3825
   Begin VB.Frame Frame2 
      Caption         =   "Hello World via PassThru Server"
      Height          =   750
      Left            =   135
      TabIndex        =   7
      Top             =   105
      Width           =   3540
      Begin VB.CommandButton cmdUpdate 
         Caption         =   "Say Hello"
         Height          =   300
         Index           =   0
         Left            =   2280
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
         Index           =   1
         Left            =   2280
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
Dim mobjPoolMngr As Object
Const miPASS_THRU_HELLO = 0
Const miINTERFACE_SVR = 1


Private Sub cmdClose_Click()
  Unload frmClient
End Sub

Private Sub cmdUpdate_Click(Index As Integer)
  'Both of these options use the Pool Manager to acquire reference ptrs to pre-created OLE servers.
  'This greatly reduces the startup time and allows the Pool Manager to control who can have what
  'objects and where they might come from.  (Although this example does not show it (yet) the Pool
  'Manager could be easily extended to manage a pool of objects that spanned multiple network
  'servers..
  
  Dim bSuccess  As Integer
  Dim nProjID  As Integer
  Dim objInterface  As Object
  Dim bInterfaceOpen As Integer
  Dim objServer As Object

  If Index = miPASS_THRU_HELLO Then
    'This option uses the PassThru server to request a references to the external Hello World server.
    'On a single machine, this is not very interesting... but if the PassThru server is registered for
    'remote execution on another machine through Remote Automation, then this method provides a
    'simple way for clients to access InProcess servers that also reside on remote machines.
    'InProcess servers can be much faster and smaller than Out-of-Process servers.  They also share
    'the process space of the PassThru server, which can greatly reduce processing overhead on the
    'remote machine when many OLE servers cooperate to solve a specific client request.
    
    Set objInterface = mobjPoolMngr.objGetProjInstance("PassThruProj.PassThruClass", nProjID)
    bInterfaceOpen = True
    
    Set objServer = objInterface.RunServer("HelloProj.HelloClass", bSuccess)
    If bSuccess Then MsgBox objServer.SayHello
    Set objServer = Nothing
    bSuccess = mobjPoolMngr.ReturnProjInstance("PassThruProj.PassThruClass", nProjID)
  Else
    'This option uses the Interface server to request a reference to the internal Data and Time objects.
    'See the intrface.txt file in the intrface sample for more details about the use of the Intrface server.
    
    Set objInterface = mobjPoolMngr.objGetProjInstance("InterfaceProj.ServerInterface", nProjID)
    If nProjID < 0 Then GoTo cuError2
    bInterfaceOpen = True
    
    Set objServer = objInterface.objGetClassInstance("InterfaceDateClass")
    labDate.Caption = objServer.GetDate
    Set objServer = Nothing
    Set objServer = objInterface.objGetClassInstance("InterfaceTimeClass")
    labTime.Caption = objServer.GetTime
    Set objServer = Nothing
    bSuccess = mobjPoolMngr.ReturnProjInstance("InterfaceProj.ServerInterface", nProjID)
  End If
  GoTo cuExit

cuError:
  MsgBox Error$
  Resume cuExit

cuError2:
  MsgBox "Unable to connect to Server"

cuExit:
  If bInterfaceOpen Then Set objInterface = Nothing
End Sub

Private Sub Form_Load()
'Under a real scenario the Pool Manager would always be running... since it can take a relatively
'long time to start up if it must initialize many large pools.

  Set mobjPoolMngr = CreateObject("PoolMngrProj.PoolMngrClass")
  cmdUpdate_Click miINTERFACE_SVR
End Sub

Private Sub Form_Unload(Cancel As Integer)
  Set mobjPoolMngr = Nothing
End Sub

