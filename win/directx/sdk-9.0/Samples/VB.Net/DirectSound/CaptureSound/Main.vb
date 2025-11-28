Imports System
Imports System.IO
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Threading
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectSound

Public Class MainForm
    Inherits Form
    Private labelMainInputformatText As Label
    Private WithEvents checkboxRecord As CheckBox
    Private WithEvents buttonSoundfile As Button
    Private labelFilename As Label
    Private labelStatic As Label

    Public PositionNotify(NumberRecordNotifications) As BufferPositionNotify
    Public Const NumberRecordNotifications As Integer = 16
    Public NotificationEvent As AutoResetEvent = Nothing
    Public applicationBuffer As CaptureBuffer = Nothing
    Public CaptureDeviceGuid As Guid = Guid.Empty
    Public applicationDevice As Capture = Nothing
    Private FileName As String = String.Empty
    Public applicationNotify As Notify = Nothing
    Private NotifyThread As Thread = Nothing
    Private WaveFile As FileStream = Nothing
    Private Writer As BinaryWriter = Nothing
    Private Path As String = String.Empty
    Public CaptureBufferSize As Integer = 0
    Public NextCaptureOffset As Integer = 0
    Private Recording As Boolean = False
    Public InputFormat As WaveFormat
    Private SampleCount As Integer = 0
    Public NotifySize As Integer = 0

    Public Shared Function Main(ByVal Args() As String) As Integer
        Dim form As New MainForm()
        form.ShowDialog()
        form.Dispose()

        Return 0
    End Function 'Main


    Private Sub MainForm_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        If Not Nothing Is NotificationEvent Then
            NotificationEvent.Set()
        End If
        If Not Nothing Is applicationBuffer Then
            If applicationBuffer.Capturing Then
                StartOrStopRecord(False)
            End If
        End If
    End Sub 'MainForm_Closing

    Public Sub New()
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()

        Dim devices As New DevicesForm(Me)
        devices.ShowDialog(Me)
        InitDirectSound()
        If Nothing Is applicationDevice Then
            Close()
        Else
            Dim formats As New FormatsForm(Me)
            formats.ShowDialog(Me)

            labelMainInputformatText.Text = String.Format("{0} Hz, {1}-bit ", InputFormat.SamplesPerSecond.ToString(), InputFormat.BitsPerSample.ToString()) + IIf(1 = InputFormat.Channels, "Mono", "Stereo")

            CreateCaptureBuffer()
        End If
    End Sub 'New

    Sub InitializeComponent()
        Me.labelStatic = New System.Windows.Forms.Label()
        Me.labelMainInputformatText = New System.Windows.Forms.Label()
        Me.checkboxRecord = New System.Windows.Forms.CheckBox()
        Me.buttonSoundfile = New System.Windows.Forms.Button()
        Me.labelFilename = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        ' 
        ' labelStatic
        ' 
        Me.labelStatic.Location = New System.Drawing.Point(10, 46)
        Me.labelStatic.Name = "labelStatic"
        Me.labelStatic.Size = New System.Drawing.Size(75, 13)
        Me.labelStatic.TabIndex = 0
        Me.labelStatic.Text = "Input Format:"
        ' 
        ' labelMainInputformatText
        ' 
        Me.labelMainInputformatText.Location = New System.Drawing.Point(90, 46)
        Me.labelMainInputformatText.Name = "labelMainInputformatText"
        Me.labelMainInputformatText.Size = New System.Drawing.Size(262, 13)
        Me.labelMainInputformatText.TabIndex = 1
        ' 
        ' checkboxRecord
        ' 
        Me.checkboxRecord.Appearance = System.Windows.Forms.Appearance.Button
        Me.checkboxRecord.Enabled = False
        Me.checkboxRecord.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.checkboxRecord.Location = New System.Drawing.Point(369, 40)
        Me.checkboxRecord.Name = "checkboxRecord"
        Me.checkboxRecord.Size = New System.Drawing.Size(75, 23)
        Me.checkboxRecord.TabIndex = 2
        Me.checkboxRecord.Text = "&Record"
        ' 
        ' buttonSoundfile
        ' 
        Me.buttonSoundfile.Location = New System.Drawing.Point(10, 11)
        Me.buttonSoundfile.Name = "buttonSoundfile"
        Me.buttonSoundfile.Size = New System.Drawing.Size(78, 21)
        Me.buttonSoundfile.TabIndex = 3
        Me.buttonSoundfile.Text = "Sound &file..."
        ' 
        ' labelFilename
        ' 
        Me.labelFilename.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelFilename.Location = New System.Drawing.Point(96, 11)
        Me.labelFilename.Name = "labelFilename"
        Me.labelFilename.Size = New System.Drawing.Size(348, 21)
        Me.labelFilename.TabIndex = 4
        Me.labelFilename.Text = "No file loaded."
        Me.labelFilename.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        ' 
        ' MainForm
        ' 
        Me.AcceptButton = Me.buttonSoundfile
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(454, 77)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.labelStatic, Me.labelMainInputformatText, Me.checkboxRecord, Me.buttonSoundfile, Me.labelFilename})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "MainForm"
        Me.Text = "CaptureSound"
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    Sub InitDirectSound()

        CaptureBufferSize = 0
        NotifySize = 0
        Try
            applicationDevice = New Capture(CaptureDeviceGuid)
        Catch
        End Try
    End Sub 'InitDirectSound

    Sub CreateCaptureBuffer()
        '-----------------------------------------------------------------------------
        ' Name: CreateCaptureBuffer()
        ' Desc: Creates a capture buffer and sets the format 
        '-----------------------------------------------------------------------------
        Dim dscheckboxd As New CaptureBufferDescription()

        If Not Nothing Is applicationNotify Then
            applicationNotify.Dispose()
            applicationNotify = Nothing
        End If
        If Not Nothing Is applicationBuffer Then
            applicationBuffer.Dispose()
            applicationBuffer = Nothing
        End If

        If 0 = InputFormat.Channels Then
            Return
        End If
        ' Set the notification size
        NotifySize = IIf(1024 > InputFormat.AverageBytesPerSecond / 8, 1024, InputFormat.AverageBytesPerSecond / 8)
        NotifySize -= NotifySize Mod InputFormat.BlockAlign

        ' Set the buffer sizes
        CaptureBufferSize = NotifySize * NumberRecordNotifications

        ' Create the capture buffer
        dscheckboxd.BufferBytes = CaptureBufferSize
        InputFormat.FormatTag = WaveFormatTag.Pcm
        dscheckboxd.Format = InputFormat ' Set the format during creatation
        applicationBuffer = New CaptureBuffer(dscheckboxd, applicationDevice)
        NextCaptureOffset = 0

        InitNotifications()
    End Sub 'CreateCaptureBuffer

    Sub InitNotifications()
        '-----------------------------------------------------------------------------
        ' Name: InitNotifications()
        ' Desc: Inits the notifications on the capture buffer which are handled
        '       in the notify thread.
        '-----------------------------------------------------------------------------
        If Nothing Is applicationBuffer Then
            Throw New ArgumentNullException()
        End If
        ' Create a thread to monitor the notify events
        If Nothing Is NotifyThread Then
            NotifyThread = New Thread(New ThreadStart(AddressOf WaitThread))
            NotifyThread.Start()

            ' Create a notification event, for when the sound stops playing
            NotificationEvent = New AutoResetEvent(False)
        End If

        ' Setup the notification positions
        Dim i As Integer
        For i = 0 To NumberRecordNotifications - 1
            PositionNotify(i).Offset = NotifySize * i + NotifySize - 1
            PositionNotify(i).EventNotifyHandle = NotificationEvent.Handle
        Next i

        applicationNotify = New Notify(applicationBuffer)

        ' Tell DirectSound when to notify the app. The notification will come in the from 
        ' of signaled events that are handled in the notify thread.
        applicationNotify.SetNotificationPositions(PositionNotify, NumberRecordNotifications)
    End Sub 'InitNotifications

    Private Sub checkboxRecord_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles checkboxRecord.CheckedChanged
        Recording = Not Recording
        StartOrStopRecord(Recording)

        If Not Recording Then
            checkboxRecord.Enabled = False
        End If
    End Sub 'checkboxRecord_CheckedChanged

    Sub StartOrStopRecord(ByVal StartRecording As Boolean)
        '-----------------------------------------------------------------------------
        ' Name: StartOrStopRecord()
        ' Desc: Starts or stops the capture buffer from recording
        '-----------------------------------------------------------------------------
        If StartRecording Then
            ' Create a capture buffer, and tell the capture 
            ' buffer to start recording   
            CreateCaptureBuffer()
            applicationBuffer.Start(True)
        Else
            ' Stop the capture and read any data that 
            ' was not caught by a notification
            If Nothing Is applicationBuffer Then
                Return
            End If
            ' Stop the buffer, and read any data that was not 
            ' caught by a notification
            applicationBuffer.Stop()

            RecordCapturedData()

            Writer.Seek(4, SeekOrigin.Begin) ' Seek to the length descriptor of the RIFF file.
            Writer.Write(CInt(SampleCount + 36)) ' Write the file length, minus first 8 bytes of RIFF description.
            Writer.Seek(40, SeekOrigin.Begin) ' Seek to the data length descriptor of the RIFF file.
            Writer.Write(SampleCount) ' Write the length of the sample data in bytes.
            Writer.Close() ' Close the file now.
            Writer = Nothing ' Set the writer to null.
            WaveFile = Nothing ' Set the FileStream to null.
        End If
    End Sub 'StartOrStopRecord

    Sub CreateRIFF()
        '*************************************************************************
        '
        '
        '	Here is where the file will be created. A
        '
        '	wave file is a RIFF file, which has chunks
        '
        '	of data that describe what the file contains.
        '
        '	A wave RIFF file is put together like this:
        '
        '
        '
        '	The 12 byte RIFF chunk is constructed like this:
        '
        '   Bytes(0 - 3) 'R' 'I' 'F' 'F'
        '
        '	Bytes 4 - 7 :	Length of file, minus the first 8 bytes of the RIFF description.
        '
        '					(4 bytes for "WAVE" + 24 bytes for format chunk length +
        '
        '					8 bytes for data chunk description + actual sample data size.)
        '
        '   Bytes(8 - 11) 'W' 'A' 'V' 'E'
        '
        '
        '
        '	The 24 byte FORMAT chunk is constructed like this:
        '
        '   Bytes(0 - 3) 'f' 'm' 't' ' '
        '
        '	Bytes 4 - 7 :	The format chunk length. This is always 16.
        '
        '	Bytes 8 - 9 :	File padding. Always 1.
        '
        '	Bytes 10- 11:	Number of channels. Either 1 for mono,  or 2 for stereo.
        '
        '	Bytes 12- 15:	Sample rate.
        '
        '	Bytes 16- 19:	Number of bytes per second.
        '
        '	Bytes 20- 21:	Bytes per sample. 1 for 8 bit mono, 2 for 8 bit stereo or
        '
        '					16 bit mono, 4 for 16 bit stereo.
        '
        '	Bytes 22- 23:	Number of bits per sample.
        '
        '
        '
        '	The DATA chunk is constructed like this:
        '
        '   Bytes(0 - 3) 'd' 'a' 't' 'a'
        '
        '	Bytes 4 - 7 :	Length of data, in bytes.
        '
        '	Bytes 8 -...:	Actual sample data.
        '
        '
        '
        '**************************************************************************

        ' Open up the wave file for writing.
        WaveFile = New FileStream(FileName, FileMode.Create)
        Writer = New BinaryWriter(WaveFile)

        ' Set up file with RIFF chunk info.
        Dim ChunkRiff As Char() = {"R", "I", "F", "F"}
        Dim ChunkType As Char() = {"W", "A", "V", "E"}
        Dim ChunkFmt As Char() = {"f", "m", "t", " "}
        Dim ChunkData As Char() = {"d", "a", "t", "a"}

        Dim shPad As Short = 1 ' File padding
        Dim nFormatChunkLength As Integer = &H10 ' Format chunk length.
        Dim nLength As Integer = 0 ' File length, minus first 8 bytes of RIFF description. This will be filled in later.
        Dim shBytesPerSample As Short = 0 ' Bytes per sample.
        ' Figure out how many bytes there will be per sample.
        If 8 = InputFormat.BitsPerSample And 1 = InputFormat.Channels Then
            shBytesPerSample = 1
        ElseIf 8 = InputFormat.BitsPerSample And 2 = InputFormat.Channels Or (16 = InputFormat.BitsPerSample And 1 = InputFormat.Channels) Then
            shBytesPerSample = 2
        ElseIf 16 = InputFormat.BitsPerSample And 2 = InputFormat.Channels Then
            shBytesPerSample = 4
        End If
        ' Fill in the riff info for the wave file.
        Writer.Write(ChunkRiff)
        Writer.Write(nLength)
        Writer.Write(ChunkType)

        ' Fill in the format info for the wave file.
        Writer.Write(ChunkFmt)
        Writer.Write(nFormatChunkLength)
        Writer.Write(shPad)
        Writer.Write(InputFormat.Channels)
        Writer.Write(InputFormat.SamplesPerSecond)
        Writer.Write(InputFormat.AverageBytesPerSecond)
        Writer.Write(shBytesPerSample)
        Writer.Write(InputFormat.BitsPerSample)

        ' Now fill in the data chunk.
        Writer.Write(ChunkData)
        Writer.Write(CInt(0)) ' The sample length will be written in later.
    End Sub 'CreateRIFF

    Sub RecordCapturedData()
        '-----------------------------------------------------------------------------
        ' Name: RecordCapturedData()
        ' Desc: Copies data from the capture buffer to the output buffer 
        '-----------------------------------------------------------------------------
        Dim CaptureData As Byte() = Nothing
        Dim ReadPos As Integer
        Dim CapturePos As Integer
        Dim LockSize As Integer

        If Nothing Is applicationBuffer Or Nothing Is WaveFile Then
            Return
        End If
        applicationBuffer.GetCurrentPosition(CapturePos, ReadPos)
        LockSize = ReadPos - NextCaptureOffset
        If LockSize < 0 Then
            LockSize += CaptureBufferSize
        End If
        ' Block align lock size so that we are always write on a boundary
        LockSize -= LockSize Mod NotifySize

        If 0 = LockSize Then
            Return
        End If
        ' Read the capture buffer.
        CaptureData = CType(applicationBuffer.Read(NextCaptureOffset, GetType(Byte), LockFlag.None, LockSize), Byte())

        ' Write the data into the wav file
        Writer.Write(CaptureData, 0, CaptureData.Length)

        ' Update the number of samples, in bytes, of the file so far.
        SampleCount += CaptureData.Length

        ' Move the capture offset along
        NextCaptureOffset += CaptureData.Length
        NextCaptureOffset = NextCaptureOffset Mod CaptureBufferSize ' Circular buffer
    End Sub 'RecordCapturedData


    Private Sub buttonSoundfile_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonSoundfile.Click
        '-----------------------------------------------------------------------------
        ' Name: OnSaveSoundFile()
        ' Desc: Called when the user requests to save to a sound file
        '-----------------------------------------------------------------------------
        Dim ofd As New SaveFileDialog()
        Dim wf As New WaveFormat()
        Dim result As DialogResult

        ' Get the default media path (something like C:\WINDOWS\MEDIA)
        If String.Empty = Path Then
            Path = Environment.SystemDirectory.Substring(0, Environment.SystemDirectory.LastIndexOf("\")) + "\media"
        End If
        If Recording Then
            ' Stop the capture and read any data that 
            ' was not caught by a notification
            StartOrStopRecord(False)
            Recording = False
        End If

        ofd.DefaultExt = ".wav"
        ofd.Filter = "Wave Files|*.wav|All Files|*.*"
        ofd.FileName = FileName
        ofd.InitialDirectory = Path

        ' Update the UI controls to show the sound as loading a file
        checkboxRecord.Enabled = False
        labelFilename.Text = "Saving file..."

        ' Display the OpenFileName dialog. Then, try to load the specified file
        result = ofd.ShowDialog(Me)

        If DialogResult.Abort = result Or DialogResult.Cancel = result Then
            labelFilename.Text = "Save aborted."
            Return
        End If
        FileName = ofd.FileName

        CreateRIFF()

        ' Update the UI controls to show the sound as the file is loaded
        labelFilename.Text = FileName
        checkboxRecord.Enabled = True

        ' Remember the path for next time
        Path = FileName.Substring(0, FileName.LastIndexOf("\"))
    End Sub 'buttonSoundfile_Click

    Private Sub WaitThread()
        While Created
            'Sit here and wait for a message to arrive
            NotificationEvent.WaitOne(Timeout.Infinite, True)
            RecordCapturedData()
        End While
    End Sub 'WaitThread
End Class 'MainForm 