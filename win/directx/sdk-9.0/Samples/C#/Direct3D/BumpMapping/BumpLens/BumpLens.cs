//-----------------------------------------------------------------------------
// File: BumpLens.cs
//
// Desc: Code to simulate a magnifying glass using bumpmapping.
//
// Note: Based on a sample from the Matrox web site
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;




namespace BumpLensSample
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
            public float tu1, tv1;
            public float tu2, tv2;

            public static readonly VertexFormats Format = VertexFormats.Position | VertexFormats.Texture2;
        };


        private GraphicsFont font = null;                 // Font for drawing text
        // Scene
        private VertexBuffer background = null;
        private VertexBuffer lens = null;
        private Texture bumpTex = null;
        private Texture backTex = null;
        private float lensX = 0.0f;
        private float lensY = 0.0f;
        bool deviceValidationFailed = false;


        

        /// <summary>
        /// Application constructor. Sets attributes for the app.
        /// </summary>
        public MyGraphicsSample()
        {
            // Set the window text
            this.Text = "BumpLens: Lens Effect Using BumpMapping";
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

            font = new GraphicsFont("Arial", System.Drawing.FontStyle.Bold);
            enumerationSettings.AppUsesDepthBuffer = false;
        }




        /// <summary>
        /// Called once per frame, the call is the entry point for animating the scene.
        /// </summary>
        protected override void FrameMove()
        {
            // Get a triangle wave between -1 and 1
            lensX = (float)(2 * Math.Abs(2 * ((appTime/2) - Math.Floor(appTime/2)) - 1) - 1);

            // Get a regulated sine wave between -1 and 1
            lensY = (float)(2 * Math.Abs(Math.Sin(appTime)) - 1);

        }




        /// <summary>
        /// Called once per frame, the call is the entry point for 3d rendering. This 
        /// function sets up render states, clears the viewport, and renders the scene.
        /// </summary>
        protected override void Render()
        {
            // Clear the viewport
            device.BeginScene();

            // Render the background
            device.SetTexture(0, backTex);
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
            device.TextureState[1].ColorOperation = TextureOperation.Disable;

            device.VertexFormat = CustomVertex.TransformedColoredTextured.Format;
            device.SetStreamSource(0, background, 0);
            device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);

            // Render the lens
            device.SetTexture(0, bumpTex);
            device.SetTexture(1, backTex);

            device.TextureState[0].ColorOperation = TextureOperation.BumpEnvironmentMap;
            device.TextureState[0].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].ColorArgument2 = TextureArgument.Current;

            device.TextureState[0].BumpEnvironmentMaterial00 = 0.2f;
            device.TextureState[0].BumpEnvironmentMaterial01 = 0.0f;
            device.TextureState[0].BumpEnvironmentMaterial10 = 0.0f;
            device.TextureState[0].BumpEnvironmentMaterial11 = 0.2f;
            device.TextureState[0].BumpEnvironmentLuminanceScale = 1.0f;
            device.TextureState[0].BumpEnvironmentLuminanceOffset = 0.0f;

            device.TextureState[1].ColorOperation = TextureOperation.SelectArg1;
            device.TextureState[1].ColorArgument1 = TextureArgument.TextureColor;
            device.TextureState[1].ColorArgument2 = TextureArgument.Current;

            // Generate texture coords depending on objects camera space position
            Matrix mat = new Matrix();
            mat.M11 = 0.5f; mat.M12 = 0.0f;
            mat.M21 = 0.0f; mat.M22 =-0.5f;
            mat.M31 = 0.0f; mat.M32 = 0.0f;
            mat.M41 = 0.5f; mat.M42 = 0.5f;

            // Scale-by-z here
            Matrix matView, matProj;
            matView = device.Transform.View;
            matProj = device.Transform.Projection;
            Vector3 vEyePt = new Vector3(matView.M41, matView.M42, matView.M43);
            float z = vEyePt.Length();
            mat.M11 *= (matProj.M11 / (matProj.M33 * z + matProj.M34));
            mat.M22 *= (matProj.M22 / (matProj.M33 * z + matProj.M34));

            device.Transform.Texture1 = mat;
            device.TextureState[1].TextureTransform = TextureTransform.Count2;
            device.TextureState[1].TextureCoordinateIndex = (int)TextureCoordinateIndex.CameraSpacePosition | 1;

            // Position the lens
            Matrix matWorld = Matrix.Translation(0.7f * (1000.0f-256.0f)*lensX,
                0.7f * (1000.0f-256.0f)*lensY, 0.0f);
            device.Transform.World = matWorld;

            device.VertexFormat = BumpVertex.Format;
            device.SetStreamSource(0, lens, 0);

            // Verify that the texture operations are possible on the device
            ValidateDeviceParams validParams = device.ValidateDevice();
            if (validParams.Result != 0)
                // The right thing to do when device validation fails is to try
                // a different rendering technique.  This sample just warns the user.
                deviceValidationFailed = true;

            // Render the lens
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
        /// Creates a bumpmap surface
        /// </summary>
        public void CreateBumpMap(int width, int height)
        {

            // Create the bumpmap's surface and texture objects
            bumpTex = new Texture(device, width, height, 1, 0, Format.V8U8, Pool.Managed);

            int pitch;
            // Fill the bumpmap texels to simulate a lens
            byte[] dst = (byte[])bumpTex.LockRectangle(typeof(byte), 0, 0, out pitch, width * height * 2);
            int mid = width/2;
            int count = 0;

            for (int y=0; y<height; y++)
            {
                count = 0;
                for (int x=0; x<width; x++)
                {
                    int x1 = ((x==width-1)  ? x : x+1);
                    int y1 = ((x==height-1) ? y : y+1);

                    float fDistSq00 = (float)((x-mid)*(x-mid) + (y-mid)*(y-mid));
                    float fDistSq01 = (float)((x1-mid)*(x1-mid) + (y-mid)*(y-mid));
                    float fDistSq10 = (float)((x-mid)*(x-mid) + (y1-mid)*(y1-mid));

                    float v00 = (float)((fDistSq00 > (mid*mid)) ? 0.0f : Math.Sqrt((mid*mid) - fDistSq00));
                    float v01 = (float)((fDistSq01 > (mid*mid)) ? 0.0f : Math.Sqrt((mid*mid) - fDistSq01));
                    float v10 = (float)((fDistSq10 > (mid*mid)) ? 0.0f : Math.Sqrt((mid*mid) - fDistSq10));

                    float du = (float)((128/Math.PI)*Math.Atan(v00-v01)); // The delta-u bump value
                    float dv = (float)((128/Math.PI)*Math.Atan(v00-v10)); // The delta-v bump value

                    dst[(count++)  + (y * pitch)] = (byte)du;
                    dst[(count++) + (y * pitch)] = (byte)dv;
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
                // Load the texture for the background image
                backTex = GraphicsUtility.CreateTexture(device, "lake.bmp", Format.Unknown);
                // Create the bump map texture
                CreateBumpMap(256,256);

                // Create a square for rendering the lens
                if ((lens == null) || (lens.Disposed))
                {
                    // We only need to create this buffer once
                    lens = new VertexBuffer(typeof(BumpVertex), 4, device, Usage.WriteOnly, BumpVertex.Format, Pool.Default);
                    lens.Created += new System.EventHandler(this.LensCreated);
                    // Call it manually the first time
                    this.LensCreated(lens, null);
                }

                // Create a square for rendering the background
                if ((background == null) || (background.Disposed))
                {
                    // We only need to create this buffer once
                    background = new VertexBuffer(typeof(CustomVertex.TransformedColoredTextured), 4, device, Usage.WriteOnly, CustomVertex.TransformedColoredTextured.Format, Pool.Default);
                    background.Created += new System.EventHandler(this.BackGroundCreated);
                    // Call it manually the first time
                    this.BackGroundCreated(background, null);
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
        private void BackGroundCreated(object sender, EventArgs e)
        {
            VertexBuffer vb = (VertexBuffer)sender;
            CustomVertex.TransformedColoredTextured[] v = new CustomVertex.TransformedColoredTextured[4];
            for (int i=0; i<4; i ++)
            {
                v[i].SetPosition(new Vector4(0.0f, 0.0f, 0.9f, 1.0f));
                v[i].Color = unchecked((int)0xffffffff);
            }
            v[0].Y = (float)device.PresentationParameters.BackBufferHeight;
            v[2].Y = (float)device.PresentationParameters.BackBufferHeight;
            v[2].X = (float)device.PresentationParameters.BackBufferWidth;
            v[3].X = (float)device.PresentationParameters.BackBufferWidth;
            v[0].Tu = 0.0f; v[0].Tv = 1.0f;
            v[1].Tu = 0.0f; v[1].Tv = 0.0f;
            v[2].Tu = 1.0f; v[2].Tv = 1.0f;
            v[3].Tu = 1.0f; v[3].Tv = 0.0f;
            vb.SetData(v, 0, 0);
        }




        /// <summary>
        /// Set the data for the water's vertex buffer
        /// </summary>
        private void LensCreated(object sender, EventArgs e)
        {
            VertexBuffer vb = (VertexBuffer)sender;
            BumpVertex[] v = new BumpVertex[4];
            v[0].p = new Vector3(-256.0f,-256.0f, 0.0f);
            v[1].p = new Vector3(-256.0f, 256.0f, 0.0f);
            v[2].p = new Vector3(256.0f,-256.0f, 0.0f);
            v[3].p = new Vector3(256.0f, 256.0f, 0.0f);
            v[0].tu1 = 0.0f; v[0].tv1 = 1.0f;
            v[1].tu1 = 0.0f; v[1].tv1 = 0.0f;
            v[2].tu1 = 1.0f; v[2].tv1 = 1.0f;
            v[3].tu1 = 1.0f; v[3].tv1 = 0.0f;
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
            Vector3 vEyePt    = new Vector3(0.0f, 0.0f, -2001.0f);
            Vector3 vLookatPt = new Vector3(0.0f, 0.0f,     0.0f);
            Vector3 vUpVec    = new Vector3(0.0f, 1.0f,     0.0f);
            Matrix matWorld, matView, matProj;

            matWorld = Matrix.Identity;
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec);
            float fAspect = device.PresentationParameters.BackBufferWidth / (float)device.PresentationParameters.BackBufferHeight;
            matProj = Matrix.PerspectiveFovLH((float)Math.PI/4, fAspect, 1.0f, 3000.0f);
            device.Transform.World = matWorld;
            device.Transform.View = matView;
            device.Transform.Projection = matProj;

            // Set any appropiate state
            device.SamplerState[0].MinFilter = TextureFilter.Linear;
            device.SamplerState[0].MagFilter = TextureFilter.Linear;
            device.SamplerState[1].AddressU = TextureAddress.Clamp;
            device.SamplerState[1].AddressV = TextureAddress.Clamp;
            device.SamplerState[1].MinFilter = TextureFilter.Linear;
            device.SamplerState[1].MagFilter = TextureFilter.Linear;
            device.SamplerState[1].MipFilter = TextureFilter.None;
        }




        /// <summary>
        /// Called during device initialization, this code checks the device for some 
        /// minimum set of capabilities
        /// </summary>
        protected override bool ConfirmDevice(Caps caps, VertexProcessingType vertexProcessingType, 
            Format adapterFormat, Format backBufferFormat)
        {
            if (vertexProcessingType == VertexProcessingType.PureHardware)
                return false; // GetTransform doesn't work on PUREDEVICE

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
