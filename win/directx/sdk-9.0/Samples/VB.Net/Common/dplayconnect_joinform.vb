'-----------------------------------------------------------------------------
' File: DPlayConnect_JoinForm.vb
'
' Desc: Application class for the DirectPlay samples framework.
'
' Copyright (c) Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Collections
Imports System.Windows.Forms
Imports System.Threading
Imports System.Timers
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay
Imports System.Runtime.InteropServices

'/ <summary>
'/ This form will search for existing running samples and allow you to join them or will
'/ allow you to create your own session as a host
'/ </summary>
Public Class CreateJoinForm
    Inherits System.Windows.Forms.Form
    Private EnumExpireThreshold As Integer = 2000
    Private amJoining As Boolean = False
    _

    '/ <summary>
    '/ Hold our information of hosts we found
    '/ </summary>
    Private Structure FindHostsResponseInformation
        Public LastFoundTime As Integer
        Public ApplicationDesc As ApplicationDescription
        Public RoundTripLatencyMs As Integer
        Public sender As Address
        Public device As Address

        Public Overrides Function ToString() As String
            If ApplicationDesc.MaxPlayers > 0 Then
                Return ApplicationDesc.SessionName & " (" & ApplicationDesc.CurrentPlayers.ToString() & "/" & ApplicationDesc.MaxPlayers.ToString() & ") (" & RoundTripLatencyMs.ToString() & "ms)"
            Else
                Return ApplicationDesc.SessionName & " (" & ApplicationDesc.CurrentPlayers.ToString() & ") (" & RoundTripLatencyMs.ToString() & "ms)"
            End If
        End Function 'ToString
    End Structure 'FindHostsResponseInformation

    Private peer As peer
    Private connectionWizard As ConnectWizard
    Private deviceAddress As Address
    Private hostAddress As Address = Nothing
    Private isSearching As Boolean = False
    Private findHostHandle As Integer = 0
    Private createSessionForm As CreateSessionForm = Nothing
    Private resultCode As ResultCode
    Private foundHosts As New ArrayList()
    Private isConnected As Boolean = False
    Private connectEvent As ManualResetEvent = Nothing
    Private updateListTimer As System.Timers.Timer = Nothing
    Private connectTimer As System.Timers.Timer = Nothing

    Private WithEvents lstSession As System.Windows.Forms.ListBox
    Private label1 As System.Windows.Forms.Label
    Private btnCancel As System.Windows.Forms.Button
    Private WithEvents btnSearch As System.Windows.Forms.Button
    Private WithEvents btnJoin As System.Windows.Forms.Button
    Private WithEvents btnCreate As System.Windows.Forms.Button



    '/ <summary>
    '/ Constructor
    '/ </summary>
    Public Sub New(ByVal peerObject As Peer, ByVal addressObject As Address, ByVal connectionWizard As ConnectWizard)
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()
        Peer = peerObject
        Me.connectionWizard = connectionWizard
        Me.Text = connectionWizard.SampleName + " - " + Me.Text
        deviceAddress = addressObject

        'Set up the event handlers
        AddHandler Peer.FindHostResponse, AddressOf FindHostResponseMessage
        AddHandler Peer.ConnectComplete, AddressOf ConnectResult
        AddHandler Peer.AsyncOperationComplete, AddressOf CancelAsync

        'Set up our timer
        updateListTimer = New System.Timers.Timer(300) ' A 300 ms interval
        AddHandler updateListTimer.Elapsed, AddressOf Me.UpdateTimer
        updateListTimer.SynchronizingObject = Me
        updateListTimer.Start()
        'Set up our connect timer
        ConnectTimer = New System.Timers.Timer(100) ' A 100ms interval
        AddHandler connectTimer.Elapsed, AddressOf Me.ConnectTimerElapsed
        connectTimer.SynchronizingObject = Me
        ' Set up our connect event
        connectEvent = New ManualResetEvent(False)
    End Sub 'New


    '/ <summary>
    '/ An asynchronous operation was cancelled
    '/ </summary>
    Private Sub CancelAsync(ByVal sender As Object, ByVal e As AsyncOperationCompleteEventArgs)
        If e.Message.AsyncOperationHandle = findHostHandle Then
            findHostHandle = 0
            btnSearch.Text = "Start Search"
            isSearching = False
            btnCreate.Enabled = Not isSearching
        End If
    End Sub 'CancelAsync



    '/ <summary>
    '/ A host was found and responded to our query
    '/ </summary>
    Private Sub FindHostResponseMessage(ByVal sender As Object, ByVal dpMessage As FindHostResponseEventArgs)
        ' Now we need to add this to our list of available sessions
        SessionAdd(dpMessage.Message)
    End Sub 'FindHostResponseMessage



    '/ <summary>
    '/ Add this session to our list
    '/ </summary>
    '/ <param name="dpMessage"></param>
    Private Sub SessionAdd(ByVal dpMessage As FindHostsResponseMessage)
        Dim dpInfo As New FindHostsResponseInformation()

        dpInfo.ApplicationDesc = dpMessage.ApplicationDescription
        dpInfo.device = dpMessage.AddressDevice
        dpInfo.sender = dpMessage.AddressSender
        dpInfo.RoundTripLatencyMs = dpMessage.RoundTripLatencyMs
        dpInfo.LastFoundTime = Environment.TickCount

        ' Let's check the items first and see if this one already exists
        Dim isFound As Boolean = False

        SyncLock foundHosts
            Dim i As Integer
            For i = 0 To lstSession.Items.Count - 1
                If dpInfo.ApplicationDesc.GuidInstance.Equals(CType(lstSession.Items(i), FindHostsResponseInformation).ApplicationDesc.GuidInstance) Then
                    foundHosts(i) = dpInfo
                    lstSession.Items(i) = dpInfo
                    isFound = True
                End If
            Next i
            If Not isFound Then
                lstSession.Items.Add(dpInfo)
                foundHosts.Add(dpInfo)
            End If
        End SyncLock
    End Sub 'SessionAdd


    '/ <summary>
    '/ Clean up any resources being used.
    '/ </summary>
    Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
        MyBase.Dispose(Disposing)

        If isSearching Then
            If findHostHandle <> 0 Then
                peer.CancelAsyncOperation(findHostHandle)
            End If
            isSearching = Not isSearching
        End If
        If Not (connectTimer Is Nothing) Then
            connectTimer.Dispose()
        End If
        If Not (updateListTimer Is Nothing) Then
            updateListTimer.Dispose()
        End If
    End Sub 'Dispose

    '/ <summary>
    '/ Required method for Designer support - do not modify
    '/ the contents of this method with the code editor.
    '/ </summary>
    Private Sub InitializeComponent()
        Me.btnJoin = New System.Windows.Forms.Button()
        Me.lstSession = New System.Windows.Forms.ListBox()
        Me.label1 = New System.Windows.Forms.Label()
        Me.btnCreate = New System.Windows.Forms.Button()
        Me.btnSearch = New System.Windows.Forms.Button()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        ' 
        ' btnJoin
        ' 
        Me.btnJoin.Enabled = False
        Me.btnJoin.Location = New System.Drawing.Point(7, 227)
        Me.btnJoin.Name = "btnJoin"
        Me.btnJoin.Size = New System.Drawing.Size(74, 27)
        Me.btnJoin.TabIndex = 2
        Me.btnJoin.Text = "Join"
        ' 
        ' lstSession
        ' 
        Me.lstSession.Location = New System.Drawing.Point(5, 37)
        Me.lstSession.Name = "lstSession"
        Me.lstSession.Size = New System.Drawing.Size(400, 186)
        Me.lstSession.TabIndex = 1
        ' 
        ' label1
        ' 
        Me.label1.Location = New System.Drawing.Point(5, 13)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(312, 16)
        Me.label1.TabIndex = 0
        Me.label1.Text = "Select session to join, or click Create to start a new session."
        ' 
        ' btnCreate
        ' 
        Me.btnCreate.DialogResult = System.Windows.Forms.DialogResult.Yes
        Me.btnCreate.Location = New System.Drawing.Point(84, 227)
        Me.btnCreate.Name = "btnCreate"
        Me.btnCreate.Size = New System.Drawing.Size(74, 27)
        Me.btnCreate.TabIndex = 2
        Me.btnCreate.Text = "Create"
        ' 
        ' btnSearch
        ' 
        Me.btnSearch.Location = New System.Drawing.Point(319, 7)
        Me.btnSearch.Name = "btnSearch"
        Me.btnSearch.Size = New System.Drawing.Size(86, 27)
        Me.btnSearch.TabIndex = 2
        Me.btnSearch.Text = "Start Search"
        ' 
        ' btnCancel
        ' 
        Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnCancel.Location = New System.Drawing.Point(333, 227)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(74, 27)
        Me.btnCancel.TabIndex = 3
        Me.btnCancel.Text = "Cancel"
        ' 
        ' CreateJoinForm
        ' 
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.CancelButton = Me.btnCancel
        Me.ClientSize = New System.Drawing.Size(411, 264)
        Me.ControlBox = False
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnCreate, Me.btnJoin, Me.btnSearch, Me.btnCancel, Me.lstSession, Me.label1})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "CreateJoinForm"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Join or create a session"
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    '
    '/ <summary>
    '/ The selected session was changed.
    '/ </summary>
    Private Sub SelectedChange(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstSession.SelectedValueChanged
        If amJoining Then
            Return ' Do nothing if we are already joining a session
        End If
        btnJoin.Enabled = Not (lstSession.SelectedItem Is Nothing)
    End Sub 'SelectedChange



    '/ <summary>
    '/ We either want to start or stop searching
    '/ </summary>
    Private Sub btnSearch_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnSearch.Click
        If Not isSearching Then
            If Not hostAddress Is Nothing Then
                hostAddress.Dispose()
            End If

            hostAddress = New Address()
            hostAddress.ServiceProvider = deviceAddress.ServiceProvider

            ' See if we should prompt the user for the remote address
            If ConnectWizard.ProviderRequiresPort(hostAddress.ServiceProvider) Then
                Dim addressDialog As New AddressForm(connectionWizard.DefaultPort)
                addressDialog.ShowDialog(Me)

                ' If the user cancelled the address form, abort the search
                If Not addressDialog.DialogResult = DialogResult.OK Then
                    Exit Sub
                End If

                ' If a port was specified, add the component
                If Not addressDialog.Hostname = "" Then
                    hostAddress.AddComponent(Address.KeyHostname, addressDialog.Hostname)
                End If

                ' If a hostname was specified, add the component
                If addressDialog.Port > 0 Then
                    hostAddress.AddComponent(Address.KeyPort, addressDialog.Port)
                End If
            End If

        'Time to enum our hosts
        Dim desc As New ApplicationDescription()
        desc.GuidApplication = connectionWizard.ApplicationGuid

        ' If the user was not already asked for address information, DirectPlay
        ' should prompt with native UI
        Dim flags As FindHostsFlags = 0
        If Not ConnectWizard.ProviderRequiresPort(deviceAddress.ServiceProvider) Then
            flags = FindHostsFlags.OkToQueryForAddressing
        End If

            peer.FindHosts(desc, hostAddress, deviceAddress, Nothing, Timeout.Infinite, 0, Timeout.Infinite, flags, findHostHandle)
        btnSearch.Text = "Stop Search"
        Else
        If findHostHandle <> 0 Then
            peer.CancelAsyncOperation(findHostHandle)
        End If
        btnSearch.Text = "Start Search"
        End If
        isSearching = Not isSearching
        btnCreate.Enabled = Not isSearching
    End Sub 'btnSearch_Click



    '/ <summary>
    '/ A form was disposed
    '/ </summary>
    '/ <param name="sender"></param>
    '/ <param name="e"></param>
    Public Sub FormDisposed(ByVal sender As Object, ByVal e As EventArgs)
        If sender Is createSessionForm Then
            createSessionForm = Nothing
        End If
    End Sub 'FormDisposed



    '/ <summary>
    '/ We should create a new session.  Display a dialog allowing the user to set
    '/ certain options
    '/ </summary>
    Private Sub btnCreate_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnCreate.Click
        Me.DialogResult = DialogResult.None
        CType(sender, Button).DialogResult = DialogResult.None
        If createSessionForm Is Nothing Then
            createSessionForm = New CreateSessionForm(peer, deviceAddress, connectionWizard)
            AddHandler createSessionForm.Disposed, AddressOf Me.FormDisposed
        End If
        If createSessionForm.ShowDialog(Me) <> DialogResult.Cancel Then
            CType(sender, Button).DialogResult = DialogResult.Yes
            Me.DialogResult = DialogResult.Yes
        End If
    End Sub 'btnCreate_Click




    '/ <summary>
    '/ Determine if we need to expire any of our currently listed servers
    '/ </summary>
    Private Sub UpdateTimer(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs)
        SyncLock foundHosts
            Dim i As Integer
            For i = 0 To foundHosts.Count - 1
                ' First check to see if this session has expired
                If Environment.TickCount - CType(foundHosts(i), FindHostsResponseInformation).LastFoundTime > EnumExpireThreshold Then
                    foundHosts.RemoveAt(i)
                    Exit For
                End If
            Next i
            For i = 0 To lstSession.Items.Count - 1
                ' First check to see if this session has expired
                If Environment.TickCount - CType(lstSession.Items(i), FindHostsResponseInformation).LastFoundTime > EnumExpireThreshold Then
                    lstSession.Items.RemoveAt(i)
                    Exit For
                End If
            Next i
        End SyncLock
    End Sub 'UpdateTimer




    '/ <summary>
    '/ Wait for a connect to complete
    '/ </summary>
    Private Sub ConnectTimerElapsed(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs)
        If connectEvent.WaitOne(0, False) Then ' Wait for the Connect event to be fired
            If isConnected Then ' Are we connected?
                ' Get rid of the timer
                If Not (updateListTimer Is Nothing) Then
                    updateListTimer.Stop()
                End If
                If Not (connectTimer Is Nothing) Then
                    connectTimer.Stop()
                End If
                Me.DialogResult = DialogResult.OK
                Me.Close()
                Return
            Else
                MessageBox.Show(Me, "Failed to connect.  The error code was: " + ControlChars.Lf + resultCode.ToString(), "Failed to connect", MessageBoxButtons.OK, MessageBoxIcon.Information)
                ' Restart our timer
                updateListTimer.Start()
            End If
        End If
    End Sub 'ConnectTimer




    '/ <summary>
    '/ Fired when a connect complete message is received from DirectPlay.  Fire
    '/ our event to notify the sample we've connected
    '/ </summary>
    Private Sub ConnectResult(ByVal sender As Object, ByVal e As ConnectCompleteEventArgs)
        If e.Message.ResultCode = 0 Then
            isConnected = True
        Else
            isConnected = False
        End If
        resultCode = e.Message.ResultCode
        ' Notify the timer that we've connected.
        connectEvent.Set()
    End Sub 'ConnectResult




    '/ <summary>
    '/ Join an existing session
    '/ </summary>
    Private Sub btnJoin_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnJoin.Click, lstSession.DoubleClick
        Dim dpInfo As FindHostsResponseInformation
        If lstSession.SelectedItem Is Nothing Then
            MessageBox.Show(Me, "Please select a session before clicking join.", "No session", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        ' Turn off all buttons
        btnCreate.Enabled = False
        btnCancel.Enabled = False
        btnJoin.Enabled = False
        btnSearch.Enabled = False
        amJoining = True
        ' Stop our secondary timer
        updateListTimer.Stop()
        dpInfo = CType(lstSession.SelectedItem, FindHostsResponseInformation)
        connectionWizard.SetUserInfo()
        peer.Connect(dpInfo.ApplicationDesc, dpInfo.sender, dpInfo.device, Nothing, ConnectFlags.OkToQueryForAddressing)
        ' Now start our 'Connect' timer
        connectTimer.Start()
    End Sub 'btnJoin_Click
End Class 'CreateJoinForm