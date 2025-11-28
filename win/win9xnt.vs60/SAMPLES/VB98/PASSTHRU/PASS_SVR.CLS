VERSION 1.0 CLASS
BEGIN
  MultiUse = 0   'False
END
Attribute VB_Name = "PassThruClass"
Attribute VB_Creatable = True
Attribute VB_Exposed = True
Option Explicit

Public Function RunServer(rstrProgID As String, rbSuccess As Integer) As Object
'Note: This routine assumes PassThruClass has been created with SingleUse.
'Returns the handle to a requested server if rbSuccess = true, otherwise rbSuccess = false

  On Error GoTo csError

  Set RunServer = CreateObject(rstrProgID)
  frmPassThru.labSvrProgID = rstrProgID
  rbSuccess = True
  GoTo csExit
  
csError:
  DisplayError
  Resume csExit
  
csExit:
End Function

Private Sub DisplayError()
  #If gbDEBUG2 Then
    MsgBox Error$
  #End If
End Sub

