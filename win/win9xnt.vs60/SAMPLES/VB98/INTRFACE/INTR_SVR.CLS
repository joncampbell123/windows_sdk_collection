VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ServerInterface"
Attribute VB_Creatable = True
Attribute VB_Exposed = True
Public Function objGetClassInstance(rstrClassName As String) As Object
'Return an OLE automation object reference to the requested class
'Return "Nothing" if error
On Error GoTo gciError

Set objGetClassInstance = CreateObject("InterfaceProj." & rstrClassName)
GoTo gciExit

gciError:
  objGetClassInstance = Nothing
  Resume gciExit
  
gciExit:
End Function
