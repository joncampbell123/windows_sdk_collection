//-----------------------------------------------------------------------------
// File: VolumeTexture.cs
//
// Desc: Example code showing how to do volume textures in D3D.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace VolumeTextureSample
{
	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		public struct VolumeVertex
		{
			public float x,y,z; // Position
			public int color;
			public float tu, tv, tw; // Tex coordinates

			public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Diffuse | VertexFormats.Texture1 | VertexTextureCoordinate.Size3(0);

			public VolumeVertex(float px, float py, float pz, int c, float u, float v, float w)
			{
				x = px; y = py; z = pz; color = c; tu = u; tv = v; tw = w;
			}
		}

		public static readonly VolumeVertex[] vertices = new VolumeVertex[] { new VolumeVertex(1.0f, 1.0f, 0.0f, Color.White.ToArgb(), 1.0f, 1.0f, 0.0f) ,
																				new VolumeVertex(-1.0f, 1.0f, 0.0f, Color.White.ToArgb(), 0.0f, 1.0f, 0.0f),
																				new VolumeVertex(1.0f,-1.0f, 0.0f, Color.White.ToArgb(), 1.0f, 0.0f, 0.0f),
																				new VolumeVertex(-1.0f,-1.0f, 0.0f, Color.White.ToArgb(), 0.0f, 0.0f, 0.0f)};


		private GraphicsFont drawingFont = null; // Font for drawing text
		private VertexBuffer vertex = null; // VertexBuffer to render texture on
		private VolumeTexture volume = null; // The Volume Texture
		



		/// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "VolumeTexture";
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
			drawingFont = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			enumerationSettings.AppUsesDepthBuffer = true;
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			float fAngle = appTime / 2.0f;

			// Play with the volume texture coordinate
			GraphicsStream stm = vertex.Lock(0, 0, 0);
			// Seek to the correct spot and write the data
			for (int i=0; i<4; i++)
			{
				stm.Seek(24, System.IO.SeekOrigin.Current); // Seek 24 bytes into the structure
				stm.Write((float)(Math.Sin(fAngle) * Math.Cos(fAngle)));
			}
			vertex.Unlock();

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

			// Draw the quad, with the volume texture
			device.SetTexture(0, volume);
			device.VertexFormat = VolumeVertex.Format;
			device.SetStreamSource(0, vertex, 0);
			device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

			// Output statistics
			drawingFont.DrawText(2,  1, Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, Color.Yellow, deviceStats);

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
			// Create a volume texture
			volume = new VolumeTexture(device, 16, 16, 16, 1, Format.A8R8G8B8, Pool.Managed);
			// Fill the volume texture
			int[,,] data = (int[,,])volume.LockBox(typeof(int), 0, 0, 16, 16, 16);
			for (int w = 0; w < 16; w++)
			{
				for (int v = 0; v < 16; v++)
				{
					for (int u = 0; u < 16; u++)
					{
						float du = (u-7.5f)/7.5f;
						float dv = (v-7.5f)/7.5f;
						float dw = (w-7.5f)/7.5f;
						float fScale = (float)Math.Sqrt(du*du + dv*dv + dw*dw) / (float)Math.Sqrt(1.0f);

						if (fScale > 1.0f) 
							fScale = 0.0f;
						else                
							fScale = 1.0f - fScale;

						int r = (int)((w<<4)*fScale);
						int g = (int)((v<<4)*fScale);
						int b = (int)((u<<4)*fScale);

						data[w,v,u] = unchecked((int)0xff000000 + (r<<16) + (g<<8) + (b));
					}
				}
			}
			volume.UnlockBox(0);

			// Create a vertex buffer
			vertex = new VertexBuffer(typeof(VolumeVertex), 4, device, Usage.WriteOnly, VolumeVertex.Format, Pool.Managed);
			GraphicsStream vertStream = vertex.Lock(0, 0, 0);
			// Copy our vertices in
			vertStream.Write(vertices);
			vertex.Unlock();
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
			// Set the matrices
			Vector3 vEye = new Vector3(0.0f, 0.0f,-3.0f);
			Vector3 vAt = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vUp = new Vector3(0.0f, 1.0f, 0.0f);
			float fAspect = (float)device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
			device.Transform.World = Matrix.Identity;
			device.Transform.View = Matrix.LookAtLH(vEye, vAt, vUp);
			device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 1.0f, 100.0f);

			// Set state
			device.RenderState.DitherEnable = false;
			device.RenderState.Clipping = false;
			device.RenderState.CullMode = Cull.None;
			device.RenderState.Lighting = false;
			device.RenderState.ZBufferEnable = false;
			device.RenderState.ZBufferWriteEnable = false;

			device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
			device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
			device.TextureState[0].AlphaOperation = TextureOperation.SelectArg1;
			device.TextureState[0].AlphaArgument1 = TextureArgument.TextureColor;
			device.TextureState[0].AlphaArgument2 = TextureArgument.Diffuse;
			device.SamplerState[0].MinFilter = TextureFilter.Linear;
			device.SamplerState[0].MagFilter = TextureFilter.Linear;
		}




        /// <summary>
        /// Called during device initialization, this code checks the device for 
        /// some minimum set of capabilities
        /// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
			// Make sure we can do a volume map, that's all we care about
			return caps.TextureCaps.SupportsVolumeMap;
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
