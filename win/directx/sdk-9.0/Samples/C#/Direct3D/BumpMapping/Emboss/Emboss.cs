//-----------------------------------------------------------------------------
// File: Emboss.cs
//
// Desc: Shows how to do a bumpmapping technique called embossing, in which a
//       heightmap is subtracted from itself, with slightly offset texture
//       coordinates for the second pass.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace BumpWavesSample
{
    /// <summary>
    /// Global Emboss structure
    /// </summary>
    public struct EmbossVertex
    {
        public Vector3 p;
        public Vector3 n;
        public float tu, tv;
        public float tu2, tv2;

        public const VertexFormats Format = VertexFormats.PositionNormal | VertexFormats.Texture2;
    }

	


	/// <summary>
	/// Application class. The base class (GraphicsSample) provides the generic 
	/// functionality needed in all Direct3D samples. MyGraphicsSample adds 
	/// functionality specific to this sample program.
	/// </summary>
	public class MyGraphicsSample : GraphicsSample
	{
		private GraphicsFont drawingFont = null; // Font for drawing text
		// Scene
        GraphicsMesh renderObject = null; // Object to render
        bool isShowingEmbossMethod = true; // Whether to do the embossing

        Texture embossTexture; // The emboss texture
        Vector3 bumpLightPos; // Light position
        Vector3[] tangents = null; // Array of vertex tangents
        Vector3[] binormals = null; // Array of vertex binormals
        Matrix worldMatrix; // The world matrix

		
        // Menu items for this sample
        System.Windows.Forms.MenuItem mnuOptions;
        System.Windows.Forms.MenuItem mnuEmboss;


        
        
        /// <summary>
		/// Application constructor. Sets attributes for the app.
		/// </summary>
		public MyGraphicsSample()
		{
			// Set the window text
			this.Text = "Emboss: BumpMapping Technique";
            try
            {
                // Load the icon from our resources
                System.Resources.ResourceManager resources = new System.Resources.ResourceManager(this.GetType());
                this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            }
            catch
            {
                // It's no big deal if we can't load our icons
            }

			drawingFont = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
			enumerationSettings.AppUsesDepthBuffer = true;


            // Add our new menu options
            mnuOptions = new System.Windows.Forms.MenuItem("&Options");
            mnuEmboss = new System.Windows.Forms.MenuItem("Toggle &emboss mode");
            // Add to the main menu screen
            mnuMain.MenuItems.Add(this.mnuOptions);
            mnuOptions.MenuItems.Add(mnuEmboss);
            mnuEmboss.Shortcut = System.Windows.Forms.Shortcut.CtrlE;
            mnuEmboss.ShowShortcut = true;
            mnuEmboss.Click += new System.EventHandler(this.EmbossModeChanged);
        }




        /// <summary>
        /// To find a tangent that heads in the direction of +tv(texcoords), find 
        /// the components of both vectors on the tangent surface, and add a 
        /// linear combination of the two projections that head in the +tv direction
        /// </summary>
        Vector3 ComputeTangentVector(EmbossVertex vertexA, EmbossVertex vertexB,  EmbossVertex vertexC)
        {
            Vector3 vAB = vertexB.p - vertexA.p;
            Vector3 vAC = vertexC.p - vertexA.p;
            Vector3 n = vertexA.n;

            // Components of vectors to neghboring vertices that are orthogonal to the
            // vertex normal
            Vector3 vProjAB = vAB - (Vector3.Dot(n, vAB) * n);
            Vector3 vProjAC = vAC - (Vector3.Dot(n, vAC) * n);

            // tu and tv texture coordinate differences
            float duAB = vertexB.tu - vertexA.tu;
            float duAC = vertexC.tu - vertexA.tu;
            float dvAB = vertexB.tv - vertexA.tv;
            float dvAC = vertexC.tv - vertexA.tv;

            if (duAC*dvAB > duAB*dvAC)
            {
                duAC = -duAC;
                duAB = -duAB;
            }
    
            Vector3 vTangent = duAC*vProjAB - duAB*vProjAC;
            vTangent.Normalize();
            return vTangent;
        }




        /// <summary>
        /// Compute the tangents and Binormals
        /// </summary>
        void ComputeTangentsAndBinormals()
        {
            EmbossVertex[] vertices = null;
            short[] indices = null;

            int numVertices = 0;
            int numIndices = 0;

            // Gain access to the object's vertex and index buffers
            VertexBuffer vertexBuffer = renderObject.SystemVertexBuffer;
            numVertices = renderObject.SystemMesh.NumberVertices;
            vertices = (EmbossVertex[])vertexBuffer.Lock(0, typeof(EmbossVertex), 0, numVertices);

            IndexBuffer indexBuffer = renderObject.SystemIndexBuffer;
            numIndices = renderObject.SystemMesh.NumberFaces * 3;
            indices = (short[])indexBuffer.Lock(0, typeof(short), 0, numIndices);

            // Allocate space for the vertices' tangents and binormals
            tangents = new Vector3[numVertices];
            binormals = new Vector3[numVertices];

            // Generate the vertices' tangents and binormals
            for (int i = 0; i < numIndices; i += 3)
            {
                short a = indices[i+0];
                short b = indices[i+1];
                short c = indices[i+2];

                // To find a tangent that heads in the direction of +tv(texcoords),
                // find the components of both vectors on the tangent surface ,
                // and add a linear combination of the two projections that head in the +tv direction
                tangents[a] += ComputeTangentVector(vertices[a], vertices[b], vertices[c]);
                tangents[b] += ComputeTangentVector(vertices[b], vertices[a], vertices[c]);
                tangents[c] += ComputeTangentVector(vertices[c], vertices[a], vertices[b]);
            }

            for (int i = 0; i < numVertices; i++)
            {
                // Normalize the tangents
                tangents[i].Normalize();

                // Compute the binormals
                binormals[i] = Vector3.Cross(vertices[i].n, tangents[i]);
            }        

            // Unlock the buffers
            vertexBuffer.Unlock();
            indexBuffer.Unlock();
        }




        /// <summary>
        /// Performs a calculation on each of the vertices' normals to determine
        /// what the texture coordinates should be for the environment map (in this 
        /// case the bump map).
        /// </summary>
        void ApplyEnvironmentMap()
        {
            EmbossVertex[] vertices = null;
            int numVertices = 0;

            // Gain access to the object's vertex and index buffers
            VertexBuffer vertexBuffer = renderObject.LocalVertexBuffer;
            numVertices = renderObject.LocalMesh.NumberVertices;
            vertices = (EmbossVertex[])vertexBuffer.Lock(0, typeof(EmbossVertex), 0, numVertices);

            // Get an inverse world matrix
            Matrix invertWorld = Matrix.Invert(worldMatrix);

            // Get the current light position in object space
            Vector4 transformed = Vector3.Transform(device.Lights[0].Position, invertWorld);
            bumpLightPos.X = transformed.X;
            bumpLightPos.Y = transformed.Y;
            bumpLightPos.Z = transformed.Z;

            // Dimensions of texture needed for shifting tex coords
            SurfaceDescription surfacedesc = embossTexture.GetLevelDescription(0);
    
            // Loop through the vertices, transforming each one and calculating
            // the correct texture coordinates.
            for (short i = 0; i < numVertices; i++)
            {
                // Find light vector in tangent space
                Vector3 vDir = bumpLightPos - vertices[i].p;
                Vector3 vLightToVertex = Vector3.Normalize(vDir);
        
                // Create rotation matrix (rotate into tangent space)
                float r = Vector3.Dot(vLightToVertex, vertices[i].n);

                if (r < 0.0f) 
                {
                    // Don't shift coordinates when light below surface
                    vertices[i].tu2 = vertices[i].tu;
                    vertices[i].tv2 = vertices[i].tv;
                }
                else
                {
                    // Shift coordinates for the emboss effect
                    Vector2 vEmbossShift = Vector2.Empty;
                    vEmbossShift.X = Vector3.Dot(vLightToVertex, tangents[i]);
                    vEmbossShift.Y = Vector3.Dot(vLightToVertex, binormals[i]);
                    vEmbossShift.Normalize();
                    vertices[i].tu2 = vertices[i].tu + vEmbossShift.X / device.PresentationParameters.BackBufferWidth;
                    vertices[i].tv2 = vertices[i].tv - vEmbossShift.Y / device.PresentationParameters.BackBufferHeight;
                }
            }

            vertexBuffer.Unlock();
        }




		/// <summary>
		/// Called once per frame, the call is the entry point for animating the scene.
		/// </summary>
		protected override void FrameMove()
		{
            // Rotate the object
            worldMatrix = Matrix.RotationY(appTime);
            device.Transform.World = worldMatrix;
        }




		/// <summary>
		/// Called once per frame, the call is the entry point for 3d rendering. This 
		/// function sets up render states, clears the viewport, and renders the scene.
		/// </summary>
		protected override void Render()
		{
			// Clear the viewport
			device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.Black, 1.0f, 0);

			device.BeginScene();


            // Stage 0 is the base texture, with the height map in the alpha channel
            device.SetTexture(0, embossTexture);
            device.TextureState[0].TextureCoordinateIndex = 0;
            device.TextureState[0].ColorOperation = TextureOperation.Modulate;
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
            device.TextureState[0].AlphaOperation = TextureOperation.SelectArg1;
            device.TextureState[0].AlphaArgument1 = TextureArgument.TextureColor;

            if (isShowingEmbossMethod)
            {
                // Stage 1 passes through the RGB channels (SelectArg2 = Current), and 
                // does a signed add with the inverted alpha channel. The texture coords
                // associated with Stage 1 are the shifted ones, so the result is:
                //    (height - shifted_height) * tex.RGB * diffuse.RGB
                device.SetTexture(1, embossTexture);
                device.TextureState[1].TextureCoordinateIndex = 1;
                device.TextureState[1].ColorOperation = TextureOperation.SelectArg2;
                device.TextureState[1].ColorArgument1 = TextureArgument.TextureColor;
                device.TextureState[1].ColorArgument2 = TextureArgument.Current;
                device.TextureState[1].AlphaOperation = TextureOperation.AddSigned;
                device.TextureState[1].AlphaArgument1 = TextureArgument.TextureColor | TextureArgument.Complement;
                device.TextureState[1].AlphaArgument2 = TextureArgument.Current;

                // Set up the alpha blender to multiply the alpha channel (monochrome emboss)
                // with the src color (lighted texture)
                device.RenderState.AlphaBlendEnable = true;
                device.RenderState.SourceBlend = Blend.SourceAlpha;
                device.RenderState.DestinationBlend = Blend.Zero;
            }

            // Render the object
            renderObject.Render(device);
    
            // Restore render states
            device.TextureState[1].ColorOperation = TextureOperation.Disable;
            device.TextureState[1].AlphaOperation = TextureOperation.Disable;
            device.RenderState.AlphaBlendEnable = false;

			// Output statistics
			drawingFont.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
			drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);
            drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, "Move the light with the mouse");
            drawingFont.DrawText(2, 60, System.Drawing.Color.Yellow, "Emboss-mode:");
            drawingFont.DrawText(130, 60, System.Drawing.Color.White, isShowingEmbossMethod ? "ON" : "OFF");

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
			try
			{
                // Load texture map. Note that this is a special texture, which has a
                // height field stored in the alpha channel
                embossTexture = GraphicsUtility.CreateTexture(device, "emboss1.dds", Format.A8R8G8B8);

                // Load geometry
                renderObject = new GraphicsMesh();
                renderObject.Create(device, "tiger.x");

                // Set attributes for geometry
                renderObject.SetVertexFormat(device, EmbossVertex.Format);
                renderObject.IsUsingMaterials = false;

                // Compute the object's tangents and binormals, whaich are needed for the 
                // emboss-tecnhique's texture-coordinate shifting calculations
                ComputeTangentsAndBinormals();
            }
			catch 
			{
				SampleException e = new MediaNotFoundException();
				HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
				throw e;
			}
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
            renderObject.RestoreDeviceObjects(device, null);
            // Set up the textures
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
            device.TextureState[0].ColorOperation = TextureOperation.Modulate;
            device.SamplerState[0].MinFilter = TextureFilter.Linear;
            device.SamplerState[0].MagFilter = TextureFilter.Linear;

			// Set the transform matrices
			Vector3 vEyePt = new Vector3(0.0f, 0.0f, 3.5f);
			Vector3 vLookatPt = new Vector3(0.0f, 0.0f, 0.0f);
			Vector3 vUpVec = new Vector3(0.0f, 1.0f, 0.0f);

            Matrix matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
            float aspect = device.PresentationParameters.BackBufferWidth / device.PresentationParameters.BackBufferHeight;

            Matrix matProj = Matrix.PerspectiveFovLH((float)Math.PI / 4, aspect, 1.0f, 1000.0f);
			device.Transform.View = matView;
			device.Transform.Projection = matProj;

            // Setup a material
            device.Material = GraphicsUtility.InitMaterial(System.Drawing.Color.White);

            // Setup a light
            GraphicsUtility.InitLight(device.Lights[0], LightType.Point, 5.0f, 5.0f, -20.0f);
            device.Lights[0].Attenuation0 = 1.0f;
            device.Lights[0].Commit();
            device.Lights[0].Enabled = true;

            // Set miscellaneous render states
            device.RenderState.ZBufferEnable = true;
            device.RenderState.Lighting = true;
            device.RenderState.Ambient = System.Drawing.Color.FromArgb(0x00444444);

            ApplyEnvironmentMap();
		}




		/// <summary>
		/// Called during device initialization, this code checks the device for some 
		/// minimum set of capabilities
		/// </summary>
		protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
			Format adapterFormat, Format backBufferFormat)
		{
            // This sample uses the ADDSIGNED texture blending mode
            if (!caps.TextureOperationCaps.SupportsAddSigned)
				return false;

            if (caps.MaxTextureBlendStages < 2)
                return false;

            return true;
		}




        /// <summary>
        /// Move the light with the mouse
        /// </summary>
        protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
        {
            float w = device.PresentationParameters.BackBufferWidth;
            float h = device.PresentationParameters.BackBufferHeight;
            device.Lights[0].XPosition = 200.0f * (0.5f - e.X / w);
            device.Lights[0].YPosition = 200.0f * (0.5f - e.Y / h);
            device.Lights[0].ZPosition = 100.0f;
            device.Lights[0].Commit();
            ApplyEnvironmentMap();

            // Let the control handle the mouse now
            base.OnMouseMove(e);
        }




        /// <summary>
        /// Fired when the emboss mode has changed
        /// </summary>
        private void EmbossModeChanged(object sender, EventArgs e)
        {
            isShowingEmbossMethod = !isShowingEmbossMethod;
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
