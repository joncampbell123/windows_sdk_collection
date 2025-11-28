'-----------------------------------------------------------------------------
' File: FishEye.vb
'
' Desc: Example code showing how to do a fisheye lens effect with cubemapping.
'       The scene is rendering into a cubemap each frame, and then a
'       funky-shaped object is rendered using the cubemap and an environment
'       map.
'
' Copyright (c) 1997-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D



Namespace FishEyeSample
    _
    '-----------------------------------------------------------------------------
    ' Name: class MyGraphicsSample
    ' Desc: Application class. The base class (GraphicsSample) provides the 
    '       generic functionality needed in all Direct3D samples. MyGraphicsSample 
    '       adds functionality specific to this sample program.
    '-----------------------------------------------------------------------------
    Public Class MyGraphicsSample
        Inherits GraphicsSample

        ' Local variables for the sample
        Private drawingFont As GraphicsFont = Nothing
        Private skyBox As GraphicsMesh = Nothing
        Private cubeTex As CubeTexture = Nothing

        Private fishEyeVertex As VertexBuffer = Nothing
        Private fishEyeIndex As IndexBuffer = Nothing
        Private numFishEyeVertices As Integer = 0
        Private numFishEyeFaces As Integer = 0


        Private NumberRings As Integer = 0
        Private NumberSections As Integer = 0
        Private ScaleSize As Single = 0.0F




        '-----------------------------------------------------------------------------
        ' Name: MyGraphicsSample()
        ' Desc: Application constructor. Sets attributes for the app.
        '-----------------------------------------------------------------------------
        Public Sub New()
            Me.Text = "FishEye: Environment mapping"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon(this.GetType(), "directx.ico");
            enumerationSettings.AppUsesDepthBuffer = True

            drawingFont = New GraphicsFont("Arial", FontStyle.Bold)
            skyBox = New GraphicsMesh()
        End Sub 'New





        '-----------------------------------------------------------------------------
        ' Name: FrameMove()
        ' Desc: Called once per frame, the call is the entry point for animating
        '       the scene.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub FrameMove()
            ' When the window has focus, let the mouse adjust the scene
            If System.Windows.Forms.Form.ActiveForm Is Me Then
                Dim quat As Quaternion = GraphicsUtility.GetRotationFromCursor(Me)
                Device.Transform.World = Matrix.RotationQuaternion(quat)
                RenderSceneIntoCubeMap()
            End If
        End Sub 'FrameMove





        '-----------------------------------------------------------------------------
        ' Name: RenderScene()
        ' Desc: Renders all visual elements in the scene. This is called by the main
        '       Render() function, and also by the RenderIntoCubeMap() function.
        '-----------------------------------------------------------------------------
        Sub RenderScene()
            ' Render the skybox
            ' Save current state
            Dim matViewSave, matProjSave As Matrix
            matViewSave = device.Transform.View
            matProjSave = device.Transform.Projection

            ' Disable zbuffer, center view matrix, and set FOV to 90 degrees
            Dim matView As Matrix = matViewSave
            Dim matProj As Matrix = matViewSave
            matView.M41 = 0.0F : matView.M42 = 0.0F : matView.M43 = 0.0F
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 2, 1.0F, 0.5F, 10000.0F)
            Device.Transform.Projection = matProj
            Device.Transform.View = matView
            device.RenderState.ZBufferEnable = False

            ' Render the skybox
            skyBox.Render(device)

            ' Restore the render states
            device.Transform.Projection = matProjSave
            device.Transform.View = matViewSave
            device.RenderState.ZBufferEnable = True

            ' Render any other elements of the scene here. In this sample, only a
            ' skybox is render, but a much more interesting scene could be rendered
            ' instead.

        End Sub 'RenderScene



        '-----------------------------------------------------------------------------
        ' Name: RenderSceneIntoCubeMap()
        ' Desc: Renders the scene to each of the 6 faces of the cube map
        '-----------------------------------------------------------------------------
        Sub RenderSceneIntoCubeMap()
            ' Save transformation matrices of the device
            Dim matViewSave, matProjSave As Matrix
            matViewSave = Device.Transform.View
            matProjSave = Device.Transform.Projection

            ' Set the projection matrix for a field of view of 90 degrees
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 2, 1.0F, 0.5F, 100.0F)

            ' Get the current view matrix, to concat it with the cubemap view vectors
            Dim matViewDir As Matrix = Device.Transform.View
            matViewDir.M41 = 0.0F
            matViewDir.M42 = 0.0F
            matViewDir.M43 = 0.0F

            ' Store the current backbuffer and zbuffer
            Dim ourBackBuffer As Surface = Device.GetRenderTarget(0)

            ' Render to the six faces of the cube map
            Dim i As Integer
            For i = 0 To 5
                ' Set the view transform for this cubemap surface
                Dim matView As Matrix = GraphicsUtility.GetCubeMapViewMatrix(CType(i, CubeMapFace))
                matView = Matrix.Multiply(matViewDir, matView)
                Device.Transform.View = matView

                ' Set the rendertarget to the i'th cubemap surface
                Dim cubeMapFace As Surface = cubeTex.GetCubeMapSurface(CType(i, CubeMapFace), 0)
                Device.SetRenderTarget(0, cubeMapFace)
                cubeMapFace.Dispose()

                ' Render the scene
                Device.BeginScene()
                RenderScene()
                Device.EndScene()
            Next i

            ' Change the rendertarget back to the main backbuffer
            Device.SetRenderTarget(0, ourBackBuffer)
            ourBackBuffer.Dispose()

            ' Restore the original transformation matrices
            Device.Transform.Projection = matProjSave
            Device.Transform.View = matViewSave
        End Sub 'RenderSceneIntoCubeMap





        '-----------------------------------------------------------------------------
        ' Name: Render()
        ' Desc: Called once per frame, the call is the entry point for 3d
        '       rendering. This function sets up render states, clears the
        '       viewport, and renders the scene.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub Render()
            ' Begin the scene
            Device.BeginScene()

            ' Set the states we want: identity matrix, no z-buffer, and cubemap texture
            ' coordinate generation
            Device.Transform.World = Matrix.Identity
            Device.RenderState.ZBufferEnable = False
            Device.TextureState(0).TextureCoordinateIndex = CInt(TextureCoordinateIndex.CameraSpaceReflectionVector)
            Device.TextureState(0).TextureTransform = TextureTransform.Count3

            ' Render the fisheye lens object with the environment-mapped body.
            Device.SetTexture(0, cubeTex)
            Device.VertexFormat = CustomVertex.PositionNormal.Format
            Device.SetStreamSource(0, fishEyeVertex, 0)
            Device.Indices = fishEyeIndex
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numFishEyeVertices, 0, numFishEyeFaces)

            ' Restore the render states
            Device.TextureState(0).TextureCoordinateIndex = CInt(TextureCoordinateIndex.PassThru)
            Device.TextureState(0).TextureTransform = TextureTransform.Disable
            Device.RenderState.ZBufferEnable = True


            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 21, System.Drawing.Color.Yellow, deviceStats)

            ' End the scene.
            Device.EndScene()
        End Sub 'Render





        '-----------------------------------------------------------------------------
        ' Name: InitializeDeviceObjects()
        ' Desc: Initialize scene objects.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)

            ' Load the file objects
            Try
                skyBox.Create(Device, "lobby_skybox.x")
                GenerateFishEyeLens(20, 20, 1.0F)
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try
        End Sub 'InitializeDeviceObjects





        '-----------------------------------------------------------------------------
        ' Name: RestoreDeviceObjects()
        ' Desc: Restore device-memory objects and state after a device is created or
        '       resized.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As Object, ByVal e As System.EventArgs)
            ' InitializeDeviceObjects for file objects (build textures and vertex buffers)
            skyBox.RestoreDeviceObjects(Device, Nothing)

            ' Create the cubemap
            cubeTex = New CubeTexture(Device, 256, 1, Usage.RenderTarget, Device.PresentationParameters.BackBufferFormat, Pool.Default)

            ' Set default render states
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
            Device.TextureState(0).AlphaArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).AlphaArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).AlphaOperation = TextureOperation.SelectArg1
            Device.SamplerState(0).AddressU = TextureAddress.Mirror
            Device.SamplerState(0).AddressV = TextureAddress.Mirror
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear
            Device.SamplerState(0).MipFilter = TextureFilter.None

            ' Set the transforms
            Dim eyeVector As New Vector3(0.0F, 0.0F, -5.0F)
            Dim lookAtVector As New Vector3(0.0F, 0.0F, 0.0F)
            Dim upVector As New Vector3(0.0F, 1.0F, 0.0F)

            Dim matView, matProj As Matrix
            matView = Matrix.LookAtLH(eyeVector, lookAtVector, upVector)
            matProj = Matrix.OrthoLH(2.0F, 2.0F, 0.5F, 100.0F)
            Device.Transform.World = Matrix.Identity
            Device.Transform.View = matView
            Device.Transform.Projection = matProj
        End Sub 'RestoreDeviceObjects





        '-----------------------------------------------------------------------------
        ' Name: ConfirmDevice()
        ' Desc: Called during device intialization, this code checks the device
        '       for some minimum set of capabilities
        '-----------------------------------------------------------------------------
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            ' GetTransform doesn't work on PUREDEVICE
            If vertexProcessingType = vertexProcessingType.PureHardware Then
                Return False
            End If
            ' Check for cubemapping devices
            If Not caps.TextureCaps.SupportsCubeMap Then
                Return False
            End If
            ' Check that we can create a cube texture that we can render into
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget, ResourceType.CubeTexture, backBufferFmt) Then
                Return False
            End If
            Return True
        End Function 'ConfirmDevice





        '-----------------------------------------------------------------------------
        ' Name: GenerateFishEyeLens()
        ' Desc: Makes vertex and index data for a fish eye lens
        '-----------------------------------------------------------------------------
        Sub GenerateFishEyeLens(ByVal numRings As Integer, ByVal numSections As Integer, ByVal scale As Single)
            ' Save our params
            ScaleSize = scale
            NumberRings = numRings
            NumberSections = numSections

            Dim numTriangles As Integer = (numRings + 1) * numSections * 2
            Dim numVertices As Integer = (numRings + 1) * numSections + 2

            ' Generate space for the required triangles and vertices.
            If fishEyeVertex Is Nothing Then
                CreateFishEyeVertex(numVertices)
            ElseIf fishEyeVertex.Disposed Then
                CreateFishEyeVertex(numVertices)
            End If

            If fishEyeIndex Is Nothing Then
                CreateFishEyeIndex(numTriangles)
            ElseIf fishEyeIndex.Disposed Then
                CreateFishEyeIndex(numTriangles)
            End If

            numFishEyeVertices = numVertices
            numFishEyeFaces = numTriangles
        End Sub 'GenerateFishEyeLens

        Private Sub CreateFishEyeVertex(ByVal numVertices As Integer)
            fishEyeVertex = New VertexBuffer(GetType(CustomVertex.PositionNormal), numVertices, Device, Usage.WriteOnly, CustomVertex.PositionNormal.Format, Pool.Default)
            ' Hook our events
            AddHandler fishEyeVertex.Created, AddressOf Me.FishEyeVertexCreated
            Me.FishEyeVertexCreated(fishEyeVertex, Nothing)
        End Sub

        Private Sub CreateFishEyeIndex(ByVal numTriangles As Integer)
            fishEyeIndex = New IndexBuffer(GetType(Short), numTriangles * 3, Device, Usage.WriteOnly, Pool.Default)
            ' Hook our events
            AddHandler fishEyeIndex.Created, AddressOf Me.FishEyeIndexCreated
            Me.FishEyeIndexCreated(fishEyeIndex, Nothing)
        End Sub




        '/ <summary>
        '/ Called when our fish eye vertex buffer has been created, or recreated due to a
        '/ device reset or change
        '/ </summary>
        '/ <param name="sender">The vertex buffer sending the event</param>
        Sub FishEyeVertexCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim stm As GraphicsStream = vb.Lock(0, 0, 0)

            ' Generate vertices at the end points.
            stm.Write(New CustomVertex.PositionNormal(New Vector3(0.0F, 0.0F, ScaleSize), New Vector3(0.0F, 0.0F, 1.0F)))

            ' Generate vertex points for rings
            Dim r As Single = 0.0F
            Dim i As Integer
            For i = 0 To (NumberRings + 1) - 1
                Dim phi As Single = 0.0F

                Dim j As Integer
                For j = 0 To NumberSections - 1
                    Dim x As Single = r * CSng(Math.Sin(phi))
                    Dim y As Single = r * CSng(Math.Cos(phi))
                    Dim z As Single = 0.5F - 0.5F * (x * x + y * y)

                    Dim nx As Single = -x
                    Dim ny As Single = -y
                    Dim nz As Single = 1.0F

                    stm.Write(New CustomVertex.PositionNormal(New Vector3(x, y, z), New Vector3(nx, ny, nz)))
                    phi += CSng(2 * CSng(Math.PI) / NumberSections)
                Next j

                r += 1.5F / NumberRings
            Next i

            vb.Unlock()
        End Sub 'FishEyeVertexCreated




        '/ <summary>
        '/ Called when our fish eye index buffer has been created, or recreated due to a
        '/ device reset or change
        '/ </summary>
        '/ <param name="sender">The index buffer sending the event</param>
        Sub FishEyeIndexCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim ib As IndexBuffer = CType(sender, IndexBuffer)
            Dim stm As GraphicsStream = ib.Lock(0, 0, 0)

            ' Generate triangles for the centerpiece
            Dim i As Integer
            For i = 0 To (2 * NumberSections) - 1
                stm.Write(CShort(0))
                stm.Write(CShort(i + 1))
                stm.Write(CShort(i + (i + 1) Mod NumberSections))
            Next i

            ' Generate triangles for the rings
            Dim m As Integer = 1 ' 1st vertex begins at 1 to skip top point
            For i = 0 To NumberRings - 1
                Dim j As Integer
                For j = 0 To NumberSections - 1
                    stm.Write(CShort(m + j))
                    stm.Write(CShort(m + NumberSections + j))
                    stm.Write(CShort(m + NumberSections + (j + 1) Mod NumberSections))

                    stm.Write(CShort(m + j))
                    stm.Write(CShort(m + NumberSections + (j + 1) Mod NumberSections))
                    stm.Write(CShort(m + (j + 1) Mod NumberSections))
                Next j
                m += NumberSections
            Next i


            ib.Unlock()
        End Sub 'FishEyeIndexCreated




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
End Namespace 'FishEyeSample