Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Collections
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectSound

Public Class FormatsForm
    Inherits Form

    Private Structure FormatInfo
        Public format As WaveFormat

        Public Overrides Function ToString() As String
            Return ConvertWaveFormatToString(format)
        End Function 'ToString
    End Structure 'FormatInfo

    Private WithEvents buttonOk As Button
    Private WithEvents buttonCancel As Button
    Private WithEvents lbFormatsInputListbox As ListBox
    Private labelStatic As Label

    Private mf As MainForm = Nothing
    Private formats As New ArrayList()
    Private InputFormatSupported(19) As Boolean

    Public Sub New(ByVal mf As MainForm)
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()
        Me.mf = mf

        ScanAvailableInputFormats()
        FillFormatListBox()
    End Sub 'New

    Sub InitializeComponent()
        Me.buttonOk = New System.Windows.Forms.Button()
        Me.buttonCancel = New System.Windows.Forms.Button()
        Me.lbFormatsInputListbox = New System.Windows.Forms.ListBox()
        Me.labelStatic = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        ' 
        ' buttonOk
        ' 
        Me.buttonOk.Enabled = False
        Me.buttonOk.Location = New System.Drawing.Point(10, 128)
        Me.buttonOk.Name = "buttonOk"
        Me.buttonOk.TabIndex = 0
        Me.buttonOk.Text = "OK"
        ' 
        ' buttonCancel
        ' 
        Me.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.buttonCancel.Location = New System.Drawing.Point(97, 128)
        Me.buttonCancel.Name = "buttonCancel"
        Me.buttonCancel.TabIndex = 1
        Me.buttonCancel.Text = "Cancel"
        ' 
        ' lbFormatsInputListbox
        ' 
        Me.lbFormatsInputListbox.Location = New System.Drawing.Point(10, 24)
        Me.lbFormatsInputListbox.Name = "lbFormatsInputListbox"
        Me.lbFormatsInputListbox.Size = New System.Drawing.Size(162, 95)
        Me.lbFormatsInputListbox.TabIndex = 2
        ' 
        ' labelStatic
        ' 
        Me.labelStatic.Location = New System.Drawing.Point(10, 11)
        Me.labelStatic.Name = "labelStatic"
        Me.labelStatic.Size = New System.Drawing.Size(75, 13)
        Me.labelStatic.TabIndex = 3
        Me.labelStatic.Text = "Input Format:"
        ' 
        ' FormatsForm
        ' 
        Me.AcceptButton = Me.buttonOk
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.CancelButton = Me.buttonCancel
        Me.ClientSize = New System.Drawing.Size(183, 170)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.buttonOk, Me.buttonCancel, Me.lbFormatsInputListbox, Me.labelStatic})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "FormatsForm"
        Me.Text = "Select Capture Format"
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    Sub ScanAvailableInputFormats()
        '-----------------------------------------------------------------------------
        ' Name: ScanAvailableInputFormats()
        ' Desc: Tests to see if 20 different standard wave formats are supported by
        '       the capture device 
        '-----------------------------------------------------------------------------
        Dim format As New WaveFormat()
        Dim dscheckboxd As New CaptureBufferDescription()
        Dim pDSCaptureBuffer As CaptureBuffer = Nothing

        ' This might take a second or two, so throw up the hourglass
        Cursor = Cursors.WaitCursor

        format.FormatTag = WaveFormatTag.Pcm

        ' Try 20 different standard formats to see if they are supported
        Dim iIndex As Integer
        For iIndex = 0 To 19
            GetWaveFormatFromIndex(iIndex, format)

            ' To test if a capture format is supported, try to create a 
            ' new capture buffer using a specific format.  If it works
            ' then the format is supported, otherwise not.
            dscheckboxd.BufferBytes = format.AverageBytesPerSecond
            dscheckboxd.Format = format

            Try
                pDSCaptureBuffer = New CaptureBuffer(dscheckboxd, mf.applicationDevice)
                InputFormatSupported(iIndex) = True
            Catch
                InputFormatSupported(iIndex) = False
            End Try
            pDSCaptureBuffer.Dispose()
        Next iIndex
        Cursor = Cursors.Default
    End Sub 'ScanAvailableInputFormats

    Private Sub GetWaveFormatFromIndex(ByVal Index As Integer, ByRef format As WaveFormat)
        '-----------------------------------------------------------------------------
        ' Name: GetWaveFormatFromIndex()
        ' Desc: Returns 20 different wave formats based on Index
        '-----------------------------------------------------------------------------
        Dim SampleRate As Integer = Index / 4
        Dim iType As Integer = Index Mod 4

        Select Case SampleRate
            Case 0
                format.SamplesPerSecond = 48000
            Case 1
                format.SamplesPerSecond = 44100
            Case 2
                format.SamplesPerSecond = 22050
            Case 3
                format.SamplesPerSecond = 11025
            Case 4
                format.SamplesPerSecond = 8000
        End Select

        Select Case iType
            Case 0
                format.BitsPerSample = 8
                format.Channels = 1
            Case 1
                format.BitsPerSample = 16
                format.Channels = 1
            Case 2
                format.BitsPerSample = 8
                format.Channels = 2
            Case 3
                format.BitsPerSample = 16
                format.Channels = 2
        End Select

        format.BlockAlign = CShort(format.Channels * (format.BitsPerSample / 8))
        format.AverageBytesPerSecond = format.BlockAlign * format.SamplesPerSecond
    End Sub 'GetWaveFormatFromIndex

    Sub FillFormatListBox()
        '-----------------------------------------------------------------------------
        ' Name: FillFormatListBox()
        ' Desc: Fills the format list box based on the availible formats
        '-----------------------------------------------------------------------------
        Dim info As New FormatInfo()
        Dim strFormatName As String = String.Empty
        Dim format As New WaveFormat()

        Dim iIndex As Integer
        For iIndex = 0 To InputFormatSupported.Length - 1
            If True = InputFormatSupported(iIndex) Then
                ' Turn the index into a WaveFormat then turn that into a
                ' string and put the string in the listbox
                GetWaveFormatFromIndex(iIndex, format)
                info.format = format
                formats.Add(info)
            End If
        Next iIndex
        lbFormatsInputListbox.DataSource = formats
    End Sub 'FillFormatListBox

    Private Shared Function ConvertWaveFormatToString(ByVal format As WaveFormat) As String
        '-----------------------------------------------------------------------------
        ' Name: ConvertWaveFormatToString()
        ' Desc: Converts a wave format to a text string
        '-----------------------------------------------------------------------------
        Return format.SamplesPerSecond.ToString() + " Hz, " + format.BitsPerSample.ToString() + "-bit " + IIf(format.Channels = 1, "Mono", "Stereo")
    End Function 'ConvertWaveFormatToString

    Private Sub FormatsOK()
        '-----------------------------------------------------------------------------
        ' Name: FormatsOK()
        ' Desc: Stores the capture buffer format based on what was selected
        '-----------------------------------------------------------------------------
        mf.InputFormat = CType(formats(lbFormatsInputListbox.SelectedIndex), FormatInfo).format
        Close()
    End Sub 'FormatsOK

    Private Sub buttonOk_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonOk.Click
        FormatsOK()
    End Sub 'buttonOk_Click


    Private Sub lbFormatsInputListbox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles lbFormatsInputListbox.SelectedIndexChanged
        buttonOk.Enabled = True
    End Sub 'lbFormatsInputListbox_SelectedIndexChanged
End Class 'FormatsForm