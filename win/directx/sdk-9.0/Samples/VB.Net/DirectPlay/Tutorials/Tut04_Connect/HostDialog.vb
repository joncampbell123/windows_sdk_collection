Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms

Namespace Tut04_Connect

    '/ <summary>
    '/ Summary description for HostDialog.
    '/ </summary>
    Public Class HostDialog
        Inherits System.Windows.Forms.Form
        Private m_SessionName As String = "New Host" ' Hosted session name 
        Private label1 As System.Windows.Forms.Label
        Private label2 As System.Windows.Forms.Label
        Private pictureBox1 As System.Windows.Forms.PictureBox
        Private WithEvents OKButton As System.Windows.Forms.Button
        Private WithEvents buttonCancel As System.Windows.Forms.Button
        Private SessionNameTextBox As System.Windows.Forms.TextBox
        Private SessionNameLabel As System.Windows.Forms.Label

        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing

        ' Properties

        Public ReadOnly Property SessionName() As String
            Get
                Return m_SessionName
            End Get
        End Property

        ' Property: Local port on which to host
        Public Property LocalPort() As Integer
            Get
                Dim retValue As Integer

                Try
                    retValue = Integer.Parse(portTextBox.Text)
                Catch ex As Exception
                End Try

                Return retValue
            End Get

            Set(ByVal Value As Integer)
                portTextBox.Text = Value.ToString()
            End Set
        End Property

        '/ <summary>
        '/ Constructor
        '/ </summary>
        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()
            SessionNameTextBox.Text = m_SessionName
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
        Friend WithEvents portLabel As System.Windows.Forms.Label
        Friend WithEvents portTextBox As System.Windows.Forms.TextBox
        Private Sub InitializeComponent()
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.pictureBox1 = New System.Windows.Forms.PictureBox()
            Me.OKButton = New System.Windows.Forms.Button()
            Me.buttonCancel = New System.Windows.Forms.Button()
            Me.SessionNameTextBox = New System.Windows.Forms.TextBox()
            Me.SessionNameLabel = New System.Windows.Forms.Label()
            Me.portLabel = New System.Windows.Forms.Label()
            Me.portTextBox = New System.Windows.Forms.TextBox()
            Me.SuspendLayout()
            '
            'label1
            '
            Me.label1.BackColor = System.Drawing.SystemColors.Info
            Me.label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.label1.Location = New System.Drawing.Point(15, 16)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(224, 16)
            Me.label1.TabIndex = 0
            Me.label1.Text = "Please provide a name for the new session."
            '
            'label2
            '
            Me.label2.BackColor = System.Drawing.SystemColors.Info
            Me.label2.Location = New System.Drawing.Point(15, 32)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(232, 16)
            Me.label2.TabIndex = 1
            Me.label2.Text = "This name will help other players identify you."
            '
            'pictureBox1
            '
            Me.pictureBox1.BackColor = System.Drawing.SystemColors.Info
            Me.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.pictureBox1.Location = New System.Drawing.Point(8, 8)
            Me.pictureBox1.Name = "pictureBox1"
            Me.pictureBox1.Size = New System.Drawing.Size(240, 48)
            Me.pictureBox1.TabIndex = 2
            Me.pictureBox1.TabStop = False
            '
            'OKButton
            '
            Me.OKButton.Location = New System.Drawing.Point(112, 160)
            Me.OKButton.Name = "OKButton"
            Me.OKButton.Size = New System.Drawing.Size(64, 24)
            Me.OKButton.TabIndex = 3
            Me.OKButton.Text = "&OK"
            '
            'buttonCancel
            '
            Me.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.buttonCancel.Location = New System.Drawing.Point(184, 160)
            Me.buttonCancel.Name = "buttonCancel"
            Me.buttonCancel.Size = New System.Drawing.Size(64, 24)
            Me.buttonCancel.TabIndex = 4
            Me.buttonCancel.Text = "&Cancel"
            '
            'SessionNameTextBox
            '
            Me.SessionNameTextBox.AutoSize = False
            Me.SessionNameTextBox.Location = New System.Drawing.Point(104, 80)
            Me.SessionNameTextBox.Name = "SessionNameTextBox"
            Me.SessionNameTextBox.Size = New System.Drawing.Size(120, 20)
            Me.SessionNameTextBox.TabIndex = 5
            Me.SessionNameTextBox.Text = "New Host"
            '
            'SessionNameLabel
            '
            Me.SessionNameLabel.Location = New System.Drawing.Point(16, 82)
            Me.SessionNameLabel.Name = "SessionNameLabel"
            Me.SessionNameLabel.Size = New System.Drawing.Size(88, 14)
            Me.SessionNameLabel.TabIndex = 6
            Me.SessionNameLabel.Text = "Session Name"
            Me.SessionNameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            '
            'portLabel
            '
            Me.portLabel.Location = New System.Drawing.Point(51, 114)
            Me.portLabel.Name = "portLabel"
            Me.portLabel.Size = New System.Drawing.Size(48, 16)
            Me.portLabel.TabIndex = 7
            Me.portLabel.Text = "Port"
            Me.portLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight
            '
            'portTextBox
            '
            Me.portTextBox.Location = New System.Drawing.Point(104, 112)
            Me.portTextBox.Name = "portTextBox"
            Me.portTextBox.Size = New System.Drawing.Size(56, 20)
            Me.portTextBox.TabIndex = 8
            Me.portTextBox.Text = ""
            '
            'HostDialog
            '
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(258, 192)
            Me.ControlBox = False
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.portTextBox, Me.portLabel, Me.SessionNameLabel, Me.SessionNameTextBox, Me.buttonCancel, Me.OKButton, Me.label2, Me.label1, Me.pictureBox1})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "HostDialog"
            Me.ShowInTaskbar = False
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
            Me.Text = "Host New Session"
            Me.ResumeLayout(False)

        End Sub 'InitializeComponent

        Private Sub OKButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles OKButton.Click '
            ' Save the current settings
            m_SessionName = SessionNameTextBox.Text

            DialogResult = DialogResult.OK
        End Sub 'OKButton_Click


        Private Sub cancelButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonCancel.Click
            ' Discard settings
            DialogResult = DialogResult.Cancel
        End Sub 'cancelButton_Click
    End Class 'HostDialog
End Namespace 'Tut04_Connect