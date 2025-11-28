VERSION 5.00
Begin VB.Form frmProcessClaim 
   Caption         =   "Form1"
   ClientHeight    =   3435
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5055
   LinkTopic       =   "Form1"
   ScaleHeight     =   3435
   ScaleWidth      =   5055
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdSubmitClaim 
      Caption         =   "&Submit Claim"
      Height          =   255
      Left            =   2880
      TabIndex        =   9
      Top             =   3000
      Visible         =   0   'False
      Width           =   2055
   End
   Begin VB.CommandButton cmdNewClaim 
      Caption         =   "&New Claim"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   3000
      Visible         =   0   'False
      Width           =   2055
   End
   Begin VB.CommandButton cmdApprove 
      Caption         =   "&Approve"
      Height          =   255
      Left            =   2880
      TabIndex        =   16
      Top             =   3000
      Width           =   2055
   End
   Begin VB.TextBox txtName 
      BackColor       =   &H8000000F&
      Height          =   285
      Left            =   1800
      TabIndex        =   0
      Text            =   "John Doe"
      Top             =   120
      Width           =   3135
   End
   Begin VB.TextBox txtStreet 
      BackColor       =   &H8000000F&
      Height          =   285
      Left            =   1800
      TabIndex        =   1
      Text            =   "1600 Pennsilvania Ave"
      Top             =   600
      Width           =   3135
   End
   Begin VB.TextBox txtCity 
      BackColor       =   &H8000000F&
      Height          =   285
      Left            =   1800
      TabIndex        =   2
      Text            =   "Washington"
      Top             =   1080
      Width           =   3135
   End
   Begin VB.TextBox txtState 
      BackColor       =   &H8000000F&
      Height          =   285
      Left            =   1800
      TabIndex        =   3
      Text            =   "D.C."
      Top             =   1560
      Width           =   3135
   End
   Begin VB.OptionButton optAuto 
      Caption         =   "Auto"
      Enabled         =   0   'False
      Height          =   255
      Left            =   1800
      TabIndex        =   4
      Top             =   2040
      Value           =   -1  'True
      Width           =   975
   End
   Begin VB.OptionButton optHome 
      Caption         =   "Home"
      Enabled         =   0   'False
      Height          =   255
      Left            =   3000
      TabIndex        =   5
      Top             =   2040
      Width           =   975
   End
   Begin VB.OptionButton optBoat 
      Caption         =   "Boat"
      Enabled         =   0   'False
      Height          =   255
      Left            =   4200
      TabIndex        =   6
      Top             =   2040
      Width           =   975
   End
   Begin VB.TextBox txtAmountOfClaim 
      BackColor       =   &H8000000F&
      Height          =   285
      Left            =   1800
      TabIndex        =   8
      Text            =   "2000"
      Top             =   2520
      Width           =   3135
   End
   Begin VB.Label Label1 
      Caption         =   "Name:"
      Height          =   255
      Left            =   120
      TabIndex        =   15
      Top             =   120
      Width           =   1335
   End
   Begin VB.Label lblStreet 
      Caption         =   "Street:"
      Height          =   255
      Left            =   120
      TabIndex        =   14
      Top             =   600
      Width           =   1335
   End
   Begin VB.Label lblCity 
      Caption         =   "City:"
      Height          =   255
      Left            =   120
      TabIndex        =   13
      Top             =   1080
      Width           =   1335
   End
   Begin VB.Label lblState 
      Caption         =   "State:"
      Height          =   255
      Left            =   120
      TabIndex        =   12
      Top             =   1560
      Width           =   1335
   End
   Begin VB.Label lblClaim 
      Caption         =   "Claim:"
      Height          =   255
      Left            =   120
      TabIndex        =   11
      Top             =   2040
      Width           =   1335
   End
   Begin VB.Label lblClaimAmmount 
      Caption         =   "Amount of claim:"
      Height          =   255
      Left            =   120
      TabIndex        =   7
      Top             =   2520
      Width           =   1335
   End
End
Attribute VB_Name = "frmProcessClaim"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'--------------------------------------------------------
'Template form for the UI processing of workflow
'This template form is used by 3 different apps:
' CLAIM_ENTRY       : App that enters the claim (starts the workflow)
' CLAIM_PROCESSING  : App that processes claims (moves the workflow from one queue to another)
' ACCOUNTING_PROCESSING  : App that finishes the workflow (processes it in the final queue)
'
' Its worth noting that the core logic in each of these steps is the same:
' a) Get the info
' b) Process it
' c) Move it on
'
' [11/13/97, IvoSa] Created
'--------------------------------------------------------

#If CLAIM_PROCESSING Then
    Const FORM_CAPTION = "Insurance Claim Processing"
#ElseIf ACCOUNTING_PROCESSING Then
    Const FORM_CAPTION = "Accounting Claim Processing"
#ElseIf CLAIM_ENTRY Then
    Const FORM_CAPTION = "Claim Entry"
#End If

Private m_mqSendQueue As MSMQQueue 'Queue for claims department

'Colors we want
Const WINDOW_BACKGROUND = &H80000005
Const BUTTON_FACE = &H8000000F
'--------------------------------------------
'Set up the form with the claim data
' [in] objStrBag      : Object that contains our data
' [in] objMQNextQueue : Queue to place approved items into
'--------------------------------------------
Public Sub Initailzie(ByVal objStrBag As StringBag, ByVal objMQNextQueue As MSMQQueue)
    Set m_mqSendQueue = objMQNextQueue
    
    txtName = objStrBag.Retrieve(KEY_NAME)

'-------------------------------------------------
'Messages in the accounting queue do not store street, city, state, etc...
'-------------------------------------------------
#If CLAIM_PROCESSING Then
    txtStreet = objStrBag.Retrieve(KEY_STREET)
    txtCity = objStrBag.Retrieve(KEY_CITY)
    txtState = objStrBag.Retrieve(KEY_STATE)
        
    Dim strClaim As String
    strClaim = objStrBag.Retrieve(KEY_CLAIM)
    
    If (strClaim = CLAIM_AUTO) Then
        optAuto.Value = True
    ElseIf (strClaim = CLAIM_HOME) Then
        optHome.Value = True
    Else
        optBoat.Value = True
    End If
#End If
    txtAmountOfClaim = CStr(objStrBag.Retrieve(KEY_AMOUNT))
End Sub

Private Sub cmdApprove_Click()
    ApproveClaim
End Sub

'------------------------------------------------
'Claim has been approved, send it onto the next queue
' [11/13/97, IvoSa]
'------------------------------------------------
Sub ApproveClaim()
    'If there is no next queue, then exit
    If (m_mqSendQueue Is Nothing) Then
        MsgBox "Workflow Finished"
        Unload Me
        Exit Sub
    End If

    'Put together a property bag with the work flow & send it
    Dim objStringBag As StringBag
    Set objStringBag = New StringBag
    objStringBag.Add KEY_NAME, txtName
    objStringBag.Add KEY_AMOUNT, txtAmountOfClaim

    objStringBag.serilizeToQueue m_mqSendQueue, txtName

    MsgBox "Message Sent Finished"
    Unload Me
End Sub

Private Sub cmdNewClaim_Click()
    ClearClaimInfo
End Sub

'----------------------------------------------
'Submits a claim to our MQ
'----------------------------------------------
Private Sub cmdSubmitClaim_Click()
    SubmitClaim
    MsgBox "Claim sucessfully submitted!"
End Sub

'------------------------------------------------
'Setup the UI of the form
'------------------------------------------------
Private Sub Form_Load()
    Me.Caption = FORM_CAPTION
#If CLAIM_PROCESSING Then
    'Color
    txtName.BackColor = BUTTON_FACE
    txtStreet.BackColor = BUTTON_FACE
    txtCity.BackColor = BUTTON_FACE
    txtState.BackColor = BUTTON_FACE
    txtAmountOfClaim.BackColor = BUTTON_FACE
    
#ElseIf ACCOUNTING_PROCESSING Then
    'Visibility
    lblStreet.Visible = False
    lblCity.Visible = False
    lblState.Visible = False
    lblClaim.Visible = False
    
    optAuto.Visible = False
    optBoat.Visible = False
    optHome.Visible = False

    txtStreet.Visible = False
    txtCity.Visible = False
    txtState.Visible = False
    
    'Color
    txtAmountOfClaim.BackColor = BUTTON_FACE
#ElseIf CLAIM_ENTRY Then
    'Visible
    cmdApprove.Visible = False
    cmdNewClaim.Visible = True
    cmdSubmitClaim.Visible = True
    
    'Back color
    txtName.BackColor = WINDOW_BACKGROUND
    txtStreet.BackColor = WINDOW_BACKGROUND
    txtCity.BackColor = WINDOW_BACKGROUND
    txtState.BackColor = WINDOW_BACKGROUND
    txtAmountOfClaim.BackColor = WINDOW_BACKGROUND
    
    'Locking
    txtName.Locked = False
    txtStreet.Locked = False
    txtCity.Locked = False
    txtState.Locked = False
    txtAmountOfClaim.Locked = False
    
    'Enabled
    optAuto.Enabled = True
    optBoat.Enabled = True
    optHome.Enabled = True

    'Open up a submit queue..
    OpenMessageQueueForSubmit
#End If

End Sub

'-----------------------------
'Clears the claim info
'-----------------------------
Sub ClearClaimInfo()
    txtName = ""
    txtStreet = ""
    txtCity = ""
    txtState = ""
    
    optAuto.Value = True
    txtAmountOfClaim = ""
    
End Sub

'----------------------------------------------
'Submits a claim to our MQ
'----------------------------------------------
Private Sub SubmitClaim()

'Fill a property bag with the values we want to send...
Dim objStrBag As StringBag
    Set objStrBag = New StringBag
    objStrBag.Add KEY_NAME, Trim(txtName)
    objStrBag.Add KEY_STREET, Trim(txtStreet)
    objStrBag.Add KEY_CITY, Trim(txtCity)
    objStrBag.Add KEY_STATE, Trim(txtState)
    
    If (optAuto.Value) Then
        objStrBag.Add KEY_CLAIM, CLAIM_AUTO
    ElseIf (optHome.Value) Then
        objStrBag.Add KEY_CLAIM, CLAIM_HOME
    Else
        objStrBag.Add KEY_CLAIM, CLAIM_BOAT
    End If
    
    'Store this as currency
    objStrBag.Add KEY_AMOUNT, CCur(txtAmountOfClaim)
    
    '--------------------------------------------------
    'PERSIST OUR DATA COLLECTION TO A MESSAGE IN THE QUEUE
    '--------------------------------------------------
    objStrBag.serilizeToQueue m_mqSendQueue, objStrBag.Retrieve(KEY_NAME)
End Sub


'---------------------------------------------------------
'Opens a the 'Claims' message queue for us to submit messages to
' [11/13/97, IvoSa] Created
' [12/6/97, IvoSa] Added error checking
'---------------------------------------------------------
Sub OpenMessageQueueForSubmit()
Dim lngError As Long
Dim strError As String

    On Error Resume Next
        Set m_mqSendQueue = openMessageQueue(INSURANCE_CLAIMS_QUEUE_NAME, MQ_SEND_ACCESS, True)
        lngError = Err
        strError = Err.Description
    On Error GoTo 0

    If lngError Then
        MsgBox ERROR_TEXT_RUNADMINTOOL & "(specific error text reads: " & strError & ")"
        
        Stop 'Allow the app to be debugged here...
        End 'We should not continue running at this point
    End If
End Sub

