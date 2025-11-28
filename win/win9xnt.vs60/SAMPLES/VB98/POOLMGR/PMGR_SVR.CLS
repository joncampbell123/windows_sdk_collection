VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "PoolMngrClass"
Attribute VB_Creatable = True
Attribute VB_Exposed = True
Option Explicit

Public Function objGetProjInstance(rstrProgID As String, rnProjID As Integer) As Object
'Note: This routine assumes PoolMngrClass has been created with MultiUse and is not reentrant!
'Return an OLE automation object reference to the requested project
'Return rnProjID < 0 if no Instances are available or error
Dim nPrjNode As Integer
Dim nSvrNode As Integer
Dim bFoundFree As Integer
Dim nFirstFree As Integer
On Error GoTo gciError

rnProjID = -1
nPrjNode = gnFirstPrjNode

While nPrjNode <> gnNIL And rnProjID < 0
  If gaPrjInfo(nPrjNode).strProgID = rstrProgID Then
    If gaPrjInfo(nPrjNode).intCurUseCount >= gaPrjInfo(nPrjNode).intMaxUseCount Then Exit Function
    nSvrNode = gaPrjInfo(nPrjNode).nFirstSvrNode
    bFoundFree = False
    While nSvrNode <> gnNIL And Not bFoundFree
      If gaSvrInst(nSvrNode).bInUse Then
        nSvrNode = gaSvrInst(nSvrNode).Next
      Else
        bFoundFree = True
      End If
    Wend
      
    If Not bFoundFree Then
      nSvrNode = GetNewServerNode(nPrjNode)
      If nSvrNode = gnNIL Then Exit Function
      Set gaSvrInst(nSvrNode).ObjHandle = CreateObject(rstrProgID)
    End If
      
    Set objGetProjInstance = gaSvrInst(nSvrNode).ObjHandle
    gaSvrInst(nSvrNode).DeallocTime = "12/31/9999"
    gaSvrInst(nSvrNode).bInUse = True
    gaPrjInfo(nPrjNode).intCurUseCount = gaPrjInfo(nPrjNode).intCurUseCount + 1
    rnProjID = nSvrNode
    
    If gaPrjInfo(nPrjNode).bLookAheadCreate And _
          gaPrjInfo(nPrjNode).intCurUseCount < gaPrjInfo(nPrjNode).intMaxUseCount Then
      'Save nPrjNode and set timer to wake up and do look ahead create
    End If
  Else
    nPrjNode = gaPrjInfo(nPrjNode).Next
  End If
Wend

  If rnProjID < 0 Then
'   objGetClassInstance = Nothing << causes error!  what can you return??
  End If
GoTo gciExit

gciError:
'  objGetClassInstance = Nothing << causes error!  what can you return??
  Resume gciExit
  
gciExit:
End Function
Public Function ReturnProjInstance(ByVal rstrProgID As String, rnSvrNode As Integer) As Integer
'Note: This routine assumes MultiUse and is not reentrant!
'Return False if error
Dim nPrjNode As Integer
Dim bDone As Integer
On Error GoTo rpiError

ReturnProjInstance = False
If rnSvrNode < 0 Then Exit Function
If Not gaSvrInst(rnSvrNode).bInUse Then Exit Function
nPrjNode = gnFirstPrjNode

While nPrjNode <> gnNIL And Not bDone
  If gaPrjInfo(nPrjNode).strProgID = rstrProgID Then
    gaPrjInfo(nPrjNode).intCurUseCount = gaPrjInfo(nPrjNode).intCurUseCount - 1
    If gaPrjInfo(nPrjNode).intCurUseCount >= gaPrjInfo(nPrjNode).intMinUseCount Then
      If gaPrjInfo(nPrjNode).intCloseDelay > 0 Then
        'Save nPrjNode and set timer to wake up and destroy node later
        gaSvrInst(rnSvrNode).DeallocTime = Now
      Else
        Set gaSvrInst(rnSvrNode).ObjHandle = Nothing
        If gaPrjInfo(nPrjNode).nFirstSvrNode = rnSvrNode Then
          gaPrjInfo(nPrjNode).nFirstSvrNode = gaSvrInst(gaPrjInfo(nPrjNode).nFirstSvrNode).Next
        End If
        NodeRemove gnINST_TYPE, rnSvrNode
      End If
      bDone = True
    Else
      gaSvrInst(rnSvrNode).bInUse = False
    End If
    ReturnProjInstance = True
  End If
  nPrjNode = gaPrjInfo(nPrjNode).Next
Wend
GoTo rpiExit

rpiError:
Resume rpiExit

rpiExit:
End Function

