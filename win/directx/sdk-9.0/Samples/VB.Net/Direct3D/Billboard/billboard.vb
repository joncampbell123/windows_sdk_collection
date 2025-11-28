'-----------------------------------------------------------------------------
' File: Billboard.vb
'
' Desc: Example code showing how to do billboarding. The sample uses
'       billboarding to draw some trees.
'
'       Note: This implementation is for billboards that are fixed to rotate
'       about the Y-axis, which is good for things like trees. For
'       unconstrained billboards, like explosions in a flight sim, the
'       technique is the same, but the the billboards are positioned slightly
'       differently. Try using the inverse of the view matrix, TL-vertices, or
'       some other technique.
'
'       Note: This code uses the D3D Framework helper library.
'
' Copyright (c) 1995-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D

Namespace Billboard
    _

    '/ <summary>
    '/ Simple structure to hold data for rendering a tree
    '/ </summary>
    Public Structure Tree
        Public v0, v1, v2, v3 As CustomVertex.PositionColoredTextured ' Four corners of billboard quad
        Public position As Vector3 ' Origin of tree
        Public treeTextureIndex As Integer ' Which texture map to use
        Public offsetIndex As Integer ' Offset into vertex buffer of tree's vertices
    End Structure 'Tree
    _

    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
        Private NumberTrees As Integer = 500
        Public Shared globalDirection As Vector3

        ' Tree textures to use
        Private Shared treeTextureFileNames() As String = {"Tree02S.dds", "Tree35S.dds", "Tree01S.dds"}
        Private Const numTreeTextures = 3

        Private terrainMesh As GraphicsMesh = Nothing
        Private skyBoxMesh As GraphicsMesh = Nothing
        Private drawingFont As GraphicsFont = Nothing

        Private treeVertexBuffer As VertexBuffer = Nothing ' Vertex buffer for rendering a tree
        Private treeTextures(numTreeTextures - 1) As Texture ' Tree images
        Private billboardMatrix As Matrix ' Used for billboard orientation
        Private trees As New System.Collections.ArrayList() ' Array of tree info
        ' Vectors defining the camera
        Private eyePart As New Vector3(0.0F, 2.0F, -6.5F)
        Private lookAtPart As New Vector3(0.0F, 0.0F, 0.0F)
        Private upVector As New Vector3(0.0F, 1.0F, 0.0F)





        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Billboard: D3D Billboarding Example"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            skyBoxMesh = New GraphicsMesh()
            terrainMesh = New GraphicsMesh()
            enumerationSettings.AppUsesDepthBuffer = True
        End Sub 'New




        '/ <summary>
        '/ Verifies that the tree at the given index is sufficiently spaced
        '/ from the other trees. If trees are placed too closely, one tree
        '/ can quickly pop in front of the other as the camera angle changes.
        '/ </summary>
        '/ <param name="treeIndex">index of tree we are testing</param>
        '/ <returns>true if valid, false otherwise</returns>
        Function IsTreePositionValid(ByVal pos As Vector3) As Boolean
            If trees.Count < 1 Then
                Return True
            End If
            Dim x As Single = pos.X
            Dim z As Single = pos.Z

            Dim i As Integer
            For i = 0 To trees.Count - 1
                Dim fDeltaX As Double = Math.Abs((x - CType(trees(i), Tree).position.X))
                Dim fDeltaZ As Double = Math.Abs((z - CType(trees(i), Tree).position.Z))

                If 3.0 > Math.Pow(fDeltaX, 2) + Math.Pow(fDeltaZ, 2) Then
                    Return False
                End If
            Next i
            Return True
        End Function 'IsTreePositionValid




        '/ <summary>
        '/ Called during initial app startup, this function performs all the
        '/ permanent initialization.
        '/ </summary>
        Protected Overrides Sub OneTimeSceneInitialization()
            Dim rand As New System.Random()
            ' Initialize the tree data
            Dim i As Integer
            For i = 0 To NumberTrees - 1
                Dim t As New Tree()
                ' Position the trees randomly
                Do
                    Dim fTheta As Single = 2.0F * CSng(Math.PI) * CSng(rand.NextDouble())
                    Dim fRadius As Single = 25.0F + 55.0F * CSng(rand.NextDouble())
                    t.position.X = fRadius * CSng(Math.Sin(fTheta))
                    t.position.Z = fRadius * CSng(Math.Cos(fTheta))
                    t.position.Y = HeightField(t.position.X, t.position.Z)
                Loop While Not IsTreePositionValid(t.position)

                ' Size the trees randomly
                Dim fWidth As Single = 1.0F + 0.2F * CSng(rand.NextDouble() - rand.NextDouble())
                Dim fHeight As Single = 1.4F + 0.4F * CSng(rand.NextDouble() - rand.NextDouble())

                ' Each tree is a random color between red and green
                Dim r As Integer = 255 - 190 + CInt(190 * CSng(rand.NextDouble()))
                Dim g As Integer = 255 - 190 + CInt(190 * CSng(rand.NextDouble()))
                Dim b As Integer = 0
                Dim color As Integer = &HFF000000 + DXHelp.ShiftLeft(r, 16) + DXHelp.ShiftLeft(g, 8) + DXHelp.ShiftLeft(b, 0)

                t.v0.SetPosition(New Vector3(-fWidth, 0 * fHeight, 0.0F))
                t.v0.Color = color
                t.v0.Tu = 0.0F
                t.v0.Tv = 1.0F
                t.v1.SetPosition(New Vector3(-fWidth, 2 * fHeight, 0.0F))
                t.v1.Color = color
                t.v1.Tu = 0.0F
                t.v1.Tv = 0.0F
                t.v2.SetPosition(New Vector3(fWidth, 0 * fHeight, 0.0F))
                t.v2.Color = color
                t.v2.Tu = 1.0F
                t.v2.Tv = 1.0F
                t.v3.SetPosition(New Vector3(fWidth, 2 * fHeight, 0.0F))
                t.v3.Color = color
                t.v3.Tu = 1.0F
                t.v3.Tv = 0.0F

                ' Pick a random texture for the tree
                t.treeTextureIndex = CInt(2 * rand.NextDouble())
                trees.Add(t)
            Next i
        End Sub 'OneTimeSceneInitialization



        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Get the eye and lookat points from the camera's path
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim vEyePt As New Vector3()
            Dim vLookatPt As New Vector3()

            vEyePt.X = 30.0F * CSng(Math.Cos((0.8F * appTime)))
            vEyePt.Z = 30.0F * CSng(Math.Sin((0.8F * appTime)))
            vEyePt.Y = 4 + HeightField(vEyePt.X, vEyePt.Z)

            vLookatPt.X = 30.0F * CSng(Math.Cos((0.8F * (appTime + 0.5F))))
            vLookatPt.Z = 30.0F * CSng(Math.Sin((0.8F * (appTime + 0.5F))))
            vLookatPt.Y = vEyePt.Y - 1.0F

            ' Set the app view matrix for normal viewing
            Device.Transform.View = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)

            ' Set up a rotation matrix to orient the billboard towards the camera.
            Dim vDir As Vector3 = Vector3.Subtract(vLookatPt, vEyePt)
            If vDir.X > 0.0F Then
                billboardMatrix = Matrix.RotationY(CSng(-Math.Atan((vDir.Z / vDir.X)) + Math.PI / 2))
            Else
                billboardMatrix = Matrix.RotationY(CSng(-Math.Atan((vDir.Z / vDir.X)) - Math.PI / 2))
            End If
            globalDirection = vDir

            ' Sort trees in back-to-front order
            trees.Sort(New TreeSortClass())

            ' Store vectors
            eyePart = vEyePt
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Black, 1.0F, 0)

            Device.BeginScene()
            ' Center view matrix for skybox and disable zbuffer
            Dim matView, matViewSave As Matrix
            matViewSave = Device.Transform.View
            matView = matViewSave
            matView.M41 = 0.0F
            matView.M42 = -0.3F
            matView.M43 = 0.0F
            Device.Transform.View = matView
            Device.RenderState.ZBufferEnable = False
            ' Some cards do not disable writing to Z when 
            ' D3DRS_ZENABLE is FALSE. So do it explicitly
            Device.RenderState.ZBufferWriteEnable = False

            ' Render the skybox
            skyBoxMesh.Render(Device)

            ' Restore the render states
            Device.Transform.View = matViewSave
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.ZBufferWriteEnable = True

            ' Draw the terrain
            terrainMesh.Render(Device)

            ' Draw the trees
            DrawTrees()

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            Device.EndScene()
        End Sub 'Render



        '/ <summary>
        '/ 
        '/ </summary>
        Protected Sub DrawTrees()
            ' Set diffuse blending for alpha set in vertices.
            renderState.AlphaBlendEnable = True
            renderState.SourceBlend = Blend.SourceAlpha
            renderState.DestinationBlend = Blend.InvSourceAlpha

            ' Enable alpha testing (skips pixels with less than a certain alpha.)
            If Caps.AlphaCompareCaps.SupportsGreaterEqual Then
                Device.RenderState.AlphaTestEnable = True
                Device.RenderState.ReferenceAlpha = &H8
                Device.RenderState.AlphaFunction = [Compare].GreaterEqual
            End If

            ' Loop through and render all trees
            Device.SetStreamSource(0, treeVertexBuffer, 0)
            Device.VertexFormat = CustomVertex.PositionColoredTextured.Format
            Dim t As Tree
            For Each t In trees
                ' Quick culling for trees behind the camera
                ' This calculates the tree position relative to the camera, and
                ' projects that vector against the camera's direction vector. A
                ' negative dot product indicates a non-visible tree.
                If 0 > (t.position.X - eyePart.X) * globalDirection.X + (t.position.Z - eyePart.Z) * globalDirection.Z Then
                    Exit For
                End If
                ' Set the tree texture
                Device.SetTexture(0, treeTextures(t.treeTextureIndex))

                ' Translate the billboard into place
                billboardMatrix.M41 = t.position.X
                billboardMatrix.M42 = t.position.Y
                billboardMatrix.M43 = t.position.Z
                Device.Transform.World = billboardMatrix

                ' Render the billboard
                Device.DrawPrimitives(PrimitiveType.TriangleStrip, t.offsetIndex, 2)
            Next t

            ' Restore state
            Device.Transform.World = Matrix.Identity
            Device.RenderState.AlphaTestEnable = False
            Device.RenderState.AlphaBlendEnable = False
        End Sub 'DrawTrees





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize the font's internal textures
            drawingFont.InitializeDeviceObjects(Device)

            Dim i As Integer
            Try
                ' Create the tree textures
                For i = 0 To treeTextureFileNames.Length - 1
                    treeTextures(i) = GraphicsUtility.CreateTexture(Device, treeTextureFileNames(i))
                Next i
            Catch
                Dim e As MediaNotFoundException = New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try

            Try
                ' Load the skybox
                skyBoxMesh.Create(Device, "SkyBox2.x")

                ' Load the terrain
                terrainMesh.Create(Device, "SeaFloor.x")
            Catch
                Dim e As MediaNotFoundException = New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try

            ' Add some "hilliness" to the terrain
            Dim pVB As VertexBuffer
            pVB = terrainMesh.SysMemMesh.VertexBuffer
            Dim pVertices() As CustomVertex.PositionTextured
            Dim dwNumVertices As Integer = terrainMesh.SysMemMesh.NumberVertices
            pVertices = CType(pVB.Lock(0, GetType(CustomVertex.PositionTextured), 0, dwNumVertices), CustomVertex.PositionTextured())
            For i = 0 To dwNumVertices - 1
                pVertices(i).Y = HeightField(pVertices(i).X, pVertices(i).Z)
            Next i
            pVB.Unlock()
        End Sub 'InitializeDeviceObjects

        Public Sub CreateTreeData(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            ' Copy tree mesh data into vertexbuffer
            Dim v(NumberTrees * 4 - 1) As CustomVertex.PositionColoredTextured
            Dim iTree As Integer
            Dim offsetIndex As Integer = 0
            For iTree = 0 To NumberTrees - 1
                v((offsetIndex + 0)) = CType(trees(iTree), Tree).v0
                v((offsetIndex + 1)) = CType(trees(iTree), Tree).v1
                v((offsetIndex + 2)) = CType(trees(iTree), Tree).v2
                v((offsetIndex + 3)) = CType(trees(iTree), Tree).v3
                Dim t As Tree = CType(trees(iTree), Tree)
                t.offsetIndex = offsetIndex
                trees(iTree) = t
                offsetIndex += 4
            Next iTree
            vb.SetData(v, 0, 0)
        End Sub 'CreateTreeData





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            DirectXException.IgnoreExceptions()
            If (treeVertexBuffer Is Nothing) Then
                ' Create a quad for rendering each tree
                treeVertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionColoredTextured), NumberTrees * 4, Device, Usage.WriteOnly, CustomVertex.PositionColoredTextured.Format, Pool.Default)

                AddHandler treeVertexBuffer.Created, AddressOf Me.CreateTreeData
                Me.CreateTreeData(treeVertexBuffer, Nothing)
            ElseIf (treeVertexBuffer.Disposed) Then
                ' Create a quad for rendering each tree
                treeVertexBuffer = New VertexBuffer(GetType(CustomVertex.PositionColoredTextured), NumberTrees * 4, Device, Usage.WriteOnly, CustomVertex.PositionColoredTextured.Format, Pool.Default)

                AddHandler treeVertexBuffer.Created, AddressOf Me.CreateTreeData
                Me.CreateTreeData(treeVertexBuffer, Nothing)
            End If

            ' Restore the device objects for the meshes and fonts
            terrainMesh.RestoreDeviceObjects(Device, Nothing)
            skyBoxMesh.RestoreDeviceObjects(Device, Nothing)

            ' Set the transform matrices (view and world are updated per frame)
            Dim matProj As Matrix
            Dim fAspect As Single = Device.PresentationParameters.BackBufferWidth / CSng(Device.PresentationParameters.BackBufferHeight)
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 100.0F)
            Device.Transform.Projection = matProj

            ' Set up the default texture states
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).AlphaOperation = TextureOperation.SelectArg1
            Device.TextureState(0).AlphaArgument1 = TextureArgument.TextureColor
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear
            Device.SamplerState(0).MipFilter = TextureFilter.Linear
            Device.SamplerState(0).AddressU = TextureAddress.Clamp
            Device.SamplerState(0).AddressV = TextureAddress.Clamp

            Device.RenderState.DitherEnable = True
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.Lighting = False

            DirectXException.EnableExceptions()
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called when the app is exiting, or the device is being changed, this 
        '/ function deletes any device-dependent objects.
        '/ </summary>
        Protected Overrides Sub DeleteDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            If Not (terrainMesh Is Nothing) Then
                terrainMesh.Dispose()
            End If
            If Not (skyBoxMesh Is Nothing) Then
                skyBoxMesh.Dispose()
            End If
            Dim i As Integer
            For i = 0 To treeTextureFileNames.Length - 1
                If Not (treeTextures(i) Is Nothing) Then
                    treeTextures(i).Dispose()
                End If
            Next i
            If Not (treeVertexBuffer Is Nothing) Then
                treeVertexBuffer.Dispose()
            End If
        End Sub 'DeleteDeviceObjects



        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for alpha blending)
            End If
            If vertexProcessingType = vertexProcessingType.PureHardware Then
                Return False ' GetTransform doesn't work on PUREDEVICE
            End If
            ' This sample uses alpha textures and/or straight alpha. Make sure the
            ' device supports them
            If caps.TextureCaps.SupportsAlphaPalette Then
                Return True
            End If
            If caps.TextureCaps.SupportsAlpha Then
                Return True
            End If
            Return False
        End Function 'ConfirmDevice



        '/ <summary>
        '/ Simple function to define "hilliness" for terrain
        '/ </summary>
        Function HeightField(ByVal x As Single, ByVal y As Single) As Single
            Return 9 * (CSng(Math.Cos((x / 20 + 0.2F))) * CSng(Math.Cos((y / 15 - 0.2F))) + 1.0F)
        End Function 'HeightField




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
    '/ A class to sort our trees in back-to-front order
    '/ </summary>
    Public Class TreeSortClass
        Implements System.Collections.IComparer

        Public Function Compare(ByVal left As Object, ByVal right As Object) As Integer Implements Collections.IComparer.Compare
            Dim l As Tree = CType(left, Tree)
            Dim r As Tree = CType(right, Tree)

            Dim d1 As Single = l.position.X * MyGraphicsSample.globalDirection.X + l.position.Z * MyGraphicsSample.globalDirection.Z
            Dim d2 As Single = r.position.X * MyGraphicsSample.globalDirection.X + r.position.Z * MyGraphicsSample.globalDirection.Z

            If d1 = d2 Then
                Return 0
            End If
            If d1 < d2 Then
                Return +1
            End If
            Return -1
        End Function 'Compare
    End Class 'TreeSortClass
End Namespace 'Billboard