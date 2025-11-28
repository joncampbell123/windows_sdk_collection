//-----------------------------------------------------------------------------
// File: VertexBlend.cs
//
// Desc: Example code showing how to do a skinning effect, using the vertex
//       blending feature of Direct3D. Normally, Direct3D transforms each
//       vertex through the world matrix. The vertex blending feature,
//       however, uses mulitple world matrices and a per-vertex blend factor
//       to transform each vertex.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace VertexBlendSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
        /// <summary>
        /// Custom vertex which includes a blending factor
        /// </summary>
		public struct BlendVertex
		{
			public Vector3 v;       // Referenced as v0 in the vertex shader
			public float blend;   // Referenced as v1.x in the vertex shader
			public Vector3 n;       // Referenced as v3 in the vertex shader
			public float tu, tv;  // Referenced as v7 in the vertex shader

			public static readonly VertexFormats Format = VertexFormats.PositionBlend1 | VertexFormats.Normal |  VertexFormats.Texture1;
		};

		private GraphicsFont drawingFont = null;                 // Font for drawing text
		private GraphicsMesh blendObject = null;           // Object to use for vertex blending
		private int numVertices = 0;
		private int numFaces = 0;
		private VertexBuffer vertexBuffer = null;    
		private IndexBuffer indexBuffer = null;

		private Matrix upperArm = Matrix.Identity;       // Vertex blending matrices
		private Matrix lowerArm = Matrix.Identity;

		private VertexShader shader = null;    // Vertex shader
		private VertexDeclaration declaration = null;
		private bool useShader = false;
		private System.Windows.Forms.MenuItem mnuOptions;
		private System.Windows.Forms.MenuItem mnuUseVS;


		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "VertexBlend: Surface Skinning Example";
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

			// Add our new menu options
			this.mnuOptions = new System.Windows.Forms.MenuItem();
			this.mnuUseVS = new System.Windows.Forms.MenuItem();
			// Add the Options menu to the main menu
			this.mnuMain.MenuItems.Add(this.mnuOptions);
			this.mnuOptions.Index = 1;
			this.mnuOptions.Text = "&Options";

			this.mnuOptions.MenuItems.Add(this.mnuUseVS);
			this.mnuUseVS.Text = "Use custom &vertex shader";
			this.mnuUseVS.Shortcut = System.Windows.Forms.Shortcut.CtrlV;
			this.mnuUseVS.ShowShortcut = true;
			this.mnuUseVS.Click += new System.EventHandler(this.UseCustomShaderClick);
		}




		/// <summary>
		/// Called when our menu item is clicked.
		/// </summary>
		private void UseCustomShaderClick(object sender, System.EventArgs e)
		{
			useShader = !useShader;
			mnuUseVS.Checked = !mnuUseVS.Checked;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Set the vertex blending matrices for this frame
			Vector3 vAxis = new Vector3(2 + (float)Math.Sin(appTime*3.1f), 2 + (float)Math.Sin(appTime*3.3f), (float)Math.Sin(appTime*3.5f)); 
			lowerArm = Matrix.RotationAxis(vAxis, (float)Math.Sin(3*appTime));
			upperArm =Matrix.Identity;

			// Set the vertex shader constants. Note: outside of the blend matrices,
			// most of these values don't change, so don't need to really be set every
			// frame. It's just done here for clarity
			if (useShader)
			{
				// Some basic constants
				Vector4 vZero = new Vector4(0,0,0,0);
				Vector4 vOne = new Vector4(1,1,1,1);

				// Lighting vector (normalized) and material colors. (Use non-yellow light
				// to show difference from non-vertex shader case.)
				Vector4 vLight = new Vector4(0.5f, 1.0f, -1.0f, 0.0f);
				vLight.Normalize();
				float[] fDiffuse = { 0.5f, 1.00f, 0.00f, 0.00f }; 
				float[] fAmbient = { 0.25f, 0.25f, 0.25f, 0.25f };

				// Vertex shader operations use transposed matrices
				Matrix matWorld0Transpose = new Matrix(), matWorld1Transpose = new Matrix();
				Matrix matView, matProj, matViewProj, matViewProjTranspose = new Matrix();
				matView = device.Transform.View;
				matProj = device.Transform.Projection;
				matViewProj = Matrix.Multiply(matView, matProj);
				matWorld0Transpose.Transpose(upperArm);
				matWorld1Transpose.Transpose(lowerArm);
				matViewProjTranspose.Transpose(matViewProj);

				// Set the vertex shader constants
				device.SetVertexShaderConstant(0, new Vector4[] { vZero });
				device.SetVertexShaderConstant(1, new Vector4[] { vOne });
				device.SetVertexShaderConstant(4, new Matrix[] { matWorld0Transpose });
				device.SetVertexShaderConstant(8, new Matrix[] { matWorld1Transpose });
				device.SetVertexShaderConstant(12, new Matrix[] { matViewProjTranspose });
				device.SetVertexShaderConstant(20, new Vector4[] { vLight });
				device.SetVertexShaderConstant(21, fDiffuse);
				device.SetVertexShaderConstant(22, fAmbient);
			}

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

			if (useShader)        
			{
				device.VertexFormat = BlendVertex.Format;
				device.VertexShader = shader;
				device.SetStreamSource(0, vertexBuffer, 0, DXHelp.GetTypeSize(typeof(BlendVertex)));
				device.Indices = indexBuffer;
				device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numVertices, 0, numFaces);
			}
			else
			{
				device.VertexShader = null;
				// Enable vertex blending using API
				device.Transform.World = upperArm;
				device.Transform.World1 = lowerArm;
				renderState.VertexBlend = VertexBlend.OneWeights;

				// Display the object
				blendObject.Render(device);
			}


			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

			if (useShader)
				drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using vertex shader");
			else
				drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using RenderState.VertexBlend");

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
			// Load an object to render
			try
			{
				if (blendObject == null)
					blendObject = new GraphicsMesh();

				blendObject.Create(device, "mslogo.x");
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}


			if ((BehaviorFlags.HardwareVertexProcessing || BehaviorFlags.MixedVertexProcessing) &&
				Caps.VertexShaderVersion.Major < 1)
			{
				// No VS available, so don't try to use it or allow user to
				// switch to it
				useShader = false;
				mnuUseVS.Enabled = false;
			}
			else if (Caps.MaxVertexBlendMatrices < 2)
			{
				// No blend matrices available, so don't try to use them or 
				// allow user to switch to them
				useShader = true;
				mnuUseVS.Enabled = false;
			}
			else
			{
				// Both techniques available, so default to blend matrices and 
				// allow the user to switch techniques
				useShader = false;
				mnuUseVS.Enabled = true;
			}

			// Set a custom FVF for the mesh
			blendObject.SetVertexFormat(device, BlendVertex.Format);

			// Add blending weights to the mesh
			// Gain acces to the mesh's vertices
			VertexBuffer vb = null;
			BlendVertex[] vertices = null;
			int numVertices = blendObject.SystemMesh.NumberVertices;
			vb = blendObject.SystemMesh.VertexBuffer;
			vertices = (BlendVertex[])vb.Lock(0,typeof(BlendVertex), 0, numVertices);

			// Calculate the min/max z values for all the vertices
			float fMinX =  1e10f;
			float fMaxX = -1e10f;

			for (int i=0; i<numVertices; i++)
			{
				if (vertices[i].v.X < fMinX) 
					fMinX = vertices[i].v.X;
				if (vertices[i].v.X > fMaxX) 
					fMaxX = vertices[i].v.X;
			}

			for (int i=0; i<numVertices; i++)
			{
				// Set the blend factors for the vertices
				float a = (vertices[i].v.X - fMinX) / (fMaxX - fMinX);
				vertices[i].blend = 1.0f-(float)Math.Sin(a*(float)Math.PI*1.0f);
			}

			// Done with the mesh's vertex buffer data
			vb.Unlock();
			vb.Dispose();
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
			// Restore mesh's local memory objects
			blendObject.RestoreDeviceObjects(device, null);

			// Get access to the mesh vertex and index buffers
			vertexBuffer = blendObject.LocalMesh.VertexBuffer;
			indexBuffer = blendObject.LocalMesh.IndexBuffer;
			numVertices = blendObject.LocalMesh.NumberVertices;
			numFaces = blendObject.LocalMesh.NumberFaces;

			if (BehaviorFlags.SoftwareVertexProcessing ||
				Caps.VertexShaderVersion.Major >= 1)
			{
				// Setup the vertex declaration
				VertexElement[] decl = VertexInformation.DeclaratorFromFormat(BlendVertex.Format);
				declaration = new VertexDeclaration(device, decl);

				// Create vertex shader from a file
				try
				{
					shader = GraphicsUtility.CreateVertexShader(device, "blend.vsh");
				}
				catch
				{
					SampleException d3de = new MediaNotFoundException();
					HandleSampleException(d3de, ApplicationMessage.ApplicationMustExit);
					throw d3de;
				}
			}
			// Set miscellaneous render states
			renderState.ZBufferEnable = true;
			renderState.Ambient = System.Drawing.Color.FromArgb(0x00404040);

			// Set the projection matrix
			float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 1.0f, 10000.0f);

			// Set the app view matrix for normal viewing
			Vector3 vEyePt    = new Vector3(0.0f,-5.0f,-10.0f);
			Vector3 vLookatPt = new Vector3(0.0f, 0.0f,  0.0f);
			Vector3 vUpVec    = new Vector3(0.0f, 1.0f,  0.0f);
			device.Transform.View = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);

			// Create a directional light. (Use yellow light to distinguish from
			// vertex shader case.)
			GraphicsUtility.InitLight(device.Lights[0], LightType.Directional, -0.5f, -1.0f, 1.0f);
			device.Lights[0].Diffuse = System.Drawing.Color.Yellow;
			device.Lights[0].Commit();
			device.Lights[0].Enabled = true;
			renderState.Lighting = true;
		}




        /// <summary>
        /// Invalidate the device objects
        /// </summary>
        protected override void InvalidateDeviceObjects(System.Object sender, System.EventArgs e)
		{
			if (shader != null)
				shader.Dispose();
		}




		/// <summary>
		/// Called when the app is exiting, or the device is being changed, this 
		/// function deletes any device-dependent objects.
		/// </summary>
		protected override void DeleteDeviceObjects(System.Object sender, System.EventArgs e)
		{
			blendObject.Dispose();
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			if (vertexProcessingType == VertexProcessingType.PureHardware)
				return false; // GetTransform doesn't work on PUREDEVICE

			// Check that the device supports at least one of the two techniques
			// used in this sample: either a vertex shader, or at least two blend
			// matrices and a directional light.

			if ((vertexProcessingType == VertexProcessingType.Hardware) ||
				(vertexProcessingType == VertexProcessingType.Mixed))
			{
				if (caps.VertexShaderVersion.Major >= 1)
					return true;
			}
			else
			{
				// Software vertex processing always supports vertex shaders
				return true;
			}

			// Check that the device can blend vertices with at least two matrices
			// (Software can always do up to 4 blend matrices)
			if (caps.MaxVertexBlendMatrices < 2)
				return false;

			// If this is a TnL device, make sure it supports directional lights
			if ((vertexProcessingType == VertexProcessingType.Hardware) ||
				(vertexProcessingType == VertexProcessingType.Mixed))
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
}
