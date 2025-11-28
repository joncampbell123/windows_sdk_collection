Imports System
Imports System.Drawing
Imports Color = System.Drawing.Color
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectSound
Imports Buffer = Microsoft.DirectX.DirectSound.Buffer

Public Class Play3DSound
    Inherits Form

    Private WithEvents buttonSoundfile As Button
    Private labelFilename As Label '
    Private labelStatic As Label
    Private labelStatus As Label
    Private groupboxStatic As GroupBox
    Private labelStatic1 As Label
    Private labelDopplerfactor As Label
    Private WithEvents trackbarDopplerSlider As TrackBar
    Private labelStatic2 As Label
    Private labelRollofffactor As Label
    Private WithEvents trackbarRolloffSlider As TrackBar
    Private labelStatic3 As Label
    Private labelMindistance As Label
    Private WithEvents trackbarMindistanceSlider As TrackBar
    Private labelStatic4 As Label
    Private labelMaxdistance As Label
    Private WithEvents trackbarMaxdistanceSlider As TrackBar
    Private groupboxStatic1 As GroupBox
    Private pictureboxRenderWindow As PictureBox
    Private WithEvents trackbarVerticalSlider As TrackBar
    Private WithEvents trackbarHorizontalSlider As TrackBar
    Private WithEvents buttonPlay As Button
    Private WithEvents buttonStop As Button
    Private WithEvents buttonCancel As Button
    Private WithEvents checkboxDefer As CheckBox
    Private WithEvents buttonApply As Button
    Private WithEvents timerMovement As System.Timers.Timer
    Private listenerParameters As New Listener3DSettings() '
    Private application3DSettings As New Buffer3DSettings()
    Private applicationBuffer As SecondaryBuffer = Nothing
    Public Shared guid3DAlgorithm As Guid = Guid.Empty
    Private applicationDevice As New Device()
    Private applicationListener As Listener3D = Nothing
    Private applicationBuffer3D As Buffer3D = Nothing
    Private applicationGraphics As Graphics = Nothing
    Private maxOrbitRadius As Single = 1.0F
    Private FileName As String = String.Empty
    Private Path As String = String.Empty
    Private bitmapGrid As Bitmap = Nothing
    Private GridHeight As Integer = 0
    Private GridWidth As Integer = 0
    Private X As Integer = 0
    Private Y As Integer = 0

    Public Shared Function Main(ByVal Args() As String) As Integer
        Application.Run(New Play3DSound())
        Return 0
    End Function 'Main

    Public Sub New()
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()

        Dim description As New BufferDescription()
        Dim fmt As New WaveFormat()
        description.PrimaryBuffer = True
        description.Control3D = True
        Dim buff As Buffer = Nothing

        fmt.FormatTag = WaveFormatTag.Pcm
        fmt.Channels = 2
        fmt.SamplesPerSecond = 22050
        fmt.BitsPerSample = 16
        fmt.BlockAlign = CShort(fmt.BitsPerSample / 8 * fmt.Channels)
        fmt.AverageBytesPerSecond = fmt.SamplesPerSecond * fmt.BlockAlign

        applicationDevice.SetCooperativeLevel(Me, CooperativeLevel.Priority)

        ' Get the primary buffer and set the format.
        buff = New Buffer(description, applicationDevice)
        buff.Format = fmt

        applicationListener = New Listener3D(buff)
        listenerParameters = applicationListener.AllParameters

        labelFilename.Text = [String].Empty
        labelStatus.Text = "No file loaded."

        pictureboxRenderWindow.BackgroundImage = Image.FromFile((DXUtil.SdkMediaPath + "grid.jpg"))
        GridWidth = pictureboxRenderWindow.Width
        GridHeight = pictureboxRenderWindow.Height

        trackbarDopplerSlider.Maximum = ConvertLogScaleToLinearSliderPos(DSoundHelper.MaxDopplerFactor)
        trackbarDopplerSlider.Minimum = ConvertLogScaleToLinearSliderPos(DSoundHelper.MinDopplerFactor)

        trackbarRolloffSlider.Maximum = ConvertLogScaleToLinearSliderPos(DSoundHelper.MaxRolloffFactor)
        trackbarRolloffSlider.Minimum = ConvertLogScaleToLinearSliderPos(DSoundHelper.MinRolloffFactor)

        trackbarMindistanceSlider.Maximum = 40
        trackbarMindistanceSlider.Minimum = 1

        trackbarMaxdistanceSlider.Maximum = 40
        trackbarMaxdistanceSlider.Minimum = 1

        trackbarVerticalSlider.Maximum = 100
        trackbarVerticalSlider.Minimum = -100
        trackbarVerticalSlider.Value = 100

        trackbarHorizontalSlider.Maximum = 100
        trackbarHorizontalSlider.Minimum = -100
        trackbarHorizontalSlider.Value = 100

        SetSlidersPos(0, 0, maxOrbitRadius, maxOrbitRadius * 20.0F)
        SliderChanged()
    End Sub 'New

    Sub InitializeComponent()

        Me.buttonSoundfile = New System.Windows.Forms.Button()
        Me.labelFilename = New System.Windows.Forms.Label()
        Me.labelStatic = New System.Windows.Forms.Label()
        Me.labelStatus = New System.Windows.Forms.Label()
        Me.groupboxStatic = New System.Windows.Forms.GroupBox()
        Me.buttonApply = New System.Windows.Forms.Button()
        Me.trackbarMaxdistanceSlider = New System.Windows.Forms.TrackBar()
        Me.trackbarDopplerSlider = New System.Windows.Forms.TrackBar()
        Me.trackbarMindistanceSlider = New System.Windows.Forms.TrackBar()
        Me.trackbarRolloffSlider = New System.Windows.Forms.TrackBar()
        Me.checkboxDefer = New System.Windows.Forms.CheckBox()
        Me.labelMaxdistance = New System.Windows.Forms.Label()
        Me.labelStatic4 = New System.Windows.Forms.Label()
        Me.labelRollofffactor = New System.Windows.Forms.Label()
        Me.labelStatic2 = New System.Windows.Forms.Label()
        Me.labelStatic3 = New System.Windows.Forms.Label()
        Me.labelMindistance = New System.Windows.Forms.Label()
        Me.labelStatic1 = New System.Windows.Forms.Label()
        Me.labelDopplerfactor = New System.Windows.Forms.Label()
        Me.groupboxStatic1 = New System.Windows.Forms.GroupBox()
        Me.pictureboxRenderWindow = New System.Windows.Forms.PictureBox()
        Me.trackbarVerticalSlider = New System.Windows.Forms.TrackBar()
        Me.trackbarHorizontalSlider = New System.Windows.Forms.TrackBar()
        Me.buttonPlay = New System.Windows.Forms.Button()
        Me.buttonStop = New System.Windows.Forms.Button()
        Me.buttonCancel = New System.Windows.Forms.Button()
        Me.timerMovement = New System.Timers.Timer()
        Me.groupboxStatic.SuspendLayout()
        CType(Me.trackbarMaxdistanceSlider, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarDopplerSlider, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarMindistanceSlider, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarRolloffSlider, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarVerticalSlider, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarHorizontalSlider, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.timerMovement, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        ' 
        ' buttonSoundfile
        ' 
        Me.buttonSoundfile.Location = New System.Drawing.Point(10, 11)
        Me.buttonSoundfile.Name = "buttonSoundfile"
        Me.buttonSoundfile.Size = New System.Drawing.Size(78, 21)
        Me.buttonSoundfile.TabIndex = 0
        Me.buttonSoundfile.Text = "Sound &file..."
        ' 
        ' labelFilename
        ' 
        Me.labelFilename.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelFilename.Location = New System.Drawing.Point(102, 11)
        Me.labelFilename.Name = "labelFilename"
        Me.labelFilename.Size = New System.Drawing.Size(334, 21)
        Me.labelFilename.TabIndex = 1
        Me.labelFilename.Text = "Static"
        Me.labelFilename.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' labelStatic
        ' 
        Me.labelStatic.Location = New System.Drawing.Point(10, 41)
        Me.labelStatic.Name = "labelStatic"
        Me.labelStatic.Size = New System.Drawing.Size(63, 21)
        Me.labelStatic.TabIndex = 2
        Me.labelStatic.Text = "Status"
        Me.labelStatic.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' labelStatus
        ' 
        Me.labelStatus.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelStatus.Location = New System.Drawing.Point(102, 41)
        Me.labelStatus.Name = "labelStatus"
        Me.labelStatus.Size = New System.Drawing.Size(334, 21)
        Me.labelStatus.TabIndex = 3
        Me.labelStatus.Text = "Static"
        Me.labelStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' groupboxStatic
        ' 
        Me.groupboxStatic.Controls.AddRange(New System.Windows.Forms.Control() {Me.buttonApply, Me.trackbarMaxdistanceSlider, Me.trackbarDopplerSlider, Me.trackbarMindistanceSlider, Me.trackbarRolloffSlider, Me.checkboxDefer, Me.labelMaxdistance, Me.labelStatic4, Me.labelRollofffactor, Me.labelStatic2, Me.labelStatic3, Me.labelMindistance})
        Me.groupboxStatic.Location = New System.Drawing.Point(10, 72)
        Me.groupboxStatic.Name = "groupboxStatic"
        Me.groupboxStatic.Size = New System.Drawing.Size(300, 240)
        Me.groupboxStatic.TabIndex = 4
        Me.groupboxStatic.TabStop = False
        Me.groupboxStatic.Text = "Sound properties"
        ' 
        ' buttonApply
        ' 
        Me.buttonApply.Location = New System.Drawing.Point(184, 208)
        Me.buttonApply.Name = "buttonApply"
        Me.buttonApply.Size = New System.Drawing.Size(88, 18)
        Me.buttonApply.TabIndex = 25
        Me.buttonApply.Text = "Apply Settings"
        ' 
        ' trackbarMaxdistanceSlider
        ' 
        Me.trackbarMaxdistanceSlider.AutoSize = False
        Me.trackbarMaxdistanceSlider.Location = New System.Drawing.Point(160, 168)
        Me.trackbarMaxdistanceSlider.Name = "trackbarMaxdistanceSlider"
        Me.trackbarMaxdistanceSlider.Size = New System.Drawing.Size(135, 45)
        Me.trackbarMaxdistanceSlider.TabIndex = 16
        Me.trackbarMaxdistanceSlider.Text = "Slider3"
        Me.trackbarMaxdistanceSlider.TickStyle = System.Windows.Forms.TickStyle.None
        ' 
        ' trackbarDopplerSlider
        ' 
        Me.trackbarDopplerSlider.AutoSize = False
        Me.trackbarDopplerSlider.Location = New System.Drawing.Point(160, 24)
        Me.trackbarDopplerSlider.Name = "trackbarDopplerSlider"
        Me.trackbarDopplerSlider.Size = New System.Drawing.Size(135, 45)
        Me.trackbarDopplerSlider.TabIndex = 7
        Me.trackbarDopplerSlider.Text = "Slider1"
        Me.trackbarDopplerSlider.TickStyle = System.Windows.Forms.TickStyle.None
        ' 
        ' trackbarMindistanceSlider
        ' 
        Me.trackbarMindistanceSlider.AutoSize = False
        Me.trackbarMindistanceSlider.Location = New System.Drawing.Point(160, 120)
        Me.trackbarMindistanceSlider.Name = "trackbarMindistanceSlider"
        Me.trackbarMindistanceSlider.Size = New System.Drawing.Size(135, 45)
        Me.trackbarMindistanceSlider.TabIndex = 13
        Me.trackbarMindistanceSlider.Text = "Slider3"
        Me.trackbarMindistanceSlider.TickStyle = System.Windows.Forms.TickStyle.None
        ' 
        ' trackbarRolloffSlider
        ' 
        Me.trackbarRolloffSlider.AutoSize = False
        Me.trackbarRolloffSlider.Location = New System.Drawing.Point(160, 72)
        Me.trackbarRolloffSlider.Name = "trackbarRolloffSlider"
        Me.trackbarRolloffSlider.Size = New System.Drawing.Size(135, 45)
        Me.trackbarRolloffSlider.TabIndex = 10
        Me.trackbarRolloffSlider.Text = "Slider2"
        Me.trackbarRolloffSlider.TickStyle = System.Windows.Forms.TickStyle.None
        ' 
        ' checkboxDefer
        ' 
        Me.checkboxDefer.Appearance = System.Windows.Forms.Appearance.Button
        Me.checkboxDefer.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.checkboxDefer.Location = New System.Drawing.Point(64, 208)
        Me.checkboxDefer.Name = "checkboxDefer"
        Me.checkboxDefer.Size = New System.Drawing.Size(87, 18)
        Me.checkboxDefer.TabIndex = 24
        Me.checkboxDefer.Text = "Defer Settings"
        ' 
        ' labelMaxdistance
        ' 
        Me.labelMaxdistance.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelMaxdistance.Location = New System.Drawing.Point(96, 168)
        Me.labelMaxdistance.Name = "labelMaxdistance"
        Me.labelMaxdistance.Size = New System.Drawing.Size(52, 21)
        Me.labelMaxdistance.TabIndex = 15
        Me.labelMaxdistance.Text = "0"
        Me.labelMaxdistance.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        ' 
        ' labelStatic4
        ' 
        Me.labelStatic4.Location = New System.Drawing.Point(16, 168)
        Me.labelStatic4.Name = "labelStatic4"
        Me.labelStatic4.Size = New System.Drawing.Size(71, 21)
        Me.labelStatic4.TabIndex = 14
        Me.labelStatic4.Text = "Max distance"
        Me.labelStatic4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' labelRollofffactor
        ' 
        Me.labelRollofffactor.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelRollofffactor.Location = New System.Drawing.Point(96, 72)
        Me.labelRollofffactor.Name = "labelRollofffactor"
        Me.labelRollofffactor.Size = New System.Drawing.Size(52, 21)
        Me.labelRollofffactor.TabIndex = 9
        Me.labelRollofffactor.Text = "0"
        Me.labelRollofffactor.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        ' 
        ' labelStatic2
        ' 
        Me.labelStatic2.Location = New System.Drawing.Point(16, 72)
        Me.labelStatic2.Name = "labelStatic2"
        Me.labelStatic2.Size = New System.Drawing.Size(71, 21)
        Me.labelStatic2.TabIndex = 8
        Me.labelStatic2.Text = "Rolloff factor"
        Me.labelStatic2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' labelStatic3
        ' 
        Me.labelStatic3.Location = New System.Drawing.Point(16, 120)
        Me.labelStatic3.Name = "labelStatic3"
        Me.labelStatic3.Size = New System.Drawing.Size(71, 21)
        Me.labelStatic3.TabIndex = 11
        Me.labelStatic3.Text = "Min distance"
        Me.labelStatic3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' labelMindistance
        ' 
        Me.labelMindistance.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelMindistance.Location = New System.Drawing.Point(96, 120)
        Me.labelMindistance.Name = "labelMindistance"
        Me.labelMindistance.Size = New System.Drawing.Size(52, 21)
        Me.labelMindistance.TabIndex = 12
        Me.labelMindistance.Text = "0"
        Me.labelMindistance.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        ' 
        ' labelStatic1
        ' 
        Me.labelStatic1.Location = New System.Drawing.Point(25, 96)
        Me.labelStatic1.Name = "labelStatic1"
        Me.labelStatic1.Size = New System.Drawing.Size(75, 21)
        Me.labelStatic1.TabIndex = 5
        Me.labelStatic1.Text = "Doppler factor"
        Me.labelStatic1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' labelDopplerfactor
        ' 
        Me.labelDopplerfactor.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelDopplerfactor.Location = New System.Drawing.Point(108, 96)
        Me.labelDopplerfactor.Name = "labelDopplerfactor"
        Me.labelDopplerfactor.Size = New System.Drawing.Size(52, 21)
        Me.labelDopplerfactor.TabIndex = 6
        Me.labelDopplerfactor.Text = "0"
        Me.labelDopplerfactor.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        ' 
        ' groupboxStatic1
        ' 
        Me.groupboxStatic1.Location = New System.Drawing.Point(318, 72)
        Me.groupboxStatic1.Name = "groupboxStatic1"
        Me.groupboxStatic1.Size = New System.Drawing.Size(162, 240)
        Me.groupboxStatic1.TabIndex = 17
        Me.groupboxStatic1.TabStop = False
        Me.groupboxStatic1.Text = "Sound movement"
        ' 
        ' pictureboxRenderWindow
        ' 
        Me.pictureboxRenderWindow.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.pictureboxRenderWindow.Location = New System.Drawing.Point(352, 136)
        Me.pictureboxRenderWindow.Name = "pictureboxRenderWindow"
        Me.pictureboxRenderWindow.Size = New System.Drawing.Size(69, 70)
        Me.pictureboxRenderWindow.TabIndex = 18
        Me.pictureboxRenderWindow.TabStop = False
        Me.pictureboxRenderWindow.Text = "130"
        ' 
        ' trackbarVerticalSlider
        ' 
        Me.trackbarVerticalSlider.AutoSize = False
        Me.trackbarVerticalSlider.Location = New System.Drawing.Point(432, 128)
        Me.trackbarVerticalSlider.Name = "trackbarVerticalSlider"
        Me.trackbarVerticalSlider.Orientation = System.Windows.Forms.Orientation.Vertical
        Me.trackbarVerticalSlider.Size = New System.Drawing.Size(45, 84)
        Me.trackbarVerticalSlider.TabIndex = 19
        Me.trackbarVerticalSlider.Text = "Slider1"
        Me.trackbarVerticalSlider.TickStyle = System.Windows.Forms.TickStyle.None
        ' 
        ' trackbarHorizontalSlider
        ' 
        Me.trackbarHorizontalSlider.AutoSize = False
        Me.trackbarHorizontalSlider.Location = New System.Drawing.Point(344, 208)
        Me.trackbarHorizontalSlider.Name = "trackbarHorizontalSlider"
        Me.trackbarHorizontalSlider.Size = New System.Drawing.Size(82, 45)
        Me.trackbarHorizontalSlider.TabIndex = 20
        Me.trackbarHorizontalSlider.Text = "Slider1"
        Me.trackbarHorizontalSlider.TickStyle = System.Windows.Forms.TickStyle.None
        ' 
        ' buttonPlay
        ' 
        Me.buttonPlay.Enabled = False
        Me.buttonPlay.Location = New System.Drawing.Point(8, 328)
        Me.buttonPlay.Name = "buttonPlay"
        Me.buttonPlay.TabIndex = 21
        Me.buttonPlay.Text = "&Play"
        ' 
        ' buttonStop
        ' 
        Me.buttonStop.Enabled = False
        Me.buttonStop.Location = New System.Drawing.Point(88, 328)
        Me.buttonStop.Name = "buttonStop"
        Me.buttonStop.TabIndex = 22
        Me.buttonStop.Text = "&Stop"
        ' 
        ' buttonCancel
        ' 
        Me.buttonCancel.Location = New System.Drawing.Point(400, 328)
        Me.buttonCancel.Name = "buttonCancel"
        Me.buttonCancel.TabIndex = 23
        Me.buttonCancel.Text = "E&xit"
        ' 
        ' timerMovement
        ' 
        Me.timerMovement.Enabled = True
        Me.timerMovement.SynchronizingObject = Me
        ' 
        ' Play3DSound
        ' 
        Me.AcceptButton = Me.buttonSoundfile
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(496, 358)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.buttonSoundfile, Me.labelFilename, Me.labelStatic, Me.labelStatus, Me.labelStatic1, Me.labelDopplerfactor, Me.pictureboxRenderWindow, Me.trackbarVerticalSlider, Me.trackbarHorizontalSlider, Me.buttonPlay, Me.buttonStop, Me.buttonCancel, Me.groupboxStatic, Me.groupboxStatic1})
        Me.Name = "Play3DSound"
        Me.Text = "Play3DSound"
        Me.groupboxStatic.ResumeLayout(False)
        CType(Me.trackbarMaxdistanceSlider, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarDopplerSlider, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarMindistanceSlider, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarRolloffSlider, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarVerticalSlider, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarHorizontalSlider, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.timerMovement, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    Function ConvertLinearSliderPosToLogScale(ByVal iSliderPos As Integer) As Single
        If iSliderPos > 0 And iSliderPos <= 10 Then
            Return iSliderPos * 0.01F
        ElseIf iSliderPos > 10 And iSliderPos <= 20 Then
            Return (iSliderPos - 10) * 0.1F
        ElseIf iSliderPos > 20 And iSliderPos <= 30 Then
            Return (iSliderPos - 20) * 1.0F
        ElseIf iSliderPos > 30 And iSliderPos <= 40 Then
            Return (iSliderPos - 30) * 10.0F
        End If
        Return 0.0F
    End Function 'ConvertLinearSliderPosToLogScale

    Function ConvertLogScaleToLinearSliderPos(ByVal fValue As Single) As Integer
        '-----------------------------------------------------------------------------
        ' Name: ConvertLinearSliderPosToLogScale()
        ' Desc: Converts a quasi logrithmic scale to a slider position
        '-----------------------------------------------------------------------------
        If fValue > 0.0F And fValue <= 0.1F Then
            Return CInt(fValue / 0.01F)
        ElseIf fValue > 0.1F And fValue <= 1.0F Then
            Return CInt(fValue / 0.1F) + 10
        ElseIf fValue > 1.0F And fValue <= 10.0F Then
            Return CInt(fValue / 1.0F) + 20
        ElseIf fValue > 10.0F And fValue <= 100.0F Then
            Return CInt(fValue / 10.0F) + 30
        End If
        Return 0
    End Function 'ConvertLogScaleToLinearSliderPos

    Sub SetSlidersPos(ByVal fDopplerValue As Single, ByVal fRolloffValue As Single, ByVal fMinDistValue As Single, ByVal fMaxDistValue As Single)
        '-----------------------------------------------------------------------------
        ' Name: SetSlidersPos()
        ' Desc: Sets the slider positions
        '-----------------------------------------------------------------------------
        Dim lDopplerSlider As Integer = ConvertLogScaleToLinearSliderPos(fDopplerValue)
        Dim lRolloffSlider As Integer = ConvertLogScaleToLinearSliderPos(fRolloffValue)
        Dim lMinDistSlider As Integer = ConvertLogScaleToLinearSliderPos(fMinDistValue)
        Dim lMaxDistSlider As Integer = ConvertLogScaleToLinearSliderPos(fMaxDistValue)

        trackbarDopplerSlider.Value = lDopplerSlider
        trackbarRolloffSlider.Value = lRolloffSlider
        trackbarMindistanceSlider.Value = lMinDistSlider
        trackbarMaxdistanceSlider.Value = lMaxDistSlider
    End Sub 'SetSlidersPos

    Private Sub buttonSoundfile_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonSoundfile.Click
        ' Stop the timer while dialogs are displayed
        timerMovement.Enabled = False
        If OpenSoundFile() Then
            timerMovement.Enabled = True
        End If
    End Sub 'buttonSoundfile_Click

    Function OpenSoundFile() As Boolean
        '-----------------------------------------------------------------------------
        ' Name: OnOpenSoundFile()
        ' Desc: Called when the user requests to open a sound file
        '-----------------------------------------------------------------------------
        Dim ofd As New OpenFileDialog()
        Dim guid3DAlgorithm As Guid = Guid.Empty

        ' Get the default media path (something like C:\WINDOWS\MEDIA)
        If String.Empty = Path Then
            Path = Environment.SystemDirectory.Substring(0, Environment.SystemDirectory.LastIndexOf("\")) + "\media"
        End If
        ofd.DefaultExt = ".wav"
        ofd.Filter = "Wave Files|*.wav|All Files|*.*"
        ofd.FileName = FileName
        ofd.InitialDirectory = Path

        If Not Nothing Is applicationBuffer Then
            applicationBuffer.Stop()
            applicationBuffer.SetCurrentPosition(0)
        End If

        ' Update the UI controls to show the sound as loading a file
        buttonPlay.Enabled = False
        buttonStop.Enabled = False
        labelStatus.Text = "Loading file..."

        ' Display the OpenFileName dialog. Then, try to load the specified file
        If DialogResult.Cancel = ofd.ShowDialog(Me) Then
            labelStatus.Text = "Load aborted."
            timerMovement.Enabled = True
            Return False
        End If
        FileName = ofd.FileName
        Path = ofd.FileName.Substring(0, ofd.FileName.LastIndexOf("\"))

        ' Free any previous sound, and make a new one
        If Not Nothing Is applicationBuffer Then
            applicationBuffer.Dispose()
            applicationBuffer = Nothing
        End If

        ' Get the software DirectSound3D emulation algorithm to use
        ' Ask the user for this sample, so display the algorithm dialog box.		
        Dim frm As New AlgorithmForm()
        If DialogResult.Cancel = frm.ShowDialog(Me) Then
            ' User canceled dialog box
            labelStatus.Text = "Load aborted."
            labelFilename.Text = String.Empty
            Return False
        End If

        LoadSoundFile(ofd.FileName)
        If Nothing Is applicationBuffer Then
            Return False
        End If
        ' Get the 3D buffer from the secondary buffer
        Try
            applicationBuffer3D = New Buffer3D(applicationBuffer)
        Catch
            labelStatus.Text = "Could not get 3D buffer."
            labelFilename.Text = String.Empty
            Return False
        End Try

        ' Get the 3D buffer parameters
        application3DSettings = applicationBuffer3D.AllParameters

        ' Set new 3D buffer parameters
        application3DSettings.Mode = Mode3D.HeadRelative
        applicationBuffer3D.AllParameters = application3DSettings

        If True = applicationBuffer.Caps.LocateInHardware Then
            labelStatus.Text = "File loaded using hardware mixing."
        Else
            labelStatus.Text = "File loaded using software mixing."
        End If
        ' Update the UI controls to show the sound as the file is loaded
        labelFilename.Text = FileName
        EnablePlayUI(True)

        ' Remember the file for next time
        If Not Nothing Is applicationBuffer3D Then
            FileName = ofd.FileName
        End If
        Path = FileName.Substring(0, FileName.LastIndexOf("\"))

        ' Set the slider positions
        SetSlidersPos(0.0F, 0.0F, maxOrbitRadius, maxOrbitRadius * 2.0F)
        SliderChanged()
        Return True
    End Function 'OpenSoundFile

    Private Sub LoadSoundFile(ByVal FileName As String)
        Dim description As New BufferDescription()
        Dim wf As New WaveFormat()

        buttonPlay.Enabled = False
        buttonStop.Enabled = False
        labelStatus.Text = "Loading file..."

        description.Guid3DAlgorithm = guid3DAlgorithm
        description.Control3D = True

        If Not Nothing Is applicationBuffer Then
            applicationBuffer.Stop()
            applicationBuffer.SetCurrentPosition(0)
        End If

        ' Load the wave file into a DirectSound buffer
        Try
            applicationBuffer = New SecondaryBuffer(FileName, description, applicationDevice)
            If (applicationBuffer.NotVirtualized) Then
                MessageBox.Show(Me, "The 3D virtualization algorithm requested is not supported under this " + "operating system.  It is available only on Windows 2000, Windows ME, and Windows 98 with WDM " + "drivers and beyond.  Creating buffer with no virtualization.", "DirectSound Sample", MessageBoxButtons.OK)
            End If
        Catch
            ' Check to see if it was a stereo buffer that threw the exception.
            labelStatus.Text = "Wave file must be mono for 3D control."
            labelFilename.Text = String.Empty
            Return
        Catch
            ' Unknown error, but not a critical failure, so just update the status
            labelStatus.Text = "Could not create sound buffer."
            Return
        End Try

        If WaveFormatTag.Pcm <> (WaveFormatTag.Pcm And description.Format.FormatTag) Then
            labelStatus.Text = "Wave file must be PCM for 3D control."
            If Not Nothing Is applicationBuffer Then
                applicationBuffer.Dispose()
            End If
            applicationBuffer = Nothing
        End If

        ' Remember the file for next time
        If Not Nothing Is applicationBuffer Then
            FileName = FileName
        End If
        labelStatus.Text = "Ready."
        labelFilename.Text = FileName
    End Sub 'LoadSoundFile


    Private Function RestoreBuffer() As Boolean
        If False = applicationBuffer.Status.BufferLost Then
            Return False
        End If
        While True = applicationBuffer.Status.BufferLost
            applicationBuffer.Restore()
            Application.DoEvents()
        End While
        Return True
    End Function 'RestoreBuffer


    Private Sub PlaySound()
        Dim flags As BufferPlayFlags = BufferPlayFlags.Looping

        If RestoreBuffer() Then
            LoadSoundFile(FileName)
            applicationBuffer.SetCurrentPosition(0)
        End If
        applicationBuffer.Play(0, flags)
    End Sub 'PlaySound


    Private Sub buttonCancel_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonCancel.Click
        Me.Close()
    End Sub 'buttonCancel_Click


    Private Sub buttonPlay_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonPlay.Click
        EnablePlayUI(False)
        PlaySound()
    End Sub 'buttonPlay_Click

    Private Sub EnablePlayUI(ByVal bEnable As Boolean)
        If True = bEnable Then
            buttonPlay.Enabled = True
            buttonStop.Enabled = False
            buttonPlay.Focus()
        Else
            buttonPlay.Enabled = False
            buttonStop.Enabled = True
            buttonStop.Focus()
        End If
    End Sub 'EnablePlayUI

    Sub SliderChanged()
        '-----------------------------------------------------------------------------
        ' Name: SliderChanged()  
        ' Desc: Called when the dialog's slider bars are changed by the user, or need
        '       updating
        '-----------------------------------------------------------------------------
        ' Get the position of the sliders
        Dim DopplerFactor As Single = ConvertLinearSliderPosToLogScale(trackbarDopplerSlider.Value)
        Dim RolloffFactor As Single = ConvertLinearSliderPosToLogScale(trackbarRolloffSlider.Value)
        Dim MinDistance As Single = ConvertLinearSliderPosToLogScale(trackbarMindistanceSlider.Value)
        Dim MaxDistance As Single = ConvertLinearSliderPosToLogScale(trackbarMaxdistanceSlider.Value)

        ' Set the static text boxes
        labelDopplerfactor.Text = DopplerFactor.ToString("0.00")
        labelRollofffactor.Text = RolloffFactor.ToString("0.00")
        labelMindistance.Text = MinDistance.ToString("0.00")
        labelMaxdistance.Text = MaxDistance.ToString("0.00")

        ' Set the options in the DirectSound buffer
        Set3DParameters(DopplerFactor, RolloffFactor, MinDistance, MaxDistance)

        buttonApply.Enabled = IIf(True = checkboxDefer.Checked, True, False)
    End Sub 'SliderChanged

    Sub Set3DParameters(ByVal DopplerFactor As Single, ByVal RolloffFactor As Single, ByVal MinDistance As Single, ByVal MaxDistance As Single)
        '-----------------------------------------------------------------------------
        ' Name: Set3DParameters()
        ' Desc: Set the 3D buffer parameters
        '-----------------------------------------------------------------------------
        ' Every change to 3-D sound buffer and listener settings causes 
        ' DirectSound to remix, at the expense of CPU cycles. 
        ' To minimize the performance impact of changing 3-D settings, 
        ' use the DS3D_DEFERRED flag in the dwApply parameter of any of 
        ' the IDirectSound3DListener or IDirectSound3DBuffer methods that 
        ' change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings 
        ' method to execute all of the deferred commands at once.
        listenerParameters.DopplerFactor = DopplerFactor
        listenerParameters.RolloffFactor = RolloffFactor

        If Not Nothing Is applicationListener Then
            applicationListener.AllParameters = listenerParameters
        End If
        application3DSettings.MinDistance = MinDistance
        application3DSettings.MaxDistance = MaxDistance

        If Not Nothing Is applicationBuffer3D Then
            applicationBuffer3D.AllParameters = application3DSettings
        End If
    End Sub 'Set3DParameters

    Sub StopPlaying()
        If Not Nothing Is applicationBuffer3D Then
            applicationBuffer.Stop()
            applicationBuffer.SetCurrentPosition(0)
        End If

        ' Update the UI controls to show the sound as stopped
        EnablePlayUI(True)
        labelStatus.Text = "Sound stopped."
    End Sub 'Stop

    Private Sub buttonStop_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonStop.Click
        EnablePlayUI(True)
        StopPlaying()
    End Sub 'buttonStop_Click

    Private Sub checkboxDefer_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles checkboxDefer.CheckedChanged
        applicationBuffer3D.Deferred = checkboxDefer.Checked
        SliderChanged()
    End Sub 'checkboxDefer_CheckedChanged

    Private Sub Apply()
        ' Call the DirectSound.3DListener.CommitDeferredSettings 
        ' method to execute all of the deferred commands at once.
        ' This is many times more efficent than recomputing everything
        ' for every call.
        If Not Nothing Is applicationListener Then
            applicationListener.CommitDeferredSettings()
        End If
    End Sub 'Apply

    Private Sub buttonApply_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonApply.Click
        Apply()
    End Sub 'buttonApply_Click

    Private Sub MovementTimer_Elapsed(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs) Handles timerMovement.Elapsed
        '-----------------------------------------------------------------------------
        ' Name: MovementTimer_Elapsed()
        ' Desc: Periodically updates the position of the object 
        '-----------------------------------------------------------------------------
        Dim XScale As Single
        Dim YScale As Single

        XScale = trackbarHorizontalSlider.Value / 110.0F
        YScale = trackbarVerticalSlider.Value / 110.0F
        Dim t As Single = Environment.TickCount / 1000.0F

        ' Move the sound object around the listener. The maximum radius of the
        ' orbit is 27.5 units.
        Dim vPosition As Vector3
        vPosition.X = maxOrbitRadius * XScale * CSng(Math.Sin(t))
        vPosition.Y = 0.0F
        vPosition.Z = maxOrbitRadius * YScale * CSng(Math.Cos(t))

        Dim vVelocity As Vector3
        vVelocity.X = maxOrbitRadius * XScale * CSng(Math.Sin((t + 0.05F)))
        vVelocity.Y = 0.0F
        vVelocity.Z = maxOrbitRadius * YScale * CSng(Math.Cos((t + 0.05F)))

        ' Show the object's position on the dialog's grid control
        UpdateGrid(vPosition.X, vPosition.Z)

        ' Set the sound buffer velocity and position
        SetObjectProperties(vPosition, vVelocity)
    End Sub 'MovementTimer_Elapsed


    Sub UpdateGrid(ByVal x As Single, ByVal y As Single)
        '-----------------------------------------------------------------------------
        ' Name: UpdateGrid()
        ' Desc: Draws a red dot in the dialog's grid bitmap at the x,y coordinate.
        '-----------------------------------------------------------------------------	   		
        If Nothing Is bitmapGrid Then
            bitmapGrid = New Bitmap(pictureboxRenderWindow.BackgroundImage)
            applicationGraphics = pictureboxRenderWindow.CreateGraphics()
        End If

        ' Convert the world space x,y coordinates to pixel coordinates
        x = CInt((x / maxOrbitRadius + 1) * (pictureboxRenderWindow.ClientRectangle.Left + pictureboxRenderWindow.ClientRectangle.Right) / 2)
        y = CInt((-y / maxOrbitRadius + 1) * (pictureboxRenderWindow.ClientRectangle.Top + pictureboxRenderWindow.ClientRectangle.Bottom) / 2)

        ' Draw a crosshair object in red pixels
        Dim r As New Rectangle(x, y, 3, 3)

        applicationGraphics.DrawImage(bitmapGrid, 1, 1)
        applicationGraphics.DrawEllipse(New Pen(Color.Red), r)
    End Sub 'UpdateGrid

    Sub SetObjectProperties(ByVal position As Vector3, ByVal velocity As Vector3)
        '-----------------------------------------------------------------------------
        ' Name: SetObjectProperties()
        ' Desc: Sets the position and velocity on the 3D buffer
        '-----------------------------------------------------------------------------
        ' Every change to 3-D sound buffer and listener settings causes 
        ' DirectSound to remix, at the expense of CPU cycles. 
        ' To minimize the performance impact of changing 3-D settings, 
        ' use the Deferred flag of the DirectSound.Listener3D or DirectSound.Buffer3D methods that 
        ' change 3-D settings. Then call the DirectSound.Listener3D.CommitSettings 
        ' method to execute all of the deferred commands at once.
        application3DSettings.Position = position
        application3DSettings.Velocity = velocity

        If Not Nothing Is applicationBuffer3D Then
            applicationBuffer3D.AllParameters = application3DSettings
        End If
    End Sub 'SetObjectProperties

    Private Sub trackbarSlider_Scroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles trackbarMaxdistanceSlider.Scroll, trackbarDopplerSlider.Scroll, trackbarMindistanceSlider.Scroll, trackbarRolloffSlider.Scroll, trackbarVerticalSlider.Scroll, trackbarHorizontalSlider.Scroll
        SliderChanged()
    End Sub 'trackbarSlider_Scroll
End Class 'Play3DSound