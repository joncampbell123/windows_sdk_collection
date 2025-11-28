Imports System
Imports System.Windows.Forms
Imports System.Drawing
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectDraw


Namespace FullScreenDialog
    _
    '/ <summary>
    '/ Summary description for FullScreenDialog.
    '/ </summary>
    Public Structure Sprite
        ' This structure describes the sprites the app will use.
        Public posX As Single
        Public posY As Single
        Public velX As Single
        Public velY As Single
    End Structure 'Sprite
    _

    Public Class FullScreenDialog
        Inherits System.Windows.Forms.Form
        ' Declare the class scope variables here.
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing

        Private textHelp As String = "Press escape to quit. Press F1 to bring up the dialog." ' Text to display.
        Private clip As Clipper = Nothing ' Clipper to prevent the app from drawing over the top of the dialog.
        Private needRestore As Boolean = False ' Flag to determine the app needs to restore the surfaces.
        Private surfaceLogo As Surface = Nothing ' The surface that contains the sprites bitmap.
        Private tickLast As Integer = 0 ' Holds the value of the last call to GetTick.
        Private surfaceText As Surface = Nothing ' The text to draw on the screen.
        Private Const numSprites As Integer = 25 ' Number of sprites to draw.
        Private displayDevice As Device = Nothing ' The Direct Draw Device.
        Private nameFile As String = DXUtil.SdkMediaPath + "directx.bmp"
        Private sprites(numSprites) As Sprite ' Array of sprites structs.
        Private diameterSprite As Integer = 48 ' Sprites width & height.
        Private front As Surface = Nothing ' The front surface.
        Private screenHeight As Integer = 480 ' Screen height.
        Private back As Surface = Nothing ' The back surface.
        Private screenWidth As Integer = 640 ' Screen width.
        Private Shared dialogSample As DialogSampleForm

        ' Bitmap to load.
        Public Shared Sub Main()
            '-----------------------------------------------------------------------------
            ' Name: Main()
            ' Desc: Entry point for the application.
            '-----------------------------------------------------------------------------
            Dim app As New FullScreenDialog()
            Application.Exit()
        End Sub 'Main

        Public Sub New()
            '-----------------------------------------------------------------------------
            ' Name: FullScreenDialog()
            ' Desc: Constructor for the main class.
            '-----------------------------------------------------------------------------
            '
            ' Required for Windows Form Designer support.
            '
            InitializeComponent()

            ' Create a new DrawDevice.
            InitDirectDraw()

            dialogSample = New DialogSampleForm() ' Create a new dialog box to display.
            Me.AddOwnedForm(dialogSample) ' Add the dialog as a child to this form.
            dialogSample.TopMost = True ' Makes the window stay on top of everything else.
            dialogSample.Show() ' Display the dialog.
            ' Keep looping until told to quit.
            While Created
                ProcessNextFrame() ' Process and draw the next frame.
                Application.DoEvents() ' Make sure the app has time to process messages.
            End While
        End Sub 'New

        Sub RestoreSurfaces()
            '-----------------------------------------------------------------------------
            ' Name: RestoreSurfaces()
            ' Desc: Restore all the surfaces, and redraw the sprites surfaces.
            '-----------------------------------------------------------------------------
            Dim description As New SurfaceDescription()

            displayDevice.RestoreAllSurfaces()

            ' No need to re-create the surface, just re-draw it.
            surfaceText.ColorFill(Color.Black)
            surfaceText.DrawText(0, 0, textHelp, True)

            ' Release and re-load the sprites bitmap.
            surfaceLogo.Dispose()
            surfaceLogo = Nothing
            surfaceLogo = New Surface(nameFile, description, displayDevice)
            Dim key As New ColorKey()
            surfaceLogo.SetColorKey(ColorKeyFlags.SourceDraw, key)

            Return
        End Sub 'RestoreSurfaces


        Private Sub UpdateSprite(ByRef s As Sprite, ByVal deltaTime As Single)
            '-----------------------------------------------------------------------------
            ' Name: UpdateSprite()
            ' Desc: Move the sprites around and make it bounce based on how much time 
            '       has passed
            '-----------------------------------------------------------------------------
            ' Update the sprites position
            s.posX += s.velX * deltaTime
            s.posY += s.velY * deltaTime

            ' Clip the position, and bounce if it hits the edge.
            If s.posX < 0.0F Then
                s.posX = 0
                s.velX = -s.velX
            End If

            If s.posX >= screenWidth - diameterSprite Then
                s.posX = screenWidth - 1 - diameterSprite
                s.velX = -s.velX
            End If

            If s.posY < 0 Then
                s.posY = 0
                s.velY = -s.velY
            End If

            If s.posY > screenHeight - diameterSprite Then
                s.posY = screenHeight - 1 - diameterSprite
                s.velY = -s.velY
            End If
        End Sub 'UpdateSprite


        Private Sub ProcessNextFrame()
            '-----------------------------------------------------------------------------
            ' Name: ProcessNextFrame()
            ' Desc: Move the sprites, blt them to the back buffer, then 
            '       flips the back buffer to the primary buffer
            '-----------------------------------------------------------------------------
            ' Figure how much time has passed since the last time.
            Dim tickCurrent As Integer = Environment.TickCount
            Dim tickDifference As Integer = tickCurrent - tickLast

            ' Don't update if no time has passed.
            If 0 = tickDifference Then
                Return
            End If
            tickLast = tickCurrent

            ' Move the sprites according to how much time has passed.
            Dim i As Integer
            For i = 0 To numSprites - 1
                UpdateSprite(sprites(i), tickDifference / 1000.0F)
            Next i
            'Draw the sprites and text to the screen.
            DisplayFrame()
        End Sub 'ProcessNextFrame


        Private Sub DisplayFrame()
            '-----------------------------------------------------------------------------
            ' Name: DisplayFrame()
            ' Desc: Draw the sprites and text to the screen.
            '-----------------------------------------------------------------------------
            If Nothing Is front Then
                Return
            End If
            If False = displayDevice.TestCooperativeLevel() Then
                needRestore = True
                Return
            End If

            If True = needRestore Then
                needRestore = False
                ' The surfaces were lost so restore them .
                RestoreSurfaces()
            End If

            ' Fill the back buffer with black.
            back.ColorFill(0)

            ' Draw the help text on the backbuffer.
            back.DrawFast(10, 10, surfaceText, DrawFastFlags.DoNotWait)

            ' Draw all the sprites onto the back buffer using color keying,
            ' ignoring errors until the flip. Note that all of these sprites 
            ' use the same DirectDraw surface.
            Dim i As Integer
            For i = 0 To numSprites - 1
                back.DrawFast(CInt(sprites(i).posX), CInt(sprites(i).posY), surfaceLogo, DrawFastFlags.DoNotWait Or DrawFastFlags.SourceColorKey)
            Next i
            front.Draw(back, DrawFlags.DoNotWait)
        End Sub 'DisplayFrame


        Private Sub InitDirectDraw()
            '-----------------------------------------------------------------------------
            ' Name: InitDirectDraw()
            ' Desc: Initializes DirectDraw and all the surfaces to be used.
            '-----------------------------------------------------------------------------
            Dim description As New SurfaceDescription() ' Describes a surface.
            Dim randomNumber As New Random() ' Random number for position and velocity.
            displayDevice = New Device() ' Create a new DirectDrawDevice.            
            displayDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.FullscreenExclusive) ' Set the cooperative level.
            displayDevice.SetDisplayMode(screenWidth, screenHeight, 16, 0, False) ' Set the display mode width and height, and 8 bit color depth.
            description.SurfaceCaps.Flip = True
            description.SurfaceCaps.Complex = True
            description.SurfaceCaps.PrimarySurface = True
            description.BackBufferCount = 1 ' Create 1 backbuffer.
            front = New Surface(description, displayDevice) ' Create the surface using the description above.
            Dim caps As New SurfaceCaps()
            caps.BackBuffer = True ' Caps of the surface.
            back = front.GetAttachedSurface(caps) ' Get the attached surface that matches the caps, which will be the backbuffer.
            clip = New Clipper(displayDevice) ' Create a clipper object.
            clip.Window = Me ' Set the clippers window handle to the window handle of the main window.
            front.Clipper = clip ' Tell the front surface to use the clipper object.
            description.Clear() ' Clear out the SurfaceDescription structure.
            surfaceLogo = New Surface(nameFile, description, displayDevice) ' Create the sprites bitmap surface.
            description.Clear()

            description.Width = 400 ' Set the width and height of the surface.
            description.Height = 20
            description.SurfaceCaps.OffScreenPlain = True

            surfaceText = New Surface(description, displayDevice) ' Create the surface using the above description.
            surfaceText.ForeColor = Color.White
            surfaceText.ColorFill(Color.Black)
            surfaceText.DrawText(0, 0, textHelp, True)

            Dim ck As New ColorKey() ' Create a new colorkey.
            surfaceLogo.SetColorKey(ColorKeyFlags.SourceDraw, ck) ' Set the colorkey to the bitmap surface. 0 is used for the colorkey, which is what the ColorKey struct is initialized to.
            Dim i As Integer
            For i = 0 To numSprites - 1
                ' Set the sprites's position and velocity
                sprites(i).posX = randomNumber.Next() Mod screenWidth
                sprites(i).posY = randomNumber.Next() Mod screenHeight

                sprites(i).velX = 500.0F * randomNumber.Next() / Int32.MaxValue - 250.0F
                sprites(i).velY = 500.0F * randomNumber.Next() / Int32.MaxValue - 250.0F
            Next i
        End Sub 'InitDirectDraw


        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            '/ <summary>
            '/ Clean up any resources being used.
            '/ </summary>
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose

        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            ' 
            ' FullScreenDialog
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(208, 69)
            Me.ForeColor = System.Drawing.Color.Black
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
            Me.Name = "FullScreenDialog"
            Me.Text = "FullScreenDialog"
        End Sub 'InitializeComponent

        Private Sub FullScreenDialog_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
            '-----------------------------------------------------------------------------
            ' Name: FullScreenDialog_KeyUp()
            ' Desc: Processes key up events.
            '-----------------------------------------------------------------------------
            If Keys.Escape = e.KeyCode Then
                Close()
            ElseIf Keys.F1 = e.KeyCode And dialogSample.IsDisposed Then
                dialogSample = New DialogSampleForm() ' Create a new dialog window.
                Me.AddOwnedForm(dialogSample) ' Add the dialog as a child to this form.
                dialogSample.TopMost = True ' Makes the window stay on top of everything else.
                dialogSample.Show() ' Display the dialog.
            End If
        End Sub 'FullScreenDialog_KeyUp
    End Class 'FullScreenDialog
End Namespace 'FullScreenDialog