'----------------------------------------------------------------------------
' File: Host.cs
'
' Desc: This simple program builds upon the last tutorial by adding the 
'       creation of an address object and hosting a session.
'
' Copyright (c) Microsoft Corp. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.IO
Imports System.Windows.Forms
Imports System.Collections
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay

Namespace Tut02_Host
    ' Possible connection states
    Public Enum ConnectionType
        Disconnected
        Hosting
    End Enum 'ConnectionType
    '/ <summary>
    '/ Application class.
    '/ </summary>
    Public Class HostApp
        Implements IDisposable 
        ' Application identifier
        ' This guid allows DirectPlay to find other instances of the same game on
        ' the network, so it must be unique for every game, and the same for 
        ' every instance of that game. You can use the guidgen.exe program to
        ' generate a new guid.
        Private Shared m_AppGuid As New Guid("5e4ab2ee-6a50-4614-807e-c632807b5eb1")
        Public Shared DefaultPort As Integer = 2602

        ' DirectPlay
        Private m_Peer As Peer = Nothing ' DirectPlay Peer object
        Private m_LocalAddress As New Address() ' Local address
        ' Application 
        Private m_Form As ApplicationForm = Nothing ' Main application WinForm
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
        '/ Initializes the DirectPlay Peer object
        '/ </summary>
        Private Sub InitDirectPlay()
            ' Release any exising resources
            If Not (m_Peer Is Nothing) Then
                m_Peer.Dispose()
            End If
            ' Create a new DirectPlay Peer object
            m_Peer = New Peer()

            m_Connection = ConnectionType.Disconnected
        End Sub 'InitDirectPlay

        '/ <summary>
        '/ Host a new DirectPlay session.
        '/ </summary>
        Public Sub HostSession()
            ' Update the UI
            m_Form.SessionStatusLabel.Text = "Connecting..."
            m_Form.Update()

            ' Create an application description
            Dim AppDesc As New ApplicationDescription()
            AppDesc.GuidApplication = m_AppGuid
            AppDesc.Flags = SessionFlags.NoDpnServer

            ' Set the port number on which to host
            m_LocalAddress.AddComponent("port", DefaultPort)

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
            ' Set UI elements based on current connection state
            Select Case m_Connection
                Case ConnectionType.Hosting
                    m_Form.SessionStatusLabel.Text = "Hosting a session"
                    m_Form.HostButton.Text = "&Disconnect"

                Case ConnectionType.Disconnected
                    m_Form.SessionStatusLabel.Text = "Not connected to a session"
                    m_Form.HostButton.Text = "&Host"

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
            Dim App As New HostApp()

            ' Start the form's message loop
            If Not App.m_Form.IsDisposed Then
                Application.Run(App.m_Form)
            End If
            ' Release resources
            App.Dispose()
        End Sub 'Main
    End Class 'HostApp
End Namespace 'Tut02_Host