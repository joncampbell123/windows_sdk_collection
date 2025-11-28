'-----------------------------------------------------------------------------
' File: VolumeTexture.vb
'
' Desc: Example code showing how to do volume textures in D3D.
'
' Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace VolumeTextureSample
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
       _

        Public Structure VolumeVertex
            Public x, y, z As Single ' Position
            Public color As Integer
            Public tu, tv, tw As Single ' Tex coordinates


            Public Sub New(ByVal px As Single, ByVal py As Single, ByVal pz As Single, ByVal c As Integer, ByVal u As Single, ByVal v As Single, ByVal w As Single)
                x = px
                y = py
                z = pz
                color = c
                tu = u
                tv = v
                tw = w
            End Sub 'New
        End Structure 'VolumeVertex
        Public Shared VolumeFormat As VertexFormats = VertexFormats.Position Or VertexFormats.Diffuse Or VertexFormats.Texture1 Or VertexTextureCoordinate.Size3(0)

        Public Shared vertices() As VolumeVertex = {New VolumeVertex(1.0F, 1.0F, 0.0F, Color.White.ToArgb(), 1.0F, 1.0F, 0.0F), New VolumeVertex(-1.0F, 1.0F, 0.0F, Color.White.ToArgb(), 0.0F, 1.0F, 0.0F), New VolumeVertex(1.0F, -1.0F, 0.0F, Color.White.ToArgb(), 1.0F, 0.0F, 0.0F), New VolumeVertex(-1.0F, -1.0F, 0.0F, Color.White.ToArgb(), 0.0F, 0.0F, 0.0F)}


        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private vertex As VertexBuffer = Nothing ' VertexBuffer to render texture on
        Private volume As VolumeTexture = Nothing
        ' The Volume Texture

        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "VolumeTexture"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            ' Create our font objects
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
        End Sub 'New




        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            Dim fAngle As Single = appTime / 2.0F

            ' Play with the volume texture coordinate
            Dim stm As GraphicsStream = vertex.Lock(0, 0, 0)
            ' Seek to the correct spot and write the data
            Dim i As Integer
            For i = 0 To 3
                stm.Seek(24, System.IO.SeekOrigin.Current) ' Seek 24 bytes into the structure
                stm.Write(CSng(Math.Sin(fAngle) * Math.Cos(fAngle)))
            Next i
            vertex.Unlock()
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, Color.Black, 1.0F, 0)

            device.BeginScene()

            ' Draw the quad, with the volume texture
            device.SetTexture(0, volume)
            Device.VertexFormat = VolumeFormat
            device.SetStreamSource(0, vertex, 0)
            device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Output statistics
            drawingFont.DrawText(2, 1, Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, Color.Yellow, deviceStats)

            device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize all of the fonts
            drawingFont.InitializeDeviceObjects(device)
            ' Create a volume texture
            volume = New VolumeTexture(device, 16, 16, 16, 1, Format.A8R8G8B8, Pool.Managed)
            ' Fill the volume texture
            Dim data As Integer(,,) = CType(volume.LockBox(GetType(Integer), 0, 0, 16, 16, 16), Integer(,,))
            Dim w As Integer
            For w = 0 To 15
                Dim v As Integer
                For v = 0 To 15
                    Dim u As Integer
                    For u = 0 To 15
                        Dim du As Single = (u - 7.5F) / 7.5F
                        Dim dv As Single = (v - 7.5F) / 7.5F
                        Dim dw As Single = (w - 7.5F) / 7.5F
                        Dim fScale As Single = CSng(Math.Sqrt((du * du + dv * dv + dw * dw))) / CSng(Math.Sqrt(1.0F))

                        If fScale > 1.0F Then
                            fScale = 0.0F
                        Else
                            fScale = 1.0F - fScale
                        End If
                        Dim r As Integer = CInt(DXHelp.ShiftLeft(w, 4) * fScale)
                        Dim g As Integer = CInt(DXHelp.ShiftLeft(v, 4) * fScale)
                        Dim b As Integer = CInt(DXHelp.ShiftLeft(u, 4) * fScale)

                        data(w, v, u) = &HFF000000 + (DXHelp.ShiftLeft(r, 16)) + (DXHelp.ShiftLeft(g, 8)) + b
                    Next u
                Next v
            Next w
            volume.UnlockBox(0)

            ' Create a vertex buffer
            vertex = New VertexBuffer(GetType(VolumeVertex), 4, Device, Usage.WriteOnly, VolumeFormat, Pool.Managed)
            Dim vertStream As GraphicsStream = vertex.Lock(0, 0, 0)
            ' Copy our vertices in
            vertStream.Write(vertices)
            vertex.Unlock()
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Set the matrices
            Dim vEye As New Vector3(0.0F, 0.0F, -3.0F)
            Dim vAt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUp As New Vector3(0.0F, 1.0F, 0.0F)
            Dim fAspect As Single = CSng(Device.PresentationParameters.BackBufferWidth) / CSng(Device.PresentationParameters.BackBufferHeight)
            Device.Transform.World = Matrix.Identity
            Device.Transform.View = Matrix.LookAtLH(vEye, vAt, vUp)
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 100.0F)

            ' Set state
            Device.RenderState.DitherEnable = False
            Device.RenderState.Clipping = False
            Device.RenderState.CullMode = Cull.None
            Device.RenderState.Lighting = False
            Device.RenderState.ZBufferEnable = False
            Device.RenderState.ZBufferWriteEnable = False

            Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).AlphaOperation = TextureOperation.SelectArg1
            Device.TextureState(0).AlphaArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).AlphaArgument2 = TextureArgument.Diffuse
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear
        End Sub 'RestoreDeviceObjects





        '-----------------------------------------------------------------------------
        ' Name: ConfirmDevice()
        ' Desc: Called during device intialization, this code checks the device
        '       for some minimum set of capabilities
        '-----------------------------------------------------------------------------
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            ' Make sure we can do a volume map, that's all we care about
            Return caps.TextureCaps.SupportsVolumeMap
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
End Namespace 'VolumeTextureSample