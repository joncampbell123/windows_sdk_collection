Attribute VB_Name = "modQueueSupport"
Option Explicit
'--------------------------------------------------
' Contains functions useful for message queues.  These functions
' are intended to be called from applications to create, destroy, manage
' and access queues.
'
' [11/10/97, IvoSa] Created
'--------------------------------------------------

'Registry info used to manage Queue IDs, etc.
Global Const regAPPLICATION_NAME = "MessageQueueAdministration"
Global Const regKEY_NAME = "QueueGUID"
Global Const regNOT_FOUND = "<Not Found>"

'Time
Global Const ONE_SECOND = 1000 '1000ms
Global Const FIVE_SECONDS = ONE_SECOND * 5


'Errors that this module may raise
Public Enum QueueErrors
    qeNO_QUEUE = 30000
    qeBOGUS_GUID = 30001
End Enum

'--------------------------------------------------
'Opens a message queue
' [in] strQueueName : Name of queue we want to open
' [in] qaAccessMode : Mode of access (read/write)
' [in] boolPromotUser : prompt the user if we need more information?
' [out] objQueueInfo : Queue info (may be of some use to the caller)
' [retval] : Queue object
'
' This is a general function who's purpose is to open a message queue
' for the user, regarless of wheather or not we (or the queue) are currently
' connectected to the network.  This function must be called at least once
' with us connected to the network or it will fail.  (The first time we call
' this function on a machine/user we will note the message queues GUID & record
' it for future reference)
'
' [11/10/97, IvoSa] Created
'--------------------------------------------------
Public Function openMessageQueue(ByVal strQueueName As String, ByVal qaAccessMode As MQACCESS, Optional ByVal boolPromotUser As Boolean = True, Optional objQueueInfo As MSMQQueueInfo) As MSMQQueue
    Dim strQueueGUID As String
    
    '-------------------------------------------------------------------
    'Determine the GUID of the message queue
    '-------------------------------------------------------------------
    
    'Get the GUID that uniquely identifies our queue
    strQueueGUID = GetSetting(regAPPLICATION_NAME, strQueueName, regKEY_NAME, regNOT_FOUND)
    
    'Was there no value specified for our QUEUE?
    If (strQueueGUID = regNOT_FOUND) Then
        
        'See if we can ask the user
        If (boolPromotUser = True) Then
            Dim lngYesNo As VbMsgBoxResult
            lngYesNo = MsgBox("There is no locally registered queue for '" & strQueueName & ". Would you like to look for this queue on the network ('No' will abort)?", vbYesNo)
            
            'If they said no, abort
            If (lngYesNo = vbNo) Then
                Err.Raise qeNO_QUEUE, "OpenMessageQueue", "There is no registered queue for this user/machine for " & strQueueName
            End If
        End If
        
        strQueueGUID = LookupGUIDAndPersistToRegisty(strQueueName)
    End If
    
    '-------------------------------------------------------------------
    'Attempt to open the message queue...
    '-------------------------------------------------------------------
    Dim lngError As Long
    Dim strError As Long
    On Error Resume Next
        Set objQueueInfo = getQueueInfoForLogon(strQueueGUID)
        Set openMessageQueue = objQueueInfo.Open(qaAccessMode, MQ_DENY_NONE)
        lngError = Err.Number
        strError = Err.Description
    On Error GoTo 0

    '-------------------------------------------------
    'Deal with any errors that may have occured...
    '-------------------------------------------------
    Select Case lngError
    Case 0  'No error
        Exit Function 'We are all set
    
    Case MQ_ERROR_QUEUE_NOT_FOUND 'Queue was misregistered (or more likely destroyed & recreated)
        
        'See if we can ask the user
        If (boolPromotUser = True) Then
            lngYesNo = MsgBox("The local information for '" & strQueueName & " is incorrect. Would you like to look for this queue on the network ('No' will abort)?", vbYesNo)
            
            'If they said no, abort
            If (lngYesNo = vbNo) Then
                Err.Raise qeNO_QUEUE, "OpenMessageQueue", "There is no registered queue for this user/machine for " & strQueueName
            End If
        End If
        
        '--------------------------------------------------
        'Attempt to open the queue
        '--------------------------------------------------
        'If an error occurs again then we should justlet it bubble up
        strQueueGUID = LookupGUIDAndPersistToRegisty(strQueueName)
        Set objQueueInfo = getQueueInfoForLogon(strQueueGUID)
        Set openMessageQueue = objQueueInfo.Open(qaAccessMode, MQ_DENY_NONE)
    
    Case Else 'Not an error we're trapping, bubble it up.
        Err.Raise lngError, , strError
    End Select
    
End Function



'--------------------------------------------------
'Create message queue
' [in] strQueueName     : Name of queue we want to open
' [in] strQueueLocation : Location of our queue ('MACHINENAME\PATH')
' [in] boolPromptUser   : Prompt the user for direction if we have problems creating the queue
' [in] boolWorldReadable: Should we make the queue readable by everyone or only by the person who created it...
'
' [retval] T/F : was a 'new' queue created
'
' This is a general function who's purpose is to create a new message queue
'
' [11/10/97, IvoSa] Created
' [12/6/97, IvoSa]  Made queue 'World readable'
'--------------------------------------------------
Public Function createMessageQueue(ByVal strQueueName As String, ByVal strQueueLocation As String, Optional ByVal boolPromptUser As Boolean = True, Optional ByVal boolWorldReadable As Boolean = True) As Boolean
Dim objQInfo As MSMQQueueInfo
Dim boolNewQueueCreated As Boolean
    boolNewQueueCreated = True 'assume true
    
    'UNDONE: Should add code to check if a Queue of this name already exists.
    
    'Attempt to create the queue
    Dim lngError As Long
    Dim strError As Long
    On Error Resume Next
        getQueueInfoForCreation(strQueueLocation, strQueueName).Create
        lngError = Err.Number
        strError = Err.Description
    On Error GoTo 0

    Dim lngYesNo As Long
    '-------------------------------------------------
    'Deal with any errors that may have occured...
    '-------------------------------------------------
    Select Case lngError
    Case 0  'No error
        GoTo exit_function '[11/19,97, IvoSa] We are all set
    
    Case MQ_ERROR_QUEUE_EXISTS 'Queue was misregistered (or more likely destroyed & recreated)
                
        'See if we can ask the user
        If (boolPromptUser = True) Then
            lngYesNo = MsgBox("The queue '" & strQueueName & "' exists.  Do you want to delete this and create a new queue? ", vbYesNo)
            
            'If they said no, abort
            If (lngYesNo = vbNo) Then
                'We have qa queue, they are satisfied, exit.
                boolNewQueueCreated = False
                GoTo exit_function
            End If
        Else
            'To be on the safe side, we must raise an error
            Err.Raise MQ_ERROR_QUEUE_EXISTS, "CreateMessageQueue", "The queue exists '" & strQueueName & "'"
        End If
        
        '--------------------------------------------------
        'Attempt to delete the queue
        '--------------------------------------------------
        getQueueInfoForDeletion(strQueueName).Delete
        
        '--------------------------------------------------
        'Attempt to create a new queue
        '--------------------------------------------------
        getQueueInfoForCreation(strQueueLocation, strQueueName).Create IsWorldReadable:=boolWorldReadable '[12/6/97, IvoSa] Public queue access...
        
        'If an error occurs again then we should justlet it bubble up
    
    Case Else 'Not an error we're trapping, bubble it up.
        Err.Raise lngError, , strError
    End Select

exit_function:
    createMessageQueue = boolNewQueueCreated
End Function

'--------------------------------------------------
'Delets a message queue
' [in] strQueueName : Name of queue we want to open
' [in] boolPromotUser : prompt the user if we need more information?
'
' This is a general function who's purpose is to delete a message queue
' for the user.
'
' [11/12/97, IvoSa] Created
'--------------------------------------------------
Public Sub deleteMessageQueue(ByVal strQueueName As String, Optional ByVal boolPromotUser As Boolean = True)
    Dim strQueueGUID As String
    
    '-------------------------------------------------------------------
    'Determine the GUID of the message queue
    '-------------------------------------------------------------------
    
    'Get the GUID that uniquely identifies our queue
    strQueueGUID = GetSetting(regAPPLICATION_NAME, strQueueName, regKEY_NAME, regNOT_FOUND)
    
    'Was there no value specified for our QUEUE?
    If (strQueueGUID = regNOT_FOUND) Then
        
        'See if we can ask the user
        If (boolPromotUser = True) Then
            Dim lngYesNo As VbMsgBoxResult
            lngYesNo = MsgBox("There is no locally registered queue for '" & strQueueName & ". Would you like to look for this queue on the network ('No' will abort)?", vbYesNo)
            
            'If they said no, abort
            If (lngYesNo = vbNo) Then
                Err.Raise qeNO_QUEUE, "deleteMessageQueue", "There is no registered queue for this user/machine for " & strQueueName
            End If
        End If
        
        strQueueGUID = LookupGUIDAndPersistToRegisty(strQueueName)
    End If
    
    '-------------------------------------------------------------------
    'Attempt to delete the message queue...
    '-------------------------------------------------------------------
    getQueueInfoForLogon(strQueueGUID).Delete
End Sub


'--------------------------------------------------
'Returns the lablels of all the messages in the queue
' [in] objQueue     : Message Queue
'
' [11/10/97, IvoSa] Created
'--------------------------------------------------
Public Function getLabelsOfMessagesInQueue(ByVal objQueue As MSMQQueue) As Collection
Dim objMessage As MSMQMessage
Dim colLables As Collection
    
    'Collection to store the labels in
    Set colLables = New Collection
    
    'Go to the first item in the queue
    objQueue.Reset
    
    'Itterate through all of the items and add the labels to the collection
    Set objMessage = objQueue.PeekCurrent(, , ONE_SECOND) 'ONE_SECOND needed (default delay is infinite)
    While Not (objMessage Is Nothing)
        colLables.Add objMessage.Label
        Set objMessage = objQueue.PeekNext(, , ONE_SECOND) 'ONE_SECOND needed (default delay is infinite)
    Wend
    
    'Return the collection of message lablels
    Set getLabelsOfMessagesInQueue = colLables
End Function


'-----------------------------------------------------
'Given as message label, retieve the message
' [in] objQueue     : Message Queue
' [in] strLabel     : Label we want
'
' [11/10/97, IvoSa] Created
'-----------------------------------------------------
Public Function getMessageGivenLabelAndQueue(ByVal objQueue As MSMQQueue, ByVal strLabel As String) As MSMQMessage
Dim objMessage As MSMQMessage
    
    objQueue.Reset 'Go to the first item
        
    'Itterate through all of the items and add the labels to the collection
    Set objMessage = objQueue.PeekCurrent(, , ONE_SECOND) 'ONE_SECOND needed (default delay is infinite)
    While Not (objMessage Is Nothing)
        If objMessage.Label = strLabel Then GoTo found_label
        Set objMessage = objQueue.PeekNext(, , ONE_SECOND) 'ONE_SECOND needed (default delay is infinite)
    Wend
        
    Set getMessageGivenLabelAndQueue = Nothing
    Exit Function

found_label:

    Set getMessageGivenLabelAndQueue = objQueue.ReceiveCurrent(receivetimeout:=FIVE_SECONDS)
End Function


'-----------------------------------------------------------
'Sends a message to a queue
' [in] objQueue
' [in] strMessageLabel
' [in] strMessage
' [in] mdMode:
'       MQMSG_DELIVERY_RECOVERABLE = Persist it to disk (survives reboot)
'       MQMSG_DELIVERY_EXPRESS     = Keep it in memory (does not survive reboot)
' [11/12/97, IvoSa] Created
'-----------------------------------------------------------
Sub sendMessageToQueue(ByVal objQueue As MSMQQueue, strMessageLabel As String, strMessage As String, Optional ByVal mdMode As MQMSGDELIVERY = MQMSG_DELIVERY_RECOVERABLE)
    Dim objMsg As MSMQMessage
    
    Set objMsg = New MSMQMessage
    objMsg.Label = strMessageLabel
    objMsg.Body = strMessage
    objMsg.Delivery = mdMode
    
    objMsg.Send objQueue
End Sub

'-----------------------------------------------------------
'Get a the labels of all of the messages in the queue & set an event to wait for new messages
' [in] objQueue : message queue
' [out] colMsgLabels : Labels of messages in queue
' [11/13/97, IvoSa] Created
'-----------------------------------------------------------
Public Function getEventFireOnNextMessageArrival(ByVal objQueue As MSMQQueue, Optional colMsgLabels As Collection) As MSMQEvent

    '-------------------------------------------------
    'Get the labels of all the existing messages & advance to cursor to the end of the queue
    '-------------------------------------------------
    Set colMsgLabels = getLabelsOfMessagesInQueue(objQueue)
    
    Dim objQEvent As MSMQEvent
    Set objQEvent = New MSMQEvent
    
    objQueue.EnableNotification objQEvent, MQMSG_CURRENT
    Set getEventFireOnNextMessageArrival = objQEvent
End Function

