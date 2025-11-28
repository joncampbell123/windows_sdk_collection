//-----------------------------------------------------------------------------
// File: ProgressiveMesh.cs
//
// Desc: Sample of creating progressive meshes in D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D = Microsoft.DirectX.Direct3D;




namespace ProgressiveMeshSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		// Menu items specific for this application
		private MenuItem mnuOptions = null;
		private MenuItem mnuOptimize = null;
		private MenuItem mnuOpenMesh = null;
		private MenuItem mnuMeshBreak = null;

		private GraphicsFont font = null;                 // Font for drawing text
		string initialDirectory = null;
		string meshFilename = "tiger.x"; // Filename of mesh
		ProgressiveMesh[] pmeshes = null;          
		ProgressiveMesh fullPmesh = null;          
		int currentPmesh = 0;

		Direct3D.Material[] meshMaterials = null;
		Texture[] meshTextures = null;        // Array of textures, entries are NULL if no texture specified

		GraphicsArcBall arcBall = null;              // Mouse rotation utility
		Vector3 objectCenter;        // Center of bounding sphere of object
		float objectRadius = 0.0f;        // Radius of bounding sphere of object

		bool displayHelp = false;
		bool showOptimized = true;

		bool initDone = false;            // hold off on any reaction to messages until fully inited


		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "ProgressiveMesh: Using Progressive Meshes in D3D";
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
			font = new GraphicsFont( "Arial", System.Drawing.FontStyle.Bold );
			enumerationSettings.AppUsesDepthBuffer = true;

			// Add our new menu options
			this.mnuOptions = new MenuItem();
			this.mnuOptimize = new MenuItem();
			this.mnuOpenMesh = new MenuItem();
			this.mnuMeshBreak = new MenuItem();

			// Add the Options menu to the main menu
			this.mnuMain.MenuItems.Add(this.mnuOptions);
			this.mnuOptions.Index = 1;
			this.mnuOptions.Text = "&Options";

			// Add the optimize menu to the options menu
			this.mnuOptions.MenuItems.Add(this.mnuOptimize);
			this.mnuOptimize.Text = "Show Optimized PMeshes 'o'";
			this.mnuOptimize.Click += new System.EventHandler(this.OptimizeClick);

			// Add the 'Open Mesh' dialog to the file menu.
			this.mnuFile.MenuItems.Add(0, mnuMeshBreak);
			this.mnuFile.MenuItems.Add(0, mnuOpenMesh);
			this.mnuMeshBreak.Text = "-";
			this.mnuOpenMesh.Text = "Open File...";
			this.mnuOpenMesh.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
			this.mnuOpenMesh.ShowShortcut = true;
			this.mnuOpenMesh.Click += new System.EventHandler(this.OpenMesh);

			mnuOptimize.Checked = showOptimized;

			// Set up our event handlers
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyDown);
			this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyUp);
		}




        /// <summary>
        /// Called during initial app startup, this function performs all the permanent initialization.
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
			Matrix matWorld = Matrix.Translation( -objectCenter.X,
				-objectCenter.Y,
				-objectCenter.Z );
			matWorld.Multiply(arcBall.RotationMatrix);
			matWorld.Multiply(arcBall.TranslationMatrix);
			device.Transform.World = matWorld;

			// Set up view matrix
			Vector3 vFrom= new Vector3( 0, 0,-3*objectRadius );
			Vector3 vAt = new Vector3( 0, 0, 0 );
			Vector3 vUp = new Vector3( 0, 1, 0 );
			device.Transform.View = Matrix.LookAtLH( vFrom, vAt, vUp );
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			if (!initDone)
				return;

			// Clear the viewport
			device.Clear( ClearFlags.Target | ClearFlags.ZBuffer , System.Drawing.Color.Blue, 1.0f, 0 );

			device.BeginScene();

			if( pmeshes != null )
			{
				// Set and draw each of the materials in the mesh
				for( int i=0; i < meshMaterials.Length; i++ )
				{
					device.Material = meshMaterials[i];
					device.SetTexture( 0, meshTextures[i] );
					if (showOptimized)
						pmeshes[currentPmesh].DrawSubset( i );
					else
						fullPmesh.DrawSubset( i );
				}
			}

			// Output statistics
			font.DrawText( 2,  1, System.Drawing.Color.Yellow, frameStats );
			font.DrawText( 2, 20, System.Drawing.Color.Yellow, deviceStats );

			if (showOptimized)
			{
				if (pmeshes == null)
				{
					font.DrawText( 2, 60, System.Drawing.Color.Yellow, "Unoptimized" );
					font.DrawText( 2, 40, System.Drawing.Color.Yellow, string.Format("Num Vertices = {0}, Optimized {1}", 0, 0.0f) );
				}
				else
				{
					font.DrawText( 2, 60, System.Drawing.Color.Yellow, string.Format("PMesh {0} out of {1}", currentPmesh + 1, pmeshes.Length) );

					font.DrawText( 2, 40, System.Drawing.Color.Yellow, string.Format("Num Vertices = {0}, Min = {1}, Max = {2}",
						((BaseMesh)pmeshes[currentPmesh]).NumberVertices,
						pmeshes[currentPmesh].MinVertices,
						pmeshes[currentPmesh].MaxVertices ) );
				}
			}
			else
			{
				font.DrawText( 2, 60, System.Drawing.Color.Yellow, "Unoptimized" );
				font.DrawText( 2, 40, System.Drawing.Color.Yellow, string.Format("Num Vertices = {0}, Min = {1}, Max = {2}",
					((BaseMesh)fullPmesh).NumberVertices,
					fullPmesh.MinVertices,
					fullPmesh.MaxVertices ) );
			}

			// Output text
			if( displayHelp )
			{
				font.DrawText(  2, 80, System.Drawing.Color.White, "<F1>\n\n<Up>\n<Down>\n\n<PgUp>\n<PgDn>\n\n<Home>\n<End>\n<o>", 0 );
				font.DrawText( 70, 80, System.Drawing.Color.White, "Toggle help\n\nAdd one vertex\nSubtract one vertex\n\nAdd 100 vertices\nSubtract 100 vertices\n\nMax vertices\nMin vertices\nShow optimized pmeshes");
			}
			else
			{
				font.DrawText(  2, 80, System.Drawing.Color.White, "<F1>\n" );
				font.DrawText( 70, 80, System.Drawing.Color.White, "Toggle help\n" );
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
			// Initialize the font's internal textures
			font.InitializeDeviceObjects(device);

			string path = DXUtil.FindMediaFile(initialDirectory, meshFilename);
			Mesh pMesh = null;
			Mesh pTempMesh = null;
			GraphicsStream adj = null;
			ExtendedMaterial[] mtrl = null;
			MeshFlags i32BitFlag;
			WeldEpsilons Epsilons = new WeldEpsilons();
			ProgressiveMesh pPMesh = null;
			int cVerticesMin = 0;
			int cVerticesMax = 0;
			int cVerticesPerMesh = 0;

			try
			{
				// Load the mesh from the specified file
				pMesh = Mesh.FromFile(path, MeshFlags.Managed, device, out adj, out mtrl);
				i32BitFlag = pMesh.Options.Use32Bit ? MeshFlags.Use32Bit : 0;

				// perform simple cleansing operations on mesh
				pTempMesh = Mesh.Clean(pMesh, adj, adj);
				pMesh.Dispose();
				pMesh = pTempMesh;

				//  Perform a weld to try and remove excess vertices like the model bigship1.x in the DX9.0 SDK (current model is fixed)
				//    Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
				pMesh.WeldVertices( 0, Epsilons, adj, adj);
				// verify validity of mesh for simplification
				pMesh.Validate(adj);

				meshMaterials = new Direct3D.Material[mtrl.Length];
				meshTextures = new Texture[mtrl.Length];
				for (int i = 0; i < mtrl.Length; i++)
				{
					meshMaterials[i] = mtrl[i].Material3D;
					meshMaterials[i].Ambient = meshMaterials[i].Diffuse;
					if ((mtrl[i].TextureFilename != null) && (mtrl[i].TextureFilename != ""))
					{
						path = DXUtil.FindMediaFile(initialDirectory, mtrl[i].TextureFilename);
						// Find the path to the texture and create that texture
						try 
						{
							meshTextures[i] = TextureLoader.FromFile(device, path);
						}
						catch 
						{ 
							meshTextures[i] = null; 
						}
					}
				}

				// Lock the vertex buffer to generate a simple bounding sphere
				VertexBuffer vb = pMesh.VertexBuffer;
				GraphicsStream vertexData = vb.Lock(0, 0, LockFlags.NoSystemLock);
				objectRadius = Geometry.ComputeBoundingSphere(vertexData, pMesh.NumberVertices, pMesh.VertexFormat, out objectCenter);
				vb.Unlock();
				vb.Dispose();
				if (meshMaterials.Length == 0)
					throw new Exception();

				if ( (pMesh.VertexFormat & VertexFormats.Normal) == 0)
				{
					pTempMesh = pMesh.Clone(i32BitFlag | MeshFlags.Managed, pMesh.VertexFormat | VertexFormats.Normal, device);
					pTempMesh.ComputeNormals();
					pMesh.Dispose();
					pMesh = pTempMesh;
				}
				pPMesh = new ProgressiveMesh(pMesh, adj, null, 1, MeshFlags.SimplifyVertex);

				cVerticesMin = pPMesh.MinVertices;
				cVerticesMax = pPMesh.MaxVertices;

				cVerticesPerMesh = (cVerticesMax - cVerticesMin) / 10;
				pmeshes = new ProgressiveMesh[(int)Math.Max(1, Math.Ceiling((cVerticesMax - cVerticesMin) / (float)cVerticesPerMesh))];

				// clone full size pmesh
				fullPmesh = pPMesh.Clone( MeshFlags.Managed | MeshFlags.VbShare, pPMesh.VertexFormat, device);

				// clone all the separate pmeshes
				for (int iPMesh = 0; iPMesh < pmeshes.Length; iPMesh++)
				{
					pmeshes[iPMesh] = pPMesh.Clone( MeshFlags.Managed | MeshFlags.VbShare, pPMesh.VertexFormat, device);
					// trim to appropriate space
					pmeshes[iPMesh].TrimByVertices(cVerticesMin + cVerticesPerMesh * iPMesh, cVerticesMin + cVerticesPerMesh * (iPMesh+1));

					pmeshes[iPMesh].OptimizeBaseLevelOfDetail(MeshFlags.OptimizeVertexCache);
				}
				currentPmesh = pmeshes.Length - 1;
				pmeshes[currentPmesh].NumberVertices = cVerticesMax;
				fullPmesh.NumberVertices = cVerticesMax;
				pPMesh.Dispose();
			}
			catch
			{
				// hide error so that device changes will not cause exit, shows blank screen instead
				return;
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
			// Setup render state
			renderState.Lighting = true;
			renderState.DitherEnable = true;
			renderState.ZBufferEnable = true;
			sampleState[0].MagFilter = TextureFilter.Linear;
			sampleState[0].MinFilter = TextureFilter.Linear;

			// Setup the light
			GraphicsUtility.InitLight( device.Lights[0], LightType.Directional, 0.0f, -0.5f, 1.0f );
			device.Lights[0].Commit();
			device.Lights[0].Enabled = true;
			renderState.Ambient = System.Drawing.Color.FromArgb(0x00333333);

			// Set the arcball parameters
			arcBall.SetWindow( device.PresentationParameters.BackBufferWidth, device.PresentationParameters.BackBufferHeight,
				0.85f );
			arcBall.Radius = objectRadius;

			// Set the projection matrix
			float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			device.Transform.Projection = Matrix.PerspectiveFovLH( (float)Math.PI/4, fAspect, objectRadius/64.0f,objectRadius*200.0f);

			initDone = true;
		}

		

		
		/// <summary>
		/// Event Handler for windows messages
		/// </summary>
		private void OnPrivateKeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == System.Windows.Forms.Keys.F1)
				displayHelp = !displayHelp;

			if (e.KeyCode == System.Windows.Forms.Keys.O)
				OptimizeClick(null, null);
		}


        
        
        /// <summary>
        /// Event Handler for windows messages
        /// </summary>
        private void OnPrivateKeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (pmeshes != null)
			{
				uint numMeshVertices = (uint)((BaseMesh)pmeshes[currentPmesh]).NumberVertices;
				if (e.KeyCode == System.Windows.Forms.Keys.Up)
				{
					// Sometimes it is necessary to add more than one
					// vertex when increasing the resolution of a 
					// progressive mesh, so keep adding until the 
					// vertex count increases.
					for( int i = 1; i <= 8; i++ )
					{
						SetNumVertices( (uint)(numMeshVertices+i) );
						if( ((BaseMesh)pmeshes[currentPmesh]).NumberVertices == numMeshVertices+i )
							break;
					}
				}
				else if (e.KeyCode == System.Windows.Forms.Keys.Down)
					SetNumVertices( numMeshVertices-1 );
				else if (e.KeyCode == System.Windows.Forms.Keys.Prior)
					SetNumVertices( numMeshVertices+100 );
				else if (e.KeyCode == System.Windows.Forms.Keys.Next)
					SetNumVertices( numMeshVertices <= 100 ? 1 : numMeshVertices - 100 );
				else if (e.KeyCode == System.Windows.Forms.Keys.Home)
					SetNumVertices( 0xffffffff );
				else if (e.KeyCode == System.Windows.Forms.Keys.End)
					SetNumVertices( 1 );
			}
		}




        /// <summary>
        /// Sets the number of vertices to display on the current progressive mesh
        /// </summary>
		void SetNumVertices(uint numVertices)
		{
			fullPmesh.NumberVertices = (int)numVertices;

			// if current pm valid for desired value, then set the number of vertices directly
			if ((numVertices >= pmeshes[currentPmesh].MinVertices) && (numVertices <= pmeshes[currentPmesh].MaxVertices))
			{
				pmeshes[currentPmesh].NumberVertices = (int)numVertices;
			}
			else  // search for the right one
			{
				currentPmesh = pmeshes.Length - 1;

				// look for the correct "bin" 
				while (currentPmesh > 0)
				{
					// if number of vertices is less than current max then we found one to fit
					if (numVertices >= pmeshes[currentPmesh].MinVertices)
						break;

					currentPmesh -= 1;
				}

				// set the vertices on the newly selected mesh
				pmeshes[currentPmesh].NumberVertices = (int)numVertices;
			}

		}




		/// <summary>
		/// Fired when the 'Optimize' option is clicked
		/// </summary>
		private void OptimizeClick(object sender, EventArgs e)
		{
			showOptimized = !showOptimized;
			mnuOptimize.Checked = showOptimized;
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
					"a different .x file.","ProgressiveMesh", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
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
