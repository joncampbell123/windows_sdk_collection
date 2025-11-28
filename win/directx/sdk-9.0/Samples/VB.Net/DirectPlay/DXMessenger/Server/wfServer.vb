Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay


Namespace DXMessengerServer
    _
    Public Enum LogonTypes
        LogonSuccess ' The logon was a success
        InvalidPassword ' The user exists, but this isn't his password
        AccountDoesNotExist ' This account doesn't exist.
    End Enum 'LogonTypes
    _
    '/ <summary>
    '/ Summary description for wfServer.
    '/ </summary>
    Public Class wfServer
        Inherits System.Windows.Forms.Form
        Private allowExit As Boolean = False
        Public maxUsers As Integer = 1000
        Private serverStarted As Boolean = False
        Private server As Server = Nothing ' The server object
        Private data As DataStore = Nothing
        Private numPlayers As Integer = 0
        Private updateTimer As System.Timers.Timer = Nothing
        Private timeCount As Integer = 0
       _

        Private Structure PlayerObject
            Public playerId As Integer
            Public playerName As String

            ' We could also store more statistics here, ie, msg's sent, etc
            Public Overrides Function ToString() As String
                Return String.Format("{0} - DirectPlay ID = 0x{1}", Me.playerName, Me.playerId.ToString("X"))
            End Function 'ToString
        End Structure 'PlayerObject


        Private Sub SendChatMessage(ByVal username As String, ByVal from As String, ByVal chatmsg As String)
            Dim sendId As Integer = 0
            Dim stm As New NetworkPacket()

            If Not serverStarted Then
                Return
            End If
            ' Before sending this message, check to see if we are blocked.
            If data.AmIBlocked(username, from) Then
                sendId = data.GetCurrentPlayerId(from)
                stm.Write(MessageType.UserBlocked)
                stm.Write(username)
                server.SendTo(sendId, stm, 0, 0)
            Else
                sendId = data.GetCurrentPlayerId(username)
                If sendId = 0 Then ' This user isn't logged on anymore
                    stm.Write(MessageType.UserUnavailable)
                    stm.Write(username)
                    stm.Write(chatmsg)
                    ' Ok, send the message to the client
                Else
                    stm.Write(MessageType.ReceiveMessage)
                    stm.Write(from)
                    stm.Write(chatmsg)
                End If
                server.SendTo(sendId, stm, 0, 0)
            End If
        End Sub 'SendChatMessage
        '
        Private mnuMain As System.Windows.Forms.MainMenu
        Private menuItem1 As System.Windows.Forms.MenuItem '
        Private menuItem4 As System.Windows.Forms.MenuItem
        Private WithEvents mnuKick As System.Windows.Forms.MenuItem
        Private lstPop As System.Windows.Forms.ContextMenu
        Private WithEvents mnuMainExit As System.Windows.Forms.MenuItem
        Private WithEvents mnuKickPop As System.Windows.Forms.MenuItem
        Private label1 As System.Windows.Forms.Label
        Private WithEvents lstUsers As System.Windows.Forms.ListBox
        Private WithEvents btnStart As System.Windows.Forms.Button
        Private statusBar As System.Windows.Forms.StatusBar
        Private notifyIcon As System.Windows.Forms.NotifyIcon
        Private popUp As System.Windows.Forms.ContextMenu
        Private mnuShow As System.Windows.Forms.MenuItem
        Private menuItem2 As System.Windows.Forms.MenuItem
        Private WithEvents mnuExit As System.Windows.Forms.MenuItem
        Private components As System.ComponentModel.IContainer

        Public Sub New() '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            ' Set up our icon in the system tray
            Me.notifyIcon.Icon = Me.Icon
            Me.notifyIcon.Visible = True
            'this.notifyIcon.MouseUp += new System.Windows.Forms.MouseEventHandler(this.NotifyClick);
            AddHandler Me.notifyIcon.DoubleClick, AddressOf Me.ShowForm
            ' Set our status bar /Icon text
            UpdateText()

            'Load our XML data
            data = New DataStore()

            ' Start a timer, it will fire off every 60 seconds
            updateTimer = New System.Timers.Timer(60000)
            AddHandler updateTimer.Elapsed, AddressOf Me.TimerElapsed
            updateTimer.Start()
            Me.UpdateMenuItems()

            ' Automatically start the server
            Me.btnStart_Click(Me.btnStart, Nothing)
        End Sub 'New


        Protected Shadows Sub Dispose()
            If Not (updateTimer Is Nothing) Then
                updateTimer.Dispose()
                updateTimer = Nothing
            End If
            If Not (server Is Nothing) Then
                server.Dispose()
                lstUsers.Items.Clear()
                server = Nothing
            End If
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
            If Not (data Is Nothing) Then
                data.Close()
            End If
        End Sub 'Dispose

        Private Sub TimerElapsed(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs) '
            ' Just to make sure the data isn't lost, every 5 minutes we will save our 
            ' data store to a file.
            timeCount += 1
            If timeCount > 4 Then
                timeCount = 0
                If Not (data Is Nothing) Then
                    data.SaveDataStore()
                End If
            End If
        End Sub 'TimerElapsed

        Private Sub ShowForm(ByVal sender As Object, ByVal e As EventArgs)
            ' If we're calling this, the window is probably minimized.  First restore it
            Me.WindowState = FormWindowState.Normal
            Me.Refresh()
            Me.BringToFront()
            Me.Show() ' Now show the window and bring it to the front
        End Sub 'ShowForm

        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.components = New System.ComponentModel.Container()
            Me.label1 = New System.Windows.Forms.Label()
            Me.lstUsers = New System.Windows.Forms.ListBox()
            Me.lstPop = New System.Windows.Forms.ContextMenu()
            Me.mnuKickPop = New System.Windows.Forms.MenuItem()
            Me.btnStart = New System.Windows.Forms.Button()
            Me.statusBar = New System.Windows.Forms.StatusBar()
            Me.notifyIcon = New System.Windows.Forms.NotifyIcon(Me.components)
            Me.popUp = New System.Windows.Forms.ContextMenu()
            Me.mnuShow = New System.Windows.Forms.MenuItem()
            Me.menuItem2 = New System.Windows.Forms.MenuItem()
            Me.mnuExit = New System.Windows.Forms.MenuItem()
            Me.mnuMain = New System.Windows.Forms.MainMenu()
            Me.menuItem1 = New System.Windows.Forms.MenuItem()
            Me.mnuMainExit = New System.Windows.Forms.MenuItem()
            Me.menuItem4 = New System.Windows.Forms.MenuItem()
            Me.mnuKick = New System.Windows.Forms.MenuItem()
            Me.SuspendLayout()
            ' 
            ' label1
            ' 
            Me.label1.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(288, 16)
            Me.label1.TabIndex = 0
            Me.label1.Text = "Users currently in this session:"
            Me.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' lstUsers
            ' 
            Me.lstUsers.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right
            Me.lstUsers.ContextMenu = Me.lstPop
            Me.lstUsers.Location = New System.Drawing.Point(3, 20)
            Me.lstUsers.Name = "lstUsers"
            Me.lstUsers.Size = New System.Drawing.Size(284, 238)
            Me.lstUsers.TabIndex = 1
            ' 
            ' lstPop
            ' 
            Me.lstPop.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuKickPop})
            ' 
            ' mnuKickPop
            ' 
            Me.mnuKickPop.Index = 0
            Me.mnuKickPop.Text = "Kick User"
            ' 
            ' btnStart
            ' 
            Me.btnStart.Anchor = System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right
            Me.btnStart.Location = New System.Drawing.Point(211, 265)
            Me.btnStart.Name = "btnStart"
            Me.btnStart.Size = New System.Drawing.Size(74, 23)
            Me.btnStart.TabIndex = 2
            Me.btnStart.Text = "Start Server"
            ' 
            ' statusBar
            ' 
            Me.statusBar.Location = New System.Drawing.Point(0, 291)
            Me.statusBar.Name = "statusBar"
            Me.statusBar.Size = New System.Drawing.Size(292, 17)
            Me.statusBar.TabIndex = 3
            ' 
            ' notifyIcon
            ' 
            Me.notifyIcon.ContextMenu = Me.popUp
            Me.notifyIcon.Text = ""
            Me.notifyIcon.Visible = True
            ' 
            ' popUp
            ' 
            Me.popUp.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuShow, Me.menuItem2, Me.mnuExit})
            ' 
            ' mnuShow
            ' 
            Me.mnuShow.DefaultItem = True
            Me.mnuShow.Index = 0
            Me.mnuShow.Text = "Show"
            ' 
            ' menuItem2
            ' 
            Me.menuItem2.Index = 1
            Me.menuItem2.Text = "-"
            ' 
            ' mnuExit
            ' 
            Me.mnuExit.Index = 2
            Me.mnuExit.Text = "Exit"
            ' 
            ' mnuMain
            ' 
            Me.mnuMain.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuItem1, Me.menuItem4})
            ' 
            ' menuItem1
            ' 
            Me.menuItem1.Index = 0
            Me.menuItem1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuMainExit})
            Me.menuItem1.Text = "&File"
            ' 
            ' mnuMainExit
            ' 
            Me.mnuMainExit.Index = 0
            Me.mnuMainExit.Text = "E&xit"
            ' 
            ' menuItem4
            ' 
            Me.menuItem4.Index = 1
            Me.menuItem4.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuKick})
            Me.menuItem4.Text = "&Options"
            ' 
            ' mnuKick
            ' 
            Me.mnuKick.Index = 0
            Me.mnuKick.Text = "&Kick User"
            ' 
            ' wfServer
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(292, 308)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.statusBar, Me.btnStart, Me.lstUsers, Me.label1})
            Me.MaximizeBox = False
            Me.Menu = Me.mnuMain
            Me.MinimumSize = New System.Drawing.Size(300, 250)
            Me.Name = "wfServer"
            Me.Text = "DirectX Messenger Server"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '
        Private Sub wfServer_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Resize '
            If Me.WindowState = FormWindowState.Minimized Then
                Me.Hide()
            End If
        End Sub 'wfServer_Resize

        Private Sub UpdateText()
            If Not serverStarted Then
                statusBar.Text = "Sever not yet started... (" + numPlayers.ToString() + "/" + maxUsers.ToString() + " connected)."
                notifyIcon.Text = statusBar.Text
            Else
                statusBar.Text = "Sever running... (" + numPlayers.ToString() + "/" + maxUsers.ToString() + " connected)."
                notifyIcon.Text = statusBar.Text
            End If
        End Sub 'UpdateText


        Private Sub btnStart_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnStart.Click
            serverStarted = Not serverStarted
            If serverStarted Then ' Do the opposite since we just changed it a second ago
                ' Set up the application description
                Dim desc As New ApplicationDescription()
                desc.GuidApplication = MessengerShared.applicationGuid
                desc.MaxPlayers = maxUsers
                desc.SessionName = "DxMessengerServer"
                desc.Flags = SessionFlags.ClientServer Or SessionFlags.NoDpnServer

                ' Create a new address that will reside on tcpip and listen only on our port
                Dim add As New Address()
                add.ServiceProvider = Address.ServiceProviderTcpIp
                add.AddComponent(Address.KeyPort, MessengerShared.DefaultPort)

                ' Now we can start our server
                server = New Server()
                ' Add our event handlers
                AddHandler server.PlayerDestroyed, AddressOf Me.DestroyPlayerMsg
                AddHandler server.Receive, AddressOf Me.DataReceivedMsg
                ' Host this session
                server.Host(desc, add)

                Me.btnStart.Text = "Stop Server"
                UpdateText()
            Else
                Me.btnStart.Text = "Start Server"
                lstUsers.Items.Clear()
                numPlayers = 0
                server.Dispose()
                server = Nothing
                'Update our text, the server should not be running
                UpdateText()
            End If
        End Sub 'btnStart_Click

        Private Sub mnuExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuExit.Click, mnuMainExit.Click
            serverStarted = False
            allowExit = True
            Me.Dispose()
            Application.Exit()
        End Sub 'mnuExit_Click

        Private Sub wfServer_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
            ' We only want to close by the 'Exit' menu item.
            e.Cancel = Not allowExit
            If Not allowExit Then
                ' We should warn them that closing the window doesn't really close the window, but rather
                ' just hides it.
                ' Load the default values from the registry
                Dim key As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\" + MessengerShared.ApplicationName + "\Server", True)
                ' If the key doesn't exist, create it
                If key Is Nothing Then
                    key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey(("Software\" + MessengerShared.ApplicationName + "\Server"))
                End If
                Dim bWarn As Boolean = Boolean.Parse(key.GetValue("Warn", True).ToString())

                If bWarn Then
                    If MessageBox.Show("Closing this window doesn't actually stop the application.  The application is still running in the taskbar." + ControlChars.Cr + ControlChars.Lf + "In order to shut down the application entirely, please choose Exit from the menu." + ControlChars.Cr + ControlChars.Lf + ControlChars.Cr + ControlChars.Lf + "Do you wish to see this warning again?", "Not closing", MessageBoxButtons.YesNo, MessageBoxIcon.Question) = DialogResult.Yes Then
                        key.SetValue("Warn", True)
                    Else
                        key.SetValue("Warn", False)
                    End If
                End If
                key.Close()

                Me.Hide()
            End If
        End Sub 'wfServer_Closing

        Private Sub lstUsers_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstUsers.SelectedIndexChanged
            Me.UpdateMenuItems()
        End Sub 'lstUsers_SelectedIndexChanged

        Private Sub mnuKick_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuKickPop.Click, mnuKick.Click
            Dim stm As New NetworkPacket()

            ' First check to see if this user is already logged on, if so, store the DPlayID
            Dim playerId As Integer = CType(lstUsers.SelectedItem, PlayerObject).playerId
            ' Let the user know that they've been destroyed for this reason
            stm.Write(MessageType.ServerKick)
            server.DestroyClient(playerId, stm)
        End Sub 'mnuKick_Click

        Private Sub UpdateMenuItems()
            mnuKick.Enabled = lstUsers.SelectedIndex >= 0
            mnuKickPop.Enabled = lstUsers.SelectedIndex >= 0
        End Sub 'UpdateMenuItems


        '
        Sub DestroyPlayerMsg(ByVal sender As Object, ByVal e As PlayerDestroyedEventArgs) '
            If Not serverStarted Then
                Return
            End If
            ' A user has quit
            Dim p As PlayerObject
            For Each p In lstUsers.Items
                ' If they are in the user list, remove them
                If p.playerId = e.Message.PlayerID Then
                    lstUsers.Items.Remove(p)
                    numPlayers -= 1
                    ' We should also notify everyone they've logged off.
                    If Not (data Is Nothing) Then
                        ' Notify everyone that we've logged off
                        data.NotifyFriends(p.playerName, MessageType.FriendLogoff, server)
                        ' Update the schema to show we've logged off
                        data.UpdateDataToReflectLogoff(e.Message.PlayerID)
                    End If
                    Exit For
                End If
            Next p
            'We always want to update our text
            UpdateText()
        End Sub 'DestroyPlayerMsg

        Sub DataReceivedMsg(ByVal sender As Object, ByVal e As ReceiveEventArgs)
            ' We've received data, process it
            Dim username As String = Nothing
            Dim password As String = Nothing
            Dim stm As New NetworkPacket()

            If Not serverStarted Then
                Return
            End If
            Dim msg As MessageType = CType(e.Message.ReceiveData.Read(GetType(MessageType)), MessageType)
            Select Case msg
                Case MessageType.AddFriend
                    username = e.Message.ReceiveData.ReadString()
                    If Not data.DoesUserExist(username) Then
                        ' This user doesn't exist, how can we add them?  Notify the user
                        ' of this error.
                        stm.Write(MessageType.FriendDoesNotExist)
                        server.SendTo(e.Message.SenderID, stm, 0, 0)
                    Else
                        ' Great, add this user to our friend list
                        Dim loggedin As Boolean = data.AddFriend(e.Message.SenderID, username, True)
                        ' Let the client know it's been added
                        stm.Write(MessageType.FriendAdded)
                        stm.Write(username)
                        server.SendTo(e.Message.SenderID, stm, 0, 0)
                        If loggedin Then
                            stm = New NetworkPacket()
                            stm.Write(MessageType.FriendLogon)
                            stm.Write(username)
                            server.SendTo(e.Message.SenderID, stm, 0, 0)
                        End If
                    End If
                Case MessageType.BlockFriend
                    username = e.Message.ReceiveData.ReadString()
                    If Not data.DoesUserExist(username) Then
                        ' This user doesn't exist, how can we block them?  Notify the user
                        ' of this error.
                        stm.Write(MessageType.BlockUserDoesNotExist)
                        server.SendTo(e.Message.SenderID, stm, 0, 0)
                    Else
                        ' Great, block this user from our friend list
                        Dim loggedin As Boolean = data.AddFriend(e.Message.SenderID, username, False)
                        ' Let the client know it's been blocked
                        stm.Write(MessageType.FriendBlocked)
                        stm.Write(username)
                        server.SendTo(e.Message.SenderID, stm, 0, 0)
                        If loggedin Then
                            stm = New NetworkPacket()
                            stm.Write(MessageType.FriendLogon)
                            stm.Write(username)
                            server.SendTo(e.Message.SenderID, stm, 0, 0)
                        End If
                    End If
                Case MessageType.DeleteFriend
                    username = e.Message.ReceiveData.ReadString()
                    If Not data.DoesUserExist(username) Then
                        ' This user doesn't exist, we can't delete them, this should never occur so just ignore it
                    Else
                        ' Great, block this user from our friend list
                        data.DeleteFriend(e.Message.SenderID, username)
                        ' Let the client know it's been blocked
                        stm.Write(MessageType.FriendDeleted)
                        stm.Write(username)
                        server.SendTo(e.Message.SenderID, stm, 0, 0)
                    End If
                Case MessageType.CreateNewAccount
                    username = e.Message.ReceiveData.ReadString()
                    password = e.Message.ReceiveData.ReadString()
                    If data.DoesUserExist(username) Then
                        ' This user already exists, inform the user so they can try a new name
                        stm.Write(MessageType.UserAlreadyExists)
                        server.SendTo(e.Message.SenderID, stm, 0, 0)
                    Else
                        ' Good, this user doesn't exist, let's add it.
                        data.AddUser(username, password, e.Message.SenderID)
                        ' We don't need to inform anyone of our logon, we were just created
                        ' it's impossible to be listed as someone's friend already
                        ' Notify the client they've been logged on succesfully
                        stm.Write(MessageType.LoginSuccess)
                        server.SendTo(e.Message.SenderID, stm, 0, 0)

                        ' Increment the player count
                        numPlayers += 1
                        ' Add this user to our listbox of current players
                        Dim player As New PlayerObject()
                        player.playerId = e.Message.SenderID
                        player.playerName = username
                        lstUsers.Items.Add(player)
                        UpdateText()
                    End If
                Case MessageType.Login
                    username = e.Message.ReceiveData.ReadString()
                    password = e.Message.ReceiveData.ReadString()
                    ' First check to see if this user is already logged on, if so, store the DPlayID
                    Dim playerId As Integer = data.GetCurrentPlayerId(username)
                    ' Try to login with this username/password
                    Select Case data.LogonUser(username, password)
                        Case LogonTypes.LogonSuccess
                            ' Great, they've logged on
                            data.UpdateDataToReflectLogon(username, e.Message.SenderID)
                            ' Now check to see if they were logged on somewhere else before.  
                            ' If so, kill that instance.
                            If playerId <> 0 Then
                                ' Let the user know that they've been destroyed for this reason
                                stm.Write(MessageType.LogonOtherLocation)
                                server.DestroyClient(playerId, stm)
                                stm = New NetworkPacket()
                            End If
                            ' Notify the user they've been logged in
                            stm.Write(MessageType.LoginSuccess)
                            server.SendTo(e.Message.SenderID, stm, 0, 0)

                            ' Tell everyone who is my friend I've logged on
                            data.NotifyFriends(username, MessageType.FriendLogon, server)
                            ' Find out if any of my friends are online, and notify me if so
                            data.FindMyFriends(username, server)
                            ' Increment the player count
                            numPlayers += 1
                            ' Add this user to our listbox of current players
                            Dim player As New PlayerObject()
                            player.playerId = e.Message.SenderID
                            player.playerName = username
                            lstUsers.Items.Add(player)
                            UpdateText()
                        Case LogonTypes.InvalidPassword
                            ' Notify the user they don't know the password
                            stm.Write(MessageType.InvalidPassword)
                            server.SendTo(e.Message.SenderID, stm, 0, 0)
                        Case LogonTypes.AccountDoesNotExist
                            ' Notify the user this account doesn't exist
                            stm.Write(MessageType.InvalidUser)
                            server.SendTo(e.Message.SenderID, stm, 0, 0)
                    End Select
                Case MessageType.SendMessage
                    username = e.Message.ReceiveData.ReadString()
                    Dim from As String = e.Message.ReceiveData.ReadString()
                    Dim chatmsg As String = e.Message.ReceiveData.ReadString()
                    Me.SendChatMessage(username, from, chatmsg)
            End Select
            e.Message.ReceiveData.Dispose() 'Don't need the data anymore
        End Sub 'DataReceivedMsg

        '
        Shared Sub Main()
            Application.Run(New wfServer())
        End Sub 'Main
    End Class 'wfServer 
End Namespace 'DXMessengerServer