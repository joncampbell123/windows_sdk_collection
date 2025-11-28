'----------------------------------------------------------------------------
' File: LobbyLaunch.cs
'
' Desc: This tutorial adds lobby registration and remote launch capability
'
' Copyright (c) Microsoft Corp. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.IO
Imports System.Windows.Forms
Imports System.Text
Imports System.Collections
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay
Imports Lobby = Microsoft.DirectX.DirectPlay.Lobby

Namespace Tut07_LobbyLaunch

    ' Possible connection states
    Public Enum ConnectionType
        Disconnected
        Hosting
        Connected
    End Enum 'ConnectionType

    '/ <summary>
    '/ Application class.
    '/ </summary>
    Public Class LobbyLaunchApp
        Implements IDisposable 
        ' Application identifier
        ' This guid allows DirectPlay to find other instances of the same game on
        ' the network, so it must be unique for every game, and the same for 
        ' every instance of that game. You can use the guidgen.exe program to
        ' generate a new guid.
        Private Shared m_AppGuid As New Guid("C301FCED-E884-41a9-94D7-DE66943EC7DB")
        Public Shared DefaultPort As Integer = 2607 ' Default port number

        ' DirectPlay
        Private m_Peer As Peer = Nothing ' DirectPlay Peer object
        Private m_LocalAddress As New Address() ' Local address
        ' DirectPlay Lobby
        Private m_LobbyApp As Lobby.Application = Nothing ' DirectPlay Lobby Application
        Private m_LobbyHandle As Integer = 0 ' Lobby client handle
        ' Application 
        Private m_Form As ApplicationForm = Nothing ' Main application WinForm
        Private m_SessionName As String = "New Host" ' Hosted session name 
        Private m_FoundSessions As New ArrayList() ' Enumerated sessions
        Private m_Connection As ConnectionType = ConnectionType.Disconnected ' Current connection state     

        Public ReadOnly Property Connection() As ConnectionType
            Get
                Return m_Connection
            End Get
        End Property

        Public ReadOnly Property FoundSessions() As ArrayList
            Get
                Return m_FoundSessions
            End Get
        End Property
        '/ <summary>
        '/ Constructor
        '/ </summary>
        Public Sub New()
            ' Initialize the application's UI
            m_Form = New ApplicationForm(Me)

            ' Update the UI
            UpdateUI()

            ' Initialize DirectPlay and event handlers
            InitDirectPlay()

            ' Initialize DirectPlay lobby and event handlers
            InitDirectPlayLobby()

            ' Set the service provider for our local address to TCP/IP
            m_LocalAddress.ServiceProvider = Address.ServiceProviderTcpIp

            ' Verify the computer supports our chosen service provider
            If Not IsServiceProviderValid(m_LocalAddress.ServiceProvider) Then
                MessageBox.Show(m_Form, "You must have TCP/IP installed on your " + "computer to run this tutorial.", "DirectPlay Tutorial", MessageBoxButtons.OK, MessageBoxIcon.Error)

                m_Form.Dispose()
            End If

            ' If there's an associated lobby handle then we were launched
            ' from a lobby client, and should create or join a session
            ' based on settings received from the lobby
            If m_LobbyHandle <> 0 Then
                LobbyLaunch()
            End If
        End Sub 'New

        '/ <summary>
        '/ Initializes the DirectPlay Peer object
        '/ </summary>
        Private Sub InitDirectPlay()
            ' Release any exising resources
            If Not (m_Peer Is Nothing) Then
                m_Peer.Dispose()
            End If
            ' Create a new DirectPlay Peer object
            m_Peer = New Peer()

            ' Add handlers for DirectPlay events
            AddHandler m_Peer.FindHostResponse, AddressOf FindHostResponseHandler
            AddHandler m_Peer.Receive, AddressOf ReceiveHandler
            AddHandler m_Peer.HostMigrated, AddressOf HostMigratedHandler
            AddHandler m_Peer.SessionTerminated, AddressOf SessionTerminatedHandler

            m_Connection = ConnectionType.Disconnected
        End Sub 'InitDirectPlay

        '/ <summary>
        '/ Initializes the DirectPlay Lobby.Application object
        '/ </summary>
        Private Sub InitDirectPlayLobby()
            ' Initialize the Lobby application object
            m_LobbyApp = New Lobby.Application(m_LobbyHandle)

            ' Add handlers for DirectPlay Lobby events
            AddHandler m_LobbyApp.Connect, AddressOf LobbyConnectHandler
        End Sub 'InitDirectPlayLobby

        '/ <summary>
        '/ Event handler for the DirectPlay FindHostResponse message
        '/ </summary>
        Public Sub FindHostResponseHandler(ByVal sender As Object, ByVal args As FindHostResponseEventArgs)
            Dim Node As New HostInfo()
            Node.GuidInstance = args.Message.ApplicationDescription.GuidInstance
            Node.HostAddress = CType(args.Message.AddressSender.Clone(), Address)
            Node.SessionName = args.Message.ApplicationDescription.SessionName

            ' If he haven't already seen this host, add the detected session 
            ' to the stored list
            If Not FoundSessions.Contains(Node) Then
                FoundSessions.Add(Node)
            End If
        End Sub 'FindHostResponseHandler

        '/ <summary>
        '/ Handler for incoming DirectPlay Receive events
        '/ </summary>
        Public Sub ReceiveHandler(ByVal sender As Object, ByVal args As ReceiveEventArgs)
            If m_Form.ReceivedMessagesListBox.Enabled Then
                ' Read the incoming chat message. Since network data is always
                ' broken down into raw bytes for transfer, the sent object must
                ' be rebuilt. The NetworkPacket class contains methods to 
                ' easily send and receive unicode strings, but this sample
                ' manually decodes the string for compatibility with the C++
                ' tutorials. The data is first read into a byte array, then Unicode 
                ' decoded, and finally added to our list of received messages.
                Dim packet As NetworkPacket = args.Message.ReceiveData
                Dim data As Byte() = CType(packet.Read(GetType(Byte), packet.Length), Byte())
                m_Form.ReceivedMessagesListBox.Items.Add(Encoding.Unicode.GetString(data))
            End If
        End Sub 'ReceiveHandler

        '/ <summary>
        '/ Handler for incoming DirectPlay HostMigrated events
        '/ </summary>
        Public Sub HostMigratedHandler(ByVal sender As Object, ByVal args As HostMigratedEventArgs)
            Dim info As PlayerInformation = m_Peer.GetPeerInformation(args.Message.NewHostID)

            ' See if we are the new host
            If info.Local Then
                m_Connection = ConnectionType.Hosting
                UpdateUI()
            End If
        End Sub 'HostMigratedHandler

        '/ <summary>
        '/ Handler for incoming DirectPlay SessionTerminated events
        '/ </summary>
        Public Sub SessionTerminatedHandler(ByVal sender As Object, ByVal args As SessionTerminatedEventArgs)
            MessageBox.Show(m_Form, "Connect lost or host terminated session", "DirectPlay Tutorial")
            m_Connection = ConnectionType.Disconnected
            UpdateUI()
        End Sub 'SessionTerminatedHandler

        '/ <summary>
        '/ Handler for LobbyConnectEvent. By registering with the lobby which triggered
        '/ this event, we can continue to receive notifications from this lobby client.
        '/ </summary>
        Public Sub LobbyConnectHandler(ByVal sender As Object, ByVal args As Lobby.ConnectEventArgs)
            Try
                ' Register with the DirectPlay lobby so we'll receive automatic notifications
                m_LobbyHandle = args.Message.ConnectionHandle
                m_Peer.RegisterLobby(m_LobbyHandle, m_LobbyApp)
            Catch ex As Exception
                m_Form.ShowException(ex, "RegisterLobby", True)
                m_Form.Dispose()
                Return
            End Try
        End Sub 'LobbyConnectHandler

        '/ <summary>
        '/ Host or connect to a session based on the settings provided by the lobby
        '/ </summary>
        Public Sub LobbyLaunch()
            ' Create an object to retrieve the connection settings from the lobby client
            Dim settings As Lobby.ConnectionSettings

            Try
                ' Get settings
                settings = m_LobbyApp.GetConnectionSettings(m_LobbyHandle)
                m_SessionName = settings.ApplicationDescriptionChanged.SessionName

                ' If host flag is set, this application should create a new session
                If 0 <> (settings.Flags And Lobby.ConnectionSettingsFlags.Host) Then
                    ' Host a new session
                    m_Peer.Host(settings.ApplicationDescriptionChanged, settings.GetDeviceAddresses()) ' Application description
                    ' Local device addresses
                    m_Connection = ConnectionType.Hosting
                Else
                    ' Connect to an existing session
                    m_Peer.Connect(settings.ApplicationDescriptionChanged, settings.HostAddress, settings.GetDeviceAddresses()(0), Nothing, ConnectFlags.Sync) ' Application description
                    ' Host address
                    ' Local device address
                    ' Async handle                                   
                    ' Connect flags
                    m_Connection = ConnectionType.Connected
                End If
            Catch ex As Exception
                m_Form.ShowException(ex, Nothing, True)
                m_Form.Dispose()
                Return
            End Try

            UpdateUI()
        End Sub 'LobbyLaunch

        '/ <summary>
        '/ Register this program for lobby access. 
        '/ </summary>
        Public Sub Register()
            ' Fill in a new ProgramDescription for this application. The information
            ' in these fields will be entered into the system registry so lobby
            ' clients can find and launch instances of this program.
            Dim desc As New Lobby.ProgramDescription()
            desc.ApplicationName = m_Form.Text
            desc.GuidApplication = m_AppGuid

            Dim index As Integer = Application.ExecutablePath.LastIndexOf("\"c)
            desc.ExecutablePath = Application.ExecutablePath.Substring(0, index)
            desc.ExecutableFilename = Application.ExecutablePath.Substring((index + 1))

            Try
                ' Register the program
                m_LobbyApp.RegisterProgram(desc)
            Catch ex As Exception
                m_Form.ShowException(ex, "RegisterProgram", False)
                Return
            End Try

            MessageBox.Show(m_Form, "Successfully registered lobby support.", "DirectPlay Tutorial")
        End Sub 'Register


        '/ <summary>
        '/ Remove this application's lobby information from the system registry
        '/ </summary>
        Public Sub Unregister()
            Try
                m_LobbyApp.UnregisterProgram(m_AppGuid)
            Catch ex As Exception
                m_Form.ShowException(ex, "UnregisterProgram", False)
                Return
            End Try

            MessageBox.Show(m_Form, "Application lobby information removed.", "DirectPlay Tutorial")
        End Sub 'Unregister



        '/ <summary>
        '/ Host a new DirectPlay session.
        '/ </summary>
        Public Sub HostSession()
            ' Get desired hosting options
            Dim dialog As New HostDialog()
            dialog.LocalPort = DefaultPort

            If DialogResult.Cancel = dialog.ShowDialog() Then
                Return
            End If
            ' Update the UI
            m_Form.SessionStatusLabel.Text = "Connecting..."
            m_Form.Update()

            ' Store host dialog choices
            m_SessionName = dialog.SessionName

            ' Add the port number
            If dialog.LocalPort > 0 Then
                m_LocalAddress.AddComponent("port", dialog.LocalPort)
            End If

            ' Create an application description
            Dim AppDesc As New ApplicationDescription()
            AppDesc.GuidApplication = m_AppGuid
            AppDesc.SessionName = m_SessionName
            AppDesc.Flags = SessionFlags.NoDpnServer

            If dialog.IsHostMigrationEnabled Then
                AppDesc.Flags = AppDesc.Flags Or SessionFlags.MigrateHost
            End If

            Try
                ' Host a new session
                m_Peer.Host(AppDesc, m_LocalAddress) ' Application description 
                ' Local device address
                m_Connection = ConnectionType.Hosting
            Catch ex As Exception
                m_Form.ShowException(ex, "Host", True)
                m_Form.Dispose()
                Return
            End Try
        End Sub 'HostSession

        '/ <summary>
        '/ Find all sessions at the given host address
        '/ </summary>
        '/ <param name="hostname">IP address or hostname to search</param>
        '/ <param name="port">Remote port to search</param>
        Public Sub EnumerateSessions(ByVal hostname As String, ByVal port As Integer)
            ' Set the desired search options
            Dim HostAddress As New Address()
            HostAddress.ServiceProvider = Address.ServiceProviderTcpIp

            If hostname.Length > 0 Then
                HostAddress.AddComponent("hostname", hostname)
            End If

            If port > 0 Then
                HostAddress.AddComponent("port", port)
            End If

            Dim AppDesc As New ApplicationDescription()
            AppDesc.GuidApplication = m_AppGuid

            ' Find all sessions hosted at the given address. When a session is
            ' found, DirectPlay calls our FindHostResponse delegate. Since we're
            ' passing in the "Sync" flag, Connect will block until the search
            ' timeout has expired.
            Try
                m_Peer.FindHosts(AppDesc, HostAddress, m_LocalAddress, Nothing, 0, 0, 0, FindHostsFlags.Sync) ' Application description
                ' Host address
                ' Local device address
                ' Enumeration data
                ' Enumeration count (using default)
                ' Retry interval (using default)
                ' Timeout (using default)
                ' Flags
            Catch ex As Exception
                m_Form.ShowException(ex, "FindHosts", True)
                m_Form.Dispose()
                Return
            End Try
        End Sub 'EnumerateSessions

        '/ <summary>
        '/ Connect to the currently selected session
        '/ </summary>
        Public Sub ConnectToSession()
            ' Display connection dialog
            Dim dialog As New ConnectDialog(Me)
            dialog.RemotePort = DefaultPort

            If DialogResult.Cancel = dialog.ShowDialog() Then
                Return
            End If
            ' Update the UI
            m_Form.SessionStatusLabel.Text = "Connecting..."
            m_Form.Update()

            ' Store connection dialog choices
            Dim SelectedHost As HostInfo = dialog.SelectedHost
            If SelectedHost Is Nothing Then
                Return
            End If
            ' Create an application description object to hold the desired
            ' host's instance guid.
            Dim appDesc As New ApplicationDescription()
            appDesc.GuidInstance = SelectedHost.GuidInstance

            ' Attempt to connect to the selected DirectPlay session. Once we
            ' are connected to the session, we will receive DirectPlay messages
            ' about session events (like players joining/exiting and received game 
            ' data). Since we're passing in the "Sync" flag, the Connect call will
            ' block until the session is connected or the timeout expires.
            Try
                m_Peer.Connect(appDesc, SelectedHost.HostAddress, m_LocalAddress, Nothing, ConnectFlags.Sync) ' Application description
                ' Host address
                ' Local device address
                ' User connection data (none)  
                ' Flags
                m_SessionName = SelectedHost.SessionName
                m_Connection = ConnectionType.Connected
            Catch ex As Exception
                m_Form.ShowException(ex, "Connect", False)
                Return
            End Try
        End Sub 'ConnectToSession



        '/ <summary>
        '/ Sends the current outgoing chat string to all connected peers
        '/ </summary>
        Public Sub SendData()
            ' Create a network packet object to which we can write our chat message.
            ' For compatibility with the current C++ tutorials, the unicode text
            ' will be encoded as a null-terminated string for network transfer, as 
            ' opposed to the .NET convention of string length followed by characters; 
            ' however, there are no limits on how the data can be formatted since 
            ' all data is received as a raw byte array.
            Dim packet As New NetworkPacket()
            packet.Write(Encoding.Unicode.GetBytes(m_Form.SendTextBox.Text))

            ' Now that all the outgoing data has been encoded to a network
            ' packet, the DirectPlay send method can be called. Since in a
            ' peer-to-peer topology there is no server, you must inform
            ' DirectPlay which peer you wish to receive the data. For this
            ' sample, we'll deliver the message to all connected players
            ' (except the local player, which we exclude with the NoLoopback
            ' flag )
            m_Peer.SendTo(CInt(PlayerID.AllPlayers), packet, 0, SendFlags.Sync Or SendFlags.NoLoopback) ' Send to session
            ' Outgoing data
            ' Timeout (default)        
            ' Flags

            m_Form.SendTextBox.Text = ""
        End Sub 'SendData


        '/ <summary>
        '/ Terminate or disconnect from the session.
        '/ </summary>
        Public Sub Disconnect()
            ' Disconnect by closing the current peer and opening
            ' a new one.
            InitDirectPlay()
            UpdateUI()
        End Sub 'Disconnect


        '/ <summary>
        '/ Verify the given service provider if found in the enumerated list
        '/ of installed providers on this computer.
        '/ </summary>
        '/ <param name="provider">Guid of the provider to search for</param>
        '/ <returns>true if the given provider is found</returns>
        Private Function IsServiceProviderValid(ByVal provider As Guid) As Boolean
            ' Ask DirectPlay for the service provider list
            Dim SPInfoArray As ServiceProviderInformation() = m_Peer.GetServiceProviders(True)

            ' For each service provider in the returned list...
            Dim info As ServiceProviderInformation
            For Each info In SPInfoArray
                ' Compare the current provider against the passed provider
                If info.Guid.Equals(provider) Then
                    Return True
                End If
            Next info
            ' Not found
            Return False
        End Function 'IsServiceProviderValid


        '/ <summary>
        '/ Update visual elements according to current state
        '/ </summary>
        Public Sub UpdateUI()
            ' Flush the incoming/outgoing messages box
            m_Form.SendTextBox.Text = ""
            m_Form.ReceivedMessagesListBox.Items.Clear()

            ' Set UI elements based on current connection state
            Select Case m_Connection
                Case ConnectionType.Hosting
                    m_Form.SessionStatusLabel.Text = "Hosting session """ + m_SessionName + """"
                    m_Form.HostButton.Text = "&Disconnect"

                    m_Form.ReceivedMessagesListBox.Enabled = True
                    m_Form.SendTextBox.Enabled = True
                    m_Form.SendButton.Enabled = True
                    m_Form.ConnectButton.Enabled = False

                Case ConnectionType.Connected
                    m_Form.SessionStatusLabel.Text = "Connected to session """ + m_SessionName + """"
                    m_Form.HostButton.Text = "&Disconnect"

                    m_Form.ReceivedMessagesListBox.Enabled = True
                    m_Form.SendTextBox.Enabled = True
                    m_Form.SendButton.Enabled = True
                    m_Form.ConnectButton.Enabled = False

                Case ConnectionType.Disconnected
                    m_Form.SessionStatusLabel.Text = "Not connected to a session"
                    m_Form.HostButton.Text = "&Host..."
                    m_Form.ReceivedMessagesListBox.Items.Add("You must first connect to a session.")

                    m_Form.ReceivedMessagesListBox.Enabled = False
                    m_Form.SendTextBox.Enabled = False
                    m_Form.SendButton.Enabled = False
                    m_Form.ConnectButton.Enabled = True

            End Select
        End Sub 'UpdateUI


        '/ <summary>
        '/ Handles program cleanup
        '/ </summary>
        Protected Overridable Overloads Sub Dispose() Implements IDisposable.Dispose
            If Not (m_Peer Is Nothing) And Not m_Peer.Disposed Then
                m_Peer.Dispose()
            End If
        End Sub 'Dispose

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Public Shared Sub Main()
            ' Create the main application object
            Dim App As New LobbyLaunchApp()

            ' Start the form's message loop
            If Not App.m_Form.IsDisposed Then
                Application.Run(App.m_Form)
            End If
            ' Release resources
            App.Dispose()
        End Sub 'Main
    End Class 'LobbyLaunchApp
End Namespace 'Tut07_LobbyLaunch