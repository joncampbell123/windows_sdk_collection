VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 1  'vbDataSource
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "MyDataSource"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

Private Sub Class_GetDataMember(DataMember As String, Data As Object)
    ' Declare an instance of the MyObject class
    Dim MyOSP As New MyOSPObject
    
    ' Make sure the DataMember is valid
    If DataMember = "" Then
        Err.Raise (E_FAIL)
    End If
    
    ' Set the FilePath property
    MyOSP.FilePath = DataMember
    
    ' Call the LoadData method to populate the class
    MyOSP.LoadData
    
    ' Set MyDataSource's data to MyOSPObject's data
    Set Data = MyOSP
End Sub

