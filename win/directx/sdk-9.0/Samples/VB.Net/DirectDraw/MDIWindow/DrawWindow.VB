'-----------------------------------------------------------------------------
' File: DrawWindow.cs
'
' Desc: Example code showing how to do use MDI windows via DirectDraw
'
' Copyright (c) 1995-2002 Microsoft Corporation. All rights reserved.
'-----------------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms

Namespace MDIWindow
    _
    '/ <summary>
    '/ Summary description for DrawWindow.
    '/ </summary>
    Public Class DrawWindow
        Inherits System.Windows.Forms.Form
        '/ <summary>
        '/ Required designer variable.
        '/ </summary>
        Private components As System.ComponentModel.Container = Nothing

        Public Sub New()
            '
            ' Required for Windows Form Designer support
            '
            InitializeComponent()
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

        '/ Required method for Designer support - do not modify
        '/ the contents of this method with the code editor.
        '/ </summary>
        Private Sub InitializeComponent()
            ' 
            ' DrawWindow
            ' 
            Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
            Me.ClientSize = New System.Drawing.Size(292, 266)
            Me.Name = "DrawWindow"
            Me.Text = "DrawWindow"
        End Sub 'InitializeComponent

        ' various events to do an updated draw
        Private Sub DrawWindow_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles MyBase.Paint
            CType(Me.MdiParent, MainWindow).DrawSurfaces(Me, PointToScreen(New Point(0, 0)), Me.ClientSize)
        End Sub 'DrawWindow_Paint

        Private Sub DrawWindow_SizeChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.SizeChanged, MyBase.Resize
            If FormWindowState.Minimized = WindowState Then
                Return
            End If
            Dim parent As MainWindow = CType(Me.MdiParent, MainWindow)
            If Not (parent Is Nothing) Then
                parent.DrawSurfaces(Me, PointToScreen(New Point(0, 0)), Me.ClientSize)
            End If
        End Sub 'DrawWindow_SizeChanged

        Private Sub DrawWindow_Move(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Move
            Dim parent As MainWindow = CType(Me.MdiParent, MainWindow)
            If Not (parent Is Nothing) Then
                parent.DrawSurfaces(Me, PointToScreen(New Point(0, 0)), Me.ClientSize)
            End If
        End Sub 'DrawWindow_Move
    End Class 'DrawWindow
End Namespace 'MDIWindow