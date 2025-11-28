Imports System
Imports System.Drawing
Imports System.Collections
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectSound
Imports Buffer = Microsoft.DirectX.DirectSound.Buffer

Public Class MainForm
    Inherits Form

    Private WithEvents listboxEffects As System.Windows.Forms.ListBox
    Private WithEvents radiobuttonRadioSine As System.Windows.Forms.RadioButton '
    Private WithEvents buttonOk As Button
    Private groupboxFrame As GroupBox
    Private labelParamName1 As Label
    Private labelParamValue1 As Label
    Private WithEvents trackbarSlider1 As TrackBar
    Private labelParamMin1 As Label
    Private labelParamMax1 As Label
    Private labelParamName2 As Label
    Private labelParamValue2 As Label
    Private WithEvents trackbarSlider2 As TrackBar
    Private labelParamMin2 As Label
    Private labelParamMax2 As Label
    Private labelParamName3 As Label
    Private labelParamValue3 As Label
    Private WithEvents trackbarSlider3 As TrackBar
    Private labelParamMin3 As Label
    Private labelParamMax3 As Label
    Private labelParamName4 As Label
    Private labelParamValue4 As Label
    Private WithEvents trackbarSlider4 As TrackBar
    Private labelParamMin4 As Label
    Private labelParamMax4 As Label
    Private labelParamName5 As Label
    Private labelParamValue5 As Label
    Private WithEvents trackbarSlider5 As TrackBar
    Private labelParamMin5 As Label
    Private labelParamMax5 As Label
    Private labelParamName6 As Label
    Private labelParamValue6 As Label
    Private WithEvents trackbarSlider6 As TrackBar
    Private labelParamMin6 As Label
    Private labelParamMax6 As Label
    Private WithEvents radiobuttonTriangle As RadioButton
    Private WithEvents radiobuttonSquare As RadioButton
    Private groupboxFrameWaveform As GroupBox
    Private WithEvents buttonButtonOpen As Button
    Private labelTextFilename As Label
    Private labelStatic2 As Label
    Private labelTextStatus As Label
    Private WithEvents checkboxCheckLoop As CheckBox
    Private WithEvents buttonPlay As Button
    Private WithEvents buttonStop As Button
    Private labelStatic3 As Label
    Private groupboxFramePhase As GroupBox
    Private labelStatic4 As Label
    Private WithEvents radiobuttonRadioNeg180 As RadioButton
    Private WithEvents radiobuttonRadioNeg90 As RadioButton
    Private WithEvents radiobuttonRadioZero As RadioButton
    Private WithEvents radiobuttonRadio90 As RadioButton
    Private WithEvents radiobuttonRadio180 As RadioButton
    Private groupboxEffects As System.Windows.Forms.GroupBox
    Private WithEvents comboEffects As System.Windows.Forms.ComboBox

    Private Structure EffectInfo
        Public description As effectDescription
        Public EffectSettings As Object
        Public Effect As Object
    End Structure 'EffectInfo

    Private effectDescription As New ArrayList()
    Private applicationBuffer As SecondaryBuffer = Nothing
    Private applicationDevice As Device = Nothing
    Private fileName As String = String.Empty
    Private path As String = String.Empty
    Private shouldLoop As Boolean = False
    Private currentIndex As Integer = 0

    Public Shared Function Main(ByVal Args() As String) As Integer
        Application.Run(New MainForm())
        Return 0
    End Function 'Main

    Public Sub New()
        InitializeComponent()
        InitDirectSound()
    End Sub 'New

    Sub FormLoad(ByVal o As Object, ByVal args As EventArgs) Handles MyBase.Load
        ClearUI(True)
    End Sub

    Friend WithEvents buttonDelete As System.Windows.Forms.Button
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Private components As System.ComponentModel.IContainer

    Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.buttonOk = New System.Windows.Forms.Button()
        Me.groupboxFrame = New System.Windows.Forms.GroupBox()
        Me.labelParamMax1 = New System.Windows.Forms.Label()
        Me.labelParamMax2 = New System.Windows.Forms.Label()
        Me.labelParamMax3 = New System.Windows.Forms.Label()
        Me.labelParamMax4 = New System.Windows.Forms.Label()
        Me.labelParamMax5 = New System.Windows.Forms.Label()
        Me.labelParamMax6 = New System.Windows.Forms.Label()
        Me.labelParamMin1 = New System.Windows.Forms.Label()
        Me.labelParamMin2 = New System.Windows.Forms.Label()
        Me.labelParamMin3 = New System.Windows.Forms.Label()
        Me.labelParamMin4 = New System.Windows.Forms.Label()
        Me.labelParamMin5 = New System.Windows.Forms.Label()
        Me.labelParamMin6 = New System.Windows.Forms.Label()
        Me.trackbarSlider1 = New System.Windows.Forms.TrackBar()
        Me.trackbarSlider2 = New System.Windows.Forms.TrackBar()
        Me.trackbarSlider3 = New System.Windows.Forms.TrackBar()
        Me.trackbarSlider4 = New System.Windows.Forms.TrackBar()
        Me.trackbarSlider5 = New System.Windows.Forms.TrackBar()
        Me.trackbarSlider6 = New System.Windows.Forms.TrackBar()
        Me.labelParamValue1 = New System.Windows.Forms.Label()
        Me.labelParamValue2 = New System.Windows.Forms.Label()
        Me.labelParamValue3 = New System.Windows.Forms.Label()
        Me.labelParamValue4 = New System.Windows.Forms.Label()
        Me.labelParamValue5 = New System.Windows.Forms.Label()
        Me.labelParamValue6 = New System.Windows.Forms.Label()
        Me.labelParamName1 = New System.Windows.Forms.Label()
        Me.labelParamName2 = New System.Windows.Forms.Label()
        Me.labelParamName3 = New System.Windows.Forms.Label()
        Me.labelParamName4 = New System.Windows.Forms.Label()
        Me.labelParamName5 = New System.Windows.Forms.Label()
        Me.labelParamName6 = New System.Windows.Forms.Label()
        Me.radiobuttonTriangle = New System.Windows.Forms.RadioButton()
        Me.radiobuttonSquare = New System.Windows.Forms.RadioButton()
        Me.radiobuttonRadioSine = New System.Windows.Forms.RadioButton()
        Me.groupboxFrameWaveform = New System.Windows.Forms.GroupBox()
        Me.buttonButtonOpen = New System.Windows.Forms.Button()
        Me.labelTextFilename = New System.Windows.Forms.Label()
        Me.labelStatic2 = New System.Windows.Forms.Label()
        Me.labelTextStatus = New System.Windows.Forms.Label()
        Me.checkboxCheckLoop = New System.Windows.Forms.CheckBox()
        Me.buttonPlay = New System.Windows.Forms.Button()
        Me.buttonStop = New System.Windows.Forms.Button()
        Me.labelStatic3 = New System.Windows.Forms.Label()
        Me.labelStatic4 = New System.Windows.Forms.Label()
        Me.radiobuttonRadioNeg180 = New System.Windows.Forms.RadioButton()
        Me.radiobuttonRadioNeg90 = New System.Windows.Forms.RadioButton()
        Me.radiobuttonRadioZero = New System.Windows.Forms.RadioButton()
        Me.radiobuttonRadio90 = New System.Windows.Forms.RadioButton()
        Me.radiobuttonRadio180 = New System.Windows.Forms.RadioButton()
        Me.groupboxFramePhase = New System.Windows.Forms.GroupBox()
        Me.groupboxEffects = New System.Windows.Forms.GroupBox()
        Me.buttonDelete = New System.Windows.Forms.Button()
        Me.listboxEffects = New System.Windows.Forms.ListBox()
        Me.comboEffects = New System.Windows.Forms.ComboBox()
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.groupboxFrame.SuspendLayout()
        CType(Me.trackbarSlider1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarSlider2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarSlider3, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarSlider4, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarSlider5, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trackbarSlider6, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupboxFrameWaveform.SuspendLayout()
        Me.groupboxFramePhase.SuspendLayout()
        Me.groupboxEffects.SuspendLayout()
        Me.SuspendLayout()
        '
        'buttonOk
        '
        Me.buttonOk.Location = New System.Drawing.Point(87, 432)
        Me.buttonOk.Name = "buttonOk"
        Me.buttonOk.Size = New System.Drawing.Size(67, 23)
        Me.buttonOk.TabIndex = 0
        Me.buttonOk.Text = "E&xit"
        '
        'groupboxFrame
        '
        Me.groupboxFrame.Controls.AddRange(New System.Windows.Forms.Control() {Me.labelParamMax1, Me.labelParamMax2, Me.labelParamMax3, Me.labelParamMax4, Me.labelParamMax5, Me.labelParamMax6, Me.labelParamMin1, Me.labelParamMin2, Me.labelParamMin3, Me.labelParamMin4, Me.labelParamMin5, Me.labelParamMin6, Me.trackbarSlider1, Me.trackbarSlider2, Me.trackbarSlider3, Me.trackbarSlider4, Me.trackbarSlider5, Me.trackbarSlider6, Me.labelParamValue1, Me.labelParamValue2, Me.labelParamValue3, Me.labelParamValue4, Me.labelParamValue5, Me.labelParamValue6, Me.labelParamName1, Me.labelParamName2, Me.labelParamName3, Me.labelParamName4, Me.labelParamName5, Me.labelParamName6})
        Me.groupboxFrame.Location = New System.Drawing.Point(165, 76)
        Me.groupboxFrame.Name = "groupboxFrame"
        Me.groupboxFrame.Size = New System.Drawing.Size(525, 380)
        Me.groupboxFrame.TabIndex = 1
        Me.groupboxFrame.TabStop = False
        Me.groupboxFrame.Text = "Parameters"
        '
        'labelParamMax1
        '
        Me.labelParamMax1.Location = New System.Drawing.Point(464, 35)
        Me.labelParamMax1.Name = "labelParamMax1"
        Me.labelParamMax1.Size = New System.Drawing.Size(48, 13)
        Me.labelParamMax1.TabIndex = 6
        Me.labelParamMax1.Text = "max"
        Me.labelParamMax1.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMax2
        '
        Me.labelParamMax2.Location = New System.Drawing.Point(464, 83)
        Me.labelParamMax2.Name = "labelParamMax2"
        Me.labelParamMax2.Size = New System.Drawing.Size(48, 13)
        Me.labelParamMax2.TabIndex = 11
        Me.labelParamMax2.Text = "max"
        Me.labelParamMax2.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMax3
        '
        Me.labelParamMax3.Location = New System.Drawing.Point(464, 131)
        Me.labelParamMax3.Name = "labelParamMax3"
        Me.labelParamMax3.Size = New System.Drawing.Size(48, 13)
        Me.labelParamMax3.TabIndex = 16
        Me.labelParamMax3.Text = "max"
        Me.labelParamMax3.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMax4
        '
        Me.labelParamMax4.Location = New System.Drawing.Point(464, 179)
        Me.labelParamMax4.Name = "labelParamMax4"
        Me.labelParamMax4.Size = New System.Drawing.Size(48, 13)
        Me.labelParamMax4.TabIndex = 21
        Me.labelParamMax4.Text = "max"
        Me.labelParamMax4.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMax5
        '
        Me.labelParamMax5.Location = New System.Drawing.Point(464, 227)
        Me.labelParamMax5.Name = "labelParamMax5"
        Me.labelParamMax5.Size = New System.Drawing.Size(48, 13)
        Me.labelParamMax5.TabIndex = 26
        Me.labelParamMax5.Text = "max"
        Me.labelParamMax5.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMax6
        '
        Me.labelParamMax6.Location = New System.Drawing.Point(464, 275)
        Me.labelParamMax6.Name = "labelParamMax6"
        Me.labelParamMax6.Size = New System.Drawing.Size(48, 13)
        Me.labelParamMax6.TabIndex = 31
        Me.labelParamMax6.Text = "max"
        Me.labelParamMax6.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMin1
        '
        Me.labelParamMin1.Location = New System.Drawing.Point(200, 32)
        Me.labelParamMin1.Name = "labelParamMin1"
        Me.labelParamMin1.Size = New System.Drawing.Size(48, 16)
        Me.labelParamMin1.TabIndex = 5
        Me.labelParamMin1.Text = "min"
        Me.labelParamMin1.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMin2
        '
        Me.labelParamMin2.Location = New System.Drawing.Point(200, 80)
        Me.labelParamMin2.Name = "labelParamMin2"
        Me.labelParamMin2.Size = New System.Drawing.Size(48, 16)
        Me.labelParamMin2.TabIndex = 10
        Me.labelParamMin2.Text = "min"
        Me.labelParamMin2.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMin3
        '
        Me.labelParamMin3.Location = New System.Drawing.Point(200, 128)
        Me.labelParamMin3.Name = "labelParamMin3"
        Me.labelParamMin3.Size = New System.Drawing.Size(48, 16)
        Me.labelParamMin3.TabIndex = 15
        Me.labelParamMin3.Text = "min"
        Me.labelParamMin3.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMin4
        '
        Me.labelParamMin4.Location = New System.Drawing.Point(200, 176)
        Me.labelParamMin4.Name = "labelParamMin4"
        Me.labelParamMin4.Size = New System.Drawing.Size(48, 16)
        Me.labelParamMin4.TabIndex = 20
        Me.labelParamMin4.Text = "min"
        Me.labelParamMin4.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMin5
        '
        Me.labelParamMin5.Location = New System.Drawing.Point(200, 224)
        Me.labelParamMin5.Name = "labelParamMin5"
        Me.labelParamMin5.Size = New System.Drawing.Size(48, 16)
        Me.labelParamMin5.TabIndex = 25
        Me.labelParamMin5.Text = "min"
        Me.labelParamMin5.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamMin6
        '
        Me.labelParamMin6.Location = New System.Drawing.Point(200, 272)
        Me.labelParamMin6.Name = "labelParamMin6"
        Me.labelParamMin6.Size = New System.Drawing.Size(48, 16)
        Me.labelParamMin6.TabIndex = 30
        Me.labelParamMin6.Text = "min"
        Me.labelParamMin6.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'trackbarSlider1
        '
        Me.trackbarSlider1.Location = New System.Drawing.Point(256, 32)
        Me.trackbarSlider1.Name = "trackbarSlider1"
        Me.trackbarSlider1.Size = New System.Drawing.Size(195, 45)
        Me.trackbarSlider1.TabIndex = 4
        Me.trackbarSlider1.Text = "Slider1"
        Me.trackbarSlider1.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'trackbarSlider2
        '
        Me.trackbarSlider2.Location = New System.Drawing.Point(256, 80)
        Me.trackbarSlider2.Name = "trackbarSlider2"
        Me.trackbarSlider2.Size = New System.Drawing.Size(195, 45)
        Me.trackbarSlider2.TabIndex = 9
        Me.trackbarSlider2.Text = "Slider1"
        Me.trackbarSlider2.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'trackbarSlider3
        '
        Me.trackbarSlider3.Location = New System.Drawing.Point(256, 128)
        Me.trackbarSlider3.Name = "trackbarSlider3"
        Me.trackbarSlider3.Size = New System.Drawing.Size(195, 45)
        Me.trackbarSlider3.TabIndex = 14
        Me.trackbarSlider3.Text = "Slider1"
        Me.trackbarSlider3.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'trackbarSlider4
        '
        Me.trackbarSlider4.Location = New System.Drawing.Point(256, 176)
        Me.trackbarSlider4.Name = "trackbarSlider4"
        Me.trackbarSlider4.Size = New System.Drawing.Size(195, 45)
        Me.trackbarSlider4.TabIndex = 19
        Me.trackbarSlider4.Text = "Slider1"
        Me.trackbarSlider4.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'trackbarSlider5
        '
        Me.trackbarSlider5.Location = New System.Drawing.Point(256, 224)
        Me.trackbarSlider5.Name = "trackbarSlider5"
        Me.trackbarSlider5.Size = New System.Drawing.Size(195, 45)
        Me.trackbarSlider5.TabIndex = 24
        Me.trackbarSlider5.Text = "Slider1"
        Me.trackbarSlider5.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'trackbarSlider6
        '
        Me.trackbarSlider6.Location = New System.Drawing.Point(256, 272)
        Me.trackbarSlider6.Name = "trackbarSlider6"
        Me.trackbarSlider6.Size = New System.Drawing.Size(195, 45)
        Me.trackbarSlider6.TabIndex = 29
        Me.trackbarSlider6.Text = "Slider1"
        Me.trackbarSlider6.TickStyle = System.Windows.Forms.TickStyle.None
        '
        'labelParamValue1
        '
        Me.labelParamValue1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelParamValue1.Location = New System.Drawing.Point(136, 32)
        Me.labelParamValue1.Name = "labelParamValue1"
        Me.labelParamValue1.Size = New System.Drawing.Size(56, 16)
        Me.labelParamValue1.TabIndex = 3
        Me.labelParamValue1.Text = "Value"
        Me.labelParamValue1.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamValue2
        '
        Me.labelParamValue2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelParamValue2.Location = New System.Drawing.Point(136, 80)
        Me.labelParamValue2.Name = "labelParamValue2"
        Me.labelParamValue2.Size = New System.Drawing.Size(56, 16)
        Me.labelParamValue2.TabIndex = 8
        Me.labelParamValue2.Text = "Value"
        Me.labelParamValue2.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamValue3
        '
        Me.labelParamValue3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelParamValue3.Location = New System.Drawing.Point(136, 128)
        Me.labelParamValue3.Name = "labelParamValue3"
        Me.labelParamValue3.Size = New System.Drawing.Size(56, 16)
        Me.labelParamValue3.TabIndex = 13
        Me.labelParamValue3.Text = "Value"
        Me.labelParamValue3.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamValue4
        '
        Me.labelParamValue4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelParamValue4.Location = New System.Drawing.Point(136, 176)
        Me.labelParamValue4.Name = "labelParamValue4"
        Me.labelParamValue4.Size = New System.Drawing.Size(56, 16)
        Me.labelParamValue4.TabIndex = 18
        Me.labelParamValue4.Text = "Value"
        Me.labelParamValue4.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamValue5
        '
        Me.labelParamValue5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelParamValue5.Location = New System.Drawing.Point(136, 224)
        Me.labelParamValue5.Name = "labelParamValue5"
        Me.labelParamValue5.Size = New System.Drawing.Size(56, 16)
        Me.labelParamValue5.TabIndex = 23
        Me.labelParamValue5.Text = "Value"
        Me.labelParamValue5.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamValue6
        '
        Me.labelParamValue6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelParamValue6.Location = New System.Drawing.Point(136, 272)
        Me.labelParamValue6.Name = "labelParamValue6"
        Me.labelParamValue6.Size = New System.Drawing.Size(56, 16)
        Me.labelParamValue6.TabIndex = 28
        Me.labelParamValue6.Text = "Value"
        Me.labelParamValue6.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelParamName1
        '
        Me.labelParamName1.Location = New System.Drawing.Point(8, 35)
        Me.labelParamName1.Name = "labelParamName1"
        Me.labelParamName1.Size = New System.Drawing.Size(117, 13)
        Me.labelParamName1.TabIndex = 2
        Me.labelParamName1.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'labelParamName2
        '
        Me.labelParamName2.Location = New System.Drawing.Point(8, 83)
        Me.labelParamName2.Name = "labelParamName2"
        Me.labelParamName2.Size = New System.Drawing.Size(117, 13)
        Me.labelParamName2.TabIndex = 7
        Me.labelParamName2.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'labelParamName3
        '
        Me.labelParamName3.Location = New System.Drawing.Point(8, 131)
        Me.labelParamName3.Name = "labelParamName3"
        Me.labelParamName3.Size = New System.Drawing.Size(117, 13)
        Me.labelParamName3.TabIndex = 12
        Me.labelParamName3.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'labelParamName4
        '
        Me.labelParamName4.Location = New System.Drawing.Point(8, 179)
        Me.labelParamName4.Name = "labelParamName4"
        Me.labelParamName4.Size = New System.Drawing.Size(117, 13)
        Me.labelParamName4.TabIndex = 17
        Me.labelParamName4.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'labelParamName5
        '
        Me.labelParamName5.Location = New System.Drawing.Point(8, 227)
        Me.labelParamName5.Name = "labelParamName5"
        Me.labelParamName5.Size = New System.Drawing.Size(117, 13)
        Me.labelParamName5.TabIndex = 22
        Me.labelParamName5.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'labelParamName6
        '
        Me.labelParamName6.Location = New System.Drawing.Point(8, 275)
        Me.labelParamName6.Name = "labelParamName6"
        Me.labelParamName6.Size = New System.Drawing.Size(117, 13)
        Me.labelParamName6.TabIndex = 27
        Me.labelParamName6.TextAlign = System.Drawing.ContentAlignment.TopRight
        '
        'radiobuttonTriangle
        '
        Me.radiobuttonTriangle.Location = New System.Drawing.Point(16, 16)
        Me.radiobuttonTriangle.Name = "radiobuttonTriangle"
        Me.radiobuttonTriangle.Size = New System.Drawing.Size(69, 16)
        Me.radiobuttonTriangle.TabIndex = 32
        Me.radiobuttonTriangle.Text = "Triangle"
        '
        'radiobuttonSquare
        '
        Me.radiobuttonSquare.Location = New System.Drawing.Point(88, 16)
        Me.radiobuttonSquare.Name = "radiobuttonSquare"
        Me.radiobuttonSquare.Size = New System.Drawing.Size(64, 16)
        Me.radiobuttonSquare.TabIndex = 33
        Me.radiobuttonSquare.Text = "Square"
        '
        'radiobuttonRadioSine
        '
        Me.radiobuttonRadioSine.Location = New System.Drawing.Point(152, 16)
        Me.radiobuttonRadioSine.Name = "radiobuttonRadioSine"
        Me.radiobuttonRadioSine.Size = New System.Drawing.Size(48, 16)
        Me.radiobuttonRadioSine.TabIndex = 34
        Me.radiobuttonRadioSine.Text = "Sine"
        '
        'groupboxFrameWaveform
        '
        Me.groupboxFrameWaveform.Controls.AddRange(New System.Windows.Forms.Control() {Me.radiobuttonSquare, Me.radiobuttonTriangle, Me.radiobuttonRadioSine})
        Me.groupboxFrameWaveform.Location = New System.Drawing.Point(180, 400)
        Me.groupboxFrameWaveform.Name = "groupboxFrameWaveform"
        Me.groupboxFrameWaveform.Size = New System.Drawing.Size(225, 42)
        Me.groupboxFrameWaveform.TabIndex = 35
        Me.groupboxFrameWaveform.TabStop = False
        Me.groupboxFrameWaveform.Text = "Waveform"
        '
        'buttonButtonOpen
        '
        Me.buttonButtonOpen.Location = New System.Drawing.Point(12, 12)
        Me.buttonButtonOpen.Name = "buttonButtonOpen"
        Me.buttonButtonOpen.TabIndex = 47
        Me.buttonButtonOpen.Text = "&Open File"
        '
        'labelTextFilename
        '
        Me.labelTextFilename.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelTextFilename.Location = New System.Drawing.Point(94, 14)
        Me.labelTextFilename.Name = "labelTextFilename"
        Me.labelTextFilename.Size = New System.Drawing.Size(595, 20)
        Me.labelTextFilename.TabIndex = 48
        Me.labelTextFilename.Text = "Filename"
        Me.labelTextFilename.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'labelStatic2
        '
        Me.labelStatic2.Location = New System.Drawing.Point(19, 44)
        Me.labelStatic2.Name = "labelStatic2"
        Me.labelStatic2.Size = New System.Drawing.Size(67, 16)
        Me.labelStatic2.TabIndex = 49
        Me.labelStatic2.Text = "Status"
        '
        'labelTextStatus
        '
        Me.labelTextStatus.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelTextStatus.Location = New System.Drawing.Point(94, 44)
        Me.labelTextStatus.Name = "labelTextStatus"
        Me.labelTextStatus.Size = New System.Drawing.Size(595, 20)
        Me.labelTextStatus.TabIndex = 50
        Me.labelTextStatus.Text = "No file loaded."
        Me.labelTextStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'checkboxCheckLoop
        '
        Me.checkboxCheckLoop.Location = New System.Drawing.Point(42, 376)
        Me.checkboxCheckLoop.Name = "checkboxCheckLoop"
        Me.checkboxCheckLoop.Size = New System.Drawing.Size(86, 16)
        Me.checkboxCheckLoop.TabIndex = 51
        Me.checkboxCheckLoop.Text = "&Loop Sound"
        '
        'buttonPlay
        '
        Me.buttonPlay.Location = New System.Drawing.Point(10, 400)
        Me.buttonPlay.Name = "buttonPlay"
        Me.buttonPlay.Size = New System.Drawing.Size(67, 23)
        Me.buttonPlay.TabIndex = 52
        Me.buttonPlay.Text = "&Play"
        '
        'buttonStop
        '
        Me.buttonStop.Location = New System.Drawing.Point(87, 400)
        Me.buttonStop.Name = "buttonStop"
        Me.buttonStop.Size = New System.Drawing.Size(67, 23)
        Me.buttonStop.TabIndex = 53
        Me.buttonStop.Text = "&Stop"
        '
        'labelStatic3
        '
        Me.labelStatic3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelStatic3.Location = New System.Drawing.Point(372, 88)
        Me.labelStatic3.Name = "labelStatic3"
        Me.labelStatic3.Size = New System.Drawing.Size(52, 16)
        Me.labelStatic3.TabIndex = 62
        Me.labelStatic3.Text = "Min"
        Me.labelStatic3.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'labelStatic4
        '
        Me.labelStatic4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.labelStatic4.Location = New System.Drawing.Point(627, 88)
        Me.labelStatic4.Name = "labelStatic4"
        Me.labelStatic4.Size = New System.Drawing.Size(52, 16)
        Me.labelStatic4.TabIndex = 64
        Me.labelStatic4.Text = "Max"
        Me.labelStatic4.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'radiobuttonRadioNeg180
        '
        Me.radiobuttonRadioNeg180.Location = New System.Drawing.Point(16, 16)
        Me.radiobuttonRadioNeg180.Name = "radiobuttonRadioNeg180"
        Me.radiobuttonRadioNeg180.Size = New System.Drawing.Size(45, 16)
        Me.radiobuttonRadioNeg180.TabIndex = 65
        Me.radiobuttonRadioNeg180.Text = "-180"
        '
        'radiobuttonRadioNeg90
        '
        Me.radiobuttonRadioNeg90.Location = New System.Drawing.Point(72, 16)
        Me.radiobuttonRadioNeg90.Name = "radiobuttonRadioNeg90"
        Me.radiobuttonRadioNeg90.Size = New System.Drawing.Size(39, 16)
        Me.radiobuttonRadioNeg90.TabIndex = 66
        Me.radiobuttonRadioNeg90.Text = "-90"
        '
        'radiobuttonRadioZero
        '
        Me.radiobuttonRadioZero.Location = New System.Drawing.Point(120, 16)
        Me.radiobuttonRadioZero.Name = "radiobuttonRadioZero"
        Me.radiobuttonRadioZero.Size = New System.Drawing.Size(30, 16)
        Me.radiobuttonRadioZero.TabIndex = 67
        Me.radiobuttonRadioZero.Text = "0"
        '
        'radiobuttonRadio90
        '
        Me.radiobuttonRadio90.Location = New System.Drawing.Point(152, 16)
        Me.radiobuttonRadio90.Name = "radiobuttonRadio90"
        Me.radiobuttonRadio90.Size = New System.Drawing.Size(36, 16)
        Me.radiobuttonRadio90.TabIndex = 68
        Me.radiobuttonRadio90.Text = "90"
        '
        'radiobuttonRadio180
        '
        Me.radiobuttonRadio180.Location = New System.Drawing.Point(200, 16)
        Me.radiobuttonRadio180.Name = "radiobuttonRadio180"
        Me.radiobuttonRadio180.Size = New System.Drawing.Size(42, 16)
        Me.radiobuttonRadio180.TabIndex = 69
        Me.radiobuttonRadio180.Text = "180"
        '
        'groupboxFramePhase
        '
        Me.groupboxFramePhase.Controls.AddRange(New System.Windows.Forms.Control() {Me.radiobuttonRadioNeg180, Me.radiobuttonRadioNeg90, Me.radiobuttonRadioZero, Me.radiobuttonRadio90, Me.radiobuttonRadio180})
        Me.groupboxFramePhase.Location = New System.Drawing.Point(420, 400)
        Me.groupboxFramePhase.Name = "groupboxFramePhase"
        Me.groupboxFramePhase.Size = New System.Drawing.Size(247, 42)
        Me.groupboxFramePhase.TabIndex = 63
        Me.groupboxFramePhase.TabStop = False
        Me.groupboxFramePhase.Text = "Phase (Degrees)"
        '
        'groupboxEffects
        '
        Me.groupboxEffects.Controls.AddRange(New System.Windows.Forms.Control() {Me.buttonDelete, Me.listboxEffects, Me.comboEffects})
        Me.groupboxEffects.Location = New System.Drawing.Point(8, 80)
        Me.groupboxEffects.Name = "groupboxEffects"
        Me.groupboxEffects.Size = New System.Drawing.Size(144, 280)
        Me.groupboxEffects.TabIndex = 71
        Me.groupboxEffects.TabStop = False
        Me.groupboxEffects.Text = "Effects"
        '
        'buttonDelete
        '
        Me.buttonDelete.Location = New System.Drawing.Point(40, 248)
        Me.buttonDelete.Name = "buttonDelete"
        Me.buttonDelete.Size = New System.Drawing.Size(64, 24)
        Me.buttonDelete.TabIndex = 3
        Me.buttonDelete.Text = "Delete"
        '
        'listboxEffects
        '
        Me.listboxEffects.Location = New System.Drawing.Point(8, 48)
        Me.listboxEffects.Name = "listboxEffects"
        Me.listboxEffects.Size = New System.Drawing.Size(128, 186)
        Me.listboxEffects.TabIndex = 2
        '
        'comboEffects
        '
        Me.comboEffects.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.comboEffects.Items.AddRange(New Object() {"Chorus", "Compressor", "Distortion", "Echo", "Flanger", "Gargle", "Waves Reverb", "ParamEq"})
        Me.comboEffects.Location = New System.Drawing.Point(8, 16)
        Me.comboEffects.Name = "comboEffects"
        Me.comboEffects.Size = New System.Drawing.Size(128, 21)
        Me.comboEffects.TabIndex = 1
        '
        'Timer1
        '
        '
        'MainForm
        '
        Me.AcceptButton = Me.buttonOk
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(700, 472)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.groupboxEffects, Me.buttonOk, Me.buttonButtonOpen, Me.labelTextFilename, Me.labelStatic2, Me.labelTextStatus, Me.checkboxCheckLoop, Me.buttonPlay, Me.buttonStop, Me.labelStatic3, Me.labelStatic4, Me.groupboxFrameWaveform, Me.groupboxFramePhase, Me.groupboxFrame})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Location = New System.Drawing.Point(150, 160)
        Me.MaximizeBox = False
        Me.Name = "MainForm"
        Me.Text = "SoundFX - Sound effects applied to Device.SecondaryBuffer"
        Me.groupboxFrame.ResumeLayout(False)
        CType(Me.trackbarSlider1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarSlider2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarSlider3, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarSlider4, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarSlider5, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trackbarSlider6, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupboxFrameWaveform.ResumeLayout(False)
        Me.groupboxFramePhase.ResumeLayout(False)
        Me.groupboxEffects.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub 'InitializeComponent

    Sub InitDirectSound()

        Dim description As New BufferDescription()
        Dim wfx As New WaveFormat()

        applicationDevice = New Device()
        applicationDevice.SetCooperativeLevel(Me, CooperativeLevel.Normal)
    End Sub 'InitDirectSound

    Private Sub buttonButtonOpen_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonButtonOpen.Click

        Dim description As New BufferDescription()
        Dim ofd As New OpenFileDialog()

        labelTextStatus.Text = "Loading file..."
        ' Get the default media path (something like C:\WINDOWS\MEDIA)
        If String.Empty = path Then
            path = Environment.SystemDirectory.Substring(0, Environment.SystemDirectory.LastIndexOf("\")) + "\media"
        End If
        ofd.DefaultExt = ".wav"
        ofd.Filter = "Wave Files|*.wav|All Files|*.*"
        ofd.FileName = fileName
        ofd.InitialDirectory = path

        If Not Nothing Is applicationBuffer Then
            applicationBuffer.Stop()
            applicationBuffer.SetCurrentPosition(0)
        End If

        ' Display the OpenFileName dialog. Then, try to load the specified file
        If DialogResult.Cancel = ofd.ShowDialog(Me) Then
            If Not Nothing Is applicationBuffer Then
                applicationBuffer.Play(0, IIf(shouldLoop = True, BufferPlayFlags.Looping, BufferPlayFlags.Default))
            End If
            labelTextStatus.Text = "No file loaded."
            Return
        End If
        fileName = String.Empty

        description.ControlEffects = True
        Try
            applicationBuffer = New SecondaryBuffer(ofd.FileName, description, applicationDevice)
        Catch ex As BufferTooSmallException
            labelTextStatus.Text = "Wave file is too small to be used with effects."
            Return
        Catch ex2 As FormatException
            labelTextStatus.Text = "Unable to load wave file."
            Return
        End Try

        ' Remember the file for next time
        If Not Nothing Is applicationBuffer Then
            fileName = ofd.FileName
            path = fileName.Substring(0, fileName.LastIndexOf("\"))
        End If
        labelTextFilename.Text = fileName
        labelTextStatus.Text = "File loaded."
    End Sub 'buttonButtonOpen_Click

    Private Sub buttonPlay_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonPlay.Click
        Dim bpf As New BufferPlayFlags()

        If Not Nothing Is applicationBuffer Then
            applicationBuffer.Play(0, IIf(shouldLoop = True, BufferPlayFlags.Looping, BufferPlayFlags.Default))
            Timer1.Enabled = True
            labelTextStatus.Text = "Sound playing."
        End If
    End Sub 'buttonPlay_Click

    Private Sub buttonStop_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonStop.Click
        If Not Nothing Is applicationBuffer Then
            If applicationBuffer.Status.Playing = True Then
                applicationBuffer.Stop()
                applicationBuffer.SetCurrentPosition(0)
                Timer1.Enabled = False
                labelTextStatus.Text = "Sound stopped."
            End If
        End If
    End Sub 'buttonStop_Click

    Private Sub comboEffects_SelectedValueChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles comboEffects.SelectedValueChanged
        Dim description As String = String.Empty

        If Nothing Is applicationBuffer Then
            Return
        End If
        Dim temp(effectDescription.Count) As EffectInfo
        effectDescription.CopyTo(temp, 0)

        Select Case comboEffects.SelectedIndex
            Case 0
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardChorusGuid
                description = "Chorus"
            Case 1
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardCompressorGuid
                description = "Compressor"
            Case 2
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardDistortionGuid
                description = "Distortion"
            Case 3
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardEchoGuid
                description = "Echo"
            Case 4
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardFlangerGuid
                description = "Flanger"
            Case 5
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardGargleGuid
                description = "Gargle"
            Case 6
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardWavesReverbGuid
                description = "Waves Reverb"
            Case 7
                temp(temp.Length - 1).description.GuidEffectClass = DSoundHelper.StandardParamEqGuid
                description = "ParamEq"
        End Select

        If AddEffect(temp) Then
            effectDescription.Clear()
            effectDescription.AddRange(temp)
            listboxEffects.Items.Add(description)
            listboxEffects.SelectedIndex = listboxEffects.Items.Count - 1
        End If
    End Sub 'checkboxoEffects_SelectedValueChanged

    Private Function AddEffect(ByVal temp() As EffectInfo) As Boolean
        Dim ret As EffectsReturnValue() = Nothing
        Dim fx As EffectDescription() = Nothing
        Dim WasPlaying As Boolean = False
        Dim count As Integer = 0
        Dim i As Integer

        If Not Nothing Is temp Then
            count = temp.Length - 1
            fx = New EffectDescription(count) {}
        End If

        If True = applicationBuffer.Status.Playing Then
            WasPlaying = True
        End If
        applicationBuffer.Stop()

        If (Not Nothing Is temp) Then
            ' Store the current params for each effect.
            For i = 0 To count
                fx(i) = temp(i).description
            Next i
        End If

        Try
            ret = applicationBuffer.SetEffects(fx)
        Catch
            labelTextStatus.Text = "Unable to set effect on the buffer. Some effects can't be set on 8 bit wave files."

            ' Revert to the last valid effects.
            If (temp.Length > 0) Then
                fx = New EffectDescription(temp.Length - 1) {}
                For i = 0 To count - 1
                    fx(i) = CType(temp(i), EffectInfo).description
                Next i
                Try
                    applicationBuffer.SetEffects(fx)
                Catch
                End Try
                Return False
            End If
        End Try

        If Not Nothing Is temp Then
            ' Restore the params for each effect.
            For i = 0 To count
                Dim eff As New EffectInfo()

                eff.Effect = applicationBuffer.GetEffects(i)
                eff.EffectSettings = temp(i).EffectSettings
                eff.description = temp(i).description

                Dim efftype As Type = eff.Effect.GetType()

                If GetType(ChorusEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, ChorusEffect).AllParameters = CType(eff.EffectSettings, EffectsChorus)
                    Else
                        eff.EffectSettings = CType(eff.Effect, ChorusEffect).AllParameters
                    End If
                ElseIf GetType(CompressorEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, CompressorEffect).AllParameters = CType(eff.EffectSettings, EffectsCompressor)
                    Else
                        eff.EffectSettings = CType(eff.Effect, CompressorEffect).AllParameters
                    End If
                ElseIf GetType(DistortionEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, DistortionEffect).AllParameters = CType(eff.EffectSettings, EffectsDistortion)
                    Else
                        eff.EffectSettings = CType(eff.Effect, DistortionEffect).AllParameters
                    End If
                ElseIf GetType(EchoEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, EchoEffect).AllParameters = CType(eff.EffectSettings, EffectsEcho)
                    Else
                        eff.EffectSettings = CType(eff.Effect, EchoEffect).AllParameters
                    End If
                ElseIf GetType(FlangerEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, FlangerEffect).AllParameters = CType(eff.EffectSettings, EffectsFlanger)
                    Else
                        eff.EffectSettings = CType(eff.Effect, FlangerEffect).AllParameters
                    End If
                ElseIf GetType(GargleEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, GargleEffect).AllParameters = CType(eff.EffectSettings, EffectsGargle)
                    Else
                        eff.EffectSettings = CType(eff.Effect, GargleEffect).AllParameters
                    End If
                ElseIf GetType(ParamEqEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, ParamEqEffect).AllParameters = CType(eff.EffectSettings, EffectsParamEq)
                    Else
                        eff.EffectSettings = CType(eff.Effect, ParamEqEffect).AllParameters
                    End If
                ElseIf GetType(WavesReverbEffect) Is efftype Then
                    If Not Nothing Is eff.EffectSettings Then
                        CType(eff.Effect, WavesReverbEffect).AllParameters = CType(eff.EffectSettings, EffectsWavesReverb)
                    Else
                        eff.EffectSettings = CType(eff.Effect, WavesReverbEffect).AllParameters
                    End If
                End If
                temp(i) = eff
            Next i
        Else
            ClearUI(False)
        End If

        If WasPlaying Then
            applicationBuffer.Play(0, IIf(shouldLoop = True, BufferPlayFlags.Looping, BufferPlayFlags.Default))
        End If

        If Not Nothing Is temp Then
            If EffectsReturnValue.LocatedInHardware = ret(count) Or EffectsReturnValue.LocatedInSoftware = ret(count) Then
                Return True
            End If
        End If

        Return False
    End Function 'AddEffect

    Private Sub checkboxCheckLoop_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles checkboxCheckLoop.CheckedChanged
        shouldLoop = checkboxCheckLoop.Checked
    End Sub 'checkboxCheckLoop_CheckedChanged

    Private Sub listrackbaroxEffects_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles listboxEffects.SelectedIndexChanged
        If -1 = listboxEffects.SelectedIndex Then
            Return
        End If
        currentIndex = listboxEffects.SelectedIndex

        UpdateUI(True)
    End Sub 'listrackbaroxEffects_SelectedIndexChanged

    Private Sub UpdateUI(ByVal MoveControls As Boolean)

        ClearUI(MoveControls)

        Dim eff As EffectInfo = CType(effectDescription(currentIndex), EffectInfo)

        If TypeOf eff.Effect Is ChorusEffect Then
            Dim temp As EffectsChorus = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = ChorusEffect.WetDryMixMin
                trackbarSlider1.Maximum = ChorusEffect.WetDryMixMax
                trackbarSlider1.Value = temp.WetDryMix
                trackbarSlider2.Minimum = ChorusEffect.DepthMin
                trackbarSlider2.Maximum = ChorusEffect.DepthMax
                trackbarSlider2.Value = temp.Depth
                trackbarSlider3.Minimum = ChorusEffect.FeedbackMin
                trackbarSlider3.Maximum = ChorusEffect.FeedbackMax
                trackbarSlider3.Value = temp.Feedback
                trackbarSlider4.Minimum = ChorusEffect.FrequencyMin
                trackbarSlider4.Maximum = ChorusEffect.FrequencyMax
                trackbarSlider4.Value = temp.Frequency
                trackbarSlider5.Minimum = ChorusEffect.DelayMin
                trackbarSlider5.Maximum = ChorusEffect.DelayMax
                trackbarSlider5.Value = temp.Delay

                If ChorusEffect.WaveSin = temp.Waveform Then
                    radiobuttonRadioSine.Checked = True
                Else
                    radiobuttonTriangle.Checked = True
                End If
                If ChorusEffect.PhaseNegative180 = temp.Phase Then
                    radiobuttonRadioNeg180.Checked = True
                ElseIf ChorusEffect.PhaseNegative90 = temp.Phase Then
                    radiobuttonRadioNeg90.Checked = True
                ElseIf ChorusEffect.PhaseZero = temp.Phase Then
                    radiobuttonRadioZero.Checked = True
                ElseIf ChorusEffect.Phase90 = temp.Phase Then
                    radiobuttonRadio90.Checked = True
                ElseIf ChorusEffect.Phase180 = temp.Phase Then
                    radiobuttonRadio180.Checked = True
                End If

                groupboxFramePhase.Enabled = True
                radiobuttonRadioNeg180.Enabled = True
                radiobuttonRadioNeg90.Enabled = True
                radiobuttonRadioZero.Enabled = True
                radiobuttonRadio90.Enabled = True
                radiobuttonRadio180.Enabled = True
                groupboxFrameWaveform.Enabled = True
                radiobuttonRadioSine.Enabled = True
                radiobuttonTriangle.Enabled = True

                trackbarSlider1.Enabled = True
                trackbarSlider2.Enabled = True
                trackbarSlider3.Enabled = True
                trackbarSlider4.Enabled = True
                trackbarSlider5.Enabled = True

            End If

            labelParamMin1.Text = ChorusEffect.WetDryMixMin.ToString()
            labelParamMax1.Text = ChorusEffect.WetDryMixMax.ToString()
            labelParamMax2.Text = ChorusEffect.DepthMin.ToString()
            labelParamMin2.Text = ChorusEffect.DepthMax.ToString()
            labelParamMax3.Text = ChorusEffect.FeedbackMin.ToString()
            labelParamMin3.Text = ChorusEffect.FeedbackMax.ToString()
            labelParamMax4.Text = ChorusEffect.FrequencyMin.ToString()
            labelParamMin4.Text = ChorusEffect.FrequencyMax.ToString()
            labelParamMin5.Text = ChorusEffect.DelayMin.ToString()
            labelParamMax5.Text = ChorusEffect.DelayMax.ToString()

            labelParamValue1.Text = temp.WetDryMix.ToString()
            labelParamName1.Text = "Wet/Dry Mix (%)"

            labelParamValue2.Text = temp.Depth.ToString()
            labelParamName2.Text = "Depth (%)"

            labelParamValue3.Text = temp.Feedback.ToString()
            labelParamName3.Text = "Feedback (%)"

            labelParamValue4.Text = temp.Frequency.ToString()
            labelParamName4.Text = "Frequency (Hz)"

            labelParamValue5.Text = temp.Delay.ToString()
            labelParamName5.Text = "Delay (ms)"
        ElseIf TypeOf eff.Effect Is CompressorEffect Then
            Dim temp As EffectsCompressor = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = CompressorEffect.GainMin
                trackbarSlider1.Maximum = CompressorEffect.GainMax
                trackbarSlider1.Value = temp.Gain
                trackbarSlider2.Minimum = CompressorEffect.AttackMin
                trackbarSlider2.Maximum = CompressorEffect.AttackMax
                trackbarSlider2.Value = temp.Attack
                trackbarSlider3.Minimum = CompressorEffect.ReleaseMin
                trackbarSlider3.Maximum = CompressorEffect.ReleaseMax
                trackbarSlider3.Value = temp.Release
                trackbarSlider4.Minimum = CompressorEffect.ThresholdMin
                trackbarSlider4.Maximum = CompressorEffect.ThresholdMax
                trackbarSlider4.Value = temp.Threshold
                trackbarSlider5.Minimum = CompressorEffect.RatioMin
                trackbarSlider5.Maximum = CompressorEffect.RatioMax
                trackbarSlider5.Value = temp.Ratio
                trackbarSlider6.Minimum = CompressorEffect.PreDelayMin
                trackbarSlider6.Maximum = CompressorEffect.PreDelayMax
                trackbarSlider6.Value = temp.Predelay

                trackbarSlider1.Enabled = True
                trackbarSlider2.Enabled = True
                trackbarSlider3.Enabled = True
                trackbarSlider4.Enabled = True
                trackbarSlider5.Enabled = True
                trackbarSlider6.Enabled = True
            End If

            labelParamMin1.Text = CompressorEffect.GainMin.ToString()
            labelParamMax1.Text = CompressorEffect.GainMax.ToString()
            labelParamMax2.Text = CompressorEffect.AttackMin.ToString()
            labelParamMin2.Text = CompressorEffect.AttackMax.ToString()
            labelParamMax3.Text = CompressorEffect.ReleaseMin.ToString()
            labelParamMin3.Text = CompressorEffect.ReleaseMax.ToString()
            labelParamMax4.Text = CompressorEffect.ThresholdMin.ToString()
            labelParamMin4.Text = CompressorEffect.ThresholdMax.ToString()
            labelParamMin5.Text = CompressorEffect.RatioMin.ToString()
            labelParamMax5.Text = CompressorEffect.RatioMax.ToString()
            labelParamMin6.Text = CompressorEffect.PreDelayMin.ToString()
            labelParamMax6.Text = CompressorEffect.PreDelayMax.ToString()

            labelParamValue1.Text = temp.Gain.ToString()
            labelParamName1.Text = "Gain (dB)"

            labelParamName2.Text = "Attack (ms)"
            labelParamValue2.Text = temp.Attack.ToString()

            labelParamName3.Text = "Release (ms)"
            labelParamValue3.Text = temp.Release.ToString()

            labelParamName4.Text = "Threshold (dB)"
            labelParamValue4.Text = temp.Threshold.ToString()

            labelParamName5.Text = "Ratio (x:1)"
            labelParamValue5.Text = temp.Ratio.ToString()

            labelParamName6.Text = "Predelay (ms)"
            labelParamValue6.Text = temp.Predelay.ToString()
        ElseIf TypeOf eff.Effect Is DistortionEffect Then
            Dim temp As EffectsDistortion = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = DistortionEffect.GainMin
                trackbarSlider1.Maximum = DistortionEffect.GainMax
                trackbarSlider1.Value = temp.Gain
                trackbarSlider2.Minimum = DistortionEffect.EdgeMin
                trackbarSlider2.Maximum = DistortionEffect.EdgeMax
                trackbarSlider2.Value = temp.Edge
                trackbarSlider3.Minimum = DistortionEffect.PostEqCenterFrequencyMin
                trackbarSlider3.Maximum = DistortionEffect.PostEqCenterFrequencyMax
                trackbarSlider3.Value = temp.PostEqCenterFrequency
                trackbarSlider4.Minimum = DistortionEffect.PostEqBandwidthMin
                trackbarSlider4.Maximum = DistortionEffect.PostEqBandwidthMax
                trackbarSlider4.Value = temp.PostEqBandwidth
                trackbarSlider5.Minimum = DistortionEffect.PreLowPassCutoffMin
                trackbarSlider5.Maximum = DistortionEffect.PreLowPassCutoffMax
                trackbarSlider5.Value = temp.PreLowpassCutoff

                trackbarSlider1.Enabled = True
                trackbarSlider2.Enabled = True
                trackbarSlider3.Enabled = True
                trackbarSlider4.Enabled = True
                trackbarSlider5.Enabled = True

            End If

            labelParamMin1.Text = DistortionEffect.GainMin.ToString()
            labelParamMax1.Text = DistortionEffect.GainMax.ToString()
            labelParamMax2.Text = DistortionEffect.EdgeMin.ToString()
            labelParamMin2.Text = DistortionEffect.EdgeMax.ToString()
            labelParamMax3.Text = DistortionEffect.PostEqCenterFrequencyMin.ToString()
            labelParamMin3.Text = DistortionEffect.PostEqCenterFrequencyMax.ToString()
            labelParamMax4.Text = DistortionEffect.PostEqBandwidthMin.ToString()
            labelParamMin4.Text = DistortionEffect.PostEqBandwidthMax.ToString()
            labelParamMin5.Text = DistortionEffect.PreLowPassCutoffMin.ToString()
            labelParamMax5.Text = DistortionEffect.PreLowPassCutoffMax.ToString()

            labelParamName1.Text = "Gain (dB)"
            labelParamValue1.Text = temp.Gain.ToString()

            labelParamName2.Text = "Edge (%)"
            labelParamValue2.Text = temp.Edge.ToString()

            labelParamName3.Text = "PostEQ Center Freq (Hz)"
            labelParamValue3.Text = temp.PostEqCenterFrequency.ToString()

            labelParamName4.Text = "PostEQ Bandwidth (Hz)"
            labelParamValue4.Text = temp.PostEqBandwidth.ToString()

            labelParamName5.Text = "PreLowpass Cutoff (Hz)"
            labelParamValue5.Text = temp.PreLowpassCutoff.ToString()
        ElseIf TypeOf eff.Effect Is EchoEffect Then
            Dim temp As EffectsEcho = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = EchoEffect.WetDryMixMin
                trackbarSlider1.Maximum = EchoEffect.WetDryMixMax
                trackbarSlider1.Value = temp.WetDryMix
                trackbarSlider2.Minimum = EchoEffect.FeedbackMin
                trackbarSlider2.Maximum = EchoEffect.FeedbackMax
                trackbarSlider2.Value = temp.Feedback
                trackbarSlider3.Minimum = EchoEffect.LeftDelayMin
                trackbarSlider3.Maximum = EchoEffect.LeftDelayMax
                trackbarSlider3.Value = temp.LeftDelay
                trackbarSlider4.Minimum = EchoEffect.RightDelayMin
                trackbarSlider4.Maximum = EchoEffect.RightDelayMax
                trackbarSlider4.Value = temp.RightDelay
                trackbarSlider5.Minimum = EchoEffect.PanDelayMin
                trackbarSlider5.Maximum = EchoEffect.PanDelayMax
                trackbarSlider5.Value = temp.PanDelay

                trackbarSlider1.Enabled = True
                trackbarSlider2.Enabled = True
                trackbarSlider3.Enabled = True
                trackbarSlider4.Enabled = True
                trackbarSlider5.Enabled = True
            End If

            labelParamMin1.Text = EchoEffect.WetDryMixMin.ToString()
            labelParamMax1.Text = EchoEffect.WetDryMixMax.ToString()
            labelParamMax2.Text = EchoEffect.FeedbackMin.ToString()
            labelParamMin2.Text = EchoEffect.FeedbackMax.ToString()
            labelParamMax3.Text = EchoEffect.LeftDelayMin.ToString()
            labelParamMin3.Text = EchoEffect.LeftDelayMax.ToString()
            labelParamMax4.Text = EchoEffect.RightDelayMin.ToString()
            labelParamMin4.Text = EchoEffect.RightDelayMax.ToString()
            labelParamMin5.Text = EchoEffect.PanDelayMin.ToString()
            labelParamMax5.Text = EchoEffect.PanDelayMax.ToString()

            labelParamName1.Text = "Wet/Dry Mix (%)"
            labelParamValue1.Text = temp.WetDryMix.ToString()

            labelParamName2.Text = "Feedback (%)"
            labelParamValue2.Text = temp.Feedback.ToString()

            labelParamName3.Text = "Left Delay (ms)"
            labelParamValue3.Text = temp.LeftDelay.ToString()

            labelParamName4.Text = "Right Delay (ms)"
            labelParamValue4.Text = temp.RightDelay.ToString()

            labelParamName5.Text = "Pan Delay (bool)"
            labelParamValue5.Text = temp.PanDelay.ToString()
        ElseIf TypeOf eff.Effect Is FlangerEffect Then
            Dim temp As EffectsFlanger = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = FlangerEffect.WetDryMixMin
                trackbarSlider1.Maximum = FlangerEffect.WetDryMixMax
                trackbarSlider1.Value = temp.WetDryMix
                trackbarSlider2.Minimum = FlangerEffect.DepthMin
                trackbarSlider2.Maximum = FlangerEffect.DepthMax
                trackbarSlider2.Value = temp.Depth
                trackbarSlider3.Minimum = FlangerEffect.FeedbackMin
                trackbarSlider3.Maximum = FlangerEffect.FeedbackMax
                trackbarSlider3.Value = temp.Feedback
                trackbarSlider4.Minimum = FlangerEffect.FrequencyMin
                trackbarSlider4.Maximum = FlangerEffect.FrequencyMax
                trackbarSlider4.Value = temp.Frequency
                trackbarSlider5.Minimum = FlangerEffect.DelayMin
                trackbarSlider5.Maximum = FlangerEffect.DelayMax
                trackbarSlider5.Value = temp.Delay

                trackbarSlider1.Enabled = True
                trackbarSlider2.Enabled = True
                trackbarSlider3.Enabled = True
                trackbarSlider4.Enabled = True
                trackbarSlider5.Enabled = True

                If ChorusEffect.WaveSin = temp.Waveform Then
                    radiobuttonRadioSine.Checked = True
                Else
                    radiobuttonTriangle.Checked = True
                End If
                If FlangerEffect.PhaseNeg180 = temp.Phase Then
                    radiobuttonRadioNeg180.Checked = True
                ElseIf FlangerEffect.PhaseNeg90 = temp.Phase Then
                    radiobuttonRadioNeg90.Checked = True
                ElseIf FlangerEffect.PhaseZero = temp.Phase Then
                    radiobuttonRadioZero.Checked = True
                ElseIf FlangerEffect.Phase90 = temp.Phase Then
                    radiobuttonRadio90.Checked = True
                ElseIf FlangerEffect.Phase180 = temp.Phase Then
                    radiobuttonRadio180.Checked = True
                End If

                groupboxFramePhase.Enabled = True
                radiobuttonRadioNeg180.Enabled = True
                radiobuttonRadioNeg90.Enabled = True
                radiobuttonRadioZero.Enabled = True
                radiobuttonRadio90.Enabled = True
                radiobuttonRadio180.Enabled = True
                groupboxFrameWaveform.Enabled = True
                radiobuttonRadioSine.Enabled = True
                radiobuttonTriangle.Enabled = True

            End If

            labelParamMin1.Text = FlangerEffect.WetDryMixMin.ToString()
            labelParamMax1.Text = FlangerEffect.WetDryMixMax.ToString()
            labelParamMax2.Text = FlangerEffect.DepthMin.ToString()
            labelParamMin2.Text = FlangerEffect.DepthMax.ToString()
            labelParamMax3.Text = FlangerEffect.FeedbackMin.ToString()
            labelParamMin3.Text = FlangerEffect.FeedbackMax.ToString()
            labelParamMax4.Text = FlangerEffect.FrequencyMin.ToString()
            labelParamMin4.Text = FlangerEffect.FrequencyMax.ToString()
            labelParamMin5.Text = FlangerEffect.DelayMin.ToString()
            labelParamMax5.Text = FlangerEffect.DelayMax.ToString()

            labelParamName1.Text = "Wet/Dry Mix (%)"
            labelParamValue1.Text = temp.WetDryMix.ToString()

            labelParamName2.Text = "Depth (%)"
            labelParamValue2.Text = temp.Depth.ToString()

            labelParamName3.Text = "Feedback (%)"
            labelParamValue3.Text = temp.Feedback.ToString()

            labelParamName4.Text = "Frequency (Hz)"
            labelParamValue4.Text = temp.Frequency.ToString()

            labelParamName5.Text = "Delay (ms)"
            labelParamValue5.Text = temp.Delay.ToString()
        ElseIf TypeOf eff.Effect Is GargleEffect Then
            Dim temp As EffectsGargle = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = GargleEffect.RateHzMin
                trackbarSlider1.Maximum = GargleEffect.RateHzMax
                trackbarSlider1.Value = temp.RateHz

                If GargleEffect.WaveSquare = temp.WaveShape Then
                    radiobuttonSquare.Checked = True
                Else
                    radiobuttonTriangle.Checked = True
                End If
                groupboxFrameWaveform.Enabled = True
                radiobuttonSquare.Enabled = True
                radiobuttonTriangle.Enabled = True
                trackbarSlider1.Enabled = True
            End If

            labelParamMin1.Text = GargleEffect.RateHzMin.ToString()
            labelParamMax1.Text = GargleEffect.RateHzMax.ToString()

            labelParamName1.Text = "Rate (Hz)"
            labelParamValue1.Text = temp.RateHz.ToString()

        ElseIf TypeOf eff.Effect Is ParamEqEffect Then
            Dim temp As EffectsParamEq = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = ParamEqEffect.CenterMin
                trackbarSlider1.Maximum = ParamEqEffect.CenterMax
                trackbarSlider1.Value = temp.Center
                trackbarSlider2.Minimum = ParamEqEffect.BandwidthMin
                trackbarSlider2.Maximum = ParamEqEffect.BandwidthMax
                trackbarSlider2.Value = temp.Bandwidth
                trackbarSlider3.Minimum = ParamEqEffect.GainMin
                trackbarSlider3.Maximum = ParamEqEffect.GainMax
                trackbarSlider3.Value = temp.Gain

                trackbarSlider1.Enabled = True
                trackbarSlider2.Enabled = True
                trackbarSlider3.Enabled = True
            End If

            labelParamMin1.Text = ParamEqEffect.CenterMin.ToString()
            labelParamMax1.Text = ParamEqEffect.CenterMax.ToString()
            labelParamMax2.Text = ParamEqEffect.BandwidthMin.ToString()
            labelParamMin2.Text = ParamEqEffect.BandwidthMax.ToString()
            labelParamMax3.Text = ParamEqEffect.GainMin.ToString()
            labelParamMin3.Text = ParamEqEffect.GainMax.ToString()

            labelParamName1.Text = "Center Freq (Hz)"
            labelParamValue1.Text = temp.Center.ToString()

            labelParamName2.Text = "Bandwidth (Hz)"
            labelParamValue2.Text = temp.Bandwidth.ToString()

            labelParamName3.Text = "Gain (dB)"
            labelParamValue3.Text = temp.Gain.ToString()
        ElseIf TypeOf eff.Effect Is WavesReverbEffect Then
            Dim temp As EffectsWavesReverb = eff.Effect.AllParameters

            If MoveControls Then
                trackbarSlider1.Minimum = WavesReverbEffect.InGainMin
                trackbarSlider1.Maximum = WavesReverbEffect.InGainMax
                trackbarSlider1.Value = temp.InGain
                trackbarSlider2.Minimum = WavesReverbEffect.ReverbMixMin
                trackbarSlider2.Maximum = WavesReverbEffect.ReverbMixMax
                trackbarSlider2.Value = temp.ReverbMix
                trackbarSlider3.Minimum = 1000 * WavesReverbEffect.ReverbTimeMin
                trackbarSlider3.Maximum = 1000 * WavesReverbEffect.ReverbTimeMax
                trackbarSlider3.Value = 1000 * temp.ReverbTime
                trackbarSlider4.Minimum = 1000 * WavesReverbEffect.HighFrequencyRtRatioMin
                trackbarSlider4.Maximum = 1000 * WavesReverbEffect.HighFrequencyRtRatioMax
                trackbarSlider4.Value = 1000 * temp.HighFrequencyRtRatio

                trackbarSlider1.Enabled = True
                trackbarSlider2.Enabled = True
                trackbarSlider3.Enabled = True
                trackbarSlider4.Enabled = True
            End If

            labelParamMin1.Text = WavesReverbEffect.InGainMin.ToString()
            labelParamMax1.Text = WavesReverbEffect.InGainMax.ToString()
            labelParamMax2.Text = WavesReverbEffect.ReverbMixMin.ToString()
            labelParamMin2.Text = WavesReverbEffect.ReverbMixMax.ToString()
            labelParamMax3.Text = WavesReverbEffect.ReverbTimeMin.ToString()
            labelParamMin3.Text = WavesReverbEffect.ReverbTimeMax.ToString()
            labelParamMax4.Text = WavesReverbEffect.HighFrequencyRtRatioMin.ToString()
            labelParamMin4.Text = WavesReverbEffect.HighFrequencyRtRatioMax.ToString()

            labelParamName1.Text = "In Gain (dB)"
            labelParamValue1.Text = temp.InGain.ToString()

            labelParamName2.Text = "Waves Reverb Mix (dB)"
            labelParamValue2.Text = temp.ReverbMix.ToString()

            labelParamName3.Text = "Waves Reverb Time (ms)"
            labelParamValue3.Text = temp.ReverbTime.ToString()

            labelParamName4.Text = "HighFreq RT Ratio (x:1)"
            labelParamValue4.Text = temp.HighFrequencyRtRatio.ToString()
        End If
    End Sub 'UpdateUI

    Private Sub ClearUI(ByVal ClearControls As Boolean)

        Dim c As Control
        Dim l As Label
        Dim t As TrackBar

        If ClearControls Then
            groupboxFrameWaveform.Enabled = False
            radiobuttonTriangle.Enabled = False
            radiobuttonTriangle.Enabled = False
            radiobuttonRadioSine.Enabled = False
            groupboxFramePhase.Enabled = False
            radiobuttonRadioNeg180.Enabled = False
            radiobuttonRadioNeg90.Enabled = False
            radiobuttonRadioZero.Enabled = False
            radiobuttonRadio90.Enabled = False
            radiobuttonRadio180.Enabled = False
            For Each c In groupboxFrame.Controls
                If TypeOf c Is TrackBar Then
                    t = c
                    t.Minimum = 0
                    t.Value = 0
                    t.Enabled = False
                End If
            Next
        End If

        For Each c In groupboxFrame.Controls
            If TypeOf c Is Label Then
                l = c
                If (l.Name.StartsWith("labelParam")) Then
                    l.Text = String.Empty
                End If
            End If
        Next

    End Sub 'ClearUI

    Private Sub trackbarSliderScroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles trackbarSlider1.Scroll, trackbarSlider2.Scroll, trackbarSlider3.Scroll, trackbarSlider4.Scroll, trackbarSlider5.Scroll, trackbarSlider6.Scroll, radiobuttonTriangle.CheckedChanged, radiobuttonSquare.CheckedChanged, radiobuttonRadioSine.CheckedChanged, radiobuttonRadioNeg180.CheckedChanged, radiobuttonRadioNeg90.CheckedChanged, radiobuttonRadioZero.CheckedChanged, radiobuttonRadio90.CheckedChanged, radiobuttonRadio180.CheckedChanged

        Dim eff As EffectInfo = effectDescription(currentIndex)
        Dim efftype As Type = eff.Effect.GetType()

        If GetType(ChorusEffect) Is efftype Then
            Dim temp As New EffectsChorus()
            temp.WetDryMix = trackbarSlider1.Value
            temp.Frequency = trackbarSlider4.Value
            temp.Feedback = trackbarSlider3.Value
            temp.Depth = trackbarSlider2.Value
            temp.Delay = trackbarSlider5.Value

            If True = radiobuttonRadioSine.Checked Then
                temp.Waveform = ChorusEffect.WaveSin
            Else
                temp.Waveform = ChorusEffect.WaveTriangle
            End If
            If True = radiobuttonRadioNeg180.Checked Then
                temp.Phase = ChorusEffect.PhaseNegative180
            ElseIf True = radiobuttonRadioNeg90.Checked Then
                temp.Phase = ChorusEffect.PhaseNegative90
            ElseIf True = radiobuttonRadioZero.Checked Then
                temp.Phase = ChorusEffect.PhaseZero
            ElseIf True = radiobuttonRadio90.Checked Then
                temp.Phase = ChorusEffect.Phase90
            ElseIf True = radiobuttonRadio180.Checked Then
                temp.Phase = ChorusEffect.Phase180
            End If
            eff.EffectSettings = temp
            CType(eff.Effect, ChorusEffect).AllParameters = temp
        ElseIf GetType(CompressorEffect) Is efftype Then
            Dim temp As New EffectsCompressor()
            temp.Gain = trackbarSlider1.Value
            temp.Attack = trackbarSlider2.Value
            temp.Release = trackbarSlider3.Value
            temp.Threshold = trackbarSlider4.Value
            temp.Ratio = trackbarSlider5.Value
            temp.Predelay = trackbarSlider6.Value

            eff.EffectSettings = temp
            CType(eff.Effect, CompressorEffect).AllParameters = temp
        ElseIf GetType(DistortionEffect) Is efftype Then
            Dim temp As New EffectsDistortion()
            temp.Gain = trackbarSlider1.Value
            temp.Edge = trackbarSlider2.Value
            temp.PostEqCenterFrequency = trackbarSlider3.Value
            temp.PostEqBandwidth = trackbarSlider4.Value
            temp.PreLowpassCutoff = trackbarSlider5.Value

            eff.EffectSettings = temp
            CType(eff.Effect, DistortionEffect).AllParameters = temp
        ElseIf GetType(EchoEffect) Is efftype Then
            Dim temp As New EffectsEcho()
            temp.WetDryMix = trackbarSlider1.Value
            temp.Feedback = trackbarSlider2.Value
            temp.LeftDelay = trackbarSlider3.Value
            temp.RightDelay = trackbarSlider4.Value
            temp.PanDelay = trackbarSlider5.Value

            eff.EffectSettings = temp
            CType(eff.Effect, EchoEffect).AllParameters = temp
        ElseIf GetType(FlangerEffect) Is efftype Then
            Dim temp As New EffectsFlanger()
            temp.WetDryMix = trackbarSlider1.Value
            temp.Depth = trackbarSlider2.Value
            temp.Feedback = trackbarSlider3.Value
            temp.Frequency = trackbarSlider4.Value
            temp.Delay = trackbarSlider5.Value

            If True = radiobuttonRadioSine.Checked Then
                temp.Waveform = FlangerEffect.WaveSin
            Else
                temp.Waveform = FlangerEffect.WaveTriangle
            End If
            If True = radiobuttonRadioNeg180.Checked Then
                temp.Phase = ChorusEffect.PhaseNegative180
            ElseIf True = radiobuttonRadioNeg90.Checked Then
                temp.Phase = ChorusEffect.PhaseNegative90
            ElseIf True = radiobuttonRadioZero.Checked Then
                temp.Phase = ChorusEffect.PhaseZero
            ElseIf True = radiobuttonRadio90.Checked Then
                temp.Phase = ChorusEffect.Phase90
            ElseIf True = radiobuttonRadio180.Checked Then
                temp.Phase = ChorusEffect.Phase180
            End If
            eff.EffectSettings = temp
            CType(eff.Effect, FlangerEffect).AllParameters = temp
        ElseIf GetType(GargleEffect) Is efftype Then
            Dim temp As New EffectsGargle()
            temp.RateHz = trackbarSlider1.Value
            If radiobuttonSquare.Checked Then
                temp.WaveShape = GargleEffect.WaveSquare
            Else
                temp.WaveShape = GargleEffect.WaveTriangle
            End If
            If True = radiobuttonSquare.Checked Then
                temp.WaveShape = GargleEffect.WaveSquare
            Else
                temp.WaveShape = GargleEffect.WaveTriangle
            End If
            eff.EffectSettings = temp
            CType(eff.Effect, GargleEffect).AllParameters = temp
        ElseIf GetType(ParamEqEffect) Is efftype Then
            Dim temp As New EffectsParamEq()
            temp.Center = trackbarSlider1.Value
            temp.Bandwidth = trackbarSlider2.Value
            temp.Gain = trackbarSlider3.Value

            eff.EffectSettings = temp
            CType(eff.Effect, ParamEqEffect).AllParameters = temp
        ElseIf GetType(WavesReverbEffect) Is efftype Then
            Dim temp As New EffectsWavesReverb()
            temp.InGain = trackbarSlider1.Value
            temp.ReverbMix = trackbarSlider2.Value
            temp.ReverbTime = 0.001 * trackbarSlider3.Value
            temp.HighFrequencyRtRatio = 0.001 * trackbarSlider4.Value

            eff.EffectSettings = temp
            CType(eff.Effect, WavesReverbEffect).AllParameters = temp
        End If
        effectDescription(currentIndex) = eff
        UpdateUI(False)
    End Sub 'trackbarSliderScroll

    Private Sub DeleteEffect()
        Dim temp As EffectInfo() = Nothing

        If (-1 = listboxEffects.SelectedIndex) Then Return

        effectDescription.RemoveAt(listboxEffects.SelectedIndex)

        If effectDescription.Count > 0 Then
            temp = New EffectInfo(effectDescription.Count - 1) {}
            effectDescription.CopyTo(temp, 0)
            AddEffect(temp)
            listboxEffects.Items.RemoveAt(listboxEffects.SelectedIndex)
            listboxEffects.SelectedIndex = 0
            currentIndex = 0
        Else
            temp = Nothing
            AddEffect(temp)
            listboxEffects.Items.Clear()
            ClearUI(True)
        End If

        effectDescription.Clear()
        If Not Nothing Is temp Then
            effectDescription.AddRange(temp)
        End If

    End Sub

    Private Sub listboxEffects_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles listboxEffects.KeyUp
        If e.KeyCode = Keys.Delete Then
            DeleteEffect()
        End If
    End Sub 'listboxEffects_KeyUp

    Private Sub buttonOk_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonOk.Click
        Me.Close()
    End Sub 'buttonOk_Click

    Private Sub buttonDelete_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonDelete.Click
        DeleteEffect()
    End Sub

    Private Sub Timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer1.Tick

        If applicationBuffer.Status.Playing Then
            Return
        Else
            Timer1.Enabled = False
            labelTextStatus.Text = "Sound stopped."
        End If

    End Sub

    Private Sub Form_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        If (Not Nothing Is applicationBuffer) Then
            If (applicationBuffer.Status.Playing) Then
                applicationBuffer.Stop()
            End If
        End If
    End Sub
End Class 'MainForm 