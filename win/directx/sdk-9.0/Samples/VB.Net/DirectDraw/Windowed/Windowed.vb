Imports System
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectDraw

Namespace Windowed
    _
    Public Class Windowed
        Inherits System.Windows.Forms.Form
        Private components As System.ComponentModel.Container = Nothing

        Private dev As Device = Nothing ' Holds the DrawDevice object.
        Private primary As Surface = Nothing ' Holds the primary destination surface.
        Private offscreen As Surface = Nothing ' Holds the offscreen surface that the bitmap will be loaded on.
        Private clip As Clipper = Nothing
        Private destination As New Rectangle()

        ' Holds the clipper object.
        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            dev = New Device() ' Create a new DrawDevice, using the default device.
            dev.SetCooperativeLevel(Me, CooperativeLevelFlags.Normal) ' Set the coop level to normal windowed mode.
            CreateSurfaces() ' Call the function that creates the surface objects.
            Width = 295
        End Sub 'New
        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose
        Private Sub InitializeComponent()
            Dim resources As New System.Resources.ResourceManager(GetType(Windowed))
            ' 
            ' Windowed
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(292, 266)
            Me.Name = "Windowed"
            Me.Text = "Windowed"
        End Sub 'InitializeComponent

        Public Shared Sub Main()
            Application.Run(New Windowed())
        End Sub 'Main

        Private Sub Windowed_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles MyBase.Paint
            Draw()
        End Sub 'Windowed_Paint

        Private Sub Windowed_SizeChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.SizeChanged
            Draw()
        End Sub 'Windowed_SizeChanged

        Private Sub Draw()
            '
            ' This function Draws the offscreen bitmap surface to the primary visible surface.
            '

            If ((primary Is Nothing) Or (offscreen Is Nothing)) Then
                ' Check to make sure both surfaces exist.
                Exit Sub
            End If

            If (WindowState = FormWindowState.Minimized) Then Return
            Height = IIf(Height < 50, 50, Height) ' Make sure the height is always valid.

            ' Get the new client size to Draw to.
            destination = New Rectangle(PointToScreen(New Point(ClientRectangle.X, ClientRectangle.Y)), ClientSize)

            Try
                ' Try and Draw the offscreen surface on to the primary surface.
                primary.Draw(destination, offscreen, DrawFlags.Wait)
            Catch e As SurfaceLostException
                ' The surface can be lost if power saving
                ' mode kicks in, or any other number of
                ' reasons.
                CreateSurfaces() ' Surface was lost. Recreate them.
            End Try
        End Sub 'Blt
        Private Sub CreateSurfaces()
            '
            ' This function is where the surfaces and
            ' clipper object are created.
            '
            Dim desc As New SurfaceDescription() ' Create a new SurfaceDescription struct.
            Dim caps As New SurfaceCaps() ' Create a new SurfaceCaps2 struct.

            caps.PrimarySurface = True ' The caps for this surface is simply primary.
            desc.SurfaceCaps = caps ' Point the SurfaceCaps member to the caps struct.
            primary = New Surface(desc, dev) ' Create the primary surface.

            clip = New Clipper(dev) ' Create a new clipper.
            clip.Window = Me ' The clipper will use the main window handle.
            primary.Clipper = clip ' Assign this clipper to the primary surface.

            desc = New SurfaceDescription() ' Create a new SurfaceDescription struct.
            offscreen = New Surface(DXUtil.SdkMediaPath + "\earthenvmap.bmp", desc, dev) ' Create the surface using the specified file.
        End Sub 'CreateSurfaces
    End Class 'Windowed
End Namespace 'Windowed