'-----------------------------------------------------------------------------
' File: MotionBlur.vb
'
' Desc: Sample code showing how to use vertex shader to create a motion blur
'       effect in D3D.
'
' Copyright (c) Microsoft Corporation. All rights reserved.
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

        Private ballMesh As Mesh = Nothing
        Private effect As Effect = Nothing
        Private worldMatrixPrevious1 As Matrix = Matrix.Zero
        Private worldMatrixPrevious2 As Matrix = Matrix.Zero
        Private worldMatrixCurrent1 As Matrix = Matrix.Zero
        Private worldMatrixCurrent2 As Matrix = Matrix.Zero
        Private arcBallMatrix As Matrix = Matrix.Zero
        Private viewMatrix As Matrix = Matrix.Zero
        Private projectionMatrix As Matrix = Matrix.Zero
        Private trailLength As Single = 0.0F
        Private rotateAngle1 As Single = 0.0F
        Private rotateAngle2 As Single = 0.0F
        Private isFirstObjectInFront As Boolean
        Private isHelpShowing As Boolean = False




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Motion Blur"
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
            rotateAngle1 += 2.0F * elapsedTime
            rotateAngle2 += 2.0F * elapsedTime

            worldMatrixPrevious1 = worldMatrixCurrent1
            worldMatrixPrevious2 = worldMatrixCurrent2

            Dim position1 As New Vector3(14 * CSng(Math.Cos(rotateAngle1)), 0.0F, 7 * CSng(Math.Sin(rotateAngle1)))
            Dim position2 As New Vector3(14 * CSng(Math.Cos(rotateAngle2)), 0.0F, 7 * CSng(Math.Sin(rotateAngle2)))

            worldMatrixCurrent1 = Matrix.Translation(position1.X, position1.Y, position1.Z)
            worldMatrixCurrent2 = Matrix.Translation(position2.X, position2.Y, position2.Z)

            ' Transform positions into camera space to see which is in front
            Dim worldViewMatrix As Matrix
            worldViewMatrix = Matrix.Multiply(Matrix.Multiply(worldMatrixCurrent1, arcBallMatrix), viewMatrix)
            position1.TransformCoordinate(worldViewMatrix)

            worldViewMatrix = Matrix.Multiply(Matrix.Multiply(worldMatrixCurrent2, arcBallMatrix), viewMatrix)
            position2.TransformCoordinate(worldViewMatrix)

            isFirstObjectInFront = position1.Z < position2.Z


            worldMatrixCurrent1.Transpose(worldMatrixCurrent1)
            worldMatrixCurrent2.Transpose(worldMatrixCurrent2)

            Dim viewMatrixTranspose As Matrix = Matrix.Zero

            viewMatrixTranspose.Transpose(viewMatrix)

            ' Update rotation matrix from ArcBall
            arcBallMatrix.Transpose(ourArcball.RotationMatrix)
            Device.SetVertexShaderConstant(8, New Matrix() {arcBallMatrix})

            Device.SetVertexShaderConstant(12, New Matrix() {viewMatrixTranspose})
            Device.SetVertexShaderConstant(16, New Matrix() {projectionMatrix})

            Dim trailConst As New Vector4(trailLength, 0.0F, 1.0F, 0.0F)
            Device.SetVertexShaderConstant(20, New Vector4() {trailConst})
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            Dim numPasses As Integer
            Dim pass As Integer

            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)

            Device.BeginScene()

            ' draw sphere mesh
            If isFirstObjectInFront Then
                Device.SetVertexShaderConstant(0, New Matrix() {worldMatrixPrevious2})
                Device.SetVertexShaderConstant(4, New Matrix() {worldMatrixCurrent2})
            Else
                Device.SetVertexShaderConstant(0, New Matrix() {worldMatrixPrevious1})
                Device.SetVertexShaderConstant(4, New Matrix() {worldMatrixCurrent1})
            End If
            numPasses = effect.Begin(0)
            For pass = 0 To numPasses - 1
                effect.Pass(pass)
                ballMesh.DrawSubset(0)
            Next pass
            effect.End()

            If isFirstObjectInFront Then
                Device.SetVertexShaderConstant(0, New Matrix() {worldMatrixPrevious1})
                Device.SetVertexShaderConstant(4, New Matrix() {worldMatrixCurrent1})
            Else
                Device.SetVertexShaderConstant(0, New Matrix() {worldMatrixPrevious2})
                Device.SetVertexShaderConstant(4, New Matrix() {worldMatrixCurrent2})
            End If
            numPasses = effect.Begin(0)
            For pass = 0 To numPasses - 1
                effect.Pass(pass)
                ballMesh.DrawSubset(0)
            Next pass
            effect.End()


            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)
            If isHelpShowing Then
                drawingFontSmall.DrawText(2, 42, System.Drawing.Color.Cyan, "Use mouse to rotate the scene:")
                drawingFontSmall.DrawText(2, 62, System.Drawing.Color.Cyan, "Increase Trail Length")
                drawingFontSmall.DrawText(150, 62, System.Drawing.Color.Cyan, "Up Arrow Key")
                drawingFontSmall.DrawText(2, 80, System.Drawing.Color.Cyan, "Decrease Trail Length")
                drawingFontSmall.DrawText(150, 80, System.Drawing.Color.Cyan, "Down Arrow Key")
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
            ' World matrix for previous frame
            ' NOTE: We need one-time initialization for previous frame (for 1st frame)
            worldMatrixCurrent1.Translate(0.0F, 0.0F, 7.0F)
            worldMatrixCurrent1.Transpose(worldMatrixPrevious1)
            worldMatrixCurrent2.Translate(-14.0F, 0.0F, 0.0F)
            worldMatrixCurrent2.Transpose(worldMatrixPrevious2)

            ' VIEW matrix for the entire scene
            ' NOTE: VIEW matrix is fixed
            Dim vFromPt As New Vector3(0.0F, 0.0F, -26.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            viewMatrix = Matrix.LookAtLH(vFromPt, vLookatPt, vUpVec)

            rotateAngle1 = CSng(Math.PI) / 2.0F
            rotateAngle2 = CSng(Math.PI)

            trailLength = 3.0F

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

            ' Load a sphere mesh
            ballMesh = Mesh.Sphere(Device, 1.2F, 36, 36)

            ' Load the effect
            Dim strPath As String = DXUtil.FindMediaFile(Nothing, "MotionBlur.fx")
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
            If vertexProcessingType = vertexProcessingType.Software Then
                Return True
            End If
            If Version.op_GreaterThanOrEqual(caps.VertexShaderVersion, New Version(1, 1)) Then
                Return True
            End If
            Return False
        End Function 'ConfirmDevice




        '/ <summary>
        '/ Event Handler for windows messages
        '/ </summary>
        Private Sub OnPrivateKeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs)
            If e.KeyCode = System.Windows.Forms.Keys.Up Then
                If trailLength < 8.0F Then
                    trailLength += 0.3F
                End If
            End If
            If e.KeyCode = System.Windows.Forms.Keys.Down Then
                If trailLength > 2.0F Then
                    trailLength -= 0.3F
                End If
            End If
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