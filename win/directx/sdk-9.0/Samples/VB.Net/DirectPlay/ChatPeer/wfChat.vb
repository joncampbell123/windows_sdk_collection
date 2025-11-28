'----------------------------------------------------------------------------
' File: ChatPeer.vb
'
' Desc: The main game file for the ChatPeer sample.  It connects 
'       players together with dialog boxes to prompt users on the 
'       connection settings to join or create a session. After the user 
'       connects to a session, the sample displays the chat dialog. 
' 
'       After a new game has started the sample begins a very simplistic 
'       chat session where users can send text to each other.
'      
'       This sample Interops with the C++ and C# version of the 
'       sample as well.
'
' Copyright (c) 2000-2002 Microsoft Corp. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay


Namespace ChatPeerSample
    _
    '/ <summary>
    '/ Summary description for ChatPeer.
    '/ </summary>
    Public Class ChatPeer
        Inherits System.Windows.Forms.Form

        Delegate Sub PeerCloseCallback() ' This delegate will be called when the session terminated event is fired.
        Private groupBox1 As System.Windows.Forms.GroupBox
        Private label1 As System.Windows.Forms.Label
        Private lblUsers As System.Windows.Forms.Label
        Private WithEvents btnExit As System.Windows.Forms.Button
        Private txtChat As System.Windows.Forms.TextBox
        Private WithEvents btnSend As System.Windows.Forms.Button
        Private WithEvents txtSend As System.Windows.Forms.TextBox
       _


        '/ <summary>
        '/ Our players structure
        '/ </summary>
        Public Structure Players
            Public playerId As Integer
            Public Name As String

            Public Sub New(ByVal id As Integer, ByVal n As String)
                PlayerID = id
                Name = n
            End Sub 'New
        End Structure 'Players 
        Private MaxChatStringLength As Integer = 508
        Private ChatMessageId As Byte = 1
        Private defaultPort As Integer = 2502

        ' Local variables for this app
        Public peerObject As Peer = Nothing ' Main DPlay object
        Private connectWizard As ConnectWizard = Nothing ' The wizard to create/join a DPlay Session
        Private playerList As New ArrayList()
        Private localPlayerId As Integer

        ' This GUID allows DirectPlay to find other instances of the same game on
        ' the network.  So it must be unique for every game, and the same for 
        ' every instance of that game.  // {876A3036-FFD7-46bc-9209-B42F617B9BE7}
        ' However, we are using the same guid the C++ and C# version of the 
        ' samples use so they can all communicate together.
        Public localApplicationGuid As New Guid("{876A3036-FFD7-46bc-9209-B42F617B9BE7}")



        '/ <summary>
        '/ Constuctor
        '/ </summary>
        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            peerObject = New Peer()
            ' First set up our event handlers (We only need events for the ones we care about)
            AddHandler peerObject.PlayerCreated, AddressOf Me.PlayerCreated
            AddHandler peerObject.PlayerDestroyed, AddressOf Me.PlayerDestroyed
            AddHandler peerObject.HostMigrated, AddressOf Me.HostMigrated
            AddHandler peerObject.Receive, AddressOf Me.DataReceived
            AddHandler peerObject.SessionTerminated, AddressOf Me.SessionTerminated
            connectWizard = New ConnectWizard(peerObject, localApplicationGuid, "Chat Peer")
            connectWizard.DefaultPort = defaultPort
            If ConnectWizard.StartWizard() Then
                ' Great we've connected (or joined)..  Now we can start the sample
                ' Are we the host?
                If ConnectWizard.IsHost Then
                    Me.Text += " (HOST)"
                End If
                ' We obviously didn't want to start a session
            Else
                Me.Dispose()
            End If
        End Sub 'New

        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            Me.Hide()
            MyBase.Dispose(disposing)

            ' Cleanup DPlay
            If Not (peerObject Is Nothing) Then
                peerObject.Dispose()
            End If
            peerObject = Nothing

            Application.Exit()
        End Sub 'Dispose


        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.txtSend = New System.Windows.Forms.TextBox()
            Me.btnSend = New System.Windows.Forms.Button()
            Me.txtChat = New System.Windows.Forms.TextBox()
            Me.btnExit = New System.Windows.Forms.Button()
            Me.lblUsers = New System.Windows.Forms.Label()
            Me.label1 = New System.Windows.Forms.Label()
            Me.groupBox1.SuspendLayout()
            Me.SuspendLayout()
            '
            'groupBox1
            '
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.txtSend, Me.btnSend, Me.txtChat, Me.btnExit, Me.lblUsers, Me.label1})
            Me.groupBox1.Location = New System.Drawing.Point(9, 6)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(377, 235)
            Me.groupBox1.TabIndex = 0
            Me.groupBox1.TabStop = False
            '
            'txtSend
            '
            Me.txtSend.Location = New System.Drawing.Point(14, 205)
            Me.txtSend.MaxLength = 508
            Me.txtSend.Name = "txtSend"
            Me.txtSend.Size = New System.Drawing.Size(280, 20)
            Me.txtSend.TabIndex = 5
            Me.txtSend.Text = ""
            '
            'btnSend
            '
            Me.btnSend.Enabled = False
            Me.btnSend.Location = New System.Drawing.Point(297, 204)
            Me.btnSend.Name = "btnSend"
            Me.btnSend.Size = New System.Drawing.Size(72, 22)
            Me.btnSend.TabIndex = 4
            Me.btnSend.Text = "&Send"
            '
            'txtChat
            '
            Me.txtChat.Location = New System.Drawing.Point(15, 45)
            Me.txtChat.Multiline = True
            Me.txtChat.Name = "txtChat"
            Me.txtChat.ReadOnly = True
            Me.txtChat.Size = New System.Drawing.Size(354, 152)
            Me.txtChat.TabIndex = 3
            Me.txtChat.Text = ""
            '
            'btnExit
            '
            Me.btnExit.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.btnExit.Location = New System.Drawing.Point(294, 16)
            Me.btnExit.Name = "btnExit"
            Me.btnExit.Size = New System.Drawing.Size(72, 22)
            Me.btnExit.TabIndex = 2
            Me.btnExit.Text = "E&xit"
            '
            'lblUsers
            '
            Me.lblUsers.Location = New System.Drawing.Point(198, 18)
            Me.lblUsers.Name = "lblUsers"
            Me.lblUsers.Size = New System.Drawing.Size(37, 14)
            Me.lblUsers.TabIndex = 1
            Me.lblUsers.Text = "0"
            Me.lblUsers.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
            '
            'label1
            '
            Me.label1.Location = New System.Drawing.Point(11, 18)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(183, 14)
            Me.label1.TabIndex = 0
            Me.label1.Text = "Number of people in conversation: "
            '
            'ChatPeer
            '
            Me.AcceptButton = Me.btnSend
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.CancelButton = Me.btnExit
            Me.ClientSize = New System.Drawing.Size(394, 253)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.groupBox1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "ChatPeer"
            Me.Text = "Chat Peer Sample"
            Me.groupBox1.ResumeLayout(False)
            Me.ResumeLayout(False)

        End Sub 'InitializeComponent

        '/ <summary>
        '/ A player was created
        '/ </summary>
        Private Sub PlayerCreated(ByVal sender As Object, ByVal e As PlayerCreatedEventArgs)
            ' Get the PlayerInformation and store it 
            Dim peerInfo As PlayerInformation = peerObject.GetPeerInformation(e.Message.PlayerID)
            Dim oPlayer As New Players(e.Message.PlayerID, peerInfo.Name)
            ' We lock the data here since it is shared across multiple threads.
            SyncLock playerList
                playerList.Add(oPlayer)
                ' Update our number of players and our button
                lblUsers.Text = playerList.Count.ToString()
            End SyncLock
            ' Save this player id if it's ourselves
            If peerInfo.Local Then
                localPlayerId = e.Message.PlayerID
            End If
        End Sub 'PlayerCreated



        '/ <summary>
        '/ A player was destroyed
        '/ </summary>
        Private Sub PlayerDestroyed(ByVal sender As Object, ByVal e As PlayerDestroyedEventArgs)
            ' Remove this player from our list
            ' We lock the data here since it is shared across multiple threads.
            SyncLock playerList
                Dim player As Players
                For Each player In playerList
                    If e.Message.PlayerID = player.playerId Then
                        playerList.Remove(player)
                        Exit For
                    End If
                Next player
                ' Update our number of players and our button
                lblUsers.Text = playerList.Count.ToString()
            End SyncLock
        End Sub 'PlayerDestroyed




        '/ <summary>
        '/ The host was migrated, see if you're the new host
        '/ </summary>
        Private Sub HostMigrated(ByVal sender As Object, ByVal e As HostMigratedEventArgs)
            If localPlayerId = e.Message.NewHostID Then
                ' I'm the new host, update my UI
                Me.Text += " (HOST)"
            End If
        End Sub 'HostMigrated




        '/ <summary>
        '/ We've received data, parse it
        '/ </summary>
        Private Sub DataReceived(ByVal sender As Object, ByVal e As ReceiveEventArgs)
            If CByte(e.Message.ReceiveData.Read(GetType(Byte))) = ChatMessageId Then ' We've received text chat
                ' We won't be using the helper functions here since we want to 
                ' interop with the c++ version of the app.  It packages it's messages
                ' up with the first byte being the msg id (ChatMessageId), and 
                ' the next xxx bytes as an ANSI string.
                ' Get the default ASCII decoder
                Dim dec As System.Text.Decoder = System.Text.Encoding.ASCII.GetDecoder()
                Dim length As Integer = CInt(e.Message.ReceiveData.Length) - 1
                ' Create a char array of the right length
                Dim data As Byte() = CType(e.Message.ReceiveData.Read(GetType(Byte), length), Byte())
                Dim c(dec.GetCharCount(data, 0, length)) As Char
                ' Get the actual decoded characters
                dec.GetChars(data, 0, length, c, 0)
                ' Now we can use the string builder to actually build our string
                Dim sb As New System.Text.StringBuilder(c.Length)
                sb.Insert(0, c, 0, dec.GetCharCount(data, 0, length))

                Dim sChatText As String = sb.ToString() ' The actual chat text
                ' Now build the string we will be displaying to the user:
                Dim sChatString As String = "<" + GetPlayerName(e.Message.SenderID) + "> " + sChatText
                ' Now update our text
                SyncLock txtChat
                    If txtChat.Text.Length > txtChat.MaxLength * 0.95 Then
                        txtChat.Text = txtChat.Text.Remove(0, CInt(txtChat.MaxLength / 2))
                    End If
                    txtChat.AppendText(sChatString)
                    txtChat.AppendText(ControlChars.Cr + ControlChars.Lf)
                    txtChat.SelectionStart = txtChat.Text.Length
                    txtChat.ScrollToCaret()
                End SyncLock
            End If
            e.Message.ReceiveData.Dispose() ' We no longer need the data, Dispose the buffer
        End Sub 'DataReceived




        '/ <summary>
        '/ The session was terminated
        '/ </summary>
        Private Sub SessionTerminated(ByVal sender As Object, ByVal e As SessionTerminatedEventArgs)
            ' Well, this session is being terminated, let the user know
            If e.Message.ResultCode = ResultCode.HostTerminatedSession Then
                MessageBox.Show("The Host has terminated this session.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Else
                MessageBox.Show("The session has been lost.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information)
            End If
            ' This will post a message on the main thread to shut down our form
            Me.BeginInvoke(New PeerCloseCallback(AddressOf Me.PeerClose))
        End Sub 'SessionTerminated


        '/ <summary>
        '/ Exit the application
        '/ </summary>
        Private Sub btnExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnExit.Click
            ' Exit the application
            Me.Dispose(Disposing)
        End Sub 'btnExit_Click


        '/ <summary>
        '/ This will return a players name based on the ID of that player
        '/ </summary>
        Private Function GetPlayerName(ByVal idPlayer As Integer) As String
            SyncLock playerList
                Dim p As Players
                For Each p In playerList
                    If p.playerId = idPlayer Then
                        Return p.Name
                    End If
                Next p
            End SyncLock
            Return Nothing
        End Function 'GetPlayerName



        '/ <summary>
        '/ Fired when the text to send has been changed
        '/ </summary>
        Private Sub SendTextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles txtSend.TextChanged
            btnSend.Enabled = CType(sender, TextBox).Text.Length > 0
        End Sub 'SendTextChanged




        '/ <summary>
        '/ We want to send our chat message
        '/ </summary>
        Private Sub btnSend_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnSend.Click
            ' Ok, we need to package up the text into a format 
            ' that the c++ version of the sample expects since 
            ' we are interop'ing with it.
            Dim strlen As Integer = System.Text.Encoding.ASCII.GetByteCount(txtSend.Text)
            Dim data As New NetworkPacket()
            Dim stringdata(strlen) As Byte ' Create our buffer
            data.Write(ChatMessageId) ' Set the msg type
            'Now fill up the rest of the byte array with the ASCII chars of the string
            System.Text.Encoding.ASCII.GetBytes(txtSend.Text, 0, strlen, stringdata, 0)
            data.Write(stringdata)
            ' Now we've got the data setup, send it off.
            peerObject.SendTo(CInt(PlayerID.AllPlayers), data, 0, SendFlags.Guaranteed)
            ' Now that we've sent out the text, clear it
            txtSend.Text = Nothing
        End Sub 'btnSend_Click




        '/ <summary>
        '/ Shut down the sample
        '/ </summary>
        Public Sub PeerClose()
            ' The session was terminated, go ahead and shut down
            Me.Dispose()
        End Sub 'PeerClose


        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Dim wfData As New ChatPeer()
            Try
                Application.Run(wfData)
            Catch
            Finally
                wfData.Dispose()
            End Try
        End Sub 'Main
    End Class 'ChatPeer
End Namespace 'ChatPeerSample