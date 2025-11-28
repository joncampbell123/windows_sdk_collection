Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectInput
Imports Microsoft.DirectX


Namespace Joystick
    _
    Public Class MainClass
        Inherits System.Windows.Forms.Form
        ' These three fields hold common data that
        ' different threads will have to access
        Public Shared state As New JoystickState()
        Private applicationDevice As Device = Nothing
        Public Shared numPOVs As Integer = -1
        Private SliderCount As Integer = -1 ' Number of returned slider controls
        Private groupBox1 As System.Windows.Forms.GroupBox
        Private labelXAxis As System.Windows.Forms.Label '
        Private labelYAxis As System.Windows.Forms.Label
        Private labelZAxis As System.Windows.Forms.Label
        Private labelZRotation As System.Windows.Forms.Label
        Private labelYRotation As System.Windows.Forms.Label
        Private labelXRotation As System.Windows.Forms.Label
        Private labelPOV1 As System.Windows.Forms.Label
        Private labelPOV0 As System.Windows.Forms.Label
        Private labelSlider1 As System.Windows.Forms.Label
        Private labelSlider0 As System.Windows.Forms.Label
        Private labelPOV2 As System.Windows.Forms.Label
        Private labelPOV3 As System.Windows.Forms.Label
        Private WithEvents buttonExit As System.Windows.Forms.Button
        Private labelXAxisText As System.Windows.Forms.Label
        Private labelYAxisText As System.Windows.Forms.Label
        Private labelZAxisText As System.Windows.Forms.Label
        Private labelXRotationText As System.Windows.Forms.Label
        Private labelYRotationText As System.Windows.Forms.Label
        Private labelZRotationText As System.Windows.Forms.Label
        Private labelSlider0Text As System.Windows.Forms.Label
        Private labelSlider1Text As System.Windows.Forms.Label
        Private labelPOV2Text As System.Windows.Forms.Label
        Private labelPOV1Text As System.Windows.Forms.Label
        Private labelPOV0Text As System.Windows.Forms.Label
        Private labelPOV3Text As System.Windows.Forms.Label
        Private label1 As System.Windows.Forms.Label
        Private labelButtons As System.Windows.Forms.Label
        Private label2 As System.Windows.Forms.Label
        Private WithEvents timer1 As System.Windows.Forms.Timer
        Private components As System.ComponentModel.IContainer
        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.components = New System.ComponentModel.Container()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.labelButtons = New System.Windows.Forms.Label()
            Me.label1 = New System.Windows.Forms.Label()
            Me.labelPOV3 = New System.Windows.Forms.Label()
            Me.labelSlider1 = New System.Windows.Forms.Label()
            Me.labelSlider0 = New System.Windows.Forms.Label()
            Me.labelPOV2 = New System.Windows.Forms.Label()
            Me.labelPOV1 = New System.Windows.Forms.Label()
            Me.labelPOV0 = New System.Windows.Forms.Label()
            Me.labelZRotation = New System.Windows.Forms.Label()
            Me.labelYRotation = New System.Windows.Forms.Label()
            Me.labelXRotation = New System.Windows.Forms.Label()
            Me.labelZAxis = New System.Windows.Forms.Label()
            Me.labelYAxis = New System.Windows.Forms.Label()
            Me.labelXAxis = New System.Windows.Forms.Label()
            Me.labelPOV3Text = New System.Windows.Forms.Label()
            Me.labelPOV2Text = New System.Windows.Forms.Label()
            Me.labelPOV1Text = New System.Windows.Forms.Label()
            Me.labelPOV0Text = New System.Windows.Forms.Label()
            Me.labelSlider1Text = New System.Windows.Forms.Label()
            Me.labelSlider0Text = New System.Windows.Forms.Label()
            Me.labelZRotationText = New System.Windows.Forms.Label()
            Me.labelYRotationText = New System.Windows.Forms.Label()
            Me.labelXRotationText = New System.Windows.Forms.Label()
            Me.labelZAxisText = New System.Windows.Forms.Label()
            Me.labelYAxisText = New System.Windows.Forms.Label()
            Me.labelXAxisText = New System.Windows.Forms.Label()
            Me.buttonExit = New System.Windows.Forms.Button()
            Me.label2 = New System.Windows.Forms.Label()
            Me.timer1 = New System.Windows.Forms.Timer(Me.components)
            Me.groupBox1.SuspendLayout()
            Me.SuspendLayout()
            '
            'groupBox1
            '
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.labelButtons, Me.label1, Me.labelPOV3, Me.labelSlider1, Me.labelSlider0, Me.labelPOV2, Me.labelPOV1, Me.labelPOV0, Me.labelZRotation, Me.labelYRotation, Me.labelXRotation, Me.labelZAxis, Me.labelYAxis, Me.labelXAxis, Me.labelPOV3Text, Me.labelPOV2Text, Me.labelPOV1Text, Me.labelPOV0Text, Me.labelSlider1Text, Me.labelSlider0Text, Me.labelZRotationText, Me.labelYRotationText, Me.labelXRotationText, Me.labelZAxisText, Me.labelYAxisText, Me.labelXAxisText})
            Me.groupBox1.Location = New System.Drawing.Point(8, 64)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(264, 184)
            Me.groupBox1.TabIndex = 0
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Joystick State"
            '
            'labelButtons
            '
            Me.labelButtons.Location = New System.Drawing.Point(72, 157)
            Me.labelButtons.Name = "labelButtons"
            Me.labelButtons.Size = New System.Drawing.Size(184, 13)
            Me.labelButtons.TabIndex = 25
            '
            'label1
            '
            Me.label1.AutoSize = True
            Me.label1.Location = New System.Drawing.Point(16, 157)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(46, 13)
            Me.label1.TabIndex = 24
            Me.label1.Text = "Buttons:"
            '
            'labelPOV3
            '
            Me.labelPOV3.AutoSize = True
            Me.labelPOV3.Enabled = False
            Me.labelPOV3.Location = New System.Drawing.Point(192, 128)
            Me.labelPOV3.Name = "labelPOV3"
            Me.labelPOV3.Size = New System.Drawing.Size(10, 13)
            Me.labelPOV3.TabIndex = 23
            Me.labelPOV3.Text = "0"
            '
            'labelSlider1
            '
            Me.labelSlider1.AutoSize = True
            Me.labelSlider1.Enabled = False
            Me.labelSlider1.Location = New System.Drawing.Point(192, 40)
            Me.labelSlider1.Name = "labelSlider1"
            Me.labelSlider1.Size = New System.Drawing.Size(10, 13)
            Me.labelSlider1.TabIndex = 22
            Me.labelSlider1.Text = "0"
            '
            'labelSlider0
            '
            Me.labelSlider0.AutoSize = True
            Me.labelSlider0.Enabled = False
            Me.labelSlider0.Location = New System.Drawing.Point(192, 24)
            Me.labelSlider0.Name = "labelSlider0"
            Me.labelSlider0.Size = New System.Drawing.Size(10, 13)
            Me.labelSlider0.TabIndex = 21
            Me.labelSlider0.Text = "0"
            '
            'labelPOV2
            '
            Me.labelPOV2.AutoSize = True
            Me.labelPOV2.Enabled = False
            Me.labelPOV2.Location = New System.Drawing.Point(192, 112)
            Me.labelPOV2.Name = "labelPOV2"
            Me.labelPOV2.Size = New System.Drawing.Size(10, 13)
            Me.labelPOV2.TabIndex = 20
            Me.labelPOV2.Text = "0"
            '
            'labelPOV1
            '
            Me.labelPOV1.AutoSize = True
            Me.labelPOV1.Enabled = False
            Me.labelPOV1.Location = New System.Drawing.Point(192, 96)
            Me.labelPOV1.Name = "labelPOV1"
            Me.labelPOV1.Size = New System.Drawing.Size(10, 13)
            Me.labelPOV1.TabIndex = 19
            Me.labelPOV1.Text = "0"
            '
            'labelPOV0
            '
            Me.labelPOV0.AutoSize = True
            Me.labelPOV0.Enabled = False
            Me.labelPOV0.Location = New System.Drawing.Point(192, 80)
            Me.labelPOV0.Name = "labelPOV0"
            Me.labelPOV0.Size = New System.Drawing.Size(10, 13)
            Me.labelPOV0.TabIndex = 18
            Me.labelPOV0.Text = "0"
            '
            'labelZRotation
            '
            Me.labelZRotation.AutoSize = True
            Me.labelZRotation.Enabled = False
            Me.labelZRotation.Location = New System.Drawing.Point(88, 128)
            Me.labelZRotation.Name = "labelZRotation"
            Me.labelZRotation.Size = New System.Drawing.Size(10, 13)
            Me.labelZRotation.TabIndex = 17
            Me.labelZRotation.Text = "0"
            '
            'labelYRotation
            '
            Me.labelYRotation.AutoSize = True
            Me.labelYRotation.Enabled = False
            Me.labelYRotation.Location = New System.Drawing.Point(88, 112)
            Me.labelYRotation.Name = "labelYRotation"
            Me.labelYRotation.Size = New System.Drawing.Size(10, 13)
            Me.labelYRotation.TabIndex = 16
            Me.labelYRotation.Text = "0"
            '
            'labelXRotation
            '
            Me.labelXRotation.AutoSize = True
            Me.labelXRotation.Enabled = False
            Me.labelXRotation.Location = New System.Drawing.Point(88, 96)
            Me.labelXRotation.Name = "labelXRotation"
            Me.labelXRotation.Size = New System.Drawing.Size(10, 13)
            Me.labelXRotation.TabIndex = 15
            Me.labelXRotation.Text = "0"
            '
            'labelZAxis
            '
            Me.labelZAxis.AutoSize = True
            Me.labelZAxis.Enabled = False
            Me.labelZAxis.Location = New System.Drawing.Point(88, 56)
            Me.labelZAxis.Name = "labelZAxis"
            Me.labelZAxis.Size = New System.Drawing.Size(10, 13)
            Me.labelZAxis.TabIndex = 14
            Me.labelZAxis.Text = "0"
            '
            'labelYAxis
            '
            Me.labelYAxis.AutoSize = True
            Me.labelYAxis.Enabled = False
            Me.labelYAxis.Location = New System.Drawing.Point(88, 40)
            Me.labelYAxis.Name = "labelYAxis"
            Me.labelYAxis.Size = New System.Drawing.Size(10, 13)
            Me.labelYAxis.TabIndex = 13
            Me.labelYAxis.Text = "0"
            '
            'labelXAxis
            '
            Me.labelXAxis.AutoSize = True
            Me.labelXAxis.Enabled = False
            Me.labelXAxis.Location = New System.Drawing.Point(88, 24)
            Me.labelXAxis.Name = "labelXAxis"
            Me.labelXAxis.Size = New System.Drawing.Size(10, 13)
            Me.labelXAxis.TabIndex = 12
            Me.labelXAxis.Text = "0"
            '
            'labelPOV3Text
            '
            Me.labelPOV3Text.AutoSize = True
            Me.labelPOV3Text.Enabled = False
            Me.labelPOV3Text.Location = New System.Drawing.Point(136, 128)
            Me.labelPOV3Text.Name = "labelPOV3Text"
            Me.labelPOV3Text.Size = New System.Drawing.Size(41, 13)
            Me.labelPOV3Text.TabIndex = 11
            Me.labelPOV3Text.Text = "POV 3:"
            '
            'labelPOV2Text
            '
            Me.labelPOV2Text.AutoSize = True
            Me.labelPOV2Text.Enabled = False
            Me.labelPOV2Text.Location = New System.Drawing.Point(136, 112)
            Me.labelPOV2Text.Name = "labelPOV2Text"
            Me.labelPOV2Text.Size = New System.Drawing.Size(41, 13)
            Me.labelPOV2Text.TabIndex = 10
            Me.labelPOV2Text.Text = "POV 2:"
            '
            'labelPOV1Text
            '
            Me.labelPOV1Text.AutoSize = True
            Me.labelPOV1Text.Enabled = False
            Me.labelPOV1Text.Location = New System.Drawing.Point(136, 96)
            Me.labelPOV1Text.Name = "labelPOV1Text"
            Me.labelPOV1Text.Size = New System.Drawing.Size(41, 13)
            Me.labelPOV1Text.TabIndex = 9
            Me.labelPOV1Text.Text = "POV 1:"
            '
            'labelPOV0Text
            '
            Me.labelPOV0Text.AutoSize = True
            Me.labelPOV0Text.Enabled = False
            Me.labelPOV0Text.Location = New System.Drawing.Point(136, 80)
            Me.labelPOV0Text.Name = "labelPOV0Text"
            Me.labelPOV0Text.Size = New System.Drawing.Size(41, 13)
            Me.labelPOV0Text.TabIndex = 8
            Me.labelPOV0Text.Text = "POV 0:"
            '
            'labelSlider1Text
            '
            Me.labelSlider1Text.AutoSize = True
            Me.labelSlider1Text.Enabled = False
            Me.labelSlider1Text.Location = New System.Drawing.Point(136, 40)
            Me.labelSlider1Text.Name = "labelSlider1Text"
            Me.labelSlider1Text.Size = New System.Drawing.Size(46, 13)
            Me.labelSlider1Text.TabIndex = 7
            Me.labelSlider1Text.Text = "Slider 1:"
            '
            'labelSlider0Text
            '
            Me.labelSlider0Text.AutoSize = True
            Me.labelSlider0Text.Enabled = False
            Me.labelSlider0Text.Location = New System.Drawing.Point(136, 24)
            Me.labelSlider0Text.Name = "labelSlider0Text"
            Me.labelSlider0Text.Size = New System.Drawing.Size(46, 13)
            Me.labelSlider0Text.TabIndex = 6
            Me.labelSlider0Text.Text = "Slider 0:"
            '
            'labelZRotationText
            '
            Me.labelZRotationText.AutoSize = True
            Me.labelZRotationText.Enabled = False
            Me.labelZRotationText.Location = New System.Drawing.Point(16, 128)
            Me.labelZRotationText.Name = "labelZRotationText"
            Me.labelZRotationText.Size = New System.Drawing.Size(60, 13)
            Me.labelZRotationText.TabIndex = 5
            Me.labelZRotationText.Text = "Z Rotation:"
            '
            'labelYRotationText
            '
            Me.labelYRotationText.AutoSize = True
            Me.labelYRotationText.Enabled = False
            Me.labelYRotationText.Location = New System.Drawing.Point(16, 112)
            Me.labelYRotationText.Name = "labelYRotationText"
            Me.labelYRotationText.Size = New System.Drawing.Size(60, 13)
            Me.labelYRotationText.TabIndex = 4
            Me.labelYRotationText.Text = "Y Rotation:"
            '
            'labelXRotationText
            '
            Me.labelXRotationText.AutoSize = True
            Me.labelXRotationText.Enabled = False
            Me.labelXRotationText.Location = New System.Drawing.Point(16, 96)
            Me.labelXRotationText.Name = "labelXRotationText"
            Me.labelXRotationText.Size = New System.Drawing.Size(60, 13)
            Me.labelXRotationText.TabIndex = 3
            Me.labelXRotationText.Text = "X Rotation:"
            '
            'labelZAxisText
            '
            Me.labelZAxisText.AutoSize = True
            Me.labelZAxisText.Enabled = False
            Me.labelZAxisText.Location = New System.Drawing.Point(16, 56)
            Me.labelZAxisText.Name = "labelZAxisText"
            Me.labelZAxisText.Size = New System.Drawing.Size(39, 13)
            Me.labelZAxisText.TabIndex = 2
            Me.labelZAxisText.Text = "Z Axis:"
            '
            'labelYAxisText
            '
            Me.labelYAxisText.AutoSize = True
            Me.labelYAxisText.Enabled = False
            Me.labelYAxisText.Location = New System.Drawing.Point(16, 40)
            Me.labelYAxisText.Name = "labelYAxisText"
            Me.labelYAxisText.Size = New System.Drawing.Size(39, 13)
            Me.labelYAxisText.TabIndex = 1
            Me.labelYAxisText.Text = "Y Axis:"
            '
            'labelXAxisText
            '
            Me.labelXAxisText.AutoSize = True
            Me.labelXAxisText.Enabled = False
            Me.labelXAxisText.Location = New System.Drawing.Point(16, 24)
            Me.labelXAxisText.Name = "labelXAxisText"
            Me.labelXAxisText.Size = New System.Drawing.Size(39, 13)
            Me.labelXAxisText.TabIndex = 0
            Me.labelXAxisText.Text = "X Axis:"
            '
            'buttonExit
            '
            Me.buttonExit.Location = New System.Drawing.Point(232, 264)
            Me.buttonExit.Name = "buttonExit"
            Me.buttonExit.Size = New System.Drawing.Size(40, 24)
            Me.buttonExit.TabIndex = 0
            Me.buttonExit.Text = "Exit"
            '
            'label2
            '
            Me.label2.AutoSize = True
            Me.label2.Location = New System.Drawing.Point(8, 24)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(255, 13)
            Me.label2.TabIndex = 1
            Me.label2.Text = "This sample continously polls the joystick for data."
            '
            'timer1
            '
            '
            'MainClass
            '
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(280, 293)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.label2, Me.buttonExit, Me.groupBox1})
            Me.KeyPreview = True
            Me.MaximizeBox = False
            Me.Name = "MainClass"
            Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide
            Me.Text = "Joystick"
            Me.groupBox1.ResumeLayout(False)
            Me.ResumeLayout(False)

        End Sub 'InitializeComponent

        Public Sub New() '
            '
            ' Required for Windows Form Designer support.
            '
            InitializeComponent()
        End Sub 'New


        Private Sub buttonExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonExit.Click
            Close()
        End Sub 'buttonExit_Click


        Public Function InitDirectInput() As Boolean
            ' Enumerate joysticks in the system.
            Dim instance As DeviceInstance

            For Each instance In Manager.GetDevices(DeviceClass.GameControl, EnumDevicesFlags.AttachedOnly)
                ' Create the device.
                applicationDevice = New Device(instance.InstanceGuid)
                'Just pick the first one
                Exit For
            Next instance

            If (Nothing Is applicationDevice) Then
                MessageBox.Show("Unable to create a joystick device. Sample will exit.", "No joystick found")
                Return False
            End If

            ' Set the data format to the c_dfDIJoystick pre-defined format.
            applicationDevice.SetDataFormat(DeviceDataFormat.Joystick)
            ' Set the cooperative level for the device.
            applicationDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.Exclusive Or CooperativeLevelFlags.Foreground)
            ' Enumerate all the objects on the device.
            Dim d As DeviceObjectInstance
            For Each d In applicationDevice.Objects
                ' For axes that are returned, set the DIPROP_RANGE property for the
                ' enumerated axis in order to scale min/max values.
                If 0 <> (d.ObjectId And CInt(DeviceObjectTypeFlags.Axis)) Then
                    ' Set the range for the axis.
                    applicationDevice.Properties.SetRange(ParameterHow.ById, d.ObjectId, New InputRange(-1000, +1000))
                End If
                ' Update the controls to reflect what
                ' objects the device supports.
                UpdateControls(d)
            Next d
            Return True
        End Function 'InitDirectInput

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()
            ' Create a new instance of 
            ' the MainClass class.
            Application.Run(New MainClass())
        End Sub 'Main


        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            timer1.Stop()

            ' Unacquire all DirectInput objects.
            If Not Nothing Is applicationDevice Then
                applicationDevice.Unacquire()
            End If
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose


        Public Sub GetData()
            ' Make sure there is a valid device.
            If Nothing Is applicationDevice Then
                Return
            End If
            Try
                ' Poll the device for info.
                applicationDevice.Poll()
            Catch inputex As InputException
                If TypeOf inputex Is NotAcquiredException Or TypeOf inputex Is InputLostException Then
                    ' Check to see if either the app
                    ' needs to acquire the device, or
                    ' if the app lost the device to another
                    ' process.
                    Try
                        ' Acquire the device.
                        applicationDevice.Acquire()
                    Catch
                        ' Failed to acquire the device.
                        ' This could be because the app
                        ' doesn't have focus.
                        Return
                    End Try
                End If
            End Try
            'catch(InputException inputex)
            ' Get the state of the device.
            Try
                state = applicationDevice.CurrentJoystickState
                ' Catch any exceptions. None will be handled here, 
                ' any device re-aquisition will be handled above.  
            Catch
                Return
            End Try

            UpdateUI()
        End Sub 'GetData

        Private Sub UpdateUI()
            ' This function updated the UI with
            ' joystick state information.
            Dim strText As String = Nothing

            labelXAxis.Text = state.X.ToString()
            labelYAxis.Text = state.Y.ToString()
            labelZAxis.Text = state.Z.ToString()

            labelXRotation.Text = state.Rx.ToString()
            labelYRotation.Text = state.Ry.ToString()
            labelZRotation.Text = state.Rz.ToString()


            Dim slider As Integer() = state.GetSlider()

            labelSlider0.Text = slider(0).ToString()
            labelSlider1.Text = slider(1).ToString()

            Dim pov As Integer() = state.GetPointOfView()

            labelPOV0.Text = pov(0).ToString()
            labelPOV1.Text = pov(1).ToString()
            labelPOV2.Text = pov(2).ToString()
            labelPOV3.Text = pov(3).ToString()

            ' Fill up text with which buttons are pressed
            Dim buttons As Byte() = state.GetButtons()

            Dim button As Integer = 0
            Dim b As Byte
            For Each b In buttons
                If 0 <> (b And &H80) Then
                    strText += button.ToString("00 ")
                End If
                button += 1
            Next b
            labelButtons.Text = strText
        End Sub 'UpdateUI


        Public Sub UpdateControls(ByVal d As DeviceObjectInstance)
            ' Set the UI to reflect what objects the joystick supports.
            If ObjectTypeGuid.XAxis.Equals(d.ObjectType) Then
                labelXAxis.Enabled = True
                labelXAxisText.Enabled = True
            End If
            If ObjectTypeGuid.YAxis.Equals(d.ObjectType) Then
                labelYAxis.Enabled = True
                labelYAxisText.Enabled = True
            End If
            If ObjectTypeGuid.ZAxis.Equals(d.ObjectType) Then
                labelZAxis.Enabled = True
                labelZAxisText.Enabled = True
            End If
            If ObjectTypeGuid.RxAxis.Equals(d.ObjectType) Then
                labelXRotation.Enabled = True
                labelXRotationText.Enabled = True
            End If
            If ObjectTypeGuid.RyAxis.Equals(d.ObjectType) Then
                labelYRotation.Enabled = True
                labelYRotationText.Enabled = True
            End If
            If ObjectTypeGuid.RzAxis.Equals(d.ObjectType) Then
                labelZRotation.Enabled = True
                labelZRotationText.Enabled = True
            End If
            If ObjectTypeGuid.Slider.Equals(d.ObjectType) Then

                SliderCount += 1

                Select Case SliderCount
                    Case 0
                        labelSlider0.Enabled = True
                        labelSlider0Text.Enabled = True

                    Case 1
                        labelSlider1.Enabled = True
                        labelSlider1Text.Enabled = True
                End Select
            End If
            If ObjectTypeGuid.PointOfView.Equals(d.ObjectType) Then
                numPOVs += 1
                Select Case numPOVs
                    Case 0
                        labelPOV0.Enabled = True
                        labelPOV0Text.Enabled = True

                    Case 1
                        labelPOV1.Enabled = True
                        labelPOV1Text.Enabled = True

                    Case 2
                        labelPOV2.Enabled = True
                        labelPOV2Text.Enabled = True

                    Case 3
                        labelPOV3.Enabled = True
                        labelPOV3Text.Enabled = True
                End Select
            End If
        End Sub 'UpdateControls


        Private Sub MainClass_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
            If Not InitDirectInput() Then
                Close()
            End If
            timer1.Start()
        End Sub 'MainClass_Load


        Private Sub timer1_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles timer1.Tick
            GetData()
        End Sub 'timer1_Tick
    End Class 'MainClass
End Namespace 'Joystick