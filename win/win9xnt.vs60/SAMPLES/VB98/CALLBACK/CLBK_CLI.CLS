VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "CbClientClass"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit
'This internal class is created by the client and passed to the remote server.
'The remote server hangs onto it and "feeds it" data periodically.
'Note: the class properties (hit F4) mark this class as Public.  This
'allows it to be used by the external clock server.

Private Sub class_terminate()
  Debug.Print "CbClientProj.CbClientClass Terminated."
End Sub

Private Sub class_initialize()
  Debug.Print "CbClientProj.CbClientClass Initialized."
End Sub

Public Sub TellTime(sCurTime As String)
  'This is the *public* method the server calls to update the client's time.
  'In a real scenario, the server would typically be doing somethig
  'much more substantial... such as delivering stock quotes, user
  'requested news articles... or business reports.  Note, it this sub
  'were not marked as public, the server could not call it.
  
    frmCBCli.lblTime.Caption = sCurTime
End Sub

