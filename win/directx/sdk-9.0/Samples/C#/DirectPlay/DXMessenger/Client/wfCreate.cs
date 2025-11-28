//----------------------------------------------------------------------------
// File: wfCreate.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using DXMessenger;

namespace DXMessengerClient
{
	/// <summary>
	/// Summary description for wfCreate.
	/// </summary>
	public class wfCreate : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnOk;
		private System.Windows.Forms.TextBox txtServer;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox txtPwd;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox txtUser;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox txtVerify;
		private System.Windows.Forms.Label label5;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private wfClient parent = null;

		public wfCreate(wfClient obj)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			parent = obj;

			// Load the default values from the registry
			Microsoft.Win32.RegistryKey key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\\" + MessengerShared.ApplicationName + "\\Client", true);
			// If the key doesn't exist, create it
			if (key == null)
				key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("Software\\" + MessengerShared.ApplicationName + "\\Client");

			txtServer.Text = (string)key.GetValue("ServerName", null);
			key.Close();
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
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnOk = new System.Windows.Forms.Button();
			this.txtServer = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.txtPwd = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.txtUser = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.txtVerify = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Location = new System.Drawing.Point(214, 233);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(72, 22);
			this.btnCancel.TabIndex = 4;
			this.btnCancel.Text = "Cancel";
			// 
			// btnOk
			// 
			this.btnOk.Location = new System.Drawing.Point(290, 233);
			this.btnOk.Name = "btnOk";
			this.btnOk.Size = new System.Drawing.Size(72, 22);
			this.btnOk.TabIndex = 5;
			this.btnOk.Text = "Create";
			this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
			// 
			// txtServer
			// 
			this.txtServer.Location = new System.Drawing.Point(10, 206);
			this.txtServer.MaxLength = 255;
			this.txtServer.Name = "txtServer";
			this.txtServer.Size = new System.Drawing.Size(352, 20);
			this.txtServer.TabIndex = 3;
			this.txtServer.Text = "";
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(10, 188);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(100, 16);
			this.label4.TabIndex = 16;
			this.label4.Text = "Server name:";
			// 
			// txtPwd
			// 
			this.txtPwd.Location = new System.Drawing.Point(10, 116);
			this.txtPwd.MaxLength = 255;
			this.txtPwd.Name = "txtPwd";
			this.txtPwd.PasswordChar = '*';
			this.txtPwd.Size = new System.Drawing.Size(352, 20);
			this.txtPwd.TabIndex = 1;
			this.txtPwd.Text = "";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(10, 98);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(100, 16);
			this.label3.TabIndex = 14;
			this.label3.Text = "Password:";
			// 
			// txtUser
			// 
			this.txtUser.Location = new System.Drawing.Point(10, 69);
			this.txtUser.MaxLength = 255;
			this.txtUser.Name = "txtUser";
			this.txtUser.Size = new System.Drawing.Size(352, 20);
			this.txtUser.TabIndex = 0;
			this.txtUser.Text = "";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(10, 51);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(100, 16);
			this.label2.TabIndex = 12;
			this.label2.Text = "Username:";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(10, 7);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(352, 38);
			this.label1.TabIndex = 11;
			this.label1.Text = "Please type the username you wish to use, a password, and the server you wish to " +
				"connect with.";
			// 
			// txtVerify
			// 
			this.txtVerify.Location = new System.Drawing.Point(10, 162);
			this.txtVerify.MaxLength = 255;
			this.txtVerify.Name = "txtVerify";
			this.txtVerify.PasswordChar = '*';
			this.txtVerify.Size = new System.Drawing.Size(352, 20);
			this.txtVerify.TabIndex = 2;
			this.txtVerify.Text = "";
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(10, 144);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(100, 16);
			this.label5.TabIndex = 21;
			this.label5.Text = "Verify password:";
			// 
			// wfCreate
			// 
			this.AcceptButton = this.btnOk;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.btnCancel;
			this.ClientSize = new System.Drawing.Size(370, 265);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.txtVerify,
																		  this.label5,
																		  this.btnCancel,
																		  this.btnOk,
																		  this.txtServer,
																		  this.label4,
																		  this.txtPwd,
																		  this.label3,
																		  this.txtUser,
																		  this.label2,
																		  this.label1});
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "wfCreate";
			this.Text = "Create new account";
			this.ResumeLayout(false);

		}
		#endregion

		private void btnOk_Click(object sender, System.EventArgs e)
		{
			// Make sure the values our filled out before continuing.
			if ((txtServer.Text == null) || (txtServer.Text == ""))
			{
				MessageBox.Show("You must enter a server name.", "No server name", MessageBoxButtons.OK, MessageBoxIcon.Information);
				return;
			}
			if ((txtUser.Text == null) || (txtUser.Text == ""))
			{
				MessageBox.Show("You must enter a user name.", "No user name", MessageBoxButtons.OK, MessageBoxIcon.Information);
				return;
			}
			if ((txtPwd.Text == null) || (txtPwd.Text == ""))
			{
				MessageBox.Show("You must enter a password.", "No password", MessageBoxButtons.OK, MessageBoxIcon.Information);
				return;
			}
			if (txtPwd.Text != txtVerify.Text)
			{
				MessageBox.Show("The passwords you've entered do not match.", "Passwords unmatched", MessageBoxButtons.OK, MessageBoxIcon.Information);
				return;
			}
			// Great, save these settings, and lets move on.
			Microsoft.Win32.RegistryKey key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("Software\\" + MessengerShared.ApplicationName + "\\Client",true);
			// If the key doesn't exist, create it
			if (key == null)
				key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("Software\\" + MessengerShared.ApplicationName + "\\Client");

			key.SetValue("ServerName", txtServer.Text);
			key.SetValue("Username", txtUser.Text);
			key.SetValue("Password", "");

			key.Close();

			parent.Select();
			parent.BringToFront();
			this.Hide();
			if ((parent.gConnected) && (parent.gServer == txtServer.Text))
				// We've already connected to this server, and are still connected.  No
				// need to try and connect once more
				parent.CreatePlayer(txtUser.Text, MessengerShared.EncodePassword(txtPwd.Text));
			else
				// If we're connected, it's not to this server, go ahead and try to connect now
				parent.CreatePlayer(txtServer.Text, txtUser.Text, MessengerShared.EncodePassword(txtPwd.Text));

		}
	}
}
