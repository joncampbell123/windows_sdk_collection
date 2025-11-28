'----------------------------------------------------------------------------
' File: ApplicationForm.cs
'
' Desc: The main WinForm for the application class.
'
' Copyright (c) Microsoft Corp. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX

Namespace Tut04_Connect
    '/ <summary>
    '/ Application's main WinForm
    '/ </summary>
    Public Class ApplicationForm
        Inherits System.Windows.Forms.Form
        Private label1 As System.Windows.Forms.Label
        Private label2 As System.Windows.Forms.Label
        Private pictureBox1 As System.Windows.Forms.PictureBox
        Private WithEvents ExitButton As System.Windows.Forms.Button
        Private groupBox1 As System.Windows.Forms.GroupBox
        Public WithEvents HostButton As System.Windows.Forms.Button
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing
        Public SessionStatusLabel As System.Windows.Forms.Label
        Private groupBox2 As System.Windows.Forms.GroupBox
        Public WithEvents DetectedSessionsListBox As System.Windows.Forms.ListBox
        Public WithEvents SearchButton As System.Windows.Forms.Button
        Public SearchAddressTextBox As System.Windows.Forms.TextBox
        Private label4 As System.Windows.Forms.Label
        Private label3 As System.Windows.Forms.Label
        Public WithEvents ConnectButton As System.Windows.Forms.Button

        Private App As ConnectApp

        ' Property: Remote port on which to connect
        Public Property RemotePort() As Integer
            Get
                Dim retValue As Integer

                Try
                    retValue = Integer.Parse(RemotePortTextBox.Text)
                Catch ex As Exception
                End Try

                Return retValue
            End Get

            Set(ByVal Value As Integer)
                RemotePortTextBox.Text = Value.ToString()
            End Set
        End Property


        Public Sub New(ByVal app As ConnectApp)
            Me.App = app
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()
        End Sub 'New

        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose

        '/ <summary>
        '/ Print information about the provided exception
        '/ </summary>
        '/ <param name="ex">Exception instance</param>
        '/ <param name="calling">Name of the method which returned the exception</param>
        '/ <param name="isFatal">Flag to indicate whether the given error is fatal</param>
        Public Sub ShowException(ByVal ex As Exception, ByVal calling As String, ByVal isFatal As Boolean)
            Dim output As String = Nothing

            If Not (calling Is Nothing) Then
                output = "An error occurred while calling """ + calling + ControlChars.Lf + ControlChars.Lf
            Else
                output = "An error occurred while executing the tutorial" + ControlChars.Lf + ControlChars.Lf
            End If
            output += "Message: " + ex.Message + ControlChars.Lf

            If TypeOf ex Is DirectXException Then
                ' DirectX-specific info
                Dim dex As DirectXException = CType(ex, DirectXException)
                output += "HRESULT: " + dex.ErrorString + " (" + dex.ErrorCode.ToString("X") + ")" + ControlChars.Lf
            End If

            output += "Source: " + ex.Source + ControlChars.Lf

            If isFatal Then
                output += ControlChars.Lf + "The application will now exit" + ControlChars.Lf
            End If
            MessageBox.Show(Me, output, "DirectPlay Tutorial", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Sub 'ShowException

        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Friend WithEvents RemotePortTextBox As System.Windows.Forms.TextBox
        Friend WithEvents remotePortLabel As System.Windows.Forms.Label
        Private Sub InitializeComponent()
            Me.ExitButton = New System.Windows.Forms.Button()
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.pictureBox1 = New System.Windows.Forms.PictureBox()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.SessionStatusLabel = New System.Windows.Forms.Label()
            Me.HostButton = New System.Windows.Forms.Button()
            Me.groupBox2 = New System.Windows.Forms.GroupBox()
            Me.ConnectButton = New System.Windows.Forms.Button()
            Me.DetectedSessionsListBox = New System.Windows.Forms.ListBox()
            Me.SearchButton = New System.Windows.Forms.Button()
            Me.SearchAddressTextBox = New System.Windows.Forms.TextBox()
            Me.label4 = New System.Windows.Forms.Label()
            Me.label3 = New System.Windows.Forms.Label()
            Me.RemotePortTextBox = New System.Windows.Forms.TextBox()
            Me.remotePortLabel = New System.Windows.Forms.Label()
            Me.groupBox1.SuspendLayout()
            Me.groupBox2.SuspendLayout()
            Me.SuspendLayout()
            '
            'ExitButton
            '
            Me.ExitButton.Location = New System.Drawing.Point(264, 376)
            Me.ExitButton.Name = "ExitButton"
            Me.ExitButton.Size = New System.Drawing.Size(64, 24)
            Me.ExitButton.TabIndex = 1
            Me.ExitButton.Text = "E&xit"
            '
            'label1
            '
            Me.label1.BackColor = System.Drawing.SystemColors.Info
            Me.label1.Location = New System.Drawing.Point(19, 16)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(304, 16)
            Me.label1.TabIndex = 2
            Me.label1.Text = "This tutorial adds the ability to connect to a session. Once"
            '
            'label2
            '
            Me.label2.BackColor = System.Drawing.SystemColors.Info
            Me.label2.Location = New System.Drawing.Point(19, 32)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(296, 16)
            Me.label2.TabIndex = 3
            Me.label2.Text = "connected, an application can send and receive data."
            '
            'pictureBox1
            '
            Me.pictureBox1.BackColor = System.Drawing.SystemColors.Info
            Me.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.pictureBox1.Location = New System.Drawing.Point(8, 8)
            Me.pictureBox1.Name = "pictureBox1"
            Me.pictureBox1.Size = New System.Drawing.Size(320, 48)
            Me.pictureBox1.TabIndex = 6
            Me.pictureBox1.TabStop = False
            '
            'groupBox1
            '
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.SessionStatusLabel, Me.HostButton})
            Me.groupBox1.Location = New System.Drawing.Point(8, 264)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(320, 96)
            Me.groupBox1.TabIndex = 7
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Session Status"
            '
            'SessionStatusLabel
            '
            Me.SessionStatusLabel.Location = New System.Drawing.Point(24, 24)
            Me.SessionStatusLabel.Name = "SessionStatusLabel"
            Me.SessionStatusLabel.Size = New System.Drawing.Size(288, 16)
            Me.SessionStatusLabel.TabIndex = 1
            Me.SessionStatusLabel.Text = "Not connected to a session."
            '
            'HostButton
            '
            Me.HostButton.Location = New System.Drawing.Point(24, 56)
            Me.HostButton.Name = "HostButton"
            Me.HostButton.Size = New System.Drawing.Size(80, 24)
            Me.HostButton.TabIndex = 0
            Me.HostButton.Text = "&Host..."
            '
            'groupBox2
            '
            Me.groupBox2.Controls.AddRange(New System.Windows.Forms.Control() {Me.RemotePortTextBox, Me.remotePortLabel, Me.ConnectButton, Me.DetectedSessionsListBox, Me.SearchButton, Me.SearchAddressTextBox, Me.label4, Me.label3})
            Me.groupBox2.Location = New System.Drawing.Point(8, 72)
            Me.groupBox2.Name = "groupBox2"
            Me.groupBox2.Size = New System.Drawing.Size(320, 184)
            Me.groupBox2.TabIndex = 8
            Me.groupBox2.TabStop = False
            Me.groupBox2.Text = "Host Search"
            '
            'ConnectButton
            '
            Me.ConnectButton.Enabled = False
            Me.ConnectButton.Location = New System.Drawing.Point(220, 152)
            Me.ConnectButton.Name = "ConnectButton"
            Me.ConnectButton.Size = New System.Drawing.Size(72, 24)
            Me.ConnectButton.TabIndex = 15
            Me.ConnectButton.Text = "C&onnect"
            '
            'DetectedSessionsListBox
            '
            Me.DetectedSessionsListBox.Location = New System.Drawing.Point(28, 88)
            Me.DetectedSessionsListBox.Name = "DetectedSessionsListBox"
            Me.DetectedSessionsListBox.Size = New System.Drawing.Size(264, 56)
            Me.DetectedSessionsListBox.TabIndex = 14
            '
            'SearchButton
            '
            Me.SearchButton.Location = New System.Drawing.Point(220, 40)
            Me.SearchButton.Name = "SearchButton"
            Me.SearchButton.Size = New System.Drawing.Size(72, 24)
            Me.SearchButton.TabIndex = 13
            Me.SearchButton.Text = "&Search"
            '
            'SearchAddressTextBox
            '
            Me.SearchAddressTextBox.AutoSize = False
            Me.SearchAddressTextBox.Location = New System.Drawing.Point(28, 40)
            Me.SearchAddressTextBox.Name = "SearchAddressTextBox"
            Me.SearchAddressTextBox.Size = New System.Drawing.Size(140, 24)
            Me.SearchAddressTextBox.TabIndex = 12
            Me.SearchAddressTextBox.Text = "localhost"
            '
            'label4
            '
            Me.label4.Location = New System.Drawing.Point(28, 72)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(120, 16)
            Me.label4.TabIndex = 11
            Me.label4.Text = "Detected Sessions:"
            '
            'label3
            '
            Me.label3.Location = New System.Drawing.Point(28, 24)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(100, 16)
            Me.label3.TabIndex = 10
            Me.label3.Text = "Search Address:"
            '
            'RemotePortTextBox
            '
            Me.RemotePortTextBox.AutoSize = False
            Me.RemotePortTextBox.Location = New System.Drawing.Point(172, 40)
            Me.RemotePortTextBox.Name = "RemotePortTextBox"
            Me.RemotePortTextBox.Size = New System.Drawing.Size(40, 24)
            Me.RemotePortTextBox.TabIndex = 18
            Me.RemotePortTextBox.Text = ""
            '
            'remotePortLabel
            '
            Me.remotePortLabel.Location = New System.Drawing.Point(172, 24)
            Me.remotePortLabel.Name = "remotePortLabel"
            Me.remotePortLabel.Size = New System.Drawing.Size(40, 16)
            Me.remotePortLabel.TabIndex = 17
            Me.remotePortLabel.Text = "Port:"
            '
            'ApplicationForm
            '
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(336, 408)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.groupBox2, Me.groupBox1, Me.label2, Me.label1, Me.ExitButton, Me.pictureBox1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "ApplicationForm"
            Me.Text = "Tutorial 4: Connect"
            Me.groupBox1.ResumeLayout(False)
            Me.groupBox2.ResumeLayout(False)
            Me.ResumeLayout(False)

        End Sub 'InitializeComponent

        '/ <summary>
        '/ Handler for Exit button click
        '/ </summary>
        Private Sub ExitButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ExitButton.Click
            Dispose()
        End Sub 'ExitButton_Click

        '/ <summary>
        '/ Handler for Host button click. If the application is currently hosting,
        '/ this button will display "Disconnect"
        '/ </summary>
        Private Sub HostButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles HostButton.Click
            HostButton.Enabled = False
            Me.Cursor = Cursors.WaitCursor

            If App.Connection = ConnectionType.Disconnected Then
                App.HostSession()
            Else
                App.Disconnect()
            End If
            HostButton.Enabled = True
            Me.Cursor = Cursors.Default
            App.UpdateUI()
        End Sub 'HostButton_Click

        '/ <summary>
        '/ Handler for Connect button click
        '/ </summary>
        Private Sub ConnectButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ConnectButton.Click
            ConnectButton.Enabled = False
            Me.Cursor = Cursors.WaitCursor

            App.ConnectToSession()

            Me.Cursor = Cursors.Default
            App.UpdateUI()
        End Sub 'ConnectButton_Click

        '/ <summary>
        '/ Handler for Search button click
        '/ </summary>
        Private Sub SearchButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles SearchButton.Click
            SearchButton.Enabled = False
            Me.Cursor = Cursors.WaitCursor

            ' Clear the current host list
            App.FoundSessions.Clear()

            ' Have the application perform the enumeration
            App.EnumerateSessions(SearchAddressTextBox.Text, RemotePort)

            Me.Cursor = Cursors.Default
            SearchButton.Enabled = True
            App.UpdateUI()

            DetectedSessionsListBox.Items.Clear()

            ' Add detected items to thelist
            Dim host As HostInfo
            For Each host In App.FoundSessions
                DetectedSessionsListBox.Items.Add(host)
                DetectedSessionsListBox.Enabled = True
            Next host

            ' Give default message for no hosts
            If App.FoundSessions.Count = 0 Then
                DetectedSessionsListBox.Items.Add("No hosts found.")
            End If
        End Sub 'SearchButton_Click

        '/ <summary>
        '/ Handler for Detected Sessions selection change
        '/ </summary>
        Private Sub DetectedSessionsListBox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles DetectedSessionsListBox.SelectedIndexChanged
            ConnectButton.Enabled = DetectedSessionsListBox.SelectedIndex <> -1
        End Sub 'DetectedSessionsListBox_SelectedIndexChanged
    End Class 'ApplicationForm
End Namespace 'Tut04_Connect