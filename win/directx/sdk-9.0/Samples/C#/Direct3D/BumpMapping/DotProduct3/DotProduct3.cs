//-----------------------------------------------------------------------------
// File: DotProduct3.cs
//
// Desc: D3D sample showing how to do bumpmapping using the DotProduct3 
//       texture operation.
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace DotProductSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		/// <summary>
		/// Custom Bump vertex
		/// </summary>
		public struct DotVertex
		{
			public Vector3 p;
			public int diffuse;
			public int specular;
			public float tu1, tv1;

			public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Diffuse | VertexFormats.Specular | VertexFormats.Texture1;
		};



		private GraphicsFont drawingFont = null;                 // Font for drawing text
		// Scene
		private DotVertex[] quadVerts = new DotVertex[4]; // Quad vertex buffer data
		private Texture customNormalMap = null;
		private Texture fileBasedNormalMap = null;
		private Vector3 lightVector;

		bool isUsingFileBasedTexture = false;
		bool isShowingNormalMap = false;


		// Menu items for extra actions the sample can perform
		private MenuItem mnuOptions = null;
		private MenuItem mnuFileTexture = null;
		private MenuItem mnuCustomTexture = null;
		private MenuItem mnuBreak = null;
		private MenuItem mnuShowNormal = null;


		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "DotProduct3: BumpMapping Technique";
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
			enumerationSettings.AppUsesDepthBuffer = false;

			// Add the options menu to the main menu
			mnuOptions = new MenuItem("&Options");
			this.mnuMain.MenuItems.Add(mnuOptions);

			// Now create the rest of the menu items
			mnuFileTexture = new MenuItem("Use file-based texture");
			mnuFileTexture.Shortcut = Shortcut.CtrlF;
			mnuFileTexture.ShowShortcut = true;
			mnuFileTexture.Click += new System.EventHandler(this.FileTextureClicked);

			mnuCustomTexture = new MenuItem("Use custom texture");
			mnuCustomTexture.Shortcut = Shortcut.CtrlC;
			mnuCustomTexture.ShowShortcut = true;
			mnuCustomTexture.Click += new System.EventHandler(this.CustomTextureClicked);

			mnuBreak = new MenuItem("-");
			mnuShowNormal = new MenuItem("Show normal map");
			mnuShowNormal.Shortcut = Shortcut.CtrlN;
			mnuShowNormal.ShowShortcut = true;
			mnuShowNormal.Click += new System.EventHandler(this.ShowNormalClicked);

			// Now add the new items to the option menu
			mnuOptions.MenuItems.AddRange(new MenuItem[] { mnuFileTexture, mnuCustomTexture, mnuBreak, mnuShowNormal });
		}




		/// <summary>
		/// Turns a normalized vector into RGBA form. Used to encode vectors into a height map. 
		/// </summary>
		/// <param name="v">Normalized Vector</param>
		/// <param name="height">Height</param>
		/// <returns>RGBA Form of the vector</returns>
		private int VectorToRgba(Vector3 v, float height)
		{
			int r = (int)(127.0f * v.X + 128.0f);
			int g = (int)(127.0f * v.Y + 128.0f);
			int b = (int)(127.0f * v.Z + 128.0f);
			int a = (int)(255.0f * height);
    
			return((a<<24) + (r<<16) + (g<<8) + (b<<0));
		}




		/// <summary>
		/// Initializes a vertex
		/// </summary>
		/// <returns>The initialized vertex</returns>
		private DotVertex InitializeVertex(float x, float y, float z, float tu, float tv)
		{
			DotVertex vertex = new DotVertex();
			Vector3 v = new Vector3(1,1,1);
			v.Normalize();

			vertex.p = new Vector3(x, y, z);
			vertex.diffuse = VectorToRgba(v, 1.0f);
			vertex.specular = 0x40400000;
			vertex.tu1 = tu; vertex.tv1 = tv;
			return vertex;
		}



		
        /// <summary>
        /// The window has been created, but the device has not been created yet.  
        /// Here you can perform application-related initialization and cleanup that 
        /// does not depend on a device.
        /// </summary>
        protected override void OneTimeSceneInitialization()
		{
			quadVerts[0] = InitializeVertex(-1.0f,-1.0f,-1.0f, 0.0f, 0.0f);
			quadVerts[1] = InitializeVertex(1.0f,-1.0f,-1.0f, 1.0f, 0.0f);
			quadVerts[2] = InitializeVertex(-1.0f, 1.0f,-1.0f, 0.0f, 1.0f);
			quadVerts[3] = InitializeVertex(1.0f, 1.0f,-1.0f, 1.0f, 1.0f);

			lightVector = new Vector3(0.0f, 0.0f, 1.0f);
		}



		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			if (System.Windows.Forms.Form.ActiveForm == this)
			{
				System.Drawing.Point pt = this.PointToClient(System.Windows.Forms.Cursor.Position);

				lightVector.X = -(((2.0f * pt.X) / device.PresentationParameters.BackBufferWidth) - 1);
				lightVector.Y = -(((2.0f * pt.Y) / device.PresentationParameters.BackBufferHeight) - 1);
				lightVector.Z = 0.0f;

				if (lightVector.Length() > 1.0f)
					lightVector.Normalize();
				else
					lightVector.Z = (float)Math.Sqrt(1.0f - lightVector.X*lightVector.X
						- lightVector.Y*lightVector.Y);
			}
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target , System.Drawing.Color.Black.ToArgb(), 1.0f, 0);

			device.BeginScene();

			// Store the light vector, so it can be referenced in TextureArgument.TFactor
			int factor = VectorToRgba(lightVector, 0.0f);
			device.RenderState.TextureFactor = factor;

			// Modulate the texture (the normal map) with the light vector (stored
			// above in the texture factor)
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorOperation = TextureOperation.DotProduct3;
			device.TextureState[0].ColorArgument2 = TextureArgument.TFactor;

			// If user wants to see the normal map, override the above renderstates and
			// simply show the texture
			if (isShowingNormalMap)
				device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;

			// Select which normal map to use
			if (isUsingFileBasedTexture)
				device.SetTexture(0, fileBasedNormalMap);
			else
				device.SetTexture(0, customNormalMap);

			// Draw the bumpmapped quad
			device.VertexFormat = DotVertex.Format;
			device.DrawUserPrimitives(PrimitiveType.TriangleStrip, 2, quadVerts);

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

			device.EndScene();
		}


		/// <summary>
		/// Create a file based normap map
		/// </summary>
		void CreateFileBasedNormalMap()
		{
			Texture fileBasedSource = null;
			try
			{
				// Load the texture from a file
				fileBasedSource = GraphicsUtility.CreateTexture(device, "earthbump.bmp", Format.A8R8G8B8);
				SurfaceDescription desc = fileBasedSource.GetLevelDescription(0);
				fileBasedNormalMap = new Texture(device, desc.Width, desc.Height, fileBasedSource.LevelCount, 0, Format.A8R8G8B8, Pool.Managed);
				TextureLoader.ComputeNormalMap(fileBasedNormalMap, fileBasedSource, 0, Channel.Red, 1.0f);
			}
			catch 
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}
			finally
			{
				if (fileBasedSource != null)
					fileBasedSource.Dispose();
			}
		}




        /// <summary>
        /// Creates a custom normal map
        /// </summary>
        void CreateCustomNormalMap()
		{
			int localHeight = 512;
			int localWidth = 512;

			// Create a 32-bit texture for the custom normal map
			customNormalMap = new Texture(device, localWidth, localHeight, 1, 0, Format.A8R8G8B8, Pool.Managed);

			try
			{
				int[,] data = (int[,])customNormalMap.LockRectangle(typeof(int), 0, 0, localWidth, localHeight);
				// Fill each pixel
				for (int j=0; j<localHeight; j++)
				{
					for (int i=0; i<localWidth; i++)
					{
						float xp = ((5.0f*i) / (localWidth-1));
						float yp = ((5.0f*j) / (localHeight-1));
						float x  = 2*(xp-(float)Math.Floor(xp))-1;
						float y  = 2*(yp-(float)Math.Floor(yp))-1;
						float z  = (float)Math.Sqrt(1.0f - x*x - y*y);

						// Make image of raised circle. Outside of circle is gray
						if ((x*x + y*y) <= 1.0f)
						{
							Vector3 vector = new Vector3(x, y, z);
							data[j, i] = VectorToRgba(vector, 1.0f);
						}
						else
							data[j, i] = unchecked((int)0x80808080);
					}
				}
			}
			finally
			{
				// Make sure to unlock the rectangle even if there's an error
				customNormalMap.UnlockRectangle(0);
			}
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
			CreateFileBasedNormalMap();
			CreateCustomNormalMap();

			// Set the menu states
			mnuCustomTexture.Checked = !isUsingFileBasedTexture;
			mnuFileTexture.Checked = isUsingFileBasedTexture;
			mnuShowNormal.Checked = isShowingNormalMap;
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

			// Set the transform matrices
			Vector3 vEyePt    = new Vector3(0.0f, 0.0f, 2.0f);
			Vector3 vLookatPt = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vUpVec    = new Vector3(0.0f, 1.0f, 0.0f);
			Matrix matWorld, matView, matProj;

			matWorld = Matrix.Identity;
			matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
			float aspect = (float)device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			matProj = Matrix.PerspectiveFovLH((float)Math.PI/4, aspect, 1.0f, 500.0f);
			device.Transform.World = matWorld;
			device.Transform.View = matView;
			device.Transform.Projection = matProj;

			// Set any appropiate state
			device.RenderState.Lighting = false;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// All we need is the DotProduct texture operation
			return caps.TextureOperationCaps.SupportsDotProduct3;
		}




        /// <summary>
		/// Menu event handlers
		/// </summary>
		private void FileTextureClicked(object sender, EventArgs e)
		{
			isUsingFileBasedTexture = true;
			// Set the menu states
			mnuCustomTexture.Checked = !isUsingFileBasedTexture;
			mnuFileTexture.Checked = isUsingFileBasedTexture;
		}
		private void CustomTextureClicked(object sender, EventArgs e)
		{
			isUsingFileBasedTexture = false;
			// Set the menu states
			mnuCustomTexture.Checked = !isUsingFileBasedTexture;
			mnuFileTexture.Checked = isUsingFileBasedTexture;
		}
		private void ShowNormalClicked(object sender, EventArgs e)
		{
			isShowingNormalMap = !isShowingNormalMap;
			// Set the menu states
			mnuShowNormal.Checked = isShowingNormalMap;
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
