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

namespace Tut08_Voice
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
        private System.Windows.Forms.Label label4;
        public  System.Windows.Forms.Button SendButton;
        public  System.Windows.Forms.TextBox SendTextBox;
        public  System.Windows.Forms.ListBox ReceivedMessagesListBox;
        public  System.Windows.Forms.Button ConnectButton;
        private System.Windows.Forms.MainMenu mainMenu1;
        private System.Windows.Forms.MenuItem LobbyMenu;
        private System.Windows.Forms.MenuItem LobbyMenuRegisterItem;
        private System.Windows.Forms.MenuItem LobbyMenuUnregisterItem;
        public  System.Windows.Forms.PictureBox PlayIcon;
        public  System.Windows.Forms.PictureBox RecordIcon;
        public  System.Windows.Forms.PictureBox ActiveRecordIcon;
        public  System.Windows.Forms.PictureBox ActivePlayIcon;

        private VoiceApp App;
		
        public ApplicationForm(VoiceApp app)
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
            this.ConnectButton = new System.Windows.Forms.Button();
            this.SessionStatusLabel = new System.Windows.Forms.Label();
            this.HostButton = new System.Windows.Forms.Button();
            this.ActiveRecordIcon = new System.Windows.Forms.PictureBox();
            this.ActivePlayIcon = new System.Windows.Forms.PictureBox();
            this.RecordIcon = new System.Windows.Forms.PictureBox();
            this.PlayIcon = new System.Windows.Forms.PictureBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.SendButton = new System.Windows.Forms.Button();
            this.SendTextBox = new System.Windows.Forms.TextBox();
            this.ReceivedMessagesListBox = new System.Windows.Forms.ListBox();
            this.label4 = new System.Windows.Forms.Label();
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.LobbyMenu = new System.Windows.Forms.MenuItem();
            this.LobbyMenuRegisterItem = new System.Windows.Forms.MenuItem();
            this.LobbyMenuUnregisterItem = new System.Windows.Forms.MenuItem();
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
            this.label1.Text = "This tutorial uses the DirectPlay Voice API to send and";
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.SystemColors.Info;
            this.label2.Location = new System.Drawing.Point(19, 32);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(296, 16);
            this.label2.TabIndex = 3;
            this.label2.Text = "receive voice messages.";
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
                                                                                    this.ConnectButton,
                                                                                    this.SessionStatusLabel,
                                                                                    this.HostButton,
                                                                                    this.ActiveRecordIcon,
                                                                                    this.ActivePlayIcon,
                                                                                    this.RecordIcon,
                                                                                    this.PlayIcon});
            this.groupBox1.Location = new System.Drawing.Point(8, 264);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(320, 96);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Session Status";
            // 
            // ConnectButton
            // 
            this.ConnectButton.Location = new System.Drawing.Point(112, 56);
            this.ConnectButton.Name = "ConnectButton";
            this.ConnectButton.Size = new System.Drawing.Size(80, 24);
            this.ConnectButton.TabIndex = 2;
            this.ConnectButton.Text = "&Connect...";
            this.ConnectButton.Click += new System.EventHandler(this.ConnectButton_Click);
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
            // ActiveRecordIcon
            // 
            this.ActiveRecordIcon.Image = ((System.Drawing.Bitmap)(resources.GetObject("ActiveRecordIcon.Image")));
            this.ActiveRecordIcon.Location = new System.Drawing.Point(232, 48);
            this.ActiveRecordIcon.Name = "ActiveRecordIcon";
            this.ActiveRecordIcon.Size = new System.Drawing.Size(32, 32);
            this.ActiveRecordIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.ActiveRecordIcon.TabIndex = 5;
            this.ActiveRecordIcon.TabStop = false;
            this.ActiveRecordIcon.Visible = false;
            // 
            // ActivePlayIcon
            // 
            this.ActivePlayIcon.Image = ((System.Drawing.Bitmap)(resources.GetObject("ActivePlayIcon.Image")));
            this.ActivePlayIcon.Location = new System.Drawing.Point(272, 48);
            this.ActivePlayIcon.Name = "ActivePlayIcon";
            this.ActivePlayIcon.Size = new System.Drawing.Size(32, 32);
            this.ActivePlayIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.ActivePlayIcon.TabIndex = 6;
            this.ActivePlayIcon.TabStop = false;
            this.ActivePlayIcon.Visible = false;
            // 
            // RecordIcon
            // 
            this.RecordIcon.Image = ((System.Drawing.Bitmap)(resources.GetObject("RecordIcon.Image")));
            this.RecordIcon.Location = new System.Drawing.Point(232, 48);
            this.RecordIcon.Name = "RecordIcon";
            this.RecordIcon.Size = new System.Drawing.Size(32, 32);
            this.RecordIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.RecordIcon.TabIndex = 4;
            this.RecordIcon.TabStop = false;
            // 
            // PlayIcon
            // 
            this.PlayIcon.Image = ((System.Drawing.Bitmap)(resources.GetObject("PlayIcon.Image")));
            this.PlayIcon.Location = new System.Drawing.Point(272, 48);
            this.PlayIcon.Name = "PlayIcon";
            this.PlayIcon.Size = new System.Drawing.Size(32, 32);
            this.PlayIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PlayIcon.TabIndex = 3;
            this.PlayIcon.TabStop = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.SendButton,
                                                                                    this.SendTextBox,
                                                                                    this.ReceivedMessagesListBox,
                                                                                    this.label4});
            this.groupBox2.Location = new System.Drawing.Point(8, 72);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(320, 184);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Communication";
            // 
            // SendButton
            // 
            this.SendButton.Location = new System.Drawing.Point(240, 144);
            this.SendButton.Name = "SendButton";
            this.SendButton.Size = new System.Drawing.Size(56, 24);
            this.SendButton.TabIndex = 4;
            this.SendButton.Text = "&Send";
            this.SendButton.Click += new System.EventHandler(this.SendButton_Click);
            // 
            // SendTextBox
            // 
            this.SendTextBox.AutoSize = false;
            this.SendTextBox.Location = new System.Drawing.Point(24, 144);
            this.SendTextBox.Name = "SendTextBox";
            this.SendTextBox.Size = new System.Drawing.Size(208, 24);
            this.SendTextBox.TabIndex = 3;
            this.SendTextBox.Text = "";
            // 
            // ReceivedMessagesListBox
            // 
            this.ReceivedMessagesListBox.IntegralHeight = false;
            this.ReceivedMessagesListBox.Location = new System.Drawing.Point(24, 40);
            this.ReceivedMessagesListBox.Name = "ReceivedMessagesListBox";
            this.ReceivedMessagesListBox.Size = new System.Drawing.Size(272, 96);
            this.ReceivedMessagesListBox.TabIndex = 2;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(24, 24);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(128, 16);
            this.label4.TabIndex = 0;
            this.label4.Text = "Received Messages:";
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.LobbyMenu});
            // 
            // LobbyMenu
            // 
            this.LobbyMenu.Index = 0;
            this.LobbyMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.LobbyMenuRegisterItem,
                                                                                      this.LobbyMenuUnregisterItem});
            this.LobbyMenu.Text = "&Lobby";
            // 
            // LobbyMenuRegisterItem
            // 
            this.LobbyMenuRegisterItem.Index = 0;
            this.LobbyMenuRegisterItem.Text = "&Register";
            this.LobbyMenuRegisterItem.Click += new System.EventHandler(this.LobbyMenuRegisterItem_Click);
            // 
            // LobbyMenuUnregisterItem
            // 
            this.LobbyMenuUnregisterItem.Index = 1;
            this.LobbyMenuUnregisterItem.Text = "&Unregister";
            this.LobbyMenuUnregisterItem.Click += new System.EventHandler(this.LobbyMenuUnregisterItem_Click);
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
            this.Menu = this.mainMenu1;
            this.MinimizeBox = false;
            this.Name = "ApplicationForm";
            this.Text = "Tutorial 8: Voice";
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
            ConnectButton.Enabled = true;
            App.UpdateUI();
        }

        /// <summary>
        /// Handler for Send button click
        /// </summary>
        private void SendButton_Click(object sender, System.EventArgs e)
        {
            App.SendData();
        }

        /// <summary>
        /// Handler for Lobby->Register item click
        /// </summary>
        private void LobbyMenuRegisterItem_Click(object sender, System.EventArgs e)
        {
            App.Register();
        }

        /// <summary>
        /// Handler for Lobby->Unregister item click
        /// </summary>
        private void LobbyMenuUnregisterItem_Click(object sender, System.EventArgs e)
        {
            App.Unregister();
        }
	}
}
