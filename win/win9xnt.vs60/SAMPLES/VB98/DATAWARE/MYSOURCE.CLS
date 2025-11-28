VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 1  'vbDataSource
END
Attribute VB_Name = "MySource"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit
Private rs As ADODB.Recordset

Public Property Get DataMembers() As DataMembers

End Property

Private Sub Class_GetDataMember(DataMember As String, Data As Object)
    ' Assign the Recordset to the Data object
    Set Data = rs
End Sub

Private Sub Class_Initialize()
    Dim strPath As String, strName As String
    Dim i As Integer
    
    ' Create an instance of the Recordset
    Set rs = New ADODB.Recordset
    
    ' Set the properties of the Recordset
    With rs
        .Fields.Append "DirID", adInteger
        .Fields.Append "Directory", adBSTR, 255
        .CursorType = adOpenStatic
        .LockType = adLockOptimistic
        .Open
    End With
    
    ' Loop through the directories and populate
    ' the Recordset.
    strPath = "C:\"
    strName = Dir(strPath, vbDirectory)
    i = 0
    Do While strName <> ""
        If strName <> "." And strName <> ".." _
            And strName <> "pagefile.sys" Then
            If (GetAttr(strPath & strName) And _
                vbDirectory) = vbDirectory Then
                i = i + 1
                With rs
                    .AddNew
                    .Fields.Item("DirID") = i
                    .Fields.Item("Directory") = strName
                    .Update
                End With
            End If
        End If
        strName = Dir
    Loop
    
    ' Return to the first record
    rs.MoveFirst
End Sub
Public Sub Cycle()
    ' Cycle through the Recordset
    rs.MoveNext
    If rs.EOF = True Then
        rs.MoveFirst
    End If
End Sub
