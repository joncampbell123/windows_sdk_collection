'-----------------------------------------------------------------------------
' File: texture.vb
'
' Desc: Better than just lights and materials, 3D objects look much more
'       convincing when texture-mapped. Textures can be thought of as a sort
'       of wallpaper, that is shrinkwrapped to fit a texture. Textures are
'       typically loaded from image files, and D3DX provides a utility to
'       function to do this for us. Like a vertex buffer, textures have
'       Lock() and Unlock() functions to access (read or write) the image
'       data. Textures have a width, height, miplevel, and pixel format. The
'       miplevel is for "mipmapped" textures, an advanced performance-
'       enhancing feature which uses lower resolutions of the texture for
'       objects in the distance where detail is less noticeable. The pixel
'       format determines how the colors are stored in a texel. The most
'       common formats are the 16-bit R5G6B5 format (5 bits of red, 6-bits of
'       green and 5 bits of blue) and the 32-bit A8R8G8B8 format (8 bits each
'       of alpha, red, green, and blue).
'
'       Textures are associated with geometry through texture coordinates.
'       Each vertex has one or more sets of texture coordinates, which are
'       named tu and tv and range from 0.0 to 1.0. Texture coordinates can be
'       supplied by the geometry, or can be automatically generated using
'       Direct3D texture coordinate generation (which is an advanced feature).
'
' Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace TextureTutorial
    Public Class Textures
        Inherits Form
        ' Our global variables for this project
        Private device As device = Nothing ' Our rendering device
        Private vertexBuffer As vertexBuffer = Nothing
        Private texture As texture = Nothing
        Private presentParams As New PresentParameters()
        Private pause As Boolean = False


        Public Sub New()
            ' Set the initial size of our form
            Me.ClientSize = New System.Drawing.Size(400, 300)
            ' And it's caption
            Me.Text = "Direct3D Tutorial 5 - Textures"
        End Sub 'New


        Public Function InitializeGraphics() As Boolean
            Try
                presentParams.Windowed = True ' We don't want to run fullscreen
                presentParams.SwapEffect = SwapEffect.Discard ' Discard the frames 
                presentParams.EnableAutoDepthStencil = True ' Turn on a Depth stencil
                presentParams.AutoDepthStencilFormat = DepthFormat.D16 ' And the stencil format
                device = New Device(0, DeviceType.Hardware, Me, CreateFlags.SoftwareVertexProcessing, presentParams) 'Create a device
                AddHandler device.DeviceCreated, AddressOf Me.OnCreateDevice
                AddHandler device.DeviceReset, AddressOf Me.OnResetDevice
                Me.OnCreateDevice(device, Nothing)
                Me.OnResetDevice(device, Nothing)
                pause = False

                Return True
            Catch e As DirectXException
                ' Catch any errors and return a failure
                Return False
            End Try
        End Function 'InitializeGraphics

        Public Sub OnCreateDevice(ByVal sender As Object, ByVal e As EventArgs)
            Dim dev As Device = CType(sender, Device)
            ' Now Create the VB
            vertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionNormalTextured), 100, dev, Usage.WriteOnly, CustomVertex.PositionNormalTextured.Format, Pool.Default)
            AddHandler vertexBuffer.Created, AddressOf Me.OnCreateVertexBuffer
            Me.OnCreateVertexBuffer(vertexBuffer, Nothing)
        End Sub 'OnCreateDevice

        Public Sub OnResetDevice(ByVal sender As Object, ByVal e As EventArgs)
            Dim dev As Device = CType(sender, Device)
            ' Turn off culling, so we see the front and back of the triangle
            dev.RenderState.CullMode = Cull.None
            ' Turn off D3D lighting
            dev.RenderState.Lighting = False
            ' Turn on the ZBuffer
            dev.RenderState.ZBufferEnable = True
            ' Now create our texture
            texture = TextureLoader.FromFile(dev, Application.StartupPath + "\..\banana.bmp")
        End Sub 'OnResetDevice

        Public Sub OnCreateVertexBuffer(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            ' Create a vertex buffer (100 customervertex)
            Dim verts As CustomVertex.PositionNormalTextured() = CType(vb.Lock(0, 0), CustomVertex.PositionNormalTextured()) ' Lock the buffer (which will return our structs)
            Dim i As Integer
            For i = 0 To 49
                ' Fill up our structs
                Dim theta As Single = CSng(2 * Math.PI * i) / 49
                verts((2 * i)).SetPosition(New Vector3(CSng(Math.Sin(theta)), -1, CSng(Math.Cos(theta))))
                verts((2 * i)).SetNormal(New Vector3(CSng(Math.Sin(theta)), 0, CSng(Math.Cos(theta))))
                verts((2 * i)).Tu = CSng(i) / (50 - 1)
                verts((2 * i)).Tv = 1.0F
                verts((2 * i + 1)).SetPosition(New Vector3(CSng(Math.Sin(theta)), 1, CSng(Math.Cos(theta))))
                verts((2 * i + 1)).SetNormal(New Vector3(CSng(Math.Sin(theta)), 0, CSng(Math.Cos(theta))))
                verts((2 * i + 1)).Tu = CSng(i) / (50 - 1)
                verts((2 * i + 1)).Tv = 0.0F
            Next i
            ' Unlock (and copy) the data
            vb.Unlock()
        End Sub 'OnCreateVertexBuffer

        Private Sub SetupMatrices()
            ' For our world matrix, we will just rotate the object about the y-axis.
            device.Transform.World = Matrix.RotationAxis(New Vector3(CSng(Math.Cos((Environment.TickCount / 250.0F))), 1, CSng(Math.Sin((Environment.TickCount / 250.0F)))), Environment.TickCount / 1000.0F)

            ' Set up our view matrix. A view matrix can be defined given an eye point,
            ' a point to lookat, and a direction for which way is up. Here, we set the
            ' eye five units back along the z-axis and up three units, look at the
            ' origin, and define "up" to be in the y-direction.
            device.Transform.View = Matrix.LookAtLH(New Vector3(0.0F, 3.0F, -5.0F), New Vector3(0.0F, 0.0F, 0.0F), New Vector3(0.0F, 1.0F, 0.0F))

            ' For the projection matrix, we set up a perspective transform (which
            ' transforms geometry from 3D view space to 2D viewport space, with
            ' a perspective divide making objects smaller in the distance). To build
            ' a perpsective transform, we need the field of view (1/4 pi is common),
            ' the aspect ratio, and the near and far clipping planes (which define at
            ' what distances geometry should be no longer be rendered).
            device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4.0F, 1.0F, 1.0F, 100.0F)
        End Sub 'SetupMatrices


        Private Sub Render()
            If pause Then
                Return
            End If
            'Clear the backbuffer to a blue color 
            device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)
            'Begin the scene
            device.BeginScene()
            ' Setup the world, view, and projection matrices
            SetupMatrices()
            ' Setup our texture. Using textures introduces the texture stage states,
            ' which govern how textures get blended together (in the case of multiple
            ' textures) and lighting information. In this case, we are modulating
            ' (blending) our texture with the diffuse color of the vertices.
            device.SetTexture(0, texture)
            device.TextureState(0).ColorOperation = TextureOperation.Modulate
            device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            device.TextureState(0).AlphaOperation = TextureOperation.Disable

            device.SetStreamSource(0, vertexBuffer, 0)
            device.VertexFormat = CustomVertex.PositionNormalTextured.Format
            device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 4 * 25 - 2)
            'End the scene
            device.EndScene()
            ' Update the screen
            device.Present()
        End Sub 'Render


        Protected Overrides Sub OnPaint(ByVal e As System.Windows.Forms.PaintEventArgs)
            Me.Render() ' Render on painting
        End Sub 'OnPaint

        Protected Overrides Sub OnKeyPress(ByVal e As System.Windows.Forms.KeyPressEventArgs)
            If Asc(e.KeyChar) = CInt(System.Windows.Forms.Keys.Escape) Then
                Me.Dispose() ' Esc was pressed
            End If
        End Sub 'OnKeyPress


        Protected Overrides Sub OnResize(ByVal e As System.EventArgs)
            pause = (Me.WindowState = FormWindowState.Minimized Or Not Me.Visible)
        End Sub 'OnResize

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()

            Dim frm As New Textures()
            If Not frm.InitializeGraphics() Then ' Initialize Direct3D
                MessageBox.Show("Could not initialize Direct3D.  This tutorial will exit.")
                Return
            End If
            frm.Show()

            While frm.Created
                frm.Render()
                Application.DoEvents()
            End While
        End Sub 'Main
    End Class 'Textures 
End Namespace 'TextureTutorial