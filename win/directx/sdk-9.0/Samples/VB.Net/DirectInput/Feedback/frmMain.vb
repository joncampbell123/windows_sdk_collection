Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectInput

Namespace Feedback
    '/ <summary>
    '/ Summary description for frmMain.
    '/ </summary>
    Public Class frmMain
        Inherits System.Windows.Forms.Form
        'This structure will contain information about an effect,
        'its Effect structure, and the DirectInputEffect object.
        Private Structure EffectDescription
            Public info As EffectInformation
            Public effectSelected As EffectObject

            Public Overrides Function ToString() As String
                Return info.Name
            End Function 'ToString
        End Structure 'EffectDescription

        Private applicationDevice As Device 'DirectInput device object.
        Private effectSelected As EffectObject 'The currently selected effect.
        Private axis() As Integer 'Holds the FF axes offsets.
        Private isChanging As Boolean ' Flag that is set when that app is changing control values.
        Friend Label1 As System.Windows.Forms.Label
        Friend gbGeneralParams As System.Windows.Forms.GroupBox '
        Friend WithEvents GeneralPeriod As System.Windows.Forms.TrackBar
        Friend GeneralPeriodLabel As System.Windows.Forms.Label
        Friend WithEvents GeneralGain As System.Windows.Forms.TrackBar
        Friend GeneralGainLabel As System.Windows.Forms.Label
        Friend WithEvents GeneralDuration As System.Windows.Forms.TrackBar
        Friend GeneralDurationLabel As System.Windows.Forms.Label
        Friend WithEvents lstEffects As System.Windows.Forms.ListBox
        Friend gbTypeContainer As System.Windows.Forms.GroupBox
        Friend GroupPeriodForce As System.Windows.Forms.GroupBox
        Friend PeriodicPeriodLabel As System.Windows.Forms.Label
        Friend WithEvents PeriodicPeriod As System.Windows.Forms.TrackBar
        Friend PeriodicPhaseLabel As System.Windows.Forms.Label
        Friend WithEvents PeriodicPhase As System.Windows.Forms.TrackBar
        Friend PeriodicOffsetLabel As System.Windows.Forms.Label
        Friend WithEvents PeriodicOffset As System.Windows.Forms.TrackBar
        Friend PeriodicMagnitudeLabel As System.Windows.Forms.Label
        Friend WithEvents PeriodicMagnitude As System.Windows.Forms.TrackBar
        Friend GroupConstantForce As System.Windows.Forms.GroupBox
        Friend Magnitude As System.Windows.Forms.Label
        Friend WithEvents ConstantForceMagnitude As System.Windows.Forms.TrackBar
        Friend GroupConditionalForce As System.Windows.Forms.GroupBox
        Friend WithEvents rbConditionalAxis2 As System.Windows.Forms.RadioButton
        Friend WithEvents ConditionalAxis1 As System.Windows.Forms.RadioButton
        Friend ConditionalPositiveSaturationLabel As System.Windows.Forms.Label
        Friend WithEvents ConditionalPositiveSaturation As System.Windows.Forms.TrackBar
        Friend ConditionalNegativeSaturationLabel As System.Windows.Forms.Label
        Friend WithEvents ConditionalNegativeSaturation As System.Windows.Forms.TrackBar
        Friend ConditionalPositiveCoefficientLabel As System.Windows.Forms.Label
        Friend WithEvents ConditionalPositiveCoefficient As System.Windows.Forms.TrackBar
        Friend ConditionalNegativeCoeffcientLabel As System.Windows.Forms.Label
        Friend WithEvents ConditionalNegativeCoeffcient As System.Windows.Forms.TrackBar
        Friend ConditionalOffsetLabel As System.Windows.Forms.Label
        Friend WithEvents ConditionalOffset As System.Windows.Forms.TrackBar
        Friend ConditionalDeadBandLabel As System.Windows.Forms.Label
        Friend WithEvents ConditionalDeadBand As System.Windows.Forms.TrackBar
        Friend GroupRampForce As System.Windows.Forms.GroupBox
        Friend RangeEndLabel As System.Windows.Forms.Label
        Friend WithEvents RangeEnd As System.Windows.Forms.TrackBar
        Friend RangeStartLabel As System.Windows.Forms.Label
        Friend WithEvents RangeStart As System.Windows.Forms.TrackBar
        Friend EnvelopeGroupBox As System.Windows.Forms.GroupBox
        Friend WithEvents EnvelopeFadeTime As System.Windows.Forms.TrackBar
        Friend EnvelopeFadeTimeLabel As System.Windows.Forms.Label
        Friend WithEvents EnvelopeFadeLevel As System.Windows.Forms.TrackBar
        Friend EnvelopeFadeLevelLabel As System.Windows.Forms.Label
        Friend WithEvents EnvelopeAttackTime As System.Windows.Forms.TrackBar
        Friend EnvelopeAttackTimeLabel As System.Windows.Forms.Label
        Friend WithEvents EnvelopeAttackLevel As System.Windows.Forms.TrackBar
        Friend EnvelopeAttackLevelLabel As System.Windows.Forms.Label
        Friend WithEvents chkUseEnvelope As System.Windows.Forms.CheckBox
        Friend DirectionGroupBox As System.Windows.Forms.GroupBox
        Friend WithEvents NorthEast As System.Windows.Forms.RadioButton
        Friend WithEvents East As System.Windows.Forms.RadioButton
        Friend WithEvents SouthEast As System.Windows.Forms.RadioButton
        Friend WithEvents South As System.Windows.Forms.RadioButton
        Friend WithEvents SouthWest As System.Windows.Forms.RadioButton
        Friend WithEvents West As System.Windows.Forms.RadioButton
        Friend WithEvents NorthWest As System.Windows.Forms.RadioButton
        Friend WithEvents North As System.Windows.Forms.RadioButton
        ' Required designer variable.
        Private components As System.ComponentModel.Container = Nothing

        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

        End Sub 'New

        Private Function InitializeDirectInput() As Boolean
            ' Initializes DirectInput.
            'Enumerate all joysticks that are attached to the system and have FF capabilities
            Dim instanceDevice As DeviceInstance

            Try
                For Each instanceDevice In Manager.GetDevices(DeviceClass.GameControl, EnumDevicesFlags.ForceFeeback Or EnumDevicesFlags.AttachedOnly)
                    applicationDevice = New Device(instanceDevice.InstanceGuid)
                    Dim instanceObject As DeviceObjectInstance
                    For Each instanceObject In applicationDevice.GetObjects(DeviceObjectTypeFlags.Axis)  ' Get info about all the FF axis on the device
                        Dim temp() As Integer

                        If (instanceObject.Flags And CInt(ObjectInstanceFlags.Actuator)) <> 0 Then
                            If Not Nothing Is axis Then
                                temp = New Integer(axis.Length) {}
                                axis.CopyTo(temp, 0)
                                axis = temp
                            Else
                                axis = New Integer(0) {}
                            End If
                            ' Store the offset of each axis.
                            axis((axis.Length - 1)) = instanceObject.Offset
                            ' Don't need to enumerate any more if 2 were found.
                            If 2 = axis.Length Then
                                Exit For
                            End If
                        End If
                    Next instanceObject
                    If axis.Length - 1 >= 1 Then
                        ' Grab any device that contains at least one axis.
                        Exit For
                    Else
                        axis = Nothing
                        applicationDevice.Dispose()
                        applicationDevice = Nothing
                    End If
                Next instanceDevice

                If Nothing Is applicationDevice Then
                    MessageBox.Show("No force feedback device was detected. Sample will now exit.", "No suitable device")
                    Return False
                End If

                'Turn off autocenter
                applicationDevice.Properties.AutoCenter = False

                'Set the format of the device to that of a joystick
                applicationDevice.SetDataFormat(DeviceDataFormat.Joystick)

                'Enumerate all the effects on the device
                Dim ei As EffectInformation
                For Each ei In applicationDevice.GetEffects(EffectType.All)
                    ' Handles the enumeration of effects.
                    Dim effectSelected As EffectObject
                    Dim description As New EffectDescription()
                    Dim eff As Effect

                    If DInputHelper.GetTypeCode(ei.EffectType) = EffectType.CustomForce Then
                        ' Can't create a custom force without info from the hardware vendor, so skip this effect.
                        GoTo ContinueForEach
                    ElseIf DInputHelper.GetTypeCode(ei.EffectType) = EffectType.Periodic Then
                        ' This is to filter out any Periodic effects. There are known
                        ' issues with Periodic effects that will be addressed post-developer preview.
                        GoTo ContinueForEach
                    ElseIf DInputHelper.GetTypeCode(ei.EffectType) = EffectType.Hardware Then
                        If (ei.StaticParams And EffectParameterFlags.TypeSpecificParams) <> 0 Then
                            ' Can't create a hardware force without info from the hardware vendor.
                            GoTo ContinueForEach
                        End If
                    End If
                    ' Fill in some generic values for the effect.
                    eff = FillEffStruct(CType(ei.EffectType, EffectType))

                    ' Create the effect, using the passed in guid.
                    effectSelected = New EffectObject(ei.EffectGuid, eff, applicationDevice)

                    ' Fill in the EffectDescription structure.
                    description.effectSelected = effectSelected
                    description.info = ei

                    ' Add this effect to the listbox, displaying the name of the effect.
                    lstEffects.Items.Add(description)
ContinueForEach:
                Next ei

                If 0 = lstEffects.Items.Count Then
                    ' If this device has no downloadable effects, end the app.
                    MessageBox.Show("This device does not contain any downloadable effects, app will exit.")
                End If ' The app will validate all DirectInput objects in the frmMain_Load() event.
                ' When one is found missing, this will cause the app to exit.

                ' Set the cooperative level of the device as an exclusive
                ' background device, and attach it to the form's window handle.
                applicationDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.Foreground Or CooperativeLevelFlags.Exclusive)

                ' Make the first index of the listbox selected
                lstEffects.SelectedIndex = 0
                Return True
            Catch e As InputException
                ' If the app can't create a FF device, display this message.
                MessageBox.Show("Unable to initialize DirectInput")
                Return False
            End Try
        End Function 'InitializeDirectInput

        Private Function FillEffStruct(ByVal eif As EffectType) As Effect
            ' Fills in generic values in an effect struct.
            Dim eff As New Effect()

            eff.SetAxes(New Integer(axis.Length - 1) {})
            eff.SetDirection(New Integer(axis.Length - 1) {})
            eff.EffectType = eif
            eff.ConditionStruct = New Condition(axis.Length - 1) {}
            eff.Duration = DI.Infinite
            eff.Gain = 10000
            eff.SamplePeriod = 0
            eff.TriggerButton = Microsoft.DirectX.DirectInput.Button.NoTrigger
            eff.TriggerRepeatInterval = DI.Infinite
            eff.Flags = EffectFlags.ObjectOffsets Or EffectFlags.Cartesian
            eff.SetAxes(axis)

            Return eff
        End Function 'FillEffStruct

        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub 'Dispose

        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        Private Sub InitializeComponent()
            Me.Label1 = New System.Windows.Forms.Label()
            Me.gbGeneralParams = New System.Windows.Forms.GroupBox()
            Me.GeneralPeriod = New System.Windows.Forms.TrackBar()
            Me.GeneralPeriodLabel = New System.Windows.Forms.Label()
            Me.GeneralGain = New System.Windows.Forms.TrackBar()
            Me.GeneralGainLabel = New System.Windows.Forms.Label()
            Me.GeneralDuration = New System.Windows.Forms.TrackBar()
            Me.GeneralDurationLabel = New System.Windows.Forms.Label()
            Me.lstEffects = New System.Windows.Forms.ListBox()
            Me.gbTypeContainer = New System.Windows.Forms.GroupBox()
            Me.GroupConditionalForce = New System.Windows.Forms.GroupBox()
            Me.rbConditionalAxis2 = New System.Windows.Forms.RadioButton()
            Me.ConditionalAxis1 = New System.Windows.Forms.RadioButton()
            Me.ConditionalPositiveSaturationLabel = New System.Windows.Forms.Label()
            Me.ConditionalPositiveSaturation = New System.Windows.Forms.TrackBar()
            Me.ConditionalNegativeSaturationLabel = New System.Windows.Forms.Label()
            Me.ConditionalNegativeSaturation = New System.Windows.Forms.TrackBar()
            Me.ConditionalPositiveCoefficientLabel = New System.Windows.Forms.Label()
            Me.ConditionalPositiveCoefficient = New System.Windows.Forms.TrackBar()
            Me.ConditionalNegativeCoeffcientLabel = New System.Windows.Forms.Label()
            Me.ConditionalNegativeCoeffcient = New System.Windows.Forms.TrackBar()
            Me.ConditionalOffsetLabel = New System.Windows.Forms.Label()
            Me.ConditionalOffset = New System.Windows.Forms.TrackBar()
            Me.ConditionalDeadBandLabel = New System.Windows.Forms.Label()
            Me.ConditionalDeadBand = New System.Windows.Forms.TrackBar()
            Me.GroupPeriodForce = New System.Windows.Forms.GroupBox()
            Me.PeriodicPeriodLabel = New System.Windows.Forms.Label()
            Me.PeriodicPeriod = New System.Windows.Forms.TrackBar()
            Me.PeriodicPhaseLabel = New System.Windows.Forms.Label()
            Me.PeriodicPhase = New System.Windows.Forms.TrackBar()
            Me.PeriodicOffsetLabel = New System.Windows.Forms.Label()
            Me.PeriodicOffset = New System.Windows.Forms.TrackBar()
            Me.PeriodicMagnitudeLabel = New System.Windows.Forms.Label()
            Me.PeriodicMagnitude = New System.Windows.Forms.TrackBar()
            Me.GroupRampForce = New System.Windows.Forms.GroupBox()
            Me.RangeEndLabel = New System.Windows.Forms.Label()
            Me.RangeEnd = New System.Windows.Forms.TrackBar()
            Me.RangeStartLabel = New System.Windows.Forms.Label()
            Me.RangeStart = New System.Windows.Forms.TrackBar()
            Me.GroupConstantForce = New System.Windows.Forms.GroupBox()
            Me.Magnitude = New System.Windows.Forms.Label()
            Me.ConstantForceMagnitude = New System.Windows.Forms.TrackBar()
            Me.EnvelopeGroupBox = New System.Windows.Forms.GroupBox()
            Me.EnvelopeFadeTime = New System.Windows.Forms.TrackBar()
            Me.EnvelopeFadeTimeLabel = New System.Windows.Forms.Label()
            Me.EnvelopeFadeLevel = New System.Windows.Forms.TrackBar()
            Me.EnvelopeFadeLevelLabel = New System.Windows.Forms.Label()
            Me.EnvelopeAttackTime = New System.Windows.Forms.TrackBar()
            Me.EnvelopeAttackTimeLabel = New System.Windows.Forms.Label()
            Me.EnvelopeAttackLevel = New System.Windows.Forms.TrackBar()
            Me.EnvelopeAttackLevelLabel = New System.Windows.Forms.Label()
            Me.chkUseEnvelope = New System.Windows.Forms.CheckBox()
            Me.DirectionGroupBox = New System.Windows.Forms.GroupBox()
            Me.NorthEast = New System.Windows.Forms.RadioButton()
            Me.East = New System.Windows.Forms.RadioButton()
            Me.SouthEast = New System.Windows.Forms.RadioButton()
            Me.South = New System.Windows.Forms.RadioButton()
            Me.SouthWest = New System.Windows.Forms.RadioButton()
            Me.West = New System.Windows.Forms.RadioButton()
            Me.NorthWest = New System.Windows.Forms.RadioButton()
            Me.North = New System.Windows.Forms.RadioButton()
            Me.gbGeneralParams.SuspendLayout()
            CType(Me.GeneralPeriod, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.GeneralGain, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.GeneralDuration, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.gbTypeContainer.SuspendLayout()
            Me.GroupConditionalForce.SuspendLayout()
            CType(Me.ConditionalPositiveSaturation, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.ConditionalNegativeSaturation, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.ConditionalPositiveCoefficient, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.ConditionalNegativeCoeffcient, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.ConditionalOffset, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.ConditionalDeadBand, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.GroupPeriodForce.SuspendLayout()
            CType(Me.PeriodicPeriod, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.PeriodicPhase, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.PeriodicOffset, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.PeriodicMagnitude, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.GroupRampForce.SuspendLayout()
            CType(Me.RangeEnd, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.RangeStart, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.GroupConstantForce.SuspendLayout()
            CType(Me.ConstantForceMagnitude, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.EnvelopeGroupBox.SuspendLayout()
            CType(Me.EnvelopeFadeTime, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.EnvelopeFadeLevel, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.EnvelopeAttackTime, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.EnvelopeAttackLevel, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.DirectionGroupBox.SuspendLayout()
            Me.SuspendLayout()
            ' 
            ' Label1
            ' 
            Me.Label1.AutoSize = True
            Me.Label1.Location = New System.Drawing.Point(6, 6)
            Me.Label1.Name = "Label1"
            Me.Label1.Size = New System.Drawing.Size(91, 13)
            Me.Label1.TabIndex = 7
            Me.Label1.Text = "Available Effects:"
            ' 
            ' gbGeneralParams
            ' 
            Me.gbGeneralParams.Controls.AddRange(New System.Windows.Forms.Control() {Me.GeneralPeriod, Me.GeneralPeriodLabel, Me.GeneralGain, Me.GeneralGainLabel, Me.GeneralDuration, Me.GeneralDurationLabel})
            Me.gbGeneralParams.Location = New System.Drawing.Point(6, 262)
            Me.gbGeneralParams.Name = "gbGeneralParams"
            Me.gbGeneralParams.Size = New System.Drawing.Size(224, 216)
            Me.gbGeneralParams.TabIndex = 11
            Me.gbGeneralParams.TabStop = False
            Me.gbGeneralParams.Text = "General Parameters"
            ' 
            ' GeneralPeriod
            ' 
            Me.GeneralPeriod.AutoSize = False
            Me.GeneralPeriod.Location = New System.Drawing.Point(8, 152)
            Me.GeneralPeriod.Maximum = 100000
            Me.GeneralPeriod.Name = "GeneralPeriod"
            Me.GeneralPeriod.Size = New System.Drawing.Size(208, 42)
            Me.GeneralPeriod.TabIndex = 5
            Me.GeneralPeriod.TickFrequency = 5000
            ' 
            ' GeneralPeriodLabel
            ' 
            Me.GeneralPeriodLabel.Location = New System.Drawing.Point(16, 136)
            Me.GeneralPeriodLabel.Name = "GeneralPeriodLabel"
            Me.GeneralPeriodLabel.Size = New System.Drawing.Size(192, 16)
            Me.GeneralPeriodLabel.TabIndex = 4
            Me.GeneralPeriodLabel.Text = "Sample Period: Default"
            ' 
            ' GeneralGain
            ' 
            Me.GeneralGain.AutoSize = False
            Me.GeneralGain.Location = New System.Drawing.Point(8, 96)
            Me.GeneralGain.Maximum = 10000
            Me.GeneralGain.Name = "GeneralGain"
            Me.GeneralGain.Size = New System.Drawing.Size(208, 42)
            Me.GeneralGain.TabIndex = 3
            Me.GeneralGain.TickFrequency = 1000
            Me.GeneralGain.Value = 10000
            ' 
            ' GeneralGainLabel
            ' 
            Me.GeneralGainLabel.Location = New System.Drawing.Point(16, 84)
            Me.GeneralGainLabel.Name = "GeneralGainLabel"
            Me.GeneralGainLabel.Size = New System.Drawing.Size(192, 16)
            Me.GeneralGainLabel.TabIndex = 2
            Me.GeneralGainLabel.Text = "Effect Gain: 10000"
            ' 
            ' GeneralDuration
            ' 
            Me.GeneralDuration.AutoSize = False
            Me.GeneralDuration.LargeChange = 2
            Me.GeneralDuration.Location = New System.Drawing.Point(8, 48)
            Me.GeneralDuration.Minimum = 1
            Me.GeneralDuration.Name = "GeneralDuration"
            Me.GeneralDuration.Size = New System.Drawing.Size(208, 42)
            Me.GeneralDuration.TabIndex = 1
            Me.GeneralDuration.Value = 10
            ' 
            ' GeneralDurationLabel
            ' 
            Me.GeneralDurationLabel.Location = New System.Drawing.Point(16, 32)
            Me.GeneralDurationLabel.Name = "GeneralDurationLabel"
            Me.GeneralDurationLabel.Size = New System.Drawing.Size(192, 16)
            Me.GeneralDurationLabel.TabIndex = 0
            Me.GeneralDurationLabel.Text = "Effect Duration: Infinite"
            ' 
            ' lstEffects
            ' 
            Me.lstEffects.Location = New System.Drawing.Point(6, 22)
            Me.lstEffects.Name = "lstEffects"
            Me.lstEffects.Size = New System.Drawing.Size(225, 225)
            Me.lstEffects.TabIndex = 6
            ' 
            ' gbTypeContainer
            ' 
            Me.gbTypeContainer.Controls.AddRange(New System.Windows.Forms.Control() {Me.GroupConditionalForce, Me.GroupPeriodForce, Me.GroupRampForce, Me.GroupConstantForce})
            Me.gbTypeContainer.Location = New System.Drawing.Point(246, 14)
            Me.gbTypeContainer.Name = "gbTypeContainer"
            Me.gbTypeContainer.Size = New System.Drawing.Size(361, 233)
            Me.gbTypeContainer.TabIndex = 8
            Me.gbTypeContainer.TabStop = False
            Me.gbTypeContainer.Text = "Type-Specific Parameters"
            ' 
            ' GroupConditionalForce
            ' 
            Me.GroupConditionalForce.Controls.AddRange(New System.Windows.Forms.Control() {Me.rbConditionalAxis2, Me.ConditionalAxis1, Me.ConditionalPositiveSaturationLabel, Me.ConditionalPositiveSaturation, Me.ConditionalNegativeSaturationLabel, Me.ConditionalNegativeSaturation, Me.ConditionalPositiveCoefficientLabel, Me.ConditionalPositiveCoefficient, Me.ConditionalNegativeCoeffcientLabel, Me.ConditionalNegativeCoeffcient, Me.ConditionalOffsetLabel, Me.ConditionalOffset, Me.ConditionalDeadBandLabel, Me.ConditionalDeadBand})
            Me.GroupConditionalForce.Location = New System.Drawing.Point(8, 16)
            Me.GroupConditionalForce.Name = "GroupConditionalForce"
            Me.GroupConditionalForce.Size = New System.Drawing.Size(344, 200)
            Me.GroupConditionalForce.TabIndex = 3
            Me.GroupConditionalForce.TabStop = False
            Me.GroupConditionalForce.Tag = ""
            Me.GroupConditionalForce.Text = "Conditional Force"
            Me.GroupConditionalForce.Visible = False
            ' 
            ' rbConditionalAxis2
            ' 
            Me.rbConditionalAxis2.Location = New System.Drawing.Point(160, 176)
            Me.rbConditionalAxis2.Name = "rbConditionalAxis2"
            Me.rbConditionalAxis2.Size = New System.Drawing.Size(56, 16)
            Me.rbConditionalAxis2.TabIndex = 13
            Me.rbConditionalAxis2.Text = "Axis 2"
            ' 
            ' ConditionalAxis1
            ' 
            Me.ConditionalAxis1.Checked = True
            Me.ConditionalAxis1.Location = New System.Drawing.Point(160, 160)
            Me.ConditionalAxis1.Name = "ConditionalAxis1"
            Me.ConditionalAxis1.Size = New System.Drawing.Size(56, 16)
            Me.ConditionalAxis1.TabIndex = 12
            Me.ConditionalAxis1.TabStop = True
            Me.ConditionalAxis1.Text = "Axis 1"
            ' 
            ' ConditionalPositiveSaturationLabel
            ' 
            Me.ConditionalPositiveSaturationLabel.AutoSize = True
            Me.ConditionalPositiveSaturationLabel.Location = New System.Drawing.Point(192, 120)
            Me.ConditionalPositiveSaturationLabel.Name = "ConditionalPositiveSaturationLabel"
            Me.ConditionalPositiveSaturationLabel.Size = New System.Drawing.Size(136, 13)
            Me.ConditionalPositiveSaturationLabel.TabIndex = 11
            Me.ConditionalPositiveSaturationLabel.Text = "Positive Saturation: 10000"
            ' 
            ' ConditionalPositiveSaturation
            ' 
            Me.ConditionalPositiveSaturation.AutoSize = False
            Me.ConditionalPositiveSaturation.LargeChange = 1000
            Me.ConditionalPositiveSaturation.Location = New System.Drawing.Point(192, 136)
            Me.ConditionalPositiveSaturation.Maximum = 10000
            Me.ConditionalPositiveSaturation.Name = "ConditionalPositiveSaturation"
            Me.ConditionalPositiveSaturation.Size = New System.Drawing.Size(136, 42)
            Me.ConditionalPositiveSaturation.SmallChange = 100
            Me.ConditionalPositiveSaturation.TabIndex = 10
            Me.ConditionalPositiveSaturation.TickFrequency = 1000
            Me.ConditionalPositiveSaturation.Value = 10000
            ' 
            ' ConditionalNegativeSaturationLabel
            ' 
            Me.ConditionalNegativeSaturationLabel.AutoSize = True
            Me.ConditionalNegativeSaturationLabel.Location = New System.Drawing.Point(24, 120)
            Me.ConditionalNegativeSaturationLabel.Name = "ConditionalNegativeSaturationLabel"
            Me.ConditionalNegativeSaturationLabel.Size = New System.Drawing.Size(141, 13)
            Me.ConditionalNegativeSaturationLabel.TabIndex = 9
            Me.ConditionalNegativeSaturationLabel.Text = "Negative Saturation: 10000"
            ' 
            ' ConditionalNegativeSaturation
            ' 
            Me.ConditionalNegativeSaturation.AutoSize = False
            Me.ConditionalNegativeSaturation.LargeChange = 1000
            Me.ConditionalNegativeSaturation.Location = New System.Drawing.Point(24, 136)
            Me.ConditionalNegativeSaturation.Maximum = 10000
            Me.ConditionalNegativeSaturation.Name = "ConditionalNegativeSaturation"
            Me.ConditionalNegativeSaturation.Size = New System.Drawing.Size(136, 42)
            Me.ConditionalNegativeSaturation.SmallChange = 100
            Me.ConditionalNegativeSaturation.TabIndex = 8
            Me.ConditionalNegativeSaturation.TickFrequency = 1000
            Me.ConditionalNegativeSaturation.Value = 10000
            ' 
            ' ConditionalPositiveCoefficientLabel
            ' 
            Me.ConditionalPositiveCoefficientLabel.AutoSize = True
            Me.ConditionalPositiveCoefficientLabel.Location = New System.Drawing.Point(192, 72)
            Me.ConditionalPositiveCoefficientLabel.Name = "ConditionalPositiveCoefficientLabel"
            Me.ConditionalPositiveCoefficientLabel.Size = New System.Drawing.Size(113, 13)
            Me.ConditionalPositiveCoefficientLabel.TabIndex = 7
            Me.ConditionalPositiveCoefficientLabel.Text = "Positive Coefficient: 0"
            ' 
            ' ConditionalPositiveCoefficient
            ' 
            Me.ConditionalPositiveCoefficient.AutoSize = False
            Me.ConditionalPositiveCoefficient.LargeChange = 1000
            Me.ConditionalPositiveCoefficient.Location = New System.Drawing.Point(192, 88)
            Me.ConditionalPositiveCoefficient.Maximum = 10000
            Me.ConditionalPositiveCoefficient.Minimum = -10000
            Me.ConditionalPositiveCoefficient.Name = "ConditionalPositiveCoefficient"
            Me.ConditionalPositiveCoefficient.Size = New System.Drawing.Size(136, 42)
            Me.ConditionalPositiveCoefficient.SmallChange = 100
            Me.ConditionalPositiveCoefficient.TabIndex = 6
            Me.ConditionalPositiveCoefficient.TickFrequency = 1000
            ' 
            ' ConditionalNegativeCoeffcientLabel
            ' 
            Me.ConditionalNegativeCoeffcientLabel.AutoSize = True
            Me.ConditionalNegativeCoeffcientLabel.Location = New System.Drawing.Point(24, 72)
            Me.ConditionalNegativeCoeffcientLabel.Name = "ConditionalNegativeCoeffcientLabel"
            Me.ConditionalNegativeCoeffcientLabel.Size = New System.Drawing.Size(118, 13)
            Me.ConditionalNegativeCoeffcientLabel.TabIndex = 5
            Me.ConditionalNegativeCoeffcientLabel.Text = "Negative Coefficient: 0"
            ' 
            ' ConditionalNegativeCoeffcient
            ' 
            Me.ConditionalNegativeCoeffcient.AutoSize = False
            Me.ConditionalNegativeCoeffcient.LargeChange = 1000
            Me.ConditionalNegativeCoeffcient.Location = New System.Drawing.Point(24, 88)
            Me.ConditionalNegativeCoeffcient.Maximum = 10000
            Me.ConditionalNegativeCoeffcient.Minimum = -10000
            Me.ConditionalNegativeCoeffcient.Name = "ConditionalNegativeCoeffcient"
            Me.ConditionalNegativeCoeffcient.Size = New System.Drawing.Size(136, 42)
            Me.ConditionalNegativeCoeffcient.SmallChange = 100
            Me.ConditionalNegativeCoeffcient.TabIndex = 4
            Me.ConditionalNegativeCoeffcient.TickFrequency = 1000
            ' 
            ' ConditionalOffsetLabel
            ' 
            Me.ConditionalOffsetLabel.AutoSize = True
            Me.ConditionalOffsetLabel.Location = New System.Drawing.Point(192, 24)
            Me.ConditionalOffsetLabel.Name = "ConditionalOffsetLabel"
            Me.ConditionalOffsetLabel.Size = New System.Drawing.Size(47, 13)
            Me.ConditionalOffsetLabel.TabIndex = 3
            Me.ConditionalOffsetLabel.Text = "Offset: 0"
            ' 
            ' ConditionalOffset
            ' 
            Me.ConditionalOffset.AutoSize = False
            Me.ConditionalOffset.LargeChange = 1000
            Me.ConditionalOffset.Location = New System.Drawing.Point(192, 40)
            Me.ConditionalOffset.Maximum = 10000
            Me.ConditionalOffset.Minimum = -10000
            Me.ConditionalOffset.Name = "ConditionalOffset"
            Me.ConditionalOffset.Size = New System.Drawing.Size(136, 42)
            Me.ConditionalOffset.SmallChange = 100
            Me.ConditionalOffset.TabIndex = 2
            Me.ConditionalOffset.TickFrequency = 1000
            ' 
            ' ConditionalDeadBandLabel
            ' 
            Me.ConditionalDeadBandLabel.AutoSize = True
            Me.ConditionalDeadBandLabel.Location = New System.Drawing.Point(24, 24)
            Me.ConditionalDeadBandLabel.Name = "ConditionalDeadBandLabel"
            Me.ConditionalDeadBandLabel.Size = New System.Drawing.Size(73, 13)
            Me.ConditionalDeadBandLabel.TabIndex = 1
            Me.ConditionalDeadBandLabel.Text = "Dead Band: 0"
            ' 
            ' ConditionalDeadBand
            ' 
            Me.ConditionalDeadBand.AutoSize = False
            Me.ConditionalDeadBand.LargeChange = 1000
            Me.ConditionalDeadBand.Location = New System.Drawing.Point(24, 40)
            Me.ConditionalDeadBand.Maximum = 10000
            Me.ConditionalDeadBand.Name = "ConditionalDeadBand"
            Me.ConditionalDeadBand.Size = New System.Drawing.Size(136, 42)
            Me.ConditionalDeadBand.SmallChange = 100
            Me.ConditionalDeadBand.TabIndex = 0
            Me.ConditionalDeadBand.TickFrequency = 1000
            ' 
            ' GroupPeriodForce
            ' 
            Me.GroupPeriodForce.Controls.AddRange(New System.Windows.Forms.Control() {Me.PeriodicPeriodLabel, Me.PeriodicPeriod, Me.PeriodicPhaseLabel, Me.PeriodicPhase, Me.PeriodicOffsetLabel, Me.PeriodicOffset, Me.PeriodicMagnitudeLabel, Me.PeriodicMagnitude})
            Me.GroupPeriodForce.Location = New System.Drawing.Point(8, 16)
            Me.GroupPeriodForce.Name = "GroupPeriodForce"
            Me.GroupPeriodForce.Size = New System.Drawing.Size(344, 200)
            Me.GroupPeriodForce.TabIndex = 2
            Me.GroupPeriodForce.TabStop = False
            Me.GroupPeriodForce.Tag = ""
            Me.GroupPeriodForce.Text = "Periodic Force"
            Me.GroupPeriodForce.Visible = False
            ' 
            ' PeriodicPeriodLabel
            ' 
            Me.PeriodicPeriodLabel.AutoSize = True
            Me.PeriodicPeriodLabel.Location = New System.Drawing.Point(96, 142)
            Me.PeriodicPeriodLabel.Name = "PeriodicPeriodLabel"
            Me.PeriodicPeriodLabel.Size = New System.Drawing.Size(49, 13)
            Me.PeriodicPeriodLabel.TabIndex = 7
            Me.PeriodicPeriodLabel.Text = "Period: 0"
            ' 
            ' PeriodicPeriod
            ' 
            Me.PeriodicPeriod.LargeChange = 1000
            Me.PeriodicPeriod.Location = New System.Drawing.Point(24, 153)
            Me.PeriodicPeriod.Maximum = 500000
            Me.PeriodicPeriod.Name = "PeriodicPeriod"
            Me.PeriodicPeriod.Size = New System.Drawing.Size(304, 42)
            Me.PeriodicPeriod.SmallChange = 100
            Me.PeriodicPeriod.TabIndex = 6
            Me.PeriodicPeriod.TickFrequency = 20000
            ' 
            ' PeriodicPhaseLabel
            ' 
            Me.PeriodicPhaseLabel.AutoSize = True
            Me.PeriodicPhaseLabel.Location = New System.Drawing.Point(96, 100)
            Me.PeriodicPhaseLabel.Name = "PeriodicPhaseLabel"
            Me.PeriodicPhaseLabel.Size = New System.Drawing.Size(49, 13)
            Me.PeriodicPhaseLabel.TabIndex = 5
            Me.PeriodicPhaseLabel.Text = "Phase: 0"
            ' 
            ' PeriodicPhase
            ' 
            Me.PeriodicPhase.LargeChange = 100
            Me.PeriodicPhase.Location = New System.Drawing.Point(24, 110)
            Me.PeriodicPhase.Maximum = 35999
            Me.PeriodicPhase.Name = "PeriodicPhase"
            Me.PeriodicPhase.Size = New System.Drawing.Size(304, 42)
            Me.PeriodicPhase.SmallChange = 10
            Me.PeriodicPhase.TabIndex = 4
            Me.PeriodicPhase.TickFrequency = 1000
            ' 
            ' PeriodicOffsetLabel
            ' 
            Me.PeriodicOffsetLabel.AutoSize = True
            Me.PeriodicOffsetLabel.Location = New System.Drawing.Point(96, 58)
            Me.PeriodicOffsetLabel.Name = "PeriodicOffsetLabel"
            Me.PeriodicOffsetLabel.Size = New System.Drawing.Size(47, 13)
            Me.PeriodicOffsetLabel.TabIndex = 3
            Me.PeriodicOffsetLabel.Text = "Offset: 0"
            ' 
            ' PeriodicOffset
            ' 
            Me.PeriodicOffset.LargeChange = 100
            Me.PeriodicOffset.Location = New System.Drawing.Point(24, 67)
            Me.PeriodicOffset.Maximum = 10000
            Me.PeriodicOffset.Minimum = -10000
            Me.PeriodicOffset.Name = "PeriodicOffset"
            Me.PeriodicOffset.Size = New System.Drawing.Size(304, 42)
            Me.PeriodicOffset.SmallChange = 10
            Me.PeriodicOffset.TabIndex = 2
            Me.PeriodicOffset.TickFrequency = 1000
            ' 
            ' PeriodicMagnitudeLabel
            ' 
            Me.PeriodicMagnitudeLabel.AutoSize = True
            Me.PeriodicMagnitudeLabel.Location = New System.Drawing.Point(96, 16)
            Me.PeriodicMagnitudeLabel.Name = "PeriodicMagnitudeLabel"
            Me.PeriodicMagnitudeLabel.Size = New System.Drawing.Size(70, 13)
            Me.PeriodicMagnitudeLabel.TabIndex = 1
            Me.PeriodicMagnitudeLabel.Text = "Magnitude: 0"
            ' 
            ' PeriodicMagnitude
            ' 
            Me.PeriodicMagnitude.LargeChange = 1000
            Me.PeriodicMagnitude.Location = New System.Drawing.Point(24, 24)
            Me.PeriodicMagnitude.Maximum = 10000
            Me.PeriodicMagnitude.Name = "PeriodicMagnitude"
            Me.PeriodicMagnitude.Size = New System.Drawing.Size(304, 42)
            Me.PeriodicMagnitude.SmallChange = 100
            Me.PeriodicMagnitude.TabIndex = 0
            Me.PeriodicMagnitude.TickFrequency = 1000
            Me.PeriodicMagnitude.Value = 5000
            ' 
            ' GroupRampForce
            ' 
            Me.GroupRampForce.Controls.AddRange(New System.Windows.Forms.Control() {Me.RangeEndLabel, Me.RangeEnd, Me.RangeStartLabel, Me.RangeStart})
            Me.GroupRampForce.Location = New System.Drawing.Point(8, 16)
            Me.GroupRampForce.Name = "GroupRampForce"
            Me.GroupRampForce.Size = New System.Drawing.Size(344, 200)
            Me.GroupRampForce.TabIndex = 1
            Me.GroupRampForce.TabStop = False
            Me.GroupRampForce.Tag = ""
            Me.GroupRampForce.Text = "Ramp Force"
            Me.GroupRampForce.Visible = False
            ' 
            ' RangeEndLabel
            ' 
            Me.RangeEndLabel.AutoSize = True
            Me.RangeEndLabel.Location = New System.Drawing.Point(93, 120)
            Me.RangeEndLabel.Name = "RangeEndLabel"
            Me.RangeEndLabel.Size = New System.Drawing.Size(73, 13)
            Me.RangeEndLabel.TabIndex = 3
            Me.RangeEndLabel.Text = "Range End: 0"
            ' 
            ' RangeEnd
            ' 
            Me.RangeEnd.LargeChange = 100
            Me.RangeEnd.Location = New System.Drawing.Point(28, 144)
            Me.RangeEnd.Maximum = 10000
            Me.RangeEnd.Minimum = -10000
            Me.RangeEnd.Name = "RangeEnd"
            Me.RangeEnd.Size = New System.Drawing.Size(304, 42)
            Me.RangeEnd.SmallChange = 10
            Me.RangeEnd.TabIndex = 2
            Me.RangeEnd.TickFrequency = 1000
            ' 
            ' RangeStartLabel
            ' 
            Me.RangeStartLabel.AutoSize = True
            Me.RangeStartLabel.Location = New System.Drawing.Point(93, 24)
            Me.RangeStartLabel.Name = "RangeStartLabel"
            Me.RangeStartLabel.Size = New System.Drawing.Size(77, 13)
            Me.RangeStartLabel.TabIndex = 1
            Me.RangeStartLabel.Text = "Range Start: 0"
            ' 
            ' RangeStart
            ' 
            Me.RangeStart.LargeChange = 100
            Me.RangeStart.Location = New System.Drawing.Point(28, 48)
            Me.RangeStart.Maximum = 10000
            Me.RangeStart.Minimum = -10000
            Me.RangeStart.Name = "RangeStart"
            Me.RangeStart.Size = New System.Drawing.Size(304, 42)
            Me.RangeStart.SmallChange = 10
            Me.RangeStart.TabIndex = 0
            Me.RangeStart.TickFrequency = 1000
            ' 
            ' GroupConstantForce
            ' 
            Me.GroupConstantForce.Controls.AddRange(New System.Windows.Forms.Control() {Me.Magnitude, Me.ConstantForceMagnitude})
            Me.GroupConstantForce.Location = New System.Drawing.Point(8, 16)
            Me.GroupConstantForce.Name = "GroupConstantForce"
            Me.GroupConstantForce.Size = New System.Drawing.Size(344, 200)
            Me.GroupConstantForce.TabIndex = 0
            Me.GroupConstantForce.TabStop = False
            Me.GroupConstantForce.Tag = ""
            Me.GroupConstantForce.Text = "Constant Force"
            Me.GroupConstantForce.Visible = False
            ' 
            ' Magnitude
            ' 
            Me.Magnitude.AutoSize = True
            Me.Magnitude.Location = New System.Drawing.Point(89, 72)
            Me.Magnitude.Name = "Magnitude"
            Me.Magnitude.Size = New System.Drawing.Size(175, 13)
            Me.Magnitude.TabIndex = 1
            Me.Magnitude.Text = "Constant Force Magnitude: 10000"
            ' 
            ' ConstantForceMagnitude
            ' 
            Me.ConstantForceMagnitude.LargeChange = 100
            Me.ConstantForceMagnitude.Location = New System.Drawing.Point(24, 96)
            Me.ConstantForceMagnitude.Maximum = 10000
            Me.ConstantForceMagnitude.Name = "ConstantForceMagnitude"
            Me.ConstantForceMagnitude.Size = New System.Drawing.Size(304, 42)
            Me.ConstantForceMagnitude.SmallChange = 10
            Me.ConstantForceMagnitude.TabIndex = 0
            Me.ConstantForceMagnitude.TickFrequency = 1000
            Me.ConstantForceMagnitude.Value = 10000
            ' 
            ' EnvelopeGroupBox
            ' 
            Me.EnvelopeGroupBox.Controls.AddRange(New System.Windows.Forms.Control() {Me.EnvelopeFadeTime, Me.EnvelopeFadeTimeLabel, Me.EnvelopeFadeLevel, Me.EnvelopeFadeLevelLabel, Me.EnvelopeAttackTime, Me.EnvelopeAttackTimeLabel, Me.EnvelopeAttackLevel, Me.EnvelopeAttackLevelLabel, Me.chkUseEnvelope})
            Me.EnvelopeGroupBox.Location = New System.Drawing.Point(246, 262)
            Me.EnvelopeGroupBox.Name = "EnvelopeGroupBox"
            Me.EnvelopeGroupBox.Size = New System.Drawing.Size(168, 216)
            Me.EnvelopeGroupBox.TabIndex = 9
            Me.EnvelopeGroupBox.TabStop = False
            Me.EnvelopeGroupBox.Text = "Envelope"
            ' 
            ' EnvelopeFadeTime
            ' 
            Me.EnvelopeFadeTime.AutoSize = False
            Me.EnvelopeFadeTime.LargeChange = 10000
            Me.EnvelopeFadeTime.Location = New System.Drawing.Point(16, 172)
            Me.EnvelopeFadeTime.Maximum = 5000000
            Me.EnvelopeFadeTime.Name = "EnvelopeFadeTime"
            Me.EnvelopeFadeTime.Size = New System.Drawing.Size(144, 42)
            Me.EnvelopeFadeTime.SmallChange = 1000
            Me.EnvelopeFadeTime.TabIndex = 8
            Me.EnvelopeFadeTime.TickFrequency = 1000000
            ' 
            ' EnvelopeFadeTimeLabel
            ' 
            Me.EnvelopeFadeTimeLabel.AutoSize = True
            Me.EnvelopeFadeTimeLabel.Location = New System.Drawing.Point(16, 160)
            Me.EnvelopeFadeTimeLabel.Name = "EnvelopeFadeTimeLabel"
            Me.EnvelopeFadeTimeLabel.Size = New System.Drawing.Size(71, 13)
            Me.EnvelopeFadeTimeLabel.TabIndex = 7
            Me.EnvelopeFadeTimeLabel.Text = "Fade Time: 0"
            ' 
            ' EnvelopeFadeLevel
            ' 
            Me.EnvelopeFadeLevel.AutoSize = False
            Me.EnvelopeFadeLevel.LargeChange = 1000
            Me.EnvelopeFadeLevel.Location = New System.Drawing.Point(16, 128)
            Me.EnvelopeFadeLevel.Maximum = 10000
            Me.EnvelopeFadeLevel.Name = "EnvelopeFadeLevel"
            Me.EnvelopeFadeLevel.Size = New System.Drawing.Size(144, 42)
            Me.EnvelopeFadeLevel.SmallChange = 100
            Me.EnvelopeFadeLevel.TabIndex = 6
            Me.EnvelopeFadeLevel.TickFrequency = 1000
            ' 
            ' EnvelopeFadeLevelLabel
            ' 
            Me.EnvelopeFadeLevelLabel.AutoSize = True
            Me.EnvelopeFadeLevelLabel.Location = New System.Drawing.Point(16, 116)
            Me.EnvelopeFadeLevelLabel.Name = "EnvelopeFadeLevelLabel"
            Me.EnvelopeFadeLevelLabel.Size = New System.Drawing.Size(73, 13)
            Me.EnvelopeFadeLevelLabel.TabIndex = 5
            Me.EnvelopeFadeLevelLabel.Text = "Fade Level: 0"
            ' 
            ' EnvelopeAttackTime
            ' 
            Me.EnvelopeAttackTime.AutoSize = False
            Me.EnvelopeAttackTime.LargeChange = 50000
            Me.EnvelopeAttackTime.Location = New System.Drawing.Point(16, 88)
            Me.EnvelopeAttackTime.Maximum = 5000000
            Me.EnvelopeAttackTime.Name = "EnvelopeAttackTime"
            Me.EnvelopeAttackTime.Size = New System.Drawing.Size(144, 42)
            Me.EnvelopeAttackTime.SmallChange = 1000
            Me.EnvelopeAttackTime.TabIndex = 4
            Me.EnvelopeAttackTime.TickFrequency = 1000000
            ' 
            ' EnvelopeAttackTimeLabel
            ' 
            Me.EnvelopeAttackTimeLabel.AutoSize = True
            Me.EnvelopeAttackTimeLabel.Location = New System.Drawing.Point(16, 76)
            Me.EnvelopeAttackTimeLabel.Name = "EnvelopeAttackTimeLabel"
            Me.EnvelopeAttackTimeLabel.Size = New System.Drawing.Size(76, 13)
            Me.EnvelopeAttackTimeLabel.TabIndex = 3
            Me.EnvelopeAttackTimeLabel.Text = "Attack Time: 0"
            ' 
            ' EnvelopeAttackLevel
            ' 
            Me.EnvelopeAttackLevel.AutoSize = False
            Me.EnvelopeAttackLevel.LargeChange = 1000
            Me.EnvelopeAttackLevel.Location = New System.Drawing.Point(16, 44)
            Me.EnvelopeAttackLevel.Maximum = 10000
            Me.EnvelopeAttackLevel.Name = "EnvelopeAttackLevel"
            Me.EnvelopeAttackLevel.Size = New System.Drawing.Size(144, 42)
            Me.EnvelopeAttackLevel.SmallChange = 100
            Me.EnvelopeAttackLevel.TabIndex = 2
            Me.EnvelopeAttackLevel.TickFrequency = 1000
            ' 
            ' EnvelopeAttackLevelLabel
            ' 
            Me.EnvelopeAttackLevelLabel.AutoSize = True
            Me.EnvelopeAttackLevelLabel.Location = New System.Drawing.Point(16, 32)
            Me.EnvelopeAttackLevelLabel.Name = "EnvelopeAttackLevelLabel"
            Me.EnvelopeAttackLevelLabel.Size = New System.Drawing.Size(78, 13)
            Me.EnvelopeAttackLevelLabel.TabIndex = 1
            Me.EnvelopeAttackLevelLabel.Text = "Attack Level: 0"
            ' 
            ' chkUseEnvelope
            ' 
            Me.chkUseEnvelope.Location = New System.Drawing.Point(16, 12)
            Me.chkUseEnvelope.Name = "chkUseEnvelope"
            Me.chkUseEnvelope.Size = New System.Drawing.Size(96, 24)
            Me.chkUseEnvelope.TabIndex = 0
            Me.chkUseEnvelope.Text = "Use Envelope"
            ' 
            ' DirectionGroupBox
            ' 
            Me.DirectionGroupBox.Controls.AddRange(New System.Windows.Forms.Control() {Me.NorthEast, Me.East, Me.SouthEast, Me.South, Me.SouthWest, Me.West, Me.NorthWest, Me.North})
            Me.DirectionGroupBox.Location = New System.Drawing.Point(438, 262)
            Me.DirectionGroupBox.Name = "DirectionGroupBox"
            Me.DirectionGroupBox.Size = New System.Drawing.Size(168, 216)
            Me.DirectionGroupBox.TabIndex = 10
            Me.DirectionGroupBox.TabStop = False
            Me.DirectionGroupBox.Text = "Direction"
            ' 
            ' NorthEast
            ' 
            Me.NorthEast.Location = New System.Drawing.Point(120, 64)
            Me.NorthEast.Name = "NorthEast"
            Me.NorthEast.Size = New System.Drawing.Size(16, 24)
            Me.NorthEast.TabIndex = 7
            Me.NorthEast.Tag = "1,-1"
            ' 
            ' East
            ' 
            Me.East.Checked = True
            Me.East.Location = New System.Drawing.Point(136, 104)
            Me.East.Name = "East"
            Me.East.Size = New System.Drawing.Size(16, 24)
            Me.East.TabIndex = 6
            Me.East.TabStop = True
            Me.East.Tag = "2,0"
            ' 
            ' SouthEast
            ' 
            Me.SouthEast.Location = New System.Drawing.Point(120, 144)
            Me.SouthEast.Name = "SouthEast"
            Me.SouthEast.Size = New System.Drawing.Size(16, 24)
            Me.SouthEast.TabIndex = 5
            Me.SouthEast.Tag = "1,1"
            ' 
            ' South
            ' 
            Me.South.Location = New System.Drawing.Point(80, 168)
            Me.South.Name = "South"
            Me.South.Size = New System.Drawing.Size(16, 24)
            Me.South.TabIndex = 4
            Me.South.Tag = "0,2"
            ' 
            ' SouthWest
            ' 
            Me.SouthWest.Location = New System.Drawing.Point(40, 144)
            Me.SouthWest.Name = "SouthWest"
            Me.SouthWest.Size = New System.Drawing.Size(16, 24)
            Me.SouthWest.TabIndex = 3
            Me.SouthWest.Tag = "-1,1"
            ' 
            ' West
            ' 
            Me.West.Location = New System.Drawing.Point(24, 104)
            Me.West.Name = "West"
            Me.West.Size = New System.Drawing.Size(16, 24)
            Me.West.TabIndex = 2
            Me.West.Tag = "-2,0"
            ' 
            ' NorthWest
            ' 
            Me.NorthWest.Location = New System.Drawing.Point(40, 64)
            Me.NorthWest.Name = "NorthWest"
            Me.NorthWest.Size = New System.Drawing.Size(16, 24)
            Me.NorthWest.TabIndex = 1
            Me.NorthWest.Tag = "-1,-1"
            ' 
            ' North
            ' 
            Me.North.Location = New System.Drawing.Point(80, 40)
            Me.North.Name = "North"
            Me.North.Size = New System.Drawing.Size(16, 24)
            Me.North.TabIndex = 0
            Me.North.Tag = "0,-2"
            ' 
            ' frmMain
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(613, 485)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.Label1, Me.gbGeneralParams, Me.lstEffects, Me.gbTypeContainer, Me.EnvelopeGroupBox, Me.DirectionGroupBox})
            Me.Name = "frmMain"
            Me.Text = "Feedback"
            Me.gbGeneralParams.ResumeLayout(False)
            CType(Me.GeneralPeriod, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.GeneralGain, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.GeneralDuration, System.ComponentModel.ISupportInitialize).EndInit()
            Me.gbTypeContainer.ResumeLayout(False)
            Me.GroupConditionalForce.ResumeLayout(False)
            CType(Me.ConditionalPositiveSaturation, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.ConditionalNegativeSaturation, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.ConditionalPositiveCoefficient, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.ConditionalNegativeCoeffcient, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.ConditionalOffset, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.ConditionalDeadBand, System.ComponentModel.ISupportInitialize).EndInit()
            Me.GroupPeriodForce.ResumeLayout(False)
            CType(Me.PeriodicPeriod, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.PeriodicPhase, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.PeriodicOffset, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.PeriodicMagnitude, System.ComponentModel.ISupportInitialize).EndInit()
            Me.GroupRampForce.ResumeLayout(False)
            CType(Me.RangeEnd, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.RangeStart, System.ComponentModel.ISupportInitialize).EndInit()
            Me.GroupConstantForce.ResumeLayout(False)
            CType(Me.ConstantForceMagnitude, System.ComponentModel.ISupportInitialize).EndInit()
            Me.EnvelopeGroupBox.ResumeLayout(False)
            CType(Me.EnvelopeFadeTime, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.EnvelopeFadeLevel, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.EnvelopeAttackTime, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.EnvelopeAttackLevel, System.ComponentModel.ISupportInitialize).EndInit()
            Me.DirectionGroupBox.ResumeLayout(False)
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Public Shared Sub Main()
            Dim main As New frmMain()
            If Not main.IsDisposed Then
                Application.Run(main)
            End If
        End Sub 'Main

        Private Sub lstEffects_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstEffects.SelectedIndexChanged
            ' Handles the changing of an effect.
            Dim description As EffectDescription

            If Not Nothing Is effectSelected Then
                effectSelected.Unload()
            End If
            description = CType(lstEffects.Items(lstEffects.SelectedIndex), EffectDescription)
            effectSelected = description.effectSelected
            UpdateVisibilty()
            Try
                effectSelected.Start(1)
            Catch
            End Try
        End Sub 'lstEffects_SelectedIndexChanged

        Private Function ChangeParameter() As Effect
            ' Changes the parameters of an effect.
            Dim eff As Effect = GetEffectParameters()
            Dim flags As Integer = CInt(EffectParameterFlags.Start)
            Dim i As Integer = 0

            Select Case eff.EffectType
                Case EffectType.ConstantForce
                    eff.Constant.Magnitude = ConstantForceMagnitude.Value
                    flags = flags Or CInt(EffectParameterFlags.TypeSpecificParams)
                Case EffectType.RampForce
                    eff.RampStruct.Start = RangeStart.Value
                    eff.RampStruct.End = RangeEnd.Value
                    flags = CInt(EffectParameterFlags.TypeSpecificParams)
                    If CInt(DI.Infinite) = eff.Duration Then
                        ' Default to a 2 second ramp effect
                        ' if DI.Infinite is passed in.
                        ' DI.Infinite is invalid for ramp forces.
                        eff.Duration = 2 * CInt(DI.Seconds)
                        flags = flags Or CInt(EffectParameterFlags.Duration)
                    End If
                    flags = flags Or CInt(EffectParameterFlags.Start)
                Case EffectType.Periodic

                    eff.Periodic.Magnitude = PeriodicMagnitude.Value
                    eff.Periodic.Offset = PeriodicOffset.Value
                    eff.Periodic.Period = PeriodicPeriod.Value
                    eff.Periodic.Phase = PeriodicPhase.Value

                    flags = flags Or CInt(EffectParameterFlags.TypeSpecificParams)
                Case EffectType.Condition
                    If ConditionalAxis1.Checked = True Then
                        i = 0
                    Else
                        i = 1
                    End If
                    eff.ConditionStruct(i).DeadBand = ConditionalDeadBand.Value
                    eff.ConditionStruct(i).NegativeCoefficient = ConditionalNegativeCoeffcient.Value
                    eff.ConditionStruct(i).NegativeSaturation = ConditionalNegativeSaturation.Value
                    eff.ConditionStruct(i).Offset = ConditionalOffset.Value
                    eff.ConditionStruct(i).PositiveCoefficient = ConditionalPositiveCoefficient.Value
                    eff.ConditionStruct(i).PositiveSaturation = ConditionalPositiveSaturation.Value

                    flags = flags Or CInt(EffectParameterFlags.TypeSpecificParams)
            End Select

            ' Some feedback drivers will fail when setting parameters that aren't supported by
            ' an effect. DirectInput will will in turn pass back the driver error to the application.
            ' Since these are hardware specific error messages that can't be handled individually, 
            ' the app will ignore any failures returned to SetParameters().
            Try
                effectSelected.SetParameters(eff, EffectParameterFlags.TypeSpecificParams)
            Catch
                eff = GetEffectParameters()
            End Try

            Return eff
        End Function 'ChangeParameter

        Private Sub ChangeDirection(ByVal direction() As Integer)
            ' This sub changes the direction of an effect.
            Dim eff As New Effect()

            eff.Flags = EffectFlags.Cartesian Or EffectFlags.ObjectOffsets
            effectSelected.GetParameters(eff, EffectParameterFlags.AllParams)
            eff.SetDirection(direction)

            ' Some feedback drivers will fail when setting parameters that aren't supported by
            ' an effect. DirectInput will will in turn pass back the driver error to the application.
            ' Since these are hardware specific error messages that can't be handled individually, 
            ' the app will ignore any failures returned to SetParameters().
            Try
                effectSelected.SetParameters(eff, EffectParameterFlags.Direction Or EffectParameterFlags.Start)
            Catch
            End Try
        End Sub 'ChangeDirection

        Private Function ChangeEnvelope() As Effect
            ' This sub changes the envelope of an effect.
            Dim eff As Effect = GetEffectParameters()

            If Not isChanging Then
                eff.UsesEnvelope = chkUseEnvelope.Checked
                eff.EnvelopeStruct.AttackLevel = EnvelopeAttackLevel.Value
                eff.EnvelopeStruct.AttackTime = EnvelopeAttackTime.Value
                eff.EnvelopeStruct.FadeLevel = EnvelopeFadeLevel.Value
                eff.EnvelopeStruct.FadeTime = EnvelopeFadeTime.Value

                ' Some feedback drivers will fail when setting parameters that aren't supported by
                ' an effect. DirectInput will will in turn pass back the driver error to the application.
                ' Since these are hardware specific error messages that can't be handled individually, 
                ' the app will ignore any failures returned to SetParameters().
                Try
                    effectSelected.SetParameters(eff, EffectParameterFlags.Envelope Or EffectParameterFlags.Start)
                Catch
                End Try
            End If
            Return eff
        End Function 'ChangeEnvelope

        Private Function GetEffectParameters() As Effect
            ' Fills in an Effect structure with effect information.
            Dim eff As New Effect()

            eff.Flags = EffectFlags.ObjectIds Or EffectFlags.Cartesian
            effectSelected.GetParameters(eff, EffectParameterFlags.AllParams)

            ' If this is a condition effect, see if the Effect.Condition member
            ' array length that was returned from GetParameters() has enough elements 
            ' to cover 2 axes if this is a two axis device. In most cases, conditional 
            ' effects will return 1 Condition element that can be applied across 
            ' all force-feedback axes.
            If eff.EffectType = EffectType.Condition And Not (eff.ConditionStruct Is Nothing) Then
                If rbConditionalAxis2.Enabled And eff.ConditionStruct.Length < 2 Then
                    ' Resize the array for two axes.
                    Dim temp(2) As Condition
                    eff.ConditionStruct.CopyTo(temp, 0)
                    eff.ConditionStruct = temp
                    ' Copy the conditional effect info from one struct to the other.
                    eff.ConditionStruct(1) = eff.ConditionStruct(0)
                End If
            End If
            Return eff
        End Function 'GetEffectParameters

        Private Sub UpdateVisibilty()
            ' Updates the visibility of each
            ' effect parameters group box, as well
            ' as general parameter, envelope, and
            ' direction group boxes.
            isChanging = True

            If Nothing Is effectSelected Then
                Return
            End If
            Dim description As EffectDescription = CType(lstEffects.Items(lstEffects.SelectedIndex), EffectDescription)
            Dim eff As Effect = GetEffectParameters()

            Dim Current As New GroupBox()

            ' Check to see what type of effect this is,
            ' and then change the visibilty of the
            ' group boxes accordingly.
            Select Case DInputHelper.GetTypeCode(CInt(eff.EffectType))
                Case CInt(EffectType.Condition)
                    Current = GroupConditionalForce
                    UpdateConditionalGroupBox(eff)
                Case CInt(EffectType.ConstantForce)
                    Current = GroupConstantForce
                    UpdateConstantGroupBox(eff)
                Case CInt(EffectType.Periodic)
                    Current = GroupPeriodForce
                    UpdatePeriodicGroupBox(eff)
                Case CInt(EffectType.RampForce)
                    Current = GroupRampForce
                    UpdateRampGroupBox(eff)
            End Select

            Dim target As GroupBox
            For Each target In gbTypeContainer.Controls
                If Current Is target Then
                    target.Visible = True
                Else
                    target.Visible = False
                End If
            Next target
            ' Check the effect info and update the controls
            ' to show whether the parameters are supported.
            If 0 = (description.info.StaticParams And CInt(EffectParameterFlags.Direction)) Then
                DirectionGroupBox.Enabled = False
            Else
                DirectionGroupBox.Enabled = True
            End If
            If 0 = (description.info.StaticParams And CInt(EffectParameterFlags.Duration)) Then
                GeneralDurationLabel.Enabled = False
                GeneralDuration.Enabled = False
            Else
                GeneralDurationLabel.Enabled = True
                GeneralDuration.Enabled = True
            End If
            If 0 = (description.info.StaticParams And CInt(EffectParameterFlags.Gain)) Then
                GeneralGainLabel.Enabled = False
                GeneralGain.Enabled = False
            Else
                GeneralGainLabel.Enabled = True
                GeneralGain.Enabled = True
            End If
            If 0 = (description.info.StaticParams And CInt(EffectParameterFlags.SamplePeriod)) Then
                GeneralPeriodLabel.Enabled = False
                GeneralPeriod.Enabled = False
            Else
                GeneralPeriodLabel.Enabled = True
                GeneralPeriod.Enabled = True
            End If
            ' Update the general parameter
            ' and envelope controls.
            UpdateGeneralParamsGroupBox(eff)

            ' Reflect support for envelopes on this effect.
            UpdateEnvParamsGroupBox(eff)
            EnvelopeGroupBox.Enabled = IIf(description.info.StaticParams And CInt(EffectParameterFlags.Envelope) <> 0, True, False)

            ' Update direction radio buttons.
            If 1 = axis.Length Then
                If 2 = eff.GetDirection()(0) Then
                    East.Checked = True
                Else
                    West.Checked = True
                End If
            ElseIf 2 >= axis.Length Then
                If 2 = eff.GetDirection()(0) And 0 = eff.GetDirection()(1) Then
                    East.Checked = True
                ElseIf -2 = eff.GetDirection()(0) And 0 = eff.GetDirection()(1) Then
                    West.Checked = True
                ElseIf 0 = eff.GetDirection()(0) And -2 = eff.GetDirection()(1) Then
                    North.Checked = True
                ElseIf 0 = eff.GetDirection()(0) And 2 = eff.GetDirection()(1) Then
                    South.Checked = True
                ElseIf 1 = eff.GetDirection()(0) And -1 = eff.GetDirection()(1) Then
                    NorthEast.Checked = True
                ElseIf 1 = eff.GetDirection()(0) And 1 = eff.GetDirection()(1) Then
                    SouthEast.Checked = True
                ElseIf -1 = eff.GetDirection()(0) And 1 = eff.GetDirection()(1) Then
                    SouthWest.Checked = True
                ElseIf -1 = eff.GetDirection()(0) And -1 = eff.GetDirection()(1) Then
                    NorthWest.Checked = True
                ElseIf 0 = eff.GetDirection()(0) And 0 = eff.GetDirection()(1) Then
                    East.Checked = True
                End If
            End If
            isChanging = False
        End Sub 'UpdateVisibilty

        Private Sub UpdateGeneralParamsGroupBox(ByVal eff As Effect)
            ' Updates the general parameters controls and labels.
            If eff.Duration / CInt(DI.Seconds) > GeneralDuration.Maximum Or eff.Duration < 0 Then
                GeneralDuration.Value = GeneralDuration.Maximum
            Else
                GeneralDuration.Value = eff.Duration / CInt(DI.Seconds)
            End If
            If eff.Gain > GeneralGain.Maximum Then
                GeneralGain.Value = GeneralGain.Maximum
            Else
                GeneralGain.Value = eff.Gain
            End If
            If eff.SamplePeriod > GeneralPeriod.Maximum Then
                GeneralPeriod.Value = GeneralPeriod.Maximum
            Else
                GeneralPeriod.Value = eff.SamplePeriod
            End If
            If CInt(DI.Infinite) = eff.Duration Then
                GeneralDurationLabel.Text = "Effect Duration: Infinite"
            Else
                GeneralDurationLabel.Text = "Effect Duration: " + (eff.Duration / DI.Seconds).ToString + " seconds"
            End If
            GeneralGainLabel.Text = "Effect Gain: " + GeneralGain.Value.ToString()

            If 0 = eff.SamplePeriod Then
                GeneralPeriodLabel.Text = "Sample Rate: Default"
            Else
                GeneralPeriodLabel.Text = "Sample Period: " + eff.SamplePeriod.ToString()
            End If
        End Sub 'UpdateGeneralParamsGroupBox

        Private Sub UpdateConstantGroupBox(ByVal eff As Effect)
            ' Updates the controls and labels for constant force effects.
            ConstantForceMagnitude.Value = eff.Constant.Magnitude
            Magnitude.Text = "Constant Force Magnitude: " + ConstantForceMagnitude.Value.ToString()
        End Sub 'UpdateConstantGroupBox

        Private Sub UpdateRampGroupBox(ByVal eff As Effect)
            ' Updates the controls and labels for ramp effects.
            RangeStart.Value = eff.RampStruct.Start
            RangeEnd.Value = eff.RampStruct.End
            RangeStartLabel.Text = "Range Start: " + RangeStart.Value.ToString()
            RangeEndLabel.Text = "Range End: " + RangeEnd.Value.ToString()
        End Sub 'UpdateRampGroupBox

        Private Sub UpdatePeriodicGroupBox(ByVal eff As Effect)
            ' Updates the controls and labels for periodic effects.
            If eff.Periodic.Magnitude < PeriodicMagnitude.Maximum Then
                PeriodicMagnitude.Value = eff.Periodic.Magnitude
            Else
                PeriodicMagnitude.Value = PeriodicMagnitude.Maximum
            End If
            If eff.Periodic.Offset < PeriodicOffset.Maximum Then
                PeriodicOffset.Value = eff.Periodic.Offset
            Else
                PeriodicOffset.Value = PeriodicOffset.Maximum
            End If
            If eff.Periodic.Period < PeriodicPeriod.Maximum Then
                PeriodicPeriod.Value = eff.Periodic.Period
            Else
                PeriodicPeriod.Value = PeriodicPeriod.Maximum
            End If
            If eff.Periodic.Phase < PeriodicPhase.Maximum Then
                PeriodicPhase.Value = eff.Periodic.Phase
            Else
                PeriodicPhase.Value = PeriodicPhase.Maximum
            End If
            PeriodicMagnitudeLabel.Text = "Magnitude: " + PeriodicMagnitude.Value.ToString()
            PeriodicOffsetLabel.Text = "Offset: " + PeriodicOffset.Value.ToString()
            PeriodicPeriodLabel.Text = "Period: " + PeriodicPeriod.Value.ToString()
            PeriodicPhaseLabel.Text = "Phase: " + PeriodicPhase.Value.ToString()
        End Sub 'UpdatePeriodicGroupBox

        Private Sub UpdateConditionalGroupBox(ByVal eff As Effect)
            ' Updates the controls in the Conditional group box.
            Dim i As Integer

            If True = ConditionalAxis1.Checked Then
                i = 0
            Else
                i = 1
            End If

            If (Nothing Is eff.ConditionStruct) Then
                Return
            End If

            ConditionalDeadBand.Value = eff.ConditionStruct(i).DeadBand
            ConditionalOffset.Value = eff.ConditionStruct(i).Offset
            ConditionalNegativeCoeffcient.Value = eff.ConditionStruct(i).NegativeCoefficient
            ConditionalNegativeSaturation.Value = eff.ConditionStruct(i).NegativeSaturation
            ConditionalPositiveCoefficient.Value = eff.ConditionStruct(i).PositiveCoefficient
            ConditionalPositiveSaturation.Value = eff.ConditionStruct(i).PositiveSaturation

            ConditionalDeadBandLabel.Text = "Dead Band: " + ConditionalDeadBand.Value.ToString()
            ConditionalOffsetLabel.Text = "Offset: " + ConditionalOffset.Value.ToString()
            ConditionalNegativeCoeffcientLabel.Text = "Negative Coefficient: " + ConditionalNegativeCoeffcient.Value.ToString()
            ConditionalNegativeSaturationLabel.Text = "Negative Saturation: " + ConditionalNegativeSaturation.Value.ToString()
            ConditionalPositiveCoefficientLabel.Text = "Positive Coefficient: " + ConditionalPositiveCoefficient.Value.ToString()
            ConditionalPositiveSaturationLabel.Text = "Positive Saturation: " + ConditionalPositiveSaturation.Value.ToString()
        End Sub 'UpdateConditionalGroupBox

        Private Sub UpdateEnvParamsGroupBox(ByVal eff As Effect)
            chkUseEnvelope.Checked = IIf(eff.UsesEnvelope, True, False)

            If eff.EnvelopeStruct.AttackLevel > EnvelopeAttackLevel.Maximum Then
                EnvelopeAttackLevel.Value = EnvelopeAttackLevel.Maximum
            Else
                EnvelopeAttackLevel.Value = eff.EnvelopeStruct.AttackLevel
            End If
            If eff.EnvelopeStruct.AttackTime > EnvelopeAttackTime.Maximum Then
                EnvelopeAttackTime.Value = EnvelopeAttackTime.Maximum
            Else
                EnvelopeAttackTime.Value = eff.EnvelopeStruct.AttackTime
            End If
            If eff.EnvelopeStruct.FadeLevel > EnvelopeFadeLevel.Maximum Then
                EnvelopeFadeLevel.Value = EnvelopeFadeLevel.Maximum
            Else
                EnvelopeFadeLevel.Value = eff.EnvelopeStruct.FadeLevel
            End If
            If eff.EnvelopeStruct.FadeTime > EnvelopeFadeTime.Maximum Then
                EnvelopeFadeTime.Value = EnvelopeFadeTime.Maximum
            Else
                EnvelopeFadeTime.Value = eff.EnvelopeStruct.FadeTime
            End If
            EnvelopeAttackLevelLabel.Text = "Attack Level: " + eff.EnvelopeStruct.AttackLevel.ToString()
            EnvelopeAttackTimeLabel.Text = "Attack Time: " + (eff.EnvelopeStruct.AttackTime / 1000).ToString()
            EnvelopeFadeLevelLabel.Text = "Fade Level: " + eff.EnvelopeStruct.FadeLevel.ToString()
            EnvelopeFadeTimeLabel.Text = "Fade Time: " + (eff.EnvelopeStruct.FadeTime / 1000).ToString()
        End Sub 'UpdateEnvParamsGroupBox


        Private Sub ConditionalAxisChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles rbConditionalAxis2.CheckedChanged, ConditionalAxis1.CheckedChanged
            ' Handles changing the axis on a conditional effect.

            If (Nothing Is applicationDevice) Then Return

            Dim eff As Effect = GetEffectParameters()
            UpdateConditionalGroupBox(eff)
        End Sub 'ConditionalAxisChanged


        Private Sub ConstantForceMagnitudeScroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles ConstantForceMagnitude.Scroll
            ' Handles the trackbar scroll events for constant effects.

            If (Nothing Is applicationDevice) Then Return

            Dim eff As Effect = ChangeParameter()
            UpdateConstantGroupBox(eff)
        End Sub 'ConstantForceMagnitudeScroll


        Private Sub RangeScroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles RangeEnd.Scroll, RangeStart.Scroll
            ' Handles the trackbar scroll events for ramp effects.

            If (Nothing Is applicationDevice) Then Return

            Dim eff As Effect = ChangeParameter()
            UpdateRampGroupBox(eff)
        End Sub 'RangeScroll


        Private Sub PeriodicScroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles PeriodicPeriod.Scroll, PeriodicPhase.Scroll, PeriodicOffset.Scroll, PeriodicMagnitude.Scroll
            ' Handles the trackbar scroll events for periodic effects.
            Dim eff As Effect = ChangeParameter()
            UpdatePeriodicGroupBox(eff)
        End Sub 'PeriodicScroll


        Private Sub ConditionalScroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles ConditionalPositiveSaturation.Scroll, ConditionalNegativeSaturation.Scroll, ConditionalPositiveCoefficient.Scroll, ConditionalNegativeCoeffcient.Scroll, ConditionalOffset.Scroll, ConditionalDeadBand.Scroll
            ' Handles the trackbar scroll events for conditional effects.

            If (Nothing Is applicationDevice) Then Return

            Dim eff As New Effect()

            If 1 <= axis.Length Then
                rbConditionalAxis2.Enabled = True
            Else
                rbConditionalAxis2.Enabled = False
            End If
            eff = ChangeParameter()
            UpdateConditionalGroupBox(eff)
        End Sub 'ConditionalScroll


        Private Sub DirectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles NorthEast.CheckedChanged, East.CheckedChanged, SouthEast.CheckedChanged, South.CheckedChanged, SouthWest.CheckedChanged, West.CheckedChanged, NorthWest.CheckedChanged, North.CheckedChanged
            ' Handles direction changes.

            If (Nothing Is applicationDevice) Then Return

            Dim direction(2) As Integer
            Dim values() As String

            Dim rb As RadioButton
            For Each rb In DirectionGroupBox.Controls
                If rb.Checked Then
                    values = rb.Tag.ToString().Split(","c)
                    direction(0) = Convert.ToInt32(values(0))
                    direction(1) = Convert.ToInt32(values(1))
                    ChangeDirection(direction)
                    Return
                End If
            Next rb
        End Sub 'DirectionChanged


        Private Sub GenScroll(ByVal sender As Object, ByVal e As System.EventArgs) Handles GeneralPeriod.Scroll, GeneralGain.Scroll, GeneralDuration.Scroll
            ' Handles general parameter changes.

            If (Nothing Is applicationDevice) Then Return

            Dim eff As Effect = GetEffectParameters()

            If GeneralDuration.Value = GeneralDuration.Maximum Then
                eff.Duration = CInt(DI.Infinite)
            Else
                eff.Duration = GeneralDuration.Value * CInt(DI.Seconds)
            End If
            eff.Gain = GeneralGain.Value
            eff.SamplePeriod = GeneralPeriod.Value

            UpdateGeneralParamsGroupBox(eff)

            ' Some feedback drivers will fail when setting parameters that aren't supported by
            ' an effect. DirectInput will will in turn pass back the driver error to the application.
            ' Since these are hardware specific error messages that can't be handled individually, 
            ' the app will ignore any failures returned to SetParameters().
            Try
                effectSelected.SetParameters(eff, EffectParameterFlags.Duration Or EffectParameterFlags.Gain Or EffectParameterFlags.SamplePeriod Or EffectParameterFlags.Start)
            Catch
            End Try
        End Sub 'GenScroll

        Private Sub EnvChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles EnvelopeFadeTime.Scroll, EnvelopeFadeLevel.Scroll, EnvelopeAttackTime.Scroll, EnvelopeAttackLevel.Scroll, chkUseEnvelope.CheckedChanged
            Dim eff As New Effect()

            If (Nothing Is applicationDevice) Then Return

            eff = ChangeEnvelope()
            UpdateEnvParamsGroupBox(eff)
        End Sub 'EnvChanged


        Private Sub frmMain_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
            DestroyObjects()
        End Sub 'frmMain_Closing


        Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
            'Initialize the DirectInput objects
            If Not InitializeDirectInput() Then
                Close()
            End If
        End Sub 'frmMain_Load

        Private Sub frmMain_Activate(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated

            Try
                ' Aquire the device
                applicationDevice.Acquire()
            Catch
            End Try
            lstEffects_SelectedIndexChanged(Nothing, EventArgs.Empty)
        End Sub
        Private Sub DestroyObjects()
            ' Destroys all objects.
            If Not Nothing Is applicationDevice Then
                Return
            End If

            Try
                If Not Nothing Is effectSelected Then
                    effectSelected.Stop()
                End If
                Dim description As EffectDescription
                For Each description In lstEffects.Items
                    If Not Nothing Is description.effectSelected Then
                        description.effectSelected.Dispose()
                    End If
                Next description
                applicationDevice.Unacquire()
                applicationDevice.Properties.AutoCenter = True
                applicationDevice.Dispose()
            Catch
            End Try

        End Sub 'DestroyObjects
    End Class 'frmMain 
End Namespace 'Feedback