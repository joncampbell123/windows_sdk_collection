//-----------------------------------------------------------------------------
// File: SpriteAnimate.cs
//
// Desc: SpriteAnimate demonstrates a simple technique to animate DirectDraw surfaces. 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpriteAnimate
{
    struct SpriteStruct
    {
        public float    speedRotation;
        public float    tickRotation;
        public int      frame;
        public bool     isClockwise;
        public float    posX; 
        public float    posY;
        public float    velX; 
        public float    velY;
    };

    public class SpriteAnimate : System.Windows.Forms.Form
    {
        private System.ComponentModel.Container components = null;

        // Declare the class scope variables here.
        const int diameterSprite = 32; // Sprites width & height.
        const int numSprites = 25; // Number of sprites to draw.
        const int widthScreen = 640; // Screen width.
        const int heightScreen = 480; // Screen height.
        const int numFrames = 30; // Number of animation frames.
        const int numRandom = 100; // Number of random values in the random array.
        const int maxRand = 0x7fff;

        Device draw = null; // The Direct Draw Device.
        Surface front = null; // The front surface.
        Surface back = null; // The back surface.
        Surface surfaceAnimation = null; // The surface that contains the sprite bitmap.
        int tickLast = 0; // Holds the value of the last call to GetTick.
        int randomIndex = 0; // Current random index.
        bool needRestore = false; // Flag to determine the app needs to restore the surfaces.
        SpriteStruct[] sprite = new SpriteStruct[numSprites]; // Array of sprite structs.
        PaletteEntry[] entry = new PaletteEntry[256]; // Palette entry array.
        Random randomNumber = new Random(Environment.TickCount); // Random number generator.      
        System.Drawing.Rectangle[] frame= new System.Drawing.Rectangle[numFrames]; // Holds the animation frame dimensions.
        int[] randomTable = new int[numRandom]; //Holds the table of random values.
        string nameFile = DXUtil.SdkMediaPath + "animate.bmp"; // Bitmap to load.

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {
            SpriteAnimate app = new SpriteAnimate();
            Application.Exit();
        }




        public SpriteAnimate()
        {
            // Required for Windows Form Designer support
            InitializeComponent();

            // Create a new DrawDevice.
            InitDirectDraw();

            // Keep looping until told to quit.
            while (Created)
            {               
                ProcessNextFrame();     // Process and draw the next frame.
                Application.DoEvents(); // Make sure the app has time to process messages.
            }
        }
        
        
        

        protected override void Dispose(bool disposing)
        {
            /// <summary>
            /// Clean up any resources being used.
            /// </summary>

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
            // SpriteAnimate
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 273);
            this.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "SpriteAnimate";
            this.Text = "SpriteAnimate";
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.SpriteAnimate_KeyUp);

        }
        #endregion




        /// <summary>
        /// Gets the next element from the circular array of random values
        /// </summary>
        int GetNextRand()
        {    
            int randomNext = randomTable[ randomIndex++ ];
            randomIndex %= numRandom;

            return randomNext;
        }




        /// <summary>
        /// Draw the sprites and text to the screen.
        /// </summary>
        private void DisplayFrame()
        {
            if (null == front)
                return;

            if (false == draw.TestCooperativeLevel())
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

            // Draw all the sprites onto the back buffer using color keying,
            // ignoring errors until the flip. Note that all of these sprites 
            // use the same DirectDraw surface.
            try
            {
                for (int i = 0; i < numSprites; i++)
                {
                    back.DrawFast((int)sprite[i].posX, (int)sprite[i].posY, surfaceAnimation, frame[ sprite[i].frame ], DrawFastFlags.DoNotWait | DrawFastFlags.SourceColorKey);
                }
                back.DrawText(10, 30, "Press escape to exit", true);

                // We are in fullscreen mode, so perform a flip.
                front.Flip(back, FlipFlags.DoNotWait);
            }
            catch
            {
                // Catch any exceptions that might crop up.
            }
        }




        /// <summary>
        /// Move the sprite around and make it bounce based on how much time 
        /// has passed
        /// </summary>
        private void UpdateSprite(ref SpriteStruct s, float timeDelta)
        {    
            // Update the sprite position
            s.posX += s.velX * timeDelta;
            s.posY += s.velY * timeDelta;

            // See if its time to advance the sprite to the next frame
            s.tickRotation += timeDelta;
            if (s.tickRotation > s.speedRotation)
            {
                // If it is, then either change the frame clockwise or counter-clockwise
                if (s.isClockwise)
                {
                    s.frame++;
                    s.frame %= numFrames; 
                }
                else
                {
                    s.frame--;
                    if (s.frame < 0)
                        s.frame = numFrames - 1;
                }

                s.tickRotation = 0;        
            }

            // Using the next element from the random array, 
            // randomize the velocity of the sprite
            if (GetNextRand() % 100 < 2)
            {
                s.velX = ((500.0f * randomNumber.Next(0, maxRand)) / (float)maxRand) - 250.0f;
                s.velY = ((500.0f * randomNumber.Next(0, maxRand)) / (float)maxRand) - 250.0f;
            }

            // Using the next element from the random array, 
            // randomize the rotational speed of the sprite.
            if (GetNextRand() % 100 < 5)
                s.speedRotation = (GetNextRand() % 50 + 5) / 1000.0f;

            // Clip the position, and bounce if it hits the edge.
            if (s.posX < 0.0f)
            {
                s.posX  = 0;
                s.velX = -s.velX;
            }

            if (s.posX >= widthScreen - diameterSprite)
            {
                s.posX = widthScreen - 1 - diameterSprite;
                s.velX = -s.velX;
            }

            if (s.posY < 0)
            {
                s.posY = 0;
                s.velY = -s.velY;
            }

            if (s.posY > heightScreen - diameterSprite)
            {
                s.posY = heightScreen - 1 - diameterSprite;
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
                UpdateSprite(ref sprite[i], tickDifference / 1000.0f);

            //Draw the sprites and text to the screen.
            DisplayFrame();
        }




        /// <summary>
        /// Initializes DirectDraw and all the surfaces to be used.
        /// </summary>
        private void InitDirectDraw()
        {
            SurfaceDescription description = new SurfaceDescription(); // Describes a surface.

            draw = new Device(); // Create a new DirectDrawDevice.
            draw.SetCooperativeLevel(this, CooperativeLevelFlags.FullscreenExclusive); // Set the cooperative level.
            draw.SetDisplayMode(widthScreen, heightScreen, 16, 0, false); // Set the display mode width and height, and 16 bit color depth.
            
            description.SurfaceCaps.PrimarySurface = description.SurfaceCaps.Flip = description.SurfaceCaps.Complex = true; // Capabilities of the surface.            
            description.BackBufferCount = 1; // Create 1 backbuffer.

            front = new Surface(description, draw); // Create the surface using the description above.

            SurfaceCaps caps = new SurfaceCaps();
            caps.BackBuffer = true; // Caps of the surface.
            back = front.GetAttachedSurface(caps); // Get the attached surface that matches the caps, which will be the backbuffer.            
            back.ForeColor = Color.White;

            description.Clear(); // Clear out the SurfaceDescription structure.
            surfaceAnimation = new Surface(nameFile, description, draw); // Create the sprite bitmap surface.
            
            ColorKey ck = new ColorKey(); // Create a new colorkey.
            surfaceAnimation.SetColorKey(ColorKeyFlags.SourceDraw, ck); // Set the colorkey to the bitmap surface. 0 is used for the colorkey, which is what the ColorKey struct is initialized to.

            for (int i=0; i < numSprites; i++)
            {
                // Set the sprite's position, velocity, frame, rotation direction, and rotation speed.
                sprite[i].posX = (randomNumber.Next(0, maxRand)  % widthScreen);
                sprite[i].posY = (randomNumber.Next(0, maxRand) % heightScreen); 
                sprite[i].velX = ((500.0f * randomNumber.Next(0, maxRand)) / (float)maxRand) - 250.0f;
                sprite[i].velY = ((500.0f * randomNumber.Next(0, maxRand)) / (float)maxRand) - 250.0f;
                sprite[i].frame = randomNumber.Next(0, maxRand) % numFrames;
                sprite[i].tickRotation = 0;
                sprite[i].speedRotation = (randomNumber.Next(0, maxRand) % 50 + 25) / 1000.0f; 
                sprite[i].isClockwise = (0 == randomNumber.Next(0, maxRand) % 2) ? true : false;
            }

            // Precompute the source rects for frame.  The source rects 
            // are used during the blt of the dds to the backbuffer. 
            for (int i = 0; i < numFrames; i++)
            {
                System.Drawing.Point p = new System.Drawing.Point((i % 5) * diameterSprite, (i / 5) * diameterSprite);
                frame[i] = new System.Drawing.Rectangle(p , new Size(diameterSprite, diameterSprite));
            }

            // Init an array of random values.  This array is used to create the
            // 'flocking' effect seen in the sample.
            randomIndex = 0;
            for (int randomNext = 0; randomNext < numRandom; randomNext++)
                randomTable[randomNext] = randomNumber.Next(0, maxRand); 
        }




        /// <summary>
        /// Restore all the surfaces, and redraw the sprite surfaces.
        /// </summary>
        void RestoreSurfaces()
        {
            SurfaceDescription description = new SurfaceDescription();

            draw.RestoreAllSurfaces();

            // Release and re-load the sprite bitmap.
            surfaceAnimation.Dispose();
            surfaceAnimation = null;
            surfaceAnimation = new Surface(nameFile, description, draw);
            ColorKey ck = new ColorKey();

            surfaceAnimation.SetColorKey(ColorKeyFlags.SourceDraw, ck);

            return;
        }




        /// <summary>
        /// Processes key up events.
        /// </summary>
        private void SpriteAnimate_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            // If the user presses the Escape key, the app needs to exit.
            if (Keys.Escape == e.KeyCode)
                Close();
        }
    }
}
