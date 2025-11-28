//-----------------------------------------------------------------------------
// File: DolphinVS.cs
//
// Desc: Sample of swimming dolphin
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




namespace DolphinVS
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private readonly System.Drawing.Color WaterColor = System.Drawing.Color.FromArgb(0x00004080);

		public struct Vertex
		{
			public Vector3 p;
			public Vector3 n;
			public float tu, tv;
			public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Normal | VertexFormats.Texture1;
		};

		private GraphicsFont drawingFont = null;                 // Font for drawing text

		// Transform matrices
		private Matrix worldMatrix = Matrix.Identity;
		private Matrix viewMatrix = Matrix.Identity;
		private Matrix projectionMatrix = Matrix.Identity;

		// Dolphin object
		private Texture dolphinTexture = null;
		private VertexBuffer dolphinVertexBuffer1 = null;
		private VertexBuffer dolphinVertexBuffer2 = null;
		private VertexBuffer dolphinVertexBuffer3 = null;
		private IndexBuffer dolphinIndexBuffer = null;
		private int numDolphinVertices = 0;
		private int numDolphinFaces = 0;
		private VertexDeclaration dolphinVertexDeclaration = null;
		private VertexShader dolphinVertexShader = null;
		private VertexShader dolphinVertexShader2 = null;

		// Seafloor object
		private Texture seaFloorTexture = null;
		private VertexBuffer seaFloorVertexBuffer = null;
		private IndexBuffer seaFloorIndexBuffer = null;
		private int numSeaFloorVertices = 0;
		private int numSeaFloorFaces = 0;
		private VertexDeclaration seaFloorVertexDeclaration = null;
		private VertexShader seaFloorVertexShader = null;
		private VertexShader seaFloorVertexShader2 = null;

		// Water caustics
		private Texture[] causticTextures = new Texture[32];
		private Texture currentCausticTexture = null;

		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "DolphinVS: Tweening Vertex Shader";
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
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Animation attributes for the dolphin
			float fKickFreq    = 2*appTime;
			float fPhase       = appTime/3;
			float fBlendWeight = (float)Math.Sin(fKickFreq);

			// Move the dolphin in a circle
			Matrix matDolphin, matTrans, matRotate1, matRotate2;
			matDolphin = Matrix.Scaling(0.01f, 0.01f, 0.01f);
			matRotate1 = Matrix.RotationZ(-(float)Math.Cos(fKickFreq)/6);
			matDolphin *= matRotate1;
			matRotate2 = Matrix.RotationY(fPhase);
			matDolphin *= matRotate2;
			matTrans = Matrix.Translation(-5*(float)Math.Sin(fPhase), (float)Math.Sin(fKickFreq)/2, 10-10*(float)Math.Cos(fPhase));
			matDolphin *= matTrans;

			// Animate the caustic textures
			int tex = ((int)(appTime * 32)) % 32;
			currentCausticTexture = causticTextures[tex];

			// Set the vertex shader constants. Note: outside of the blend matrices,
			// most of these values don't change, so don't need to really be set every
			// frame. It's just done here for clarity

			// Some basic constants
			Vector4 vZero = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			Vector4 vOne = new Vector4(1.0f, 0.5f, 0.2f, 0.05f);

			float fWeight1;
			float fWeight2;
			float fWeight3;

			if (fBlendWeight > 0.0f)
			{
				fWeight1 = (float)Math.Abs(fBlendWeight);
				fWeight2 = 1.0f - (float)Math.Abs(fBlendWeight);
				fWeight3 = 0.0f;
			}
			else
			{
				fWeight1 = 0.0f;
				fWeight2 = 1.0f - (float)Math.Abs(fBlendWeight);
				fWeight3 = (float)Math.Abs(fBlendWeight);
			}
			Vector4 vWeight = new Vector4(fWeight1, fWeight2, fWeight3, 0.0f);

			// Lighting vectors (in world space and in dolphin model space)
			// and other constants
			Vector4 fLight    = new Vector4(0.0f,  1.0f, 0.0f, 0.0f);
			Vector4 fLightDolphinSpace    = new Vector4(0.0f,  1.0f, 0.0f, 0.0f);
			float[] fDiffuse  = { 1.00f, 1.00f, 1.00f, 1.00f };
			float[] fAmbient  = { 0.25f, 0.25f, 0.25f, 0.25f };
			float[] fFog      = { 0.5f, 50.0f, 1.0f/(50.0f-1.0f), 0.0f };
			float[] fCaustics = { 0.05f, 0.05f, (float)Math.Sin(appTime)/8, (float)Math.Cos(appTime)/10 };

			Matrix matDolphinInv = Matrix.Invert(matDolphin);
			fLightDolphinSpace = Vector4.Transform(fLight, matDolphinInv);
			fLightDolphinSpace.Normalize();

			// Vertex shader operations use transposed matrices
			Matrix mat, matCamera = new Matrix(), matTranspose = new Matrix(), matCameraTranspose = new Matrix();
			Matrix matViewTranspose = new Matrix(), matProjTranspose = new Matrix();
			matCamera = Matrix.Multiply(matDolphin, viewMatrix);
			mat = Matrix.Multiply(matCamera, projectionMatrix);
			matTranspose.Transpose(mat);
			matCameraTranspose.Transpose(matCamera);
			matViewTranspose.Transpose(viewMatrix);
			matProjTranspose.Transpose(projectionMatrix);

			// Set the vertex shader constants
			device.SetVertexShaderConstant(0, new Vector4[] { vZero });
			device.SetVertexShaderConstant(1, new Vector4[] { vOne });
			device.SetVertexShaderConstant(2, new Vector4[] { vWeight });
			device.SetVertexShaderConstant(4, new Matrix[] { matTranspose }) ;
			device.SetVertexShaderConstant(8, new Matrix[] { matCameraTranspose });
			device.SetVertexShaderConstant(12, new Matrix[] { matViewTranspose });
			device.SetVertexShaderConstant(19, new Vector4[] { fLightDolphinSpace });
			device.SetVertexShaderConstant(20, new Vector4[] { fLight });
			device.SetVertexShaderConstant(21, fDiffuse);
			device.SetVertexShaderConstant(22, fAmbient);
			device.SetVertexShaderConstant(23, fFog);
			device.SetVertexShaderConstant(24, fCaustics);
			device.SetVertexShaderConstant(28, new Matrix[] { matProjTranspose });
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer , WaterColor, 1.0f, 0);

			device.BeginScene();

			float[] fAmbientLight  = { 0.25f, 0.25f, 0.25f, 0.25f };
			device.SetVertexShaderConstant(22, fAmbientLight);

			// Render the seafloor
			device.SetTexture(0, seaFloorTexture);
			device.VertexDeclaration = seaFloorVertexDeclaration;
			device.VertexShader = seaFloorVertexShader;
			device.SetStreamSource(0, seaFloorVertexBuffer,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.Indices = seaFloorIndexBuffer;
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList,0,
				0, numSeaFloorVertices,
				0, numSeaFloorFaces);

			// Render the dolphin
			device.SetTexture(0, dolphinTexture);
			device.VertexDeclaration = dolphinVertexDeclaration;
			device.VertexShader = dolphinVertexShader;
			device.SetStreamSource(0, dolphinVertexBuffer1,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.SetStreamSource(1, dolphinVertexBuffer2,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.SetStreamSource(2, dolphinVertexBuffer3,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.Indices = dolphinIndexBuffer;
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 
				0, numDolphinVertices,
				0, numDolphinFaces);

			// Now, we are going to do a 2nd pass, to alpha-blend in the caustics.
			// The caustics use a 2nd set of texture coords that are generated
			// by the vertex shaders. Lighting from the light above is used, but
			// ambient is turned off to avoid lighting objects from below (for
			// instance, we don't want caustics appearing on the dolphin's
			// underbelly). Finally, fog color is set to black, so that caustics
			// fade in distance.

			// Turn on alpha blending
			device.RenderState.AlphaBlendEnable = true;
        
			device.RenderState.SourceBlend = Blend.One;
			device.RenderState.DestinationBlend = Blend.One;

			// Setup the caustic texture
			device.SetTexture(0, currentCausticTexture);

			// Set ambient and fog colors to black
			float[] fAmbientDark = { 0.0f, 0.0f, 0.0f, 0.0f };
			device.SetVertexShaderConstant(22, fAmbientDark);
			device.RenderState.FogColor = System.Drawing.Color.Black;

			// Render the caustic effects for the seafloor
			device.VertexDeclaration = seaFloorVertexDeclaration;
			device.VertexShader = seaFloorVertexShader2;
			device.SetStreamSource(0, seaFloorVertexBuffer,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.Indices = seaFloorIndexBuffer;
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 
				0, numSeaFloorVertices,
				0, numSeaFloorFaces);

			// Finally, render the caustic effects for the dolphin
			device.VertexDeclaration = dolphinVertexDeclaration;
			device.VertexShader = dolphinVertexShader2;
			device.SetStreamSource(0, dolphinVertexBuffer1,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.SetStreamSource(1, dolphinVertexBuffer2,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.SetStreamSource(2, dolphinVertexBuffer3,  0, DXHelp.GetTypeSize(typeof(Vertex)));
			device.Indices = dolphinIndexBuffer;
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 
				0, numDolphinVertices,
				0, numDolphinFaces);

			// Restore modified render states
			device.RenderState.AlphaBlendEnable = false;
			device.RenderState.FogColor = WaterColor;


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
			VertexBuffer pMeshSourceVB = null;
			IndexBuffer pMeshSourceIB = null;
			Vertex[] src = null;
			GraphicsStream dst = null;
			GraphicsMesh DolphinMesh01 = new GraphicsMesh();
			GraphicsMesh DolphinMesh02 = new GraphicsMesh();
			GraphicsMesh DolphinMesh03 = new GraphicsMesh();
			GraphicsMesh SeaFloorMesh = new GraphicsMesh();

			// Initialize the font's internal textures
			drawingFont.InitializeDeviceObjects(device);

			try
			{
				// Create texture for the dolphin
				dolphinTexture = GraphicsUtility.CreateTexture(device, "Dolphin.bmp");

				// Create textures for the seafloor
				seaFloorTexture = GraphicsUtility.CreateTexture(device, "SeaFloor.bmp");

				// Create textures for the water caustics
				for (int t=0; t<32; t++)
				{
					string name = string.Format("Caust{0:D2}.tga", t);
					causticTextures[t] = GraphicsUtility.CreateTexture(device, name);
				}

				// Load the file-based mesh objects
				DolphinMesh01.Create(device, "dolphin1.x");
				DolphinMesh02.Create(device, "dolphin2.x");
				DolphinMesh03.Create(device, "dolphin3.x");
				SeaFloorMesh.Create(device, "SeaFloor.x");
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}
			// Set the FVF type to match the vertex format we want
			DolphinMesh01.SetVertexFormat(device, Vertex.Format);
			DolphinMesh02.SetVertexFormat(device, Vertex.Format);
			DolphinMesh03.SetVertexFormat(device, Vertex.Format);
			SeaFloorMesh.SetVertexFormat(device, Vertex.Format);

			// Get the number of vertices and faces for the meshes
			numDolphinVertices  = DolphinMesh01.SystemMesh.NumberVertices;
			numDolphinFaces     = DolphinMesh01.SystemMesh.NumberFaces;
			numSeaFloorVertices = SeaFloorMesh.SystemMesh.NumberVertices;
			numSeaFloorFaces    = SeaFloorMesh.SystemMesh.NumberFaces;

			// Create the dolphin and seafloor vertex and index buffers
			dolphinVertexBuffer1 = new VertexBuffer(typeof(Vertex), numDolphinVertices, device, Usage.WriteOnly, 0, Pool.Managed);
			dolphinVertexBuffer2 = new VertexBuffer(typeof(Vertex), numDolphinVertices, device, Usage.WriteOnly, 0, Pool.Managed);
			dolphinVertexBuffer3 = new VertexBuffer(typeof(Vertex), numDolphinVertices, device, Usage.WriteOnly, 0, Pool.Managed);
			seaFloorVertexBuffer = new VertexBuffer(typeof(Vertex), numSeaFloorVertices, device, Usage.WriteOnly, 0, Pool.Managed);
			dolphinIndexBuffer = new IndexBuffer(typeof(short), numDolphinFaces * 3, device, Usage.WriteOnly, Pool.Managed);
			seaFloorIndexBuffer = new IndexBuffer(typeof(short), numSeaFloorFaces * 3, device, Usage.WriteOnly, Pool.Managed);

			// Copy vertices for mesh 01
			pMeshSourceVB = DolphinMesh01.SystemMesh.VertexBuffer;
			dst = dolphinVertexBuffer1.Lock(0, DXHelp.GetTypeSize(typeof(Vertex)) * numDolphinVertices, 0);
			src = (Vertex[])pMeshSourceVB.Lock(0, typeof(Vertex), 0, numDolphinVertices);
			dst.Write(src);
			dolphinVertexBuffer1.Unlock();
			pMeshSourceVB.Unlock();
			pMeshSourceVB.Dispose();

			// Copy vertices for mesh 2
			pMeshSourceVB = DolphinMesh02.SystemMesh.VertexBuffer;
			dst = dolphinVertexBuffer2.Lock(0, DXHelp.GetTypeSize(typeof(Vertex)) * numDolphinVertices, 0);
			src = (Vertex[])pMeshSourceVB.Lock(0, typeof(Vertex), 0, numDolphinVertices);
			dst.Write(src);
			dolphinVertexBuffer2.Unlock();
			pMeshSourceVB.Unlock();
			pMeshSourceVB.Dispose();

			// Copy vertices for mesh 3
			pMeshSourceVB = DolphinMesh03.SystemMesh.VertexBuffer;
			dst = dolphinVertexBuffer3.Lock(0, DXHelp.GetTypeSize(typeof(Vertex)) * numDolphinVertices, 0);
			src = (Vertex[])pMeshSourceVB.Lock(0, typeof(Vertex), 0 , numDolphinVertices);
			dst.Write(src);
			dolphinVertexBuffer3.Unlock();
			pMeshSourceVB.Unlock();
			pMeshSourceVB.Dispose();

			// Copy vertices for the seafloor mesh, and add some bumpiness
			pMeshSourceVB = SeaFloorMesh.SystemMesh.VertexBuffer;
			dst = seaFloorVertexBuffer.Lock(0, DXHelp.GetTypeSize(typeof(Vertex)) * numSeaFloorVertices, 0);
			src = (Vertex[])pMeshSourceVB.Lock(0, typeof(Vertex), 0, numSeaFloorVertices);

			System.Random r = new System.Random();
			for (int i=0; i<numSeaFloorVertices; i++)
			{
				src[i].p.Y += (r.Next()/(float)int.MaxValue);
				src[i].p.Y += (r.Next()/(float)int.MaxValue);
				src[i].p.Y += (r.Next()/(float)int.MaxValue);
				src[i].tu  *= 10;
				src[i].tv  *= 10;
			}
			dst.Write(src);
			seaFloorVertexBuffer.Unlock();
			pMeshSourceVB.Unlock();
			pMeshSourceVB.Dispose();

			GraphicsStream dstib = null;
			short[] srcib = null;

			// Copy indices for the dolphin mesh
			pMeshSourceIB = DolphinMesh01.SystemMesh.IndexBuffer;
			dstib = dolphinIndexBuffer.Lock(0, DXHelp.GetTypeSize(typeof(short)) * numDolphinFaces * 3, 0);
			srcib = (short[])pMeshSourceIB.Lock(0, typeof(short), 0, numDolphinFaces * 3);
			dstib.Write(srcib);
			dolphinIndexBuffer.Unlock();
			pMeshSourceIB.Unlock();
			pMeshSourceIB.Dispose();

			// Copy indices for the seafloor mesh
			pMeshSourceIB = SeaFloorMesh.SystemMesh.IndexBuffer;
			dstib = seaFloorIndexBuffer.Lock(0, DXHelp.GetTypeSize(typeof(short)) * numSeaFloorFaces * 3, 0);
			srcib = (short[])pMeshSourceIB.Lock(0, typeof(short), 0, numSeaFloorFaces * 3);
			dstib.Write(srcib);
			seaFloorIndexBuffer.Unlock();
			pMeshSourceIB.Unlock();
			pMeshSourceIB.Dispose();
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
			Vector3 vEyePt      = new Vector3(0.0f, 0.0f, -5.0f);
			Vector3 vLookatPt   = new Vector3(0.0f, 0.0f,  0.0f);
			Vector3 vUpVec      = new Vector3(0.0f, 1.0f,  0.0f);
			float fAspect = ((float)device.PresentationParameters.BackBufferWidth) / device.PresentationParameters.BackBufferHeight;
			worldMatrix  = Matrix.Identity;
			viewMatrix = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
			projectionMatrix = Matrix.PerspectiveFovLH((float)Math.PI /3, fAspect, 1.0f, 10000.0f);

			// Set default render states
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;
			device.RenderState.ZBufferEnable = true;
			device.RenderState.FogEnable = true;
			device.RenderState.FogColor = WaterColor;

			// Create vertex shader for the dolphin
			VertexElement[] dolphinVertexDecl = new VertexElement[] 
			{
				// First stream is first mesh
				new VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0), 
				new VertexElement(0, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 0), 
				new VertexElement(0, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0), 
				// Second stream is second mesh
				new VertexElement(1, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 1), 
				new VertexElement(1, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 1), 
				new VertexElement(1, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 1), 
				// Third stream is third mesh
				new VertexElement(2, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 2), 
				new VertexElement(2, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 2), 
				new VertexElement(2, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 2), 
				VertexElement.VertexDeclarationEnd };

			dolphinVertexDeclaration = new VertexDeclaration(device, dolphinVertexDecl);
			dolphinVertexShader = GraphicsUtility.CreateVertexShader(device, "DolphinTween.vsh");
			dolphinVertexShader2 = GraphicsUtility.CreateVertexShader(device, "DolphinTween2.vsh");


			// Create vertex shader for the seafloor
			VertexElement[] seaFloorVertexDecl = new VertexElement[] 
			{
				new VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0), 
				new VertexElement(0, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 0), 
				new VertexElement(0, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0), 
				VertexElement.VertexDeclarationEnd };

			seaFloorVertexDeclaration = new VertexDeclaration(device, seaFloorVertexDecl);
			seaFloorVertexShader = GraphicsUtility.CreateVertexShader(device, "SeaFloor.vsh");
			seaFloorVertexShader2 = GraphicsUtility.CreateVertexShader(device, "SeaFloor2.vsh");
		}




		/// <summary>
		/// Invalidates device objects
		/// </summary>
		protected override void InvalidateDeviceObjects(System.Object sender, System.EventArgs e)
		{
			// Clean up vertex shaders
			if (dolphinVertexShader != null)
			{
				dolphinVertexShader.Dispose();
				dolphinVertexShader = null;
			}

			if (dolphinVertexShader2 != null)
			{
				dolphinVertexShader2.Dispose();
				dolphinVertexShader2 = null;
			}
    
			if (seaFloorVertexShader != null)
			{
				seaFloorVertexShader.Dispose();
				seaFloorVertexShader = null;
			}
    
			if (seaFloorVertexShader2 != null)
			{
				seaFloorVertexShader2.Dispose();
				seaFloorVertexShader2 = null;
			}
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// Need to support post-pixel processing (for fog)
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
				if (caps.VertexShaderVersion.Major < 1)
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
