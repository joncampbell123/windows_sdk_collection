'----------------------------------------------------------------------------
' File: Send.cs
'
' Desc: This tutorial adds the ability to send and receive data between
'       connected peers.
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

Namespace Tut05_Send
    ' Possible connection states
    Public Enum ConnectionType
        Disconnected
        Hosting
        Connected
    End Enum 'ConnectionType

    '/ <summary>
    '/ Application class.
    '/ </summary>
    Public Class SendApp
        Implements IDisposable

        ' Application identifier
        ' This guid allows DirectPlay to find other instances of the same game on
        ' the network, so it must be unique for every game, and the same for 
        ' every instance of that game. You can use the guidgen.exe program to
        ' generate a new guid.
        Private Shared m_AppGuid As New Guid("590E16DD-538F-40fb-A7DF-9654EA6BE71E")
        Public Shared DefaultPort As Integer = 2605 ' Default port number

        ' DirectPlay
        Private m_Peer As Peer = Nothing ' DirectPlay Peer object
        Private m_LocalAddress As New Address() ' Local address
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

            ' Set the service provider for our local address to TCP/IP
            m_LocalAddress.ServiceProvider = Address.ServiceProviderTcpIp

            ' Verify the computer supports our chosen service provider
            If Not IsServiceProviderValid(m_LocalAddress.ServiceProvider) Then
                MessageBox.Show(m_Form, "You must have TCP/IP installed on your " + "computer to run this tutorial.", "DirectPlay Tutorial", MessageBoxButtons.OK, MessageBoxIcon.Error)

                m_Form.Dispose()
            End If
        End Sub      'SendApp

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
            AddHandler m_Peer.SessionTerminated, AddressOf SessionTerminatedHandler

            m_Connection = ConnectionType.Disconnected
        End Sub 'InitDirectPlay

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
        '/ Handler for incoming DirectPlay SessionTerminated events
        '/ </summary>
        Public Sub SessionTerminatedHandler(ByVal sender As Object, ByVal args As SessionTerminatedEventArgs)
            MessageBox.Show(m_Form, "Connect lost or host terminated session", "DirectPlay Tutorial")
            m_Connection = ConnectionType.Disconnected
            UpdateUI()
        End Sub 'SessionTerminatedHandler

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
        Overridable Overloads Sub Dispose() Implements IDisposable.Dispose
            If Not (m_Peer Is Nothing) And Not m_Peer.Disposed Then
                m_Peer.Dispose()
            End If
        End Sub 'Dispose

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Public Shared Sub Main()
            ' Create the main application object
            Dim App As New SendApp()

            ' Start the form's message loop
            If Not App.m_Form.IsDisposed Then
                Application.Run(App.m_Form)
            End If
            ' Release resources
            App.Dispose()
        End Sub 'Main
    End Class 'SendApp
End Namespace