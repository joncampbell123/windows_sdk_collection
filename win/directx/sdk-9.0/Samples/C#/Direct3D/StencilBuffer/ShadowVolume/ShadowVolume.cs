//-----------------------------------------------------------------------------
// File: ShadowVolume.cs
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




namespace ShadowVolumeApplication
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

		public struct ShadowVertex
		{
			public Vector4 p;
			public int color;
			public static readonly VertexFormats Format = VertexFormats.Transformed | VertexFormats.Diffuse;
		};


		private GraphicsFont font = null;                 // Font for drawing text
		private GraphicsArcBall arcBall = null; // Used for mouse control of the scene
		private GraphicsMesh airplane = new GraphicsMesh(); // the airplane mesh
		private GraphicsMesh terrainObject = new GraphicsMesh(); // The terrain
		private ShadowVolume shadowVolume = new ShadowVolume(); // The shadow volume object
		private Matrix objectMatrix; // The matrices for the objects
		private Matrix terrainMatrix;
		private VertexBuffer squareVertexBuffer = null; // The vertex buffer
		private readonly System.Drawing.Color fogColor = System.Drawing.Color.DeepSkyBlue;

		


		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "ShadowVolume: RealTime Shadows Using The StencilBuffer";
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

			arcBall = new GraphicsArcBall(this);
			font = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			enumerationSettings.AppUsesDepthBuffer = true;
			MinDepthBits    = 16;
			MinStencilBits  = 4;

		}




        /// <summary>
        /// The window has been created, but the device has not been created yet.  
        /// Here you can perform application-related initialization and cleanup that 
        /// does not depend on a device.
        /// </summary>
        protected override void OneTimeSceneInitialization()
		{
			this.Cursor = System.Windows.Forms.Cursors.SizeAll;		
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Position the terrain
			terrainMatrix = Matrix.Translation(0.0f, 0.0f, 0.0f);

			// Setup viewing postion from ArcBall
			objectMatrix = arcBall.RotationMatrix;
			objectMatrix.Multiply(arcBall.TranslationMatrix);

			// Move the light
			float x = 5;
			float y = 5;
			float z = -5;
			GraphicsUtility.InitLight(device.Lights[0], LightType.Point, x, y, z);
			device.Lights[0].Attenuation0 = 0.9f;
			device.Lights[0].Attenuation1 = 0.0f;
            device.Lights[0].Commit();

			// Transform the light vector to be in object space
			Vector3 vLight = new Vector3();
			Matrix m = Matrix.Invert(objectMatrix);
			vLight.X = x*m.M11 + y*m.M21 + z*m.M31 + m.M41;
			vLight.Y = x*m.M12 + y*m.M22 + z*m.M32 + m.M42;
			vLight.Z = x*m.M13 + y*m.M23 + z*m.M33 + m.M43;

			// Build the shadow volume
			shadowVolume.Reset();
			shadowVolume.BuildFromMesh(airplane.SystemMesh, vLight);
		}




        /// <summary>
        /// Render the shadow
        /// </summary>
		public void RenderShadow()
		{
			// Disable z-buffer writes (note: z-testing still occurs), and enable the
			// stencil-buffer
			device.RenderState.ZBufferWriteEnable = false;
			device.RenderState.StencilEnable = true;

			// Dont bother with interpolating color
			device.RenderState.ShadeMode = ShadeMode.Flat;

			// Set up stencil compare fuction, reference value, and masks.
			// Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
			// Note: since we set up the stencil-test to always pass, the STENCILFAIL
			// renderstate is really not needed.
			device.RenderState.StencilFunction = Compare.Always;
			device.RenderState.StencilZBufferFail = StencilOperation.Keep;
			device.RenderState.StencilFail = StencilOperation.Keep;

			// If ztest passes, inc/decrement stencil buffer value
			device.RenderState.ReferenceStencil = 0x1;
			device.RenderState.StencilMask = unchecked((int)0xffffffff);
			device.RenderState.StencilWriteMask = unchecked((int)0xffffffff);
			device.RenderState.StencilPass = StencilOperation.Increment;

			// Make sure that no pixels get drawn to the frame buffer
			device.RenderState.AlphaBlendEnable = true;
			device.RenderState.SourceBlend = Blend.Zero;
			device.RenderState.DestinationBlend = Blend.One;
			if (Caps.StencilCaps.SupportsTwoSided)
			{
				// With 2-sided stencil, we can avoid rendering twice:
				device.RenderState.TwoSidedStencilMode = true;
				device.RenderState.CounterClockwiseStencilFunction = Compare.Always;
				device.RenderState.CounterClockwiseStencilZBufferFail = StencilOperation.Keep;
				device.RenderState.CounterClockwiseStencilFail = StencilOperation.Keep;
				device.RenderState.CounterClockwiseStencilPass = StencilOperation.Decrement;

				device.RenderState.CullMode = Cull.None;

				// Draw both sides of shadow volume in stencil/z only
				device.Transform.World = objectMatrix;
				shadowVolume.Render(device);

				device.RenderState.TwoSidedStencilMode = false;
			}
			else
			{
				// Draw front-side of shadow volume in stencil/z only
				device.Transform.World = objectMatrix;
				shadowVolume.Render(device);

				// Now reverse cull order so back sides of shadow volume are written.
				device.RenderState.CullMode = Cull.Clockwise;

				// Decrement stencil buffer value
				device.RenderState.StencilPass = StencilOperation.Decrement;

				// Draw back-side of shadow volume in stencil/z only
				device.Transform.World = objectMatrix;
				shadowVolume.Render(device);
			}

			// Restore render states
			device.RenderState.ShadeMode = ShadeMode.Gouraud;
			device.RenderState.CullMode = Cull.CounterClockwise;
			device.RenderState.ZBufferWriteEnable = true;
			device.RenderState.StencilEnable = false;
			device.RenderState.AlphaBlendEnable = false;
		}




        /// <summary>
        /// Draws a big gray polygon over scene according to the mask in the 
        /// stencil buffer. (Any pixel with stencil==1 is in the shadow.)
        /// </summary>
		public void DrawShadow()
		{
			// Set renderstates (disable z-buffering, enable stencil, disable fog, and
			// turn on alphablending)
			device.RenderState.ZBufferEnable = false;
			device.RenderState.StencilEnable = true;
			device.RenderState.FogEnable = false;
			device.RenderState.AlphaBlendEnable = true;
			device.RenderState.SourceBlend = Blend.SourceAlpha;
			device.RenderState.DestinationBlend = Blend.InvSourceAlpha;

			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.TextureState[0].AlphaArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].AlphaArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].AlphaOperation = TextureOperation.Modulate;

			// Only write where stencil val >= 1 (count indicates # of shadows that
			// overlap that pixel)
			device.RenderState.ReferenceStencil = 0x1;
			device.RenderState.StencilFunction = Compare.LessEqual;
			device.RenderState.StencilPass = StencilOperation.Keep;

			// Draw a big, gray square
			device.VertexFormat = ShadowVertex.Format;
			device.SetStreamSource(0, squareVertexBuffer, 0);
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

			// Restore render states
			device.RenderState.ZBufferEnable = true;
			device.RenderState.StencilEnable = false;
			device.RenderState.FogEnable = true;
			device.RenderState.AlphaBlendEnable = false;
		}
		



		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer | ClearFlags.Stencil, fogColor, 1.0f, 0);

			device.BeginScene();

			device.Transform.World = terrainMatrix;
			terrainObject.Render(device);

			device.Transform.World = objectMatrix;
			airplane.Render(device, true, false);

			// Render the shadow volume into the stenicl buffer, then add it into
			// the scene
			RenderShadow();
			DrawShadow();

			// Output statistics
			font.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			font.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

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
			if (airplane == null)
				airplane = new GraphicsMesh();

			try
			{
				airplane.Create(device, "airplane 2.x");

				// Load the terrain
				if (terrainObject == null)
					terrainObject = new GraphicsMesh();

				terrainObject.Create(device, "SeaFloor.x");
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}

			// Set a reasonable vertex type
			terrainObject.SetVertexFormat(device, MeshVertex.Format);
			airplane.SetVertexFormat(device, MeshVertex.Format);

			// Tweak the terrain vertices to add some bumpy terrain
			if (terrainObject != null)
			{
				// Get access to the mesh vertices
				VertexBuffer vertBuffer = null;
				MeshVertex[] vertices = null;
				int numVertices = terrainObject.SystemMesh.NumberVertices;
				vertBuffer  = terrainObject.SystemMesh.VertexBuffer;
				vertices = (MeshVertex[])vertBuffer.Lock(0,typeof(MeshVertex), 0, numVertices);

				Random r = new Random();
				// Add some more bumpiness to the terrain object
				for (int i=0; i<numVertices; i++)
				{
					vertices[i].p.Y  += 1*(float)r.NextDouble();
					vertices[i].p.Y  += 2*(float)r.NextDouble();
					vertices[i].p.Y  += 1*(float)r.NextDouble();
				}

				vertBuffer.Unlock();
				vertBuffer.Dispose();
			}

		}




        /// <summary>
        /// Handles the vertex buffer created for the shadow
        /// </summary>
        private void ShadowCreated(object sender, EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			ShadowVertex[] v = (ShadowVertex[])vb.Lock(0, 0);
			float xpos = (float)device.PresentationParameters.BackBufferWidth;
			float ypos = (float)device.PresentationParameters.BackBufferHeight;
			v[0].p = new Vector4(0, ypos, 0.0f, 1.0f);
			v[1].p = new Vector4(0,  0, 0.0f, 1.0f);
			v[2].p = new Vector4(xpos, ypos, 0.0f, 1.0f);
			v[3].p = new Vector4(xpos,  0, 0.0f, 1.0f);
			v[0].color = 0x7f000000;
			v[1].color = 0x7f000000;
			v[2].color = 0x7f000000;
			v[3].color = 0x7f000000;
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
			airplane.RestoreDeviceObjects(device , null);
			terrainObject.RestoreDeviceObjects(device , null);

            if ((squareVertexBuffer == null) || (squareVertexBuffer.Disposed))
            {
                // Create a big square for rendering the mirror, we don't need to recreate this every time, if the VertexBuffer
                // is destroyed (by a call to Reset for example), it will automatically be recreated and the 'Created' event fired.
                squareVertexBuffer = new VertexBuffer(typeof(ShadowVertex), 4, device, Usage.WriteOnly, ShadowVertex.Format, Pool.Default);
                squareVertexBuffer.Created += new System.EventHandler(this.ShadowCreated);
                // Manually fire the created event the first time
                this.ShadowCreated(squareVertexBuffer, null);
            }

            // Create and set up the shine materials w/ textures
			device.Material = GraphicsUtility.InitMaterial(System.Drawing.Color.White);

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

			// Set the transform matrices
			Vector3 vEyePt    = new Vector3(0.0f, 10.0f, -20.0f);
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

			// Turn on fog
			float fFogStart =  30.0f;
			float fFogEnd   = 80.0f;
			device.RenderState.FogEnable = true ;
			device.RenderState.FogColor = fogColor;
			device.RenderState.FogTableMode = FogMode.None;
			device.RenderState.FogVertexMode = FogMode.Linear;
			device.RenderState.RangeFogEnable = false;
			device.RenderState.FogStart = fFogStart;
			device.RenderState.FogEnd = fFogEnd;

			// Set the ArcBall parameters
			arcBall.SetWindow(device.PresentationParameters.BackBufferWidth, device.PresentationParameters.BackBufferHeight, 2.0f);
			arcBall.Radius = 5.0f;

			device.Lights[0].Enabled = true;
			device.RenderState.Lighting = true;
			device.RenderState.Ambient = System.Drawing.Color.FromArgb(0x00303030);
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




    /// <summary>
    /// A shadow volume object
    /// </summary>
	public class ShadowVolume
	{
        /// <summary>
        /// Custom vertex types
        /// </summary>
		public struct MeshVertex
		{
			public Vector3 p;
			public Vector3 n;
			public float tu, tv;
		};




        /// <summary>
        /// Class member variables
        /// </summary>
		Vector3[] vertices = new Vector3[32000];
		int numVertices = 0;




        /// <summary>
        /// Reset the shadow volume
        /// </summary>
		public void Reset()
		{
			numVertices = 0;
		}




        /// <summary>
        /// Render the shadow volume
        /// </summary>
		public void Render(Device device)
		{
			device.VertexFormat = VertexFormats.Position;
			device.DrawUserPrimitives(PrimitiveType.TriangleList, numVertices / 3, vertices);
		}




        /// <summary>
        /// Takes a mesh as input, and uses it to build a shadowvolume. The
        //  technique used considers each triangle of the mesh, and adds it's
        //  edges to a temporary list. The edge list is maintained, such that
        //  only silohuette edges are kept. Finally, the silohuette edges are
        //  extruded to make the shadow volume vertex list.
        /// </summary>
		public void BuildFromMesh(Mesh mesh, Vector3 light)
		{
			// Note: the MeshVertex format depends on the FVF of the mesh
	
			MeshVertex[] tempVertices = null;
			short[] indices = null;
			short[] edges = null;

			int numFaces = mesh.NumberFaces;
			int numVerts = mesh.NumberVertices;
			int numEdges = 0;

			// Allocate a temporary edge list
			edges = new short[numFaces * 6];

			// Lock the geometry buffers
			tempVertices =  (MeshVertex[])mesh.LockVertexBuffer(typeof(MeshVertex), 0, numVerts);
			indices = (short[])mesh.LockIndexBuffer(typeof(short), 0, numFaces * 3);

			// For each face
			for (int i=0; i<numFaces; i++)
			{
				short face0 = indices[3*i+0];
				short face1 = indices[3*i+1];
				short face2 = indices[3*i+2];
				Vector3 v0 = tempVertices[face0].p;
				Vector3 v1 = tempVertices[face1].p;
				Vector3 v2 = tempVertices[face2].p;

				// Transform vertices or transform light?
				Vector3 vCross1 = v2 - v1;
				Vector3 vCross2 = v1 - v0;
				Vector3 vNormal = Vector3.Cross(vCross1, vCross2);

				if (Vector3.Dot(vNormal, light) >= 0.0f)
				{
					AddEdge(edges, ref numEdges, face0, face1);
					AddEdge(edges, ref numEdges, face1, face2);
					AddEdge(edges, ref numEdges, face2, face0);
				}
			}

			for (int i=0; i<numEdges; i++)
			{
				Vector3 v1 = tempVertices[edges[2*i+0]].p;
				Vector3 v2 = tempVertices[edges[2*i+1]].p;
				Vector3 v3 = v1 - light*10;
				Vector3 v4 = v2 - light*10;

				// Add a quad (two triangles) to the vertex list
				vertices[numVertices++] = v1;
				vertices[numVertices++] = v2;
				vertices[numVertices++] = v3;

				vertices[numVertices++] = v2;
				vertices[numVertices++] = v4;
				vertices[numVertices++] = v3;
			}

			// Unlock the geometry buffers
			mesh.UnlockVertexBuffer();
			mesh.UnlockIndexBuffer();
		}




        /// <summary>
        /// Adds an edge to a list of silohuette edges of a shadow volume.
        /// </summary>
		public void AddEdge(short[] edges, ref int numEdges, short v0, short v1)
		{
			// Remove interior edges (which appear in the list twice)
			for (int i=0; i < numEdges; i++)
			{
				if ((edges[2*i+0] == v0 && edges[2*i+1] == v1) ||
					(edges[2*i+0] == v1 && edges[2*i+1] == v0))
				{
					if (numEdges > 1)
					{
						edges[2*i+0] = edges[2*(numEdges-1)+0];
						edges[2*i+1] = edges[2*(numEdges-1)+1];
					}
					numEdges--;
					return;
				}
			}
			edges[2*numEdges+0] = v0;
			edges[2*numEdges+1] = v1;
			numEdges++;
		}
	}
}
