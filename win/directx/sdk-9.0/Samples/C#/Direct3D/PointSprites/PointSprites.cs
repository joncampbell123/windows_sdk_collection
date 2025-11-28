//-----------------------------------------------------------------------------
// File: PointSprites.cs
//
// Desc: Example code showing how to do vertex shaders in D3D.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace PointSprites
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
        /// <summary>
        /// Custom vertex types
        /// </summary>
		public struct ColorVertex
		{
			public Vector3 v;
			public int color;
			public float tu, tv;
			public static readonly VertexFormats Format =  VertexFormats.Position | VertexFormats.Diffuse | VertexFormats.Texture1;
		};




        /// <summary>
        /// Global structs and data for the ground object
        /// </summary>
		public const float GroundWidth = 256.0f;
		public const float GroundHeight = 256.0f;
		public const int GroundGridSize = 8;
		public const int GroundTile = 32;
		public const uint GroundColor = 0xcccccccc;

		public enum ParticleColors { White, Red, Green, Blue, NumColors};

		public System.Drawing.Color[] g_clrColor =
		{
			System.Drawing.Color.White,
			System.Drawing.Color.Firebrick,
			System.Drawing.Color.MediumSeaGreen,
			System.Drawing.Color.DodgerBlue
		};

		System.Drawing.Color[] g_clrColorFade =
		{
			System.Drawing.Color.WhiteSmoke,
			System.Drawing.Color.Pink,
			System.Drawing.Color.LightSeaGreen,
			System.Drawing.Color.LightCyan
		};

		private GraphicsFont drawingFont = null;                 // Font for drawing text
		private GraphicsFont drawingFontSmall = null;                 // Font for drawing text

		// Ground stuff
		Texture groundTexture = null;
		Plane planeGround;

		VertexBuffer groundVertexBuffer = null;
		IndexBuffer groundIndexBuffer = null;
		int numGroundIndices = 0;
		int numGroundVertices = 0;

		// Particle stuff
		Texture particleTexture = null;
		ParticleSystem particleSystem = null;
		int numberParticlesToEmit = 0;
		ParticleColors particleColor = ParticleColors.White;
		bool animateEmitter = false;

		private byte [] keyValues = new byte[256];
		bool canDrawReflection = false;
		bool canDoALphaBlend = false;
		bool shouldDrawHelp = false;
		bool animateColor = false;

		// Variables for determining view position
		Vector3 position;
		Vector3 velocity;
		float yaw = 0.0f;
		float yawVelocity = 0.0f;
		float pitch = 0.0f;
		float pitchVelocity = 0.0f;
		Matrix viewMatrix;
		Matrix orientationMatrix;


		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "PointSprites: Using particle effects";
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

			drawingFont = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			drawingFontSmall = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold, 9);
			enumerationSettings.AppUsesDepthBuffer = true;

			particleSystem      = new ParticleSystem(512, 2048, 0.03f);
			numberParticlesToEmit = 10;
			animateEmitter      = false;

			numGroundVertices  = (GroundGridSize + 1) * (GroundGridSize + 1);
			numGroundIndices   = (GroundGridSize * GroundGridSize) * 6;
			planeGround          = new Plane(0.0f, 1.0f, 0.0f, 0.0f);

			position      = new Vector3(0.0f, 3.0f,-4.0f);
			velocity      = new Vector3(0.0f, 0.0f, 0.0f);
			yaw           = 0.03f;
			yawVelocity   = 0.0f;
			pitch         = 0.5f;
			pitchVelocity = 0.0f;
			viewMatrix = Matrix.Translation(0.0f, 0.0f, 10.0f);
			orientationMatrix = Matrix.Translation(0.0f, 0.0f, 0.0f);

			// Set up our event handlers
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyDown);
			this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyUp);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Slow things down for the REF device
			if (Caps.DeviceType == DeviceType.Reference)
				elapsedTime = 0.05f;

			// Determine emitter position
			Vector3 vEmitterPostion;
			if (animateEmitter)
				vEmitterPostion = new Vector3(3*(float)Math.Sin(appTime), 0.0f, 3*(float)Math.Cos(appTime));
			else
				vEmitterPostion = new Vector3(0.0f, 0.0f, 0.0f);

			if (animateColor)
			{
				if (++particleColor == ParticleColors.NumColors)
					particleColor = ParticleColors.White;
			}
			// Update particle system
			particleSystem.Update(elapsedTime, numberParticlesToEmit,
				g_clrColor[(int)particleColor],
				g_clrColorFade[(int)particleColor], 8.0f,
				vEmitterPostion);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Update the camera here rather than in FrameMove() so you can
			// move the camera even when the scene is paused
			UpdateCamera();

			device.BeginScene();

			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, 0x00000000, 1.0f, 0);

			// Draw reflection of particles
			if (canDrawReflection)
			{
				Matrix matReflectedView = new Matrix();
				matReflectedView.Reflect(planeGround);
				matReflectedView *= viewMatrix;        
    
				device.RenderState.ZBufferWriteEnable = false;
				device.RenderState.AlphaBlendEnable = true;
				device.RenderState.SourceBlend = Blend.One;
				device.RenderState.DestinationBlend = Blend.One;

				device.Transform.View = matReflectedView;
				device.SetTexture(0, particleTexture);
				particleSystem.Render(device);

				device.RenderState.ZBufferWriteEnable = true;
				device.RenderState.AlphaBlendEnable = false;
			}


			// Draw the ground
			if (canDrawReflection)
			{
				device.RenderState.AlphaBlendEnable = true;
				device.RenderState.SourceBlend = Blend.SourceAlpha;
				device.RenderState.DestinationBlend = Blend.InvSourceAlpha;
			}

			device.Transform.View = viewMatrix;
			device.SetTexture(0, groundTexture);
			device.VertexFormat = ColorVertex.Format;
			device.SetStreamSource(0, groundVertexBuffer, 0);
			device.Indices = groundIndexBuffer;
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numGroundVertices, 0, numGroundIndices/3);


			// Draw particles
			device.RenderState.ZBufferWriteEnable = false;
			device.RenderState.AlphaBlendEnable = true;
			device.RenderState.SourceBlend = Blend.One;
			device.RenderState.DestinationBlend = Blend.One;

			device.SetTexture(0, particleTexture);
			particleSystem.Render(device);

			device.RenderState.ZBufferWriteEnable = true;
			device.RenderState.AlphaBlendEnable = false;

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);
			if (shouldDrawHelp)
			{
				drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White,
					"Keyboard controls:");
				drawingFontSmall.DrawText(20, 60, System.Drawing.Color.White,
					"Move\nTurn\nPitch\nSlide\n\nHelp\nChange device\nAnimate emitter\nChange color\nCycle Colors\n\nToggle reflection\nExit");
				drawingFontSmall.DrawText(210, 60, System.Drawing.Color.White,
					"W,S\nE,Q\nA,Z\nArrow keys\n\nF1\nF2\nF3\nF4\nC\n\nR\nEsc");
			}
			else
			{
				drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White,
					"Press F1 for help");
			}

			device.EndScene();
		}




        /// <summary>
        /// Update camera
        /// </summary>
		void UpdateCamera()
		{
			float fElapsedTime;

			if (elapsedTime > 0.0f)
				fElapsedTime = elapsedTime;
			else
				fElapsedTime = 0.05f;

			float fSpeed        = 3.0f*fElapsedTime;
			float fAngularSpeed = 1.0f*fElapsedTime;

			// De-accelerate the camera movement (for smooth motion)
			velocity      *= 0.9f;
			yawVelocity   *= 0.9f;
			pitchVelocity *= 0.9f;

			// Process keyboard input
			if (keyValues[(int)Keys.Left] != 0 || keyValues[(int)Keys.NumPad1] != 0)                 velocity.X -= fSpeed; // Slide Left
			if (keyValues[(int)Keys.Right] != 0 || keyValues[(int)Keys.NumPad3]  != 0)                velocity.X += fSpeed; // Slide Right
			if (keyValues[(int)Keys.Down]  != 0)                                       velocity.Y -= fSpeed; // Slide Down
			if (keyValues[(int)Keys.Up]  != 0)                                         velocity.Y += fSpeed; // Slide Up
			if (keyValues['W'] != 0)                                           velocity.Z += fSpeed; // Move Forward
			if (keyValues['S'] != 0)                                           velocity.Z -= fSpeed; // Move Backward
			if (keyValues['A'] != 0 || keyValues[(int)Keys.NumPad8] != 0)                     pitchVelocity -= fSpeed; // Pitch Down
			if (keyValues['Z'] != 0 || keyValues[(int)Keys.NumPad2] != 0)                     pitchVelocity += fSpeed; // Pitch Up
			if (keyValues['E'] != 0 || keyValues[(int)Keys.NumPad6] != 0)                     yawVelocity += fSpeed; // Turn Right
			if (keyValues['Q'] != 0 || keyValues[(int)Keys.NumPad4] != 0)                     yawVelocity -= fSpeed; // Turn Left
			if (keyValues[(int)Keys.Add] != 0)                                    if (numberParticlesToEmit < 10) numberParticlesToEmit++;
			if (keyValues[(int)Keys.Subtract] != 0)                                    if (numberParticlesToEmit > 0)  numberParticlesToEmit--;

			// Update the position vector
			Vector3 vT = velocity * fSpeed;
			vT = Vector3.TransformNormal(vT, orientationMatrix);
			position += vT;
			if (position.Y < 1.0f)
				position.Y = 1.0f;

			// Update the yaw-pitch-rotation vector
			yaw   += fAngularSpeed * yawVelocity;
			pitch += fAngularSpeed * pitchVelocity;
			if (pitch < 0.0f)      pitch = 0.0f;
			if (pitch > (float)Math.PI/2) pitch = (float)Math.PI/2;

			// Set the view matrix
			Quaternion qR  = Quaternion.RotationYawPitchRoll(yaw, pitch, 0.0f);
			orientationMatrix.AffineTransformation(1.25f, new Vector3(0,0,0), qR, position);
			viewMatrix = Matrix.Invert(orientationMatrix);
		}




		/// <summary>
        /// The device has been created.  Resources that are not lost on
        /// Reset() can be created here -- resources in Pool.Managed,
        /// Pool.Scratch, or Pool.SystemMemory.  Image surfaces created via
        /// CreateImageSurface are never lost and can be created here.  Vertex
        /// shaders and pixel shaders can also be created here as they are not
        /// lost on Reset().
		/// </summary>
		protected override void InitializeDeviceObjects()
		{
			drawingFont.InitializeDeviceObjects(device);
			drawingFontSmall.InitializeDeviceObjects(device);

			try
			{
				// Create textures
				groundTexture = GraphicsUtility.CreateTexture(device, "Ground2.bmp", Format.Unknown);
				particleTexture = GraphicsUtility.CreateTexture(device, "Particle.bmp", Format.Unknown);
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}


			// Check if we can do the reflection effect
			canDoALphaBlend = (Caps.SourceBlendCaps.SupportsSourceAlpha) && (Caps.DestinationBlendCaps.SupportsInverseSourceAlpha);

			if (canDoALphaBlend)
				canDrawReflection = true;

		}


        
        
        /// <summary>
        /// Handles the vertex buffer creatation for the ground
        /// </summary>
        private void CreateGround(object sender, EventArgs e)
		{

			VertexBuffer vb = (VertexBuffer)sender;
			// Fill vertex buffer
			ColorVertex[] vertices = (ColorVertex[])vb.Lock(0, 0);

			int count = 0;
			// Fill in vertices
			for (int zz = 0; zz <= GroundGridSize; zz++)
			{
				for (int xx = 0; xx <= GroundGridSize; xx++)
				{
					vertices[count].v.X   = GroundWidth * (xx/(float)GroundGridSize-0.5f);
					vertices[count].v.Y   = 0.0f;
					vertices[count].v.Z   = GroundHeight * (zz/(float)GroundGridSize-0.5f);
					vertices[count].color = unchecked((int)GroundColor);
					vertices[count].tu    = xx*GroundTile/(float)GroundGridSize;
					vertices[count].tv    = zz*GroundTile/(float)GroundGridSize;
					count++;
				}
			}

			vb.Unlock();
		}

        

        
        /// <summary>
        /// Handles the index buffer creatation for the ground
        /// </summary>
        private void CreateGroundIndex(object sender, EventArgs e)
		{

			IndexBuffer ib = (IndexBuffer)sender;
			// Fill the index buffer
			short[] indices =  (short[])ib.Lock(0, 0);

			int count = 0;
			// Fill in indices
			for (int z = 0; z < GroundGridSize; z++)
			{
				for (int x = 0; x < GroundGridSize; x++)
				{
					int vtx = x + z * (GroundGridSize+1);
					indices[count++] = (short)(vtx + 1);
					indices[count++] = (short)(vtx + 0);
					indices[count++] = (short)(vtx + 0 + (GroundGridSize+1));
					indices[count++] = (short)(vtx + 1);
					indices[count++] = (short)(vtx + 0 + (GroundGridSize+1));
					indices[count++] = (short)(vtx + 1 + (GroundGridSize+1));
				}
			}

			ib.Unlock();

		}




		/// <summary>
        /// The device exists, but may have just been Reset().  Resources in
        /// Pool.Default and any other device state that persists during
        /// rendering should be set here.  Render states, matrices, textures,
        /// etc., that don't change during rendering can be set once here to
        /// avoid redundant state setting during Render() or FrameMove().
		/// </summary>
		protected override void RestoreDeviceObjects(System.Object sender, System.EventArgs e)
		{
            // Create ground object

            // Create vertex buffer for ground object
            if (groundVertexBuffer == null)
            {
                groundVertexBuffer = new VertexBuffer(typeof(ColorVertex), numGroundVertices, device, Usage.WriteOnly, ColorVertex.Format, Pool.Default);
                groundVertexBuffer.Created += new System.EventHandler(this.CreateGround);
                this.CreateGround(groundVertexBuffer, null);
            }

            // Create the index buffer
            if (groundIndexBuffer == null)
            {
                groundIndexBuffer = new IndexBuffer(typeof(short), numGroundIndices, device, Usage.WriteOnly, Pool.Default);
                groundIndexBuffer.Created += new System.EventHandler(this.CreateGroundIndex);
                this.CreateGroundIndex(groundIndexBuffer, null);
            }

            // Set the world matrix
			device.Transform.World = Matrix.Identity;

			// Set projection matrix
			float fAspect = ((float)device.PresentationParameters.BackBufferWidth) / device.PresentationParameters.BackBufferHeight;
			
			device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 0.1f, 100.0f);

			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.TextureState[0].AlphaOperation = TextureOperation.Modulate;
			device.TextureState[1].ColorOperation = TextureOperation.Disable;
			device.TextureState[1].AlphaOperation = TextureOperation.Disable;

			device.RenderState.SourceBlend = Blend.One;
			device.RenderState.DestinationBlend = Blend.One;
			device.RenderState.Lighting = false;
			device.RenderState.CullMode = Cull.CounterClockwise;
			device.RenderState.ShadeMode = ShadeMode.Flat;

			// Initialize the particle system
			particleSystem.RestoreDeviceObjects(device);
		}




		/// <summary>
		/// Called when the app is exiting, or the device is being changed, this 
		/// function deletes any device-dependent objects.
		/// </summary>
		protected override void DeleteDeviceObjects(System.Object sender, System.EventArgs e)
		{
			groundTexture.Dispose();
			particleTexture.Dispose();
			groundVertexBuffer.Dispose();
			groundIndexBuffer.Dispose();

			groundTexture = null;
			particleTexture = null;

			groundVertexBuffer = null;
			groundIndexBuffer = null;
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// Need to support post-pixel processing (for alpha blending)
			if (!Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFormat,
				Usage.RenderTarget | Usage.QueryPostPixelShaderBlending, ResourceType.Surface,
				backBufferFormat))
			{
				return false;
			}
			// Make sure device can do ONE:ONE alphablending
			if (!caps.SourceBlendCaps.SupportsOne)
				return false;
			if (!caps.DestinationBlendCaps.SupportsOne)
				return false;

			// Make sure HW TnL devices can do point sprites
			if ((vertexProcessingType == VertexProcessingType.Hardware) ||
				(vertexProcessingType == VertexProcessingType.Mixed))
			{
				if (caps.MaxPointSize <= 1.0f)
					return false;
			}
			return true;
		}




		/// <summary>
		/// Event Handler for windows messages
		/// </summary>
		private void OnPrivateKeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			keyValues[(int)e.KeyCode] = 0;
			if (e.KeyCode == System.Windows.Forms.Keys.F1)
				shouldDrawHelp = !shouldDrawHelp;

			if (e.KeyCode == System.Windows.Forms.Keys.R)
				if (canDoALphaBlend)
					canDrawReflection = !canDrawReflection;

			if (e.KeyCode == System.Windows.Forms.Keys.C)
				animateColor = !animateColor;

			if (e.KeyCode == System.Windows.Forms.Keys.F3)
				animateEmitter = !animateEmitter;

			if (e.KeyCode == System.Windows.Forms.Keys.F4)
				if (++particleColor == ParticleColors.NumColors)
					particleColor = ParticleColors.White;

		}




        /// <summary>
        /// Event Handler for windows messages
        /// </summary>
        private void OnPrivateKeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			keyValues[(int)e.KeyCode] = 1;
		}



		
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {
            using (MyGraphicsSample d3dApp = new MyGraphicsSample())
            {                                 
                if (d3dApp.CreateGraphicsSample())
                    d3dApp.Run();
            }
        }
    }


    
    
    /// <summary>
	/// The particle system class
	/// </summary>
	public class ParticleSystem
	{
		public struct PointVertex
		{
			public Vector3 v;
			public int color;
			public static readonly VertexFormats Format =  VertexFormats.Position | VertexFormats.Diffuse;
		};




        /// <summary>
        /// Global data for the particles
        /// </summary>
		public struct Particle
		{
			public bool isSpark;     // Sparks are less energetic particles that
			// are generated where/when the main particles
			// hit the ground

			public Vector3 positionVector;       // Current position
			public Vector3 velocityVector;       // Current velocity

			public Vector3 initialPosition;      // Initial position
			public Vector3 initialVelocity;      // Initial velocity
			public float creationTime;     // Time of creation

			public System.Drawing.Color diffuseColor; // Initial diffuse color
			public System.Drawing.Color fadeColor;    // Faded diffuse color
			public float fadeProgression;      // Fade progression
		};

		private float radius = 0.0f;

		private float time = 0.0f;
		private int baseParticle = 0;
		private int flush = 0;
		private int discard = 0;

		private int particles = 0;
		private int particlesLimit = 0;
		private System.Collections.ArrayList particlesList = new System.Collections.ArrayList();
		private System.Collections.ArrayList freeParticles = new System.Collections.ArrayList();

		private System.Random rand = new System.Random();

		// Geometry
		private VertexBuffer vertexBuffer = null;

        /// <summary>
        /// Constructor
        /// </summary>
        public ParticleSystem(int numFlush, int numDiscard, float fRadius)
		{
			radius        = fRadius;

			baseParticle         = numDiscard;
			flush        = numFlush;
			discard      = numDiscard;

			particles    = 0;
			particlesLimit = 2048;
		}


        
        
        /// <summary>
        /// Restores the device objects
        /// </summary>
        public void RestoreDeviceObjects(Device dev)
		{
			// Create a vertex buffer for the particle system.  The size of this buffer
			// does not relate to the number of particles that exist.  Rather, the
			// buffer is used as a communication channel with the device.. we fill in 
			// a bit, and tell the device to draw.  While the device is drawing, we
			// fill in the next bit using NOOVERWRITE.  We continue doing this until 
			// we run out of vertex buffer space, and are forced to DISCARD the buffer
			// and start over at the beginning.

			vertexBuffer = new VertexBuffer(typeof(PointVertex), discard, dev,  Usage.Dynamic | Usage.WriteOnly | Usage.Points, PointVertex.Format, Pool.Default);
		}

        
        
        
        /// <summary>
        /// Updates the scene
        /// </summary>
        public void Update(float fSecsPerFrame, int NumParticlesToEmit,
			System.Drawing.Color clrEmitColor,System.Drawing.Color clrFadeColor, float fEmitVel, Vector3 vPosition)
		{
			time += fSecsPerFrame;
			for (int ii = particlesList.Count-1; ii >= 0; ii--)
			{
				Particle p = (Particle)particlesList[ii];
				// Calculate new position
				float fT = time - p.creationTime;
				float fGravity;

				if (p.isSpark)
				{
					fGravity = -5.0f;
					p.fadeProgression -= (fSecsPerFrame * 2.25f);
				}
				else
				{
					fGravity = -9.8f;
					p.fadeProgression -= fSecsPerFrame * 0.25f;
				}

				p.positionVector    = p.initialVelocity * fT + p.initialPosition;
				p.positionVector.Y += (0.5f * fGravity) * (fT * fT);
				p.velocityVector.Y  = p.initialVelocity.Y + fGravity * fT;

				if (p.fadeProgression < 0.0f)
					p.fadeProgression = 0.0f;

				// Kill old particles
				if (p.positionVector.Y < radius ||
					p.isSpark && p.fadeProgression <= 0.0f)
				{
					// Emit sparks
					if (!p.isSpark)
					{
						for (int i=0; i<4; i++)
						{
							Particle spark;

							if (freeParticles.Count > 0)
							{
								spark = (Particle)freeParticles[0];
								freeParticles.RemoveAt(0);
							}
							else
							{
								spark = new Particle();
							}

							spark.isSpark  = true;
							spark.initialVelocity = new Vector3();
							spark.initialPosition   = p.positionVector;
							spark.initialPosition.Y = radius;

							float fRand1 = ((float)rand.Next(int.MaxValue)/(float)int.MaxValue) * (float)Math.PI * 2.00f;
							float fRand2 = ((float)rand.Next(int.MaxValue)/(float)int.MaxValue) * (float)Math.PI * 0.25f;

							spark.initialVelocity.X  = p.velocityVector.X * 0.25f + (float)Math.Cos(fRand1) * (float)Math.Sin(fRand2);
							spark.initialVelocity.Z  = p.velocityVector.Z * 0.25f + (float)Math.Sin(fRand1) * (float)Math.Sin(fRand2);
							spark.initialVelocity.Y  = (float)Math.Cos(fRand2);
							spark.initialVelocity.Y *= ((float)rand.Next(int.MaxValue)/(float)int.MaxValue) * 1.5f;

							spark.positionVector = spark.initialPosition;
							spark.velocityVector = spark.initialVelocity;

							spark.diffuseColor = ColorOperator.Lerp(p.fadeColor, p.diffuseColor, p.fadeProgression);
							spark.fadeColor = System.Drawing.Color.Black;
							spark.fadeProgression   = 1.0f;
							spark.creationTime  = time;

							particlesList.Add(spark);
						}
					}

					// Kill particle
					freeParticles.Add(p);
					particlesList.RemoveAt(ii);

					if (!p.isSpark)
						particles--;
				}
				else
					particlesList[ii] = p;
			}

			// Emit new particles
			int particlesEmit = particles + NumParticlesToEmit;
			while(particles < particlesLimit && particles < particlesEmit)
			{
				Particle particle;

				if (freeParticles.Count > 0)
				{
					particle = (Particle)freeParticles[0];
					freeParticles.RemoveAt(0);
				}
				else
				{
					particle = new Particle();
				}

				// Emit new particle
				float fRand1 = ((float)rand.Next(int.MaxValue)/(float)int.MaxValue) * (float)Math.PI * 2.0f;
				float fRand2 = ((float)rand.Next(int.MaxValue)/(float)int.MaxValue) * (float)Math.PI * 0.25f;

				particle.isSpark = false;

				particle.initialPosition = vPosition + new Vector3(0.0f, radius, 0.0f);

				particle.initialVelocity.X  = (float)Math.Cos(fRand1) * (float)Math.Sin(fRand2) * 2.5f;
				particle.initialVelocity.Z  = (float)Math.Sin(fRand1) * (float)Math.Sin(fRand2) * 2.5f;
				particle.initialVelocity.Y  = (float)Math.Cos(fRand2);
				particle.initialVelocity.Y *= ((float)rand.Next(int.MaxValue)/(float)int.MaxValue) * fEmitVel;

				particle.positionVector = particle.initialPosition;
				particle.velocityVector = particle.initialVelocity;

				particle.diffuseColor = clrEmitColor;
				particle.fadeColor    = clrFadeColor;
				particle.fadeProgression      = 1.0f;
				particle.creationTime     = time;

				particlesList.Add(particle);
				particles++;
			}
		}

        
        
        
        /// <summary>
        /// Renders the scene
        /// </summary>
        public void Render(Device dev)
		{

			// Set the render states for using point sprites
			dev.RenderState.PointSpriteEnable = true;
			dev.RenderState.PointScaleEnable = true ;
			dev.RenderState.PointSize = 0.08f;
			dev.RenderState.PointSizeMin = 0.00f;
			dev.RenderState.PointScaleA = 0.00f;
			dev.RenderState.PointScaleB = 0.00f;
			dev.RenderState.PointScaleC = 1.00f;

			// Set up the vertex buffer to be rendered
			dev.SetStreamSource(0, vertexBuffer, 0);
			dev.VertexFormat = PointVertex.Format;

			PointVertex[] vertices = null;
			int numParticlesToRender = 0;



			// Lock the vertex buffer.  We fill the vertex buffer in small
			// chunks, using LockFlags.NoOverWrite.  When we are done filling
			// each chunk, we call DrawPrim, and lock the next chunk.  When
			// we run out of space in the vertex buffer, we start over at
			// the beginning, using LockFlags.Discard.

			baseParticle += flush;

			if (baseParticle >= discard)
				baseParticle = 0;

			int count = 0;
			vertices = (PointVertex[])vertexBuffer.Lock(baseParticle * DXHelp.GetTypeSize(typeof(PointVertex)), typeof(PointVertex), (baseParticle != 0) ? LockFlags.NoOverwrite : LockFlags.Discard, flush);
			foreach(Particle p in particlesList)
			{
				Vector3 vPos = p.positionVector;
				Vector3 vVel = p.velocityVector;
				float fLengthSq = vVel.LengthSq();
				uint steps;

				if (fLengthSq < 1.0f)        steps = 2;
				else if (fLengthSq <  4.00f) steps = 3;
				else if (fLengthSq <  9.00f) steps = 4;
				else if (fLengthSq < 12.25f) steps = 5;
				else if (fLengthSq < 16.00f) steps = 6;
				else if (fLengthSq < 20.25f) steps = 7;
				else                          steps = 8;

				vVel *= -0.04f / (float)steps;

				System.Drawing.Color diffuse = ColorOperator.Lerp(p.fadeColor, p.diffuseColor, p.fadeProgression);

				// Render each particle a bunch of times to get a blurring effect
				for (int i = 0; i < steps; i++)
				{
					vertices[count].v     = vPos;
					vertices[count].color = diffuse.ToArgb();
					count++;

					if (++numParticlesToRender == flush)
					{
						// Done filling this chunk of the vertex buffer.  Lets unlock and
						// draw this portion so we can begin filling the next chunk.

						vertexBuffer.Unlock();

						dev.DrawPrimitives(PrimitiveType.PointList, baseParticle, numParticlesToRender);

						// Lock the next chunk of the vertex buffer.  If we are at the 
						// end of the vertex buffer, LockFlags.Discard the vertex buffer and start
						// at the beginning.  Otherwise, specify LockFlags.NoOverWrite, so we can
						// continue filling the VB while the previous chunk is drawing.
						baseParticle += flush;

						if (baseParticle >= discard)
							baseParticle = 0;

						vertices = (PointVertex[])vertexBuffer.Lock(baseParticle * DXHelp.GetTypeSize(typeof(PointVertex)), typeof(PointVertex), (baseParticle != 0) ? LockFlags.NoOverwrite : LockFlags.Discard, flush);
						count = 0;

						numParticlesToRender = 0;
					}

					vPos += vVel;
				}
			}

			// Unlock the vertex buffer
			vertexBuffer.Unlock();
			// Render any remaining particles
			if (numParticlesToRender > 0)
				dev.DrawPrimitives(PrimitiveType.PointList, baseParticle, numParticlesToRender);

			// Reset render states
			dev.RenderState.PointSpriteEnable = false;
			dev.RenderState.PointScaleEnable = false;
		}
	}
}
