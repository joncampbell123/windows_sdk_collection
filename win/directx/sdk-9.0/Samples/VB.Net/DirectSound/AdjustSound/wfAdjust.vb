Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectSound
Imports Buffer = Microsoft.DirectX.DirectSound.Buffer

Namespace csAdjustSound

    Public Class wfAdjust
        Inherits System.Windows.Forms.Form
        Private applicationDevice As Device = Nothing
        Private applicationBuffer As SecondaryBuffer = Nothing
        Private lastGoodFrequency As Integer = 0
        Private maxFrequency As Integer = 200000 ' The maximum frequency we'll allow this sample to support
        Private WithEvents buttonSound As System.Windows.Forms.Button
        Private textFile As System.Windows.Forms.TextBox '
        Private textStatus As System.Windows.Forms.TextBox
        Private label1 As System.Windows.Forms.Label
        Private WithEvents buttonExit As System.Windows.Forms.Button
        Private WithEvents buttonPlay As System.Windows.Forms.Button
        Private WithEvents buttonStop As System.Windows.Forms.Button
        Private WithEvents checkLoop As System.Windows.Forms.CheckBox
        Private label2 As System.Windows.Forms.Label
        Private textFreq As System.Windows.Forms.TextBox
        Private WithEvents tbarFreq As System.Windows.Forms.TrackBar
        Private label5 As System.Windows.Forms.Label
        Private label6 As System.Windows.Forms.Label
        Private textPan As System.Windows.Forms.TextBox
        Private label7 As System.Windows.Forms.Label
        Private label8 As System.Windows.Forms.Label
        Private label9 As System.Windows.Forms.Label
        Private label10 As System.Windows.Forms.Label
        Private groupBox1 As System.Windows.Forms.GroupBox
        Private WithEvents tmrUpdate As System.Timers.Timer
        Private label11 As System.Windows.Forms.Label
        Private groupBox2 As System.Windows.Forms.GroupBox
        Private WithEvents radioSticky As System.Windows.Forms.RadioButton
        Private label13 As System.Windows.Forms.Label
        Private WithEvents radioGlobal As System.Windows.Forms.RadioButton
        Private WithEvents radioNormal As System.Windows.Forms.RadioButton
        Private groupBox4 As System.Windows.Forms.GroupBox
        Private WithEvents radioHardware As System.Windows.Forms.RadioButton
        Private label12 As System.Windows.Forms.Label
        Private WithEvents radioSoftware As System.Windows.Forms.RadioButton
        Private WithEvents radioDefault As System.Windows.Forms.RadioButton
        Private groupBox5 As System.Windows.Forms.GroupBox
        Private lblBehavior As System.Windows.Forms.Label
        Private WithEvents tbarPan As System.Windows.Forms.TrackBar
        Private WithEvents tbarVolume As System.Windows.Forms.TrackBar
        Private ofdFile As System.Windows.Forms.OpenFileDialog
        Private textVolume As System.Windows.Forms.TextBox
        Private lblMaxFreq As System.Windows.Forms.Label
        Private lblMinFreq As System.Windows.Forms.Label
        Private components As System.ComponentModel.Container = Nothing

        Public Sub New() '
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            Try
                'Initialize DirectSound
                applicationDevice = New Device()
                applicationDevice.SetCooperativeLevel(Me, CooperativeLevel.Priority)
            Catch
                MessageBox.Show("Could not initialize DirectSound.  Sample will exit.", "Exiting...", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Close()
                Exit Sub
            End Try

            ' Now that we have a sound device object set the frequency sliders correctly
            Me.tbarFreq.Minimum = 100
            Me.tbarFreq.Maximum = maxFrequency
            Me.lblMinFreq.Text = String.Format("{0} Hz", Me.tbarFreq.Minimum)
            Me.lblMaxFreq.Text = String.Format("{0} KHz", Me.tbarFreq.Maximum / 1000)

            UpdateBehaviorText()

        End Sub 'New

        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not Nothing Is components Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose

        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.radioDefault = New System.Windows.Forms.RadioButton()
            Me.tmrUpdate = New System.Timers.Timer()
            Me.checkLoop = New System.Windows.Forms.CheckBox()
            Me.textVolume = New System.Windows.Forms.TextBox()
            Me.label10 = New System.Windows.Forms.Label()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.groupBox4 = New System.Windows.Forms.GroupBox()
            Me.radioHardware = New System.Windows.Forms.RadioButton()
            Me.label12 = New System.Windows.Forms.Label()
            Me.radioSoftware = New System.Windows.Forms.RadioButton()
            Me.groupBox2 = New System.Windows.Forms.GroupBox()
            Me.radioSticky = New System.Windows.Forms.RadioButton()
            Me.label13 = New System.Windows.Forms.Label()
            Me.radioGlobal = New System.Windows.Forms.RadioButton()
            Me.radioNormal = New System.Windows.Forms.RadioButton()
            Me.textFile = New System.Windows.Forms.TextBox()
            Me.groupBox5 = New System.Windows.Forms.GroupBox()
            Me.lblBehavior = New System.Windows.Forms.Label()
            Me.buttonSound = New System.Windows.Forms.Button()
            Me.textStatus = New System.Windows.Forms.TextBox()
            Me.buttonPlay = New System.Windows.Forms.Button()
            Me.tbarPan = New System.Windows.Forms.TrackBar()
            Me.ofdFile = New System.Windows.Forms.OpenFileDialog()
            Me.buttonExit = New System.Windows.Forms.Button()
            Me.label11 = New System.Windows.Forms.Label()
            Me.tbarVolume = New System.Windows.Forms.TrackBar()
            Me.label8 = New System.Windows.Forms.Label()
            Me.label9 = New System.Windows.Forms.Label()
            Me.buttonStop = New System.Windows.Forms.Button()
            Me.lblMaxFreq = New System.Windows.Forms.Label()
            Me.label5 = New System.Windows.Forms.Label()
            Me.label6 = New System.Windows.Forms.Label()
            Me.label7 = New System.Windows.Forms.Label()
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.lblMinFreq = New System.Windows.Forms.Label()
            Me.textFreq = New System.Windows.Forms.TextBox()
            Me.tbarFreq = New System.Windows.Forms.TrackBar()
            Me.textPan = New System.Windows.Forms.TextBox()
            CType(Me.tmrUpdate, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.groupBox1.SuspendLayout()
            Me.groupBox4.SuspendLayout()
            Me.groupBox2.SuspendLayout()
            Me.groupBox5.SuspendLayout()
            CType(Me.tbarPan, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.tbarVolume, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.tbarFreq, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.SuspendLayout()
            ' 
            ' radioDefault
            ' 
            Me.radioDefault.Checked = True
            Me.radioDefault.Location = New System.Drawing.Point(93, 16)
            Me.radioDefault.Name = "radioDefault"
            Me.radioDefault.Size = New System.Drawing.Size(79, 14)
            Me.radioDefault.TabIndex = 3
            Me.radioDefault.TabStop = True
            Me.radioDefault.Text = "Default"
            ' 
            ' tmrUpdate
            ' 
            Me.tmrUpdate.Enabled = True
            Me.tmrUpdate.SynchronizingObject = Me
            ' 
            ' checkLoop
            ' 
            Me.checkLoop.Location = New System.Drawing.Point(8, 399)
            Me.checkLoop.Name = "checkLoop"
            Me.checkLoop.Size = New System.Drawing.Size(151, 19)
            Me.checkLoop.TabIndex = 3
            Me.checkLoop.Text = "Loop Sound"
            ' 
            ' textVolume
            ' 
            Me.textVolume.Location = New System.Drawing.Point(85, 148)
            Me.textVolume.Name = "textVolume"
            Me.textVolume.ReadOnly = True
            Me.textVolume.Size = New System.Drawing.Size(43, 20)
            Me.textVolume.TabIndex = 1
            Me.textVolume.Text = "0"
            ' 
            ' label10
            ' 
            Me.label10.Location = New System.Drawing.Point(6, 140)
            Me.label10.Name = "label10"
            Me.label10.Size = New System.Drawing.Size(73, 38)
            Me.label10.TabIndex = 2
            Me.label10.Text = "Volume"
            Me.label10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' groupBox1
            ' 
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.groupBox4, Me.groupBox2})
            Me.groupBox1.Location = New System.Drawing.Point(6, 181)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(437, 91)
            Me.groupBox1.TabIndex = 5
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Buffer Settings"
            ' 
            ' groupBox4
            ' 
            Me.groupBox4.Controls.AddRange(New System.Windows.Forms.Control() {Me.radioHardware, Me.label12, Me.radioSoftware, Me.radioDefault})
            Me.groupBox4.Location = New System.Drawing.Point(8, 48)
            Me.groupBox4.Name = "groupBox4"
            Me.groupBox4.Size = New System.Drawing.Size(423, 36)
            Me.groupBox4.TabIndex = 6
            Me.groupBox4.TabStop = False
            ' 
            ' radioHardware
            ' 
            Me.radioHardware.Location = New System.Drawing.Point(173, 16)
            Me.radioHardware.Name = "radioHardware"
            Me.radioHardware.Size = New System.Drawing.Size(79, 14)
            Me.radioHardware.TabIndex = 3
            Me.radioHardware.Text = "Hardware"
            ' 
            ' label12
            ' 
            Me.label12.Location = New System.Drawing.Point(6, 13)
            Me.label12.Name = "label12"
            Me.label12.Size = New System.Drawing.Size(73, 15)
            Me.label12.TabIndex = 2
            Me.label12.Text = "Buffer Mixing"
            Me.label12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' radioSoftware
            ' 
            Me.radioSoftware.Location = New System.Drawing.Point(268, 17)
            Me.radioSoftware.Name = "radioSoftware"
            Me.radioSoftware.Size = New System.Drawing.Size(79, 14)
            Me.radioSoftware.TabIndex = 3
            Me.radioSoftware.Text = "Software"
            ' 
            ' groupBox2
            ' 
            Me.groupBox2.Controls.AddRange(New System.Windows.Forms.Control() {Me.radioSticky, Me.label13, Me.radioGlobal, Me.radioNormal})
            Me.groupBox2.Location = New System.Drawing.Point(7, 11)
            Me.groupBox2.Name = "groupBox2"
            Me.groupBox2.Size = New System.Drawing.Size(423, 36)
            Me.groupBox2.TabIndex = 6
            Me.groupBox2.TabStop = False
            ' 
            ' radioSticky
            ' 
            Me.radioSticky.Location = New System.Drawing.Point(173, 16)
            Me.radioSticky.Name = "radioSticky"
            Me.radioSticky.Size = New System.Drawing.Size(79, 16)
            Me.radioSticky.TabIndex = 3
            Me.radioSticky.Text = "Sticky"
            ' 
            ' label13
            ' 
            Me.label13.Location = New System.Drawing.Point(6, 13)
            Me.label13.Name = "label13"
            Me.label13.Size = New System.Drawing.Size(73, 15)
            Me.label13.TabIndex = 2
            Me.label13.Text = "Focus"
            Me.label13.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' radioGlobal
            ' 
            Me.radioGlobal.Location = New System.Drawing.Point(268, 17)
            Me.radioGlobal.Name = "radioGlobal"
            Me.radioGlobal.Size = New System.Drawing.Size(79, 14)
            Me.radioGlobal.TabIndex = 3
            Me.radioGlobal.Text = "Global"
            ' 
            ' radioNormal
            ' 
            Me.radioNormal.Checked = True
            Me.radioNormal.Location = New System.Drawing.Point(93, 16)
            Me.radioNormal.Name = "radioNormal"
            Me.radioNormal.Size = New System.Drawing.Size(79, 14)
            Me.radioNormal.TabIndex = 3
            Me.radioNormal.TabStop = True
            Me.radioNormal.Text = "Normal"
            ' 
            ' textFile
            ' 
            Me.textFile.Location = New System.Drawing.Point(85, 6)
            Me.textFile.Name = "textFile"
            Me.textFile.ReadOnly = True
            Me.textFile.Size = New System.Drawing.Size(350, 20)
            Me.textFile.TabIndex = 1
            Me.textFile.Text = ""
            ' 
            ' groupBox5
            ' 
            Me.groupBox5.Controls.AddRange(New System.Windows.Forms.Control() {Me.lblBehavior})
            Me.groupBox5.Location = New System.Drawing.Point(8, 278)
            Me.groupBox5.Name = "groupBox5"
            Me.groupBox5.Size = New System.Drawing.Size(431, 120)
            Me.groupBox5.TabIndex = 6
            Me.groupBox5.TabStop = False
            Me.groupBox5.Text = "Expected Behavior"
            ' 
            ' lblBehavior
            ' 
            Me.lblBehavior.Location = New System.Drawing.Point(6, 16)
            Me.lblBehavior.Name = "lblBehavior"
            Me.lblBehavior.Size = New System.Drawing.Size(422, 100)
            Me.lblBehavior.TabIndex = 0
            Me.lblBehavior.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' buttonSound
            ' 
            Me.buttonSound.Location = New System.Drawing.Point(3, 6)
            Me.buttonSound.Name = "buttonSound"
            Me.buttonSound.Size = New System.Drawing.Size(74, 21)
            Me.buttonSound.TabIndex = 0
            Me.buttonSound.Text = "Sound File..."
            ' 
            ' textStatus
            ' 
            Me.textStatus.Location = New System.Drawing.Point(85, 33)
            Me.textStatus.Name = "textStatus"
            Me.textStatus.ReadOnly = True
            Me.textStatus.Size = New System.Drawing.Size(350, 20)
            Me.textStatus.TabIndex = 1
            Me.textStatus.Text = "No File Loaded."
            ' 
            ' buttonPlay
            ' 
            Me.buttonPlay.Enabled = False
            Me.buttonPlay.Location = New System.Drawing.Point(7, 421)
            Me.buttonPlay.Name = "buttonPlay"
            Me.buttonPlay.Size = New System.Drawing.Size(74, 21)
            Me.buttonPlay.TabIndex = 0
            Me.buttonPlay.Text = "Play"
            ' 
            ' tbarPan
            ' 
            Me.tbarPan.Location = New System.Drawing.Point(164, 97)
            Me.tbarPan.Maximum = 20
            Me.tbarPan.Minimum = -20
            Me.tbarPan.Name = "tbarPan"
            Me.tbarPan.Size = New System.Drawing.Size(236, 42)
            Me.tbarPan.TabIndex = 4
            Me.tbarPan.TickFrequency = 5
            ' 
            ' ofdFile
            ' 
            Me.ofdFile.Filter = "Wave Files (*.wav)|*.wav|All Files (*.*)|*.*"
            Me.ofdFile.Title = "Open Audio File"
            ' 
            ' buttonExit
            ' 
            Me.buttonExit.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.buttonExit.Location = New System.Drawing.Point(362, 421)
            Me.buttonExit.Name = "buttonExit"
            Me.buttonExit.Size = New System.Drawing.Size(74, 21)
            Me.buttonExit.TabIndex = 0
            Me.buttonExit.Text = "Exit"
            ' 
            ' label11
            ' 
            Me.label11.Location = New System.Drawing.Point(6, 13)
            Me.label11.Name = "label11"
            Me.label11.Size = New System.Drawing.Size(73, 15)
            Me.label11.TabIndex = 2
            Me.label11.Text = "Focus"
            Me.label11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' tbarVolume
            ' 
            Me.tbarVolume.Location = New System.Drawing.Point(165, 140)
            Me.tbarVolume.Maximum = 0
            Me.tbarVolume.Minimum = -50
            Me.tbarVolume.Name = "tbarVolume"
            Me.tbarVolume.Size = New System.Drawing.Size(236, 42)
            Me.tbarVolume.TabIndex = 4
            ' 
            ' label8
            ' 
            Me.label8.Location = New System.Drawing.Point(131, 147)
            Me.label8.Name = "label8"
            Me.label8.Size = New System.Drawing.Size(41, 20)
            Me.label8.TabIndex = 2
            Me.label8.Text = "Low"
            Me.label8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label9
            ' 
            Me.label9.Location = New System.Drawing.Point(396, 149)
            Me.label9.Name = "label9"
            Me.label9.Size = New System.Drawing.Size(47, 20)
            Me.label9.TabIndex = 2
            Me.label9.Text = "High"
            Me.label9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' buttonStop
            ' 
            Me.buttonStop.Enabled = False
            Me.buttonStop.Location = New System.Drawing.Point(87, 421)
            Me.buttonStop.Name = "buttonStop"
            Me.buttonStop.Size = New System.Drawing.Size(74, 21)
            Me.buttonStop.TabIndex = 0
            Me.buttonStop.Text = "Stop"
            ' 
            ' lblMaxFreq
            ' 
            Me.lblMaxFreq.Location = New System.Drawing.Point(396, 68)
            Me.lblMaxFreq.Name = "lblMaxFreq"
            Me.lblMaxFreq.Size = New System.Drawing.Size(47, 20)
            Me.lblMaxFreq.TabIndex = 2
            Me.lblMaxFreq.Text = "100 KHz"
            Me.lblMaxFreq.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label5
            ' 
            Me.label5.Location = New System.Drawing.Point(130, 104)
            Me.label5.Name = "label5"
            Me.label5.Size = New System.Drawing.Size(41, 20)
            Me.label5.TabIndex = 2
            Me.label5.Text = "Left"
            Me.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label6
            ' 
            Me.label6.Location = New System.Drawing.Point(395, 106)
            Me.label6.Name = "label6"
            Me.label6.Size = New System.Drawing.Size(47, 20)
            Me.label6.TabIndex = 2
            Me.label6.Text = "Right"
            Me.label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label7
            ' 
            Me.label7.Location = New System.Drawing.Point(5, 97)
            Me.label7.Name = "label7"
            Me.label7.Size = New System.Drawing.Size(73, 38)
            Me.label7.TabIndex = 2
            Me.label7.Text = "Pan"
            Me.label7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label1
            ' 
            Me.label1.Location = New System.Drawing.Point(4, 33)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(73, 20)
            Me.label1.TabIndex = 2
            Me.label1.Text = "Status"
            Me.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label2
            ' 
            Me.label2.Location = New System.Drawing.Point(6, 59)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(73, 38)
            Me.label2.TabIndex = 2
            Me.label2.Text = "Frequency"
            Me.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' lblMinFreq
            ' 
            Me.lblMinFreq.Location = New System.Drawing.Point(131, 66)
            Me.lblMinFreq.Name = "lblMinFreq"
            Me.lblMinFreq.Size = New System.Drawing.Size(41, 20)
            Me.lblMinFreq.TabIndex = 2
            Me.lblMinFreq.Text = "100 Hz"
            Me.lblMinFreq.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' textFreq
            ' 
            Me.textFreq.Location = New System.Drawing.Point(85, 67)
            Me.textFreq.Name = "textFreq"
            Me.textFreq.ReadOnly = True
            Me.textFreq.Size = New System.Drawing.Size(43, 20)
            Me.textFreq.TabIndex = 1
            Me.textFreq.Text = "0"
            ' 
            ' tbarFreq
            ' 
            Me.tbarFreq.LargeChange = 1000
            Me.tbarFreq.Location = New System.Drawing.Point(165, 59)
            Me.tbarFreq.Maximum = 100000
            Me.tbarFreq.Minimum = 100
            Me.tbarFreq.Name = "tbarFreq"
            Me.tbarFreq.Size = New System.Drawing.Size(236, 42)
            Me.tbarFreq.SmallChange = 100
            Me.tbarFreq.TabIndex = 4
            Me.tbarFreq.TickFrequency = 10000
            Me.tbarFreq.Value = 100
            ' 
            ' textPan
            ' 
            Me.textPan.Location = New System.Drawing.Point(85, 105)
            Me.textPan.Name = "textPan"
            Me.textPan.ReadOnly = True
            Me.textPan.Size = New System.Drawing.Size(43, 20)
            Me.textPan.TabIndex = 1
            Me.textPan.Text = "0"
            ' 
            ' wfAdjust
            ' 
            Me.AcceptButton = Me.buttonSound
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.CancelButton = Me.buttonExit
            Me.ClientSize = New System.Drawing.Size(460, 448)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.groupBox5, Me.groupBox1, Me.textVolume, Me.label8, Me.label9, Me.label10, Me.tbarVolume, Me.label5, Me.label6, Me.tbarPan, Me.textPan, Me.label7, Me.lblMaxFreq, Me.lblMinFreq, Me.textFreq, Me.tbarFreq, Me.label2, Me.checkLoop, Me.buttonStop, Me.buttonPlay, Me.buttonExit, Me.label1, Me.textStatus, Me.textFile, Me.buttonSound})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.Name = "wfAdjust"
            Me.Text = "AdjustSound"
            CType(Me.tmrUpdate, System.ComponentModel.ISupportInitialize).EndInit()
            Me.groupBox1.ResumeLayout(False)
            Me.groupBox4.ResumeLayout(False)
            Me.groupBox2.ResumeLayout(False)
            Me.groupBox5.ResumeLayout(False)
            CType(Me.tbarPan, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.tbarVolume, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.tbarFreq, System.ComponentModel.ISupportInitialize).EndInit()
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            Application.Run(New wfAdjust())
        End Sub 'Main

        Private Sub buttonExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonExit.Click
            Me.Dispose()
        End Sub 'buttonExit_Click

        Private Sub buttonSound_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonSound.Click
            Dim FocusSticky As Boolean = radioSticky.Checked
            Dim FocusGlobal As Boolean = radioGlobal.Checked
            Dim MixHardware As Boolean = radioHardware.Checked
            Dim MixSoftware As Boolean = radioSoftware.Checked

            ' Make sure we're stopped
            Me.buttonStop_Click(Nothing, Nothing)
            textStatus.Text = "Loading file..."
            If Not Nothing Is applicationBuffer Then
                applicationBuffer.Dispose()
            End If
            applicationBuffer = Nothing

            tbarFreq.Value = tbarFreq.Minimum
            tbarPan.Value = 0
            tbarVolume.Value = 0
            ' Show the open file dialog and let's load the file
            If Nothing = ofdFile.InitialDirectory Then
                ' Default to the 'My documents' folder if it's available
                ofdFile.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal)
            End If
            ' Clear out any old file name that may be in there
            ofdFile.FileName = Nothing
            If DialogResult.OK = ofdFile.ShowDialog() Then
                ' Save the initial dir as the last one picked.
                ofdFile.InitialDirectory = System.IO.Path.GetDirectoryName(ofdFile.FileName)
                Try
                    Dim desc As New BufferDescription()

                    desc.ControlFrequency = True
                    desc.ControlPan = True
                    desc.ControlVolume = True

                    If FocusGlobal Then
                        desc.GlobalFocus = True
                    End If
                    If FocusSticky Then
                        desc.StickyFocus = True
                    End If
                    If MixHardware Then
                        desc.LocateInHardware = True
                    End If
                    If MixSoftware Then
                        desc.LocateInSoftware = True
                    End If
                    applicationBuffer = New SecondaryBuffer(ofdFile.FileName, desc, applicationDevice)
                    tbarFreq.Value = desc.Format.SamplesPerSecond
                    lastGoodFrequency = desc.Format.SamplesPerSecond
                    textFile.Text = ofdFile.FileName
                    textFreq.Text = tbarFreq.Value.ToString()
                    textStatus.Text = "File loaded."
                    EnablePlayUI(True)
                Catch de As Exception
                    Console.WriteLine(de.ToString())
                    textFile.Text = Nothing
                    textStatus.Text = "Could not load this segment."
                    DefaultPlayUI()
                End Try
            Else
                textFile.Text = Nothing
                textStatus.Text = "Load aborted."
                DefaultPlayUI()
            End If
        End Sub 'buttonSound_Click

        Private Sub buttonStop_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonStop.Click
            If Not Nothing Is applicationBuffer Then
                textStatus.Text = "Sound stopped."
                EnablePlayUI(True)
                applicationBuffer.Stop()
                applicationBuffer.SetCurrentPosition(0)
            End If
        End Sub 'buttonStop_Click

        Private Sub tbarFreq_Scroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles tbarFreq.Scroll
            Dim newFrequency As Integer = 0
            If Not Nothing Is applicationBuffer Then
                Try
                    newFrequency = CType(sender, TrackBar).Value
                    ' Attempt to set the frequency to the new value
                    applicationBuffer.Frequency = newFrequency
                    textFreq.Text = newFrequency.ToString()
                    lastGoodFrequency = newFrequency
                Catch
                    ' Let's try to guess why it failed..
                    If applicationBuffer.Caps.LocateInHardware And newFrequency > applicationDevice.Caps.MaxSecondarySampleRate Then
                        textStatus.Text = "Hardware buffers don't support greater than Caps.MaxSecondarySampleRate"
                    ElseIf 100000 < newFrequency Then
                        ' Some platforms (pre-WinXP SP1) don't support 
                        ' >100k Hz so they will fail when setting it higher
                        textStatus.Text = "Some OS platforms do not support >100k Hz"
                    Else
                        textStatus.Text = "Setting the frequency failed"
                    End If
                    ' Reset to the last valid frequency
                    applicationBuffer.Frequency = lastGoodFrequency
                    CType(sender, TrackBar).Value = lastGoodFrequency
                End Try
            End If
        End Sub 'tbarFreq_Scroll

        Private Sub buttonPlay_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonPlay.Click
            Dim FocusSticky As Boolean = radioSticky.Checked
            Dim FocusGlobal As Boolean = radioGlobal.Checked
            Dim MixHardware As Boolean = radioHardware.Checked
            Dim MixSoftware As Boolean = radioSoftware.Checked

            If Not Nothing Is applicationBuffer Then
                textStatus.Text = "Sound playing."
                EnablePlayUI(False)
                Application.DoEvents() ' Process the Stop click that EnablePlayUI generates.
                ' First we need to 'recreate' the buffer
                If Not Nothing Is applicationBuffer Then
                    applicationBuffer.Dispose()
                End If
                applicationBuffer = Nothing

                Dim desc As New BufferDescription()
                desc.ControlFrequency = True
                desc.ControlPan = True
                desc.ControlVolume = True

                If FocusGlobal Then
                    desc.GlobalFocus = True
                End If
                If FocusSticky Then
                    desc.StickyFocus = True
                End If
                If MixHardware Then
                    desc.LocateInHardware = True
                End If
                If MixSoftware Then
                    desc.LocateInSoftware = True
                End If
                Try
                    applicationBuffer = New SecondaryBuffer(ofdFile.FileName, desc, applicationDevice)

                    Dim PlayFlags As BufferPlayFlags = IIf(checkLoop.Checked, BufferPlayFlags.Looping, 0)

                    ' Before we play, make sure we're using the correct settings
                    tbarFreq_Scroll(tbarFreq, Nothing)
                    tbarPan_Scroll(tbarPan, Nothing)
                    tbarVolume_Scroll(tbarVolume, Nothing)
                    applicationBuffer.Play(0, PlayFlags)
                Catch
                    textStatus.Text = "Could not open this file with these settings."
                    DefaultPlayUI()
                End Try
            End If
        End Sub 'buttonPlay_Click

        Private Sub tbarPan_Scroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles tbarPan.Scroll
            If Not Nothing Is applicationBuffer Then
                textPan.Text = CType(sender, TrackBar).Value.ToString()
                applicationBuffer.Pan = CType(sender, TrackBar).Value * 500
            End If
        End Sub 'tbarPan_Scroll

        Private Sub tbarVolume_Scroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles tbarVolume.Scroll
            If Not Nothing Is applicationBuffer Then
                textVolume.Text = CType(sender, TrackBar).Value.ToString()
                applicationBuffer.Volume = CType(sender, TrackBar).Value * 100
            End If
        End Sub 'tbarVolume_Scroll

        Private Sub EnablePlayUI(ByVal bEnable As Boolean)
            buttonPlay.Enabled = bEnable
            buttonStop.Enabled = Not bEnable
            buttonSound.Enabled = bEnable
            checkLoop.Enabled = bEnable
            radioDefault.Enabled = bEnable
            radioGlobal.Enabled = bEnable
            radioHardware.Enabled = bEnable
            radioNormal.Enabled = bEnable
            radioSoftware.Enabled = bEnable
            radioSticky.Enabled = bEnable
        End Sub 'EnablePlayUI

        Private Sub DefaultPlayUI()
            buttonPlay.Enabled = False
            buttonStop.Enabled = False
            buttonSound.Enabled = True
            checkLoop.Enabled = False
            radioDefault.Enabled = True
            radioGlobal.Enabled = True
            radioHardware.Enabled = True
            radioNormal.Enabled = True
            radioSoftware.Enabled = True
            radioSticky.Enabled = True
            textFile.Text = Nothing
        End Sub 'DefaultPlayUI

        Private Sub tmrUpdate_Elapsed(ByVal sender As Object, ByVal e As System.Timers.ElapsedEventArgs) Handles tmrUpdate.Elapsed
            If Not Nothing Is applicationBuffer Then
                If False = applicationBuffer.Status.Playing And False = applicationBuffer.Status.Looping Then
                    buttonStop_Click(Nothing, Nothing)
                End If
            End If
        End Sub 'tmrUpdate_Elapsed

        Private Sub UpdateBehaviorText()
            Dim sText As String = Nothing
            Dim Looped As Boolean = checkLoop.Checked
            Dim FocusSticky As Boolean = radioSticky.Checked
            Dim FocusGlobal As Boolean = radioGlobal.Checked
            Dim MixHardware As Boolean = radioHardware.Checked
            Dim MixSoftware As Boolean = radioSoftware.Checked

            ' Figure what the user should expect based on the dialog choice
            If FocusSticky Then
                sText = "Buffers with ""sticky"" focus will continue to play if the user switches to another application not using DirectSound.  However, if the user switches to another DirectSound application, all normal-focus and sticky-focus buffers in the previous application are muted."

            ElseIf FocusGlobal Then
                sText = "Buffers with global focus will continue to play if the user switches focus to another application, even if the new application uses DirectSound. The one exception is if you switch focus to a DirectSound application that uses the DSSCL_WRITEPRIMARY cooperative level. In this case, the global-focus buffers from other applications will not be audible."
            Else
                ' Normal focus
                sText = "Buffers with normal focus will mute if the user switches focus to any other application"
            End If

            If MixHardware Then
                sText = sText + ControlChars.Lf + ControlChars.Lf + "With the hardware mixing flag, the new buffer will be forced to use hardware mixing. If the device does not support hardware mixing or if the required hardware resources are not available, the call to the DirectSound.CreateSoundBuffer method will fail."
            ElseIf MixSoftware Then
                sText = sText + ControlChars.Lf + ControlChars.Lf + "With the software mixing flag, the new buffer will use software mixing, even if hardware resources are available."
            Else
                ' Default mixing
                sText = sText + ControlChars.Lf + ControlChars.Lf + "With default mixing, the new buffer will use hardware mixing if available, otherwise software mixing will be used."
            End If
            lblBehavior.Text = sText
        End Sub 'UpdateBehaviorText

        Private Sub RadioChecked(ByVal sender As Object, ByVal e As System.EventArgs) Handles radioDefault.CheckedChanged, radioHardware.CheckedChanged, radioSoftware.CheckedChanged, radioSticky.CheckedChanged, radioGlobal.CheckedChanged, radioNormal.CheckedChanged
            UpdateBehaviorText()
        End Sub 'RadioChecked
    End Class 'wfAdjust
End Namespace 'csAdjustSound