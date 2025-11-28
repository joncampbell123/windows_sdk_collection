//-----------------------------------------------------------------------------
// File: VertexShader.cs
//
// Desc: Example code showing how to do vertex shaders in D3D.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace VertexShaderSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private GraphicsFont drawingFont = null;                 // Font for drawing text
		private GraphicsFont drawingFontSmall = null;                 // Font for drawing text
		// Scene
		private VertexBuffer vertexBuffer = null;
		private IndexBuffer indexBuffer = null;
		private int numberVertices;
		private int numberIndices;
		private VertexShader ourShader = null;
		private VertexDeclaration ourDeclaration = null;
		private int m_Size;

		// Transforms
		private Matrix  positionMatrix;
		private Matrix  viewMatrix;
		private Matrix  projectionMatrix;

		// Navigation
		private byte [] keyValues = new byte[256];
		private float speed;
		private float angularSpeed;
		private bool doShowHelp;

		private Vector3 velocity;
		private Vector3 angularVelocity;

		


		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Vertex Shader";
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

			m_Size           = 32;
			numberIndices     = (m_Size - 1) * (m_Size - 1) * 6;
			numberVertices    = m_Size * m_Size;
			ourShader         = null;

			speed           = 5.0f;
			angularSpeed    = 1.0f;
			doShowHelp        = false;

			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyDown);
			this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyUp);
		}




        /// <summary>
        /// The window has been created, but the device has not been created yet.  
        /// Here you can perform application-related initialization and cleanup that 
        /// does not depend on a device.
        /// </summary>
		protected override void OneTimeSceneInitialization()
		{
			// Setup the view matrix
			Vector3 vEye = new Vector3(2.0f, 3.0f, 3.0f);
			Vector3 vAt  = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vUp  = new Vector3(0.0f, 1.0f, 0.0f);
			viewMatrix = Matrix.LookAtRH(vEye, vAt, vUp);

			// Set the position matrix
			positionMatrix = Matrix.Invert(viewMatrix);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			float fSecsPerFrame = elapsedTime;

			// Process keyboard input
			Vector3 vT = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vR = new Vector3(0.0f, 0.0f, 0.0f);

			if (keyValues[(int)Keys.Left] != 0 || keyValues[(int)Keys.NumPad1] != 0)                 vT.X -= 1.0f; // Slide Left
			if (keyValues[(int)Keys.Right] != 0 || keyValues[(int)Keys.NumPad3]  != 0)                vT.X += 1.0f; // Slide Right
			if (keyValues[(int)Keys.Down]  != 0)                                       vT.Y -= 1.0f; // Slide Down
			if (keyValues[(int)Keys.Up]  != 0)                                         vT.Y += 1.0f; // Slide Up
			if (keyValues['W'] != 0)                                           vT.Z -= 2.0f; // Move Forward
			if (keyValues['S'] != 0)                                           vT.Z += 2.0f; // Move Backward
			if (keyValues['A'] != 0 || keyValues[(int)Keys.NumPad8] != 0)                     vR.X -= 1.0f; // Pitch Down
			if (keyValues['Z'] != 0 || keyValues[(int)Keys.NumPad2] != 0)                     vR.X += 1.0f; // Pitch Up
			if (keyValues['E'] != 0 || keyValues[(int)Keys.NumPad6] != 0)                     vR.Y -= 1.0f; // Turn Right
			if (keyValues['Q'] != 0 || keyValues[(int)Keys.NumPad4] != 0)                     vR.Y += 1.0f; // Turn Left
			if (keyValues[(int)Keys.NumPad9] != 0)                                    vR.Z -= 2.0f; // Roll CW
			if (keyValues[(int)Keys.NumPad7] != 0)                                    vR.Z += 2.0f; // Roll CCW

			velocity        = velocity * 0.9f + vT * 0.1f;
			angularVelocity = angularVelocity * 0.9f + vR * 0.1f;

			// Update position and view matricies
			Matrix     matT, matR;
			Quaternion qR;

			vT = velocity * fSecsPerFrame * speed;
			vR = angularVelocity * fSecsPerFrame * angularSpeed;

			matT = Matrix.Translation(vT.X, vT.Y, vT.Z);
			positionMatrix = Matrix.Multiply(matT, positionMatrix);

			qR = Quaternion.RotationYawPitchRoll(vR.Y, vR.X, vR.Z);
			matR = Matrix.RotationQuaternion(qR);

			positionMatrix = Matrix.Multiply(matR, positionMatrix);
			viewMatrix = Matrix.Invert(positionMatrix);
			device.Transform.View = viewMatrix;

			// Set up the vertex shader constants
			Matrix mat = Matrix.Multiply(viewMatrix, projectionMatrix);
			mat.Transpose(mat);

			Vector4 vA = new Vector4 ((float)Math.Sin(appTime)*15.0f, 0.0f, 0.5f, 1.0f);
			Vector4 vD = new Vector4 ((float)Math.PI, 1.0f/(2.0f*(float)Math.PI), 2.0f*(float)Math.PI, 0.05f);

			// Taylor series coefficients for sin and cos
			Vector4 vSin = new Vector4(1.0f, -1.0f/6.0f, 1.0f/120.0f, -1.0f/5040.0f);
			Vector4 vCos = new Vector4(1.0f, -1.0f/2.0f, 1.0f/ 24.0f, -1.0f/ 720.0f);

			device.SetVertexShaderConstant(0, new Matrix[] { mat });
			device.SetVertexShaderConstant(4, new Vector4[] { vA });
			device.SetVertexShaderConstant(7, new Vector4[] { vD });
			device.SetVertexShaderConstant(10, new Vector4[] { vSin });
			device.SetVertexShaderConstant(11, new Vector4[] { vCos });
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, 0x000000ff, 1.0f, 0);

			device.BeginScene();

			device.VertexDeclaration = ourDeclaration;
			device.VertexShader = ourShader;
			device.SetStreamSource(0, vertexBuffer, 0, DXHelp.GetTypeSize(typeof(Vector2)));
			device.Indices = indexBuffer;
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numberVertices,
				0, numberIndices/3);


			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);
			if (doShowHelp)
			{
				drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White,
					"Keyboard controls:");
				drawingFontSmall.DrawText(20, 60, System.Drawing.Color.White,
					"Move\nTurn\nPitch\nSlide\nHelp\nChange device\nExit");
				drawingFontSmall.DrawText(210, 60, System.Drawing.Color.White,
					"W,S\nE,Q\nA,Z\nArrow keys\nF1\nF2\nEsc");
			}
			else
			{
				drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White,
					"Press F1 for help");
			}

			device.EndScene();
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
			// Setup render states
			device.RenderState.Lighting = false;
			device.RenderState.CullMode = Cull.None;

			// Create index buffer

			indexBuffer = new IndexBuffer(typeof(short), numberIndices, device, 0, Pool.Default);

			short[] indices = (short[])indexBuffer.Lock(0, 0);

			int count = 0;
			for (int y=1; y<m_Size; y++)
			{
				for (int x=1; x<m_Size; x++)
				{
					indices[count++] = (short)((y-1)*m_Size + (x-1));
					indices[count++] = (short)((y-0)*m_Size + (x-1));
					indices[count++] = (short)((y-1)*m_Size + (x-0));

					indices[count++] = (short)((y-1)*m_Size + (x-0));
					indices[count++] = (short)((y-0)*m_Size + (x-1));
					indices[count++] = (short)((y-0)*m_Size + (x-0));
				}
			}

			indexBuffer.Unlock();


			// Create vertex buffer
			vertexBuffer = new VertexBuffer(typeof(Vector2), numberVertices, device, Usage.WriteOnly, 0, Pool.Default);

			Vector2[] vertices  = (Vector2[])vertexBuffer.Lock(0, 0);

			count = 0;
			for (int y=0; y<m_Size; y++)
			{
				for (int x=0; x<m_Size; x++)
				{
					vertices[count++] = new Vector2(((float)x / (float)(m_Size-1) - 0.5f) * (float)Math.PI,
						((float)y / (float)(m_Size-1) - 0.5f) * (float)Math.PI);
				}
			}

			vertexBuffer.Unlock();
			// Create vertex shader
			string shaderPath = null;
			GraphicsStream code = null;
			// Create our declaration
			VertexElement[] decl = new VertexElement[] { new VertexElement(0, 0, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.Position, 0), VertexElement.VertexDeclarationEnd };
			ourDeclaration = new VertexDeclaration(device, decl);

			// Find the vertex shader file
			shaderPath = DXUtil.FindMediaFile(null, "Ripple.vsh");

			// Assemble the vertex shader from the file
			code = ShaderLoader.FromFile(shaderPath, null, 0);
			// Create the vertex shader
			ourShader = new VertexShader(device, code);
			code.Close();

			// Set up the projection matrix
			float fAspectRatio = (float)device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			projectionMatrix = Matrix.PerspectiveFovRH(Geometry.DegreeToRadian(60.0f), fAspectRatio, 0.1f, 100.0f);
			device.Transform.Projection = projectionMatrix;

		}




        /// <summary>
        /// Invalidate device objects 
        /// </summary>
        protected override void InvalidateDeviceObjects(System.Object sender, System.EventArgs e)
		{
			if (ourShader != null)
				ourShader.Dispose();
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			if ((vertexProcessingType == VertexProcessingType.Hardware) ||
				(vertexProcessingType == VertexProcessingType.PureHardware) ||
				(vertexProcessingType == VertexProcessingType.Mixed))
			{
				if (caps.VertexShaderVersion.Major < 1)
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
				doShowHelp = !doShowHelp;
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
}
