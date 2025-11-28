//-----------------------------------------------------------------------------
// File: texture.cs
//
// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Microsoft.DirectX.AudioVideoPlayback;
using Direct3D=Microsoft.DirectX.Direct3D;

namespace VideoTexture
{
	public class VideoTexture : GraphicsSample
	{
		VertexBuffer vertexBuffer = null;
		Texture texture = null;
        Video videoTexture = null;


		public VideoTexture()
		{
			// Set it's caption
			this.Text = "Direct3D Video Textures";
            isUsingMenus = false;
            isMultiThreaded = true;
		}

        void RenderIt(object sender, TextureRenderEventArgs e)
        {
            lock(this)
            {
                // Set our texture so we can render it
                texture = e.Texture;
                RenderTexture();
            }
        }
        void MovieOver(object sender, EventArgs e)
        {
            videoTexture.Stop();
            videoTexture.Play();
        }
		public void OnCreateVertexBuffer(object sender, EventArgs e)
		{
            VertexBuffer vb = (VertexBuffer)sender;
			// Create a vertex buffer (100 customervertex)
			CustomVertex.PositionNormalTextured[] verts = (CustomVertex.PositionNormalTextured[])vb.Lock(0,0); // Lock the buffer (which will return our structs)
			for (int i = 0; i < 50; i++)
			{
				// Fill up our structs
				float theta = (float)(2 * Math.PI * i) / 49;
				verts[2 * i].SetPosition(new Vector3((float)Math.Sin(theta), -1, (float)Math.Cos(theta)));
				verts[2 * i].SetNormal(new Vector3((float)Math.Sin(theta), 0, (float)Math.Cos(theta)));
				verts[2 * i].Tu       = ((float)i)/(50-1);
				verts[2 * i].Tv       = 1.0f;
				verts[2 * i + 1].SetPosition(new Vector3((float)Math.Sin(theta), 1, (float)Math.Cos(theta)));
				verts[2 * i + 1].SetNormal(new Vector3((float)Math.Sin(theta), 0, (float)Math.Cos(theta)));
				verts[2 * i + 1].Tu       = ((float)i)/(50-1);
				verts[2 * i + 1].Tv       = 0.0f;
			}
			// Unlock (and copy) the data
			vb.Unlock();
		}
		private void SetupMatrices()
		{
			// For our world matrix, we will just rotate the object about the y-axis.
			device.Transform.World = Matrix.RotationAxis(new Vector3((float)Math.Cos(Environment.TickCount / 250.0f),1,(float)Math.Sin(Environment.TickCount / 250.0f)), Environment.TickCount / 1000.0f );

			// Set up our view matrix. A view matrix can be defined given an eye point,
			// a point to lookat, and a direction for which way is up. Here, we set the
			// eye five units back along the z-axis and up three units, look at the
			// origin, and define "up" to be in the y-direction.
			device.Transform.View = Matrix.LookAtLH( new Vector3( 0.0f, 3.0f,-5.0f ), new Vector3( 0.0f, 0.0f, 0.0f ), new Vector3( 0.0f, 1.0f, 0.0f ) );

			// For the projection matrix, we set up a perspective transform (which
			// transforms geometry from 3D view space to 2D viewport space, with
			// a perspective divide making objects smaller in the distance). To build
			// a perpsective transform, we need the field of view (1/4 pi is common),
			// the aspect ratio, and the near and far clipping planes (which define at
			// what distances geometry should be no longer be rendered).
			device.Transform.Projection = Matrix.PerspectiveFovLH( (float)Math.PI / 4.0f, 1.0f, 1.0f, 100.0f );
		}

        


        protected override void RestoreDeviceObjects(object sender, System.EventArgs e)
        {
            // Now Create the VB
            if ((vertexBuffer == null) || (vertexBuffer.Disposed))
            {
                vertexBuffer = new VertexBuffer(typeof(CustomVertex.PositionNormalTextured), 100, device, Usage.WriteOnly, CustomVertex.PositionNormalTextured.Format, Pool.Default);
                vertexBuffer.Created += new System.EventHandler(this.OnCreateVertexBuffer);
                this.OnCreateVertexBuffer(vertexBuffer, null);
            }

            device.RenderState.Ambient = System.Drawing.Color.White;
            // Turn off culling, so we see the front and back of the triangle
            device.RenderState.CullMode = Cull.None;
            // Turn off D3D lighting
            device.RenderState.Lighting = false;
            // Turn on the ZBuffer
            device.RenderState.ZBufferEnable = true;

            device.SamplerState[0].AddressU = TextureAddress.Clamp;
            device.SamplerState[0].AddressV = TextureAddress.Clamp;

            string path = DXUtil.FindMediaFile(null, "ruby.avi");

            try
            {
                videoTexture = Video.FromFile(path);
                videoTexture.Ending += new System.EventHandler(this.MovieOver);
                videoTexture.TextureReadyToRender += new TextureRenderEventHandler(this.RenderIt);

                // Now start rendering to our texture
                videoTexture.RenderToTexture(device);
            }
            catch(Exception err)
            {
                MessageBox.Show(string.Format("An error has occurred that will not allow this sample to continue.\r\nException={0}", err.ToString()), "This sample must exit.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                this.Close();
                throw err;
            }
        }

		private void RenderTexture()
		{
            //Clear the backbuffer to a blue color 
            device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0f, 0);
            //Begin the scene
            device.BeginScene();
            // Setup the world, view, and projection matrices
            SetupMatrices();
            // Setup our texture. Using textures introduces the texture stage states,
            // which govern how textures get blended together (in the case of multiple
            // textures) and lighting information. In this case, we are modulating
            // (blending) our texture with the diffuse color of the vertices.
            device.SetTexture(0,texture);
            device.SetStreamSource(0, vertexBuffer, 0);
            device.VertexFormat = CustomVertex.PositionNormalTextured.Format;
            device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, (4*25)-2);
            //End the scene
            device.EndScene();
            device.Present();
        }

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main() 
		{
            using (VideoTexture frm = new VideoTexture())
            {
                if (!frm.CreateGraphicsSample()) // Initialize Direct3D
                {
                    MessageBox.Show("Could not initialize Direct3D.  This sample will exit.");
                    return;
                }

                if (frm.IsHandleCreated) // Only run if we haven't closed the form yet (ie, from an error)
                    frm.ShowDialog();
            }
		}
	}
}
