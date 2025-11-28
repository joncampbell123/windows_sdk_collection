Imports System
Imports System.Drawing
Imports System.Windows.Forms

 _

Public Class DialogSampleForm
    Inherits Form
    Private components As System.ComponentModel.Container = Nothing

    Private WithEvents btnOk As Button
    Private WithEvents btnCancel As Button
    Private rbRadio1 As RadioButton
    Private rbRadio2 As RadioButton
    Private rbRadio3 As RadioButton
    Private lblStatic As Label
    Private txtEdit1 As TextBox
    Private cboCombo1 As ComboBox
    Private gbStatic As GroupBox


    Public Sub New()
        '
        ' Required for Windows Form Designer support
        '      
        InitializeComponent()
    End Sub 'New

    Private Sub InitializeComponent()
        Me.btnOk = New System.Windows.Forms.Button()
        Me.btnCancel = New System.Windows.Forms.Button()
        Me.rbRadio1 = New System.Windows.Forms.RadioButton()
        Me.rbRadio2 = New System.Windows.Forms.RadioButton()
        Me.rbRadio3 = New System.Windows.Forms.RadioButton()
        Me.lblStatic = New System.Windows.Forms.Label()
        Me.txtEdit1 = New System.Windows.Forms.TextBox()
        Me.cboCombo1 = New System.Windows.Forms.ComboBox()
        Me.gbStatic = New System.Windows.Forms.GroupBox()
        Me.SuspendLayout()
        ' 
        ' btnOk
        ' 
        Me.btnOk.Location = New System.Drawing.Point(76, 198)
        Me.btnOk.Name = "btnOk"
        Me.btnOk.TabIndex = 0
        Me.btnOk.Text = "OK"
        ' 
        ' btnCancel
        ' 
        Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnCancel.Location = New System.Drawing.Point(162, 198)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.TabIndex = 1
        Me.btnCancel.Text = "Cancel"
        ' 
        ' rbRadio1
        ' 
        Me.rbRadio1.Location = New System.Drawing.Point(22, 80)
        Me.rbRadio1.Name = "rbRadio1"
        Me.rbRadio1.Size = New System.Drawing.Size(60, 16)
        Me.rbRadio1.TabIndex = 2
        Me.rbRadio1.Text = "Radio 1"
        ' 
        ' rbRadio2
        ' 
        Me.rbRadio2.Location = New System.Drawing.Point(22, 104)
        Me.rbRadio2.Name = "rbRadio2"
        Me.rbRadio2.Size = New System.Drawing.Size(60, 16)
        Me.rbRadio2.TabIndex = 3
        Me.rbRadio2.Text = "Radio 2"
        ' 
        ' rbRadio3
        ' 
        Me.rbRadio3.Location = New System.Drawing.Point(22, 128)
        Me.rbRadio3.Name = "rbRadio3"
        Me.rbRadio3.Size = New System.Drawing.Size(60, 16)
        Me.rbRadio3.TabIndex = 4
        Me.rbRadio3.Text = "Radio 3"
        ' 
        ' lblStatic
        ' 
        Me.lblStatic.Location = New System.Drawing.Point(7, 8)
        Me.lblStatic.Name = "lblStatic"
        Me.lblStatic.Size = New System.Drawing.Size(240, 42)
        Me.lblStatic.TabIndex = 5
        Me.lblStatic.Text = "This dialog has a few types of controls to show that everything can work on a non" + "-GDI display as it does with a GDI based display."
        ' 
        ' txtEdit1
        ' 
        Me.txtEdit1.Location = New System.Drawing.Point(7, 168)
        Me.txtEdit1.Name = "txtEdit1"
        Me.txtEdit1.Size = New System.Drawing.Size(232, 20)
        Me.txtEdit1.TabIndex = 6
        Me.txtEdit1.Text = ""
        ' 
        ' cboCombo1
        ' 
        Me.cboCombo1.Location = New System.Drawing.Point(165, 64)
        Me.cboCombo1.Name = "cboCombo1"
        Me.cboCombo1.Size = New System.Drawing.Size(75, 21)
        Me.cboCombo1.TabIndex = 7
        ' 
        ' gbStatic
        ' 
        Me.gbStatic.Location = New System.Drawing.Point(7, 56)
        Me.gbStatic.Name = "gbStatic"
        Me.gbStatic.Size = New System.Drawing.Size(150, 109)
        Me.gbStatic.TabIndex = 8
        Me.gbStatic.TabStop = False
        Me.gbStatic.Text = "A Group of Radio Buttons"
        ' 
        ' DialogSampleForm
        ' 
        Me.AcceptButton = Me.btnOk
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(247, 245)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnOk, Me.btnCancel, Me.rbRadio1, Me.rbRadio2, Me.rbRadio3, Me.lblStatic, Me.txtEdit1, Me.cboCombo1, Me.gbStatic})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "DialogSampleForm"
        Me.ShowInTaskbar = False
        Me.Text = "Sample Dialog"
        Me.TopMost = True
        Me.ResumeLayout(False)
    End Sub 'InitializeComponent
    Protected Overloads Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub 'Dispose

    Private Sub ExitDialog(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnOk.Click, btnCancel.Click
        Me.Close()
    End Sub 'Exit
    Private Sub DialogSampleForm_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
        If Keys.Escape = e.KeyCode Then
            e.Handled = True
            Me.Close()
        End If
    End Sub 'DialogSampleForm_KeyUp
End Class 'DialogSampleForm