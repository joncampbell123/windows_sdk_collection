'----------------------------------------------------------------------------
' File: Client.cs
'
' Desc: This tutorial modifes the Send tutorial to be a client/server
'       topology instead of peer to peer. This file contains the client
'       code.
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


Namespace Tut09_Client
    ' Possible connection states
    Public Enum ConnectionType
        Disconnected
        Connected
    End Enum 'ConnectionType

    '/ <summary>
    '/ Application class.
    '/ </summary>
    Public Class ClientApp
        Implements IDisposable 'ToDo: Add Implements Clauses for implementation methods of these interface(s)
        ' Application identifier
        ' This guid allows DirectPlay to find other instances of the same game on
        ' the network, so it must be unique for every game, and the same for 
        ' every instance of that game. You can use the guidgen.exe program to
        ' generate a new guid.
        Private Shared m_AppGuid As New Guid("1AD4CA3B-AC68-4d9b-9522-BE59CD485276")
        Public Shared DefaultPort As Integer = 2609 ' Default port number

        ' DirectPlay
        Private m_Client As Microsoft.DirectX.DirectPlay.Client = Nothing ' DirectPlay Client object
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
        End Sub 'New

        '/ <summary>
        '/ Initializes the DirectPlay Client object
        '/ </summary>
        Private Sub InitDirectPlay()
            ' Release any exising resources
            If Not (m_Client Is Nothing) Then
                m_Client.Dispose()
            End If
            ' Create a new DirectPlay Client object
            m_Client = New Microsoft.DirectX.DirectPlay.Client()

            ' Add handlers for DirectPlay events
            AddHandler m_Client.FindHostResponse, AddressOf FindHostResponseHandler
            AddHandler m_Client.Receive, AddressOf ReceiveHandler
            AddHandler m_Client.SessionTerminated, AddressOf SessionTerminatedHandler

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
                m_Client.FindHosts(AppDesc, HostAddress, m_LocalAddress, Nothing, 0, 0, 0, FindHostsFlags.Sync) ' Application description
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
                m_Client.Connect(appDesc, SelectedHost.HostAddress, m_LocalAddress, Nothing, ConnectFlags.Sync) ' Application description
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
        '/ Sends the current outgoing chat string to all connected players
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
            ' client/server topology, all data is sent between server and
            ' clients, there is no need to specify a destination player.
            m_Client.Send(packet, 0, SendFlags.Sync Or SendFlags.NoLoopback) ' Outgoing data
            ' Timeout (default)        
            ' Flags

            m_Form.SendTextBox.Text = ""
        End Sub 'SendData


        '/ <summary>
        '/ Terminate or disconnect from the session.
        '/ </summary>
        Public Sub Disconnect()
            ' Disconnect by closing the current client and opening
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
            Dim SPInfoArray As ServiceProviderInformation() = m_Client.GetServiceProviders(True)

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
                Case ConnectionType.Connected
                    m_Form.SessionStatusLabel.Text = "Connected to session """ + m_SessionName + """"
                    m_Form.ConnectButton.Text = "&Disconnect"

                    m_Form.ReceivedMessagesListBox.Enabled = True
                    m_Form.SendTextBox.Enabled = True
                    m_Form.SendButton.Enabled = True

                Case ConnectionType.Disconnected
                    m_Form.SessionStatusLabel.Text = "Not connected to a session"
                    m_Form.ConnectButton.Text = "&Connect..."

                    m_Form.ReceivedMessagesListBox.Items.Add("You must first connect to a session.")

                    m_Form.ReceivedMessagesListBox.Enabled = False
                    m_Form.SendTextBox.Enabled = False
                    m_Form.SendButton.Enabled = False

            End Select
        End Sub 'UpdateUI


        '/ <summary>
        '/ Handles program cleanup
        '/ </summary>
        Protected Overridable Overloads Sub Dispose() Implements IDisposable.Dispose
            If Not (m_Client Is Nothing) And Not m_Client.Disposed Then
                m_Client.Dispose()
            End If
        End Sub 'Dispose

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Public Shared Sub Main()
            ' Create the main application object
            Dim App As New ClientApp()

            ' Start the form's message loop
            If Not App.m_Form.IsDisposed Then
                Application.Run(App.m_Form)
            End If
            ' Release resources
            App.Dispose()
        End Sub 'Main
    End Class 'ClientApp
End Namespace 'Tut09_Client