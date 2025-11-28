//----------------------------------------------------------------------------
// File: ConnectDialog.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX.DirectPlay;

namespace Tut10_ThreadPool
{
    /// <summary>
    /// Summary description for ConnectDialog.
    /// </summary>
    public class ConnectDialog : System.Windows.Forms.Form
    {
        private ThreadPoolApp App = null;                      // Application instance
        private HostInfo         mySelectedHost = null;           // Host selected for connection
          
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Button ConnectButton;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.TextBox SearchAddressTextBox;
        private System.Windows.Forms.ListBox DetectedSessionsListBox;
        private System.Windows.Forms.Label remotePortLabel;
        public System.Windows.Forms.TextBox RemotePortTextBox;
		
        // Properties
        public  HostInfo SelectedHost { get { return mySelectedHost; } }

        /// <summary>
        /// Property: remote port on which to connect
        /// </summary>
        public int RemotePort
        {
            get
            { 
                int retValue = 0;

                try
                {
                    retValue = int.Parse(RemotePortTextBox.Text); 
                }
                catch (Exception) {}
                
                return retValue;
            }
            set
            { RemotePortTextBox.Text = value.ToString(); }
        }

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public ConnectDialog(ThreadPoolApp app)
        {
            App = app;

            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            //
            // TODO: Add any constructor code after InitializeComponent call
            //
            SearchAddressTextBox.Text = "localhost";
            DetectedSessionsListBox.Enabled = false;
            ConnectButton.Enabled = false;
            DetectedSessionsListBox.Items.Clear();
            DetectedSessionsListBox.Items.Add("Click \"Search\" to find hosts.");
        }

        

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

		#region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.cancelButton = new System.Windows.Forms.Button();
            this.ConnectButton = new System.Windows.Forms.Button();
            this.SearchAddressTextBox = new System.Windows.Forms.TextBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.DetectedSessionsListBox = new System.Windows.Forms.ListBox();
            this.remotePortLabel = new System.Windows.Forms.Label();
            this.RemotePortTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.SystemColors.Info;
            this.label1.Location = new System.Drawing.Point(24, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(312, 16);
            this.label1.TabIndex = 0;
            this.label1.Text = "Find hosts by using the search field below. Select a detected";
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.SystemColors.Info;
            this.label2.Location = new System.Drawing.Point(24, 32);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(312, 16);
            this.label2.TabIndex = 1;
            this.label2.Text = "host and click \"Connect\" to join the session.";
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.SystemColors.Info;
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(8, 8);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(336, 48);
            this.pictureBox1.TabIndex = 2;
            this.pictureBox1.TabStop = false;
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(40, 72);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(100, 16);
            this.label3.TabIndex = 3;
            this.label3.Text = "Search Address:";
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(40, 120);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(120, 16);
            this.label4.TabIndex = 4;
            this.label4.Text = "Detected Sessions:";
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(192, 216);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(72, 24);
            this.cancelButton.TabIndex = 5;
            this.cancelButton.Text = "&Cancel";
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // ConnectButton
            // 
            this.ConnectButton.Location = new System.Drawing.Point(272, 216);
            this.ConnectButton.Name = "ConnectButton";
            this.ConnectButton.Size = new System.Drawing.Size(72, 24);
            this.ConnectButton.TabIndex = 6;
            this.ConnectButton.Text = "C&onnect";
            this.ConnectButton.Click += new System.EventHandler(this.ConnectButton_Click);
            // 
            // SearchAddressTextBox
            // 
            this.SearchAddressTextBox.AutoSize = false;
            this.SearchAddressTextBox.Location = new System.Drawing.Point(40, 88);
            this.SearchAddressTextBox.Name = "SearchAddressTextBox";
            this.SearchAddressTextBox.Size = new System.Drawing.Size(136, 24);
            this.SearchAddressTextBox.TabIndex = 7;
            this.SearchAddressTextBox.Text = "localhost";
            // 
            // SearchButton
            // 
            this.SearchButton.Location = new System.Drawing.Point(232, 88);
            this.SearchButton.Name = "SearchButton";
            this.SearchButton.Size = new System.Drawing.Size(72, 24);
            this.SearchButton.TabIndex = 8;
            this.SearchButton.Text = "&Search";
            this.SearchButton.Click += new System.EventHandler(this.SearchButton_Click);
            // 
            // DetectedSessionsListBox
            // 
            this.DetectedSessionsListBox.Location = new System.Drawing.Point(40, 136);
            this.DetectedSessionsListBox.Name = "DetectedSessionsListBox";
            this.DetectedSessionsListBox.Size = new System.Drawing.Size(264, 56);
            this.DetectedSessionsListBox.TabIndex = 9;
            this.DetectedSessionsListBox.SelectedIndexChanged += new System.EventHandler(this.DetectedSessionsListBox_SelectedIndexChanged);
            // 
            // remotePortLabel
            // 
            this.remotePortLabel.Location = new System.Drawing.Point(181, 72);
            this.remotePortLabel.Name = "remotePortLabel";
            this.remotePortLabel.Size = new System.Drawing.Size(32, 16);
            this.remotePortLabel.TabIndex = 19;
            this.remotePortLabel.Text = "Port:";
            // 
            // RemotePortTextBox
            // 
            this.RemotePortTextBox.AutoSize = false;
            this.RemotePortTextBox.Location = new System.Drawing.Point(181, 88);
            this.RemotePortTextBox.Name = "RemotePortTextBox";
            this.RemotePortTextBox.Size = new System.Drawing.Size(40, 24);
            this.RemotePortTextBox.TabIndex = 20;
            this.RemotePortTextBox.Text = "";
            // 
            // ConnectDialog
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(352, 246);
            this.ControlBox = false;
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.remotePortLabel,
                                                                          this.RemotePortTextBox,
                                                                          this.DetectedSessionsListBox,
                                                                          this.SearchButton,
                                                                          this.SearchAddressTextBox,
                                                                          this.ConnectButton,
                                                                          this.cancelButton,
                                                                          this.label4,
                                                                          this.label3,
                                                                          this.label2,
                                                                          this.label1,
                                                                          this.pictureBox1});
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConnectDialog";
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Connect to Session";
            this.ResumeLayout(false);

        }
		#endregion

        /// <summary>
        /// Handler for Search button click
        /// </summary>
        private void SearchButton_Click(object sender, System.EventArgs e)
        {
            SearchButton.Enabled = false;
            this.Cursor = Cursors.WaitCursor;
            
            // Clear the current UI list
            DetectedSessionsListBox.Enabled = false;
            DetectedSessionsListBox.Items.Clear();

            // Have the application perform the enumeration
            App.EnumerateSessions(SearchAddressTextBox.Text, RemotePort);

            // Add detected items to thelist
            foreach (HostInfo host in App.FoundSessions)
            {
                DetectedSessionsListBox.Items.Add(host);
                DetectedSessionsListBox.Enabled = true;
            }

            // Give default message for no hosts
            if (App.FoundSessions.Count == 0)
                DetectedSessionsListBox.Items.Add("No hosts found.");

            this.Cursor = Cursors.Default;
            SearchButton.Enabled = true;
        }

        /// <summary>
        /// Handler for Connect button click
        /// </summary>
        private void ConnectButton_Click(object sender, System.EventArgs e)
        {
            // Save the current settings
            mySelectedHost = (HostInfo) DetectedSessionsListBox.SelectedItem;
            DialogResult = DialogResult.OK;
        }

        /// <summary>
        /// Handler for Cancel button click
        /// </summary>
        private void cancelButton_Click(object sender, System.EventArgs e)
        {
            // Discard the settings
            DialogResult = DialogResult.Cancel;
        }

        /// <summary>
        /// Handler for Detected Sessions selection change
        /// </summary>
        private void DetectedSessionsListBox_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            ConnectButton.Enabled = (DetectedSessionsListBox.SelectedIndex != -1);
        }
    }
}
