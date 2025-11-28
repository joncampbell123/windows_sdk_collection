Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Threading
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay


Namespace DXMessengerClient
    _
    '/ <summary>
    '/ Summary description for wfClient.
    '/ </summary>
    Public Class wfClient
        Inherits System.Windows.Forms.Form

        Delegate Sub ServerExitCallback(ByVal msg As MessageType)

        Delegate Sub AddNodeCallback(ByVal parent As TreeNode, ByVal username As String, ByVal [friend] As Boolean)

        Delegate Sub RemoveNodeCallback(ByVal username As String)

        Delegate Sub NodeLogonOffCallback(ByVal parent As TreeNode, ByVal username As String)

        Delegate Sub CreateMsgWindowCallback(ByVal username As String)

        Private applicationTitle As String = "DxMessengerClient"
        Private gOnLineKey As String = "OnlineLeafKey"
        Private gOffLineKey As String = "OfflineLeafKey"
        Private gOnLineText As String = "Friends Online"
        Private gOffLineText As String = "Friends Offline"

        Private msgForms As New ArrayList() ' This will hold our forms that messages come in on
        Private allowExit As Boolean = False

        ' App specific variables
        Private client As Client = Nothing
        Private device As Address = Nothing
        Private host As Address = Nothing

        Public gConnected As Boolean = False
        Public gServer As String = Nothing
        Public gUsername As String = Nothing
        Private hConnect As AutoResetEvent = Nothing
        Private onlineNode As TreeNode = Nothing
        Private offlineNode As TreeNode = Nothing

        '
        Private mnuMain As System.Windows.Forms.MainMenu
        Private WithEvents tvwItems As System.Windows.Forms.TreeView '
        Private imlTree As System.Windows.Forms.ImageList
        Private WithEvents notifyIcon As System.Windows.Forms.NotifyIcon
        Private menuItem1 As System.Windows.Forms.MenuItem
        Private menuItem4 As System.Windows.Forms.MenuItem
        Private menuItem6 As System.Windows.Forms.MenuItem
        Private menuItem9 As System.Windows.Forms.MenuItem
        Private mnuIcon As System.Windows.Forms.ContextMenu
        Private WithEvents mnuTree As System.Windows.Forms.ContextMenu
        Private WithEvents mnuAddFriend As System.Windows.Forms.MenuItem
        Private WithEvents mnuBlock As System.Windows.Forms.MenuItem
        Private WithEvents mnuExit As System.Windows.Forms.MenuItem
        Private WithEvents mnuPopupExit As System.Windows.Forms.MenuItem
        Private WithEvents mnuLogon As System.Windows.Forms.MenuItem
        Private WithEvents mnuLogoff As System.Windows.Forms.MenuItem
        Private WithEvents mnuSend As System.Windows.Forms.MenuItem
        Private WithEvents mnuSendPopup As System.Windows.Forms.MenuItem
        Private WithEvents mnuPopDelete As System.Windows.Forms.MenuItem
        Private WithEvents mnuDelete As System.Windows.Forms.MenuItem
        Private components As System.ComponentModel.IContainer

        '
        Public Sub New() '
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            hConnect = New AutoResetEvent(False)
            notifyIcon.Text = applicationTitle + " - Not logged in."
            AddHandler Me.notifyIcon.DoubleClick, AddressOf Me.ShowForm
            AddHandler Me.mnuPopupExit.Click, AddressOf Me.mnuExit_Click
            Me.notifyIcon.Visible = True

            Me.SetupDefaultTree()
            Me.EnableLoggedInUi(False)
            Me.EnableSendUi(False, False)
            Me.Text = applicationTitle + " - Not logged in."
        End Sub 'New


        Protected Shadows Sub Dispose()
            If Not (client Is Nothing) Then
                client.Dispose()
                client = Nothing
            End If
            SyncLock msgForms
                Try
                    ' Get rid of any open message forms
                    Dim f As Form
                    For Each f In msgForms
                        f.Dispose()
                    Next f
                Catch
                End Try
                msgForms.Clear()
            End SyncLock

            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose


        '
        Public Sub Initialize()
            '
            If Not (client Is Nothing) Then
                client.Dispose()
            End If
            client = New Client()
            device = New Address()
            host = New Address()

            ' Set the device to use TCP/IP
            device.ServiceProvider = Address.ServiceProviderTcpIp

            ' The host is running on tcp/ip on the port listed in shared
            host.ServiceProvider = Address.ServiceProviderTcpIp
            host.AddComponent(Address.KeyPort, MessengerShared.DefaultPort)

            ' Set up our event handlers
            AddHandler client.ConnectComplete, AddressOf Me.ConnectComplete
            AddHandler client.Receive, AddressOf Me.DataReceived
            AddHandler client.SessionTerminated, AddressOf Me.SessionLost
        End Sub 'Initialize

        Private Sub UpdateText(ByVal [text] As String)
            Me.notifyIcon.Text = [text]
            Me.Text = [text]
        End Sub 'UpdateText

        Private Sub ShowForm(ByVal sender As Object, ByVal e As EventArgs)
            ' Show the form
            Me.Visible = True
            Me.Select()
            Me.BringToFront()
        End Sub 'ShowForm

        '
        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.components = New System.ComponentModel.Container()
            Dim resources As New System.Resources.ResourceManager(GetType(wfClient))
            Me.tvwItems = New System.Windows.Forms.TreeView()
            Me.mnuTree = New System.Windows.Forms.ContextMenu()
            Me.mnuSendPopup = New System.Windows.Forms.MenuItem()
            Me.mnuPopDelete = New System.Windows.Forms.MenuItem()
            Me.imlTree = New System.Windows.Forms.ImageList(Me.components)
            Me.mnuMain = New System.Windows.Forms.MainMenu()
            Me.menuItem1 = New System.Windows.Forms.MenuItem()
            Me.mnuLogon = New System.Windows.Forms.MenuItem()
            Me.mnuLogoff = New System.Windows.Forms.MenuItem()
            Me.menuItem4 = New System.Windows.Forms.MenuItem()
            Me.mnuExit = New System.Windows.Forms.MenuItem()
            Me.menuItem6 = New System.Windows.Forms.MenuItem()
            Me.mnuAddFriend = New System.Windows.Forms.MenuItem()
            Me.mnuBlock = New System.Windows.Forms.MenuItem()
            Me.menuItem9 = New System.Windows.Forms.MenuItem()
            Me.mnuSend = New System.Windows.Forms.MenuItem()
            Me.mnuDelete = New System.Windows.Forms.MenuItem()
            Me.notifyIcon = New System.Windows.Forms.NotifyIcon(Me.components)
            Me.mnuIcon = New System.Windows.Forms.ContextMenu()
            Me.mnuPopupExit = New System.Windows.Forms.MenuItem()
            Me.SuspendLayout()
            ' 
            ' tvwItems
            ' 
            Me.tvwItems.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right
            Me.tvwItems.ContextMenu = Me.mnuTree
            Me.tvwItems.ImageList = Me.imlTree
            Me.tvwItems.Location = New System.Drawing.Point(0, 1)
            Me.tvwItems.Name = "tvwItems"
            Me.tvwItems.ShowLines = False
            Me.tvwItems.ShowRootLines = False
            Me.tvwItems.Size = New System.Drawing.Size(291, 272)
            Me.tvwItems.TabIndex = 0
            ' 
            ' mnuTree
            ' 
            Me.mnuTree.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuSendPopup, Me.mnuPopDelete})
            ' 
            ' mnuSendPopup
            ' 
            Me.mnuSendPopup.Index = 0
            Me.mnuSendPopup.Text = "Send Message"
            ' 
            ' mnuPopDelete
            ' 
            Me.mnuPopDelete.Index = 1
            Me.mnuPopDelete.Text = "Delete Friend"
            ' 
            ' imlTree
            ' 
            Me.imlTree.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit
            Me.imlTree.ImageSize = New System.Drawing.Size(16, 16)
            Me.imlTree.ImageStream = CType(resources.GetObject("imlTree.ImageStream"), System.Windows.Forms.ImageListStreamer)
            Me.imlTree.TransparentColor = System.Drawing.Color.Transparent
            ' 
            ' mnuMain
            ' 
            Me.mnuMain.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuItem1, Me.menuItem6})
            ' 
            ' menuItem1
            ' 
            Me.menuItem1.Index = 0
            Me.menuItem1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuLogon, Me.mnuLogoff, Me.menuItem4, Me.mnuExit})
            Me.menuItem1.Text = "&File"
            ' 
            ' mnuLogon
            ' 
            Me.mnuLogon.Index = 0
            Me.mnuLogon.Shortcut = System.Windows.Forms.Shortcut.CtrlL
            Me.mnuLogon.Text = "&Log on..."
            ' 
            ' mnuLogoff
            ' 
            Me.mnuLogoff.Index = 1
            Me.mnuLogoff.Text = "Lo&g off"
            ' 
            ' menuItem4
            ' 
            Me.menuItem4.Index = 2
            Me.menuItem4.Text = "-"
            ' 
            ' mnuExit
            ' 
            Me.mnuExit.Index = 3
            Me.mnuExit.Text = "E&xit"
            ' 
            ' menuItem6
            ' 
            Me.menuItem6.Index = 1
            Me.menuItem6.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuAddFriend, Me.mnuBlock, Me.menuItem9, Me.mnuSend, Me.mnuDelete})
            Me.menuItem6.Text = "&Options"
            ' 
            ' mnuAddFriend
            ' 
            Me.mnuAddFriend.Index = 0
            Me.mnuAddFriend.Text = "&Add Friend ..."
            ' 
            ' mnuBlock
            ' 
            Me.mnuBlock.Index = 1
            Me.mnuBlock.Text = "&Block User ..."
            ' 
            ' menuItem9
            ' 
            Me.menuItem9.Index = 2
            Me.menuItem9.Text = "-"
            ' 
            ' mnuSend
            ' 
            Me.mnuSend.Index = 3
            Me.mnuSend.Text = "&Send Message ..."
            ' 
            ' mnuDelete
            ' 
            Me.mnuDelete.Index = 4
            Me.mnuDelete.Text = "&Delete Friend"
            ' 
            ' notifyIcon
            ' 
            Me.notifyIcon.ContextMenu = Me.mnuIcon
            Me.notifyIcon.Icon = CType(resources.GetObject("notifyIcon.Icon"), System.Drawing.Icon)
            Me.notifyIcon.Text = "notifyIcon1"
            Me.notifyIcon.Visible = True
            ' 
            ' mnuIcon
            ' 
            Me.mnuIcon.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuPopupExit})
            ' 
            ' mnuPopupExit
            ' 
            Me.mnuPopupExit.Index = 0
            Me.mnuPopupExit.Text = "E&xit"
            ' 
            ' wfClient
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(292, 273)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.tvwItems})
            Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
            Me.Menu = Me.mnuMain
            Me.MinimumSize = New System.Drawing.Size(300, 250)
            Me.Name = "wfClient"
            Me.Text = "DxMessengerClient"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '
        Private Sub SetupDefaultTree() '
            ' Clear the tree
            tvwItems.Nodes.Clear()
            ' Now add the default leafs, online/offline
            onlineNode = New TreeNode(gOnLineText, 0, 0)
            offlineNode = New TreeNode(gOffLineText, 0, 0)
            tvwItems.Nodes.Add(onlineNode)
            tvwItems.Nodes.Add(offlineNode)
            onlineNode.Tag = CType(gOnLineKey, Object)
            offlineNode.Tag = CType(gOffLineKey, Object)
        End Sub 'SetupDefaultTree

        Private Sub EnableLoggedInUi(ByVal enable As Boolean)
            Me.mnuAddFriend.Enabled = enable
            Me.mnuBlock.Enabled = enable
            Me.mnuLogoff.Enabled = enable
            Me.mnuLogon.Enabled = Not enable
        End Sub 'EnableLoggedInUi

        Private Sub EnableSendUi(ByVal enable As Boolean, ByVal childLeaf As Boolean)
            Me.mnuSend.Enabled = enable
            Me.mnuSendPopup.Enabled = enable
            Me.mnuDelete.Enabled = childLeaf
            Me.mnuPopDelete.Enabled = childLeaf
        End Sub 'EnableSendUi


        Private Sub AddFriend(ByVal friendName As String)
            Dim stm As New NetworkPacket()

            ' Add the AddFriend message type, and friends name, and let the server know
            stm.Write(MessageType.AddFriend)
            stm.Write(friendName)
            client.Send(stm, 0, 0)
        End Sub 'AddFriend

        Private Sub BlockUser(ByVal friendName As String)
            Dim stm As New NetworkPacket()

            ' Add the BlockFriend message type, and friends name, and let the server know
            stm.Write(MessageType.BlockFriend)
            stm.Write(friendName)
            client.Send(stm, 0, 0)
        End Sub 'BlockUser


        '
        Private Sub mnuAddFriend_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuAddFriend.Click
            Dim name As String = wfInput.InputBox("Add Friend", "Please enter the name of the friend you wish to add.")
            If name = gUsername Then
                MessageBox.Show("Everyone wants to be friends with themselves, but in this sample, I simply can't allow it..", "No friend with self.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            ElseIf name Is Nothing Then
                MessageBox.Show("I can't add a friend if you don't enter a name.", "No name.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Else
                Me.AddFriend(name)
            End If
        End Sub 'mnuAddFriend_Click

        Private Sub mnuBlock_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuBlock.Click
            Dim name As String = wfInput.InputBox("Block User", "Please enter the name of the user you wish to block.")
            If name = gUsername Then
                MessageBox.Show("It's not often someone wants to be block themselves, but in this sample, I simply can't allow it..", "No block with self.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            ElseIf name Is Nothing Then
                MessageBox.Show("I can't block a user if you don't enter a name.", "No name.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Else
                Me.BlockUser(name)
            End If
        End Sub 'mnuBlock_Click

        Private Sub wfClient_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
            ' We only want to close by the 'Exit' menu item.
            e.Cancel = Not allowExit
            If Not allowExit Then
                ' We should warn them that closing the window doesn't really close the window, but rather
                ' just hides it.
                ' Load the default values from the registry
                Dim key As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\" + MessengerShared.ApplicationName + "\Client", True)
                ' If the key doesn't exist, create it
                If key Is Nothing Then
                    key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey(("Software\" + MessengerShared.ApplicationName + "\Client"))
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
        End Sub 'wfClient_Closing


        Private Sub mnuExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuExit.Click
            allowExit = True
            Me.Dispose()
            Application.Exit()
        End Sub 'mnuExit_Click


        Private Sub wfClient_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Resize
            If Me.WindowState = FormWindowState.Minimized Then
                Me.Hide()
            End If
        End Sub 'wfClient_Resize

        Private Sub mnuLogon_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuLogon.Click
            Dim loginDialog As New wfLogin(Me)
            loginDialog.ShowDialog(Me)
            loginDialog.Dispose()
        End Sub 'mnuLogon_Click


        Private Sub tvwItems_BeforeSelect(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeViewCancelEventArgs) Handles tvwItems.BeforeSelect
            Dim node As TreeNode = Nothing
            If e.Node.Parent Is Nothing Then
                EnableSendUi(False, False)
                Return
            Else
                node = e.Node
            End If
            If node.Nodes.Count = 0 And (Not (node.Parent Is offlineNode)) Then
                EnableSendUi(True, node.Nodes.Count = 0)
            Else
                EnableSendUi(False, node.Nodes.Count = 0)
            End If
        End Sub 'tvwItems_BeforeSelect

        Private Sub mnuSend_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuSendPopup.Click, mnuSend.Click
            Dim msgWindow As wfMsg = Me.GetMessageWindow(tvwItems.SelectedNode.Tag.ToString())
        End Sub 'mnuSend_Click

        Private Sub mnuLogoff_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuLogoff.Click
            ' We've logged off.  
            Me.EnableLoggedInUi(False)
            gConnected = False
            gUsername = Nothing
            gServer = Nothing
            Me.UpdateText((MessengerShared.ApplicationName + " Service (Not logged in)"))
            Me.SetupDefaultTree()
            ' Shut down our DPlay client
            If Not (client Is Nothing) Then
                client.Dispose()
            End If
            client = Nothing
        End Sub 'mnuLogoff_Click

        Private Sub mnuTree_Popup(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuTree.Popup
            ' Get the node where the mouse is right now
            Dim node As TreeNode = tvwItems.GetNodeAt(tvwItems.PointToClient(Control.MousePosition))
            If node Is Nothing Then
                Return
            End If
            ' Set this node as our selected node
            tvwItems.SelectedNode = node
            If node.Parent Is Nothing Then
                EnableSendUi(False, False)
                Return
            End If

            If node.Nodes.Count = 0 And (Not (node.Parent Is offlineNode)) Then
                EnableSendUi(True, node.Nodes.Count = 0)
            Else
                EnableSendUi(False, node.Nodes.Count = 0)
            End If
        End Sub 'mnuTree_Popup

        Private Sub mnuDelete_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuPopDelete.Click, mnuDelete.Click
            ' We want to delete a friend, who?
            Dim username As String = CStr(tvwItems.SelectedNode.Tag)
            Dim stm As New NetworkPacket()

            ' Add the DeleteFriend message type, and friends name, and let the server know
            stm.Write(MessageType.DeleteFriend)
            stm.Write(username)
            client.Send(stm, 0, 0)
        End Sub 'mnuDelete_Click

        '
        Public Overloads Sub LogonPlayer(ByVal playername As String, ByVal password As String) '
            Dim stm As New NetworkPacket()

            gUsername = playername
            ' We're connected now, send our login information
            stm.Write(MessageType.Login)
            stm.Write(playername)
            stm.Write(password)
            ' Send to the server
            client.Send(stm, 0, 0)
        End Sub 'LogonPlayer

        Public Overloads Sub LogonPlayer(ByVal server As String, ByVal playername As String, ByVal password As String)
            ' Try the connect now
            Me.Initialize() ' Make sure dplay is setup properly
            host.AddComponent(Address.KeyHostname, server)

            Dim desc As New ApplicationDescription()
            desc.GuidApplication = MessengerShared.applicationGuid

            Try
                ' Connect to this server
                client.Connect(desc, host, device, Nothing, 0)

                ' Now we will sit and wait in a loop for our connect complete event to fire
                While Not hConnect.WaitOne(0, False)
                    ' Don't kill the CPU, allow other things to happen while we wait
                    Application.DoEvents()
                End While

                If gConnected Then ' We connected, try to log on now
                    gServer = server
                    LogonPlayer(playername, password)
                End If
            Catch
                MessageBox.Show("This server could not be contacted.  Please check the server name and try again.", "Server not found.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            End Try
        End Sub 'LogonPlayer

        Public Overloads Sub CreatePlayer(ByVal playername As String, ByVal password As String)
            Dim stm As New NetworkPacket()

            gUsername = playername
            ' We're connected now, send our login information
            stm.Write(MessageType.CreateNewAccount)
            stm.Write(playername)
            stm.Write(password)
            ' Send to the server
            client.Send(stm, 0, 0)
        End Sub 'CreatePlayer

        Public Overloads Sub CreatePlayer(ByVal server As String, ByVal playername As String, ByVal password As String)
            ' Try the connect now
            Me.Initialize() ' Make sure dplay is setup properly
            host.AddComponent(Address.KeyHostname, server)

            Dim desc As New ApplicationDescription()
            desc.GuidApplication = MessengerShared.applicationGuid

            Try
                ' Connect to this server
                client.Connect(desc, host, device, Nothing, 0)
            Catch
                MessageBox.Show("This server could not be contacted.  Please check the server name and try again.", "Server not found.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            End Try

            ' Now we will sit and wait in a loop for our connect complete event to fire
            While Not hConnect.WaitOne(0, False)
                ' Don't kill the CPU, allow other things to happen while we wait
                Application.DoEvents()
            End While

            If gConnected Then ' We connected, try to log on now
                gServer = server
                CreatePlayer(playername, password)
            End If
        End Sub 'CreatePlayer

        Public Sub SendChatMessage(ByVal username As String, ByVal msg As String)
            Dim stm As New NetworkPacket()

            stm.Write(MessageType.SendMessage)
            stm.Write(username)
            stm.Write(gUsername)
            stm.Write(msg)
            client.Send(stm, 0, 0)
        End Sub 'SendChatMessage

        '
        Public Function GetMessageWindow(ByVal username As String) As wfMsg '
            SyncLock msgForms
                ' Scroll through all existing message windows and see if we have one for this user
                Dim f As wfMsg
                For Each f In msgForms
                    If f.UserName.ToLower() = username.ToLower() Then
                        Return f
                    End If
                Next f
            End SyncLock ' Well shoot.. Nothing here, guess we need to create one..  We don't want to 
            ' Create the form on a thread that isn't our main UI thread (ie, a DPlay thread)
            ' so we will use the Invoke method.  We use invoke rather than BeginInvoke
            ' because we want this call to be synchronous
            Dim newParams As Object() = {username}
            Me.Invoke(New CreateMsgWindowCallback(AddressOf Me.CreateMessageWindow), newParams)
            SyncLock msgForms
                Dim f As wfMsg
                For Each f In msgForms
                    If f.UserName.ToLower() = username.ToLower() Then
                        Return f
                    End If
                Next f
            End SyncLock
            Return Nothing
        End Function 'GetMessageWindow

        Public Sub CreateMessageWindow(ByVal username As String)
            ' Well, we got here, guess there hasn't been one yet
            ' Let's create it
            Dim retMsg As New wfMsg(username, Me)
            ' Show the window now (we will do this again, but that's ok)
            retMsg.Show()
            retMsg.Select()
            retMsg.BringToFront()
            SyncLock msgForms
                msgForms.Add(retMsg)
            End SyncLock
            ' We need to know if a form is disposed
            AddHandler retMsg.Disposed, AddressOf Me.MessageWindowDisposed
        End Sub 'CreateMessageWindow

        Public Sub MessageWindowDisposed(ByVal sender As Object, ByVal e As EventArgs)
            SyncLock msgForms
                ' Search through our forms, and remove the item that's been disposed
                Dim f As wfMsg
                For Each f In msgForms
                    If f.Equals(sender) Then
                        msgForms.Remove(f)
                        Exit For
                    End If
                Next f
            End SyncLock
        End Sub 'MessageWindowDisposed

        '
        Private Sub ServerExit(ByVal msg As MessageType) '
            If msg = MessageType.LogonOtherLocation Then
                MessageBox.Show("You have been disconnected because you've logged on at another location.", "Logged on elsewhere.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            ElseIf msg = MessageType.ServerKick Then
                MessageBox.Show("The server administrator has disconnected you.", "Server kicked.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Else
                MessageBox.Show("Your connection with the server has been lost.  You have been logged off.", "Server disconnected.", MessageBoxButtons.OK, MessageBoxIcon.Information)
            End If
            Me.mnuLogoff_Click(Nothing, Nothing)
        End Sub 'ServerExit

        Private Sub AddNode(ByVal parent As TreeNode, ByVal username As String, ByVal [friend] As Boolean)
            Dim newNode As TreeNode = Nothing

            Dim found As Boolean = False
            ' First scroll through all of the nodes, and remove this if it exists
            Dim nTop As TreeNode
            For Each nTop In tvwItems.Nodes  ' This will only scroll through top level nodes
                ' Since our outter loop only scrolls through the top level nodes
                ' namely, the Online/Offline leafs, we will scroll through each of 
                ' their nodes in this loop
                Dim n As TreeNode
                For Each n In nTop.Nodes
                    If n.Tag.ToString().ToLower() = username.ToLower() Then
                        If [friend] Then
                            n.Text = username
                        Else
                            n.Text = username + " (BLOCKED)"
                        End If
                        found = True
                        Exit For
                    End If
                Next n
            Next nTop

            If Not found Then
                ' Now let's add this user to the correct node
                If [friend] Then
                    newNode = New TreeNode(username, 0, 0)
                Else
                    newNode = New TreeNode(username + " (BLOCKED)", 0, 0)
                End If
                newNode.Tag = CType(username, Object)

                parent.Nodes.Add(newNode)
            End If

            tvwItems.ExpandAll()
        End Sub 'AddNode

        Private Sub NodeLogonOff(ByVal parent As TreeNode, ByVal username As String)
            Dim newNode As TreeNode = Nothing

            Dim found As Boolean = False
            Dim [text] As String = Nothing

            ' First scroll through all of the nodes, and remove this if it exists
            Dim nTop As TreeNode
            For Each nTop In tvwItems.Nodes  ' This will only scroll through top level nodes
                ' Since our outter loop only scrolls through the top level nodes
                ' namely, the Online/Offline leafs, we will scroll through each of 
                ' their nodes in this loop
                Dim n As TreeNode
                For Each n In nTop.Nodes
                    If n.Tag.ToString().ToLower() = username.ToLower() Then
                        found = True
                        [text] = n.Text
                        nTop.Nodes.Remove(n)
                        Exit For
                    End If
                Next n
            Next nTop

            If found Then
                ' Now let's add this user to the correct node
                newNode = New TreeNode([text], 0, 0)
            Else
                newNode = New TreeNode(username, 0, 0)
            End If
            newNode.Tag = CType(username, Object)
            parent.Nodes.Add(newNode)

            tvwItems.ExpandAll()
        End Sub 'NodeLogonOff

        Private Sub RemoveNode(ByVal username As String)
            ' First scroll through all of the nodes, and remove this if it exists
            Dim nTop As TreeNode
            For Each nTop In tvwItems.Nodes  ' This will only scroll through top level nodes
                ' Since our outter loop only scrolls through the top level nodes
                ' namely, the Online/Offline leafs, we will scroll through each of 
                ' their nodes in this loop
                Dim n As TreeNode
                For Each n In nTop.Nodes
                    If n.Tag.ToString().ToLower() = username.ToLower() Then
                        nTop.Nodes.Remove(n)
                        Exit For
                    End If
                Next n
            Next nTop
            tvwItems.ExpandAll()
        End Sub 'RemoveNode

        '
        Private Sub ConnectComplete(ByVal sender As Object, ByVal e As ConnectCompleteEventArgs) '
            ' Did we really connect?
            If e.Message.ResultCode <> 0 Then
                MessageBox.Show("The server does not exist, or is unavailable.", "Unavailable", MessageBoxButtons.OK, MessageBoxIcon.Information)
            Else
                gConnected = True
            End If
            ' Set off the connect event
            hConnect.Set()
        End Sub 'ConnectComplete

        Private Sub DataReceived(ByVal sender As Object, ByVal e As ReceiveEventArgs)
            ' We've received data, process it
            Dim username As String = Nothing
            Dim chatmsg As String = Nothing
            Dim [friend] As Boolean = False
            Dim msgWindow As wfMsg = Nothing

            Dim msg As MessageType = CType(e.Message.ReceiveData.Read(GetType(MessageType)), MessageType)
            Select Case msg
                Case MessageType.LoginSuccess ' We've been logged in
                    Me.EnableLoggedInUi(True)
                    Me.UpdateText((MessengerShared.ApplicationName + " - (" + gUsername + ")"))
                Case MessageType.InvalidPassword ' Server says we entered the wrong password
                    MessageBox.Show("The password you've entered is invalid.", "Invalid password", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Case MessageType.InvalidUser
                    MessageBox.Show("The username you've entered is invalid.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Case MessageType.UserAlreadyExists
                    MessageBox.Show("The username you've entered already exists." + ControlChars.Lf + "You must choose a different user name.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Case MessageType.SendClientFriends
                    Dim numFriends As Integer = CInt(e.Message.ReceiveData.Read(GetType(Integer)))
                    ' Ok, we know how many friends are in the message, let's go through them all
                    Dim i As Integer
                    For i = 0 To numFriends - 1
                        [friend] = CBool(e.Message.ReceiveData.Read(GetType(Boolean)))
                        username = e.Message.ReceiveData.ReadString()

                        Dim newParams As Object() = {offlineNode, username, [friend]}
                        ' User our callback to add our friend to this node
                        Me.BeginInvoke(New AddNodeCallback(AddressOf Me.AddNode), newParams)
                    Next i
                Case MessageType.FriendAdded
                    username = e.Message.ReceiveData.ReadString()
                    Dim addParams As Object() = {offlineNode, username, True}
                    ' User our callback to add our friend to this node
                    Me.BeginInvoke(New AddNodeCallback(AddressOf Me.AddNode), addParams)
                    MessageBox.Show(username + " added succesfully to your friends list.", "Added", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Case MessageType.FriendDeleted
                    username = e.Message.ReceiveData.ReadString()
                    Dim deleteParams As Object() = {username}
                    ' User our callback to add our friend to this node
                    Me.BeginInvoke(New RemoveNodeCallback(AddressOf Me.RemoveNode), deleteParams)
                Case MessageType.FriendBlocked
                    username = e.Message.ReceiveData.ReadString()
                    Dim blockParams As Object() = {offlineNode, username, False}
                    ' User our callback to add our friend to this node
                    Me.BeginInvoke(New AddNodeCallback(AddressOf Me.AddNode), blockParams)
                    MessageBox.Show(username + " has been blocked succesfully.", "Blocked", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Case MessageType.FriendDoesNotExist
                    MessageBox.Show("You cannot add this friend since they do not exist.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Case MessageType.BlockUserDoesNotExist
                    MessageBox.Show("You cannot block this user since they do not exist.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Case MessageType.FriendLogon
                    username = e.Message.ReceiveData.ReadString()
                    Dim logonParams As Object() = {onlineNode, username}
                    ' User our callback to add our friend to this node
                    Me.BeginInvoke(New NodeLogonOffCallback(AddressOf Me.NodeLogonOff), logonParams)
                Case MessageType.FriendLogoff
                    username = e.Message.ReceiveData.ReadString()
                    Dim logoffParams As Object() = {offlineNode, username}
                    ' User our callback to add our friend to this node
                    Me.BeginInvoke(New NodeLogonOffCallback(AddressOf Me.NodeLogonOff), logoffParams)
                Case MessageType.ReceiveMessage
                    username = e.Message.ReceiveData.ReadString()
                    chatmsg = e.Message.ReceiveData.ReadString()
                    ' We need to get a message window
                    msgWindow = Me.GetMessageWindow(username)

                    ' Show the window, and add the text
                    msgWindow.BeginInvoke(New wfMsg.ShowWindowCallback(AddressOf msgWindow.ShowWindow))

                    Dim receiveMsg As Object() = {chatmsg, False, False}
                    msgWindow.BeginInvoke(New wfMsg.AddChatMsgCallback(AddressOf msgWindow.AddChatMessage), receiveMsg)

                Case MessageType.UserBlocked
                    username = e.Message.ReceiveData.ReadString()
                    chatmsg = "Your message to " + username + " could not be delivered because this user has you blocked."
                    msgWindow = Me.GetMessageWindow(username)
                    ' Show the window, and add the text
                    msgWindow.BeginInvoke(New wfMsg.ShowWindowCallback(AddressOf msgWindow.ShowWindow))

                    Dim blockMsg As Object() = {chatmsg, False, True}
                    msgWindow.BeginInvoke(New wfMsg.AddChatMsgCallback(AddressOf msgWindow.AddChatMessage), blockMsg)
                Case MessageType.UserUnavailable
                    username = e.Message.ReceiveData.ReadString()
                    chatmsg = e.Message.ReceiveData.ReadString()
                    msgWindow = Me.GetMessageWindow(username)
                    ' Show the window, and add the text
                    msgWindow.BeginInvoke(New wfMsg.ShowWindowCallback(AddressOf msgWindow.ShowWindow))

                    Dim unavailableMsg As Object() = {"Your Message: " + ControlChars.Cr + ControlChars.Lf + chatmsg + ControlChars.Cr + ControlChars.Lf + "to " + username + " could not be delivered because the user is no longer available.", False, True}
                    msgWindow.BeginInvoke(New wfMsg.AddChatMsgCallback(AddressOf msgWindow.AddChatMessage), unavailableMsg)
            End Select
            e.Message.ReceiveData.Dispose() 'Don't need the data anymore
        End Sub 'DataReceived

        Private Sub SessionLost(ByVal sender As Object, ByVal e As SessionTerminatedEventArgs)
            If e.Message.TerminateData.Length <> 0 Then ' We've received terminate session data.
                Dim msg As MessageType = CType(e.Message.TerminateData.Read(GetType(MessageType)), MessageType)
                Dim logOffParams As Object() = {msg}
                Me.BeginInvoke(New ServerExitCallback(AddressOf Me.ServerExit), logOffParams)
            Else
                Dim discParams As Object() = {MessageType.AddFriend}  ' Doesn't really matter, just disconnect
                ' We need to inform the user of the disconnection.  We will call BeginInvoke as to not
                ' block this thread
                Me.BeginInvoke(New ServerExitCallback(AddressOf Me.ServerExit), discParams)
            End If
        End Sub 'SessionLost

        '
        Shared Sub Main()
            '
            'ToDo: Error processing original source shown below
            '
            '
            '--^--- Unexpected pre-processor directive
            Application.Run(New wfClient())
        End Sub 'Main
    End Class 'wfClient 
End Namespace 'DXMessengerClient