using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace DXMessengerClient
{
	/// <summary>
	/// This class will mimic the InputBox function for VB
	/// </summary>
	public class wfInput : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label lblMessage;
		private System.Windows.Forms.TextBox txtInput;
		private System.Windows.Forms.Button btnOk;
		private System.Windows.Forms.Button btnCancel;
		private System.ComponentModel.Container components = null;

		private wfInput(string windowTitle, string messageTitle)
		{
			InitializeComponent();
			this.Text = windowTitle;
			this.lblMessage.Text = messageTitle;
		}

		public static string InputBox(string windowTitle, string messageTitle)
		{
			wfInput input = new wfInput(windowTitle, messageTitle);
			try
			{
				if (input.ShowDialog() == DialogResult.OK)
				{
					return input.InputText;
				}
				else
					return null;
			}
			catch {return null;}
			finally
			{
				input.Dispose();
			}
		}
		public string InputText
		{
			get { return this.txtInput.Text; }
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
			this.lblMessage = new System.Windows.Forms.Label();
			this.txtInput = new System.Windows.Forms.TextBox();
			this.btnOk = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// lblMessage
			// 
			this.lblMessage.Location = new System.Drawing.Point(5, 8);
			this.lblMessage.Name = "lblMessage";
			this.lblMessage.Size = new System.Drawing.Size(283, 17);
			this.lblMessage.TabIndex = 0;
			// 
			// txtInput
			// 
			this.txtInput.Location = new System.Drawing.Point(4, 36);
			this.txtInput.Name = "txtInput";
			this.txtInput.Size = new System.Drawing.Size(282, 20);
			this.txtInput.TabIndex = 1;
			this.txtInput.Text = "";
			// 
			// btnOk
			// 
			this.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOk.Location = new System.Drawing.Point(211, 63);
			this.btnOk.Name = "btnOk";
			this.btnOk.Size = new System.Drawing.Size(72, 22);
			this.btnOk.TabIndex = 2;
			this.btnOk.Text = "OK";
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Location = new System.Drawing.Point(135, 63);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(72, 22);
			this.btnCancel.TabIndex = 3;
			this.btnCancel.Text = "Cancel";
			// 
			// wfInput
			// 
			this.AcceptButton = this.btnOk;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.btnCancel;
			this.ClientSize = new System.Drawing.Size(292, 92);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnCancel,
																		  this.btnOk,
																		  this.txtInput,
																		  this.lblMessage});
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "wfInput";
			this.ShowInTaskbar = false;
			this.ResumeLayout(false);

		}
		#endregion
	}
}
