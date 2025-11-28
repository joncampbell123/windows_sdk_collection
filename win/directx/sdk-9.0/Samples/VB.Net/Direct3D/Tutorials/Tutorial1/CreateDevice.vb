'-----------------------------------------------------------------------------
' File: CreateDevice.vb
'
' Desc: This is the first tutorial for using Direct3D. In this tutorial, all
'       we are doing is creating a Direct3D device and using it to clear the
'       window.
'
' Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace DeviceTutorial
    Public Class CreateDevice
        Inherits Form
        ' Our global variables for this project
        Private device As device = Nothing
        ' Our rendering device
        Public Sub New()
            ' Set the initial size of our form
            Me.ClientSize = New System.Drawing.Size(400, 300)
            ' And it's caption
            Me.Text = "D3D Tutorial 01: CreateDevice"
        End Sub 'New


        Public Function InitializeGraphics() As Boolean
            Try
                ' Now let's setup our D3D stuff
                Dim presentParams As New PresentParameters()
                presentParams.Windowed = True
                presentParams.SwapEffect = SwapEffect.Discard
                device = New Device(0, DeviceType.Hardware, Me, CreateFlags.SoftwareVertexProcessing, presentParams)
                Return True
            Catch e As DirectXException
                Return False
            End Try
        End Function 'InitializeGraphics

        Private Sub Render()
            If device Is Nothing Then
                Return
            End If
            'Clear the backbuffer to a blue color
            device.Clear(ClearFlags.Target, System.Drawing.Color.Blue, 1.0F, 0)
            'Begin the scene
            device.BeginScene()

            ' Rendering of scene objects can happen here
            'End the scene
            device.EndScene()
            device.Present()
        End Sub 'Render

        Protected Overrides Sub OnPaint(ByVal e As System.Windows.Forms.PaintEventArgs)
            Me.Render() ' Render on painting
        End Sub 'OnPaint

        Protected Overrides Sub OnKeyPress(ByVal e As System.Windows.Forms.KeyPressEventArgs)
            If Asc(e.KeyChar) = CInt(System.Windows.Forms.Keys.Escape) Then
                Me.Close() ' Esc was pressed
            End If
        End Sub 'OnKeyPress

        Protected Overrides Sub OnLoad(ByVal e As System.EventArgs)
            ' Show and select the form
            Me.Show()
            Me.Select()
        End Sub 'OnLoad


        '<summary>
        'The main entry point for the application.
        '</summary>
        Shared Sub Main()

            Dim frm As New CreateDevice()
            If Not frm.InitializeGraphics() Then ' Initialize Direct3D
                MessageBox.Show("Could not initialize Direct3D.  This tutorial will exit.")
                Return
            End If
            frm.Show()

            While frm.Created
                frm.Render()
                Application.DoEvents()
            End While
        End Sub 'Main
    End Class 'CreateDevice
End Namespace 'DeviceTutorial