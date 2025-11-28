'-----------------------------------------------------------------------------
' File: Fractal.vb
'
' Desc: Draw our fractals as a height map
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace FractalSample
    _


    '/ <summary>
    '/ Application class. The base class (MyGraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample



        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        'Fractal
        Private fracpatch As FractalTool.ElevationPoints = Nothing
        Private shape As Double = 0.5
        Private maxLevel As Integer = 6
        Private bufferSize As Integer = 0
        Private vert_size As Integer = 0
        Private indexX, indexY As Integer
        Private points(,) As Double
        Private ourScale As Double

        'indices buffer
        Private indexBuffer As indexBuffer = Nothing
        Private indices() As Short
        'Vertexbuffer
        Private vertexBuffer As vertexBuffer = Nothing


        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Fractal"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
            AddHandler Me.KeyUp, AddressOf Me.OnPrivateKeyUp
        End Sub 'New




        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Setup the lights and materials
            SetupLights()
            ' Setup the world, view, and projection matrices
            SetupMatrices()
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            'Clear the backbuffer to a black color 
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, Color.Black, 1.0F, 0)
            'Begin the scene
            Device.BeginScene()

            Device.VertexFormat = CustomVertex.PositionNormalColored.Format

            ' set the vertexbuffer stream source
            Device.SetStreamSource(0, vertexBuffer, 0, VertexInformation.GetFormatSize(CustomVertex.PositionNormalColored.Format))
            ' set fill mode
            Device.RenderState.FillMode = FillMode.Solid
            ' set the indices			
            Device.Indices = indexBuffer
            'use the indices buffer
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, bufferSize * bufferSize, 0, vert_size / 3)

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)
            drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Hit 'F' to generate new fractal.")

            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)

            Try
                FractSetup()
            Catch ee As Exception
                Console.WriteLine(ee.ToString())
            End Try

            ' Now Create the VB
            vertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionNormalColored), bufferSize * bufferSize, Device, Usage.WriteOnly, CustomVertex.PositionNormalColored.Format, Pool.Default)
            AddHandler vertexBuffer.Created, AddressOf Me.OnCreateVertexBuffer
            Me.OnCreateVertexBuffer(vertexBuffer, Nothing)

            'create the indices buffer
            indexBuffer = New IndexBuffer(GetType(Short), vert_size, Device, Usage.WriteOnly, Pool.Default)
            indices = New Short((bufferSize * 6 * bufferSize) - 1) {}
            AddHandler indexBuffer.Created, AddressOf Me.OnCreateIndexBuffer
            Me.OnCreateIndexBuffer(indexBuffer, Nothing)
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Turn off culling, so we see the front and back of the triangle
            Device.RenderState.CullMode = Cull.None
            ' Turn on the ZBuffer
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.Lighting = False 'make sure lighting is enabled
        End Sub 'RestoreDeviceObjects




        '/ <summary>
        '/ Event Handler for windows messages
        '/ </summary>
        Private Sub OnPrivateKeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs)
            If e.KeyCode = System.Windows.Forms.Keys.F Then
                FractSetup()
                OnCreateVertexBuffer(vertexBuffer, Nothing)
                OnCreateIndexBuffer(indexBuffer, Nothing)
            End If
        End Sub 'OnPrivateKeyUp



        Private Sub SetupMatrices()
            ' move the object
            Dim temp As Matrix = Matrix.Translation(-(bufferSize * 0.5F), 0, -(bufferSize * 0.5F))

            ' For our world matrix, we will just rotate the object about the indexY-axis.
            Device.Transform.World = Matrix.Multiply(temp, Matrix.RotationAxis(New Vector3(0, CSng(Environment.TickCount) / 2150.0F, 0), Environment.TickCount / 3000.0F))

            ' Set up our view matrix. A view matrix can be defined given an eye point,
            ' a point to lookat, and a direction for which way is up. Here, we set the
            ' eye five units back along the z-axis and up three units, look at the
            ' origin, and define "up" to be in the indexY-direction.
            Device.Transform.View = Matrix.LookAtLH(New Vector3(0.0F, 25.0F, -30.0F), New Vector3(0.0F, 0.0F, 0.0F), New Vector3(0.0F, 1.0F, 0.0F))

            ' For the projection matrix, we set up a perspective transform (which
            ' transforms geometry from 3D view space to 2D viewport space, with
            ' a perspective divide making objects smaller in the distance). To build
            ' a perpsective transform, we need the field of view (1/4 pi is common),
            ' the aspect ratio, and the near and far clipping planes (which define at
            ' what distances geometry should be no longer be rendered).
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4.0F, 1.0F, 1.0F, 100.0F)
        End Sub 'SetupMatrices

        Private Sub SetupLights()
            Dim col As Color = Color.White
            'Set up a material. The material here just has the diffuse and ambient
            'colors set to yellow. Note that only one material can be used at a time.
            Dim mtrl As New Material()
            mtrl.Diffuse = col : mtrl.Ambient = col
            Device.Material = mtrl

            'Set up a white, directional light, with an oscillating direction.
            'Note that many lights may be active at a time (but each one slows down
            'the rendering of our scene). However, here we are just using one. Also,
            'we need to set the renderstate to enable lighting
            Device.Lights(0).Type = LightType.Directional
            Device.Lights(0).Diffuse = Color.Purple
            Device.Lights(0).Direction = New Vector3(0, 1.0F, 0)

            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True
            'Finally, turn on some ambient light.
            'Ambient light is light that scatters and lights all objects evenly
            Device.RenderState.Ambient = Color.Gray
        End Sub 'SetupLights


        Public Sub FractSetup()
            '/////////////////////////////////
            '/Setup the fractal
            '/////////////////////////////////
            fracpatch = New FractalTool.ElevationPoints(maxLevel, False, 2.5, shape)
            fracpatch.CalcMidpointFM2D()

            bufferSize = CInt(Math.Pow(2, maxLevel)) + 1

            Dim max As Double = 0
            ourScale = 1.0
            points = fracpatch.points

            For indexX = 0 To bufferSize - 1
                For indexY = 0 To bufferSize - 1
                    If points(indexX, indexY) < 0 Then
                        points(indexX, indexY) = 0
                    End If
                    If points(indexX, indexY) > max Then
                        max = points(indexX, indexY)
                    End If
                Next indexY
            Next indexX
            ourScale = 12.0 / max

            vert_size = bufferSize * 6 * bufferSize
        End Sub 'FractSetup


        Public Sub OnCreateVertexBuffer(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            ' Create a vertex buffer (100 customervertex)
            Dim verts As CustomVertex.PositionNormalColored() = CType(vb.Lock(0, 0), CustomVertex.PositionNormalColored()) ' Lock the buffer (which will return our structs)
            For indexX = 0 To bufferSize - 1
                For indexY = 0 To bufferSize - 1
                    verts((indexY + indexX * bufferSize)).SetPosition(New Vector3(indexX, CSng(points(indexX, indexY)), indexY))
                    verts((indexY + indexX * bufferSize)).SetNormal(New Vector3(indexX, -CSng(points(indexX, indexY)), indexY))

                    ' set the color of the vertices
                    If points(indexX, indexY) * ourScale = 0 Then
                        verts((indexY + indexX * bufferSize)).Color = Color.Blue.ToArgb()
                    End If
                    If points(indexX, indexY) * ourScale > 0 And points(indexX, indexY) * ourScale < 1 Then
                        verts((indexY + indexX * bufferSize)).Color = Color.MediumBlue.ToArgb()
                    End If
                    If points(indexX, indexY) * ourScale >= 1 And points(indexX, indexY) < 3 Then
                        verts((indexY + indexX * bufferSize)).Color = Color.Green.ToArgb()
                    End If
                    If points(indexX, indexY) * ourScale >= 3 And points(indexX, indexY) < 5 Then
                        verts((indexY + indexX * bufferSize)).Color = Color.DarkGreen.ToArgb()
                    End If
                    If points(indexX, indexY) * ourScale >= 5 And points(indexX, indexY) < 7 Then
                        verts((indexY + indexX * bufferSize)).Color = Color.Gray.ToArgb()
                    End If
                    If points(indexX, indexY) * ourScale >= 7 And points(indexX, indexY) < 10 Then
                        verts((indexY + indexX * bufferSize)).Color = Color.DarkGray.ToArgb()
                    End If
                    If points(indexX, indexY) * ourScale >= 10 And points(indexX, indexY) < 12 Then
                        verts((indexY + indexX * bufferSize)).Color = Color.White.ToArgb()
                    End If
                Next indexY
            Next indexX
            ' Unlock (and copy) the data
            vb.Unlock()
        End Sub 'OnCreateVertexBuffer

        Public Sub OnCreateIndexBuffer(ByVal sender As Object, ByVal e As EventArgs)

            Dim g As IndexBuffer = CType(sender, IndexBuffer)
            For indexY = 1 To (bufferSize - 2)
                For indexX = 1 To (bufferSize - 2)
                    indices((6 * (indexX - 1 + (indexY - 1) * bufferSize))) = CShort((indexY - 1) * bufferSize + (indexX - 1))
                    indices((6 * (indexX - 1 + (indexY - 1) * bufferSize) + 1)) = CShort((indexY - 0) * bufferSize + (indexX - 1))
                    indices((6 * (indexX - 1 + (indexY - 1) * bufferSize) + 2)) = CShort((indexY - 1) * bufferSize + (indexX - 0))

                    indices((6 * (indexX - 1 + (indexY - 1) * bufferSize) + 3)) = CShort((indexY - 1) * bufferSize + (indexX - 0))
                    indices((6 * (indexX - 1 + (indexY - 1) * bufferSize) + 4)) = CShort((indexY - 0) * bufferSize + (indexX - 1))
                    indices((6 * (indexX - 1 + (indexY - 1) * bufferSize) + 5)) = CShort((indexY - 0) * bufferSize + (indexX - 0))
                Next indexX
            Next indexY
            g.SetData(indices, 0, 0)
        End Sub 'OnCreateIndexBuffer



        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Dim d3dApp As New MyGraphicsSample()
            If d3dApp.CreateGraphicsSample() Then
                d3dApp.Run()
            End If
        End Sub 'Main 
    End Class 'MyGraphicsSample
End Namespace 'FractalSample