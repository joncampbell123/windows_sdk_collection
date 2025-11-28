'-----------------------------------------------------------------------------
' File: BumpLens.vb
'
' Desc: Code to simulate a magnifying glass using bumpmapping.
'
' Note: Based on a sample from the Matrox web site
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace BumpLensSample
    _



    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
       _

        '/ <summary>
        '/ Custom Bump vertex
        '/ </summary>
        Public Structure BumpVertex
            Public p As Vector3
            Public tu1, tv1 As Single
            Public tu2, tv2 As Single

            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Texture2
        End Structure 'BumpVertex


        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        ' Scene
        Private background As VertexBuffer = Nothing
        Private lens As VertexBuffer = Nothing
        Private bumpTex As Texture = Nothing
        Private backTex As Texture = Nothing
        Private lensX As Single = 0.0F
        Private lensY As Single = 0.0F
        Private deviceValidationFailed As Boolean = False




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "BumpLens: Lens Effect Using BumpMapping"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = False
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Get a triangle wave between -1 and 1
            lensX = CSng(2 * Math.Abs((2 * (appTime / 2 - Math.Floor((appTime / 2))) - 1)) - 1)

            ' Get a regulated sine wave between -1 and 1
            lensY = CSng(2 * Math.Abs(Math.Sin(appTime)) - 1)
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            device.BeginScene()

            ' Render the background
            device.SetTexture(0, backTex)
            device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
            device.TextureState(1).ColorOperation = TextureOperation.Disable

            Device.VertexFormat = CustomVertex.TransformedColoredTextured.Format
            Device.SetStreamSource(0, background, 0)
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Render the lens
            Device.SetTexture(0, bumpTex)
            Device.SetTexture(1, backTex)

            Device.TextureState(0).ColorOperation = TextureOperation.BumpEnvironmentMap
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Current

            Device.TextureState(0).BumpEnvironmentMaterial00 = 0.2F
            Device.TextureState(0).BumpEnvironmentMaterial01 = 0.0F
            Device.TextureState(0).BumpEnvironmentMaterial10 = 0.0F
            Device.TextureState(0).BumpEnvironmentMaterial11 = 0.2F
            Device.TextureState(0).BumpEnvironmentLuminanceScale = 1.0F
            Device.TextureState(0).BumpEnvironmentLuminanceOffset = 0.0F

            Device.TextureState(1).ColorOperation = TextureOperation.SelectArg1
            Device.TextureState(1).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(1).ColorArgument2 = TextureArgument.Current

            ' Generate texture coords depending on objects camera space position
            Dim mat As New Matrix()
            mat.M11 = 0.5F
            mat.M12 = 0.0F
            mat.M21 = 0.0F
            mat.M22 = -0.5F
            mat.M31 = 0.0F
            mat.M32 = 0.0F
            mat.M41 = 0.5F
            mat.M42 = 0.5F

            ' Scale-by-z here
            Dim matView, matProj As Matrix
            matView = Device.Transform.View
            matProj = Device.Transform.Projection
            Dim vEyePt As New Vector3(matView.M41, matView.M42, matView.M43)
            Dim z As Single = vEyePt.Length()
            mat.M11 *= matProj.M11 / (matProj.M33 * z + matProj.M34)
            mat.M22 *= matProj.M22 / (matProj.M33 * z + matProj.M34)

            Device.Transform.Texture1 = mat
            Device.TextureState(1).TextureTransform = TextureTransform.Count2
            Device.TextureState(1).TextureCoordinateIndex = CInt(TextureCoordinateIndex.CameraSpacePosition) Or 1

            ' Position the lens
            Dim matWorld As Matrix = Matrix.Translation(0.7F * (1000.0F - 256.0F) * lensX, 0.7F * (1000.0F - 256.0F) * lensY, 0.0F)
            Device.Transform.World = matWorld

            Device.VertexFormat = BumpVertex.Format
            Device.SetStreamSource(0, lens, 0)

            ' Verify that the texture operations are possible on the device
            Dim validParams As ValidateDeviceParams = Device.ValidateDevice()
            If (validParams.Result <> 0) Then
                deviceValidationFailed = True
            End If

            ' Render the lens
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)


            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            If deviceValidationFailed Then
                drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, "Warning: Device validation failed.  Rendering may not look right.")
            End If

            Device.EndScene()
        End Sub 'Render





        '-----------------------------------------------------------------------------
        ' Name: CreateBumpMap()
        ' Desc: Creates a bumpmap from a surface
        '-----------------------------------------------------------------------------
        Public Sub CreateBumpMap(ByVal width As Integer, ByVal height As Integer)

            ' Create the bumpmap's surface and texture objects
            bumpTex = New Texture(Device, width, height, 1, 0, Format.V8U8, Pool.Managed)

            Dim pitch As Integer
            ' Fill the bumpmap texels to simulate a lens
            Dim dst As Byte() = CType(bumpTex.LockRectangle(GetType(Byte), 0, 0, pitch, width * height * 2), Byte())
            Dim mid As Integer = width / 2
            Dim count As Integer = 0

            Dim y As Integer
            For y = 0 To height - 1
                count = 0
                Dim x As Integer
                For x = 0 To width - 1
                    Dim x1 As Integer = IIf(x = width - 1, x, x + 1)
                    Dim y1 As Integer = IIf(x = height - 1, y, y + 1)

                    Dim fDistSq00 As Single = CSng((x - mid) * (x - mid) + (y - mid) * (y - mid))
                    Dim fDistSq01 As Single = CSng((x1 - mid) * (x1 - mid) + (y - mid) * (y - mid))
                    Dim fDistSq10 As Single = CSng((x - mid) * (x - mid) + (y1 - mid) * (y1 - mid))

                    Dim v00 As Single = CSng(IIf(fDistSq00 > mid * mid, 0.0F, Math.Sqrt((mid * mid - fDistSq00))))
                    Dim v01 As Single = CSng(IIf(fDistSq01 > mid * mid, 0.0F, Math.Sqrt((mid * mid - fDistSq01))))
                    Dim v10 As Single = CSng(IIf(fDistSq10 > mid * mid, 0.0F, Math.Sqrt((mid * mid - fDistSq10))))

                    Dim du As Single = CSng(128 / Math.PI * Math.Atan((v00 - v01))) ' The delta-u bump value
                    Dim dv As Single = CSng(128 / Math.PI * Math.Atan((v00 - v10))) ' The delta-v bump value
                    dst(count + (y * pitch)) = CByte(du) : count += 1
                    dst(count + (y * pitch)) = CByte(dv) : count += 1
                Next x
            Next y
            bumpTex.UnlockRectangle(0)
        End Sub 'CreateBumpMap





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
            Try
                ' Load the texture for the background image
                backTex = GraphicsUtility.CreateTexture(Device, "lake.bmp", Format.Unknown)
                ' Create the bump map texture
                CreateBumpMap(256, 256)

                ' Create a square for rendering the lens
                If lens Is Nothing Then
                    CreateLensVertexBuffer()
                ElseIf lens.Disposed Then
                    CreateLensVertexBuffer()
                End If

                ' Create a square for rendering the background
                If background Is Nothing Then
                    CreateBackgroundVertexBuffer()
                ElseIf background.Disposed Then
                    CreateBackgroundVertexBuffer()
                End If
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try
        End Sub 'InitializeDeviceObjects


        ' Subs to create our vertex buffers
        Private Sub CreateLensVertexBuffer()
            ' We only need to create this buffer once
            lens = New VertexBuffer(GetType(BumpVertex), 4, Device, Usage.WriteOnly, BumpVertex.Format, Pool.Default)
            AddHandler lens.Created, AddressOf Me.LensCreated
            ' Call it manually the first time
            Me.LensCreated(lens, Nothing)
        End Sub
        Private Sub CreateBackgroundVertexBuffer()
            ' We only need to create this buffer once
            background = New VertexBuffer(GetType(CustomVertex.TransformedColoredTextured), 4, Device, Usage.WriteOnly, CustomVertex.TransformedColoredTextured.Format, Pool.Default)
            AddHandler background.Created, AddressOf Me.BackGroundCreated
            ' Call it manually the first time
            Me.BackGroundCreated(background, Nothing)
        End Sub





        '/ <summary>
        '/ Set the data for the water's vertex buffer
        '/ </summary>
        Private Sub BackGroundCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim v(3) As CustomVertex.TransformedColoredTextured
            Dim i As Integer
            For i = 0 To 3
                v(i).SetPosition(New Vector4(0.0F, 0.0F, 0.9F, 1.0F))
                v(i).Color = System.Drawing.Color.White.ToArgb()
            Next i
            v(0).Y = CSng(Device.PresentationParameters.BackBufferHeight)
            v(2).Y = CSng(Device.PresentationParameters.BackBufferHeight)
            v(2).X = CSng(Device.PresentationParameters.BackBufferWidth)
            v(3).X = CSng(Device.PresentationParameters.BackBufferWidth)
            v(0).Tu = 0.0F
            v(0).Tv = 1.0F
            v(1).Tu = 0.0F
            v(1).Tv = 0.0F
            v(2).Tu = 1.0F
            v(2).Tv = 1.0F
            v(3).Tu = 1.0F
            v(3).Tv = 0.0F
            vb.SetData(v, 0, 0)
        End Sub 'BackGroundCreated





        '/ <summary>
        '/ Set the data for the water's vertex buffer
        '/ </summary>
        Private Sub LensCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim v(3) As BumpVertex
            v(0).p = New Vector3(-256.0F, -256.0F, 0.0F)
            v(1).p = New Vector3(-256.0F, 256.0F, 0.0F)
            v(2).p = New Vector3(256.0F, -256.0F, 0.0F)
            v(3).p = New Vector3(256.0F, 256.0F, 0.0F)
            v(0).tu1 = 0.0F
            v(0).tv1 = 1.0F
            v(1).tu1 = 0.0F
            v(1).tv1 = 0.0F
            v(2).tu1 = 1.0F
            v(2).tv1 = 1.0F
            v(3).tu1 = 1.0F
            v(3).tv1 = 0.0F
            vb.SetData(v, 0, 0)
        End Sub 'LensCreated





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 0.0F, -2001.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim matWorld, matView, matProj As Matrix

            matWorld = Matrix.Identity
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            Dim fAspect As Single = Device.PresentationParameters.BackBufferWidth / CSng(Device.PresentationParameters.BackBufferHeight)
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 3000.0F)
            Device.Transform.World = matWorld
            Device.Transform.View = matView
            Device.Transform.Projection = matProj

            ' Set any appropiate state
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear
            Device.SamplerState(1).AddressU = TextureAddress.Clamp
            Device.SamplerState(1).AddressV = TextureAddress.Clamp
            Device.SamplerState(1).MinFilter = TextureFilter.Linear
            Device.SamplerState(1).MagFilter = TextureFilter.Linear
            Device.SamplerState(1).MipFilter = TextureFilter.None
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If vertexProcessingType = vertexProcessingType.PureHardware Then
                Return False ' GetTransform doesn't work on PUREDEVICE
            End If
            If Not caps.TextureOperationCaps.SupportsBumpEnvironmentMap Then
                Return False
            End If
            Return Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, 0, ResourceType.Textures, Format.V8U8)
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
End Namespace 'BumpLensSample