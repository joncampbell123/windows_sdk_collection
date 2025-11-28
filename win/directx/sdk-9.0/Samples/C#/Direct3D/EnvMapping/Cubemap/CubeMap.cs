//-----------------------------------------------------------------------------
// File: CubeMap.cs
//
// Desc: Example code showing how to do environment cube-mapping.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace CubeMapSample
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
		/// are shown.. one which simply uses cubemaps, and a fallback which uses 
		/// a vertex shader to do a sphere-map lookup.
        /// </summary>
		string[] effectString = 
		{
			"texture texCubeMap;\n",
			"texture texSphereMap;\n",

			"matrix matWorld;\n",
			"matrix matView;\n",
			"matrix matProject;\n",
			"matrix matWorldView;\n",


			"technique Cube\n",
			"{\n",
				"pass P0\n",
				"{\n",
					// Vertex state
					"VertexShader = null;\n",
					"WorldTransform[0] = <matWorld>;\n",
					"ViewTransform = <matView>;\n",
					"ProjectionTransform = <matProject>;\n",

					// Pixel state
					"Texture[0] = <texCubeMap>;\n",
			            
					"MinFilter[0] = Linear;\n",
					"MagFilter[0] = Linear;\n",

					"AddressU[0] = Clamp;\n",
					"AddressV[0] = Clamp;\n",
					"AddressW[0] = Clamp;\n",

					"ColorOp[0] = SelectArg1;\n",
					"ColorArg1[0] = Texture;\n",

					"TexCoordIndex[0] = CameraSpaceReflectionVector;\n",
					"TextureTransformFlags[0] = Count3;\n",
				"}\n",
			"}\n",


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
					"vs.1.1\n",
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
					"mov r2, -r0\n",
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

				"VertexShaderConstant4[0] = <matWorldView>;\n",
				"VertexShaderConstant4[4] = <matProject>;\n",

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

		// CubeMapResolution indicates how big to make the cubemap texture.  Larger
		// textures will generate a better-looking reflection.
		public const int CubeMapResolution = 256;

		Matrix projectionMatrix;
		Matrix viewMatrix;
		Matrix worldMatrix;
		Matrix airplaneMatrix;
		Matrix trackBallMatrix;

		GraphicsFont drawingFont = null;
		GraphicsMesh shinyTeapotMesh  = null;
		GraphicsMesh skyBoxMesh = null;
		GraphicsMesh airplaneMesh = null;

		Effect effect = null;
		RenderToEnvironmentMap renderToEnvironmentMap = null;
		CubeTexture cubeMap = null;
		Texture sphereMap = null;




        /// <summary>
        /// Application constructor. Sets attributes for the app.
        /// </summary>
		public MyGraphicsSample()
		{
			this.Text = "CubeMap: Environment cube-mapping";
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

			drawingFont = new GraphicsFont("Arial", FontStyle.Bold);
			shinyTeapotMesh = new GraphicsMesh();
			skyBoxMesh = new GraphicsMesh();
			airplaneMesh = new GraphicsMesh();

			worldMatrix = Matrix.Identity;

			trackBallMatrix  = Matrix.Identity;
			viewMatrix = Matrix.Translation(0.0f, 0.0f, 3.0f);
		}




        /// <summary>
        /// Called once per frame, the call is the entry point for animating the scene.
        /// </summary>
		protected override void FrameMove()
		{
			// Animate file object
			Matrix  mat1 = Matrix.Translation(0.0f, 2.0f, 0.0f);
			airplaneMatrix = Matrix.Scaling(0.2f, 0.2f, 0.2f);
		
			airplaneMatrix = Matrix.Multiply(airplaneMatrix, mat1);
			mat1 = Matrix.RotationX(-2.9f * appTime);
			airplaneMatrix = Matrix.Multiply(airplaneMatrix, mat1);
			mat1 = Matrix.RotationY(1.055f * appTime);
			airplaneMatrix = Matrix.Multiply(airplaneMatrix, mat1);

			// When the window has focus, let the mouse adjust the camera view
			if (this.Capture)
			{
				Matrix matCursor;
				Quaternion quat = GraphicsUtility.GetRotationFromCursor(this);
				matCursor = Matrix.RotationQuaternion(quat);
				viewMatrix = Matrix.Multiply(trackBallMatrix, matCursor);

				Matrix matTrans;
				matTrans = Matrix.Translation(0.0f, 0.0f, 3.0f);
				viewMatrix = Matrix.Multiply(viewMatrix, matTrans);
			}

			// Render the scene into the surfaces of the cubemap
			RenderSceneIntoEnvMap();
		}




        /// <summary>
        /// Renders the scene to each of the 6 faces of the cube map
        /// </summary>
		private void RenderSceneIntoEnvMap()
		{

			// Set the projection matrix for a field of view of 90 degrees
			Matrix matProj;
			matProj = Matrix.PerspectiveFovLH((float)Math.PI * 0.5f, 1.0f, 0.5f, 1000.0f);


			// Get the current view matrix, to concat it with the cubemap view vectors
			Matrix matViewDir = viewMatrix;
			matViewDir.M41 = 0.0f; matViewDir.M42 = 0.0f; matViewDir.M43 = 0.0f;



			// Render the six cube faces into the environment map
			if (cubeMap != null)
				renderToEnvironmentMap.BeginCube(cubeMap);
			else
				renderToEnvironmentMap.BeginSphere(sphereMap);

			for (int i = 0; i < 6; i++)
			{
				renderToEnvironmentMap.Face((CubeMapFace) i , 1);

				// Set the view transform for this cubemap surface
				Matrix matView = Matrix.Multiply(matViewDir, GraphicsUtility.GetCubeMapViewMatrix((CubeMapFace) i));

				// Render the scene (except for the teapot)
				RenderScene(matView, matProj, false);
			}

			renderToEnvironmentMap.End(1);
		}




        /// <summary>
        /// Renders all visual elements in the scene. This is called by the main
		/// Render() function, and also by the RenderIntoCubeMap() function.
        /// </summary>
		private void RenderScene(Matrix View, Matrix Project, bool canRenderTeapot)
		{
			// Render the Skybox
			{
				Matrix matWorld = Matrix.Scaling(10.0f, 10.0f, 10.0f);

				Matrix matView = View;
				matView.M41 = matView.M42 = matView.M43 = 0.0f;

				device.Transform.World = matWorld;
				device.Transform.View = matView;
				device.Transform.Projection = Project;

				device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
				device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
				device.SamplerState[0].MinFilter = TextureFilter.Linear;
				device.SamplerState[0].MagFilter = TextureFilter.Linear;
				device.SamplerState[0].AddressU = TextureAddress.Mirror;
				device.SamplerState[0].AddressV = TextureAddress.Mirror;

				// Always pass Z-test, so we can avoid clearing color and depth buffers
				device.RenderState.ZBufferFunction = Compare.Always;
				skyBoxMesh.Render(device);
				device.RenderState.ZBufferFunction = Compare.LessEqual;
			}


			// Render the Airplane
			{
				device.Transform.World = airplaneMatrix;
				device.Transform.View = View;
				device.Transform.Projection = Project;

				device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
				device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
				device.SamplerState[0].MinFilter = TextureFilter.Linear;
				device.SamplerState[0].MagFilter = TextureFilter.Linear;
				device.SamplerState[0].AddressU = TextureAddress.Wrap;
				device.SamplerState[0].AddressV = TextureAddress.Wrap;

				airplaneMesh.Render(device, true, false);

				device.Transform.World = worldMatrix;
			}

			// Render the environment-mapped ShinyTeapot
			if (canRenderTeapot)
			{
				// Set transform state
				if (cubeMap != null)
				{
					effect.SetValue("matWorld", worldMatrix);
					effect.SetValue("matView", View);
				}
				else
				{
					Matrix matWorldView = Matrix.Multiply(worldMatrix, View);
					effect.SetValue("matWorldView", matWorldView);
				}

				effect.SetValue("matProject", Project);


				// Draw teapot
				VertexBuffer tempVertexBuffer = shinyTeapotMesh.LocalVertexBuffer;
				IndexBuffer tempIndexBuffer = shinyTeapotMesh.LocalIndexBuffer;
				int numVert = shinyTeapotMesh.LocalMesh.NumberVertices;
				int numFace = shinyTeapotMesh.LocalMesh.NumberFaces;

				device.SetStreamSource(0, tempVertexBuffer, 0, DXHelp.GetTypeSize(typeof(EnvMappedVertex)));
				device.VertexFormat = EnvMappedVertex.Format;
				device.Indices = tempIndexBuffer;

				int Passes = effect.Begin(0);

				for (int iPass = 0; iPass < Passes; iPass++)
				{
					effect.Pass(iPass);

					device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 
						0, 0, numVert, 0, numFace);

				}

				effect.End();
			}
		}




        /// <summary>
        /// Called once per frame, the call is the entry point for 3d
		///  rendering. This function sets up render states, clears the
		///  viewport, and renders the scene.
        /// </summary>
		protected override void Render()
		{
			// Begin the scene
			device.BeginScene();
            
            // Ignore any exceptions during rendering
            DirectXException.IgnoreExceptions();
			// Render the scene, including the teapot
			RenderScene(viewMatrix, projectionMatrix, true);

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 21, System.Drawing.Color.Yellow, deviceStats);

            // We're done ignoring them
            DirectXException.EnableExceptions();
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
				shinyTeapotMesh.Create(device, "teapot.x");
				skyBoxMesh.Create(device, "lobby_skybox.x");
				airplaneMesh.Create(device, "airplane 2.x");
			}
			catch
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}

			// Set mesh properties
			airplaneMesh.SetVertexFormat(device, VertexFormats.Position | VertexFormats.Normal | VertexFormats.Texture1);
			shinyTeapotMesh.SetVertexFormat(device, EnvMappedVertex.Format);

			// Restore the device-dependent objects
			drawingFont.InitializeDeviceObjects(device);
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
			shinyTeapotMesh.RestoreDeviceObjects(device, null);
			skyBoxMesh.RestoreDeviceObjects(device , null);
			airplaneMesh.RestoreDeviceObjects(device , null);

			// Create RenderToEnvironmentMap object
			renderToEnvironmentMap = new RenderToEnvironmentMap(device, CubeMapResolution, 1,  
				device.PresentationParameters.BackBufferFormat, true, DepthFormat.D16);

			// Create the cubemap, with a format that matches the backbuffer
			if (Caps.TextureCaps.SupportsCubeMap)
			{
				try
				{
					cubeMap = new CubeTexture(device, CubeMapResolution, 1,
						Usage.RenderTarget, device.PresentationParameters.BackBufferFormat, Pool.Default);
				}
				catch
				{
					try
					{
						cubeMap = new CubeTexture(device, CubeMapResolution, 1,
							0, device.PresentationParameters.BackBufferFormat, Pool.Default);
					}
					catch { cubeMap = null;}
				}
			}


			// Create the spheremap, with a format that matches the backbuffer
			if (cubeMap == null)
			{
				try
				{
					sphereMap = new Texture(device, CubeMapResolution, CubeMapResolution, 
						1, Usage.RenderTarget, device.PresentationParameters.BackBufferFormat, Pool.Default);
				}
				catch
				{
					sphereMap = new Texture(device, CubeMapResolution, CubeMapResolution, 
						1, 0, device.PresentationParameters.BackBufferFormat, Pool.Default);
				}
			}

			// Initialize effect
			effect.SetValue("texCubeMap", cubeMap);
			effect.SetValue("texSphereMap", sphereMap);

			if (cubeMap != null)
			{
				effect.Technique = "Cube";
				this.Text = "CubeMap: Environment cube-mapping";
			}
			else
			{
				effect.Technique = "Sphere";
				this.Text = "CubeMap: Environment cube-mapping (using dynamic spheremap)";
			}


			// Set the transform matrices
			float fAspect = (float) device.PresentationParameters.BackBufferWidth / (float) device.PresentationParameters.BackBufferHeight;
			projectionMatrix = Matrix.PerspectiveFovLH((float)Math.PI * 0.4f, fAspect, 0.5f, 100.0f);

		}




        /// <summary>
        /// Called during device initialization, this code checks the device
		///  for some minimum set of capabilities
        /// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// Make sure this device can support cube textures, software vertex processing, 
			// and vertex shaders of at least v1.0
			if ((!caps.TextureCaps.SupportsCubeMap) &&
				(vertexProcessingType != VertexProcessingType.Software) &&
				(caps.VertexShaderVersion.Major < 1))
				return false;

			// Check that we can create a cube texture that we can render into
			return true;
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
				trackBallMatrix = Matrix.Multiply(trackBallMatrix, matCursor);

				this.Capture = true;
			}
			base.OnMouseDown(e);
		}
		protected override void OnMouseUp(MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				Quaternion qCursor = GraphicsUtility.GetRotationFromCursor(this);
				Matrix matCursor = Matrix.RotationQuaternion(qCursor);
				trackBallMatrix = Matrix.Multiply(trackBallMatrix, matCursor);

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