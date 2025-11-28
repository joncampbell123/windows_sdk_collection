'-----------------------------------------------------------------------------
' File: ShadowVolume.vb
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


Namespace ShadowVolumeApplication
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

        Public Structure ShadowVertex
            Public p As Vector4
            Public color As Integer
            Public Const Format As VertexFormats = VertexFormats.Transformed Or VertexFormats.Diffuse
        End Structure 'ShadowVertex


        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private arcBall As GraphicsArcBall ' Used for mouse control of the scene
        Private airplane As New GraphicsMesh() ' the airplane mesh
        Private terrainObject As New GraphicsMesh() ' The terrain
        Private shadowVolume As New shadowVolume() ' The shadow volume object
        Private objectMatrix As Matrix ' The matrices for the objects
        Private terrainMatrix As Matrix
        Private squareVertexBuffer As VertexBuffer = Nothing ' The vertex buffer
        Private fogColor As System.Drawing.Color = System.Drawing.Color.DeepSkyBlue




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "ShadowVolume: RealTime Shadows Using The StencilBuffer"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            arcBall = New GraphicsArcBall(Me)
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
            MinDepthBits = 16
            MinStencilBits = 4
        End Sub 'New





        '-----------------------------------------------------------------------------
        ' Name: OneTimeSceneInitialization()
        ' Desc: Called during initial app startup, this function performs all the
        '       permanent initialization.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub OneTimeSceneInitialization()
            Me.Cursor = System.Windows.Forms.Cursors.SizeAll
        End Sub 'OneTimeSceneInitialization





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Position the terrain
            terrainMatrix = Matrix.Translation(0.0F, 0.0F, 0.0F)

            ' Setup viewing postion from ArcBall
            objectMatrix = arcBall.RotationMatrix
            objectMatrix.Multiply(arcBall.TranslationMatrix)

            ' Move the light
            Dim x As Single = 5
            Dim y As Single = 5
            Dim z As Single = -5
            GraphicsUtility.InitLight(Device.Lights(0), LightType.Point, x, y, z)
            Device.Lights(0).Attenuation0 = 0.9F
            Device.Lights(0).Attenuation1 = 0.0F
            Device.Lights(0).Commit()

            ' Transform the light vector to be in object space
            Dim vLight As New Vector3()
            Dim m As Matrix = Matrix.Invert(objectMatrix)
            vLight.X = x * m.M11 + y * m.M21 + z * m.M31 + m.M41
            vLight.Y = x * m.M12 + y * m.M22 + z * m.M32 + m.M42
            vLight.Z = x * m.M13 + y * m.M23 + z * m.M33 + m.M43

            ' Build the shadow volume
            shadowVolume.Reset()
            shadowVolume.BuildFromMesh(airplane.SysMemMesh, vLight)
        End Sub 'FrameMove





        '-----------------------------------------------------------------------------
        ' Name: RenderShadow()
        ' Desc: Render the shadow
        '-----------------------------------------------------------------------------
        Public Sub RenderShadow()
            ' Disable z-buffer writes (note: z-testing still occurs), and enable the
            ' stencil-buffer
            Device.RenderState.ZBufferWriteEnable = False
            Device.RenderState.StencilEnable = True

            ' Dont bother with interpolating color
            Device.RenderState.ShadeMode = ShadeMode.Flat

            ' Set up stencil compare fuction, reference value, and masks.
            ' Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
            ' Note: since we set up the stencil-test to always pass, the STENCILFAIL
            ' renderstate is really not needed.
            Device.RenderState.StencilFunction = Compare.Always
            Device.RenderState.StencilZBufferFail = StencilOperation.Keep
            Device.RenderState.StencilFail = StencilOperation.Keep

            ' If ztest passes, inc/decrement stencil buffer value
            Device.RenderState.ReferenceStencil = &H1
            Device.RenderState.StencilMask = -1
            Device.RenderState.StencilWriteMask = -1
            Device.RenderState.StencilPass = StencilOperation.Increment

            ' Make sure that no pixels get drawn to the frame buffer
            Device.RenderState.AlphaBlendEnable = True
            Device.RenderState.SourceBlend = Blend.Zero
            Device.RenderState.DestinationBlend = Blend.One
            If Caps.StencilCaps.SupportsTwoSided Then
                ' With 2-sided stencil, we can avoid rendering twice:
                Device.RenderState.TwoSidedStencilMode = True
                Device.RenderState.CounterClockwiseStencilFunction = Compare.Always
                Device.RenderState.CounterClockwiseStencilZBufferFail = StencilOperation.Keep
                Device.RenderState.CounterClockwiseStencilFail = StencilOperation.Keep
                Device.RenderState.CounterClockwiseStencilPass = StencilOperation.Decrement

                Device.RenderState.CullMode = Cull.None

                ' Draw both sides of shadow volume in stencil/z only
                Device.Transform.World = objectMatrix
                shadowVolume.Render(Device)

                Device.RenderState.TwoSidedStencilMode = False
            Else
                ' Draw front-side of shadow volume in stencil/z only
                Device.Transform.World = objectMatrix
                shadowVolume.Render(Device)

                ' Now reverse cull order so back sides of shadow volume are written.
                Device.RenderState.CullMode = Cull.Clockwise

                ' Decrement stencil buffer value
                Device.RenderState.StencilPass = StencilOperation.Decrement

                ' Draw back-side of shadow volume in stencil/z only
                Device.Transform.World = objectMatrix
                shadowVolume.Render(Device)
            End If

            ' Restore render states
            Device.RenderState.ShadeMode = ShadeMode.Gouraud
            Device.RenderState.CullMode = Cull.CounterClockwise
            Device.RenderState.ZBufferWriteEnable = True
            Device.RenderState.StencilEnable = False
            Device.RenderState.AlphaBlendEnable = False
        End Sub 'RenderShadow





        '-----------------------------------------------------------------------------
        ' Name: DrawShadow()
        ' Desc: Draws a big gray polygon over scene according to the mask in the
        '       stencil buffer. (Any pixel with stencil==1 is in the shadow.)
        '-----------------------------------------------------------------------------
        Public Sub DrawShadow()
            ' Set renderstates (disable z-buffering, enable stencil, disable fog, and
            ' turn on alphablending)
            Device.RenderState.ZBufferEnable = False
            Device.RenderState.StencilEnable = True
            Device.RenderState.FogEnable = False
            Device.RenderState.AlphaBlendEnable = True
            Device.RenderState.SourceBlend = Blend.SourceAlpha
            Device.RenderState.DestinationBlend = Blend.InvSourceAlpha

            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.TextureState(0).AlphaArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).AlphaArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).AlphaOperation = TextureOperation.Modulate

            ' Only write where stencil val >= 1 (count indicates # of shadows that
            ' overlap that pixel)
            Device.RenderState.ReferenceStencil = &H1
            Device.RenderState.StencilFunction = Compare.LessEqual
            Device.RenderState.StencilPass = StencilOperation.Keep

            ' Draw a big, gray square
            Device.VertexFormat = ShadowVertex.Format
            Device.SetStreamSource(0, squareVertexBuffer, 0)
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Restore render states
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.StencilEnable = False
            Device.RenderState.FogEnable = True
            Device.RenderState.AlphaBlendEnable = False
        End Sub 'DrawShadow




        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer Or ClearFlags.Stencil, fogColor, 1.0F, 0)

            Device.BeginScene()

            Device.Transform.World = terrainMatrix
            terrainObject.Render(Device)

            Device.Transform.World = objectMatrix
            airplane.Render(Device, True, False)

            ' Render the shadow volume into the stenicl buffer, then add it into
            ' the scene
            RenderShadow()
            DrawShadow()

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
            If airplane Is Nothing Then
                airplane = New GraphicsMesh()
            End If
            Try
                airplane.Create(Device, "airplane 2.x")

                ' Load the terrain
                If terrainObject Is Nothing Then
                    terrainObject = New GraphicsMesh()
                End If
                terrainObject.Create(Device, "SeaFloor.x")
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try

            ' Set a reasonable vertex type
            terrainObject.SetFVF(Device, MeshVertex.Format)
            airplane.SetFVF(Device, MeshVertex.Format)

            ' Tweak the terrain vertices to add some bumpy terrain
            If Not (terrainObject Is Nothing) Then
                ' Get access to the mesh vertices
                Dim vertBuffer As VertexBuffer = Nothing
                Dim vertices As MeshVertex() = Nothing
                Dim numVertices As Integer = terrainObject.SysMemMesh.NumberVertices
                vertBuffer = terrainObject.SysMemMesh.VertexBuffer
                vertices = CType(vertBuffer.Lock(0, GetType(MeshVertex), 0, numVertices), MeshVertex())

                Dim r As New Random()
                ' Add some more bumpiness to the terrain object
                Dim i As Integer
                For i = 0 To numVertices - 1
                    vertices(i).p.Y += 1 * CSng(r.NextDouble())
                    vertices(i).p.Y += 2 * CSng(r.NextDouble())
                    vertices(i).p.Y += 1 * CSng(r.NextDouble())
                Next i

                vertBuffer.Unlock()
                vertBuffer.Dispose()
            End If

        End Sub 'InitializeDeviceObjects


        Private Sub ShadowCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim v As ShadowVertex() = CType(vb.Lock(0, 0), ShadowVertex())
            Dim sx As Single = CSng(Device.PresentationParameters.BackBufferWidth)
            Dim sy As Single = CSng(Device.PresentationParameters.BackBufferHeight)
            v(0).p = New Vector4(0, sy, 0.0F, 1.0F)
            v(1).p = New Vector4(0, 0, 0.0F, 1.0F)
            v(2).p = New Vector4(sx, sy, 0.0F, 1.0F)
            v(3).p = New Vector4(sx, 0, 0.0F, 1.0F)
            v(0).color = &H7F000000
            v(1).color = &H7F000000
            v(2).color = &H7F000000
            v(3).color = &H7F000000
            vb.Unlock()
        End Sub 'ShadowCreated




        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Build the device objects for the file-based objecs
            airplane.RestoreDeviceObjects(Device, Nothing)
            terrainObject.RestoreDeviceObjects(Device, Nothing)

            If squareVertexBuffer Is Nothing Then
                ' Create a big square for rendering the mirror, we don't need to recreate this every time, if the VertexBuffer
                ' is destroyed (by a call to Reset for example), it will automatically be recreated and the 'Created' event fired.
                squareVertexBuffer = New VertexBuffer(GetType(ShadowVertex), 4, Device, Usage.WriteOnly, ShadowVertex.Format, Pool.Default)
                AddHandler squareVertexBuffer.Created, AddressOf Me.ShadowCreated
                ' Manually fire the created event the first time
                Me.ShadowCreated(squareVertexBuffer, Nothing)
            ElseIf squareVertexBuffer.Disposed Then
                ' Create a big square for rendering the mirror, we don't need to recreate this every time, if the VertexBuffer
                ' is destroyed (by a call to Reset for example), it will automatically be recreated and the 'Created' event fired.
                squareVertexBuffer = New VertexBuffer(GetType(ShadowVertex), 4, Device, Usage.WriteOnly, ShadowVertex.Format, Pool.Default)
                AddHandler squareVertexBuffer.Created, AddressOf Me.ShadowCreated
                ' Manually fire the created event the first time
                Me.ShadowCreated(squareVertexBuffer, Nothing)
            End If

            ' Create and set up the shine materials w/ textures
            Device.Material = GraphicsUtility.InitMaterial(System.Drawing.Color.White)

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

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 10.0F, -20.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim matWorld, matView, matProj As Matrix

            matWorld = Matrix.Identity
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            Dim fAspect As Single = Device.PresentationParameters.BackBufferWidth / CSng(Device.PresentationParameters.BackBufferHeight)
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 1000.0F)

            Device.Transform.World = matWorld
            Device.Transform.View = matView
            Device.Transform.Projection = matProj

            ' Turn on fog
            Dim fFogStart As Single = 30.0F
            Dim fFogEnd As Single = 80.0F
            Device.RenderState.FogEnable = True
            Device.RenderState.FogColor = fogColor
            Device.RenderState.FogTableMode = FogMode.None
            Device.RenderState.FogVertexMode = FogMode.Linear
            Device.RenderState.RangeFogEnable = False
            Device.RenderState.FogStart = fFogStart
            Device.RenderState.FogEnd = fFogEnd

            ' Set the ArcBall parameters
            arcBall.SetWindow(Device.PresentationParameters.BackBufferWidth, Device.PresentationParameters.BackBufferHeight, 2.0F)
            arcBall.Radius = 5.0F

            Device.Lights(0).Enabled = True
            Device.RenderState.Lighting = True
            Device.RenderState.Ambient = System.Drawing.Color.FromArgb(&H303030)
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for stencil buffer operations)
            End If
            ' Make sure device supports directional lights
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.Mixed Then
                If Not caps.VertexProcessingCaps.SupportsDirectionAllLights Then
                    Return False
                End If
            End If

            Return True
        End Function 'ConfirmDevice




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
    _ 




    '-----------------------------------------------------------------------------
    ' Name: ShadowVolume
    ' Desc: A shadow volume object
    '-----------------------------------------------------------------------------
    Public Class ShadowVolume
       _



        '-----------------------------------------------------------------------------
        ' Custom vertex types
        '-----------------------------------------------------------------------------
        Public Structure MeshVertex
            Public p As Vector3
            Public n As Vector3
            Public tu, tv As Single
        End Structure 'MeshVertex



        '-----------------------------------------------------------------------------
        ' Class member variables
        '-----------------------------------------------------------------------------
        Private vertices(32000) As Vector3
        Private numVertices As Integer = 0






        '-----------------------------------------------------------------------------
        ' Name: Reset
        ' Desc: Reset the shadow volume
        '-----------------------------------------------------------------------------
        Public Sub Reset()
            numVertices = 0
        End Sub 'Reset






        '-----------------------------------------------------------------------------
        ' Name: Render
        ' Desc: Render the shadow volume
        '-----------------------------------------------------------------------------
        Public Sub Render(ByVal device As Device)
            Device.VertexFormat = VertexFormats.Position
            device.DrawUserPrimitives(PrimitiveType.TriangleList, numVertices / 3, vertices)
        End Sub 'Render





        '-----------------------------------------------------------------------------
        ' Name: BuildFromMesh()
        ' Desc: Takes a mesh as input, and uses it to build a shadowvolume. The
        '       technique used considers each triangle of the mesh, and adds it's
        '       edges to a temporary list. The edge list is maintained, such that
        '       only silohuette edges are kept. Finally, the silohuette edges are
        '       extruded to make the shadow volume vertex list.
        '-----------------------------------------------------------------------------
        Public Sub BuildFromMesh(ByVal mesh As Mesh, ByVal light As Vector3)
            ' Note: the MeshVertex format depends on the FVF of the mesh
            Dim tempVertices As MeshVertex() = Nothing
            Dim indices As Short() = Nothing
            Dim edges As Short() = Nothing

            Dim numFaces As Integer = mesh.NumberFaces
            Dim numVerts As Integer = mesh.NumberVertices
            Dim numEdges As Integer = 0

            ' Allocate a temporary edge list
            edges = New Short(numFaces * 6) {}

            ' Lock the geometry buffers
            tempVertices = CType(mesh.LockVertexBuffer(GetType(MeshVertex), 0, numVerts), MeshVertex())
            indices = CType(mesh.LockIndexBuffer(GetType(Short), 0, numFaces * 3), Short())

            ' For each face
            Dim i As Integer
            For i = 0 To numFaces - 1
                Dim face0 As Short = indices((3 * i + 0))
                Dim face1 As Short = indices((3 * i + 1))
                Dim face2 As Short = indices((3 * i + 2))
                Dim v0 As Vector3 = tempVertices(face0).p
                Dim v1 As Vector3 = tempVertices(face1).p
                Dim v2 As Vector3 = tempVertices(face2).p

                ' Transform vertices or transform light?
                Dim vCross1 As Vector3 = Vector3.Subtract(v2, v1)
                Dim vCross2 As Vector3 = Vector3.Subtract(v1, v0)
                Dim vNormal As Vector3 = Vector3.Cross(vCross1, vCross2)

                If Vector3.Dot(vNormal, light) >= 0.0F Then
                    AddEdge(edges, numEdges, face0, face1)
                    AddEdge(edges, numEdges, face1, face2)
                    AddEdge(edges, numEdges, face2, face0)
                End If
            Next i

            For i = 0 To numEdges - 1
                Dim v1 As Vector3 = tempVertices(edges((2 * i + 0))).p
                Dim v2 As Vector3 = tempVertices(edges((2 * i + 1))).p
                Dim v3 As Vector3 = Vector3.Subtract(v1, Vector3.Multiply(light, 10))
                Dim v4 As Vector3 = Vector3.Subtract(v2, Vector3.Multiply(light, 10))

                ' Add a quad (two triangles) to the vertex list
                vertices(numVertices) = v1 : numVertices += 1
                vertices(numVertices) = v2 : numVertices += 1
                vertices(numVertices) = v3 : numVertices += 1

                vertices(numVertices) = v2 : numVertices += 1
                vertices(numVertices) = v4 : numVertices += 1
                vertices(numVertices) = v3 : numVertices += 1
            Next i

            ' Unlock the geometry buffers
            mesh.UnlockVertexBuffer()
            mesh.UnlockIndexBuffer()
        End Sub 'BuildFromMesh





        '-----------------------------------------------------------------------------
        ' Name: AddEdge()
        ' Desc: Adds an edge to a list of silohuette edges of a shadow volume.
        '-----------------------------------------------------------------------------
        Public Sub AddEdge(ByVal edges() As Short, ByRef numEdges As Integer, ByVal v0 As Short, ByVal v1 As Short)
            ' Remove interior edges (which appear in the list twice)
            Dim i As Integer
            For i = 0 To numEdges - 1
                If edges((2 * i + 0)) = v0 And edges((2 * i + 1)) = v1 Or (edges((2 * i + 0)) = v1 And edges((2 * i + 1)) = v0) Then
                    If numEdges > 1 Then
                        edges((2 * i + 0)) = edges((2 * (numEdges - 1) + 0))
                        edges((2 * i + 1)) = edges((2 * (numEdges - 1) + 1))
                    End If
                    numEdges -= 1
                    Return
                End If
            Next i
            edges((2 * numEdges + 0)) = v0
            edges((2 * numEdges + 1)) = v1
            numEdges += 1
        End Sub 'AddEdge
    End Class 'ShadowVolume
End Namespace 'ShadowVolumeApplication