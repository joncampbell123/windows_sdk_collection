Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectSound

Public Class DevicesForm
    Inherits Form
    Private WithEvents buttonOk As Button
    Private WithEvents buttonCancel As Button
    Private labelStatic As Label
    Private comboboxCaptureDeviceCombo As ComboBox
    Private mf As MainForm = Nothing

    Private devices As New CaptureDevicesCollection()

    Public Sub New(ByVal mf As MainForm)
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()
        Me.mf = mf

        Dim info As DeviceInformation
        For Each info In devices
            comboboxCaptureDeviceCombo.Items.Add(info.Description)
        Next info
        comboboxCaptureDeviceCombo.SelectedIndex = 0
    End Sub 'New

    Sub InitializeComponent()
        Me.buttonOk = New System.Windows.Forms.Button()
        Me.buttonCancel = New System.Windows.Forms.Button()
        Me.labelStatic = New System.Windows.Forms.Label()
        Me.comboboxCaptureDeviceCombo = New System.Windows.Forms.ComboBox()
        Me.SuspendLayout()
        ' 
        ' buttonOk
        ' 
        Me.buttonOk.Location = New System.Drawing.Point(10, 41)
        Me.buttonOk.Name = "buttonOk"
        Me.buttonOk.TabIndex = 0
        Me.buttonOk.Text = "OK"
        ' 
        ' buttonCancel
        ' 
        Me.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.buttonCancel.Location = New System.Drawing.Point(231, 41)
        Me.buttonCancel.Name = "buttonCancel"
        Me.buttonCancel.TabIndex = 1
        Me.buttonCancel.Text = "Cancel"
        ' 
        ' labelStatic
        ' 
        Me.labelStatic.Location = New System.Drawing.Point(10, 14)
        Me.labelStatic.Name = "labelStatic"
        Me.labelStatic.Size = New System.Drawing.Size(78, 13)
        Me.labelStatic.TabIndex = 2
        Me.labelStatic.Text = "Capture Device:"
        ' 
        ' comboboxCaptureDeviceCombo
        ' 
        Me.comboboxCaptureDeviceCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.comboboxCaptureDeviceCombo.Location = New System.Drawing.Point(93, 11)
        Me.comboboxCaptureDeviceCombo.Name = "comboboxCaptureDeviceCombo"
        Me.comboboxCaptureDeviceCombo.Size = New System.Drawing.Size(213, 21)
        Me.comboboxCaptureDeviceCombo.Sorted = True
        Me.comboboxCaptureDeviceCombo.TabIndex = 3
        ' 
        ' DevicesForm
        ' 
        Me.AcceptButton = Me.buttonOk
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.CancelButton = Me.buttonCancel
        Me.ClientSize = New System.Drawing.Size(316, 79)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.buttonOk, Me.buttonCancel, Me.labelStatic, Me.comboboxCaptureDeviceCombo})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "DevicesForm"
        Me.Text = "Select Capture Device"
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    Sub buttonOk_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonOk.Click
        If 0 < comboboxCaptureDeviceCombo.Items.Count Then
            mf.CaptureDeviceGuid = devices(0).DriverGuid
        End If
        Close()
    End Sub 'buttonOk_Click
End Class 'DevicesForm