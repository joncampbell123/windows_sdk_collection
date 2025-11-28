'-----------------------------------------------------------------------------
' File: VertexShader.vb
'
' Desc: Example code showing how to do vertex shaders in D3D.
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace VertexShaderSample
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
        ' Scene
        Private vertexBuffer As VertexBuffer = Nothing
        Private indexBuffer As IndexBuffer = Nothing
        Private numberVertices As Integer
        Private numberIndices As Integer
        Private ourShader As VertexShader = Nothing
        Private ourDeclaration As VertexDeclaration = Nothing
        Private ourSize As Integer

        ' Transforms
        Private positionMatrix As Matrix
        Private viewMatrix As Matrix
        Private projectionMatrix As Matrix

        ' Navigation
        Private keyValues(256) As Byte
        Private speed As Single
        Private angularSpeed As Single
        Private doShowHelp As Boolean

        Private velocity As Vector3
        Private angularVelocity As Vector3




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Vertex Shader"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            drawingFontSmall = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold, 9)
            enumerationSettings.AppUsesDepthBuffer = True

            ourSize = 32
            numberIndices = (ourSize - 1) * (ourSize - 1) * 6
            numberVertices = ourSize * ourSize
            ourShader = Nothing

            speed = 5.0F
            angularSpeed = 1.0F
            doShowHelp = False

        End Sub 'New





        '-----------------------------------------------------------------------------
        ' Name: OneTimeSceneInitialization()
        ' Desc: Called during initial app startup, this function performs all the
        '       permanent initialization.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub OneTimeSceneInitialization()
            ' Setup the view matrix
            Dim vEye As New Vector3(2.0F, 3.0F, 3.0F)
            Dim vAt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUp As New Vector3(0.0F, 1.0F, 0.0F)
            viewMatrix = Matrix.LookAtRH(vEye, vAt, vUp)

            ' Set the position matrix
            positionMatrix = Matrix.Invert(viewMatrix)
        End Sub 'OneTimeSceneInitialization





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            Dim fSecsPerFrame As Single = elapsedTime

            ' Process keyboard input
            Dim vT As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vR As New Vector3(0.0F, 0.0F, 0.0F)

            If keyValues(CInt(Keys.Left)) <> 0 Or keyValues(CInt(Keys.NumPad1)) <> 0 Then
                vT.X -= 1.0F ' Slide Left
            End If
            If keyValues(CInt(Keys.Right)) <> 0 Or keyValues(CInt(Keys.NumPad3)) <> 0 Then
                vT.X += 1.0F ' Slide Right
            End If
            If keyValues(CInt(Keys.Down)) <> 0 Then
                vT.Y -= 1.0F ' Slide Down
            End If
            If keyValues(CInt(Keys.Up)) <> 0 Then
                vT.Y += 1.0F ' Slide Up
            End If
            If keyValues(Asc("W")) <> 0 Then
                vT.Z -= 2.0F ' Move Forward
            End If
            If keyValues(Asc("S")) <> 0 Then
                vT.Z += 2.0F ' Move Backward
            End If
            If keyValues(Asc("A")) <> 0 Or keyValues(CInt(Keys.NumPad8)) <> 0 Then
                vR.X -= 1.0F ' Pitch Down
            End If
            If keyValues(Asc("Z")) <> 0 Or keyValues(CInt(Keys.NumPad2)) <> 0 Then
                vR.X += 1.0F ' Pitch Up
            End If
            If keyValues(Asc("E")) <> 0 Or keyValues(CInt(Keys.NumPad6)) <> 0 Then
                vR.Y -= 1.0F ' Turn Right
            End If
            If keyValues(Asc("Q")) <> 0 Or keyValues(CInt(Keys.NumPad4)) <> 0 Then
                vR.Y += 1.0F ' Turn Left
            End If
            If keyValues(CInt(Keys.NumPad9)) <> 0 Then
                vR.Z -= 2.0F ' Roll CW
            End If
            If keyValues(CInt(Keys.NumPad7)) <> 0 Then
                vR.Z += 2.0F ' Roll CCW
            End If
            velocity = Vector3.Add(Vector3.Multiply(velocity, 0.9F), Vector3.Multiply(vT, 0.1F))
            angularVelocity = Vector3.Add(Vector3.Multiply(angularVelocity, 0.9F), Vector3.Multiply(vR, 0.1F))

            ' Update position and view matricies
            Dim matT, matR As Matrix
            Dim qR As Quaternion

            vT = Vector3.Multiply(Vector3.Multiply(velocity, fSecsPerFrame), speed)
            vR = Vector3.Multiply(Vector3.Multiply(angularVelocity, fSecsPerFrame), angularSpeed)

            matT = Matrix.Translation(vT.X, vT.Y, vT.Z)
            positionMatrix = Matrix.Multiply(matT, positionMatrix)

            qR = Quaternion.RotationYawPitchRoll(vR.Y, vR.X, vR.Z)
            matR = Matrix.RotationQuaternion(qR)

            positionMatrix = Matrix.Multiply(matR, positionMatrix)
            viewMatrix = Matrix.Invert(positionMatrix)
            Device.Transform.View = viewMatrix

            ' Set up the vertex shader constants
            Dim mat As Matrix = Matrix.Multiply(viewMatrix, projectionMatrix)
            mat.Transpose(mat)

            Dim vA As New Vector4(CSng(Math.Sin(appTime)) * 15.0F, 0.0F, 0.5F, 1.0F)
            Dim vD As New Vector4(CSng(Math.PI), 1.0F / (2.0F * CSng(Math.PI)), 2.0F * CSng(Math.PI), 0.05F)

            ' Taylor series coefficients for sin and cos
            Dim vSin As New Vector4(1.0F, -1.0F / 6.0F, 1.0F / 120.0F, -1.0F / 5040.0F)
            Dim vCos As New Vector4(1.0F, -1.0F / 2.0F, 1.0F / 24.0F, -1.0F / 720.0F)

            Device.SetVertexShaderConstant(0, New Matrix() {mat})
            Device.SetVertexShaderConstant(4, New Vector4() {vA})
            Device.SetVertexShaderConstant(7, New Vector4() {vD})
            Device.SetVertexShaderConstant(10, New Vector4() {vSin})
            Device.SetVertexShaderConstant(11, New Vector4() {vCos})
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, &HFF, 1.0F, 0)

            Device.BeginScene()

            Device.VertexDeclaration = ourDeclaration
            Device.VertexShader = ourShader
            Device.SetStreamSource(0, vertexBuffer, 0, DXHelp.GetTypeSize(GetType(Vector2)))
            Device.Indices = indexBuffer
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numberVertices, 0, numberIndices / 3)


            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)
            If doShowHelp Then
                drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White, "Keyboard controls:")
                drawingFontSmall.DrawText(20, 60, System.Drawing.Color.White, "Move" + ControlChars.Lf + "Turn" + ControlChars.Lf + "Pitch" + ControlChars.Lf + "Slide" + ControlChars.Lf + "Help" + ControlChars.Lf + "Change device" + ControlChars.Lf + "Exit")
                drawingFontSmall.DrawText(210, 60, System.Drawing.Color.White, "W,S" + ControlChars.Lf + "E,Q" + ControlChars.Lf + "A,Z" + ControlChars.Lf + "Arrow keys" + ControlChars.Lf + "F1" + ControlChars.Lf + "F2" + ControlChars.Lf + "Esc")
            Else
                drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White, "Press F1 for help")
            End If

            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
            drawingFontSmall.InitializeDeviceObjects(Device)
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Setup render states
            Device.RenderState.Lighting = False
            Device.RenderState.CullMode = Cull.None

            ' Create index buffer
            indexBuffer = New IndexBuffer(GetType(Short), numberIndices, Device, 0, Pool.Default)

            Dim indices As Short() = CType(indexBuffer.Lock(0, 0), Short())

            Dim count As Integer = 0
            Dim y As Integer
            For y = 1 To ourSize - 1
                Dim x As Integer
                For x = 1 To ourSize - 1
                    indices(count) = CShort((y - 1) * ourSize + (x - 1)) : count += 1
                    indices(count) = CShort((y - 0) * ourSize + (x - 1)) : count += 1
                    indices(count) = CShort((y - 1) * ourSize + (x - 0)) : count += 1

                    indices(count) = CShort((y - 1) * ourSize + (x - 0)) : count += 1
                    indices(count) = CShort((y - 0) * ourSize + (x - 1)) : count += 1
                    indices(count) = CShort((y - 0) * ourSize + (x - 0)) : count += 1
                Next x
            Next y

            indexBuffer.Unlock()


            ' Create vertex buffer
            vertexBuffer = New VertexBuffer(GetType(Vector2), numberVertices, Device, Usage.WriteOnly, 0, Pool.Default)

            Dim vertices As Vector2() = CType(vertexBuffer.Lock(0, 0), Vector2())

            count = 0
            For y = 0 To ourSize - 1
                Dim x As Integer
                For x = 0 To ourSize - 1
                    vertices(count) = New Vector2((CSng(x) / CSng(ourSize - 1) - 0.5F) * CSng(Math.PI), (CSng(y) / CSng(ourSize - 1) - 0.5F) * CSng(Math.PI)) : count += 1
                Next x
            Next y

            vertexBuffer.Unlock()
            ' Create vertex shader
            Dim shaderPath As String = Nothing
            Dim code As GraphicsStream = Nothing
            ' Create our declaration
            Dim decl() As VertexElement = {New VertexElement(0, 0, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.Position, 0), VertexElement.VertexDeclarationEnd}
            ourDeclaration = New VertexDeclaration(Device, decl)

            ' Find the vertex shader file
            shaderPath = DXUtil.FindMediaFile(Nothing, "Ripple.vsh")

            ' Assemble the vertex shader from the file
            code = ShaderLoader.FromFile(shaderPath, Nothing, 0)
            ' Create the vertex shader
            ourShader = New VertexShader(Device, code)
            code.Close()

            ' Set up the projection matrix
            Dim fAspectRatio As Single = CSng(Device.PresentationParameters.BackBufferWidth) / CSng(Device.PresentationParameters.BackBufferHeight)
            projectionMatrix = Matrix.PerspectiveFovRH(Geometry.DegreeToRadian(60.0F), fAspectRatio, 0.1F, 100.0F)
            Device.Transform.Projection = projectionMatrix
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub InvalidateDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            If Not (ourShader Is Nothing) Then
                ourShader.Dispose()
            End If
        End Sub 'InvalidateDeviceObjects




        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.PureHardware Or vertexProcessingType = vertexProcessingType.Mixed Then
                If caps.VertexShaderVersion.Major < 1 Then
                    Return False
                End If
            End If
            Return True
        End Function 'ConfirmDevice




        '/ <summary>
        '/ Event Handler for windows messages
        '/ </summary>
        Private Sub OnPrivateKeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
            keyValues(CInt(e.KeyCode)) = 0
            If e.KeyCode = System.Windows.Forms.Keys.F1 Then
                doShowHelp = Not doShowHelp
            End If
        End Sub 'OnPrivateKeyUp

        Private Sub OnPrivateKeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
            keyValues(CInt(e.KeyCode)) = 1
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
End Namespace 'VertexShaderSample