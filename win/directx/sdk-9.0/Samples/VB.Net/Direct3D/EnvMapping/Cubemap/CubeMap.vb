'-----------------------------------------------------------------------------
' File: CubeMap.vb
'
' Desc: Example code showing how to do environment cube-mapping.
'
' Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D



Namespace CubeMapSample
    _
    '-----------------------------------------------------------------------------
    ' Name: class MyGraphicsSample
    ' Desc: Application class. The base class (GraphicsSample) provides the 
    '       generic functionality needed in all Direct3D samples. MyGraphicsSample 
    '       adds functionality specific to this sample program.
    '-----------------------------------------------------------------------------
    Public Class MyGraphicsSample
        Inherits GraphicsSample


        '-----------------------------------------------------------------------------
        ' Name: effectString
        ' Desc: String containing effect used to render shiny teapot.  Two techniques
        '       are shown.. one which simply uses cubemaps, and a fallback which uses 
        '       a vertex shader to do a sphere-map lookup.
        '-----------------------------------------------------------------------------
        Private effectString As String() = {"texture texCubeMap;" + ControlChars.Lf, "texture texSphereMap;" + ControlChars.Lf, "matrix matWorld;" + ControlChars.Lf, "matrix matView;" + ControlChars.Lf, "matrix matProject;" + ControlChars.Lf, "matrix matWorldView;" + ControlChars.Lf, "technique Cube" + ControlChars.Lf, "{" + ControlChars.Lf, "pass P0" + ControlChars.Lf, "{" + ControlChars.Lf, "VertexShader = null;" + ControlChars.Lf, "WorldTransform[0] = <matWorld>;" + ControlChars.Lf, "ViewTransform = <matView>;" + ControlChars.Lf, "ProjectionTransform = <matProject>;" + ControlChars.Lf, "Texture[0] = <texCubeMap>;" + ControlChars.Lf, "MinFilter[0] = Linear;" + ControlChars.Lf, "MagFilter[0] = Linear;" + ControlChars.Lf, "AddressU[0] = Clamp;" + ControlChars.Lf, "AddressV[0] = Clamp;" + ControlChars.Lf, "AddressW[0] = Clamp;" + ControlChars.Lf, "ColorOp[0] = SelectArg1;" + ControlChars.Lf, "ColorArg1[0] = Texture;" + ControlChars.Lf, "TexCoordIndex[0] = CameraSpaceReflectionVector;" + ControlChars.Lf, "TextureTransformFlags[0] = Count3;" + ControlChars.Lf, "}" + ControlChars.Lf, "}" + ControlChars.Lf, "technique Sphere" + ControlChars.Lf, "{" + ControlChars.Lf, "pass P0" + ControlChars.Lf, "{" + ControlChars.Lf, "VertexShader =" + ControlChars.Lf, "decl" + ControlChars.Lf, "{" + ControlChars.Lf, "}" + ControlChars.Lf, "asm" + ControlChars.Lf, "{" + ControlChars.Lf, "vs.1.1" + ControlChars.Lf, "def c64, 0.25f, 0.5f, 1.0f, -1.0f" + ControlChars.Lf, "dcl_position v0" + ControlChars.Lf, "dcl_normal v1" + ControlChars.Lf, "m4x4 r0, v0, c0" + ControlChars.Lf, "m3x3 r1.xyz, v1, c0" + ControlChars.Lf, "mov r1.w, c64.z" + ControlChars.Lf, "mov r2, -r0" + ControlChars.Lf, "dp3 r3, r2, r2" + ControlChars.Lf, "rsq r3, r3.w" + ControlChars.Lf, "mul r2, r2, r3" + ControlChars.Lf, "dp3 r3, r1, r2" + ControlChars.Lf, "mul r1, r1, r3" + ControlChars.Lf, "add r1, r1, r1" + ControlChars.Lf, "add r3, r1, -r2" + ControlChars.Lf, "mad r4.w, -r3.z, c64.y, c64.y" + ControlChars.Lf, "rsq r4, r4.w" + ControlChars.Lf, "mul r4, r3, r4" + ControlChars.Lf, "mad r4, r4, c64.x, c64.y" + ControlChars.Lf, "m4x4 oPos, r0, c4" + ControlChars.Lf, "mul oT0.xy, r4.xy, c64.zw" + ControlChars.Lf, "mov oT0.zw, c64.z" + ControlChars.Lf, "};" + ControlChars.Lf, "VertexShaderConstant4[0] = <matWorldView>;" + ControlChars.Lf, "VertexShaderConstant4[4] = <matProject>;" + ControlChars.Lf, "Texture[0] = <texSphereMap>;" + ControlChars.Lf, "AddressU[0] = Wrap;" + ControlChars.Lf, "AddressV[0] = Wrap;" + ControlChars.Lf, "MinFilter[0] = Linear;" + ControlChars.Lf, "MagFilter[0] = Linear;" + ControlChars.Lf, "ColorOp[0] = SelectArg1;" + ControlChars.Lf, "ColorArg1[0] = Texture;" + ControlChars.Lf, "}" + ControlChars.Lf, "}" + ControlChars.Lf}


        '-----------------------------------------------------------------------------
        ' Name: struct EnvMappedVertex
        ' Desc: D3D vertex type for environment-mapped objects
        '-----------------------------------------------------------------------------
        Public Structure EnvMappedVertex
            Public p As Vector3 ' Position
            Public n As Vector3 ' Normal
            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal
        End Structure 'EnvMappedVertex

        ' CubeMapResolution indicates how big to make the cubemap texture.  Larger
        ' textures will generate a better-looking reflection.
        Public CubeMapResolution As Integer = 256

        Private projectionMatrix As Matrix
        Private viewMatrix As Matrix
        Private worldMatrix As Matrix
        Private airplaneMatrix As Matrix
        Private trackBallMatrix As Matrix

        Private drawingFont As GraphicsFont = Nothing
        Private shinyTeapotMesh As GraphicsMesh = Nothing
        Private skyBoxMesh As GraphicsMesh = Nothing
        Private airplaneMesh As GraphicsMesh = Nothing

        Private effect As effect = Nothing
        Private renderToEnvironmentMap As renderToEnvironmentMap = Nothing
        Private cubeMap As CubeTexture = Nothing
        Private sphereMap As Texture = Nothing





        '-----------------------------------------------------------------------------
        ' Name: MyGraphicsSample()
        ' Desc: Application constructor. Sets attributes for the app.
        '-----------------------------------------------------------------------------
        Public Sub New()
            Me.Text = "CubeMap: Environment cube-mapping"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon(this.GetType(), "directx.ico");
            enumerationSettings.AppUsesDepthBuffer = True

            drawingFont = New GraphicsFont("Arial", FontStyle.Bold)
            shinyTeapotMesh = New GraphicsMesh()
            skyBoxMesh = New GraphicsMesh()
            airplaneMesh = New GraphicsMesh()

            worldMatrix = Matrix.Identity

            trackBallMatrix = Matrix.Identity
            viewMatrix = Matrix.Translation(0.0F, 0.0F, 3.0F)
        End Sub 'New





        '-----------------------------------------------------------------------------
        ' Name: FrameMove()
        ' Desc: Called once per frame, the call is the entry point for animating
        '       the scene.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub FrameMove()
            ' Animate file object
            Dim mat1 As Matrix = Matrix.Translation(0.0F, 2.0F, 0.0F)
            airplaneMatrix = Matrix.Scaling(0.2F, 0.2F, 0.2F)

            airplaneMatrix = Matrix.Multiply(airplaneMatrix, mat1)
            mat1 = Matrix.RotationX((-2.9F * appTime))
            airplaneMatrix = Matrix.Multiply(airplaneMatrix, mat1)
            mat1 = Matrix.RotationY((1.055F * appTime))
            airplaneMatrix = Matrix.Multiply(airplaneMatrix, mat1)

            ' When the window has focus, let the mouse adjust the camera view
            If Me.Capture Then
                Dim matCursor As Matrix
                Dim quat As Quaternion = GraphicsUtility.GetRotationFromCursor(Me)
                matCursor = Matrix.RotationQuaternion(quat)
                viewMatrix = Matrix.Multiply(trackBallMatrix, matCursor)

                Dim matTrans As Matrix
                matTrans = Matrix.Translation(0.0F, 0.0F, 3.0F)
                viewMatrix = Matrix.Multiply(viewMatrix, matTrans)
            End If

            ' Render the scene into the surfaces of the cubemap
            RenderSceneIntoEnvMap()
        End Sub 'FrameMove





        '-----------------------------------------------------------------------------
        ' Name: RenderSceneIntoEnvMap()
        ' Desc: Renders the scene to each of the 6 faces of the cube map
        '-----------------------------------------------------------------------------
        Private Sub RenderSceneIntoEnvMap()

            ' Set the projection matrix for a field of view of 90 degrees
            Dim matProj As Matrix
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) * 0.5F, 1.0F, 0.5F, 1000.0F)


            ' Get the current view matrix, to concat it with the cubemap view vectors
            Dim matViewDir As Matrix = viewMatrix
            matViewDir.M41 = 0.0F
            matViewDir.M42 = 0.0F
            matViewDir.M43 = 0.0F



            ' Render the six cube faces into the environment map
            If Not (cubeMap Is Nothing) Then
                renderToEnvironmentMap.BeginCube(cubeMap)
            Else
                renderToEnvironmentMap.BeginSphere(sphereMap)
            End If
            Dim i As Integer
            For i = 0 To 5
                renderToEnvironmentMap.Face(CType(i, CubeMapFace), 1)

                ' Set the view transform for this cubemap surface
                Dim matView As Matrix = Matrix.Multiply(matViewDir, GraphicsUtility.GetCubeMapViewMatrix(CType(i, CubeMapFace)))

                ' Render the scene (except for the teapot)
                RenderScene(matView, matProj, False)
            Next i

            renderToEnvironmentMap.End(1)
        End Sub 'RenderSceneIntoEnvMap





        '-----------------------------------------------------------------------------
        ' Name: RenderScene()
        ' Desc: Renders all visual elements in the scene. This is called by the main
        '       Render() function, and also by the RenderIntoCubeMap() function.
        '-----------------------------------------------------------------------------
        Private Sub RenderScene(ByVal View As Matrix, ByVal Project As Matrix, ByVal bRenderTeapot As Boolean)
            DirectXException.IgnoreExceptions()
            ' Render the Skybox
            If (True) Then
                Dim matWorld As Matrix = Matrix.Scaling(10.0F, 10.0F, 10.0F)

                Dim matView As Matrix = View
                matView.M41 = 0.0F : matView.M42 = 0.0F : matView.M43 = 0.0F

                Device.Transform.World = matWorld
                Device.Transform.View = matView
                Device.Transform.Projection = Project

                Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
                Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
                Device.SamplerState(0).MinFilter = TextureFilter.Linear
                Device.SamplerState(0).MagFilter = TextureFilter.Linear
                Device.SamplerState(0).AddressU = TextureAddress.Mirror
                Device.SamplerState(0).AddressV = TextureAddress.Mirror

                ' Always pass Z-test, so we can avoid clearing color and depth buffers
                Device.RenderState.ZBufferFunction = Compare.Always
                skyBoxMesh.Render(Device)
                Device.RenderState.ZBufferFunction = Compare.LessEqual
            End If


            ' Render the Airplane
            If (True) Then
                Device.Transform.World = airplaneMatrix
                Device.Transform.View = View
                Device.Transform.Projection = Project

                Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
                Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
                Device.SamplerState(0).MinFilter = TextureFilter.Linear
                Device.SamplerState(0).MagFilter = TextureFilter.Linear
                Device.SamplerState(0).AddressU = TextureAddress.Wrap
                Device.SamplerState(0).AddressV = TextureAddress.Wrap

                airplaneMesh.Render(Device, True, False)

                Device.Transform.World = worldMatrix
            End If


            ' Render the environment-mapped ShinyTeapot
            If bRenderTeapot Then
                ' Set transform state
                If Not (cubeMap Is Nothing) Then
                    effect.SetValue(EffectHandle.FromString("matWorld"), worldMatrix)
                    effect.SetValue(EffectHandle.FromString("matView"), View)
                Else
                    Dim matWorldView As Matrix = Matrix.Multiply(worldMatrix, View)
                    effect.SetValue(EffectHandle.FromString("matWorldView"), matWorldView)
                End If

                effect.SetValue(EffectHandle.FromString("matProject"), Project)


                ' Draw teapot
                Dim pVB As VertexBuffer = shinyTeapotMesh.LocalVertexBuffer
                Dim pIB As IndexBuffer = shinyTeapotMesh.LocalIndexBuffer
                Dim numVert As Integer = shinyTeapotMesh.LocalMesh.NumberVertices
                Dim numFace As Integer = shinyTeapotMesh.LocalMesh.NumberFaces

                Device.SetStreamSource(0, pVB, 0, DXHelp.GetTypeSize(GetType(EnvMappedVertex)))
                Device.VertexFormat = EnvMappedVertex.Format
                Device.Indices = pIB

                Dim Passes As Integer = effect.Begin(0)

                Dim iPass As Integer
                For iPass = 0 To Passes - 1
                    effect.Pass(iPass)

                    Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numVert, 0, numFace)
                Next iPass


                effect.End()
            End If
            DirectXException.EnableExceptions()
        End Sub 'RenderScene





        '-----------------------------------------------------------------------------
        ' Name: Render()
        ' Desc: Called once per frame, the call is the entry point for 3d
        '       rendering. This function sets up render states, clears the
        '       viewport, and renders the scene.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub Render()
            ' Begin the scene
            Device.BeginScene()
            ' Ignore any exceptions during rendering
            DirectXException.IgnoreExceptions()
            ' Render the scene, including the teapot
            RenderScene(viewMatrix, projectionMatrix, True)

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 21, System.Drawing.Color.Yellow, deviceStats)

            ' Turn exception handling back on
            DirectXException.EnableExceptions()
            ' End the scene.
            Device.EndScene()
        End Sub 'Render





        '-----------------------------------------------------------------------------
        ' Name: InitializeDeviceObjects()
        ' Desc: Initialize scene objects.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub InitializeDeviceObjects()
            ' Load the file objects
            Try
                shinyTeapotMesh.Create(Device, "teapot.x")
                skyBoxMesh.Create(Device, "lobby_skybox.x")
                airplaneMesh.Create(Device, "airplane 2.x")
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try

            ' Set mesh properties
            airplaneMesh.SetFVF(Device, VertexFormats.Position Or VertexFormats.Normal Or VertexFormats.Texture1)
            shinyTeapotMesh.SetFVF(Device, EnvMappedVertex.Format)

            ' Restore the device-dependent objects
            drawingFont.InitializeDeviceObjects(Device)
            Dim sEffect As String = Nothing
            Dim i As Integer
            For i = 0 To effectString.Length - 1
                sEffect += effectString(i)
            Next i
            effect = effect.FromString(Device, sEffect, Nothing, 0, Nothing)
        End Sub 'InitializeDeviceObjects





        '-----------------------------------------------------------------------------
        ' Name: RestoreDeviceObjects()
        ' Desc: Restore device-memory objects and state after a device is created or
        '       resized.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As Object, ByVal e As System.EventArgs)
            ' InitializeDeviceObjects for file objects (build textures and vertex buffers)
            shinyTeapotMesh.RestoreDeviceObjects(Device, Nothing)
            skyBoxMesh.RestoreDeviceObjects(Device, Nothing)
            airplaneMesh.RestoreDeviceObjects(Device, Nothing)

            ' Create RenderToEnvironmentMap object
            renderToEnvironmentMap = New RenderToEnvironmentMap(Device, CubeMapResolution, 1, Device.PresentationParameters.BackBufferFormat, True, DepthFormat.D16)

            ' Create the cubemap, with a format that matches the backbuffer
            If Caps.TextureCaps.SupportsCubeMap Then
                Try
                    cubeMap = New CubeTexture(Device, CubeMapResolution, 1, Usage.RenderTarget, Device.PresentationParameters.BackBufferFormat, Pool.Default)
                Catch
                    Try
                        cubeMap = New CubeTexture(Device, CubeMapResolution, 1, 0, Device.PresentationParameters.BackBufferFormat, Pool.Default)
                    Catch
                        cubeMap = Nothing
                    End Try
                End Try
            End If

            ' Create the spheremap, with a format that matches the backbuffer
            If cubeMap Is Nothing Then
                Try
                    sphereMap = New Texture(Device, CubeMapResolution, CubeMapResolution, 1, Usage.RenderTarget, Device.PresentationParameters.BackBufferFormat, Pool.Default)
                Catch
                    sphereMap = New Texture(Device, CubeMapResolution, CubeMapResolution, 1, 0, Device.PresentationParameters.BackBufferFormat, Pool.Default)
                End Try
            End If

            ' Initialize effect
            effect.SetValue(EffectHandle.FromString("texCubeMap"), cubeMap)
            effect.SetValue(EffectHandle.FromString("texSphereMap"), sphereMap)

            If Not (cubeMap Is Nothing) Then
                effect.Technique = EffectHandle.FromString("Cube")
                Me.Text = "CubeMap: Environment cube-mapping"
            Else
                effect.Technique = EffectHandle.FromString("Sphere")
                Me.Text = "CubeMap: Environment cube-mapping (using dynamic spheremap)"
            End If


            ' Set the transform matrices
            Dim fAspect As Single = CSng(Device.PresentationParameters.BackBufferWidth) / CSng(Device.PresentationParameters.BackBufferHeight)
            projectionMatrix = Matrix.PerspectiveFovLH(CSng(Math.PI) * 0.4F, fAspect, 0.5F, 100.0F)
        End Sub 'RestoreDeviceObjects






        '-----------------------------------------------------------------------------
        ' Name: ConfirmDevice()
        ' Desc: Called during device intialization, this code checks the device
        '       for some minimum set of capabilities
        '-----------------------------------------------------------------------------
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            ' Make sure this device can support cube textures, software vertex processing, 
            ' and vertex shaders of at least v1.0
            If Not caps.TextureCaps.SupportsCubeMap And vertexProcessingType <> vertexProcessingType.Software And caps.VertexShaderVersion.Major < 1 Then
                Return False
            End If
            ' Check that we can create a cube texture that we can render into
            Return True
        End Function 'ConfirmDevice



        '-----------------------------------------------------------------------------
        ' Name: WndProc()
        ' Desc: Message proc function to handle mouse input
        '-----------------------------------------------------------------------------
        Protected Overrides Sub OnMouseDown(ByVal e As MouseEventArgs)
            If e.Button = MouseButtons.Left Then
                Dim qCursor As Quaternion = GraphicsUtility.GetRotationFromCursor(Me)
                Dim matCursor As Matrix = Matrix.RotationQuaternion(qCursor)
                matCursor.Transpose(matCursor)
                trackBallMatrix = Matrix.Multiply(trackBallMatrix, matCursor)

                Me.Capture = True
            End If
            MyBase.OnMouseDown(e)
        End Sub 'OnMouseDown

        Protected Overrides Sub OnMouseUp(ByVal e As MouseEventArgs)
            If e.Button = MouseButtons.Left Then
                Dim qCursor As Quaternion = GraphicsUtility.GetRotationFromCursor(Me)
                Dim matCursor As Matrix = Matrix.RotationQuaternion(qCursor)
                trackBallMatrix = Matrix.Multiply(trackBallMatrix, matCursor)

                Me.Capture = False
            End If
            MyBase.OnMouseUp(e)
        End Sub 'OnMouseUp



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
End Namespace 'CubeMapSample