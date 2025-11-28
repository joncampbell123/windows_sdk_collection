'-----------------------------------------------------------------------------
' File: Text3D.vb
'
' Desc: Example code showing how to do text in a Direct3D scene.
'
' Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace Text3D
    _


    '/ <summary>
    '/ Application class. The base class (GraphicsSample) provides the generic 
    '/ functionality needed in all Direct3D samples. MyGraphicsSample adds 
    '/ functionality specific to this sample program.
    '/ </summary>
    Public Class MyGraphicsSample
        Inherits GraphicsSample



        Private drawingFont As GraphicsFont = Nothing ' Font for drawing text
        Private statsFont As GraphicsFont = Nothing ' Font for drawing text (Stats)
        Private mesh3DText As Mesh = Nothing ' Mesh to draw 3d text
        Private otherFont As D3DXFont = Nothing ' The D3DX Font object
        Private ourFont As New System.Drawing.Font("Arial", 18, System.Drawing.FontStyle.Bold) ' Font we use 
        Private objectOne As New Matrix()
        Private objectTwo As New Matrix()

        Private mnuOptions As System.Windows.Forms.MenuItem
        Private mnuChangeFont As System.Windows.Forms.MenuItem


        '/ <summary>
        '/ Application constructor. Sets attributes for the app.
        '/ </summary>
        Public Sub New()
            ' Set the window text
            Me.Text = "Text3D: Text in a 3D scene"
            ' Load the icon from our resources
            'this.Icon = new System.Drawing.Icon( this.GetType(), "directx.ico" );
            ' Create our font objects
            statsFont = New GraphicsFont("Arial", System.Drawing.FontStyle.Bold)
            drawingFont = New GraphicsFont(ourFont)
            otherFont = New D3DXFont(ourFont)
            enumerationSettings.AppUsesDepthBuffer = True

            ' Add our new menu options
            mnuOptions = New System.Windows.Forms.MenuItem("&Options")
            mnuChangeFont = New System.Windows.Forms.MenuItem("&Change Font...")
            ' Add to the main menu screen
            mnuMain.MenuItems.Add(Me.mnuOptions)
            mnuOptions.MenuItems.Add(mnuChangeFont)
            mnuChangeFont.Shortcut = System.Windows.Forms.Shortcut.CtrlO
            mnuChangeFont.ShowShortcut = True
            AddHandler mnuChangeFont.Click, AddressOf Me.ChangeFontClick
        End Sub 'New




        Private Sub ChangeFontClick(ByVal sender As Object, ByVal e As EventArgs)
            Dim dlg As New System.Windows.Forms.FontDialog()

            ' Show the dialog
            dlg.FontMustExist = True
            dlg.Font = ourFont

            If dlg.ShowDialog(Me) = System.Windows.Forms.DialogResult.OK Then ' We selected something
                ourFont = dlg.Font
                If Not (drawingFont Is Nothing) Then
                    drawingFont.Dispose(Nothing, Nothing)
                End If ' Set the new font
                drawingFont = New GraphicsFont(ourFont)
                drawingFont.InitializeDeviceObjects(Device)

                If Not (otherFont Is Nothing) Then
                    otherFont.Dispose()
                End If
                otherFont = New D3DXFont(ourFont)
                otherFont.InitializeDeviceObjects(Device)

                ' Create our 3d text mesh
                mesh3DText = Mesh.TextFromFont(Device, ourFont, "Mesh.TextFromFont", 0.001F, 0.4F)
            End If
        End Sub 'ChangeFontClick



        '/ <summary>
        '/ Called once per frame, the call is the entry point for animating the scene.
        '/ </summary>
        Protected Overrides Sub FrameMove()
            ' Setup five rotation matrices (for rotating text strings)
            Dim vAxis1 As New Vector3(1.0F, 2.0F, 0.0F)
            Dim vAxis2 As New Vector3(2.0F, 1.0F, 0.0F)
            objectOne = Matrix.RotationAxis(vAxis1, appTime / 2.0F)
            objectTwo = Matrix.RotationAxis(vAxis2, appTime / 2.0F)

            ' Add some translational values to the matrices
            objectOne.M41 = 1.0F
            objectOne.M42 = 6.0F
            objectOne.M43 = 20.0F
            objectTwo.M41 = -4.0F
            objectTwo.M42 = -1.0F
            objectTwo.M43 = 0.0F
        End Sub 'FrameMove





        '/ <summary>
        '/ Called once per frame, the call is the entry point for 3d rendering. This 
        '/ function sets up render states, clears the viewport, and renders the scene.
        '/ </summary>
        Protected Overrides Sub Render()
            ' Clear the viewport
            Device.Clear(ClearFlags.Target Or ClearFlags.ZBuffer, Color.Black, 1.0F, 0)

            Device.BeginScene()

            ' Output statistics
            statsFont.DrawText(2, 1, Color.Yellow, frameStats)
            statsFont.DrawText(2, 20, Color.Yellow, deviceStats)

            ' Draw GraphicsFont in 2D (red)
            drawingFont.DrawText(60, 100, Color.Red, "GraphicsFont.DrawText")

            ' Draw GraphicsFont scaled in 2D (cyan)
            drawingFont.DrawTextScaled(-1.0F, 0.8F, 0.5F, 0.1F, 0.1F, Color.Cyan, "GraphicsFont.DrawTextScaled") ' position
            ' scale

            ' Draw GraphicsFont in 3D (green)
            Dim mtrl As Material = GraphicsUtility.InitMaterial(Color.Green)
            Device.Material = mtrl
            Device.Transform.World = objectOne
            drawingFont.Render3DText("GraphicsFont.Render3DText", GraphicsFont.RenderFlags.Centered Or GraphicsFont.RenderFlags.TwoSided Or GraphicsFont.RenderFlags.Filtered)

            ' Draw D3DXFont mesh in 3D (blue)
            If Not (mesh3DText Is Nothing) Then
                Dim mtrl3d As Material = GraphicsUtility.InitMaterial(Color.Blue)
                Device.Material = mtrl3d
                Device.Transform.World = objectTwo
                mesh3DText.DrawSubset(0)
            End If

            ' Draw D3DXFont in 2D (purple)
            otherFont.BeginText()
            otherFont.DrawText(60, 200, Color.Purple.ToArgb(), "D3DXFont.DrawText")
            otherFont.EndText()

            Device.EndScene()
        End Sub 'Render





        '/ <summary>
        '/ Initialize scene objects.
        '/ </summary>
        Protected Overrides Sub InitializeDeviceObjects()
            ' Initialize all of the fonts
            drawingFont.InitializeDeviceObjects(Device)
            statsFont.InitializeDeviceObjects(Device)
            otherFont.InitializeDeviceObjects(Device)
        End Sub 'InitializeDeviceObjects





        '/ <summary>
        '/ 
        '/ </summary>
        Protected Overrides Sub RestoreDeviceObjects(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ' Restore the textures
            Device.TextureState(0).ColorArgument1 = TextureArgument.TextureColor
            Device.TextureState(0).ColorArgument2 = TextureArgument.Diffuse
            Device.TextureState(0).ColorOperation = TextureOperation.Modulate
            Device.SamplerState(0).MinFilter = TextureFilter.Linear
            Device.SamplerState(0).MagFilter = TextureFilter.Linear

            Device.RenderState.ZBufferEnable = True
            Device.RenderState.DitherEnable = True
            Device.RenderState.SpecularEnable = True
            Device.RenderState.Lighting = True
            Device.RenderState.Ambient = System.Drawing.Color.FromArgb(&H80808080)

            GraphicsUtility.InitLight(Device.Lights(0), LightType.Directional, 10.0F, -10.0F, 10.0F)
            Device.Lights(0).Commit()
            Device.Lights(0).Enabled = True

            ' Set the transform matrices
            Dim vEyePt As New Vector3(0.0F, -5.0F, -10.0F)
            Dim vLookatPt As New Vector3(0.0F, 0.0F, 0.0F)
            Dim vUpVec As New Vector3(0.0F, 1.0F, 0.0F)
            Dim matWorld, matView, matProj As Matrix

            matWorld = Matrix.Identity
            matView = Matrix.LookAtLH(vEyePt, vLookatPt, vUpVec)
            Dim fAspect As Single = device.PresentationParameters.BackBufferWidth / CSng(device.PresentationParameters.BackBufferHeight)
            matProj = Matrix.PerspectiveFovLH(CSng(Math.PI) / 4, fAspect, 1.0F, 100.0F)

            Device.Transform.World = matWorld
            Device.Transform.View = matView
            Device.Transform.Projection = matProj

            If Not (mesh3DText Is Nothing) Then
                mesh3DText.Dispose()
            End If
            ' Create our 3d text mesh
            mesh3DText = Mesh.TextFromFont(Device, ourFont, "Mesh.TextFromFont", 0.001F, 0.4F)
        End Sub 'RestoreDeviceObjects




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
End Namespace 'Text3D