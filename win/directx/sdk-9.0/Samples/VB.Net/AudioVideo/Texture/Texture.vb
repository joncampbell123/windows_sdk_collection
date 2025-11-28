'-----------------------------------------------------------------------------
' File: texture.cs
'
' Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Microsoft.DirectX.AudioVideoPlayback
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace VideoTexture
    _
    Public Class VideoTexture
        Inherits GraphicsSample
        ' Our global variables for this project
        Private vertexBuffer As vertexBuffer = Nothing
        Private texture As texture = Nothing
        Private videoTexture As Video = Nothing



        Public Sub New()
            ' Set it's caption
            Me.Text = "Direct3D Video Textures"
            isUsingMenus = False
            isMultiThreaded = False
        End Sub 'New


        Sub RenderIt(ByVal sender As Object, ByVal e As TextureRenderEventArgs)
            'Set our texture so we can render it
            texture = e.Texture
            RenderTexture()
        End Sub

        Sub MovieOver(ByVal sender As Object, ByVal e As EventArgs)
            videoTexture.Stop()
            videoTexture.Play()
        End Sub 'MovieOver

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

        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As Object, ByVal e As System.EventArgs)

            If vertexBuffer Is Nothing Then
                ' Now Create the VB
                vertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionNormalTextured), 100, Device, Usage.WriteOnly, CustomVertex.PositionNormalTextured.Format, Pool.Default)
                AddHandler vertexBuffer.Created, AddressOf Me.OnCreateVertexBuffer
                Me.OnCreateVertexBuffer(vertexBuffer, Nothing)
            ElseIf vertexBuffer.Disposed Then
                ' Now Create the VB
                vertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionNormalTextured), 100, Device, Usage.WriteOnly, CustomVertex.PositionNormalTextured.Format, Pool.Default)
                AddHandler vertexBuffer.Created, AddressOf Me.OnCreateVertexBuffer
                Me.OnCreateVertexBuffer(vertexBuffer, Nothing)
            End If

            Device.RenderState.Ambient = System.Drawing.Color.White
            ' Turn off culling, so we see the front and back of the triangle
            Device.RenderState.CullMode = Cull.None
            ' Turn off D3D lighting
            Device.RenderState.Lighting = False
            ' Turn on the ZBuffer
            Device.RenderState.ZBufferEnable = True

            Device.SamplerState(0).AddressU = TextureAddress.Clamp
            Device.SamplerState(0).AddressV = TextureAddress.Clamp

            Try

                Dim path As String = DXUtil.FindMediaFile(Nothing, "ruby.avi")
                videoTexture = Video.FromFile(path)
                AddHandler videoTexture.Ending, AddressOf Me.MovieOver
                AddHandler videoTexture.TextureReadyToRender, AddressOf Me.RenderIt

            ' Now start rendering to our texture
                videoTexture.RenderToTexture(Device)
            Catch err As Exception
                MessageBox.Show(String.Format("An error has occurred that will not allow this sample to continue.\r\nException={0}", err.ToString()), "This sample must exit.", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Me.Close()
                Throw err
            End Try

        End Sub

        Private Sub SetupMatrices()
            ' For our world matrix, we will just rotate the object about the y-axis.
            Device.Transform.World = Matrix.RotationAxis(New Vector3(CSng(Math.Cos((Environment.TickCount / 250.0F))), 1, CSng(Math.Sin((Environment.TickCount / 250.0F)))), Environment.TickCount / 1000.0F)

            ' Set up our view matrix. A view matrix can be defined given an eye point,
            ' a point to lookat, and a direction for which way is up. Here, we set the
            ' eye five units back along the z-axis and up three units, look at the
            ' origin, and define "up" to be in the y-direction.
            Device.Transform.View = Matrix.LookAtLH(New Vector3(0.0F, 3.0F, -5.0F), New Vector3(0.0F, 0.0F, 0.0F), New Vector3(0.0F, 1.0F, 0.0F))

            ' For the projection matrix, we set up a perspective transform (which
            ' transforms geometry from 3D view space to 2D viewport space, with
            ' a perspective divide making objects smaller in the distance). To build
            ' a perpsective transform, we need the field of view (1/4 pi is common),
            ' the aspect ratio, and the near and far clipping planes (which define at
            ' what distances geometry should be no longer be rendered).
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4.0F, 1.0F, 1.0F, 100.0F)
        End Sub 'SetupMatrices


        Private Sub RenderTexture()
            'Clear the backbuffer to a blue color 
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)

            'Begin the scene
            Device.BeginScene()
            ' Setup the world, view, and projection matrices
            SetupMatrices()
            ' Setup our texture. Using textures introduces the texture stage states,
            ' which govern how textures get blended together (in the case of multiple
            ' textures) and lighting information. In this case, we are modulating
            ' (blending) our texture with the diffuse color of the vertices.
            Device.SetTexture(0, texture)
            Device.SetStreamSource(0, vertexBuffer, 0)
            Device.VertexFormat = CustomVertex.PositionNormalTextured.Format
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 4 * 25 - 2)
            'End the scene
            Device.EndScene()
            ' Update the screen
            Device.Present()
        End Sub 'Render


        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()

            Dim frm As New VideoTexture()
            Try
                If Not frm.CreateGraphicsSample() Then ' Initialize Direct3D
                    MessageBox.Show("Could not initialize Direct3D.  This tutorial will exit.")
                    frm.Dispose()
                    Return
                End If

                If (frm.IsHandleCreated) Then
                    frm.ShowDialog() ' Only need to show the dialog if we haven't closed
                End If
            Finally
                frm.Dispose()
            End Try
        End Sub 'Main

    End Class 'VideoTexture 
End Namespace 'VideoTexture 