'-----------------------------------------------------------------------------
' File: DPlayConnect_ServiceProviderForm.vb
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

'/ <summary>
'/ This form allows you to choose a service provider and username for your 
'/ sample
'/ </summary>
Public Class ChooseServiceProviderForm
    Inherits System.Windows.Forms.Form
    Private peer As peer
    Private connectionWizard As ConnectWizard
    Private label1 As System.Windows.Forms.Label
    Private WithEvents lstSP As System.Windows.Forms.ListBox
    Private WithEvents btnOK As System.Windows.Forms.Button
    Private label2 As System.Windows.Forms.Label
    Private txtUser As System.Windows.Forms.TextBox
    Private WithEvents btnCancel As System.Windows.Forms.Button




    '/ <summary>
    '/ Constructor
    '/ </summary>
    Public Sub New(ByVal peerObject As peer, ByVal connectionWizard As ConnectWizard)
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()
        peer = peerObject
        Me.connectionWizard = connectionWizard
        Me.Text = connectionWizard.SampleName + " - " + Me.Text
        ' Fill up our listbox with the service providers
        Dim serviceProviders As ServiceProviderInformation() = peer.GetServiceProviders(False)
        Dim info As ServiceProviderInformation
        For Each info In serviceProviders
            lstSP.Items.Add(info)
        Next info
        txtUser.Text = Nothing
        'Get the default username from the registry if it exists
        Dim regKey As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("Software\Microsoft\DirectX\SDK\csDPlay")
        If Not (regKey Is Nothing) Then
            Try
                txtUser.Text = CStr(regKey.GetValue("DirectPlayUserName", Nothing))
                lstSP.SelectedIndex = CInt(regKey.GetValue("DirectPlaySPIndex", 0))
                regKey.Close()
            Catch
                txtUser.Text = Nothing
                lstSP.SelectedIndex = 0
            End Try
        Else
            lstSP.SelectedIndex = 0
        End If
        If txtUser.Text Is Nothing Or txtUser.Text = "" Then
            txtUser.Text = SystemInformation.UserName
        End If
    End Sub 'New


    '/ <summary>
    '/ Clean up any resources being used.
    '/ </summary>
    Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
        MyBase.Dispose(Disposing)
    End Sub 'Dispose


    '/ <summary>
    '/ Required method for Designer support - do not modify
    '/ the contents of this method with the code editor.
    '/ </summary>
    Private Sub InitializeComponent()
        Me.lstSP = New System.Windows.Forms.ListBox()
        Me.txtUser = New System.Windows.Forms.TextBox()
        Me.btnOK = New System.Windows.Forms.Button()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.label1 = New System.Windows.Forms.Label()
        Me.label2 = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        ' 
        ' lstSP
        ' 
        Me.lstSP.Location = New System.Drawing.Point(12, 63)
        Me.lstSP.Name = "lstSP"
        Me.lstSP.Size = New System.Drawing.Size(324, 147)
        Me.lstSP.TabIndex = 1
        ' 
        ' txtUser
        ' 
        Me.txtUser.Location = New System.Drawing.Point(13, 20)
        Me.txtUser.MaxLength = 30
        Me.txtUser.Name = "txtUser"
        Me.txtUser.Size = New System.Drawing.Size(326, 20)
        Me.txtUser.TabIndex = 5
        Me.txtUser.Text = ""
        ' 
        ' btnOK
        ' 
        Me.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.btnOK.Location = New System.Drawing.Point(181, 215)
        Me.btnOK.Name = "btnOK"
        Me.btnOK.Size = New System.Drawing.Size(74, 27)
        Me.btnOK.TabIndex = 2
        Me.btnOK.Text = "OK"
        ' 
        ' btnCancel
        ' 
        Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnCancel.Location = New System.Drawing.Point(260, 215)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(74, 27)
        Me.btnCancel.TabIndex = 3
        Me.btnCancel.Text = "Cancel"
        ' 
        ' label1
        ' 
        Me.label1.Location = New System.Drawing.Point(11, 44)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(376, 19)
        Me.label1.TabIndex = 0
        Me.label1.Text = "Please pick your service provider:"
        ' 
        ' label2
        ' 
        Me.label2.Location = New System.Drawing.Point(10, 5)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(327, 12)
        Me.label2.TabIndex = 4
        Me.label2.Text = "User Name:"
        ' 
        ' ChooseServiceProviderForm
        ' 
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.AcceptButton = Me.btnOK
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.CancelButton = Me.btnCancel
        Me.ClientSize = New System.Drawing.Size(343, 246)
        Me.ControlBox = False
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.txtUser, Me.label2, Me.btnCancel, Me.btnOK, Me.lstSP, Me.label1})
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "ChooseServiceProviderForm"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Choose Service Provider"
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    '/ <summary>
    '/ They don't want to run this sample
    '/ </summary>
    Private Sub btnCancel_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnCancel.Click
        Me.Dispose(Disposing)
    End Sub 'btnCancel_Click




    '/ <summary>
    '/ Same as clicking ok
    '/ </summary>
    Private Sub lstSP_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstSP.DoubleClick
        ' Call the ok button click handler
        Dim parameters() As Object = {sender, e}
        Me.BeginInvoke(New System.EventHandler(AddressOf btnOK_Click), parameters)
    End Sub 'lstSP_DoubleClick



    '/ <summary>
    '/ Select this username and service provider if valid, then continue the wizard
    '/ </summary>
    Private Sub btnOK_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnOK.Click

        If txtUser.Text Is Nothing Or txtUser.Text = "" Then
            MessageBox.Show(Me, "Please enter a username before clicking OK.", "No Username", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If
        Try
            connectionWizard.ServiceProvider = CType(lstSP.SelectedItem, ServiceProviderInformation).Guid
            connectionWizard.Username = txtUser.Text
        Catch ' We assume if we got here there was no selected item.
            MessageBox.Show(Me, "Please select a service provider before clicking OK.", "No Service Provider", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End Try

        Dim regKey As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.CurrentUser.CreateSubKey("Software\Microsoft\DirectX\SDK\csDPlay")
        If Not (regKey Is Nothing) Then
            regKey.SetValue("DirectPlayUserName", txtUser.Text)
            regKey.SetValue("DirectPlaySPIndex", lstSP.SelectedIndex)
            regKey.Close()
        End If
        Me.Dispose()
    End Sub 'btnOK_Click
End Class 'ChooseServiceProviderForm Form