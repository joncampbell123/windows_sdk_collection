VERSION 5.00
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#1.1#0"; "comctl32.ocx"
Begin VB.Form frmCBCli 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Callback Clock Client"
   ClientHeight    =   2205
   ClientLeft      =   2880
   ClientTop       =   2610
   ClientWidth     =   5370
   ClipControls    =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2205
   ScaleWidth      =   5370
   Begin MSComCtlLib.StatusBar stbStatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   8
      Top             =   1830
      Width           =   5370
      _ExtentX        =   9472
      _ExtentY        =   661
      Style           =   1
      SimpleText      =   ""
      BeginProperty Panels {0713E89E-850A-101B-AFC0-4210102A8DA7} 
         NumPanels       =   1
         BeginProperty Panel1 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            TextSave        =   ""
            Object.Tag             =   ""
         EndProperty
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin VB.PictureBox Picture1 
      Height          =   615
      Left            =   240
      ScaleHeight     =   555
      ScaleWidth      =   3435
      TabIndex        =   5
      Top             =   960
      Width           =   3495
      Begin VB.Label lblTime 
         AutoSize        =   -1  'True
         BackColor       =   &H00FFFFFF&
         BackStyle       =   0  'Transparent
         Caption         =   "00:00:00"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00800000&
         Height          =   300
         Left            =   1800
         TabIndex        =   7
         Top             =   120
         Width           =   1065
      End
      Begin VB.Label Label1 
         AutoSize        =   -1  'True
         Caption         =   "Current Time:"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   300
         Index           =   0
         Left            =   120
         TabIndex        =   6
         Top             =   120
         Width           =   1635
      End
   End
   Begin VB.TextBox txtInterval 
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   300
      Left            =   2685
      TabIndex        =   4
      Text            =   "1"
      Top             =   360
      Width           =   495
   End
   Begin VB.CommandButton cmdButton 
      Caption         =   "E&xit"
      Height          =   495
      Index           =   2
      Left            =   3975
      TabIndex        =   2
      Top             =   1200
      Width           =   1215
   End
   Begin VB.CommandButton cmdButton 
      Caption         =   "&Disconnect"
      Enabled         =   0   'False
      Height          =   495
      Index           =   1
      Left            =   3975
      TabIndex        =   1
      Top             =   660
      Width           =   1215
   End
   Begin VB.CommandButton cmdButton 
      Caption         =   "&Connect"
      Default         =   -1  'True
      Height          =   495
      Index           =   0
      Left            =   3975
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
   Begin VB.Label Label2 
      BackStyle       =   0  'Transparent
      Caption         =   "Update Interval:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   285
      Left            =   1140
      TabIndex        =   3
      Top             =   435
      Width           =   1500
   End
   Begin VB.Image Image1 
      Height          =   480
      Index           =   1
      Left            =   480
      Top             =   240
      Visible         =   0   'False
      Width           =   480
   End
   Begin VB.Image Image1 
      Height          =   480
      Index           =   0
      Left            =   480
      Top             =   240
      Width           =   480
   End
End
Attribute VB_Name = "frmCBCli"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'Please see the notes in the server project for how this application works.

Dim objCbSvr As Object
Dim mnCallInterval  As Integer
Dim mbIsConnected As Boolean

Private Sub cmdButton_Click(Index As Integer)
  Static objMyClassInstance As Object
  Dim bSuccess As Integer
  Dim bRetVal As Boolean
  Dim iCounter As Integer
  
  ' Used by OLE Collision Handler
  Dim nCurErrorCount As Integer
  Const MAX_ERROR_COUNT = 10
  
  On Error GoTo cmdButtonError
  
  Screen.MousePointer = vbHourglass
  
  Select Case Index
  
    Case 0  'Connect
      Set objMyClassInstance = New CbClientProj.CbClientClass
      Set objCbSvr = CreateObject("CbServerProj.CbServerClass")
      mnCallInterval = Val(txtInterval.Text)
      bSuccess = objCbSvr.SetInterval(mnCallInterval)
      If objCbSvr.AddObjectReference(objMyClassInstance) Then
        mbIsConnected = True
        cmdButton(0).Enabled = False
        cmdButton(1).Enabled = True
        cmdButton(2).Enabled = False
        Image1(0).Visible = False
        Image1(1).Visible = True
        stbStatusBar.SimpleText = "Connection Successful..."
        
      Else
        stbStatusBar.SimpleText = "Connect Failed..."
      
      End If
      
    Case 1  'Disconnect
        'Increase callback interval to reduce the risk of collision.
        bSuccess = objCbSvr.SetInterval(30)
        'Now tell the server to disconnect from us.
100   If objCbSvr.DropObjectReference(objMyClassInstance) Then
110   mbIsConnected = False
        'Dereference our local object
        Set objMyClassInstance = Nothing
        'Dereference the server object.
        Set objCbSvr = Nothing
        cmdButton(0).Enabled = True
        cmdButton(1).Enabled = False
        cmdButton(2).Enabled = True
        Image1(0).Visible = True
        Image1(1).Visible = False
        stbStatusBar.SimpleText = "Disconnect Successful..."
        
      Else
        stbStatusBar.SimpleText = "Disconnect Failed..."
        
      End If
    
    Case 2  'Exit
      Unload Me
    
  End Select
  
  Screen.MousePointer = vbDefault
  Exit Sub

cmdButtonError:
  'When using asynchronous callbacks between two OLE objects, this error checking code is
  'necessary to deal with a chance of a collision.  This collision can occur when a client and
  'server attempt to call each at the same time. This error handler forces the client to wait for a
  'random period of time and retry the failed operation.  During this wait time, the server should
  'complete it's call to the client allowing the client to succeed when it retrys the call to the server.
  'The same error handling code also needs to be implemented in the server object.
  If Erl = 100 And Err = &H80010001 Then
    If nCurErrorCount >= MAX_ERROR_COUNT Then
      MsgBox "Unable to release server reference.  Retry later.", vbExclamation, "Remote Server Disconnect Error"
      Resume EndOfError
    Else
      For iCounter = 1 To 2000 * Rnd()
        DoEvents
      Next iCounter
      Resume
    End If
  End If
  
  Screen.MousePointer = vbDefault
  MsgBox Error$, vbCritical, "cmdButton Error"
EndOfError:
End Sub
Private Sub Form_Load()
  Me.Left = (Screen.Width - Me.Width) / 2
  Me.Top = (Screen.Height - Me.Height) / 2
  
End Sub

Private Sub txtInterval_LostFocus()
        Dim bSuccess As Integer
        Dim iCounter As Integer
        
        ' Used by OLE Collision Handler
        Dim nCurErrorCount As Integer
        Const MAX_ERROR_COUNT = 10

        On Error GoTo SetIntervalError
        If Val(txtInterval.Text) <> mnCallInterval And mbIsConnected Then
          mnCallInterval = Val(txtInterval.Text)
100     bSuccess = objCbSvr.SetInterval(mnCallInterval)
110  End If

SetIntervalError:
  'When using asynchronous callbacks between two OLE objects, this error checking code is
  'necessary to deal with a chance of a collision.  This collision can occur when a client and
  'server attempt to call each at the same time. This error handler forces the client to wait for a
  'random period of time and retry the failed operation.  During this wait time, the server should
  'complete it's call to the client allowing the client to succeed when it retrys the call to the server.
  'The same error handling code also needs to be implemented in the server object.
  If Erl = 100 And Err = &H80010001 Then
    If nCurErrorCount >= MAX_ERROR_COUNT Then
      MsgBox "Unable to update timer interval.  Retry later.", vbExclamation, "Update Interval Error"
      Resume EndOfError
    Else
      For iCounter = 1 To 2000 * Rnd()
        DoEvents
      Next iCounter
      Resume
    End If
  End If
EndOfError:
End Sub


