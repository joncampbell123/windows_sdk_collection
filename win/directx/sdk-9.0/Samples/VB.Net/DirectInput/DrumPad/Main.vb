Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports System.IO
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectInput
Imports Microsoft.DirectX.DirectSound
Imports SoundDevice = Microsoft.DirectX.DirectSound.Device

Public Class DlgMainForm
    Inherits System.Windows.Forms.Form

    Private WithEvents btnOk As System.Windows.Forms.Button '
    Private WithEvents gbStatic As GroupBox
    Private WithEvents btnButtonBass As System.Windows.Forms.Button
    Private WithEvents btnButtonSnare As System.Windows.Forms.Button
    Private WithEvents btnButtonHihatUp As System.Windows.Forms.Button
    Private WithEvents btnButtonHihatDown As System.Windows.Forms.Button
    Private WithEvents btnButtonCrash As System.Windows.Forms.Button
    Private WithEvents btnButtonUser1 As System.Windows.Forms.Button
    Private WithEvents btnButtonUser2 As System.Windows.Forms.Button
    Private WithEvents btnButtonUser3 As System.Windows.Forms.Button
    Private WithEvents timer1 As System.Windows.Forms.Timer
    Private lblTextFilename1 As Label
    Private lblTextFilename2 As Label
    Private lblTextFilename3 As Label
    Private lblTextFilename4 As Label
    Private lblTextFilename5 As Label
    Private lblTextFilename6 As Label
    Private lblTextFilename7 As Label
    Private lblTextFilename8 As Label
    Private WithEvents btnButtonDevice As System.Windows.Forms.Button

    '-----------------------------------------------------------------------------
    ' enumeration of hypothetical control functions
    '-----------------------------------------------------------------------------
    Enum GameActions
        BassDrum ' base drum
        SnareDrum ' snare drum
        HiHatOpen ' open hihat
        HiHatClose ' closed hihat
        Crash ' crash
        User1 ' user assigned one
        User2 ' user assigned two
        User3 ' user assigned three
        NumberActions ' auto count for number of enumerated actions
    End Enum 'GameActions

    ' Unique application guid for the action mapping.
    Private Shared guidApp As New Guid(&H21BEE2A7, &H32D7, &H43C8, &H92, &H41, &H1D, &H38, &HB, &H6, &HD0, &H5)
    Private pathSoundFile As String = DXUtil.SdkMediaPath
    Private applicationSoundDevice As SoundDevice = Nothing
    Private applicationSoundBuffers As SecondaryBuffer() = Nothing
    Private applicationDevices As DeviceList = Nothing
    Private mappings As ActionFormat() = Nothing
    Private actions() As Action
    Private buttonStates(CInt(GameActions.NumberActions)) As Boolean
    Private boxColors(CInt(GameActions.NumberActions)) As Single
    Private RectangleTop As Integer = 27
    Private RectangleLeft As Integer = 315
    Private applicationGraphics As Graphics = Nothing

#Region " Windows Form Designer generated code "

    Public Shared Function Main(ByVal Args() As String) As Integer
        Application.Run(New DlgMainForm())
        Return 0
    End Function 'Main

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        components = New System.ComponentModel.Container()
        Me.btnOk = New System.Windows.Forms.Button()
        Me.gbStatic = New System.Windows.Forms.GroupBox()
        Me.btnButtonBass = New System.Windows.Forms.Button()
        Me.btnButtonSnare = New System.Windows.Forms.Button()
        Me.btnButtonHihatUp = New System.Windows.Forms.Button()
        Me.btnButtonHihatDown = New System.Windows.Forms.Button()
        Me.btnButtonCrash = New System.Windows.Forms.Button()
        Me.btnButtonUser1 = New System.Windows.Forms.Button()
        Me.btnButtonUser2 = New System.Windows.Forms.Button()
        Me.btnButtonUser3 = New System.Windows.Forms.Button()
        Me.lblTextFilename1 = New System.Windows.Forms.Label()
        Me.lblTextFilename2 = New System.Windows.Forms.Label()
        Me.lblTextFilename3 = New System.Windows.Forms.Label()
        Me.lblTextFilename4 = New System.Windows.Forms.Label()
        Me.lblTextFilename5 = New System.Windows.Forms.Label()
        Me.lblTextFilename6 = New System.Windows.Forms.Label()
        Me.lblTextFilename7 = New System.Windows.Forms.Label()
        Me.lblTextFilename8 = New System.Windows.Forms.Label()
        Me.btnButtonDevice = New System.Windows.Forms.Button()
        Me.timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.SuspendLayout()
        '
        'btnOk
        '
        Me.btnOk.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnOk.Location = New System.Drawing.Point(288, 320)
        Me.btnOk.Name = "btnOk"
        Me.btnOk.TabIndex = 0
        Me.btnOk.Text = "OK"
        '
        'gbStatic
        '
        Me.gbStatic.Location = New System.Drawing.Point(7, 8)
        Me.gbStatic.Name = "gbStatic"
        Me.gbStatic.Size = New System.Drawing.Size(360, 304)
        Me.gbStatic.TabIndex = 1
        Me.gbStatic.TabStop = False
        Me.gbStatic.Text = "Samples"
        '
        'btnButtonBass
        '
        Me.btnButtonBass.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnButtonBass.Location = New System.Drawing.Point(22, 32)
        Me.btnButtonBass.Name = "btnButtonBass"
        Me.btnButtonBass.TabIndex = 2
        Me.btnButtonBass.Tag = "0"
        Me.btnButtonBass.Text = "Bass Drum"
        '
        'btnButtonSnare
        '
        Me.btnButtonSnare.Location = New System.Drawing.Point(22, 64)
        Me.btnButtonSnare.Name = "btnButtonSnare"
        Me.btnButtonSnare.TabIndex = 3
        Me.btnButtonSnare.Tag = "1"
        Me.btnButtonSnare.Text = "Snare"
        '
        'btnButtonHihatUp
        '
        Me.btnButtonHihatUp.Location = New System.Drawing.Point(22, 96)
        Me.btnButtonHihatUp.Name = "btnButtonHihatUp"
        Me.btnButtonHihatUp.TabIndex = 4
        Me.btnButtonHihatUp.Tag = "2"
        Me.btnButtonHihatUp.Text = "Hi-Hat Up"
        '
        'btnButtonHihatDown
        '
        Me.btnButtonHihatDown.Location = New System.Drawing.Point(22, 128)
        Me.btnButtonHihatDown.Name = "btnButtonHihatDown"
        Me.btnButtonHihatDown.TabIndex = 5
        Me.btnButtonHihatDown.Tag = "3"
        Me.btnButtonHihatDown.Text = "Hi-Hat"
        '
        'btnButtonCrash
        '
        Me.btnButtonCrash.Location = New System.Drawing.Point(22, 160)
        Me.btnButtonCrash.Name = "btnButtonCrash"
        Me.btnButtonCrash.TabIndex = 6
        Me.btnButtonCrash.Tag = "4"
        Me.btnButtonCrash.Text = "Crash"
        '
        'btnButtonUser1
        '
        Me.btnButtonUser1.Location = New System.Drawing.Point(22, 192)
        Me.btnButtonUser1.Name = "btnButtonUser1"
        Me.btnButtonUser1.TabIndex = 7
        Me.btnButtonUser1.Tag = "5"
        Me.btnButtonUser1.Text = "User 1"
        '
        'btnButtonUser2
        '
        Me.btnButtonUser2.Location = New System.Drawing.Point(22, 224)
        Me.btnButtonUser2.Name = "btnButtonUser2"
        Me.btnButtonUser2.TabIndex = 8
        Me.btnButtonUser2.Tag = "6"
        Me.btnButtonUser2.Text = "User 2"
        '
        'btnButtonUser3
        '
        Me.btnButtonUser3.Location = New System.Drawing.Point(22, 256)
        Me.btnButtonUser3.Name = "btnButtonUser3"
        Me.btnButtonUser3.TabIndex = 9
        Me.btnButtonUser3.Tag = "7"
        Me.btnButtonUser3.Text = "User 3"
        '
        'lblTextFilename1
        '
        Me.lblTextFilename1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename1.Location = New System.Drawing.Point(105, 35)
        Me.lblTextFilename1.Name = "lblTextFilename1"
        Me.lblTextFilename1.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename1.TabIndex = 10
        Me.lblTextFilename1.Tag = ""
        Me.lblTextFilename1.Text = "No file loaded."
        Me.lblTextFilename1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTextFilename2
        '
        Me.lblTextFilename2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename2.Location = New System.Drawing.Point(105, 67)
        Me.lblTextFilename2.Name = "lblTextFilename2"
        Me.lblTextFilename2.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename2.TabIndex = 11
        Me.lblTextFilename2.Tag = ""
        Me.lblTextFilename2.Text = "No file loaded."
        Me.lblTextFilename2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTextFilename3
        '
        Me.lblTextFilename3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename3.Location = New System.Drawing.Point(105, 99)
        Me.lblTextFilename3.Name = "lblTextFilename3"
        Me.lblTextFilename3.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename3.TabIndex = 12
        Me.lblTextFilename3.Tag = ""
        Me.lblTextFilename3.Text = "No file loaded."
        Me.lblTextFilename3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTextFilename4
        '
        Me.lblTextFilename4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename4.Location = New System.Drawing.Point(105, 131)
        Me.lblTextFilename4.Name = "lblTextFilename4"
        Me.lblTextFilename4.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename4.TabIndex = 13
        Me.lblTextFilename4.Tag = ""
        Me.lblTextFilename4.Text = "No file loaded."
        Me.lblTextFilename4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTextFilename5
        '
        Me.lblTextFilename5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename5.Location = New System.Drawing.Point(105, 163)
        Me.lblTextFilename5.Name = "lblTextFilename5"
        Me.lblTextFilename5.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename5.TabIndex = 14
        Me.lblTextFilename5.Tag = ""
        Me.lblTextFilename5.Text = "No file loaded."
        Me.lblTextFilename5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTextFilename6
        '
        Me.lblTextFilename6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename6.Location = New System.Drawing.Point(105, 195)
        Me.lblTextFilename6.Name = "lblTextFilename6"
        Me.lblTextFilename6.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename6.TabIndex = 15
        Me.lblTextFilename6.Tag = ""
        Me.lblTextFilename6.Text = "No file loaded."
        Me.lblTextFilename6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTextFilename7
        '
        Me.lblTextFilename7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename7.Location = New System.Drawing.Point(105, 227)
        Me.lblTextFilename7.Name = "lblTextFilename7"
        Me.lblTextFilename7.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename7.TabIndex = 16
        Me.lblTextFilename7.Tag = ""
        Me.lblTextFilename7.Text = "No file loaded."
        Me.lblTextFilename7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTextFilename8
        '
        Me.lblTextFilename8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTextFilename8.Location = New System.Drawing.Point(105, 259)
        Me.lblTextFilename8.Name = "lblTextFilename8"
        Me.lblTextFilename8.Size = New System.Drawing.Size(210, 16)
        Me.lblTextFilename8.TabIndex = 17
        Me.lblTextFilename8.Tag = ""
        Me.lblTextFilename8.Text = "No file loaded."
        Me.lblTextFilename8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnButtonDevice
        '
        Me.btnButtonDevice.Location = New System.Drawing.Point(7, 320)
        Me.btnButtonDevice.Name = "btnButtonDevice"
        Me.btnButtonDevice.Size = New System.Drawing.Size(157, 23)
        Me.btnButtonDevice.TabIndex = 18
        Me.btnButtonDevice.Text = "&View Device Mappings"
        '
        'timer1
        '
        '
        'DlgMainForm
        '
        Me.AcceptButton = Me.btnOk
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(378, 354)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnOk, Me.btnButtonBass, Me.btnButtonSnare, Me.btnButtonHihatUp, Me.btnButtonHihatDown, Me.btnButtonCrash, Me.btnButtonUser1, Me.btnButtonUser2, Me.btnButtonUser3, Me.lblTextFilename1, Me.lblTextFilename2, Me.lblTextFilename3, Me.lblTextFilename4, Me.lblTextFilename5, Me.lblTextFilename6, Me.lblTextFilename7, Me.lblTextFilename8, Me.btnButtonDevice, Me.gbStatic})
        Me.Name = "DlgMainForm"
        Me.Text = "Drum Pad"
        Me.ResumeLayout(False)

    End Sub 'InitializeComponent

#End Region

    Private Sub DlgMainForm_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim sdkpath As String = DXUtil.SdkMediaPath

        ' Get the graphics object for the static groupbox control.
        applicationGraphics = gbStatic.CreateGraphics()
        mappings = New ActionFormat(0) {}
        mappings(0) = New ActionFormat()

        ConstructActionMap()
        ' Get the devices on the system that can be mapped to the action map, and are attached.
        applicationDevices = Manager.GetDevices(mappings(0), EnumDevicesBySemanticsFlags.AttachedOnly)

        ' Make sure there are sound devices available.
        If 0 = applicationDevices.Count Then
            MessageBox.Show("No available input devices. Sample will close.")
            Close()
            Return
        End If

        Try
            ' Create a sound device, and sound buffer array.
            applicationSoundDevice = New SoundDevice()
            applicationSoundDevice.SetCooperativeLevel(Me, CooperativeLevel.Normal)
            applicationSoundBuffers = New SecondaryBuffer(8) {}
        Catch
            MessageBox.Show("No available sound device. Sample will close.")
            Close()
            Return
        End Try

        Dim instance As SemanticsInstance
        For Each instance In applicationDevices
            ' Build and set the action map against each device.
            instance.Device.BuildActionMap(mappings(0), ActionMapControl.Default)
            instance.Device.SetActionMap(mappings(0), ApplyActionMap.Default)
        Next instance

        ' Load default wav files.
        LoadSoundFile(sdkpath + "drumpad-bass_drum.wav", 0)
        LoadSoundFile(sdkpath + "drumpad-snare_drum.wav", 1)
        LoadSoundFile(sdkpath + "drumpad-hhat_up.wav", 2)
        LoadSoundFile(sdkpath + "drumpad-hhat_down.wav", 3)
        LoadSoundFile(sdkpath + "drumpad-crash.wav", 4)
        LoadSoundFile(sdkpath + "drumpad-voc_female_ec.wav", 5)
        LoadSoundFile(sdkpath + "drumpad-speech.wav", 6)

        ' Turn on the timer.
        timer1.Enabled = True
    End Sub 'DlgMainForm_Load

    Sub ConstructActionMap()
        '-----------------------------------------------------------------------------
        ' Name: ConstructActionMap()
        ' Desc: Prepares the action map to be used
        '-----------------------------------------------------------------------------
        ' Allocate and copy static DIACTION array
        InitializeActions()

        Dim a As Action
        For Each a In actions
            mappings(0).Actions.Add(a)
        Next a
        ' Set the application Guid
        mappings(0).ActionMapGuid = guidApp
        mappings(0).AxisMax = 100
        mappings(0).AxisMin = -100
        mappings(0).BufferSize = 16

        ' Game genre
        mappings(0).Genre = CInt(FightingThirdPerson.FightingThirdPerson)

        ' Friendly name for this mapping
        mappings(0).ActionMap = "DeviceView - Sample Action Map"
    End Sub 'ConstructActionMap


    Sub InitializeActions()
        '-----------------------------------------------------------------------------
        ' actions array
        '-----------------------------------------------------------------------------
        ' Be sure to delete user map files 
        ' (GameActions.C:\Program Files\Common Files\DirectX\DirectInput\User Maps\*.ini)
        ' after changing this, otherwise settings won't reset and will be read 
        ' from the out of date ini files 
        actions = New Action() {MakeAction(GameActions.BassDrum, CInt(FightingThirdPerson.ButtonAction), "Bass Drum"), MakeAction(GameActions.SnareDrum, CInt(FightingThirdPerson.ButtonJump), "Snare Drum"), MakeAction(GameActions.HiHatOpen, CInt(FightingThirdPerson.ButtonUse), "Open Hi-Hat"), MakeAction(GameActions.HiHatClose, CInt(FightingThirdPerson.ButtonRun), "Closed Hi-Hat"), MakeAction(GameActions.Crash, CInt(FightingThirdPerson.ButtonMenu), "Crash"), MakeAction(GameActions.User1, CInt(FightingThirdPerson.ButtonDodge), "User 1"), MakeAction(GameActions.User2, CInt(FightingThirdPerson.ButtonSelect), "User 2"), MakeAction(GameActions.User3, CInt(FightingThirdPerson.ButtonView), "User 3"), MakeAction(GameActions.BassDrum, CInt(Keyboard.B), "Bass Drum"), MakeAction(GameActions.SnareDrum, CInt(Keyboard.N), "Snare Drum"), MakeAction(GameActions.HiHatOpen, CInt(Keyboard.A), "Open Hi-Hat"), MakeAction(GameActions.HiHatClose, CInt(Keyboard.Z), "Closed Hi-Hat"), MakeAction(GameActions.Crash, CInt(Keyboard.C), "Crash"), MakeAction(GameActions.User1, CInt(Keyboard.K), "User 1"), MakeAction(GameActions.User2, CInt(Keyboard.L), "User 2"), MakeAction(GameActions.User3, CInt(Keyboard.SemiColon), "User 3"), MakeAction(GameActions.BassDrum, CInt(Mouse.Button2), "Bass Drum"), MakeAction(GameActions.SnareDrum, CInt(Mouse.Button1), "Snare Drum"), MakeAction(GameActions.HiHatOpen, CInt(Mouse.Button3), "Open Hi-Hat"), MakeAction(GameActions.HiHatClose, CInt(Mouse.Button4), "Closed Hi-Hat"), MakeAction(GameActions.Crash, CInt(Mouse.Wheel), "Crash"), MakeAction(GameActions.User1, CInt(Mouse.Button5), "User 1"), MakeAction(GameActions.User2, CInt(Mouse.Button6), "User 2"), MakeAction(GameActions.User3, CInt(Mouse.Button7), "User 3")}
    End Sub 'InitializeActions

    ' genre defined virtual buttons

    ' keyboard mapping

    ' mouse mapping
    Function MakeAction(ByVal a As GameActions, ByVal semantic As Integer, ByVal name As String) As Action
        ' Fills in an action struct with data.
        Dim ret As New Action()
        ret.ActionName = name
        ret.Semantic = semantic
        ret.ApplicationData = a

        Return ret
    End Function 'MakeAction

    Sub ProcessInput()
        '-----------------------------------------------------------------------------
        ' Name: ProcessInput()
        ' Desc: Gathers user input, plays audio, and draws output. Input is gathered 
        '       from the DInput devices, and output is displayed in the app's window.
        '-----------------------------------------------------------------------------
        ' Loop through all devices and check game input
        applicationDevices.Reset()
        Dim instance As SemanticsInstance
        For Each instance In applicationDevices
            Dim data As BufferedDataCollection = Nothing

            ' Need to ensure that the devices are acquired, and pollable devices
            ' are polled.
            Try
                instance.Device.Acquire()
                instance.Device.Poll()
                data = instance.Device.GetBufferedData()
            Catch
                GoTo ContinueForEach1
            End Try
            If Nothing Is data Then
                GoTo ContinueForEach1
            End If
            ' Get the sematics codes. 
            ' Each event has a type stored in "ApplicationData", and actual data is stored in
            ' "Data".
            Dim d As BufferedData
            For Each d In data
                ' Non-axis data is recieved as "button pressed" or "button
                ' released". Parse input as such.
                Dim state As Boolean = IIf(&H80 = d.Data, True, False)
                Dim index As Integer = CInt(d.ApplicationData)

                If buttonStates(index) = False And state Then
                    PlaySound(index)
                    boxColors(index) = 255.0F
                End If
                buttonStates(index) = state
            Next d
ContinueForEach1:
        Next instance

        DrawRects(False)
    End Sub 'ProcessInput


    Sub DrawRects(ByVal drawAll As Boolean)
        Dim c As New Color()

        Dim i As Integer
        For i = 0 To CInt(GameActions.NumberActions) - 1
            Dim r As New Rectangle(RectangleLeft, 0, 35, 16)

            If boxColors(i) >= 0.0F Or drawAll Then
                ' make sure color is not negative
                If boxColors(i) < 0.0F Then
                    boxColors(i) = 0.0F
                End If
                ' figure out what color to use
                c = Color.FromArgb(CInt(boxColors(i)) / 2, CInt(boxColors(i)), 0)

                r.Offset(0, RectangleTop + CInt(i * 32))

                ' draw the rectangle
                applicationGraphics.FillRectangle(New SolidBrush(c), r)

                ' fade the color to black
                boxColors(i) -= 3.5F
            End If
        Next i
    End Sub 'DrawRects

    Private Sub btnOk_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnOk.Click
        Close()
    End Sub 'btnOk_Click


    Sub PlaySound(ByVal index As Integer)
        If Not Nothing Is applicationSoundBuffers(index) Then
            applicationSoundBuffers(index).Play(0, BufferPlayFlags.Default)
        End If
    End Sub 'PlaySound

    Private Sub FileChange_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnButtonBass.Click, btnButtonSnare.Click, btnButtonHihatUp.Click, btnButtonHihatDown.Click, btnButtonCrash.Click, btnButtonUser1.Click, btnButtonUser2.Click, btnButtonUser3.Click
        ' Get the index from the tag object of the sender.
        Dim s As String = CStr(CType(sender, System.Windows.Forms.Button).Tag)
        Dim i As Integer = Val(s.Chars(0))

        Dim ofd As New OpenFileDialog()
        ofd.InitialDirectory = pathSoundFile
        ofd.Filter = "Wave files(*.wav)|*.wav"

        If DialogResult.Cancel = ofd.ShowDialog() Then
            Return
        End If
        If LoadSoundFile(ofd.FileName, i) Then
            pathSoundFile = Path.GetDirectoryName(ofd.FileName)
        End If
    End Sub 'FileChange_Click

    Private Function LoadSoundFile(ByVal filename As String, ByVal index As Integer) As Boolean
        Dim desc As New BufferDescription()
        Dim target As Label = Nothing

        ' Iterate the collection to find the label control that corresponds to this index.
        Dim temp As Control
        For Each temp In Controls
            If GetType(Label) Is temp.GetType() Then
                If temp.Name = "lblTextFilename" + (index + 1).ToString() Then
                    target = CType(temp, Label)
                    Exit For
                End If
            End If
        Next temp

        Try
            ' Attempt to create a sound buffer out of this file.
            applicationSoundBuffers(index) = New SecondaryBuffer(filename, desc, applicationSoundDevice)
        Catch
            target.Text = "No file loaded."
            Return False
        End Try

        target.Text = Path.GetFileName(filename)
        Return True
    End Function 'LoadSoundFile


    Private Sub gbStatic_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles gbStatic.Paint
        DrawRects(True)
    End Sub 'gbStatic_Paint


    Private Sub btnButtonDevice_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnButtonDevice.Click
        Dim cdp As New ConfigureDevicesParameters()
        cdp.SetActionFormats(mappings)

        Manager.ConfigureDevices(cdp, ConfigureDevicesFlags.Default)
    End Sub 'btnButtonDevice_Click

    Private Sub timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timer1.Tick
        ProcessInput()
    End Sub

End Class