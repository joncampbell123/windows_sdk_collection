//-----------------------------------------------------------------------------
// File: ClipMirror.cs
//
// Desc: This sample shows how to use clip planes to implement a planar mirror.
//       The scene is reflected in a mirror and rendered in a 2nd pass. The
//       corners of the mirrors, together with the camera eye point, are used
//       to define a custom set of clip planes so that the reflected geometry
//       appears only within the mirror's boundaries.
//
//       Note: This code uses the D3D Framework helper library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace ClipMirrorSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		/// <summary>
		/// Custom mirror vertex type.
		/// </summary>
		private GraphicsMesh teapotMesh = null;
		private GraphicsFont drawingFont = null;
		private Matrix teapotMatrix = new Matrix();
		private VertexBuffer mirrorVertexBuffer = null;

		// Vectors defining the camera
		private Vector3 eyePart = new Vector3(0.0f, 2.0f, -6.5f);
		private Vector3 lookAtPart = new Vector3(0.0f, 0.0f, 0.0f);
		private Vector3 upVector = new Vector3(0.0f, 1.0f, 0.0f);




		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "ClipMirror: Using D3D Clip Planes";
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

            this.ClientSize = new System.Drawing.Size(400,300);

			drawingFont = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			enumerationSettings.AppUsesDepthBuffer = true;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Set the teapot's local matrix (rotating about the y-axis)
			teapotMatrix.RotateY(appTime);

			// When the window has focus, let the mouse adjust the camera view
			if (this == System.Windows.Forms.Form.ActiveForm)
			{
				Quaternion quat = GraphicsUtility.GetRotationFromCursor(this);
				eyePart.X = 5 * quat.Y;
				eyePart.Y = 5 * quat.X;
				eyePart.Z = -(float)Math.Sqrt(50.0f - 25*quat.X*quat.X - 25*quat.Y*quat.Y);

				device.Transform.View = Matrix.LookAtLH(eyePart, lookAtPart, upVector);
			}
		}




		/// <summary>
		/// Renders all objects in the scene.
		/// </summary>
		public void RenderScene()
		{
			Matrix matLocal, matWorldSaved;
			matWorldSaved = device.Transform.World;

			// Build the local matrix
			matLocal = Matrix.Multiply(teapotMatrix, matWorldSaved);
			device.Transform.World = matLocal;

			// Render the object
			teapotMesh.Render(device);

			device.Transform.World = matWorldSaved;

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);
		}




		/// <summary>
		/// Renders the scene as reflected in a mirror. The corners of the mirror
		/// define a plane, which is used to build the reflection matrix. The scene is 
		/// rendered with the cull-mode reversed, since all normals in the scene are 
		/// likewise reflected.
		/// </summary>
		public void RenderMirror()
		{
			Matrix matWorldSaved;
			Matrix matReflectInMirror = new Matrix();
			Plane plane;

			// Save the world matrix so it can be restored
			matWorldSaved = device.Transform.World;

			// Get the four corners of the mirror. (This should be dynamic rather than
			// hardcoded.)
			Vector3 a = new Vector3(-1.5f, 1.5f, 3.0f);
			Vector3 b = new Vector3(1.5f, 1.5f, 3.0f);
			Vector3 c = new Vector3(-1.5f,-1.5f, 3.0f);
			Vector3 d = new Vector3(1.5f,-1.5f, 3.0f);

			// Construct the reflection matrix
			plane = Plane.FromPoints(a, b, c);
			matReflectInMirror.Reflect(plane);
			device.Transform.World = matReflectInMirror;

			// Reverse the cull mode (since normals will be reflected)
			device.RenderState.CullMode = Cull.Clockwise;

			// Set the custom clip planes (so geometry is clipped by mirror edges).
			// This is the heart of this sample. The mirror has 4 edges, so there are
			// 4 clip planes, each defined by two mirror vertices and the eye point.
			device.ClipPlanes[0].Plane = Plane.FromPoints(b, a, eyePart);
			device.ClipPlanes[1].Plane = Plane.FromPoints(d, b, eyePart);
			device.ClipPlanes[2].Plane = Plane.FromPoints(c, d, eyePart);
			device.ClipPlanes[3].Plane = Plane.FromPoints(a, c, eyePart);
			device.ClipPlanes.EnableAll();

			// Render the scene
			RenderScene();

			// Restore the modified render states
			device.Transform.World = matWorldSaved;
			device.ClipPlanes.DisableAll();
			device.RenderState.CullMode = Cull.CounterClockwise;

			// Finally, render the mirror itself (as an alpha-blended quad)
			device.RenderState.AlphaBlendEnable = true;
			device.RenderState.SourceBlend = Blend.SourceAlpha;
			device.RenderState.DestinationBlend = Blend.InvSourceAlpha	;

			device.SetStreamSource(0, mirrorVertexBuffer, 0);
			device.VertexFormat = CustomVertex.PositionNormalColored.Format;
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

			device.RenderState.AlphaBlendEnable = false;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0f, 0);

			device.BeginScene();
			RenderScene();
			RenderMirror(); // Render the scene in the mirror
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

			// Set up the geometry objects
			try
			{
				if (teapotMesh == null)
					teapotMesh = new GraphicsMesh();

				teapotMesh.Create(device, "teapot.x");
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
		protected override void RestoreDeviceObjects(System.Object sender, System.EventArgs e)
		{
			// Set up the geometry objects
			teapotMesh.RestoreDeviceObjects(device , null);

            if ((mirrorVertexBuffer == null) || (mirrorVertexBuffer.Disposed))
            {								 
                // Create a square for rendering the mirror
                mirrorVertexBuffer  = new VertexBuffer(typeof(CustomVertex.PositionNormalColored), 4,  device, 
                    Usage.WriteOnly, CustomVertex.PositionNormalColored.Format, Pool.Default);

                // Hook the created event so we can repopulate our data
                mirrorVertexBuffer.Created += new System.EventHandler(this.VertexBufferCreated);
                // Call our created handler for the first time
                this.VertexBufferCreated(mirrorVertexBuffer, null);
            }

            // Set up the textures
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;

			// Set miscellaneous render states
			device.RenderState.DitherEnable = true;
			device.RenderState.SpecularEnable = true;
			device.RenderState.ZBufferEnable = true;

			// Set up the matrices
			device.Transform.World = Matrix.Identity;
			device.Transform.View = Matrix.LookAtLH(eyePart, lookAtPart, upVector);
			float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 1.0f, 100.0f);

			// Set up a light
			if ((Caps.VertexProcessingCaps.SupportsDirectionAllLights) ||
				!(BehaviorFlags.HardwareVertexProcessing))
			{
				GraphicsUtility.InitLight(device.Lights[0], LightType.Directional, 0.2f, -1.0f, -0.2f);
				device.Lights[0].Commit();
				device.Lights[0].Enabled = true;
			}
			device.RenderState.Ambient = System.Drawing.Color.Black;
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

			if ((vertexProcessingType == VertexProcessingType.Hardware) ||
				(vertexProcessingType == VertexProcessingType.PureHardware) ||
				(vertexProcessingType == VertexProcessingType.Mixed))
			{
				if (caps.MaxUserClipPlanes < 4)
					return false;
			}
			return true;
		}




        /// <summary>
        /// Handles vertex buffer creation
        /// </summary>
        public void VertexBufferCreated(object sender, System.EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			// Initialize the mirror's vertices
			CustomVertex.PositionNormalColored[] v = (CustomVertex.PositionNormalColored[])vb.Lock(0, typeof(CustomVertex.PositionNormalColored), 0, 4);
			v[0].SetPosition(new Vector3(-1.5f, 1.5f, 3.0f));
			v[2].SetPosition(new Vector3(-1.5f,-1.5f, 3.0f));
			v[1].SetPosition(new Vector3(1.5f, 1.5f, 3.0f));
			v[3].SetPosition(new Vector3(1.5f,-1.5f, 3.0f));
			v[0].SetNormal(new Vector3(0.0f,0.0f,-1.0f)); v[1].SetNormal(new Vector3(0.0f,0.0f,-1.0f)); v[2].SetNormal(new Vector3(0.0f,0.0f,-1.0f)); v[3].SetNormal(new Vector3(0.0f,0.0f,-1.0f));
			v[0].Color = v[1].Color = v[2].Color = v[3].Color = unchecked((int)0x80ffffff);
			vb.Unlock();
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