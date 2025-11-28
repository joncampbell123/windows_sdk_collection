Attribute VB_Name = "modSubMain"
'------------------------------------------------------------------------
'     Copyright © 1997 Microsoft Corporation. All rights reserved.
'You have a royalty-free right to use, modify, reproduce and distribute
'the Sample Application Files (and/or any modified version) in any way
'you find useful, provided that you agree that Microsoft has no warranty,
'     obligations or liability for any Sample Application Files.
'------------------------------------------------------------------------
Option Explicit

'------------------------------------------------------------
' constant declares...
'------------------------------------------------------------
Public Const IID_IDispatch = "{00020400-0000-0000-C000-000000000046}"           ' IDispatch interface id
Public Const IID_IPersistStorage = "{0000010A-0000-0000-C000-000000000046}"     ' IPersistStorage interface id
Public Const IID_IPersistStream = "{00000109-0000-0000-C000-000000000046}"      ' IPersistStream interface id
Public Const IID_IPersistPropertyBag = "{37D84F60-42CB-11CE-8135-00AA004BB851}" ' IPersistPropertyBag interface id

' Option bit definitions for IObjectSafety
Public Const INTERFACESAFE_FOR_UNTRUSTED_CALLER = &H1   ' Caller of interface may be untrusted
Public Const INTERFACESAFE_FOR_UNTRUSTED_DATA = &H2     ' Data passed into interface may be untrusted

Public Const E_NOINTERFACE = &H80004002 ' No such interface supported
Public Const E_FAIL = &H80004005        ' Unspecified error

Public Const MAX_GUIDLEN = 40           ' This must be 40 for NT.

'------------------------------------------------------------
' API declares...
'------------------------------------------------------------
Public Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (pDest As Any, pSource As Any, ByVal ByteLen As Long)
Public Declare Function StringFromGUID2 Lib "ole32.dll" (rguid As Any, ByVal lpstrClsId As Long, ByVal cbMax As Integer) As Long

'------------------------------------------------------------
' UDT declares...
'------------------------------------------------------------
Public Type uGUID
    Data1 As Long
    Data2 As Integer
    Data3 As Integer
    Data4(7) As Byte
End Type

'------------------------------------------------------------
Public Function GetIIDFromPTR(ByVal riid As Long) As String
'------------------------------------------------------------
    Dim Rc          As Long                                         ' function return code
    Dim rClsId      As uGUID                                        ' guid struct
    Dim bIID()      As Byte                                         ' byte array for interface id
'------------------------------------------------------------
    If (riid <> 0) Then                                             ' validate pointer to interface id
        CopyMemory rClsId, ByVal riid, Len(rClsId)                  ' copy interface guid to struct
        
        bIID = String$(MAX_GUIDLEN, 0)                              ' pre-allocate byte array
        Rc = StringFromGUID2(rClsId, VarPtr(bIID(0)), MAX_GUIDLEN)  ' get clsid from guid struct
        Rc = InStr(1, bIID, vbNullChar) - 1                         ' look for trailing null char.s
        GetIIDFromPTR = Left$(UCase(bIID), Rc)                      ' trim extra nulls and convert to upper-case for comparison
    End If
'------------------------------------------------------------
End Function
'------------------------------------------------------------
