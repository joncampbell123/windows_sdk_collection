'-----------------------------------------------------------------------------
' File: Matrices.vb
'
' Desc: Now that we know how to create a device and render some 2D vertices,
'       this tutorial goes the next step and renders 3D geometry. To deal with
'       3D geometry we need to introduce the use of 4x4 matrices to transform
'       the geometry with translations, rotations, scaling, and setting up our
'       camera.
'
'       Geometry is defined in model space. We can move it (translation),
'       rotate it (rotation), or stretch it (scaling) using a world transform.
'       The geometry is then said to be in world space. Next, we need to
'       position the camera, or eye point, somewhere to look at the geometry.
'       Another transform, via the view matrix, is used, to position and
'       rotate our view. With the geometry then in view space, our last
'       transform is the projection transform, which "projects" the 3D scene
'       into our 2D viewport.
'
'       Note that in this tutorial, we are introducing the use of D3DX, which
'       is a set of helper utilities for D3D. In this case, we are using some
'       of D3DX's useful matrix initialization functions. To use D3DX, simply
'       include the D3DX reference in your project
'
' Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace MatricesTutorial
    Public Class Matrices
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
            Me.Text = "Direct3D Tutorial 3 - Matrices"
        End Sub 'New


        Public Function InitializeGraphics() As Boolean
            Try
                ' Now let's setup our D3D stuff
                presentParams.Windowed = True
                presentParams.SwapEffect = SwapEffect.Discard
                device = New Device(0, DeviceType.Hardware, Me, CreateFlags.SoftwareVertexProcessing, presentParams)
                AddHandler device.DeviceCreated, AddressOf Me.OnCreateDevice
                AddHandler device.DeviceReset, AddressOf Me.OnResetDevice
                Me.OnCreateDevice(device, Nothing)
                Me.OnResetDevice(device, Nothing)
                pause = False
                Return True
            Catch e As DirectXException
                Return False
            End Try
        End Function 'InitializeGraphics

        Public Sub OnCreateDevice(ByVal sender As Object, ByVal e As EventArgs)
            Dim dev As Device = CType(sender, Device)
            ' Now Create the VB
            vertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionColored), 3, dev, 0, CustomVertex.PositionColored.Format, Pool.Default)
            AddHandler vertexBuffer.Created, AddressOf Me.OnCreateVertexBuffer
            Me.OnCreateVertexBuffer(vertexBuffer, Nothing)
        End Sub 'OnCreateDevice

        Public Sub OnResetDevice(ByVal sender As Object, ByVal e As EventArgs)
            Dim dev As Device = CType(sender, Device)
            ' Turn off culling, so we see the front and back of the triangle
            dev.RenderState.CullMode = Cull.None
            ' Turn off D3D lighting, since we are providing our own vertex colors
            dev.RenderState.Lighting = False
        End Sub 'OnResetDevice

        Public Sub OnCreateVertexBuffer(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim verts As CustomVertex.PositionColored() = CType(vb.Lock(0, 0), CustomVertex.PositionColored())
            verts(0).X = -1.0F
            verts(0).Y = -1.0F
            verts(0).Z = 0.0F
            verts(0).Color = System.Drawing.Color.DarkGoldenrod.ToArgb()
            verts(1).X = 1.0F
            verts(1).Y = -1.0F
            verts(1).Z = 0.0F
            verts(1).Color = System.Drawing.Color.MediumOrchid.ToArgb()
            verts(2).X = 0.0F
            verts(2).Y = 1.0F
            verts(2).Z = 0.0F
            verts(2).Color = System.Drawing.Color.Cornsilk.ToArgb()
            vb.Unlock()
        End Sub 'OnCreateVertexBuffer


        Private Sub Render()
            If device Is Nothing Then
                Return
            End If
            If pause Then
                Return
            End If
            'Clear the backbuffer to a blue color 
            device.Clear(ClearFlags.Target, System.Drawing.Color.Blue, 1.0F, 0)
            'Begin the scene
            device.BeginScene()
            ' Setup the world, view, and projection matrices
            SetupMatrices()

            device.SetStreamSource(0, vertexBuffer, 0)
            device.VertexFormat = CustomVertex.PositionColored.Format
            device.DrawPrimitives(PrimitiveType.TriangleList, 0, 1)
            'End the scene
            device.EndScene()
            device.Present()
        End Sub 'Render


        Private Sub SetupMatrices()
            ' For our world matrix, we will just rotate the object about the y-axis.

            ' Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
            ' every 1000 ms. To avoid the loss of precision inherent in very high 
            ' floating point numbers, the system time is modulated by the rotation 
            ' period before conversion to a radian angle.
            Dim iTime As Integer = Environment.TickCount Mod 1000
            Dim fAngle As Single = iTime * (2.0F * Math.PI) / 1000.0F
            device.Transform.World = Matrix.RotationY(fAngle)

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
            device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, 1.0F, 1.0F, 100.0F)
        End Sub 'SetupMatrices


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
            Dim frm As New Matrices()
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
    End Class 'Matrices
End Namespace 'MatricesTutorial