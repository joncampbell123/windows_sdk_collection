'-----------------------------------------------------------------------------
' File: DolphinVS.vb
'
' Desc: Sample of swimming dolphin
'
'       Note: This code uses the D3D Framework helper library.
'
' Copyright (c) Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D
Imports Direct3D = Microsoft.DirectX.Direct3D


Namespace DolphinVS
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample



        Private WaterColor As System.Drawing.Color = System.Drawing.Color.FromArgb(&H4080)
       _

        Public Structure Vertex
            Public p As Vector3
            Public n As Vector3
            Public tu, tv As Single
            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Normal Or VertexFormats.Texture1
        End Structure 'Vertex

        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        ' Transform matrices
        Private worldMatrix As Matrix = Matrix.Identity
        Private viewMatrix As Matrix = Matrix.Identity
        Private projectionMatrix As Matrix = Matrix.Identity

        ' Dolphin object
        Private dolphinTexture As Texture = Nothing
        Private dolphinVertexBuffer1 As VertexBuffer = Nothing
        Private dolphinVertexBuffer2 As VertexBuffer = Nothing
        Private dolphinVertexBuffer3 As VertexBuffer = Nothing
        Private dolphinIndexBuffer As IndexBuffer = Nothing
        Private numDolphinVertices As Integer = 0
        Private numDolphinFaces As Integer = 0
        Private dolphinVertexDeclaration As VertexDeclaration = Nothing
        Private dolphinVertexShader As VertexShader = Nothing
        Private dolphinVertexShader2 As VertexShader = Nothing

        ' Seafloor object
        Private seaFloorTexture As Texture = Nothing
        Private seaFloorVertexBuffer As VertexBuffer = Nothing
        Private seaFloorIndexBuffer As IndexBuffer = Nothing
        Private numSeaFloorVertices As Integer = 0
        Private numSeaFloorFaces As Integer = 0
        Private seaFloorVertexDeclaration As VertexDeclaration = Nothing
        Private seaFloorVertexShader As VertexShader = Nothing
        Private seaFloorVertexShader2 As VertexShader = Nothing

        ' Water caustics
        Private causticTextures(32) As Texture
        Private currentCausticTexture As Texture = Nothing




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "DolphinVS: Tweening Vertex Shader"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True
        End Sub 'New





        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Animation attributes for the dolphin
            Dim fKickFreq As Single = 2 * appTime
            Dim fPhase As Single = appTime / 3
            Dim fBlendWeight As Single = CSng(Math.Sin(fKickFreq))

            ' Move the dolphin in a circle
            Dim matDolphin, matTrans, matRotate1, matRotate2 As Matrix
            matDolphin = Matrix.Scaling(0.01F, 0.01F, 0.01F)
            matRotate1 = Matrix.RotationZ((-CSng(Math.Cos(fKickFreq)) / 6))
            matDolphin.Multiply(matRotate1)
            matRotate2 = Matrix.RotationY(fPhase)
            matDolphin.Multiply(matRotate2)
            matTrans = Matrix.Translation(-5 * CSng(Math.Sin(fPhase)), CSng(Math.Sin(fKickFreq)) / 2, 10 - 10 * CSng(Math.Cos(fPhase)))
            matDolphin.Multiply(matTrans)

            ' Animate the caustic textures
            Dim tex As Integer = CInt(appTime * 32) Mod 32
            currentCausticTexture = causticTextures(tex)

            ' Set the vertex shader constants. Note: outside of the blend matrices,
            ' most of these values don't change, so don't need to really be set every
            ' frame. It's just done here for clarity
            ' Some basic constants
            Dim vZero As New Vector4(0.0F, 0.0F, 0.0F, 0.0F)
            Dim vOne As New Vector4(1.0F, 0.5F, 0.2F, 0.05F)

            Dim fWeight1 As Single
            Dim fWeight2 As Single
            Dim fWeight3 As Single

            If fBlendWeight > 0.0F Then
                fWeight1 = CSng(Math.Abs(fBlendWeight))
                fWeight2 = 1.0F - CSng(Math.Abs(fBlendWeight))
                fWeight3 = 0.0F
            Else
                fWeight1 = 0.0F
                fWeight2 = 1.0F - CSng(Math.Abs(fBlendWeight))
                fWeight3 = CSng(Math.Abs(fBlendWeight))
            End If
            Dim vWeight As New Vector4(fWeight1, fWeight2, fWeight3, 0.0F)

            ' Lighting vectors (in world space and in dolphin model space)
            ' and other constants
            Dim fLight As New Vector4(0.0F, 1.0F, 0.0F, 0.0F)
            Dim fLightDolphinSpace As New Vector4(0.0F, 1.0F, 0.0F, 0.0F)
            Dim fDiffuse As Single() = {1.0F, 1.0F, 1.0F, 1.0F}
            Dim fAmbient As Single() = {0.25F, 0.25F, 0.25F, 0.25F}
            Dim fFog As Single() = {0.5F, 50.0F, 1.0F / (50.0F - 1.0F), 0.0F}
            Dim fCaustics As Single() = {0.05F, 0.05F, CSng(Math.Sin(appTime)) / 8, CSng(Math.Cos(appTime)) / 10}

            Dim matDolphinInv As Matrix = Matrix.Invert(matDolphin)
            fLightDolphinSpace = Vector4.Transform(fLight, matDolphinInv)
            fLightDolphinSpace.Normalize()

            ' Vertex shader operations use transposed matrices
            Dim matCamera As New Matrix()
            Dim matTranspose As New Matrix()
            Dim matCameraTranspose As New Matrix()
            Dim mat As Matrix
            Dim matViewTranspose As New Matrix()
            Dim matProjTranspose As New Matrix()
            matCamera = Matrix.Multiply(matDolphin, viewMatrix)
            mat = Matrix.Multiply(matCamera, projectionMatrix)
            matTranspose.Transpose(mat)
            matCameraTranspose.Transpose(matCamera)
            matViewTranspose.Transpose(viewMatrix)
            matProjTranspose.Transpose(projectionMatrix)

            ' Set the vertex shader constants
            Device.SetVertexShaderConstant(0, New Vector4() {vZero})
            Device.SetVertexShaderConstant(1, New Vector4() {vOne})
            Device.SetVertexShaderConstant(2, New Vector4() {vWeight})
            Device.SetVertexShaderConstant(4, New Matrix() {matTranspose})
            Device.SetVertexShaderConstant(8, New Matrix() {matCameraTranspose})
            Device.SetVertexShaderConstant(12, New Matrix() {matViewTranspose})
            Device.SetVertexShaderConstant(19, New Vector4() {fLightDolphinSpace})
            Device.SetVertexShaderConstant(20, New Vector4() {fLight})
            Device.SetVertexShaderConstant(21, fDiffuse)
            Device.SetVertexShaderConstant(22, fAmbient)
            Device.SetVertexShaderConstant(23, fFog)
            Device.SetVertexShaderConstant(24, fCaustics)
            Device.SetVertexShaderConstant(28, New Matrix() {matProjTranspose})
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, WaterColor, 1.0F, 0)

            Device.BeginScene()

            Dim fAmbientLight As Single() = {0.25F, 0.25F, 0.25F, 0.25F}
            Device.SetVertexShaderConstant(22, fAmbientLight)

            ' Render the seafloor
            Device.SetTexture(0, seaFloorTexture)
            Device.VertexDeclaration = seaFloorVertexDeclaration
            Device.VertexShader = seaFloorVertexShader
            Device.SetStreamSource(0, seaFloorVertexBuffer, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.Indices = seaFloorIndexBuffer
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numSeaFloorVertices, 0, numSeaFloorFaces)

            ' Render the dolphin
            Device.SetTexture(0, dolphinTexture)
            Device.VertexDeclaration = dolphinVertexDeclaration
            Device.VertexShader = dolphinVertexShader
            Device.SetStreamSource(0, dolphinVertexBuffer1, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.SetStreamSource(1, dolphinVertexBuffer2, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.SetStreamSource(2, dolphinVertexBuffer3, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.Indices = dolphinIndexBuffer
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numDolphinVertices, 0, numDolphinFaces)

            ' Now, we are going to do a 2nd pass, to alpha-blend in the caustics.
            ' The caustics use a 2nd set of texture coords that are generated
            ' by the vertex shaders. Lighting from the light above is used, but
            ' ambient is turned off to avoid lighting objects from below (for
            ' instance, we don't want caustics appearing on the dolphin's
            ' underbelly). Finally, fog color is set to black, so that caustics
            ' fade in distance.
            ' Turn on alpha blending
            Device.RenderState.AlphaBlendEnable = True

            Device.RenderState.SourceBlend = Blend.One
            Device.RenderState.DestinationBlend = Blend.One

            ' Setup the caustic texture
            Device.SetTexture(0, currentCausticTexture)

            ' Set ambient and fog colors to black
            Dim fAmbientDark As Single() = {0.0F, 0.0F, 0.0F, 0.0F}
            Device.SetVertexShaderConstant(22, fAmbientDark)
            Device.RenderState.FogColor = System.Drawing.Color.Black

            ' Render the caustic effects for the seafloor
            Device.VertexDeclaration = seaFloorVertexDeclaration
            Device.VertexShader = seaFloorVertexShader2
            Device.SetStreamSource(0, seaFloorVertexBuffer, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.Indices = seaFloorIndexBuffer
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numSeaFloorVertices, 0, numSeaFloorFaces)

            ' Finally, render the caustic effects for the dolphin
            Device.VertexDeclaration = dolphinVertexDeclaration
            Device.VertexShader = dolphinVertexShader2
            Device.SetStreamSource(0, dolphinVertexBuffer1, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.SetStreamSource(1, dolphinVertexBuffer2, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.SetStreamSource(2, dolphinVertexBuffer3, 0, DXHelp.GetTypeSize(GetType(Vertex)))
            Device.Indices = dolphinIndexBuffer
            Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numDolphinVertices, 0, numDolphinFaces)

            ' Restore modified render states
            Device.RenderState.AlphaBlendEnable = False
            Device.RenderState.FogColor = WaterColor


            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            Dim pMeshSourceVB As VertexBuffer = Nothing
            Dim pMeshSourceIB As IndexBuffer = Nothing
            Dim src As Vertex() = Nothing
            Dim dst As GraphicsStream = Nothing
            Dim DolphinMesh01 As New GraphicsMesh()
            Dim DolphinMesh02 As New GraphicsMesh()
            Dim DolphinMesh03 As New GraphicsMesh()
            Dim SeaFloorMesh As New GraphicsMesh()

            ' Initialize the font's internal textures
            drawingFont.InitializeDeviceObjects(Device)

            Try
                ' Create texture for the dolphin
                dolphinTexture = GraphicsUtility.CreateTexture(Device, "Dolphin.bmp")

                ' Create textures for the seafloor
                seaFloorTexture = GraphicsUtility.CreateTexture(Device, "SeaFloor.bmp")

                ' Create textures for the water caustics
                Dim t As Integer
                For t = 0 To 31
                    Dim name As String = String.Format("Caust{0:D2}.tga", t)
                    causticTextures(t) = GraphicsUtility.CreateTexture(Device, name)
                Next t

                ' Load the file-based mesh objects
                DolphinMesh01.Create(Device, "dolphin1.x")
                DolphinMesh02.Create(Device, "dolphin2.x")
                DolphinMesh03.Create(Device, "dolphin3.x")
                SeaFloorMesh.Create(Device, "SeaFloor.x")
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try
            ' Set the FVF type to match the vertex format we want
            DolphinMesh01.SetFVF(Device, Vertex.Format)
            DolphinMesh02.SetFVF(Device, Vertex.Format)
            DolphinMesh03.SetFVF(Device, Vertex.Format)
            SeaFloorMesh.SetFVF(Device, Vertex.Format)

            ' Get the number of vertices and faces for the meshes
            numDolphinVertices = DolphinMesh01.SysMemMesh.NumberVertices
            numDolphinFaces = DolphinMesh01.SysMemMesh.NumberFaces
            numSeaFloorVertices = SeaFloorMesh.SysMemMesh.NumberVertices
            numSeaFloorFaces = SeaFloorMesh.SysMemMesh.NumberFaces

            ' Create the dolphin and seafloor vertex and index buffers
            dolphinVertexBuffer1 = New VertexBuffer(GetType(Vertex), numDolphinVertices, Device, Usage.WriteOnly, 0, Pool.Managed)
            dolphinVertexBuffer2 = New VertexBuffer(GetType(Vertex), numDolphinVertices, Device, Usage.WriteOnly, 0, Pool.Managed)
            dolphinVertexBuffer3 = New VertexBuffer(GetType(Vertex), numDolphinVertices, Device, Usage.WriteOnly, 0, Pool.Managed)
            seaFloorVertexBuffer = New VertexBuffer(GetType(Vertex), numSeaFloorVertices, Device, Usage.WriteOnly, 0, Pool.Managed)
            dolphinIndexBuffer = New IndexBuffer(GetType(Short), numDolphinFaces * 3, Device, Usage.WriteOnly, Pool.Managed)
            seaFloorIndexBuffer = New IndexBuffer(GetType(Short), numSeaFloorFaces * 3, Device, Usage.WriteOnly, Pool.Managed)

            ' Copy vertices for mesh 01
            pMeshSourceVB = DolphinMesh01.SysMemMesh.VertexBuffer
            dst = dolphinVertexBuffer1.Lock(0, DXHelp.GetTypeSize(GetType(Vertex)) * numDolphinVertices, 0)
            src = CType(pMeshSourceVB.Lock(0, GetType(Vertex), 0, numDolphinVertices), Vertex())
            dst.Write(src)
            dolphinVertexBuffer1.Unlock()
            pMeshSourceVB.Unlock()
            pMeshSourceVB.Dispose()

            ' Copy vertices for mesh 2
            pMeshSourceVB = DolphinMesh02.SysMemMesh.VertexBuffer
            dst = dolphinVertexBuffer2.Lock(0, DXHelp.GetTypeSize(GetType(Vertex)) * numDolphinVertices, 0)
            src = CType(pMeshSourceVB.Lock(0, GetType(Vertex), 0, numDolphinVertices), Vertex())
            dst.Write(src)
            dolphinVertexBuffer2.Unlock()
            pMeshSourceVB.Unlock()
            pMeshSourceVB.Dispose()

            ' Copy vertices for mesh 3
            pMeshSourceVB = DolphinMesh03.SysMemMesh.VertexBuffer
            dst = dolphinVertexBuffer3.Lock(0, DXHelp.GetTypeSize(GetType(Vertex)) * numDolphinVertices, 0)
            src = CType(pMeshSourceVB.Lock(0, GetType(Vertex), 0, numDolphinVertices), Vertex())
            dst.Write(src)
            dolphinVertexBuffer3.Unlock()
            pMeshSourceVB.Unlock()
            pMeshSourceVB.Dispose()

            ' Copy vertices for the seafloor mesh, and add some bumpiness
            pMeshSourceVB = SeaFloorMesh.SysMemMesh.VertexBuffer
            dst = seaFloorVertexBuffer.Lock(0, DXHelp.GetTypeSize(GetType(Vertex)) * numSeaFloorVertices, 0)
            src = CType(pMeshSourceVB.Lock(0, GetType(Vertex), 0, numSeaFloorVertices), Vertex())

            Dim r As New System.Random()
            Dim i As Integer
            For i = 0 To numSeaFloorVertices - 1
                src(i).p.Y += r.Next() / CSng(Integer.MaxValue)
                src(i).p.Y += r.Next() / CSng(Integer.MaxValue)
                src(i).p.Y += r.Next() / CSng(Integer.MaxValue)
                src(i).tu *= 10
                src(i).tv *= 10
            Next i
            dst.Write(src)
            seaFloorVertexBuffer.Unlock()
            pMeshSourceVB.Unlock()
            pMeshSourceVB.Dispose()

            Dim dstib As GraphicsStream = Nothing
            Dim srcib As Short() = Nothing

            ' Copy indices for the dolphin mesh
            pMeshSourceIB = DolphinMesh01.SysMemMesh.IndexBuffer
            dstib = dolphinIndexBuffer.Lock(0, DXHelp.GetTypeSize(GetType(Short)) * numDolphinFaces * 3, 0)
            srcib = CType(pMeshSourceIB.Lock(0, GetType(Short), 0, numDolphinFaces * 3), Short())
            dstib.Write(srcib)
            dolphinIndexBuffer.Unlock()
            pMeshSourceIB.Unlock()
            pMeshSourceIB.Dispose()

            ' Copy indices for the seafloor mesh
            pMeshSourceIB = SeaFloorMesh.SysMemMesh.IndexBuffer
            dstib = seaFloorIndexBuffer.Lock(0, DXHelp.GetTypeSize(GetType(Short)) * numSeaFloorFaces * 3, 0)
            srcib = CType(pMeshSourceIB.Lock(0, GetType(Short), 0, numSeaFloorFaces * 3), Short())
            dstib.Write(srcib)
            seaFloorIndexBuffer.Unlock()
            pMeshSourceIB.Unlock()
            pMeshSourceIB.Dispose()
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 0.0F, -5.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim fAspect As Single = CSng(device.PresentationParameters.BackBufferWidth) / device.PresentationParameters.BackBufferHeight
            worldMatrix = Matrix.Identity
            viewMatrix = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            projectionMatrix = Matrix.PerspectiveFovLH(CSng(Math.PI) / 3, fAspect, 1.0F, 10000.0F)

            ' Set default render states
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear
            Device.RenderState.ZBufferEnable = True
            Device.RenderState.FogEnable = True
            Device.RenderState.FogColor = WaterColor

            ' Create vertex shader for the dolphin
            Dim dolphinVertexDecl() As VertexElement = {New VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0), New VertexElement(0, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 0), New VertexElement(0, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0), New VertexElement(1, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 1), New VertexElement(1, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 1), New VertexElement(1, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 1), New VertexElement(2, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 2), New VertexElement(2, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 2), New VertexElement(2, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 2), VertexElement.VertexDeclarationEnd}
            ' First stream is first mesh
            ' Second stream is second mesh
            ' Third stream is third mesh

            dolphinVertexDeclaration = New VertexDeclaration(Device, dolphinVertexDecl)
            dolphinVertexShader = GraphicsUtility.CreateVertexShader(Device, "DolphinTween.vsh")
            dolphinVertexShader2 = GraphicsUtility.CreateVertexShader(Device, "DolphinTween2.vsh")


            ' Create vertex shader for the seafloor
            Dim seaFloorVertexDecl() As VertexElement = {New VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0), New VertexElement(0, 12, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Normal, 0), New VertexElement(0, 24, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0), VertexElement.VertexDeclarationEnd}

            seaFloorVertexDeclaration = New VertexDeclaration(Device, seaFloorVertexDecl)
            seaFloorVertexShader = GraphicsUtility.CreateVertexShader(Device, "SeaFloor.vsh")
            seaFloorVertexShader2 = GraphicsUtility.CreateVertexShader(Device, "SeaFloor2.vsh")
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub InvalidateDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Clean up vertex shaders
            If Not (dolphinVertexShader Is Nothing) Then
                dolphinVertexShader.Dispose()
                dolphinVertexShader = Nothing
            End If

            If Not (dolphinVertexShader2 Is Nothing) Then
                dolphinVertexShader2.Dispose()
                dolphinVertexShader2 = Nothing
            End If

            If Not (seaFloorVertexShader Is Nothing) Then
                seaFloorVertexShader.Dispose()
                seaFloorVertexShader = Nothing
            End If

            If Not (seaFloorVertexShader2 Is Nothing) Then
                seaFloorVertexShader2.Dispose()
                seaFloorVertexShader2 = Nothing
            End If
        End Sub 'InvalidateDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If Not Manager.CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, adapterFmt, Usage.RenderTarget Or Usage.QueryPostPixelShaderBlending, ResourceType.Surface, backBufferFmt) Then
                Return False ' Need to support post-pixel processing (for fog)
            End If
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.PureHardware Or vertexProcessingType = vertexProcessingType.Mixed Then
                If caps.VertexShaderVersion.Major < 1 Then
                    Return False
                End If
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
End Namespace 'DolphinVS