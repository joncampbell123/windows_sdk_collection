//-----------------------------------------------------------------------------
// File: EnhancedMesh.cs
//
// Desc: Sample showing enhanced meshes in D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D = Microsoft.DirectX.Direct3D;




namespace EnhancedMeshSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		// Menu items specific for this application
		private MenuItem mnuOpenMesh = null;
		private MenuItem mnuMeshBreak = null;

		private GraphicsFont drawingFont = null;                 // Font for drawing text
		string initialDirectory = null;
		string meshFilename = "tiger.x"; // Filename of mesh
		
		Mesh systemMemoryMesh = null; // system memory version of mesh, lives through resize's
		Mesh enhancedMesh = null; // vid mem version of mesh that is enhanced
		int numberSegments; // number of segments per edge (tesselation level)
		Direct3D.Material[] meshMaterials = null; // array of materials
		Texture[] meshTextures = null;        // Array of textures, entries are NULL if no texture specified

		GraphicsArcBall arcBall = null;              // Mouse rotation utility
		Vector3 objectCenter;        // Center of bounding sphere of object
		float objectRadius = 0.0f;        // Radius of bounding sphere of object

		GraphicsStream adjacency = null; // Contains the adjacency info loaded with the mesh
		bool isUsingHardwareNPatches;
		bool isWireframe = false;


		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Enhanced Mesh - N-Patches";
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
			initialDirectory = DXUtil.SdkMediaPath;
			drawingFont = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			enumerationSettings.AppUsesDepthBuffer = true;
			numberSegments = 2;

			// Add our new menu options
			this.mnuOpenMesh = new MenuItem();
			this.mnuMeshBreak = new MenuItem();

			// Add the Open File menu to the file menu
			this.mnuFile.MenuItems.Add(0, this.mnuMeshBreak);
			this.mnuMeshBreak.Text = "-";
			this.mnuFile.MenuItems.Add(0, this.mnuOpenMesh);
			this.mnuOpenMesh.Text = "Open File...";
			this.mnuOpenMesh.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
			this.mnuOpenMesh.ShowShortcut = true;
			this.mnuOpenMesh.Click += new System.EventHandler(this.OpenMesh);

			// Set up our event handlers
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyDown);
		}





        /// <summary>
        /// The window has been created, but the device has not been created yet.  
        /// Here you can perform application-related initialization and cleanup that 
        /// does not depend on a device.
        /// </summary>
        protected override void OneTimeSceneInitialization()
		{
			// Set cursor to indicate that user can move the object with the mouse
			this.Cursor = System.Windows.Forms.Cursors.SizeAll;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Setup world matrix
			Matrix matWorld = Matrix.Translation(-objectCenter.X,
				-objectCenter.Y,
				-objectCenter.Z);
			matWorld.Multiply(arcBall.RotationMatrix);
			matWorld.Multiply(arcBall.TranslationMatrix);
			device.Transform.World = matWorld;

			// Set up view matrix
			Vector3 vFrom= new Vector3(0, 0,-3*objectRadius);
			Vector3 vAt = new Vector3(0, 0, 0);
			Vector3 vUp = new Vector3(0, 1, 0);
			device.Transform.View = Matrix.LookAtLH(vFrom, vAt, vUp);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer , System.Drawing.Color.Blue, 1.0f, 0);

			device.BeginScene();
			if (isUsingHardwareNPatches)
			{
				float fNumSegs;

				fNumSegs = (float)numberSegments;
				device.NPatchMode = fNumSegs;
			}

			// set and draw each of the materials in the mesh
			for (int i = 0; i < meshMaterials.Length; i++)
			{
				device.Material = meshMaterials[i];
				device.SetTexture(0, meshTextures[i]);

				enhancedMesh.DrawSubset(i);
			}

			if (isUsingHardwareNPatches)
			{
				device.NPatchMode = 0.0f;
			}

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

			// Display info on mesh
			drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, "Number Segments: ");
			drawingFont.DrawText(150, 40, System.Drawing.Color.White, numberSegments.ToString());

			drawingFont.DrawText(2, 60, System.Drawing.Color.Yellow, "Number Faces: ");
			drawingFont.DrawText(150, 60, System.Drawing.Color.White, (enhancedMesh == null) ? "0" : enhancedMesh.NumberFaces.ToString());

			drawingFont.DrawText(2, 80, System.Drawing.Color.Yellow, "Number Vertices: ");
			drawingFont.DrawText(150, 80, System.Drawing.Color.White, (enhancedMesh == null) ? "0" : enhancedMesh.NumberVertices.ToString());

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
			// Initialize the drawingFont's internal textures
			drawingFont.InitializeDeviceObjects(device);

			ExtendedMaterial[] mtrl = null;
			string path = DXUtil.FindMediaFile(initialDirectory, meshFilename);
			try
			{
				systemMemoryMesh = Mesh.FromFile(path, MeshFlags.SystemMemory, device, out adjacency, out mtrl);
			}
			catch
			{
				// Hide the error so we display a blue screen
				return;
			}

			// Lock the vertex buffer, to generate a simple bounding sphere
			VertexBuffer vb = null;
			try
			{
				vb = systemMemoryMesh.VertexBuffer;
				GraphicsStream vbStream = vb.Lock(0, 0, 0);
				objectRadius = Geometry.ComputeBoundingSphere(vbStream, systemMemoryMesh.NumberVertices, systemMemoryMesh.VertexFormat, out objectCenter);
			}
			finally
			{
				// Make sure we unlock the buffer if we fail
				if (vb != null)
					vb.Unlock();
			}

			meshMaterials = new Material[mtrl.Length];
			meshTextures = new Texture[mtrl.Length];
			for (int i = 0; i< mtrl.Length; i++)
			{
				meshMaterials[i] = mtrl[i].Material3D;
				if ((mtrl[i].TextureFilename != null) && ((mtrl[i].TextureFilename != string.Empty)))
					meshTextures[i] = TextureLoader.FromFile(device, DXUtil.FindMediaFile(null, mtrl[i].TextureFilename));
			}
			// Make sure there are normals, which are required for the tesselation
			// enhancement
			if ((systemMemoryMesh.VertexFormat & VertexFormats.Normal) != VertexFormats.Normal)
			{
				Mesh tempMesh = systemMemoryMesh.Clone(systemMemoryMesh.Options.Value, systemMemoryMesh.VertexFormat | VertexFormats.Normal, device);

				tempMesh.ComputeNormals();
				systemMemoryMesh.Dispose();
				systemMemoryMesh = tempMesh;
			}
		}




        /// <summary>
        /// Generates the enhanced mesh
        /// </summary>
		void GenerateEnhancedMesh(int newNumberSegs)
		{
			Mesh meshEnhancedSysMem = null;
			Mesh meshTemp = null;

			if (systemMemoryMesh == null)
				return;

			// if using hw, just copy the mesh
			if (isUsingHardwareNPatches)
			{
				meshTemp = systemMemoryMesh.Clone(MeshFlags.WriteOnly | MeshFlags.NPatches | (systemMemoryMesh.Options.Value & MeshFlags.Use32Bit), 
					systemMemoryMesh.VertexFormat, device);
			}
			else  // tesselate the mesh in sw
			{

				// Create an enhanced version of the mesh, will be in sysmem since source is
				try
				{
					meshEnhancedSysMem = Mesh.TessellateNPatches(systemMemoryMesh, adjacency, newNumberSegs, false);
				}
				catch
				{
					// If the tessellate failed, there might have been more triangles or vertices 
					// than can fit into a 16bit mesh, so try cloning to 32bit before tessellation

					meshTemp = systemMemoryMesh.Clone(MeshFlags.SystemMemory | MeshFlags.Use32Bit, systemMemoryMesh.VertexFormat, device);
					meshEnhancedSysMem = Mesh.TessellateNPatches(meshTemp, adjacency, newNumberSegs, false);
					meshTemp.Dispose();
				}

				// Make a video memory version of the mesh  
				// Only set WriteOnly if it doesn't use 32bit indices, because those 
				// often need to be emulated, which means that D3DX needs read-access.
				MeshFlags meshEnhancedFlags = meshEnhancedSysMem.Options.Value & MeshFlags.Use32Bit;
				if ((meshEnhancedFlags & MeshFlags.Use32Bit) != MeshFlags.Use32Bit)
					meshEnhancedFlags |= MeshFlags.WriteOnly;

				meshTemp = meshEnhancedSysMem.Clone(meshEnhancedFlags, systemMemoryMesh.VertexFormat, device);
			}

			enhancedMesh = meshTemp;
			numberSegments = newNumberSegs;
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

			isUsingHardwareNPatches = Caps.DeviceCaps.SupportsNPatches; // Do we have hardware support?

			GenerateEnhancedMesh(numberSegments);

			// Setup render state
			renderState.Lighting = true;
			renderState.DitherEnable = true;
			renderState.ZBufferEnable = true;
			renderState.Ambient = System.Drawing.Color.FromArgb(0x33333333);
			sampleState[0].MagFilter = TextureFilter.Linear;
			sampleState[0].MinFilter = TextureFilter.Linear;

			// Setup the light
			GraphicsUtility.InitLight(device.Lights[0], LightType.Directional, 0.0f, -1.0f, 1.0f);
			device.Lights[0].Commit();
			device.Lights[0].Enabled = true;

			// Set the arcball parameters
			arcBall.SetWindow(device.PresentationParameters.BackBufferWidth, device.PresentationParameters.BackBufferHeight, 0.85f);
			arcBall.Radius = 1.0f;

			// Set the projection matrix
			float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, objectRadius/64.0f,objectRadius*200.0f);

			if (isWireframe)
				device.RenderState.FillMode = FillMode.WireFrame;
			else
				device.RenderState.FillMode = FillMode.Solid;
		}


		
		
		/// <summary>
		/// Event Handler for windows messages
		/// </summary>
		private void OnPrivateKeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == System.Windows.Forms.Keys.Up)
			{
				GenerateEnhancedMesh(numberSegments + 1);
			}
			if (e.KeyCode == System.Windows.Forms.Keys.Down)
			{
				GenerateEnhancedMesh(Math.Max(1, numberSegments - 1));
			}
			if (e.KeyCode == System.Windows.Forms.Keys.W)
			{
				device.RenderState.FillMode = FillMode.WireFrame;
				isWireframe = true;
			}
			if (e.KeyCode == System.Windows.Forms.Keys.S)
			{
				device.RenderState.FillMode = FillMode.Solid;
				isWireframe = false;
			}
		}




		/// <summary>
		/// Fired when a new mesh needs to be opened
		/// </summary>
		private void OpenMesh(object sender, EventArgs e)
		{
			// Display the OpenFileName dialog. Then, try to load the specified file
			System.Windows.Forms.OpenFileDialog ofn = new OpenFileDialog();
			ofn.Filter = ".X Files (*.x)|*.x";
			ofn.FileName = meshFilename;
			ofn.InitialDirectory = initialDirectory;
			ofn.Title = "Open Mesh File";
			ofn.CheckFileExists = true;
			ofn.Multiselect = false;
			ofn.ShowReadOnly = false;
			ofn.ShowDialog();
			System.IO.FileInfo fo = new System.IO.FileInfo(ofn.FileName);
			meshFilename = fo.Name;
			System.IO.Directory.SetCurrentDirectory(fo.DirectoryName);
			initialDirectory = fo.DirectoryName;

			// Destroy and recreate everything
			InvalidateDeviceObjects(null, null);
			DeleteDeviceObjects(null, null);
			try
			{
				InitializeDeviceObjects();
				RestoreDeviceObjects(null, null);
			}
			catch
			{
				MessageBox.Show("Error loading mesh: mesh may not\n" +
					"be valid. See debug output for\n" +
					"more information.\n\nPlease select " +
					"a different .x file.","EnhancedMesh", MessageBoxButtons.OK, MessageBoxIcon.Error);

			}
		}




        /// <summary>
        /// Called during device initialization, this code checks the device for 
        /// some minimum set of capabilities
        /// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			if ((vertexProcessingType == VertexProcessingType.PureHardware) && 
			    (!caps.DeviceCaps.SupportsNPatches) && (caps.DeviceCaps.SupportsRtPatches))
				return false;

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
