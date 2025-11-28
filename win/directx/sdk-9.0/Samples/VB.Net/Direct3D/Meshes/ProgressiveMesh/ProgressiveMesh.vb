'-----------------------------------------------------------------------------
' File: ProgressiveMesh.vb
'
' Desc: Sample of creating progressive meshes in D3D
'
' Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace ProgressiveMeshSample
    _
    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample

        ' Menu items specific for this application
        Private mnuOptions As MenuItem = Nothing
        Private WithEvents mnuOptimize As MenuItem = Nothing
        Private WithEvents mnuOpenMesh As MenuItem = Nothing
        Private mnuMeshBreak As MenuItem = Nothing

        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private initialDirectory As String = Nothing
        Private meshFilename As String = "tiger.x" ' Filename of mesh
        Private pmeshes As ProgressiveMesh() = Nothing
        Private fullPmesh As ProgressiveMesh = Nothing
        Private currentPmesh As Integer = 0

        Private meshMaterials As Direct3D.Material() = Nothing
        Private meshTextures As Texture() = Nothing ' Array of textures, entries are NULL if no texture specified
        Private arcBall As GraphicsArcBall ' Mouse rotation utility
        Private objectCenter As Vector3 ' Center of bounding sphere of object
        Private objectRadius As Single = 0.0F ' Radius of bounding sphere of object
        Private displayHelp As Boolean = False
        Private showOptimized As Boolean = True

        Private initDone As Boolean = False
        ' hold off on any reaction to messages until fully inited


        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "ProgressiveMesh: Using Progressive Meshes in D3D"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            arcBall = New GraphicsArcBall(Me)
            initialDirectory = DXUtil.SdkMediaPath
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True

            ' Add our new menu options
            Me.mnuOptions = New MenuItem()
            Me.mnuOptimize = New MenuItem()
            Me.mnuOpenMesh = New MenuItem()
            Me.mnuMeshBreak = New MenuItem()

            ' Add the Options menu to the main menu
            Me.mnuMain.MenuItems.Add(Me.mnuOptions)
            Me.mnuOptions.Index = 1
            Me.mnuOptions.Text = "&Options"

            ' Add the optimize menu to the options menu
            Me.mnuOptions.MenuItems.Add(Me.mnuOptimize)
            Me.mnuOptimize.Text = "Show Optimized PMeshes 'o'"

            ' Add the 'Open Mesh' dialog to the file menu.
            Me.mnuFile.MenuItems.Add(0, mnuMeshBreak)
            Me.mnuFile.MenuItems.Add(0, mnuOpenMesh)
            Me.mnuMeshBreak.Text = "-"
            Me.mnuOpenMesh.Text = "Open File..."
            Me.mnuOpenMesh.Shortcut = System.Windows.Forms.Shortcut.CtrlO
            Me.mnuOpenMesh.ShowShortcut = True

            mnuOptimize.Checked = showOptimized

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
            Device.Transform.World = matWorld

            ' Set up view matrix
            Dim vFrom As New Vector3(0, 0, -3 * objectRadius)
            Dim vAt As New Vector3(0, 0, 0)
            Dim vUp As New Vector3(0, 1, 0)
            Device.Transform.View = Matrix.LookAtLH(vFrom, vAt, vUp)
        End Sub 'FrameMove




        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            If Not initDone Then
                Return
            End If
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)

            Device.BeginScene()

            If Not (pmeshes Is Nothing) Then
                ' Set and draw each of the materials in the mesh
                Dim i As Integer
                For i = 0 To meshMaterials.Length - 1
                    Device.Material = meshMaterials(i)
                    Device.SetTexture(0, meshTextures(i))
                    If showOptimized Then
                        pmeshes(currentPmesh).DrawSubset(i)
                    Else
                        fullPmesh.DrawSubset(i)
                    End If
                Next i
            End If
            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            If showOptimized Then
                If pmeshes Is Nothing Then
                    drawingFont.DrawText(2, 60, System.Drawing.Color.Yellow, "Unoptimized")
                    drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, String.Format("Num Vertices = {0}, Optimized {1}", 0, 0.0F))
                Else
                    drawingFont.DrawText(2, 60, System.Drawing.Color.Yellow, String.Format("PMesh {0} out of {1}", currentPmesh + 1, pmeshes.Length))

                    drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, String.Format("Num Vertices = {0}, Min = {1}, Max = {2}", CType(pmeshes(currentPmesh), BaseMesh).NumberVertices, pmeshes(currentPmesh).MinVertices, pmeshes(currentPmesh).MaxVertices))
                End If
            Else
                drawingFont.DrawText(2, 60, System.Drawing.Color.Yellow, "Unoptimized")
                drawingFont.DrawText(2, 40, System.Drawing.Color.Yellow, String.Format("Num Vertices = {0}, Min = {1}, Max = {2}", CType(fullPmesh, BaseMesh).NumberVertices, fullPmesh.MinVertices, fullPmesh.MaxVertices))
            End If

            ' Output text
            If displayHelp Then
                drawingFont.DrawText(2, 80, System.Drawing.Color.White, "<F1>" + ControlChars.Lf + ControlChars.Lf + "<Up>" + ControlChars.Lf + "<Down>" + ControlChars.Lf + ControlChars.Lf + "<PgUp>" + ControlChars.Lf + "<PgDn>" + ControlChars.Lf + ControlChars.Lf + "<Home>" + ControlChars.Lf + "<End>" + ControlChars.Lf + "<o>", 0)
                drawingFont.DrawText(70, 80, System.Drawing.Color.White, "Toggle help" + ControlChars.Lf + ControlChars.Lf + "Add one vertex" + ControlChars.Lf + "Subtract one vertex" + ControlChars.Lf + ControlChars.Lf + "Add 100 vertices" + ControlChars.Lf + "Subtract 100 vertices" + ControlChars.Lf + ControlChars.Lf + "Max vertices" + ControlChars.Lf + "Min vertices" + ControlChars.Lf + "Show optimized pmeshes")
            Else
                drawingFont.DrawText(2, 80, System.Drawing.Color.White, "<F1>" + ControlChars.Lf)
                drawingFont.DrawText(70, 80, System.Drawing.Color.White, "Toggle help" + ControlChars.Lf)
            End If


            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize the font's internal textures
            drawingFont.InitializeDeviceObjects(Device)

            Dim sPath As String = DXUtil.FindMediaFile(initialDirectory, meshFilename)
            Dim pMesh As Mesh = Nothing
            Dim pTempMesh As Mesh = Nothing
            Dim adj As GraphicsStream = Nothing
            Dim mtrl As ExtendedMaterial() = Nothing
            Dim i32BitFlag As MeshFlags
            Dim Epsilons As New WeldEpsilons()
            Dim pPMesh As ProgressiveMesh = Nothing
            Dim cVerticesMin As Integer = 0
            Dim cVerticesMax As Integer = 0
            Dim cVerticesPerMesh As Integer = 0

            Try
                ' Load the mesh from the specified file
                pMesh = Mesh.FromFile(sPath, MeshFlags.Managed, Device, adj, mtrl)
                i32BitFlag = IIf(pMesh.Options.Use32Bit, MeshFlags.Use32Bit, 0)

                ' perform simple cleansing operations on mesh
                pTempMesh = Mesh.Clean(pMesh, adj, adj)
                pMesh.Dispose()
                pMesh = pTempMesh

                '  Perform a weld to try and remove excess vertices like the model bigship1.x in the DX9.0 SDK (current model is fixed)
                '    Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
                pMesh.WeldVertices(0, Epsilons, adj, adj)
                ' verify validity of mesh for simplification
                pMesh.Validate(adj)

                meshMaterials = New Direct3D.Material(mtrl.Length) {}
                meshTextures = New Texture(mtrl.Length) {}
                Dim i As Integer
                For i = 0 To mtrl.Length - 1
                    meshMaterials(i) = mtrl(i).Material3D
                    meshMaterials(i).Ambient = meshMaterials(i).Diffuse
                    If Not (mtrl(i).TextureFilename Is Nothing) And mtrl(i).TextureFilename <> "" Then
                        sPath = DXUtil.FindMediaFile(initialDirectory, mtrl(i).TextureFilename)
                        ' Find the path to the texture and create that texture
                        Try
                            meshTextures(i) = TextureLoader.FromFile(Device, sPath)
                        Catch
                            meshTextures(i) = Nothing
                        End Try
                    End If
                Next i

                ' Lock the vertex buffer to generate a simple bounding sphere
                Dim vb As VertexBuffer = pMesh.VertexBuffer
                Dim vertexData As GraphicsStream = vb.Lock(0, 0, LockFlags.NoSystemLock)
                objectRadius = Geometry.ComputeBoundingSphere(vertexData, pMesh.NumberVertices, pMesh.VertexFormat, objectCenter)
                vb.Unlock()
                vb.Dispose()
                If meshMaterials.Length = 0 Then
                    Throw New Exception()
                End If
                If (pMesh.VertexFormat And VertexFormats.Normal) = 0 Then
                    pTempMesh = pMesh.Clone(i32BitFlag Or MeshFlags.Managed, pMesh.VertexFormat Or VertexFormats.Normal, Device)
                    pTempMesh.ComputeNormals()
                    pMesh.Dispose()
                    pMesh = pTempMesh
                End If
                pPMesh = New ProgressiveMesh(pMesh, adj, Nothing, 1, MeshFlags.SimplifyVertex)

                cVerticesMin = pPMesh.MinVertices
                cVerticesMax = pPMesh.MaxVertices

                cVerticesPerMesh = (cVerticesMax - cVerticesMin) / 10
                pmeshes = New ProgressiveMesh(CInt(Math.Max(1, Math.Ceiling(((cVerticesMax - cVerticesMin) / CSng(cVerticesPerMesh)))))) {}

                ' clone full size pmesh
                fullPmesh = pPMesh.Clone(MeshFlags.Managed Or MeshFlags.VbShare, pPMesh.VertexFormat, Device)

                ' clone all the separate pmeshes
                Dim iPMesh As Integer
                For iPMesh = 0 To pmeshes.Length - 1
                    pmeshes(iPMesh) = pPMesh.Clone(MeshFlags.Managed Or MeshFlags.VbShare, pPMesh.VertexFormat, Device)
                    ' trim to appropriate space
                    pmeshes(iPMesh).TrimByVertices(cVerticesMin + cVerticesPerMesh * iPMesh, cVerticesMin + cVerticesPerMesh * (iPMesh + 1))

                    pmeshes(iPMesh).OptimizeBaseLevelOfDetail(MeshFlags.OptimizeVertexCache)
                Next iPMesh
                currentPmesh = pmeshes.Length - 1
                pmeshes(currentPmesh).NumberVertices = cVerticesMax
                fullPmesh.NumberVertices = cVerticesMax
                pPMesh.Dispose()
            Catch
                ' hide error so that device changes will not cause exit, shows blank screen instead
                Return
            End Try
        End Sub 'InitializeDeviceObjects




        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Setup render state
            RenderState.Lighting = True
            RenderState.DitherEnable = True
            RenderState.ZBufferEnable = True
            sampleState(0).MagFilter = TextureFilter.Linear
            sampleState(0).MinFilter = TextureFilter.Linear

            ' Setup the light
            GraphicsUtility.InitLight(Device.Lights(0), LightType.Directional, 0.0F, -0.5F, 1.0F)
            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True
            RenderState.Ambient = System.Drawing.Color.FromArgb(&H333333)

            ' Set the arcball parameters
            arcBall.SetWindow(Device.PresentationParameters.BackBufferWidth, Device.PresentationParameters.BackBufferHeight, 0.85F)
            arcBall.Radius = objectRadius

            ' Set the projection matrix
            Dim fAspect As Single = Device.PresentationParameters.BackBufferWidth / CSng(Device.PresentationParameters.BackBufferHeight)
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, objectRadius / 64.0F, objectRadius * 200.0F)

            initDone = True
        End Sub 'RestoreDeviceObjects




        '/ <summary>
        '/ Event Handler for windows messages
        '/ </summary>
        Private Sub OnPrivateKeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
            If e.KeyCode = System.Windows.Forms.Keys.F1 Then
                displayHelp = Not displayHelp
            End If
            If e.KeyCode = System.Windows.Forms.Keys.O Then
                OptimizeClick(Nothing, Nothing)
            End If
        End Sub 'OnPrivateKeyUp

        Private Sub OnPrivateKeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
            If Not (pmeshes Is Nothing) Then
                Dim numMeshVertices As Integer = CType(CType(pmeshes(currentPmesh), BaseMesh).NumberVertices, Integer)
                If e.KeyCode = System.Windows.Forms.Keys.Up Then
                    ' Sometimes it is necessary to add more than one
                    ' vertex when increasing the resolution of a 
                    ' progressive mesh, so keep adding until the 
                    ' vertex count increases.
                    Dim i As Integer
                    For i = 1 To 8
                        SetNumVertices(CType(numMeshVertices + i, Integer))
                        If CType(pmeshes(currentPmesh), BaseMesh).NumberVertices = numMeshVertices + i Then
                            Exit For
                        End If
                    Next i
                ElseIf e.KeyCode = System.Windows.Forms.Keys.Down Then
                    SetNumVertices((numMeshVertices - 1))
                ElseIf e.KeyCode = System.Windows.Forms.Keys.Prior Then
                    SetNumVertices((numMeshVertices + 100))
                ElseIf e.KeyCode = System.Windows.Forms.Keys.Next Then
                    SetNumVertices(IIf(numMeshVertices <= 100, 1, numMeshVertices - 100))
                ElseIf e.KeyCode = System.Windows.Forms.Keys.Home Then
                    SetNumVertices(Integer.MaxValue)
                ElseIf e.KeyCode = System.Windows.Forms.Keys.End Then
                    SetNumVertices(1)
                End If
            End If
        End Sub 'OnPrivateKeyDown






        '-----------------------------------------------------------------------------
        ' Name: SetNumVertices()
        ' Desc: Sets the number of vertices to display on the current progressive mesh
        '-----------------------------------------------------------------------------
        Sub SetNumVertices(ByVal numVertices As Integer)
            fullPmesh.NumberVertices = CInt(numVertices)

            ' if current pm valid for desired value, then set the number of vertices directly
            If numVertices >= pmeshes(currentPmesh).MinVertices And numVertices <= pmeshes(currentPmesh).MaxVertices Then
                pmeshes(currentPmesh).NumberVertices = CInt(numVertices)
                ' search for the right one
            Else
                currentPmesh = pmeshes.Length - 1

                ' look for the correct "bin" 
                While currentPmesh > 0
                    ' if number of vertices is less than current max then we found one to fit
                    If numVertices >= pmeshes(currentPmesh).MinVertices Then
                        Exit While
                    End If
                    currentPmesh -= 1
                End While

                ' set the vertices on the newly selected mesh
                pmeshes(currentPmesh).NumberVertices = CInt(numVertices)
            End If
        End Sub 'SetNumVertices




        '/ <summary>
        '/ Fired when the 'Optimize' option is clicked
        '/ </summary>
        Private Sub OptimizeClick(ByVal sender As Object, ByVal e As EventArgs) Handles mnuOptimize.Click
            showOptimized = Not showOptimized
            mnuOptimize.Checked = showOptimized
        End Sub 'OptimizeClick




        '/ <summary>
        '/ Fired when a new mesh needs to be opened
        '/ </summary>
        Private Sub OpenMesh(ByVal sender As Object, ByVal e As EventArgs) Handles mnuOpenMesh.Click
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
                MessageBox.Show("Error loading mesh: mesh may not" + ControlChars.Lf + "be valid. See debug output for" + ControlChars.Lf + "more information." + ControlChars.Lf + ControlChars.Lf + "Please select " + "a different .x file.", "ProgressiveMesh", MessageBoxButtons.OK, MessageBoxIcon.Error)
            End Try
        End Sub 'OpenMesh




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
End Namespace 'ProgressiveMeshSample