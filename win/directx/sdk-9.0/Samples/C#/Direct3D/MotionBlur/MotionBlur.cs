//-----------------------------------------------------------------------------
// File: MotionBlur.cs
//
// Desc: Sample code showing how to use vertex shader to create a motion blur
//       effect in D3D.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace MotionBlurSample
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

		Mesh ballMesh = null;
		Effect effect = null;
		Matrix worldMatrixPrevious1 = Matrix.Zero;
		Matrix worldMatrixPrevious2 = Matrix.Zero;
		Matrix worldMatrixCurrent1 = Matrix.Zero;
		Matrix worldMatrixCurrent2 = Matrix.Zero;
		Matrix arcBallMatrix = Matrix.Zero;
		Matrix viewMatrix = Matrix.Zero;
		Matrix projectionMatrix = Matrix.Zero;
		float trailLength =  0.0f;
		float rotateAngle1 =  0.0f;
		float rotateAngle2 =  0.0f;
		bool isFirstObjectInFront;
		bool isHelpShowing = false;

		


		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Motion Blur";
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
			rotateAngle1 += 2.0f * elapsedTime;
			rotateAngle2 += 2.0f * elapsedTime;

			worldMatrixPrevious1 = worldMatrixCurrent1;
			worldMatrixPrevious2 = worldMatrixCurrent2;

			Vector3 position1 = new Vector3(14 * (float)Math.Cos(rotateAngle1), 0.0f, 7 * (float)Math.Sin(rotateAngle1));
			Vector3 position2 = new Vector3(14 * (float)Math.Cos(rotateAngle2), 0.0f, 7 * (float)Math.Sin(rotateAngle2));

			worldMatrixCurrent1 = Matrix.Translation(position1.X, position1.Y, position1.Z);
			worldMatrixCurrent2 = Matrix.Translation(position2.X, position2.Y, position2.Z);

			// Transform positions into camera space to see which is in front
			Matrix worldViewMatrix;
			worldViewMatrix = worldMatrixCurrent1 * arcBallMatrix * viewMatrix;
			position1.TransformCoordinate(worldViewMatrix);

			worldViewMatrix = worldMatrixCurrent2 * arcBallMatrix * viewMatrix;
			position2.TransformCoordinate(worldViewMatrix);

			isFirstObjectInFront = (position1.Z < position2.Z);


			worldMatrixCurrent1.Transpose(worldMatrixCurrent1);
			worldMatrixCurrent2.Transpose(worldMatrixCurrent2);

			Matrix viewMatrixTranspose = Matrix.Zero;

			viewMatrixTranspose.Transpose(viewMatrix);

			// Update rotation matrix from ArcBall
			arcBallMatrix.Transpose(ourArcball.RotationMatrix);
			device.SetVertexShaderConstant(8, new Matrix[] {arcBallMatrix});

			device.SetVertexShaderConstant(12, new Matrix[] { viewMatrixTranspose});
			device.SetVertexShaderConstant(16, new Matrix[] { projectionMatrix});

			Vector4 trailConst = new Vector4(trailLength, 0.0f, 1.0f, 0.0f);
			device.SetVertexShaderConstant(20, new Vector4[] { trailConst });
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			int numPasses;

			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0f, 0);

			device.BeginScene();

			// draw first sphere mesh
			if (isFirstObjectInFront)
			{
				device.SetVertexShaderConstant(0, new Matrix[] { worldMatrixPrevious2});
				device.SetVertexShaderConstant(4, new Matrix[] { worldMatrixCurrent2});
			}
			else
			{
				device.SetVertexShaderConstant(0, new Matrix[] { worldMatrixPrevious1});
				device.SetVertexShaderConstant(4, new Matrix[] { worldMatrixCurrent1});
			}
			numPasses = effect.Begin(0);
			for (int pass = 0; pass < numPasses; pass++)
			{
				effect.Pass(pass);
				ballMesh.DrawSubset(0);
			}
			effect.End();

			// draw second sphere mesh
			if (isFirstObjectInFront)
			{
				device.SetVertexShaderConstant(0, new Matrix[] { worldMatrixPrevious1});
				device.SetVertexShaderConstant(4, new Matrix[] { worldMatrixCurrent1});
			}
			else
			{
				device.SetVertexShaderConstant(0, new Matrix[] { worldMatrixPrevious2});
				device.SetVertexShaderConstant(4, new Matrix[] { worldMatrixCurrent2});
			}
			numPasses = effect.Begin(0);
			for (int pass = 0; pass < numPasses; pass++)
			{
				effect.Pass(pass);
				ballMesh.DrawSubset(0);
			}
			effect.End();

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);
			if (isHelpShowing)
			{
				drawingFontSmall.DrawText(2, 42, System.Drawing.Color.Cyan,
					"Use mouse to rotate the scene:");
				drawingFontSmall.DrawText(2, 62, System.Drawing.Color.Cyan,
					"Increase Trail Length");
				drawingFontSmall.DrawText(150, 62, System.Drawing.Color.Cyan,
					"Up Arrow Key");
				drawingFontSmall.DrawText(2, 80, System.Drawing.Color.Cyan,
					"Decrease Trail Length");
				drawingFontSmall.DrawText(150, 80, System.Drawing.Color.Cyan,
					"Down Arrow Key");
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
			// World matrix for previous frame
			// NOTE: We need one-time initialization for previous frame (for 1st frame)
			worldMatrixCurrent1.Translate(0.0f, 0.0f, 7.0f);
			worldMatrixCurrent1.Transpose(worldMatrixPrevious1);
			worldMatrixCurrent2.Translate(-14.0f, 0.0f, 0.0f);
			worldMatrixCurrent2.Transpose(worldMatrixPrevious2);

			// VIEW matrix for the entire scene
			// NOTE: VIEW matrix is fixed
			Vector3 vFromPt = new Vector3(0.0f, 0.0f, -26.0f);
			Vector3 vLookatPt= new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vUpVec= new Vector3(0.0f, 1.0f, 0.0f);
			viewMatrix = Matrix.LookAtLH(vFromPt, vLookatPt, vUpVec);

			rotateAngle1 = (float)Math.PI / 2.0f;
			rotateAngle2 = (float)Math.PI;

			trailLength = 3.0f;

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

			// Load a sphere mesh
			ballMesh = Mesh.Sphere(device, 1.2f, 36, 36);

			// Load effect file
			string strPath = DXUtil.FindMediaFile(null, "MotionBlur.fx");
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
			if (vertexProcessingType == VertexProcessingType.Software)
				return true;

			if (caps.VertexShaderVersion >= new Version(1, 1))
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
			if (e.KeyCode == System.Windows.Forms.Keys.Up)
			{
				if (trailLength < 8.0f)
					trailLength += 0.3f;
			}
			if (e.KeyCode == System.Windows.Forms.Keys.Down)
			{
				if (trailLength > 2.0f)
					trailLength -= 0.3f;
			}
			if (e.KeyCode == System.Windows.Forms.Keys.F1)
			{
				isHelpShowing = !isHelpShowing;
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
