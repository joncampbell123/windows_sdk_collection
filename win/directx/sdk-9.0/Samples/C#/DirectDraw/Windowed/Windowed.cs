//-----------------------------------------------------------------------------
// File: Windowed.cs
//
// Desc: WindowedMode demonstrates the tasks required to initialize and run 
//       a windowed DirectDraw application.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;




namespace Windowed
{
    public class Windowed : System.Windows.Forms.Form
    {
        private System.ComponentModel.Container components = null;
        
        private Device draw = null; // Holds the DrawDevice object.
        private Surface primary = null; // Holds the primary destination surface.
        private Surface offscreen = null; // Holds the offscreen surface that the bitmap will be loaded on.
        private Clipper clip = null; // Holds the clipper object.
        private Rectangle destination = new Rectangle();




        public Windowed()
        {
            // Required for Windows Form Designer support
            InitializeComponent();

            draw = new Device(); // Create a new DrawDevice, using the default device.
            draw.SetCooperativeLevel(this, CooperativeLevelFlags.Normal); // Set the coop level to normal windowed mode.
            CreateSurfaces(); // Call the function that creates the surface objects.
            Width = 295;            
        }




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
        private void InitializeComponent()
        {
            // 
            // Windowed
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Name = "Windowed";
            this.Text = "Windowed";
            this.Resize += new System.EventHandler(this.Windowed_SizeChanged);
            this.SizeChanged += new System.EventHandler(this.Windowed_SizeChanged);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Windowed_Paint);

        }
        #endregion




        static void Main() 
        {
            Application.Run(new Windowed());
        }




        private void Windowed_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            Draw();
        }
        
        
        

        private void Windowed_SizeChanged(object sender, System.EventArgs e)
        {
            Draw();
        }
        
        
        

        /// <summary>
        /// This function Draws the offscreen bitmap surface to the primary visible surface.
        /// </summary>
        private void Draw()
        {
            if (null == primary)
                return;
            
            if (FormWindowState.Minimized == WindowState)
                return;

            Height = Height < 50 ? 50 : Height; // Make sure the height is always valid.
            
            // Get the new client size to Draw to.
            destination = new Rectangle(PointToScreen(new Point(0,0)), ClientSize);

            try
            {
                // Try and Draw the offscreen surface on to the primary surface.
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

            description.SurfaceCaps.PrimarySurface = true; // The caps for this surface is simply primary.

            primary = new Surface(description, draw); // Create the primary surface.
            clip = new Clipper(draw); // Create a new clipper.
            clip.Window = this; // The clipper will use the main window handle.
            primary.Clipper = clip; // Assign this clipper to the primary surface.

            description.Clear(); // Clear the SurfaceDescription struct.
            offscreen = new Surface(DXUtil.SdkMediaPath + "\\earthenvmap.bmp", description, draw); // Create the surface using the specified file.
        }
    }
}
