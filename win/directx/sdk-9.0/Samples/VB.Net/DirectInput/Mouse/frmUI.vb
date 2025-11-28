Imports System
Imports System.Windows.Forms
Imports Microsoft.DirectX.DirectInput
Imports Microsoft.DirectX
Imports System.Threading


Namespace mouse
    '/ <summary>
    '/ Summary description for Form1.
    '/ </summary>
    Public Class frmUI
        Inherits System.Windows.Forms.Form
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        '/
        Private applicationDevice As Device = Nothing
        Private threadData As Thread = Nothing
        Private eventFire As AutoResetEvent = Nothing

        Delegate Sub UIDelegate()

        Private label1 As System.Windows.Forms.Label
        Private lblButton0 As System.Windows.Forms.Label
        Private label2 As System.Windows.Forms.Label
        Private lblButton1 As System.Windows.Forms.Label
        Private label3 As System.Windows.Forms.Label
        Private lblButton2 As System.Windows.Forms.Label
        Private label4 As System.Windows.Forms.Label
        Private lbStatus As System.Windows.Forms.ListBox
        Private components As System.ComponentModel.Container = Nothing


        Public Sub MouseEvent()
            ' This is the mouse event thread.
            ' This thread will wait until a
            ' mouse event occurrs, and invoke
            ' the delegate on the main thread to
            ' grab the current mouse state and update
            ' the UI.
            While Created
                eventFire.WaitOne(-1, False)

                If Not Created Then Exit While

                Try
                    applicationDevice.Poll()
                Catch
                    GoTo ContinueWhile
                End Try
                Me.BeginInvoke(New UIDelegate(AddressOf UpdateUI))
ContinueWhile:
            End While
        End Sub 'MouseEvent

        <MTAThread()> _
      Public Shared Sub Main()
            Application.Run(New frmUI())
        End Sub 'Main


        Public Sub New()
            '
            ' Required for Windows Form Designer support.
            '
            InitializeComponent()
        End Sub 'New

        '/ <summary>
        '/ Clean up any resources being used.
        '/ </summary>
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)

            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)

            eventFire.Set()

        End Sub 'Dispose

        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Me.lbStatus = New System.Windows.Forms.ListBox()
            Me.lblButton2 = New System.Windows.Forms.Label()
            Me.lblButton0 = New System.Windows.Forms.Label()
            Me.lblButton1 = New System.Windows.Forms.Label()
            Me.label4 = New System.Windows.Forms.Label()
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.label3 = New System.Windows.Forms.Label()
            Me.SuspendLayout()
            ' 
            ' lbStatus
            ' 
            Me.lbStatus.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.lbStatus.Items.AddRange(New Object() {"0,0,0"})
            Me.lbStatus.Location = New System.Drawing.Point(32, 200)
            Me.lbStatus.Name = "lbStatus"
            Me.lbStatus.Size = New System.Drawing.Size(96, 106)
            Me.lbStatus.TabIndex = 4
            ' 
            ' lblButton2
            ' 
            Me.lblButton2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.lblButton2.Location = New System.Drawing.Point(32, 144)
            Me.lblButton2.Name = "lblButton2"
            Me.lblButton2.Size = New System.Drawing.Size(96, 16)
            Me.lblButton2.TabIndex = 0
            Me.lblButton2.Text = "Up"
            Me.lblButton2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' lblButton0
            ' 
            Me.lblButton0.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.lblButton0.Location = New System.Drawing.Point(32, 32)
            Me.lblButton0.Name = "lblButton0"
            Me.lblButton0.Size = New System.Drawing.Size(96, 16)
            Me.lblButton0.TabIndex = 0
            Me.lblButton0.Text = "Up"
            Me.lblButton0.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' lblButton1
            ' 
            Me.lblButton1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
            Me.lblButton1.Location = New System.Drawing.Point(32, 88)
            Me.lblButton1.Name = "lblButton1"
            Me.lblButton1.Size = New System.Drawing.Size(96, 16)
            Me.lblButton1.TabIndex = 0
            Me.lblButton1.Text = "Up"
            Me.lblButton1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label4
            ' 
            Me.label4.AutoSize = True
            Me.label4.Location = New System.Drawing.Point(32, 184)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(99, 13)
            Me.label4.TabIndex = 3
            Me.label4.Text = "Coordinates (x,y,z)"
            Me.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            ' 
            ' label1
            ' 
            Me.label1.AutoSize = True
            Me.label1.Location = New System.Drawing.Point(56, 16)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(43, 13)
            Me.label1.TabIndex = 1
            Me.label1.Text = "Button0"
            ' 
            ' label2
            ' 
            Me.label2.AutoSize = True
            Me.label2.Location = New System.Drawing.Point(56, 72)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(43, 13)
            Me.label2.TabIndex = 1
            Me.label2.Text = "Button1"
            ' 
            ' label3
            ' 
            Me.label3.AutoSize = True
            Me.label3.Location = New System.Drawing.Point(56, 128)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(43, 13)
            Me.label3.TabIndex = 1
            Me.label3.Text = "Button2"
            ' 
            ' frmUI
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(162, 325)
            Me.ControlBox = False
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.lbStatus, Me.label4, Me.label3, Me.label2, Me.label1, Me.lblButton2, Me.lblButton1, Me.lblButton0})
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.KeyPreview = True
            Me.MaximizeBox = False
            Me.Name = "frmUI"
            Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide
            Me.Text = "Mouse (Escape exits)"
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        Private Sub frmUI_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
            ' If escape is pressed, user wants to exit.
            If e.KeyCode = Keys.Escape Then
                Close()
            End If
        End Sub 'frmUI_KeyUp

        Public Sub UpdateUI()
            Dim mouseStateData As New MouseState()

            ' Get the current state of the mouse device.
            mouseStateData = applicationDevice.CurrentMouseState
            Dim buttons As Byte() = mouseStateData.GetMouseButtons()

            ' Display some info about the device.
            If 0 <> buttons(0) Then
                lblButton0.Text = "Down"
            Else
                lblButton0.Text = "Up"
            End If
            If 0 <> buttons(1) Then
                lblButton1.Text = "Down"
            Else
                lblButton1.Text = "Up"
            End If
            If 0 <> buttons(2) Then
                lblButton2.Text = "Down"
            Else
                lblButton2.Text = "Up"
            End If
            If 0 <> (mouseStateData.X Or mouseStateData.Y Or mouseStateData.Z) Then
                lbStatus.Items.Add((mouseStateData.X.ToString() + ", " + mouseStateData.Y.ToString() + ", " + mouseStateData.Z.ToString()))
                If lbStatus.Items.Count > 10 Then
                    lbStatus.Items.RemoveAt(1)
                End If
                lbStatus.SelectedIndex = lbStatus.Items.Count - 1
            End If
        End Sub 'UpdateUI


        Private Sub frmUI_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
            threadData = New Thread(New ThreadStart(AddressOf Me.MouseEvent))
            threadData.Start()

            eventFire = New AutoResetEvent(False)

            Try
                ' Create the device.
                applicationDevice = New Device(SystemGuid.Mouse)
            Catch ex As InputException
                MessageBox.Show("Unable to create device. Sample will now exit.")
                Close()
            End Try
            ' Set the cooperative level for the device.
            applicationDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.Exclusive Or CooperativeLevelFlags.Foreground)
            ' Set a notification event.    
            applicationDevice.SetEventNotification(eventFire)
            ' Acquire the device.
            Try
                applicationDevice.Acquire()
            Catch
            End Try
        End Sub 'frmUI_Load

        Private Sub frmUI_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated
            ' Acquire the device whenever the application window is activated.
            If Not Nothing Is applicationDevice Then
                Try
                    applicationDevice.Acquire()
                Catch
                End Try
            End If
        End Sub 'frmUI_Activated
    End Class 'frmUI 
End Namespace 'Mouse