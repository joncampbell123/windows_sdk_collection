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

namespace Tut10_ThreadPool
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
        public  System.Windows.Forms.Button ConnectButton;
        private System.Windows.Forms.MainMenu mainMenu1;
        private System.Windows.Forms.MenuItem LobbyMenu;
        private System.Windows.Forms.MenuItem LobbyMenuRegisterItem;
        private System.Windows.Forms.MenuItem LobbyMenuUnregisterItem;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.RadioButton RedButton;
        private System.Windows.Forms.RadioButton GreenButton;
        private System.Windows.Forms.RadioButton BlueButton;
        private System.Windows.Forms.RadioButton BlackButton;
        private System.Windows.Forms.RadioButton GrayButton;
        private System.Windows.Forms.RadioButton WhiteButton;
        
        private ThreadPoolApp App;
        private bool IsMousePressed = false;
        public  Bitmap CanvasBitmap;
        private Graphics CanvasGraphics;
        private Graphics BitmapGraphics;
        private System.Windows.Forms.Button ClearButton;
        private Color BrushColor = Color.Red;

        private int CanvasWidth;
        private System.Windows.Forms.Panel Canvas;
        private int CanvasHeight;

        public ApplicationForm( ThreadPoolApp app )
		{
            this.App = app;

			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

            // Initialize the shared canvas
            CanvasWidth = Canvas.Width;
            CanvasHeight = Canvas.Height;

            CanvasBitmap = new Bitmap(CanvasWidth, CanvasHeight);
            BitmapGraphics = Graphics.FromImage(CanvasBitmap);
            CanvasGraphics = Canvas.CreateGraphics();

            SolidBrush whiteBrush = new SolidBrush(Color.White);
            BitmapGraphics.FillRectangle(whiteBrush, 0, 0, 
                                         CanvasBitmap.Width, CanvasBitmap.Height);
            
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

        /// <summary>
        /// Print information about the provided exception
        /// </summary>
        /// <param name="ex">Exception instance</param>
        /// <param name="calling">Name of the method which returned the exception</param>
        /// <param name="isFatal">Flag to indicate whether the given error is fatal</param>
        public void ShowException( Exception ex, string calling, bool isFatal )
        {
            string output = null;

            if( calling != null )
                output = "An error occurred while calling \"" + calling + "\"\n\n";
            else
                output = "An error occurred while executing the tutorial\n\n";

            output += "Message: " + ex.Message + "\n";
            
            if( ex is DirectXException )
            {
                // DirectX-specific info
                DirectXException dex = (DirectXException) ex;
                output += "HRESULT: " + dex.ErrorString + " (" + dex.ErrorCode.ToString( "X" ) + ")\n";
            }
           
            output += "Source: " + ex.Source + "\n";  

            if( isFatal )
                output += "\nThe application will now exit\n";

            MessageBox.Show( this, output, "DirectPlay Tutorial", MessageBoxButtons.OK, MessageBoxIcon.Error );
        }


        public void RenderCanvas()
        {
            if(this.Visible)
                CanvasGraphics.DrawImage(CanvasBitmap, 0, 0);
        }

        /// <summary>
        /// Draw a point at the given coordinates on the local canvas image. Any 
        /// points drawn with this method won't show up until the underlying
        /// PictureBox is invalidated inside the game loop.
        /// </summary>
        public void DrawPoint( int x, int y, Color color )
        {  
            SolidBrush pointBrush = new SolidBrush(color);

            lock(BitmapGraphics)
            {
                BitmapGraphics.FillRectangle(pointBrush, new Rectangle(x-1, y-1, 3, 3));
            }
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
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.WhiteButton = new System.Windows.Forms.RadioButton();
            this.GrayButton = new System.Windows.Forms.RadioButton();
            this.BlackButton = new System.Windows.Forms.RadioButton();
            this.BlueButton = new System.Windows.Forms.RadioButton();
            this.GreenButton = new System.Windows.Forms.RadioButton();
            this.RedButton = new System.Windows.Forms.RadioButton();
            this.ClearButton = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.LobbyMenu = new System.Windows.Forms.MenuItem();
            this.LobbyMenuRegisterItem = new System.Windows.Forms.MenuItem();
            this.LobbyMenuUnregisterItem = new System.Windows.Forms.MenuItem();
            this.Canvas = new System.Windows.Forms.Panel();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // ExitButton
            // 
            this.ExitButton.Location = new System.Drawing.Point(264, 427);
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
            this.label1.Text = "This tutorial uses the IDirectPlay8ThreadPool interface to";
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.SystemColors.Info;
            this.label2.Location = new System.Drawing.Point(19, 32);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(296, 16);
            this.label2.TabIndex = 3;
            this.label2.Text = "better integrate networking code into a game loop.";
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
                                                                                    this.HostButton});
            this.groupBox1.Location = new System.Drawing.Point(8, 320);
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
            // groupBox2
            // 
            this.groupBox2.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                    this.Canvas,
                                                                                    this.WhiteButton,
                                                                                    this.GrayButton,
                                                                                    this.BlackButton,
                                                                                    this.BlueButton,
                                                                                    this.GreenButton,
                                                                                    this.RedButton,
                                                                                    this.ClearButton,
                                                                                    this.label4,
                                                                                    this.label3});
            this.groupBox2.Location = new System.Drawing.Point(8, 72);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(320, 240);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Shared Canvas";
            // 
            // WhiteButton
            // 
            this.WhiteButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.WhiteButton.BackColor = System.Drawing.Color.White;
            this.WhiteButton.Location = new System.Drawing.Point(162, 200);
            this.WhiteButton.Name = "WhiteButton";
            this.WhiteButton.Size = new System.Drawing.Size(20, 20);
            this.WhiteButton.TabIndex = 9;
            this.WhiteButton.CheckedChanged += new System.EventHandler(this.ColorChanged);
            // 
            // GrayButton
            // 
            this.GrayButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.GrayButton.BackColor = System.Drawing.Color.Gray;
            this.GrayButton.Location = new System.Drawing.Point(136, 200);
            this.GrayButton.Name = "GrayButton";
            this.GrayButton.Size = new System.Drawing.Size(20, 20);
            this.GrayButton.TabIndex = 8;
            this.GrayButton.CheckedChanged += new System.EventHandler(this.ColorChanged);
            // 
            // BlackButton
            // 
            this.BlackButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.BlackButton.BackColor = System.Drawing.Color.Black;
            this.BlackButton.Location = new System.Drawing.Point(110, 200);
            this.BlackButton.Name = "BlackButton";
            this.BlackButton.Size = new System.Drawing.Size(20, 20);
            this.BlackButton.TabIndex = 7;
            this.BlackButton.CheckedChanged += new System.EventHandler(this.ColorChanged);
            // 
            // BlueButton
            // 
            this.BlueButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.BlueButton.BackColor = System.Drawing.Color.Blue;
            this.BlueButton.Location = new System.Drawing.Point(84, 200);
            this.BlueButton.Name = "BlueButton";
            this.BlueButton.Size = new System.Drawing.Size(20, 20);
            this.BlueButton.TabIndex = 6;
            this.BlueButton.CheckedChanged += new System.EventHandler(this.ColorChanged);
            // 
            // GreenButton
            // 
            this.GreenButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.GreenButton.BackColor = System.Drawing.Color.Lime;
            this.GreenButton.Location = new System.Drawing.Point(58, 200);
            this.GreenButton.Name = "GreenButton";
            this.GreenButton.Size = new System.Drawing.Size(20, 20);
            this.GreenButton.TabIndex = 5;
            this.GreenButton.CheckedChanged += new System.EventHandler(this.ColorChanged);
            // 
            // RedButton
            // 
            this.RedButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.RedButton.BackColor = System.Drawing.Color.Red;
            this.RedButton.Checked = true;
            this.RedButton.Location = new System.Drawing.Point(32, 200);
            this.RedButton.Name = "RedButton";
            this.RedButton.Size = new System.Drawing.Size(20, 20);
            this.RedButton.TabIndex = 4;
            this.RedButton.TabStop = true;
            this.RedButton.CheckedChanged += new System.EventHandler(this.ColorChanged);
            // 
            // ClearButton
            // 
            this.ClearButton.Location = new System.Drawing.Point(216, 200);
            this.ClearButton.Name = "ClearButton";
            this.ClearButton.Size = new System.Drawing.Size(80, 24);
            this.ClearButton.TabIndex = 3;
            this.ClearButton.Text = "C&lear";
            this.ClearButton.Click += new System.EventHandler(this.ClearButton_Click);
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(24, 40);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(232, 23);
            this.label4.TabIndex = 2;
            this.label4.Text = "the canvas to paint";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(24, 24);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(264, 23);
            this.label3.TabIndex = 1;
            this.label3.Text = "Hold down the left mouse button and drag within";
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
            // Canvas
            // 
            this.Canvas.Location = new System.Drawing.Point(32, 72);
            this.Canvas.Name = "Canvas";
            this.Canvas.Size = new System.Drawing.Size(264, 120);
            this.Canvas.TabIndex = 10;
            this.Canvas.MouseUp += new System.Windows.Forms.MouseEventHandler(this.CanvasPictureBox_MouseUp);
            this.Canvas.MouseMove += new System.Windows.Forms.MouseEventHandler(this.CanvasPictureBox_MouseMove);
            this.Canvas.MouseDown += new System.Windows.Forms.MouseEventHandler(this.CanvasPictureBox_MouseDown);
            // 
            // ApplicationForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(336, 459);
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
            this.Text = "Tutorial 10: ThreadPool";
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

            if( App.Connection == ConnectionType.Disconnected )
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

       

        /// <summary>
        /// Handler for Canvas MouseDown event. This signals the beginning of the brush stroke.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CanvasPictureBox_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            IsMousePressed = true;
        }



        /// <summary>
        /// Handler for Canvas MouseMove event. This paints the current color on the canvas.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CanvasPictureBox_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (IsMousePressed)
            {
                DrawPoint( e.X, e.Y, BrushColor );
                App.AddPoint( e.X, e.Y, BrushColor );
            }
        }




        /// <summary>
        /// Handler for Canvas MouseUp Event. This signals the end of the brush stroke
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CanvasPictureBox_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            IsMousePressed = false;
        }

        private void ColorChanged(object sender, System.EventArgs e)
        {
            RadioButton radio = (RadioButton)sender;
            BrushColor = radio.BackColor;
        }

        private void ClearButton_Click(object sender, System.EventArgs e)
        {
            SolidBrush whiteBrush = new SolidBrush(Color.White);
            lock(BitmapGraphics)
            {
                BitmapGraphics.FillRectangle(whiteBrush, 0, 0, 
                                             CanvasBitmap.Width, CanvasBitmap.Height);
            }
        }
	}
}
