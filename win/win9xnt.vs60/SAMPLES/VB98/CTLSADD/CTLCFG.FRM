VERSION 5.00
Object = "{CDE57A40-8B86-11D0-B3C6-00A0C90AEA82}#1.0#0"; "MSDatGrd.Ocx"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "ComDlg32.ocx"
Begin VB.Form frmCtlCfg 
   Caption         =   "Control Configuration Sample"
   ClientHeight    =   7950
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   10545
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   9.75
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form1"
   ScaleHeight     =   7950
   ScaleWidth      =   10545
   StartUpPosition =   3  'Windows Default
   Begin MSComDlg.CommonDialog dlgFind 
      Left            =   9000
      Top             =   7440
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin MSDataGridLib.DataGrid grdControls 
      Height          =   6855
      Left            =   120
      TabIndex        =   1
      Top             =   480
      Width           =   10095
      _ExtentX        =   17806
      _ExtentY        =   12091
      _Version        =   393216
      AllowUpdate     =   -1  'True
      BackColor       =   -2147483633
      HeadLines       =   1
      RowHeight       =   15
      TabAction       =   2
      BeginProperty HeadFont {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
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
      ColumnCount     =   2
      BeginProperty Column00 
         DataField       =   ""
         Caption         =   ""
         BeginProperty DataFormat {6D835690-900B-11D0-9484-00A0C91110ED} 
            Type            =   0
            Format          =   ""
            HaveTrueFalseNull=   0
            FirstDayOfWeek  =   0
            FirstWeekOfYear =   0
            LCID            =   1024
         EndProperty
      EndProperty
      BeginProperty Column01 
         DataField       =   ""
         Caption         =   ""
         BeginProperty DataFormat {6D835690-900B-11D0-9484-00A0C91110ED} 
            Type            =   0
            Format          =   ""
            HaveTrueFalseNull=   0
            FirstDayOfWeek  =   0
            FirstWeekOfYear =   0
            LCID            =   1024
         EndProperty
      EndProperty
      SplitCount      =   1
      BeginProperty Split0 
         BeginProperty Column00 
         EndProperty
         BeginProperty Column01 
         EndProperty
      EndProperty
   End
   Begin VB.Label lblInfo 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   $"CtlCfg.frx":0000
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H8000000D&
      Height          =   435
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   10245
      WordWrap        =   -1  'True
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuSave 
         Caption         =   "&Save"
         Shortcut        =   ^S
      End
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuEdit 
      Caption         =   "&Edit"
      Begin VB.Menu mnuNewEntry 
         Caption         =   "&New Entry"
         Shortcut        =   ^N
      End
      Begin VB.Menu mnuDeleteEntry 
         Caption         =   "&Delete Entry"
         Shortcut        =   ^D
      End
   End
End
Attribute VB_Name = "frmCtlCfg"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim rsControls As New ADODB.Recordset
Dim cnControls As New ADODB.Connection

Private Sub Form_Load()
    On Error GoTo FindErr
    Dim strQ As String
    strQ = "provider=Microsoft.Jet.OLEDB.3.51;data source=" & App.Path & "\controls.mdb"
    cnControls.Open strQ
  
    rsControls.Open "select * from controls order by description", cnControls, adOpenKeyset, adLockOptimistic
    Set grdControls.DataSource = rsControls
    Exit Sub
FindErr:
    ' If the database isn't found, use the FindDB function to find it.
    If Err.Number = -2147467259 Then
    cnControls.Open "Provider=Microsoft.Jet.OLEDB.3.51;Data source=" & FindDB("controls.mdb")
    Resume Next
    End If
    Exit Sub
End Sub

Private Function FindDB(dbName As String) As String
    On Error GoTo ErrHandler

    ' Configure cmdDialog in case the database can't be found.
    With dlgFind
        .DialogTitle = "Can't Find " & dbName
        .Filter = "(*.MDB)|*.mdb"
        .CancelError = True   'Causes an error if user clicks on cancel
        .ShowOpen
    End With
    ' Test the string to ensure it's the sought database.
    Do While Right(Trim(dlgFind.FileName), Len(dbName)) <> dbName
       MsgBox "File Name is not equal to " & dbName
       dlgFind.ShowOpen
    Loop
    
    FindDB = dlgFind.FileName ' return the full path.
    Exit Function
ErrHandler:
    Select Case Err.Number
    Case 32755
      Unload Me
    Case Else
        MsgBox Err.Number & ": " & Err.Description
    End Select

End Function



Private Sub Form_Resize()
  lblInfo.Width = ScaleWidth
  grdControls.Move 0, lblInfo.Height, ScaleWidth, ScaleHeight - lblInfo.Height
End Sub

Private Sub grdControls_Error(ByVal DataError As Integer, Response As Integer)
  Response = 0
End Sub

Private Sub mnuDeleteEntry_Click()
  rsControls.Delete
End Sub

Private Sub mnuExit_Click()
    Unload Me
End Sub

Private Sub mnuNewEntry_Click()
  rsControls.AddNew
End Sub

Private Sub mnuSave_Click()
  Dim vControlLicense As Variant
  Dim sControlType As String
  
  On Error Resume Next
  rsControls.MoveFirst
  While Not rsControls.EOF
    vControlLicense = Null
    sControlType = rsControls.Fields("ControlType")
    vControlLicense = Licenses.Add(sControlType)
    Licenses.Remove sControlType
    rsControls.Fields("ControlLicense") = vControlLicense
    rsControls.MoveNext
  Wend

  rsControls.Update
  rsControls.MoveFirst
End Sub
