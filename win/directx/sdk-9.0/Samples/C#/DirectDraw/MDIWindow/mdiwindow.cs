//-----------------------------------------------------------------------------
// File: MDIWindow.cs
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
using System.Data;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;
using System.Diagnostics;

namespace MDIWindow
{
    /// <summary>
    /// Summary description for MainWindow.
    /// </summary>
    public class MainWindow : System.Windows.Forms.Form
    {
        private System.Windows.Forms.MainMenu mainMenu1;
        private System.Windows.Forms.MenuItem FileMenu;
        private System.Windows.Forms.MenuItem OpenWindowMenuItem;
        private System.Windows.Forms.MenuItem ExitMenuItem;
        private System.Windows.Forms.MenuItem WindowMenu;
        private System.Windows.Forms.MenuItem TileHorizontalMenuItem;
        private System.Windows.Forms.MenuItem TileVerticalMenuItem;
        private System.Windows.Forms.MenuItem CascadeMenuItem;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
        private Device draw = new Device(); // Holds the DrawDevice object.
        private Surface primary = null; // Holds the primary destination surface.
        private Surface offscreen = null; // Holds the offscreen source surface.
        private Clipper clip = null; // Holds the clipper object.
        



        public MainWindow()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            draw = new Device(); // Create a new DrawDevice, using the default device.
            draw.SetCooperativeLevel(this, CooperativeLevelFlags.Normal); // Set the coop level to normal windowed mode.
            clip = new Clipper(draw); // Create a new clipper.
            CreateSurfaces(); // Call the function that creates the surface objects.
        }




        public void Draw(Control window, Point Location, Size WindowSize)
        {
            // This function draws from the source surface to the surfaces
            // located in the MDI child windows.        
            
            Rectangle destination = new Rectangle(Location, WindowSize);

            if (this.WindowState == FormWindowState.Minimized)
                return;

            try
            {
                // Try and blit the offscreen surface on to the primary surface.
                clip.Window = window;   // The clipper will use the main window handle.
                primary.Draw(destination, offscreen, DrawFlags.Wait);
            }
            catch(SurfaceLostException)
            {
                // The surface can be lost if power saving
                // mode kicks in, or any other number of
                // reasons.
                CreateSurfaces(); // Surface was lost. Recreate them.
            }
        }




        /// <summary>
        /// This function is where the surfaces and
        /// clipper object are created.
        /// </summary>
        private void CreateSurfaces()
        {
            SurfaceDescription description = new SurfaceDescription(); // Create a new SurfaceDescription struct.
            
            description.SurfaceCaps.PrimarySurface = true;
            
            // If any object currently exist, destroy them.
            if (null != offscreen)
                offscreen.Dispose();
            if (null != primary)
                primary.Dispose();
                    
            primary = new Surface(description, draw); // Create the primary surface.
            clip.Window = this; // The clipper will use the main window handle.
            primary.Clipper = clip; // Assign this clipper to the primary surface.
            description.Clear();
            
            offscreen = new Surface(DXUtil.SdkMediaPath + "\\earthenvmap.bmp", description, draw); // Create the surface using the specified file.
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
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.FileMenu = new System.Windows.Forms.MenuItem();
            this.OpenWindowMenuItem = new System.Windows.Forms.MenuItem();
            this.ExitMenuItem = new System.Windows.Forms.MenuItem();
            this.WindowMenu = new System.Windows.Forms.MenuItem();
            this.TileHorizontalMenuItem = new System.Windows.Forms.MenuItem();
            this.TileVerticalMenuItem = new System.Windows.Forms.MenuItem();
            this.CascadeMenuItem = new System.Windows.Forms.MenuItem();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.FileMenu,
                                                                                      this.WindowMenu});
            // 
            // FileMenu
            // 
            this.FileMenu.Index = 0;
            this.FileMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                     this.OpenWindowMenuItem,
                                                                                     this.ExitMenuItem});
            this.FileMenu.Text = "&File";
            // 
            // OpenWindowMenuItem
            // 
            this.OpenWindowMenuItem.Index = 0;
            this.OpenWindowMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
            this.OpenWindowMenuItem.Text = "&Open Window";
            this.OpenWindowMenuItem.Click += new System.EventHandler(this.OpenWindowMenuItem_Click);
            // 
            // ExitMenuItem
            // 
            this.ExitMenuItem.Index = 1;
            this.ExitMenuItem.Text = "E&xit";
            this.ExitMenuItem.Click += new System.EventHandler(this.ExitMenuItem_Click);
            // 
            // WindowMenu
            // 
            this.WindowMenu.Index = 1;
            this.WindowMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.TileHorizontalMenuItem,
                                                                                       this.TileVerticalMenuItem,
                                                                                       this.CascadeMenuItem});
            this.WindowMenu.Text = "&Window";
            // 
            // TileHorizontalMenuItem
            // 
            this.TileHorizontalMenuItem.Index = 0;
            this.TileHorizontalMenuItem.Text = "Tile &Horizontal";
            this.TileHorizontalMenuItem.Click += new System.EventHandler(this.TileHorizontal_Click);
            // 
            // TileVerticalMenuItem
            // 
            this.TileVerticalMenuItem.Index = 1;
            this.TileVerticalMenuItem.Text = "Tile &Vertical";
            this.TileVerticalMenuItem.Click += new System.EventHandler(this.TileVertical_Click);
            // 
            // CascadeMenuItem
            // 
            this.CascadeMenuItem.Index = 2;
            this.CascadeMenuItem.Text = "&Cascade";
            this.CascadeMenuItem.Click += new System.EventHandler(this.Cascade_Click);
            // 
            // MainWindow
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(744, 612);
            this.IsMdiContainer = true;
            this.Menu = this.mainMenu1;
            this.Name = "MainWindow";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
            this.Text = "MDIDraw";

        }
        #endregion




        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {
            Application.Run(new MainWindow());
        }




        private void OpenWindowMenuItem_Click(object sender, System.EventArgs e)
        {
            DrawWindow NewDrawWindow = new DrawWindow();
            NewDrawWindow.MdiParent = this;
            NewDrawWindow.Show();
        }




        private void ExitMenuItem_Click(object sender, System.EventArgs e)
        {
            this.Close();
        }




        private void TileHorizontal_Click(object sender, System.EventArgs e)
        {           
            this.LayoutMdi(MdiLayout.TileHorizontal);
        }




        private void TileVertical_Click(object sender, System.EventArgs e)
        {
            this.LayoutMdi(MdiLayout.TileVertical);
        }




        private void Cascade_Click(object sender, System.EventArgs e)
        {
            this.LayoutMdi(MdiLayout.Cascade);
        }
    }
}
