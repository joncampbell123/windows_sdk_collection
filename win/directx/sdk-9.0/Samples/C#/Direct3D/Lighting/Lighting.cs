//-----------------------------------------------------------------------------
// File: Lighting.cs
//
// Desc: Example code showing how to use D3D lights.
//
//       Note: This code uses the D3D Framework helper library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace Lighting
{
	// Custom D3D vertex format used by the vertex buffer
	struct MyVertex
	{
		public Vector3 p;       // vertex position
		public Vector3 n;       // vertex normal

		public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Normal;
	};

	

    
    /// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private Mesh wallMesh = null;            // Tessellated plane to serve as the walls and floor
		private Mesh sphereMesh = null;          // Representation of point light
		private Mesh coneMesh = null;            // Representation of dir/spot light
		private GraphicsFont drawingFont = null;                 // Font for drawing text
		private Light lightData;                 // Description of the D3D light
		private uint numberVertsX = 32;                          // Number of vertices in the wall mesh along X
		private uint numberVertsZ = 32;                          // Number of vertices in the wall mesh along Z
		private int numTriangles = 0;		  // Number of triangles in the wall mesh

		


        /// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Lighting";
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
			numTriangles = (int)((numberVertsX-1)*(numberVertsZ-1)*2);
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
			lightData = device.Lights[2];
			// Rotate through the various light types
			device.Lights[2].Type = (LightType)(1+(((uint)appTime)/5)%3);

			// Make sure the light type is supported by the device.  If 
			// VertexProcessingCaps.PositionAllLights is not set, the device does not support 
			// point or spot lights, so change light #2's type to a directional light.
			if (!Caps.VertexProcessingCaps.SupportsPositionAllLights)
			{
				if (device.Lights[2].Type == LightType.Point || device.Lights[2].Type == LightType.Spot)
					device.Lights[2].Type = LightType.Directional;
			}

			// Values for the light position, direction, and color
			float x = (float)Math.Sin(appTime*2.000f);
			float y = (float)Math.Sin(appTime*2.246f);
			float z = (float)Math.Sin(appTime*2.640f);

			byte r = (byte)(0.5f + 0.5f * x * 0xff);
			byte g = (byte)(0.5f + 0.5f * y * 0xff);
			byte b = (byte)(0.5f + 0.5f * z * 0xff);
			device.Lights[2].Diffuse = System.Drawing.Color.FromArgb(0xff, r ,g ,b);
			device.Lights[2].Range = 100.0f;
    
			switch(device.Lights[2].Type)
			{
				case LightType.Point:
					device.Lights[2].Position     = new Vector3(4.5f * x, 4.5f * y, 4.5f * z);
					device.Lights[2].Attenuation1 = 0.4f;
					break;
				case LightType.Directional:
					device.Lights[2].Direction    = new Vector3(x, y, z);
					break;
				case LightType.Spot:
					device.Lights[2].Position     = new Vector3(2.0f * x, 2.0f * y, 2.0f * z);
					device.Lights[2].Direction    = new Vector3(x, y, z);
					device.Lights[2].InnerConeAngle        = 0.5f;
					device.Lights[2].OuterConeAngle          = 1.0f;
					device.Lights[2].Falloff      = 1.0f;
					device.Lights[2].Attenuation0 = 1.0f;
					break;
			}
			device.Lights[2].Commit();
		}




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, 0x000000ff, 1.0f, 0);

			device.BeginScene();
			Matrix matWorld;
			Matrix matTrans;
			Matrix matRotate;

			// Turn on light #0 and #2, and turn off light #1
			device.Lights[0].Enabled = true;
			device.Lights[1].Enabled = false;
			device.Lights[2].Enabled = true;

			// Draw the floor
			matTrans = Matrix.Translation(-5.0f, -5.0f, -5.0f);
			matRotate = Matrix.RotationZ(0.0f);
			matWorld = matRotate * matTrans;
			device.SetTransform(TransformType.World, matWorld);
			wallMesh.DrawSubset(0);

			// Draw the back wall
			matTrans = Matrix.Translation(5.0f,-5.0f, -5.0f);
			matRotate = Matrix.RotationZ((float)Math.PI/2);
			matWorld = matRotate * matTrans;
			device.SetTransform(TransformType.World, matWorld);
			wallMesh.DrawSubset(0);

			// Draw the side wall
			matTrans = Matrix.Translation(-5.0f, -5.0f, 5.0f);
			matRotate = Matrix.RotationX((float)-Math.PI/2);
			matWorld = matRotate * matTrans;
			device.SetTransform(TransformType.World, matWorld);
			wallMesh.DrawSubset(0);

			// Turn on light #1, and turn off light #0 and #2
			device.Lights[0].Enabled = false;
			device.Lights[1].Enabled = true;
			device.Lights[2].Enabled = false;

			// Draw the mesh representing the light
			if (lightData.Type == LightType.Point)
			{
				// Just position the point light -- no need to orient it
				matWorld = Matrix.Translation(lightData.Position.X, 
					lightData.Position.Y, lightData.Position.Z);
				device.SetTransform(TransformType.World, matWorld);
				sphereMesh.DrawSubset(0);
			}
			else
			{
				// Position the light and point it in the light's direction
				Vector3 vecFrom = new Vector3(lightData.Position.X, lightData.Position.Y, lightData.Position.Z);
				Vector3 vecAt = new Vector3(lightData.Position.X + lightData.Direction.X, 
											 lightData.Position.Y + lightData.Direction.Y,
											 lightData.Position.Z + lightData.Direction.Z);
				Vector3 vecUp = new Vector3(0, 1, 0);
				Matrix matWorldInv;
				matWorldInv = Matrix.LookAtLH(vecFrom, vecAt, vecUp);
				matWorld = Matrix.Invert(matWorldInv);
				device.SetTransform(TransformType.World, matWorld);
				coneMesh.DrawSubset(0);
			}

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 21, System.Drawing.Color.Yellow, deviceStats);
			string strLight = (lightData.Type == LightType.Point ? "Point Light" : 
				lightData.Type == LightType.Spot ? "Spot Light" : "Directional Light");
			drawingFont.DrawText(2, 41, System.Drawing.Color.White, strLight);
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
			MyVertex[] v;
			Mesh pWallMeshTemp = null;

			// Create a square grid numberVertsX*numberVertsZ for rendering the wall
			pWallMeshTemp = new Mesh(numTriangles, numTriangles * 3, 
				0, MyVertex.Format, device);

			// Fill in the grid vertex data
			v = (MyVertex[])pWallMeshTemp.LockVertexBuffer(typeof(MyVertex), 0, numTriangles * 3);
			float dX = 1.0f/(numberVertsX-1);
			float dZ = 1.0f/(numberVertsZ-1);
			uint k = 0;
			for (uint z=0; z < (numberVertsZ-1); z++)
			{
				for (uint x=0; x < (numberVertsX-1); x++)
				{
					v[k].p  = new Vector3(10 * x*dX, 0.0f, 10 * z*dZ);
					v[k].n  = new Vector3(0.0f, 1.0f, 0.0f);
					k++;
					v[k].p  = new Vector3(10 * x*dX, 0.0f, 10 * (z+1)*dZ);
					v[k].n  = new Vector3(0.0f, 1.0f, 0.0f);
					k++;
					v[k].p  = new Vector3(10 * (x+1)*dX, 0.0f, 10 * (z+1)*dZ);
					v[k].n  = new Vector3(0.0f, 1.0f, 0.0f);
					k++;
					v[k].p  = new Vector3(10 * x*dX, 0.0f, 10 * z*dZ);
					v[k].n  = new Vector3(0.0f, 1.0f, 0.0f);
					k++;
					v[k].p  = new Vector3(10 * (x+1)*dX, 0.0f, 10 * (z+1)*dZ);
					v[k].n  = new Vector3(0.0f, 1.0f, 0.0f);
					k++;
					v[k].p  = new Vector3(10 * (x+1)*dX, 0.0f, 10 * z*dZ);
					v[k].n  = new Vector3(0.0f, 1.0f, 0.0f);
					k++;
				}
			}
			pWallMeshTemp.UnlockVertexBuffer();

			// Fill in index data
			ushort[] pIndex;
			pIndex = (ushort[])pWallMeshTemp.LockIndexBuffer(typeof(ushort), 0, numTriangles * 3);
			for (ushort iIndex = 0; iIndex < numTriangles * 3; iIndex++)
			{
				pIndex[iIndex] = iIndex;
			}
			pWallMeshTemp.UnlockIndexBuffer();

			// Eliminate redundant vertices
			int[] pdwAdjacency = new int[3 * numTriangles];
			WeldEpsilons we = new WeldEpsilons();
			pWallMeshTemp.GenerateAdjacency(0.01f, pdwAdjacency);
			pWallMeshTemp.WeldVertices(WeldEpsilonsFlags.WeldAll, we, pdwAdjacency);

			// Optimize the mesh
			wallMesh = pWallMeshTemp.Optimize(MeshFlags.OptimizeCompact | MeshFlags.OptimizeVertexCache | 
				MeshFlags.VbDynamic | MeshFlags.VbWriteOnly, pdwAdjacency);

			pWallMeshTemp = null;
			pdwAdjacency = null;

			// Create sphere and cone meshes to represent the lights
			sphereMesh = Mesh.Sphere(device, 0.25f, 20, 20);
			coneMesh = Mesh.Cylinder(device, 0.0f, 0.25f, 0.5f, 20, 20);

			// Set up a material
			Microsoft.DirectX.Direct3D.Material mtrl = GraphicsUtility.InitMaterial(System.Drawing.Color.White);
			device.Material = mtrl;

			// Set miscellaneous render states
			device.RenderState.DitherEnable = false;
			device.RenderState.SpecularEnable = false;

			// Set the world matrix
			Matrix matIdentity = Matrix.Identity;
			device.SetTransform(TransformType.World, matIdentity);

			// Set the view matrix.
			Matrix matView;
			Vector3 vFromPt = new Vector3(-10, 10, -10);
			Vector3 vLookatPt = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vUpVec = new Vector3(0.0f, 1.0f, 0.0f);
			matView = Matrix.LookAtLH(vFromPt, vLookatPt, vUpVec);
			device.SetTransform(TransformType.View, matView);

			// Set the projection matrix
			Matrix matProj;
			float fAspect = ((float)device.PresentationParameters.BackBufferWidth) / device.PresentationParameters.BackBufferHeight;
			matProj = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 1.0f, 100.0f);
			device.SetTransform(TransformType.Projection, matProj);

			// Turn on lighting.
			device.RenderState.Lighting = true;

			// Enable ambient lighting to a dim, grey light, so objects that
			// are not lit by the other lights are not completely black
			device.RenderState.Ambient = System.Drawing.Color.Gray;

			// Set light #0 to be a simple, faint grey directional light so 
			// the walls and floor are slightly different shades of grey
			device.Lights[0].Type = LightType.Directional;
			device.Lights[0].Direction = new Vector3(0.3f, -0.5f, 0.2f);
			device.Lights[0].Diffuse = System.Drawing.Color.FromArgb(64,64,64);
			device.Lights[0].Commit();

			// Set light #1 to be a simple, bright directional light to use 
			// on the mesh representing light #2
			device.Lights[1].Type = LightType.Directional;
			device.Lights[1].Direction = new Vector3(0.5f, -0.5f, 0.5f);
			device.Lights[1].Diffuse = System.Drawing.Color.White;
			device.Lights[1].Commit();

			// Light #2 will be the light used to light the floor and walls.  It will
			// be set up in FrameMove() since it changes every frame.

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
