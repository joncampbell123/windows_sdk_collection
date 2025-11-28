Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectSound

Public Class wfEnum
    Inherits System.Windows.Forms.Form

    Private Structure DeviceDescription
        Public info As DeviceInformation

        Public Overrides Function ToString() As String
            Return info.Description
        End Function 'ToString

        Public Sub New(ByVal d As DeviceInformation)
            info = d
        End Sub 'New
    End Structure 'DeviceDescription
    ' The devices
    Private applicationDevice As Device = Nothing
    Private applicationCapture As Capture = Nothing
    Private devicesCollection As devicesCollection = Nothing
    Private captureCollection As CaptureDevicesCollection = Nothing
    Private label1 As System.Windows.Forms.Label
    Private label2 As System.Windows.Forms.Label
    Private label3 As System.Windows.Forms.Label
    Private comboboxSound As System.Windows.Forms.ComboBox
    Private comboboxCapture As System.Windows.Forms.ComboBox
    Private WithEvents buttonCreate As System.Windows.Forms.Button
    Private WithEvents buttonExit As System.Windows.Forms.Button
    '/ <summary>
    '/ Required designer variable.
    '/ </summary>
    Private components As System.ComponentModel.Container = Nothing

    Public Sub New()
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()

        ' Retrieve the DirectSound Devices first.
        devicesCollection = New DevicesCollection()

        Dim dev As DeviceInformation

        For Each dev In devicesCollection
            Dim dd As New DeviceDescription(dev)
            comboboxSound.Items.Add(dd)
        Next dev

        ' Select the first item in the combobox
        If 0 < comboboxSound.Items.Count Then
            comboboxSound.SelectedIndex = 0
        End If
        ' Now the capture devices
        captureCollection = New CaptureDevicesCollection()

        For Each dev In captureCollection
            Dim dd As New DeviceDescription(dev)
            comboboxCapture.Items.Add(dd)
        Next dev

        ' Select the first item in the combobox
        If comboboxCapture.Items.Count > 0 Then
            comboboxCapture.SelectedIndex = 0
        End If
    End Sub 'New

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

    '/ <summary>
    '/ Required method for Designer support - do not modify
    '/ the contents of this method with the code editor.
    '/ </summary>
    Private Sub InitializeComponent()
        Me.label1 = New System.Windows.Forms.Label()
        Me.label2 = New System.Windows.Forms.Label()
        Me.label3 = New System.Windows.Forms.Label()
        Me.comboboxCapture = New System.Windows.Forms.ComboBox()
        Me.buttonCreate = New System.Windows.Forms.Button()
        Me.buttonExit = New System.Windows.Forms.Button()
        Me.comboboxSound = New System.Windows.Forms.ComboBox()
        Me.SuspendLayout()
        ' 
        ' label1
        ' 
        Me.label1.Location = New System.Drawing.Point(6, 4)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(288, 15)
        Me.label1.TabIndex = 0
        Me.label1.Text = "This sample simply shows how to enumerate devices."
        ' 
        ' label2
        ' 
        Me.label2.Location = New System.Drawing.Point(6, 29)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(79, 17)
        Me.label2.TabIndex = 1
        Me.label2.Text = "Sound Device:"
        ' 
        ' label3
        ' 
        Me.label3.Location = New System.Drawing.Point(6, 53)
        Me.label3.Name = "label3"
        Me.label3.Size = New System.Drawing.Size(91, 17)
        Me.label3.TabIndex = 1
        Me.label3.Text = "Capture Device:"
        ' 
        ' comboboxCapture
        ' 
        Me.comboboxCapture.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.comboboxCapture.DropDownWidth = 202
        Me.comboboxCapture.Location = New System.Drawing.Point(92, 51)
        Me.comboboxCapture.Name = "comboboxCapture"
        Me.comboboxCapture.Size = New System.Drawing.Size(202, 21)
        Me.comboboxCapture.TabIndex = 3
        ' 
        ' buttonCreate
        ' 
        Me.buttonCreate.Location = New System.Drawing.Point(9, 81)
        Me.buttonCreate.Name = "buttonCreate"
        Me.buttonCreate.Size = New System.Drawing.Size(86, 21)
        Me.buttonCreate.TabIndex = 4
        Me.buttonCreate.Text = "Create"
        ' 
        ' buttonExit
        ' 
        Me.buttonExit.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.buttonExit.Location = New System.Drawing.Point(205, 81)
        Me.buttonExit.Name = "buttonExit"
        Me.buttonExit.Size = New System.Drawing.Size(86, 21)
        Me.buttonExit.TabIndex = 4
        Me.buttonExit.Text = "Exit"
        ' 
        ' comboboxSound
        ' 
        Me.comboboxSound.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.comboboxSound.DropDownWidth = 202
        Me.comboboxSound.Location = New System.Drawing.Point(92, 27)
        Me.comboboxSound.Name = "comboboxSound"
        Me.comboboxSound.Size = New System.Drawing.Size(202, 21)
        Me.comboboxSound.TabIndex = 2
        ' 
        ' wfEnum
        ' 
        Me.AcceptButton = Me.buttonCreate
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.CancelButton = Me.buttonExit
        Me.ClientSize = New System.Drawing.Size(300, 121)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.buttonExit, Me.buttonCreate, Me.comboboxCapture, Me.comboboxSound, Me.label3, Me.label2, Me.label1})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "wfEnum"
        Me.Text = "DirectSound Enumerate Devices"
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    '/ <summary>
    '/ The main entry point for the application.
    '/ </summary>
    Shared Sub Main()
        Application.Run(New wfEnum())
    End Sub 'Main


    Private Sub buttonExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonExit.Click
        Me.Dispose()
    End Sub 'buttonExit_Click


    Private Sub buttonCreate_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonCreate.Click
        Dim itemSelect As DeviceDescription

        ' Check to see if there are any devices available.
        If 0 = comboboxSound.Items.Count And 0 = comboboxCapture.Items.Count Then
            MessageBox.Show("No devices available.")
            Return
        End If

        ' First try to create the Sound Device
        Try
            itemSelect = CType(comboboxSound.Items(comboboxSound.SelectedIndex), DeviceDescription)
            If Guid.Empty.Equals(itemSelect.info.DriverGuid) Then
                applicationDevice = New Device()
            Else
                applicationDevice = New Device(itemSelect.info.DriverGuid)
            End If
        Catch
            MessageBox.Show("Could not create DirectSound device.", "Failure!", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End Try

        ' Next try to create a capture device
        Try
            itemSelect = CType(comboboxCapture.Items(comboboxCapture.SelectedIndex), DeviceDescription)
            If Guid.Empty.Equals(itemSelect.info.DriverGuid) Then
                applicationCapture = New Capture()
            Else
                applicationCapture = New Capture(itemSelect.info.DriverGuid)
            End If
        Catch
            MessageBox.Show("Could not create DirectSound Capture device.", "Failure!", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End Try
        MessageBox.Show("Devices created successfully.", "Success!", MessageBoxButtons.OK, MessageBoxIcon.Information)
    End Sub 'buttonCreate_Click
End Class 'wfEnum
