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

Namespace Tut01_GetServiceProviders
 _
    '/ <summary>
    '/ Application's main WinForm
    '/ </summary>
    Public Class ApplicationForm
        Inherits System.Windows.Forms.Form
        Private pictureBox1 As System.Windows.Forms.PictureBox
        Private WithEvents ExitButton As System.Windows.Forms.Button
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing
        Private label4 As System.Windows.Forms.Label
        Private label3 As System.Windows.Forms.Label
        Private label6 As System.Windows.Forms.Label
        Private label7 As System.Windows.Forms.Label
        Public SPListBox As System.Windows.Forms.ListBox

        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()
        End Sub

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
            Me.pictureBox1 = New System.Windows.Forms.PictureBox()
            Me.label4 = New System.Windows.Forms.Label()
            Me.SPListBox = New System.Windows.Forms.ListBox()
            Me.label3 = New System.Windows.Forms.Label()
            Me.label6 = New System.Windows.Forms.Label()
            Me.label7 = New System.Windows.Forms.Label()
            Me.SuspendLayout()
            ' 
            ' ExitButton
            ' 
            Me.ExitButton.Location = New System.Drawing.Point(264, 264)
            Me.ExitButton.Name = "ExitButton"
            Me.ExitButton.Size = New System.Drawing.Size(64, 24)
            Me.ExitButton.TabIndex = 1
            Me.ExitButton.Text = "E&xit"
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
            ' label4
            ' 
            Me.label4.Location = New System.Drawing.Point(8, 96)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(144, 16)
            Me.label4.TabIndex = 11
            Me.label4.Text = "Detected Service Providers:"
            ' 
            ' SPListBox
            ' 
            Me.SPListBox.Location = New System.Drawing.Point(8, 112)
            Me.SPListBox.Name = "SPListBox"
            Me.SPListBox.SelectionMode = System.Windows.Forms.SelectionMode.None
            Me.SPListBox.Size = New System.Drawing.Size(320, 134)
            Me.SPListBox.TabIndex = 10
            ' 
            ' label3
            ' 
            Me.label3.BackColor = System.Drawing.SystemColors.Info
            Me.label3.Location = New System.Drawing.Point(20, 48)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(296, 16)
            Me.label3.TabIndex = 14
            Me.label3.Text = "protocol to be used for network communication."
            ' 
            ' label6
            ' 
            Me.label6.BackColor = System.Drawing.SystemColors.Info
            Me.label6.Location = New System.Drawing.Point(20, 32)
            Me.label6.Name = "label6"
            Me.label6.Size = New System.Drawing.Size(296, 16)
            Me.label6.TabIndex = 13
            Me.label6.Text = "found on your computer. A service provider defines the"
            ' 
            ' label7
            ' 
            Me.label7.BackColor = System.Drawing.SystemColors.Info
            Me.label7.Location = New System.Drawing.Point(20, 16)
            Me.label7.Name = "label7"
            Me.label7.Size = New System.Drawing.Size(296, 16)
            Me.label7.TabIndex = 12
            Me.label7.Text = "This tutorial lists the available DirectPlay service providers"
            ' 
            ' ApplicationForm
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(336, 296)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.label3, Me.label6, Me.label7, Me.label4, Me.SPListBox, Me.ExitButton, Me.pictureBox1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "ApplicationForm"
            Me.Text = "Tutorial 1: Get Service Providers"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '/ <summary>
        '/ Handler for Exit button click
        '/ </summary>
        Private Sub ExitButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ExitButton.Click
            Dispose()
        End Sub 'ExitButton_Click
    End Class 'ApplicationForm
End Namespace
