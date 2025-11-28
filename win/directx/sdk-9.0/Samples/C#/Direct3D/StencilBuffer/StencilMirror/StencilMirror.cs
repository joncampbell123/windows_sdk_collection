//-----------------------------------------------------------------------------
// File: StencilMirror.cs
//
// Desc: Example code showing how to use stencil buffers to implement planar
//       mirrors.
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

namespace StencilMirror
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
		public struct MeshVertex
		{
			public Vector3 p;
			public Vector3 n;
			public float tu, tv;
			public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Normal | VertexFormats.Texture1;
		};

		public struct MirrorVertex
		{
			public Vector3 p;
			public Vector3 n;
			public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Normal;
		};

		private readonly System.Drawing.Color FogColor = System.Drawing.Color.FromArgb(0x00000080);


		private GraphicsFont drawingFont = null;                 // Font for drawing text
		VertexBuffer mirrorVertexBuffer = null;
		Direct3D.Material mirrorMaterial = new Direct3D.Material();      // Material of the mirror
		Matrix mirrorMatrix = Matrix.Identity;         // Matrix to position mirror

		GraphicsMesh terrainMesh = null;                // X file of terrain
		Matrix terrainMatrix = Matrix.Identity;        // Matrix to position terrain

		GraphicsMesh helicopterMesh = null;             // X file object to render
		Matrix helicopterMatrix = Matrix.Identity;     // Matrix to animate X file object


		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "StencilMirror: Doing Reflections with Stencils";
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
			enumerationSettings.AppUsesDepthBuffer = true;
			MinDepthBits    = 16;
			MinStencilBits  = 4;
			terrainMesh = new GraphicsMesh();
			helicopterMesh = new GraphicsMesh();
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Position the terrain
			terrainMatrix = Matrix.Translation(0.0f, -2.6f, 0.0f);

			// Position the mirror (water polygon intersecting with terrain)
			mirrorMatrix = Matrix.Translation(0.0f, -1.0f, 0.0f);

			// Position and animate the main object
			float fObjectPosX = 50.0f*(float)Math.Sin(appTime/2);
			float fObjectPosY = 6;
			float fObjectPosZ = 10.0f*(float)Math.Cos(appTime/2);
			Matrix matRoll, matPitch, matRotate, matScale, matTranslate;
			matRoll = Matrix.RotationZ(0.2f*(float)Math.Sin(appTime/2));
			matRotate = Matrix.RotationY(appTime/2-(float)Math.PI/2);
			matPitch = Matrix.RotationX(-0.1f * (1+(float)Math.Cos(appTime)));
			matScale = Matrix.Scaling(0.5f, 0.5f, 0.5f);
			matTranslate = Matrix.Translation(fObjectPosX, fObjectPosY, fObjectPosZ);
			helicopterMatrix = Matrix.Multiply(matScale, matTranslate);
			helicopterMatrix = Matrix.Multiply(matRoll, helicopterMatrix);
			helicopterMatrix = Matrix.Multiply(matRotate, helicopterMatrix);
			helicopterMatrix = Matrix.Multiply(matPitch, helicopterMatrix);

			// Move the camera around
			float fEyeX = 10.0f * (float)Math.Sin(appTime/2.0f);
			float fEyeY =  3.0f * (float)Math.Sin(appTime/25.0f) + 13.0f;
			float fEyeZ =  5.0f * (float)Math.Cos(appTime/2.0f);

			Vector3 vEyePt    = new Vector3(fEyeX, fEyeY, fEyeZ);
			Vector3 vLookatPt = new Vector3(fObjectPosX, fObjectPosY, fObjectPosZ);
			Vector3 vUpVec    = new Vector3(0.0f, 1.0f, 0.0f);
			
			device.Transform.View = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
		}




        /// <summary>
        /// Renders the scene
        /// </summary>
		private void RenderScene()
		{
			// Render terrain
			device.Transform.World = terrainMatrix;
			terrainMesh.Render(device);

			// Draw the mirror
			device.SetTexture(0, null);
			device.Material = mirrorMaterial;
			device.Transform.World = mirrorMatrix;
			device.RenderState.AlphaBlendEnable = true;
			device.RenderState.SourceBlend = Blend.DestinationColor;
			device.RenderState.DestinationBlend = Blend.Zero;
			device.VertexFormat = MirrorVertex.Format;
			device.SetStreamSource(0, mirrorVertexBuffer, 0);
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);
			device.RenderState.AlphaBlendEnable = false;

			// Draw the object. Note: do this last, in case the object has alpha
			device.Transform.World = helicopterMatrix;
			helicopterMesh.Render(device);

		}




        /// <summary>
        /// Renders the mirrored scene 
        /// </summary>
		private void RenderMirror()
		{
			// Turn depth buffer off, and stencil buffer on
			device.RenderState.StencilEnable = true;
			device.RenderState.StencilFunction = Compare.Always;
			device.RenderState.ReferenceStencil = 0x1;
			device.RenderState.StencilMask = unchecked((int)0xffffffff);
			device.RenderState.StencilWriteMask = unchecked((int)0xffffffff);
			device.RenderState.StencilZBufferFail = StencilOperation.Keep;
			device.RenderState.StencilFail =  StencilOperation.Keep;
			device.RenderState.StencilPass = StencilOperation.Replace;

			// Make sure no pixels are written to the z-buffer or frame buffer
			device.RenderState.ZBufferWriteEnable = false;
			device.RenderState.AlphaBlendEnable = true ;
			device.RenderState.SourceBlend = Blend.Zero;
			device.RenderState.DestinationBlend = Blend.One;

			// Draw the reflecting surface into the stencil buffer
			device.SetTexture(0, null);
			device.Transform.World = mirrorMatrix;
			device.VertexFormat = MirrorVertex.Format;
			device.SetStreamSource(0, mirrorVertexBuffer, 0);
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

			// Save the view matrix
			Matrix matViewSaved = device.Transform.View;

			// Reflect camera in X-Z plane mirror
			Matrix matView, matReflect = new Matrix();
			Plane plane = Plane.FromPointNormal(new Vector3(0,0,0), new Vector3(0,1,0));
			matReflect.Reflect(plane);
			matView = Matrix.Multiply(matReflect, matViewSaved);
			device.Transform.View = matView;

			// Set a clip plane, so that only objects above the water are reflected, we don't care about any errors
			DirectXException.IgnoreExceptions();
			device.ClipPlanes[0].Plane = plane;
			device.ClipPlanes[0].Enabled = true;
			DirectXException.EnableExceptions();

			// Setup render states to a blended render scene against mask in stencil
			// buffer. An important step here is to reverse the cull-order of the
			// polygons, since the view matrix is being relected.
			device.RenderState.ZBufferWriteEnable = true;
			device.RenderState.StencilFunction = Compare.Equal;
			device.RenderState.StencilPass = StencilOperation.Keep;
			device.RenderState.SourceBlend = Blend.DestinationColor;
			device.RenderState.DestinationBlend = Blend.Zero;
			device.RenderState.CullMode = Cull.Clockwise;

			// Clear the zbuffer (leave frame- and stencil-buffer intact)
			device.Clear(ClearFlags.ZBuffer, 0, 1.0f, 0);

			// Render the scene
			RenderScene();

			// Restore render states
			device.RenderState.CullMode = Cull.CounterClockwise;
			device.RenderState.StencilEnable = false;
			device.RenderState.AlphaBlendEnable = false;
			device.ClipPlanes.DisableAll();
			device.Transform.View = matViewSaved;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer | ClearFlags.Stencil, FogColor, 1.0f, 0);

			device.BeginScene();

			// Render the scene
			RenderScene();

			// Render the reflection in the mirror
			RenderMirror();

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

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
			drawingFont.InitializeDeviceObjects(device);

			// Load the main file object
			if (helicopterMesh == null)
				helicopterMesh = new GraphicsMesh();

			try
			{
				helicopterMesh.Create(device, "Heli.x");

				// Load the terrain
				if (terrainMesh == null)
					terrainMesh = new GraphicsMesh();

				terrainMesh.Create(device, "SeaFloor.x");
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}

			// Tweak the terrain vertices to add some bumpy terrain
			if (terrainMesh != null)
			{
				// Set FVF to VertexFVF
				terrainMesh.SetVertexFormat(device, MeshVertex.Format);

				// Get access to the mesh vertices
				VertexBuffer tempVertexBuffer = null;
				MeshVertex[] vertices = null;
				int numVertices = terrainMesh.SystemMesh.NumberVertices;
				tempVertexBuffer  = terrainMesh.SystemMesh.VertexBuffer;
				vertices = (MeshVertex[])tempVertexBuffer.Lock(0,typeof(MeshVertex), 0, numVertices);

				for (int i=0; i<numVertices; i++)
				{
					Vector3 v00 = new Vector3(vertices[i].p.X + 0.0f, 0.0f, vertices[i].p.Z + 0.0f);
					Vector3 v10 = new Vector3(vertices[i].p.X + 0.1f, 0.0f, vertices[i].p.Z + 0.0f);
					Vector3 v01 = new Vector3(vertices[i].p.X + 0.0f, 0.0f, vertices[i].p.Z + 0.1f);
					v00.Y = HeightField(1*v00.X, 1*v00.Z);
					v10.Y = HeightField(1*v10.X, 1*v10.Z);
					v01.Y = HeightField(1*v01.X, 1*v01.Z);

					Vector3 n = Vector3.Cross((v01-v00), (v10-v00));
					n.Normalize();

					vertices[i].p.Y  = v00.Y;
					vertices[i].n.X  = n.X;
					vertices[i].n.Y  = n.Y;
					vertices[i].n.Z  = n.Z;
					vertices[i].tu  *= 10;
					vertices[i].tv  *= 10;
				}

				tempVertexBuffer.Unlock();
			}

			if (mirrorVertexBuffer == null)
			{
				// Create a big square for rendering the mirror, we don't need to recreate this every time, if the VertexBuffer
				// is destroyed (by a call to Reset for example), it will automatically be recreated and the 'Created' event fired.
				mirrorVertexBuffer = new VertexBuffer(typeof(MirrorVertex), 4, device, Usage.WriteOnly, MirrorVertex.Format, Pool.Default);
				mirrorVertexBuffer.Created += new System.EventHandler(this.MirrorCreated);
				// Manually fire the created event the first time
				this.MirrorCreated(mirrorVertexBuffer, null);
			}
		}




        /// <summary>
        /// Handles the vertex buffer creation event for the mirror 
        /// </summary>
        private void MirrorCreated(object sender, EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			MirrorVertex[] v = (MirrorVertex[])vb.Lock(0, 0);
			v[0].p = new Vector3(-80.0f, 0.0f,-80.0f);
			v[0].n = new Vector3(0.0f,  1.0f,  0.0f);
			v[1].p = new Vector3(-80.0f, 0.0f, 80.0f);
			v[1].n = new Vector3(0.0f,  1.0f,  0.0f);
			v[2].p = new Vector3(80.0f, 0.0f,-80.0f);
			v[2].n = new Vector3(0.0f,  1.0f,  0.0f);
			v[3].p = new Vector3(80.0f, 0.0f, 80.0f);
			v[3].n = new Vector3(0.0f,  1.0f,  0.0f);
			vb.Unlock();
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
			helicopterMesh.RestoreDeviceObjects(device , null);
			terrainMesh.RestoreDeviceObjects(device , null);

			// Set up textures
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;

			// Set up misc render states
			device.RenderState.ZBufferEnable = true;
			device.RenderState.DitherEnable = true;
			device.RenderState.SpecularEnable = false;
			device.RenderState.Ambient = System.Drawing.Color.FromArgb(0x00555555);

			// Set the transform matrices
			Vector3 vEyePt    = new Vector3(0.0f, 5.5f, -15.0f);
			Vector3 vLookatPt = new Vector3(0.0f, 1.5f,   0.0f);
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
			mirrorMaterial = GraphicsUtility.InitMaterial(System.Drawing.Color.LightBlue);

			// Set up the light
			GraphicsUtility.InitLight(device.Lights[0], LightType.Directional, 0.0f, -1.0f, 1.0f);
			device.Lights[0].Commit();
			device.Lights[0].Enabled = true;

			// Turn on fog
			float fFogStart =  80.0f;
			float fFogEnd   = 100.0f;
			device.RenderState.FogEnable = true ;
			device.RenderState.FogColor = FogColor;
			device.RenderState.FogTableMode = FogMode.None;
			device.RenderState.FogVertexMode = FogMode.Linear;
			device.RenderState.RangeFogEnable = false;
			device.RenderState.FogStart = fFogStart;
			device.RenderState.FogEnd = fFogEnd;
		}




		/// <summary>
		/// Called when the app is exiting, or the device is being changed, this 
		/// function deletes any device-dependent objects.
		/// </summary>
		protected override void DeleteDeviceObjects(System.Object sender, System.EventArgs e)
		{
			terrainMesh.Dispose();
			helicopterMesh.Dispose();
			mirrorVertexBuffer.Dispose();

			terrainMesh = null;
			helicopterMesh = null;
			mirrorVertexBuffer = null;
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
			if (vertexProcessingType == VertexProcessingType.PureHardware)
				return false; // GetTransform doesn't work on PUREDEVICE

			// Make sure device supports directional lights
			if ((vertexProcessingType ==  VertexProcessingType.Hardware) ||
				(vertexProcessingType ==  VertexProcessingType.Mixed))
			{
				if (!caps.VertexProcessingCaps.SupportsDirectionAllLights)
					return false;
				
				if (caps.MaxUserClipPlanes < 1)
					return false;

			}

			return true;
		}



		
        /// <summary>
        /// Returns the value of the height field at a point
        /// </summary>
        private float HeightField(float x, float z)
		{
			float y = 0.0f;
			y += 7.0f * (float)Math.Cos(0.051f*x + 0.0f) * (float)Math.Sin(0.055f*x + 0.0f);
			y += 7.0f * (float)Math.Cos(0.053f*z + 0.0f) * (float)Math.Sin(0.057f*z + 0.0f);
			y += 1.0f * (float)Math.Cos(0.101f*x + 0.0f) * (float)Math.Sin(0.105f*x + 0.0f);
			y += 1.0f * (float)Math.Cos(0.103f*z + 0.0f) * (float)Math.Sin(0.107f*z + 0.0f);
			y += 1.0f * (float)Math.Cos(0.251f*x + 0.0f) * (float)Math.Sin(0.255f*x + 0.0f);
			y += 1.0f * (float)Math.Cos(0.253f*z + 0.0f) * (float)Math.Sin(0.257f*z + 0.0f);
			return y;
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
