'-----------------------------------------------------------------------------
' File: DotProduct3.vb
'
' Desc: D3D sample showing how to do bumpmapping using the DotProduct3 
'       texture operation.
' 
' Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace BumpUnderwaterSample
    _



    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample
       _

        '/ <summary>
        '/ Custom Bump vertex
        '/ </summary>
        Public Structure DotVertex
            Public p As Vector3
            Public diffuse As Integer
            Public specular As Integer
            Public tu1, tv1 As Single

            Public Const Format As VertexFormats = VertexFormats.Position Or VertexFormats.Diffuse Or VertexFormats.Specular Or VertexFormats.Texture1
        End Structure 'DotVertex



        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        ' Scene
        Private quadVerts(4) As DotVertex ' Quad vertex buffer data
        Private customNormalMap As Texture = Nothing
        Private fileBasedNormalMap As Texture = Nothing
        Private lightVector As Vector3

        Private isUsingFileBasedTexture As Boolean = False
        Private isShowingNormalMap As Boolean = False


        ' Menu items for extra actions the sample can perform
        Private mnuOptions As MenuItem = Nothing
        Private mnuFileTexture As MenuItem = Nothing
        Private mnuCustomTexture As MenuItem = Nothing
        Private mnuBreak As MenuItem = Nothing
        Private mnuShowNormal As MenuItem = Nothing




        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "DotProduct3: BumpMapping Technique"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            drawingFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            enumerationSettings.AppUsesDepthBuffer = False

            ' Add the options menu to the main menu
            mnuOptions = New MenuItem("&Options")
            Me.mnuMain.MenuItems.Add(mnuOptions)

            ' Now create the rest of the menu items
            mnuFileTexture = New MenuItem("Use file-based texture")
            mnuFileTexture.Shortcut = Shortcut.CtrlF
            mnuFileTexture.ShowShortcut = True
            AddHandler mnuFileTexture.Click, AddressOf Me.FileTextureClicked

            mnuCustomTexture = New MenuItem("Use custom texture")
            mnuCustomTexture.Shortcut = Shortcut.CtrlC
            mnuCustomTexture.ShowShortcut = True
            AddHandler mnuCustomTexture.Click, AddressOf Me.CustomTextureClicked

            mnuBreak = New MenuItem("-")
            mnuShowNormal = New MenuItem("Show normal map")
            mnuShowNormal.Shortcut = Shortcut.CtrlN
            mnuShowNormal.ShowShortcut = True
            AddHandler mnuShowNormal.Click, AddressOf Me.ShowNormalClicked

            ' Now add the new items to the option menu
            mnuOptions.MenuItems.AddRange(New MenuItem() {mnuFileTexture, mnuCustomTexture, mnuBreak, mnuShowNormal})
        End Sub 'New




        '/ <summary>
        '/ Turns a normalized vector into RGBA form. Used to encode vectors into a height map. 
        '/ </summary>
        '/ <param name="v">Normalized Vector</param>
        '/ <param name="height">Height</param>
        '/ <returns>RGBA Form of the vector</returns>
        Private Function VectorToRgba(ByVal v As Vector3, ByVal height As Single) As Integer
            Dim r As Integer = CInt(127.0F * v.X + 128.0F)
            Dim g As Integer = CInt(127.0F * v.Y + 128.0F)
            Dim b As Integer = CInt(127.0F * v.Z + 128.0F)
            Dim a As Integer = CInt(255.0F * Height)

            Return DXHelp.ShiftLeft(a, 24) + DXHelp.ShiftLeft(r, 16) + DXHelp.ShiftLeft(g, 8) + DXHelp.ShiftLeft(b, 0)
        End Function 'VectorToRgba




        '/ <summary>
        '/ Initializes a vertex
        '/ </summary>
        '/ <returns>The initialized vertex</returns>
        Private Function InitializeVertex(ByVal x As Single, ByVal y As Single, ByVal z As Single, ByVal tu As Single, ByVal tv As Single) As DotVertex
            Dim vertex As New DotVertex()
            Dim v As New Vector3(1, 1, 1)
            v.Normalize()

            vertex.p = New Vector3(x, y, z)
            vertex.diffuse = VectorToRgba(v, 1.0F)
            vertex.specular = &H40400000
            vertex.tu1 = tu
            vertex.tv1 = tv
            Return vertex
        End Function 'InitializeVertex




        '-----------------------------------------------------------------------------
        ' Name: OneTimeSceneInitialization()
        ' Desc: Called during initial app startup, this function performs all the
        '       permanent initialization.
        '-----------------------------------------------------------------------------
        Protected Overrides Sub OneTimeSceneInitialization()
            quadVerts(0) = InitializeVertex(-1.0F, -1.0F, -1.0F, 0.0F, 0.0F)
            quadVerts(1) = InitializeVertex(1.0F, -1.0F, -1.0F, 1.0F, 0.0F)
            quadVerts(2) = InitializeVertex(-1.0F, 1.0F, -1.0F, 0.0F, 1.0F)
            quadVerts(3) = InitializeVertex(1.0F, 1.0F, -1.0F, 1.0F, 1.0F)

            lightVector = New Vector3(0.0F, 0.0F, 1.0F)
        End Sub 'OneTimeSceneInitialization




        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            If System.Windows.Forms.Form.ActiveForm Is Me Then
                Dim pt As System.Drawing.Point = Me.PointToClient(System.Windows.Forms.Cursor.Position)

                lightVector.X = -(2.0F * pt.X / device.PresentationParameters.BackBufferWidth - 1)
                lightVector.Y = -(2.0F * pt.Y / device.PresentationParameters.BackBufferHeight - 1)
                lightVector.Z = 0.0F

                If lightVector.Length() > 1.0F Then
                    lightVector.Normalize()
                Else
                    lightVector.Z = CSng(Math.Sqrt((1.0F - lightVector.X * lightVector.X - lightVector.Y * lightVector.Y)))
                End If
            End If
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target, System.Drawing.Color.Black.ToArgb(), 1.0F, 0)

            Device.BeginScene()

            ' Store the light vector, so it can be referenced in TextureArgument.TFactor
            Dim factor As Integer = VectorToRgba(lightVector, 0.0F)
            Device.RenderState.TextureFactor = factor

            ' Modulate the texture (the normal map) with the light vector (stored
            ' above in the texture factor)
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorOperation = TextureOperation.DotProduct3
            Device.TextureState(0).ColorArgument2 = TextureArgument.TFactor

            ' If user wants to see the normal map, override the above renderstates and
            ' simply show the texture
            If isShowingNormalMap Then
                Device.TextureState(0).ColorOperation = TextureOperation.SelectArg1
            End If
            ' Select which normal map to use
            If isUsingFileBasedTexture Then
                Device.SetTexture(0, fileBasedNormalMap)
            Else
                Device.SetTexture(0, customNormalMap)
            End If
            ' Draw the bumpmapped quad
            Device.VertexFormat = DotVertex.Format
            Device.DrawUserPrimitives(PrimitiveType.TriangleStrip, 2, quadVerts)

            ' Output statistics
            drawingFont.DrawText(2, 1, System.Drawing.Color.Yellow, frameStats)
            drawingFont.DrawText(2, 20, System.Drawing.Color.Yellow, deviceStats)

            Device.EndScene()
        End Sub 'Render



        '/ <summary>
        '/ Create a file based normap map
        '/ </summary>
        Sub CreateFileBasedNormalMap()
            Dim fileBasedSource As Texture = Nothing
            Try
                ' Load the texture from a file
                fileBasedSource = GraphicsUtility.CreateTexture(Device, "earthbump.bmp", Format.A8R8G8B8)
                Dim desc As SurfaceDescription = fileBasedSource.GetLevelDescription(0)
                fileBasedNormalMap = New Texture(Device, desc.Width, desc.Height, fileBasedSource.LevelCount, 0, Format.A8R8G8B8, Pool.Managed)
                TextureLoader.ComputeNormalMap(fileBasedNormalMap, fileBasedSource, 0, Channel.Red, 1.0F)
            Catch
                Dim e As New MediaNotFoundException()
                HandleSampleException(e, ApplicationMessage.ApplicationMustExit)
                Throw e
            Finally
                If Not (fileBasedSource Is Nothing) Then
                    fileBasedSource.Dispose()
                End If
            End Try
        End Sub 'CreateFileBasedNormalMap



        Sub CreateCustomNormalMap()
            Dim localHeight As Integer = 512
            Dim localWidth As Integer = 512

            ' Create a 32-bit texture for the custom normal map
            customNormalMap = New Texture(Device, localWidth, localHeight, 1, 0, Format.A8R8G8B8, Pool.Managed)

            Try
                ' We use the 'New Integer() { } format here so VB doesn't get confused and think we want the ByRef Pitch
                ' variable instead of the ranks
                Dim data As Integer(,) = customNormalMap.LockRectangle(GetType(Integer), 0, 0, New Integer() {localWidth, localHeight})
                ' Fill each pixel
                Dim j As Integer
                For j = 0 To localHeight - 1
                    Dim i As Integer
                    For i = 0 To localWidth - 1
                        Dim xp As Single = 5.0F * i / (localWidth - 1)
                        Dim yp As Single = 5.0F * j / (localHeight - 1)
                        Dim x As Single = 2 * (xp - CSng(Math.Floor(xp))) - 1
                        Dim y As Single = 2 * (yp - CSng(Math.Floor(yp))) - 1
                        Dim z As Single = CSng(Math.Sqrt((1.0F - x * x - y * y)))

                        ' Make image of raised circle. Outside of circle is gray
                        If x * x + y * y <= 1.0F Then
                            Dim vector As New Vector3(x, y, z)
                            data(j, i) = VectorToRgba(vector, 1.0F)
                        Else
                            data(j, i) = &H80808080
                        End If
                    Next i
                Next j
            Catch eee As Exception
                Console.WriteLine(eee.ToString())
            Finally
                ' Make sure to unlock the rectangle even if there's an error
                customNormalMap.UnlockRectangle(0)
            End Try
        End Sub 'CreateCustomNormalMap



        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            drawingFont.InitializeDeviceObjects(Device)
            CreateFileBasedNormalMap()
            CreateCustomNormalMap()

            ' Set the menu states
            mnuCustomTexture.Checked = Not isUsingFileBasedTexture
            mnuFileTexture.Checked = isUsingFileBasedTexture
            mnuShowNormal.Checked = isShowingNormalMap
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, 0.0F, 2.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim matWorld, matView, matProj As Matrix

            matWorld = Matrix.Identity
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            Dim aspect As Single = CSng(device.PresentationParameters.BackBufferWidth) / CSng(device.PresentationParameters.BackBufferHeight)
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, aspect, 1.0F, 500.0F)
            device.Transform.World = matWorld
            device.Transform.View = matView
            device.Transform.Projection = matProj

            ' Set any appropiate state
            device.RenderState.Lighting = False
            device.SamplerState(0).MinFilter = TextureFilter.Linear
            device.SamplerState(0).MagFilter = TextureFilter.Linear
        End Sub 'RestoreDeviceObjects





        '/ <summary>
        '/ Called during device initialization, this code checks the device for some 
        '/ minimum set of capabilities
        '/ </summary>
        Protected Overrides Function ConfirmDevice(ByVal caps As Microsoft.DirectX.Direct3D.Caps, ByVal vertexProcessingType As VertexProcessingType, ByVal adapterFmt As Microsoft.DirectX.Direct3D.Format, ByVal backBufferFmt As Microsoft.DirectX.Direct3D.Format) As Boolean
            ' All we need is the DotProduct texture operation
            Return caps.TextureOperationCaps.SupportsDotProduct3
        End Function 'ConfirmDevice




        '/ <summary>
        '/ Menu event handlers
        '/ </summary>
        Private Sub FileTextureClicked(ByVal sender As Object, ByVal e As EventArgs)
            isUsingFileBasedTexture = True
            ' Set the menu states
            mnuCustomTexture.Checked = Not isUsingFileBasedTexture
            mnuFileTexture.Checked = isUsingFileBasedTexture
        End Sub 'FileTextureClicked

        Private Sub CustomTextureClicked(ByVal sender As Object, ByVal e As EventArgs)
            isUsingFileBasedTexture = False
            ' Set the menu states
            mnuCustomTexture.Checked = Not isUsingFileBasedTexture
            mnuFileTexture.Checked = isUsingFileBasedTexture
        End Sub 'CustomTextureClicked

        Private Sub ShowNormalClicked(ByVal sender As Object, ByVal e As EventArgs)
            isShowingNormalMap = Not isShowingNormalMap
            ' Set the menu states
            mnuShowNormal.Checked = isShowingNormalMap
        End Sub 'ShowNormalClicked



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
End Namespace 'BumpUnderwaterSample