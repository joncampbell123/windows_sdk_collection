//-----------------------------------------------------------------------------
// File: DrawWindow.cs
//
// Desc: Example code showing how to do use MDI windows via DirectDraw
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace MDIWindow
{
    public class DrawWindow : System.Windows.Forms.Form
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
      
        public DrawWindow()
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
        



        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            // 
            // DrawWindow
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Name = "DrawWindow";
            this.Text = "DrawWindow";
            this.Resize += new System.EventHandler(this.DrawWindow_SizeChanged);
            this.SizeChanged += new System.EventHandler(this.DrawWindow_SizeChanged);
            this.Move += new System.EventHandler(this.DrawWindow_Move);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.DrawWindow_Paint);

        }
        #endregion




        /// <summary>
        /// various events to do an updated draw
        /// </summary>
        private void DrawWindow_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            ((MainWindow)this.MdiParent).Draw(this, PointToScreen(new Point(0, 0)), this.ClientSize);   
        }




        private void DrawWindow_SizeChanged(object sender, System.EventArgs e)
        {
            if (FormWindowState.Minimized == WindowState)
                return;

            ((MainWindow)this.MdiParent).Draw(this, PointToScreen(new Point(0, 0)), this.ClientSize);
        }




        private void DrawWindow_Move(object sender, System.EventArgs e)
        {
            ((MainWindow)this.MdiParent).Draw(this, PointToScreen(new Point(0, 0)), this.ClientSize);
        }
    }
}
