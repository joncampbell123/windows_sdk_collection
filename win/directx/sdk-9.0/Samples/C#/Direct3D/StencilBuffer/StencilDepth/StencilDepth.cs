//-----------------------------------------------------------------------------
// File: StencilDepth.cs
//
// Desc: Example code showing how to use stencil buffers to show the depth
//       complexity of a scene.
//
//       Note: This code uses the D3D Framework helper library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D = Microsoft.DirectX.Direct3D;




namespace StencilDepth
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private GraphicsFont font = null; // Font for drawing text
		private bool showDepthComplexity = true; // Should we show the depth complexity?
		private bool drawHelicopter = false; // Should we draw the helicopter or airplane?
		private GraphicsMesh[] fileObject = new GraphicsMesh[2];  // The graphical object we'll be rendering
		private VertexBuffer squareVertexBuffer = null; // The vertex buffer we'll use to draw
		private Matrix worldMatrix = Matrix.Identity;  // World matrix
		private System.Windows.Forms.MenuItem mnuOptions = null; // Main options window
		private System.Windows.Forms.MenuItem mnuShowDepth = null; // Show Depth
		private System.Windows.Forms.MenuItem mnuPick = null; // Pick object to draw

		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "StencilDepth: Displaying Depth Complexity";
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

			font = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			enumerationSettings.AppUsesDepthBuffer = true;
			MinDepthBits    = 16;
			MinStencilBits  = 4;

			mnuShowDepth = new MenuItem("Show &Depth Complexity");
			mnuShowDepth.Shortcut = Shortcut.CtrlD;
			mnuShowDepth.Checked = showDepthComplexity;
			mnuShowDepth.Click += new System.EventHandler(this.DepthClicked);

			mnuPick = new MenuItem();
			mnuPick.Text = drawHelicopter ? "&Show Airplane" : "&Show Helicopter";
			mnuPick.Shortcut = Shortcut.CtrlS;
			mnuPick.Click += new System.EventHandler(this.PickClicked);

			mnuOptions = new MenuItem("&Options", new MenuItem[] { mnuShowDepth, mnuPick});
			mnuMain.MenuItems.Add(mnuOptions);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
		    // Setup the world spin matrix
			worldMatrix = Matrix.RotationAxis(new Vector3(1.0f, 1.0f, 0.0f), appTime/2);
		}




        /// <summary>
        /// Turns on stencil and other states for recording the depth complexity 
        /// during the rendering of a scene.
        /// </summary>
		void SetStatesForRecordingDepthComplexity()
		{
			// Clear the stencil buffer
			device.Clear(ClearFlags.Stencil, System.Drawing.Color.Black.ToArgb(), 1.0f, 0);

			// Turn stenciling
			device.RenderState.StencilEnable = true;
			device.RenderState.StencilFunction = Compare.Always;
			device.RenderState.ReferenceStencil = 0;
			device.RenderState.StencilMask = 0x00000000;
			device.RenderState.StencilWriteMask = unchecked((int)0xffffffff);

			// Increment the stencil buffer for each pixel drawn
			device.RenderState.StencilZBufferFail = StencilOperation.IncrementSaturation;
			device.RenderState.StencilFail =  StencilOperation.Keep;
			device.RenderState.StencilPass =  StencilOperation.IncrementSaturation;
		}




        /// <summary>
        /// Draws the contents of the stencil buffer in false color. Use alpha 
        /// blending of one red, one green, and one blue rectangle to do false coloring 
        /// of bits 1, 2, and 4 in the stencil buffer.
        /// </summary>
		void ShowDepthComplexity()
		{
			// Turn off the buffer, and enable alpha blending
			device.RenderState.ZBufferEnable  = false;
			device.RenderState.AlphaBlendEnable = true;
			device.RenderState.SourceBlend =  Blend.SourceColor;
			device.RenderState.DestinationBlend = Blend.InvSourceColor;

			// Set up the stencil states
			device.RenderState.StencilZBufferFail = StencilOperation.Keep;
			device.RenderState.StencilFail = StencilOperation.Keep;
			device.RenderState.StencilPass = StencilOperation.Keep;
			device.RenderState.StencilFunction = Compare.NotEqual;
			device.RenderState.ReferenceStencil = 0;

			// Set the background to black
			device.Clear(ClearFlags.Target, System.Drawing.Color.Black.ToArgb(), 1.0f, 0);

			// Set render states for drawing a rectangle that covers the viewport.
			// The color of the rectangle will be passed in D3DRS_TEXTUREFACTOR
			device.VertexFormat = VertexFormats.Transformed;
			device.SetStreamSource(0, squareVertexBuffer, 0);
			device.TextureState[0].ColorArgument1 = TextureArgument.TFactor;
			device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;

			// Draw a red rectangle wherever the 1st stencil bit is set
			device.RenderState.StencilMask = 0x01;
			device.RenderState.TextureFactor = System.Drawing.Color.Red.ToArgb();
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

			// Draw a green rectangle wherever the 2nd stencil bit is set
			device.RenderState.StencilMask = 0x02;
			device.RenderState.TextureFactor = System.Drawing.Color.LightGreen.ToArgb();
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

			// Draw a blue rectangle wherever the 3rd stencil bit is set
			device.RenderState.StencilMask = 0x04;
			device.RenderState.TextureFactor = System.Drawing.Color.Blue.ToArgb();
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

			// Restore states
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.RenderState.ZBufferEnable = true;
			device.RenderState.StencilEnable  = false;
			device.RenderState.AlphaBlendEnable = false;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer , System.Drawing.Color.Blue.ToArgb(), 1.0f, 0);

			device.BeginScene();

            // Ignore exceptions during rendering
            DirectXException.IgnoreExceptions();

			if (showDepthComplexity)
				SetStatesForRecordingDepthComplexity();


			device.Transform.World = worldMatrix;
			if (drawHelicopter)
				fileObject[0].Render(device);
			else
				fileObject[1].Render(device);

			// Show the depth complexity of the scene
			if (showDepthComplexity)
				ShowDepthComplexity();

			// Output statistics
			font.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			font.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

            // Turn exceptions back on
            DirectXException.EnableExceptions();

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
			// Initialize the font's internal textures
			font.InitializeDeviceObjects(device);

			// Load the main file object
			for (int i = 0; i < 2; i++)
				if (fileObject[i] == null)
					fileObject[i] = new GraphicsMesh();

			try
			{
				// Load a .X file
				fileObject[0].Create(device, "Heli.x");
				fileObject[1].Create(device, "airplane 2.x");
				if ((squareVertexBuffer == null) || (squareVertexBuffer.Disposed))
				{
					// Create a big square for rendering the stencilbuffer contents
					squareVertexBuffer = new VertexBuffer(typeof(Vector4), 4, device, Usage.WriteOnly, VertexFormats.Transformed, Pool.Default);
					squareVertexBuffer.Created += new System.EventHandler(this.SquareVBCreated);
					this.SquareVBCreated(squareVertexBuffer, null);
				}
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}
		}




		/// <summary>
		/// Called when the vertex buffer has been created
		/// </summary>
		private void SquareVBCreated(object sender, EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			float xpos = (float)device.PresentationParameters.BackBufferWidth;
			float ypos = (float)device.PresentationParameters.BackBufferHeight;
			Vector4[] v = (Vector4[])vb.Lock(0, 0);
			v[0] = new Vector4(0, ypos, 0.0f, 1.0f);
			v[1] = new Vector4(0,  0, 0.0f, 1.0f);
			v[2] = new Vector4(xpos, ypos, 0.0f, 1.0f);
			v[3] = new Vector4(xpos,  0, 0.0f, 1.0f);
			vb.Unlock();
		}




		/// <summary>
		/// The ShowDepthComplexity menu item has been clicked, switch
		/// modes
		/// </summary>
		private void DepthClicked(object sender, EventArgs e)
		{
			showDepthComplexity = !showDepthComplexity;
			((MenuItem)sender).Checked = showDepthComplexity;
		}




		/// <summary>
		/// The user wants to see the other object
		/// </summary>
		private void PickClicked(object sender, EventArgs e)
		{
			drawHelicopter = !drawHelicopter;
			((MenuItem)sender).Text = drawHelicopter ? "&Show Airplane" : "&Show Helicopter";
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

			// Build the device objects for the file-based objecs
			for (int i = 0; i<2; i++)
				fileObject[i].RestoreDeviceObjects(device , null);

			// Setup textures (the .X file may have textures)
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;

			// Set up misc render states
			device.RenderState.ZBufferEnable = true;
			device.RenderState.DitherEnable = true;
			device.RenderState.SpecularEnable = false;
			device.RenderState.ColorVertex = true;
			device.RenderState.Ambient = System.Drawing.Color.Black;

			// Set the transform matrices
			Vector3 vEyePt    = new Vector3(0.0f, 0.0f, -15.0f);
			Vector3 vLookatPt = new Vector3(0.0f, 0.0f,   0.0f);
			Vector3 vUpVec    = new Vector3(0.0f, 1.0f,   0.0f);
			Matrix matWorld, matView, matProj;

			matWorld = Matrix.Identity;
			matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
			float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			matProj = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 1.0f, 1000.0f);

			device.Transform.World = matWorld;
			device.Transform.View = matView;
			device.Transform.Projection = matProj;

			// Set up a material
			device.Material = GraphicsUtility.InitMaterial(System.Drawing.Color.White);

			// Set up the light
			GraphicsUtility.InitLight(device.Lights[0], LightType.Directional, 0.0f, -1.0f, 0.0f);
			device.Lights[0].Commit();
			device.Lights[0].Enabled = true;
			device.RenderState.Lighting = true;
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// Need to support post-pixel processing (for stencil operations)
			if (!Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFormat,
				Usage.RenderTarget | Usage.QueryPostPixelShaderBlending, ResourceType.Surface,
				backBufferFormat))
			{
				return false;
			}
			// Make sure device supports directional lights
			if ((vertexProcessingType ==  VertexProcessingType.Hardware) ||
				(vertexProcessingType ==  VertexProcessingType.Mixed))
			{
				if (!caps.VertexProcessingCaps.SupportsDirectionAllLights)
					return false;

			}

			return true;
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
