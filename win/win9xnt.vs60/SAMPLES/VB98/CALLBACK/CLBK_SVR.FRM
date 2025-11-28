VERSION 5.00
Begin VB.Form frmMain 
   Caption         =   "CallBack Clock Server"
   ClientHeight    =   1140
   ClientLeft      =   4350
   ClientTop       =   3870
   ClientWidth     =   3465
   ClipControls    =   0   'False
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1140
   ScaleWidth      =   3465
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   5000
      Left            =   2985
      Top             =   60
   End
   Begin VB.Label lblInterval 
      AutoSize        =   -1  'True
      Caption         =   "00"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Left            =   2025
      TabIndex        =   4
      Top             =   645
      Width           =   255
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "(Secs.)"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Index           =   2
      Left            =   2595
      TabIndex        =   3
      Top             =   630
      Width           =   750
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "Update Interval:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Index           =   1
      Left            =   150
      TabIndex        =   2
      Top             =   630
      Width           =   1665
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "Current Time:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Index           =   0
      Left            =   150
      TabIndex        =   1
      Top             =   270
      Width           =   1395
   End
   Begin VB.Label lblTime 
      AutoSize        =   -1  'True
      Caption         =   "00:00:00"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Left            =   2025
      TabIndex        =   0
      Top             =   285
      Width           =   855
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_TemplateDerived = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim sCurTime As String

Private Sub lblInterval_Click()
  lblInterval.Caption = Format$(Timer1.Interval / 1000)
End Sub

Private Sub Timer1_Timer()
  Dim iCounter As Integer
  ' Used by OLE Collision Handler
  Dim nCurErrorCount As Integer
  Const MAX_ERROR_COUNT = 10

  On Error GoTo CallbackError
  
      If gbConnected Then
        sCurTime = Time
        lblTime.Caption = sCurTime
100   gObjRef.TellTime (sCurTime)
110 End If
Exit Sub

CallbackError:
  'When using asynchronous callbacks between two OLE objects, this error checking code is
  'necessary to deal with a chance of a collision.  This collision can occur when a client and
  'server attempt to call each at the same time. This error handler forces the server to wait for a
  'random period of time and retry the failed operation.  During this wait time, the client should
  'complete it's call to the server allowing the server to succeed when it retrys the call to the client.
  'The same error handling code also needs to be implemented in the client object.
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
EndOfError:
End Sub










