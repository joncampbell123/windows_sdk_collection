//----------------------------------------------------------------------------
// File: HostDialog.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Tut04_Connect
{
    /// <summary>
    /// Summary description for HostDialog.
    /// </summary>
    public class HostDialog : System.Windows.Forms.Form
    {
        private string   m_SessionName           = "New Host";  // Hosted session name 
       
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.TextBox SessionNameTextBox;
        private System.Windows.Forms.Label SessionNameLabel;
        private System.Windows.Forms.TextBox portTextBox;
        private System.Windows.Forms.Label portLabel;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        // Properties
        public string SessionName{ get{ return m_SessionName; } } 
        
        /// <summary>
        /// Property: Local port on which to host
        /// </summary>
        public int LocalPort
        {
            get
            { 
                int retValue = 0;

                try
                {
                    retValue = int.Parse(portTextBox.Text); 
                }
                catch (Exception) {}
                
                return retValue;
            }
            set
            { portTextBox.Text = value.ToString(); }
        }

        /// <summary>
        /// Constructor
        /// </summary>
        public HostDialog()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            //
            // TODO: Add any constructor code after InitializeComponent call
            //
            SessionNameTextBox.Text = m_SessionName;
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
            this.OKButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.SessionNameTextBox = new System.Windows.Forms.TextBox();
            this.SessionNameLabel = new System.Windows.Forms.Label();
            this.portTextBox = new System.Windows.Forms.TextBox();
            this.portLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.SystemColors.Info;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
            this.label1.Location = new System.Drawing.Point(15, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(224, 16);
            this.label1.TabIndex = 0;
            this.label1.Text = "Please provide a name for the new session.";
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.SystemColors.Info;
            this.label2.Location = new System.Drawing.Point(15, 32);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(232, 16);
            this.label2.TabIndex = 1;
            this.label2.Text = "This name will help other players identify you.";
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.SystemColors.Info;
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(8, 8);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(240, 48);
            this.pictureBox1.TabIndex = 2;
            this.pictureBox1.TabStop = false;
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(112, 144);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(64, 24);
            this.OKButton.TabIndex = 3;
            this.OKButton.Text = "&OK";
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(184, 144);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(64, 24);
            this.cancelButton.TabIndex = 4;
            this.cancelButton.Text = "&Cancel";
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // SessionNameTextBox
            // 
            this.SessionNameTextBox.AutoSize = false;
            this.SessionNameTextBox.Location = new System.Drawing.Point(104, 72);
            this.SessionNameTextBox.Name = "SessionNameTextBox";
            this.SessionNameTextBox.Size = new System.Drawing.Size(128, 20);
            this.SessionNameTextBox.TabIndex = 5;
            this.SessionNameTextBox.Text = "New Host";
            // 
            // SessionNameLabel
            // 
            this.SessionNameLabel.Location = new System.Drawing.Point(16, 74);
            this.SessionNameLabel.Name = "SessionNameLabel";
            this.SessionNameLabel.Size = new System.Drawing.Size(83, 16);
            this.SessionNameLabel.TabIndex = 6;
            this.SessionNameLabel.Text = " Session Name ";
            this.SessionNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // portTextBox
            // 
            this.portTextBox.Location = new System.Drawing.Point(104, 104);
            this.portTextBox.Name = "portTextBox";
            this.portTextBox.Size = new System.Drawing.Size(56, 20);
            this.portTextBox.TabIndex = 7;
            this.portTextBox.Text = "";
            // 
            // portLabel
            // 
            this.portLabel.Location = new System.Drawing.Point(48, 106);
            this.portLabel.Name = "portLabel";
            this.portLabel.Size = new System.Drawing.Size(48, 16);
            this.portLabel.TabIndex = 8;
            this.portLabel.Text = " Port ";
            this.portLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // HostDialog
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(258, 176);
            this.ControlBox = false;
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.portLabel,
                                                                          this.portTextBox,
                                                                          this.SessionNameLabel,
                                                                          this.SessionNameTextBox,
                                                                          this.cancelButton,
                                                                          this.OKButton,
                                                                          this.label2,
                                                                          this.label1,
                                                                          this.pictureBox1});
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "HostDialog";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Host New Session";
            this.ResumeLayout(false);

        }
		#endregion

        private void OKButton_Click(object sender, System.EventArgs e)
        {
            // Save the current settings
            m_SessionName = SessionNameTextBox.Text;
            
            DialogResult = DialogResult.OK;
        }

        private void cancelButton_Click(object sender, System.EventArgs e)
        {
            // Discard settings
            DialogResult = DialogResult.Cancel;
        }
    }
}
