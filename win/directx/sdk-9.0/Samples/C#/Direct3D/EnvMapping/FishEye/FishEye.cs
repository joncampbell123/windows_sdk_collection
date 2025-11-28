//-----------------------------------------------------------------------------
// File: FishEye.cs
//
// Desc: Example code showing how to do a fisheye lens effect with cubemapping.
//       The scene is rendering into a cubemap each frame, and then a
//       funky-shaped object is rendered using the cubemap and an environment
//       map.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace FishEyeSample
{
    /// <summary>
    /// Application class. The base class (GraphicsSample) provides the 
    /// generic functionality needed in all Direct3D samples. MyGraphicsSample 
    /// adds functionality specific to this sample program.
    /// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		// Local variables for the sample
		GraphicsFont drawingFont = null;
		GraphicsMesh skyBox = null;
		CubeTexture cubeTex = null;

		VertexBuffer fishEyeVertex = null;
		IndexBuffer fishEyeIndex = null;
		int numFishEyeVertices = 0;
		int numFishEyeFaces = 0;

		private int NumberRings = 0;
		private int NumberSections = 0;
		private float ScaleSize = 0.0f;




        /// <summary>
        /// Application constructor. Sets attributes for the app.
        /// </summary>
		public MyGraphicsSample()
		{
			this.Text = "FishEye: Environment mapping";
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
            enumerationSettings.AppUsesDepthBuffer = true;

			drawingFont = new GraphicsFont("Arial", FontStyle.Bold);
			skyBox = new GraphicsMesh();
		}




        /// <summary>
        /// Called once per frame, the call is the entry point for animating the scene.
        /// </summary>
		protected override void FrameMove()
		{
			// When the window has focus, let the mouse adjust the scene
			if (System.Windows.Forms.Form.ActiveForm == this)
			{
				Quaternion quat = GraphicsUtility.GetRotationFromCursor(this);
				device.Transform.World = Matrix.RotationQuaternion(quat);
				RenderSceneIntoCubeMap();
			}
		}




        /// <summary>
        /// Renders all visual elements in the scene. This is called by the main 
        /// Render() function, and also by the RenderIntoCubeMap() function.
        /// </summary>
		void RenderScene()
		{
			// Render the skybox

			// Save current state
			Matrix matViewSave, matProjSave;
			matViewSave = device.Transform.View;
			matProjSave = device.Transform.Projection;

			// Disable zbuffer, center view matrix, and set FOV to 90 degrees
			Matrix matView = matViewSave;
			Matrix matProj = matViewSave;
			matView.M41 = matView.M42 = matView.M43 = 0.0f;
			matProj = Matrix.PerspectiveFovLH((float)Math.PI/2, 1.0f, 0.5f, 10000.0f);
			device.Transform.Projection = matProj;
			device.Transform.View = matView;
			device.RenderState.ZBufferEnable = false;

			// Render the skybox
			skyBox.Render(device);

			// Restore the render states
			device.Transform.Projection = matProjSave;
			device.Transform.View = matViewSave;
			device.RenderState.ZBufferEnable = true;

			// Render any other elements of the scene here. In this sample, only a
			// skybox is render, but a much more interesting scene could be rendered
			// instead.
		}




        /// <summary>
        /// Renders the scene to each of the 6 faces of the cube map
        /// </summary>
		void RenderSceneIntoCubeMap()
		{
			// Save transformation matrices of the device
			Matrix matViewSave, matProjSave;
			matViewSave = device.Transform.View;
			matProjSave = device.Transform.Projection;

			// Set the projection matrix for a field of view of 90 degrees
			device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI/2, 1.0f, 0.5f, 100.0f);

			// Get the current view matrix, to concat it with the cubemap view vectors
			Matrix matViewDir = device.Transform.View;
			matViewDir.M41 = 0.0f; matViewDir.M42 = 0.0f; matViewDir.M43 = 0.0f;

			// Store the current backbuffer and zbuffer
			Surface ourBackBuffer = device.GetRenderTarget(0);

			// Render to the six faces of the cube map
			for (int i=0; i<6; i++)
			{
				// Set the view transform for this cubemap surface
				Matrix matView = GraphicsUtility.GetCubeMapViewMatrix((CubeMapFace)i);
				matView = Matrix.Multiply(matViewDir, matView);
				device.Transform.View = matView;

				// Set the rendertarget to the i'th cubemap surface
				Surface cubeMapFace = cubeTex.GetCubeMapSurface((CubeMapFace)i, 0);
				device.SetRenderTarget(0, cubeMapFace);
				cubeMapFace.Dispose();

				// Render the scene
				device.BeginScene();
				RenderScene();
				device.EndScene();
			}

			// Change the rendertarget back to the main backbuffer
			device.SetRenderTarget(0, ourBackBuffer);
			ourBackBuffer.Dispose();

			// Restore the original transformation matrices
			device.Transform.Projection = matProjSave;
			device.Transform.View = matViewSave;
		}




        /// <summary>
        /// Called once per frame, the call is the entry point for 3d rendering. 
        /// This function sets up render states, clears the viewport, and renders the scene.
        /// </summary>
		protected override void Render()
		{
			// Begin the scene
			device.BeginScene();

			// Set the states we want: identity matrix, no z-buffer, and cubemap texture
			// coordinate generation
			device.Transform.World = Matrix.Identity;
			device.RenderState.ZBufferEnable = false;
			device.TextureState[0].TextureCoordinateIndex = (int)TextureCoordinateIndex.CameraSpaceReflectionVector;
			device.TextureState[0].TextureTransform = TextureTransform.Count3;

			// Render the fisheye lens object with the environment-mapped body.
			device.SetTexture(0, cubeTex);
			device.VertexFormat = CustomVertex.PositionNormal.Format;
			device.SetStreamSource(0, fishEyeVertex, 0);
			device.Indices = fishEyeIndex;
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numFishEyeVertices, 0, numFishEyeFaces);

			// Restore the render states
			device.TextureState[0].TextureCoordinateIndex = (int)TextureCoordinateIndex.PassThru;
			device.TextureState[0].TextureTransform = TextureTransform.Disable;
			device.RenderState.ZBufferEnable = true;


			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 21, System.Drawing.Color.Yellow, deviceStats);

			// End the scene.
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

			// Load the file objects
			try
			{
				skyBox.Create(device, "lobby_skybox.x");
				GenerateFishEyeLens(20, 20, 1.0f);
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}
		}




		/// <summary>
        /// The device exists, but may have just been Reset().  Resources in
        /// Pool.Default and any other device state that persists during
        /// rendering should be set here.  Render states, matrices, textures,
        /// etc., that don't change during rendering can be set once here to
        /// avoid redundant state setting during Render() or FrameMove().
        /// </summary>
		protected override void RestoreDeviceObjects(object sender, System.EventArgs e)
		{
			// InitializeDeviceObjects for file objects (build textures and vertex buffers)
			skyBox.RestoreDeviceObjects(device , null);

			// Create the cubemap
			cubeTex = new CubeTexture(device, 256, 1, Usage.RenderTarget, device.PresentationParameters.BackBufferFormat, Pool.Default);

			// Set default render states
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
			device.TextureState[0].AlphaArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].AlphaArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].AlphaOperation = TextureOperation.SelectArg1;
			device.SamplerState[0].AddressU = TextureAddress.Mirror;
			device.SamplerState[0].AddressV = TextureAddress.Mirror;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;
			device.SamplerState[0].MipFilter = TextureFilter.None;

			// Set the transforms
			Vector3 eyeVector    = new Vector3(0.0f, 0.0f,-5.0f);
			Vector3 lookAtVector = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 upVector    = new Vector3(0.0f, 1.0f, 0.0f);

			Matrix matView, matProj;
			matView = Matrix.LookAtLH(eyeVector, lookAtVector, upVector);
			matProj = Matrix.OrthoLH(2.0f, 2.0f, 0.5f, 100.0f);
			device.Transform.World = Matrix.Identity;
			device.Transform.View = matView;
			device.Transform.Projection = matProj;
		}




        /// <summary>
        /// Called during device initialization, this code checks the 
        /// device for some minimum set of capabilities
        /// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// GetTransform doesn't work on PUREDEVICE
			if (vertexProcessingType == VertexProcessingType.PureHardware)
				return false;

			// Check for cubemapping devices
			if (!caps.TextureCaps.SupportsCubeMap)
				return false;

			// Check that we can create a cube texture that we can render into
			if (!Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, 
				adapterFormat, Usage.RenderTarget, ResourceType.CubeTexture, backBufferFormat))
			{
				return false;
			}

			return true;
		}




        /// <summary>
        /// Makes vertex and index data for a fish eye lens
        /// </summary>
		void GenerateFishEyeLens(int numRings, int numSections, float scale)
		{
			// Save our params
			ScaleSize = scale;
			NumberRings = numRings;
			NumberSections = numSections;

			int numTriangles = (numRings + 1) * numSections * 2;
			int numVertices  = (numRings + 1) * numSections + 2;

			// Generate space for the required triangles and vertices.
			if ((fishEyeVertex == null) || (fishEyeVertex.Disposed))
			{
				fishEyeVertex = new VertexBuffer(typeof(CustomVertex.PositionNormal), numVertices, device, Usage.WriteOnly, CustomVertex.PositionNormal.Format, Pool.Default);
				// Hook our events
				fishEyeVertex.Created += new System.EventHandler(this.FishEyeVertexCreated);
				this.FishEyeVertexCreated(fishEyeVertex, null);
			}

			if ((fishEyeIndex == null) || (fishEyeIndex.Disposed))
			{
				fishEyeIndex = new IndexBuffer(typeof(short), numTriangles * 3, device, Usage.WriteOnly, Pool.Default);
				// Hook our events
				fishEyeIndex.Created += new System.EventHandler(this.FishEyeIndexCreated);
				this.FishEyeIndexCreated(fishEyeIndex, null);
			}

			numFishEyeVertices = numVertices;
			numFishEyeFaces = numTriangles;
		}




		/// <summary>
		/// Called when our fish eye vertex buffer has been created, or recreated due to a
		/// device reset or change
		/// </summary>
		/// <param name="sender">The vertex buffer sending the event</param>
		void FishEyeVertexCreated(object sender, EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			GraphicsStream stm = vb.Lock(0, 0, 0);

			// Generate vertices at the end points.
			stm.Write(new CustomVertex.PositionNormal(new Vector3(0.0f, 0.0f, ScaleSize), new Vector3(0.0f, 0.0f, 1.0f)));

			// Generate vertex points for rings
			float r = 0.0f;
			for (int i = 0; i < (NumberRings + 1); i++)
			{
				float phi = 0.0f;

				for (int j = 0; j < NumberSections; j++)
				{
					float x  =  r * (float)Math.Sin(phi);
					float y  =  r * (float)Math.Cos(phi);
					float z  = 0.5f - 0.5f * (x*x + y*y);

					float nx = -x;
					float ny = -y;
					float nz = 1.0f;

					stm.Write(new CustomVertex.PositionNormal(new Vector3(x, y, z), new Vector3(nx, ny, nz)));
					phi += (float)(2 * (float)Math.PI / NumberSections);
				}

				r += 1.5f / NumberRings;
			}
	
			vb.Unlock();
		}




		/// <summary>
		/// Called when our fish eye index buffer has been created, or recreated due to a
		/// device reset or change
		/// </summary>
		/// <param name="sender">The index buffer sending the event</param>
		void FishEyeIndexCreated(object sender, EventArgs e)
		{
			IndexBuffer ib = (IndexBuffer)sender;
			GraphicsStream stm = ib.Lock(0, 0, 0);

			// Generate triangles for the centerpiece
			for (int i = 0; i < 2 * NumberSections; i++)
			{
				stm.Write((short)0);
				stm.Write((short)(i + 1));
				stm.Write((short)(i + ((i + 1) % NumberSections)));
			}

			// Generate triangles for the rings
			int m = 1;  // 1st vertex begins at 1 to skip top point

			for (int i = 0; i < NumberRings; i++)
			{
				for (int j = 0; j < NumberSections; j++)
				{
					stm.Write((short)(m + j));
					stm.Write((short)(m + NumberSections + j));
					stm.Write((short)(m + NumberSections + ((j + 1) % NumberSections)));

					stm.Write((short)(m + j));
					stm.Write((short)(m + NumberSections + ((j + 1) % NumberSections)));
					stm.Write((short)(m + ((j + 1) % NumberSections)));
				}
				m += NumberSections;
			}


			ib.Unlock();
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