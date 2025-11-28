'-----------------------------------------------------------------------------
' File: BumpUnderwater.vb
'
' Desc: Code to simulate underwater distortion.
'       Games could easily make use of this technique to achieve an underwater
'       effect without affecting geometry by rendering the scene to a texture
'       and applying the bump effect on a rectangle mapped with it.
'
' Note: From the Matrox web site demos
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace BumpUnderwaterSample
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
            Public n As Vector3
            Public tu1, tv1 As Single
            Public tu2, tv2 As Single

            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal Or VertexFormats.Texture2
        End Structure 'BumpVertex



        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        ' Scene
        Private waterBuffer As VertexBuffer = Nothing
        Private bumpTex As Texture = Nothing
        Private background As Texture = Nothing
        Private deviceValidationFailed As Boolean = False




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "BumpUnderWater: Effect Using BumpMapping"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            Device.TextureState(0).BumpEnvironmentMaterial00 = 0.01F
            Device.TextureState(0).BumpEnvironmentMaterial01 = 0.0F
            Device.TextureState(0).BumpEnvironmentMaterial10 = 0.0F
            Device.TextureState(0).BumpEnvironmentMaterial11 = 0.01F

            ' We will lock the data into a stream because we
            ' have set the data to be 'WriteOnly'.  This way
            ' we won't overwrite any data by seeking to the places 
            ' we want to write to.
            ' In this case we want to write the first set of UV 
            ' coords (the bump map coords) to make the 'underwater'
            ' effect
            Dim stm As GraphicsStream = waterBuffer.Lock(0, 0, 0)
            ' Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(24, System.IO.SeekOrigin.Current)
            Dim u1 As Single = 0.0F
            Dim v1 As Single = 0.5F * appTime + 2.0F
            stm.Write(u1)
            stm.Write(v1)
            ' Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(32, System.IO.SeekOrigin.Current)
            u1 = 0.0F
            v1 = 0.5F * appTime
            stm.Write(u1)
            stm.Write(v1)

            ' Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(32, System.IO.SeekOrigin.Current)
            u1 = 1.0F
            v1 = 0.5F * appTime
            stm.Write(u1)
            stm.Write(v1)

            ' Skip the position and normal, write our data, and skip to the next vertex
            stm.Seek(32, System.IO.SeekOrigin.Current)
            u1 = 1.0F
            v1 = 0.5F * appTime + 2.0F
            stm.Write(u1)
            stm.Write(v1)

            waterBuffer.Unlock()
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Black.ToArgb(), 1.0F, 0)

            Device.BeginScene()


            ' Render the waves
            Device.SetTexture(0, bumpTex)
            Device.SetTexture(1, background)

            Device.TextureState(0).TextureCoordinateIndex = 0
            Device.TextureState(0).ColorOperation = TextureOperation.BumpEnvironmentMap
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Current

            Device.TextureState(1).TextureCoordinateIndex = 1
            Device.TextureState(1).ColorOperation = TextureOperation.SelectArg1
            Device.TextureState(1).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(1).ColorArgument2 = TextureArgument.Current

            Device.TextureState(2).ColorOperation = TextureOperation.Disable

            Device.VertexFormat = BumpVertex.Format
            Device.SetStreamSource(0, waterBuffer, 0)

            ' Verify that the texture operations are possible on the device
            Dim validParams As ValidateDeviceParams = Device.ValidateDevice()
            If (validParams.Result <> 0) Then
                deviceValidationFailed = True
            End If

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
        Public Sub CreateBumpMap()
            Dim width As Integer = 256
            Dim height As Integer = 256

            ' Create the bumpmap's surface and texture objects
            bumpTex = New Texture(Device, width, height, 1, 0, Format.V8U8, Pool.Managed)

            Dim pitch As Integer
            ' Fill the bumpmap texels to simulate a lens
            Dim dst As Byte() = CType(bumpTex.LockRectangle(GetType(Byte), 0, 0, pitch, width * height * 2), Byte())

            Dim y As Integer
            For y = 0 To height - 1
                Dim x As Integer
                For x = 0 To width - 1
                    Dim fx As Single = x / CSng(width) - 0.5F
                    Dim fy As Single = y / CSng(height) - 0.5F

                    Dim du As Byte = CByte(64 * Math.Cos((4.0F * (fx + fy) * Math.PI)))
                    Dim dv As Byte = CByte(64 * Math.Sin((4.0F * (fx + fy) * Math.PI)))

                    dst((2 * x + 0 + y * pitch)) = du
                    dst((2 * x + 1 + y * pitch)) = dv
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
                background = GraphicsUtility.CreateTexture(Device, "LobbyXPos.jpg", Format.A8R8G8B8)
                ' create a bumpmap from info in source surface
                CreateBumpMap()
                If (waterBuffer Is Nothing) Then
                    CreateWaterBuffer()
                ElseIf (waterBuffer.Disposed) Then
                    CreateWaterBuffer()
                End If
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try
        End Sub 'InitializeDeviceObjects


        Private Sub CreateWaterBuffer()
            ' We only need to create this buffer once
            waterBuffer = New VertexBuffer(GetType(BumpVertex), 4, Device, Usage.WriteOnly, BumpVertex.Format, Pool.Managed)
            AddHandler waterBuffer.Created, AddressOf Me.WaterBufferCreated
            ' Call it manually the first time
            Me.WaterBufferCreated(waterBuffer, Nothing)
        End Sub




        '/ <summary>
        '/ Set the data for the water's vertex buffer
        '/ </summary>
        Private Sub WaterBufferCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim v(3) As BumpVertex
            v(0).p = New Vector3(-60.0F, -60.0F, 0.0F)
            v(0).n = New Vector3(0, 1, 0)
            v(1).p = New Vector3(-60.0F, 60.0F, 0.0F)
            v(1).n = New Vector3(0, 1, 0)
            v(2).p = New Vector3(60.0F, -60.0F, 0.0F)
            v(2).n = New Vector3(0, 1, 0)
            v(3).p = New Vector3(60.0F, 60.0F, 0.0F)
            v(3).n = New Vector3(0, 1, 0)
            v(0).tu2 = 0.0F
            v(0).tv2 = 1.0F
            v(1).tu2 = 0.0F
            v(1).tv2 = 0.0F
            v(2).tu2 = 1.0F
            v(2).tv2 = 1.0F
            v(3).tu2 = 1.0F
            v(3).tv2 = 0.0F
            vb.SetData(v, 0, 0)
        End Sub 'WaterBufferCreated





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 0.0F, -100.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim matWorld, matView, matProj As Matrix

            matWorld = Matrix.Identity
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            matProj = Matrix.PerspectiveFovLH(1.0F, 1.0F, 1.0F, 3000.0F)
            Device.Transform.World = matWorld
            Device.Transform.View = matView
            Device.Transform.Projection = matProj

            ' Set any appropiate state
            Device.RenderState.Ambient = System.Drawing.Color.White
            Device.RenderState.DitherEnable = True
            Device.RenderState.SpecularEnable = False
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear
            Device.SamplerState(1).MinFilter = TextureFilter.Linear
            Device.SamplerState(1).MagFilter = TextureFilter.Linear
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
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
End Namespace 'BumpUnderwaterSample