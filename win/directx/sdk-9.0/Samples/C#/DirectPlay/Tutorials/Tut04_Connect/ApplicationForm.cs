//----------------------------------------------------------------------------
// File: ApplicationForm.cs
//
// Desc: The main WinForm for the application class.
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

namespace Tut04_Connect
{
	/// <summary>
	/// Application's main WinForm
	/// </summary>
	public class ApplicationForm : System.Windows.Forms.Form
	{
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button ExitButton;
        private System.Windows.Forms.GroupBox groupBox1;
        public  System.Windows.Forms.Button HostButton;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
        public  System.Windows.Forms.Label SessionStatusLabel;
        private System.Windows.Forms.GroupBox groupBox2;
        public  System.Windows.Forms.ListBox DetectedSessionsListBox;
        public  System.Windows.Forms.Button SearchButton;
        public  System.Windows.Forms.TextBox SearchAddressTextBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        public  System.Windows.Forms.Button ConnectButton;
        public System.Windows.Forms.TextBox RemotePortTextBox;
        private System.Windows.Forms.Label remotePortLabel;

        private ConnectApp App;
		
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

        public ApplicationForm(ConnectApp app)
		{
            this.App = app;

			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
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

        /// <summary>
        /// Print information about the provided exception
        /// </summary>
        /// <param name="ex">Exception instance</param>
        /// <param name="calling">Name of the method which returned the exception</param>
        /// <param name="isFatal">Flag to indicate whether the given error is fatal</param>
        public void ShowException(Exception ex, string calling, bool isFatal)
        {
            string output = null;

            if (calling != null)
                output = "An error occurred while calling \"" + calling + "\"\n\n";
            else
                output = "An error occurred while executing the tutorial\n\n";

            output += "Message: " + ex.Message + "\n";
            
            if (ex is DirectXException)
            {
                // DirectX-specific info
                DirectXException dex = (DirectXException) ex;
                output += "HRESULT: " + dex.ErrorString + " (" + dex.ErrorCode.ToString("X") + ")\n";
            }
           
            output += "Source: " + ex.Source + "\n";  

            if (isFatal)
                output += "\nThe application will now exit\n";

            MessageBox.Show(this, output, "DirectPlay Tutorial", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ApplicationForm));
            this.ExitButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SessionStatusLabel = new System.Windows.Forms.Label();
            this.HostButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.ConnectButton = new System.Windows.Forms.Button();
            this.DetectedSessionsListBox = new System.Windows.Forms.ListBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.SearchAddressTextBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.RemotePortTextBox = new System.Windows.Forms.TextBox();
            this.remotePortLabel = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // ExitButton
            // 
            this.ExitButton.Location = new System.Drawing.Point(264, 376);
            this.ExitButton.Name = "ExitButton";
            this.ExitButton.Size = new System.Drawing.Size(64, 24);
            this.ExitButton.TabIndex = 1;
            this.ExitButton.Text = "E&xit";
            this.ExitButton.Click += new System.EventHandler(this.ExitButton_Click);
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.SystemColors.Info;
            this.label1.Location = new System.Drawing.Point(19, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(304, 16);
            this.label1.TabIndex = 2;
            this.label1.Text = "This tutorial adds the ability to connect to a session. Once";
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.SystemColors.Info;
            this.label2.Location = new System.Drawing.Point(19, 32);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(296, 16);
            this.label2.TabIndex = 3;
            this.label2.Text = "connected, an application can send and receive data.";
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.SystemColors.Info;
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(8, 8);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(320, 48);
            this.pictureBox1.TabIndex = 6;
            this.pictureBox1.TabStop = false;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.SessionStatusLabel,
                                                                                    this.HostButton});
            this.groupBox1.Location = new System.Drawing.Point(8, 264);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(320, 96);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Session Status";
            // 
            // SessionStatusLabel
            // 
            this.SessionStatusLabel.Location = new System.Drawing.Point(24, 24);
            this.SessionStatusLabel.Name = "SessionStatusLabel";
            this.SessionStatusLabel.Size = new System.Drawing.Size(288, 16);
            this.SessionStatusLabel.TabIndex = 1;
            this.SessionStatusLabel.Text = "Not connected to a session.";
            // 
            // HostButton
            // 
            this.HostButton.Location = new System.Drawing.Point(24, 56);
            this.HostButton.Name = "HostButton";
            this.HostButton.Size = new System.Drawing.Size(80, 24);
            this.HostButton.TabIndex = 0;
            this.HostButton.Text = "&Host...";
            this.HostButton.Click += new System.EventHandler(this.HostButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.ConnectButton,
                                                                                    this.DetectedSessionsListBox,
                                                                                    this.SearchButton,
                                                                                    this.SearchAddressTextBox,
                                                                                    this.label4,
                                                                                    this.label3,
                                                                                    this.remotePortLabel,
                                                                                    this.RemotePortTextBox});
            this.groupBox2.Location = new System.Drawing.Point(8, 72);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(320, 184);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Host Search";
            // 
            // ConnectButton
            // 
            this.ConnectButton.Enabled = false;
            this.ConnectButton.Location = new System.Drawing.Point(220, 152);
            this.ConnectButton.Name = "ConnectButton";
            this.ConnectButton.Size = new System.Drawing.Size(72, 24);
            this.ConnectButton.TabIndex = 15;
            this.ConnectButton.Text = "C&onnect";
            this.ConnectButton.Click += new System.EventHandler(this.ConnectButton_Click);
            // 
            // DetectedSessionsListBox
            // 
            this.DetectedSessionsListBox.Location = new System.Drawing.Point(28, 88);
            this.DetectedSessionsListBox.Name = "DetectedSessionsListBox";
            this.DetectedSessionsListBox.Size = new System.Drawing.Size(264, 56);
            this.DetectedSessionsListBox.TabIndex = 14;
            this.DetectedSessionsListBox.SelectedIndexChanged += new System.EventHandler(this.DetectedSessionsListBox_SelectedIndexChanged);
            // 
            // SearchButton
            // 
            this.SearchButton.Location = new System.Drawing.Point(220, 40);
            this.SearchButton.Name = "SearchButton";
            this.SearchButton.Size = new System.Drawing.Size(72, 24);
            this.SearchButton.TabIndex = 13;
            this.SearchButton.Text = "&Search";
            this.SearchButton.Click += new System.EventHandler(this.SearchButton_Click);
            // 
            // SearchAddressTextBox
            // 
            this.SearchAddressTextBox.AutoSize = false;
            this.SearchAddressTextBox.Location = new System.Drawing.Point(28, 40);
            this.SearchAddressTextBox.Name = "SearchAddressTextBox";
            this.SearchAddressTextBox.Size = new System.Drawing.Size(132, 24);
            this.SearchAddressTextBox.TabIndex = 12;
            this.SearchAddressTextBox.Text = "localhost";
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(28, 72);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(120, 16);
            this.label4.TabIndex = 11;
            this.label4.Text = "Detected Sessions:";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(28, 24);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(100, 16);
            this.label3.TabIndex = 10;
            this.label3.Text = "Search Address:";
            // 
            // RemotePortTextBox
            // 
            this.RemotePortTextBox.AutoSize = false;
            this.RemotePortTextBox.Location = new System.Drawing.Point(168, 40);
            this.RemotePortTextBox.Name = "RemotePortTextBox";
            this.RemotePortTextBox.Size = new System.Drawing.Size(40, 24);
            this.RemotePortTextBox.TabIndex = 18;
            this.RemotePortTextBox.Text = "";
            // 
            // remotePortLabel
            // 
            this.remotePortLabel.Location = new System.Drawing.Point(168, 24);
            this.remotePortLabel.Name = "remotePortLabel";
            this.remotePortLabel.Size = new System.Drawing.Size(32, 16);
            this.remotePortLabel.TabIndex = 17;
            this.remotePortLabel.Text = "Port:";
            // 
            // ApplicationForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(336, 408);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.groupBox2,
                                                                          this.groupBox1,
                                                                          this.label2,
                                                                          this.label1,
                                                                          this.ExitButton,
                                                                          this.pictureBox1});
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ApplicationForm";
            this.Text = "Tutorial 4: Connect";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }
		#endregion

        /// <summary>
        /// Handler for Exit button click
        /// </summary>
        private void ExitButton_Click(object sender, System.EventArgs e)
        {
            Dispose();
        }

        /// <summary>
        /// Handler for Host button click. If the application is currently hosting,
        /// this button will display "Disconnect"
        /// </summary>
        private void HostButton_Click(object sender, System.EventArgs e)
        {
            HostButton.Enabled = false;
            this.Cursor = Cursors.WaitCursor;

            if (App.Connection == ConnectionType.Disconnected)
                App.HostSession();
            else
                App.Disconnect();

            HostButton.Enabled = true;
            this.Cursor = Cursors.Default;
            App.UpdateUI();
        }

        /// <summary>
        /// Handler for Connect button click
        /// </summary>
        private void ConnectButton_Click(object sender, System.EventArgs e)
        {
            ConnectButton.Enabled = false;
            this.Cursor = Cursors.WaitCursor;

            App.ConnectToSession();

            this.Cursor = Cursors.Default;
            App.UpdateUI();
        }

        /// <summary>
        /// Handler for Search button click
        /// </summary>
        private void SearchButton_Click(object sender, System.EventArgs e)
        {
            SearchButton.Enabled = false;
            this.Cursor = Cursors.WaitCursor;
            
            // Clear the current host list
            App.FoundSessions.Clear();

            // Have the application perform the enumeration
            App.EnumerateSessions(SearchAddressTextBox.Text, RemotePort);

            this.Cursor = Cursors.Default;
            SearchButton.Enabled = true;
            App.UpdateUI();

            DetectedSessionsListBox.Items.Clear();
            
            // Add detected items to thelist
            foreach (HostInfo host in App.FoundSessions)
            {
                DetectedSessionsListBox.Items.Add(host);
                DetectedSessionsListBox.Enabled = true;
            }

            // Give default message for no hosts
            if (App.FoundSessions.Count == 0)
                DetectedSessionsListBox.Items.Add("No hosts found.");
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
