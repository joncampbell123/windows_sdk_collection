//-----------------------------------------------------------------------------
// File: Text3D.cs
//
// Desc: Example code showing how to do text in a Direct3D scene.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace Text3D
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private GraphicsFont drawingFont = null; // Font for drawing text
		private GraphicsFont statsFont = null; // Font for drawing text (Stats)
		private Mesh mesh3DText = null;  // Mesh to draw 3d text
		private D3DXFont otherFont = null; // The D3DX Font object

		private System.Drawing.Font ourFont = new System.Drawing.Font("Arial", 18, System.Drawing.FontStyle.Bold); // Font we use 

		private Matrix objectOne = new Matrix();
		private Matrix objectTwo = new Matrix();

		private System.Windows.Forms.MenuItem mnuOptions;
		private System.Windows.Forms.MenuItem mnuChangeFont;


        
        
        /// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Text3D: Text in a 3D scene";
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

			// Create our font objects
			statsFont = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			drawingFont = new GraphicsFont(ourFont);
			otherFont = new D3DXFont(ourFont);
			enumerationSettings.AppUsesDepthBuffer = true;

			// Add our new menu options
			mnuOptions = new System.Windows.Forms.MenuItem("&Options");
			mnuChangeFont = new System.Windows.Forms.MenuItem("&Change Font...");
			// Add to the main menu screen
			mnuMain.MenuItems.Add(this.mnuOptions);
			mnuOptions.MenuItems.Add(mnuChangeFont);
			mnuChangeFont.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
			mnuChangeFont.ShowShortcut = true;
			mnuChangeFont.Click += new System.EventHandler(this.ChangeFontClick);
		}




        /// <summary>
        /// Handles font change
        /// </summary>
        private void ChangeFontClick(object sender, EventArgs e)
		{
			System.Windows.Forms.FontDialog dlg = new System.Windows.Forms.FontDialog();

			// Show the dialog
			dlg.FontMustExist = true;
			dlg.Font = ourFont;
			
			if (dlg.ShowDialog(this) == System.Windows.Forms.DialogResult.OK) // We selected something
			{
				ourFont = dlg.Font;
				if (drawingFont != null)
					drawingFont.Dispose(null, null);
				// Set the new font
				drawingFont = new GraphicsFont(ourFont);
				drawingFont.InitializeDeviceObjects(device);

				if (otherFont != null)
					otherFont.Dispose();
				otherFont = new D3DXFont(ourFont);
				otherFont.InitializeDeviceObjects(device);

				// Create our 3d text mesh
				mesh3DText = Mesh.TextFromFont(device, ourFont, "Mesh.TextFromFont", 0.001f, 0.4f);
			}
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Setup five rotation matrices (for rotating text strings)
			Vector3 vAxis1 = new Vector3(1.0f,2.0f,0.0f);
			Vector3 vAxis2 = new Vector3(2.0f,1.0f,0.0f);
			objectOne = Matrix.RotationAxis(vAxis1, appTime / 2.0f);
			objectTwo = Matrix.RotationAxis(vAxis2, appTime / 2.0f);

			// Add some translational values to the matrices
			objectOne.M41 = 1.0f;   objectOne.M42 = 6.0f;   objectOne.M43 = 20.0f; 
			objectTwo.M41 = -4.0f;  objectTwo.M42 = -1.0f;  objectTwo.M43 = 0.0f; 
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Black, 1.0f, 0);

			device.BeginScene();

			// Output statistics
			statsFont.DrawText(2,  1, Color.Yellow, frameStats);
			statsFont.DrawText(2, 20, Color.Yellow, deviceStats);

			// Draw GraphicsFont in 2D (red)
			drawingFont.DrawText(60, 100, Color.Red, "GraphicsFont.DrawText");

			// Draw GraphicsFont scaled in 2D (cyan)
			drawingFont.DrawTextScaled(-1.0f, 0.8f, 0.5f, // position
								        0.1f, 0.1f,       // scale
								        Color.Cyan, "GraphicsFont.DrawTextScaled");

			// Draw GraphicsFont in 3D (green)
			Material mtrl = GraphicsUtility.InitMaterial(Color.Green);
			device.Material = mtrl;
			device.Transform.World = objectOne;
			drawingFont.Render3DText("GraphicsFont.Render3DText", GraphicsFont.RenderFlags.Centered | GraphicsFont.RenderFlags.TwoSided | GraphicsFont.RenderFlags.Filtered);

			// Draw D3DXFont mesh in 3D (blue)
			if (mesh3DText != null)
			{
				Material mtrl3d = GraphicsUtility.InitMaterial(Color.Blue);
				device.Material = mtrl3d;
				device.Transform.World = objectTwo;
				mesh3DText.DrawSubset(0);
			}

			// Draw D3DXFont in 2D (purple)
			otherFont.BeginText();
			otherFont.DrawText(60, 200, Color.Purple.ToArgb(), "D3DXFont.DrawText");
			otherFont.EndText();

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
			// Initialize all of the fonts
			drawingFont.InitializeDeviceObjects(device);
			statsFont.InitializeDeviceObjects(device);
			otherFont.InitializeDeviceObjects(device);
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
			// Restore the textures
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].ColorOperation = TextureOperation.Modulate;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;

			device.RenderState.ZBufferEnable = true;
			device.RenderState.DitherEnable = true;
			device.RenderState.SpecularEnable = true;
			device.RenderState.Lighting = true;
			device.RenderState.Ambient = System.Drawing.Color.FromArgb(unchecked((int)0x80808080));

			GraphicsUtility.InitLight(device.Lights[0], LightType.Directional, 10.0f, -10.0f, 10.0f);
			device.Lights[0].Commit();
			device.Lights[0].Enabled = true;

			// Set the transform matrices
			Vector3 vEyePt = new Vector3(0.0f,-5.0f,-10.0f);
			Vector3 vLookatPt = new Vector3(0.0f, 0.0f,  0.0f);
			Vector3 vUpVec = new Vector3(0.0f, 1.0f,  0.0f);
			Matrix matWorld, matView, matProj;

			matWorld = Matrix.Identity;
			matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
			float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			matProj = Matrix.PerspectiveFovLH((float)Math.PI / 4, fAspect, 1.0f, 100.0f);

			device.Transform.World = matWorld;
			device.Transform.View = matView;
			device.Transform.Projection = matProj;

			if (mesh3DText != null)
				mesh3DText.Dispose();

			// Create our 3d text mesh
			mesh3DText = Mesh.TextFromFont(device, ourFont, "Mesh.TextFromFont", 0.001f, 0.4f);
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
