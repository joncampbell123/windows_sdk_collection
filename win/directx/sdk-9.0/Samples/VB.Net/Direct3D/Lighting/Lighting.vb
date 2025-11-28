'-----------------------------------------------------------------------------
' File: Lighting.vb
'
' Desc: Example code showing how to use D3D lights.
'
'       Note: This code uses the D3D Framework helper library.
'
' Copyright (c) 1995-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace Lighting
    _
    ' Custom D3D vertex format used by the vertex buffer
    Structure MyVertex
        Public p As Vector3 ' vertex position
        Public n As Vector3 ' vertex normal
        Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal
    End Structure 'MyVertex
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
        Private wallMesh As Mesh = Nothing ' Tessellated plane to serve as the walls and floor
        Private sphereMesh As Mesh = Nothing ' Representation of point light
        Private coneMesh As Mesh = Nothing ' Representation of dir/spot light
        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private lightData As Light ' Description of the D3D light
        Private numberVertsX As Integer = 32
        Private numberVertsZ As Integer = 32
        Private numTriangles As Integer = 0
        ' Number of triangles in the wall mesh


        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Lighting"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
            numTriangles = CInt((numberVertsX - 1) * (numberVertsZ - 1) * 2)
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            lightData = Device.Lights(2)

            ' Rotate through the various light types
            lightData.Type = CType(1 + CInt((CType(appTime, Integer) \ 5) Mod 3), LightType)

            ' Make sure the light type is supported by the device.  If 
            ' VertexProcessingCaps.PositionAllLights is not set, the device does not support 
            ' point or spot lights, so change light #2's type to a directional light.
            If Not Caps.VertexProcessingCaps.SupportsPositionAllLights Then
                If lightData.Type = LightType.Point Or lightData.Type = LightType.Spot Then
                    lightData.Type = LightType.Directional
                End If
            End If
            ' Values for the light position, direction, and color
            Dim x As Single = CSng(Math.Sin((appTime * 2.0F)))
            Dim y As Single = CSng(Math.Sin((appTime * 2.246F)))
            Dim z As Single = CSng(Math.Sin((appTime * 2.64F)))

            Dim r As Byte = CByte(0.5F + 0.5F * x * &HFF)
            Dim g As Byte = CByte(0.5F + 0.5F * y * &HFF)
            Dim b As Byte = CByte(0.5F + 0.5F * z * &HFF)
            lightData.Diffuse = System.Drawing.Color.FromArgb(r, g, b)
            lightData.Range = 100.0F

            Select Case lightData.Type
                Case LightType.Point
                    lightData.Position = New Vector3(4.5F * x, 4.5F * y, 4.5F * z)
                    lightData.Attenuation1 = 0.4F
                Case LightType.Directional
                    lightData.Direction = New Vector3(x, y, z)
                Case LightType.Spot
                    lightData.Position = New Vector3(2.0F * x, 2.0F * y, 2.0F * z)
                    lightData.Direction = New Vector3(x, y, z)
                    lightData.InnerConeAngle = 0.5F
                    lightData.OuterConeAngle = 1.0F
                    lightData.Falloff = 1.0F
                    lightData.Attenuation0 = 1.0F
            End Select
            Device.Lights(2).Commit()
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, &HFF, 1.0F, 0)

            Device.BeginScene()
            Dim matWorld As Matrix
            Dim matTrans As Matrix
            Dim matRotate As Matrix

            ' Turn on light #0 and #2, and turn off light #1
            Device.Lights(0).Enabled = True
            Device.Lights(1).Enabled = False
            Device.Lights(2).Enabled = True

            ' Draw the floor
            matTrans = Matrix.Translation(-5.0F, -5.0F, -5.0F)
            matRotate = Matrix.RotationZ(0.0F)
            matWorld = Matrix.Multiply(matRotate, matTrans)
            Device.SetTransform(TransformType.World, matWorld)
            wallMesh.DrawSubset(0)

            ' Draw the back wall
            matTrans = Matrix.Translation(5.0F, -5.0F, -5.0F)
            matRotate = Matrix.RotationZ((CSng(Math.PI) / 2))
            matWorld = Matrix.Multiply(matRotate, matTrans)
            Device.SetTransform(TransformType.World, matWorld)
            wallMesh.DrawSubset(0)

            ' Draw the side wall
            matTrans = Matrix.Translation(-5.0F, -5.0F, 5.0F)
            matRotate = Matrix.RotationX((CSng(-Math.PI) / 2))
            matWorld = Matrix.Multiply(matRotate, matTrans)
            Device.SetTransform(TransformType.World, matWorld)
            wallMesh.DrawSubset(0)

            ' Turn on light #1, and turn off light #0 and #2
            Device.Lights(0).Enabled = False
            Device.Lights(1).Enabled = True
            Device.Lights(2).Enabled = False

            ' Draw the mesh representing the light
            If lightData.Type = LightType.Point Then
                ' Just position the point light -- no need to orient it
                matWorld = Matrix.Translation(lightData.Position.X, lightData.Position.Y, lightData.Position.Z)
                Device.SetTransform(TransformType.World, matWorld)
                sphereMesh.DrawSubset(0)
            Else
                ' Position the light and point it in the light's direction
                Dim vecFrom As New Vector3(lightData.Position.X, lightData.Position.Y, lightData.Position.Z)
                Dim vecAt As New Vector3(lightData.Position.X + lightData.Direction.X, lightData.Position.Y + lightData.Direction.Y, lightData.Position.Z + lightData.Direction.Z)
                Dim vecUp As New Vector3(0, 1, 0)
                Dim matWorldInv As Matrix
                matWorldInv = Matrix.LookAtLH(vecFrom, vecAt, vecUp)
                matWorld = Matrix.Invert(matWorldInv)
                Device.SetTransform(TransformType.World, matWorld)
                coneMesh.DrawSubset(0)
            End If

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 21, System.Drawing.Color.Yellow, deviceStats)
            Dim strLight As String = IIf(lightData.Type = LightType.Point, "Point Light", IIf(lightData.Type = LightType.Spot, "Spot Light", "Directional Light"))
            drawingFont.DrawText(2, 41, System.Drawing.Color.White, strLight)
            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Dim v() As MyVertex
            Dim pWallMeshTemp As Mesh = Nothing

            ' Create a square grid numberVertsX*numberVertsZ for rendering the wall
            pWallMeshTemp = New Mesh(numTriangles, numTriangles * 3, 0, MyVertex.Format, Device)

            ' Fill in the grid vertex data
            v = CType(pWallMeshTemp.LockVertexBuffer(GetType(MyVertex), 0, numTriangles * 3), MyVertex())
            Dim dX As Single = 1.0F / (numberVertsX - 1)
            Dim dZ As Single = 1.0F / (numberVertsZ - 1)
            Dim k As Integer = 0
            Dim z As Integer
            For z = 0 To (numberVertsZ - 1) - 1
                Dim x As Integer
                For x = 0 To (numberVertsX - 1) - 1
                    v(k).p = New Vector3(10 * x * dX, 0.0F, 10 * z * dZ)
                    v(k).n = New Vector3(0.0F, 1.0F, 0.0F)
                    k += 1
                    v(k).p = New Vector3(10 * x * dX, 0.0F, 10 * (z + 1) * dZ)
                    v(k).n = New Vector3(0.0F, 1.0F, 0.0F)
                    k += 1
                    v(k).p = New Vector3(10 * (x + 1) * dX, 0.0F, 10 * (z + 1) * dZ)
                    v(k).n = New Vector3(0.0F, 1.0F, 0.0F)
                    k += 1
                    v(k).p = New Vector3(10 * x * dX, 0.0F, 10 * z * dZ)
                    v(k).n = New Vector3(0.0F, 1.0F, 0.0F)
                    k += 1
                    v(k).p = New Vector3(10 * (x + 1) * dX, 0.0F, 10 * (z + 1) * dZ)
                    v(k).n = New Vector3(0.0F, 1.0F, 0.0F)
                    k += 1
                    v(k).p = New Vector3(10 * (x + 1) * dX, 0.0F, 10 * z * dZ)
                    v(k).n = New Vector3(0.0F, 1.0F, 0.0F)
                    k += 1
                Next x
            Next z
            pWallMeshTemp.UnlockVertexBuffer()

            ' Fill in index data
            Dim pIndex() As Short
            pIndex = CType(pWallMeshTemp.LockIndexBuffer(GetType(Short), 0, numTriangles * 3), Short())
            Dim iIndex As Short
            For iIndex = 0 To (numTriangles * 3) - 1
                pIndex(iIndex) = iIndex
            Next iIndex
            pWallMeshTemp.UnlockIndexBuffer()

            ' Eliminate redundant vertices
            Dim pdwAdjacency(3 * numTriangles) As Integer
            Dim we As New WeldEpsilons()
            pWallMeshTemp.GenerateAdjacency(0.01F, pdwAdjacency)
            pWallMeshTemp.WeldVertices(CInt(WeldEpsilonsFlags.WeldAll), we, pdwAdjacency)

            ' Optimize the mesh
            wallMesh = pWallMeshTemp.Optimize(MeshFlags.OptimizeCompact Or MeshFlags.OptimizeVertexCache Or MeshFlags.VbDynamic Or MeshFlags.VbWriteOnly, pdwAdjacency)

            pWallMeshTemp = Nothing
            pdwAdjacency = Nothing

            ' Create sphere and cone meshes to represent the lights
            sphereMesh = Mesh.Sphere(Device, 0.25F, 20, 20)
            coneMesh = Mesh.Cylinder(Device, 0.0F, 0.25F, 0.5F, 20, 20)

            ' Set up a material
            Dim mtrl As Microsoft.DirectX.Direct3D.Material = GraphicsUtility.InitMaterial(System.Drawing.Color.White)
            Device.Material = mtrl

            ' Set miscellaneous render states
            Device.RenderState.DitherEnable = False
            Device.RenderState.SpecularEnable = False

            ' Set the world matrix
            Dim matIdentity As Matrix = Matrix.Identity
            Device.SetTransform(TransformType.World, matIdentity)

            ' Set the view matrix.
            Dim matView As Matrix
            Dim vFromPt As New Vector3(-10, 10, -10)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            matView = Matrix.LookAtLH(vFromPt, vLookatPt, vUpVec)
            Device.SetTransform(TransformType.View, matView)

            ' Set the projection matrix
            Dim matProj As Matrix
            Dim fAspect As Single = CSng(Device.PresentationParameters.BackBufferWidth) / Device.PresentationParameters.BackBufferHeight
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 100.0F)
            Device.SetTransform(TransformType.Projection, matProj)

            ' Turn on lighting.
            Device.RenderState.Lighting = True

            ' Enable ambient lighting to a dim, grey light, so objects that
            ' are not lit by the other lights are not completely black
            Device.RenderState.Ambient = System.Drawing.Color.FromArgb(64, 64, 64)

            ' Set light #0 to be a simple, faint grey directional light so 
            ' the walls and floor are slightly different shades of grey
            Device.Lights(0).Type = LightType.Directional
            Device.Lights(0).Direction = New Vector3(0.3F, -0.5F, 0.2F)
            Device.Lights(0).Diffuse = System.Drawing.Color.FromArgb(64, 64, 64)
            Device.Lights(0).Commit()

            ' Set light #1 to be a simple, bright directional light to use 
            ' on the mesh representing light #2
            Device.Lights(1).Type = LightType.Directional
            Device.Lights(1).Direction = New Vector3(0.5F, -0.5F, 0.5F)
            Device.Lights(1).Diffuse = System.Drawing.Color.White
            Device.Lights(1).Commit()
        End Sub 'RestoreDeviceObjects

        ' Light #2 will be the light used to light the floor and walls.  It will
        ' be set up in FrameMove() since it changes every frame.


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
End Namespace 'Lighting