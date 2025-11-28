'-----------------------------------------------------------------------------
' File: Meshes.vb
'
' Desc: For advanced geometry, most apps will prefer to load pre-authored
'       meshes from a file. Fortunately, when using meshes, D3DX does most of
'       the work for this, parsing a geometry file and creating vertx buffers
'       (and index buffers) for us. This tutorial shows how to use a D3DXMESH
'       object, including loading it from a file and rendering it. One thing
'       D3DX does not handle for us is the materials and textures for a mesh,
'       so note that we have to handle those manually.
'
'       Note: one advanced (but nice) feature that we don't show here is that
'       when cloning a mesh we can specify the FVF. So, regardless of how the
'       mesh was authored, we can add/remove normals, add more texture
'       coordinate sets (for multi-texturing), etc.
'
' Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.IO
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace MeshesTutorial
    Public Class Meshes
        Inherits Form

        Private device As device = Nothing ' Our rendering device
        Private mesh As mesh = Nothing ' Our mesh object in sysmem
        Private meshMaterials() As Direct3D.Material ' Materials for our mesh
        Private meshTextures() As Texture ' Textures for our mesh
        Private presentParams As New PresentParameters()
        Private pause As Boolean = False


        Public Sub New()
            ' Set the initial size of our form
            Me.ClientSize = New System.Drawing.Size(400, 300)
            ' And it's caption
            Me.Text = "Direct3D Tutorial 6 - Meshes"
        End Sub 'New


        Function InitializeGraphics() As Boolean
            ' Get the current desktop display mode, so we can set up a back
            ' buffer of the same format
            Try
                ' Set up the structure used to create the D3DDevice. Since we are now
                ' using more complex geometry, we will create a device with a zbuffer.
                presentParams.Windowed = True
                presentParams.SwapEffect = SwapEffect.Discard
                presentParams.EnableAutoDepthStencil = True
                presentParams.AutoDepthStencilFormat = DepthFormat.D16

                ' Create the D3DDevice
                device = New Device(0, DeviceType.Hardware, Me, CreateFlags.SoftwareVertexProcessing, presentParams)
                AddHandler device.DeviceReset, AddressOf Me.OnResetDevice
                Me.OnResetDevice(device, Nothing)
                pause = False
            Catch e As DirectXException
                Return False
            End Try
            Return True
        End Function 'InitializeGraphics

        Public Sub OnResetDevice(ByVal sender As Object, ByVal e As EventArgs)
            Dim materials As ExtendedMaterial() = Nothing

            ' Set the directory up two to load the right data (since the default build location is bin\debug or bin\release
            Directory.SetCurrentDirectory((Application.StartupPath + "\..\"))
            Dim dev As Device = CType(sender, Device)
            ' Turn on the zbuffer
            dev.RenderState.ZBufferEnable = True

            ' Turn on ambient lighting 
            dev.RenderState.Ambient = System.Drawing.Color.White
            ' Load the mesh from the specified file
            mesh = mesh.FromFile("tiger.x", MeshFlags.SystemMemory, device, materials)


            If meshTextures Is Nothing Then
                ' We need to extract the material properties and texture names 
                meshTextures = New Texture(materials.Length) {}
                meshMaterials = New Direct3D.Material(materials.Length) {}
                Dim i As Integer
                For i = 0 To materials.Length - 1
                    meshMaterials(i) = materials(i).Material3D
                    ' Set the ambient color for the material (D3DX does not do this)
                    meshMaterials(i).Ambient = meshMaterials(i).Diffuse

                    ' Create the texture
                    meshTextures(i) = TextureLoader.FromFile(dev, materials(i).TextureFilename)
                Next i
            End If
        End Sub 'OnResetDevice

        Sub SetupMatrices()
            ' For our world matrix, we will just leave it as the identity
            device.Transform.World = Matrix.RotationY((Environment.TickCount / 1000.0F))

            ' Set up our view matrix. A view matrix can be defined given an eye point,
            ' a point to lookat, and a direction for which way is up. Here, we set the
            ' eye five units back along the z-axis and up three units, look at the 
            ' origin, and define "up" to be in the y-direction.
            device.Transform.View = Matrix.LookAtLH(New Vector3(0.0F, 3.0F, -5.0F), New Vector3(0.0F, 0.0F, 0.0F), New Vector3(0.0F, 1.0F, 0.0F))

            ' For the projection matrix, we set up a perspective transform (which
            ' transforms geometry from 3D view space to 2D viewport space, with
            ' a perspective divide making objects smaller in the distance). To build
            ' a perpsective transform, we need the field of view (1/4 pi is common),
            ' the aspect ratio, and the near and far clipping planes (which define at
            ' what distances geometry should be no longer be rendered).
            device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI / 4), 1.0F, 1.0F, 100.0F)
        End Sub 'SetupMatrices

        Private Sub Render()
            If device Is Nothing Then
                Return
            End If
            If pause Then
                Return
            End If
            'Clear the backbuffer to a blue color 
            device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)
            'Begin the scene
            device.BeginScene()
            ' Setup the world, view, and projection matrices
            SetupMatrices()

            ' Meshes are divided into subsets, one for each material. Render them in
            ' a loop
            Dim i As Integer
            For i = 0 To meshMaterials.Length - 1
                ' Set the material and texture for this subset
                device.Material = meshMaterials(i)
                device.SetTexture(0, meshTextures(i))

                ' Draw the mesh subset
                mesh.DrawSubset(i)
            Next i

            'End the scene
            device.EndScene()
            device.Present()
        End Sub 'Render


        Protected Overrides Sub OnPaint(ByVal e As System.Windows.Forms.PaintEventArgs)
            Me.Render() ' Render on painting
        End Sub 'OnPaint

        Protected Overrides Sub OnKeyPress(ByVal e As System.Windows.Forms.KeyPressEventArgs)
            If Asc(e.KeyChar) = CInt(System.Windows.Forms.Keys.Escape) Then
                Me.Dispose() ' Esc was pressed
            End If
        End Sub 'OnKeyPress

        Protected Overrides Sub OnResize(ByVal e As System.EventArgs)
            pause = (Me.WindowState = FormWindowState.Minimized Or Not Me.Visible)
        End Sub 'OnResize

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Dim frm As New Meshes()
            If Not frm.InitializeGraphics() Then ' Initialize Direct3D
                MessageBox.Show("Could not initialize Direct3D.  This tutorial will exit.")
                Return
            End If
            frm.Show()

            While frm.Created
                frm.Render()
                Application.DoEvents()
            End While
        End Sub 'Main
    End Class 'Meshes
End Namespace 'MeshesTutorial