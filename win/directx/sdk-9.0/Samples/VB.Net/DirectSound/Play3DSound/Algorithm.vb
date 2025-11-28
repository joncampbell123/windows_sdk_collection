Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectSound

Public Class AlgorithmForm
    Inherits Form
    Private WithEvents buttonOk As Button
    Private WithEvents buttonCancel As Button
    Private WithEvents radiobuttonNoVirtRadio As RadioButton
    Private WithEvents radiobuttonHighVirtRadio As RadioButton
    Private WithEvents radiobuttonLightVirtRadio As RadioButton

    Public Sub New()
        '
        ' Required for Windows Form Designer support
        '
        InitializeComponent()
    End Sub 'New

    Sub InitializeComponent()
        Me.buttonOk = New System.Windows.Forms.Button()
        Me.buttonCancel = New System.Windows.Forms.Button()
        Me.radiobuttonNoVirtRadio = New System.Windows.Forms.RadioButton()
        Me.radiobuttonHighVirtRadio = New System.Windows.Forms.RadioButton()
        Me.radiobuttonLightVirtRadio = New System.Windows.Forms.RadioButton()
        Me.SuspendLayout()
        ' 
        ' buttonOk
        ' 
        Me.buttonOk.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.buttonOk.Location = New System.Drawing.Point(294, 72)
        Me.buttonOk.Name = "buttonOk"
        Me.buttonOk.TabIndex = 0
        Me.buttonOk.Text = "OK"
        ' 
        ' buttonCancel
        ' 
        Me.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.buttonCancel.Location = New System.Drawing.Point(375, 72)
        Me.buttonCancel.Name = "buttonCancel"
        Me.buttonCancel.TabIndex = 1
        Me.buttonCancel.Text = "Cancel"
        ' 
        ' radiobuttonNoVirtRadio
        ' 
        Me.radiobuttonNoVirtRadio.Checked = True
        Me.radiobuttonNoVirtRadio.Location = New System.Drawing.Point(10, 11)
        Me.radiobuttonNoVirtRadio.Name = "radiobuttonNoVirtRadio"
        Me.radiobuttonNoVirtRadio.Size = New System.Drawing.Size(339, 16)
        Me.radiobuttonNoVirtRadio.TabIndex = 2
        Me.radiobuttonNoVirtRadio.TabStop = True
        Me.radiobuttonNoVirtRadio.Text = "&No Virtualization (WDM or VxD. CPU efficient, but basic 3-D effect)"
        ' 
        ' radiobuttonHighVirtRadio
        ' 
        Me.radiobuttonHighVirtRadio.Location = New System.Drawing.Point(10, 28)
        Me.radiobuttonHighVirtRadio.Name = "radiobuttonHighVirtRadio"
        Me.radiobuttonHighVirtRadio.Size = New System.Drawing.Size(391, 16)
        Me.radiobuttonHighVirtRadio.TabIndex = 3
        Me.radiobuttonHighVirtRadio.Text = "&High Quality (WDM only.  Highest quality 3D audio effect, but uses more CPU)"
        ' 
        ' radiobuttonLightVirtRadio
        ' 
        Me.radiobuttonLightVirtRadio.Location = New System.Drawing.Point(10, 46)
        Me.radiobuttonLightVirtRadio.Name = "radiobuttonLightVirtRadio"
        Me.radiobuttonLightVirtRadio.Size = New System.Drawing.Size(430, 16)
        Me.radiobuttonLightVirtRadio.TabIndex = 4
        Me.radiobuttonLightVirtRadio.Text = "&Light Quality (WDM only.  Good 3-D audio effect, but uses less CPU than High Qua" + "lity)"
        ' 
        ' AlgorithmForm
        ' 
        Me.AcceptButton = Me.buttonOk
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(460, 111)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.buttonOk, Me.buttonCancel, Me.radiobuttonNoVirtRadio, Me.radiobuttonHighVirtRadio, Me.radiobuttonLightVirtRadio})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "AlgorithmForm"
        Me.Text = "Select 3D Algorithm "
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent

    Private Sub buttonOk_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonOk.Click
        If True = radiobuttonNoVirtRadio.Checked Then
            Play3DSound.guid3DAlgorithm = DSoundHelper.Guid3DAlgorithmNoVirtualization
        ElseIf True = radiobuttonHighVirtRadio.Checked Then
            Play3DSound.guid3DAlgorithm = DSoundHelper.Guid3DAlgorithmHrtfFull
        ElseIf True = radiobuttonLightVirtRadio.Checked Then
            Play3DSound.guid3DAlgorithm = DSoundHelper.Guid3DAlgorithmHrtfLight
        End If
        Me.Close()
    End Sub 'buttonOk_Click
End Class 'AlgorithmForm