'-----------------------------------------------------------------------------
' File: ActionBasicUI.cs
'
' Desc: ActionBasic user interface
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports System.Text
Imports Microsoft.DirectX

Namespace ActionBasic
    '/ <summary>
    '/ description.
    '/ </summary>
    Public Class ActionBasicUI
        Inherits System.Windows.Forms.Form
        Private app As ActionBasicApp
        Private label1 As System.Windows.Forms.Label
        Private label2 As System.Windows.Forms.Label
        Private label3 As System.Windows.Forms.Label
        Private label4 As System.Windows.Forms.Label
        Private groupBox1 As System.Windows.Forms.GroupBox
        Private chart As ActionBasic.Chart
        Private WithEvents ConfigurationButton As System.Windows.Forms.Button
        Private WithEvents ExitButton As System.Windows.Forms.Button
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing

        '/ <summary>
        '/ Constructor
        '/ </summary>
        '/ <param name="app">Reference to the application object</param>
        '/ <param name="ActionNames">List of game action strings</param>
        '/ <param name="deviceStates">Reference array for the device states</param>
        Public Sub New(ByVal app As ActionBasicApp, ByVal ActionNames() As [String], ByVal deviceStates As ArrayList)

            Try
                ' Load the icon from our resources
                Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(Me.GetType())
                Me.Icon = resources.GetObject("$this.Icon")
            Catch
                ' It's no big deal if we can't load our icons, but try to load the embedded one
                Try
                    Me.Icon = New System.Drawing.Icon(Me.GetType(), "directx.ico")
                Catch
                End Try
            End Try


            Me.app = app
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()

            chart.ColumnTitles = ActionNames
            chart.RowData = deviceStates
        End Sub 'New

        '/ <summary>
        '/ Print information about the provided exception
        '/ </summary>
        '/ <param name="ex">Exception instance</param>
        '/ <param name="calling">Name of the method which returned the exception</param>
        Public Sub ShowException(ByVal ex As Exception, ByVal calling As String)
            Dim output As String = ex.Message + ControlChars.Lf + ControlChars.Lf

            If ex.GetType().DeclaringType Is Type.GetType("System.DirectX.DirectXException") Then
                ' DirectX-specific info
                Dim dex As DirectXException = CType(ex, DirectXException)
                output += "HRESULT: " + dex.ErrorString + " (" + dex.ErrorCode.ToString("X") + ")" + ControlChars.Lf
            End If

            output += "Calling: " + calling + ControlChars.Lf
            output += "Source: " + ex.Source + ControlChars.Lf
            MessageBox.Show(Me, output, "ActionBasic Sample Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Sub 'ShowException

        '/ <summary>
        '/ Force the chart object to repaint
        '/ </summary>
        Public Sub UpdateChart()
            If Not chart.Created Then
                Return
            End If
            Try
                chart.UpdateData()
            Catch e As Exception
            End Try
        End Sub 'UpdateChart


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
        '/ Display the user guide
        '/ </summary>
        Private Sub DisplayHelp()
            Dim message As New StringBuilder()

            message.Append("The chart shows the list of devices found ")
            message.Append("attached to your computer, plotted against a" + ControlChars.Lf)
            message.Append("defined set of actions for an imaginary fighting ")
            message.Append("game." + ControlChars.Lf + ControlChars.Lf)
            message.Append("30 times per second, the program polls for new ")
            message.Append("input from the devices, and displays which actions" + ControlChars.Lf)
            message.Append("are currently being sent by each device. During ")
            message.Append("initialization, the program attempts to establish" + ControlChars.Lf)
            message.Append("a mapping between actions and device objects for ")
            message.Append("each attached device. Actions which were not" + ControlChars.Lf)
            message.Append("mapped to a device object are shown as a ")
            message.Append("crosshatch-filled cell on the chart." + ControlChars.Lf + ControlChars.Lf)
            message.Append("To view the current action mappings for all the ")
            message.Append("devices, click the ""View Configuration"" button," + ControlChars.Lf)
            message.Append("which will access the default configuration UI ")
            message.Append("managed by DirectInput.")

            MessageBox.Show(Me, message.ToString(), "ActionBasic Help", MessageBoxButtons.OK)
        End Sub 'DisplayHelp

        '/ <summary>
        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            Dim resources As New System.Resources.ResourceManager(GetType(ActionBasicUI))
            Me.label1 = New System.Windows.Forms.Label()
            Me.label2 = New System.Windows.Forms.Label()
            Me.label3 = New System.Windows.Forms.Label()
            Me.label4 = New System.Windows.Forms.Label()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.chart = New ActionBasic.Chart()
            Me.ConfigurationButton = New System.Windows.Forms.Button()
            Me.ExitButton = New System.Windows.Forms.Button()
            Me.groupBox1.SuspendLayout()
            Me.SuspendLayout()
            ' 
            ' label1
            ' 
            Me.label1.Location = New System.Drawing.Point(16, 24)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(344, 16)
            Me.label1.TabIndex = 0
            Me.label1.Text = "This tutorial polls for action-mapped data from the attached devices,"
            ' 
            ' label2
            ' 
            Me.label2.Location = New System.Drawing.Point(16, 40)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(344, 16)
            Me.label2.TabIndex = 1
            Me.label2.Text = "and displays a chart showing triggered game actions plotted against"
            ' 
            ' label3
            ' 
            Me.label3.Location = New System.Drawing.Point(16, 56)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(344, 16)
            Me.label3.TabIndex = 2
            Me.label3.Text = "the corresponding input devices."
            ' 
            ' label4
            ' 
            Me.label4.Location = New System.Drawing.Point(16, 80)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(344, 16)
            Me.label4.TabIndex = 3
            Me.label4.Text = "Press the F1 key for help."
            ' 
            ' groupBox1
            ' 
            Me.groupBox1.Controls.AddRange(New System.Windows.Forms.Control() {Me.chart, Me.ConfigurationButton, Me.label4, Me.label3, Me.label2, Me.label1})
            Me.groupBox1.Location = New System.Drawing.Point(8, 8)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Size = New System.Drawing.Size(408, 400)
            Me.groupBox1.TabIndex = 4
            Me.groupBox1.TabStop = False
            ' 
            ' chart
            ' 
            Me.chart.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.chart.ColumnTitles = Nothing
            Me.chart.Location = New System.Drawing.Point(16, 112)
            Me.chart.Name = "chart"
            Me.chart.RowData = Nothing
            Me.chart.Size = New System.Drawing.Size(376, 240)
            Me.chart.TabIndex = 6
            Me.chart.Text = "chart"
            ' 
            ' ConfigurationButton
            ' 
            Me.ConfigurationButton.Location = New System.Drawing.Point(272, 368)
            Me.ConfigurationButton.Name = "ConfigurationButton"
            Me.ConfigurationButton.Size = New System.Drawing.Size(120, 24)
            Me.ConfigurationButton.TabIndex = 5
            Me.ConfigurationButton.Text = "&View Configuration"
            ' 
            ' ExitButton
            ' 
            Me.ExitButton.Location = New System.Drawing.Point(344, 416)
            Me.ExitButton.Name = "ExitButton"
            Me.ExitButton.Size = New System.Drawing.Size(72, 24)
            Me.ExitButton.TabIndex = 5
            Me.ExitButton.Text = "Exit"
            ' 
            ' ActionBasicUI
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(424, 446)
            Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.ExitButton, Me.groupBox1})
            Me.Name = "ActionBasicUI"
            Me.Text = "ActionBasic Sample"
            Me.groupBox1.ResumeLayout(False)
            Me.ResumeLayout(False)
        End Sub 'InitializeComponent

        Private Sub ExitButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ExitButton.Click '
            CType(sender, Button).DialogResult = DialogResult.Cancel
            Me.Close()
        End Sub 'ExitButton_Click

        Protected Overrides Sub OnHelpRequested(ByVal e As HelpEventArgs)
            e.Handled = True
            DisplayHelp()
        End Sub 'OnHelpRequested

        Private Sub ConfigButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ConfigurationButton.Click
            app.ConfigureDevices()
        End Sub 'ConfigButton_Click
    End Class 'ActionBasicUI 
End Namespace 'ActionBasic 
