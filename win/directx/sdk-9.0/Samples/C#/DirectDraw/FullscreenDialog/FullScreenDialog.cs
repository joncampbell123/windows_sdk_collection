//-----------------------------------------------------------------------------
// File: FullScreenDialog.cs
//
// Desc: FullScreenDialog demonstrates how to display a GDI dialog while using DirectDraw
//       in full-screen exclusive mode. 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace FullScreenDialog
{
    public struct Sprite
    {
        // This structure describes the sprites the app will use.
        public float posX; 
        public float posY;
        public float velX; 
        public float velY;
    };

    public class FullScreenDialog : System.Windows.Forms.Form
    {
        // Declare the class scope variables here.
        private System.ComponentModel.Container components = null;

        static DialogSampleForm dialogSample;
        const int diameterSprite = 48; // Sprites width & height.
        const int numSprites = 25; // Number of sprites to draw.
        const int screenWidth = 640; // Screen width.
        const int screenHeight = 480; // Screen height.
        const string textHelp = "Press escape to quit. Press F1 to bring up the dialog."; // Text to display.
        Device displayDevice = null; // The Direct Draw Device.
        Surface front = null; // The front surface.
        Surface back = null; // The back surface.
        Surface surfaceLogo = null; // The surface that contains the sprites bitmap.
        Surface text = null; // The text to draw on the screen.
        int tickLast = 0; // Holds the value of the last call to GetTick.
        bool needRestore = false; // Flag to determine the app needs to restore the surfaces.
        Sprite[] sprites = new Sprite[numSprites]; // Array of sprites structs.
        Clipper clip = null; // Clipper to prevent the app from drawing over the top of the dialog.
        string nameFile = DXUtil.SdkMediaPath + "directx.bmp"; // Bitmap to load.
        
        /// <summary>
        /// Entry point for the application.
        /// </summary>
        public static void Main()
        {
            FullScreenDialog app = new FullScreenDialog();
            Application.Exit();
        }

        /// <summary>
        /// Constructor for the main class.
        /// </summary>
        public FullScreenDialog()
        {
            //
            // Required for Windows Form Designer support.
            //
            InitializeComponent();
            
            // Create a new DrawDevice.
            InitDirectDraw();

            dialogSample = new DialogSampleForm(); // Create a new dialog box to display.
            this.AddOwnedForm(dialogSample); // Add the dialog as a child to this form.
            dialogSample.TopMost = true; // Makes the window stay on top of everything else.
            dialogSample.Show(); // Display the dialog.

            // Keep looping until told to quit.
            while (Created)
            {               
                ProcessNextFrame();     // Process and draw the next frame.
                Application.DoEvents(); // Make sure the app has time to process messages.
            }
        }




        /// <summary>
        /// Restore all the surfaces, and redraw the sprites surfaces.
        /// </summary>
        void RestoreSurfaces()
        {
            SurfaceDescription description = new SurfaceDescription();

            displayDevice.RestoreAllSurfaces();

            // No need to re-create the surface, just re-draw it.
            text.ColorFill(Color.Black);
            text.DrawText(0, 0, textHelp, true);

            // Release and re-load the sprites bitmap.
            surfaceLogo.Dispose();
            surfaceLogo = null;
            surfaceLogo = new Surface(nameFile, description, displayDevice);
            ColorKey key = new ColorKey();
            surfaceLogo.SetColorKey(ColorKeyFlags.SourceDraw, key);

            return;
        }




        /// <summary>
        /// Move the sprites around and make it bounce based on how much time 
        /// has passed
        /// </summary>
        private void UpdateSprite(ref Sprite s, float deltaTime)
        {    
            // Update the sprites position
            s.posX += s.velX * deltaTime;
            s.posY += s.velY * deltaTime;

            // Clip the position, and bounce if it hits the edge.
            if (s.posX < 0.0f)
            {
                s.posX  = 0;
                s.velX = -s.velX;
            }

            if (s.posX >= screenWidth - diameterSprite)
            {
                s.posX = screenWidth - 1 - diameterSprite;
                s.velX = -s.velX;
            }

            if (s.posY < 0)
            {
                s.posY = 0;
                s.velY = -s.velY;
            }

            if (s.posY > screenHeight - diameterSprite)
            {
                s.posY = screenHeight - 1 - diameterSprite;
                s.velY = -s.velY;
            }   
        }




        /// <summary>
        /// Move the sprites, blt them to the back buffer, then 
        /// flips the back buffer to the primary buffer
        /// </summary>
        private void ProcessNextFrame()
        {
            // Figure how much time has passed since the last time.
            int tickCurrent = Environment.TickCount;
            int tickDifference = tickCurrent - tickLast;

            // Don't update if no time has passed.
            if (0 == tickDifference)
                return; 

            tickLast = tickCurrent;

            // Move the sprites according to how much time has passed.
            for (int i = 0; i < numSprites; i++)
                UpdateSprite(ref sprites[i], tickDifference / 1000.0f);

            //Draw the sprites and text to the screen.
            DisplayFrame();
        }




        /// <summary>
        /// Draw the sprites and text to the screen.
        /// </summary>
        private void DisplayFrame()
        {
            if (null == front)
                return;

            if (false == displayDevice.TestCooperativeLevel())
            {
                needRestore = true;
                return;
            }

            if (true == needRestore)
            {
                needRestore = false;
                // The surfaces were lost so restore them .
                RestoreSurfaces();
            }

            // Fill the back buffer with black.
            back.ColorFill(0);

            // Draw the help text on the backbuffer.
            back.DrawFast(10, 10, text, DrawFastFlags.DoNotWait);

            // Draw all the sprites onto the back buffer using color keying,
            // ignoring errors until the flip. Note that all of these sprites 
            // use the same DirectDraw surface.
            for (int i = 0; i < numSprites; i++)
            {
                back.DrawFast((int)sprites[i].posX, (int)sprites[i].posY, surfaceLogo, DrawFastFlags.DoNotWait | DrawFastFlags.SourceColorKey);
            }            
            front.Draw(back, DrawFlags.DoNotWait);
        }




        /// <summary>
        /// Initializes DirectDraw and all the surfaces to be used.
        /// </summary>
        private void InitDirectDraw()
        {
            SurfaceDescription description = new SurfaceDescription(); // Describes a surface.
            Random randomNumber = new Random(); // Random number for position and velocity.

            displayDevice = new Device(); // Create a new DirectDrawDevice.            
            displayDevice.SetCooperativeLevel(this, CooperativeLevelFlags.FullscreenExclusive); // Set the cooperative level.
            displayDevice.SetDisplayMode(screenWidth, screenHeight, 16, 0, false); // Set the display mode width and height, and 8 bit color depth.
            
            description.SurfaceCaps.Flip = description.SurfaceCaps.Complex = description.SurfaceCaps.PrimarySurface = true;
            description.BackBufferCount = 1; // Create 1 backbuffer.
            front = new Surface(description, displayDevice); // Create the surface using the description above.

            SurfaceCaps caps = new SurfaceCaps();
            caps.BackBuffer = true; // Caps of the surface.
            back = front.GetAttachedSurface(caps); // Get the attached surface that matches the caps, which will be the backbuffer.
            
            clip = new Clipper(displayDevice); // Create a clipper object.
            clip.Window = this; // Set the clippers window handle to the window handle of the main window.
            front.Clipper = clip; // Tell the front surface to use the clipper object.

            description.Clear(); // Clear out the SurfaceDescription structure.
            surfaceLogo = new Surface(nameFile, description, displayDevice); // Create the sprites bitmap surface.
            
            description.Clear();

            description.Width = 400; // Set the width and height of the surface.
            description.Height = 20;
            description.SurfaceCaps.OffScreenPlain = true;

            text = new Surface(description, displayDevice); // Create the surface using the above description.
            text.ColorFill(Color.Black);
            text.ForeColor = Color.White;
            text.DrawText(0, 0, textHelp, true);

            ColorKey ck = new ColorKey(); // Create a new colorkey.
            surfaceLogo.SetColorKey(ColorKeyFlags.SourceDraw, ck); // Set the colorkey to the bitmap surface. 0 is used for the colorkey, which is what the ColorKey struct is initialized to.

            for (int i=0; i < numSprites; i++)
            {
                // Set the sprites's position and velocity
                sprites[i].posX = (randomNumber.Next()  % screenWidth);
                sprites[i].posY = (randomNumber.Next() % screenHeight); 

                sprites[i].velX = 500.0f * randomNumber.Next() / Int32.MaxValue - 250.0f;
                sprites[i].velY = 500.0f * randomNumber.Next() / Int32.MaxValue - 250.0f;
            }           
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
            // FullScreenDialog
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(208, 69);
            this.ForeColor = System.Drawing.Color.Black;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "FullScreenDialog";
            this.Text = "FullScreenDialog";
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.FullScreenDialog_KeyUp);

        }
        #endregion




        /// <summary>
        /// Processes key up events.
        /// </summary>
        private void FullScreenDialog_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if (Keys.Escape == e.KeyCode)
                Close();
            else if (Keys.F1 == e.KeyCode && dialogSample.IsDisposed)
            {
                dialogSample = new DialogSampleForm(); // Create a new dialog window.
                this.AddOwnedForm(dialogSample); // Add the dialog as a child to this form.
                dialogSample.TopMost = true; // Makes the window stay on top of everything else.
                dialogSample.Show(); // Display the dialog.
            }
        }
    }
}
