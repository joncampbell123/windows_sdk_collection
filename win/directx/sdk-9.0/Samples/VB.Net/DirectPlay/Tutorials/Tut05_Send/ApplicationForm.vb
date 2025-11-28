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

Namespace Tut05_Send
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
        Private label4 As System.Windows.Forms.Label
        Public WithEvents SendButton As System.Windows.Forms.Button
        Public SendTextBox As System.Windows.Forms.TextBox
        Public ReceivedMessagesListBox As System.Windows.Forms.ListBox
        Public WithEvents ConnectButton As System.Windows.Forms.Button

        Private App As SendApp

        Public Sub New(ByVal app As SendApp)
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
        Private Sub InitializeComponent()
            Dim resources As New System.Resources.ResourceManager(GetType(ApplicationForm))
            Me.ExitButton = New System.Windows.Forms.Button()
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.pictureBox1 = New System.Windows.Forms.PictureBox()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.ConnectButton = New System.Windows.Forms.Button()
            Me.SessionStatusLabel = New System.Windows.Forms.Label()
            Me.HostButton = New System.Windows.Forms.Button()
            Me.groupBox2 = New System.Windows.Forms.GroupBox()
            Me.SendButton = New System.Windows.Forms.Button()
            Me.SendTextBox = New System.Windows.Forms.TextBox()
            Me.ReceivedMessagesListBox = New System.Windows.Forms.ListBox()
            Me.label4 = New System.Windows.Forms.Label()
            Me.groupBox1.SuspendLayout()
            Me.groupBox2.SuspendLayout()
            Me.SuspendLayout()
            ' 
            ' ExitButton
            ' 
            Me.ExitButton.Location = New System.Drawing.Point(264, 376)
            Me.ExitButton.Name = "ExitButton"
            Me.ExitButton.Size = New System.Drawing.Size(64, 24)
            Me.ExitButton.TabIndex = 1
            Me.ExitButton.Text = "E&xit"
            ' 
            ' label1
            ' 
            Me.label1.BackColor = System.Drawing.SystemColors.Info
            Me.label1.Location = New System.Drawing.Point(19, 16)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(304, 16)
            Me.label1.TabIndex = 2
            Me.label1.Text = "This tutorial allows the user to send and receive information"
            ' 
            ' label2
            ' 
            Me.label2.BackColor = System.Drawing.SystemColors.Info
            Me.label2.Location = New System.Drawing.Point(19, 32)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(296, 16)
            Me.label2.TabIndex = 3
            Me.label2.Text = "between connected peers."
            ' 
            ' pictureBox1
            ' 
            Me.pictureBox1.BackColor = System.Drawing.SystemColors.Info
            Me.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.pictureBox1.Location = New System.Drawing.Point(8, 8)
            Me.pictureBox1.Name = "pictureBox1"
            Me.pictureBox1.Size = New System.Drawing.Size(320, 48)
            Me.pictureBox1.TabIndex = 6
            Me.pictureBox1.TabStop = False
            ' 
            ' groupBox1
            ' 
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.ConnectButton, Me.SessionStatusLabel, Me.HostButton})
            Me.groupBox1.Location = New System.Drawing.Point(8, 264)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(320, 96)
            Me.groupBox1.TabIndex = 7
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Session Status"
            ' 
            ' ConnectButton
            ' 
            Me.ConnectButton.Location = New System.Drawing.Point(112, 56)
            Me.ConnectButton.Name = "ConnectButton"
            Me.ConnectButton.Size = New System.Drawing.Size(80, 24)
            Me.ConnectButton.TabIndex = 2
            Me.ConnectButton.Text = "&Connect..."
            ' 
            ' SessionStatusLabel
            ' 
            Me.SessionStatusLabel.Location = New System.Drawing.Point(24, 24)
            Me.SessionStatusLabel.Name = "SessionStatusLabel"
            Me.SessionStatusLabel.Size = New System.Drawing.Size(288, 16)
            Me.SessionStatusLabel.TabIndex = 1
            Me.SessionStatusLabel.Text = "Not connected to a session."
            ' 
            ' HostButton
            ' 
            Me.HostButton.Location = New System.Drawing.Point(24, 56)
            Me.HostButton.Name = "HostButton"
            Me.HostButton.Size = New System.Drawing.Size(80, 24)
            Me.HostButton.TabIndex = 0
            Me.HostButton.Text = "&Host..."
            ' 
            ' groupBox2
            ' 
            Me.groupBox2.Controls.AddRange(New System.Windows.Forms.Control() {Me.SendButton, Me.SendTextBox, Me.ReceivedMessagesListBox, Me.label4})
            Me.groupBox2.Location = New System.Drawing.Point(8, 72)
            Me.groupBox2.Name = "groupBox2"
            Me.groupBox2.Size = New System.Drawing.Size(320, 184)
            Me.groupBox2.TabIndex = 8
            Me.groupBox2.TabStop = False
            Me.groupBox2.Text = "Communication"
            ' 
            ' SendButton
            ' 
            Me.SendButton.Location = New System.Drawing.Point(240, 144)
            Me.SendButton.Name = "SendButton"
            Me.SendButton.Size = New System.Drawing.Size(56, 24)
            Me.SendButton.TabIndex = 4
            Me.SendButton.Text = "&Send"
            ' 
            ' SendTextBox
            ' 
            Me.SendTextBox.AutoSize = False
            Me.SendTextBox.Location = New System.Drawing.Point(24, 144)
            Me.SendTextBox.Name = "SendTextBox"
            Me.SendTextBox.Size = New System.Drawing.Size(208, 24)
            Me.SendTextBox.TabIndex = 3
            Me.SendTextBox.Text = ""
            ' 
            ' ReceivedMessagesListBox
            ' 
            Me.ReceivedMessagesListBox.IntegralHeight = False
            Me.ReceivedMessagesListBox.Location = New System.Drawing.Point(24, 40)
            Me.ReceivedMessagesListBox.Name = "ReceivedMessagesListBox"
            Me.ReceivedMessagesListBox.Size = New System.Drawing.Size(272, 96)
            Me.ReceivedMessagesListBox.TabIndex = 2
            ' 
            ' label4
            ' 
            Me.label4.Location = New System.Drawing.Point(24, 24)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(128, 16)
            Me.label4.TabIndex = 0
            Me.label4.Text = "Received Messages:"
            ' 
            ' ApplicationForm
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(336, 408)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.groupBox2, Me.groupBox1, Me.label2, Me.label1, Me.ExitButton, Me.pictureBox1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "ApplicationForm"
            Me.Text = "Tutorial 5: Send"
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
            ConnectButton.Enabled = True
            App.UpdateUI()

        End Sub 'ConnectButton_Click

        '/ <summary>
        '/ Handler for Send button click
        '/ </summary>
        Private Sub SendButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles SendButton.Click
            App.SendData()
        End Sub 'SendButton_Click
    End Class 'ApplicationForm
End Namespace 'Tut05_Send