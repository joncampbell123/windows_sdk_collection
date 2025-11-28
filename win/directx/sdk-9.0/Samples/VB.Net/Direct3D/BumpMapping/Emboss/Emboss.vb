'-----------------------------------------------------------------------------
' File: Emboss.vb
'
' Desc: Shows how to do a bumpmapping technique called embossing, in which a
'       heightmap is subtracted from itself, with slightly offset texture
'       coordinates for the second pass.
'
' Copyright (c) Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace BumpWavesSample
    _



    '/ <summary>
    '/ Global Emboss structure
    '/ </summary>
    Public Structure EmbossVertex
        Public p As Vector3
        Public n As Vector3
        Public tu, tv As Single
        Public tu2, tv2 As Single

        Public Const Format As VertexFormats = VertexFormats.PositionNormal Or VertexFormats.Texture2
    End Structure 'EmbossVertex
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
        Private renderObject As GraphicsMesh = Nothing ' Object to render
        Private isShowingEmbossMethod As Boolean = True ' Whether to do the embossing
        Private embossTexture As Texture ' The emboss texture
        Private bumpLightPos As Vector3 ' Light position
        Private tangents As Vector3() = Nothing ' Array of vertex tangents
        Private binormals As Vector3() = Nothing ' Array of vertex binormals
        Private worldMatrix As Matrix ' The world matrix

        ' Menu items for this sample
        Private mnuOptions As System.Windows.Forms.MenuItem
        Private mnuEmboss As System.Windows.Forms.MenuItem


        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Emboss: BumpMapping Technique"
            Try
                ' Load the icon from our resources
                Dim resources As New System.Resources.ResourceManager(Me.GetType())
                Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
            Catch
            End Try
            ' It's no big deal if we can't load our icons

            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True


            ' Add our new menu options
            mnuOptions = New System.Windows.Forms.MenuItem("&Options")
            mnuEmboss = New System.Windows.Forms.MenuItem("Toggle &emboss mode")
            ' Add to the main menu screen
            mnuMain.MenuItems.Add(Me.mnuOptions)
            mnuOptions.MenuItems.Add(mnuEmboss)
            mnuEmboss.Shortcut = System.Windows.Forms.Shortcut.CtrlE
            mnuEmboss.ShowShortcut = True
            AddHandler mnuEmboss.Click, AddressOf Me.EmbossModeChanged
        End Sub 'New





        '/ <summary>
        '/ To find a tangent that heads in the direction of +tv(texcoords), find 
        '/ the components of both vectors on the tangent surface, and add a 
        '/ linear combination of the two projections that head in the +tv direction
        '/ </summary>
        Function ComputeTangentVector(ByVal vertexA As EmbossVertex, ByVal vertexB As EmbossVertex, ByVal vertexC As EmbossVertex) As Vector3
            Dim vAB As Vector3 = Vector3.Subtract(vertexB.p, vertexA.p)
            Dim vAC As Vector3 = Vector3.Subtract(vertexC.p, vertexA.p)
            Dim n As Vector3 = vertexA.n

            ' Components of vectors to neghboring vertices that are orthogonal to the
            ' vertex normal
            Dim vProjAB As Vector3 = Vector3.Subtract(vAB, Vector3.Multiply(n, Vector3.Dot(n, vAB)))
            Dim vProjAC As Vector3 = Vector3.Subtract(vAC, Vector3.Multiply(n, Vector3.Dot(n, vAC)))

            ' tu and tv texture coordinate differences
            Dim duAB As Single = vertexB.tu - vertexA.tu
            Dim duAC As Single = vertexC.tu - vertexA.tu
            Dim dvAB As Single = vertexB.tv - vertexA.tv
            Dim dvAC As Single = vertexC.tv - vertexA.tv

            If duAC * dvAB > duAB * dvAC Then
                duAC = -duAC
                duAB = -duAB
            End If

            Dim vTangent As Vector3 = Vector3.Subtract(Vector3.Multiply(vProjAB, duAC), Vector3.Multiply(vProjAC, duAB))
            vTangent.Normalize()
            Return vTangent
        End Function 'ComputeTangentVector





        '/ <summary>
        '/ Compute the tangents and Binormals
        '/ </summary>
        Sub ComputeTangentsAndBinormals()
            Dim vertices As EmbossVertex() = Nothing
            Dim indices As Short() = Nothing

            Dim numVertices As Integer = 0
            Dim numIndices As Integer = 0

            ' Gain access to the object's vertex and index buffers
            Dim vertexBuffer As VertexBuffer = renderObject.SystemVertexBuffer
            numVertices = renderObject.SysMemMesh.NumberVertices
            vertices = CType(vertexBuffer.Lock(0, GetType(EmbossVertex), 0, numVertices), EmbossVertex())

            Dim indexBuffer As IndexBuffer = renderObject.SystemIndexBuffer
            numIndices = renderObject.SysMemMesh.NumberFaces * 3
            indices = CType(indexBuffer.Lock(0, GetType(Short), 0, numIndices), Short())

            ' Allocate space for the vertices' tangents and binormals
            tangents = New Vector3(numVertices) {}
            binormals = New Vector3(numVertices) {}

            ' Generate the vertices' tangents and binormals
            Dim i As Integer
            For i = 0 To numIndices - 3 Step 3
                Dim a As Short = indices((i + 0))
                Dim b As Short = indices((i + 1))
                Dim c As Short = indices((i + 2))

                ' To find a tangent that heads in the direction of +tv(texcoords),
                ' find the components of both vectors on the tangent surface ,
                ' and add a linear combination of the two projections that head in the +tv direction
                tangents(a) = Vector3.Add(tangents(a), ComputeTangentVector(vertices(a), vertices(b), vertices(c)))
                tangents(b) = Vector3.Add(tangents(b), ComputeTangentVector(vertices(b), vertices(a), vertices(c)))
                tangents(c) = Vector3.Add(tangents(c), ComputeTangentVector(vertices(c), vertices(a), vertices(b)))
            Next i

            For i = 0 To numVertices - 1
                ' Normalize the tangents
                tangents(i).Normalize()

                ' Compute the binormals
                binormals(i) = Vector3.Cross(vertices(i).n, tangents(i))
            Next i

            ' Unlock the buffers
            vertexBuffer.Unlock()
            indexBuffer.Unlock()
        End Sub 'ComputeTangentsAndBinormals





        '/ <summary>
        '/ Performs a calculation on each of the vertices' normals to determine
        '/ what the texture coordinates should be for the environment map (in this 
        '/ case the bump map).
        '/ </summary>
        Sub ApplyEnvironmentMap()
            Dim vertices As EmbossVertex() = Nothing
            Dim numVertices As Integer = 0

            ' Gain access to the object's vertex and index buffers
            Dim vertexBuffer As VertexBuffer = renderObject.LocalVertexBuffer
            numVertices = renderObject.LocalMesh.NumberVertices
            vertices = CType(vertexBuffer.Lock(0, GetType(EmbossVertex), 0, numVertices), EmbossVertex())

            ' Get an inverse world matrix
            Dim invertWorld As Matrix = Matrix.Invert(worldMatrix)

            ' Get the current light position in object space
            Dim transformed As Vector4 = Vector3.Transform(device.Lights(0).Position, invertWorld)
            bumpLightPos.X = transformed.X
            bumpLightPos.Y = transformed.Y
            bumpLightPos.Z = transformed.Z

            ' Dimensions of texture needed for shifting tex coords
            Dim surfacedesc As SurfaceDescription = embossTexture.GetLevelDescription(0)

            ' Loop through the vertices, transforming each one and calculating
            ' the correct texture coordinates.
            Dim i As Short
            For i = 0 To numVertices - 1
                ' Find light vector in tangent space
                Dim vDir As Vector3 = Vector3.Subtract(bumpLightPos, vertices(i).p)
                Dim vLightToVertex As Vector3 = Vector3.Normalize(vDir)

                ' Create rotation matrix (rotate into tangent space)
                Dim r As Single = Vector3.Dot(vLightToVertex, vertices(i).n)

                If r < 0.0F Then
                    ' Don't shift coordinates when light below surface
                    vertices(i).tu2 = vertices(i).tu
                    vertices(i).tv2 = vertices(i).tv
                Else
                    ' Shift coordinates for the emboss effect
                    Dim vEmbossShift As Vector2 = Vector2.Empty
                    vEmbossShift.X = Vector3.Dot(vLightToVertex, tangents(i))
                    vEmbossShift.Y = Vector3.Dot(vLightToVertex, binormals(i))
                    vEmbossShift.Normalize()
                    vertices(i).tu2 = vertices(i).tu + vEmbossShift.X / device.PresentationParameters.BackBufferWidth
                    vertices(i).tv2 = vertices(i).tv - vEmbossShift.Y / device.PresentationParameters.BackBufferHeight
                End If
            Next i

            vertexBuffer.Unlock()
        End Sub 'ApplyEnvironmentMap




        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Rotate the object
            worldMatrix = Matrix.RotationY(appTime)
            Device.Transform.World = worldMatrix
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Black, 1.0F, 0)

            Device.BeginScene()


            ' Stage 0 is the base texture, with the height map in the alpha channel
            Device.SetTexture(0, embossTexture)
            Device.TextureState(0).TextureCoordinateIndex = 0
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).AlphaOperation = TextureOperation.SelectArg1
            Device.TextureState(0).AlphaArgument1 = TextureArgument.TextureColor

            If isShowingEmbossMethod Then
                ' Stage 1 passes through the RGB channels (SelectArg2 = Current), and 
                ' does a signed add with the inverted alpha channel. The texture coords
                ' associated with Stage 1 are the shifted ones, so the result is:
                '    (height - shifted_height) * tex.RGB * diffuse.RGB
                Device.SetTexture(1, embossTexture)
                Device.TextureState(1).TextureCoordinateIndex = 1
                Device.TextureState(1).ColorOperation = TextureOperation.SelectArg2
                Device.TextureState(1).ColorArgument1 = TextureArgument.TextureColor
                Device.TextureState(1).ColorArgument2 = TextureArgument.Current
                Device.TextureState(1).AlphaOperation = TextureOperation.AddSigned
                Device.TextureState(1).AlphaArgument1 = TextureArgument.TextureColor Or TextureArgument.Complement
                Device.TextureState(1).AlphaArgument2 = TextureArgument.Current

                ' Set up the alpha blender to multiply the alpha channel (monochrome emboss)
                ' with the src color (lighted texture)
                Device.RenderState.AlphaBlendEnable = True
                Device.RenderState.SourceBlend = Blend.SourceAlpha
                Device.RenderState.DestinationBlend = Blend.Zero
            End If

            ' Render the object
            renderObject.Render(Device)

            ' Restore render states
            Device.TextureState(1).ColorOperation = TextureOperation.Disable
            Device.TextureState(1).AlphaOperation = TextureOperation.Disable
            Device.RenderState.AlphaBlendEnable = False

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)
            drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, "Move the light with the mouse")
            drawingFont.DrawText(2, 60, System.Drawing.Color.Yellow, "Emboss-mode:")
            drawingFont.DrawText(130, 60, System.Drawing.Color.White, IIf(isShowingEmbossMethod, "ON", "OFF"))

            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
            Try
                ' Load texture map. Note that this is a special texture, which has a
                ' height field stored in the alpha channel
                embossTexture = GraphicsUtility.CreateTexture(Device, "emboss1.dds", Format.A8R8G8B8)

                ' Load geometry
                renderObject = New GraphicsMesh()
                renderObject.Create(Device, "tiger.x")

                ' Set attributes for geometry
                renderObject.SetFVF(Device, EmbossVertex.Format)
                renderObject.UseMeshMaterials = False

                ' Compute the object's tangents and binormals, whaich are needed for the 
                ' emboss-tecnhique's texture-coordinate shifting calculations
                ComputeTangentsAndBinormals()
            Catch
                Dim e As MediaNotFoundException = New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            renderObject.RestoreDeviceObjects(Device, Nothing)
            ' Set up the textures
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 0.0F, 3.5F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)

            Dim matView As Matrix = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            Dim aspect As Single = Device.PresentationParameters.BackBufferWidth / Device.PresentationParameters.BackBufferHeight

            Dim matProj As Matrix = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, aspect, 1.0F, 1000.0F)
            Device.Transform.View = matView
            Device.Transform.Projection = matProj

            ' Setup a material
            Device.Material = GraphicsUtility.InitMaterial(System.Drawing.Color.White)

            ' Setup a light
            GraphicsUtility.InitLight(Device.Lights(0), LightType.Point, 5.0F, 5.0F, -20.0F)
            Device.Lights(0).Attenuation0 = 1.0F
            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True

            ' Set miscellaneous render states
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.Lighting = True
            Device.RenderState.Ambient = System.Drawing.Color.FromArgb(&H444444)

            ApplyEnvironmentMap()
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            ' This sample uses the ADDSIGNED texture blending mode
            If Not caps.TextureOperationCaps.SupportsAddSigned Then
                Return False
            End If
            If caps.MaxTextureBlendStages < 2 Then
                Return False
            End If
            Return True
        End Function 'ConfirmDevice







        '/ <summary>
        '/ Move the light with the mouse
        '/ </summary>
        Protected Overrides Sub OnMouseMove(ByVal e As System.Windows.Forms.MouseEventArgs)
            Dim w As Single = Device.PresentationParameters.BackBufferWidth
            Dim h As Single = Device.PresentationParameters.BackBufferHeight
            Device.Lights(0).XPosition = 200.0F * (0.5F - e.X / w)
            Device.Lights(0).YPosition = 200.0F * (0.5F - e.Y / h)
            Device.Lights(0).ZPosition = 100.0F
            Device.Lights(0).Commit()
            ApplyEnvironmentMap()

            ' Let the control handle the mouse now
            MyBase.OnMouseMove(e)
        End Sub 'OnMouseMove




        '/ <summary>
        '/ Fired when the emboss mode has changed
        '/ </summary>
        Private Sub EmbossModeChanged(ByVal sender As Object, ByVal e As EventArgs)
            isShowingEmbossMethod = Not isShowingEmbossMethod
        End Sub 'EmbossModeChanged





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