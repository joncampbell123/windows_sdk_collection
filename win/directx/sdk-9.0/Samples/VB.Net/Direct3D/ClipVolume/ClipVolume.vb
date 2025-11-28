'-----------------------------------------------------------------------------
' File: ClipVolume.vb
'
' Desc: Sample code showing how to use vertex shader and pixel shader to do
'       clipping effect in D3D.
'
' Copyright (c) 2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace MotionBlurSample
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample




        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private drawingFontSmall As GraphicsFont = Nothing ' Font for drawing text
        Private ourArcball As GraphicsArcBall = Nothing

        Private teapotMesh As Mesh = Nothing
        Private sphereMesh As Mesh = Nothing
        Private effect As Effect = Nothing
        Private teapotWorldMatrix As Matrix = Matrix.Zero
        Private sphereWorldMatrix As Matrix = Matrix.Zero
        Private arcBallMatrix As Matrix = Matrix.Zero
        Private projectionMatrix As Matrix = Matrix.Zero
        Private sphereMove As Single = 0.0F
        Private sphereCenter As Vector4

        Private isHelpShowing As Boolean = False




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Clip Volume"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            drawingFontSmall = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold, 9)
            enumerationSettings.AppUsesDepthBuffer = True

            ourArcball = New GraphicsArcBall(Me)
            AddHandler Me.KeyDown, AddressOf Me.OnPrivateKeyDown
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Update translation matrix for sphere
            If sphereCenter.X > 2.0F Then
                sphereMove = -0.6F
            ElseIf sphereCenter.X < -2.0F Then
                sphereMove = 0.6F
            End If
            sphereCenter.X += sphereMove * elapsedTime
            sphereWorldMatrix.Translate(sphereCenter.X, sphereCenter.Y, sphereCenter.Z)

            sphereWorldMatrix.Transpose(sphereWorldMatrix)
            'Update rotation matrix from ArcBall
            arcBallMatrix.Transpose(ourArcball.RotationMatrix)
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            Dim numPasses As Integer
            Dim pass As Integer

            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Black, 1.0F, 0)

            Device.BeginScene()

            ' draw teapot mesh
            Device.SetVertexShaderConstant(0, New Matrix() {arcBallMatrix})
            Device.SetVertexShaderConstant(4, New Matrix() {teapotWorldMatrix})
            Device.SetVertexShaderConstant(8, New Matrix() {projectionMatrix})
            Device.SetVertexShaderConstant(12, New Vector4() {sphereCenter})
            effect.Technique = effect.GetTechnique("Teapot")
            numPasses = effect.Begin(0)
            For pass = 0 To numPasses - 1
                effect.Pass(pass)
                teapotMesh.DrawSubset(0)
            Next pass
            effect.End()

            ' draw sphere mesh
            Device.SetVertexShaderConstant(4, New Matrix() {sphereWorldMatrix})
            effect.Technique = effect.GetTechnique("Sphere")
            numPasses = effect.Begin(0)
            For pass = 0 To numPasses - 1
                effect.Pass(pass)
                sphereMesh.DrawSubset(0)
            Next pass
            effect.End()

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)
            If isHelpShowing Then
                drawingFontSmall.DrawText(2, 42, System.Drawing.Color.Cyan, "Use mouse to rotate the teapot:")
            Else
                drawingFontSmall.DrawText(2, 42, System.Drawing.Color.LightSteelBlue, "Press F1 for help")
            End If

            Device.EndScene()
        End Sub 'Render





        '-----------------------------------------------------------------------------
        ' Name: OneTimeSceneInitialization()
        ' Desc: Called during initial app startup, this function performs all the
        '       permanent initialization.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub OneTimeSceneInitialization()
            ' Translation matrix for teapot mesh
            ' NOTE: This translation is fixed and will preceded by the ArcBall
            '       rotation that is calculated per-frame in FrameMove
            teapotWorldMatrix = Matrix.Translation(0.0F, 0.0F, 5.0F)
            teapotWorldMatrix.Transpose(teapotWorldMatrix)

            ' Translation matrix for sphere mesh
            ' NOTE: This is built per-frame in FrameMove
            ' VIEW matrix for the entire scene
            ' NOTE: This is fixed to an identity matrix
            ' PROJECTION matrix for the entire scene
            ' NOTE: The projection is based on the window dimensions
            '       and is built in RestoreDeviceObjects
            ' Initial per-second movement delta for sphere
            sphereMove = 0.6F

            ' Initial location of center of sphere
            ' NOTE: .xyz = center of sphere
            '       .w   = radius of sphere
            sphereCenter = New Vector4(0.0F, 0.0F, 5.0F, 1.0F)

            ' Set cursor to indicate that user can move the object with the mouse
            Me.Cursor = System.Windows.Forms.Cursors.SizeAll
        End Sub 'OneTimeSceneInitialization





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize the font's internal textures
            drawingFont.InitializeDeviceObjects(Device)
            drawingFontSmall.InitializeDeviceObjects(Device)

            ' Create teapot mesh and sphere mesh
            ' Note: We don't need a declarator here since D3DXCreateSphere already 
            ' implicitly creates one for us, and it is set inside the DrawSubset call.
            teapotMesh = Mesh.Teapot(Device)
            sphereMesh = Mesh.Sphere(Device, 1.0F, 30, 30)

            ' Load the effect
            Dim strPath As String = DXUtil.FindMediaFile(Nothing, "ClipVolume.fx")
            effect = effect.FromFile(Device, strPath, Nothing, 0, Nothing)
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ourArcball.SetWindow(Device.PresentationParameters.BackBufferWidth, Device.PresentationParameters.BackBufferHeight, 0.9F)

            ' PROJECTION matrix for the entire scene
            ' NOTE: The projection is fixed
            Dim fAspect As Single = CSng(Device.PresentationParameters.BackBufferWidth) / Device.PresentationParameters.BackBufferHeight
            projectionMatrix = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 60.0F)
            projectionMatrix.Transpose(projectionMatrix)
        End Sub 'RestoreDeviceObjects






        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for alpha blending)
            End If
            ' Device should support at least both VS.1.1 and PS.1.1
            Dim ver11 As New Version(1, 1)
            If Version.op_GreaterThanOrEqual(caps.VertexShaderVersion, ver11) And Version.op_GreaterThanOrEqual(caps.PixelShaderVersion, ver11) Then
                Return True
            End If
            Return False
        End Function 'ConfirmDevice




        '/ <summary>
        '/ Event Handler for windows messages
        '/ </summary>
        Private Sub OnPrivateKeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs)
            If e.KeyCode = System.Windows.Forms.Keys.F1 Then
                isHelpShowing = Not isHelpShowing
            End If
        End Sub 'OnPrivateKeyDown



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
End Namespace 'MotionBlurSample