'-----------------------------------------------------------------------------
' File: DPlayConnect_AddressForm.vb
'
' Desc: Application class for the DirectPlay samples framework.
'
' Copyright (c) Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Collections
Imports System.Windows.Forms
Imports System.Threading
Imports System.Timers
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay
Imports System.Runtime.InteropServices

Public Class AddressForm
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New(ByVal defaultPort As Integer)
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Set the default port value
        Port = defaultPort

    End Sub


    '/ <summary>
    '/ The remote port on which to connect
    '/ </summary>
    Public Property Port() As Integer
        Get
            Dim tempPort As Integer = 0
            Try
                tempPort = Integer.Parse(portTextBox.Text)
            Catch
            End Try

            Return tempPort
        End Get
        Set(ByVal Value As Integer)
            If Value > 0 Then
                portTextBox.Text = Value.ToString()
            End If
        End Set
    End Property



    '/ <summary>
    '/ Remote hostname
    '/ </summary>
    Public Property Hostname() As String
        Get
            Return hostnameTextBox.Text
        End Get
        Set(ByVal Value As String)
            hostnameTextBox.Text = Value
        End Set
    End Property


    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents portTextBox As System.Windows.Forms.TextBox
    Friend WithEvents hostnameTextBox As System.Windows.Forms.TextBox
    Friend WithEvents label3 As System.Windows.Forms.Label
    Friend WithEvents label2 As System.Windows.Forms.Label
    Friend WithEvents okButton As System.Windows.Forms.Button
    Friend WithEvents label1 As System.Windows.Forms.Label
    Friend WithEvents cancelBtn As System.Windows.Forms.Button
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.portTextBox = New System.Windows.Forms.TextBox()
        Me.hostnameTextBox = New System.Windows.Forms.TextBox()
        Me.label3 = New System.Windows.Forms.Label()
        Me.label2 = New System.Windows.Forms.Label()
        Me.cancelBtn = New System.Windows.Forms.Button()
        Me.okButton = New System.Windows.Forms.Button()
        Me.label1 = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        'portTextBox
        '
        Me.portTextBox.Location = New System.Drawing.Point(160, 96)
        Me.portTextBox.Name = "portTextBox"
        Me.portTextBox.Size = New System.Drawing.Size(56, 20)
        Me.portTextBox.TabIndex = 13
        Me.portTextBox.Text = ""
        '
        'hostnameTextBox
        '
        Me.hostnameTextBox.Location = New System.Drawing.Point(32, 96)
        Me.hostnameTextBox.Name = "hostnameTextBox"
        Me.hostnameTextBox.Size = New System.Drawing.Size(120, 20)
        Me.hostnameTextBox.TabIndex = 12
        Me.hostnameTextBox.Text = ""
        '
        'label3
        '
        Me.label3.Location = New System.Drawing.Point(160, 80)
        Me.label3.Name = "label3"
        Me.label3.Size = New System.Drawing.Size(56, 16)
        Me.label3.TabIndex = 11
        Me.label3.Text = "Port"
        '
        'label2
        '
        Me.label2.Location = New System.Drawing.Point(32, 80)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(100, 16)
        Me.label2.TabIndex = 10
        Me.label2.Text = "Hostname"
        '
        'cancelBtn
        '
        Me.cancelBtn.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.cancelBtn.Location = New System.Drawing.Point(184, 144)
        Me.cancelBtn.Name = "cancelBtn"
        Me.cancelBtn.Size = New System.Drawing.Size(72, 24)
        Me.cancelBtn.TabIndex = 9
        Me.cancelBtn.Text = "&Cancel"
        '
        'okButton
        '
        Me.okButton.Location = New System.Drawing.Point(104, 144)
        Me.okButton.Name = "okButton"
        Me.okButton.Size = New System.Drawing.Size(72, 24)
        Me.okButton.TabIndex = 8
        Me.okButton.Text = "&OK"
        '
        'label1
        '
        Me.label1.Location = New System.Drawing.Point(16, 16)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(248, 48)
        Me.label1.TabIndex = 7
        Me.label1.Text = "Please enter the optional components of the remote session address. If set blank," & _
        " DirectPlay will attempt to search the local network."
        '
        'AddressForm
        '
        Me.AcceptButton = Me.okButton
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.CancelButton = Me.cancelBtn
        Me.ClientSize = New System.Drawing.Size(272, 182)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.portTextBox, Me.hostnameTextBox, Me.label3, Me.label2, Me.cancelBtn, Me.okButton, Me.label1})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "AddressForm"
        Me.Text = "Remote Address"
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Sub okButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles okButton.Click
        DialogResult = DialogResult.OK
    End Sub

    Private Sub cancelBtn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cancelBtn.Click
        DialogResult = DialogResult.Cancel
    End Sub
End Class
