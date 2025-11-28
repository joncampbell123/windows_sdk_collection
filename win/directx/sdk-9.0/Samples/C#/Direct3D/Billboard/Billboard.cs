//-----------------------------------------------------------------------------
// File: Billboard.cs
//
// Desc: Example code showing how to do billboarding. The sample uses
//       billboarding to draw some trees.
//
//       Note: This implementation is for billboards that are fixed to rotate
//       about the Y-axis, which is good for things like trees. For
//       unconstrained billboards, like explosions in a flight sim, the
//       technique is the same, but the the billboards are positioned slightly
//       differently. Try using the inverse of the view matrix, TL-vertices, or
//       some other technique.
//
//       Note: This code uses the D3D Framework helper library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace Billboard
{
	/// <summary>
	/// Simple structure to hold data for rendering a tree
	/// </summary>
	public struct Tree
	{
		public CustomVertex.PositionColoredTextured v0, v1, v2, v3; // Four corners of billboard quad
		public Vector3 position;              // Origin of tree
		public int treeTextureIndex;          // Which texture map to use
		public int offsetIndex;               // Offset into vertex buffer of tree's vertices
	};




	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private const int NumberTrees = 500;
		public static Vector3 globalDirection;

		// Tree textures to use
		static readonly string[] treeTextureFileNames = new string[]
		{
			"Tree02S.dds",
			"Tree35S.dds",
			"Tree01S.dds",
		};

		private GraphicsMesh terrainMesh = null;
		private GraphicsMesh skyBoxMesh = null;
		private GraphicsFont drawingFont = null;

		private VertexBuffer treeVertexBuffer = null;  // Vertex buffer for rendering a tree
		private Texture[] treeTextures = new Texture[treeTextureFileNames.Length]; // Tree images
		Matrix billboardMatrix; // Used for billboard orientation
		System.Collections.ArrayList trees = new System.Collections.ArrayList();   // Array of tree info

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
			this.Text = "Billboard: D3D Billboarding Example";
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
			skyBoxMesh = new GraphicsMesh();
			terrainMesh = new GraphicsMesh();
			enumerationSettings.AppUsesDepthBuffer = true;
		}




		/// <summary>
		/// Verifies that the tree at the given index is sufficiently spaced
		/// from the other trees. If trees are placed too closely, one tree
		/// can quickly pop in front of the other as the camera angle changes.
		/// </summary>
		/// <param name="treeIndex">index of tree we are testing</param>
		/// <returns>true if valid, false otherwise</returns>
		bool IsTreePositionValid(Vector3 pos)
		{
			if (trees.Count < 1)
				return true;

			float x = pos.X;
			float z = pos.Z;

			for (int i=0; i < trees.Count; i++)
			{
				double fDeltaX = Math.Abs(x - ((Tree)trees[i]).position.X);
				double fDeltaZ = Math.Abs(z - ((Tree)trees[i]).position.Z);

				if (3.0 > Math.Pow(fDeltaX, 2) + Math.Pow(fDeltaZ, 2))
					return false;
			}

			return true;
		}




		/// <summary>
        /// The window has been created, but the device has not been created yet.  
        /// Here you can perform application-related initialization and cleanup that 
        /// does not depend on a device.
        /// </summary>
		protected override void OneTimeSceneInitialization()
		{
			System.Random rand = new System.Random();
			// Initialize the tree data
			for (int i=0; i<NumberTrees; i++)
			{
				Tree t = new Tree();
				// Position the trees randomly
				do
				{
					float fTheta  = 2.0f*(float)Math.PI*(float)rand.NextDouble();
					float fRadius = 25.0f + 55.0f * (float)rand.NextDouble();
					t.position.X  = fRadius * (float)Math.Sin(fTheta);
					t.position.Z  = fRadius * (float)Math.Cos(fTheta);
					t.position.Y  = HeightField(t.position.X, t.position.Z);
				}
				while (!IsTreePositionValid(t.position));

				// Size the trees randomly
				float fWidth  = 1.0f + 0.2f * (float)(rand.NextDouble()-rand.NextDouble());
				float fHeight = 1.4f + 0.4f * (float)(rand.NextDouble()-rand.NextDouble());

				// Each tree is a random color between red and green
				int r = (255-190) + (int)(190*(float)(rand.NextDouble()));
				int g = (255-190) + (int)(190*(float)(rand.NextDouble()));
				int b = 0;
				int color = unchecked((int)(0xff000000 + (r<<16) + (g<<8) + (b<<0)));

				t.v0.SetPosition(new Vector3(-fWidth, 0*fHeight, 0.0f));
				t.v0.Color = color;
				t.v0.Tu    = 0.0f;   t.v0.Tv = 1.0f;
				t.v1.SetPosition(new Vector3(-fWidth, 2*fHeight, 0.0f));
				t.v1.Color = color;
				t.v1.Tu    = 0.0f;   t.v1.Tv = 0.0f;
				t.v2.SetPosition(new Vector3(fWidth, 0*fHeight, 0.0f));
				t.v2.Color = color;
				t.v2.Tu    = 1.0f;   t.v2.Tv = 1.0f;
				t.v3.SetPosition(new Vector3(fWidth, 2*fHeight, 0.0f));
				t.v3.Color = color;
				t.v3.Tu    = 1.0f;   t.v3.Tv = 0.0f;

				// Pick a random texture for the tree
				t.treeTextureIndex = (int)(3 * rand.NextDouble());
				trees.Add(t);
			}
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Get the eye and lookat points from the camera's path
			Vector3 vUpVec = new Vector3(0.0f, 1.0f, 0.0f);
			Vector3 vEyePt = new Vector3();
			Vector3 vLookatPt = new Vector3();

			vEyePt.X = 30.0f*(float)Math.Cos(0.8f * (appTime));
			vEyePt.Z = 30.0f*(float)Math.Sin(0.8f * (appTime));
			vEyePt.Y = 4 + HeightField(vEyePt.X, vEyePt.Z);

			vLookatPt.X = 30.0f*(float)Math.Cos(0.8f * (appTime + 0.5f));
			vLookatPt.Z = 30.0f*(float)Math.Sin(0.8f * (appTime + 0.5f));
			vLookatPt.Y = vEyePt.Y - 1.0f;

			// Set the app view matrix for normal viewing
			device.Transform.View = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);

			// Set up a rotation matrix to orient the billboard towards the camera.
			Vector3 vDir = Vector3.Subtract(vLookatPt, vEyePt);
			if (vDir.X > 0.0f)
				billboardMatrix = Matrix.RotationY((float)(-Math.Atan(vDir.Z/vDir.X)+Math.PI/2));
			else
				billboardMatrix = Matrix.RotationY((float)(-Math.Atan(vDir.Z/vDir.X)-Math.PI/2));
			globalDirection = vDir;

			// Sort trees in back-to-front order
			trees.Sort(new TreeSortClass());

			// Store vectors
			eyePart = vEyePt;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.Black, 1.0f, 0);

			device.BeginScene();
			// Center view matrix for skybox and disable zbuffer
			Matrix matView, matViewSave;
			matViewSave = device.Transform.View;
			matView = matViewSave;
			matView.M41 = 0.0f; matView.M42 = -0.3f; matView.M43 = 0.0f;
			device.Transform.View = matView;
			device.RenderState.ZBufferEnable = false;
			// Some cards do not disable writing to Z when 
			// D3DRS_ZENABLE is FALSE. So do it explicitly
			device.RenderState.ZBufferWriteEnable = false;

			// Render the skybox
			skyBoxMesh.Render(device);

			// Restore the render states
			device.Transform.View = matViewSave;
			device.RenderState.ZBufferEnable = true;
			device.RenderState.ZBufferWriteEnable = true;

			// Draw the terrain
			terrainMesh.Render(device);

			// Draw the trees
			DrawTrees();
			
			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

			device.EndScene();
		}




		/// <summary>
		/// Render the trees in the sample
		/// </summary>
		protected void DrawTrees()
		{
			// Set diffuse blending for alpha set in vertices.
			renderState.AlphaBlendEnable = true;
			renderState.SourceBlend = Blend.SourceAlpha;
			renderState.DestinationBlend = Blend.InvSourceAlpha;

			// Enable alpha testing (skips pixels with less than a certain alpha.)
			if (Caps.AlphaCompareCaps.SupportsGreaterEqual)
			{
				device.RenderState.AlphaTestEnable = true;
				device.RenderState.ReferenceAlpha = 0x08;
				device.RenderState.AlphaFunction = Compare.GreaterEqual;
			}

			// Loop through and render all trees
			device.SetStreamSource(0, treeVertexBuffer, 0);
			device.VertexFormat = CustomVertex.PositionColoredTextured.Format;
			foreach(Tree t in trees)
			{
				// Quick culling for trees behind the camera
				// This calculates the tree position relative to the camera, and
				// projects that vector against the camera's direction vector. A
				// negative dot product indicates a non-visible tree.
				if (0 > (t.position.X - eyePart.X) * globalDirection.X + 
					(t.position.Z - eyePart.Z) * globalDirection.Z)
				{
					break;
				}
				// Set the tree texture
				device.SetTexture(0, treeTextures[t.treeTextureIndex]);

				// Translate the billboard into place
				billboardMatrix.M41 = t.position.X;
				billboardMatrix.M42 = t.position.Y;
				billboardMatrix.M43 = t.position.Z;
				device.Transform.World = billboardMatrix;

				// Render the billboard
				device.DrawPrimitives(PrimitiveType.TriangleStrip, t.offsetIndex, 2);
			}

			// Restore state
			device.Transform.World = Matrix.Identity;
			device.RenderState.AlphaTestEnable = false;
			device.RenderState.AlphaBlendEnable = false;
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

            try
            {
                // Create the tree textures
                for (int i=0; i<treeTextureFileNames.Length; i++)
                {
                    treeTextures[i] = GraphicsUtility.CreateTexture(device, treeTextureFileNames[i]); 
                }
            }
            catch
            {
                SampleException e = new MediaNotFoundException();
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
                throw e;
            }


            try
            {
                // Load the skybox
                skyBoxMesh.Create(device, "SkyBox2.x");

                // Load the terrain
                terrainMesh.Create(device, "SeaFloor.x");
            }
            catch
            {
                SampleException e = new MediaNotFoundException();
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
                throw e;
            }

            // Add some "hilliness" to the terrain
            VertexBuffer tempVertexBuffer;
            tempVertexBuffer = terrainMesh.SystemMesh.VertexBuffer;
            CustomVertex.PositionTextured[] pVertices;
            int numberVertices = terrainMesh.SystemMesh.NumberVertices;
            pVertices = (CustomVertex.PositionTextured[])tempVertexBuffer.Lock(0, typeof(CustomVertex.PositionTextured), 0, numberVertices);
            for (int i=0; i<numberVertices; i++)
                pVertices[i].Y = HeightField(pVertices[i].X, pVertices[i].Z);
            tempVertexBuffer.Unlock();

		}




        /// <summary>
        /// Fill the trees vertex buffer
        /// </summary>
		public void CreateTreeData(object sender, EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			// Copy tree mesh data into vertexbuffer
			CustomVertex.PositionColoredTextured[] v = new CustomVertex.PositionColoredTextured[NumberTrees * 4];
			int iTree;
			int offsetIndex = 0;
			for (iTree = 0; iTree < NumberTrees; iTree++)
			{
				v[offsetIndex+0] = ((Tree)trees[iTree]).v0;
				v[offsetIndex+1] = ((Tree)trees[iTree]).v1;
				v[offsetIndex+2] = ((Tree)trees[iTree]).v2;
				v[offsetIndex+3] = ((Tree)trees[iTree]).v3;
				Tree t = (Tree)trees[iTree];
				t.offsetIndex = offsetIndex;
				trees[iTree] = t;
				offsetIndex += 4;
			}
			vb.SetData(v, 0, 0);
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
            DirectXException.IgnoreExceptions();
            if ((treeVertexBuffer == null) || (treeVertexBuffer.Disposed))
            {
                // Create a quad for rendering each tree
                treeVertexBuffer = new VertexBuffer(typeof(CustomVertex.PositionColoredTextured), NumberTrees*4, device, Usage.WriteOnly, CustomVertex.PositionColoredTextured.Format, Pool.Default);

                treeVertexBuffer.Created += new System.EventHandler(this.CreateTreeData);
                this.CreateTreeData(treeVertexBuffer, null);
            }

            // Restore the device objects for the meshes and fonts
			terrainMesh.RestoreDeviceObjects(device , null);
			skyBoxMesh.RestoreDeviceObjects(device , null);

			// Set the transform matrices (view and world are updated per frame)
			Matrix matProj;
			float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			matProj = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 1.0f, 100.0f);
			device.Transform.Projection = matProj;

			// Set up the default texture states
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].AlphaOperation = TextureOperation.SelectArg1;
			device.TextureState[0].AlphaArgument1 = TextureArgument.TextureColor;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;
			device.SamplerState[0].MipFilter = TextureFilter.Linear;
			device.SamplerState[0].AddressU = TextureAddress.Clamp;
			device.SamplerState[0].AddressV = TextureAddress.Clamp;

			device.RenderState.DitherEnable = true;
			device.RenderState.ZBufferEnable = true;
			device.RenderState.Lighting = false;
            DirectXException.EnableExceptions();
        }




		/// <summary>
		/// Called when the app is exiting, or the device is being changed, this 
		/// function deletes any device-dependent objects.
		/// </summary>
		protected override void DeleteDeviceObjects(System.Object sender, System.EventArgs e)
		{
			if (terrainMesh != null)
				terrainMesh.Dispose();

			if (skyBoxMesh != null)
				skyBoxMesh.Dispose();

			for (int i=0; i<treeTextureFileNames.Length; i++)
			{
				if (treeTextures[i] != null)
					treeTextures[i].Dispose();
			}

			if (treeVertexBuffer != null)
				treeVertexBuffer.Dispose();
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

			if (vertexProcessingType == VertexProcessingType.PureHardware)
				return false; // GetTransform doesn't work on PUREDEVICE
			
			// This sample uses alpha textures and/or straight alpha. Make sure the
			// device supports them
			if (caps.TextureCaps.SupportsAlphaPalette)
				return true;
			if (caps.TextureCaps.SupportsAlpha)
				return true;

			return false;
		}




		/// <summary>
		/// Simple function to define "hilliness" for terrain
		/// </summary>
		float HeightField(float x, float y)
		{
			return 9*((float)Math.Cos(x/20+0.2f) * (float)Math.Cos(y/15-0.2f)+1.0f);
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
	/// A class to sort our trees in back-to-front order
	/// </summary>
	public class TreeSortClass : System.Collections.IComparer
	{
        /// <summary>
        /// Compare two trees
        /// </summary>
		public int Compare(object left, object right)
		{
			Tree l = (Tree)left;
			Tree r = (Tree)right;

			float d1 = l.position.X * MyGraphicsSample.globalDirection.X + l.position.Z * MyGraphicsSample.globalDirection.Z;
			float d2 = r.position.X * MyGraphicsSample.globalDirection.X + r.position.Z * MyGraphicsSample.globalDirection.Z;

			if (d1 == d2)
				return 0;

			if (d1 < d2)
				return +1;

			return -1;
		}
	}
}
