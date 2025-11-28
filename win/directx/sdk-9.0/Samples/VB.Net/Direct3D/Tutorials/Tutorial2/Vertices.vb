'-----------------------------------------------------------------------------
' File: Vertices.vb
'
' Desc: In this tutorial, we are rendering some vertices. This introduces the
'       concept of the vertex buffer, a Direct3D object used to store
'       vertices. Vertices can be defined any way we want by defining a
'       custom structure and a custom FVF (flexible vertex format). In this
'       tutorial, we are using vertices that are transformed (meaning they
'       are already in 2D window coordinates) and lit (meaning we are not
'       using Direct3D lighting, but are supplying our own colors).
'
' Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports Microsoft.DirectX
Imports Microsoft.DirectX.Direct3D


Namespace VerticesTutorial
    Public Class Vertices
        Inherits Form
        ' Our global variables for this project
        Private device As device = Nothing ' Our rendering device
        Private vertexBuffer As vertexBuffer = Nothing
       _


        Public Sub New()
            ' Set the initial size of our form
            Me.ClientSize = New System.Drawing.Size(300, 300)
            ' And it's caption
            Me.Text = "Direct3D Tutorial 2 - Vertices"
        End Sub 'New


        Public Function InitializeGraphics() As Boolean
            Try
                ' Now let's setup our D3D stuff
                Dim presentParams As New PresentParameters()
                presentParams.Windowed = True
                presentParams.SwapEffect = SwapEffect.Discard
                device = New Device(0, DeviceType.Hardware, Me, CreateFlags.SoftwareVertexProcessing, presentParams)
                AddHandler device.DeviceCreated, AddressOf Me.OnCreateDevice
                Me.OnCreateDevice(device, Nothing)
                Return True
            Catch e As DirectXException
                Return False
            End Try
        End Function 'InitializeGraphics

        Public Sub OnCreateDevice(ByVal sender As Object, ByVal e As EventArgs)
            Dim dev As Device = CType(sender, Device)
            ' Now Create the VB
            vertexBuffer = New VertexBuffer(GetType(CustomVertex.TransformedColored), 3, dev, 0, CustomVertex.TransformedColored.Format, Pool.Default)
            AddHandler vertexBuffer.Created, AddressOf Me.OnCreateVertexBuffer
            Me.OnCreateVertexBuffer(vertexBuffer, Nothing)
        End Sub 'OnCreateDevice

        Public Sub OnCreateVertexBuffer(ByVal sender As Object, ByVal e As EventArgs)
            Dim vb As VertexBuffer = CType(sender, VertexBuffer)
            Dim verts As CustomVertex.TransformedColored() = CType(vb.Lock(0, 0), CustomVertex.TransformedColored())
            verts(0).X = 150
            verts(0).Y = 50
            verts(0).Z = 0.5F
            verts(0).Rhw = 1
            verts(0).Color = System.Drawing.Color.Aqua.ToArgb()
            verts(1).X = 250
            verts(1).Y = 250
            verts(1).Z = 0.5F
            verts(1).Rhw = 1
            verts(1).Color = System.Drawing.Color.Brown.ToArgb()
            verts(2).X = 50
            verts(2).Y = 250
            verts(2).Z = 0.5F
            verts(2).Rhw = 1
            verts(2).Color = System.Drawing.Color.LightPink.ToArgb()
            vb.Unlock()
        End Sub 'OnCreateVertexBuffer

        Private Sub Render()
            If device Is Nothing Then
                Return
            End If
            'Clear the backbuffer to a blue color 
            device.Clear(ClearFlags.Target, System.Drawing.Color.Blue, 1.0F, 0)
            'Begin the scene
            device.BeginScene()

            device.SetStreamSource(0, vertexBuffer, 0)
            device.VertexFormat = CustomVertex.TransformedColored.Format
            device.DrawPrimitives(PrimitiveType.TriangleList, 0, 1)
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
        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        Shared Sub Main()

            Dim frm As New Vertices()
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
    End Class 'Vertices 
End Namespace 'VerticesTutorial