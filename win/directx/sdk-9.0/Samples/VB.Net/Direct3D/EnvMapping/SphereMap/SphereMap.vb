'-----------------------------------------------------------------------------
' File: SphereMap.vb
'
' Desc: Example code showing how to use sphere-mapping in D3D, using generated 
'       texture coordinates.
'
' Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D



Namespace SpheremapSample
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
        '       are shown.. one which simply uses SphereMaps, and a fallback which uses 
        '       a vertex shader to do a sphere-map lookup.
        '-----------------------------------------------------------------------------
        Private effectString As String() = {"texture texSphereMap;" + ControlChars.Lf, "matrix matWorld;" + ControlChars.Lf, "matrix matViewProject;" + ControlChars.Lf, "vector vecPosition;" + ControlChars.Lf, "technique Sphere" + ControlChars.Lf, "{" + ControlChars.Lf, "pass P0" + ControlChars.Lf, "{" + ControlChars.Lf, "VertexShader =" + ControlChars.Lf, "decl" + ControlChars.Lf, "{" + ControlChars.Lf, "}" + ControlChars.Lf, "asm" + ControlChars.Lf, "{" + ControlChars.Lf, "vs.1.0" + ControlChars.Lf, "def c64, 0.25f, 0.5f, 1.0f, -1.0f" + ControlChars.Lf, "dcl_position v0" + ControlChars.Lf, "dcl_normal v1" + ControlChars.Lf, "m4x4 r0, v0, c0" + ControlChars.Lf, "m3x3 r1.xyz, v1, c0" + ControlChars.Lf, "mov r1.w, c64.z" + ControlChars.Lf, "add r2, c8, -r0" + ControlChars.Lf, "dp3 r3, r2, r2" + ControlChars.Lf, "rsq r3, r3.w" + ControlChars.Lf, "mul r2, r2, r3" + ControlChars.Lf, "dp3 r3, r1, r2" + ControlChars.Lf, "mul r1, r1, r3" + ControlChars.Lf, "add r1, r1, r1" + ControlChars.Lf, "add r3, r1, -r2" + ControlChars.Lf, "mad r4.w, -r3.z, c64.y, c64.y" + ControlChars.Lf, "rsq r4, r4.w" + ControlChars.Lf, "mul r4, r3, r4" + ControlChars.Lf, "mad r4, r4, c64.x, c64.y" + ControlChars.Lf, "m4x4 oPos, r0, c4" + ControlChars.Lf, "mul oT0.xy, r4.xy, c64.zw" + ControlChars.Lf, "mov oT0.zw, c64.z" + ControlChars.Lf, "};" + ControlChars.Lf, "VertexShaderConstant4[0] = <matWorld>;" + ControlChars.Lf, "VertexShaderConstant4[4] = <matViewProject>;" + ControlChars.Lf, "VertexShaderConstant1[8] = <vecPosition>;" + ControlChars.Lf, "Texture[0] = <texSphereMap>;" + ControlChars.Lf, "AddressU[0] = Wrap;" + ControlChars.Lf, "AddressV[0] = Wrap;" + ControlChars.Lf, "MinFilter[0] = Linear;" + ControlChars.Lf, "MagFilter[0] = Linear;" + ControlChars.Lf, "ColorOp[0] = SelectArg1;" + ControlChars.Lf, "ColorArg1[0] = Texture;" + ControlChars.Lf, "}" + ControlChars.Lf, "}" + ControlChars.Lf}
       _


        ' Vertex state
        ' Decls no longer associated with vertex shaders in DX9


        ' r0: camera-space position
        ' r1: camera-space normal
        ' r2: camera-space vertex-eye vector
        ' r3: camera-space reflection vector
        ' r4: texture coordinates
        ' Transform position and normal into camera-space

        ' Compute normalized view vector

        ' Compute camera-space reflection vector

        ' Compute sphere-map texture coords

        ' Project position


        ' Pixel state



        '-----------------------------------------------------------------------------
        ' Name: struct EnvMappedVertex
        ' Desc: D3D vertex type for environment-mapped objects
        '-----------------------------------------------------------------------------
        Public Structure EnvMappedVertex
            Public p As Vector3 ' Position
            Public n As Vector3 ' Normal
            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal
        End Structure 'EnvMappedVertex

        Private matProject As Matrix
        Private matView As Matrix
        Private matWorld As Matrix
        Private matTrackBall As Matrix

        Private drawingFont As GraphicsFont = Nothing
        Private shinyTeapot As GraphicsMesh = Nothing
        Private skyBox As GraphicsMesh = Nothing

        Private effect As Effect = Nothing
        Private sphereMapTexture As Texture = Nothing





        '-----------------------------------------------------------------------------
        ' Name: MyGraphicsSample()
        ' Desc: Application constructor. Sets attributes for the app.
        '-----------------------------------------------------------------------------
        Public Sub New()
            Me.Text = "SphereMap: Environment Mapping Technique"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon(this.GetType(), "directx.ico");
            enumerationSettings.AppUsesDepthBuffer = True

            drawingFont = New GraphicsFont("Arial", FontStyle.Bold)
            shinyTeapot = New GraphicsMesh()
            skyBox = New GraphicsMesh()

            matWorld = Matrix.Identity

            matTrackBall = Matrix.Identity
            matView = Matrix.Translation(0.0F, 0.0F, 3.0F)
        End Sub 'New





        '-----------------------------------------------------------------------------
        ' Name: FrameMove()
        ' Desc: Called once per frame, the call is the entry point for animating
        '       the scene.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub FrameMove()
            ' When the window has focus, let the mouse adjust the camera view
            If Me.Capture Then
                Dim matCursor As Matrix
                Dim quat As Quaternion = GraphicsUtility.GetRotationFromCursor(Me)
                matCursor = Matrix.RotationQuaternion(quat)
                matView = Matrix.Multiply(matTrackBall, matCursor)

                Dim matTrans As Matrix
                matTrans = Matrix.Translation(0.0F, 0.0F, 3.0F)
                matView = Matrix.Multiply(matView, matTrans)
            Else
                Dim matRotation As Matrix = Matrix.RotationY(-elapsedTime)
                matWorld.Multiply(matRotation)
            End If
        End Sub 'FrameMove





        '-----------------------------------------------------------------------------
        ' Name: Render()
        ' Desc: Called once per frame, the call is the entry point for 3d
        '       rendering. This function sets up render states, clears the
        '       viewport, and renders the scene.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub Render()
            ' Begin the scene
            Device.BeginScene()
            ' Render the Skybox
            If (True) Then
                Dim world As Matrix = Matrix.Scaling(10.0F, 10.0F, 10.0F)

                Dim view As Matrix = matView
                view.M41 = 0.0F : view.M42 = 0.0F : view.M43 = 0.0F

                Device.Transform.World = world
                Device.Transform.View = view
                Device.Transform.Projection = matProject

                Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
                Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
                Device.SamplerState(0).MinFilter = TextureFilter.Linear
                Device.SamplerState(0).MagFilter = TextureFilter.Linear
                Device.SamplerState(0).AddressU = TextureAddress.Wrap
                Device.SamplerState(0).AddressV = TextureAddress.Wrap

                ' Always pass Z-test, so we can avoid clearing color and depth buffers
                Device.RenderState.ZBufferFunction = Compare.Always
                skyBox.Render(Device)
                Device.RenderState.ZBufferFunction = Compare.LessEqual
            End If

            ' Render the environment-mapped ShinyTeapot
            If (True) Then
                ' Set transform state
                Dim viewProject As Matrix = Matrix.Multiply(matView, matProject)

                Dim matViewInv As Matrix = Matrix.Invert(matView)
                Dim vecPosition As New Vector4(matViewInv.M41, matViewInv.M42, matViewInv.M43, 1.0F)

                effect.SetValue(EffectHandle.FromString("matWorld"), matWorld)
                effect.SetValue(EffectHandle.FromString("matViewProject"), viewProject)
                effect.SetValue(EffectHandle.FromString("vecPosition"), vecPosition)


                ' Draw teapot
                Dim vb As VertexBuffer = shinyTeapot.LocalVertexBuffer
                Dim ib As IndexBuffer = shinyTeapot.LocalIndexBuffer

                Device.VertexFormat = shinyTeapot.LocalMesh.VertexFormat

                Device.SetStreamSource(0, vb, 0, DXHelp.GetTypeSize(GetType(EnvMappedVertex)))
                Device.Indices = ib

                Dim passes As Integer = effect.Begin(0)

                Dim iPass As Integer
                For iPass = 0 To passes - 1
                    effect.Pass(iPass)

                    Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, shinyTeapot.LocalMesh.NumberVertices, 0, shinyTeapot.LocalMesh.NumberFaces)
                Next iPass


                effect.End()
            End If



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
            ' Load the file objects
            Try
                shinyTeapot.Create(Device, "teapot.x")
                skyBox.Create(Device, "lobby_skybox.x")
                sphereMapTexture = GraphicsUtility.CreateTexture(Device, "spheremap.bmp")
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try

            ' Set mesh properties
            shinyTeapot.SetFVF(Device, EnvMappedVertex.Format)

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
            shinyTeapot.RestoreDeviceObjects(Device, Nothing)
            skyBox.RestoreDeviceObjects(Device, Nothing)

            ' Initialize effect
            effect.SetValue(EffectHandle.FromString("texSphereMap"), sphereMapTexture)

            ' Set the transform matrices
            Dim fAspect As Single = CSng(device.PresentationParameters.BackBufferWidth) / CSng(device.PresentationParameters.BackBufferHeight)
            matProject = Matrix.PerspectiveFovLH(CSng(Math.PI) * 0.4F, fAspect, 0.5F, 100.0F)
        End Sub 'RestoreDeviceObjects





        '-----------------------------------------------------------------------------
        ' Name: ConfirmDevice()
        ' Desc: Called during device intialization, this code checks the device
        '       for some minimum set of capabilities
        '-----------------------------------------------------------------------------
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            ' Make sure this device can support software vertex processing, 
            ' and vertex shaders of at least v1.0
            If vertexProcessingType = vertexProcessingType.Software Or caps.VertexShaderVersion.Major >= 1 Then
                Return True
            End If
            'Otherwise return false
            Return False
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
                matTrackBall = Matrix.Multiply(matTrackBall, matCursor)

                Me.Capture = True
            End If
            MyBase.OnMouseDown(e)
        End Sub 'OnMouseDown

        Protected Overrides Sub OnMouseUp(ByVal e As MouseEventArgs)
            If e.Button = MouseButtons.Left Then
                Dim qCursor As Quaternion = GraphicsUtility.GetRotationFromCursor(Me)
                Dim matCursor As Matrix = Matrix.RotationQuaternion(qCursor)
                matTrackBall = Matrix.Multiply(matTrackBall, matCursor)

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
End Namespace 'SphereMapSample