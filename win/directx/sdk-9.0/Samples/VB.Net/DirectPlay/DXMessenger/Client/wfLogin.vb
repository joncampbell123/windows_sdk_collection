Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms


Namespace DXMessengerClient
    _
    '/ <summary>
    '/ Summary description for wfLogin.
    '/ </summary>
    Public Class wfLogin
        Inherits System.Windows.Forms.Form
        Private label1 As System.Windows.Forms.Label
        Private label2 As System.Windows.Forms.Label
        Private txtUser As System.Windows.Forms.TextBox
        Private txtPwd As System.Windows.Forms.TextBox
        Private label3 As System.Windows.Forms.Label
        Private txtServer As System.Windows.Forms.TextBox
        Private label4 As System.Windows.Forms.Label
        Private chkRemember As System.Windows.Forms.CheckBox
        Private WithEvents btnOk As System.Windows.Forms.Button
        Private btnCancel As System.Windows.Forms.Button
        Private WithEvents btnCreate As System.Windows.Forms.Button
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
            txtUser.Text = CStr(key.GetValue("Username", Nothing))
            txtPwd.Text = CStr(key.GetValue("Password", Nothing))
            If Not (txtPwd.Text Is Nothing) And txtPwd.Text <> "" Then
                chkRemember.Checked = True
            End If
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
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.txtUser = New System.Windows.Forms.TextBox()
            Me.txtPwd = New System.Windows.Forms.TextBox()
            Me.label3 = New System.Windows.Forms.Label()
            Me.txtServer = New System.Windows.Forms.TextBox()
            Me.label4 = New System.Windows.Forms.Label()
            Me.chkRemember = New System.Windows.Forms.CheckBox()
            Me.btnOk = New System.Windows.Forms.Button()
            Me.btnCancel = New System.Windows.Forms.Button()
            Me.btnCreate = New System.Windows.Forms.Button()
            Me.SuspendLayout()
            ' 
            ' label1
            ' 
            Me.label1.Location = New System.Drawing.Point(13, 8)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(352, 38)
            Me.label1.TabIndex = 0
            Me.label1.Text = "Please type in your username, password and server to connect to, or click the 'Cr" + "eate Account' button..."
            ' 
            ' label2
            ' 
            Me.label2.Location = New System.Drawing.Point(13, 52)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(100, 16)
            Me.label2.TabIndex = 1
            Me.label2.Text = "Username:"
            ' 
            ' txtUser
            ' 
            Me.txtUser.Location = New System.Drawing.Point(13, 70)
            Me.txtUser.MaxLength = 255
            Me.txtUser.Name = "txtUser"
            Me.txtUser.Size = New System.Drawing.Size(352, 20)
            Me.txtUser.TabIndex = 0
            Me.txtUser.Text = ""
            ' 
            ' txtPwd
            ' 
            Me.txtPwd.Location = New System.Drawing.Point(13, 117)
            Me.txtPwd.MaxLength = 255
            Me.txtPwd.Name = "txtPwd"
            Me.txtPwd.PasswordChar = "*"c
            Me.txtPwd.Size = New System.Drawing.Size(352, 20)
            Me.txtPwd.TabIndex = 1
            Me.txtPwd.Text = ""
            ' 
            ' label3
            ' 
            Me.label3.Location = New System.Drawing.Point(13, 99)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(100, 16)
            Me.label3.TabIndex = 3
            Me.label3.Text = "Password:"
            ' 
            ' txtServer
            ' 
            Me.txtServer.Location = New System.Drawing.Point(11, 181)
            Me.txtServer.MaxLength = 255
            Me.txtServer.Name = "txtServer"
            Me.txtServer.Size = New System.Drawing.Size(352, 20)
            Me.txtServer.TabIndex = 3
            Me.txtServer.Text = ""
            ' 
            ' label4
            ' 
            Me.label4.Location = New System.Drawing.Point(11, 163)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(100, 16)
            Me.label4.TabIndex = 5
            Me.label4.Text = "Server name:"
            ' 
            ' chkRemember
            ' 
            Me.chkRemember.Location = New System.Drawing.Point(13, 143)
            Me.chkRemember.Name = "chkRemember"
            Me.chkRemember.Size = New System.Drawing.Size(204, 17)
            Me.chkRemember.TabIndex = 2
            Me.chkRemember.Text = "Remember this password"
            ' 
            ' btnOk
            ' 
            Me.btnOk.Location = New System.Drawing.Point(289, 207)
            Me.btnOk.Name = "btnOk"
            Me.btnOk.Size = New System.Drawing.Size(72, 22)
            Me.btnOk.TabIndex = 6
            Me.btnOk.Text = "OK"
            ' 
            ' btnCancel
            ' 
            Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.btnCancel.Location = New System.Drawing.Point(213, 207)
            Me.btnCancel.Name = "btnCancel"
            Me.btnCancel.Size = New System.Drawing.Size(72, 22)
            Me.btnCancel.TabIndex = 5
            Me.btnCancel.Text = "Cancel"
            ' 
            ' btnCreate
            ' 
            Me.btnCreate.Location = New System.Drawing.Point(13, 208)
            Me.btnCreate.Name = "btnCreate"
            Me.btnCreate.Size = New System.Drawing.Size(101, 22)
            Me.btnCreate.TabIndex = 4
            Me.btnCreate.Text = "Create account..."
            ' 
            ' wfLogin
            ' 
            Me.AcceptButton = Me.btnOk
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.CancelButton = Me.btnCancel
            Me.ClientSize = New System.Drawing.Size(370, 237)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnCreate, Me.btnCancel, Me.btnOk, Me.chkRemember, Me.txtServer, Me.label4, Me.txtPwd, Me.label3, Me.txtUser, Me.label2, Me.label1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "wfLogin"
            Me.Text = "Login"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        Private Sub btnOk_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnOk.Click '
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
            ' Great, save these settings, and lets move on.
            Dim key As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\" + MessengerShared.ApplicationName + "\Client", True)
            ' If the key doesn't exist, create it
            If key Is Nothing Then
                key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey(("Software\" + MessengerShared.ApplicationName + "\Client"))
            End If
            key.SetValue("ServerName", txtServer.Text)
            key.SetValue("Username", txtUser.Text)
            If chkRemember.Checked Then
                key.SetValue("Password", txtPwd.Text)
            Else
                key.SetValue("Password", "")
            End If
            key.Close()

            parent.Select()
            parent.BringToFront()
            Me.Hide()
            If parent.gConnected And parent.gServer = txtServer.Text Then
                ' We've already connected to this server, and are still connected.  No
                ' need to try and connect once more
                parent.LogonPlayer(txtUser.Text, MessengerShared.EncodePassword(txtPwd.Text))
                ' If we're connected, it's not to this server, go ahead and try to connect now
            Else
                parent.LogonPlayer(txtServer.Text, txtUser.Text, MessengerShared.EncodePassword(txtPwd.Text))
            End If
        End Sub 'btnOk_Click

        Private Sub btnCreate_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnCreate.Click
            Dim createPlayer As New wfCreate(parent)
            Me.Hide()
            createPlayer.ShowDialog(parent)
            createPlayer.Dispose()
        End Sub 'btnCreate_Click
    End Class 'wfLogin
End Namespace 'DXMessengerClient