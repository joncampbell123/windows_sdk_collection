Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectPlay

Namespace Tut05_Send

    '/ <summary>
    '/ Summary description for ConnectDialog.
    '/ </summary>
    Public Class ConnectDialog
        Inherits System.Windows.Forms.Form
        Private App As SendApp = Nothing ' Application instance
        Private m_SelectedHost As HostInfo = Nothing ' Host selected for connection
        Private label1 As System.Windows.Forms.Label
        Private label2 As System.Windows.Forms.Label
        Private pictureBox1 As System.Windows.Forms.PictureBox
        Private label3 As System.Windows.Forms.Label
        Private label4 As System.Windows.Forms.Label
        Private Shadows WithEvents cancelButton As System.Windows.Forms.Button
        Private WithEvents ConnectButton As System.Windows.Forms.Button
        Private WithEvents SearchButton As System.Windows.Forms.Button
        Private SearchAddressTextBox As System.Windows.Forms.TextBox
        Private WithEvents DetectedSessionsListBox As System.Windows.Forms.ListBox

        ' Properties

        Public ReadOnly Property SelectedHost() As HostInfo
            Get
                Return m_SelectedHost
            End Get
        End Property '/ <summary>
        '/ Required designer variable.
        '/ </summary>

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

        Private components As System.ComponentModel.Container = Nothing


        Public Sub New(ByVal app As SendApp)
            Me.App = app

            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            '
            ' TODO: Add any constructor code after InitializeComponent call
            '
            SearchAddressTextBox.Text = "localhost"
            DetectedSessionsListBox.Enabled = False
            ConnectButton.Enabled = False
            DetectedSessionsListBox.Items.Clear()
            DetectedSessionsListBox.Items.Add("Click ""Search"" to find hosts.")
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
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Friend WithEvents RemotePortTextBox As System.Windows.Forms.TextBox
        Friend WithEvents remotePortLabel As System.Windows.Forms.Label
        Private Sub InitializeComponent()
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.pictureBox1 = New System.Windows.Forms.PictureBox()
            Me.label3 = New System.Windows.Forms.Label()
            Me.label4 = New System.Windows.Forms.Label()
            Me.cancelButton = New System.Windows.Forms.Button()
            Me.ConnectButton = New System.Windows.Forms.Button()
            Me.SearchAddressTextBox = New System.Windows.Forms.TextBox()
            Me.SearchButton = New System.Windows.Forms.Button()
            Me.DetectedSessionsListBox = New System.Windows.Forms.ListBox()
            Me.RemotePortTextBox = New System.Windows.Forms.TextBox()
            Me.remotePortLabel = New System.Windows.Forms.Label()
            Me.SuspendLayout()
            '
            'label1
            '
            Me.label1.BackColor = System.Drawing.SystemColors.Info
            Me.label1.Location = New System.Drawing.Point(24, 16)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(312, 16)
            Me.label1.TabIndex = 0
            Me.label1.Text = "Find hosts by using the search field below. Select a detected"
            '
            'label2
            '
            Me.label2.BackColor = System.Drawing.SystemColors.Info
            Me.label2.Location = New System.Drawing.Point(24, 32)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(312, 16)
            Me.label2.TabIndex = 1
            Me.label2.Text = "host and click ""Connect"" to join the session."
            '
            'pictureBox1
            '
            Me.pictureBox1.BackColor = System.Drawing.SystemColors.Info
            Me.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.pictureBox1.Location = New System.Drawing.Point(8, 8)
            Me.pictureBox1.Name = "pictureBox1"
            Me.pictureBox1.Size = New System.Drawing.Size(336, 48)
            Me.pictureBox1.TabIndex = 2
            Me.pictureBox1.TabStop = False
            '
            'label3
            '
            Me.label3.Location = New System.Drawing.Point(40, 72)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(100, 16)
            Me.label3.TabIndex = 3
            Me.label3.Text = "Search Address:"
            '
            'label4
            '
            Me.label4.Location = New System.Drawing.Point(40, 120)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(120, 16)
            Me.label4.TabIndex = 4
            Me.label4.Text = "Detected Sessions:"
            '
            'cancelButton
            '
            Me.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.cancelButton.Location = New System.Drawing.Point(192, 216)
            Me.cancelButton.Name = "cancelButton"
            Me.cancelButton.Size = New System.Drawing.Size(72, 24)
            Me.cancelButton.TabIndex = 5
            Me.cancelButton.Text = "&Cancel"
            '
            'ConnectButton
            '
            Me.ConnectButton.Location = New System.Drawing.Point(272, 216)
            Me.ConnectButton.Name = "ConnectButton"
            Me.ConnectButton.Size = New System.Drawing.Size(72, 24)
            Me.ConnectButton.TabIndex = 6
            Me.ConnectButton.Text = "C&onnect"
            '
            'SearchAddressTextBox
            '
            Me.SearchAddressTextBox.AutoSize = False
            Me.SearchAddressTextBox.Location = New System.Drawing.Point(40, 88)
            Me.SearchAddressTextBox.Name = "SearchAddressTextBox"
            Me.SearchAddressTextBox.Size = New System.Drawing.Size(136, 24)
            Me.SearchAddressTextBox.TabIndex = 7
            Me.SearchAddressTextBox.Text = "localhost"
            '
            'SearchButton
            '
            Me.SearchButton.Location = New System.Drawing.Point(232, 88)
            Me.SearchButton.Name = "SearchButton"
            Me.SearchButton.Size = New System.Drawing.Size(72, 24)
            Me.SearchButton.TabIndex = 8
            Me.SearchButton.Text = "&Search"
            '
            'DetectedSessionsListBox
            '
            Me.DetectedSessionsListBox.Location = New System.Drawing.Point(40, 136)
            Me.DetectedSessionsListBox.Name = "DetectedSessionsListBox"
            Me.DetectedSessionsListBox.Size = New System.Drawing.Size(264, 56)
            Me.DetectedSessionsListBox.TabIndex = 9
            '
            'RemotePortTextBox
            '
            Me.RemotePortTextBox.AutoSize = False
            Me.RemotePortTextBox.Location = New System.Drawing.Point(182, 88)
            Me.RemotePortTextBox.Name = "RemotePortTextBox"
            Me.RemotePortTextBox.Size = New System.Drawing.Size(40, 24)
            Me.RemotePortTextBox.TabIndex = 20
            Me.RemotePortTextBox.Text = ""
            '
            'remotePortLabel
            '
            Me.remotePortLabel.Location = New System.Drawing.Point(182, 72)
            Me.remotePortLabel.Name = "remotePortLabel"
            Me.remotePortLabel.Size = New System.Drawing.Size(40, 16)
            Me.remotePortLabel.TabIndex = 19
            Me.remotePortLabel.Text = "Port:"
            '
            'ConnectDialog
            '
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(352, 246)
            Me.ControlBox = False
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.RemotePortTextBox, Me.remotePortLabel, Me.DetectedSessionsListBox, Me.SearchButton, Me.SearchAddressTextBox, Me.ConnectButton, Me.cancelButton, Me.label4, Me.label3, Me.label2, Me.label1, Me.pictureBox1})
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "ConnectDialog"
            Me.ShowInTaskbar = False
            Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
            Me.Text = "Connect to Session"
            Me.ResumeLayout(False)

        End Sub 'InitializeComponent

        '/ <summary>
        '/ Handler for Search button click
        '/ </summary>
        Private Sub SearchButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles SearchButton.Click
            SearchButton.Enabled = False
            Me.Cursor = Cursors.WaitCursor

            ' Clear the current UI list
            DetectedSessionsListBox.Enabled = False
            DetectedSessionsListBox.Items.Clear()

            ' Have the application perform the enumeration
            App.EnumerateSessions(SearchAddressTextBox.Text, RemotePort)

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
            Me.Cursor = Cursors.Default
            SearchButton.Enabled = True
        End Sub 'SearchButton_Click

        '/ <summary>
        '/ Handler for Connect button click
        '/ </summary>
        Private Sub ConnectButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ConnectButton.Click
            ' Save the current settings
            m_SelectedHost = CType(DetectedSessionsListBox.SelectedItem, HostInfo)
            DialogResult = DialogResult.OK
        End Sub 'ConnectButton_Click

        '/ <summary>
        '/ Handler for Cancel button click
        '/ </summary>
        Private Sub cancelButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles cancelButton.Click
            ' Discard the settings
            DialogResult = DialogResult.Cancel
        End Sub 'cancelButton_Click

        '/ <summary>
        '/ Handler for Detected Sessions selection change
        '/ </summary>
        Private Sub DetectedSessionsListBox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles DetectedSessionsListBox.SelectedIndexChanged
            ConnectButton.Enabled = DetectedSessionsListBox.SelectedIndex <> -1
        End Sub 'DetectedSessionsListBox_SelectedIndexChanged
    End Class 'ConnectDialog

End Namespace