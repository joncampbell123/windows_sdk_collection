VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0   'False
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
END
Attribute VB_Name = "StringBag"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit
'------------------------------------------------------
'Use a property bag to store, retrieve, persist & depersist data...
'------------------------------------------------------
Private m_pbProperties As PropertyBag

'------------------------------------------------
'Adds an items to our store...
'------------------------------------------------
Public Sub Add(strKey As String, strValue As String)
    'Store the value
    m_pbProperties.WriteProperty strKey, strValue
End Sub

'------------------------------------------------
'Retrieves an item from our store...
'
' [in] strKey : Key to item we are retrieving
'------------------------------------------------
Public Function Retrieve(strKey As String) As Variant
    Retrieve = m_pbProperties.ReadProperty(strKey)
End Function

Private Sub Class_Initialize()
    'Initialize the property bag
    Set m_pbProperties = New PropertyBag
End Sub

'------------------------------------------------
'Takes contents of our class store & serializes them into as string
'
'------------------------------------------------
Public Function Serialize() As String
    'Dump the contents of the property bag into a string
    Serialize = m_pbProperties.Contents
End Function


'------------------------------------------------
'Takes contents of our persisted string & expands it into properties, etc
' [in] strString : Contents we want to place into our property bag
'------------------------------------------------
Public Sub DeSerialize(ByVal strString As String)
Dim byteArr() As Byte
    
    'Must place the string into a byte array so that .Contents will accept this assignment
    byteArr = strString
    
    m_pbProperties.Contents = byteArr
End Sub

'------------------------------------------------
'Takes contents of our persisted string & expands it into properties, etc
'
' [in] objMQ     : Message Queue we are persisting to
' [in] strLabel : Label of message
'
' [11/12/97, IvoSa] Created
'------------------------------------------------
Public Sub serilizeToQueue(ByVal objMQ As MSMQQueue, Optional ByVal strLabel As String = "")
    sendMessageToQueue objMQ, strLabel, Me.Serialize
End Sub

'------------------------------------------------
'Takes contents of our persisted string & expands it into properties, etc
'
' [in] objMQ     : Message Queue we are persisting to
' [in] strLablel : Label of message
'
' [11/12/97, IvoSa] Created
'------------------------------------------------
Public Sub deserilizeFromQueue(ByVal objMessage As MSMQMessage)
        Me.DeSerialize CStr(objMessage.Body)
End Sub



Private Sub Warning(strMsg As String)
    MsgBox strMsg
End Sub
