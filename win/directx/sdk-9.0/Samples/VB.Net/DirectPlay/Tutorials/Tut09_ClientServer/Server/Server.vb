'----------------------------------------------------------------------------
' File: Server.cs
'
' Desc: This tutorial modifes the Send tutorial to be a client/server
'       topology instead of peer to peer. This file contains the server
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

Namespace Tut09_Server
    ' Possible connection states
    Public Enum ConnectionType
        Disconnected
        Hosting
    End Enum 'ConnectionType

    '/ <summary>
    '/ Application class.
    '/ </summary>
    Public Class ServerApp
        Implements IDisposable
        ' Application identifier
        ' This guid allows DirectPlay to find other instances of the same game on
        ' the network, so it must be unique for every game, and the same for 
        ' every instance of that game. You can use the guidgen.exe program to
        ' generate a new guid.
        Private Shared m_AppGuid As New Guid("1AD4CA3B-AC68-4d9b-9522-BE59CD485276")
        Public Shared DefaultPort As Integer = 2609 ' Default port number

        ' DirectPlay
        Private m_Server As Microsoft.DirectX.DirectPlay.Server = Nothing ' DirectPlay Server object
        Private m_LocalAddress As New Address() ' Local address
        ' Application 
        Private m_Form As ApplicationForm = Nothing ' Main application WinForm
        Private m_SessionName As String = "New Host" ' Hosted session name 
        Private m_Connection As ConnectionType = ConnectionType.Disconnected ' Current connection state     

        Public ReadOnly Property Connection() As ConnectionType
            Get
                Return m_Connection
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
        '/ Initializes the DirectPlay Server object
        '/ </summary>
        Private Sub InitDirectPlay()
            ' Release any exising resources
            If Not (m_Server Is Nothing) Then
                m_Server.Dispose()
            End If
            ' Create a new DirectPlay Server object
            m_Server = New Microsoft.DirectX.DirectPlay.Server()

            ' Add handlers for DirectPlay events
            AddHandler m_Server.Receive, AddressOf ReceiveHandler

            m_Connection = ConnectionType.Disconnected
        End Sub 'InitDirectPlay

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
            AppDesc.Flags = SessionFlags.ClientServer Or SessionFlags.NoDpnServer

            Try
                ' Host a new session
                m_Server.Host(AppDesc, m_LocalAddress) ' Application description 
                ' Local device address
                m_Connection = ConnectionType.Hosting
            Catch ex As Exception
                m_Form.ShowException(ex, "Host", True)
                m_Form.Dispose()
                Return
            End Try
        End Sub 'HostSession

        '/ <summary>
        '/ Sends the current outgoing chat string to all connected clients
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
            ' packet, the DirectPlay send method can be called. You must tell
            ' DirectPlay which client you wish to receive the data; for this
            ' sample, we'll deliver the message to all connected players
            ' (except the local player, which we exclude with the NoLoopback
            ' flag )
            m_Server.SendTo(CInt(PlayerID.AllPlayers), packet, 0, SendFlags.Sync Or SendFlags.NoLoopback) ' Send to session
            ' Outgoing data
            ' Timeout (default)        
            ' Flags

            m_Form.SendTextBox.Text = ""
        End Sub 'SendData

        '/ <summary>
        '/ Terminate or disconnect from the session.
        '/ </summary>
        Public Sub Disconnect()
            ' Disconnect by closing the current server and opening
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
            Dim SPInfoArray As ServiceProviderInformation() = m_Server.GetServiceProviders(True)

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

                Case ConnectionType.Disconnected
                    m_Form.SessionStatusLabel.Text = "Not connected to a session"
                    m_Form.HostButton.Text = "&Host..."
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
            If Not (m_Server Is Nothing) And Not m_Server.Disposed Then
                m_Server.Dispose()
            End If
        End Sub 'Dispose

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Public Shared Sub Main()
            ' Create the main application object
            Dim App As New ServerApp()

            ' Start the form's message loop
            If Not App.m_Form.IsDisposed Then
                Application.Run(App.m_Form)
            End If
            ' Release resources
            App.Dispose()
        End Sub 'Main
    End Class 'ServerApp
End Namespace 'Tut09_Server