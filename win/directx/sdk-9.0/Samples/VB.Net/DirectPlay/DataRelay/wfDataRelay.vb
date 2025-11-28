Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Threading
Imports System.Timers
Imports System.Runtime.InteropServices
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay


Namespace DataRelaySample
    _
    Public Class DataRelay
        Inherits System.Windows.Forms.Form

        Delegate Sub PeerCloseCallback(ByVal code As ResultCode)
        ' This delegate will be called when the session terminated event is fired.
        Delegate Sub UpdateLogCallback(ByVal [text] As String)
        Delegate Sub DoEventsCallback()
        Private GamePacketId As Integer = 1
        Private DefaultPort As Integer = 2503
       _


        '/ <summary>
        '/ What type of data is this
        '/ </summary>
        Private Enum DataReceiveType
            Received ' We received this packet
            Sent ' This packet was sent successfully
            TimeOut ' This packet timed out
        End Enum 'DataReceiveType
       _
        '/ <summary>
        '/ This structure will be used by the updating thread for it's 
        '/ information about arriving packets (either sent by us, or 
        '/ received from another player)
        '/ </summary>
        Private Structure DataReceive
            Public Type As DataReceiveType 'What type of packet is this?
            Public PacketId As Integer ' The packetID
            Public DataSize As Integer ' The size of the data in the packet
            Public playerId As Integer ' The player ID of the person responsible for the packet
            Public Data As NetworkPacket ' The actual data in the packet
            Public Handle As GCHandle ' The GCHandle for the datapacket (Sent packets only)
        End Structure 'DataReceive
       _


        '/ <summary>
        '/ The data object we send to clients during our send phase
        '/ </summary>
        Private Structure DataObject
            Public PacketType As Integer
            Public PacketId As Integer ' The packetID of this packet
        End Structure 'DataObject
       _

        '/ <summary>
        '/ A local object containing the information about players we care about
        '/ </summary>
        Private Structure Players
            Public playerId As Integer ' The ID of this player
            Public Name As String
            ' The Name of this player
            Public Overrides Function ToString() As String ' Override the tostring so it will display the name of the player
                Return Name
            End Function 'ToString
        End Structure 'Players ' local variables for this app
        Public peerObject As Peer = Nothing ' The main DirectPlay object
        Public connectWizard As ConnectWizard = Nothing ' The wizard to create or join a DPlay Session
        ' {BA214178-AAE6-4ea6-84E0-65CE36F84479} Our local apps guid
        ' We will use the same GUID that the C++ and C# version of this sample uses
        ' so we can interop with it.
        Private Shared applicationGuid As New Guid("{BA214178-AAE6-4ea6-84E0-65CE36F84479}")
        Private updateDataThread As Thread = Nothing ' The thread that will update our UI based on avaiable packets
        Private sendDataThread As Thread = Nothing ' The thread we will use to send our data
        Private intervalChangedEvent As AutoResetEvent = Nothing ' Notify the main thread that the interval has changed.
        Private shutdownEvent As ManualResetEvent = Nothing ' The event that will be fired when we're ready to exit
        Private statsTimer As System.Timers.Timer = Nothing ' A timer used to update transfer stats
        Private sendEvent As AutoResetEvent = Nothing ' The event that will notify our update thread more data is ready.
        Private lastTime As Integer = 0 ' The last time we updated our transfer stats
        Private dataReceivedAmount As Integer = 0 ' Amount of data received since our last transfer stats update
        Private dataSentAmount As Integer = 0 ' Amount of data sent since our last transfer stats update
        Private currentPacketNumber As Integer = 0 ' What packet # are we on?
        Private sendingData As Boolean = False ' Are we sending any data now?
        Private availableData As New ArrayList() ' Packets we are ready to send to the update thread
        Private dataSize As Integer = 0
        Private target As Integer = 0
        Private timeout As Integer = 0
        Private sendRate As Integer = 1000

        Private groupBox1 As System.Windows.Forms.GroupBox
        Private label1 As System.Windows.Forms.Label '
        Private label2 As System.Windows.Forms.Label
        Private lblPlayer As System.Windows.Forms.Label
        Private lblPlayers As System.Windows.Forms.Label
        Private WithEvents btnSend As System.Windows.Forms.Button
        Private WithEvents btnExit As System.Windows.Forms.Button
        Private groupBox2 As System.Windows.Forms.GroupBox
        Private label3 As System.Windows.Forms.Label
        Private label4 As System.Windows.Forms.Label
        Private label5 As System.Windows.Forms.Label
        Private label6 As System.Windows.Forms.Label
        Private WithEvents cboTarget As System.Windows.Forms.ComboBox
        Private WithEvents cboSize As System.Windows.Forms.ComboBox
        Private WithEvents cboRate As System.Windows.Forms.ComboBox
        Private coalesceCheckBox As System.Windows.Forms.CheckBox
        Private groupBox3 As System.Windows.Forms.GroupBox
        Private lblReceiveRate As System.Windows.Forms.Label
        Private lblSendRate As System.Windows.Forms.Label
        Private label8 As System.Windows.Forms.Label
        Private label7 As System.Windows.Forms.Label
        Private groupBox4 As System.Windows.Forms.GroupBox
        Private label9 As System.Windows.Forms.Label
        Private cboInfotarget As System.Windows.Forms.ComboBox
        Private txtInfo As System.Windows.Forms.TextBox
        Private groupBox5 As System.Windows.Forms.GroupBox
        Private txtLog As System.Windows.Forms.TextBox
        Private WithEvents cboTimeout As System.Windows.Forms.ComboBox

        '/ <summary>
        '/ Constructor
        '/ </summary>
        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()
        End Sub 'New




        '/ <summary>
        '/ Shut down the sample
        '/ </summary>
        Public Sub PeerClose(ByVal code As ResultCode)
            ' Well, this session is being terminated, let the user know
            If code = ResultCode.HostTerminatedSession Then
                MessageBox.Show("The Host has terminated this session.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Else
                MessageBox.Show("The session has been lost.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information)
            End If
            ' The session was terminated, go ahead and shut down
            Me.Close()
        End Sub 'PeerClose




        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            ' Notify the threads we're shutting down
            If Not (shutdownEvent Is Nothing) Then
                shutdownEvent.Set()
            End If
            ' Turn off the timers
            If Not (statsTimer Is Nothing) Then
                statsTimer.Stop()
                statsTimer.Dispose()
                statsTimer = Nothing
            End If
            ' We lock the data here since it is shared across multiple threads.
            SyncLock availableData
                ' Clear out any available data
                availableData.Clear()
            End SyncLock

            ' The send thread should have seen the shutdown event and exited by now
            If Not (sendDataThread Is Nothing) Then
                sendDataThread.Join(0) ' Wait for the thread to end
                ' If the Join command didn't actually wait for the thread, it could be blocked.  Just abort it.
                If sendDataThread.IsAlive Then
                    sendDataThread.Abort()
                End If
                sendDataThread.Abort()
            End If

            ' The update thread should have already seen the shutdown notice and exited
            If Not (updateDataThread Is Nothing) Then ' Wait for this thread to end
                updateDataThread.Join(0) ' It should be done by now, but it could be blocked trying to set text on this thread
                ' If our Join command didn't actually wait for the thread, it must be blocked trying to set text on the UI thread (txtLog).
                ' In that case, we should just abort the thread
                If updateDataThread.IsAlive Then
                    updateDataThread.Abort()
                End If
                updateDataThread = Nothing
            End If
            ' Shut down DPlay
            If Not (peerObject Is Nothing) Then
                peerObject.Dispose()
            End If
            peerObject = Nothing
            MyBase.Dispose(disposing)
        End Sub 'Dispose




        '/ <summary>
        '/ Set the default values and hook up our events.  This will
        '/ also start the wizard
        '/ </summary>
        Public Sub Initialize()
            ' Populate the combos with default data
            PopulateCombos()

            peerObject = New Peer()
            ' First set up our event handlers (We only need events for the ones we care about)
            AddHandler peerObject.PlayerCreated, AddressOf Me.PlayerCreated
            AddHandler peerObject.PlayerDestroyed, AddressOf Me.PlayerDestroyed
            AddHandler peerObject.HostMigrated, AddressOf Me.HostMigrated
            AddHandler peerObject.Receive, AddressOf Me.DataReceived
            AddHandler peerObject.SendComplete, AddressOf Me.SendComplete
            AddHandler peerObject.SessionTerminated, AddressOf Me.SessionTerminated
            ' We do this before the wizard so we too receive the events
            ' Now we will create an additional thread.  
            updateDataThread = New Thread(New ThreadStart(AddressOf Me.DataAvailableThread))
            shutdownEvent = New ManualResetEvent(False)
            sendEvent = New AutoResetEvent(False)
            ' Start up our other threads
            updateDataThread.Start()
            connectWizard = New ConnectWizard(peerObject, applicationGuid, "DataRelay")
            connectWizard.DefaultPort = DefaultPort
            dataSize = 512
            timeout = 20
            If connectWizard.StartWizard() Then
                ' Great we've connected (or joined)..  Now we can start the sample
                ' Create a timer to update our stats
                statsTimer = New System.Timers.Timer(1000) ' 1000 ms interval
                statsTimer.AutoReset = True
                AddHandler statsTimer.Elapsed, AddressOf Me.StatsTimerElapsed
                statsTimer.SynchronizingObject = Me ' make sure the timer is sync'd with the form
                ' Start that timer
                statsTimer.Start()

                ' Create an event for when the rate has been change
                intervalChangedEvent = New AutoResetEvent(False)

                ' We should now create a thread to actually *send* the data
                sendDataThread = New Thread(New ThreadStart(AddressOf Me.DoSendData))
                sendDataThread.Start() ' Start our worker thread
                ' We will also create a timer that's job is to actually send the data out
                sendRate = Integer.Parse(cboRate.Text)
                ' Are we the host?
                If connectWizard.IsHost Then
                    Me.Text += " (HOST)"
                End If ' Set our name
                lblPlayer.Text = connectWizard.Username
                ' We obviously didn't want to start a session
            Else
                Me.Close()
            End If
        End Sub 'Initialize
        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.lblSendRate = New System.Windows.Forms.Label()
            Me.cboTarget = New System.Windows.Forms.ComboBox()
            Me.lblPlayer = New System.Windows.Forms.Label()
            Me.cboTimeout = New System.Windows.Forms.ComboBox()
            Me.cboInfotarget = New System.Windows.Forms.ComboBox()
            Me.btnSend = New System.Windows.Forms.Button()
            Me.txtInfo = New System.Windows.Forms.TextBox()
            Me.lblPlayers = New System.Windows.Forms.Label()
            Me.lblReceiveRate = New System.Windows.Forms.Label()
            Me.label8 = New System.Windows.Forms.Label()
            Me.label9 = New System.Windows.Forms.Label()
            Me.label4 = New System.Windows.Forms.Label()
            Me.label5 = New System.Windows.Forms.Label()
            Me.label6 = New System.Windows.Forms.Label()
            Me.label7 = New System.Windows.Forms.Label()
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.label3 = New System.Windows.Forms.Label()
            Me.cboSize = New System.Windows.Forms.ComboBox()
            Me.btnExit = New System.Windows.Forms.Button()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.groupBox2 = New System.Windows.Forms.GroupBox()
            Me.coalesceCheckBox = New System.Windows.Forms.CheckBox()
            Me.cboRate = New System.Windows.Forms.ComboBox()
            Me.groupBox3 = New System.Windows.Forms.GroupBox()
            Me.groupBox4 = New System.Windows.Forms.GroupBox()
            Me.groupBox5 = New System.Windows.Forms.GroupBox()
            Me.txtLog = New System.Windows.Forms.TextBox()
            Me.groupBox1.SuspendLayout()
            Me.groupBox2.SuspendLayout()
            Me.groupBox3.SuspendLayout()
            Me.groupBox4.SuspendLayout()
            Me.groupBox5.SuspendLayout()
            Me.SuspendLayout()
            ' 
            ' lblSendRate
            ' 
            Me.lblSendRate.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.lblSendRate.Location = New System.Drawing.Point(96, 17)
            Me.lblSendRate.Name = "lblSendRate"
            Me.lblSendRate.Size = New System.Drawing.Size(120, 16)
            Me.lblSendRate.TabIndex = 1
            Me.lblSendRate.Text = "0.0"
            Me.lblSendRate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' cboTarget
            ' 
            Me.cboTarget.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.cboTarget.DropDownWidth = 131
            Me.cboTarget.Location = New System.Drawing.Point(89, 16)
            Me.cboTarget.Name = "cboTarget"
            Me.cboTarget.Size = New System.Drawing.Size(131, 21)
            Me.cboTarget.TabIndex = 1
            ' 
            ' lblPlayer
            ' 
            Me.lblPlayer.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.lblPlayer.Location = New System.Drawing.Point(164, 16)
            Me.lblPlayer.Name = "lblPlayer"
            Me.lblPlayer.Size = New System.Drawing.Size(118, 16)
            Me.lblPlayer.TabIndex = 1
            Me.lblPlayer.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' cboTimeout
            ' 
            Me.cboTimeout.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.cboTimeout.DropDownWidth = 111
            Me.cboTimeout.Location = New System.Drawing.Point(89, 96)
            Me.cboTimeout.Name = "cboTimeout"
            Me.cboTimeout.Size = New System.Drawing.Size(131, 21)
            Me.cboTimeout.TabIndex = 1
            ' 
            ' cboInfotarget
            ' 
            Me.cboInfotarget.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.cboInfotarget.DropDownWidth = 131
            Me.cboInfotarget.Location = New System.Drawing.Point(89, 16)
            Me.cboInfotarget.Name = "cboInfotarget"
            Me.cboInfotarget.Size = New System.Drawing.Size(168, 21)
            Me.cboInfotarget.TabIndex = 1
            ' 
            ' btnSend
            ' 
            Me.btnSend.Location = New System.Drawing.Point(345, 25)
            Me.btnSend.Name = "btnSend"
            Me.btnSend.Size = New System.Drawing.Size(71, 20)
            Me.btnSend.TabIndex = 2
            Me.btnSend.Text = "Send"
            ' 
            ' txtInfo
            ' 
            Me.txtInfo.Location = New System.Drawing.Point(6, 41)
            Me.txtInfo.Multiline = True
            Me.txtInfo.Name = "txtInfo"
            Me.txtInfo.ReadOnly = True
            Me.txtInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
            Me.txtInfo.Size = New System.Drawing.Size(256, 168)
            Me.txtInfo.TabIndex = 2
            Me.txtInfo.Text = ""
            ' 
            ' lblPlayers
            ' 
            Me.lblPlayers.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.lblPlayers.Location = New System.Drawing.Point(164, 38)
            Me.lblPlayers.Name = "lblPlayers"
            Me.lblPlayers.Size = New System.Drawing.Size(52, 16)
            Me.lblPlayers.TabIndex = 1
            Me.lblPlayers.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' lblReceiveRate
            ' 
            Me.lblReceiveRate.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.lblReceiveRate.Location = New System.Drawing.Point(96, 36)
            Me.lblReceiveRate.Name = "lblReceiveRate"
            Me.lblReceiveRate.Size = New System.Drawing.Size(120, 16)
            Me.lblReceiveRate.TabIndex = 1
            Me.lblReceiveRate.Text = "0.0"
            Me.lblReceiveRate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label8
            ' 
            Me.label8.Location = New System.Drawing.Point(11, 34)
            Me.label8.Name = "label8"
            Me.label8.Size = New System.Drawing.Size(85, 15)
            Me.label8.TabIndex = 0
            Me.label8.Text = "Receive Rate:"
            Me.label8.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            ' 
            ' label9
            ' 
            Me.label9.Location = New System.Drawing.Point(11, 21)
            Me.label9.Name = "label9"
            Me.label9.Size = New System.Drawing.Size(75, 15)
            Me.label9.TabIndex = 0
            Me.label9.Text = "Info Target:"
            Me.label9.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            ' 
            ' label4
            ' 
            Me.label4.Location = New System.Drawing.Point(11, 45)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(75, 15)
            Me.label4.TabIndex = 0
            Me.label4.Text = "Size (bytes):"
            Me.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            ' 
            ' label5
            ' 
            Me.label5.Location = New System.Drawing.Point(11, 71)
            Me.label5.Name = "label5"
            Me.label5.Size = New System.Drawing.Size(75, 15)
            Me.label5.TabIndex = 0
            Me.label5.Text = "Rate (ms):"
            Me.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            ' 
            ' label6
            ' 
            Me.label6.Location = New System.Drawing.Point(11, 98)
            Me.label6.Name = "label6"
            Me.label6.Size = New System.Drawing.Size(75, 15)
            Me.label6.TabIndex = 0
            Me.label6.Text = "Timeout (ms):"
            Me.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            ' 
            ' label7
            ' 
            Me.label7.Location = New System.Drawing.Point(11, 16)
            Me.label7.Name = "label7"
            Me.label7.Size = New System.Drawing.Size(85, 15)
            Me.label7.TabIndex = 0
            Me.label7.Text = "Send Rate:"
            Me.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            ' 
            ' label1
            ' 
            Me.label1.Location = New System.Drawing.Point(11, 16)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(104, 15)
            Me.label1.TabIndex = 0
            Me.label1.Text = "Local Player Name: "
            ' 
            ' label2
            ' 
            Me.label2.Location = New System.Drawing.Point(11, 39)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(154, 15)
            Me.label2.TabIndex = 0
            Me.label2.Text = "Number of players in session: "
            ' 
            ' label3
            ' 
            Me.label3.Location = New System.Drawing.Point(11, 21)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(75, 15)
            Me.label3.TabIndex = 0
            Me.label3.Text = "Target:"
            Me.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            ' 
            ' cboSize
            ' 
            Me.cboSize.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.cboSize.DropDownWidth = 111
            Me.cboSize.Location = New System.Drawing.Point(89, 43)
            Me.cboSize.Name = "cboSize"
            Me.cboSize.Size = New System.Drawing.Size(131, 21)
            Me.cboSize.TabIndex = 1
            ' 
            ' btnExit
            ' 
            Me.btnExit.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.btnExit.Location = New System.Drawing.Point(421, 25)
            Me.btnExit.Name = "btnExit"
            Me.btnExit.Size = New System.Drawing.Size(71, 20)
            Me.btnExit.TabIndex = 2
            Me.btnExit.Text = "Exit"
            ' 
            ' groupBox1
            ' 
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnExit, Me.btnSend, Me.lblPlayers, Me.lblPlayer, Me.label2, Me.label1})
            Me.groupBox1.Location = New System.Drawing.Point(6, 4)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(497, 62)
            Me.groupBox1.TabIndex = 0
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Game Status"
            ' 
            ' groupBox2
            ' 
            Me.groupBox2.Controls.AddRange(New System.Windows.Forms.Control() {Me.coalesceCheckBox, Me.cboTimeout, Me.cboRate, Me.cboSize, Me.cboTarget, Me.label6, Me.label5, Me.label4, Me.label3})
            Me.groupBox2.Location = New System.Drawing.Point(6, 70)
            Me.groupBox2.Name = "groupBox2"
            Me.groupBox2.Size = New System.Drawing.Size(227, 153)
            Me.groupBox2.TabIndex = 1
            Me.groupBox2.TabStop = False
            Me.groupBox2.Text = "Send"
            ' 
            ' coalesceCheckBox
            ' 
            Me.coalesceCheckBox.Location = New System.Drawing.Point(18, 124)
            Me.coalesceCheckBox.Name = "coalesceCheckBox"
            Me.coalesceCheckBox.Size = New System.Drawing.Size(177, 24)
            Me.coalesceCheckBox.TabIndex = 2
            Me.coalesceCheckBox.Text = "Coalesce packets"
            ' 
            ' cboRate
            ' 
            Me.cboRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.cboRate.DropDownWidth = 111
            Me.cboRate.Location = New System.Drawing.Point(89, 69)
            Me.cboRate.Name = "cboRate"
            Me.cboRate.Size = New System.Drawing.Size(131, 21)
            Me.cboRate.TabIndex = 1
            ' 
            ' groupBox3
            ' 
            Me.groupBox3.Controls.AddRange(New System.Windows.Forms.Control() {Me.lblReceiveRate, Me.lblSendRate, Me.label8, Me.label7})
            Me.groupBox3.Location = New System.Drawing.Point(6, 228)
            Me.groupBox3.Name = "groupBox3"
            Me.groupBox3.Size = New System.Drawing.Size(227, 58)
            Me.groupBox3.TabIndex = 2
            Me.groupBox3.TabStop = False
            Me.groupBox3.Text = "Statistics"
            ' 
            ' groupBox4
            ' 
            Me.groupBox4.Controls.AddRange(New System.Windows.Forms.Control() {Me.txtInfo, Me.label9, Me.cboInfotarget})
            Me.groupBox4.Location = New System.Drawing.Point(237, 70)
            Me.groupBox4.Name = "groupBox4"
            Me.groupBox4.Size = New System.Drawing.Size(266, 216)
            Me.groupBox4.TabIndex = 3
            Me.groupBox4.TabStop = False
            Me.groupBox4.Text = "Connection Information"
            ' 
            ' groupBox5
            ' 
            Me.groupBox5.Controls.AddRange(New System.Windows.Forms.Control() {Me.txtLog})
            Me.groupBox5.Location = New System.Drawing.Point(6, 291)
            Me.groupBox5.Name = "groupBox5"
            Me.groupBox5.Size = New System.Drawing.Size(498, 130)
            Me.groupBox5.TabIndex = 4
            Me.groupBox5.TabStop = False
            Me.groupBox5.Text = "Log"
            ' 
            ' txtLog
            ' 
            Me.txtLog.Location = New System.Drawing.Point(9, 14)
            Me.txtLog.Multiline = True
            Me.txtLog.Name = "txtLog"
            Me.txtLog.ReadOnly = True
            Me.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
            Me.txtLog.Size = New System.Drawing.Size(479, 107)
            Me.txtLog.TabIndex = 2
            Me.txtLog.Text = ""
            ' 
            ' wfDataRelay
            ' 
            Me.AcceptButton = Me.btnSend
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.CancelButton = Me.btnExit
            Me.ClientSize = New System.Drawing.Size(509, 426)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.groupBox5, Me.groupBox4, Me.groupBox2, Me.groupBox1, Me.groupBox3})
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "wfDataRelay"
            Me.Text = "Data Relay"
            Me.groupBox1.ResumeLayout(False)
            Me.groupBox2.ResumeLayout(False)
            Me.groupBox3.ResumeLayout(False)
            Me.groupBox4.ResumeLayout(False)
            Me.groupBox5.ResumeLayout(False)
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '/ <summary>
        '/ A player was created
        '/ </summary>
        Private Sub PlayerCreated(ByVal sender As Object, ByVal dpMessage As PlayerCreatedEventArgs)
            ' We lock the data here since it is shared across multiple threads.
            SyncLock cboTarget
                ' Get the PlayerInformation and store it 
                Dim peerInfo As PlayerInformation = peerObject.GetPeerInformation(dpMessage.Message.PlayerID)
                If Not peerInfo.Local Then
                    ' This isn't me, add this player to our combos
                    Dim plNew As New Players()
                    plNew.playerId = dpMessage.Message.PlayerID
                    plNew.Name = peerInfo.Name
                    cboTarget.Items.Add(plNew)
                    cboInfotarget.Items.Add(plNew)
                End If
                ' Update our number of players and our button
                lblPlayers.Text = cboTarget.Items.Count.ToString()
                btnSend.Enabled = cboTarget.Items.Count > 1
            End SyncLock
        End Sub 'PlayerCreated



        '/ <summary>
        '/ A player was destroyed
        '/ </summary>
        Private Sub PlayerDestroyed(ByVal sender As Object, ByVal dpMessage As PlayerDestroyedEventArgs)
            ' We lock the data here since it is shared across multiple threads.
            SyncLock cboTarget
                ' Go through our list of players and remove anyone that is in our combos
                Dim item As Players
                For Each item In cboTarget.Items
                    If item.playerId = dpMessage.Message.PlayerID Then
                        cboTarget.Items.Remove(item)
                        Exit For
                    End If
                Next item
                For Each item In cboInfotarget.Items
                    If item.playerId = dpMessage.Message.PlayerID Then
                        cboInfotarget.Items.Remove(item)
                        Exit For
                    End If
                Next item
                ' Update our number of players and our button
                lblPlayers.Text = cboTarget.Items.Count.ToString()
                btnSend.Enabled = cboTarget.Items.Count > 1
                If cboTarget.SelectedIndex < 0 Then
                    cboTarget.SelectedIndex = 0
                End If
                If cboInfotarget.SelectedIndex < 0 Then
                    cboInfotarget.SelectedIndex = 0
                End If
                If cboTarget.Items.Count <= 1 Then
                    If sendingData Then
                        btnSend_Click(Nothing, Nothing)
                    End If
                End If
            End SyncLock
        End Sub 'PlayerDestroyed


        '/ <summary>
        '/ The host was migrated, see if you're the new host
        '/ </summary>
        Private Sub HostMigrated(ByVal sender As Object, ByVal dpMessage As HostMigratedEventArgs)
            Dim peerInfo As PlayerInformation = peerObject.GetPeerInformation(dpMessage.Message.NewHostID)
            If peerInfo.Local Then
                ' I'm the new host, update my UI
                Me.Text += " (HOST)"
            End If
        End Sub 'HostMigrated



        '/ <summary>
        '/ We received data from DirectPlay, process it
        '/ </summary>
        Private Sub DataReceived(ByVal sender As Object, ByVal dpMessage As ReceiveEventArgs)
            Dim drReceive As New DataReceive()
            Dim obj As DataObject = CType(dpMessage.Message.ReceiveData.Read(GetType(DataObject)), DataObject)
            dpMessage.Message.ReceiveData.Position = 0 ' Reset stream
            ' Update the amount of data we've received so our stats will update
            dataReceivedAmount += CInt(dpMessage.Message.ReceiveData.Length)
            ' Start filling out our packet so our update thread can use it.
            drReceive.Type = DataReceiveType.Received
            drReceive.playerId = dpMessage.Message.SenderID
            drReceive.DataSize = CInt(dpMessage.Message.ReceiveData.Length)
            drReceive.Data = dpMessage.Message.ReceiveData
            drReceive.PacketId = obj.PacketId
            ' Post it to the update thread
            AddAvailableData(drReceive)
        End Sub 'DataReceived




        '/ <summary>
        '/ Our send operation was complete
        '/ </summary>
        Private Sub SendComplete(ByVal sender As Object, ByVal dpMessage As SendCompleteEventArgs)
            Dim drReceive As DataReceive
            ' We lock the data here since it is shared across multiple threads.
            SyncLock Me
                ' Get our DataReceive structure from our context variable.  
                drReceive = CType(dpMessage.Message.UserContext, DataReceive)
                ' We no longer need the GCHandle or the data since it's been received by 
                ' the other members of our session.
                drReceive.Handle.Free()
                drReceive.Data = Nothing
                ' Did this packet time out?
                If dpMessage.Message.ResultCode = ResultCode.TimedOut Then
                    drReceive.Type = DataReceiveType.TimeOut
                End If
                ' Post this data so our update thread can handle it.
                AddAvailableData(drReceive)
            End SyncLock
        End Sub 'SendComplete




        '/ <summary>
        '/ The session was terminated
        '/ </summary>
        '/ <param name="sender"></param>
        '/ <param name="dpMessage"></param>
        Private Sub SessionTerminated(ByVal sender As Object, ByVal dpMessage As SessionTerminatedEventArgs)
            Dim param() As Object = {dpMessage.Message.ResultCode}
            ' This will post a message on the main thread to shut down our form
            Me.BeginInvoke(New PeerCloseCallback(AddressOf Me.PeerClose), param)
        End Sub 'SessionTerminated
        '/ <summary>
        '/ We are ready to exit the sample
        '/ </summary>
        Private Sub btnExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnExit.Click
            If sendingData Then ' We better stop that!
                btnSend_Click(Nothing, Nothing)
                'Abort our receiving thread
                updateDataThread.Abort()
            End If

            ' Now that's all out of the way, We're done, lets close this out
            Me.Close()
        End Sub 'btnExit_Click




        '/ <summary>
        '/ Start (or stop) sending data
        '/ </summary>
        Private Sub btnSend_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnSend.Click
            ' Update the text on the button
            If sendingData Then
                'Stop sending
                btnSend.Text = "Send"
            Else
                'Start sending
                btnSend.Text = "Stop"
            End If

            sendingData = Not sendingData
        End Sub 'btnSend_Click


        Private Sub cboRate_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cboRate.SelectedIndexChanged
            ' Update the interval whenever the rate is changed
            sendRate = Integer.Parse(cboRate.Text)
            ' Now fire our event to notify our thread we've updated the rate
            If Not (intervalChangedEvent Is Nothing) Then
                intervalChangedEvent.Set()
            End If
        End Sub 'cboRate_SelectedIndexChanged

        Private Sub cboSize_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cboSize.SelectedIndexChanged
            dataSize = CInt(cboSize.Items(cboSize.SelectedIndex))
        End Sub 'cboSize_SelectedIndexChanged


        Private Sub cboTarget_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cboTarget.SelectedIndexChanged
            target = CType(cboTarget.Items(cboTarget.SelectedIndex), Players).playerId
        End Sub 'cboTarget_SelectedIndexChanged


        Private Sub cboTimeout_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cboTimeout.SelectedIndexChanged
            timeout = CInt(cboTimeout.Items(cboTimeout.SelectedIndex))
        End Sub 'cboTimeout_SelectedIndexChanged

        Private Sub wfDataRelay_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
            ' Now that the form has been loaded we can initialize our application
            ' We do our initialization here because it needs to happen after the call
            ' to Application.Run (which starts the message pump).  Since you can begin
            ' receiving messages the instant you join the session (and thus update the log)
            ' We need to join the session after our form has been loaded.
            Me.Initialize()
        End Sub 'wfDataRelay_Load

        Private Sub StatsTimerElapsed(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs) '
            ' Update the current stats on screen
            Dim iCurrentTime As Integer = 0
            Dim fSeconds As Single = 0.0F
            Dim fDataIn As Single = 0.0F
            Dim fDataOut As Single = 0.0F

            If lastTime = 0 Then
                lastTime = CInt(Environment.TickCount)
            End If
            iCurrentTime = CInt(Environment.TickCount)

            If iCurrentTime - lastTime < 200 Then
                Return
            End If
            fSeconds = (iCurrentTime - lastTime) / 1000.0F
            fDataIn = CSng(dataReceivedAmount) / fSeconds
            fDataOut = CSng(dataSentAmount) / fSeconds
            lastTime = iCurrentTime
            dataReceivedAmount = 0
            dataSentAmount = 0

            lblSendRate.Text = fDataOut.ToString("f1") + " BPS"
            lblReceiveRate.Text = fDataIn.ToString("f1") + " BPS"

            If cboInfotarget.SelectedIndex > 0 Then
                Dim plSelected As Players = CType(cboInfotarget.Items(cboInfotarget.SelectedIndex), Players)
                Dim playerId As Integer = plSelected.playerId
                Dim dpnInfo As ConnectionInformation = peerObject.GetConnectionInformation(playerId)
                Dim iNumMessageHigh As Integer = 0
                Dim iNumByteHigh As Integer = 0
                Dim iNumMessageLow As Integer = 0
                Dim iNumByteLow As Integer = 0
                Dim iNumMessageNormal As Integer = 0
                Dim iNumByteNormal As Integer = 0
                Dim iDrops As Integer = (dpnInfo.PacketsDropped + dpnInfo.PacketsRetried) * 10000
                Dim fDropRate As Integer = iDrops / 100
                Dim iSends As Integer = dpnInfo.PacketsSentGuaranteed + dpnInfo.PacketsSentNonGuaranteed
                peerObject.GetSendQueueInformation(playerId, iNumMessageHigh, iNumByteHigh, GetSendQueueInformationFlags.PriorityHigh)
                peerObject.GetSendQueueInformation(playerId, iNumMessageLow, iNumByteLow, GetSendQueueInformationFlags.PriorityLow)
                peerObject.GetSendQueueInformation(playerId, iNumMessageNormal, iNumByteNormal, GetSendQueueInformationFlags.PriorityNormal)

                Dim sText As String = Nothing
                sText = "Send Queue Messages High Priority=" + iNumMessageHigh.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Send Queue Bytes High Priority=" + iNumByteHigh.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Send Queue Messages Normal Priority=" + iNumMessageNormal.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Send Queue Bytes Normal Priority=" + iNumByteNormal.ToString() + ControlChars.Cr + ControlChars.Lf

                sText = sText + "Send Queue Messages Low Priority=" + iNumMessageLow.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Send Queue Bytes Low Priority=" + iNumByteLow.ToString() + ControlChars.Cr + ControlChars.Lf

                sText = sText + "Round Trip Latency MS=" + dpnInfo.RoundTripLatencyMs.ToString() + " ms" + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Throughput BPS=" + dpnInfo.ThroughputBps.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Peak Throughput BPS=" + dpnInfo.PeakThroughputBps.ToString() + ControlChars.Cr + ControlChars.Lf

                sText = sText + "Bytes Sent Guaranteed=" + dpnInfo.BytesSentGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Packets Sent Guaranteed=" + dpnInfo.PacketsSentGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Bytes Sent Non-Guaranteed=" + dpnInfo.BytesSentNonGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Packets Sent Non-Guaranteed=" + dpnInfo.PacketsSentNonGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf

                sText = sText + "Bytes Retried Guaranteed=" + dpnInfo.BytesRetried.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Packets Retried Guaranteed=" + dpnInfo.PacketsRetried.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Bytes Dropped Non-Guaranteed=" + dpnInfo.BytesDropped.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Packets Dropped Non-Guaranteed=" + dpnInfo.PacketsDropped.ToString() + ControlChars.Cr + ControlChars.Lf

                sText = sText + "Messages Transmitted High Priority=" + dpnInfo.MessagesTransmittedHighPriority.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Messages Timed Out High Priority=" + dpnInfo.MessagesTimedOutHighPriority.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Messages Transmitted Normal Priority=" + dpnInfo.MessagesTransmittedNormalPriority.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Messages Timed Out Normal Priority=" + dpnInfo.MessagesTimedOutNormalPriority.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Messages Transmitted Low Priority=" + dpnInfo.MessagesTransmittedLowPriority.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Messages Timed Out Low Priority=" + dpnInfo.MessagesTimedOutLowPriority.ToString() + ControlChars.Cr + ControlChars.Lf

                sText = sText + "Bytes Received Guaranteed=" + dpnInfo.BytesReceivedGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Packets Received Guaranteed=" + dpnInfo.PacketsReceivedGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Bytes Received Non-Guaranteed=" + dpnInfo.BytesReceivedNonGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Packets Received Non-Guaranteed=" + dpnInfo.PacketsReceivedNonGuaranteed.ToString() + ControlChars.Cr + ControlChars.Lf
                sText = sText + "Messages Received=" + dpnInfo.MessagesReceived.ToString() + ControlChars.Cr + ControlChars.Lf

                sText = sText + "Loss Rate=" + fDropRate.ToString() + ControlChars.Cr + ControlChars.Lf
                txtInfo.Text = sText
            Else
                txtInfo.Text = Nothing
            End If
        End Sub 'StatsTimer

        Private Sub DataAvailableThread() '
            Dim whEvent(1) As WaitHandle
            whEvent(0) = sendEvent
            whEvent(1) = shutdownEvent
            Dim index As Integer = 0
            While True
                index = WaitHandle.WaitAny(whEvent)
                If index = 1 Then ' Our shutdown event has fired
                    Return
                    ' Our event saying we've received data has fired
                Else
                    ' Update the received data
                    Dim drReceive As New DataReceive()
                    While GetAvailableData(drReceive)
                        Dim sText As String = Nothing
                        If drReceive.Type = DataReceiveType.Sent Then
                            sText = "Sent packet # " + drReceive.PacketId.ToString() + " to "
                        End If
                        If drReceive.Type = DataReceiveType.TimeOut Then
                            sText = "Packet # " + drReceive.PacketId.ToString() + " timed out on the way to "
                        End If
                        If drReceive.Type = DataReceiveType.Received Then
                            Dim dData As DataObject = CType(drReceive.Data.Read(GetType(DataObject)), DataObject)
                            If dData.PacketType = GamePacketId Then
                                sText = "Received packet # " + dData.PacketId.ToString() + " from "
                            End If
                            ' We're done with the buffer now
                            drReceive.Data.Dispose()
                        End If
                        If drReceive.playerId = 0 Then
                            sText += "all players"
                        Else
                            sText += GetPlayerName(drReceive.playerId)
                        End If
                        sText += " (" + drReceive.DataSize.ToString() + " bytes)" + ControlChars.Cr + ControlChars.Lf

                        ' Check for a shutdown before updating our log
                        If shutdownEvent.WaitOne(0, False) Then
                            Return
                        End If
                        Dim newParams As Object() = {sText}
                        txtLog.BeginInvoke(New UpdateLogCallback(AddressOf Me.UpdateLog), newParams)
                    End While
                End If
            End While
        End Sub 'DataAvailableThread


        Private Sub DoSendData()
            Dim whEvent(1) As WaitHandle
            whEvent(0) = intervalChangedEvent
            whEvent(1) = shutdownEvent
            Dim index As Integer = 0
            While True
                ' Wait for an event.  We are waiting for either the shutdown event to be
                ' fired letting us know to shut down, or we are waiting for an event to 
                ' signal the rate has been changed.  Default the timeout to the current 
                ' send rate.
                index = WaitHandle.WaitAny(whEvent, IIf(sendRate = 0, 15, sendRate), False)
                If index = 1 Then ' Our shutdown event has fired
                    Exit While
                    ' Our event saying we've received data has fired
                Else
                    If sendingData Then
                        DirectXException.IgnoreExceptions()
                        'Make sure the UI is responsive
                        Me.Invoke(New DoEventsCallback(AddressOf Me.ProcessMessages))
                        Dim PacketId As Integer = 0
                        ' Time to send the data
                        Dim sendData As NetworkPacket = GetDataPacket(dataSize, PacketId)
                        dataSentAmount += CInt(dataSize)
                        Dim drObj As DataReceive
                        ' We lock the data here since it is shared across multiple threads.
                        drObj = AddSendData(PacketId, target, dataSize, sendData.GetData())
                        ' We can use the NoCopy flag because our 'data' has been 'pinned' by the GCHandle.
                        Dim flags As SendFlags = SendFlags.NoLoopback Or SendFlags.NoCopy
                        ' Check for packet coalescence option. If enabled, DirectPlay will
                        ' attempt to send small outgoing packets as a group
                        If coalesceCheckBox.Checked Then
                            flags = flags Or SendFlags.Coalesce
                        End If
                        peerObject.SendTo(target, drObj.Handle, dataSize, timeout, flags, drObj)
                        DirectXException.EnableExceptions()
                    End If
                End If
            End While
        End Sub 'DoSendData


        ' Make sure the UI is responsive on the main thread
        Private Sub ProcessMessages()
            Application.DoEvents()
        End Sub

        '/ <summary>
        '/ Update our log
        '/ </summary>
        '/ <param name="text">The text we want to append to the log</param>
        Private Sub UpdateLog(ByVal [text] As String)
            If txtLog.Text.Length > txtLog.MaxLength * 0.95 Then
                txtLog.Text = txtLog.Text.Remove(0, CInt(txtLog.MaxLength / 2))
            End If
            txtLog.Text += [text]
            txtLog.SelectionStart = txtLog.Text.Length
            txtLog.ScrollToCaret()
        End Sub 'UpdateLog




        '/ <summary>
        '/ Fill the combo boxes with the default data
        '/ </summary>
        Private Sub PopulateCombos()
            Dim pPlayer As New Players()
            pPlayer.playerId = 0
            pPlayer.Name = "Everyone"
            cboTarget.Items.Add(pPlayer)
            cboTarget.SelectedIndex = 0 ' Everyone
            cboRate.Items.Add(1000)
            cboRate.Items.Add(500)
            cboRate.Items.Add(250)
            cboRate.Items.Add(100)
            cboRate.Items.Add(50)
            cboRate.Items.Add(0)
            cboRate.SelectedIndex = 0 ' 1000 msg
            cboSize.Items.Add(512)
            cboSize.Items.Add(256)
            cboSize.Items.Add(128)
            cboSize.Items.Add(64)
            cboSize.Items.Add(32)
            cboSize.Items.Add(16)
            cboSize.SelectedIndex = 0 '512 bytes
            cboTimeout.Items.Add(5)
            cboTimeout.Items.Add(10)
            cboTimeout.Items.Add(20)
            cboTimeout.Items.Add(50)
            cboTimeout.Items.Add(100)
            cboTimeout.Items.Add(250)
            cboTimeout.Items.Add(500)
            cboTimeout.SelectedIndex = 2 ' 20 ms
            pPlayer = New Players()
            pPlayer.playerId = 0
            pPlayer.Name = "None"
            cboInfotarget.Items.Add(pPlayer)
            cboInfotarget.SelectedIndex = 0 ' None
        End Sub 'PopulateCombos



        '/ <summary>
        '/ Get the next data packet of the correct size
        '/ </summary>
        Private Function GetDataPacket(ByVal size As Integer, ByRef PacketId As Integer) As NetworkPacket
            Dim Packet As New DataObject()
            Dim ret As New NetworkPacket()

            currentPacketNumber += 1
            Packet.PacketId = currentPacketNumber
            Packet.PacketType = GamePacketId
            ret.Write(Packet)
            PacketId = Packet.PacketId
            Return ret
        End Function 'GetDataPacket




        '/ <summary>
        '/ Get the players name from the id
        '/ </summary>
        Private Function GetPlayerName(ByVal playerId As Integer) As String
            Dim item As Players
            For Each item In cboTarget.Items
                If item.playerId = playerId Then
                    Return item.Name
                End If
            Next item
            Return Nothing
        End Function 'GetPlayerName




        '/ <summary>
        '/ Add data to be processed by our worker thread
        '/ </summary>
        '/ <param name="PacketId"></param>
        '/ <param name="playerId"></param>
        '/ <param name="size"></param>
        '/ <param name="sendData"></param>
        '/ <returns></returns>
        Private Function AddSendData(ByVal PacketId As Integer, ByVal playerId As Integer, ByVal size As Integer, ByVal sendData() As Byte) As DataReceive
            Dim drObj As New DataReceive()
            drObj.PacketId = PacketId
            drObj.playerId = playerId
            drObj.Type = DataReceiveType.Sent
            drObj.DataSize = size
            ' Allocate a 'pinned' handle to this buffer so the garbage collector won't try to move it.
            drObj.Handle = GCHandle.Alloc(sendData, GCHandleType.Pinned)
            Return drObj
        End Function 'AddSendData




        '/ <summary>
        '/ Add the data to our available list
        '/ </summary>
        '/ <param name="Data"></param>
        Private Sub AddAvailableData(ByVal Data As DataReceive)
            ' We lock the data here since it is shared across multiple threads.
            SyncLock availableData ' Lock this buffer so no one else has access to it..
                availableData.Add(Data)
            End SyncLock
            ' Notify our update thread that new data is available
            sendEvent.Set()
        End Sub 'AddAvailableData




        '/ <summary>
        '/ Get the next available data
        '/ </summary>
        Private Function GetAvailableData(ByRef drObj As DataReceive) As Boolean
            ' We lock the data here since it is shared across multiple threads.
            SyncLock availableData ' Lock this buffer so no one else has access to it..
                If availableData.Count > 0 Then
                    drObj = CType(availableData(0), DataReceive)
                    availableData.Remove(drObj)
                    Return True
                End If
                Return False
            End SyncLock
        End Function 'GetAvailableData


        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Dim wfData As New DataRelay()
            Try
                Application.Run(wfData)
            Finally
                wfData.Dispose()
            End Try
        End Sub 'Main
    End Class 'DataRelay 
End Namespace 'DataRelaySample