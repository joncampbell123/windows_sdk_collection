//----------------------------------------------------------------------------
// File: wfMsg.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace DXMessengerClient
{
	/// <summary>
	/// Summary description for wfMsg.
	/// </summary>
	public class wfMsg : System.Windows.Forms.Form
	{
		public delegate void AddChatMsgCallback(string msg, bool meTalking, bool noTalking);
		public delegate void ShowWindowCallback();
		private System.Windows.Forms.TextBox txtMsg;
		private System.Windows.Forms.Button btnSend;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.TextBox txtChat;

		private string msUser = null;
		wfClient parent = null;

		public string UserName { get { return msUser; } }
		public wfMsg(string username, wfClient obj)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			msUser = username;
			parent = obj;
			this.Text = "Message - "  + msUser;

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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(wfMsg));
			this.txtMsg = new System.Windows.Forms.TextBox();
			this.txtChat = new System.Windows.Forms.TextBox();
			this.btnSend = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// txtMsg
			// 
			this.txtMsg.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.txtMsg.Location = new System.Drawing.Point(6, 3);
			this.txtMsg.Multiline = true;
			this.txtMsg.Name = "txtMsg";
			this.txtMsg.ReadOnly = true;
			this.txtMsg.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.txtMsg.Size = new System.Drawing.Size(454, 292);
			this.txtMsg.TabIndex = 100;
			this.txtMsg.TabStop = false;
			this.txtMsg.Text = "";
			// 
			// txtChat
			// 
			this.txtChat.Anchor = ((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.txtChat.Location = new System.Drawing.Point(4, 300);
			this.txtChat.Multiline = true;
			this.txtChat.Name = "txtChat";
			this.txtChat.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.txtChat.Size = new System.Drawing.Size(377, 41);
			this.txtChat.TabIndex = 0;
			this.txtChat.Text = "";
			this.txtChat.TextChanged += new System.EventHandler(this.txtChat_TextChanged);
			// 
			// btnSend
			// 
			this.btnSend.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right);
			this.btnSend.Location = new System.Drawing.Point(385, 302);
			this.btnSend.Name = "btnSend";
			this.btnSend.Size = new System.Drawing.Size(75, 37);
			this.btnSend.TabIndex = 1;
			this.btnSend.Text = "Send";
			this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
			// 
			// wfMsg
			// 
			this.AcceptButton = this.btnSend;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(461, 341);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnSend,
																		  this.txtChat,
																		  this.txtMsg});
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "wfMsg";
			this.Text = "MessageTemplate";
			this.Enter += new System.EventHandler(this.wfMsg_Enter);
			this.ResumeLayout(false);

		}
		#endregion

		public void ShowWindow()
		{
			// Show the window, and add the text
			this.Show();
			this.Select();
			this.BringToFront();
		}
		private void wfMsg_Enter(object sender, System.EventArgs e)
		{
			// We have the focus, make the chat textbox the default option
			this.txtChat.Select();
		}

		public void AddChatMessage(string msg, bool meTalking, bool noTalking)
		{
			if (!noTalking)
			{
				if (meTalking)
					msg = "<" + parent.gUsername + "> " + msg;
				else
					msg = "<" + msUser + "> " + msg;
			}

			// Now limit the text to 32k
			if (txtMsg.Text.Length > (txtMsg.MaxLength * 0.95))
				txtMsg.Text = txtMsg.Text.Remove(0, (int)(txtMsg.MaxLength / 2));

			// Update the message window
			txtMsg.Text += msg + "\r\n";
			txtMsg.SelectionStart = txtMsg.Text.Length;
			txtMsg.ScrollToCaret();

		}
		private void btnSend_Click(object sender, System.EventArgs e)
		{
			// We're talking
			if ((txtChat.Text != null) && (txtChat.Text != ""))
			{
				if (parent.gConnected)
				{
					parent.SendChatMessage(msUser, txtChat.Text);
					object[] unavailableMsg = {txtChat.Text, true, false };

					this.BeginInvoke(new AddChatMsgCallback(this.AddChatMessage), unavailableMsg);
					txtChat.Text = null;
				}
				else
				{
					object[] unavailableMsg = {"**** - You are not connected to a server, you cannot send messages.", true, true };
					this.BeginInvoke(new AddChatMsgCallback(this.AddChatMessage), unavailableMsg);
					txtChat.Text = null;
				}
			}
		}

		private void txtChat_TextChanged(object sender, System.EventArgs e)
		{
			// Only allow them to send if there's data to be sent.
			btnSend.Enabled = ((txtChat.Text != null) && (txtChat.Text != ""));
		}
	}
}
