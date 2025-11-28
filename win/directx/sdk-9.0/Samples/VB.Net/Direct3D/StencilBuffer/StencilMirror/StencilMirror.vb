'-----------------------------------------------------------------------------
' File: StencilMirror.vb
'
' Desc: Example code showing how to use stencil buffers to implement planar
'       mirrors.
'
'       Note: This code uses the D3D Framework helper library.
'
' Copyright (c) Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace StencilMirror
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
       _



        '-----------------------------------------------------------------------------
        ' Custom vertex types
        '-----------------------------------------------------------------------------
        Public Structure MeshVertex
            Public p As Vector3
            Public n As Vector3
            Public tu, tv As Single
            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal Or VertexFormats.Texture1
        End Structure 'MeshVertex

        Public Structure MirrorVertex
            Public p As Vector3
            Public n As Vector3
            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal
        End Structure 'MirrorVertex

        Private FogColor As System.Drawing.Color = System.Drawing.Color.FromArgb(&H80)

        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private mirrorVertexBuffer As VertexBuffer = Nothing
        Private mirrorMaterial As New Direct3D.Material() ' Material of the mirror
        Private mirrorMatrix As Matrix = Matrix.Identity ' Matrix to position mirror
        Private terrainMesh As GraphicsMesh = Nothing ' X file of terrain
        Private terrainMatrix As Matrix = Matrix.Identity ' Matrix to position terrain
        Private helicopterMesh As GraphicsMesh = Nothing ' X file object to render
        Private helicopterMatrix As Matrix = Matrix.Identity
        ' Matrix to animate X file object


        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "StencilMirror: Doing Reflections with Stencils"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
            MinDepthBits = 16
            MinStencilBits = 4
            terrainMesh = New GraphicsMesh()
            helicopterMesh = New GraphicsMesh()
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Position the terrain
            terrainMatrix = Matrix.Translation(0.0F, -2.6F, 0.0F)

            ' Position the mirror (water polygon intersecting with terrain)
            mirrorMatrix = Matrix.Translation(0.0F, -1.0F, 0.0F)

            ' Position and animate the main object
            Dim fObjectPosX As Single = 50.0F * CSng(Math.Sin((appTime / 2)))
            Dim fObjectPosY As Single = 6
            Dim fObjectPosZ As Single = 10.0F * CSng(Math.Cos((appTime / 2)))
            Dim matRoll, matPitch, matRotate, matScale, matTranslate As Matrix
            matRoll = Matrix.RotationZ((0.2F * CSng(Math.Sin((appTime / 2)))))
            matRotate = Matrix.RotationY((appTime / 2 - CSng(Math.PI) / 2))
            matPitch = Matrix.RotationX((-0.1F * (1 + CSng(Math.Cos(appTime)))))
            matScale = Matrix.Scaling(0.5F, 0.5F, 0.5F)
            matTranslate = Matrix.Translation(fObjectPosX, fObjectPosY, fObjectPosZ)
            helicopterMatrix = Matrix.Multiply(matScale, matTranslate)
            helicopterMatrix = Matrix.Multiply(matRoll, helicopterMatrix)
            helicopterMatrix = Matrix.Multiply(matRotate, helicopterMatrix)
            helicopterMatrix = Matrix.Multiply(matPitch, helicopterMatrix)

            ' Move the camera around
            Dim fEyeX As Single = 10.0F * CSng(Math.Sin((appTime / 2.0F)))
            Dim fEyeY As Single = 3.0F * CSng(Math.Sin((appTime / 25.0F))) + 13.0F
            Dim fEyeZ As Single = 5.0F * CSng(Math.Cos((appTime / 2.0F)))

            Dim vEyePt As New Vector3(fEyeX, fEyeY, fEyeZ)
            Dim vLookatPt As New Vector3(fObjectPosX, fObjectPosY, fObjectPosZ)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)

            Device.Transform.View = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
        End Sub 'FrameMove





        '-----------------------------------------------------------------------------
        ' Name: RenderScene()
        ' Desc:
        '-----------------------------------------------------------------------------
        Private Sub RenderScene()
            ' Render terrain
            Device.Transform.World = terrainMatrix
            terrainMesh.Render(Device)

            ' Draw the mirror
            Device.SetTexture(0, Nothing)
            Device.Material = mirrorMaterial
            Device.Transform.World = mirrorMatrix
            Device.RenderState.AlphaBlendEnable = True
            Device.RenderState.SourceBlend = Blend.DestinationColor
            Device.RenderState.DestinationBlend = Blend.Zero
            Device.VertexFormat = MirrorVertex.Format
            Device.SetStreamSource(0, mirrorVertexBuffer, 0)
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)
            Device.RenderState.AlphaBlendEnable = False

            ' Draw the object. Note: do this last, in case the object has alpha
            Device.Transform.World = helicopterMatrix
            helicopterMesh.Render(Device)
        End Sub 'RenderScene





        '-----------------------------------------------------------------------------
        ' Name: RenderMirror()
        ' Desc:
        '-----------------------------------------------------------------------------
        Private Sub RenderMirror()
            ' Turn depth buffer off, and stencil buffer on
            Device.RenderState.StencilEnable = True
            Device.RenderState.StencilFunction = Compare.Always
            Device.RenderState.ReferenceStencil = &H1
            Device.RenderState.StencilMask = -1
            Device.RenderState.StencilWriteMask = -1

            Device.RenderState.StencilZBufferFail = StencilOperation.Keep
            Device.RenderState.StencilFail = StencilOperation.Keep
            Device.RenderState.StencilPass = StencilOperation.Replace

            ' Make sure no pixels are written to the z-buffer or frame buffer
            Device.RenderState.ZBufferWriteEnable = False
            Device.RenderState.AlphaBlendEnable = True
            Device.RenderState.SourceBlend = Blend.Zero
            Device.RenderState.DestinationBlend = Blend.One

            ' Draw the reflecting surface into the stencil buffer
            Device.SetTexture(0, Nothing)
            Device.Transform.World = mirrorMatrix
            Device.VertexFormat = MirrorVertex.Format
            Device.SetStreamSource(0, mirrorVertexBuffer, 0)
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Save the view matrix
            Dim matViewSaved As Matrix = Device.Transform.View

            ' Reflect camera in X-Z plane mirror
            Dim matReflect As New Matrix()
            Dim matView As Matrix
            Dim plane As Plane = plane.FromPointNormal(New Vector3(0, 0, 0), New Vector3(0, 1, 0))
            matReflect.Reflect(plane)
            matView = Matrix.Multiply(matReflect, matViewSaved)
            Device.Transform.View = matView

            ' Set a clip plane, so that only objects above the water are reflected, we don't care about any errors
            DirectXException.IgnoreExceptions()
            Device.ClipPlanes(0).Plane = plane
            Device.ClipPlanes(0).Enabled = True
            DirectXException.EnableExceptions()

            ' Setup render states to a blended render scene against mask in stencil
            ' buffer. An important step here is to reverse the cull-order of the
            ' polygons, since the view matrix is being relected.
            Device.RenderState.ZBufferWriteEnable = True
            Device.RenderState.StencilFunction = Compare.Equal
            Device.RenderState.StencilPass = StencilOperation.Keep
            Device.RenderState.SourceBlend = Blend.DestinationColor
            Device.RenderState.DestinationBlend = Blend.Zero
            Device.RenderState.CullMode = Cull.Clockwise

            ' Clear the zbuffer (leave frame- and stencil-buffer intact)
            Device.Clear(ClearFlags.ZBuffer, 0, 1.0F, 0)

            ' Render the scene
            RenderScene()

            ' Restore render states
            Device.RenderState.CullMode = Cull.CounterClockwise
            Device.RenderState.StencilEnable = False
            Device.RenderState.AlphaBlendEnable = False
            Device.ClipPlanes.DisableAll()
            Device.Transform.View = matViewSaved
        End Sub 'RenderMirror




        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer Or ClearFlags.Stencil, FogColor, 1.0F, 0)

            Device.BeginScene()

            ' Render the scene
            RenderScene()

            ' Render the reflection in the mirror
            RenderMirror()

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize the font's internal textures
            drawingFont.InitializeDeviceObjects(Device)

            ' Load the main file object
            If helicopterMesh Is Nothing Then
                helicopterMesh = New GraphicsMesh()
            End If
            Try
                helicopterMesh.Create(Device, "Heli.x")

                ' Load the terrain
                If terrainMesh Is Nothing Then
                    terrainMesh = New GraphicsMesh()
                End If
                terrainMesh.Create(Device, "SeaFloor.x")
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try

            ' Tweak the terrain vertices to add some bumpy terrain
            If Not (terrainMesh Is Nothing) Then
                ' Set FVF to VertexFVF
                terrainMesh.SetFVF(Device, MeshVertex.Format)

                ' Get access to the mesh vertices
                Dim pVB As VertexBuffer = Nothing
                Dim vertices As MeshVertex() = Nothing
                Dim numVertices As Integer = terrainMesh.SysMemMesh.NumberVertices
                pVB = terrainMesh.SysMemMesh.VertexBuffer
                vertices = CType(pVB.Lock(0, GetType(MeshVertex), 0, numVertices), MeshVertex())

                Dim i As Integer
                For i = 0 To numVertices - 1
                    Dim v00 As New Vector3(vertices(i).p.X + 0.0F, 0.0F, vertices(i).p.Z + 0.0F)
                    Dim v10 As New Vector3(vertices(i).p.X + 0.1F, 0.0F, vertices(i).p.Z + 0.0F)
                    Dim v01 As New Vector3(vertices(i).p.X + 0.0F, 0.0F, vertices(i).p.Z + 0.1F)
                    v00.Y = HeightField(1 * v00.X, 1 * v00.Z)
                    v10.Y = HeightField(1 * v10.X, 1 * v10.Z)
                    v01.Y = HeightField(1 * v01.X, 1 * v01.Z)

                    Dim n As Vector3 = Vector3.Cross(Vector3.Subtract(v01, v00), Vector3.Subtract(v10, v00))
                    n.Normalize()

                    vertices(i).p.Y = v00.Y
                    vertices(i).n.X = n.X
                    vertices(i).n.Y = n.Y
                    vertices(i).n.Z = n.Z
                    vertices(i).tu *= 10
                    vertices(i).tv *= 10
                Next i

                pVB.Unlock()
            End If

            If mirrorVertexBuffer Is Nothing Then
                ' Create a big square for rendering the mirror, we don't need to recreate this every time, if the VertexBuffer
                ' is destroyed (by a call to Reset for example), it will automatically be recreated and the 'Created' event fired.
                mirrorVertexBuffer = New VertexBuffer(GetType(MirrorVertex), 4, Device, Usage.WriteOnly, MirrorVertex.Format, Pool.Default)
                AddHandler mirrorVertexBuffer.Created, AddressOf Me.MirrorCreated
                ' Manually fire the created event the first time
                Me.MirrorCreated(mirrorVertexBuffer, Nothing)
            End If
        End Sub 'InitializeDeviceObjects


        Private Sub MirrorCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim v As MirrorVertex() = CType(vb.Lock(0, 0), MirrorVertex())
            v(0).p = New Vector3(-80.0F, 0.0F, -80.0F)
            v(0).n = New Vector3(0.0F, 1.0F, 0.0F)
            v(1).p = New Vector3(-80.0F, 0.0F, 80.0F)
            v(1).n = New Vector3(0.0F, 1.0F, 0.0F)
            v(2).p = New Vector3(80.0F, 0.0F, -80.0F)
            v(2).n = New Vector3(0.0F, 1.0F, 0.0F)
            v(3).p = New Vector3(80.0F, 0.0F, 80.0F)
            v(3).n = New Vector3(0.0F, 1.0F, 0.0F)
            vb.Unlock()
        End Sub 'MirrorCreated




        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Build the device objects for the file-based objecs
            helicopterMesh.RestoreDeviceObjects(Device, Nothing)
            terrainMesh.RestoreDeviceObjects(Device, Nothing)

            ' Set up textures
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear

            ' Set up misc render states
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.DitherEnable = True
            Device.RenderState.SpecularEnable = False
            Device.RenderState.Ambient = System.Drawing.Color.FromArgb(&H555555)

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 5.5F, -15.0F)
            Dim vLookatPt As New Vector3(0.0F, 1.5F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim matWorld, matView, matProj As Matrix

            matWorld = Matrix.Identity
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            Dim fAspect As Single = Device.PresentationParameters.BackBufferWidth / CSng(Device.PresentationParameters.BackBufferHeight)
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 1000.0F)

            Device.Transform.World = matWorld
            Device.Transform.View = matView
            Device.Transform.Projection = matProj

            ' Set up a material
            mirrorMaterial = GraphicsUtility.InitMaterial(System.Drawing.Color.LightBlue)

            ' Set up the light
            GraphicsUtility.InitLight(Device.Lights(0), LightType.Directional, 0.0F, -1.0F, 1.0F)
            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True

            ' Turn on fog
            Dim fFogStart As Single = 80.0F
            Dim fFogEnd As Single = 100.0F
            Device.RenderState.FogEnable = True
            Device.RenderState.FogColor = FogColor
            Device.RenderState.FogTableMode = FogMode.None
            Device.RenderState.FogVertexMode = FogMode.Linear
            Device.RenderState.RangeFogEnable = False
            Device.RenderState.FogStart = fFogStart
            Device.RenderState.FogEnd = fFogEnd
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called when the app is exiting, or the device is being changed, this 
        '/ function deletes any device-dependent objects.
        '/ </summary>
        Protected Overrides Sub DeleteDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            terrainMesh.Dispose()
            helicopterMesh.Dispose()
            mirrorVertexBuffer.Dispose()

            terrainMesh = Nothing
            helicopterMesh = Nothing
            mirrorVertexBuffer = Nothing
        End Sub 'DeleteDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for stencil operations)
            End If
            If vertexProcessingType = vertexProcessingType.PureHardware Then
                Return False ' GetTransform doesn't work on PUREDEVICE
            End If
            ' Make sure device supports directional lights
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.Mixed Then
                If Not caps.VertexProcessingCaps.SupportsDirectionAllLights Then
                    Return False
                End If
                If caps.MaxUserClipPlanes < 1 Then
                    Return False
                End If
            End If

            Return True
        End Function 'ConfirmDevice




        Private Function HeightField(ByVal x As Single, ByVal z As Single) As Single
            Dim y As Single = 0.0F
            y += 7.0F * CSng(Math.Cos((0.051F * x + 0.0F))) * CSng(Math.Sin((0.055F * x + 0.0F)))
            y += 7.0F * CSng(Math.Cos((0.053F * z + 0.0F))) * CSng(Math.Sin((0.057F * z + 0.0F)))
            y += 1.0F * CSng(Math.Cos((0.101F * x + 0.0F))) * CSng(Math.Sin((0.105F * x + 0.0F)))
            y += 1.0F * CSng(Math.Cos((0.103F * z + 0.0F))) * CSng(Math.Sin((0.107F * z + 0.0F)))
            y += 1.0F * CSng(Math.Cos((0.251F * x + 0.0F))) * CSng(Math.Sin((0.255F * x + 0.0F)))
            y += 1.0F * CSng(Math.Cos((0.253F * z + 0.0F))) * CSng(Math.Sin((0.257F * z + 0.0F)))
            Return y
        End Function 'HeightField


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
End Namespace 'StencilMirror