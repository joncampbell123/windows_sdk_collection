VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "VisDataClass"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Attribute VB_Description = "VisData Database Utility"
'>>>>>>>>>>>>>>>>>>>>>>>>
'>>>>>>>>>>>>>>>>>>>>>>>>

Public VBInstance As VBIDE.VBE

Implements IDTExtensibility

'---------------------------------------------------
'this method is used as a call from an external
'launch utility or the VB IDE
'---------------------------------------------------
Private Sub IDTExtensibility_OnConnection(ByVal VBInst As Object, ByVal ConnectMode As vbext_ConnectMode, ByVal AddInInst As VBIDE.AddIn, custom() As Variant)
  On Error GoTo LVDErr
    
  Dim rc As Long
  Set gVDClass = Me
  'this sets the VB instance handle
  'that is used by the DataFormDesigner form
  Set VBInstance = VBInst
  frmMDI.mnuUDataFormDesigner.Visible = True
  frmMDI.WindowState = vbNormal
  frmMDI.SetFocus
  
  Exit Sub
    
LVDErr:
  MsgBox Err.Description
End Sub

Private Sub IDTExtensibility_OnDisconnection(ByVal RemoveMode As vbext_DisconnectMode, custom() As Variant)
  On Error Resume Next
  Unload frmMDI
End Sub

Private Sub IDTExtensibility_OnStartupComplete(custom() As Variant)
'
End Sub

Private Sub IDTExtensibility_OnAddInsUpdate(custom() As Variant)
'
End Sub

'---------------------------------------------------
'this method can be called from
'any vb app through OLE automation as in:
'Dim x As Object
'Set x = CreateObject("VisData.VisDataClass")
'x.VDOpenDatabase "c:\vb\biblio.mdb", "", False
'---------------------------------------------------
Sub VDOpenDatabase(sDatabaseName As String, sConnect As String, bReadOnly As Integer, bShowForm As Boolean)
  On Error GoTo RVDErr
    
  If bShowForm Then
    frmMDI.mnuUDataFormDesigner.Visible = False
  End If
  
  gnReadOnly = bReadOnly
  If Len(sConnect) = 0 Then
    'must be a Microsoft Access MDB
    gsDataType = gsMSACCESS
    gsDBName = sDatabaseName
    OpenLocalDB True
  ElseIf UCase(Left(sConnect, 5)) = gsSQLDB Then
    'must be an ODBC database
    gsDataType = gsSQLDB
    'set the other variables for ODBC
    GetODBCConnectParts sConnect
    gsDBName = sDatabaseName
    SendKeys "%FOO{Enter}"
  Else
    'must be a local ISAM database
    gsDataType = sConnect
    gsDBName = sDatabaseName
    OpenLocalDB True
  End If
  
  Exit Sub
    
RVDErr:
  MsgBox Err.Description
End Sub

'---------------------------------------------------
'this method simply closes visdata from the client
'---------------------------------------------------
Sub VDClose()
  Unload frmMDI
End Sub

