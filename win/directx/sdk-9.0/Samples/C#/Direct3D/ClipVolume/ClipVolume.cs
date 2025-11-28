//-----------------------------------------------------------------------------
// File: ClipVolume.cs
//
// Desc: Sample code showing how to use vertex shader and pixel shader to do
//       clipping effect in D3D.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace ClipVolumeSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private GraphicsFont drawingFont = null;                 // Font for drawing text
		private GraphicsFont drawingFontSmall = null;                 // Font for drawing text
		GraphicsArcBall ourArcball = null;

		Mesh teapotMesh = null;
		Mesh sphereMesh = null;
		Effect effect = null;
		Matrix teapotWorldMatrix = Matrix.Zero;
		Matrix sphereWorldMatrix = Matrix.Zero;
		Matrix arcBallMatrix = Matrix.Zero;
		Matrix projectionMatrix = Matrix.Zero;
		float sphereMove = 0.0f;
		Vector4 sphereCenter;

		bool isHelpShowing = false;

		

		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Clip Volume";
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
			drawingFontSmall = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold, 9);
			enumerationSettings.AppUsesDepthBuffer = true;

			ourArcball = new GraphicsArcBall(this);
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyDown);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Update translation matrix for sphere
			if (sphereCenter.X > 2.0f)
			{
				sphereMove = -0.6f;
			}
			else if (sphereCenter.X < -2.0f)
			{
				sphereMove = 0.6f;
			}
			sphereCenter.X += sphereMove * elapsedTime;
			sphereWorldMatrix.Translate(sphereCenter.X, sphereCenter.Y, sphereCenter.Z);

			sphereWorldMatrix.Transpose(sphereWorldMatrix);
			//Update rotation matrix from ArcBall
			arcBallMatrix.Transpose(ourArcball.RotationMatrix);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			int numPasses;

			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.Black, 1.0f, 0);

			device.BeginScene();

			// draw teapot mesh
			device.SetVertexShaderConstant(0, new Matrix[] {arcBallMatrix });
			device.SetVertexShaderConstant(4, new Matrix[] {teapotWorldMatrix});
			device.SetVertexShaderConstant(8, new Matrix[] {projectionMatrix});
			device.SetVertexShaderConstant(12, new Vector4[] {sphereCenter});
			effect.Technique = effect.GetTechnique("Teapot");
			numPasses = effect.Begin(0);
			for (int pass = 0; pass < numPasses; pass++)
			{
				effect.Pass(pass);
				teapotMesh.DrawSubset(0);
			}
			effect.End();

			// draw sphere mesh
			device.SetVertexShaderConstant(4, new Matrix[] {sphereWorldMatrix});
			effect.Technique = effect.GetTechnique("Sphere");
			numPasses = effect.Begin(0);
			for (int pass = 0; pass < numPasses; pass++)
			{
				effect.Pass(pass);
				sphereMesh.DrawSubset(0);
			}
			effect.End();

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);
			if (isHelpShowing)
			{
				drawingFontSmall.DrawText(2, 42, System.Drawing.Color.Cyan,
					"Use mouse to rotate the teapot:");
			}
			else
			{
				drawingFontSmall.DrawText(2, 42, System.Drawing.Color.LightSteelBlue,
					"Press F1 for help");
			}

			device.EndScene();
		}




        /// <summary>
        /// The window has been created, but the device has not been created yet.  
        /// Here you can perform application-related initialization and cleanup that 
        /// does not depend on a device.
        /// </summary>
		protected override void OneTimeSceneInitialization()
		{
			// Translation matrix for teapot mesh
			// NOTE: This translation is fixed and will preceded by the ArcBall
			//       rotation that is calculated per-frame in FrameMove
			teapotWorldMatrix = Matrix.Translation(0.0f, 0.0f, 5.0f);
			teapotWorldMatrix.Transpose(teapotWorldMatrix);

			// Translation matrix for sphere mesh
			// NOTE: This is built per-frame in FrameMove

			// VIEW matrix for the entire scene
			// NOTE: This is fixed to an identity matrix

			// NOTE: The projection is based on the window dimensions
			//       and is built in RestoreDeviceObjects

			// Initial per-second movement delta for sphere
			sphereMove = 0.6f;

			// Initial location of center of sphere
			// NOTE: .xyz = center of sphere
			//       .w   = radius of sphere
			sphereCenter = new Vector4(0.0f, 0.0f, 5.0f, 1.0f);

			// Set cursor to indicate that user can move the object with the mouse
			this.Cursor = System.Windows.Forms.Cursors.SizeAll;
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
			drawingFontSmall.InitializeDeviceObjects(device);

			// Create teapot mesh and sphere mesh

			// Note: We don't need a declarator here since D3DXCreateSphere already 
			// implicitly creates one for us, and it is set inside the DrawSubset call.

			teapotMesh = Mesh.Teapot(device);
			sphereMesh = Mesh.Sphere(device, 1.0f, 30, 30);

			// Load effect file
			string strPath = DXUtil.FindMediaFile(null, "ClipVolume.fx");
			effect = Effect.FromFile(device, strPath, null, 0, null);
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
			ourArcball.SetWindow(device.PresentationParameters.BackBufferWidth, device.PresentationParameters.BackBufferHeight, 0.9f);

			// PROJECTION matrix for the entire scene
			// NOTE: The projection is fixed
			float fAspect = ((float)device.PresentationParameters.BackBufferWidth) / device.PresentationParameters.BackBufferHeight;
			projectionMatrix = Matrix.PerspectiveFovLH((float)Math.PI / 4, fAspect, 1.0f, 60.0f);
			projectionMatrix.Transpose(projectionMatrix);
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

			// Device should support at least both VS.1.1 and PS.1.1
			if (caps.VertexShaderVersion >= new Version(1, 1) && 
				caps.PixelShaderVersion >= new Version(1, 1))
			{
				return true;
			}

			return false;
		}




		/// <summary>
		/// Event Handler for windows messages
		/// </summary>
		private void OnPrivateKeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == System.Windows.Forms.Keys.F1)
			{
				isHelpShowing = !isHelpShowing;
			}
		}


		

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        public static void Main() 
        {
            using(MyGraphicsSample mysample = new MyGraphicsSample())
            {
                if (mysample.CreateGraphicsSample())
                    mysample.Run();
            }
        }
    }
}
