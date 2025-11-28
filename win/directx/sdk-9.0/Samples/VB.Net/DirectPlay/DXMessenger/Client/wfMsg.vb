Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms


Namespace DXMessengerClient
    _
    '/ <summary>
    '/ Summary description for wfMsg.
    '/ </summary>
    Public Class wfMsg
        Inherits System.Windows.Forms.Form

        Delegate Sub AddChatMsgCallback(ByVal msg As String, ByVal meTalking As Boolean, ByVal noTalking As Boolean)

        Delegate Sub ShowWindowCallback()
        Private txtMsg As System.Windows.Forms.TextBox
        Private WithEvents btnSend As System.Windows.Forms.Button
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing
        Private WithEvents txtChat As System.Windows.Forms.TextBox

        Private msUser As String = Nothing
        Private Shadows parent As wfClient = Nothing


        Public ReadOnly Property UserName() As String
            Get
                Return msUser
            End Get
        End Property

        Public Sub New(ByVal username As String, ByVal obj As wfClient)
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            msUser = username
            parent = obj
            Me.Text = "Message - " + msUser
        End Sub 'New


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
            Dim resources As New System.Resources.ResourceManager(GetType(wfMsg))
            Me.txtMsg = New System.Windows.Forms.TextBox()
            Me.txtChat = New System.Windows.Forms.TextBox()
            Me.btnSend = New System.Windows.Forms.Button()
            Me.SuspendLayout()
            ' 
            ' txtMsg
            ' 
            Me.txtMsg.Anchor = System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right
            Me.txtMsg.Location = New System.Drawing.Point(6, 3)
            Me.txtMsg.Multiline = True
            Me.txtMsg.Name = "txtMsg"
            Me.txtMsg.ReadOnly = True
            Me.txtMsg.ScrollBars = System.Windows.Forms.ScrollBars.Both
            Me.txtMsg.Size = New System.Drawing.Size(454, 292)
            Me.txtMsg.TabIndex = 100
            Me.txtMsg.TabStop = False
            Me.txtMsg.Text = ""
            ' 
            ' txtChat
            ' 
            Me.txtChat.Anchor = System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left Or System.Windows.Forms.AnchorStyles.Right
            Me.txtChat.Location = New System.Drawing.Point(4, 300)
            Me.txtChat.Multiline = True
            Me.txtChat.Name = "txtChat"
            Me.txtChat.ScrollBars = System.Windows.Forms.ScrollBars.Both
            Me.txtChat.Size = New System.Drawing.Size(377, 41)
            Me.txtChat.TabIndex = 0
            Me.txtChat.Text = ""
            ' 
            ' btnSend
            ' 
            Me.btnSend.Anchor = System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right
            Me.btnSend.Location = New System.Drawing.Point(385, 302)
            Me.btnSend.Name = "btnSend"
            Me.btnSend.Size = New System.Drawing.Size(75, 37)
            Me.btnSend.TabIndex = 1
            Me.btnSend.Text = "Send"
            ' 
            ' wfMsg
            ' 
            Me.AcceptButton = Me.btnSend
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(461, 341)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnSend, Me.txtChat, Me.txtMsg})
            Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
            Me.Name = "wfMsg"
            Me.Text = "MessageTemplate"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        '
        Public Sub ShowWindow() '
            ' Show the window, and add the text
            Me.Show()
            Me.Select()
            Me.BringToFront()
        End Sub 'ShowWindow

        Private Sub wfMsg_Enter(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Enter
            ' We have the focus, make the chat textbox the default option
            Me.txtChat.Select()
        End Sub 'wfMsg_Enter


        Public Sub AddChatMessage(ByVal msg As String, ByVal meTalking As Boolean, ByVal noTalking As Boolean)
            If Not noTalking Then
                If meTalking Then
                    msg = "<" + parent.gUsername + "> " + msg
                Else
                    msg = "<" + msUser + "> " + msg
                End If
            End If
            ' Now limit the text to 32k
            If txtMsg.Text.Length > txtMsg.MaxLength * 0.95 Then
                txtMsg.Text = txtMsg.Text.Remove(0, CInt(txtMsg.MaxLength / 2))
            End If
            ' Update the message window
            txtMsg.Text += msg + ControlChars.Cr + ControlChars.Lf
            txtMsg.SelectionStart = txtMsg.Text.Length
            txtMsg.ScrollToCaret()
        End Sub 'AddChatMessage

        Private Sub btnSend_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnSend.Click
            ' We're talking
            If Not (txtChat.Text Is Nothing) And txtChat.Text <> "" Then
                If parent.gConnected Then
                    parent.SendChatMessage(msUser, txtChat.Text)
                    Dim unavailableMsg As Object() = {txtChat.Text, True, False}

                    Me.BeginInvoke(New AddChatMsgCallback(AddressOf Me.AddChatMessage), unavailableMsg)
                    txtChat.Text = Nothing
                Else
                    Dim unavailableMsg As Object() = {"**** - You are not connected to a server, you cannot send messages.", True, True}
                    Me.BeginInvoke(New AddChatMsgCallback(AddressOf Me.AddChatMessage), unavailableMsg)
                    txtChat.Text = Nothing
                End If
            End If
        End Sub 'btnSend_Click


        Private Sub txtChat_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles txtChat.TextChanged
            ' Only allow them to send if there's data to be sent.
            btnSend.Enabled = Not (txtChat.Text Is Nothing) And txtChat.Text <> ""
        End Sub 'txtChat_TextChanged
    End Class 'wfMsg
End Namespace 'DXMessengerClient