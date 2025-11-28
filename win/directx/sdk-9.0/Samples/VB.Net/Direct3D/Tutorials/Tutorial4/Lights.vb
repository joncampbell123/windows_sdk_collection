'-----------------------------------------------------------------------------
' File: Lights.vb
'
' Desc: Rendering 3D geometry is much more interesting when dynamic lighting
'       is added to the scene. To use lighting in D3D, you must create one or
'       more lights, setup a material, and make sure your geometry contains surface
'       normals. Lights may have a position, a color, and be of a certain type
'       such as directional (light comes from one direction), point (light
'       comes from a specific x,y,z coordinate and radiates in all directions)
'       or spotlight. Materials describe the surface of your geometry,
'       specifically, how it gets lit (diffuse color, ambient color, etc.).
'       Surface normals are part of a vertex, and are needed for the D3D's
'       internal lighting calculations.
'
' Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace LightsTutorial
    Public Class Lights
        Inherits Form
        ' Our global variables for this project
        Private device As device = Nothing ' Our rendering device
        Private vertexBuffer As vertexBuffer = Nothing
        Private presentParams As New PresentParameters()
        Private pause As Boolean = False


        Public Sub New()
            ' Set the initial size of our form
            Me.ClientSize = New System.Drawing.Size(400, 300)
            ' And it's caption
            Me.Text = "Direct3D Tutorial 4 - Lights"
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
            vertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionNormal), 100, dev, Usage.WriteOnly, CustomVertex.PositionNormal.Format, Pool.Default)
            AddHandler vertexBuffer.Created, AddressOf Me.OnCreateVertexBuffer
            Me.OnCreateVertexBuffer(vertexBuffer, Nothing)
        End Sub 'OnCreateDevice

        Public Sub OnResetDevice(ByVal sender As Object, ByVal e As EventArgs)
            Dim dev As Device = CType(sender, Device)
            ' Turn off culling, so we see the front and back of the triangle
            device.RenderState.CullMode = Cull.None
            ' Turn on the ZBuffer
            device.RenderState.ZBufferEnable = True
            device.RenderState.Lighting = True 'make sure lighting is enabled
        End Sub 'OnResetDevice

        Public Sub OnCreateVertexBuffer(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            ' Create a vertex buffer (100 customervertex)
            Dim verts As CustomVertex.PositionNormal() = CType(vb.Lock(0, 0), CustomVertex.PositionNormal()) ' Lock the buffer (which will return our structs)
            Dim i As Integer
            For i = 0 To 49
                ' Fill up our structs
                Dim theta As Single = CSng(2 * Math.PI * i) / 49
                verts((2 * i)).SetPosition(New Vector3(CSng(Math.Sin(theta)), -1, CSng(Math.Cos(theta))))
                verts((2 * i)).SetNormal(New Vector3(CSng(Math.Sin(theta)), 0, CSng(Math.Cos(theta))))
                verts((2 * i + 1)).SetPosition(New Vector3(CSng(Math.Sin(theta)), 1, CSng(Math.Cos(theta))))
                verts((2 * i + 1)).SetNormal(New Vector3(CSng(Math.Sin(theta)), 0, CSng(Math.Cos(theta))))
            Next i
            ' Unlock (and copy) the data
            vb.Unlock()
        End Sub 'OnCreateVertexBuffer

        Private Sub SetupMatrices()
            ' For our world matrix, we will just rotate the object about the y-axis.
            device.Transform.World = Matrix.RotationAxis(New Vector3(CSng(Math.Cos((Environment.TickCount / 250.0F))), 1, CSng(Math.Sin((Environment.TickCount / 250.0F)))), Environment.TickCount / 3000.0F)

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

        Private Sub SetupLights()
            Dim col As System.Drawing.Color = System.Drawing.Color.BlueViolet
            'Set up a material. The material here just has the diffuse and ambient
            'colors set to yellow. Note that only one material can be used at a time.
            Dim mtrl As New Direct3D.Material()
            mtrl.Diffuse = col
            mtrl.Ambient = col
            device.Material = mtrl

            'Set up a white, directional light, with an oscillating direction.
            'Note that many lights may be active at a time (but each one slows down
            'the rendering of our scene). However, here we are just using one. Also,
            'we need to set the D3DRS_LIGHTING renderstate to enable lighting

            device.Lights(0).Type = LightType.Directional
            device.Lights(0).Diffuse = System.Drawing.Color.White
            device.Lights(0).Direction = New Vector3(CSng(Math.Cos((Environment.TickCount / 250.0F))), 1.0F, CSng(Math.Sin((Environment.TickCount / 250.0F))))

            device.Lights(0).Commit() 'let d3d know about the light
            device.Lights(0).Enabled = True 'turn it on
            'Finally, turn on some ambient light.
            'Ambient light is light that scatters and lights all objects evenly
            device.RenderState.Ambient = System.Drawing.Color.FromArgb(&H202020)
        End Sub 'SetupLights


        Private Sub Render()
            If pause Then
                Return
            End If
            'Clear the backbuffer to a blue color 
            device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)
            'Begin the scene
            device.BeginScene()
            ' Setup the lights and materials
            SetupLights()
            ' Setup the world, view, and projection matrices
            SetupMatrices()

            device.SetStreamSource(0, vertexBuffer, 0)
            device.VertexFormat = CustomVertex.PositionNormal.Format
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
                Me.Close() ' Esc was pressed
            End If
        End Sub 'OnKeyPress

        Protected Overrides Sub OnResize(ByVal e As System.EventArgs)
            pause = (Me.WindowState = FormWindowState.Minimized Or Not Me.Visible)
        End Sub 'OnResize

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()

            Dim frm As New Lights()
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
    End Class 'Lights
End Namespace 'LightsTutorial