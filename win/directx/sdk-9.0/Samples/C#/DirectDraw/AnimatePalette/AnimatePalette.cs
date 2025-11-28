//-----------------------------------------------------------------------------
// File: AnimatePalette.cs
//
// Desc: AnimatePalette demonstrates DirectDraw palette animation when in full-screen 
//       on a palettized surface. 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace AnimatePalette
{
    // This structure describes the sprite the app will use.
    public struct Sprite
    {
        public float posX; 
        public float posY;
        public float velX; 
        public float velY;
    };

    public class AnimatePalette : System.Windows.Forms.Form
    {
        // Declare the class scope variables here.

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        const int indexBlack = 144; // Index of the palette entry that contains black.
        const int indexWhite = 0; // Index of the palette entry that contains white.
        const int spriteDiameter = 48; // Sprites width & height.
        const int numSprites = 25; // Number of sprites to draw.
        const int screenWidth = 640; // Screen width.
        const int screenHeight = 480; // Screen height.
        Device displayDevice = null; // The Direct Draw Device.
        Surface front = null; // The front surface.
        Surface back = null; // The back surface.
        Surface surfaceLogo = null; // The surface that contains the sprite bitmap.
        Surface text = null; // The text to draw on the screen.
        Palette pal = null; // The palette the front surface will use.
        int tickLast = 0; // Holds the value of the last call to GetTick.
        bool restoreNeeded = false;// Flag to determine the app needs to restore the surfaces.
        Sprite[] sprite = new Sprite[numSprites]; // Array of sprite structs.
        PaletteEntry[] pe = new PaletteEntry[256]; // Palette entry array.
        string bitmapFileName = DXUtil.SdkMediaPath + "directx.bmp"; // Bitmap to load.
        string textHelp = "Press Escape to quit."; // Text to display.

        /// <summary>
        /// Entry point for the application.
        /// </summary>
        public static void Main()
        {
            AnimatePalette app = new AnimatePalette();
            Application.Exit();
        }

        /// <summary>
        /// Constructor for the main class.
        /// </summary>
        public AnimatePalette()
        {
            //
            // Required for Windows Form Designer support.
            //
            InitializeComponent();
            
            // Make sure to use the arrow cursor.
            Cursor = Cursors.Arrow;

            // Create a new DrawDevice.
            InitializeDirectDraw();

            // Keep looping until app quits.
            while (Created)
            {               
                ProcessNextFrame();     // Process and draw the next frame.
                Application.DoEvents(); // Make sure the app has time to process messages.
            }
        }
        /// <summary>
        /// Restore all the surfaces, and redraw the sprite surfaces.
        /// </summary>
        void RestoreSurfaces()
        {
            SurfaceDescription description = new SurfaceDescription();

            displayDevice.RestoreAllSurfaces();

            // No need to re-create the surface, just re-draw it.
            text.ColorFill(9);
            text.DrawText(0, 0, textHelp, false);

            // We need to release and re-load, and set the palette again to 
            // redraw the bitmap on the surface.  Otherwise, GDI will not 
            // draw the bitmap on the surface with the correct palette.
            pal.Dispose();
            pal = null;
            pal = new Palette(displayDevice, bitmapFileName);
            front.Palette = pal;

            // Release and re-load the sprite bitmap.
            surfaceLogo.Dispose();
            surfaceLogo = new Surface(bitmapFileName, description, displayDevice);
            ColorKey ck = new ColorKey();
            
            surfaceLogo.SetColorKey(ColorKeyFlags.SourceDraw, ck);

            return;
        }

        /// <summary>
        /// Move the sprite around and make it bounce based on how much time 
        /// has passed
        /// </summary>
        private void UpdateSprite(ref Sprite s, float TimeDelta)
        {    
            // Update the sprite position
            s.posX += s.velX * TimeDelta;
            s.posY += s.velY * TimeDelta;

            // Clip the position, and bounce if it hits the edge.
            if (s.posX < 0.0f)
            {
                s.posX  = 0;
                s.velX = -s.velX;
            }

            if (s.posX >= screenWidth - spriteDiameter)
            {
                s.posX = screenWidth - 1 - spriteDiameter;
                s.velX = -s.velX;
            }

            if (s.posY < 0)
            {
                s.posY = 0;
                s.velY = -s.velY;
            }

            if (s.posY > screenHeight - spriteDiameter)
            {
                s.posY = screenHeight - 1 - spriteDiameter;
                s.velY = -s.velY;
            }   
        }
        /// <summary>
        /// Cycle the palette colors 2 through 37 - this 
        /// will change the color for 'X' in the sprite.
        /// </summary>
        private void CyclePalette()
        {
            for (int i = 2; i < 37; i++)
            {
                // This just does something interesting but simple by shifting each 
                // color component independently .
                pe[i].Blue  += 8;
                pe[i].Red   += 4;
                pe[i].Green += 2;
            }

            // Wait until the screen is synchronzied at a vertical-blank interval.
            // This may fail if the device is not hardware accelerated.
            try
            {
                displayDevice.WaitForVerticalBlank(WaitVbFlags.BlockBegin);
            }
            catch(DirectXException){}

            // Now that we are synchronzied at a vertical-blank 
            // interval, update the palette.
            pal.SetEntries(0, pe);

            return;
        }

        /// <summary>
        /// Move the sprites, blt them to the back buffer, then 
        /// flips the back buffer to the primary buffer
        /// </summary>
        private void ProcessNextFrame()
        {
            // Figure how much time has passed since the last time.
            int CurrTick = Environment.TickCount;
            int TickDiff = CurrTick - tickLast;

            // Don't update if no time has passed.
            if (0 == TickDiff)
                return; 

            tickLast = CurrTick;

            // Move the sprites according to how much time has passed.
            for (int i = 0; i < numSprites; i++)
                UpdateSprite(ref sprite[i], TickDiff / 1000.0f);

            // Cycle the palette every frame.
            CyclePalette(); 
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
                restoreNeeded = true;
                return;
            }

            if (true == restoreNeeded)
            {
                restoreNeeded = false;
                // The surfaces were lost so restore them .
                RestoreSurfaces();
            }

            // Fill the back buffer with black.
            back.ColorFill(indexBlack);

            // Blt the help text on the backbuffer.
            back.DrawFast(10, 10, text, DrawFastFlags.DoNotWait);

            // Blt all the sprites onto the back buffer using color keying,
            // ignoring errors until the flip. Note that all of these sprites 
            // use the same DirectDraw surface.
            for (int i = 0; i < numSprites; i++)
            {
                back.DrawFast((int)sprite[i].posX, (int)sprite[i].posY, surfaceLogo, DrawFastFlags.DoNotWait | DrawFastFlags.SourceColorKey);
            }

            // We are in fullscreen mode, so perform a flip.
            front.Flip(back, FlipFlags.DoNotWait);
        }

        /// <summary>
        /// Initializes DirectDraw and all the surfaces to be used.
        /// </summary>
        private void InitializeDirectDraw()
        {
            SurfaceDescription description = new SurfaceDescription(); // Describes a surface.
            Random randomNumber = new Random(); // Random number for position and velocity.
            ColorKeyFlags flags = new ColorKeyFlags();

            displayDevice = new Device(); // Create a new DirectDrawDevice.
            displayDevice.SetCooperativeLevel(this, CooperativeLevelFlags.FullscreenExclusive); // Set the cooperative level.

            try
            {
                displayDevice.SetDisplayMode(screenWidth, screenHeight, 8, 0, false); // Set the display mode width and height, and 8 bit color depth.
            }
            catch(UnsupportedException)
            {
                MessageBox.Show("Device doesn't support required mode. Sample will now exit.", "UnsupportedException caught");
                Close();
                return;
            }
            
            description.SurfaceCaps.PrimarySurface = description.SurfaceCaps.Flip = description.SurfaceCaps.Complex = true;         
            description.BackBufferCount = 1; // Create 1 backbuffer.

            front = new Surface(description, displayDevice); // Create the surface using the description above.

            SurfaceCaps caps = new SurfaceCaps();
            caps.BackBuffer = true; // Caps of the surface.
            back = front.GetAttachedSurface(caps); // Get the attached surface that matches the caps, which will be the backbuffer.

            pal = new Palette(displayDevice, bitmapFileName); // Create a new palette, using the bitmap file.
            pe = pal.GetEntries(0, 256); // Store the palette entries.
            front.Palette = pal; // The front surface will use this palette.
            
            description.Clear(); // Clear the data out from the SurfaceDescription class.
            surfaceLogo = new Surface(bitmapFileName, description, displayDevice); // Create the sprite bitmap surface.
            
            ColorKey key = new ColorKey(); // Create a new colorkey.
            key.ColorSpaceHighValue = key.ColorSpaceLowValue = 0;
            flags = ColorKeyFlags.SourceDraw;
            surfaceLogo.SetColorKey(flags, key); // Set the colorkey to the bitmap surface. 144 is the index used for the colorkey, which is black in this palette.

            description.Clear();
            description.SurfaceCaps.OffScreenPlain = true;
            description.Width = 200; // Set the width and height of the surface.
            description.Height = 20;
            
            text = new Surface(description, displayDevice); // Create the surface using the above description.
            text.ForeColor = Color.FromArgb(indexWhite); // Because this is a palettized environment, the app needs to use index based colors, which
                                                         // do not correspond to pre-defined color values. For this reason, the indexWhite variable exists
                                                         // and contains the index where the color white is stored in the bitmap's palette. The app makes use
                                                         // of the static FromArgb() method to convert the index to a Color value.
            text.ColorFill(indexBlack);
            text.DrawText(0, 0, textHelp, false);

            for (int i=0; i < numSprites; i++)
            {
                // Set the sprite's position and velocity
                sprite[i].posX = (randomNumber.Next()  % screenWidth);
                sprite[i].posY = (randomNumber.Next() % screenHeight); 

                sprite[i].velX = 500.0f * randomNumber.Next() / Int32.MaxValue - 250.0f;
                sprite[i].velY = 500.0f * randomNumber.Next() / Int32.MaxValue - 250.0f;
            }
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                displayDevice.RestoreDisplayMode(); // Restore the display mode to what it was.
                displayDevice.SetCooperativeLevel(this, CooperativeLevelFlags.Normal); // Set the cooperative level back to windowed.

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
            // AnimatePalette
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(208, 69);
            this.ForeColor = System.Drawing.Color.Black;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "AnimatePalette";
            this.Text = "AnimatePalette";
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.AnimatePalette_KeyUp);

        }
        #endregion

        /// <summary>
        /// Processes key up events.
        /// </summary>
        private void AnimatePalette_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            // If the user presses the Escape key, the app needs to exit.
            if (Keys.Escape == e.KeyCode)
                Close();
        }
    }
}
