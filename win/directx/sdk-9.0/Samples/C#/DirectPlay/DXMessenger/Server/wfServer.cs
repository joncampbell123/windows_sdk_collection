//----------------------------------------------------------------------------
// File: wfServer.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Xml;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;
using DXMessenger;

namespace DXMessengerServer
{
	public enum LogonTypes
	{
		LogonSuccess, // The logon was a success
		InvalidPassword, // The user exists, but this isn't his password
		AccountDoesNotExist, // This account doesn't exist.
	}
	/// <summary>
	/// Summary description for wfServer.
	/// </summary>
	public class wfServer : System.Windows.Forms.Form
	{
		private bool allowExit = false;
		public const int maxUsers = 1000;
		private bool serverStarted = false;
		Server server = null; // The server object
		DataStore data = null;
		private int numPlayers = 0;
		System.Timers.Timer updateTimer = null;
		private int timeCount = 0;

		private struct PlayerObject
		{
			public int playerId;
			public string playerName;
			// We could also store more statistics here, ie, msg's sent, etc
			public override string ToString()
			{
				return string.Format("{0} - DirectPlay ID = 0x{1}", this.playerName , this.playerId.ToString("X"));
			}
		}

		private void SendChatMessage(string username, string from, string chatmsg)
		{
			int sendId = 0;
			NetworkPacket stm = new NetworkPacket();

			if (!serverStarted)
				return;

			// Before sending this message, check to see if we are blocked.
			if (data.AmIBlocked(username, from))
			{
				sendId = data.GetCurrentPlayerId(from);
				stm.Write(MessageType.UserBlocked);
				stm.Write(username);
				server.SendTo(sendId, stm, 0, 0);
			}
			else
			{
				sendId = data.GetCurrentPlayerId(username);
				if (sendId == 0) // This user isn't logged on anymore
				{
					stm.Write(MessageType.UserUnavailable);
					stm.Write(username);
					stm.Write(chatmsg);
				}
				else // Ok, send the message to the client
				{
					stm.Write(MessageType.ReceiveMessage);
					stm.Write(from);
					stm.Write(chatmsg);
				}
				server.SendTo(sendId, stm, 0, 0);
			}
		}
		#region WindowsForms variables
		private System.Windows.Forms.MainMenu mnuMain;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem mnuKick;
		private System.Windows.Forms.ContextMenu lstPop;
		private System.Windows.Forms.MenuItem mnuMainExit;
		private System.Windows.Forms.MenuItem mnuKickPop;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ListBox lstUsers;
		private System.Windows.Forms.Button btnStart;
		private System.Windows.Forms.StatusBar statusBar;
		private System.Windows.Forms.NotifyIcon notifyIcon;
		private System.Windows.Forms.ContextMenu popUp;
		private System.Windows.Forms.MenuItem mnuShow;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem mnuExit;
		private System.ComponentModel.IContainer components;
		#endregion
		#region WindowsForms functions (Constructor/Dispose)
		public wfServer()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			// Set up our icon in the system tray
			this.notifyIcon.Icon = this.Icon;
			this.notifyIcon.Visible = true;
			//this.notifyIcon.MouseUp += new System.Windows.Forms.MouseEventHandler(this.NotifyClick);
			this.notifyIcon.DoubleClick += new System.EventHandler(this.ShowForm);
			// Set our status bar /Icon text
			UpdateText();

			//Load our XML data
			data = new DataStore();

			// Start a timer, it will fire off every 60 seconds
			updateTimer = new System.Timers.Timer(60000);
			updateTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.TimerElapsed);
			updateTimer.Start();
			this.UpdateMenuItems();

			// Automatically start the server
			this.btnStart_Click(this.btnStart, null);
		}

		protected override void Dispose(bool disposing)
		{
			if (updateTimer != null)
			{
				updateTimer.Dispose();
				updateTimer = null;
			}
			if (server != null)
			{
				server.Dispose();
				lstUsers.Items.Clear();
				server = null;
			}
			if (disposing)
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
			if (data != null)
				data.Close();
		}

		#endregion
		#region Misc event handlers (Timer/NotifyIcon)
		private void TimerElapsed(object sender, System.Timers.ElapsedEventArgs e)
		{
			// Just to make sure the data isn't lost, every 5 minutes we will save our 
			// xml schema to a file.
			timeCount++;
			if (timeCount > 4)
			{
				timeCount = 0;
				if (data != null)
					data.SaveDataStore();
			}
		}
		private void ShowForm(object sender, EventArgs e)
		{
			// If we're calling this, the window is probably minimized.  First restore it
			this.WindowState = FormWindowState.Normal;
			this.Refresh();
			this.BringToFront();
			this.Show(); // Now show the window and bring it to the front
		}
		#endregion
		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.label1 = new System.Windows.Forms.Label();
			this.lstUsers = new System.Windows.Forms.ListBox();
			this.lstPop = new System.Windows.Forms.ContextMenu();
			this.mnuKickPop = new System.Windows.Forms.MenuItem();
			this.btnStart = new System.Windows.Forms.Button();
			this.statusBar = new System.Windows.Forms.StatusBar();
			this.notifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
			this.popUp = new System.Windows.Forms.ContextMenu();
			this.mnuShow = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.mnuExit = new System.Windows.Forms.MenuItem();
			this.mnuMain = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.mnuMainExit = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.mnuKick = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Anchor = ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(288, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Users currently in this session:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// lstUsers
			// 
			this.lstUsers.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.lstUsers.ContextMenu = this.lstPop;
			this.lstUsers.Location = new System.Drawing.Point(3, 20);
			this.lstUsers.Name = "lstUsers";
			this.lstUsers.Size = new System.Drawing.Size(284, 238);
			this.lstUsers.TabIndex = 1;
			this.lstUsers.SelectedIndexChanged += new System.EventHandler(this.lstUsers_SelectedIndexChanged);
			// 
			// lstPop
			// 
			this.lstPop.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																				   this.mnuKickPop});
			// 
			// mnuKickPop
			// 
			this.mnuKickPop.Index = 0;
			this.mnuKickPop.Text = "Kick User";
			this.mnuKickPop.Click += new System.EventHandler(this.mnuKick_Click);
			// 
			// btnStart
			// 
			this.btnStart.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right);
			this.btnStart.Location = new System.Drawing.Point(211, 265);
			this.btnStart.Name = "btnStart";
			this.btnStart.Size = new System.Drawing.Size(74, 23);
			this.btnStart.TabIndex = 2;
			this.btnStart.Text = "Start Server";
			this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
			// 
			// statusBar
			// 
			this.statusBar.Location = new System.Drawing.Point(0, 291);
			this.statusBar.Name = "statusBar";
			this.statusBar.Size = new System.Drawing.Size(292, 17);
			this.statusBar.TabIndex = 3;
			// 
			// notifyIcon
			// 
			this.notifyIcon.ContextMenu = this.popUp;
			this.notifyIcon.Text = "";
			this.notifyIcon.Visible = true;
			// 
			// popUp
			// 
			this.popUp.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																				  this.mnuShow,
																				  this.menuItem2,
																				  this.mnuExit});
			// 
			// mnuShow
			// 
			this.mnuShow.DefaultItem = true;
			this.mnuShow.Index = 0;
			this.mnuShow.Text = "Show";
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 1;
			this.menuItem2.Text = "-";
			// 
			// mnuExit
			// 
			this.mnuExit.Index = 2;
			this.mnuExit.Text = "Exit";
			this.mnuExit.Click += new System.EventHandler(this.mnuExit_Click);
			// 
			// mnuMain
			// 
			this.mnuMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.menuItem1,
																					this.menuItem4});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.mnuMainExit});
			this.menuItem1.Text = "&File";
			// 
			// mnuMainExit
			// 
			this.mnuMainExit.Index = 0;
			this.mnuMainExit.Text = "E&xit";
			this.mnuMainExit.Click += new System.EventHandler(this.mnuExit_Click);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 1;
			this.menuItem4.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.mnuKick});
			this.menuItem4.Text = "&Options";
			// 
			// mnuKick
			// 
			this.mnuKick.Index = 0;
			this.mnuKick.Text = "&Kick User";
			this.mnuKick.Click += new System.EventHandler(this.mnuKick_Click);
			// 
			// wfServer
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(292, 308);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.statusBar,
																		  this.btnStart,
																		  this.lstUsers,
																		  this.label1});
			this.MaximizeBox = false;
			this.Menu = this.mnuMain;
			this.MinimumSize = new System.Drawing.Size(300, 250);
			this.Name = "wfServer";
			this.Text = "DirectX Messenger Server";
			this.Resize += new System.EventHandler(this.wfServer_Resize);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.wfServer_Closing);
			this.ResumeLayout(false);

		}
		#endregion
		#region Form EventHandlers
		private void wfServer_Resize(object sender, System.EventArgs e)
		{
			if (this.WindowState == FormWindowState.Minimized)
				this.Hide();
		}
		private void UpdateText()
		{
			if (!serverStarted)
				statusBar.Text = notifyIcon.Text = "Sever not yet started... (" + numPlayers + @"/" + maxUsers + " connected).";
			else
				statusBar.Text = notifyIcon.Text = "Sever running... (" + numPlayers + @"/" + maxUsers + " connected).";
		}

		private void btnStart_Click(object sender, System.EventArgs e)
		{
			serverStarted = !serverStarted;
			if (serverStarted) // Do the opposite since we just changed it a second ago
			{
				// Set up the application description
				ApplicationDescription desc = new ApplicationDescription();
				desc.GuidApplication = MessengerShared.applicationGuid;
				desc.MaxPlayers = maxUsers;
				desc.SessionName = "DxMessengerServer";
				desc.Flags = SessionFlags.ClientServer | SessionFlags.NoDpnServer;

				// Create a new address that will reside on tcpip and listen only on our port
				Address add = new Address();
				add.ServiceProvider = Address.ServiceProviderTcpIp;
				add.AddComponent(Address.KeyPort, MessengerShared.DefaultPort);

				// Now we can start our server
				server = new Server();
				// Add our event handlers
				server.PlayerDestroyed += new PlayerDestroyedEventHandler(this.DestroyPlayerMsg);
				server.Receive += new ReceiveEventHandler(this.DataReceivedMsg);
				// Host this session
				server.Host(desc, add);

				this.btnStart.Text = "Stop Server";
				UpdateText();
			}
			else
			{
				this.btnStart.Text = "Start Server";
				lstUsers.Items.Clear();
                numPlayers = 0;
				server.Dispose();
				server = null;
				//Update our text, the server should not be running
				UpdateText();
			}
		}
		private void mnuExit_Click(object sender, System.EventArgs e)
		{
			serverStarted = false;
			allowExit = true;
			this.Dispose();
		}
		private void wfServer_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			// We only want to close by the 'Exit' menu item.
			e.Cancel = !allowExit;
			if (!allowExit)
			{
				// We should warn them that closing the window doesn't really close the window, but rather
				// just hides it.
				// Load the default values from the registry
				Microsoft.Win32.RegistryKey key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\\" + MessengerShared.ApplicationName + "\\Server", true);
				// If the key doesn't exist, create it
				if (key == null)
					key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("Software\\" + MessengerShared.ApplicationName + "\\Server");

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
		private void lstUsers_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			this.UpdateMenuItems();
		}
		private void mnuKick_Click(object sender, System.EventArgs e)
		{
			NetworkPacket stm = new NetworkPacket();

			// First check to see if this user is already logged on, if so, store the DPlayID
			int playerId = ((PlayerObject)lstUsers.SelectedItem).playerId;
			// Let the user know that they've been destroyed for this reason
			stm.Write(MessageType.ServerKick);
			server.DestroyClient(playerId, stm);
		}
		private void UpdateMenuItems()
		{
			mnuKick.Enabled = lstUsers.SelectedIndex >= 0;
			mnuKickPop.Enabled = lstUsers.SelectedIndex >= 0;
		}

		#endregion
		#region DPlay EventHandlers
		void DestroyPlayerMsg(object sender, PlayerDestroyedEventArgs e)
		{
			if (!serverStarted)
				return;

			// A user has quit
			foreach (PlayerObject p in lstUsers.Items)
			{
				// If they are in the user list, remove them
				if (p.playerId == e.Message.PlayerID)
				{
					lstUsers.Items.Remove(p);
					numPlayers--;
					// We should also notify everyone they've logged off.
					if (data != null)
					{
						// Notify everyone that we've logged off
						data.NotifyFriends(p.playerName, MessageType.FriendLogoff, server);
						// Update the schema to show we've logged off
						data.UpdateDataToReflectLogoff(e.Message.PlayerID);
					}
					break;
				}
			}
			//We always want to update our text
			UpdateText();
		}
		void DataReceivedMsg(object sender, ReceiveEventArgs e)
		{
			// We've received data, process it
			string username = null;
			string password = null;
			NetworkPacket stm = new NetworkPacket();

			if (!serverStarted)
				return;

			MessageType msg = (MessageType)e.Message.ReceiveData.Read(typeof(MessageType));
			switch (msg)
			{
				case MessageType.AddFriend:
					username = e.Message.ReceiveData.ReadString();
					if (!data.DoesUserExist(username))
					{
						// This user doesn't exist, how can we add them?  Notify the user
						// of this error.
						stm.Write(MessageType.FriendDoesNotExist);
						server.SendTo(e.Message.SenderID, stm, 0, 0);
					}
					else
					{
						// Great, add this user to our friend list
						bool loggedin = data.AddFriend(e.Message.SenderID, username, true);
						// Let the client know it's been added
						stm.Write(MessageType.FriendAdded);
						stm.Write(username);
						server.SendTo(e.Message.SenderID, stm, 0, 0);
						if (loggedin)
						{
							stm = new NetworkPacket();
							stm.Write(MessageType.FriendLogon);
							stm.Write(username);
							server.SendTo(e.Message.SenderID, stm, 0, 0);
						}
					}
					break;
				case MessageType.BlockFriend:
					username = e.Message.ReceiveData.ReadString();
					if (!data.DoesUserExist(username))
					{
						// This user doesn't exist, how can we block them?  Notify the user
						// of this error.
						stm.Write(MessageType.BlockUserDoesNotExist);
						server.SendTo(e.Message.SenderID, stm, 0, 0);
					}
					else
					{
						// Great, block this user from our friend list
						bool loggedin = data.AddFriend(e.Message.SenderID, username, false);
						// Let the client know it's been blocked
						stm.Write(MessageType.FriendBlocked);
						stm.Write(username);
						server.SendTo(e.Message.SenderID, stm, 0, 0);
						if (loggedin)
						{
							stm = new NetworkPacket();
							stm.Write(MessageType.FriendLogon);
							stm.Write(username);
							server.SendTo(e.Message.SenderID, stm, 0, 0);
						}
					}
					break;
				case MessageType.DeleteFriend:
					username = e.Message.ReceiveData.ReadString();
					if (!data.DoesUserExist(username))
					{
						// This user doesn't exist, we can't delete them, this should never occur so just ignore it
					}
					else
					{
						// Great, block this user from our friend list
						data.DeleteFriend(e.Message.SenderID, username);
						// Let the client know it's been blocked
						stm.Write(MessageType.FriendDeleted);
						stm.Write(username);
						server.SendTo(e.Message.SenderID, stm, 0, 0);
					}
					break;
				case MessageType.CreateNewAccount:
					username = e.Message.ReceiveData.ReadString();
					password = e.Message.ReceiveData.ReadString();
					if (data.DoesUserExist(username))
					{
						// This user already exists, inform the user so they can try a new name
						stm.Write(MessageType.UserAlreadyExists);
						server.SendTo(e.Message.SenderID, stm, 0, 0);
					}
					else
					{
						// Good, this user doesn't exist, let's add it.
						data.AddUser(username, password, e.Message.SenderID);
						// We don't need to inform anyone of our logon, we were just created
						// it's impossible to be listed as someone's friend already

						// Notify the client they've been logged on succesfully
						stm.Write(MessageType.LoginSuccess);
						server.SendTo(e.Message.SenderID, stm, 0, 0);

						// Increment the player count
						numPlayers++;
						// Add this user to our listbox of current players
						PlayerObject player = new PlayerObject();
						player.playerId = e.Message.SenderID;
						player.playerName = username;
						lstUsers.Items.Add(player);
						UpdateText();
					}
					break;
				case MessageType.Login:
					username = e.Message.ReceiveData.ReadString();
					password = e.Message.ReceiveData.ReadString();
					// First check to see if this user is already logged on, if so, store the DPlayID
					int playerId = data.GetCurrentPlayerId(username);
					// Try to login with this username/password
					switch (data.LogonUser(username, password))
					{
						case LogonTypes.LogonSuccess:
							// Great, they've logged on
							data.UpdateDataToReflectLogon(username, e.Message.SenderID);
							// Now check to see if they were logged on somewhere else before.  
							// If so, kill that instance.
							if (playerId != 0)
							{
								// Let the user know that they've been destroyed for this reason
								stm.Write(MessageType.LogonOtherLocation);
								server.DestroyClient(playerId, stm);
								stm = new NetworkPacket();
							}
							// Notify the user they've been logged in
							stm.Write(MessageType.LoginSuccess);
							server.SendTo(e.Message.SenderID, stm, 0, 0);

							// Tell everyone who is my friend I've logged on
							data.NotifyFriends(username, MessageType.FriendLogon, server);
							// Find out if any of my friends are online, and notify me if so
							data.FindMyFriends(username, server);
							// Increment the player count
							numPlayers++;
							// Add this user to our listbox of current players
							PlayerObject player = new PlayerObject();
							player.playerId = e.Message.SenderID;
							player.playerName = username;
							lstUsers.Items.Add(player);
							UpdateText();
							break;
						case LogonTypes.InvalidPassword:
							// Notify the user they don't know the password
							stm.Write(MessageType.InvalidPassword);
							server.SendTo(e.Message.SenderID, stm, 0, 0);
							break;
						case LogonTypes.AccountDoesNotExist:
							// Notify the user this account doesn't exist
							stm.Write(MessageType.InvalidUser);
							server.SendTo(e.Message.SenderID, stm, 0, 0);
							break;
					}
					break;
				case MessageType.SendMessage:
					username = e.Message.ReceiveData.ReadString();
					string from = e.Message.ReceiveData.ReadString();
					string chatmsg = e.Message.ReceiveData.ReadString();
					this.SendChatMessage(username, from, chatmsg);
					break;
			}
			e.Message.ReceiveData.Dispose(); // Don't need the data anymore
		}
		#endregion
		static void Main() 
		{
			Application.Run(new wfServer());
		}

	}
}
