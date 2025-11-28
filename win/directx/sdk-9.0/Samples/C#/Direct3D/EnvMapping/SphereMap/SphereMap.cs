//-----------------------------------------------------------------------------
// File: SphereMap.cs
//
// Desc: Example code showing how to use sphere-mapping in D3D, using generated 
//       texture coordinates.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace SphereMapSample
{
    /// <summary>
    /// Application class. The base class (GraphicsSample) provides the 
    /// generic functionality needed in all Direct3D samples. MyGraphicsSample 
    /// adds functionality specific to this sample program.
    /// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
        /// <summary>
        /// String containing effect used to render shiny teapot.  Two techniques
        /// are shown.. one which simply uses SphereMaps, and a fallback which uses 
        /// a vertex shader to do a sphere-map lookup.
        /// </summary>
		string[] effectString = 
		{
			"texture texSphereMap;\n",
			"matrix matWorld;\n",
			"matrix matViewProject;\n",
			"vector vecPosition;\n",

			"technique Sphere\n",
			"{\n",
				"pass P0\n",
				"{\n",

				// Vertex state
				"VertexShader =\n",
				"decl\n",
				"{\n",
					// Decls no longer associated with vertex shaders in DX9
				"}\n"               ,
				"asm\n",
				"{\n",
					"vs.1.0\n",
					"def c64, 0.25f, 0.5f, 1.0f, -1.0f\n",
		        
					"dcl_position v0\n",
					"dcl_normal v1\n",
			        
					// r0: camera-space position
					// r1: camera-space normal
					// r2: camera-space vertex-eye vector
					// r3: camera-space reflection vector
					// r4: texture coordinates

					// Transform position and normal into camera-space
					"m4x4 r0, v0, c0\n",
					"m3x3 r1.xyz, v1, c0\n",
					"mov r1.w, c64.z\n",

					// Compute normalized view vector
					"add r2, c8, -r0\n",
					"dp3 r3, r2, r2\n",
					"rsq r3, r3.w\n",
					"mul r2, r2, r3\n",

					// Compute camera-space reflection vector
					"dp3 r3, r1, r2\n",
					"mul r1, r1, r3\n",
					"add r1, r1, r1\n",
					"add r3, r1, -r2\n",

					// Compute sphere-map texture coords
					"mad r4.w, -r3.z, c64.y, c64.y\n",
					"rsq r4, r4.w\n",
					"mul r4, r3, r4\n",
					"mad r4, r4, c64.x, c64.y\n",

					// Project position
					"m4x4 oPos, r0, c4\n",
					"mul oT0.xy, r4.xy, c64.zw\n",
					"mov oT0.zw, c64.z\n",
				"};\n",

				"VertexShaderConstant4[0] = <matWorld>;\n",
				"VertexShaderConstant4[4] = <matViewProject>;\n",
				"VertexShaderConstant1[8] = <vecPosition>;\n",

				// Pixel state
				"Texture[0] = <texSphereMap>;\n",
				"AddressU[0] = Wrap;\n",
				"AddressV[0] = Wrap;\n",
				"MinFilter[0] = Linear;\n",
				"MagFilter[0] = Linear;\n",
				"ColorOp[0] = SelectArg1;\n",
				"ColorArg1[0] = Texture;\n",
				"}\n",
			"}\n"
		};



        /// <summary>
        /// D3D vertex type for environment-mapped objects
        /// </summary>
		public struct EnvMappedVertex
		{
			public Vector3 p; // Position
			public Vector3 n; // Normal

			public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Normal;
		}

		Matrix matProject;
		Matrix matView;
		Matrix matWorld;
		Matrix matTrackBall;

		GraphicsFont font = null;
		GraphicsMesh shinyTeapot  = null;
		GraphicsMesh skyBox = null;

		Effect effect = null;
		Texture sphereMapTexture = null;




        /// <summary>
        /// Application constructor. Sets attributes for the app.
        /// </summary>
		public MyGraphicsSample()
		{
			this.Text = "SphereMap: Environment Mapping Technique";
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
            enumerationSettings.AppUsesDepthBuffer = true;

			font             = new GraphicsFont("Arial", FontStyle.Bold);
			shinyTeapot      = new GraphicsMesh();
			skyBox           = new GraphicsMesh();

			matWorld = Matrix.Identity;

			matTrackBall  = Matrix.Identity;
			matView = Matrix.Translation(0.0f, 0.0f, 3.0f);
		}




        /// <summary>
        /// Called once per frame, the call is the entry point for animating the scene.
        /// </summary>
		protected override void FrameMove()
		{
			// When the window has focus, let the mouse adjust the camera view
			if (this.Capture)
			{
				Matrix matCursor;
				Quaternion quat = GraphicsUtility.GetRotationFromCursor(this);
				matCursor = Matrix.RotationQuaternion(quat);
				matView = Matrix.Multiply(matTrackBall, matCursor);

				Matrix matTrans;
				matTrans = Matrix.Translation(0.0f, 0.0f, 3.0f);
				matView = Matrix.Multiply(matView, matTrans);
			}
			else
			{
				Matrix matRotation = Matrix.RotationY(-elapsedTime);
				matWorld.Multiply(matRotation);
			}
		}




        /// <summary>
        /// Called once per frame, the call is the entry point for 3d
        /// rendering. This function sets up render states, clears the
        /// viewport, and renders the scene.
        /// </summary>
		protected override void Render()
		{
			// Begin the scene
			device.BeginScene();
			// Render the Skybox
			{
				Matrix world = Matrix.Scaling(10.0f, 10.0f, 10.0f);

				Matrix view = matView;
				view.M41 = view.M42 = view.M43 = 0.0f;

				device.Transform.World = world;
				device.Transform.View = view;
				device.Transform.Projection = matProject;

				device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
				device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
				device.SamplerState[0].MinFilter = TextureFilter.Linear;
				device.SamplerState[0].MagFilter = TextureFilter.Linear;
				device.SamplerState[0].AddressU = TextureAddress.Wrap;
				device.SamplerState[0].AddressV = TextureAddress.Wrap;

				// Always pass Z-test, so we can avoid clearing color and depth buffers
				device.RenderState.ZBufferFunction = Compare.Always;
				skyBox.Render(device);
				device.RenderState.ZBufferFunction = Compare.LessEqual;
			}

			// Render the environment-mapped ShinyTeapot
			{
				// Set transform state
				Matrix viewProject = Matrix.Multiply(matView, matProject);

				Matrix matViewInv = Matrix.Invert(matView);
				Vector4 vecPosition = new Vector4(matViewInv.M41, matViewInv.M42, matViewInv.M43, 1.0f);

				effect.SetValue("matWorld", matWorld);
				effect.SetValue("matViewProject", viewProject);
				effect.SetValue("vecPosition", vecPosition);


				// Draw teapot
				VertexBuffer vb = shinyTeapot.LocalVertexBuffer;
				IndexBuffer ib = shinyTeapot.LocalIndexBuffer;

				device.VertexFormat = shinyTeapot.LocalMesh.VertexFormat;

				device.SetStreamSource(0, vb, 0, DXHelp.GetTypeSize(typeof(EnvMappedVertex)));
				device.Indices = ib;

				int passes = effect.Begin(0);

				for (int iPass = 0; iPass < passes; iPass++)
				{
					effect.Pass(iPass);

					device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 
						0, shinyTeapot.LocalMesh.NumberVertices,
						0, shinyTeapot.LocalMesh.NumberFaces);

				}

				effect.End();

			}



			// Output statistics
			font.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			font.DrawText(2, 21, System.Drawing.Color.Yellow, deviceStats);

			// End the scene.
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
			// Load the file objects
			try
			{
				shinyTeapot.Create(device, "teapot.x");
				skyBox.Create(device, "lobby_skybox.x");
				sphereMapTexture = GraphicsUtility.CreateTexture(device, "spheremap.bmp");
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}

			// Set mesh properties
			shinyTeapot.SetVertexFormat(device, EnvMappedVertex.Format);

			// Restore the device-dependent objects
			font.InitializeDeviceObjects(device);
			string sEffect = null;
			for (int i =0; i < effectString.Length; i++)
				sEffect += effectString[i];

			effect = Effect.FromString(device, sEffect, null, 0, null);
		}




		/// <summary>
        /// The device exists, but may have just been Reset().  Resources in
        /// Pool.Default and any other device state that persists during
        /// rendering should be set here.  Render states, matrices, textures,
        /// etc., that don't change during rendering can be set once here to
        /// avoid redundant state setting during Render() or FrameMove().
        /// </summary>
		protected override void RestoreDeviceObjects(object sender, System.EventArgs e)
		{
			// InitializeDeviceObjects for file objects (build textures and vertex buffers)
			shinyTeapot.RestoreDeviceObjects(device, null);
			skyBox.RestoreDeviceObjects(device , null);

			// Initialize effect
			effect.SetValue("texSphereMap", sphereMapTexture);

			// Set the transform matrices
			float fAspect = (float) device.PresentationParameters.BackBufferWidth / (float) device.PresentationParameters.BackBufferHeight;
			matProject = Matrix.PerspectiveFovLH((float)Math.PI * 0.4f, fAspect, 0.5f, 100.0f);

		}




        /// <summary>
        /// Called during device initialization, this code checks the device for some minimum set of capabilities
        /// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// Make sure this device can support software vertex processing, 
			// and vertex shaders of at least v1.0
			if ((vertexProcessingType == VertexProcessingType.Software) ||
				(caps.VertexShaderVersion.Major >= 1))
				return true;

            // Otherwise, return false
			return false;
		}



        
        /// <summary>
        /// Message proc function to handle mouse input
        /// </summary>
		protected override void OnMouseDown(MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				Quaternion qCursor = GraphicsUtility.GetRotationFromCursor(this);
				Matrix matCursor = Matrix.RotationQuaternion(qCursor);
				matCursor.Transpose(matCursor);
				matTrackBall = Matrix.Multiply(matTrackBall, matCursor);

				this.Capture = true;
			}
			base.OnMouseDown(e);
		}




        /// <summary>
        /// Event handler for windows events
        /// </summary>
        protected override void OnMouseUp(MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				Quaternion qCursor = GraphicsUtility.GetRotationFromCursor(this);
				Matrix matCursor = Matrix.RotationQuaternion(qCursor);
				matTrackBall = Matrix.Multiply(matTrackBall, matCursor);

				this.Capture = false;
			}
			base.OnMouseUp(e);
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