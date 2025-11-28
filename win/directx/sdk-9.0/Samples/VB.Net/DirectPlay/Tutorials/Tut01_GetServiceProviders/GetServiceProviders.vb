'----------------------------------------------------------------------------
' File: GetServiceProviders.cs
'
' Desc: This simple program inits DirectPlay and enumerates the available
'       DirectPlay Service Providers.
'
' Copyright (c) Microsoft Corp. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.IO
Imports System.Windows.Forms
Imports System.Collections
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay

Namespace Tut01_GetServiceProviders
    '/ <summary>
    '/ Application class.
    '/ </summary>
    Public Class GetServiceProvidersApp
        Implements IDisposable 'ToDo: Add Implements Clauses for implementation methods of these interface(s)

        ' DirectPlay
        Private m_Peer As Peer = Nothing ' DirectPlay Peer object
        ' Application 
        Private m_Form As ApplicationForm = Nothing
        ' Main application WinForm
        '/ <summary>
        '/ Constructor
        '/ </summary>
        Public Sub New()
            ' Initialize the application's UI
            m_Form = New ApplicationForm()

            ' Initialize DirectPlay
            InitDirectPlay()

            ' Enumerate and list the installed service providers
            ListServiceProviders()
        End Sub 'New
        '/ <summary>
        '/ Initializes the DirectPlay Peer object
        '/ </summary>
        Private Sub InitDirectPlay()
            ' Create a new DirectPlay Peer object
            m_Peer = New Peer()
        End Sub 'InitDirectPlay

        '/ <summary>
        '/ Query DirectPlay for the list of installed service providers on
        '/ this computer.
        '/ </summary>
        Public Sub ListServiceProviders()
            Dim SPInfoArray As ServiceProviderInformation() = Nothing

            Try
                ' Ask DirectPlay for the service provider list
                SPInfoArray = m_Peer.GetServiceProviders(True)
            Catch ex As Exception
                m_Form.ShowException(ex, "GetServiceProviders", True)
                m_Form.Dispose()
                Return
            End Try

            ' For each service provider in the returned list...
            Dim info As ServiceProviderInformation
            For Each info In SPInfoArray
                ' Add the service provider's name to the UI listbox
                m_Form.SPListBox.Items.Add(info.Name)
            Next info
        End Sub 'ListServiceProviders

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
            Dim App As New GetServiceProvidersApp()

            ' Start the form's message loop
            If Not App.m_Form.IsDisposed Then
                Application.Run(App.m_Form)
            End If
            ' Release resources
            App.Dispose()
        End Sub 'Main
    End Class 'GetServiceProvidersApp
End Namespace 'Tut01_GetServiceProviders