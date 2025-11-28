VERSION 5.00
Object = "{F0D2F211-CCB0-11D0-A316-00AA00688B10}#1.0#0"; "MSDatLst.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "ComDlg32.OCX"
Begin VB.Form frmCtlView 
   BackColor       =   &H8000000C&
   Caption         =   "Control Viewer Sample"
   ClientHeight    =   8595
   ClientLeft      =   165
   ClientTop       =   450
   ClientWidth     =   10680
   LinkTopic       =   "Form1"
   ScaleHeight     =   573
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   712
   StartUpPosition =   3  'Windows Default
   Begin MSComDlg.CommonDialog dlgFind 
      Left            =   7800
      Top             =   480
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin MSDataListLib.DataList lstControls 
      Height          =   9030
      Left            =   0
      TabIndex        =   1
      Top             =   420
      Width           =   2595
      _ExtentX        =   4577
      _ExtentY        =   15928
      _Version        =   393216
   End
   Begin VB.Label lblInfo 
      Appearance      =   0  'Flat
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   $"CtlView.frx":0000
      ForeColor       =   &H8000000D&
      Height          =   420
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   10635
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "frmCtlView"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim rsControls As New ADODB.Recordset
Dim cnControls As New ADODB.Connection
Dim oControl As Object

Private Sub Form_Load()
    On Error GoTo FindErr
    Dim strQ As String ' query string
    strQ = "Provider=Microsoft.Jet.OLEDB.3.51;Data source=" & App.Path & "\controls.mdb"
    cnControls.Open strQ
    rsControls.Open "select * from controls order by description", cnControls, adOpenKeyset, adLockOptimistic
    lstControls.ListField = "Description"
    Set lstControls.RowSource = rsControls
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
    If Err = 32755 Then
      Unload Me
    End If

End Function

Private Sub Form_Resize()
  lblInfo.Width = ScaleWidth
  lstControls.Move 0, lblInfo.Height, lstControls.Width, ScaleHeight - lblInfo.Height
End Sub

Private Sub lstControls_Click()
  Dim vControlLicense As Variant
  Dim vControlType As Variant
  Dim vPropertyName As Variant
  Dim vPropertyValue As Variant
  Dim vControlWidth As Variant
  Dim vControlHeight As Variant
  Dim sError As String
  
  If Not oControl Is Nothing Then
    Controls.Remove oControl
    Set oControl = Nothing
  End If
  
  rsControls.MoveFirst
  Do
    If rsControls.EOF Then Exit Do
    If rsControls.Fields("Description") = lstControls.BoundText Then
      Exit Do
    End If
    rsControls.MoveNext
  Loop
   
  vPropertyName = rsControls.Fields("PropertyName")
  vPropertyValue = rsControls.Fields("PropertyValue")
  vControlLicense = rsControls.Fields("ControlLicense")
  vControlType = rsControls.Fields("ControlType")
  vControlWidth = rsControls.Fields("ControlWidth")
  vControlHeight = rsControls.Fields("ControlWidth")
  
  On Error GoTo CantFindControl
  
  If (Not IsNull(vControlLicense)) Then
    sError = "unable to add license"
    Licenses.Add vControlType, vControlLicense
  End If
  
  sError = "unable to create control license"
  Set oControl = Controls.Add(vControlType, "MyControl")
  
  If (Not IsNull(vControlLicense)) Then
    sError = "unable to remove license"
    Licenses.Remove vControlType
  End If
  
  If (Not IsNull(vControlWidth)) Then
    sError = "unable to set Width"
    oControl.Width = vControlWidth
  End If
  
  If (Not IsNull(vControlHeight)) Then
    sError = "unable to set Height"
    oControl.Height = vControlHeight
  End If
  
  sError = "unable to set Left"
  oControl.Left = lstControls.Width + ((ScaleWidth - lstControls.Width) - oControl.Width) / 2
  
  sError = "unable to set Top"
  oControl.Top = lblInfo.Height + ((ScaleHeight - lblInfo.Height) - oControl.Height) / 2
  
  sError = "unable to set Visible"
  oControl.Visible = True
  
  If (Not IsNull(vPropertyName)) Then
    sError = "unable to set Property '" & vPropertyName & "'"
    If (Left$(vControlType, 3) = "VB.") Then
      CallByName oControl, vPropertyName, VbLet, vPropertyValue
    Else
      CallByName oControl.object, vPropertyName, VbLet, vPropertyValue
    End If
  End If
    
  Exit Sub
  
CantFindControl:
  MsgBox "Error adding control '" & vControlType & "', " & sError & ", " & Err.Description
End Sub

