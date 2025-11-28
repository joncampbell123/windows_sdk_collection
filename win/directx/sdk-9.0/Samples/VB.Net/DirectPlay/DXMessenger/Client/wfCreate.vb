Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms


Namespace DXMessengerClient
    _
    '/ <summary>
    '/ Summary description for wfCreate.
    '/ </summary>
    Public Class wfCreate
        Inherits System.Windows.Forms.Form
        Private btnCancel As System.Windows.Forms.Button
        Private WithEvents btnOk As System.Windows.Forms.Button
        Private txtServer As System.Windows.Forms.TextBox
        Private label4 As System.Windows.Forms.Label
        Private txtPwd As System.Windows.Forms.TextBox
        Private label3 As System.Windows.Forms.Label
        Private txtUser As System.Windows.Forms.TextBox
        Private label2 As System.Windows.Forms.Label
        Private label1 As System.Windows.Forms.Label
        Private txtVerify As System.Windows.Forms.TextBox
        Private label5 As System.Windows.Forms.Label
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing
        Private Shadows parent As wfClient = Nothing


        Public Sub New(ByVal obj As wfClient)
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            parent = obj

            ' Load the default values from the registry
            Dim key As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\" + MessengerShared.ApplicationName + "\Client", True)
            ' If the key doesn't exist, create it
            If key Is Nothing Then
                key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey(("Software\" + MessengerShared.ApplicationName + "\Client"))
            End If
            txtServer.Text = CStr(key.GetValue("ServerName", Nothing))
            key.Close()
        End Sub 'New


        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Shadows Sub Dispose()
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
        Private Sub InitializeComponent()
            Me.btnCancel = New System.Windows.Forms.Button()
            Me.btnOk = New System.Windows.Forms.Button()
            Me.txtServer = New System.Windows.Forms.TextBox()
            Me.label4 = New System.Windows.Forms.Label()
            Me.txtPwd = New System.Windows.Forms.TextBox()
            Me.label3 = New System.Windows.Forms.Label()
            Me.txtUser = New System.Windows.Forms.TextBox()
            Me.label2 = New System.Windows.Forms.Label()
            Me.label1 = New System.Windows.Forms.Label()
            Me.txtVerify = New System.Windows.Forms.TextBox()
            Me.label5 = New System.Windows.Forms.Label()
            Me.SuspendLayout()
            ' 
            ' btnCancel
            ' 
            Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.btnCancel.Location = New System.Drawing.Point(214, 233)
            Me.btnCancel.Name = "btnCancel"
            Me.btnCancel.Size = New System.Drawing.Size(72, 22)
            Me.btnCancel.TabIndex = 4
            Me.btnCancel.Text = "Cancel"
            ' 
            ' btnOk
            ' 
            Me.btnOk.Location = New System.Drawing.Point(290, 233)
            Me.btnOk.Name = "btnOk"
            Me.btnOk.Size = New System.Drawing.Size(72, 22)
            Me.btnOk.TabIndex = 5
            Me.btnOk.Text = "Create"
            ' 
            ' txtServer
            ' 
            Me.txtServer.Location = New System.Drawing.Point(10, 206)
            Me.txtServer.MaxLength = 255
            Me.txtServer.Name = "txtServer"
            Me.txtServer.Size = New System.Drawing.Size(352, 20)
            Me.txtServer.TabIndex = 3
            Me.txtServer.Text = ""
            ' 
            ' label4
            ' 
            Me.label4.Location = New System.Drawing.Point(10, 188)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(100, 16)
            Me.label4.TabIndex = 16
            Me.label4.Text = "Server name:"
            ' 
            ' txtPwd
            ' 
            Me.txtPwd.Location = New System.Drawing.Point(10, 116)
            Me.txtPwd.MaxLength = 255
            Me.txtPwd.Name = "txtPwd"
            Me.txtPwd.PasswordChar = "*"c
            Me.txtPwd.Size = New System.Drawing.Size(352, 20)
            Me.txtPwd.TabIndex = 1
            Me.txtPwd.Text = ""
            ' 
            ' label3
            ' 
            Me.label3.Location = New System.Drawing.Point(10, 98)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(100, 16)
            Me.label3.TabIndex = 14
            Me.label3.Text = "Password:"
            ' 
            ' txtUser
            ' 
            Me.txtUser.Location = New System.Drawing.Point(10, 69)
            Me.txtUser.MaxLength = 255
            Me.txtUser.Name = "txtUser"
            Me.txtUser.Size = New System.Drawing.Size(352, 20)
            Me.txtUser.TabIndex = 0
            Me.txtUser.Text = ""
            ' 
            ' label2
            ' 
            Me.label2.Location = New System.Drawing.Point(10, 51)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(100, 16)
            Me.label2.TabIndex = 12
            Me.label2.Text = "Username:"
            ' 
            ' label1
            ' 
            Me.label1.Location = New System.Drawing.Point(10, 7)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(352, 38)
            Me.label1.TabIndex = 11
            Me.label1.Text = "Please type the username you wish to use, a password, and the server you wish to " + "connect with."
            ' 
            ' txtVerify
            ' 
            Me.txtVerify.Location = New System.Drawing.Point(10, 162)
            Me.txtVerify.MaxLength = 255
            Me.txtVerify.Name = "txtVerify"
            Me.txtVerify.PasswordChar = "*"c
            Me.txtVerify.Size = New System.Drawing.Size(352, 20)
            Me.txtVerify.TabIndex = 2
            Me.txtVerify.Text = ""
            ' 
            ' label5
            ' 
            Me.label5.Location = New System.Drawing.Point(10, 144)
            Me.label5.Name = "label5"
            Me.label5.Size = New System.Drawing.Size(100, 16)
            Me.label5.TabIndex = 21
            Me.label5.Text = "Verify password:"
            ' 
            ' wfCreate
            ' 
            Me.AcceptButton = Me.btnOk
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.CancelButton = Me.btnCancel
            Me.ClientSize = New System.Drawing.Size(370, 265)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.txtVerify, Me.label5, Me.btnCancel, Me.btnOk, Me.txtServer, Me.label4, Me.txtPwd, Me.label3, Me.txtUser, Me.label2, Me.label1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "wfCreate"
            Me.Text = "Create new account"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '
        Private Sub btnOk_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnOk.Click
            ' Make sure the values our filled out before continuing.
            If txtServer.Text Is Nothing Or txtServer.Text = "" Then
                MessageBox.Show("You must enter a server name.", "No server name", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Return
            End If
            If txtUser.Text Is Nothing Or txtUser.Text = "" Then
                MessageBox.Show("You must enter a user name.", "No user name", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Return
            End If
            If txtPwd.Text Is Nothing Or txtPwd.Text = "" Then
                MessageBox.Show("You must enter a password.", "No password", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Return
            End If
            If txtPwd.Text <> txtVerify.Text Then
                MessageBox.Show("The passwords you've entered do not match.", "Passwords unmatched", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Return
            End If
            ' Great, save these settings, and lets move on.
            Dim key As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\" + MessengerShared.ApplicationName + "\Client", True)
            ' If the key doesn't exist, create it
            If key Is Nothing Then
                key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey(("Software\" + MessengerShared.ApplicationName + "\Client"))
            End If
            key.SetValue("ServerName", txtServer.Text)
            key.SetValue("Username", txtUser.Text)
            key.SetValue("Password", "")

            key.Close()

            parent.Select()
            parent.BringToFront()
            Me.Hide()
            If parent.gConnected And parent.gServer = txtServer.Text Then
                ' We've already connected to this server, and are still connected.  No
                ' need to try and connect once more
                parent.CreatePlayer(txtUser.Text, MessengerShared.EncodePassword(txtPwd.Text))
                ' If we're connected, it's not to this server, go ahead and try to connect now
            Else
                parent.CreatePlayer(txtServer.Text, txtUser.Text, MessengerShared.EncodePassword(txtPwd.Text))
            End If
        End Sub 'btnOk_Click
    End Class 'wfCreate
End Namespace 'DXMessengerClient