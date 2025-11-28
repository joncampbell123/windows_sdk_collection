//----------------------------------------------------------------------------
// File: ChatPeer.cs
//
// Desc: The main game file for the ChatPeer sample.  It connects 
//       players together with dialog boxes to prompt users on the 
//       connection settings to join or create a session. After the user 
//       connects to a session, the sample displays the chat dialog. 
// 
//       After a new game has started the sample begins a very simplistic 
//       chat session where users can send text to each other.
//      
//       This sample Interops with the C++ and VB.NET version of the 
//       sample as well.
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;


namespace ChatPeerSample
{
	/// <summary>
	/// Summary description for ChatPeer.
	/// </summary>
	public class ChatPeer : System.Windows.Forms.Form
	{
		private delegate void PeerCloseCallback(); // This delegate will be called when the session terminated event is fired.
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label lblUsers;
		private System.Windows.Forms.Button btnExit;
		private System.Windows.Forms.TextBox txtChat;
		private System.Windows.Forms.Button btnSend;
		private System.Windows.Forms.TextBox txtSend;


		/// <summary>
		/// Our players structure
		/// </summary>
		public struct Players
		{
			public int playerId;
			public string Name;
			public Players(int id, string n)
			{ playerId = id; Name = n; }
		}

		private const int MaxChatStringLength  = 508;
		private const byte ChatMessageId = 1;
        private const int DefaultPort = 2502;

		// Local variables for this app
		public Peer peerObject = null; // Main DPlay object
		private ConnectWizard connectWizard = null; // The wizard to create/join a DPlay Session
		private ArrayList playerList = new ArrayList();
		private int localPlayerId;

		// This GUID allows DirectPlay to find other instances of the same game on
		// the network.  So it must be unique for every game, and the same for 
		// every instance of that game.  // {876A3036-FFD7-46bc-9209-B42F617B9BE7}
		// However, we are using the same guid the C++ and VB.NET version of the 
		// samples use so they can all communicate together.
		public Guid localApplicationGuid = new Guid(0x876a3036, 0xffd7, 0x46bc, 0x92, 0x9, 0xb4, 0x2f, 0x61, 0x7b, 0x9b, 0xe7);


        /// <summary>
        /// Constuctor
        /// </summary>
		public ChatPeer()
		{
            try
            {
                // Load the icon from our resources
                System.Resources.ResourceManager resources = new System.Resources.ResourceManager(this.GetType());
                this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            }
            catch
            {
                // It's no big deal if we can't load our icons, but try to load the embedded one
                try { this.Icon = new System.Drawing.Icon(this.GetType(), "directx.ico"); } 
                catch {}
            }

            //
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			peerObject = new Peer();
			// First set up our event handlers (We only need events for the ones we care about)
			peerObject.PlayerCreated += new PlayerCreatedEventHandler(this.PlayerCreated);
			peerObject.PlayerDestroyed += new PlayerDestroyedEventHandler(this.PlayerDestroyed);
			peerObject.HostMigrated += new HostMigratedEventHandler(this.HostMigrated);
			peerObject.Receive += new ReceiveEventHandler(this.DataReceived);
			peerObject.SessionTerminated += new SessionTerminatedEventHandler(this.SessionTerminated);
			connectWizard = new ConnectWizard(peerObject, localApplicationGuid, "Chat Peer");
            
            connectWizard.DefaultPort = DefaultPort;
			if (connectWizard.StartWizard())
			{
				// Great we've connected (or joined)..  Now we can start the sample
				// Are we the host?
				if (connectWizard.IsHost)
					this.Text += " (HOST)";
			}
			else
				// We obviously didn't want to start a session
				this.Dispose();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			this.Hide();
			base.Dispose(disposing);

			// Cleanup DPlay
			if (peerObject != null)
				peerObject.Dispose();

            peerObject = null;
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.txtSend = new System.Windows.Forms.TextBox();
            this.btnSend = new System.Windows.Forms.Button();
            this.txtChat = new System.Windows.Forms.TextBox();
            this.btnExit = new System.Windows.Forms.Button();
            this.lblUsers = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.txtSend,
                                                                                    this.btnSend,
                                                                                    this.txtChat,
                                                                                    this.btnExit,
                                                                                    this.lblUsers,
                                                                                    this.label1});
            this.groupBox1.Location = new System.Drawing.Point(9, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(377, 235);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            // 
            // txtSend
            // 
            this.txtSend.Location = new System.Drawing.Point(14, 205);
            this.txtSend.MaxLength = 508;
            this.txtSend.Name = "txtSend";
            this.txtSend.Size = new System.Drawing.Size(280, 20);
            this.txtSend.TabIndex = 5;
            this.txtSend.Text = "";
            this.txtSend.TextChanged += new System.EventHandler(this.SendTextChanged);
            // 
            // btnSend
            // 
            this.btnSend.Enabled = false;
            this.btnSend.Location = new System.Drawing.Point(297, 204);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(72, 22);
            this.btnSend.TabIndex = 4;
            this.btnSend.Text = "&Send";
            this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
            // 
            // txtChat
            // 
            this.txtChat.Location = new System.Drawing.Point(15, 45);
            this.txtChat.Multiline = true;
            this.txtChat.Name = "txtChat";
            this.txtChat.ReadOnly = true;
            this.txtChat.Size = new System.Drawing.Size(354, 152);
            this.txtChat.TabIndex = 3;
            this.txtChat.Text = "";
            // 
            // btnExit
            // 
            this.btnExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnExit.Location = new System.Drawing.Point(294, 16);
            this.btnExit.Name = "btnExit";
            this.btnExit.Size = new System.Drawing.Size(72, 22);
            this.btnExit.TabIndex = 2;
            this.btnExit.Text = "E&xit";
            this.btnExit.Click += new System.EventHandler(this.btnExit_Click);
            // 
            // lblUsers
            // 
            this.lblUsers.Location = new System.Drawing.Point(198, 18);
            this.lblUsers.Name = "lblUsers";
            this.lblUsers.Size = new System.Drawing.Size(37, 14);
            this.lblUsers.TabIndex = 1;
            this.lblUsers.Text = "0";
            this.lblUsers.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(11, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(183, 14);
            this.label1.TabIndex = 0;
            this.label1.Text = "Number of people in conversation: ";
            // 
            // ChatPeer
            // 
            this.AcceptButton = this.btnSend;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.btnExit;
            this.ClientSize = new System.Drawing.Size(394, 253);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.groupBox1});
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ChatPeer";
            this.Text = "C# Chat Peer Sample";
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
		#endregion
		#region DirectPlayEvents


        /// <summary>
        /// A player was created
        /// </summary>
		private void PlayerCreated(object sender, PlayerCreatedEventArgs e)
		{
			// Get the PlayerInformation and store it 
			PlayerInformation peerInfo = peerObject.GetPeerInformation(e.Message.PlayerID);
			Players oPlayer = new Players(e.Message.PlayerID,peerInfo.Name);
			// We lock the data here since it is shared across multiple threads.
			lock (playerList)
			{
				playerList.Add(oPlayer);
				// Update our number of players and our button
				lblUsers.Text = playerList.Count.ToString();
			}
			// Save this player id if it's ourselves
			if (peerInfo.Local)
				localPlayerId = e.Message.PlayerID;

		}


        /// <summary>
        /// A player was destroyed
        /// </summary>
		private void PlayerDestroyed(object sender, PlayerDestroyedEventArgs e)
		{
			// Remove this player from our list
			// We lock the data here since it is shared across multiple threads.
			lock (playerList)
			{
				foreach (Players player in playerList)
				{
					if (e.Message.PlayerID == player.playerId)
					{
						playerList.Remove(player);
						break;
					}
				}
				// Update our number of players and our button
				lblUsers.Text = playerList.Count.ToString();
			}
		}



        /// <summary>
        /// The host was migrated, see if you're the new host
        /// </summary>
		private void HostMigrated(object sender, HostMigratedEventArgs e)
		{
			if (localPlayerId == e.Message.NewHostID) 
			{
				// I'm the new host, update my UI
				this.Text += " (HOST)";
			}
		}



        /// <summary>
        /// We've received data, parse it
        /// </summary>
		private void DataReceived(object sender, ReceiveEventArgs e)
		{
			if ((byte)e.Message.ReceiveData.Read(typeof(byte)) == ChatMessageId) // We've received text chat
			{
				// We won't be using the helper functions here since we want to 
				// interop with the c++ version of the app.  It packages it's messages
				// up with the first byte being the msg id (ChatMessageId), and 
				// the next xxx bytes as an ANSI string.

				// Get the default ASCII decoder
				System.Text.Decoder dec = System.Text.Encoding.ASCII.GetDecoder();
				int length = (int)e.Message.ReceiveData.Length - 1;
				// Create a char array of the right length
				byte[] data = (byte[])e.Message.ReceiveData.Read(typeof(byte), length);
				char[] c = new char[dec.GetCharCount(data, 0, length)];
				// Get the actual decoded characters
				dec.GetChars(data, 0, length, c, 0);
				// Now we can use the string builder to actually build our string
				System.Text.StringBuilder sb = new System.Text.StringBuilder(c.Length);
				sb.Insert(0, c, 0, dec.GetCharCount(data, 0, length));

				string sChatText = sb.ToString(); // The actual chat text
				// Now build the string we will be displaying to the user:
				string sChatString = "<" + GetPlayerName(e.Message.SenderID) + "> " + sChatText;
				// Now update our text
				lock(txtChat)
				{
					if (txtChat.Text.Length > (txtChat.MaxLength * 0.95))
						txtChat.Text = txtChat.Text.Remove(0, (int)(txtChat.MaxLength / 2));

					txtChat.AppendText(sChatString);
					txtChat.AppendText("\r\n");
					txtChat.SelectionStart = txtChat.Text.Length;
					txtChat.ScrollToCaret();
				}
			}
			e.Message.ReceiveData.Dispose(); // We no longer need the data, Dispose the buffer
		}



        /// <summary>
        /// The session was terminated
        /// </summary>
		private void SessionTerminated(object sender, SessionTerminatedEventArgs e)
		{
			// Well, this session is being terminated, let the user know
			if (e.Message.ResultCode == ResultCode.HostTerminatedSession)
				MessageBox.Show("The Host has terminated this session.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information);
			else
				MessageBox.Show("The session has been lost.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information);

			// This will post a message on the main thread to shut down our form
			this.BeginInvoke(new PeerCloseCallback(this.PeerClose));
		}
		#endregion



        /// <summary>
        /// Exit the application
        /// </summary>
		private void btnExit_Click(object sender, System.EventArgs e)
		{
			// Exit the application
			this.Dispose();
		}

		/// <summary>
		/// This will return a players name based on the ID of that player
		/// </summary>
		private string GetPlayerName(int idPlayer)
		{
			lock (playerList)
			{
				foreach (Players p in playerList)
				{
					if (p.playerId == idPlayer)
						return p.Name;
				}
			}
			return null;
		}


        /// <summary>
        /// Fired when the text to send has been changed
        /// </summary>
		private void SendTextChanged(object sender, System.EventArgs e)
		{
			btnSend.Enabled = (((TextBox)sender).Text.Length > 0);
		}



        /// <summary>
        /// We want to send our chat message
        /// </summary>
		private void btnSend_Click(object sender, System.EventArgs e)
		{
			// Ok, we need to package up the text into a format 
			// that the c++ version of the sample expects since 
			// we are interop'ing with it.
			int strlen = System.Text.Encoding.ASCII.GetByteCount(txtSend.Text);
			NetworkPacket data = new NetworkPacket();
			byte[] stringdata = new byte[strlen]; // Create our buffer
			data.Write(ChatMessageId); // Set the msg type
			//Now fill up the rest of the byte array with the ASCII chars of the string
			System.Text.Encoding.ASCII.GetBytes(txtSend.Text, 0, strlen, stringdata, 0);
			data.Write(stringdata);
			// Now we've got the data setup, send it off.
			peerObject.SendTo((int)PlayerID.AllPlayers, data, 0, SendFlags.Guaranteed);
			// Now that we've sent out the text, clear it
			txtSend.Text = null;
		}



        /// <summary>
        /// Shut down the sample
        /// </summary>
		public void PeerClose()
		{
			// The session was terminated, go ahead and shut down
			this.Dispose();
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main() 
		{
			using (ChatPeer wfData = new ChatPeer())
            {
                try
                {
                    Application.Run(wfData);
                }
                catch{}
			}
		}
	}
}
