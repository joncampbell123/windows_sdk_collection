'-----------------------------------------------------------------------------
' File: MDIWindow.cs
'
' Desc: Example code showing how to do use MDI windows via DirectDraw
'
' Copyright (c) 1995-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectDraw
Imports System.Diagnostics


Namespace MDIWindow
    '/ <summary>
    '/ Summary description for MainWindow.
    '/ </summary>
    Public Class MainWindow
        Inherits System.Windows.Forms.Form
        Private mainMenu1 As System.Windows.Forms.MainMenu
        Private FileMenu As System.Windows.Forms.MenuItem
        Private WithEvents OpenWindowMenuItem As System.Windows.Forms.MenuItem
        Private WithEvents ExitMenuItem As System.Windows.Forms.MenuItem
        Private WindowMenu As System.Windows.Forms.MenuItem
        Private WithEvents TileHorizontalMenuItem As System.Windows.Forms.MenuItem
        Private WithEvents TileVerticalMenuItem As System.Windows.Forms.MenuItem
        Private WithEvents CascadeMenuItem As System.Windows.Forms.MenuItem

        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing
        Private draw As New Device() ' Holds the DrawDevice object.
        Private primary As Surface = Nothing ' Holds the primary destination surface.
        Private offscreen As Surface = Nothing ' Holds the offscreen source surface.
        Private clip As Clipper = Nothing
        ' Holds the clipper object.

        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            Draw = New Device() ' Create a new DrawDevice, using the default device.
            Draw.SetCooperativeLevel(Me, CooperativeLevelFlags.Normal) ' Set the coop level to normal windowed mode.
            clip = New Clipper(Draw) ' Create a new clipper.
            CreateSurfaces() ' Call the function that creates the surface objects.
        End Sub 'New
        Public Sub DrawSurfaces(ByVal window As Control, ByVal Location As Point, ByVal WindowSize As Size)
            ' This function draws from the source surface to the surfaces
            ' located in the MDI child windows.        
            Dim destination As New Rectangle(Location, WindowSize)

            If Me.WindowState = FormWindowState.Minimized Then
                Return
            End If
            Try
                ' Try and blit the offscreen surface on to the primary surface.
                clip.Window = window ' The clipper will use the main window handle.
                primary.Draw(destination, offscreen, DrawFlags.Wait)
            Catch
                ' The surface can be lost if power saving
                ' mode kicks in, or any other number of
                ' reasons.
                CreateSurfaces() ' Surface was lost. Recreate them.
            End Try
        End Sub 'Draw
        Private Sub CreateSurfaces()
            '
            ' This function is where the surfaces and
            ' clipper object are created.
            '

            Dim description As New SurfaceDescription() ' Create a new SurfaceDescription struct.
            description.SurfaceCaps.PrimarySurface = True

            ' If any object currently exist, destroy them.
            If Not Nothing Is offscreen Then
                offscreen.Dispose()
            End If
            If Not Nothing Is primary Then
                primary.Dispose()
            End If
            primary = New Surface(description, draw) ' Create the primary surface.
            clip.Window = Me ' The clipper will use the main window handle.
            primary.Clipper = clip ' Assign this clipper to the primary surface.
            description.Clear()

            offscreen = New Surface(DXUtil.SdkMediaPath + "\earthenvmap.bmp", description, draw) ' Create the surface using the specified file.
        End Sub 'CreateSurfaces
        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
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
            Me.mainMenu1 = New System.Windows.Forms.MainMenu()
            Me.FileMenu = New System.Windows.Forms.MenuItem()
            Me.OpenWindowMenuItem = New System.Windows.Forms.MenuItem()
            Me.ExitMenuItem = New System.Windows.Forms.MenuItem()
            Me.WindowMenu = New System.Windows.Forms.MenuItem()
            Me.TileHorizontalMenuItem = New System.Windows.Forms.MenuItem()
            Me.TileVerticalMenuItem = New System.Windows.Forms.MenuItem()
            Me.CascadeMenuItem = New System.Windows.Forms.MenuItem()
            ' 
            ' mainMenu1
            ' 
            Me.mainMenu1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.FileMenu, Me.WindowMenu})
            ' 
            ' FileMenu
            ' 
            Me.FileMenu.Index = 0
            Me.FileMenu.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.OpenWindowMenuItem, Me.ExitMenuItem})
            Me.FileMenu.Text = "&File"
            ' 
            ' OpenWindowMenuItem
            ' 
            Me.OpenWindowMenuItem.Index = 0
            Me.OpenWindowMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlO
            Me.OpenWindowMenuItem.Text = "&Open Window"
            ' 
            ' ExitMenuItem
            ' 
            Me.ExitMenuItem.Index = 1
            Me.ExitMenuItem.Text = "E&xit"
            ' 
            ' WindowMenu
            ' 
            Me.WindowMenu.Index = 1
            Me.WindowMenu.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.TileHorizontalMenuItem, Me.TileVerticalMenuItem, Me.CascadeMenuItem})
            Me.WindowMenu.Text = "&Window"
            ' 
            ' TileHorizontalMenuItem
            ' 
            Me.TileHorizontalMenuItem.Index = 0
            Me.TileHorizontalMenuItem.Text = "Tile &Horizontal"
            ' 
            ' TileVerticalMenuItem
            ' 
            Me.TileVerticalMenuItem.Index = 1
            Me.TileVerticalMenuItem.Text = "Tile &Vertical"
            ' 
            ' CascadeMenuItem
            ' 
            Me.CascadeMenuItem.Index = 2
            Me.CascadeMenuItem.Text = "&Cascade"
            ' 
            ' MainWindow
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(744, 612)
            Me.IsMdiContainer = True
            Me.Menu = Me.mainMenu1
            Me.Name = "MainWindow"
            Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show
            Me.Text = "MDIDraw"
        End Sub 'InitializeComponent
        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Application.Run(New MainWindow())
        End Sub 'Main
        Private Sub OpenWindowMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles OpenWindowMenuItem.Click
            Dim NewDrawWindow As New DrawWindow()
            NewDrawWindow.MdiParent = Me
            NewDrawWindow.Show()
        End Sub 'OpenWindowMenuItem_Click
        Private Sub ExitMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ExitMenuItem.Click
            Me.Close()
        End Sub 'ExitMenuItem_Click
        Private Sub TileHorizontal_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles TileHorizontalMenuItem.Click
            Me.LayoutMdi(MdiLayout.TileHorizontal)
        End Sub 'TileHorizontal_Click
        Private Sub TileVertical_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles TileVerticalMenuItem.Click
            Me.LayoutMdi(MdiLayout.TileVertical)
        End Sub 'TileVertical_Click
        Private Sub Cascade_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles CascadeMenuItem.Click
            Me.LayoutMdi(MdiLayout.Cascade)
        End Sub 'Cascade_Click
    End Class 'MainWindow
End Namespace 'MDIWindow