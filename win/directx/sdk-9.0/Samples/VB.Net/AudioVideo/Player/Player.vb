Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.AudioVideoPlayback


Namespace Player
    '/ <summary>
    '/ Summary description for Form1.
    '/ </summary>
    Public Class AVPlayer
        Inherits System.Windows.Forms.Form

        Private filterText As String = "Video Files (*.avi; *.qt; *.mov; *.mpg; *.mpeg; *.m1v; *.wmv)|*.avi; *.qt; *.mov; *.mpg; *.mpeg; *.m1v; *.wmv|" + "Audio files (*.wav; *.mpa; *.mp2; *.mp3; *.au; *.aif; *.aiff; *.snd; *.wma)|*.wav; *.mpa; *.mp2; *.mp3; *.au; *.aif; *.aiff; *.snd; *.wma|" + "MIDI Files (*.mid, *.midi, *.rmi)|*.mid; *.midi; *.rmi|" + "Image Files (*.jpg, *.bmp, *.gif, *.tga)|*.jpg; *.bmp; *.gif; *.tga|" + "All Files (*.*)|*.*"

        Private WithEvents ourVideo As Video = Nothing
        Private WithEvents ourAudio As Audio = Nothing

        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing
        Private mnuMain As System.Windows.Forms.MainMenu
        Private ofdOpen As System.Windows.Forms.OpenFileDialog
        Private menuItem3 As System.Windows.Forms.MenuItem
        Private mnuFile As System.Windows.Forms.MenuItem
        Private WithEvents mnuOpen As System.Windows.Forms.MenuItem
        Private menuItem1 As System.Windows.Forms.MenuItem
        Private WithEvents mnuPlay As System.Windows.Forms.MenuItem
        Private WithEvents mnuStop As System.Windows.Forms.MenuItem
        Private WithEvents mnuPause As System.Windows.Forms.MenuItem
        Private WithEvents mnuExit As System.Windows.Forms.MenuItem

        Public Sub New() '
            InitializeComponent()

            OpenFile()
        End Sub 'New


        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            CleanupObjects()
            If Disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose


        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Friend WithEvents MenuItem2 As System.Windows.Forms.MenuItem
        Friend WithEvents mnuFull As System.Windows.Forms.MenuItem
        Private Sub InitializeComponent()
            Me.mnuMain = New System.Windows.Forms.MainMenu()
            Me.mnuFile = New System.Windows.Forms.MenuItem()
            Me.mnuOpen = New System.Windows.Forms.MenuItem()
            Me.menuItem3 = New System.Windows.Forms.MenuItem()
            Me.mnuExit = New System.Windows.Forms.MenuItem()
            Me.menuItem1 = New System.Windows.Forms.MenuItem()
            Me.mnuPlay = New System.Windows.Forms.MenuItem()
            Me.mnuStop = New System.Windows.Forms.MenuItem()
            Me.mnuPause = New System.Windows.Forms.MenuItem()
            Me.ofdOpen = New System.Windows.Forms.OpenFileDialog()
            Me.MenuItem2 = New System.Windows.Forms.MenuItem()
            Me.mnuFull = New System.Windows.Forms.MenuItem()
            '
            'mnuMain
            '
            Me.mnuMain.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuFile, Me.menuItem1})
            '
            'mnuFile
            '
            Me.mnuFile.Index = 0
            Me.mnuFile.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuOpen, Me.menuItem3, Me.mnuExit})
            Me.mnuFile.Text = "&File"
            '
            'mnuOpen
            '
            Me.mnuOpen.Index = 0
            Me.mnuOpen.Shortcut = System.Windows.Forms.Shortcut.CtrlO
            Me.mnuOpen.Text = "&Open Clip"
            '
            'menuItem3
            '
            Me.menuItem3.Index = 1
            Me.menuItem3.Text = "-"
            '
            'mnuExit
            '
            Me.mnuExit.Index = 2
            Me.mnuExit.Text = "E&xit"
            '
            'menuItem1
            '
            Me.menuItem1.Index = 1
            Me.menuItem1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuPlay, Me.mnuStop, Me.mnuPause, Me.MenuItem2, Me.mnuFull})
            Me.menuItem1.Text = "&Control"
            '
            'mnuPlay
            '
            Me.mnuPlay.Index = 0
            Me.mnuPlay.Shortcut = System.Windows.Forms.Shortcut.CtrlP
            Me.mnuPlay.Text = "&Play"
            '
            'mnuStop
            '
            Me.mnuStop.Index = 1
            Me.mnuStop.Shortcut = System.Windows.Forms.Shortcut.CtrlS
            Me.mnuStop.Text = "&Stop"
            '
            'mnuPause
            '
            Me.mnuPause.Index = 2
            Me.mnuPause.Shortcut = System.Windows.Forms.Shortcut.CtrlA
            Me.mnuPause.Text = "P&ause"
            '
            'MenuItem2
            '
            Me.MenuItem2.Index = 3
            Me.MenuItem2.Text = "-"
            '
            'mnuFull
            '
            Me.mnuFull.Index = 4
            Me.mnuFull.Shortcut = System.Windows.Forms.Shortcut.CtrlF
            Me.mnuFull.Text = "Toggle Fu&llscreen" & ControlChars.Tab & "<Alt-Enter>"
            '
            'AVPlayer
            '
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(264, 38)
            Me.Menu = Me.mnuMain
            Me.Name = "AVPlayer"
            Me.Text = "Audio Video Player"

        End Sub 'InitializeComponent

        '
        Private Sub CleanupObjects()
            If Not (ourVideo Is Nothing) Then
                ourVideo.Dispose()
            End If
            If Not (ourAudio Is Nothing) Then
                ourAudio.Dispose()
            End If
        End Sub 'CleanupObjects


        Private Sub OpenFile()
            If ofdOpen.InitialDirectory Is Nothing Or ofdOpen.InitialDirectory = String.Empty Then
                ofdOpen.InitialDirectory = DXUtil.SdkMediaPath
            End If
            ofdOpen.Filter = filterText
            ofdOpen.Title = "Open media file"
            ofdOpen.ShowDialog(Me)

            ' Now let's try to open this file
            If Not (ofdOpen.FileName Is Nothing) And ofdOpen.FileName <> String.Empty Then
                Try
                    If (ourVideo Is Nothing) Then
                        'First try to open this as a video file
                        ourVideo = New Video(ofdOpen.FileName)
                        ourVideo.Owner = Me
                        'Start playing now
                        ourVideo.Play()
                    Else
                        ourVideo.Open(ofdOpen.FileName, True)
                    End If
                Catch
                    Try
                        ' opening this as a video file failed.. Maybe it's audio only?
                        ourAudio = New Audio(ofdOpen.FileName)
                        ' Start playing now
                        ourAudio.Play()
                    Catch
                        MessageBox.Show("This file could not be opened.", "Invalid file.", MessageBoxButtons.OK, MessageBoxIcon.Information)
                    End Try
                End Try
            End If
        End Sub 'OpenFile


        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Application.Run(New AVPlayer())
        End Sub 'Main

        Private Sub ClipEnd(ByVal sender As Object, ByVal e As EventArgs) Handles ourAudio.Ending, ourVideo.Ending
            'Stop and restart our clip when it ends
            If Not (ourVideo Is Nothing) Then
                ourVideo.Stop()
                ourVideo.Play()
            Else
                If Not (ourAudio Is Nothing) Then
                    ourAudio.Stop()
                    ourAudio.Play()
                End If
            End If
        End Sub

        Private Sub mnuOpen_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuOpen.Click
            Me.OpenFile()
        End Sub 'mnuOpen_Click


        Private Sub mnuPlay_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuPlay.Click
            If Not (ourVideo Is Nothing) Then
                ourVideo.Play()
            Else
                If Not (ourAudio Is Nothing) Then
                    ourAudio.Play()
                End If
            End If
        End Sub 'mnuPlay_Click

        Private Sub mnuStop_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuStop.Click
            If Not (ourVideo Is Nothing) Then
                ourVideo.Stop()
            Else
                If Not (ourAudio Is Nothing) Then
                    ourAudio.Stop()
                End If
            End If
        End Sub 'mnuStop_Click

        Private Sub mnuPause_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuPause.Click
            If Not (ourVideo Is Nothing) Then
                ourVideo.Pause()
            Else
                If Not (ourAudio Is Nothing) Then
                    ourAudio.Pause()
                End If
            End If
        End Sub 'mnuPause_Click

        Private Sub mnuExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles mnuExit.Click
            Me.Dispose()
        End Sub 'mnuExit_Click

        Private Sub mnuFull_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles mnuFull.Click
            If Not (ourVideo Is Nothing) Then
                ourVideo.Fullscreen = Not ourVideo.Fullscreen
            End If
        End Sub
        Protected Overrides Sub OnKeyDown(ByVal e As System.Windows.Forms.KeyEventArgs)
            If e.Alt And e.KeyCode = System.Windows.Forms.Keys.Return Then
                mnuFull_Click(mnuFull, Nothing)
            End If

            ' Allow the control to handle the keystroke now
            MyBase.OnKeyDown(e)
        End Sub 'OnKeyDown    
    End Class 'AVPlayer
End Namespace 'Player