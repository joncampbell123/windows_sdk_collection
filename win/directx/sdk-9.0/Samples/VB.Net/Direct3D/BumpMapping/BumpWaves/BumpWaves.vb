'-----------------------------------------------------------------------------
' File: BumpWaves.vb
'
' Desc: Code to simulate reflections off waves using bumpmapping.
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace BumpWavesSample
    _



    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample



        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        ' Scene
        Private waterBuffer As VertexBuffer = Nothing
        Private bumpMap As Texture = Nothing
        Private bumpMapMatrix As Matrix = Matrix.Zero
        Private backgroundVertex As VertexBuffer = Nothing
        Private background As Texture = Nothing


        Private numVertx As Integer ' Number of vertices in the ground grid along X
        Private numVertz As Integer ' Number of vertices in the ground grid along Z
        Private numTriangles As Integer ' Number of triangles in the ground grid
        ' In case we use shaders
        Private isUsingVertexShader As Boolean = False ' Vertex shader for the bump waves
        Private vertexShader As VertexShader = Nothing ' Vertex declaration for the bump waves
        Private vertexDecl As VertexDeclaration = Nothing
        ' Whether to use vertex shader or FF pipeline

        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "BumpWaves: Using BumpMapping For Waves"
            Try
                ' Load the icon from our resources
                Dim resources As New System.Resources.ResourceManager(Me.GetType())
                Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
            Catch
            End Try
            ' It's no big deal if we can't load our icons

            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = False
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Setup the bump matrix
            ' Min r is 0.04 if amplitude is 32 to miss temporal aliasing
            ' Max r is 0.16 for amplitude is 8 to miss spatial aliasing
            Dim r As Single = 0.04F
            bumpMapMatrix.M11 = r * CSng(Math.Cos((appTime * 9.0F)))
            bumpMapMatrix.M12 = -r * CSng(Math.Sin((appTime * 9.0F)))
            bumpMapMatrix.M21 = r * CSng(Math.Sin((appTime * 9.0F)))
            bumpMapMatrix.M22 = r * CSng(Math.Cos((appTime * 9.0F)))
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target, System.Drawing.Color.Black, 0.0F, 0)

            Device.BeginScene()

            ' Set up texture stage states for the background
            Device.SetTexture(0, background)
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
            Device.TextureState(1).ColorOperation = TextureOperation.Disable

            ' Render the background
            Device.VertexFormat = CustomVertex.PositionTextured.Format
            Device.SetStreamSource(0, backgroundVertex, 0)
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Render the water
            SetEMBMStates()
            If isUsingVertexShader Then
                Device.VertexShader = VertexShader
                Device.VertexDeclaration = vertexDecl
            Else
                Device.VertexFormat = CustomVertex.PositionTextured.Format
            End If

            Device.SetStreamSource(0, waterBuffer, 0)
            Device.DrawPrimitives(PrimitiveType.TriangleList, 0, numTriangles)

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            If isUsingVertexShader Then
                drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using Vertex Shader")
            Else
                drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using Fixed-Function Vertex Pipeline")
            End If
            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Set up the bumpmap states
        '/ </summary>
        Sub SetEMBMStates()
            ' Set up texture stage 0's states for the bumpmap
            Device.SetTexture(0, bumpMap)
            Device.SamplerState(0).AddressU = TextureAddress.Clamp
            Device.SamplerState(0).AddressV = TextureAddress.Clamp
            Device.TextureState(0).BumpEnvironmentMaterial00 = bumpMapMatrix.M11
            Device.TextureState(0).BumpEnvironmentMaterial01 = bumpMapMatrix.M12
            Device.TextureState(0).BumpEnvironmentMaterial10 = bumpMapMatrix.M21
            Device.TextureState(0).BumpEnvironmentMaterial11 = bumpMapMatrix.M22
            Device.TextureState(0).ColorOperation = TextureOperation.BumpEnvironmentMap
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Current

            ' Set up texture stage 1's states for the environment map
            Device.SetTexture(1, background)
            Device.TextureState(1).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(1).ColorOperation = TextureOperation.SelectArg1

            If isUsingVertexShader Then
                Device.TextureState(1).TextureTransform = TextureTransform.Disable
                Device.TextureState(1).TextureCoordinateIndex = 1
            Else
                ' Set up projected texture coordinates
                ' tu = (0.8x + 0.5z) / z
                ' tv = (0.8y - 0.5z) / z
                Dim mat As Matrix = Matrix.Zero
                mat.M11 = 0.8F
                mat.M12 = 0.0F
                mat.M13 = 0.0F
                mat.M21 = 0.0F
                mat.M22 = 0.8F
                mat.M23 = 0.0F
                mat.M31 = 0.5F
                mat.M32 = -0.5F
                mat.M33 = 1.0F
                mat.M41 = 0.0F
                mat.M42 = 0.0F
                mat.M43 = 0.0F

                Device.Transform.Texture1 = mat
                Device.TextureState(1).TextureTransform = TextureTransform.Count3 Or TextureTransform.Projected
                Device.TextureState(1).TextureCoordinateIndex = CInt(TextureCoordinateIndex.CameraSpacePosition) Or 1
            End If
        End Sub 'SetEMBMStates




        '-----------------------------------------------------------------------------
        ' Name: CreateBumpMap()
        ' Desc: Creates a bumpmap from a surface
        '-----------------------------------------------------------------------------
        Public Function CreateBumpMap(ByVal width As Integer, ByVal height As Integer, ByVal bumpformat As Format) As Texture
            Dim bumpMapTexture As Texture = Nothing

            ' Check if the device can create the format
            If Manager.CheckDeviceFormat(Caps.AdapterOrdinal, Caps.DeviceType, graphicsSettings.DisplayMode.Format, 0, ResourceType.Textures, bumpformat) = False Then
                Return Nothing
            End If
            ' Create the bumpmap texture
            bumpMapTexture = New Texture(Device, width, height, 1, 0, bumpformat, Pool.Managed)

            Dim pitch As Integer
            ' Lock the surface and write in some bumps for the waves
            Dim dst As GraphicsStream = bumpMapTexture.LockRectangle(0, 0, pitch)

            Dim bumpUCoord, bumpVCoord As Byte
            Dim y As Integer
            For y = 0 To height - 1
                Dim x As Integer
                For x = 0 To width - 1
                    Dim fx As Single = x / CSng(width) - 0.5F
                    Dim fy As Single = y / CSng(height) - 0.5F

                    Dim r As Single = CSng(Math.Sqrt((fx * fx + fy * fy)))

                    bumpUCoord = CByte(64 * CSng(Math.Cos((300.0F * r))) * CSng(Math.Exp((-r * 5.0F))))
                    bumpUCoord += CByte(32 * CSng(Math.Cos((150.0F * (fx + fy)))))
                    bumpUCoord += CByte(16 * CSng(Math.Cos((140.0F * (fx * 0.85F - fy)))))

                    bumpVCoord = CByte(64 * CSng(Math.Sin((300.0F * r))) * CSng(Math.Exp((-r * 5.0F))))
                    bumpVCoord += CByte(32 * CSng(Math.Sin((150.0F * (fx + fy)))))
                    bumpVCoord += CByte(16 * CSng(Math.Sin((140.0F * (fx * 0.85F - fy)))))

                    dst.Write(bumpUCoord)
                    dst.Write(bumpVCoord)
                Next x
            Next y
            bumpMapTexture.UnlockRectangle(0)
            Return bumpMapTexture
        End Function 'CreateBumpMap





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
            Try
                ' Load the texture for the background image
                background = GraphicsUtility.CreateTexture(Device, "lake.bmp", Format.R5G6B5)
                ' Create the bumpmap. 
                bumpMap = CreateBumpMap(256, 256, Format.V8U8)
                If bumpMap Is Nothing Then
                    Throw New InvalidOperationException()
                End If
                ' Create a square for rendering the background
                If backgroundVertex Is Nothing Then
                    ' We only need to create this buffer once
                    backgroundVertex = New VertexBuffer(GetType(CustomVertex.PositionTextured), 4, Device, Usage.WriteOnly, CustomVertex.PositionTextured.Format, Pool.Default)
                    AddHandler backgroundVertex.Created, AddressOf Me.BackgroundVertexCreated
                    ' Call it manually the first time
                    Me.BackgroundVertexCreated(backgroundVertex, Nothing)
                ElseIf backgroundVertex.Disposed Then
                    ' We only need to create this buffer once
                    backgroundVertex = New VertexBuffer(GetType(CustomVertex.PositionTextured), 4, Device, Usage.WriteOnly, CustomVertex.PositionTextured.Format, Pool.Default)
                    AddHandler backgroundVertex.Created, AddressOf Me.BackgroundVertexCreated
                    ' Call it manually the first time
                    Me.BackgroundVertexCreated(backgroundVertex, Nothing)
                End If
                ' See if EMBM and projected vertices are supported at the same time
                ' in the fixed-function shader.  If not, switch to using a vertex shader.
                isUsingVertexShader = False
                SetEMBMStates()
                Device.VertexShader = Nothing
                Device.VertexFormat = CustomVertex.PositionTextured.Format

                Dim validParams As ValidateDeviceParams = Device.ValidateDevice()
                If validParams.Result <> 0 Then
                    isUsingVertexShader = True
                End If
                ' If TextureCaps.Projected is set, projected textures are computed
                ' per pixel, so this sample will work fine with just a quad for the water
                ' model.  If it's not set, textures are projected per vertex rather than 
                ' per pixel, so distortion will be visible unless we use more vertices.
                If Caps.TextureCaps.SupportsProjected And Not isUsingVertexShader Then
                    numVertx = 2 ' Number of vertices in the ground grid along X
                    numVertz = 2 ' Number of vertices in the ground grid along Z
                Else
                    numVertx = 8 ' Number of vertices in the ground grid along X
                    numVertz = 8 ' Number of vertices in the ground grid along Z
                End If
                numTriangles = (numVertx - 1) * (numVertz - 1) * 2 ' Number of triangles in the ground

                ' Create a square for rendering the water
                If waterBuffer Is Nothing Then
                    ' We only need to create this buffer once
                    waterBuffer = New VertexBuffer(GetType(CustomVertex.PositionTextured), 3 * numTriangles, Device, Usage.WriteOnly, CustomVertex.PositionTextured.Format, Pool.Default)
                    AddHandler waterBuffer.Created, AddressOf Me.WaterBufferCreated
                    ' Call it manually the first time
                    Me.WaterBufferCreated(waterBuffer, Nothing)
                ElseIf waterBuffer.Disposed Then
                    ' We only need to create this buffer once
                    waterBuffer = New VertexBuffer(GetType(CustomVertex.PositionTextured), 3 * numTriangles, Device, Usage.WriteOnly, CustomVertex.PositionTextured.Format, Pool.Default)
                    AddHandler waterBuffer.Created, AddressOf Me.WaterBufferCreated
                    ' Call it manually the first time
                    Me.WaterBufferCreated(waterBuffer, Nothing)
                End If
            Catch
                Dim e As MediaNotFoundException = New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ Set the data for the background's vertex buffer
        '/ </summary>
        Private Sub BackgroundVertexCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim v(3) As CustomVertex.PositionTextured
            v(0).SetPosition(New Vector3(-1000.0F, 0.0F, 0.0F))
            v(1).SetPosition(New Vector3(-1000.0F, 1000.0F, 0.0F))
            v(2).SetPosition(New Vector3(1000.0F, 0.0F, 0.0F))
            v(3).SetPosition(New Vector3(1000.0F, 1000.0F, 0.0F))
            v(0).Tu = 0.0F
            v(0).Tv = 147 / 256.0F
            v(1).Tu = 0.0F
            v(1).Tv = 0.0F
            v(2).Tu = 1.0F
            v(2).Tv = 147 / 256.0F
            v(3).Tu = 1.0F
            v(3).Tv = 0.0F

            vb.SetData(v, 0, 0)
        End Sub 'BackgroundVertexCreated





        '/ <summary>
        '/ Set the data for the water's vertex buffer
        '/ </summary>
        Private Sub WaterBufferCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim v As CustomVertex.PositionTextured() = CType(vb.Lock(0, 0), CustomVertex.PositionTextured())

            Dim dX As Single = 2000.0F / (numVertx - 1)
            Dim dZ As Single = 1250.0F / (numVertz - 1)
            Dim x0 As Single = -1000
            Dim z0 As Single = -1250
            Dim dU As Single = 1.0F / (numVertx - 1)
            Dim dV As Single = 0.7F / (numVertz - 1)
            Dim k As Integer = 0
            Dim z As Integer
            For z = 0 To (numVertz - 1) - 1
                Dim x As Integer
                For x = 0 To (numVertx - 1) - 1
                    v(k).SetPosition(New Vector3(x0 + x * dX, 0.0F, z0 + z * dZ))
                    v(k).Tu = x * dU
                    v(k).Tv = z * dV
                    k += 1
                    v(k).SetPosition(New Vector3(x0 + x * dX, 0.0F, z0 + (z + 1) * dZ))
                    v(k).Tu = x * dU
                    v(k).Tv = (z + 1) * dV
                    k += 1
                    v(k).SetPosition(New Vector3(x0 + (x + 1) * dX, 0.0F, z0 + (z + 1) * dZ))
                    v(k).Tu = (x + 1) * dU
                    v(k).Tv = (z + 1) * dV
                    k += 1
                    v(k).SetPosition(New Vector3(x0 + x * dX, 0.0F, z0 + z * dZ))
                    v(k).Tu = x * dU
                    v(k).Tv = z * dV
                    k += 1
                    v(k).SetPosition(New Vector3(x0 + (x + 1) * dX, 0.0F, z0 + (z + 1) * dZ))
                    v(k).Tu = (x + 1) * dU
                    v(k).Tv = (z + 1) * dV
                    k += 1
                    v(k).SetPosition(New Vector3(x0 + (x + 1) * dX, 0.0F, z0 + z * dZ))
                    v(k).Tu = (x + 1) * dU
                    v(k).Tv = z * dV
                    k += 1
                Next x
            Next z

            vb.Unlock()
        End Sub 'WaterBufferCreated





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 400.0F, -1650.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)

            Dim matWorld As Matrix = Matrix.Identity
            Dim matView As Matrix = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            Dim matProj As Matrix = Matrix.PerspectiveFovLH(1.0F, 1.0F, 1.0F, 10000.0F)
            device.Transform.World = matWorld
            device.Transform.View = matView
            device.Transform.Projection = matProj

            ' Set any appropiate state
            device.RenderState.Ambient = System.Drawing.Color.White
            device.RenderState.DitherEnable = True
            device.RenderState.SpecularEnable = False
            device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            device.TextureState(0).ColorOperation = TextureOperation.Modulate
            device.SamplerState(0).MinFilter = TextureFilter.Linear
            device.SamplerState(0).MagFilter = TextureFilter.Linear
            device.SamplerState(1).MinFilter = TextureFilter.Linear
            device.SamplerState(1).MagFilter = TextureFilter.Linear

            If isUsingVertexShader Then
                Dim matCamera, matFinal As Matrix

                ' Create our declaration
                Dim decl() As VertexElement = {New VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0), New VertexElement(0, 12, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0), VertexElement.VertexDeclarationEnd}

                vertexDecl = New VertexDeclaration(device, decl)

                vertexShader = GraphicsUtility.CreateVertexShader(device, "bumpwaves.vsh")

                matCamera = Matrix.Multiply(matWorld, matView)
                matFinal = Matrix.Multiply(matCamera, matProj)
                matCamera.Transpose(matCamera)
                matFinal.Transpose(matFinal)
                Dim camera()() As Single = {New Single() {matCamera.M11, matCamera.M12, matCamera.M13, matCamera.M14}, New Single() {matCamera.M21, matCamera.M22, matCamera.M23, matCamera.M24}, New Single() {matCamera.M31, matCamera.M32, matCamera.M33, matCamera.M34}}

                camera(0)(0) *= 0.8F
                camera(0)(1) *= 0.8F
                camera(0)(2) *= 0.8F
                camera(0)(3) *= 0.8F
                camera(1)(0) *= 0.8F
                camera(1)(1) *= 0.8F
                camera(1)(2) *= 0.8F
                camera(1)(3) *= 0.8F
                Dim i As Integer
                For i = 0 To 2
                    device.SetVertexShaderConstant(i, camera(i))
                Next i
                device.SetVertexShaderConstant(3, New Matrix() {matFinal})

                Dim data() As Single = {0.5F, -0.5F, 0, 0}
                device.SetVertexShaderConstant(8, data)
            End If
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            ' Device must be able to do bumpmapping
            If Not caps.TextureOperationCaps.SupportsBumpEnvironmentMap Then
                Return False
            End If
            ' Accept devices that can create Format.V8U8 textures
            Return Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, 0, ResourceType.Textures, Format.V8U8)
        End Function 'ConfirmDevice




        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Dim d3dApp As New MyGraphicsSample()
            Try
                If d3dApp.CreateGraphicsSample() Then
                    d3dApp.Run()
                End If
            Finally
                d3dApp.Dispose()
            End Try
        End Sub 'Main 
    End Class 'MyGraphicsSample
End Namespace 'BumpWavesSample