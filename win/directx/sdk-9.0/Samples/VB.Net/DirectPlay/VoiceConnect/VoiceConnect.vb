'----------------------------------------------------------------------------
' File: VoiceConnect.vb
'
' Desc: The main file for VoiceConnect that shows how use DirectPlay along 
'       with DirectPlayVoice to allow talking in a conference situation.
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
Imports Lobby = Microsoft.DirectX.DirectPlay.Lobby
Imports Voice = Microsoft.DirectX.DirectPlay.Voice


Namespace VoiceConnect

    '/ <summary>
    '/ Holds information needed to track a player
    '/ </summary>
    Public Class VoicePlayer

        Public Sub New(ByVal id As Integer, ByVal n As String)
            PlayerId = id
            Name = n
            Talking = False
        End Sub 'New

        Public PlayerId As Integer ' Player's DirectPlay ID
        Public Name As String ' Display name
        Public Talking As Boolean ' Currently talking
    End Class 'VoicePlayer
    _

    '/ <summary>
    '/ This is the main VoiceConnect 
    '/ </summary>
    Public Class VoiceConnect
        Inherits System.Windows.Forms.Form
        Private WithEvents buttonSetup As System.Windows.Forms.Button
        Private WithEvents buttonExit As System.Windows.Forms.Button '
        Private label1 As System.Windows.Forms.Label
        Private listViewPlayers As System.Windows.Forms.ListView
        Private columnHeaderName As System.Windows.Forms.ColumnHeader
        Private columnHeaderTalking As System.Windows.Forms.ColumnHeader
        Private labelNumPlayers As System.Windows.Forms.Label
        '/ <summary>
        '/ App specific variables
        '/ </summary>
        Private peerObject As Peer ' DirectPlay Peer object
        Private playWizard As ConnectWizard ' ConnectionWizard helper
        Private voiceSettings As VoiceWizard ' VoiceWizard helper
        Private client As Voice.Client ' DirectPlay Voice Client object
        Private server As Voice.Server ' DirectPlay Voice Server object
        Private playerList As ArrayList ' List of connected players
        Private localPlayerId As Integer ' Local player's DirectPlay ID
        Private isLocalPlayerQuitting As Boolean = False


        '/ <summary>
        '/ This delegate is used to invoke a shutdown of the peer outside of a message handler,
        '/ or to update the player list
        '/ </summary>
        Delegate Sub BeginInvokeDelegate()

        ' This GUID allows DirectPlay to find other instances of the same game on
        ' the network.  So it must be unique for every game, and the same for 
        ' every instance of that game.
        ' However, we are using the same guid the C++ and VB.NET version of the 
        ' samples use so they can all communicate together.
        Public g_guidApp As New Guid(&H2B395CA8, &H2610, &H45AC, &HA4, &H4D, &H8, &HDC, &HD1, &H66, &H9A, &HAD)
        Private DefaultPort As Integer = 2512


        '/ <summary>
        '/ Constructor
        '/ </summary>
        Public Sub New()
            InitializeComponent()

            Try

                'set up state members
                playerList = New ArrayList()

                'create the peer
                peerObject = New Peer()

                'create the voice client and attach message handlers
                client = New Voice.Client(peerObject)
                AddHandler client.HostMigrated, AddressOf VoiceHostMigrated
                AddHandler client.PlayerCreated, AddressOf VoicePlayerCreated
                AddHandler client.PlayerDeleted, AddressOf VoicePlayerDeleted
                AddHandler client.SessionLost, AddressOf VoiceSessionLost
                AddHandler client.PlayerStarted, AddressOf PlayerStarted
                AddHandler client.PlayerStopped, AddressOf PlayerStopped
                AddHandler client.RecordStarted, AddressOf RecordStarted
                AddHandler client.RecordStopped, AddressOf RecordStopped


                'session was not lobby launched -- using connection wizard
                playWizard = New ConnectWizard(peerObject, g_guidApp, "VoiceConnect")
                playWizard.DefaultPort = DefaultPort

                'create the voice wizard
                voiceSettings = New VoiceWizard()

                If playWizard.StartWizard() Then
                    If playWizard.IsHost Then
                        Me.Text += " (HOST)"

                        'create the voice server
                        server = New Voice.Server(peerObject)

                        'init the voice server
                        voiceSettings.InitVoiceHost(peerObject, server, client, Me)
                    Else
                        'connection to another host was successful
                        voiceSettings.InitVoiceClient(peerObject, client, Me)
                    End If

                Else
                    Me.Dispose()
                End If



            Catch e As Exception
                Console.WriteLine(e.ToString())
                'allow exception to fall though and exit.
                Me.Close()
            End Try
        End Sub 'New


        '/ <summary>
        '/ Session shutdown
        '/ </summary>
        Public Sub PeerClose()
            If Not isLocalPlayerQuitting Then
                ' Well, this session is being terminated, let the user know
                MessageBox.Show("The session has been lost.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information)
            End If

            ' The session was terminated, go ahead and shut down
            Me.Close()
        End Sub 'PeerClose


        '/ <summary>
        '/ Refresh the user interface
        '/ </summary>
        Private Sub UpdatePlayerList()
            SyncLock playerList
                listViewPlayers.Items.Clear()
                Dim player As VoicePlayer
                For Each player In playerList
                    Dim newItem As ListViewItem = listViewPlayers.Items.Add(player.Name)
                    newItem.SubItems.Add(player.Talking.ToString())
                Next player
            End SyncLock
            ' Update our number of players and our button
            labelNumPlayers.Text = listViewPlayers.Items.Count.ToString()
        End Sub 'UpdatePlayerList


        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            Me.Hide()
            Try
                'Cleanup Voice
                If Not (client Is Nothing) Then
                    client.Disconnect(Voice.VoiceFlags.Sync)
                End If
                If Not (server Is Nothing) Then
                    server.StopSession()
                End If
                ' Cleanup DPlay
                If Not (peerObject Is Nothing) Then
                    peerObject.Dispose()
                End If
            Catch
            End Try
            MyBase.Dispose(disposing)
        End Sub 'Dispose


        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.buttonSetup = New System.Windows.Forms.Button()
            Me.buttonExit = New System.Windows.Forms.Button()
            Me.label1 = New System.Windows.Forms.Label()
            Me.labelNumPlayers = New System.Windows.Forms.Label()
            Me.listViewPlayers = New System.Windows.Forms.ListView()
            Me.columnHeaderName = New System.Windows.Forms.ColumnHeader()
            Me.columnHeaderTalking = New System.Windows.Forms.ColumnHeader()
            Me.SuspendLayout()
            ' 
            ' buttonSetup
            ' 
            Me.buttonSetup.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right
            Me.buttonSetup.Location = New System.Drawing.Point(256, 8)
            Me.buttonSetup.Name = "buttonSetup"
            Me.buttonSetup.TabIndex = 1
            Me.buttonSetup.Text = "Setup"
            ' 
            ' buttonExit
            ' 
            Me.buttonExit.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right
            Me.buttonExit.Location = New System.Drawing.Point(336, 8)
            Me.buttonExit.Name = "buttonExit"
            Me.buttonExit.TabIndex = 2
            Me.buttonExit.Text = "Exit"
            ' 
            ' label1
            ' 
            Me.label1.Location = New System.Drawing.Point(16, 11)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(176, 16)
            Me.label1.TabIndex = 3
            Me.label1.Text = "Number of people in conversation"
            ' 
            ' labelNumPlayers
            ' 
            Me.labelNumPlayers.Location = New System.Drawing.Point(208, 11)
            Me.labelNumPlayers.Name = "labelNumPlayers"
            Me.labelNumPlayers.Size = New System.Drawing.Size(32, 16)
            Me.labelNumPlayers.TabIndex = 4
            Me.labelNumPlayers.Text = "0"
            ' 
            ' listViewPlayers
            ' 
            Me.listViewPlayers.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right
            Me.listViewPlayers.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.columnHeaderName, Me.columnHeaderTalking})
            Me.listViewPlayers.Location = New System.Drawing.Point(8, 40)
            Me.listViewPlayers.Name = "listViewPlayers"
            Me.listViewPlayers.Size = New System.Drawing.Size(408, 224)
            Me.listViewPlayers.TabIndex = 5
            Me.listViewPlayers.View = System.Windows.Forms.View.Details
            ' 
            ' columnHeaderName
            ' 
            Me.columnHeaderName.Text = "Name"
            Me.columnHeaderName.Width = 336
            ' 
            ' columnHeaderTalking
            ' 
            Me.columnHeaderTalking.Text = "Talking"
            Me.columnHeaderTalking.Width = 68
            ' 
            ' VoiceConnect
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(424, 270)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.listViewPlayers, Me.labelNumPlayers, Me.label1, Me.buttonExit, Me.buttonSetup})
            Me.MinimumSize = New System.Drawing.Size(430, 300)
            Me.Name = "VoiceConnect"
            Me.Text = "Voice Connect"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Dim vc As New VoiceConnect()
            Try
                Application.Run(vc)
            Catch
            Finally
                vc.Dispose()
            End Try
        End Sub 'Main



        Private Sub VoiceHostMigrated(ByVal sender As Object, ByVal dpMessage As Voice.HostMigratedEventArgs) '
            If localPlayerId = dpMessage.Message.NewHostID Then
                ' I'm the new host, update my UI
                Me.Text += " (HOST)"

                'add reference to new voice server
                server = dpMessage.Message.ServerObject

                'notify the voice wizard of the change
                voiceSettings.HostMigrate(server)
            End If
        End Sub 'VoiceHostMigrated

        Private Sub VoicePlayerCreated(ByVal sender As Object, ByVal e As Voice.VoicePlayerCreatedEventArgs)
            ' Get the PlayerInformation and store it 
            Dim dpPeer As PlayerInformation = peerObject.GetPeerInformation(e.Message.PlayerID)
            Dim oPlayer As New VoicePlayer(e.Message.PlayerID, dpPeer.Name)
            ' We lock the data here since it is shared across multiple threads.
            SyncLock playerList
                playerList.Add(oPlayer)
            End SyncLock
            ' Update our number of players and our button
            labelNumPlayers.Text = playerList.Count.ToString()
            ' Save this player id if it's ourselves
            If dpPeer.Local Then
                localPlayerId = e.Message.PlayerID
            End If
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.UpdatePlayerList))
        End Sub 'VoicePlayerCreated


        Private Sub VoicePlayerDeleted(ByVal sender As Object, ByVal e As Voice.VoicePlayerDeletedEventArgs)
            ' Remove this player from our list
            ' We lock the data here since it is shared across multiple threads.
            SyncLock playerList
                Dim player As VoicePlayer
                For Each player In playerList
                    If e.Message.PlayerID = player.PlayerId Then
                        playerList.Remove(player)
                        Exit For
                    End If
                Next player
            End SyncLock
            'Update our player list
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.UpdatePlayerList))
        End Sub 'VoicePlayerDeleted


        Private Sub VoiceSessionLost(ByVal sender As Object, ByVal dpMessage As Voice.SessionLostEventArgs)
            ' This will post a message on the main thread to shut down our form
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.PeerClose))
        End Sub 'VoiceSessionLost


        Private Sub PlayerStarted(ByVal sender As Object, ByVal dpMessage As Voice.PlayerStartedEventArgs)
            SyncLock playerList
                Dim i As Integer
                For i = 0 To playerList.Count - 1
                    If CType(playerList(i), VoicePlayer).PlayerId = dpMessage.Message.SourcePlayerID Then
                        Dim oPlayer As VoicePlayer = CType(playerList(i), VoicePlayer)
                        oPlayer.Talking = True
                        playerList(i) = oPlayer
                        Exit For
                    End If
                Next i
            End SyncLock
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.UpdatePlayerList))
        End Sub 'PlayerStarted


        Private Sub PlayerStopped(ByVal sender As Object, ByVal dpMessage As Voice.PlayerStoppedEventArgs)
            SyncLock playerList
                Dim i As Integer
                For i = 0 To playerList.Count - 1
                    If CType(playerList(i), VoicePlayer).PlayerId = dpMessage.Message.SourcePlayerID Then
                        Dim oPlayer As VoicePlayer = CType(playerList(i), VoicePlayer)
                        oPlayer.Talking = False
                        playerList(i) = oPlayer
                        Exit For
                    End If
                Next i
            End SyncLock
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.UpdatePlayerList))
        End Sub 'PlayerStopped

        Private Sub RecordStarted(ByVal sender As Object, ByVal dpMessage As Voice.RecordStartedEventArgs)
            SyncLock playerList
                Dim i As Integer
                For i = 0 To playerList.Count - 1
                    If CType(playerList(i), VoicePlayer).PlayerId = localPlayerId Then
                        Dim oPlayer As VoicePlayer = CType(playerList(i), VoicePlayer)
                        oPlayer.Talking = True
                        playerList(i) = oPlayer
                        Exit For
                    End If
                Next i
            End SyncLock
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.UpdatePlayerList))
        End Sub 'RecordStarted


        Private Sub RecordStopped(ByVal sender As Object, ByVal dpMessage As Voice.RecordStoppedEventArgs)
            SyncLock playerList
                Dim i As Integer
                For i = 0 To playerList.Count - 1
                    If CType(playerList(i), VoicePlayer).PlayerId = localPlayerId Then
                        Dim oPlayer As VoicePlayer = CType(playerList(i), VoicePlayer)
                        oPlayer.Talking = False
                        playerList(i) = oPlayer
                        Exit For
                    End If
                Next i
            End SyncLock
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.UpdatePlayerList))
        End Sub 'RecordStopped                               


        Private Sub buttonSetup_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonSetup.Click '
            'Open up the setup dialog through the voice wizard
            voiceSettings.ShowClientSetupDialog(Me.Handle)
        End Sub 'buttonSetup_Click


        Private Sub buttonExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonExit.Click
            isLocalPlayerQuitting = True
            ' This will post a message on the main thread to shut down our form
            Me.BeginInvoke(New BeginInvokeDelegate(AddressOf Me.PeerClose))
        End Sub 'buttonExit_Click
    End Class 'VoiceConnect 
End Namespace 'VoiceConnect '
