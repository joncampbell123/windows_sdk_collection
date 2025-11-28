'-----------------------------------------------------------------------------
' File: DPlayConnect_CreateForm.vb
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
'/ This form will allow you to create a new session, and set certain properties
'/ of the session.
'/ </summary>
Public Class CreateSessionForm
    Inherits System.Windows.Forms.Form
    Private peer As peer
    Private connectionWizard As ConnectWizard
    Private deviceAddress As Address





    '/ <summary>
    '/ Constructor
    '/ </summary>
    Public Sub New(ByVal peerObject As peer, ByVal addressObject As Address, ByVal connectionWizard As ConnectWizard)
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()

        peer = peerObject
        Me.connectionWizard = connectionWizard
        deviceAddress = addressObject
        txtSession.Text = Nothing
        Me.Text = connectionWizard.SampleName + " - " + Me.Text
        'Get the default session from the registry if it exists
        Dim regKey As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("Software\Microsoft\DirectX\SDK\csDPlay")
        If Not (regKey Is Nothing) Then
            txtSession.Text = CStr(regKey.GetValue("DirectPlaySessionName", Nothing))
            If Not (regKey.GetValue("DirectPlayMigrateHost", Nothing) Is Nothing) Then
                migrateHostCheckBox.Checked = CInt(regKey.GetValue("DirectPlayMigrateHost", 1)) = 1
            End If
            ' Get session signing option
            If Not (regKey.GetValue("DirectPlaySessionSigning", Nothing) Is Nothing) Then
                If regKey.GetValue("DirectPlaySessionSigning", Nothing) = "Full" Then
                    fullSignedRadio.Checked = True
                ElseIf regKey.GetValue("DirectPlaySessionSigning", Nothing) = "Fast" Then
                    fastSignedRadio.Checked = True
                Else
                    notSignedRadio.Checked = True
                End If
            End If

            regKey.Close()
        End If

        ' Set default port value and hide port UI if provider doesn't use them
        Port = connectionWizard.DefaultPort
        If Not ConnectWizard.ProviderRequiresPort(deviceAddress.ServiceProvider) Then
            localPortTextBox.Hide()
            localPortLabel.Hide()
        End If
    End Sub 'New



    '/ <summary>
    '/ The port on which to host
    '/ </summary>
    Public Property Port() As Integer
        Get
            Dim tempPort As Integer = 0
            Try
                tempPort = Integer.Parse(localPortTextBox.Text)
            Catch
            End Try
            Return tempPort
        End Get
        Set(ByVal Value As Integer)
            If Value > 0 Then
                localPortTextBox.Text = Value.ToString()
            End If
        End Set
    End Property



    '/ <summary>
    '/ Clean up any resources being used.
    '/ </summary>
    Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
        MyBase.Dispose(Disposing)
    End Sub 'Dispose


    '
    '/ <summary>
    '/ Required method for Designer support - do not modify
    '/ the contents of this method with the code editor.
    '/ </summary>
    Friend WithEvents localPortTextBox As System.Windows.Forms.TextBox
    Friend WithEvents localPortLabel As System.Windows.Forms.Label
    Friend WithEvents useDPNSVRCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents migrateHostCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents txtSession As System.Windows.Forms.TextBox
    Friend WithEvents label1 As System.Windows.Forms.Label
    Friend WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents fullSignedRadio As System.Windows.Forms.RadioButton
    Friend WithEvents fastSignedRadio As System.Windows.Forms.RadioButton
    Friend WithEvents notSignedRadio As System.Windows.Forms.RadioButton
    Friend WithEvents btnOK As System.Windows.Forms.Button
    Friend WithEvents btnCancel As System.Windows.Forms.Button
    Private Sub InitializeComponent()
        Me.localPortTextBox = New System.Windows.Forms.TextBox()
        Me.localPortLabel = New System.Windows.Forms.Label()
        Me.useDPNSVRCheckBox = New System.Windows.Forms.CheckBox()
        Me.migrateHostCheckBox = New System.Windows.Forms.CheckBox()
        Me.txtSession = New System.Windows.Forms.TextBox()
        Me.label1 = New System.Windows.Forms.Label()
        Me.groupBox1 = New System.Windows.Forms.GroupBox()
        Me.fullSignedRadio = New System.Windows.Forms.RadioButton()
        Me.fastSignedRadio = New System.Windows.Forms.RadioButton()
        Me.notSignedRadio = New System.Windows.Forms.RadioButton()
        Me.btnOK = New System.Windows.Forms.Button()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'localPortTextBox
        '
        Me.localPortTextBox.Location = New System.Drawing.Point(213, 112)
        Me.localPortTextBox.Name = "localPortTextBox"
        Me.localPortTextBox.Size = New System.Drawing.Size(56, 20)
        Me.localPortTextBox.TabIndex = 23
        Me.localPortTextBox.Text = ""
        '
        'localPortLabel
        '
        Me.localPortLabel.Location = New System.Drawing.Point(149, 112)
        Me.localPortLabel.Name = "localPortLabel"
        Me.localPortLabel.Size = New System.Drawing.Size(64, 17)
        Me.localPortLabel.TabIndex = 22
        Me.localPortLabel.Text = "Local Port:"
        '
        'useDPNSVRCheckBox
        '
        Me.useDPNSVRCheckBox.Location = New System.Drawing.Point(141, 80)
        Me.useDPNSVRCheckBox.Name = "useDPNSVRCheckBox"
        Me.useDPNSVRCheckBox.Size = New System.Drawing.Size(136, 16)
        Me.useDPNSVRCheckBox.TabIndex = 21
        Me.useDPNSVRCheckBox.Text = "Use DPNSVR"
        '
        'migrateHostCheckBox
        '
        Me.migrateHostCheckBox.Checked = True
        Me.migrateHostCheckBox.CheckState = System.Windows.Forms.CheckState.Checked
        Me.migrateHostCheckBox.Location = New System.Drawing.Point(141, 60)
        Me.migrateHostCheckBox.Name = "migrateHostCheckBox"
        Me.migrateHostCheckBox.Size = New System.Drawing.Size(136, 16)
        Me.migrateHostCheckBox.TabIndex = 20
        Me.migrateHostCheckBox.Text = "Enable Host Migration"
        '
        'txtSession
        '
        Me.txtSession.Location = New System.Drawing.Point(93, 16)
        Me.txtSession.Name = "txtSession"
        Me.txtSession.Size = New System.Drawing.Size(176, 20)
        Me.txtSession.TabIndex = 18
        Me.txtSession.Text = ""
        '
        'label1
        '
        Me.label1.Location = New System.Drawing.Point(13, 16)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(288, 12)
        Me.label1.TabIndex = 17
        Me.label1.Text = "Session Name:"
        '
        'groupBox1
        '
        Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.fullSignedRadio, Me.fastSignedRadio, Me.notSignedRadio})
        Me.groupBox1.Location = New System.Drawing.Point(13, 56)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(112, 88)
        Me.groupBox1.TabIndex = 19
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Session Signing"
        '
        'fullSignedRadio
        '
        Me.fullSignedRadio.Location = New System.Drawing.Point(18, 22)
        Me.fullSignedRadio.Name = "fullSignedRadio"
        Me.fullSignedRadio.Size = New System.Drawing.Size(80, 16)
        Me.fullSignedRadio.TabIndex = 9
        Me.fullSignedRadio.Text = "Full signed"
        '
        'fastSignedRadio
        '
        Me.fastSignedRadio.Checked = True
        Me.fastSignedRadio.Location = New System.Drawing.Point(18, 41)
        Me.fastSignedRadio.Name = "fastSignedRadio"
        Me.fastSignedRadio.Size = New System.Drawing.Size(88, 16)
        Me.fastSignedRadio.TabIndex = 8
        Me.fastSignedRadio.TabStop = True
        Me.fastSignedRadio.Text = "Fast signed"
        '
        'notSignedRadio
        '
        Me.notSignedRadio.Location = New System.Drawing.Point(18, 55)
        Me.notSignedRadio.Name = "notSignedRadio"
        Me.notSignedRadio.Size = New System.Drawing.Size(72, 24)
        Me.notSignedRadio.TabIndex = 10
        Me.notSignedRadio.Text = "Disabled"
        '
        'btnOK
        '
        Me.btnOK.Location = New System.Drawing.Point(125, 160)
        Me.btnOK.Name = "btnOK"
        Me.btnOK.Size = New System.Drawing.Size(74, 27)
        Me.btnOK.TabIndex = 15
        Me.btnOK.Text = "OK"
        '
        'btnCancel
        '
        Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnCancel.Location = New System.Drawing.Point(205, 160)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(74, 27)
        Me.btnCancel.TabIndex = 16
        Me.btnCancel.Text = "Cancel"
        '
        'CreateSessionForm
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(288, 192)
        Me.ControlBox = False
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.localPortTextBox, Me.localPortLabel, Me.migrateHostCheckBox, Me.txtSession, Me.label1, Me.groupBox1, Me.btnOK, Me.btnCancel, Me.useDPNSVRCheckBox})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "CreateSessionForm"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Create a Session"
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub 'InitializeComponent

    '/ <summary>
    '/ We are ready to create a session.  Ensure the data is valid
    '/ then create the session
    '/ </summary>
    Private Sub btnOK_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOK.Click
        Dim dpApp As ApplicationDescription
        If txtSession.Text Is Nothing Or txtSession.Text = "" Then
            MessageBox.Show(Me, "Please enter a session name before clicking OK.", "No sessionname", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Return
        End If

        Dim regKey As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.CurrentUser.CreateSubKey("Software\Microsoft\DirectX\SDK\csDPlay")
        If Not (regKey Is Nothing) Then
            regKey.SetValue("DirectPlaySessionName", txtSession.Text)
            If migrateHostCheckBox.Checked Then
                regKey.SetValue("DirectPlayMigrateHost", 1)
            Else
                regKey.SetValue("DirectPlayMigrateHost", 0)
            End If

            If fastSignedRadio.Checked Then
                regKey.SetValue("DirectPlaySessionSigning", "Fast")
            ElseIf fullSignedRadio.Checked Then
                regKey.SetValue("DirectPlaySessionSigning", "Full")
            Else
                regKey.SetValue("DirectPlaySessionSigning", "Disabled")
            End If
            regKey.Close()
        End If

        dpApp = New ApplicationDescription()
        dpApp.GuidApplication = connectionWizard.ApplicationGuid
        dpApp.SessionName = txtSession.Text
        dpApp.Flags = IIf(migrateHostCheckBox.Checked, SessionFlags.MigrateHost, 0)

        If Not useDPNSVRCheckBox.Checked Then
            dpApp.Flags = dpApp.Flags Or SessionFlags.NoDpnServer
        End If

        If fastSignedRadio.Checked Then
            dpApp.Flags = dpApp.Flags Or SessionFlags.FastSigned
        ElseIf fullSignedRadio.Checked Then
            dpApp.Flags = dpApp.Flags Or SessionFlags.FullSigned
        End If

        ' Specify the port number if available
        If ConnectWizard.ProviderRequiresPort(deviceAddress.ServiceProvider) Then
            If Port > 0 Then
                deviceAddress.AddComponent(Address.KeyPort, Port)
            End If
        End If

        connectionWizard.SetUserInfo()
        ' Host a game on deviceAddress as described by dpApp
        ' HostFlags.OkToQueryForAddressing allows DirectPlay to prompt the user
        ' using a dialog box for any device address information that is missing
        peer.Host(dpApp, deviceAddress, HostFlags.OkToQueryForAddressing)
        Me.DialogResult = DialogResult.OK
    End Sub 'btnOK_Click

End Class 'CreateSessionForm