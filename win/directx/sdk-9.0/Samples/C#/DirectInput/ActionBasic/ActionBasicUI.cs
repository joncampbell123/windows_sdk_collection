//-----------------------------------------------------------------------------
// File: ActionBasicUI.cs
//
// Desc: The ActionBasic sample is intended to be an introduction to action mapping, 
//       and illustrates a step by step approach to creating an action mapped 
//       application.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
using System;
using System.Text;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;

namespace ActionBasic
{
	/// <summary>
	/// description.
	/// </summary>
	public class ActionBasicUI : System.Windows.Forms.Form
	{
        private ActionBasicApp app;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox1;
        private ActionBasic.Chart chart;
        private System.Windows.Forms.Button ConfigButton;
        private System.Windows.Forms.Button ExitButton;
        /// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="app">Reference to the application object</param>
        /// <param name="ActionNames">List of game action strings</param>
        /// <param name="deviceStates">Reference array for the device states</param>
		public ActionBasicUI(ActionBasicApp app, String[] ActionNames, ArrayList deviceStates)
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

            this.app = app;
            //
			// Required for Windows Form Designer support
			//
			InitializeComponent();

            chart.ColumnTitles = ActionNames;
            chart.RowData = deviceStates;
		}

        
        
        
        /// <summary>
        /// Print information about the provided exception
        /// </summary>
        /// <param name="ex">Exception instance</param>
        /// <param name="calling">Name of the method which returned the exception</param>
        public void ShowException(Exception ex, string calling)
        {
            string output = ex.Message + "\n\n";
            
            if (ex.GetType().DeclaringType == Type.GetType("System.DirectX.DirectXException"))
            {
                // DirectX-specific info
                DirectXException dex = (DirectXException) ex;
                output += "HRESULT: " + dex.ErrorString + " (" + dex.ErrorCode.ToString("X") + ")\n";
            }
           
            output += "Calling: " + calling + "\n";
            output += "Source: " + ex.Source + "\n";  
            MessageBox.Show(this, output, "ActionBasic Sample Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        
        
        
        /// <summary>
        /// Force the chart object to repaint
        /// </summary>
        public void UpdateChart()
        {
            if (!chart.Created)
                return;
            
            try{chart.UpdateData();}
            catch(Exception){}
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
        /// Display the user guide
        /// </summary>
        private void DisplayHelp()
        {
            StringBuilder message = new StringBuilder();

            message.Append("The chart shows the list of devices found ");
            message.Append("attached to your computer, plotted against a\n");
            message.Append("defined set of actions for an imaginary fighting ");
            message.Append("game.\n\n");
            message.Append("30 times per second, the program polls for new "); 
            message.Append("input from the devices, and displays which actions\n");
            message.Append("are currently being sent by each device. During ");
            message.Append("initialization, the program attempts to establish\n");
            message.Append("a mapping between actions and device objects for ");
            message.Append("each attached device. Actions which were not\n");
            message.Append("mapped to a device object are shown as a ");
            message.Append("crosshatch-filled cell on the chart.\n\n");
            message.Append("To view the current action mappings for all the ");
            message.Append("devices, click the \"View Configuration\" button,\n");
            message.Append("which will access the default configuration UI ");
            message.Append("managed by DirectInput.");

            MessageBox.Show(this, message.ToString(), "ActionBasic Help", MessageBoxButtons.OK);
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
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.chart = new ActionBasic.Chart();
            this.ConfigButton = new System.Windows.Forms.Button();
            this.ExitButton = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(16, 24);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(344, 16);
            this.label1.TabIndex = 0;
            this.label1.Text = "This tutorial polls for action-mapped data from the attached devices,";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(16, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(344, 16);
            this.label2.TabIndex = 1;
            this.label2.Text = "and displays a chart showing triggered game actions plotted against";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(16, 56);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(344, 16);
            this.label3.TabIndex = 2;
            this.label3.Text = "the corresponding input devices.";
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(16, 80);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(344, 16);
            this.label4.TabIndex = 3;
            this.label4.Text = "Press the F1 key for help.";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.chart,
                                                                                    this.ConfigButton,
                                                                                    this.label4,
                                                                                    this.label3,
                                                                                    this.label2,
                                                                                    this.label1});
            this.groupBox1.Location = new System.Drawing.Point(8, 8);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(408, 400);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            // 
            // chart
            // 
            this.chart.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.chart.ColumnTitles = null;
            this.chart.Location = new System.Drawing.Point(16, 112);
            this.chart.Name = "chart";
            this.chart.RowData = null;
            this.chart.Size = new System.Drawing.Size(376, 240);
            this.chart.TabIndex = 6;
            this.chart.Text = "chart";
            // 
            // ConfigButton
            // 
            this.ConfigButton.Location = new System.Drawing.Point(272, 368);
            this.ConfigButton.Name = "ConfigButton";
            this.ConfigButton.Size = new System.Drawing.Size(120, 24);
            this.ConfigButton.TabIndex = 5;
            this.ConfigButton.Text = "&View Configuration";
            this.ConfigButton.Click += new System.EventHandler(this.ConfigButton_Click);
            // 
            // ExitButton
            // 
            this.ExitButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.ExitButton.Location = new System.Drawing.Point(344, 416);
            this.ExitButton.Name = "ExitButton";
            this.ExitButton.Size = new System.Drawing.Size(72, 24);
            this.ExitButton.TabIndex = 5;
            this.ExitButton.Text = "Exit";
            this.ExitButton.Click += new System.EventHandler(this.ExitButton_Click);
            // 
            // ActionBasicUI
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.ExitButton;
            this.ClientSize = new System.Drawing.Size(424, 446);
            this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                          this.ExitButton,
                                                                          this.groupBox1});
            this.MaximizeBox = false;
            this.Name = "ActionBasicUI";
            this.Text = "ActionBasic Sample";
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
		#endregion

        
        
        
        private void ExitButton_Click(object sender, System.EventArgs e)
        {
            ((Button)sender).DialogResult = DialogResult.Cancel;
            this.Close();
        }

        
        
        
        protected override void OnHelpRequested(HelpEventArgs e)
        {
            e.Handled = true;
            DisplayHelp();
        }


        
        
        
        private void ConfigButton_Click(object sender, System.EventArgs e)
        {
            app.ConfigureDevices();
        }            
	}
}
