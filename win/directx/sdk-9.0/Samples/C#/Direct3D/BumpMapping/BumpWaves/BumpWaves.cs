//-----------------------------------------------------------------------------
// File: BumpWaves.cs
//
// Desc: Code to simulate reflections off waves using bumpmapping.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace BumpWavesSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private GraphicsFont drawingFont = null;                 // Font for drawing text
		// Scene
		private VertexBuffer waterBuffer = null;
		private Texture bumpMap = null;
        private Matrix bumpMapMatrix = Matrix.Zero;
        private VertexBuffer backgroundVertex = null;
		private Texture background = null;


        private int numVertx; // Number of vertices in the ground grid along X
        private int numVertz; // Number of vertices in the ground grid along Z
        private int numTriangles; // Number of triangles in the ground grid

        // In case we use shaders
        bool isUsingVertexShader = false; // Vertex shader for the bump waves
        VertexShader vertexShader = null; // Vertex declaration for the bump waves
        VertexDeclaration vertexDecl = null; // Whether to use vertex shader or FF pipeline
		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "BumpWaves: Using BumpMapping For Waves";
            try
            {
                // Load the icon from our resources
                System.Resources.ResourceManager resources = new System.Resources.ResourceManager(this.GetType());
                this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            }
            catch
            {
                // It's no big deal if we can't load our icons
            }

			drawingFont = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			enumerationSettings.AppUsesDepthBuffer = false;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
            // Setup the bump matrix
            // Min r is 0.04 if amplitude is 32 to miss temporal aliasing
            // Max r is 0.16 for amplitude is 8 to miss spatial aliasing
            float r = 0.04f;
            bumpMapMatrix.M11 =  r * (float)Math.Cos(appTime * 9.0f);
            bumpMapMatrix.M12 = -r * (float)Math.Sin(appTime * 9.0f);
            bumpMapMatrix.M21 =  r * (float)Math.Sin(appTime * 9.0f);
            bumpMapMatrix.M22 =  r * (float)Math.Cos(appTime * 9.0f);

        }




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target, System.Drawing.Color.Black, 0.0f, 0);

			device.BeginScene();

            // Set up texture stage states for the background
            device.SetTexture(0, background);
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
            device.TextureState[1].ColorOperation = TextureOperation.Disable;

            // Render the background
            device.VertexFormat = CustomVertex.PositionTextured.Format;
            device.SetStreamSource(0, backgroundVertex, 0);
            device.DrawPrimitives(PrimitiveType.TriangleStrip , 0, 2);

            // Render the water
            SetEMBMStates();
            if (isUsingVertexShader)
            {
                device.VertexShader = vertexShader;
                device.VertexDeclaration = vertexDecl;
            }
            else
            {
                device.VertexFormat = CustomVertex.PositionTextured.Format;
            }

            device.SetStreamSource(0, waterBuffer, 0);
            device.DrawPrimitives(PrimitiveType.TriangleList , 0, numTriangles);

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

            if (isUsingVertexShader)
                drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using Vertex Shader");
            else
                drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using Fixed-Function Vertex Pipeline");

			device.EndScene();
		}




        /// <summary>
        /// Set up the bumpmap states
        /// </summary>
        void SetEMBMStates()
        {
            // Set up texture stage 0's states for the bumpmap
            device.SetTexture(0, bumpMap);
            device.SamplerState[0].AddressU = TextureAddress.Clamp;
            device.SamplerState[0].AddressV = TextureAddress.Clamp;
            device.TextureState[0].BumpEnvironmentMaterial00 = bumpMapMatrix.M11;
            device.TextureState[0].BumpEnvironmentMaterial01 = bumpMapMatrix.M12;
            device.TextureState[0].BumpEnvironmentMaterial10 = bumpMapMatrix.M21;
            device.TextureState[0].BumpEnvironmentMaterial11 = bumpMapMatrix.M22;
            device.TextureState[0].ColorOperation = TextureOperation.BumpEnvironmentMap;
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorArgument2 = TextureArgument.Current;

            // Set up texture stage 1's states for the environment map
            device.SetTexture(1, background);
            device.TextureState[1].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[1].ColorOperation = TextureOperation.SelectArg1;

            if (isUsingVertexShader)
            {
                device.TextureState[1].TextureTransform = TextureTransform.Disable;
                device.TextureState[1].TextureCoordinateIndex = 1;
            }
            else
            {
                // Set up projected texture coordinates
                // tu = (0.8x + 0.5z) / z
                // tv = (0.8y - 0.5z) / z
                Matrix mat = Matrix.Zero;
                mat.M11 = 0.8f; mat.M12 = 0.0f; mat.M13 = 0.0f;
                mat.M21 = 0.0f; mat.M22 = 0.8f; mat.M23 = 0.0f;
                mat.M31 = 0.5f; mat.M32 =-0.5f; mat.M33 = 1.0f;
                mat.M41 = 0.0f; mat.M42 = 0.0f; mat.M43 = 0.0f;

                device.Transform.Texture1 = mat;
                device.TextureState[1].TextureTransform = TextureTransform.Count3 | TextureTransform.Projected;
                device.TextureState[1].TextureCoordinateIndex = (int)TextureCoordinateIndex.CameraSpacePosition | 1;

            }
        }




        /// <summary>
        /// Creates a bumpmap from a surface
        /// </summary>
        public Texture CreateBumpMap(int width, int height, Format bumpformat)
		{
            Texture bumpMapTexture = null;

            // Check if the device can create the format
            if (Manager.CheckDeviceFormat(Caps.AdapterOrdinal, Caps.DeviceType, graphicsSettings.DisplayMode.Format, 0, ResourceType.Textures, bumpformat) == false)
                return null;

			// Create the bumpmap texture
			bumpMapTexture = new Texture(device, width, height, 1, 0, bumpformat, Pool.Managed);

            int pitch;
            // Lock the surface and write in some bumps for the waves
            GraphicsStream dst = bumpMapTexture.LockRectangle(0, 0, out pitch);

            byte bumpUCoord, bumpVCoord;
			for (int y=0; y<height; y++)
			{
				for (int x=0; x<width; x++)
				{
                    float fx = x/(float)width - 0.5f;
                    float fy = y/(float)height - 0.5f;

                    float r = (float)Math.Sqrt(fx*fx + fy*fy);

                    bumpUCoord  = (byte)(64 * (float)Math.Cos(300.0f * r) * (float)Math.Exp(-r * 5.0f));
                    bumpUCoord += (byte)(32 * (float)Math.Cos(150.0f * (fx + fy)));
                    bumpUCoord += (byte)(16 * (float)Math.Cos(140.0f * (fx * 0.85f - fy)));

                    bumpVCoord  = (byte)(64 * (float)Math.Sin(300.0f * r) * (float)Math.Exp(-r * 5.0f));
                    bumpVCoord += (byte)(32 * (float)Math.Sin(150.0f * (fx + fy)));
                    bumpVCoord += (byte)(16 * (float)Math.Sin(140.0f * (fx * 0.85f - fy)));

                    dst.Write(bumpUCoord);
                    dst.Write(bumpVCoord);
				}
			}
			bumpMapTexture.UnlockRectangle(0);
            return bumpMapTexture;
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
			try
			{
                // Load the texture for the background image
                background = GraphicsUtility.CreateTexture(device, "lake.bmp", Format.R5G6B5);
                // Create the bumpmap. 
				bumpMap = CreateBumpMap(256, 256, Format.V8U8);
                if (bumpMap == null)
                    throw new InvalidOperationException();

                // Create a square for rendering the background
                if ((backgroundVertex == null) || (backgroundVertex.Disposed))
				{
					// We only need to create this buffer once
					backgroundVertex = new VertexBuffer(typeof(CustomVertex.PositionTextured), 4, device, Usage.WriteOnly, CustomVertex.PositionTextured.Format, Pool.Default);
					backgroundVertex.Created += new System.EventHandler(this.BackgroundVertexCreated);
					// Call it manually the first time
					this.BackgroundVertexCreated(backgroundVertex, null);
				}
                // See if EMBM and projected vertices are supported at the same time
                // in the fixed-function shader.  If not, switch to using a vertex shader.
                isUsingVertexShader = false;
                SetEMBMStates();
                device.VertexShader = null;
                device.VertexFormat = CustomVertex.PositionTextured.Format;

                ValidateDeviceParams validParams = device.ValidateDevice();
                if (validParams.Result != 0)
                    isUsingVertexShader = true;

                // If TextureCaps.Projected is set, projected textures are computed
                // per pixel, so this sample will work fine with just a quad for the water
                // model.  If it's not set, textures are projected per vertex rather than 
                // per pixel, so distortion will be visible unless we use more vertices.
                if (Caps.TextureCaps.SupportsProjected && !isUsingVertexShader)
                {
                    numVertx = 2;               // Number of vertices in the ground grid along X
                    numVertz = 2;               // Number of vertices in the ground grid along Z
                }
                else
                {
                    numVertx = 8;               // Number of vertices in the ground grid along X
                    numVertz = 8;               // Number of vertices in the ground grid along Z
                }
                numTriangles = (numVertx-1)*(numVertz-1)*2;   // Number of triangles in the ground


                // Create a square for rendering the water
                if ((waterBuffer == null) || (waterBuffer.Disposed))
                {
                    // We only need to create this buffer once
                    waterBuffer = new VertexBuffer(typeof(CustomVertex.PositionTextured), 3 * numTriangles, device, Usage.WriteOnly, CustomVertex.PositionTextured.Format, Pool.Default);
                    waterBuffer.Created += new System.EventHandler(this.WaterBufferCreated);
                    // Call it manually the first time
                    this.WaterBufferCreated(waterBuffer, null);
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
		/// Set the data for the background's vertex buffer
		/// </summary>
		private void BackgroundVertexCreated(object sender, EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			CustomVertex.PositionTextured[] v = new CustomVertex.PositionTextured[4];
			v[0].SetPosition(new Vector3(-1000.0f, 0.0f, 0.0f));
            v[1].SetPosition(new Vector3(-1000.0f, 1000.0f, 0.0f));
            v[2].SetPosition(new Vector3(1000.0f, 0.0f, 0.0f));
            v[3].SetPosition(new Vector3(1000.0f, 1000.0f, 0.0f));
            v[0].Tu = 0.0f; v[0].Tv = 147/256.0f;
            v[1].Tu = 0.0f; v[1].Tv = 0.0f;
            v[2].Tu = 1.0f; v[2].Tv = 147/256.0f;
            v[3].Tu = 1.0f; v[3].Tv = 0.0f;

			vb.SetData(v, 0, 0);
		}




        /// <summary>
        /// Set the data for the water's vertex buffer
        /// </summary>
        private void WaterBufferCreated(object sender, EventArgs e)
        {
            VertexBuffer vb = (VertexBuffer)sender;
            CustomVertex.PositionTextured[] v = (CustomVertex.PositionTextured[])vb.Lock(0, 0);

            float dX = 2000.0f/(numVertx-1);
            float dZ = 1250.0f/(numVertz-1);
            float x0 = -1000;
            float z0 = -1250;
            float dU = 1.0f/(numVertx-1);
            float dV = 0.7f/(numVertz-1);
            int k = 0;
            for (int z=0; z < (numVertz-1); z++)
            {
                for (int x=0; x < (numVertx-1); x++)
                {
                    v[k].SetPosition(new Vector3(x0 + x*dX, 0.0f, z0 + z*dZ));
                    v[k].Tu = x*dU; 
                    v[k].Tv = z*dV;
                    k++;
                    v[k].SetPosition(new Vector3(x0 + x*dX, 0.0f, z0 + (z+1)*dZ));
                    v[k].Tu = x*dU; 
                    v[k].Tv = (z+1)*dV;
                    k++;
                    v[k].SetPosition(new Vector3(x0 + (x+1)*dX, 0.0f, z0 + (z+1)*dZ));
                    v[k].Tu = (x+1)*dU; 
                    v[k].Tv = (z+1)*dV;
                    k++;
                    v[k].SetPosition(new Vector3(x0 + x*dX, 0.0f, z0 + z*dZ));
                    v[k].Tu = x*dU; 
                    v[k].Tv = z*dV;
                    k++;
                    v[k].SetPosition(new Vector3(x0 + (x+1)*dX, 0.0f, z0 + (z+1)*dZ));
                    v[k].Tu = (x+1)*dU; 
                    v[k].Tv = (z+1)*dV;
                    k++;
                    v[k].SetPosition(new Vector3(x0 + (x+1)*dX, 0.0f, z0 + z*dZ));
                    v[k].Tu = (x+1)*dU; 
                    v[k].Tv = z*dV;
                    k++;
                }
            }

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

			// Set the transform matrices
			Vector3 vEyePt = new Vector3(0.0f, 400.0f, -1650.0f);
			Vector3 vLookatPt = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vUpVec = new Vector3(0.0f, 1.0f, 0.0f);

            Matrix matWorld = Matrix.Identity;
            Matrix matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
            Matrix matProj = Matrix.PerspectiveFovLH(1.00f, 1.0f, 1.0f, 10000.0f);
			device.Transform.World = matWorld;
			device.Transform.View = matView;
			device.Transform.Projection = matProj;

			// Set any appropiate state
			device.RenderState.Ambient = System.Drawing.Color.White;
			device.RenderState.DitherEnable = true;
			device.RenderState.SpecularEnable = false;
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;
			device.SamplerState[1].MinFilter = TextureFilter.Linear;
			device.SamplerState[1].MagFilter = TextureFilter.Linear;

            if (isUsingVertexShader)
            {
                Matrix matCamera, matFinal;

                // Create our declaration
                VertexElement[] decl = new VertexElement[] {new VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0), 
                                                                new VertexElement(0, 12, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0),
                                                                VertexElement.VertexDeclarationEnd };

                vertexDecl = new VertexDeclaration(device, decl);

                vertexShader = GraphicsUtility.CreateVertexShader(device, "bumpwaves.vsh");

                matCamera = matWorld * matView;
                matFinal = matCamera * matProj;
                matCamera.Transpose(matCamera);
                matFinal.Transpose(matFinal);
                float[][] camera = new float[][] { new float[] {matCamera.M11, matCamera.M12, matCamera.M13, matCamera.M14}, new float[] {matCamera.M21, matCamera.M22, matCamera.M23, matCamera.M24},  new float[] {matCamera.M31, matCamera.M32, matCamera.M33, matCamera.M34} } ;

                camera[0][0] *= 0.8f;
                camera[0][1] *= 0.8f;
                camera[0][2] *= 0.8f;
                camera[0][3] *= 0.8f;
                camera[1][0] *= 0.8f;
                camera[1][1] *= 0.8f;
                camera[1][2] *= 0.8f;
                camera[1][3] *= 0.8f;
                for (int i = 0; i < 3; i++)
                    device.SetVertexShaderConstant(i, camera[i]);

                device.SetVertexShaderConstant(3, new Matrix[] { matFinal });

                float[] data = new float[] {0.5f, -0.5f, 0, 0};
                device.SetVertexShaderConstant(8, data);
            }
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
            // Device must be able to do bumpmapping
            if (!caps.TextureOperationCaps.SupportsBumpEnvironmentMap)
				return false;

            // Accept devices that can create Format.V8U8 textures
            return Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, 
				adapterFormat, 0, ResourceType.Textures, Format.V8U8);
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
