Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectInput
Imports Microsoft.DirectX

Public Class KeyboardForm
    Inherits Form
    Private components As System.ComponentModel.IContainer
    Private staticGroupBox1 As GroupBox
    Private staticGroupBox2 As GroupBox
    Private staticGroupBox3 As GroupBox
    Private staticDataLabel As Label
    Private dataLabel As Label
    Private WithEvents createDevice As System.Windows.Forms.Button
    Private WithEvents cancel As System.Windows.Forms.Button
    Private behaviorLabel As Label
    Private WithEvents grabData As System.Windows.Forms.Timer
    Private WithEvents immediate As System.Windows.Forms.RadioButton
    Private WithEvents buffered As System.Windows.Forms.RadioButton
    Private WithEvents exclusive As System.Windows.Forms.RadioButton
    Private WithEvents nonexclusive As System.Windows.Forms.RadioButton
    Private WithEvents windowsKey As System.Windows.Forms.CheckBox
    Private panel1 As System.Windows.Forms.Panel
    Private WithEvents foreground As System.Windows.Forms.RadioButton
    Private WithEvents background As System.Windows.Forms.RadioButton

    Private sizeSampleBuffer As Integer = 8

    Private applicationDevice As Device = Nothing

    Public Shared Function Main(ByVal Args() As String) As Integer
        Application.Run(New KeyboardForm())
        Return 0
    End Function 'Main


    Public Sub New()
        '
        ' Required for Windows Form Designer support.
        '
        InitializeComponent()
    End Sub 'New

    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Dim resources As New System.Resources.ResourceManager(GetType(KeyboardForm))
        Me.staticGroupBox1 = New System.Windows.Forms.GroupBox()
        Me.exclusive = New System.Windows.Forms.RadioButton()
        Me.nonexclusive = New System.Windows.Forms.RadioButton()
        Me.windowsKey = New System.Windows.Forms.CheckBox()
        Me.panel1 = New System.Windows.Forms.Panel()
        Me.background = New System.Windows.Forms.RadioButton()
        Me.foreground = New System.Windows.Forms.RadioButton()
        Me.staticGroupBox2 = New System.Windows.Forms.GroupBox()
        Me.immediate = New System.Windows.Forms.RadioButton()
        Me.buffered = New System.Windows.Forms.RadioButton()
        Me.staticDataLabel = New System.Windows.Forms.Label()
        Me.dataLabel = New System.Windows.Forms.Label()
        Me.createDevice = New System.Windows.Forms.Button()
        Me.cancel = New System.Windows.Forms.Button()
        Me.staticGroupBox3 = New System.Windows.Forms.GroupBox()
        Me.behaviorLabel = New System.Windows.Forms.Label()
        Me.GrabData = New System.Windows.Forms.Timer(Me.components)
        Me.staticGroupBox1.SuspendLayout()
        Me.panel1.SuspendLayout()
        Me.staticGroupBox2.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' staticGroupBox1
        ' 
        Me.staticGroupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.exclusive, Me.nonexclusive, Me.windowsKey, Me.panel1})
        Me.staticGroupBox1.Location = New System.Drawing.Point(10, 11)
        Me.staticGroupBox1.Name = "staticGroupBox1"
        Me.staticGroupBox1.Size = New System.Drawing.Size(247, 79)
        Me.staticGroupBox1.TabIndex = 0
        Me.staticGroupBox1.TabStop = False
        Me.staticGroupBox1.Text = "Cooperative Level"
        ' 
        ' exclusive
        ' 
        Me.exclusive.Checked = True
        Me.exclusive.Location = New System.Drawing.Point(15, 13)
        Me.exclusive.Name = "exclusive"
        Me.exclusive.Size = New System.Drawing.Size(72, 16)
        Me.exclusive.TabIndex = 6
        Me.exclusive.TabStop = True
        Me.exclusive.Text = "Exclusive"
        ' 
        ' nonexclusive
        ' 
        Me.nonexclusive.Location = New System.Drawing.Point(15, 31)
        Me.nonexclusive.Name = "nonexclusive"
        Me.nonexclusive.Size = New System.Drawing.Size(96, 16)
        Me.nonexclusive.TabIndex = 7
        Me.nonexclusive.Text = "Nonexclusive"
        ' 
        ' windowsKey
        ' 
        Me.windowsKey.Location = New System.Drawing.Point(15, 50)
        Me.windowsKey.Name = "windowsKey"
        Me.windowsKey.Size = New System.Drawing.Size(105, 16)
        Me.windowsKey.TabIndex = 8
        Me.windowsKey.Text = "Disable Windows Key"
        ' 
        ' panel1
        ' 
        Me.panel1.Controls.AddRange(New System.Windows.Forms.Control() {Me.background, Me.foreground})
        Me.panel1.Location = New System.Drawing.Point(136, 8)
        Me.panel1.Name = "panel1"
        Me.panel1.Size = New System.Drawing.Size(104, 48)
        Me.panel1.TabIndex = 15
        ' 
        ' background
        ' 
        Me.background.Location = New System.Drawing.Point(8, 24)
        Me.background.Name = "background"
        Me.background.Size = New System.Drawing.Size(90, 16)
        Me.background.TabIndex = 12
        Me.background.Text = "Background"
        ' 
        ' foreground
        ' 
        Me.foreground.Checked = True
        Me.foreground.Location = New System.Drawing.Point(8, 8)
        Me.foreground.Name = "foreground"
        Me.foreground.Size = New System.Drawing.Size(82, 16)
        Me.foreground.TabIndex = 11
        Me.foreground.TabStop = True
        Me.foreground.Text = "Foreground"
        ' 
        ' staticGroupBox2
        ' 
        Me.staticGroupBox2.Controls.AddRange(New System.Windows.Forms.Control() {Me.immediate, Me.buffered})
        Me.staticGroupBox2.Location = New System.Drawing.Point(267, 11)
        Me.staticGroupBox2.Name = "staticGroupBox2"
        Me.staticGroupBox2.Size = New System.Drawing.Size(105, 79)
        Me.staticGroupBox2.TabIndex = 6
        Me.staticGroupBox2.TabStop = False
        Me.staticGroupBox2.Text = "Data Style"
        ' 
        ' immediate
        ' 
        Me.immediate.Checked = True
        Me.immediate.Location = New System.Drawing.Point(13, 22)
        Me.immediate.Name = "immediate"
        Me.immediate.Size = New System.Drawing.Size(78, 16)
        Me.immediate.TabIndex = 9
        Me.immediate.TabStop = True
        Me.immediate.Text = "Immediate"
        ' 
        ' buffered
        ' 
        Me.buffered.Location = New System.Drawing.Point(13, 40)
        Me.buffered.Name = "buffered"
        Me.buffered.Size = New System.Drawing.Size(70, 16)
        Me.buffered.TabIndex = 10
        Me.buffered.Text = "Buffered"
        ' 
        ' staticDataLabel
        ' 
        Me.staticDataLabel.Location = New System.Drawing.Point(16, 347)
        Me.staticDataLabel.Name = "staticDataLabel"
        Me.staticDataLabel.Size = New System.Drawing.Size(32, 13)
        Me.staticDataLabel.TabIndex = 9
        Me.staticDataLabel.Text = "Data:"
        ' 
        ' dataLabel
        ' 
        Me.dataLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.dataLabel.Location = New System.Drawing.Point(48, 344)
        Me.dataLabel.Name = "dataLabel"
        Me.dataLabel.Size = New System.Drawing.Size(504, 18)
        Me.dataLabel.TabIndex = 10
        Me.dataLabel.Text = "Device not created"
        ' 
        ' createDevice
        ' 
        Me.createDevice.Location = New System.Drawing.Point(10, 376)
        Me.createDevice.Name = "createDevice"
        Me.createDevice.Size = New System.Drawing.Size(102, 23)
        Me.createDevice.TabIndex = 11
        Me.createDevice.Text = "Create Device"
        ' 
        ' cancel
        ' 
        Me.cancel.Location = New System.Drawing.Point(477, 376)
        Me.cancel.Name = "cancel"
        Me.cancel.TabIndex = 12
        Me.cancel.Text = "Exit"
        ' 
        ' staticGroupBox3
        ' 
        Me.staticGroupBox3.Location = New System.Drawing.Point(10, 91)
        Me.staticGroupBox3.Name = "staticGroupBox3"
        Me.staticGroupBox3.Size = New System.Drawing.Size(541, 245)
        Me.staticGroupBox3.TabIndex = 13
        Me.staticGroupBox3.TabStop = False
        Me.staticGroupBox3.Text = "Expected Behavior"
        ' 
        ' behaviorLabel
        ' 
        Me.behaviorLabel.Location = New System.Drawing.Point(21, 108)
        Me.behaviorLabel.Name = "behaviorLabel"
        Me.behaviorLabel.Size = New System.Drawing.Size(519, 216)
        Me.behaviorLabel.TabIndex = 14
        Me.behaviorLabel.Text = "Behavior"
        ' 
        ' grabData
        ' 
        Me.GrabData.Interval = 83
        ' 
        ' KeyboardForm
        ' 
        Me.AcceptButton = Me.createDevice
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(562, 410)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.behaviorLabel, Me.staticDataLabel, Me.dataLabel, Me.createDevice, Me.cancel, Me.staticGroupBox1, Me.staticGroupBox2, Me.staticGroupBox3})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.KeyPreview = True
        Me.MaximizeBox = False
        Me.Name = "KeyboardForm"
        Me.Text = "DirectInput Keyboard Sample"
        Me.staticGroupBox1.ResumeLayout(False)
        Me.panel1.ResumeLayout(False)
        Me.staticGroupBox2.ResumeLayout(False)
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    Private Sub ChangeProperties(ByVal sender As Object, ByVal e As System.EventArgs) Handles exclusive.Click, nonexclusive.Click, background.Click, foreground.Click, immediate.Click, buffered.Click '
        UpdateUI()
    End Sub 'ChangeProperties


    Private Sub btnCreatedevice_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles createDevice.Click
        If Not Nothing Is applicationDevice Then
            FreeDirectInput()
            UpdateUI()
            Return
        End If

        Dim coopFlags As CooperativeLevelFlags

        ' Cleanup any previous call first.
        GrabData.Enabled = False
        FreeDirectInput()

        If True = exclusive.Checked Then
            coopFlags = CooperativeLevelFlags.Exclusive
        Else
            coopFlags = CooperativeLevelFlags.NonExclusive
        End If
        If True = foreground.Checked Then
            coopFlags = coopFlags Or CooperativeLevelFlags.Foreground
        Else
            coopFlags = coopFlags Or CooperativeLevelFlags.Background
        End If
        ' Disabling the windows key is only allowed only if we are in foreground nonexclusive.
        If True = windowsKey.Checked And Not exclusive.Checked And foreground.Checked Then
            coopFlags = coopFlags Or CooperativeLevelFlags.NoWindowsKey
        End If
        ' Obtain an instantiated system keyboard device.
        applicationDevice = New Device(SystemGuid.Keyboard)

        ' Set the cooperative level to let DirectInput know how
        ' this device should interact with the system and with other
        ' DirectInput applications.
        Try
            applicationDevice.SetCooperativeLevel(Me, coopFlags)
        Catch dx As InputException
            If TypeOf dx Is UnsupportedException And Not foreground.Checked Then
                FreeDirectInput()
                MessageBox.Show("SetCooperativeLevel() returned ErrorCode.Unsupported." + ControlChars.Lf + "For security reasons, background keyboard" + ControlChars.Lf + "access is not allowed.", "Keyboard")
                Return
            End If
        End Try

        If False = immediate.Checked Then
            ' IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
            '
            ' DirectInput uses unbuffered I/O (buffer size = 0) by default.
            ' If you want to read buffered data, you need to set a nonzero
            ' buffer size.
            '
            ' The buffer size is an int property associated with the device.
            Try
                applicationDevice.Properties.BufferSize = sizeSampleBuffer
            Catch
                Return
            End Try
        End If

        applicationDevice.Acquire()
        GrabData.Enabled = True
        UpdateUI()
    End Sub 'btnCreatedevice_Click


    Private Sub btnCancel_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles cancel.Click
        FreeDirectInput()
        Me.Close()
    End Sub 'btnCancel_Click


    Private Sub KeyboardForm_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated
        If Not Nothing Is applicationDevice Then
            Try
                applicationDevice.Acquire()
            Catch
            End Try
        End If
    End Sub 'KeyboardForm_Activated

    Private Sub GrabData_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles GrabData.Tick
        ' Update the input device every timer message.
        Dim isImmediate As Boolean = immediate.Checked

        If True = isImmediate Then
            If Not ReadImmediateData() Then
                grabData.Enabled = False
                MessageBox.Show("Error reading input state. The sample will now exit.", "Keyboard")
                Me.Close()
            End If
        Else
            If Not ReadBufferedData() Then
                grabData.Enabled = False
                MessageBox.Show("Error reading input state. The sample will now exit.", "Keyboard")
                Me.Close()
            End If
        End If
    End Sub 'GrabData_Tick


    Private Sub KeyboardForm_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        GrabData.Enabled = False
        FreeDirectInput()
    End Sub 'KeyboardForm_Closing


    Private Sub FreeDirectInput()
        ' Unacquire the device one last time just in case 
        ' the app tried to exit while the device is still acquired.
        If Not Nothing Is applicationDevice Then
            applicationDevice.Unacquire()
            applicationDevice.Dispose()
            applicationDevice = Nothing
        End If
    End Sub 'FreeDirectInput


    '-----------------------------------------------------------------------------
    ' Name: UpdateUI()
    ' Desc: Enables/disables the UI, and sets the dialog behavior text based on the UI
    '-----------------------------------------------------------------------------
    Private Sub UpdateUI()
        Dim strExpected As String = [String].Empty
        Dim isExclusive As Boolean
        Dim isForeground As Boolean
        Dim isImmediate As Boolean
        Dim DisableWindowsKey As Boolean

        ' Determine where the buffer would like to be allocated.
        isExclusive = exclusive.Checked
        isForeground = foreground.Checked
        isImmediate = immediate.Checked
        DisableWindowsKey = windowsKey.Checked

        If Not Nothing Is applicationDevice Then
            createDevice.Text = "Release Device"
            dataLabel.Text = [String].Empty

            exclusive.Enabled = False
            foreground.Enabled = False
            nonexclusive.Enabled = False
            background.Enabled = False
            immediate.Enabled = False
            buffered.Enabled = False
            windowsKey.Enabled = False
        Else
            createDevice.Text = "&Create Device"
            dataLabel.Text = "Device not created. Choose settings and click 'Create Device' then type to see results"

            exclusive.Enabled = True
            foreground.Enabled = True
            nonexclusive.Enabled = True
            background.Enabled = True
            immediate.Enabled = True
            buffered.Enabled = True

            If Not isExclusive And isForeground Then
                windowsKey.Enabled = True
            Else
                windowsKey.Enabled = False
            End If
        End If
        ' Figure what the user should expect based on the dialog choice.
        If Not isForeground And isExclusive Then
            strExpected = "For security reasons, background exclusive " + "keyboard access is not allowed." + ControlChars.Lf + ControlChars.Lf
        Else
            If isForeground Then
                strExpected = "Foreground cooperative level means that the " + "application has access to data only when in the " + "foreground or, in other words, has the input focus. " + "If the application moves to the background, " + "the device is automatically unacquired, or made " + "unavailable." + ControlChars.Lf + ControlChars.Lf
            Else
                strExpected = "Background cooperative level really means " + "foreground and background. A device with a " + "background cooperative level can be acquired " + "and used by an application at any time." + ControlChars.Lf + ControlChars.Lf
            End If

            If isExclusive Then
                strExpected += "Exclusive mode prevents other applications from " + "also acquiring the device exclusively. The fact " + "that your application is using a device at the " + "exclusive level does not mean that other " + "applications cannot get data from the device. " + "When an application has exclusive access to the " + "keyboard, DirectInput suppresses all keyboard " + "messages including the Windows key except " + "CTRL+ALT+DEL and ALT+TAB" + ControlChars.Lf + ControlChars.Lf
            Else
                strExpected += "Nonexclusive mode means that other applications " + "can acquire device in exclusive or nonexclusive mode. "

                If True = DisableWindowsKey Then
                    strExpected += "The Windows key will also be disabled so that " + "users cannot inadvertently break out of the " + "application. "
                End If
                strExpected += ControlChars.Lf + ControlChars.Lf
            End If
            If True = isImmediate Then
                strExpected += "immediate data is a snapshot of the current " + "state of a device. It provides no data about " + "what has happened with the device since the " + "last call, apart from implicit information that " + "you can derive by comparing the current state with " + "the last one. Events in between calls are lost." + ControlChars.Lf + ControlChars.Lf
            Else
                strExpected += "Buffered data is a record of events that are stored " + "until an application retrieves them. With buffered " + "data, events are stored until you are ready to deal " + "with them. If the buffer overflows, new data is lost." + ControlChars.Lf + ControlChars.Lf
            End If

            strExpected += "The sample will read the keyboard 12 times a second. " + "Typically an application would poll the keyboard " + "much faster than this, but this slow rate is simply " + "for the purposes of demonstration."
        End If
        behaviorLabel.Text = strExpected
    End Sub 'UpdateUI

    '-----------------------------------------------------------------------------
    ' Name: ReadBufferedData()
    ' Desc: Read the input device's state when in buffered mode and display it.
    '-----------------------------------------------------------------------------
    Function ReadBufferedData() As Boolean
        Dim dataCollection As BufferedDataCollection = Nothing
        Dim textNew As String = [String].Empty

        If Nothing Is applicationDevice Then
            Return True
        End If
        Dim ie As InputException = Nothing
        Try
            dataCollection = applicationDevice.GetBufferedData()
        Catch
            ' We got an error 
            '
            ' It means that continuous contact with the
            ' device has been lost, either due to an external
            ' interruption, or because the buffer overflowed
            ' and some events were lost.
            '
            ' Consequently, if a button was pressed at the time
            ' the buffer overflowed or the connection was broken,
            ' the corresponding "up" message might have been lost.
            '
            ' But since our simple sample doesn't actually have
            ' any state associated with button up or down events,
            ' there is no state to reset.  (In a real game, ignoring
            ' the buffer overflow would result in the game thinking
            ' a key was held down when in fact it isn't; it's just
            ' that the "up" event got lost because the buffer
            ' overflowed.)
            '
            ' If we want to be more clever, we could do a
            ' GetBufferedData() and compare the current state
            ' against the state we think the device is in,
            ' and process all the states that are currently
            ' different from our private state.
            Try
                applicationDevice.Acquire()
            Catch inputException As InputException
                ie = inputException
            End Try

            ' Update the dialog text 
            If TypeOf ie Is OtherApplicationHasPriorityException Or TypeOf ie Is NotAcquiredException Then
                ' Exception may be OtherApplicationHasPriorityException or other exceptions.
                ' This may occur when the app is minimized or in the process of 
                ' switching, so just try again later.
                dataLabel.Text = "Unacquired"
                Return True
            ElseIf Not ie Is Nothing Then
                ' Some other exception occurred where the application is unable
                ' to acquire the device. The app will exit at this point.
                MessageBox.Show("Unable to acquire the device. Application will now exit.")
                Return False
            End If
        End Try

        If Nothing Is dataCollection Then
            Return True
        End If

        ' Study each of the buffer elements and process them.
        '
        ' Since we really don't do anything, our "processing"
        ' consists merely of squirting the name into our
        ' local buffer.
        Dim d As BufferedData

        For Each d In dataCollection
            ' this will display then scan code of the key
            ' plus a 'D' - meaning the key was pressed 
            '   or a 'U' - meaning the key was released
            textNew += [String].Format("0x{0:X}", d.Offset)
            textNew += IIf(0 <> (d.Data And &H80), "D ", "U ")
        Next

        ' If nothing changed then don't repaint - avoid flicker.
        If dataLabel.Text <> textNew Then
            dataLabel.Text = textNew
        End If
        Return True
    End Function 'ReadBufferedData

    '-----------------------------------------------------------------------------
    ' Name: ReadImmediateData()
    ' Desc: Read the input device's state when in immediate mode and display it.
    '-----------------------------------------------------------------------------
    Function ReadImmediateData() As Boolean
        Dim textNew As String = [String].Empty
        Dim state As KeyboardState = Nothing

        If Nothing Is applicationDevice Then
            Return True
        End If

        ' Get the input's device state, and store it.
        Dim ie As InputException = Nothing
        Try
            state = applicationDevice.GetCurrentKeyboardState()
        Catch
            ' DirectInput may be telling us that the input stream has been
            ' interrupted.  We aren't tracking any state between polls, so
            ' we don't have any special reset that needs to be done.
            ' We just re-acquire and try again.
            ' If input is lost then acquire and keep trying.
            Try
                applicationDevice.Acquire()
            Catch inputException As InputException
                ie = inputException
            End Try

            ' Update the dialog text 
            If TypeOf ie Is OtherApplicationHasPriorityException Or TypeOf ie Is NotAcquiredException Then
                ' Exception may be OtherApplicationHasPriorityException or other exceptions.
                ' This may occur when the app is minimized or in the process of 
                ' switching, so just try again later.
                dataLabel.Text = "Unacquired"
                Return True
            ElseIf Not ie Is Nothing Then
                ' Some other exception occurred where the application is unable
                ' to acquire the device. The app will exit at this point.
                MessageBox.Show("Unable to acquire the device. Application will now exit.")
                Return False
            End If
        End Try

        ' No keyboard data available. Just return.
        If state Is Nothing Then Return True

        ' Make a string of the index values of the keys that are down.
        Dim k As Key
        For k = Key.Escape To Key.MediaSelect
            If state(k) Then
                textNew += k.ToString() + " "
            End If
        Next k
        ' If nothing changed then don't repaint - avoid flicker.
        If dataLabel.Text <> textNew Then
            dataLabel.Text = textNew
        End If
        Return True
    End Function 'ReadImmediateData
End Class 'KeyboardForm