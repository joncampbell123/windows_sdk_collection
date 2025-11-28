VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "CoffeeWatch2"
   ClientHeight    =   3765
   ClientLeft      =   1800
   ClientTop       =   1500
   ClientWidth     =   4365
   LinkTopic       =   "Form1"
   ScaleHeight     =   3765
   ScaleWidth      =   4365
   Begin VB.CommandButton cmdMT 
      Caption         =   "&Multithreading Demo"
      Height          =   375
      Left            =   600
      TabIndex        =   4
      Top             =   3240
      Width           =   3255
   End
   Begin VB.ListBox lstCallBacks 
      Height          =   2235
      Left            =   2280
      TabIndex        =   3
      Top             =   840
      Width           =   1935
   End
   Begin VB.CommandButton cmdCallBacks 
      Caption         =   "Start Receiving &Call-Backs"
      Height          =   615
      Left            =   2280
      TabIndex        =   1
      Top             =   120
      Width           =   1935
   End
   Begin VB.CommandButton cmdEvents 
      Caption         =   "Start Receiving CoffeeReady &Events"
      Height          =   615
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1935
   End
   Begin VB.ListBox lstEvents 
      Height          =   2235
      Left            =   120
      TabIndex        =   2
      Top             =   840
      Width           =   1935
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
' Module-level storage for the shared
'   CoffeeMonitor object.  The variable
'   is declared WithEvents so that the
'   CoffeeMonitor's events can be
'   handled.
Private WithEvents mwcmnEvents As CoffeeMonitor
Attribute mwcmnEvents.VB_VarHelpID = -1
' Connector object used to obtain a
'   reference to the shared CoffeeMonitor.
Private mcctEvents As Connector

' Module-level storage for a reference
'   to the CoffeeMonitor2 object used in
'   the Call-Back Method demo.
Private mcm2CallBacks As CoffeeMonitor2
' Storage for a reference to the call-back
'   object.
Private mNotifyMe As New NotifyMe

' Command button used to turn event reception
'   on and off.
Private Sub cmdEvents_Click()
    Static blnInUse As Boolean
    
    If blnInUse Then
        ' Setting a WithEvents variable to Nothing
        '   disconnects the object from its event
        '   procedures, so that no more events are
        '   received.
        Set mwcmnEvents = Nothing
        '
        ' Dispose of the Connector object.
        Set mcctEvents = Nothing
        cmdEvents.Caption = "Start Receiving CoffeeReady Events"
    Else
        Set mcctEvents = New Connector
        '
        ' The Connector object supplies a reference
        '   to the shared CoffeeMonitor.  When the
        '   reference is place in the WithEvents
        '   variable, the object is connected to its
        '   event procedure, so that CoffeeWatch can
        '   begin receiving events.
        Set mwcmnEvents = mcctEvents.CoffeeMonitor
        cmdEvents.Caption = "STOP Receiving CoffeeReady Events"
    End If
    blnInUse = True Xor blnInUse
End Sub

' Command button used to turn call-backs on
'   and off.
Private Sub cmdCallBacks_Click()
    Static blnInUse As Boolean
    Dim ct2 As New Connector2
    
    If blnInUse Then
        ' Tell CoffeeMonitor2 that call-backs
        '   are no longer desired.
        Call mcm2CallBacks.CeaseCallBacks(mNotifyMe)
        '
        ' Release the shared copy of CoffeeMonitor2.
        Set mcm2CallBacks = Nothing
        cmdCallBacks.Caption = "Start Receiving Call-Backs"
    Else
        ' Obtain a Connector2 object, and use it to
        '   get a reference to the shared copy of
        '   CoffeeMonitor2.
        Set ct2 = New Connector2
        Set mcm2CallBacks = ct2.CoffeeMonitor2
        '
        ' Tell CoffeeMonitor2 to begin making calls
        '   to the NotifyMe object (the object is
        '   implicitly created here, because the
        '   variable is declared As New).
        Call mcm2CallBacks.TellMeReady(mNotifyMe)
        cmdCallBacks.Caption = "STOP Receiving Call-Backs"
    End If
    blnInUse = True Xor blnInUse
End Sub

Private Sub cmdMT_Click()
    ' When switching to the multithreading
    '   demo, the call-backs and events should
    '   be disabled.  Ask the user if its okay
    '   to do that.  If the answer is vbNo,
    '   go ahead and start the multithreading
    '   demo anyway.
    If (Not mwcmnEvents Is Nothing) Or _
            (Not mcm2CallBacks Is Nothing) Then
        Select Case MsgBox("Event and Call-Back notifications should be shut down before beginning the processor-intensive threading demo.  Shut them down now?", _
                vbYesNoCancel, "Start Multithreading Demo")
            Case vbYes
                ' This doesn't unload the form,
                '   it just stops the demos.
                Call Form_Unload(False)
            Case vbCancel
                Exit Sub
        End Select
    End If
    cmdMT.Enabled = False
    frmThread.Show vbModeless
End Sub

' Shut down any running demos.
'
Private Sub Form_Unload(Cancel As Integer)
    If Not mwcmnEvents Is Nothing Then
        Call cmdEvents_Click
    End If
    If Not mcm2CallBacks Is Nothing Then
        Call cmdCallBacks_Click
    End If
End Sub

' When the CoffeeMonitor object sends a
'   CoffeeReady event, add it to the list
'   box.  If there are more than ten items
'   in the list box, delete the oldest.
'
Private Sub mwcmnEvents_CoffeeReady()
    With lstEvents
        .AddItem Format$(Now, "ddd hh:mm:ss"), 0
        If .ListCount > 10 Then .RemoveItem 10
    End With
End Sub

