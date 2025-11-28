//----------------------------------------------------------------------------
// File: wfDataRelay.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Threading;
using System.Timers;
using System.Runtime.InteropServices;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;

namespace DataRelaySample
{
	public class DataRelay : System.Windows.Forms.Form
	{
		private delegate void PeerCloseCallback(ResultCode code); // This delegate will be called when the session terminated event is fired.
		private delegate void UpdateLogCallback(string text);
        private delegate void DoEventsCallback();
        private const int GamePacketId = 1;
        private const int DefaultPort = 2503;


        /// <summary>
        /// What type of data is this
        /// </summary>
		private enum DataReceiveType
		{
			Received, // We received this packet
			Sent, // This packet was sent successfully
			TimeOut // This packet timed out
		}
        /// <summary>
        /// This structure will be used by the updating thread for it's 
        /// information about arriving packets (either sent by us, or 
        /// received from another player)
        /// </summary>
		private struct DataReceive
		{
			public DataReceiveType Type; //What type of packet is this?
			public int PacketId; // The packetID
			public int DataSize; // The size of the data in the packet
			public int playerId; // The player ID of the person responsible for the packet
			public NetworkPacket Data; // The actual data in the packet
			public GCHandle Handle; // The GCHandle for the datapacket (Sent packets only)
		}


        /// <summary>
        /// The data object we send to clients during our send phase
        /// </summary>
		private struct DataObject
		{
			public int PacketType;
			public int PacketId; // The packetID of this packet
		}
		
        /// <summary>
        /// A local object containing the information about players we care about
        /// </summary>
		private struct Players
		{
			public int playerId; // The ID of this player
			public string Name; // The Name of this player
			public override string ToString() // Override the tostring so it will display the name of the player
			{ return Name; }
		}
		// local variables for this app
		public Peer peerObject = null; // The main DirectPlay object
		public ConnectWizard connectWizard = null; // The wizard to create or join a DPlay Session
		// {BA214178-AAE6-4ea6-84E0-65CE36F84479} Our local apps guid
		// We will use the same GUID that the C++ and VB.NET version of this sample uses
		// so we can interop with it.
		private Guid applicationGuid = new Guid(0xba214178, 0xaae6, 0x4ea6, 0x84, 0xe0, 0x65, 0xce, 0x36, 0xf8, 0x44, 0x79);
		private Thread updateDataThread = null; // The thread that will update our UI based on avaiable packets
		private Thread sendDataThread = null; // The thread we will use to send our data
		private AutoResetEvent intervalChangedEvent = null; // Notify the main thread that the interval has changed.
		private ManualResetEvent shutdownEvent = null; // The event that will be fired when we're ready to exit
		private System.Timers.Timer statsTimer = null; // A timer used to update transfer stats
		private AutoResetEvent sendEvent = null; // The event that will notify our update thread more data is ready.
		private int lastTime = 0; // The last time we updated our transfer stats
		private int dataReceivedAmount = 0; // Amount of data received since our last transfer stats update
		private int dataSentAmount = 0; // Amount of data sent since our last transfer stats update
		private int currentPacketNumber = 0; // What packet # are we on?
		private bool sendingData = false; // Are we sending any data now?

		private ArrayList availableData = new ArrayList(); // Packets we are ready to send to the update thread
		private int dataSize = 0;
		private int target = 0;
		private int timeout = 0;
		private int sendRate = 1000;

		#region WindowsForms Variables
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label lblPlayer;
		private System.Windows.Forms.Label lblPlayers;
		private System.Windows.Forms.Button btnSend;
		private System.Windows.Forms.Button btnExit;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.ComboBox cboTarget;
		private System.Windows.Forms.ComboBox cboSize;
		private System.Windows.Forms.ComboBox cboRate;
		private System.Windows.Forms.CheckBox coalesceCheckBox;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.Label lblReceiveRate;
		private System.Windows.Forms.Label lblSendRate;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.GroupBox groupBox4;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.ComboBox cboInfotarget;
		private System.Windows.Forms.TextBox txtInfo;
		private System.Windows.Forms.GroupBox groupBox5;
		private System.Windows.Forms.TextBox txtLog;
		private System.Windows.Forms.ComboBox cboTimeout;
		#endregion


        /// <summary>
        /// Constructor
        /// </summary>
        public DataRelay()
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
		}



        /// <summary>
        /// Shut down the sample
        /// </summary>
		public void PeerClose(ResultCode code)
		{
            // Well, this session is being terminated, let the user know
            if (code == ResultCode.HostTerminatedSession)
                MessageBox.Show("The Host has terminated this session.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information);
            else
                MessageBox.Show("The session has been lost.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information);

            // The session was terminated, go ahead and shut down
			this.Close();
		}



		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			// Notify the threads we're shutting down
			if (shutdownEvent != null) 
				shutdownEvent.Set();

			// Turn off the timers
			if (statsTimer != null)
			{
				statsTimer.Stop();
				statsTimer.Dispose();
				statsTimer = null;
			}
			// We lock the data here since it is shared across multiple threads.
			lock(availableData)
			{ // Clear out any available data
				availableData.Clear();
			}

			// The send thread should have seen the shutdown event and exited by now
			if (sendDataThread != null)
			{
				sendDataThread.Join(0); // Wait for the thread to end

				// If the Join command didn't actually wait for the thread, it could be blocked.  Just abort it.
				if (sendDataThread.IsAlive)
					sendDataThread.Abort();

				sendDataThread.Abort();
			}

			// The update thread should have already seen the shutdown notice and exited
			if (updateDataThread != null) // Wait for this thread to end
			{
				updateDataThread.Join(0); // It should be done by now, but it could be blocked trying to set text on this thread

				// If our Join command didn't actually wait for the thread, it must be blocked trying to set text on the UI thread (txtLog).
				// In that case, we should just abort the thread
				if (updateDataThread.IsAlive) 
					updateDataThread.Abort();

				updateDataThread = null;
			}
			// Shut down DPlay
            if (peerObject != null)
			    peerObject.Dispose();

			peerObject=null;
            base.Dispose(disposing);
        }



        /// <summary>
        /// Set the default values and hook up our events.  This will
        /// also start the wizard
        /// </summary>
		public void Initialize()
		{
			// Populate the combos with default data
			PopulateCombos();

			peerObject = new Peer();
			// First set up our event handlers (We only need events for the ones we care about)
			peerObject.PlayerCreated += new PlayerCreatedEventHandler(this.PlayerCreated);
			peerObject.PlayerDestroyed += new PlayerDestroyedEventHandler(this.PlayerDestroyed);
			peerObject.HostMigrated += new HostMigratedEventHandler(this.HostMigrated);
			peerObject.Receive += new ReceiveEventHandler(this.DataReceived);
			peerObject.SendComplete += new SendCompleteEventHandler(this.SendComplete);
			peerObject.SessionTerminated += new SessionTerminatedEventHandler(this.SessionTerminated);
			// We do this before the wizard so we too receive the events
			// Now we will create an additional thread.  
			updateDataThread = new Thread(new ThreadStart(this.DataAvailableThread));
			shutdownEvent = new ManualResetEvent(false);
			sendEvent = new AutoResetEvent(false);
			// Start up our other threads
			updateDataThread.Start();
            connectWizard = new ConnectWizard(peerObject,applicationGuid, "DataRelay");
            connectWizard.DefaultPort = DefaultPort;
			dataSize = 512;
			timeout = 20;
			if (connectWizard.StartWizard())
			{
				// Great we've connected (or joined)..  Now we can start the sample
				// Create a timer to update our stats
				statsTimer = new System.Timers.Timer(1000); // 1000 ms interval
				statsTimer.AutoReset = true;
				statsTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.StatsTimer);
				statsTimer.SynchronizingObject = this; // make sure the timer is sync'd with the form
				// Start that timer
				statsTimer.Start();

				// Create an event for when the rate has been change
				intervalChangedEvent = new AutoResetEvent(false);

				// We should now create a thread to actually *send* the data
				sendDataThread = new Thread(new ThreadStart(this.DoSendData));
				sendDataThread.Start(); // Start our worker thread
				// We will also create a timer that's job is to actually send the data out
				sendRate = int.Parse(cboRate.Text);
				// Are we the host?
				if (connectWizard.IsHost)
					this.Text += " (HOST)";
				// Set our name
				lblPlayer.Text = connectWizard.Username;
			}
			else
				// We obviously didn't want to start a session
				this.Close();
		}
        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.lblSendRate = new System.Windows.Forms.Label();
            this.cboTarget = new System.Windows.Forms.ComboBox();
            this.lblPlayer = new System.Windows.Forms.Label();
            this.cboTimeout = new System.Windows.Forms.ComboBox();
            this.cboInfotarget = new System.Windows.Forms.ComboBox();
            this.btnSend = new System.Windows.Forms.Button();
            this.txtInfo = new System.Windows.Forms.TextBox();
            this.lblPlayers = new System.Windows.Forms.Label();
            this.lblReceiveRate = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.cboSize = new System.Windows.Forms.ComboBox();
            this.btnExit = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.coalesceCheckBox = new System.Windows.Forms.CheckBox();
            this.cboRate = new System.Windows.Forms.ComboBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblSendRate
            // 
            this.lblSendRate.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lblSendRate.Location = new System.Drawing.Point(96, 17);
            this.lblSendRate.Name = "lblSendRate";
            this.lblSendRate.Size = new System.Drawing.Size(120, 16);
            this.lblSendRate.TabIndex = 1;
            this.lblSendRate.Text = "0.0";
            this.lblSendRate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // cboTarget
            // 
            this.cboTarget.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboTarget.DropDownWidth = 131;
            this.cboTarget.Location = new System.Drawing.Point(89, 16);
            this.cboTarget.Name = "cboTarget";
            this.cboTarget.Size = new System.Drawing.Size(131, 21);
            this.cboTarget.TabIndex = 1;
            this.cboTarget.SelectedIndexChanged += new System.EventHandler(this.cboTarget_SelectedIndexChanged);
            // 
            // lblPlayer
            // 
            this.lblPlayer.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lblPlayer.Location = new System.Drawing.Point(164, 16);
            this.lblPlayer.Name = "lblPlayer";
            this.lblPlayer.Size = new System.Drawing.Size(118, 16);
            this.lblPlayer.TabIndex = 1;
            this.lblPlayer.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // cboTimeout
            // 
            this.cboTimeout.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboTimeout.DropDownWidth = 111;
            this.cboTimeout.Location = new System.Drawing.Point(89, 96);
            this.cboTimeout.Name = "cboTimeout";
            this.cboTimeout.Size = new System.Drawing.Size(131, 21);
            this.cboTimeout.TabIndex = 1;
            this.cboTimeout.SelectedIndexChanged += new System.EventHandler(this.cboTimeout_SelectedIndexChanged);
            // 
            // cboInfotarget
            // 
            this.cboInfotarget.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboInfotarget.DropDownWidth = 131;
            this.cboInfotarget.Location = new System.Drawing.Point(89, 16);
            this.cboInfotarget.Name = "cboInfotarget";
            this.cboInfotarget.Size = new System.Drawing.Size(168, 21);
            this.cboInfotarget.TabIndex = 1;
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(345, 25);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(71, 20);
            this.btnSend.TabIndex = 2;
            this.btnSend.Text = "Send";
            this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
            // 
            // txtInfo
            // 
            this.txtInfo.Location = new System.Drawing.Point(6, 41);
            this.txtInfo.Multiline = true;
            this.txtInfo.Name = "txtInfo";
            this.txtInfo.ReadOnly = true;
            this.txtInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtInfo.Size = new System.Drawing.Size(256, 168);
            this.txtInfo.TabIndex = 2;
            this.txtInfo.Text = "";
            // 
            // lblPlayers
            // 
            this.lblPlayers.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lblPlayers.Location = new System.Drawing.Point(164, 38);
            this.lblPlayers.Name = "lblPlayers";
            this.lblPlayers.Size = new System.Drawing.Size(52, 16);
            this.lblPlayers.TabIndex = 1;
            this.lblPlayers.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblReceiveRate
            // 
            this.lblReceiveRate.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lblReceiveRate.Location = new System.Drawing.Point(96, 36);
            this.lblReceiveRate.Name = "lblReceiveRate";
            this.lblReceiveRate.Size = new System.Drawing.Size(120, 16);
            this.lblReceiveRate.TabIndex = 1;
            this.lblReceiveRate.Text = "0.0";
            this.lblReceiveRate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(11, 34);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(85, 15);
            this.label8.TabIndex = 0;
            this.label8.Text = "Receive Rate:";
            this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(11, 21);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(75, 15);
            this.label9.TabIndex = 0;
            this.label9.Text = "Info Target:";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(11, 45);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(75, 15);
            this.label4.TabIndex = 0;
            this.label4.Text = "Size (bytes):";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(11, 71);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(75, 15);
            this.label5.TabIndex = 0;
            this.label5.Text = "Rate (ms):";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(11, 98);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(75, 15);
            this.label6.TabIndex = 0;
            this.label6.Text = "Timeout (ms):";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(11, 16);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(85, 15);
            this.label7.TabIndex = 0;
            this.label7.Text = "Send Rate:";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(11, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(104, 15);
            this.label1.TabIndex = 0;
            this.label1.Text = "Local Player Name: ";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(11, 39);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(154, 15);
            this.label2.TabIndex = 0;
            this.label2.Text = "Number of players in session: ";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(11, 21);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(75, 15);
            this.label3.TabIndex = 0;
            this.label3.Text = "Target:";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // cboSize
            // 
            this.cboSize.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboSize.DropDownWidth = 111;
            this.cboSize.Location = new System.Drawing.Point(89, 43);
            this.cboSize.Name = "cboSize";
            this.cboSize.Size = new System.Drawing.Size(131, 21);
            this.cboSize.TabIndex = 1;
            this.cboSize.SelectedIndexChanged += new System.EventHandler(this.cboSize_SelectedIndexChanged);
            // 
            // btnExit
            // 
            this.btnExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnExit.Location = new System.Drawing.Point(421, 25);
            this.btnExit.Name = "btnExit";
            this.btnExit.Size = new System.Drawing.Size(71, 20);
            this.btnExit.TabIndex = 2;
            this.btnExit.Text = "Exit";
            this.btnExit.Click += new System.EventHandler(this.btnExit_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.btnExit,
                                                                                    this.btnSend,
                                                                                    this.lblPlayers,
                                                                                    this.lblPlayer,
                                                                                    this.label2,
                                                                                    this.label1});
            this.groupBox1.Location = new System.Drawing.Point(6, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(497, 62);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Game Status";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.coalesceCheckBox,
                                                                                    this.cboTimeout,
                                                                                    this.cboRate,
                                                                                    this.cboSize,
                                                                                    this.cboTarget,
                                                                                    this.label6,
                                                                                    this.label5,
                                                                                    this.label4,
                                                                                    this.label3});
            this.groupBox2.Location = new System.Drawing.Point(6, 70);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(227, 153);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Send";
            // 
            // coalesceCheckBox
            // 
            this.coalesceCheckBox.Location = new System.Drawing.Point(18, 124);
            this.coalesceCheckBox.Name = "coalesceCheckBox";
            this.coalesceCheckBox.Size = new System.Drawing.Size(177, 24);
            this.coalesceCheckBox.TabIndex = 2;
            this.coalesceCheckBox.Text = "Coalesce packets";
            // 
            // cboRate
            // 
            this.cboRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboRate.DropDownWidth = 111;
            this.cboRate.Location = new System.Drawing.Point(89, 69);
            this.cboRate.Name = "cboRate";
            this.cboRate.Size = new System.Drawing.Size(131, 21);
            this.cboRate.TabIndex = 1;
            this.cboRate.SelectedIndexChanged += new System.EventHandler(this.cboRate_SelectedIndexChanged);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.lblReceiveRate,
                                                                                    this.lblSendRate,
                                                                                    this.label8,
                                                                                    this.label7});
            this.groupBox3.Location = new System.Drawing.Point(6, 228);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(227, 58);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Statistics";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.txtInfo,
                                                                                    this.label9,
                                                                                    this.cboInfotarget});
            this.groupBox4.Location = new System.Drawing.Point(237, 70);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(266, 216);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Connection Information";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.txtLog});
            this.groupBox5.Location = new System.Drawing.Point(6, 291);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(498, 130);
            this.groupBox5.TabIndex = 4;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Log";
            // 
            // txtLog
            // 
            this.txtLog.Location = new System.Drawing.Point(9, 14);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ReadOnly = true;
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtLog.Size = new System.Drawing.Size(479, 107);
            this.txtLog.TabIndex = 2;
            this.txtLog.Text = "";
            // 
            // wfDataRelay
            // 
            this.AcceptButton = this.btnSend;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.btnExit;
            this.ClientSize = new System.Drawing.Size(509, 426);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.groupBox5,
                                                                          this.groupBox4,
                                                                          this.groupBox2,
                                                                          this.groupBox1,
                                                                          this.groupBox3});
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "wfDataRelay";
            this.Text = "Data Relay";
            this.Load += new System.EventHandler(this.wfDataRelay_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion
        #region DirectPlayEvents

        /// <summary>
        /// A player was created
        /// </summary>
		private void PlayerCreated(object sender, PlayerCreatedEventArgs dpMessage)
		{
			// We lock the data here since it is shared across multiple threads.
			lock (cboTarget)
			{
				// Get the PlayerInformation and store it 
				PlayerInformation peerInfo = peerObject.GetPeerInformation(dpMessage.Message.PlayerID);
				if (!peerInfo.Local) 
				{
					// This isn't me, add this player to our combos
					Players plNew = new Players();
					plNew.playerId = dpMessage.Message.PlayerID;
					plNew.Name = peerInfo.Name;
					cboTarget.Items.Add(plNew);
					cboInfotarget.Items.Add(plNew);
				}
				// Update our number of players and our button
				lblPlayers.Text = cboTarget.Items.Count.ToString();
				btnSend.Enabled = (cboTarget.Items.Count > 1);
			}
		}


        /// <summary>
        /// A player was destroyed
        /// </summary>
		private void PlayerDestroyed(object sender, PlayerDestroyedEventArgs dpMessage)
		{
			// We lock the data here since it is shared across multiple threads.
			lock (cboTarget)
			{
				// Go through our list of players and remove anyone that is in our combos
				foreach (Players item in cboTarget.Items)
				{
					if (item.playerId == dpMessage.Message.PlayerID)
					{
						cboTarget.Items.Remove(item);
						break;
					}
				}
				foreach (Players item in cboInfotarget.Items)
				{
					if (item.playerId == dpMessage.Message.PlayerID)
					{
						cboInfotarget.Items.Remove(item);
						break;
					}
				}
				// Update our number of players and our button
				lblPlayers.Text = cboTarget.Items.Count.ToString();
				btnSend.Enabled = (cboTarget.Items.Count > 1);
				if (cboTarget.SelectedIndex<0)
					cboTarget.SelectedIndex = 0;

				if (cboInfotarget.SelectedIndex<0)
					cboInfotarget.SelectedIndex = 0;

				if (cboTarget.Items.Count <= 1)
				{
					if (sendingData)
						btnSend_Click(null, null);
				}
			}
		}


        /// <summary>
        /// The host was migrated, see if you're the new host
        /// </summary>
		private void HostMigrated(object sender, HostMigratedEventArgs dpMessage)
		{
			PlayerInformation peerInfo = peerObject.GetPeerInformation(dpMessage.Message.NewHostID);
			if (peerInfo.Local) 
			{
				// I'm the new host, update my UI
				this.Text += " (HOST)";
			}
		}


        /// <summary>
        /// We received data from DirectPlay, process it
        /// </summary>
		private void DataReceived(object sender, ReceiveEventArgs dpMessage)
		{
			DataReceive drReceive = new DataReceive();
			DataObject obj = (DataObject)dpMessage.Message.ReceiveData.Read(typeof(DataObject));
			dpMessage.Message.ReceiveData.Position = 0; // Reset stream

			// Update the amount of data we've received so our stats will update
			dataReceivedAmount += (int)dpMessage.Message.ReceiveData.Length;
			// Start filling out our packet so our update thread can use it.
			drReceive.Type = DataReceiveType.Received;
			drReceive.playerId = dpMessage.Message.SenderID;
			drReceive.DataSize = (int)dpMessage.Message.ReceiveData.Length;
			drReceive.Data = dpMessage.Message.ReceiveData;
			drReceive.PacketId = obj.PacketId;
			// Post it to the update thread
			AddAvailableData(drReceive);
		}



        /// <summary>
        /// Our send operation was complete
        /// </summary>
		private void SendComplete(object sender, SendCompleteEventArgs dpMessage)
		{
			DataReceive drReceive;
			// We lock the data here since it is shared across multiple threads.
			lock (this)
			{
				// Get our DataReceive structure from our context variable.  
				drReceive = (DataReceive)dpMessage.Message.UserContext;
				// We no longer need the GCHandle or the data since it's been received by 
				// the other members of our session.
				drReceive.Handle.Free();
				drReceive.Data = null;
				// Did this packet time out?
				if (dpMessage.Message.ResultCode == ResultCode.TimedOut)
					drReceive.Type = DataReceiveType.TimeOut;

				// Post this data so our update thread can handle it.
				AddAvailableData(drReceive);
			}
		}



        /// <summary>
        /// The session was terminated
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="dpMessage"></param>
		private void SessionTerminated(object sender, SessionTerminatedEventArgs dpMessage)
		{
            object[] param = { dpMessage.Message.ResultCode };
			// This will post a message on the main thread to shut down our form
			this.BeginInvoke(new PeerCloseCallback(this.PeerClose), param);
		}
		#endregion
		#region Form EventHandlers


        /// <summary>
        /// We are ready to exit the sample
        /// </summary>
		private void btnExit_Click(object sender, System.EventArgs e)
		{
			if (sendingData) // We better stop that!
			{
				btnSend_Click(null, null);
				//Abort our receiving thread
				updateDataThread.Abort();
			}

			// Now that's all out of the way, We're done, lets close this out
			this.Close();
		}



        /// <summary>
        /// Start (or stop) sending data
        /// </summary>
		private void btnSend_Click(object sender, System.EventArgs e)
		{
			// Update the text on the button
			if (sendingData)
			{
				//Stop sending
				btnSend.Text = "Send";
			}
			else
			{
				//Start sending
				btnSend.Text = "Stop";
			}

			sendingData = !sendingData;
		}

		private void cboRate_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			// Update the interval whenever the rate is changed
			sendRate = int.Parse(cboRate.Text);
			// Now fire our event to notify our thread we've updated the rate
			if (intervalChangedEvent != null)
				intervalChangedEvent.Set();
		}
		private void cboSize_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			dataSize = (int)cboSize.Items[cboSize.SelectedIndex];
		}

		private void cboTarget_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			target = ((Players)cboTarget.Items[cboTarget.SelectedIndex]).playerId;
		}

		private void cboTimeout_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			timeout = (int)cboTimeout.Items[cboTimeout.SelectedIndex];
		}
		private void wfDataRelay_Load(object sender, System.EventArgs e)
		{
			// Now that the form has been loaded we can initialize our application
			// We do our initialization here because it needs to happen after the call
			// to Application.Run (which starts the message pump).  Since you can begin
			// receiving messages the instant you join the session (and thus update the log)
			// We need to join the session after our form has been loaded.
			this.Initialize();
		}
		#endregion
		#region TimerEventHandlers
		private void StatsTimer(object sender, System.Timers.ElapsedEventArgs e)
		{
			// Update the current stats on screen
			int iCurrentTime = 0;
			float fSeconds = 0.0f, fDataIn = 0.0f, fDataOut = 0.0f;
			
			if (lastTime == 0)
				lastTime = (int)Environment.TickCount;

			iCurrentTime = (int)Environment.TickCount;

			if ((iCurrentTime - lastTime) < 200)
				return;

			fSeconds = (iCurrentTime - lastTime) / 1000.0f;
			fDataIn = (float)dataReceivedAmount / fSeconds;
			fDataOut = (float)dataSentAmount / fSeconds;
			lastTime = iCurrentTime;
			dataReceivedAmount = 0;
			dataSentAmount = 0;

			lblSendRate.Text = fDataOut.ToString("f1") + " BPS";
			lblReceiveRate.Text = fDataIn.ToString("f1") + " BPS";

			if (cboInfotarget.SelectedIndex > 0)
			{
				Players plSelected = (Players)cboInfotarget.Items[cboInfotarget.SelectedIndex];
				int playerId = plSelected.playerId;
				ConnectionInformation dpnInfo = peerObject.GetConnectionInformation(playerId);
				int iNumMessageHigh = 0, iNumByteHigh = 0;
				int iNumMessageLow = 0, iNumByteLow = 0;
				int iNumMessageNormal = 0, iNumByteNormal = 0;
				int iDrops = (dpnInfo.PacketsDropped + dpnInfo.PacketsRetried) * 10000;
				int fDropRate = iDrops/100;
				int iSends = dpnInfo.PacketsSentGuaranteed + dpnInfo.PacketsSentNonGuaranteed;
				peerObject.GetSendQueueInformation(playerId, out iNumMessageHigh, out iNumByteHigh, GetSendQueueInformationFlags.PriorityHigh);
				peerObject.GetSendQueueInformation(playerId, out iNumMessageLow, out iNumByteLow, GetSendQueueInformationFlags.PriorityLow);
				peerObject.GetSendQueueInformation(playerId, out iNumMessageNormal, out iNumByteNormal, GetSendQueueInformationFlags.PriorityNormal);

				string sText = null;
				sText = "Send Queue Messages High Priority=" + iNumMessageHigh.ToString() + "\r\n"; 
				sText = sText + "Send Queue Bytes High Priority=" + iNumByteHigh.ToString() + "\r\n";
				sText = sText + "Send Queue Messages Normal Priority=" + iNumMessageNormal.ToString() + "\r\n";
				sText = sText + "Send Queue Bytes Normal Priority=" + iNumByteNormal.ToString() + "\r\n";
	    
				sText = sText + "Send Queue Messages Low Priority=" + iNumMessageLow.ToString() + "\r\n";
				sText = sText + "Send Queue Bytes Low Priority=" + iNumByteLow.ToString() + "\r\n";
	    
				sText = sText + "Round Trip Latency MS=" + dpnInfo.RoundTripLatencyMs.ToString() + " ms\r\n";
				sText = sText + "Throughput BPS=" + dpnInfo.ThroughputBps.ToString() + "\r\n";
				sText = sText + "Peak Throughput BPS=" + dpnInfo.PeakThroughputBps.ToString() + "\r\n";
	                                                                            
				sText = sText + "Bytes Sent Guaranteed=" + dpnInfo.BytesSentGuaranteed.ToString() + "\r\n";
				sText = sText + "Packets Sent Guaranteed=" + dpnInfo.PacketsSentGuaranteed.ToString() + "\r\n";
				sText = sText + "Bytes Sent Non-Guaranteed=" + dpnInfo.BytesSentNonGuaranteed.ToString() + "\r\n";
				sText = sText + "Packets Sent Non-Guaranteed=" + dpnInfo.PacketsSentNonGuaranteed.ToString() + "\r\n";
	                                                                            
				sText = sText + "Bytes Retried Guaranteed=" + dpnInfo.BytesRetried.ToString() + "\r\n";
				sText = sText + "Packets Retried Guaranteed=" + dpnInfo.PacketsRetried.ToString() + "\r\n";
				sText = sText + "Bytes Dropped Non-Guaranteed=" + dpnInfo.BytesDropped.ToString() + "\r\n";
				sText = sText + "Packets Dropped Non-Guaranteed=" + dpnInfo.PacketsDropped.ToString() + "\r\n";
	                                                                            
				sText = sText + "Messages Transmitted High Priority=" + dpnInfo.MessagesTransmittedHighPriority.ToString() + "\r\n";
				sText = sText + "Messages Timed Out High Priority=" + dpnInfo.MessagesTimedOutHighPriority.ToString() + "\r\n";
				sText = sText + "Messages Transmitted Normal Priority=" + dpnInfo.MessagesTransmittedNormalPriority.ToString() + "\r\n";
				sText = sText + "Messages Timed Out Normal Priority=" + dpnInfo.MessagesTimedOutNormalPriority.ToString() + "\r\n";
				sText = sText + "Messages Transmitted Low Priority=" + dpnInfo.MessagesTransmittedLowPriority.ToString() + "\r\n";
				sText = sText + "Messages Timed Out Low Priority=" + dpnInfo.MessagesTimedOutLowPriority.ToString() + "\r\n";
	                                                                            
				sText = sText + "Bytes Received Guaranteed=" + dpnInfo.BytesReceivedGuaranteed.ToString() + "\r\n";
				sText = sText + "Packets Received Guaranteed=" + dpnInfo.PacketsReceivedGuaranteed.ToString() + "\r\n";
				sText = sText + "Bytes Received Non-Guaranteed=" + dpnInfo.BytesReceivedNonGuaranteed.ToString() + "\r\n";
				sText = sText + "Packets Received Non-Guaranteed=" + dpnInfo.PacketsReceivedNonGuaranteed.ToString() + "\r\n";
				sText = sText + "Messages Received=" + dpnInfo.MessagesReceived.ToString() + "\r\n";
	                                                                            
				sText = sText + "Loss Rate=" + fDropRate.ToString() + "\r\n";
				txtInfo.Text = sText;
			}
			else
				txtInfo.Text = null;
		}
		#endregion
		#region ThreadFunctions
		private void DataAvailableThread()
		{
			WaitHandle[] whEvent = new WaitHandle[2];
			whEvent[0] = sendEvent;
			whEvent[1] = shutdownEvent;
			int index = 0;
			while (true)
			{
				index = WaitHandle.WaitAny(whEvent);
				if (index == 1) // Our shutdown event has fired
					return;
				else  // Our event saying we've received data has fired
				{
					// Update the received data
					DataReceive drReceive = new DataReceive();
					while (GetAvailableData(ref drReceive))
					{
						string sText = null;
						if (drReceive.Type == DataReceiveType.Sent)
							sText = "Sent packet # " + drReceive.PacketId.ToString() + " to ";

						if (drReceive.Type == DataReceiveType.TimeOut)
							sText = "Packet # " + drReceive.PacketId.ToString() + " timed out on the way to ";

						if (drReceive.Type == DataReceiveType.Received)
						{
							DataObject dData = (DataObject)drReceive.Data.Read(typeof(DataObject));
							if (dData.PacketType == GamePacketId)
								sText = "Received packet # " + dData.PacketId.ToString() + " from ";

							// We're done with the buffer now
							drReceive.Data.Dispose();
						}
						if (drReceive.playerId == 0)
							sText += "all players";
						else
							sText += GetPlayerName(drReceive.playerId);

						sText += " (" + drReceive.DataSize.ToString() + " bytes)\r\n";

						// Check for a shutdown before updating our log
						if (shutdownEvent.WaitOne(0, false))
							return;

						object[] newParams = { sText };
						txtLog.BeginInvoke(new UpdateLogCallback(this.UpdateLog), newParams);
					}
				}
			}
		}

		private void DoSendData()
		{
			WaitHandle[] whEvent = new WaitHandle[2];
			whEvent[0] = intervalChangedEvent;
			whEvent[1] = shutdownEvent;
			int index = 0;
            while (true)
            {
                // Wait for an event.  We are waiting for either the shutdown event to be
                // fired letting us know to shut down, or we are waiting for an event to 
                // signal the rate has been changed.  Default the timeout to the current 
                // send rate.
                index = WaitHandle.WaitAny(whEvent, (sendRate == 0) ? 15 : sendRate, false);
                if (index == 1) // Our shutdown event has fired
                    break;
                else  // Our event saying we've received data has fired
                {
                    if (sendingData)
                    {
                        DirectXException.IgnoreExceptions();
                        // Make sure the UI is responsive
                        this.Invoke(new DoEventsCallback(this.ProcessMessages));
                        int PacketId = 0;
                        // Time to send the data
                        NetworkPacket sendData = GetDataPacket(dataSize, ref PacketId);
                        dataSentAmount += (int)dataSize;
                        DataReceive drObj;
                        // We lock the data here since it is shared across multiple threads.
                        drObj = AddSendData(PacketId, target, dataSize, sendData.GetData());
                        // We can use the NoCopy flag because our 'data' has been 'pinned' by the GCHandle.
                        SendFlags flags = SendFlags.NoLoopback | SendFlags.NoCopy;
                        // Check for packet coalescence option. If enabled, DirectPlay will
                        // attempt to send small outgoing packets as a group
                        if (coalesceCheckBox.Checked)
                            flags |= SendFlags.Coalesce;

                        peerObject.SendTo(target, drObj.Handle, dataSize, timeout, flags, drObj);
                        DirectXException.EnableExceptions();
                    }
                }
            }
		}

        #endregion



        /// <summary>
        /// Make sure the UI is still responsive on the main thread
        /// </summary>
        private void ProcessMessages()
        {
            Application.DoEvents(); 
        }

        /// <summary>
        /// Update our log
        /// </summary>
        /// <param name="text">The text we want to append to the log</param>
		private void UpdateLog(string text)
		{
			if (txtLog.Text.Length > (txtLog.MaxLength * 0.95))
				txtLog.Text = txtLog.Text.Remove(0, (int)(txtLog.MaxLength / 2));

			txtLog.Text += text;
			txtLog.SelectionStart = txtLog.Text.Length;
			txtLog.ScrollToCaret();
		}



        /// <summary>
        /// Fill the combo boxes with the default data
        /// </summary>
		private void PopulateCombos()
		{
			Players pPlayer = new Players();
			pPlayer.playerId = 0;
			pPlayer.Name = "Everyone";
			cboTarget.Items.Add(pPlayer);
			cboTarget.SelectedIndex = 0; // Everyone
			cboRate.Items.Add(1000);
			cboRate.Items.Add(500);
			cboRate.Items.Add(250);
			cboRate.Items.Add(100);
			cboRate.Items.Add(50);
			cboRate.Items.Add(0);
			cboRate.SelectedIndex = 0; // 1000 msg
			cboSize.Items.Add(512);
			cboSize.Items.Add(256);
			cboSize.Items.Add(128);
			cboSize.Items.Add(64);
			cboSize.Items.Add(32);
			cboSize.Items.Add(16);
			cboSize.SelectedIndex = 0; //512 bytes
			cboTimeout.Items.Add(5);
			cboTimeout.Items.Add(10);
			cboTimeout.Items.Add(20);
			cboTimeout.Items.Add(50);
			cboTimeout.Items.Add(100);
			cboTimeout.Items.Add(250);
			cboTimeout.Items.Add(500);
			cboTimeout.SelectedIndex = 2; // 20 ms
			pPlayer = new Players();
			pPlayer.playerId = 0;
			pPlayer.Name = "None";
			cboInfotarget.Items.Add(pPlayer);
			cboInfotarget.SelectedIndex = 0; // None

		}


        /// <summary>
        /// Get the next data packet of the correct size
        /// </summary>
		private NetworkPacket GetDataPacket(int size, ref int PacketId)
        {
            DataObject Packet = new DataObject();
            NetworkPacket ret = new NetworkPacket();
            
            // Initialize the outgoing packet
            Packet.PacketId = ++currentPacketNumber;
            Packet.PacketType = GamePacketId;
            ret.Write(Packet);
            
            // Write random outgoing data
            Random r = new Random();
            byte[] randomData = new byte[size-8]; // We use size less 8 to account for the structure size
            r.NextBytes(randomData);
            ret.Write(randomData);
            
            // Set the packetId and return the NetworkPacket
            PacketId = Packet.PacketId;
            return ret;
        }




        /// <summary>
        /// Get the players name from the id
        /// </summary>
		private string GetPlayerName(int playerId)
		{
			foreach (Players item in cboTarget.Items)
			{
				if (item.playerId == playerId)
				{
					return item.Name;
				}
			}
			return null;
		}



        /// <summary>
        /// Add data to be processed by our worker thread
        /// </summary>
        /// <param name="PacketId"></param>
        /// <param name="playerId"></param>
        /// <param name="size"></param>
        /// <param name="sendData"></param>
        /// <returns></returns>
		private DataReceive AddSendData(int PacketId, int playerId, int size, byte[] sendData)
		{
			DataReceive drObj = new DataReceive();
			drObj.PacketId = PacketId;
			drObj.playerId = playerId;
			drObj.Type = DataReceiveType.Sent;
			drObj.DataSize = size;
			// Allocate a 'pinned' handle to this buffer so the garbage collector won't try to move it.
			drObj.Handle = GCHandle.Alloc(sendData, GCHandleType.Pinned);
			return drObj;
		}



        /// <summary>
        /// Add the data to our available list
        /// </summary>
        /// <param name="Data"></param>
		private void AddAvailableData(DataReceive Data)
		{
			// We lock the data here since it is shared across multiple threads.
			lock (availableData) // Lock this buffer so no one else has access to it..
			{
				availableData.Add(Data);
			}
			// Notify our update thread that new data is available
			sendEvent.Set();
		}



        /// <summary>
        /// Get the next available data
        /// </summary>
		private bool GetAvailableData(ref DataReceive drObj)
		{
			// We lock the data here since it is shared across multiple threads.
			lock (availableData) // Lock this buffer so no one else has access to it..
			{
				if (availableData.Count>0)
				{
					drObj = (DataReceive)availableData[0];
					availableData.Remove(drObj);
					return true;
				}
				return false;
			}
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main() 
		{
			using (DataRelay wfData = new DataRelay())
            {
				Application.Run(wfData);
			}
		}

	}
}
