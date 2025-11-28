//-----------------------------------------------------------------------------
// File: Fractal.cs
//
// Desc: Draw our fractals as a height map
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace FractalSample
{
	/// <summary>
	/// Application class. The base class (MyGraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private GraphicsFont drawingFont = null;                 // Font for drawing text

		//Fractal
		private FractalTool.ElevationPoints fracpatch = null;
		private double shape = .5;
		private int maxLevel = 6;
		private int bufferSize = 0;
		private int vert_size=0;
		private int indexX, indexY;
		private double [,] points;
		private double scale;

		//indices buffer
		IndexBuffer indexBuffer = null;
		short[] indices;
		//Vertexbuffer
		VertexBuffer vertexBuffer = null;


        
        
        /// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Fractal";
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
			enumerationSettings.AppUsesDepthBuffer= true;
			this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.OnPrivateKeyUp);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			// Setup the lights and materials
			SetupLights();
			// Setup the world, view, and projection matrices
			SetupMatrices();
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			//Clear the backbuffer to a black color 
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, Color.Black, 1.0f, 0);
			//Begin the scene
			device.BeginScene();

			device.VertexFormat = CustomVertex.PositionNormalColored.Format;

			// set the vertexbuffer stream source
			device.SetStreamSource(0, vertexBuffer, 0, VertexInformation.GetFormatSize(CustomVertex.PositionNormalColored.Format));
			// set fill mode
			device.RenderState.FillMode = FillMode.Solid;
			// set the indices			
			device.Indices = indexBuffer;
			//use the indices buffer
			device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, bufferSize*bufferSize, 0, vert_size/3);

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);
			drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Hit 'F' to generate new fractal.");

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
			
			FractSetup();

			// Now Create the VB
			vertexBuffer = new VertexBuffer(typeof(CustomVertex.PositionNormalColored), bufferSize*bufferSize, device, Usage.WriteOnly, CustomVertex.PositionNormalColored.Format, Pool.Default);
			vertexBuffer.Created += new System.EventHandler(this.OnCreateVertexBuffer);
			this.OnCreateVertexBuffer(vertexBuffer, null);

			//create the indices buffer

			indexBuffer = new IndexBuffer(typeof(short), vert_size, device, Usage.WriteOnly, Pool.Default);
			indices = new short[(bufferSize*6)*bufferSize];
			indexBuffer.Created += new System.EventHandler(this.OnCreateIndexBuffer);
			this.OnCreateIndexBuffer(indexBuffer, null);
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

			// Turn off culling, so we see the front and back of the triangle
			device.RenderState.CullMode = Cull.None;
			// Turn on the ZBuffer
			device.RenderState.ZBufferEnable = true;
			device.RenderState.Lighting = false;    //make sure lighting is turned off

		}




		/// <summary>
		/// Event Handler for windows messages
		/// </summary>
		private void OnPrivateKeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == System.Windows.Forms.Keys.F)
			{
				FractSetup();
				OnCreateVertexBuffer(vertexBuffer, null);
				OnCreateIndexBuffer(indexBuffer, null);
			}
		}


        
        
        /// <summary>
        /// Setup the matrices
        /// </summary>
        private void SetupMatrices()
		{
			// move the object
			Matrix temp = Matrix.Translation(-(bufferSize*0.5f), 0, -(bufferSize*0.5f));

			// For our world matrix, we will just rotate the object about the indexY-axis.
			device.Transform.World = Matrix.Multiply(temp, Matrix.RotationAxis(new Vector3(0,
				(float)Environment.TickCount / 2150.0f,
				0), 
				Environment.TickCount / 3000.0f));

			// Set up our view matrix. A view matrix can be defined given an eye point,
			// a point to lookat, and a direction for which way is up. Here, we set the
			// eye five units back along the z-axis and up three units, look at the
			// origin, and define "up" to be in the indexY-direction.
			device.Transform.View = Matrix.LookAtLH(new Vector3(0.0f, 25.0f,-30.0f),
				new Vector3(0.0f, 0.0f, 0.0f), 
				new Vector3(0.0f, 1.0f, 0.0f));

			// For the projection matrix, we set up a perspective transform (which
			// transforms geometry from 3D view space to 2D viewport space, with
			// a perspective divide making objects smaller in the distance). To build
			// a perpsective transform, we need the field of view (1/4 pi is common),
			// the aspect ratio, and the near and far clipping planes (which define at
			// what distances geometry should be no longer be rendered).
			device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI / 4.0f, 1.0f, 1.0f, 100.0f);
		}

        
        
        
        /// <summary>
        /// Setup the lights
        /// </summary>
        private void SetupLights()
		{
			Color col = Color.White;
			//Set up a material. The material here just has the diffuse and ambient
			//colors set to yellow. Note that only one material can be used at a time.
			Material mtrl = new Material();
			mtrl.Diffuse = 	mtrl.Ambient = col;
			device.Material = mtrl;
			
			//Set up a white, directional light, with an oscillating direction.
			//Note that many lights may be active at a time (but each one slows down
			//the rendering of our scene). However, here we are just using one. Also,
			//we need to set the D3DRS_LIGHTING renderstate to enable lighting
    
			device.Lights[0].Type = LightType.Directional;
			device.Lights[0].Diffuse = Color.Purple;
			device.Lights[0].Direction = new Vector3(0, 1.0f,	0);

			device.Lights[0].Commit();
			device.Lights[0].Enabled = true;

			//Finally, turn on some ambient light.
			//Ambient light is light that scatters and lights all objects evenly
			device.RenderState.Ambient = Color.Gray;
		}

        
        
        
        /// <summary>
        /// Setup the fractal
        /// </summary>
        public void FractSetup()
		{
			fracpatch = new FractalTool.ElevationPoints(maxLevel, false, 2.5, shape);
			fracpatch.CalcMidpointFM2D();

			bufferSize = (int)Math.Pow(2, maxLevel)+1;

			double max = 0;
			scale = 1.0;
			points = fracpatch.X;

			for (indexX = 0; indexX < bufferSize; indexX++)
				for (indexY=0; indexY<bufferSize; indexY++)
				{
					if (points[indexX,indexY] < 0) points[indexX,indexY]=0;
					if (points[indexX,indexY] > max) max = points[indexX,indexY];
				}

			scale = 12.0/max;

			vert_size = (bufferSize*6)*bufferSize;

		}


        
             
        /// <summary>
        /// Handle the vertex creation event
        /// </summary>
        public void OnCreateVertexBuffer(object sender, EventArgs e)
		{
			VertexBuffer vb = (VertexBuffer)sender;
			// Create a vertex buffer (100 customervertex)
			CustomVertex.PositionNormalColored[] verts = (CustomVertex.PositionNormalColored[])vb.Lock(0,0); // Lock the buffer (which will return our structs)
	
			for (indexX = 0; indexX<bufferSize; indexX++)
				for (indexY=0; indexY<bufferSize; indexY++)
				{
					verts[indexY+(indexX*bufferSize)].SetPosition(new Vector3(indexX, (float)points[indexX,indexY], indexY));
					verts[indexY+(indexX*bufferSize)].SetNormal(new Vector3(indexX, -(float)points[indexX,indexY], indexY));
					
					// set the color of the vertices
					if ((points[indexX,indexY]*scale==0)) verts[(indexY+(indexX*bufferSize))].Color = Color.Blue.ToArgb();
					if ((points[indexX,indexY]*scale>0) & (points[indexX,indexY]*scale)<1) verts[(indexY+(indexX*bufferSize))].Color = Color.MediumBlue.ToArgb();
					if ((points[indexX,indexY]*scale>=1) & (points[indexX,indexY]<3)) verts[(indexY+(indexX*bufferSize))].Color = Color.Green.ToArgb();
					if ((points[indexX,indexY]*scale>=3) & (points[indexX,indexY]<5)) verts[(indexY+(indexX*bufferSize))].Color = Color.DarkGreen.ToArgb();
					if ((points[indexX,indexY]*scale>=5) & (points[indexX,indexY]<7)) verts[(indexY+(indexX*bufferSize))].Color = Color.Gray.ToArgb();
					if ((points[indexX,indexY]*scale>=7) & (points[indexX,indexY]<10)) verts[(indexY+(indexX*bufferSize))].Color = Color.DarkGray.ToArgb();
					if ((points[indexX,indexY]*scale>=10) & (points[indexX,indexY]<12)) verts[(indexY+(indexX*bufferSize))].Color = Color.White.ToArgb();

				}

			// Unlock (and copy) the data
			vb.Unlock();

		}

        
        
        
        /// <summary>
        /// Handle the index buffer creation event
        /// </summary>
        public void OnCreateIndexBuffer(object sender, EventArgs e)
		{

			IndexBuffer g = (IndexBuffer)sender;
			for (indexY=1; indexY<bufferSize-1; indexY++)
				for (indexX=1;indexX<bufferSize-1; indexX++)
				{
					indices[6*((indexX-1)+((indexY-1)*(bufferSize)))] = (short)((indexY-1)*bufferSize + (indexX-1));
					indices[6*((indexX-1)+((indexY-1)*(bufferSize)))+1] = (short)((indexY-0)*bufferSize + (indexX-1));
					indices[6*((indexX-1)+((indexY-1)*(bufferSize)))+2] = (short)((indexY-1)*bufferSize + (indexX-0));

					indices[6*((indexX-1)+((indexY-1)*(bufferSize)))+3] = (short)((indexY-1)*bufferSize + (indexX-0));
					indices[6*((indexX-1)+((indexY-1)*(bufferSize)))+4] = (short)((indexY-0)*bufferSize + (indexX-1));
					indices[6*((indexX-1)+((indexY-1)*(bufferSize)))+5] = (short)((indexY-0)*bufferSize + (indexX-0));
				}

			g.SetData(indices, 0, 0);

			
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
