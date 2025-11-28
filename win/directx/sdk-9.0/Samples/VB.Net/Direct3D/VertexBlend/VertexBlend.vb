'-----------------------------------------------------------------------------
' File: VertexBlend.vb
'
' Desc: Example code showing how to do a skinning effect, using the vertex
'       blending feature of Direct3D. Normally, Direct3D transforms each
'       vertex through the world matrix. The vertex blending feature,
'       however, uses mulitple world matrices and a per-vertex blend factor
'       to transform each vertex.
'
' Copyright (c) 1997-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace VertexBlendSample
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
       _



        '-----------------------------------------------------------------------------
        ' Name: struct BlendVertex
        ' Desc: Custom vertex which includes a blending factor
        '-----------------------------------------------------------------------------
        Public Structure BlendVertex
            Public v As Vector3 ' Referenced as v0 in the vertex shader
            Public blend As Single ' Referenced as v1.x in the vertex shader
            Public n As Vector3 ' Referenced as v3 in the vertex shader
            Public tu, tv As Single ' Referenced as v7 in the vertex shader
            Public Const Format As VertexFormats = VertexFormats.PositionBlend1 Or VertexFormats.Normal Or VertexFormats.Texture1
        End Structure 'BlendVertex

        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private blendObject As GraphicsMesh = Nothing ' Object to use for vertex blending
        Private numVertices As Integer = 0
        Private numFaces As Integer = 0
        Private vertexBuffer As vertexBuffer = Nothing
        Private indexBuffer As indexBuffer = Nothing

        Private upperArm As Matrix = Matrix.Identity ' Vertex blending matrices
        Private lowerArm As Matrix = Matrix.Identity

        Private shader As VertexShader = Nothing ' Vertex shader
        Private declaration As VertexDeclaration = Nothing
        Private useShader As Boolean = False
        Private mnuOptions As System.Windows.Forms.MenuItem
        Private WithEvents mnuUseVS As System.Windows.Forms.MenuItem




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "VertexBlend: Surface Skinning Example"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = True

            ' Add our new menu options
            Me.mnuOptions = New System.Windows.Forms.MenuItem()
            Me.mnuUseVS = New System.Windows.Forms.MenuItem()
            ' Add the Options menu to the main menu
            Me.mnuMain.MenuItems.Add(Me.mnuOptions)
            Me.mnuOptions.Index = 1
            Me.mnuOptions.Text = "&Options"

            Me.mnuOptions.MenuItems.Add(Me.mnuUseVS)
            Me.mnuUseVS.Text = "Use custom &vertex shader"
            Me.mnuUseVS.Shortcut = System.Windows.Forms.Shortcut.CtrlV
            Me.mnuUseVS.ShowShortcut = True
        End Sub 'New





        '/ <summary>
        '/ Called when our menu item is clicked.
        '/ </summary>
        Private Sub UseCustomShaderClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuUseVS.Click
            useShader = Not useShader
            mnuUseVS.Checked = Not mnuUseVS.Checked
        End Sub 'UseCustomShaderClick




        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Set the vertex blending matrices for this frame
            Dim vAxis As New Vector3(2 + CSng(Math.Sin((appTime * 3.1F))), 2 + CSng(Math.Sin((appTime * 3.3F))), CSng(Math.Sin((appTime * 3.5F))))
            lowerArm = Matrix.RotationAxis(vAxis, CSng(Math.Sin((3 * appTime))))
            upperArm = Matrix.Identity

            ' Set the vertex shader constants. Note: outside of the blend matrices,
            ' most of these values don't change, so don't need to really be set every
            ' frame. It's just done here for clarity
            If useShader Then
                ' Some basic constants
                Dim vZero As New Vector4(0, 0, 0, 0)
                Dim vOne As New Vector4(1, 1, 1, 1)

                ' Lighting vector (normalized) and material colors. (Use non-yellow light
                ' to show difference from non-vertex shader case.)
                Dim vLight As New Vector4(0.5F, 1.0F, -1.0F, 0.0F)
                vLight.Normalize()
                Dim fDiffuse As Single() = {0.5F, 1.0F, 0.0F, 0.0F}
                Dim fAmbient As Single() = {0.25F, 0.25F, 0.25F, 0.25F}

                ' Vertex shader operations use transposed matrices
                Dim matWorld0Transpose As New Matrix()
                Dim matWorld1Transpose As New Matrix()
                Dim matViewProjTranspose As New Matrix()
                Dim matView, matProj, matViewProj As Matrix
                matView = Device.Transform.View
                matProj = Device.Transform.Projection
                matViewProj = Matrix.Multiply(matView, matProj)
                matWorld0Transpose.Transpose(upperArm)
                matWorld1Transpose.Transpose(lowerArm)
                matViewProjTranspose.Transpose(matViewProj)

                ' Set the vertex shader constants
                Device.SetVertexShaderConstant(0, New Vector4() {vZero})
                Device.SetVertexShaderConstant(1, New Vector4() {vOne})
                Device.SetVertexShaderConstant(4, New Matrix() {matWorld0Transpose})
                Device.SetVertexShaderConstant(8, New Matrix() {matWorld1Transpose})
                Device.SetVertexShaderConstant(12, New Matrix() {matViewProjTranspose})
                Device.SetVertexShaderConstant(20, New Vector4() {vLight})
                Device.SetVertexShaderConstant(21, fDiffuse)
                Device.SetVertexShaderConstant(22, fAmbient)
            End If
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, System.Drawing.Color.Blue, 1.0F, 0)

            Device.BeginScene()

            If useShader Then
                Device.VertexFormat = BlendVertex.Format
                Device.VertexShader = shader
                Device.SetStreamSource(0, vertexBuffer, 0, DXHelp.GetTypeSize(GetType(BlendVertex)))
                Device.Indices = indexBuffer
                Device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, numVertices, 0, numFaces)
            Else
                Device.VertexShader = Nothing
                ' Enable vertex blending using API
                Device.Transform.World = upperArm
                Device.Transform.World1 = lowerArm
                RenderState.VertexBlend = VertexBlend.OneWeights

                ' Display the object
                blendObject.Render(Device)
            End If


            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            If useShader Then
                drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using vertex shader")
            Else
                drawingFont.DrawText(2, 40, System.Drawing.Color.White, "Using RenderState.VertexBlend")
            End If
            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
            ' Load an object to render
            Try
                If blendObject Is Nothing Then
                    blendObject = New GraphicsMesh()
                End If
                blendObject.Create(Device, "mslogo.x")
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            End Try


            If (BehaviorFlags.HardwareVertexProcessing Or BehaviorFlags.MixedVertexProcessing) And Caps.VertexShaderVersion.Major < 1 Then
                ' No VS available, so don't try to use it or allow user to
                ' switch to it
                useShader = False
                mnuUseVS.Enabled = False
            ElseIf Caps.MaxVertexBlendMatrices < 2 Then
                ' No blend matrices available, so don't try to use them or 
                ' allow user to switch to them
                useShader = True
                mnuUseVS.Enabled = False
            Else
                ' Both techniques available, so default to blend matrices and 
                ' allow the user to switch techniques
                useShader = False
                mnuUseVS.Enabled = True
            End If

            ' Set a custom FVF for the mesh
            blendObject.SetFVF(Device, BlendVertex.Format)

            ' Add blending weights to the mesh
            ' Gain acces to the mesh's vertices
            Dim vb As VertexBuffer = Nothing
            Dim vertices As BlendVertex() = Nothing
            Dim numVertices As Integer = blendObject.SysMemMesh.NumberVertices
            vb = blendObject.SysMemMesh.VertexBuffer
            vertices = CType(vb.Lock(0, GetType(BlendVertex), 0, numVertices), BlendVertex())

            ' Calculate the min/max z values for all the vertices
            Dim fMinX As Single = 1.0E+10F
            Dim fMaxX As Single = -1.0E+10F

            Dim i As Integer
            For i = 0 To numVertices - 1
                If vertices(i).v.X < fMinX Then
                    fMinX = vertices(i).v.X
                End If
                If vertices(i).v.X > fMaxX Then
                    fMaxX = vertices(i).v.X
                End If
            Next i
            For i = 0 To numVertices - 1
                ' Set the blend factors for the vertices
                Dim a As Single = (vertices(i).v.X - fMinX) / (fMaxX - fMinX)
                vertices(i).blend = 1.0F - CSng(Math.Sin((a * CSng(Math.PI) * 1.0F)))
            Next i

            ' Done with the mesh's vertex buffer data
            vb.Unlock()
            vb.Dispose()
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Restore mesh's local memory objects
            blendObject.RestoreDeviceObjects(Device, Nothing)

            ' Get access to the mesh vertex and index buffers
            vertexBuffer = blendObject.LocalMesh.VertexBuffer
            indexBuffer = blendObject.LocalMesh.IndexBuffer
            numVertices = blendObject.LocalMesh.NumberVertices
            numFaces = blendObject.LocalMesh.NumberFaces

            If BehaviorFlags.SoftwareVertexProcessing Or Caps.VertexShaderVersion.Major >= 1 Then
                ' Setup the vertex declaration
                Dim decl As VertexElement() = VertexInformation.DeclaratorFromFormat(BlendVertex.Format)
                declaration = New VertexDeclaration(Device, decl)

                ' Create vertex shader from a file
                Try
                    shader = GraphicsUtility.CreateVertexShader(Device, "blend.vsh")
                Catch
                    Dim d3de As New MediaNotFoundException()
                    HandleSampleException(d3de, ApplicationMessage.ApplicationMustExit)
                    Throw d3de
                End Try
            End If
            ' Set miscellaneous render states
            RenderState.ZBufferEnable = True
            RenderState.Ambient = System.Drawing.Color.FromArgb(&H404040)

            ' Set the projection matrix
            Dim fAspect As Single = Device.PresentationParameters.BackBufferWidth / CSng(Device.PresentationParameters.BackBufferHeight)
            Device.Transform.Projection = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 10000.0F)

            ' Set the app view matrix for normal viewing
            Dim vEyePt As New Vector3(0.0F, -5.0F, -10.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Device.Transform.View = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)

            ' Create a directional light. (Use yellow light to distinguish from
            ' vertex shader case.)
            GraphicsUtility.InitLight(Device.Lights(0), LightType.Directional, -0.5F, -1.0F, 1.0F)
            Device.Lights(0).Diffuse = System.Drawing.Color.Yellow
            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True
            RenderState.Lighting = True
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub InvalidateDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            If Not (shader Is Nothing) Then
                shader.Dispose()
            End If
        End Sub 'InvalidateDeviceObjects




        '/ <summary>
        '/ Called when the app is exiting, or the device is being changed, this 
        '/ function deletes any device-dependent objects.
        '/ </summary>
        Protected Overrides Sub DeleteDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            blendObject.Dispose()
        End Sub 'DeleteDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            If vertexProcessingType = vertexProcessingType.PureHardware Then
                Return False ' GetTransform doesn't work on PUREDEVICE
            End If
            ' Check that the device supports at least one of the two techniques
            ' used in this sample: either a vertex shader, or at least two blend
            ' matrices and a directional light.
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.Mixed Then
                If caps.VertexShaderVersion.Major >= 1 Then
                    Return True
                End If
            Else
                ' Software vertex processing always supports vertex shaders
                Return True
            End If

            ' Check that the device can blend vertices with at least two matrices
            ' (Software can always do up to 4 blend matrices)
            If caps.MaxVertexBlendMatrices < 2 Then
                Return False
            End If
            ' If this is a TnL device, make sure it supports directional lights
            If vertexProcessingType = vertexProcessingType.Hardware Or vertexProcessingType = vertexProcessingType.Mixed Then
                If Not caps.VertexProcessingCaps.SupportsDirectionAllLights Then
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
End Namespace 'VertexBlendSample