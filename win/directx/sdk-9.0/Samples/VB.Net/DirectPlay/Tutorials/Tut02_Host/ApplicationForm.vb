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

Namespace Tut02_Host
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
        Private label5 As System.Windows.Forms.Label

        Private App As HostApp

        Public Sub New(ByVal app As HostApp)
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
                output = "An error occurred while calling " + calling + ControlChars.Lf + ControlChars.Lf
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
            Me.SessionStatusLabel = New System.Windows.Forms.Label()
            Me.HostButton = New System.Windows.Forms.Button()
            Me.label5 = New System.Windows.Forms.Label()
            Me.groupBox1.SuspendLayout()
            Me.SuspendLayout()
            ' 
            ' ExitButton
            ' 
            Me.ExitButton.Location = New System.Drawing.Point(264, 208)
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
            Me.label1.Text = "This tutorial creates an address object describing the local"
            ' 
            ' label2
            ' 
            Me.label2.BackColor = System.Drawing.SystemColors.Info
            Me.label2.Location = New System.Drawing.Point(19, 32)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(296, 16)
            Me.label2.TabIndex = 3
            Me.label2.Text = "network adapter. Using this address object, the user can"
            ' 
            ' pictureBox1
            ' 
            Me.pictureBox1.BackColor = System.Drawing.SystemColors.Info
            Me.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.pictureBox1.Location = New System.Drawing.Point(8, 8)
            Me.pictureBox1.Name = "pictureBox1"
            Me.pictureBox1.Size = New System.Drawing.Size(320, 64)
            Me.pictureBox1.TabIndex = 6
            Me.pictureBox1.TabStop = False
            ' 
            ' groupBox1
            ' 
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.SessionStatusLabel, Me.HostButton})
            Me.groupBox1.Location = New System.Drawing.Point(8, 96)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(320, 96)
            Me.groupBox1.TabIndex = 7
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Session Status"
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
            Me.HostButton.Text = "&Host"
            ' 
            ' label5
            ' 
            Me.label5.BackColor = System.Drawing.SystemColors.Info
            Me.label5.Location = New System.Drawing.Point(19, 48)
            Me.label5.Name = "label5"
            Me.label5.Size = New System.Drawing.Size(296, 16)
            Me.label5.TabIndex = 9
            Me.label5.Text = "create and host a new DirectPlay session."
            ' 
            ' ApplicationForm
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(336, 240)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.label5, Me.groupBox1, Me.label2, Me.label1, Me.ExitButton, Me.pictureBox1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "ApplicationForm"
            Me.Text = "Tutorial 2: Host"
            Me.groupBox1.ResumeLayout(False)
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
    End Class 'ApplicationForm

End Namespace