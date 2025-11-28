//----------------------------------------------------------------------------
// File: VoiceConnect.cs
//
// Desc: The main file for VoiceConnect that shows how use DirectPlay along 
//       with DirectPlayVoice to allow talking in a conference situation.
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;
using Lobby = Microsoft.DirectX.DirectPlay.Lobby;
using Voice = Microsoft.DirectX.DirectPlay.Voice;

namespace VoiceConnect
{

    /// <summary>
    /// Holds information needed to track a player
    /// </summary>
	public class VoicePlayer
	{
        public VoicePlayer(int id, string n)
        {
            PlayerId = id; 
            Name = n; 
            Talking = false;
        }

		public int PlayerId; // Player's DirectPlay ID
		public string Name; // Display name
		public bool Talking; // Currently talking
	}

	/// <summary>
	/// This is the main VoiceConnect 
	/// </summary>
	public class VoiceConnect : System.Windows.Forms.Form
	{
        #region Windows Forms Variables
		private System.Windows.Forms.Button buttonSetup;
		private System.Windows.Forms.Button buttonExit;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ListView listViewPlayers;
		private System.Windows.Forms.ColumnHeader columnHeaderName;
		private System.Windows.Forms.ColumnHeader columnHeaderTalking;
		private System.Windows.Forms.Label labelNumPlayers;
        #endregion


		/// <summary>
		/// App specific variables
		/// </summary>
		private Peer peerObject; // DirectPlay Peer object
		private ConnectWizard playWizard; // ConnectionWizard helper
		private VoiceWizard voiceSettings; // VoiceWizard helper
		private Voice.Client client; // DirectPlay Voice Client object
		private Voice.Server server; // DirectPlay Voice Server object
		private ArrayList playerList; // List of connected players
		private int localPlayerId; // Local player's DirectPlay ID
        private bool isLocalPlayerQuitting = false;

		/// <summary>
		/// This delegate is used to invoke a shutdown of the peer outside of a message handler,
		/// or to update the player list
		/// </summary>
		private delegate void BeginInvokeDelegate();

		// This GUID allows DirectPlay to find other instances of the same game on
		// the network.  So it must be unique for every game, and the same for 
		// every instance of that game.
		// However, we are using the same guid the C++ and VB.NET version of the 
		// samples use so they can all communicate together.
		public Guid g_guidApp = new Guid(0x2b395ca8, 0x2610, 0x45ac, 0xa4, 0x4d, 0x8, 0xdc, 0xd1, 0x66, 0x9a, 0xad);
		private const int DefaultPort = 2512;

        /// <summary>
        /// Constructor
        /// </summary>
		public VoiceConnect()
		{
			InitializeComponent();
		
			try 
			{
			
				//set up state members
				playerList = new ArrayList();

				//create the peer
				peerObject = new Peer();

				//create the voice client and attach message handlers
				client = new Voice.Client(peerObject);
				client.HostMigrated+= new Voice.VoiceHostMigratedEventHandler(VoiceHostMigrated);
				client.PlayerCreated+= new Voice.VoicePlayerCreatedEventHandler(VoicePlayerCreated);
				client.PlayerDeleted+= new Voice.VoicePlayerDeletedEventHandler(VoicePlayerDeleted);
				client.SessionLost+= new Voice.SessionLostEventHandler(VoiceSessionLost);
				client.PlayerStarted+= new Voice.PlayerStartedEventHandler(PlayerStarted);
				client.PlayerStopped+= new Voice.PlayerStoppedEventHandler(PlayerStopped);
				client.RecordStarted+= new Voice.RecordStartedEventHandler(RecordStarted);
				client.RecordStopped+= new Voice.RecordStoppedEventHandler(RecordStopped);


				//session was not lobby launched -- using connection wizard
				playWizard = new ConnectWizard(peerObject, g_guidApp, "VoiceConnect"); 
                playWizard.DefaultPort = DefaultPort;

				//create the voice wizard
				voiceSettings = new VoiceWizard();

                if (playWizard.StartWizard())
                {
                    if (playWizard.IsHost)
                    {
                        this.Text += " (HOST)";

                        //create the voice server
                        server = new Voice.Server(peerObject);
						
                        //init the voice server
                        voiceSettings.InitVoiceHost(peerObject, server, client, this);
                    }
                    else
                    {
                        //connection to another host was successful
                        voiceSettings.InitVoiceClient(peerObject, client, this);
                    }
						
                }
                else
                {
                    this.Dispose();
                }
				

				
				
			}
			catch(Exception e)
			{
				Console.WriteLine(e.ToString());
				//allow exception to fall though and exit.
				this.Close();
			}
		}

        /// <summary>
        /// Session shutdown
        /// </summary>
		public void PeerClose()
		{
            if (!isLocalPlayerQuitting)
            {
                // Well, this session is being terminated, let the user know
                MessageBox.Show("The session has been lost.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            // The session was terminated, go ahead and shut down
            this.Close();
		}

        /// <summary>
        /// Refresh the user interface
        /// </summary>
		private void UpdatePlayerList()
		{
			lock(playerList)
			{
				listViewPlayers.Items.Clear();
				foreach (VoicePlayer player in playerList)
				{
						ListViewItem newItem = listViewPlayers.Items.Add(player.Name);
						newItem.SubItems.Add(player.Talking.ToString());
				}
			}
            // Update our number of players and our button
            labelNumPlayers.Text = listViewPlayers.Items.Count.ToString();
        }

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			this.Hide();
			try 
			{
				//Cleanup Voice
				if (client != null)
					client.Disconnect(Voice.VoiceFlags.Sync);

                if (server != null)
                    server.StopSession();

                // Cleanup DPlay
                if (peerObject != null)
                    peerObject.Dispose();
			}
			catch { }

			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(VoiceConnect));
            this.buttonSetup = new System.Windows.Forms.Button();
            this.buttonExit = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.labelNumPlayers = new System.Windows.Forms.Label();
            this.listViewPlayers = new System.Windows.Forms.ListView();
            this.columnHeaderName = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderTalking = new System.Windows.Forms.ColumnHeader();
            this.SuspendLayout();
            // 
            // buttonSetup
            // 
            this.buttonSetup.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
            this.buttonSetup.Location = new System.Drawing.Point(256, 8);
            this.buttonSetup.Name = "buttonSetup";
            this.buttonSetup.TabIndex = 1;
            this.buttonSetup.Text = "Setup";
            this.buttonSetup.Click += new System.EventHandler(this.buttonSetup_Click);
            // 
            // buttonExit
            // 
            this.buttonExit.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
            this.buttonExit.Location = new System.Drawing.Point(336, 8);
            this.buttonExit.Name = "buttonExit";
            this.buttonExit.TabIndex = 2;
            this.buttonExit.Text = "Exit";
            this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(16, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(176, 16);
            this.label1.TabIndex = 3;
            this.label1.Text = "Number of people in conversation";
            // 
            // labelNumPlayers
            // 
            this.labelNumPlayers.Location = new System.Drawing.Point(208, 11);
            this.labelNumPlayers.Name = "labelNumPlayers";
            this.labelNumPlayers.Size = new System.Drawing.Size(32, 16);
            this.labelNumPlayers.TabIndex = 4;
            this.labelNumPlayers.Text = "0";
            // 
            // listViewPlayers
            // 
            this.listViewPlayers.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right);
            this.listViewPlayers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                                                                                              this.columnHeaderName,
                                                                                              this.columnHeaderTalking});
            this.listViewPlayers.Location = new System.Drawing.Point(8, 40);
            this.listViewPlayers.Name = "listViewPlayers";
            this.listViewPlayers.Size = new System.Drawing.Size(408, 224);
            this.listViewPlayers.TabIndex = 5;
            this.listViewPlayers.View = System.Windows.Forms.View.Details;
            // 
            // columnHeaderName
            // 
            this.columnHeaderName.Text = "Name";
            this.columnHeaderName.Width = 336;
            // 
            // columnHeaderTalking
            // 
            this.columnHeaderTalking.Text = "Talking";
            this.columnHeaderTalking.Width = 68;
            // 
            // VoiceConnect
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(424, 270);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.listViewPlayers,
                                                                          this.labelNumPlayers,
                                                                          this.label1,
                                                                          this.buttonExit,
                                                                          this.buttonSetup});
            this.MinimumSize = new System.Drawing.Size(430, 300);
            this.Name = "VoiceConnect";
            this.Text = "Voice Connect";
            this.ResumeLayout(false);

        }
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main() 
		{
			using(VoiceConnect vc = new VoiceConnect())
            {
                try
                {
                    Application.Run(vc);
                }
                catch{}
			}
		}


        #region DirectPlay Voice message handlers
     
		private void VoiceHostMigrated(object sender, Voice.HostMigratedEventArgs dpMessage)
		{
			if (localPlayerId == dpMessage.Message.NewHostID) 
			{
				// I'm the new host, update my UI
				this.Text += " (HOST)";
				
				//add reference to new voice server
				server = dpMessage.Message.ServerObject;

				//notify the voice wizard of the change
				voiceSettings.HostMigrate(server);

			}
		}
		private void VoicePlayerCreated(object sender, Voice.VoicePlayerCreatedEventArgs e)
		{
			// Get the PlayerInformation and store it 
			PlayerInformation dpPeer = peerObject.GetPeerInformation(e.Message.PlayerID);
			VoicePlayer oPlayer = new VoicePlayer(e.Message.PlayerID, dpPeer.Name);
			// We lock the data here since it is shared across multiple threads.
			lock (playerList)
			{
				playerList.Add(oPlayer);
			}
			// Update our number of players and our button
			labelNumPlayers.Text = playerList.Count.ToString();
			// Save this player id if it's ourselves
			if (dpPeer.Local)
				localPlayerId = e.Message.PlayerID;

			this.BeginInvoke(new BeginInvokeDelegate(this.UpdatePlayerList));
		}

		private void VoicePlayerDeleted(object sender, Voice.VoicePlayerDeletedEventArgs e)
		{
			// Remove this player from our list
			// We lock the data here since it is shared across multiple threads.
			lock (playerList)
			{
				foreach (VoicePlayer player in playerList)
				{
					if (e.Message.PlayerID == player.PlayerId)
					{
						playerList.Remove(player);
						break;
					}
				}
			}
			//Update our player list
			this.BeginInvoke(new BeginInvokeDelegate(this.UpdatePlayerList));
		}
		
        private void VoiceSessionLost(object sender, Voice.SessionLostEventArgs dpMessage)
		{
			// This will post a message on the main thread to shut down our form
			this.BeginInvoke(new BeginInvokeDelegate(this.PeerClose));
		}

		private void PlayerStarted(object sender, Voice.PlayerStartedEventArgs dpMessage)
		{
			lock(playerList)
			{
				for (int i=0; i<playerList.Count; i++)
				{
					if (((VoicePlayer) playerList[i]).PlayerId == dpMessage.Message.SourcePlayerID)
					{
						VoicePlayer oPlayer = (VoicePlayer) playerList[i];
						oPlayer.Talking = true;
						playerList[i] = oPlayer;
						break;
					}
				}
			}
			this.BeginInvoke(new BeginInvokeDelegate(this.UpdatePlayerList));
		}

		private void PlayerStopped(object sender, Voice.PlayerStoppedEventArgs dpMessage)
		{
			lock(playerList)
			{
				for (int i=0; i<playerList.Count; i++)
				{
					if (((VoicePlayer) playerList[i]).PlayerId == dpMessage.Message.SourcePlayerID)
					{
						VoicePlayer oPlayer = (VoicePlayer) playerList[i];
						oPlayer.Talking = false;
						playerList[i] = oPlayer;
						break;
					}
				}
			}
			this.BeginInvoke(new BeginInvokeDelegate(this.UpdatePlayerList));
		}
		private void RecordStarted(object sender, Voice.RecordStartedEventArgs dpMessage)
		{
			lock(playerList)
			{
				for (int i=0; i<playerList.Count; i++)
				{
					if (((VoicePlayer) playerList[i]).PlayerId == localPlayerId)
					{
						VoicePlayer oPlayer = (VoicePlayer) playerList[i];
						oPlayer.Talking = true;
						playerList[i] = oPlayer;
						break;
					}
				}
			}
			this.BeginInvoke(new BeginInvokeDelegate(this.UpdatePlayerList));
		}

		private void RecordStopped(object sender, Voice.RecordStoppedEventArgs dpMessage)
		{
			lock(playerList)
			{
				for (int i=0; i<playerList.Count; i++)
				{
					if (((VoicePlayer) playerList[i]).PlayerId == localPlayerId)
					{
						VoicePlayer oPlayer = (VoicePlayer) playerList[i];
						oPlayer.Talking = false;
						playerList[i] = oPlayer;
						break;
					}
				}
			}
			this.BeginInvoke(new BeginInvokeDelegate(this.UpdatePlayerList));
		}

        #endregion // DirectPlay Voice message handlers

        #region Form message handlers

		private void buttonSetup_Click(object sender, System.EventArgs e)
		{
			//Open up the setup dialog through the voice wizard
			voiceSettings.ShowClientSetupDialog(this.Handle);
		}

		private void buttonExit_Click(object sender, System.EventArgs e)
		{
            isLocalPlayerQuitting = true;
            // This will post a message on the main thread to shut down our form
			this.BeginInvoke(new BeginInvokeDelegate(this.PeerClose));
		}

        #endregion // Form message handlers
	}
}
