Attribute VB_Name = "modQueueSupportHelper"
Option Explicit
'--------------------------------------------------
' Contains HELPER functions useful for message queue access.  These functions
' are NOT intended to be called from applications (they don't do anything) that
' useful.  They are largely intended to support the main functions
'
' [11/10/97, IvoSa] Created
'--------------------------------------------------

Const LEN_GUID_WITH_BRACKETS = 38
Const LEN_GUID_WITHOUT_BRACKETS = LEN_GUID_WITH_BRACKETS - 2

'------------------------------------------------------------------
'Called when the correct QUEUE GUID was not found in the system registry (either it is missing or incorrect)
' [in] strQueueName : Name of queue we need to look up
' [retval]          : GUID of QUEUE
'
' [11/10/97, IvoSa] Created
'------------------------------------------------------------------
Function LookupGUIDAndPersistToRegisty(ByVal strQueueName As String) As String
Dim strQueueGUID As String
        
    'Look for the queue
    strQueueGUID = getGUIDOfMessageQueue(strQueueName)

    'Do some validity checking on the GUID returned
    If (Not (IsGUID(strQueueGUID))) Then
        Err.Raise qeBOGUS_GUID, "LookupGUIDAndPersistToRegisty", "Invalid MSMQ ID GUID: " & qeBOGUS_GUID
    End If

    'Register the GUID w/the queue name
    SaveSetting regAPPLICATION_NAME, strQueueName, regKEY_NAME, strQueueGUID
    
    'GUID of QUEUE...
    LookupGUIDAndPersistToRegisty = strQueueGUID
End Function

'--------------------------------------------------
'Checks the validity of a 'guid' string
' [in] strQueueGUID  : Name of queue we want to open
'
' [11/10/97, IvoSa] Created
'--------------------------------------------------
Function IsGUID(ByVal strQueueGUID As String) As Boolean
    strQueueGUID = Trim(strQueueGUID)
    
    Dim lngStrLen As Long
    lngStrLen = Len(strQueueGUID)
    
    Select Case lngStrLen
    Case LEN_GUID_WITH_BRACKETS
    Case LEN_GUID_WITHOUT_BRACKETS
    Case Else
        IsGUID = False: Exit Function
    End Select

    'Proably want some additional tests
    IsGUID = True
End Function


'--------------------------------------------------
'Display warning to the application user
' [11/10/97, IvoSa] Created
'--------------------------------------------------
Sub Queue_Warning(strWarning As String)
#If Not SUPPRESS_WARNINGS Then
    MsgBox strWarning, , "Queue access warning"
#End If
End Sub

'-----------------------------------------------------------
'Makes sure a GUID is in a form w/o {}'s
' [in] strGuid : GUID w/ or w/o {}'s
'
' [11/12/97, IvoSa] Created
'-----------------------------------------------------------
Function getGUIDWithoutBrackets(strGuid As String) As String
    Dim lngStrLen As Long
    lngStrLen = Len(strGuid)
    
    Select Case lngStrLen
    Case LEN_GUID_WITH_BRACKETS
        getGUIDWithoutBrackets = Mid(strGuid, 2, LEN_GUID_WITHOUT_BRACKETS)
    Case LEN_GUID_WITHOUT_BRACKETS
        getGUIDWithoutBrackets = strGuid
    Case Else
        Err.Raise qeBOGUS_GUID, "getGUIDWithoutBrackets", strGuid & " not a GUID 0 bogus length."
    End Select
End Function

'-----------------------------------------------------------
'Makes sure a GUID is in a form w/o {}'s
' [in] strGuid : GUID w/ or w/o {}'s
'
' [11/12/97, IvoSa] Created
'-----------------------------------------------------------
Function getGUIDWithBrackets(strGuid As String) As String
    Dim lngStrLen As Long
    lngStrLen = Len(strGuid)
    
    Select Case lngStrLen
    Case LEN_GUID_WITH_BRACKETS
        getGUIDWithBrackets = strGuid
    Case LEN_GUID_WITHOUT_BRACKETS
        getGUIDWithBrackets = "{" & strGuid & "}"
    Case Else
        Err.Raise qeBOGUS_GUID, "getGUIDWithBrackets", strGuid & " not a GUID 0 bogus length."
    End Select
End Function


'--------------------------------------------------
'Queue information needed to create a queue
'
' [in] strPath  : path to queue
' [in] strLabel  : label of queue
' [retval] QueueInfo we need to create the object
'
' NOTES: The purpose of this function is to combine the creation/initialtion
'        of this object into 1 atomic unit.
'
' [11/12/97, IvoSa] Created
'--------------------------------------------------
Function getQueueInfoForCreation(strPath As String, strLabel As String) As MSMQQueueInfo
 Dim objQInfo As MSMQQueueInfo

    'UNDONE: Should add code to check if a Queue of this name already exists.
    
    Set objQInfo = New MSMQQueueInfo
    objQInfo.PathName = strPath
    objQInfo.Label = strLabel
    
    Set getQueueInfoForCreation = objQInfo
End Function


'--------------------------------------------------
'Queue information needed to open a queue by GUID name
'
' [in] strGUIDForQueue : GUID of queue
' [retval] QueueInfo we need to create the object
'
' NOTES: The purpose of this function is to combine the creation/initialtion
'        of this object into 1 atomic unit.
'
' [11/12/97, IvoSa] Created
'--------------------------------------------------
Function getQueueInfoForLogon(strGUIDForQueue As String) As MSMQQueueInfo
    Dim objQI As MSMQQueueInfo
    Set objQI = New MSMQQueueInfo
    objQI.FormatName = "PUBLIC=" & getGUIDWithoutBrackets(strGUIDForQueue)
    Set getQueueInfoForLogon = objQI
End Function

'--------------------------------------------------
'Queue information needed to delete a queue by Label
'
' [in] strLabelForQueue  : Label of queue
' [retval] QueueInfo we need to delete the object
'
' NOTES: The purpose of this function is to combine the creation/initialtion
'        of this object into 1 atomic unit.
'
' [11/12/97, IvoSa] Created
'--------------------------------------------------
Function getQueueInfoForDeletion(strLabelForQueue As String) As MSMQQueueInfo
    Dim objQuery As MSMQQuery
    Set objQuery = New MSMQQuery
    
    Dim objQI As MSMQQueueInfo
    Dim objQIs As MSMQQueueInfos
    Set objQIs = objQuery.LookupQueue(Label:=strLabelForQueue)
    
    objQIs.Reset
    
    'UNDONE: May want to check if there is more than 1
    Set getQueueInfoForDeletion = objQIs.Next
End Function


'--------------------------------------------------
'Returns the GUID that identifies a message queue
' [in] strQueueName : Name of queue we want to open
'
' ASSUMPTION: We are 'hooked up to the net' when this is called (we may hang otherwise)
'
' [11/10/97, IvoSa] Created
'--------------------------------------------------
Function getGUIDOfMessageQueue(ByVal strQueueName As String) As String
Dim objQuery As MSMQQuery
Dim objQInfos As MSMQQueueInfos
Dim objQInfo As MSMQQueueInfo
Dim objQinfoTemp As MSMQQueueInfo
    
    'New query to look for message queues
    Set objQuery = New MSMQQuery
    
    'Execute the query, will return a set of queries
    Set objQInfos = objQuery.LookupQueue(Label:=strQueueName)
    
    'Go back to the beginning (Is this necessary?)
    objQInfos.Reset

    'Itterate through all the message queues & look for ours
    Dim lngCount As Long
    Set objQinfoTemp = objQInfos.Next
    While Not (objQinfoTemp Is Nothing)
        Set objQInfo = objQinfoTemp
        lngCount = lngCount + 1
        
        'Advance to the next item
        Set objQinfoTemp = objQInfos.Next
    Wend
    
    'Should only have been one queue with out specified ID
    If (lngCount > 1) Then
        Queue_Warning "Queue count not one (" & CStr(lngCount) & ").  More than one queue has the same label, either: 1) the query for the queue should be more specific or 2) all but one of the queues with this label on the enterprise should be deleted.  Defaulting to the first queue we found."
    End If
    
    getGUIDOfMessageQueue = objQInfo.QueueGuid
End Function


