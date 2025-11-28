'-----------------------------------------------------------------------------
' File: StencilDepth.vb
'
' Desc: Example code showing how to use stencil buffers to show the depth
'       complexity of a scene.
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


Namespace StencilDepth
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample

        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private canShowDepthComplexity As Boolean = True ' Should we show the depth complexity?
        Private drawHelicopter As Boolean = False ' Should we draw the helicopter or airplane?
        Private fileObject(2) As GraphicsMesh ' The graphical object we'll be rendering
        Private squareVertexBuffer As VertexBuffer = Nothing ' The vertex buffer we'll use to draw
        Private worldMatrix As Matrix = Matrix.Identity ' World matrix
        Private mnuOptions As System.Windows.Forms.MenuItem = Nothing ' Main options window
        Private WithEvents mnuShowDepth As System.Windows.Forms.MenuItem = Nothing ' Show Depth
        Private WithEvents mnuPick As System.Windows.Forms.MenuItem = Nothing
        ' Pick object to draw


        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "StencilDepth: Displaying Depth Complexity"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
            MinDepthBits = 16
            MinStencilBits = 4

            mnuShowDepth = New MenuItem("Show &Depth Complexity")
            mnuShowDepth.Shortcut = Shortcut.CtrlD
            mnuShowDepth.Checked = canShowDepthComplexity

            mnuPick = New MenuItem()
            mnuPick.Text = IIf(drawHelicopter, "&Show Airplane", "&Show Helicopter")
            mnuPick.Shortcut = Shortcut.CtrlS

            mnuOptions = New MenuItem("&Options", New MenuItem() {mnuShowDepth, mnuPick})
            mnuMain.MenuItems.Add(mnuOptions)
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Setup the world spin matrix
            worldMatrix = Matrix.RotationAxis(New Vector3(1.0F, 1.0F, 0.0F), appTime / 2)
        End Sub 'FrameMove





        '-----------------------------------------------------------------------------
        ' Name: SetStatesForRecordingDepthComplexity()
        ' Desc: Turns on stencil and other states for recording the depth complexity
        '       during the rendering of a scene.
        '-----------------------------------------------------------------------------
        Sub SetStatesForRecordingDepthComplexity()
            ' Clear the stencil buffer
            Device.Clear(ClearFlags.Stencil, System.Drawing.Color.Black.ToArgb(), 1.0F, 0)

            ' Turn stenciling
            Device.RenderState.StencilEnable = True
            Device.RenderState.StencilFunction = Compare.Always
            Device.RenderState.ReferenceStencil = 0
            Device.RenderState.StencilMask = &H0
            Device.RenderState.StencilWriteMask = -1

            ' Increment the stencil buffer for each pixel drawn
            Device.RenderState.StencilZBufferFail = StencilOperation.IncrementSaturation
            Device.RenderState.StencilFail = StencilOperation.Keep
            Device.RenderState.StencilPass = StencilOperation.IncrementSaturation
        End Sub 'SetStatesForRecordingDepthComplexity





        '-----------------------------------------------------------------------------
        ' Name: ShowDepthComplexity()
        ' Desc: Draws the contents of the stencil buffer in false color. Use alpha
        '       blending of one red, one green, and one blue rectangle to do false
        '       coloring of bits 1, 2, and 4 in the stencil buffer.
        '-----------------------------------------------------------------------------
        Sub ShowDepthComplexity()
            ' Turn off the buffer, and enable alpha blending
            Device.RenderState.ZBufferEnable = False
            Device.RenderState.AlphaBlendEnable = True
            Device.RenderState.SourceBlend = Blend.SourceColor
            Device.RenderState.DestinationBlend = Blend.InvSourceColor

            ' Set up the stencil states
            Device.RenderState.StencilZBufferFail = StencilOperation.Keep
            Device.RenderState.StencilFail = StencilOperation.Keep
            Device.RenderState.StencilPass = StencilOperation.Keep
            Device.RenderState.StencilFunction = Compare.NotEqual
            Device.RenderState.ReferenceStencil = 0

            ' Set the background to black
            Device.Clear(ClearFlags.Target, System.Drawing.Color.Black.ToArgb(), 1.0F, 0)

            ' Set render states for drawing a rectangle that covers the viewport.
            ' The color of the rectangle will be passed in D3DRS_TEXTUREFACTOR
            Device.VertexFormat = VertexFormats.Transformed
            Device.SetStreamSource(0, squareVertexBuffer, 0)
            Device.TextureState(0).ColorArgument1 = TextureArgument.TFactor
            Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1

            ' Draw a red rectangle wherever the 1st stencil bit is set
            Device.RenderState.StencilMask = &H1
            Device.RenderState.TextureFactor = System.Drawing.Color.Red.ToArgb()
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Draw a green rectangle wherever the 2nd stencil bit is set
            Device.RenderState.StencilMask = &H2
            Device.RenderState.TextureFactor = System.Drawing.Color.LightGreen.ToArgb()
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Draw a blue rectangle wherever the 3rd stencil bit is set
            Device.RenderState.StencilMask = &H4
            Device.RenderState.TextureFactor = System.Drawing.Color.Blue.ToArgb()
            Device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2)

            ' Restore states
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.StencilEnable = False
            Device.RenderState.AlphaBlendEnable = False
        End Sub 'ShowDepthComplexity




        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue.ToArgb(), 1.0F, 0)

            Device.BeginScene()

            ' Ignore exceptions during rendering
            DirectXException.IgnoreExceptions()


            If canShowDepthComplexity Then
                SetStatesForRecordingDepthComplexity()
            End If

            Device.Transform.World = worldMatrix
            If drawHelicopter Then
                fileObject(0).Render(Device)
            Else
                fileObject(1).Render(Device)
            End If
            ' Show the depth complexity of the scene
            If canShowDepthComplexity Then
                ShowDepthComplexity()
            End If
            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            DirectXException.EnableExceptions()
            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize the font's internal textures
            drawingFont.InitializeDeviceObjects(Device)

            ' Load the main file object
            Dim i As Integer
            For i = 0 To 1
                If fileObject(i) Is Nothing Then
                    fileObject(i) = New GraphicsMesh()
                End If
            Next i
            Try
                ' Load a .X file
                fileObject(0).Create(Device, "Heli.x")
                fileObject(1).Create(Device, "airplane 2.x")
                If squareVertexBuffer Is Nothing Then
                    CreateVertexBuffer()
                ElseIf squareVertexBuffer.Disposed Then
                    CreateVertexBuffer()
                End If
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try
        End Sub 'InitializeDeviceObjects



        Private Sub CreateVertexBuffer()
            ' Create a big square for rendering the stencilbuffer contents
            squareVertexBuffer = New VertexBuffer(GetType(Vector4), 4, Device, Usage.WriteOnly, VertexFormats.Transformed, Pool.Default)
            AddHandler squareVertexBuffer.Created, AddressOf Me.SquareVBCreated
            Me.SquareVBCreated(squareVertexBuffer, Nothing)
        End Sub



        '/ <summary>
        '/ Called when the vertex buffer has been created
        '/ </summary>
        Private Sub SquareVBCreated(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim sx As Single = CSng(Device.PresentationParameters.BackBufferWidth)
            Dim sy As Single = CSng(Device.PresentationParameters.BackBufferHeight)
            Dim v As Vector4() = CType(vb.Lock(0, 0), Vector4())
            v(0) = New Vector4(0, sy, 0.0F, 1.0F)
            v(1) = New Vector4(0, 0, 0.0F, 1.0F)
            v(2) = New Vector4(sx, sy, 0.0F, 1.0F)
            v(3) = New Vector4(sx, 0, 0.0F, 1.0F)
            vb.Unlock()
        End Sub 'SquareVBCreated




        '/ <summary>
        '/ The ShowDepthComplexity menu item has been clicked, switch
        '/ modes
        '/ </summary>
        Private Sub DepthClicked(ByVal sender As Object, ByVal e As EventArgs) Handles mnuShowDepth.Click
            canShowDepthComplexity = Not canShowDepthComplexity
            CType(sender, MenuItem).Checked = canShowDepthComplexity
        End Sub 'DepthClicked




        '/ <summary>
        '/ The user wants to see the other object
        '/ </summary>
        Private Sub PickClicked(ByVal sender As Object, ByVal e As EventArgs) Handles mnuPick.Click
            drawHelicopter = Not drawHelicopter
            CType(sender, MenuItem).Text = IIf(drawHelicopter, "&Show Airplane", "&Show Helicopter")
        End Sub 'PickClicked



        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Build the device objects for the file-based objecs
            Dim i As Integer
            For i = 0 To 1
                fileObject(i).RestoreDeviceObjects(Device, Nothing)
            Next i
            ' Setup textures (the .X file may have textures)
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear

            ' Set up misc render states
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.DitherEnable = True
            Device.RenderState.SpecularEnable = False
            Device.RenderState.ColorVertex = True
            Device.RenderState.Ambient = System.Drawing.Color.Black

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 0.0F, -15.0F)
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

            ' Set up a material
            Device.Material = GraphicsUtility.InitMaterial(System.Drawing.Color.White)

            ' Set up the light
            GraphicsUtility.InitLight(Device.Lights(0), LightType.Directional, 0.0F, -1.0F, 0.0F)
            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True
            Device.RenderState.Lighting = True
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for stencil operations)
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
End Namespace 'StencilDepth