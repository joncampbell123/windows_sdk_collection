//----------------------------------------------------------------------------
// File: wfClient.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Threading;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;
using DXMessenger;

namespace DXMessengerClient
{
	/// <summary>
	/// Summary description for wfClient.
	/// </summary>
	public class wfClient : System.Windows.Forms.Form
	{
		private delegate void ServerExitCallback(MessageType msg);
		private delegate void AddNodeCallback(TreeNode parent, string username, bool friend);
		private delegate void RemoveNodeCallback(string username);
		private delegate void NodeLogonOffCallback(TreeNode parent, string username);
		private delegate void CreateMsgWindowCallback(string username);

		private const string applicationTitle = "DxMessengerClient";
		private const string gOnLineKey = "OnlineLeafKey";
		private const string gOffLineKey = "OfflineLeafKey";
		private const string gOnLineText = "Friends Online";
		private const string gOffLineText = "Friends Offline";

		private ArrayList msgForms = new ArrayList(); // This will hold our forms that messages come in on
		private bool allowExit = false;

		// App specific variables
		private Client client = null;
		private Address device = null;
		private Address host = null;

		public bool gConnected = false;
		public string gServer = null;
		public string gUsername = null;
		private AutoResetEvent hConnect = null;
		private TreeNode onlineNode = null;
		private TreeNode offlineNode = null;

		#region WindowsForms variables
		private System.Windows.Forms.MainMenu mnuMain;
		private System.Windows.Forms.TreeView tvwItems;
		private System.Windows.Forms.ImageList imlTree;
		private System.Windows.Forms.NotifyIcon notifyIcon;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem9;
		private System.Windows.Forms.ContextMenu mnuIcon;
		private System.Windows.Forms.ContextMenu mnuTree;
		private System.Windows.Forms.MenuItem mnuAddFriend;
		private System.Windows.Forms.MenuItem mnuBlock;
		private System.Windows.Forms.MenuItem mnuExit;
		private System.Windows.Forms.MenuItem mnuPopupExit;
		private System.Windows.Forms.MenuItem mnuLogon;
		private System.Windows.Forms.MenuItem mnuLogoff;
		private System.Windows.Forms.MenuItem mnuSend;
		private System.Windows.Forms.MenuItem mnuSendPopup;
		private System.Windows.Forms.MenuItem mnuPopDelete;
		private System.Windows.Forms.MenuItem mnuDelete;
		private System.ComponentModel.IContainer components;
		#endregion
		#region WindowsForms functions
		public wfClient()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			hConnect = new AutoResetEvent(false);
			notifyIcon.Text = applicationTitle + " - Not logged in.";
			this.notifyIcon.DoubleClick += new System.EventHandler(this.ShowForm);
			this.mnuPopupExit.Click += new System.EventHandler(this.mnuExit_Click);
			this.notifyIcon.Visible = true;

			this.SetupDefaultTree();
			this.EnableLoggedInUi(false);
			this.EnableSendUi(false, false);
			this.Text = applicationTitle + " - Not logged in.";
		}

		protected override void Dispose(bool disposing)
		{
			if (client != null)
			{
				client.Dispose();
				client = null;
			}
			lock(msgForms)
			{
				try
				{
					// Get rid of any open message forms
					foreach (Form f in msgForms)
						f.Dispose();
				} 
				catch { /* */ }

				msgForms.Clear();
			}

			if (disposing)
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		#endregion
		public void Initialize()
		{
			if (client != null)
				client.Dispose();

			client = new Client();
			device = new Address();
			host = new Address();

			// Set the device to use TCP/IP
			device.ServiceProvider = Address.ServiceProviderTcpIp;

			// The host is running on tcp/ip on the port listed in shared
			host.ServiceProvider = Address.ServiceProviderTcpIp;
			host.AddComponent(Address.KeyPort, MessengerShared.DefaultPort);

			// Set up our event handlers
			client.ConnectComplete += new ConnectCompleteEventHandler(this.ConnectComplete);
			client.Receive += new ReceiveEventHandler(this.DataReceived);
			client.SessionTerminated += new SessionTerminatedEventHandler(this.SessionLost);
		}
		private void UpdateText(string text)
		{
			this.notifyIcon.Text = text;
			this.Text = text;
		}
		private void ShowForm(object sender, EventArgs e)
		{
			// Show the form
			this.Visible = true;
			this.Select();
			this.BringToFront();
		}
		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(wfClient));
			this.tvwItems = new System.Windows.Forms.TreeView();
			this.mnuTree = new System.Windows.Forms.ContextMenu();
			this.mnuSendPopup = new System.Windows.Forms.MenuItem();
			this.mnuPopDelete = new System.Windows.Forms.MenuItem();
			this.imlTree = new System.Windows.Forms.ImageList(this.components);
			this.mnuMain = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.mnuLogon = new System.Windows.Forms.MenuItem();
			this.mnuLogoff = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.mnuExit = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.mnuAddFriend = new System.Windows.Forms.MenuItem();
			this.mnuBlock = new System.Windows.Forms.MenuItem();
			this.menuItem9 = new System.Windows.Forms.MenuItem();
			this.mnuSend = new System.Windows.Forms.MenuItem();
			this.mnuDelete = new System.Windows.Forms.MenuItem();
			this.notifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
			this.mnuIcon = new System.Windows.Forms.ContextMenu();
			this.mnuPopupExit = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
			// 
			// tvwItems
			// 
			this.tvwItems.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.tvwItems.ContextMenu = this.mnuTree;
			this.tvwItems.ImageList = this.imlTree;
			this.tvwItems.Location = new System.Drawing.Point(0, 1);
			this.tvwItems.Name = "tvwItems";
			this.tvwItems.ShowLines = false;
			this.tvwItems.ShowRootLines = false;
			this.tvwItems.Size = new System.Drawing.Size(291, 272);
			this.tvwItems.TabIndex = 0;
			this.tvwItems.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.tvwItems_BeforeSelect);
			// 
			// mnuTree
			// 
			this.mnuTree.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuSendPopup,
																					this.mnuPopDelete});
			this.mnuTree.Popup += new System.EventHandler(this.mnuTree_Popup);
			// 
			// mnuSendPopup
			// 
			this.mnuSendPopup.Index = 0;
			this.mnuSendPopup.Text = "Send Message";
			this.mnuSendPopup.Click += new System.EventHandler(this.mnuSend_Click);
			// 
			// mnuPopDelete
			// 
			this.mnuPopDelete.Index = 1;
			this.mnuPopDelete.Text = "Delete Friend";
			this.mnuPopDelete.Click += new System.EventHandler(this.mnuDelete_Click);
			// 
			// imlTree
			// 
			this.imlTree.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
			this.imlTree.ImageSize = new System.Drawing.Size(16, 16);
			this.imlTree.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imlTree.ImageStream")));
			this.imlTree.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// mnuMain
			// 
			this.mnuMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.menuItem1,
																					this.menuItem6});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.mnuLogon,
																					  this.mnuLogoff,
																					  this.menuItem4,
																					  this.mnuExit});
			this.menuItem1.Text = "&File";
			// 
			// mnuLogon
			// 
			this.mnuLogon.Index = 0;
			this.mnuLogon.Shortcut = System.Windows.Forms.Shortcut.CtrlL;
			this.mnuLogon.Text = "&Log on...";
			this.mnuLogon.Click += new System.EventHandler(this.mnuLogon_Click);
			// 
			// mnuLogoff
			// 
			this.mnuLogoff.Index = 1;
			this.mnuLogoff.Text = "Lo&g off";
			this.mnuLogoff.Click += new System.EventHandler(this.mnuLogoff_Click);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 2;
			this.menuItem4.Text = "-";
			// 
			// mnuExit
			// 
			this.mnuExit.Index = 3;
			this.mnuExit.Text = "E&xit";
			this.mnuExit.Click += new System.EventHandler(this.mnuExit_Click);
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 1;
			this.menuItem6.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.mnuAddFriend,
																					  this.mnuBlock,
																					  this.menuItem9,
																					  this.mnuSend,
																					  this.mnuDelete});
			this.menuItem6.Text = "&Options";
			// 
			// mnuAddFriend
			// 
			this.mnuAddFriend.Index = 0;
			this.mnuAddFriend.Text = "&Add Friend ...";
			this.mnuAddFriend.Click += new System.EventHandler(this.mnuAddFriend_Click);
			// 
			// mnuBlock
			// 
			this.mnuBlock.Index = 1;
			this.mnuBlock.Text = "&Block User ...";
			this.mnuBlock.Click += new System.EventHandler(this.mnuBlock_Click);
			// 
			// menuItem9
			// 
			this.menuItem9.Index = 2;
			this.menuItem9.Text = "-";
			// 
			// mnuSend
			// 
			this.mnuSend.Index = 3;
			this.mnuSend.Text = "&Send Message ...";
			this.mnuSend.Click += new System.EventHandler(this.mnuSend_Click);
			// 
			// mnuDelete
			// 
			this.mnuDelete.Index = 4;
			this.mnuDelete.Text = "&Delete Friend";
			this.mnuDelete.Click += new System.EventHandler(this.mnuDelete_Click);
			// 
			// notifyIcon
			// 
			this.notifyIcon.ContextMenu = this.mnuIcon;
			this.notifyIcon.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon.Icon")));
			this.notifyIcon.Text = "notifyIcon1";
			this.notifyIcon.Visible = true;
			// 
			// mnuIcon
			// 
			this.mnuIcon.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuPopupExit});
			// 
			// mnuPopupExit
			// 
			this.mnuPopupExit.Index = 0;
			this.mnuPopupExit.Text = "E&xit";
			// 
			// wfClient
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.tvwItems});
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Menu = this.mnuMain;
			this.MinimumSize = new System.Drawing.Size(300, 250);
			this.Name = "wfClient";
			this.Text = "DxMessengerClient";
			this.Resize += new System.EventHandler(this.wfClient_Resize);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.wfClient_Closing);
			this.ResumeLayout(false);

		}
		#endregion
		#region Misc functions for this application
		private void SetupDefaultTree()
		{
			// Clear the tree
			tvwItems.Nodes.Clear();
			// Now add the default leafs, online/offline
			onlineNode = new TreeNode(gOnLineText, 0, 0);
			offlineNode = new TreeNode(gOffLineText, 0, 0);
			tvwItems.Nodes.Add(onlineNode);
			tvwItems.Nodes.Add(offlineNode);
			onlineNode.Tag = (object)gOnLineKey;
			offlineNode.Tag  = (object)gOffLineKey;
		}
		private void EnableLoggedInUi(bool enable)
		{
			this.mnuAddFriend.Enabled = enable;
			this.mnuBlock.Enabled = enable;
			this.mnuLogoff.Enabled = enable;
			this.mnuLogon.Enabled = !enable;
		}
		private void EnableSendUi(bool enable, bool childLeaf)
		{
			this.mnuSend.Enabled = enable;
			this.mnuSendPopup.Enabled = enable;
			this.mnuDelete.Enabled = childLeaf;
			this.mnuPopDelete.Enabled = childLeaf;
		}

		private void AddFriend(string friendName)
		{
			NetworkPacket stm = new NetworkPacket();

			// Add the AddFriend message type, and friends name, and let the server know
			stm.Write(MessageType.AddFriend);
			stm.Write(friendName);
			client.Send(stm, 0, 0);
		}
		private void BlockUser(string friendName)
		{
			NetworkPacket stm = new NetworkPacket();

			// Add the BlockFriend message type, and friends name, and let the server know
			stm.Write(MessageType.BlockFriend);
			stm.Write(friendName);
			client.Send(stm, 0, 0);
		}

		#endregion
		#region WindowsForm EventHandlers
		private void mnuAddFriend_Click(object sender, System.EventArgs e)
		{
			string name = wfInput.InputBox("Add Friend", "Please enter the name of the friend you wish to add.");
			if (name == gUsername)
				MessageBox.Show("Everyone wants to be friends with themselves, but in this sample, I simply can't allow it..", "No friend with self.", MessageBoxButtons.OK, MessageBoxIcon.Information);
			else
				if (name == null)
					MessageBox.Show("I can't add a friend if you don't enter a name.", "No name.", MessageBoxButtons.OK, MessageBoxIcon.Information);
				else
					this.AddFriend(name);
		}

		private void mnuBlock_Click(object sender, System.EventArgs e)
		{
			string name = wfInput.InputBox("Block User", "Please enter the name of the user you wish to block.");
			if (name == gUsername)
				MessageBox.Show("It's not often someone wants to be block themselves, but in this sample, I simply can't allow it..", "No block with self.", MessageBoxButtons.OK, MessageBoxIcon.Information);
			else
				if (name == null)
					MessageBox.Show("I can't block a user if you don't enter a name.", "No name.", MessageBoxButtons.OK, MessageBoxIcon.Information);
				else
					this.BlockUser(name);
		}
		private void wfClient_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			// We only want to close by the 'Exit' menu item.
			e.Cancel = !allowExit;
			if (!allowExit)
			{
				// We should warn them that closing the window doesn't really close the window, but rather
				// just hides it.
				// Load the default values from the registry
				Microsoft.Win32.RegistryKey key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\\" + MessengerShared.ApplicationName + "\\Client", true);
				// If the key doesn't exist, create it
				if (key == null)
					key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("Software\\" + MessengerShared.ApplicationName + "\\Client");

				bool bWarn = bool.Parse(key.GetValue("Warn", true).ToString());

				if (bWarn)
				{
					if (MessageBox.Show("Closing this window doesn't actually stop the application.  The application is still running in the taskbar.\r\nIn order to shut down the application entirely, please choose Exit from the menu.\r\n\r\nDo you wish to see this warning again?","Not closing", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
						key.SetValue("Warn", true);
					else
						key.SetValue("Warn", false);
				}
				key.Close();

				this.Hide();
			}
		}

		private void mnuExit_Click(object sender, System.EventArgs e)
		{
			allowExit = true;
			this.Dispose();
		}

		private void wfClient_Resize(object sender, System.EventArgs e)
		{
			if (this.WindowState == FormWindowState.Minimized)
				this.Hide();
		}
		private void mnuLogon_Click(object sender, System.EventArgs e)
		{
			wfLogin loginDialog = new wfLogin(this);
			loginDialog.ShowDialog(this);
			loginDialog.Dispose();
		}

		private void tvwItems_BeforeSelect(object sender, System.Windows.Forms.TreeViewCancelEventArgs e)
		{
			TreeNode node = null;
			if (e.Node.Parent == null)
			{
				EnableSendUi(false, false);
				return;
			}
			else
				node = e.Node;

			if ((node.Nodes.Count == 0) && (node.Parent != offlineNode)) 
				EnableSendUi(true, node.Nodes.Count == 0);
			else
				EnableSendUi(false, node.Nodes.Count == 0);

		}
		private void mnuSend_Click(object sender, System.EventArgs e)
		{
			wfMsg msgWindow = this.GetMessageWindow(tvwItems.SelectedNode.Tag.ToString());
		}
		private void mnuLogoff_Click(object sender, System.EventArgs e)
		{
			// We've logged off.  
			this.EnableLoggedInUi(false);
			gConnected = false;
			gUsername = null;
			gServer = null;	
			this.UpdateText(MessengerShared.ApplicationName + " Service (Not logged in)");
			this.SetupDefaultTree();
			// Shut down our DPlay client
			if (client != null)
				client.Dispose();
			client = null;
		}
		private void mnuTree_Popup(object sender, System.EventArgs e)
		{
			// Get the node where the mouse is right now
			TreeNode node = tvwItems.GetNodeAt(tvwItems.PointToClient(Control.MousePosition));
			if (node == null)
				return;

			// Set this node as our selected node
			tvwItems.SelectedNode = node;
			if (node.Parent == null)
			{
				EnableSendUi(false, false);
				return;
			}

			if ((node.Nodes.Count == 0) && (node.Parent != offlineNode)) 
				EnableSendUi(true, node.Nodes.Count == 0);
			else
				EnableSendUi(false, node.Nodes.Count == 0);
		}
		private void mnuDelete_Click(object sender, System.EventArgs e)
		{
			// We want to delete a friend, who?
			string username = (string)tvwItems.SelectedNode.Tag;
			NetworkPacket stm = new NetworkPacket();

			// Add the DeleteFriend message type, and friends name, and let the server know
			stm.Write(MessageType.DeleteFriend);
			stm.Write(username);
			client.Send(stm, 0, 0);

		}
		#endregion
		#region Player Logon/Creation routines
		public void LogonPlayer(string playername, string password)
		{
			NetworkPacket stm = new NetworkPacket();

			gUsername = playername;
			// We're connected now, send our login information
			stm.Write(MessageType.Login);
			stm.Write(playername);
			stm.Write(password);
			// Send to the server
			client.Send(stm, 0, 0);
		}
		public void LogonPlayer(string server, string playername, string password)
		{
			// Try the connect now
			this.Initialize(); // Make sure dplay is setup properly
			host.AddComponent(Address.KeyHostname, server);
			
			ApplicationDescription desc = new ApplicationDescription();
			desc.GuidApplication = MessengerShared.applicationGuid;

			try
			{
				// Connect to this server
				client.Connect(desc, host, device, null, 0);

				// Now we will sit and wait in a loop for our connect complete event to fire
				while (!hConnect.WaitOne(0, false))
				{
					// Don't kill the CPU, allow other things to happen while we wait
					Application.DoEvents();
				}

				if (gConnected) // We connected, try to log on now
				{
					gServer = server;
					LogonPlayer(playername, password);
				}
			}
			catch
			{
				MessageBox.Show("This server could not be contacted.  Please check the server name and try again.", "Server not found.", MessageBoxButtons.OK, MessageBoxIcon.Information);
			}

		}
		public void CreatePlayer(string playername, string password)
		{
			NetworkPacket stm = new NetworkPacket();

			gUsername = playername;
			// We're connected now, send our login information
			stm.Write(MessageType.CreateNewAccount);
			stm.Write(playername);
			stm.Write(password);
			// Send to the server
			client.Send(stm, 0, 0);
		}
		public void CreatePlayer(string server, string playername, string password)
		{
			// Try the connect now
			this.Initialize(); // Make sure dplay is setup properly
			host.AddComponent(Address.KeyHostname, server);
			
			ApplicationDescription desc = new ApplicationDescription();
			desc.GuidApplication = MessengerShared.applicationGuid;

			try
			{
				// Connect to this server
				client.Connect(desc, host, device, null, 0);
			}
			catch
			{
				MessageBox.Show("This server could not be contacted.  Please check the server name and try again.", "Server not found.", MessageBoxButtons.OK, MessageBoxIcon.Information);
			}

			// Now we will sit and wait in a loop for our connect complete event to fire
			while (!hConnect.WaitOne(0, false))
			{
				// Don't kill the CPU, allow other things to happen while we wait
				Application.DoEvents();
			}

			if (gConnected) // We connected, try to log on now
			{
				gServer = server;
				CreatePlayer(playername, password);
			}
		}
		public void SendChatMessage(string username, string msg)
		{
			NetworkPacket stm = new NetworkPacket();

			stm.Write(MessageType.SendMessage);
			stm.Write(username);
			stm.Write(gUsername);
			stm.Write(msg);
			client.Send(stm, 0, 0);
		}
		#endregion
		#region MessageWindow function helpers
		public wfMsg GetMessageWindow(string username)
		{
			lock(msgForms)
			{
				// Scroll through all existing message windows and see if we have one for this user
				foreach (wfMsg f in msgForms)
				{
					if (f.UserName.ToLower() == username.ToLower())
						return f;
				}
			}
			// Well shoot.. Nothing here, guess we need to create one..  We don't want to 
			// Create the form on a thread that isn't our main UI thread (ie, a DPlay thread)
			// so we will use the Invoke method.  We use invoke rather than BeginInvoke
			// because we want this call to be synchronous
			object[] newParams = { username };
			this.Invoke(new CreateMsgWindowCallback(this.CreateMessageWindow), newParams);
			lock(msgForms)
			{
				foreach (wfMsg f in msgForms)
				{
					if (f.UserName.ToLower() == username.ToLower())
						return f;
				}
			}
			return null;
		}
		public void CreateMessageWindow(string username)
		{
			// Well, we got here, guess there hasn't been one yet
			// Let's create it
			wfMsg retMsg = new wfMsg(username, this);
			// Show the window now (we will do this again, but that's ok)
			retMsg.Show();
			retMsg.Select();
			retMsg.BringToFront();
			lock (msgForms)
			{
				msgForms.Add(retMsg);
			}
			// We need to know if a form is disposed
			retMsg.Disposed += new System.EventHandler(this.MessageWindowDisposed);
		}
		public void MessageWindowDisposed(object sender, EventArgs e)
		{
			lock(msgForms)
			{
				// Search through our forms, and remove the item that's been disposed
				foreach (wfMsg f in msgForms)
				{
					if (f.Equals(sender))
					{
						msgForms.Remove(f);
						break;
					}
				}
			}
		}
		#endregion
		#region BeginInvoke Delegate Handlers
		private void ServerExit(MessageType msg)
		{
			if (msg == MessageType.LogonOtherLocation)
				MessageBox.Show("You have been disconnected because you've logged on at another location.", "Logged on elsewhere.", MessageBoxButtons.OK, MessageBoxIcon.Information);
			else
				if (msg == MessageType.ServerKick)
					MessageBox.Show("The server administrator has disconnected you.", "Server kicked.", MessageBoxButtons.OK, MessageBoxIcon.Information);
				else
					MessageBox.Show("Your connection with the server has been lost.  You have been logged off.", "Server disconnected.", MessageBoxButtons.OK, MessageBoxIcon.Information);

			this.mnuLogoff_Click(null, null);
		}
		private void AddNode(TreeNode parent, string username, bool friend)
		{
			TreeNode newNode = null;

			bool found = false;
			// First scroll through all of the nodes, and remove this if it exists
			foreach (TreeNode nTop in tvwItems.Nodes) // This will only scroll through top level nodes
			{
				// Since our outter loop only scrolls through the top level nodes
				// namely, the Online/Offline leafs, we will scroll through each of 
				// their nodes in this loop
				foreach (TreeNode n in nTop.Nodes)
				{
					if (n.Tag.ToString().ToLower() == username.ToLower())
					{
						if (friend)
							n.Text = username;
						else
							n.Text = username + " (BLOCKED)";

						found = true;
						break;
					}
				}
			}

			if (!found)
			{
				// Now let's add this user to the correct node
				if (friend)
					newNode = new TreeNode(username, 0, 0);
				else
					newNode = new TreeNode(username + " (BLOCKED)", 0, 0);

				newNode.Tag = (object)username;

				parent.Nodes.Add(newNode);
			}

			tvwItems.ExpandAll();
		}
		private void NodeLogonOff(TreeNode parent, string username)
		{
			TreeNode newNode = null;

			bool found = false;
			string text = null;

			// First scroll through all of the nodes, and remove this if it exists
			foreach (TreeNode nTop in tvwItems.Nodes) // This will only scroll through top level nodes
			{
				// Since our outter loop only scrolls through the top level nodes
				// namely, the Online/Offline leafs, we will scroll through each of 
				// their nodes in this loop
				foreach (TreeNode n in nTop.Nodes)
				{
					if (n.Tag.ToString().ToLower() == username.ToLower())
					{
						found = true;
						text = n.Text;
						nTop.Nodes.Remove(n);
						break;
					}
				}
			}

			if (found)
				// Now let's add this user to the correct node
				newNode = new TreeNode(text, 0, 0);
			else
				newNode = new TreeNode(username, 0, 0);

			newNode.Tag = (object)username;
			parent.Nodes.Add(newNode);

			tvwItems.ExpandAll();
		}
		private void RemoveNode(string username)
		{
			// First scroll through all of the nodes, and remove this if it exists
			foreach (TreeNode nTop in tvwItems.Nodes) // This will only scroll through top level nodes
			{
				// Since our outter loop only scrolls through the top level nodes
				// namely, the Online/Offline leafs, we will scroll through each of 
				// their nodes in this loop
				foreach (TreeNode n in nTop.Nodes)
				{
					if (n.Tag.ToString().ToLower() == username.ToLower())
					{
						nTop.Nodes.Remove(n);
						break;
					}
				}
			}
			tvwItems.ExpandAll();
		}
		#endregion
		#region DPlayEvent Handlers
		private void ConnectComplete(object sender, ConnectCompleteEventArgs e)
		{
			// Did we really connect?
			if (e.Message.ResultCode != 0)
				MessageBox.Show("The server does not exist, or is unavailable.", "Unavailable", MessageBoxButtons.OK, MessageBoxIcon.Information);
			else
				gConnected = true;

			// Set off the connect event
			hConnect.Set();
		}
		private void DataReceived(object sender, ReceiveEventArgs e)
		{
			// We've received data, process it
			string username = null;
			string chatmsg = null;
			bool friend = false;
			wfMsg msgWindow = null;

			MessageType msg = (MessageType)e.Message.ReceiveData.Read(typeof(MessageType)); 
			switch (msg)
			{
				case MessageType.LoginSuccess: // We've been logged in
					this.EnableLoggedInUi(true);
					this.UpdateText(MessengerShared.ApplicationName + " - (" + gUsername + ")");
					break;
				case MessageType.InvalidPassword: // Server says we entered the wrong password
					MessageBox.Show("The password you've entered is invalid.", "Invalid password", MessageBoxButtons.OK, MessageBoxIcon.Information);
					break;
				case MessageType.InvalidUser:
					MessageBox.Show("The username you've entered is invalid.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information);
					break;
				case MessageType.UserAlreadyExists:
					MessageBox.Show("The username you've entered already exists.\nYou must choose a different user name.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information);
					break;
				case MessageType.SendClientFriends:
					int numFriends = (int)e.Message.ReceiveData.Read(typeof(int));
					// Ok, we know how many friends are in the message, let's go through them all
					for (int i = 0; i < numFriends; i++)
					{
						friend = (bool)e.Message.ReceiveData.Read(typeof(bool));
						username = e.Message.ReceiveData.ReadString();

						object[] newParams = {offlineNode, username, friend};
						// User our callback to add our friend to this node
						this.BeginInvoke(new AddNodeCallback(this.AddNode), newParams);
					}
					break;
				case MessageType.FriendAdded:
					username = e.Message.ReceiveData.ReadString();
					object[] addParams = {offlineNode, username, true};
					// User our callback to add our friend to this node
					this.BeginInvoke(new AddNodeCallback(this.AddNode), addParams);
					MessageBox.Show(username + " added succesfully to your friends list.", "Added", MessageBoxButtons.OK, MessageBoxIcon.Information);
					break;
				case MessageType.FriendDeleted:
					username = e.Message.ReceiveData.ReadString();
					object[] deleteParams = {username};
					// User our callback to add our friend to this node
					this.BeginInvoke(new RemoveNodeCallback(this.RemoveNode), deleteParams);
					break;
				case MessageType.FriendBlocked:
					username = e.Message.ReceiveData.ReadString();
					object[] blockParams = {offlineNode, username, false};
					// User our callback to add our friend to this node
					this.BeginInvoke(new AddNodeCallback(this.AddNode), blockParams);
					MessageBox.Show(username + " has been blocked succesfully.", "Blocked", MessageBoxButtons.OK, MessageBoxIcon.Information);
					break;
				case MessageType.FriendDoesNotExist:
					MessageBox.Show("You cannot add this friend since they do not exist.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information);
					break;
				case MessageType.BlockUserDoesNotExist:
					MessageBox.Show("You cannot block this user since they do not exist.", "Invalid user", MessageBoxButtons.OK, MessageBoxIcon.Information);
					break;
				case MessageType.FriendLogon:
					username = e.Message.ReceiveData.ReadString();
					object[] logonParams = {onlineNode, username};
					// User our callback to add our friend to this node
					this.BeginInvoke(new NodeLogonOffCallback(this.NodeLogonOff), logonParams);
					break;
				case MessageType.FriendLogoff:
					username = e.Message.ReceiveData.ReadString();
					object[] logoffParams = {offlineNode, username};
					// User our callback to add our friend to this node
					this.BeginInvoke(new NodeLogonOffCallback(this.NodeLogonOff), logoffParams);
					break;
				case MessageType.ReceiveMessage:
					username = e.Message.ReceiveData.ReadString();
					chatmsg =  e.Message.ReceiveData.ReadString();
					// We need to get a message window
					msgWindow = this.GetMessageWindow(username);

					// Show the window, and add the text
					msgWindow.BeginInvoke(new wfMsg.ShowWindowCallback(msgWindow.ShowWindow));

					object[] receiveMsg = {chatmsg, false, false };
					msgWindow.BeginInvoke(new wfMsg.AddChatMsgCallback(msgWindow.AddChatMessage), receiveMsg);

					break;
				case MessageType.UserBlocked:
					username = e.Message.ReceiveData.ReadString();
					chatmsg =  "Your message to " + username + " could not be delivered because this user has you blocked.";
					msgWindow = this.GetMessageWindow(username);
					// Show the window, and add the text
					msgWindow.BeginInvoke(new wfMsg.ShowWindowCallback(msgWindow.ShowWindow));

					object[] blockMsg = {chatmsg, false, true };
					msgWindow.BeginInvoke(new wfMsg.AddChatMsgCallback(msgWindow.AddChatMessage), blockMsg);
					break;
				case MessageType.UserUnavailable:
					username = e.Message.ReceiveData.ReadString();
					chatmsg =  e.Message.ReceiveData.ReadString();
					msgWindow = this.GetMessageWindow(username);
					// Show the window, and add the text
					msgWindow.BeginInvoke(new wfMsg.ShowWindowCallback(msgWindow.ShowWindow));

					object[] unavailableMsg = {"Your Message: \r\n" + chatmsg + "\r\nto " + username + " could not be delivered because the user is no longer available.", false, true };
					msgWindow.BeginInvoke(new wfMsg.AddChatMsgCallback(msgWindow.AddChatMessage), unavailableMsg);
					break;
			}
			e.Message.ReceiveData.Dispose(); // Don't need the data anymore
		}
		private void SessionLost(object sender, SessionTerminatedEventArgs e)
		{
			if (e.Message.TerminateData.Length != 0) // We've received terminate session data.
			{
				MessageType msg = (MessageType)e.Message.TerminateData.Read(typeof(MessageType));
				object[] logOffParams = {msg};
				this.BeginInvoke(new ServerExitCallback(this.ServerExit), logOffParams);
			}
			else
			{
				object[] discParams = {MessageType.AddFriend}; // Doesn't really matter, just disconnect
				// We need to inform the user of the disconnection.  We will call BeginInvoke as to not
				// block this thread
				this.BeginInvoke(new ServerExitCallback(this.ServerExit), discParams);
			}
		}
		#endregion
		static void Main() 
		{
			Application.Run(new wfClient());
		}

	}
}
