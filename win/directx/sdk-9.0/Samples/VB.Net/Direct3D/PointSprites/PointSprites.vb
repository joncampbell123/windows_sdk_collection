'-----------------------------------------------------------------------------
' File: PointSprites.vb
'
' Desc: Example code showing how to do vertex shaders in D3D.
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace PointSprites


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
       _




        '-----------------------------------------------------------------------------
        ' Custom vertex types
        '-----------------------------------------------------------------------------
        Public Structure ColorVertex
            Public v As Vector3
            Public color As Integer
            Public tu, tv As Single
            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Diffuse Or VertexFormats.Texture1
        End Structure 'ColorVertex





        '-----------------------------------------------------------------------------
        ' Global structs and data for the ground object
        '-----------------------------------------------------------------------------
        Public GroundWidth As Single = 256.0F
        Public GroundHeight As Single = 256.0F
        Public GroundGridSize As Integer = 8
        Public GroundTile As Integer = 32
        Public GroundColor As Integer = &HCCCCCCCC

        Public Enum ParticleColors
            White
            Red
            Green
            Blue
            NumColors '
        End Enum 'ParticleColors

        Public g_clrColor As System.Drawing.Color() = {System.Drawing.Color.White, System.Drawing.Color.Firebrick, System.Drawing.Color.MediumSeaGreen, System.Drawing.Color.DodgerBlue}

        Private g_clrColorFade As System.Drawing.Color() = {System.Drawing.Color.WhiteSmoke, System.Drawing.Color.Pink, System.Drawing.Color.LightSeaGreen, System.Drawing.Color.LightCyan}

        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private drawingFontSmall As GraphicsFont = Nothing ' Font for drawing text
        ' Ground stuff
        Private groundTexture As Texture = Nothing
        Private planeGround As Plane

        Private groundVertexBuffer As VertexBuffer = Nothing
        Private groundIndexBuffer As IndexBuffer = Nothing
        Private numGroundIndices As Integer = 0
        Private numGroundVertices As Integer = 0

        ' Particle stuff
        Private particleTexture As Texture = Nothing
        Private particleSystem As particleSystem = Nothing
        Private numberParticlesToEmit As Integer = 0
        Private particleColor As ParticleColors = ParticleColors.White
        Private animateEmitter As Boolean = False

        Private keyValues(256) As Byte
        Private canDrawReflection As Boolean = False
        Private canDoALphaBlend As Boolean = False
        Private shouldDrawHelp As Boolean = False
        Private animateColor As Boolean = False

        ' Variables for determining view position
        Private position As Vector3
        Private velocity As Vector3
        Private yaw As Single = 0.0F
        Private yawVelocity As Single = 0.0F
        Private pitch As Single = 0.0F
        Private pitchVelocity As Single = 0.0F
        Private viewMatrix As Matrix
        Private orientationMatrix As Matrix




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "PointSprites: Using particle effects"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            drawingFontSmall = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold, 9)
            enumerationSettings.AppUsesDepthBuffer = True

            particleSystem = New ParticleSystem(512, 2048, 0.03F)
            numberParticlesToEmit = 10
            animateEmitter = False

            numGroundVertices = (GroundGridSize + 1) * (GroundGridSize + 1)
            numGroundIndices = GroundGridSize * GroundGridSize * 6
            planeGround = New Plane(0.0F, 1.0F, 0.0F, 0.0F)

            position = New Vector3(0.0F, 3.0F, -4.0F)
            velocity = New Vector3(0.0F, 0.0F, 0.0F)
            yaw = 0.03F
            yawVelocity = 0.0F
            pitch = 0.5F
            pitchVelocity = 0.0F
            viewMatrix = Matrix.Translation(0.0F, 0.0F, 10.0F)
            orientationMatrix = Matrix.Translation(0.0F, 0.0F, 0.0F)
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Slow things down for the REF device
            If Caps.DeviceType = DeviceType.Reference Then
                elapsedTime = 0.05F
            End If
            ' Determine emitter position
            Dim vEmitterPostion As Vector3
            If animateEmitter Then
                vEmitterPostion = New Vector3(3 * CSng(Math.Sin(appTime)), 0.0F, 3 * CSng(Math.Cos(appTime)))
            Else
                vEmitterPostion = New Vector3(0.0F, 0.0F, 0.0F)
            End If
            If animateColor Then
                particleColor += 1
                If particleColor = ParticleColors.NumColors Then
                    particleColor = ParticleColors.White
                End If
            End If ' Update particle system
            particleSystem.Update(elapsedTime, numberParticlesToEmit, g_clrColor(CInt(particleColor)), g_clrColorFade(CInt(particleColor)), 8.0F, vEmitterPostion)
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Update the camera here rather than in FrameMove() so you can
            ' move the camera even when the scene is paused
            UpdateCamera()

            Device.BeginScene()

            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, &H0, 1.0F, 0)

            ' Draw reflection of particles
            If canDrawReflection Then
                Dim matReflectedView As New Matrix()
                matReflectedView.Reflect(planeGround)
                matReflectedView.Multiply(viewMatrix)

                Device.RenderState.ZBufferWriteEnable = False
                Device.RenderState.AlphaBlendEnable = True
                Device.RenderState.SourceBlend = Blend.One
                Device.RenderState.DestinationBlend = Blend.One

                Device.Transform.View = matReflectedView
                Device.SetTexture(0, particleTexture)
                particleSystem.Render(Device)

                Device.RenderState.ZBufferWriteEnable = True
                Device.RenderState.AlphaBlendEnable = False
            End If


            ' Draw the ground
            If canDrawReflection Then
                Device.RenderState.AlphaBlendEnable = True
                Device.RenderState.SourceBlend = Blend.SourceAlpha
                Device.RenderState.DestinationBlend = Blend.InvSourceAlpha
            End If

            Device.Transform.View = viewMatrix
            Device.SetTexture(0, groundTexture)
            Device.VertexFormat = ColorVertex.Format
            Device.SetStreamSource(0, groundVertexBuffer, 0)
            Device.Indices = groundIndexBuffer
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numGroundVertices, 0, numGroundIndices / 3)


            ' Draw particles
            Device.RenderState.ZBufferWriteEnable = False
            Device.RenderState.AlphaBlendEnable = True
            Device.RenderState.SourceBlend = Blend.One
            Device.RenderState.DestinationBlend = Blend.One

            Device.SetTexture(0, particleTexture)
            particleSystem.Render(Device)

            Device.RenderState.ZBufferWriteEnable = True
            Device.RenderState.AlphaBlendEnable = False

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)
            If shouldDrawHelp Then
                drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White, "Keyboard controls:")
                drawingFontSmall.DrawText(20, 60, System.Drawing.Color.White, "Move" + ControlChars.Lf + "Turn" + ControlChars.Lf + "Pitch" + ControlChars.Lf + "Slide" + ControlChars.Lf + ControlChars.Lf + "Help" + ControlChars.Lf + "Change device" + ControlChars.Lf + "Animate emitter" + ControlChars.Lf + "Change color" + ControlChars.Lf + "Cycle Colors" + ControlChars.Lf + ControlChars.Lf + "Toggle reflection" + ControlChars.Lf + "Exit")
                drawingFontSmall.DrawText(210, 60, System.Drawing.Color.White, "W,S" + ControlChars.Lf + "E,Q" + ControlChars.Lf + "A,Z" + ControlChars.Lf + "Arrow keys" + ControlChars.Lf + ControlChars.Lf + "F1" + ControlChars.Lf + "F2" + ControlChars.Lf + "F3" + ControlChars.Lf + "F4" + ControlChars.Lf + "C" + ControlChars.Lf + ControlChars.Lf + "R" + ControlChars.Lf + "Esc")
            Else
                drawingFontSmall.DrawText(2, 40, System.Drawing.Color.White, "Press F1 for help")
            End If

            Device.EndScene()
        End Sub 'Render





        '-----------------------------------------------------------------------------
        ' Name: UpdateCamera()
        ' Desc: 
        '-----------------------------------------------------------------------------
        Sub UpdateCamera()
            Dim fElapsedTime As Single

            If elapsedTime > 0.0F Then
                fElapsedTime = elapsedTime
            Else
                fElapsedTime = 0.05F
            End If
            Dim fSpeed As Single = 3.0F * fElapsedTime
            Dim fAngularSpeed As Single = 1.0F * fElapsedTime

            ' De-accelerate the camera movement (for smooth motion)
            velocity.Multiply(0.9F)
            yawVelocity *= 0.9F
            pitchVelocity *= 0.9F

            ' Process keyboard input
            If keyValues(CInt(Keys.Left)) <> 0 Or keyValues(CInt(Keys.NumPad1)) <> 0 Then
                velocity.X -= fSpeed ' Slide Left
            End If
            If keyValues(CInt(Keys.Right)) <> 0 Or keyValues(CInt(Keys.NumPad3)) <> 0 Then
                velocity.X += fSpeed ' Slide Right
            End If
            If keyValues(CInt(Keys.Down)) <> 0 Then
                velocity.Y -= fSpeed ' Slide Down
            End If
            If keyValues(CInt(Keys.Up)) <> 0 Then
                velocity.Y += fSpeed ' Slide Up
            End If
            If keyValues(Asc("W")) <> 0 Then
                velocity.Z += fSpeed ' Move Forward
            End If
            If keyValues(Asc("S")) <> 0 Then
                velocity.Z -= fSpeed ' Move Backward
            End If
            If keyValues(Asc("A")) <> 0 Or keyValues(CInt(Keys.NumPad8)) <> 0 Then
                pitchVelocity -= fSpeed ' Pitch Down
            End If
            If keyValues(Asc("Z")) <> 0 Or keyValues(CInt(Keys.NumPad2)) <> 0 Then
                pitchVelocity += fSpeed ' Pitch Up
            End If
            If keyValues(Asc("E")) <> 0 Or keyValues(CInt(Keys.NumPad6)) <> 0 Then
                yawVelocity += fSpeed ' Turn Right
            End If
            If keyValues(Asc("Q")) <> 0 Or keyValues(CInt(Keys.NumPad4)) <> 0 Then
                yawVelocity -= fSpeed ' Turn Left
            End If
            If keyValues(CInt(Keys.Add)) <> 0 Then
                If numberParticlesToEmit < 10 Then
                    numberParticlesToEmit += 1
                End If
            End If
            If keyValues(CInt(Keys.Subtract)) <> 0 Then
                If numberParticlesToEmit > 0 Then
                    numberParticlesToEmit -= 1
                End If
            End If ' Update the position vector
            Dim vT As Vector3 = Vector3.Multiply(velocity, fSpeed)
            vT = Vector3.TransformNormal(vT, orientationMatrix)
            position.Add(vT)
            If position.Y < 1.0F Then
                position.Y = 1.0F
            End If
            ' Update the yaw-pitch-rotation vector
            yaw += fAngularSpeed * yawVelocity
            pitch += fAngularSpeed * pitchVelocity
            If pitch < 0.0F Then
                pitch = 0.0F
            End If
            If pitch > CSng(Math.PI) / 2 Then
                pitch = CSng(Math.PI) / 2
            End If
            ' Set the view matrix
            Dim qR As Quaternion = Quaternion.RotationYawPitchRoll(yaw, pitch, 0.0F)
            orientationMatrix.AffineTransformation(1.25F, New Vector3(0, 0, 0), qR, position)
            viewMatrix = Matrix.Invert(orientationMatrix)
        End Sub 'UpdateCamera






        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
            drawingFontSmall.InitializeDeviceObjects(Device)

            Try
                ' Create textures
                groundTexture = GraphicsUtility.CreateTexture(Device, "Ground2.bmp", Format.Unknown)
                particleTexture = GraphicsUtility.CreateTexture(Device, "Particle.bmp", Format.Unknown)
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try


            ' Check if we can do the reflection effect
            canDoALphaBlend = Caps.SourceBlendCaps.SupportsSourceAlpha And Caps.DestinationBlendCaps.SupportsInverseSourceAlpha

            If canDoALphaBlend Then
                canDrawReflection = True
            End If
        End Sub 'InitializeDeviceObjects


        Private Sub CreateGround(ByVal sender As Object, ByVal e As EventArgs)

            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            ' Fill vertex buffer
            Dim vertices As ColorVertex() = CType(vb.Lock(0, 0), ColorVertex())

            Dim count As Integer = 0
            ' Fill in vertices
            Dim zz As Integer
            For zz = 0 To GroundGridSize
                Dim xx As Integer
                For xx = 0 To GroundGridSize
                    vertices(count).v.X = GroundWidth * (xx / CSng(GroundGridSize) - 0.5F)
                    vertices(count).v.Y = 0.0F
                    vertices(count).v.Z = GroundHeight * (zz / CSng(GroundGridSize) - 0.5F)
                    vertices(count).color = GroundColor
                    vertices(count).tu = xx * GroundTile / CSng(GroundGridSize)
                    vertices(count).tv = zz * GroundTile / CSng(GroundGridSize)
                    count += 1
                Next xx
            Next zz

            vb.Unlock()
        End Sub 'CreateGround

        Private Sub CreateGroundIndex(ByVal sender As Object, ByVal e As EventArgs)

            Dim ib As IndexBuffer = CType(sender, IndexBuffer)
            ' Fill the index buffer
            Dim indices As Short() = CType(ib.Lock(0, 0), Short())

            Dim count As Integer = 0
            ' Fill in indices
            Dim z As Integer
            For z = 0 To GroundGridSize - 1
                Dim x As Integer
                For x = 0 To GroundGridSize - 1
                    Dim vtx As Integer = x + z * (GroundGridSize + 1)
                    indices(count) = CShort(vtx + 1) : count += 1
                    indices(count) = CShort(vtx + 0) : count += 1
                    indices(count) = CShort(vtx + 0 + (GroundGridSize + 1)) : count += 1
                    indices(count) = CShort(vtx + 1) : count += 1
                    indices(count) = CShort(vtx + 0 + (GroundGridSize + 1)) : count += 1
                    indices(count) = CShort(vtx + 1 + (GroundGridSize + 1)) : count += 1
                Next x
            Next z

            ib.Unlock()
        End Sub 'CreateGroundIndex




        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Create ground object
            ' Create vertex buffer for ground object
            If groundVertexBuffer Is Nothing Then
                groundVertexBuffer = New VertexBuffer(GetType(ColorVertex), numGroundVertices, Device, Usage.WriteOnly, ColorVertex.Format, Pool.Default)
                AddHandler groundVertexBuffer.Created, AddressOf Me.CreateGround
                Me.CreateGround(groundVertexBuffer, Nothing)
            End If

            ' Create the index buffer
            If groundIndexBuffer Is Nothing Then
                groundIndexBuffer = New IndexBuffer(GetType(Short), numGroundIndices, Device, Usage.WriteOnly, Pool.Default)
                AddHandler groundIndexBuffer.Created, AddressOf Me.CreateGroundIndex
                Me.CreateGroundIndex(groundIndexBuffer, Nothing)
            End If
            ' Set the world matrix
            Device.Transform.World = Matrix.Identity

            ' Set projection matrix
            Dim fAspect As Single = CSng(Device.PresentationParameters.BackBufferWidth) / Device.PresentationParameters.BackBufferHeight

            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 0.1F, 100.0F)

            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.TextureState(0).AlphaOperation = TextureOperation.Modulate
            Device.TextureState(1).ColorOperation = TextureOperation.Disable
            Device.TextureState(1).AlphaOperation = TextureOperation.Disable

            Device.RenderState.SourceBlend = Blend.One
            Device.RenderState.DestinationBlend = Blend.One
            Device.RenderState.Lighting = False
            Device.RenderState.CullMode = Cull.CounterClockwise
            Device.RenderState.ShadeMode = ShadeMode.Flat

            ' Initialize the particle system
            particleSystem.RestoreDeviceObjects(Device)
        End Sub 'RestoreDeviceObjects






        '/ <summary>
        '/ Called when the app is exiting, or the device is being changed, this 
        '/ function deletes any device-dependent objects.
        '/ </summary>
        Protected Overrides Sub DeleteDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            groundTexture.Dispose()
            particleTexture.Dispose()
            groundVertexBuffer.Dispose()
            groundIndexBuffer.Dispose()

            groundTexture = Nothing
            particleTexture = Nothing

            groundVertexBuffer = Nothing
            groundIndexBuffer = Nothing
        End Sub 'DeleteDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for alpha blending)
            End If
            ' Make sure device can do ONE:ONE alphablending
            If Not caps.SourceBlendCaps.SupportsOne Then
                Return False
            End If
            If Not caps.DestinationBlendCaps.SupportsOne Then
                Return False
            End If
            ' Make sure HW TnL devices can do point sprites
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.Mixed Then
                If caps.MaxPointSize <= 1.0F Then
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
                shouldDrawHelp = Not shouldDrawHelp
            End If
            If e.KeyCode = System.Windows.Forms.Keys.R Then
                If canDoALphaBlend Then
                    canDrawReflection = Not canDrawReflection
                End If
            End If
            If e.KeyCode = System.Windows.Forms.Keys.C Then
                animateColor = Not animateColor
            End If
            If e.KeyCode = System.Windows.Forms.Keys.F3 Then
                animateEmitter = Not animateEmitter
            End If
            If e.KeyCode = System.Windows.Forms.Keys.F4 Then
                particleColor += 1
                If particleColor = ParticleColors.NumColors Then
                    particleColor = ParticleColors.White
                End If
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
    _

    '/ <summary>
    '/ The particle system class
    '/ </summary>
    Public Class ParticleSystem
        Public Structure PointVertex
            Public v As Vector3
            Public color As Integer
            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Diffuse
        End Structure 'PointVertex




        '-----------------------------------------------------------------------------
        ' Global data for the particles
        '-----------------------------------------------------------------------------
        Public Structure Particle
            Public isSpark As Boolean ' Sparks are less energetic particles that
            ' are generated where/when the main particles
            ' hit the ground
            Public positionVector As Vector3 ' Current position
            Public velocityVector As Vector3 ' Current velocity
            Public initialPosition As Vector3 ' Initial position
            Public initialVelocity As Vector3 ' Initial velocity
            Public creationTime As Single ' Time of creation
            Public diffuseColor As System.Drawing.Color ' Initial diffuse color
            Public fadeColor As System.Drawing.Color ' Faded diffuse color
            Public fadeProgression As Single ' Fade progression
        End Structure 'Particle

        Private radius As Single = 0.0F

        Private time As Single = 0.0F
        Private baseParticle As Integer = 0
        Private flush As Integer = 0
        Private discard As Integer = 0

        Private particles As Integer = 0
        Private particlesLimit As Integer = 0
        Private particlesList As New System.Collections.ArrayList()
        Private freeParticles As New System.Collections.ArrayList()

        Private rand As New System.Random()

        ' Geometry
        Private vertexBuffer As vertexBuffer = Nothing


        Public Sub New(ByVal numFlush As Integer, ByVal numDiscard As Integer, ByVal fRadius As Single)
            radius = fRadius

            baseParticle = numDiscard
            flush = numFlush
            discard = numDiscard

            particles = 0
            particlesLimit = 2048
        End Sub 'New

        Public Sub RestoreDeviceObjects(ByVal dev As Device)
            ' Create a vertex buffer for the particle system.  The size of this buffer
            ' does not relate to the number of particles that exist.  Rather, the
            ' buffer is used as a communication channel with the device.. we fill in 
            ' a bit, and tell the device to draw.  While the device is drawing, we
            ' fill in the next bit using NOOVERWRITE.  We continue doing this until 
            ' we run out of vertex buffer space, and are forced to DISCARD the buffer
            ' and start over at the beginning.
            vertexBuffer = New VertexBuffer(GetType(PointVertex), discard, dev, Usage.Dynamic Or Usage.WriteOnly Or Usage.Points, PointVertex.Format, Pool.Default)
        End Sub 'RestoreDeviceObjects

        Public Sub Update(ByVal fSecsPerFrame As Single, ByVal NumParticlesToEmit As Integer, ByVal clrEmitColor As System.Drawing.Color, ByVal clrFadeColor As System.Drawing.Color, ByVal fEmitVel As Single, ByVal vPosition As Vector3)
            time += fSecsPerFrame
            Dim ii As Integer
            For ii = particlesList.Count - 1 To 0 Step -1
                Dim p As Particle = CType(particlesList(ii), Particle)
                ' Calculate new position
                Dim fT As Single = time - p.creationTime
                Dim fGravity As Single

                If p.isSpark Then
                    fGravity = -5.0F
                    p.fadeProgression -= fSecsPerFrame * 2.25F
                Else
                    fGravity = -9.8F
                    p.fadeProgression -= fSecsPerFrame * 0.25F
                End If

                p.positionVector = Vector3.Add(Vector3.Multiply(p.initialVelocity, fT), p.initialPosition)
                p.positionVector.Y += 0.5F * fGravity * (fT * fT)
                p.velocityVector.Y = p.initialVelocity.Y + fGravity * fT

                If p.fadeProgression < 0.0F Then
                    p.fadeProgression = 0.0F
                End If
                ' Kill old particles
                If p.positionVector.Y < radius Or (p.isSpark And p.fadeProgression <= 0.0F) Then
                    ' Emit sparks
                    If Not p.isSpark Then
                        Dim i As Integer
                        For i = 0 To 3
                            Dim spark As Particle

                            If freeParticles.Count > 0 Then
                                spark = CType(freeParticles(0), Particle)
                                freeParticles.RemoveAt(0)
                            Else
                                spark = New Particle()
                            End If

                            spark.isSpark = True
                            spark.initialVelocity = New Vector3()
                            spark.initialPosition = p.positionVector
                            spark.initialPosition.Y = radius

                            Dim fRand1 As Single = CSng(rand.Next(Integer.MaxValue)) / CSng(Integer.MaxValue) * CSng(Math.PI) * 2.0F
                            Dim fRand2 As Single = CSng(rand.Next(Integer.MaxValue)) / CSng(Integer.MaxValue) * CSng(Math.PI) * 0.25F

                            spark.initialVelocity.X = p.velocityVector.X * 0.25F + CSng(Math.Cos(fRand1)) * CSng(Math.Sin(fRand2))
                            spark.initialVelocity.Z = p.velocityVector.Z * 0.25F + CSng(Math.Sin(fRand1)) * CSng(Math.Sin(fRand2))
                            spark.initialVelocity.Y = CSng(Math.Cos(fRand2))
                            spark.initialVelocity.Y *= CSng(rand.Next(Integer.MaxValue)) / CSng(Integer.MaxValue) * 1.5F

                            spark.positionVector = spark.initialPosition
                            spark.velocityVector = spark.initialVelocity

                            spark.diffuseColor = ColorOperator.Lerp(p.fadeColor, p.diffuseColor, p.fadeProgression)
                            spark.fadeColor = System.Drawing.Color.Black
                            spark.fadeProgression = 1.0F
                            spark.creationTime = time

                            particlesList.Add(spark)
                        Next i
                    End If

                    ' Kill particle
                    freeParticles.Add(p)
                    particlesList.RemoveAt(ii)

                    If Not p.isSpark Then
                        particles -= 1
                    End If
                Else
                    particlesList(ii) = p
                End If
            Next ii ' Emit new particles
            Dim particlesEmit As Integer = particles + NumParticlesToEmit
            While particles < particlesLimit And particles < particlesEmit
                Dim particle As Particle

                If freeParticles.Count > 0 Then
                    particle = CType(freeParticles(0), Particle)
                    freeParticles.RemoveAt(0)
                Else
                    particle = New Particle()
                End If

                ' Emit new particle
                Dim fRand1 As Single = CSng(rand.Next(Integer.MaxValue)) / CSng(Integer.MaxValue) * CSng(Math.PI) * 2.0F
                Dim fRand2 As Single = CSng(rand.Next(Integer.MaxValue)) / CSng(Integer.MaxValue) * CSng(Math.PI) * 0.25F

                particle.isSpark = False

                particle.initialPosition = Vector3.Add(vPosition, New Vector3(0.0F, radius, 0.0F))

                particle.initialVelocity.X = CSng(Math.Cos(fRand1)) * CSng(Math.Sin(fRand2)) * 2.5F
                particle.initialVelocity.Z = CSng(Math.Sin(fRand1)) * CSng(Math.Sin(fRand2)) * 2.5F
                particle.initialVelocity.Y = CSng(Math.Cos(fRand2))
                particle.initialVelocity.Y *= CSng(rand.Next(Integer.MaxValue)) / CSng(Integer.MaxValue) * fEmitVel

                particle.positionVector = particle.initialPosition
                particle.velocityVector = particle.initialVelocity

                particle.diffuseColor = clrEmitColor
                particle.fadeColor = clrFadeColor
                particle.fadeProgression = 1.0F
                particle.creationTime = time

                particlesList.Add(particle)
                particles += 1
            End While
        End Sub 'Update

        Public Sub Render(ByVal dev As Device)

            ' Set the render states for using point sprites
            dev.RenderState.PointSpriteEnable = True
            dev.RenderState.PointScaleEnable = True
            dev.RenderState.PointSize = 0.08F
            dev.RenderState.PointSizeMin = 0.0F
            dev.RenderState.PointScaleA = 0.0F
            dev.RenderState.PointScaleB = 0.0F
            dev.RenderState.PointScaleC = 1.0F

            ' Set up the vertex buffer to be rendered
            dev.SetStreamSource(0, vertexBuffer, 0)
            dev.VertexFormat = PointVertex.Format

            Dim vertices As PointVertex() = Nothing
            Dim numParticlesToRender As Integer = 0



            ' Lock the vertex buffer.  We fill the vertex buffer in small
            ' chunks, using LockFlags.NoOverWrite.  When we are done filling
            ' each chunk, we call DrawPrim, and lock the next chunk.  When
            ' we run out of space in the vertex buffer, we start over at
            ' the beginning, using LockFlags.Discard.
            baseParticle += flush

            If baseParticle >= discard Then
                baseParticle = 0
            End If
            Dim count As Integer = 0
            vertices = CType(vertexBuffer.Lock(baseParticle * DXHelp.GetTypeSize(GetType(PointVertex)), GetType(PointVertex), IIf(baseParticle <> 0, LockFlags.NoOverWrite, LockFlags.Discard), flush), PointVertex())
            Dim p As Particle
            For Each p In particlesList
                Dim vPos As Vector3 = p.positionVector
                Dim vVel As Vector3 = p.velocityVector
                Dim fLengthSq As Single = vVel.LengthSq()
                Dim steps As Integer

                If fLengthSq < 1.0F Then
                    steps = 2
                ElseIf fLengthSq < 4.0F Then
                    steps = 3
                ElseIf fLengthSq < 9.0F Then
                    steps = 4
                ElseIf fLengthSq < 12.25F Then
                    steps = 5
                ElseIf fLengthSq < 16.0F Then
                    steps = 6
                ElseIf fLengthSq < 20.25F Then
                    steps = 7
                Else
                    steps = 8
                End If
                vVel.Multiply(-0.04F / CSng(steps))

                Dim diffuse As System.Drawing.Color = ColorOperator.Lerp(p.fadeColor, p.diffuseColor, p.fadeProgression)

                ' Render each particle a bunch of times to get a blurring effect
                Dim i As Integer
                For i = 0 To steps - 1
                    vertices(count).v = vPos
                    vertices(count).color = diffuse.ToArgb()
                    count += 1

                    numParticlesToRender += 1
                    If numParticlesToRender = flush Then
                        ' Done filling this chunk of the vertex buffer.  Lets unlock and
                        ' draw this portion so we can begin filling the next chunk.
                        vertexBuffer.Unlock()

                        dev.DrawPrimitives(PrimitiveType.PointList, baseParticle, numParticlesToRender)

                        ' Lock the next chunk of the vertex buffer.  If we are at the 
                        ' end of the vertex buffer, LockFlags.Discard the vertex buffer and start
                        ' at the beginning.  Otherwise, specify LockFlags.NoOverWrite, so we can
                        ' continue filling the VB while the previous chunk is drawing.
                        baseParticle += flush

                        If baseParticle >= discard Then
                            baseParticle = 0
                        End If
                        vertices = CType(vertexBuffer.Lock(baseParticle * DXHelp.GetTypeSize(GetType(PointVertex)), GetType(PointVertex), IIf(baseParticle <> 0, LockFlags.NoOverwrite, LockFlags.Discard), flush), PointVertex())
                        count = 0

                        numParticlesToRender = 0
                    End If

                    vPos.Add(vVel)
                Next i
            Next p

            ' Unlock the vertex buffer
            vertexBuffer.Unlock()
            ' Render any remaining particles
            If numParticlesToRender > 0 Then
                dev.DrawPrimitives(PrimitiveType.PointList, baseParticle, numParticlesToRender)
            End If
            ' Reset render states
            dev.RenderState.PointSpriteEnable = False
            dev.RenderState.PointScaleEnable = False
        End Sub 'Render
    End Class 'ParticleSystem
End Namespace 'PointSprites