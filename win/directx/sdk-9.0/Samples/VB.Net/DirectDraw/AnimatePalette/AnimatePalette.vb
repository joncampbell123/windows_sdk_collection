Imports System
Imports System.Windows.Forms
Imports System.Drawing
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectDraw

Namespace AnimatePalette
    _
    '/ <summary>
    '/ Summary description for AnimatePalette.
    '/ </summary>
    '/ 
    ' This structure describes the sprite the app will use.
    Public Structure Sprite
        Public posX As Single
        Public posY As Single
        Public velX As Single
        Public velY As Single
    End Structure 'Sprite
    _

    Public Class AnimatePalette
        Inherits System.Windows.Forms.Form
        ' Declare the class scope variables here.
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing

        Private indexBlack As Integer = 144 ' Index of the palette entry that contains black.
        Private indexWhite As Integer = 0 ' Index of the palette entry that contains black.
        Private spriteDiameter As Integer = 48 ' Sprites width & height.
        Private Const numSprites As Integer = 25 ' Number of sprites to draw.
        Private screenWidth As Integer = 640 ' Screen width.
        Private screenHeight As Integer = 480 ' Screen height.
        Private displayDevice As Device = Nothing ' The Direct Draw Device.
        Private front As Surface = Nothing ' The front surface.
        Private back As Surface = Nothing ' The back surface.
        Private surfaceLogo As Surface = Nothing ' The surface that contains the sprite bitmap.
        Private surfaceText As Surface = Nothing ' The text to draw on the screen.
        Private pal As Palette = Nothing ' The palette the front surface will use.
        Private tickLast As Integer = 0 ' Holds the value of the last call to GetTick.
        Private restoreNeeded As Boolean = False ' Flag to determine the app needs to restore the surfaces.
        Private sprite(numSprites) As sprite ' Array of sprite structs.
        Private pe(256) As PaletteEntry ' Palette entry array.
        Private bitmapFileName As String = DXUtil.SdkMediaPath + "directx.bmp" ' Bitmap to load.
        Private textHelp As String = "Press Escape to quit."
        ' Text to display.
        '-----------------------------------------------------------------------------
        ' Name: Main()
        ' Desc: Entry point for the application.
        '-----------------------------------------------------------------------------
        Public Shared Sub Main()
            Dim app As New AnimatePalette()
            Application.Exit()
        End Sub 'Main


        '-----------------------------------------------------------------------------
        ' Name: AnimatePalette()
        ' Desc: Constructor for the main class.
        '-----------------------------------------------------------------------------
        Public Sub New()
            '
            ' Required for Windows Form Designer support.
            '
            InitializeComponent()

            ' Make sure to use the arrow cursor.
            Cursor = Cursors.Arrow

            ' Create a new DrawDevice.
            InitializeDirectDraw()

            ' Keep looping until app quits.
            While Created
                ProcessNextFrame() ' Process and draw the next frame.
                Application.DoEvents() ' Make sure the app has time to process messages.
            End While
        End Sub 'New

        '-----------------------------------------------------------------------------
        ' Name: RestoreSurfaces()
        ' Desc: Restore all the surfaces, and redraw the sprite surfaces.
        '-----------------------------------------------------------------------------
        Sub RestoreSurfaces()
            Dim description As New SurfaceDescription()

            displayDevice.RestoreAllSurfaces()

            ' No need to re-create the surface, just re-draw it.
            surfaceText.ColorFill(indexBlack)
            surfaceText.DrawText(0, 0, textHelp, False)

            ' We need to release and re-load, and set the palette again to 
            ' redraw the bitmap on the surface.  Otherwise, GDI will not 
            ' draw the bitmap on the surface with the correct palette.
            pal.Dispose()
            pal = Nothing
            pal = New Palette(displayDevice, bitmapFileName)
            front.Palette = pal

            ' Release and re-load the sprite bitmap.
            surfaceLogo.Dispose()
            surfaceLogo = New Surface(bitmapFileName, description, displayDevice)
            Dim ck As New ColorKey()

            surfaceLogo.SetColorKey(ColorKeyFlags.SourceDraw, ck)

            Return
        End Sub 'RestoreSurfaces


        '-----------------------------------------------------------------------------
        ' Name: UpdateSprite()
        ' Desc: Move the sprite around and make it bounce based on how much time 
        '       has passed
        '-----------------------------------------------------------------------------
        Private Sub UpdateSprite(ByRef s As sprite, ByVal TimeDelta As Single)
            ' Update the sprite position
            s.posX += s.velX * TimeDelta
            s.posY += s.velY * TimeDelta

            ' Clip the position, and bounce if it hits the edge.
            If s.posX < 0.0F Then
                s.posX = 0
                s.velX = -s.velX
            End If

            If s.posX >= screenWidth - spriteDiameter Then
                s.posX = screenWidth - 1 - spriteDiameter
                s.velX = -s.velX
            End If

            If s.posY < 0 Then
                s.posY = 0
                s.velY = -s.velY
            End If

            If s.posY > screenHeight - spriteDiameter Then
                s.posY = screenHeight - 1 - spriteDiameter
                s.velY = -s.velY
            End If
        End Sub 'UpdateSprite

        '-----------------------------------------------------------------------------
        ' Name: CyclePalette()
        ' Desc: Cycle the palette colors 2 through 37 - this 
        '       will change the color for 'X' in the sprite.
        '-----------------------------------------------------------------------------
        Private Sub CyclePalette()
            Dim i As Integer
            For i = 2 To 36
                ' This just does something interesting but simple by shifting each 
                ' color component independently .
                pe(i).Blue += 8
                pe(i).Red += 4
                pe(i).Green += 2
            Next i

            ' Wait until the screen is synchronzied at a vertical-blank interval.
            ' This may fail if the device is not hardware accelerated.
            Try
                displayDevice.WaitForVerticalBlank(WaitVbFlags.BlockBegin)
            Catch
            End Try
            ' Now that we are synchronzied at a vertical-blank 
            ' interval, update the palette.
            pal.SetEntries(0, pe)

            Return
        End Sub 'CyclePalette


        '-----------------------------------------------------------------------------
        ' Name: ProcessNextFrame()
        ' Desc: Move the sprites, blt them to the back buffer, then 
        '       flips the back buffer to the primary buffer
        '-----------------------------------------------------------------------------
        Private Sub ProcessNextFrame()
            ' Figure how much time has passed since the last time.
            Dim CurrTick As Integer = Environment.TickCount
            Dim TickDiff As Integer = CurrTick - tickLast

            ' Don't update if no time has passed.
            If 0 = TickDiff Then
                Return
            End If
            tickLast = CurrTick

            ' Move the sprites according to how much time has passed.
            Dim i As Integer
            For i = 0 To numSprites - 1
                UpdateSprite(sprite(i), TickDiff / 1000.0F)
            Next i
            ' Cycle the palette every frame.
            CyclePalette()
            'Draw the sprites and text to the screen.
            DisplayFrame()
        End Sub 'ProcessNextFrame


        '-----------------------------------------------------------------------------
        ' Name: DisplayFrame()
        ' Desc: Draw the sprites and text to the screen.
        '-----------------------------------------------------------------------------
        Private Sub DisplayFrame()
            If Nothing Is front Then
                Return
            End If
            If False = displayDevice.TestCooperativeLevel() Then
                restoreNeeded = True
                Return
            End If

            If True = restoreNeeded Then
                restoreNeeded = False
                ' The surfaces were lost so restore them .
                RestoreSurfaces()
            End If

            ' Fill the back buffer with black.
            back.ColorFill(indexBlack)

            ' Blt the help text on the backbuffer.
            back.DrawFast(10, 10, surfaceText, DrawFastFlags.DoNotWait)

            ' Blt all the sprites onto the back buffer using color keying,
            ' ignoring errors until the flip. Note that all of these sprites 
            ' use the same DirectDraw surface.
            Dim i As Integer
            For i = 0 To numSprites - 1
                back.DrawFast(CInt(sprite(i).posX), CInt(sprite(i).posY), surfaceLogo, DrawFastFlags.DoNotWait Or DrawFastFlags.SourceColorKey)
            Next i

            ' We are in fullscreen mode, so perform a flip.
            front.Flip(back, FlipFlags.DoNotWait)
        End Sub 'DisplayFrame


        '-----------------------------------------------------------------------------
        ' Name: InitializeDirectDraw()
        ' Desc: Initializes DirectDraw and all the surfaces to be used.
        '-----------------------------------------------------------------------------
        Private Sub InitializeDirectDraw()
            Dim description As New SurfaceDescription() ' Describes a surface.
            Dim randomNumber As New Random() ' Random number for position and velocity.
            Dim flags As New ColorKeyFlags()

            displayDevice = New Device() ' Create a new DirectDrawDevice.
            displayDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.FullscreenExclusive) ' Set the cooperative level.

            Try
                displayDevice.SetDisplayMode(screenWidth, screenHeight, 8, 0, False) ' Set the display mode width and height, and 8 bit color depth.
            Catch e As UnsupportedException
                MessageBox.Show("Device doesn't support required mode. Sample will now exit.", "UnsupportedException caught")
                Close()
                Return
            End Try

            description.SurfaceCaps.PrimarySurface = True
            description.SurfaceCaps.Flip = True
            description.SurfaceCaps.Complex = True
            description.BackBufferCount = 1 ' Create 1 backbuffer.
            front = New Surface(description, displayDevice) ' Create the surface using the description above.
            Dim caps As New SurfaceCaps()
            caps.BackBuffer = True ' Caps of the surface.
            back = front.GetAttachedSurface(caps) ' Get the attached surface that matches the caps, which will be the backbuffer.
            pal = New Palette(displayDevice, bitmapFileName) ' Create a new palette, using the bitmap file.
            pe = pal.GetEntries(0, 256) ' Store the palette entries.
            front.Palette = pal ' The front surface will use this palette.
            description.Clear() ' Clear the data out from the SurfaceDescription class.
            surfaceLogo = New Surface(bitmapFileName, description, displayDevice) ' Create the sprite bitmap surface.
            Dim key As New ColorKey() ' Create a new colorkey.
            key.ColorSpaceHighValue = 0
            key.ColorSpaceLowValue = 0
            flags = ColorKeyFlags.SourceDraw
            surfaceLogo.SetColorKey(flags, key) ' Set the colorkey to the bitmap surface. 144 is the index used for the colorkey, which is black in this palette.
            description.Clear()
            description.SurfaceCaps.OffScreenPlain = True
            description.Width = 200 ' Set the width and height of the surface.
            description.Height = 20

            surfaceText = New Surface(description, displayDevice) ' Create the surface using the above description.
            surfaceText.ForeColor = Color.FromArgb(indexWhite) ' Because this is a palettized environment, the app needs to use index based colors, which
            ' do not correspond to pre-defined color values. For this reason, the indexWhite variable exists
            ' and contains the index where the color white is stored in the bitmap's palette. The app makes use
            ' of the static FromArgb() method to convert the index to a Color value.
            surfaceText.ColorFill(indexBlack)
            surfaceText.DrawText(0, 0, textHelp, False)

            Dim i As Integer
            For i = 0 To numSprites - 1
                ' Set the sprite's position and velocity
                sprite(i).posX = randomNumber.Next() Mod screenWidth
                sprite(i).posY = randomNumber.Next() Mod screenHeight

                sprite(i).velX = 500.0F * randomNumber.Next() / Int32.MaxValue - 250.0F
                sprite(i).velY = 500.0F * randomNumber.Next() / Int32.MaxValue - 250.0F
            Next i
        End Sub 'InitializeDirectDraw


        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                displayDevice.RestoreDisplayMode() ' Restore the display mode to what it was.
                displayDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.Normal) ' Set the cooperative level back to windowed.
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
            ' AnimatePalette
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(208, 69)
            Me.ForeColor = System.Drawing.Color.Black
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
            Me.KeyPreview = True
            Me.Name = "AnimatePalette"
            Me.Text = "AnimatePalette"
        End Sub 'InitializeComponent

        '-----------------------------------------------------------------------------
        ' Name: AnimatePalette_KeyUp()
        ' Desc: Processes key up events.
        '-----------------------------------------------------------------------------
        Private Sub AnimatePalette_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
            ' If the user presses the Escape key, the app needs to exit.
            If Keys.Escape = e.KeyCode Then
                Close()
            End If
        End Sub 'AnimatePalette_KeyUp
    End Class 'AnimatePalette
End Namespace 'AnimatePalette