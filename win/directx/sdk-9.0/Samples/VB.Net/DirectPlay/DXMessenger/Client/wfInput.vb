Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms


Namespace DXMessengerClient
    _
    '/ <summary>
    '/ This class will mimic the InputBox function for VB
    '/ </summary>
    Public Class wfInput
        Inherits System.Windows.Forms.Form
        Private lblMessage As System.Windows.Forms.Label
        Private txtInput As System.Windows.Forms.TextBox
        Private btnOk As System.Windows.Forms.Button
        Private btnCancel As System.Windows.Forms.Button
        Private components As System.ComponentModel.Container = Nothing


        Private Sub New(ByVal windowTitle As String, ByVal messageTitle As String)
            InitializeComponent()
            Me.Text = windowTitle
            Me.lblMessage.Text = messageTitle
        End Sub 'New


        Public Shared Function InputBox(ByVal windowTitle As String, ByVal messageTitle As String) As String
            Dim input As New wfInput(windowTitle, messageTitle)
            Try
                If input.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                    Return input.InputText
                Else
                    Return Nothing
                End If
            Catch
                Return Nothing
            Finally
                input.Dispose()
            End Try
        End Function 'InputBox

        Public ReadOnly Property InputText() As String
            Get
                Return Me.txtInput.Text
            End Get
        End Property
        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Shadows Sub Dispose()
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
            Me.lblMessage = New System.Windows.Forms.Label()
            Me.txtInput = New System.Windows.Forms.TextBox()
            Me.btnOk = New System.Windows.Forms.Button()
            Me.btnCancel = New System.Windows.Forms.Button()
            Me.SuspendLayout()
            ' 
            ' lblMessage
            ' 
            Me.lblMessage.Location = New System.Drawing.Point(5, 8)
            Me.lblMessage.Name = "lblMessage"
            Me.lblMessage.Size = New System.Drawing.Size(283, 17)
            Me.lblMessage.TabIndex = 0
            ' 
            ' txtInput
            ' 
            Me.txtInput.Location = New System.Drawing.Point(4, 36)
            Me.txtInput.Name = "txtInput"
            Me.txtInput.Size = New System.Drawing.Size(282, 20)
            Me.txtInput.TabIndex = 1
            Me.txtInput.Text = ""
            ' 
            ' btnOk
            ' 
            Me.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK
            Me.btnOk.Location = New System.Drawing.Point(211, 63)
            Me.btnOk.Name = "btnOk"
            Me.btnOk.Size = New System.Drawing.Size(72, 22)
            Me.btnOk.TabIndex = 2
            Me.btnOk.Text = "OK"
            ' 
            ' btnCancel
            ' 
            Me.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.btnCancel.Location = New System.Drawing.Point(135, 63)
            Me.btnCancel.Name = "btnCancel"
            Me.btnCancel.Size = New System.Drawing.Size(72, 22)
            Me.btnCancel.TabIndex = 3
            Me.btnCancel.Text = "Cancel"
            ' 
            ' wfInput
            ' 
            Me.AcceptButton = Me.btnOk
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.CancelButton = Me.btnCancel
            Me.ClientSize = New System.Drawing.Size(292, 92)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnCancel, Me.btnOk, Me.txtInput, Me.lblMessage})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "wfInput"
            Me.ShowInTaskbar = False
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent 
    End Class 'wfInput
End Namespace 'DXMessengerClient '
