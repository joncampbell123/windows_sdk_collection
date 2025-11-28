Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports System.IO
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectSound
Imports Buffer = Microsoft.DirectX.DirectSound.SecondaryBuffer

Public Class MainForm
    Inherits Form
    Private components As System.ComponentModel.IContainer

    Private WithEvents btnSoundfile As Button
    Private lblFilename As Label
    Private cbLoopCheck As CheckBox
    Private WithEvents btnPlay As Button
    Private WithEvents btnStop As Button
    Private WithEvents btnCancel As Button

    Private ApplicationBuffer As SecondaryBuffer = Nothing
    Private ApplicationDevice As Device = Nothing
    Private PathSoundFile As String = String.Empty
    Private IsPlaying As Boolean = False

    Public Shared Sub Main()
        Application.Run(New MainForm())
    End Sub

    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If (disposing) Then
            If (Not Nothing Is components) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    Public Sub New()

        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()
    End Sub

#Region "InitializeComponent code"
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.btnSoundfile = New System.Windows.Forms.Button()
        Me.lblFilename = New System.Windows.Forms.Label()
        Me.cbLoopCheck = New System.Windows.Forms.CheckBox()
        Me.btnPlay = New System.Windows.Forms.Button()
        Me.btnStop = New System.Windows.Forms.Button()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.SuspendLayout()
        '
        'btnSoundfile
        '
        Me.btnSoundfile.Location = New System.Drawing.Point(10, 11)
        Me.btnSoundfile.Name = "btnSoundfile"
        Me.btnSoundfile.Size = New System.Drawing.Size(69, 21)
        Me.btnSoundfile.TabIndex = 0
        Me.btnSoundfile.Text = "Sound &file..."
        '
        'lblFilename
        '
        Me.lblFilename.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblFilename.Location = New System.Drawing.Point(94, 11)
        Me.lblFilename.Name = "lblFilename"
        Me.lblFilename.Size = New System.Drawing.Size(345, 21)
        Me.lblFilename.TabIndex = 1
        Me.lblFilename.Text = "No file loaded."
        Me.lblFilename.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cbLoopCheck
        '
        Me.cbLoopCheck.Enabled = False
        Me.cbLoopCheck.Location = New System.Drawing.Point(9, 44)
        Me.cbLoopCheck.Name = "cbLoopCheck"
        Me.cbLoopCheck.Size = New System.Drawing.Size(87, 16)
        Me.cbLoopCheck.TabIndex = 2
        Me.cbLoopCheck.Text = "&Loop sound"
        '
        'btnPlay
        '
        Me.btnPlay.Enabled = False
        Me.btnPlay.Location = New System.Drawing.Point(104, 48)
        Me.btnPlay.Name = "btnPlay"
        Me.btnPlay.TabIndex = 3
        Me.btnPlay.Text = "&Play"
        '
        'btnStop
        '
        Me.btnStop.Enabled = False
        Me.btnStop.Location = New System.Drawing.Point(176, 48)
        Me.btnStop.Name = "btnStop"
        Me.btnStop.TabIndex = 4
        Me.btnStop.Text = "&Stop"
        '
        'btnCancel
        '
        Me.btnCancel.Location = New System.Drawing.Point(364, 48)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.TabIndex = 5
        Me.btnCancel.Text = "E&xit"
        '
        'Timer1
        '
        Me.Timer1.Enabled = True
        Me.Timer1.Interval = 250
        '
        'MainForm
        '
        Me.AcceptButton = Me.btnSoundfile
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(450, 77)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnSoundfile, Me.lblFilename, Me.cbLoopCheck, Me.btnPlay, Me.btnStop, Me.btnCancel})
        Me.Name = "MainForm"
        Me.Text = "PlaySound"
        Me.ResumeLayout(False)

    End Sub
#End Region

    Private Sub btnCancel_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnCancel.Click
        Close()
    End Sub

    Private Sub btnStop_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnStop.Click
        If (Not Nothing Is ApplicationBuffer) Then
            ApplicationBuffer.Stop()
        End If
    End Sub

    Private Sub btnSoundfile_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnSoundfile.Click
        Dim ofd As OpenFileDialog = New OpenFileDialog()

        If (String.Empty = PathSoundFile) Then
            PathSoundFile = DXUtil.SdkMediaPath
        End If

        ofd.InitialDirectory = PathSoundFile
        ofd.Filter = "Wave files(*.wav)|*.wav"

        If (DialogResult.Cancel = ofd.ShowDialog()) Then Return

        If (LoadSoundFile(ofd.FileName)) Then
            PathSoundFile = Path.GetDirectoryName(ofd.FileName)
            lblFilename.Text = Path.GetFileName(ofd.FileName)
            EnablePlayUI(True)            
        Else

            lblFilename.Text = "No file loaded."
            EnablePlayUI(False)
        End If
    End Sub

    Private Function LoadSoundFile(ByVal name As String) As Boolean

        Try
            ApplicationBuffer = New SecondaryBuffer(name, ApplicationDevice)
        Catch e As SoundException
            Return False
        End Try
        Return True

    End Function

    Private Sub EnablePlayUI(ByVal enable As Boolean)
        If (enable) Then
            cbLoopCheck.Enabled = True
            btnCancel.Enabled = True
            btnPlay.Enabled = True
        Else
            cbLoopCheck.Enabled = False
            btnCancel.Enabled = False
            btnPlay.Enabled = False
            btnStop.Enabled = False
        End If
    End Sub

    Private Sub MainForm_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ApplicationDevice = New Device()
        ApplicationDevice.SetCooperativeLevel(Me, CooperativeLevel.Priority)
    End Sub

    Private Sub btnPlay_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnPlay.Click

        If (Not Nothing Is ApplicationBuffer) Then
            ApplicationBuffer.Play(0, IIf(cbLoopCheck.Checked, BufferPlayFlags.Looping, BufferPlayFlags.Default))
            lblFilename.Text = "File playing."
            btnStop.Enabled = True
            btnPlay.Enabled = False
        End If

    End Sub

    Private Sub Timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer1.Tick

        If (Not Nothing Is ApplicationBuffer) Then
            If (ApplicationBuffer.Status.Playing) Then
                IsPlaying = True
                Return
            ElseIf (IsPlaying) Then
                lblFilename.Text = "File stopped."
                btnStop.Enabled = False
                btnPlay.Enabled = True
                IsPlaying = False
            End If
        End If

    End Sub
End Class
