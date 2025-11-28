'-----------------------------------------------------------------------------
' File: ClipMirror.vb
'
' Desc: This sample shows how to use clip planes to implement a planar mirror.
'       The scene is reflected in a mirror and rendered in a 2nd pass. The
'       corners of the mirrors, together with the camera eye point, are used
'       to define a custom set of clip planes so that the reflected geometry
'       appears only within the mirror's boundaries.
'
'       Note: This code uses the D3D Framework helper library.
'
' Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace ClipMirrorSample
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
        '/ Custom mirror vertex type.
        '/ </summary>
        Private Structure MirrorVertex
            Public p As Vector3
            Public n As Vector3
            Public color As Integer

            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal Or VertexFormats.Diffuse
        End Structure 'MirrorVertex

        Private teapotMesh As GraphicsMesh = Nothing
        Private drawingFont As GraphicsFont = Nothing
        Private teapotMatrix As New Matrix()
        Private mirrorVertexBuffer As VertexBuffer = Nothing

        ' Vectors defining the camera
        Private eyePart As New Vector3(0.0F, 2.0F, -6.5F)
        Private lookAtPart As New Vector3(0.0F, 0.0F, 0.0F)
        Private upVector As New Vector3(0.0F, 1.0F, 0.0F)





        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "ClipMirror: Using D3D Clip Planes"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            Me.ClientSize = New System.Drawing.Size(400, 300)

            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Set the teapot's local matrix (rotating about the y-axis)
            teapotMatrix.RotateY(appTime)

            ' When the window has focus, let the mouse adjust the camera view
            If Me Is System.Windows.Forms.Form.ActiveForm Then
                Dim quat As Quaternion = GraphicsUtility.GetRotationFromCursor(Me)
                eyePart.X = 5 * quat.Y
                eyePart.Y = 5 * quat.X
                eyePart.Z = -CSng(Math.Sqrt((50.0F - 25 * quat.X * quat.X - 25 * quat.Y * quat.Y)))

                Device.Transform.View = Matrix.LookAtLH(eyePart, lookAtPart, upVector)
            End If
        End Sub 'FrameMove





        '/ <summary>
        '/ Renders all objects in the scene.
        '/ </summary>
        Public Sub RenderScene()
            Dim matLocal, matWorldSaved As Matrix
            matWorldSaved = Device.Transform.World

            ' Build the local matrix
            matLocal = Matrix.Multiply(teapotMatrix, matWorldSaved)
            Device.Transform.World = matLocal

            ' Render the object
            teapotMesh.Render(Device)

            Device.Transform.World = matWorldSaved

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)
        End Sub 'RenderScene





        '/ <summary>
        '/ Renders the scene as reflected in a mirror. The corners of the mirror
        '/ define a plane, which is used to build the reflection matrix. The scene is 
        '/ rendered with the cull-mode reversed, since all normals in the scene are 
        '/ likewise reflected.
        '/ </summary>
        Public Sub RenderMirror()
            Dim matWorldSaved As Matrix
            Dim matReflectInMirror As New Matrix()
            Dim plane As Plane

            ' Save the world matrix so it can be restored
            matWorldSaved = Device.Transform.World

            ' Get the four corners of the mirror. (This should be dynamic rather than
            ' hardcoded.)
            Dim a As New Vector3(-1.5F, 1.5F, 3.0F)
            Dim b As New Vector3(1.5F, 1.5F, 3.0F)
            Dim c As New Vector3(-1.5F, -1.5F, 3.0F)
            Dim d As New Vector3(1.5F, -1.5F, 3.0F)

            ' Construct the reflection matrix
            plane = plane.FromPoints(a, b, c)
            matReflectInMirror.Reflect(plane)
            Device.Transform.World = matReflectInMirror

            ' Reverse the cull mode (since normals will be reflected)
            Device.RenderState.CullMode = Cull.Clockwise

            ' Set the custom clip planes (so geometry is clipped by mirror edges).
            ' This is the heart of this sample. The mirror has 4 edges, so there are
            ' 4 clip planes, each defined by two mirror vertices and the eye point.
            Device.ClipPlanes(0).Plane = plane.FromPoints(b, a, eyePart)
            Device.ClipPlanes(1).Plane = plane.FromPoints(d, b, eyePart)
            Device.ClipPlanes(2).Plane = plane.FromPoints(c, d, eyePart)
            Device.ClipPlanes(3).Plane = plane.FromPoints(a, c, eyePart)
            Device.ClipPlanes.EnableAll()

            ' Render the scene
            RenderScene()

            ' Restore the modified render states
            Device.Transform.World = matWorldSaved
            Device.ClipPlanes.DisableAll()
            Device.RenderState.CullMode = Cull.CounterClockwise

            ' Finally, render the mirror itself (as an alpha-blended quad)
            Device.RenderState.AlphaBlendEnable = True
            Device.RenderState.SourceBlend = Blend.SourceAlpha
            Device.RenderState.DestinationBlend = Blend.InvSourceAlpha

            Device.SetStreamSource(0, mirrorVertexBuffer, 0)
            Device.VertexFormat = MIRRORVERTEX.Format
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            Device.RenderState.AlphaBlendEnable = False
        End Sub 'RenderMirror





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)

            Device.BeginScene()
            RenderScene()
            RenderMirror() ' Render the scene in the mirror
            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)

            ' Set up the geometry objects
            Try
                If teapotMesh Is Nothing Then
                    teapotMesh = New GraphicsMesh()
                End If
                teapotMesh.Create(Device, "teapot.x")
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try

            If (mirrorVertexBuffer Is Nothing) Then
                CreateVertexBuffer()
            ElseIf mirrorVertexBuffer.Disposed Then
                CreateVertexBuffer()
            End If
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Set up the geometry objects
            teapotMesh.RestoreDeviceObjects(Device, Nothing)

            ' Set up the textures
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear

            ' Set miscellaneous render states
            Device.RenderState.DitherEnable = True
            Device.RenderState.SpecularEnable = True
            Device.RenderState.ZBufferEnable = True

            ' Set up the matrices
            Device.Transform.World = Matrix.Identity
            Device.Transform.View = Matrix.LookAtLH(eyePart, lookAtPart, upVector)
            Dim fAspect As Single = device.PresentationParameters.BackBufferWidth / CSng(device.PresentationParameters.BackBufferHeight)
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 100.0F)

            ' Set up a light
            If Caps.VertexProcessingCaps.SupportsDirectionAllLights Or Not BehaviorFlags.HardwareVertexProcessing Then
                GraphicsUtility.InitLight(Device.Lights(0), LightType.Directional, 0.2F, -1.0F, -0.2F)
                Device.Lights(0).Commit()
                Device.Lights(0).Enabled = True
            End If
            Device.RenderState.Ambient = System.Drawing.Color.Black
        End Sub 'RestoreDeviceObjects




        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for alpha blending)
            End If
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.Mixed Or vertexProcessingType = vertexProcessingType.PureHardware Then
                If caps.MaxUserClipPlanes < 4 Then
                    Return False
                End If
            End If
            Return True
        End Function 'ConfirmDevice





        Public Sub CreateVertexBuffer()
            ' Create a square for rendering the mirror
            mirrorVertexBuffer = New VertexBuffer(GetType(MirrorVertex), 4, Device, Usage.WriteOnly, MirrorVertex.Format, Pool.Default)

            ' Hook the created event so we can repopulate our data
            AddHandler mirrorVertexBuffer.Created, AddressOf Me.VertexBufferCreated
            ' Call our created handler for the first time
            Me.VertexBufferCreated(mirrorVertexBuffer, Nothing)
        End Sub
        Public Sub VertexBufferCreated(ByVal sender As Object, ByVal e As System.EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            ' Initialize the mirror's vertices
            Dim v As MirrorVertex() = CType(vb.Lock(0, GetType(MirrorVertex), 0, 4), MirrorVertex())
            v(0).p = New Vector3(-1.5F, 1.5F, 3.0F)
            v(2).p = New Vector3(-1.5F, -1.5F, 3.0F)
            v(1).p = New Vector3(1.5F, 1.5F, 3.0F)
            v(3).p = New Vector3(1.5F, -1.5F, 3.0F)
            v(0).n = New Vector3(0.0F, 0.0F, -1.0F)
            v(1).n = v(0).n : v(2).n = v(0).n : v(3).n = v(0).n
            v(0).color = &H80FFFFFF
            v(1).color = v(0).color : v(2).color = v(0).color : v(3).color = v(0).color
            vb.Unlock()
        End Sub 'VertexBufferCreated

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
End Namespace 'ClipMirrorSample