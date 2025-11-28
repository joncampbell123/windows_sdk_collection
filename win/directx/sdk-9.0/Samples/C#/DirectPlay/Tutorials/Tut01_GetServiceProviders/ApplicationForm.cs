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

namespace Tut01_GetServiceProviders
{
	/// <summary>
	/// Application's main WinForm
	/// </summary>
	public class ApplicationForm : System.Windows.Forms.Form
	{
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button ExitButton;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        public  System.Windows.Forms.ListBox SPListBox;

        public ApplicationForm()
		{

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
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label4 = new System.Windows.Forms.Label();
            this.SPListBox = new System.Windows.Forms.ListBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // ExitButton
            // 
            this.ExitButton.Location = new System.Drawing.Point(264, 264);
            this.ExitButton.Name = "ExitButton";
            this.ExitButton.Size = new System.Drawing.Size(64, 24);
            this.ExitButton.TabIndex = 1;
            this.ExitButton.Text = "E&xit";
            this.ExitButton.Click += new System.EventHandler(this.ExitButton_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.SystemColors.Info;
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(8, 8);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(320, 64);
            this.pictureBox1.TabIndex = 6;
            this.pictureBox1.TabStop = false;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(8, 96);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(144, 16);
            this.label4.TabIndex = 11;
            this.label4.Text = "Detected Service Providers:";
            // 
            // SPListBox
            // 
            this.SPListBox.Location = new System.Drawing.Point(8, 112);
            this.SPListBox.Name = "SPListBox";
            this.SPListBox.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.SPListBox.Size = new System.Drawing.Size(320, 134);
            this.SPListBox.TabIndex = 10;
            // 
            // label3
            // 
            this.label3.BackColor = System.Drawing.SystemColors.Info;
            this.label3.Location = new System.Drawing.Point(20, 48);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(296, 16);
            this.label3.TabIndex = 14;
            this.label3.Text = "protocol to be used for network communication.";
            // 
            // label6
            // 
            this.label6.BackColor = System.Drawing.SystemColors.Info;
            this.label6.Location = new System.Drawing.Point(20, 32);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(296, 16);
            this.label6.TabIndex = 13;
            this.label6.Text = "found on your computer. A service provider defines the";
            // 
            // label7
            // 
            this.label7.BackColor = System.Drawing.SystemColors.Info;
            this.label7.Location = new System.Drawing.Point(20, 16);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(296, 16);
            this.label7.TabIndex = 12;
            this.label7.Text = "This tutorial lists the available DirectPlay service providers";
            // 
            // ApplicationForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(336, 296);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.label3,
                                                                          this.label6,
                                                                          this.label7,
                                                                          this.label4,
                                                                          this.SPListBox,
                                                                          this.ExitButton,
                                                                          this.pictureBox1});
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ApplicationForm";
            this.Text = "Tutorial 1: Get Service Providers";
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
	}
}
