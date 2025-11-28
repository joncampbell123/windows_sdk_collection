//-----------------------------------------------------------------------------
// File: BumpUnderwater.cs
//
// Desc: Code to simulate underwater distortion.
//       Games could easily make use of this technique to achieve an underwater
//       effect without affecting geometry by rendering the scene to a texture
//       and applying the bump effect on a rectangle mapped with it.
//
// Note: From the Matrox web site demos
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace BumpUnderwaterSample
{
    /// <summary>
    /// Application class. The base class (GraphicsSample) provides the generic 
    /// functionality needed in all Direct3D samples. MyGraphicsSample adds 
    /// functionality specific to this sample program.
    /// </summary>
    public class MyGraphicsSample : GraphicsSample
    {
        /// <summary>
        /// Custom Bump vertex
        /// </summary>
        public struct BumpVertex
        {
            public Vector3 p;
            public Vector3 n;
            public float tu1, tv1;
            public float tu2, tv2;

            public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Normal | VertexFormats.Texture2;
        };



        private GraphicsFont font = null;                 // Font for drawing text
        // Scene
        private VertexBuffer waterBuffer = null;
        private Texture bumpTex = null;
        private Texture background = null;
        bool deviceValidationFailed = false;

        


        /// <summary>
        /// Application constructor. Sets attributes for the app.
        /// </summary>
        public MyGraphicsSample()
        {
            // Set the window text
            this.Text = "BumpUnderWater: Effect Using BumpMapping";
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

            font = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
            enumerationSettings.AppUsesDepthBuffer = true;
        }




        /// <summary>
        /// Called once per frame, the call is the entry point for animating the scene.
        /// </summary>
        protected override void FrameMove()
        {
            device.TextureState[0].BumpEnvironmentMaterial00 = 0.01f;
            device.TextureState[0].BumpEnvironmentMaterial01 = 0.00f;
            device.TextureState[0].BumpEnvironmentMaterial10 = 0.00f;
            device.TextureState[0].BumpEnvironmentMaterial11 = 0.01f;

            // We will lock the data into a stream because we
            // have set the data to be 'WriteOnly'.  This way
            // we won't overwrite any data by seeking to the places 
            // we want to write to.

            // In this case we want to write the first set of UV 
            // coords (the bump map coords) to make the 'underwater'
            // effect
            GraphicsStream stm = waterBuffer.Lock(0,0,0);
            // Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(24, System.IO.SeekOrigin.Current);
            float u1 = 0.000f, v1 = 0.5f*appTime + 2.0f;
            stm.Write(u1); stm.Write(v1); 
            // Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(32, System.IO.SeekOrigin.Current);
            u1 =  0.000f; v1 = 0.5f*appTime;
            stm.Write(u1); stm.Write(v1); 

            // Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(32, System.IO.SeekOrigin.Current);
            u1 =  1.000f; v1 = 0.5f*appTime;
            stm.Write(u1); stm.Write(v1); 

            // Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(32, System.IO.SeekOrigin.Current);
            u1 =  1.000f; v1 = 0.5f*appTime + 2.0f;
            stm.Write(u1); stm.Write(v1); 

            waterBuffer.Unlock();
        }




        /// <summary>
        /// Called once per frame, the call is the entry point for 3d rendering. This 
        /// function sets up render states, clears the viewport, and renders the scene.
        /// </summary>
        protected override void Render()
        {
            // Clear the viewport
            device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.Black.ToArgb(), 1.0f, 0);

            device.BeginScene();


            // Render the waves
            device.SetTexture(0, bumpTex);
            device.SetTexture(1, background);

            device.TextureState[0].TextureCoordinateIndex = 0;
            device.TextureState[0].ColorOperation = TextureOperation.BumpEnvironmentMap;
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorArgument2 = TextureArgument.Current;

            device.TextureState[1].TextureCoordinateIndex = 1;
            device.TextureState[1].ColorOperation = TextureOperation.SelectArg1;
            device.TextureState[1].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[1].ColorArgument2 = TextureArgument.Current;

            device.TextureState[2].ColorOperation = TextureOperation.Disable;

            device.VertexFormat = BumpVertex.Format;
            device.SetStreamSource(0, waterBuffer, 0);

            // Verify that the texture operations are possible on the device
            ValidateDeviceParams validParams = device.ValidateDevice();
            if (validParams.Result != 0)
                // The right thing to do when device validation fails is to try
                // a different rendering technique.  This sample just warns the user.
                deviceValidationFailed = true;

            device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);


            // Output statistics
            font.DrawText(2,  1, System.Drawing.Color.Yellow, frameStats);
            font.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats);

            if (deviceValidationFailed)
            {
                font.DrawText(2, 40, System.Drawing.Color.Yellow, "Warning: Device validation failed.  Rendering may not look right.");
            }

            device.EndScene();
        }




        /// <summary>
        /// Creates a bumpmap from a surface
        /// </summary>
        public void CreateBumpMap()
        {
            int width  = 256;
            int height = 256;

            // Create the bumpmap's surface and texture objects
            bumpTex = new Texture(device, width, height, 1, 0, Format.V8U8, Pool.Managed);

            int pitch;
            // Fill the bumpmap texels to simulate a lens
            byte[] dst = (byte[])bumpTex.LockRectangle(typeof(byte), 0, 0, out pitch, width * height * 2);

            for (int y=0; y<height; y++)
            {
                for (int x=0; x<width; x++)
                {
                    float fx = x/(float)width  - 0.5f;
                    float fy = y/(float)height - 0.5f;

                    byte du = (byte)(64*Math.Cos(4.0f*(fx+fy)*Math.PI));
                    byte dv = (byte)(64*Math.Sin(4.0f*(fx+fy)*Math.PI));

                    dst[(2*x+0) + (y * pitch)] = du;
                    dst[(2*x+1) + (y * pitch)] = dv;
                }
            }
            bumpTex.UnlockRectangle(0);
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
            font.InitializeDeviceObjects(device);
            try
            {
                background = GraphicsUtility.CreateTexture(device, "LobbyXPos.jpg", Format.A8R8G8B8);
                // create a bumpmap from info in source surface
                CreateBumpMap();
                if ((waterBuffer == null) || (waterBuffer.Disposed))
                {
                    // We only need to create this buffer once
                    waterBuffer = new VertexBuffer(typeof(BumpVertex), 4, device, Usage.WriteOnly, BumpVertex.Format, Pool.Managed);
                    waterBuffer.Created += new System.EventHandler(this.WaterBufferCreated);
                    // Call it manually the first time
                    this.WaterBufferCreated(waterBuffer, null);
                }
            }
            catch 
            {
                SampleException e = new MediaNotFoundException();
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit);
                throw e;
            }
        }




        /// <summary>
        /// Set the data for the water's vertex buffer
        /// </summary>
        private void WaterBufferCreated(object sender, EventArgs e)
        {
            VertexBuffer vb = (VertexBuffer)sender;
            BumpVertex[] v = new BumpVertex[4];
            v[0].p = new Vector3(-60.0f,-60.0f, 0.0f); v[0].n = new Vector3(0, 1, 0);
            v[1].p = new Vector3(-60.0f, 60.0f, 0.0f); v[1].n = new Vector3(0, 1, 0);
            v[2].p = new Vector3(60.0f,-60.0f, 0.0f); v[2].n = new Vector3(0, 1, 0);
            v[3].p = new Vector3(60.0f, 60.0f, 0.0f); v[3].n = new Vector3(0, 1, 0);
            v[0].tu2 = 0.000f; v[0].tv2 = 1.0f;
            v[1].tu2 = 0.000f; v[1].tv2 = 0.0f;
            v[2].tu2 = 1.000f; v[2].tv2 = 1.0f;
            v[3].tu2 = 1.000f; v[3].tv2 = 0.0f;
            vb.SetData(v, 0, 0);
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

            // Set the transform matrices
            Vector3 vEyePt    = new Vector3(0.0f, 0.0f, -100.0f);
            Vector3 vLookatPt = new Vector3(0.0f, 0.0f,    0.0f);
            Vector3 vUpVec    = new Vector3(0.0f, 1.0f,    0.0f);
            Matrix matWorld, matView, matProj;

            matWorld = Matrix.Identity;
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
            matProj = Matrix.PerspectiveFovLH(1.00f, 1.0f, 1.0f, 3000.0f);
            device.Transform.World = matWorld;
            device.Transform.View = matView;
            device.Transform.Projection = matProj;

            // Set any appropiate state
            device.RenderState.Ambient = System.Drawing.Color.White;
            device.RenderState.DitherEnable = true;
            device.RenderState.SpecularEnable = false;
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorArgument2 = TextureArgument.Diffuse;
            device.TextureState[0].ColorOperation = TextureOperation.Modulate;
            device.SamplerState[0].MinFilter = TextureFilter.Linear;
            device.SamplerState[0].MagFilter = TextureFilter.Linear;
            device.SamplerState[1].MinFilter = TextureFilter.Linear;
            device.SamplerState[1].MagFilter = TextureFilter.Linear;
        }




        /// <summary>
        /// Called during device initialization, this code checks the device for some 
        /// minimum set of capabilities
        /// </summary>
        protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
            Format adapterFormat, Format backBufferFormat)
        {
            if (!caps.TextureOperationCaps.SupportsBumpEnvironmentMap)
                return false;

            return Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, 
                adapterFormat, 0, ResourceType.Textures, Format.V8U8);
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
