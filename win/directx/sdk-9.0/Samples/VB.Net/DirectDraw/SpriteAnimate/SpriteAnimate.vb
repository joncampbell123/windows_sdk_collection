Imports System
Imports System.Windows.Forms
Imports System.Drawing
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectDraw

Namespace SpriteAnimate

    Structure SpriteStruct
        Public speedRotation As Single
        Public tickRotation As Single
        Public frame As Integer
        Public isClockwise As Boolean
        Public posX As Single
        Public posY As Single
        Public velX As Single
        Public velY As Single
    End Structure 'SpriteStruct

    Public Class SpriteAnimate
        Inherits System.Windows.Forms.Form
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing

        ' Declare the class scope variables here.
        Private needRestore As Boolean = False ' Flag to determine the app needs to restore the surfaces.
        Private frame(numFrames - 1) As System.Drawing.Rectangle ' Holds the animation frame dimensions.
        Private surfaceAnimation As Surface = Nothing ' The surface that contains the sprite bitmap.
        Private Const numRandom As Integer = 100 ' Number of random values in the random array.
        Private randomNumber As New Random(Environment.TickCount) ' Random number generator.      
        Private randomTable(numRandom - 1) As Integer 'Holds the table of random values.
        Private tickLast As Integer = 0 ' Holds the value of the last call to GetTick.
        Private sprite(numSprites - 1) As SpriteStruct ' Array of sprite structs.
        Private Const numFrames As Integer = 30 ' Number of animation frames.
        Private Const numSprites As Integer = 25 ' Number of sprites to draw.
        Private nameFile As String = DXUtil.SdkMediaPath + "animate.bmp"
        Private diameterSprite As Integer = 32 ' Sprites width & height.
        Private randomIndex As Integer = 0 ' Current random index.
        Private entry(255) As PaletteEntry ' Palette entry array.
        Private draw As Device = Nothing ' The DirectDraw device.
        Private front As Surface = Nothing ' The front surface.
        Private heightScreen As Integer = 480 ' Screen height.
        Private back As Surface = Nothing ' The back surface.
        Private widthScreen As Integer = 640 ' Screen width.
        Private maxRand As Integer = &H7FFF

        ' Bitmap to load.
        Shared Sub Main()
            '/ <summary>
            '/ The main entry point for the application.
            '/ </summary>
            Dim app As New SpriteAnimate()
            Application.Exit()
        End Sub 'Main
        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            ' Create a new DrawDevice.
            InitDirectDraw()

            ' Keep looping until told to quit.
            While Created
                ProcessNextFrame() ' Process and draw the next frame.
                Application.DoEvents() ' Make sure the app has time to process messages.
            End While
        End Sub 'New
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
            ' SpriteAnimate
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(292, 273)
            Me.Cursor = System.Windows.Forms.Cursors.IBeam
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
            Me.Name = "SpriteAnimate"
            Me.Text = "SpriteAnimate"
        End Sub 'InitializeComponent
        Function GetNextRand() As Integer '
            '-----------------------------------------------------------------------------
            ' Name: GetNextRand()
            ' Desc: Gets the next element from the circular array of random values
            '-----------------------------------------------------------------------------
            Dim randomNext As Integer = randomTable(randomIndex = randomIndex + 1)
            randomIndex = randomIndex Mod numRandom

            Return randomNext
        End Function 'GetNextRand

        Private Sub DisplayFrame()
            '-----------------------------------------------------------------------------
            ' Name: DisplayFrame()
            ' Desc: Draw the sprites and text to the screen.
            '-----------------------------------------------------------------------------
            If Nothing Is front Then
                Return
            End If
            If False = draw.TestCooperativeLevel() Then
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

            ' Draw all the sprites onto the back buffer using color keying,
            ' ignoring errors until the flip. Note that all of these sprites 
            ' use the same DirectDraw surface.
            Try
                Dim i As Integer
                For i = 0 To numSprites - 1
                    back.DrawFast(sprite(i).posX, sprite(i).posY, surfaceAnimation, frame(sprite(i).frame), DrawFastFlags.DoNotWait Or DrawFastFlags.SourceColorKey)
                Next i
                back.DrawText(10, 30, "Press escape to exit", True)

                ' We are in fullscreen mode, so perform a flip.
                front.Flip(back, FlipFlags.DoNotWait)
            Catch
            End Try
        End Sub 'DisplayFrame
        ' Catch any exceptions that might crop up.

        Private Sub UpdateSprite(ByRef s As SpriteStruct, ByVal timeDelta As Single)
            '-----------------------------------------------------------------------------
            ' Name: UpdateSprite()
            ' Desc: Move the sprite around and make it bounce based on how much time 
            '       has passed
            '-----------------------------------------------------------------------------
            ' Update the sprite position
            s.posX += s.velX * timeDelta
            s.posY += s.velY * timeDelta

            ' See if its time to advance the sprite to the next frame
            s.tickRotation += timeDelta
            If s.tickRotation > s.speedRotation Then
                ' If it is, then either change the frame clockwise or counter-clockwise
                If s.isClockwise Then
                    s.frame += 1
                    s.frame = s.frame Mod numFrames
                Else
                    s.frame -= 1
                    If s.frame < 0 Then
                        s.frame = numFrames - 1
                    End If
                End If
                s.tickRotation = 0
            End If

            ' Using the next element from the random array, 
            ' randomize the velocity of the sprite
            If GetNextRand() Mod 100 < 2 Then
                s.velX = 500.0F * randomNumber.Next(0, maxRand) / maxRand - 250
                s.velY = 500.0F * randomNumber.Next(0, maxRand) / maxRand - 250
            End If

            ' Using the next element from the random array, 
            ' randomize the rotational speed of the sprite.
            If GetNextRand() Mod 100 < 5 Then
                s.speedRotation = (GetNextRand() Mod 50 + 5) / 1000
            End If
            ' Clip the position, and bounce if it hits the edge.
            If s.posX < 0 Then
                s.posX = 0
                s.velX = -s.velX
            End If

            If s.posX >= widthScreen - diameterSprite Then
                s.posX = widthScreen - 1 - diameterSprite
                s.velX = -s.velX
            End If

            If s.posY < 0 Then
                s.posY = 0
                s.velY = -s.velY
            End If

            If s.posY > heightScreen - diameterSprite Then
                s.posY = heightScreen - 1 - diameterSprite
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
            Dim i As Integer

            ' Don't update if no time has passed.
            If 0 = tickDifference Then
                Return
            End If
            tickLast = tickCurrent

            ' Move the sprites according to how much time has passed.
            For i = 0 To numSprites - 1
                UpdateSprite(sprite(i), tickDifference / 1000)
            Next i
            'Draw the sprites and text to the screen.
            DisplayFrame()
        End Sub 'ProcessNextFrame

        Private Sub InitDirectDraw()
            '-----------------------------------------------------------------------------
            ' Name: InitDirectDraw()
            ' Desc: Initializes DirectDraw and all the surfaces to be used.
            '-----------------------------------------------------------------------------
            Dim description As New SurfaceDescription() ' Describes a surface.
            Dim caps As New SurfaceCaps()
            Dim randomNext As Integer
            Dim ck As New ColorKey() ' Create a new colorkey.
            Dim i As Integer

            draw = New Device() ' Create a new DirectDrawDevice.
            draw.SetCooperativeLevel(Me, CooperativeLevelFlags.FullscreenExclusive) ' Set the cooperative level.
            draw.SetDisplayMode(widthScreen, heightScreen, 16, 0, False) ' Set the display mode width and height, and 16 bit color depth.
            description.SurfaceCaps.PrimarySurface = True
            description.SurfaceCaps.Flip = True
            description.SurfaceCaps.Complex = True
            description.BackBufferCount = 1 ' Create 1 backbuffer.
            front = New Surface(description, draw) ' Create the surface using the description above.
            caps.BackBuffer = True ' Caps of the surface.
            back = front.GetAttachedSurface(caps) ' Get the attached surface that matches the caps, which will be the backbuffer.
            back.ForeColor = Color.White
            description.Clear() ' Clear out the SurfaceDescription structure.
            surfaceAnimation = New Surface(nameFile, description, draw) ' Create the sprite bitmap surface.
            surfaceAnimation.SetColorKey(ColorKeyFlags.SourceDraw, ck) ' Set the colorkey to the bitmap surface. 0 is used for the colorkey, which is what the ColorKey struct is initialized to.

            For i = 0 To numSprites - 1
                ' Set the sprite's position, velocity, frame, rotation direction, and rotation speed.
                sprite(i).posX = randomNumber.Next(0, maxRand) Mod widthScreen
                sprite(i).posY = randomNumber.Next(0, maxRand) Mod heightScreen
                sprite(i).velX = 500.0F * randomNumber.Next(0, maxRand) / CSng(maxRand) - 250.0F
                sprite(i).velY = 500.0F * randomNumber.Next(0, maxRand) / CSng(maxRand) - 250.0F
                sprite(i).frame = randomNumber.Next(0, maxRand) Mod numFrames
                sprite(i).tickRotation = 0
                sprite(i).speedRotation = (randomNumber.Next(0, maxRand) Mod 50 + 25) / 1000.0F
                sprite(i).isClockwise = IIf(0 = randomNumber.Next(0, maxRand) Mod 2, True, False)
            Next i

            ' Precompute the source rects for frame.  The source rects 
            ' are used during the blt of the dds to the backbuffer. 
            For i = 0 To numFrames - 1
                Dim p As New System.Drawing.Point((i Mod 5) * diameterSprite, Fix(i / 5) * diameterSprite)
                frame(i) = New System.Drawing.Rectangle(p.X, p.Y, diameterSprite, diameterSprite)
            Next i

            ' Init an array of random values.  This array is used to create the
            ' 'flocking' effect seen in the sample.
            randomIndex = 0
            For randomNext = 0 To numRandom - 1
                randomTable(randomNext) = randomNumber.Next(0, maxRand)
            Next randomNext
        End Sub 'InitDirectDraw

        Sub RestoreSurfaces()
            '-----------------------------------------------------------------------------
            ' Name: RestoreSurfaces()
            ' Desc: Restore all the surfaces, and redraw the sprite surfaces.
            '-----------------------------------------------------------------------------
            Dim description As New SurfaceDescription()

            draw.RestoreAllSurfaces()

            ' Release and re-load the sprite bitmap.
            surfaceAnimation.Dispose()
            surfaceAnimation = Nothing
            surfaceAnimation = New Surface(nameFile, description, draw)
            Dim ck As New ColorKey()

            surfaceAnimation.SetColorKey(ColorKeyFlags.SourceDraw, ck)

            Return
        End Sub 'RestoreSurfaces

        Private Sub SpriteAnimate_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
            '-----------------------------------------------------------------------------
            ' Name: SpriteAnimate_KeyUp()
            ' Desc: Processes key up events.
            '-----------------------------------------------------------------------------
            ' If the user presses the Escape key, the app needs to exit.
            If Keys.Escape = e.KeyCode Then
                Close()
            End If
        End Sub 'SpriteAnimate_KeyUp
    End Class 'SpriteAnimate
End Namespace 'SpriteAnimate