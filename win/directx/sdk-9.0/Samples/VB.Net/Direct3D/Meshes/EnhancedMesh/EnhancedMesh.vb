'-----------------------------------------------------------------------------
' File: EnhancedMesh.vb
'
' Desc: Sample showing enhanced meshes in D3D
'
' Copyright (c) Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace EnhancedMeshSample
    _
    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample

        ' Menu items specific for this application
        Private mnuOpenMesh As MenuItem = Nothing
        Private mnuMeshBreak As MenuItem = Nothing

        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private initialDirectory As String = Nothing
        Private meshFilename As String = "tiger.x" ' Filename of mesh
        Private systemMemoryMesh As Mesh = Nothing ' system memory version of mesh, lives through resize's
        Private enhancedMesh As Mesh = Nothing ' vid mem version of mesh that is enhanced
        Private numberSegments As Integer ' number of segments per edge (tesselation level)
        Private meshMaterials As Direct3D.Material() = Nothing ' array of materials
        Private meshTextures As Texture() = Nothing ' Array of textures, entries are NULL if no texture specified
        Private arcBall As GraphicsArcBall ' Mouse rotation utility
        Private objectCenter As Vector3 ' Center of bounding sphere of object
        Private objectRadius As Single = 0.0F ' Radius of bounding sphere of object
        Private adjacency As GraphicsStream = Nothing ' Contains the adjacency info loaded with the mesh
        Private isUsingHardwareNPatches As Boolean
        Private isWireframe As Boolean = False





        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Enhanced Mesh - N-Patches"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            arcBall = New GraphicsArcBall(Me)
            initialDirectory = DXUtil.SdkMediaPath
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
            numberSegments = 2

            ' Add our new menu options
            Me.mnuOpenMesh = New MenuItem()
            Me.mnuMeshBreak = New MenuItem()

            ' Add the Open File menu to the file menu
            Me.mnuFile.MenuItems.Add(0, Me.mnuMeshBreak)
            Me.mnuMeshBreak.Text = "-"
            Me.mnuFile.MenuItems.Add(0, Me.mnuOpenMesh)
            Me.mnuOpenMesh.Text = "Open File..."
            Me.mnuOpenMesh.Shortcut = System.Windows.Forms.Shortcut.CtrlO
            Me.mnuOpenMesh.ShowShortcut = True
            AddHandler Me.mnuOpenMesh.Click, AddressOf Me.OpenMesh

            ' Set up our event handlers
            AddHandler Me.KeyDown, AddressOf Me.OnPrivateKeyDown
        End Sub 'New






        '-----------------------------------------------------------------------------
        ' Name: OneTimeSceneInitialization()
        ' Desc: Called during initial app startup, this function performs all the
        '       permanent initialization.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub OneTimeSceneInitialization()
            ' Set cursor to indicate that user can move the object with the mouse
            Me.Cursor = System.Windows.Forms.Cursors.SizeAll
        End Sub 'OneTimeSceneInitialization




        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Setup world matrix
            Dim matWorld As Matrix = Matrix.Translation(-objectCenter.X, -objectCenter.Y, -objectCenter.Z)
            matWorld.Multiply(arcBall.RotationMatrix)
            matWorld.Multiply(arcBall.TranslationMatrix)
            device.Transform.World = matWorld

            ' Set up view matrix
            Dim vFrom As New Vector3(0, 0, -3 * objectRadius)
            Dim vAt As New Vector3(0, 0, 0)
            Dim vUp As New Vector3(0, 1, 0)
            device.Transform.View = Matrix.LookAtLH(vFrom, vAt, vUp)
        End Sub 'FrameMove




        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)

            Device.BeginScene()
            If isUsingHardwareNPatches Then
                Dim fNumSegs As Single

                fNumSegs = CSng(numberSegments)
                Device.NPatchMode = fNumSegs
            End If

            ' set and draw each of the materials in the mesh
            Dim i As Integer
            For i = 0 To meshMaterials.Length - 1
                Device.Material = meshMaterials(i)
                Device.SetTexture(0, meshTextures(i))

                enhancedMesh.DrawSubset(i)
            Next i

            If isUsingHardwareNPatches Then
                Device.NPatchMode = 0.0F
            End If

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            ' Display info on mesh
            drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, "Number Segments: ")
            drawingFont.DrawText(150, 40, System.Drawing.Color.White, numberSegments.ToString())

            drawingFont.DrawText(2, 60, System.Drawing.Color.Yellow, "Number Faces: ")
            drawingFont.DrawText(150, 60, System.Drawing.Color.White, IIf(enhancedMesh Is Nothing, "0", enhancedMesh.NumberFaces.ToString()))

            drawingFont.DrawText(2, 80, System.Drawing.Color.Yellow, "Number Vertices: ")
            drawingFont.DrawText(150, 80, System.Drawing.Color.White, IIf(enhancedMesh Is Nothing, "0", enhancedMesh.NumberVertices.ToString()))

            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize the drawingFont's internal textures
            drawingFont.InitializeDeviceObjects(Device)

            Dim mtrl As ExtendedMaterial() = Nothing
            Dim sPath As String = DXUtil.FindMediaFile(initialDirectory, meshFilename)
            Try
                systemMemoryMesh = Mesh.FromFile(sPath, MeshFlags.SystemMemory, Device, adjacency, mtrl)
            Catch
                ' Hide the error so we display a blue screen
                Return
            End Try

            ' Lock the vertex buffer, to generate a simple bounding sphere
            Dim vb As VertexBuffer = Nothing
            Try
                vb = systemMemoryMesh.VertexBuffer
                Dim vbStream As GraphicsStream = vb.Lock(0, 0, 0)
                objectRadius = Geometry.ComputeBoundingSphere(vbStream, systemMemoryMesh.NumberVertices, systemMemoryMesh.VertexFormat, objectCenter)
            Finally
                ' Make sure we unlock the buffer if we fail
                If Not (vb Is Nothing) Then
                    vb.Unlock()
                End If
            End Try
            meshMaterials = New Material(mtrl.Length) {}
            meshTextures = New Texture(mtrl.Length) {}
            Dim i As Integer
            For i = 0 To mtrl.Length - 1
                meshMaterials(i) = mtrl(i).Material3D
                If Not (mtrl(i).TextureFilename Is Nothing) And mtrl(i).TextureFilename <> String.Empty Then
                    meshTextures(i) = TextureLoader.FromFile(Device, DXUtil.FindMediaFile(Nothing, mtrl(i).TextureFilename))
                End If
            Next i ' Make sure there are normals, which are required for the tesselation
            ' enhancement
            If (systemMemoryMesh.VertexFormat And VertexFormats.Normal) <> VertexFormats.Normal Then
                Dim tempMesh As Mesh = systemMemoryMesh.Clone(systemMemoryMesh.Options.Value, systemMemoryMesh.VertexFormat Or VertexFormats.Normal, Device)

                tempMesh.ComputeNormals()
                systemMemoryMesh.Dispose()
                systemMemoryMesh = tempMesh
            End If
        End Sub 'InitializeDeviceObjects





        '-----------------------------------------------------------------------------
        ' Name: GenerateEnhancedMesh()
        ' Desc: Generates the enhanced mesh
        '-----------------------------------------------------------------------------
        Sub GenerateEnhancedMesh(ByVal newNumberSegs As Integer)
            Dim meshEnhancedSysMem As Mesh = Nothing
            Dim meshTemp As Mesh = Nothing

            If systemMemoryMesh Is Nothing Then
                Return
            End If
            ' if using hw, just copy the mesh
            If isUsingHardwareNPatches Then
                meshTemp = systemMemoryMesh.Clone(MeshFlags.WriteOnly Or MeshFlags.NPatches Or (systemMemoryMesh.Options.Value And MeshFlags.Use32Bit), systemMemoryMesh.VertexFormat, Device)
                ' tesselate the mesh in sw
            Else

                ' Create an enhanced version of the mesh, will be in sysmem since source is
                Try
                    meshEnhancedSysMem = Mesh.TessellateNPatches(systemMemoryMesh, adjacency, newNumberSegs, False)
                Catch
                    ' If the tessellate failed, there might have been more triangles or vertices 
                    ' than can fit into a 16bit mesh, so try cloning to 32bit before tessellation
                    meshTemp = systemMemoryMesh.Clone(MeshFlags.SystemMemory Or MeshFlags.Use32Bit, systemMemoryMesh.VertexFormat, Device)
                    meshEnhancedSysMem = Mesh.TessellateNPatches(meshTemp, adjacency, newNumberSegs, False)
                    meshTemp.Dispose()
                End Try

                ' Make a video memory version of the mesh  
                ' Only set WriteOnly if it doesn't use 32bit indices, because those 
                ' often need to be emulated, which means that D3DX needs read-access.
                Dim meshEnhancedFlags As MeshFlags = meshEnhancedSysMem.Options.Value And MeshFlags.Use32Bit
                If (meshEnhancedFlags And MeshFlags.Use32Bit) <> MeshFlags.Use32Bit Then
                    meshEnhancedFlags = meshEnhancedFlags Or MeshFlags.WriteOnly
                End If
                meshTemp = meshEnhancedSysMem.Clone(meshEnhancedFlags, systemMemoryMesh.VertexFormat, Device)
            End If

            enhancedMesh = meshTemp
            numberSegments = newNumberSegs
        End Sub 'GenerateEnhancedMesh




        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            isUsingHardwareNPatches = Caps.DeviceCaps.SupportsNPatches ' Do we have hardware support?
            GenerateEnhancedMesh(numberSegments)

            ' Setup render state
            RenderState.Lighting = True
            RenderState.DitherEnable = True
            RenderState.ZBufferEnable = True
            RenderState.Ambient = System.Drawing.Color.FromArgb(&H33333333)
            sampleState(0).MagFilter = TextureFilter.Linear
            sampleState(0).MinFilter = TextureFilter.Linear

            ' Setup the light
            GraphicsUtility.InitLight(Device.Lights(0), LightType.Directional, 0.0F, -1.0F, 1.0F)
            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True

            ' Set the arcball parameters
            arcBall.SetWindow(Device.PresentationParameters.BackBufferWidth, Device.PresentationParameters.BackBufferHeight, 0.85F)
            arcBall.Radius = 1.0F

            ' Set the projection matrix
            Dim fAspect As Single = device.PresentationParameters.BackBufferWidth / CSng(device.PresentationParameters.BackBufferHeight)
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, objectRadius / 64.0F, objectRadius * 200.0F)

            If isWireframe Then
                Device.RenderState.FillMode = FillMode.WireFrame
            Else
                Device.RenderState.FillMode = FillMode.Solid
            End If
        End Sub 'RestoreDeviceObjects




        '/ <summary>
        '/ Event Handler for windows messages
        '/ </summary>

        Private Sub OnPrivateKeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs)
            If e.KeyCode = System.Windows.Forms.Keys.Up Then
                GenerateEnhancedMesh((numberSegments + 1))
            End If
            If e.KeyCode = System.Windows.Forms.Keys.Down Then
                GenerateEnhancedMesh(Math.Max(1, numberSegments - 1))
            End If
            If e.KeyCode = System.Windows.Forms.Keys.W Then
                Device.RenderState.FillMode = FillMode.WireFrame
                isWireframe = True
            End If
            If e.KeyCode = System.Windows.Forms.Keys.S Then
                Device.RenderState.FillMode = FillMode.Solid
                isWireframe = False
            End If
        End Sub 'OnPrivateKeyDown




        '/ <summary>
        '/ Fired when a new mesh needs to be opened
        '/ </summary>
        Private Sub OpenMesh(ByVal sender As Object, ByVal e As EventArgs)
            ' Display the OpenFileName dialog. Then, try to load the specified file
            Dim ofn = New OpenFileDialog()
            ofn.Filter = ".X Files (*.x)|*.x"
            ofn.FileName = meshFilename
            ofn.InitialDirectory = initialDirectory
            ofn.Title = "Open Mesh File"
            ofn.CheckFileExists = True
            ofn.Multiselect = False
            ofn.ShowReadOnly = False
            ofn.ShowDialog()
            Dim fo As New System.IO.FileInfo(ofn.FileName)
            meshFilename = fo.Name
            System.IO.Directory.SetCurrentDirectory(fo.DirectoryName)
            initialDirectory = fo.DirectoryName

            ' Destroy and recreate everything
            InvalidateDeviceObjects(Nothing, Nothing)
            DeleteDeviceObjects(Nothing, Nothing)
            Try
                InitializeDeviceObjects()
                RestoreDeviceObjects(Nothing, Nothing)
            Catch
                MessageBox.Show("Error loading mesh: mesh may not" + ControlChars.Lf + "be valid. See debug output for" + ControlChars.Lf + "more information." + ControlChars.Lf + ControlChars.Lf + "Please select " + "a different .x file.", "EnhancedMesh", MessageBoxButtons.OK, MessageBoxIcon.Error)
            End Try
        End Sub 'OpenMesh





        '-----------------------------------------------------------------------------
        ' Name: ConfirmDevice()
        ' Desc: Called during device intialization, this code checks the device
        '       for some minimum set of capabilities
        '-----------------------------------------------------------------------------
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If vertexProcessingType = vertexProcessingType.PureHardware And Not caps.DeviceCaps.SupportsNPatches And caps.DeviceCaps.SupportsRtPatches Then
                Return False
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
End Namespace 'EnhancedMeshSample